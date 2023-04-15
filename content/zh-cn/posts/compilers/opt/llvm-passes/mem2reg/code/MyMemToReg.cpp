#include "llvm/Transforms/Utils/MyMemToReg.h"

#include <memory>
#include <stack>

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/IteratedDominanceFrontier.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

static bool canPromote(const AllocaInst *AI);
static void doPromote(const SmallVectorImpl<AllocaInst *> &Allocas,
                      DominatorTree &DT);

PreservedAnalyses MyMemToRegPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  auto &DT = AM.getResult<DominatorTreeAnalysis>(F);

  bool Changed = false;

  SmallVector<AllocaInst *, 32> Allocas;
  while (true) {
    Allocas.clear();
    for (auto &InstRef : F.getEntryBlock()) {
      if (AllocaInst *AI = dyn_cast<AllocaInst>(&InstRef)) {
        if (canPromote(AI)) {
          Allocas.push_back(AI);
        }
      }
    }
    if (Allocas.empty()) {
      break;
    }

    doPromote(Allocas, DT);

    Changed = true;
  }

  if (!Changed) {
    return PreservedAnalyses::all();
  }

  PreservedAnalyses PA;
  PA.preserveSet<CFGAnalyses>();
  return PA;
}

static bool canPromote(const AllocaInst *AI) {
  for (const auto *U : AI->users()) {
    if (auto *SI = dyn_cast<StoreInst>(U)) {
      if (SI->getValueOperand() == AI ||
          SI->getValueOperand()->getType() != AI->getAllocatedType()) {
        return false;
      }
    } else if (auto *LI = dyn_cast<LoadInst>(U)) {
      if (LI->getType() != AI->getAllocatedType()) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

static void getDefiningBlocks(AllocaInst *AI,
                              SmallPtrSetImpl<BasicBlock *> &DefiningBlocks) {
  for (auto *U : AI->users()) {
    if (auto *SI = dyn_cast<StoreInst>(U)) {
      DefiningBlocks.insert(SI->getParent());
    }
  }
}

static void
insertPHINodes(AllocaInst *AI, const SmallVectorImpl<BasicBlock *> &PHIBlocks,
               DenseMap<PHINode *, AllocaInst *> &AllocasByPHINode) {
  unsigned Version = 0;
  for (auto *BB : PHIBlocks) {
    auto *PN =
        PHINode::Create(AI->getAllocatedType(), 0,
                        AI->getName() + "." + Twine(Version++), &BB->front());
    AllocasByPHINode[PN] = AI;
  }
}

static void
doRename(DomTreeNode *DN,
         DenseMap<AllocaInst *, std::stack<Value *>> &StacksByAlloca,
         DenseMap<PHINode *, AllocaInst *> &AllocasByPHINode) {
  auto &BB = *DN->getBlock();
  for (auto &InstRef : make_early_inc_range(BB)) {
    auto *Inst = &InstRef;
    if (auto *SI = dyn_cast<StoreInst>(Inst)) {
      AllocaInst *AI = dyn_cast<AllocaInst>(SI->getPointerOperand());
      if (!AI || !StacksByAlloca.count(AI)) {
        continue;
      }
      StacksByAlloca.find(AI)->second.push(SI->getValueOperand());
    } else if (auto *PHI = dyn_cast<PHINode>(Inst)) {
      auto I = AllocasByPHINode.find(PHI);
      if (I == AllocasByPHINode.end()) {
        continue;
      }
      StacksByAlloca.find(I->second)->second.push(PHI);
    } else if (auto *LI = dyn_cast<LoadInst>(Inst)) {
      AllocaInst *AI = dyn_cast<AllocaInst>(LI->getPointerOperand());
      if (!AI || !StacksByAlloca.count(AI)) {
        continue;
      }
      LI->replaceAllUsesWith(StacksByAlloca.find(AI)->second.top());
      LI->eraseFromParent();
    }
  }

  for (auto *S : successors(&BB)) {
    for (auto &InstRef : *S) {
      auto *Inst = &InstRef;
      auto *PHI = dyn_cast<PHINode>(Inst);
      // If the instruction is not a phi-node, exit the loop.
      if (!PHI) {
        break;
      }
      auto I = AllocasByPHINode.find(PHI);
      // Finding a phi-node not in AllocasByPHINode means we are done with
      // those phi-nodes we inserted.
      if (I == AllocasByPHINode.end()) {
        break;
      }
      PHI->addIncoming(StacksByAlloca.find(I->second)->second.top(), &BB);
    }
  }

  for (auto *C : DN->children()) {
    doRename(C, StacksByAlloca, AllocasByPHINode);
  }

  for (auto &InstRef : make_early_inc_range(BB)) {
    auto *Inst = &InstRef;
    if (auto *SI = dyn_cast<StoreInst>(Inst)) {
      AllocaInst *AI = dyn_cast<AllocaInst>(SI->getPointerOperand());
      if (!AI || !StacksByAlloca.count(AI)) {
        continue;
      }
      StacksByAlloca.find(AI)->second.pop();
      SI->eraseFromParent();
    } else if (auto *PHI = dyn_cast<PHINode>(Inst)) {
      auto I = AllocasByPHINode.find(PHI);
      if (I == AllocasByPHINode.end()) {
        continue;
      }
      StacksByAlloca.find(I->second)->second.pop();
    }
  }
}

static void doPromote(const SmallVectorImpl<AllocaInst *> &Allocas,
                      DominatorTree &DT) {
  DenseMap<PHINode *, AllocaInst *> AllocasByPHINode;
  for (auto *AI : Allocas) {
    assert(canPromote(AI) && "AI cannot be promoted");

    SmallPtrSet<BasicBlock *, 8> DefiningBlocks;
    getDefiningBlocks(AI, DefiningBlocks);

    ForwardIDFCalculator IDF(DT);
    IDF.setDefiningBlocks(DefiningBlocks);
    SmallVector<BasicBlock *, 8> PHIBlocks;
    IDF.calculate(PHIBlocks);

    insertPHINodes(AI, PHIBlocks, AllocasByPHINode);
  }

  DenseMap<AllocaInst *, std::stack<Value *>> StacksByAlloca;
  for (unsigned I = 0, E = Allocas.size(); I != E; I++) {
    StacksByAlloca[Allocas[I]].push(
        UndefValue::get(Allocas[I]->getAllocatedType()));
  }
  doRename(DT.getRootNode(), StacksByAlloca, AllocasByPHINode);

  for (auto *AI : Allocas) {
    AI->eraseFromParent();
  }
}

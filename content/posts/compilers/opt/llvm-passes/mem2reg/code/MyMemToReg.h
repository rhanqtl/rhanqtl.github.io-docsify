#ifndef LLVM_TRANSFORMS_UTILS_MYMEMTOREG_H
#define LLVM_TRANSFORMS_UTILS_MYMEMTOREG_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/Analysis/IteratedDominanceFrontier.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"

namespace llvm {

class Function;

struct MyMemToRegPass : public PassInfoMixin<MyMemToRegPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // end namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_MYMEMTOREG_H

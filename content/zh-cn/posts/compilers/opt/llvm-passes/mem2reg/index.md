---
title: "mem2reg"
date: 2023-03-07T08:03:18+08:00
tags: ["编译器", "编译优化", "LLVM"]
# series: ["LLVM 源码阅读"]
summary: \-

draft: false
---

## 背景 {#sec:background}

为了应对栈变量，LLVM 引入了 [alloca](https://llvm.org/docs/LangRef.html#alloca-instruction:) 指令，以 C 语言为例，前端可以简单地将每个局部变量映射到一个 alloca 指令。然而，有些 alloca 指令其实是不必要的，不过让前端来负责简化它们会增加很多难度，因此 LLVM 引入了一个称为“[mem2reg](https://llvm.org/docs/Passes.html#mem2reg-promote-memory-to-register)”的 pass，用于自动地将 alloca 变量提升为寄存器变量。

本文旨在以 mem2reg 为切入点深入学习 LLVM。

## 实验

为了看看 mem2reg 的效果，我们可以来尝试一些例子

<dl><dt><strong>💡 TIP</strong></dt><dd>

编译命令为 `clang -S -emit-llvm -Xclang -disable-O0-optnone -fno-discard-value-names _/path/to/src_`，其中，`-disable-O0-optnone` 是为了禁止 clang 在优化级别为 `-O0` 时给函数加上 `optnone` 属性，这个属性会让 PassManager 跳过可选的优化（包括 mem2reg），参见 [bib:opt-no-effect](#bib:opt-no-effect)；`-fno-discard-value-names` 是为了保留变量的名字，否则 LLVM IR 中所有的变量都会变成匿名的[bib:llvm-langref](#bib:llvm-langref)，不便于阅读。

优化命令为 `opt -passes=mem2reg -S _/path/to/ll-file_ $$[ >$$ _/path/to/opt-result_ $$]$$`
</dd></dl>

### 仅包含一条语句

一种情况是只有参数，没有局部变量。

```c
void foo(int x) {
  printf("x = %d\n", x);
}
```

生成的 LLVM IR 和优化后的对比如下：

```llvm
define void @foo(i32 %x) {                                   define void @foo(i32 %x) {
entry:                                                       entry:
  %x.addr = alloca i32, align 4                           &lt;
  store i32 %x, ptr %x.addr, align 4                      &lt;
  %0 = load i32, ptr %x.addr, align 4                     &lt;
  %call = call i32 (ptr, ...) @printf(ptr @.str, i32 %0)  |    %call = call i32 (ptr, ...) @printf(ptr @.str, i32 %x)
  ret void                                                     ret void
}                                                            }
```

本来要 store/load 一次 `%x` 用匿名中间结果作为 printf 实参，mem2reg 替换成了 `%x`。可以想见多个参数应该也是相同的处理。

```c
void foo() {
  int x = 42;
  x++;
}
```

对比：

```llvm
define void @foo() {                  define void @foo() {
entry:                                entry:
  %x = alloca i32, align 4         <
  store i32 42, ptr %x, align 4    <
  %0 = load i32, ptr %x, align 4   <
  %inc = add nsw i32 %0, 1         |   %inc = add nsw i32 42, 1
  store i32 %inc, ptr %x, align 4  <
  ret void                             ret void
}                                    }
```

### 分支

```c
int bar(int x, int y) {
  if (x > 42) {
    y += 1;
  } else {
    y += 2;
  }
  return y;
}
```

对比：

```llvm
define i32 @bar(i32 %x, i32 %y) {                define i32 @bar(i32 %x, i32 %y) {
entry:                                           entry:
  %x.addr = alloca i32, align 4               &lt;
  %y.addr = alloca i32, align 4               &lt;
  store i32 %x, ptr %x.addr, align 4          &lt;
  store i32 %y, ptr %y.addr, align 4          &lt;
  %0 = load i32, ptr %x.addr, align 4         &lt;
  %cmp = icmp sgt i32 %0, 42                  |    %cmp = icmp sgt i32 %x, 42
  br i1 %cmp, label %if.then, label %if.else       br i1 %cmp, label %if.then, label %if.else

if.then:                                         if.then:
  %1 = load i32, ptr %y.addr, align 4         &lt;
  %add = add nsw i32 %1, 1                    |    %add = add nsw i32 %y, 1
  store i32 %add, ptr %y.addr, align 4        &lt;
  br label %if.end                                 br label %if.end

if.else:                                         if.else:
  %2 = load i32, ptr %y.addr, align 4         &lt;
  %add1 = add nsw i32 %2, 2                   |    %add = add nsw i32 %y, 1
  store i32 %add1, ptr %y.addr, align 4       &lt;
  br label %if.end                                 br label %if.end

if.end:                                          if.end:
  %3 = load i32, ptr %y.addr, align 4         |    %y.addr.0 = phi i32 [ %add, %if.then ], [ %add1, %if.else ]
  ret i32 %3                                  |    ret i32 %y.addr.0
}                                                }
```
NOTE: 实际上在这种情况中 LLVM 会改用 [`select`](https://llvm.org/docs/LangRef.html#select-instruction:) 指令。

### 循环

```c
int foo(int n) {
  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += i;
  }
  return sum;
}
```

对比

```llvm
define i32 @foo(i32 %n) {                         define i32 @foo(i32 %n) {
entry:                                            entry:
  %n.addr = alloca i32, align 4                &lt;
  %sum = alloca i32, align 4                   &lt;
  %i = alloca i32, align 4                     &lt;
  store i32 %n, ptr %n.addr, align 4           &lt;
  store i32 0, ptr %sum, align 4               &lt;
  store i32 0, ptr %i, align 4                 &lt;
  br label %for.cond                                br label %for.cond

for.cond:                                         for.cond:
  %0 = load i32, ptr %i, align 4               &lt;
  %1 = load i32, ptr %n.addr, align 4          &lt;
                                               >    %sum.0 = phi i32 [ 0, %entry ], [ %add, %for.inc ]
                                               >    %i.0 = phi i32 [ 0, %entry ], [ %inc, %for.inc ]
  %cmp = icmp slt i32 %0, %1                   |    %cmp = icmp slt i32 %i.0, %n
  br i1 %cmp, label %for.body, label %for.end       br i1 %cmp, label %for.body, label %for.end

for.body:                                         for.body:
  %2 = load i32, ptr %i, align 4               &lt;
  %3 = load i32, ptr %sum, align 4             &lt;
  %add = add nsw i32 %3, %2                    |    %add = add nsw i32 %sum.0, %i.0
  store i32 %add, ptr %sum, align 4            &lt;
  br label %for.inc                                 br label %for.inc

for.inc:                                          for.inc:
  %4 = load i32, ptr %i, align 4               &lt;
  %inc = add nsw i32 %4, 1                     |    %inc = add nsw i32 %i.0, 1
  store i32 %inc, ptr %i, align 4              &lt;
  br label %for.cond, !llvm.loop !6                 br label %for.cond, !llvm.loop !6

for.end:                                          for.end:
  %5 = load i32, ptr %sum, align 4             &lt;
  ret i32 %5                                   |    ret i32 %sum.0
}                                                 }
```

这里跟前面分支都有一个很有趣的点：除了 `ret`，其他的 terminator 指令都没有发生变化，也就是说，mem2reg 不会改变 CFG。

### 指针

```c
void foo() {
  int a = 42;
  int *p = &a;
}
```

生成代码如下:

```llvm
define void @foo() {
  %a = alloca i32, align 4
  %p = alloca ptr, align 8
  store i32 42, ptr %a, align 4
  store ptr %a, ptr %p, align 8
  ret void
}
```

这里有一个需要注意的点：尽管两条 alloca 最后都能去掉，但是 `%a` 必须在 `%p` 之后才能被干掉。这是因为最初 `%a` 的地址作为了第二条 store 的 value operand，如果去掉，对应的 store 就无法表示；而在 `%p` 被去掉以后，`%a` 只剩下一个 use，就可以被去掉了。因此 mem2reg 还需要以不动点的方式运行：

```c++
while (true) {
  // 1. find AllocaInst's can be promoted
  // 2. if not found, exit the loop
  // 3. promote those AllocaInst's
}
```

## 自己实现一个 mem2reg

不难发现，提升 `alloca` 变量是围绕着 `store` 和 `load` 两类指令进行的，具体地：

* `store` 是 def，`load` 是 use；
* 处理过程跟构建 SSA 很相似，涉及到 &phi;-node 的插入和重命名（重命名）；
* 完成替换后，需要删除无用的 `store`、`load` 和 `alloca`。

**💡 TIP**\
如果不了解 SSA 形式的构建过程，可以参考 link:{{&lt; ref "/posts/compilers/ssa-construction" >}}[这篇文章]

因此，大致流程可以是这样的：

1. 插入 &phi;-node
   1. 计算 IDF
   2. 创建 &phi;-node
2. 重命名
   1. 对于 BB 中每一条指令
      1. 如果是 store 和 phi，记录变量 AI 的新值
      2. 如果是 load，将所有 use 替换为 AI 当前值，并删除这条 load
   2. 为每个后继中新增的 phi 添加入边
   3. 对于 dom. tree 中的每个子节点递归重命名
   4. 删除所有无用的 store

### 框架

**💡 TIP**\
你需要先了解如何在 LLVM 中实现一个 pass，参见 [bib:writing-an-llvm-pass-new-pm](#bib:writing-an-llvm-pass-new-pm)。

在 `include/llvm/Transforms/Utils` 中创建文件 `MyMemToReg.h`，加入如下的内容：

```c++
#ifndef LLVM_TRANSFORMS_UTILS_MYMEMTOREG_H
#define LLVM_TRANSFORMS_UTILS_MYMEMTOREG_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class Function;

struct MyMemToRegPass : public PassInfoMixin<MyMemToRegPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // end namespace llvm

#endif  // LLVM_TRANSFORMS_UTILS_MYMEMTOREG_H
```

**💡 TIP**\
递归这里其实还有另一种做法，可以先处理 &phi;-node，后面在 LLVM 的 mem2reg 中会见到。

先来搭框架（基本上就是前面列出的流程的直接映射）：

```c++
// file: MyMemToReg.cpp
PreservedAnalyses MyMemToRegPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  // 我们在两个阶段都需要用的 dom. tree
  auto &DT = AM.getResult<DominatorTreeAnalysis>(F);

  // 用于返回时判断哪些分析结果可以被保留
  bool Changed = false;

  SmallVector<AllocaInst *, 32> Allocas;
  while (true) {
    // 收集这一轮能够提升的 alloca
    Allocas.clear();
    for (auto &InstRef : F.getEntryBlock())
      if (AllocaInst *AI = dyn_cast<AllocaInst>(&InstRef))
        if (canPromote(AI)) Allocas.push_back(AI);
    if (Allocas.empty()) break;

    doPromote(Allocas, DT);

    Changed = true;
  }

  if (!Changed) return PreservedAnalyses::all();
  PreservedAnalyses PA;
  // 如前文所述，mem2reg 不会改变 CFG
  PA.preserveSet<CFGAnalyses>();
  return PA;
}
```

接下来是 `canPromote`，用于判断哪些 alloca 能够提升：

```c++
static bool canPromote(const AllocaInst *AI) {
  for (const auto *U : AI->users()) {
    if (auto *SI = dyn_cast<StoreInst>(U)) {
      // 1. 如果 alloca 是作为被存入的值，不能提升
      // 2. 这里涉及到“透明指针”，LLVM 目前在使用指针时不关心指针指向的类型，例如
      //      %0 = alloca i32, align 4
      //      %1 = load i16, ptr %0, align 4
      //    参见 https://llvm.org/docs/OpaquePointers.html
      if (SI->getValueOperand() == AI ||
          SI->getValueOperand()->getType() != AI->getAllocatedType()) {
        return false;
      }
    } else if (auto *LI = dyn_cast<LoadInst>(U)) {
      // 同上
      if (LI->getType() != AI->getAllocatedType()) {
        return false;
      }
    } else {
      // 如果被任何其它类型的指令使用，不提升
      return false;
    }
  }
  return true;
}
```

为了保持简单，我们这里忽略了 load 和 store 的 volatile 问题。实际上，如果有 volatile，那么是不能提升的，因为 LLVM IR 将 volatile 访问定义为有“可见的副作用”（visible side effects）[bib:llvm-langref-volatile](#bib:llvm-langref-volatile)

接下来是 `doPromote`：

```c++
static void doPromote(const SmallVectorImpl<AllocaInst *> &Allocas,
                      DominatorTree &DT) {
  // 记录 phi-node 到 alloca 的对应关系
  DenseMap<PHINode *, AllocaInst *> AllocasByPHINode;
  for (auto *AI : Allocas) {
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
    // 可能存在没有被任何 store 支配的 load 和 phi-node 用 ⊥ 兜底
    StacksByAlloca[Allocas[I]].push(
        UndefValue::get(Allocas[I]->getAllocatedType()));
  }
  doRename(DT.getRootNode(), StacksByAlloca, AllocasByPHINode);

  // 在最后删除无用的 alloca
  for (auto *AI : Allocas) {
    AI->eraseFromParent();
  }
}
```

`getDefiningBlocks` 和 `insertPHINodes` 很简单，这里不再贴代码解释。重点是 `doRename`：

```c++
static void
doRename(DomTreeNode *DN,
         DenseMap<AllocaInst *, std::stack<Value *>> &StacksByAlloca,
         DenseMap<PHINode *, AllocaInst *> &AllocasByPHINode) {
  // 替换 load 指令
  auto &BB = *DN->getBlock();
  // 注意这里的 make_early_inc_range，由于 LI->eraseFromParent 会导致迭代器失效，
  // 如果不用这个函数包装一下，运行时会崩溃
  for (auto &InstRef : make_early_inc_range(BB)) {
    auto *Inst = &InstRef;
    if (auto *SI = dyn_cast<StoreInst>(Inst)) {
      AllocaInst *AI = dyn_cast<AllocaInst>(SI->getPointerOperand());
      // 确保这是一条 alloca 并且可以提升
      if (!AI || !StacksByAlloca.count(AI)) continue;
      StacksByAlloca.find(AI)->second.push(SI->getValueOperand());
    } else if (auto *PHI = dyn_cast<PHINode>(Inst)) {
      auto I = AllocasByPHINode.find(PHI);
      // 我们插入 phi-node 时，都是在 BB 的最开始连续插入的，因此遇到不在
      // AllocasByPHINode 的 phi-node 说明已经处理完了本轮插入的 phi-node
      if (I == AllocasByPHINode.end()) break;
      StacksByAlloca.find(I->second)->second.push(PHI);
    } else if (auto *LI = dyn_cast<LoadInst>(Inst)) {
      AllocaInst *AI = dyn_cast<AllocaInst>(LI->getPointerOperand());
      if (!AI || !StacksByAlloca.count(AI)) continue;
      LI->replaceAllUsesWith(StacksByAlloca.find(AI)->second.top());
      LI->eraseFromParent();
    }
  }
  ...
}
```

下一步是递归：

```c++
static void doRename(...) {
  ...
  // 注意这里是修改所有 succ 的 phi-node
  for (auto *S : successors(&BB)) {
    for (auto &InstRef : *S) {
      auto *PHI = dyn_cast<PHINode>(&InstRef);
      // If the instruction is not a phi-node, exit the loop.
      if (!PHI) break;
      auto I = AllocasByPHINode.find(PHI);
      // Finding a phi-node not in AllocasByPHINode means we are done with
      // those phi-nodes we inserted.
      if (I == AllocasByPHINode.end()) break;
      PHI->addIncoming(StacksByAlloca.find(I->second)->second.top(), &BB);
    }
  }

  // 注意这里仅仅是对 dom. tree 中的子节点进行递归，因为没有被当前 BB
  // dominate 的 succ 一定会有对应的 phi-node，且仅需要处理这些 phi-node
  for (auto *C : DN->children()) {
    doRename(C, StacksByAlloca, AllocasByPHINode);
  }
}
```

最后，删除无用的 store，并还原 value stack：

```c++
static void doRename(...) {
  ...
  // 同样需要使用 make_early_inc_range，这个循环跟第一个很类似
  for (auto &InstRef : make_early_inc_range(BB)) {
    auto *Inst = &InstRef;
    if (auto *SI = dyn_cast<StoreInst>(Inst)) {
      AllocaInst *AI = dyn_cast<AllocaInst>(SI->getPointerOperand());
      if (!AI || !StacksByAlloca.count(AI)) continue;
      StacksByAlloca.find(AI)->second.pop();
      SI->eraseFromParent();
    } else if (auto *PHI = dyn_cast<PHINode>(Inst)) {
      auto I = AllocasByPHINode.find(PHI);
      if (I == AllocasByPHINode.end()) break;
      StacksByAlloca.find(I->second)->second.pop();
    }
  }
}
```

别忘了把 pass 注册到 PassRegistry.def 和 PassBuilder 中。编译之后就可以用 opt 调用了：

```plaintext
$ opt -passes=my-mem-to-reg -S ...
```

完整代码参见 [MyMemToReg.h](code/MyMemToReg.h) 和 [MyMemToReg.cpp](code/MyMemToReg.cpp)

## LLVM 的实现

LLVM 的实现的框架跟我们自己的类似，多了一些优化和细节上的考虑。

<dl><dt><strong>⚠️ WARNING</strong></dt><dd>
这里比较凌乱，不过尽量保证了大框架的规整。如果你在读代码过程中有一些不理解的问题，可以来看看这里有没有相关内容。更欢迎解答我的疑惑！
</dd></dl>

### 判断能否提升

如 `isAllocaPromotable` 的注释所说的：“Only allow direct and non-volatile loads and stores”。

mem2reg 只允许被提升变量的 uses 包含某些特定的指令，而且这些 uses 也必须满足一定的限制。目前（LLVM17）这些指令包括：LoadInst、StoreInst、IntrinsicInst、BitCastInst、GetElementPtrInst 和 AddrSpaceCastInst。

LoadInst 的分支中如下的测试比较令人疑惑（StoreInst 也有类似的）:

```c++
if (const LoadInst *LI = dyn_cast<LoadInst>(U)) {
  //                      vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  if (LI->isVolatile() || LI->getType() != AI->getAllocatedType())
    return false;
}
```

这个在文档中没有说明，不过通过翻 Git 历史（[这个](https://github.com/llvm/llvm-project/commit/074561a4a22f610d756109170285d8626c4cc3bc)和[这个](https://reviews.llvm.org/D109259)）看到了原因：

> Alloca promotion can only deal with cases where the load/store types match the alloca type (it explicitly does not support bitcasted load/stores).
>
> With [opaque pointers](https://llvm.org/docs/OpaquePointers.html) this is no longer enforced through the pointer type, so add an explicit check.

至于 volatile，参见 [bib:llvm-langref-volatile](#bib:llvm-langref-volatile)。

> A volatile load or store may have additional target-specific semantics. Any volatile operation can have side effects, and any volatile operation can read and/or modify state which is not accessible via a regular load or store in this module. Volatile operations may use addresses which do not point to memory (like MMIO registers). This means the compiler may not use a volatile operation to prove a non-volatile access to that address has defined behavior.
>
> — LLVM Programmer's Manual

**LLVM 的 Intrinsic Function**

LLVM 的 intrinsic function 是由 LLVM 编译器特殊处理的函数。这些函数通常低级操作，要么在源语言中无法表达，要么用普通的函数调用实现成本太高。Intrinsic function 提供了更多的优化空间，例如，LLVM 可以将它们内联，特化或者用 target-specific 指令代替（摘录自 ChatGPT 的回答）。

这些函数具有众所周知的名称和语义，并且需要遵循某些限制。总的来说，这些内在函数代表了 LLVM 语言的扩展机制，在添加到语言中时不需要更改 LLVM 中的所有变换[bib:llvm-langref](#bib:llvm-langref)。

* `llvm.pseudoprobe` 用于加入探针，可以进行 PGO
* `llvm.assume` 用于为优化器和代码生成器提供无法通过代码本身提供的信息，例如在某个程序点假设 `x > 0`

TODO: 除了 load/store 之外，其他四个指令的特殊限制暂时还没想明白

### PromoteMem2Reg::run

TODO: droppable user
TODO: removeIntrinsicUsers 为什么要过滤掉 void 类型的 Inst（目前应该只有 probe 是 void 的，但是 probe 本身是可以 droppable）
TODO: 如果将 probe 和 assume 移除了，如何保留原来的功能？

这个函数的流程如下：

1. 将所有 trivial 的 alloca 及相关的 store 和 load 移除（包括没有 use 的、只有一个 store 的和所有的 use 在同一个 BB 的）
2. 对于非 trivial 的 alloca，进行通用的插入 &phi;-node 和重命名的操作
3. 删除无用的 alloca
4. 处理 &phi;-node 相关的边界情况，包括简化 &phi;-node 和完善不可达 BB 的 &phi;-node

removeIntrinsicUsers 结束之后，我们只需要关心 load 和 store 即可。

#### 边界情况

如注释所说的：

> Loop over all of the PHI nodes and see if there are any that we can get rid of because they merge all of the same incoming values.  This can happen due to undef values coming into the PHI nodes.  This process is iterative, because eliminating one PHI node can cause others to be removed.

TODO: non-deterministic def-use chain 会有什么影响吗？
TODO: unreachable bb 为什么还需要处理？不会被去掉吗？

关于最后补足 &phi;-node 的入边，通过入边的数量来区分哪些 &phi;-node 是本轮 mem2reg 插入的（换句话说，何时停止），那么如果恰好有在 mem2reg 之前已经存在的 phi 的 incoming 跟 mem2reg 添加的数量一样，岂不是会出问题？就目前能想到的情况而言，还好——要么要求前面的 pass 保证做了同样的处理，要么本轮给补充完整，反正是 undef，问题不大。

### rewriteSingleStoreAlloca

该函数中有一个判断：

```c++
bool StoringGlobalVal = !isa<Instruction>(OnlyStore->getOperand(0));
/* ... */
for (/* ... */) {
  if (!StoringGlobalVal) { /* ... */ }
}
```

这里的目的是将所有被 `store` dominate 的 `load` 替换成 `store` 的参数，如果存在没有被 dominate 的走通用流程（这一点通常应该对应于未初始化的局部变量，不过至少在 C11 中，这种行为不是 UB，仅仅会导致读取出来的值是“indetermine”的，因此走通用流程是合理的）。函数的参数（类型为 `llvm::Argument`）和全局变量（类型为 `llvm::GlobalValue`）均属于“non-instructions”（也许这个变量的名字改成 `StoringNonInstValue` 更合适），其中函数参数确保在进入函数体之前就存在了，而且本身作为寄存器变量保证了不会被修改，因此“always dominated”，可以直接替换。不过全局变量其实涉及到一个很小的点。我们来看如下的代码：

```c
void foo() {
  static int baz = 42;
  int *p, *q = p;
  printf("%p %p\n", p, q);
  p = &baz;
  q = p;
}
```

这段代码在我的机器上（amd64，clang，-O0/-O1/-O2/-O3/-Ofast）会输出两个相同的值，即 `baz` 的地址。优化后的结果是这样的：

```llvm
define void @foo() {
entry:
  ;                                                  vvvvvvvv      vvvvvvvv
  %call = call i32 (ptr, ...) @printf(ptr @.str, ptr @bar.baz, ptr @bar.baz)
  ret void
}
```

如果将最后两行注释掉，又会输出两个不同于 `baz` 的地址的值

```llvm
define void @foo() {
entry:
  ;                                                  vvvvv      vvvvv
  %call = call i32 (ptr, ...) @printf(ptr @.str, ptr undef, ptr undef)
  ret void
}
```

直接原因是，当 store 的内容是全局变量时 `StoringGlobalVal` 为 true，执行流跳过了 `if` 分支，直接将出现在***后面****的 `store` 的值替换到出现在****前面***的 `load`。不过，使用未初始化的局部变量是 UB[bib:c11-annex-j.2](#bib:c11-annex-j.2)，所以其实影响不大（我在 LLVM 社区提了一个[问题](https://discourse.llvm.org/t/question-about-mem2regs-handling-of-global-variables/69279)，忽略我的英语）。

### promoteSingleBlockAlloca

有一个比较特殊的点——当发现一个 load 之前没有任何 store 时，直觉上将 load 的 uses 替换为 undef 就好（因为当前情况是所有的 load 和 store 都在同一个 BB 中），但是 LLVM 进行了分情况考虑：

* 如果没有任何 store，将 load 替换为 undef
* 否则，返回 false 用通用逻辑

**💡 TIP**\
这里 `llvm::lower_bound` 返回的是紧跟着 load 的 store 的索引，因此如果 `it == begin()`，因为这 load 之前没有 store

关键在于，这里考虑的***仅仅***是 load 和 store，但是还有 alloca！mem2reg 只考虑 entry 中的 alloca，也就是说 alloca 一定 dominate 所有的 store 和 load。考虑如下的 BB：

![only-in-one-bb](images/only-in-one-bb.png)

这里 `%gotcha` 虽然一开始是 undef，但是后面会被修改。

**等价 C 代码**

如下代码会产生上述行为：

```c
int gotcha;
for (int i = 0; i < 10; i++) {
  int t = gotcha;
  t++;
  gotcha = t;
}
```

### PromoteMem2Reg::ComputeLiveInBlocks

进行 liveness 分析[bib:liveness-analysis](#bib:liveness-analysis)，这主要是为了消除无用的 &phi;-node，也就是说，最终 mem2reg 的结果是 pruned 形式。

分析的流程也比较直观：

1. 从 uses 开始，先将没有 live-in 的 block 剔除，这里的逻辑有一点绕

   ```c++
   for (BasicBlock::iterator I = BB->begin();; ++I) {
     if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
       if (SI->getOperand(1) != AI)
         continue;

       LiveInBlockWorklist[i] = LiveInBlockWorklist.back();
       LiveInBlockWorklist.pop_back();
       --i;
       --e;
       break;
     }

     if (LoadInst *LI = dyn_cast<LoadInst>(I))
       if (LI->getOperand(0) == AI)
         break;
   }
   ```
   进入这个循环时能够确定的是，`BB` 包含 `AI` 的 def 和 use，为了确定是否 live-in，我们需要确定 def 和 use 出现的先后顺序——如果先出现的是 def，那么没有 live-in，需要将 BB 从 work list 中删除；如果先出现的是 use，那么 live-in，保留。为了实现这个逻辑，这里使用了两个并列的、可能跳出循环的 `if`，找到的是第一个使用 `AI` 的 load 或者 store。如果找到的是 store，那么说明没有 live-in，否则，说明 live-in。
2. 然后就是经典的 backward data-flow analysis

结束 liveness 分析，就可以开始进行插入 &phi;-node 和 rename 的工作了。插入 &phi;-node 比较简单，因为已经有了 IDF。

为什么要对 PHIBlocks 进行排序？目前能想到的唯一理由就是让 `Version` 增加的顺序跟 BB 的顺序一致。

### RenamePass

手动将递归转换为了迭代，见 commit 记录：

> Change the rename pass to be "tail recursive", only adding N-1 successors
> to the worklist, and handling the last one with a 'tail call'. This speeds
> up PR1432 from 2.0578s to 2.0012s (2.8%)
>
> — Chris Lattner

```c++
NextIteration:
  // 当前 BB 是否有 phi-node? 注意, 我们假设 BB 中所有的 phi-node 一定出现
  // 在开头的部分
  if (PHINode *APN = dyn_cast<PHINode>(BB->begin())) {
    // 由于我们的 phi-node 是在 BB 的最前面插入的, 所以如果
    // PhiToAllocaMap.count(APN) 不为 0, 那么说明我们在当前 BB 插入了
    // phi-node.
    //
    // 有了这两个保证, 我们可以确认当前 BB 有需要 rename 的 phi-node.
    if (PhiToAllocaMap.count(APN)) {
      /* ... */
      // 第一个被处理的 BB 是 entry, 但是由于 entry 没有 phi-node,
      // 所以控制流根本不会走到这里, 因此不会崩溃.
      unsigned NumEdge = llvm::count(successors(Pred), BB);
    }
  }
```

接下来就是给 &phi;-node 添加入边，流程很直观：首先获取 &phi;-node 的迭代器（也就是 Instruction 的迭代器，调用 `BB.begin` 即可），然后给遇到的每一个 &phi;-node 调用 `addIncoming` 添加入边，同时还要记得由于 &phi;-node 本身就是变量的 def，要用它更新变量当前的值。

不过，这里有一个需要注意的问题：我们并没有记录 mem2reg 在当前 BB 添加了多少 &phi;-node，因此需要其他的方式确定何时停止。LLVM 解决的方式基于一个前提，即，新添加的（没有调用过 `addIncoming` 的）&phi;-node 的 `getNumOperands` 的结果都是一样的，一旦遇见一个不一样的，说明就不是 mem2reg 添加的，就可以终止。

```c++
do {
  ++PNI;
  APN = dyn_cast<PHINode>(PNI);
  if (!APN)
    break;
  // 这里的判断基于下一个待处理的 phi-node
} while (APN->getNumOperands() == NewPHINumOperands);
```

TODO: 正常情况下 pred 到 BB 为什么会有多条边
TODO: 疑惑，直觉上来讲，新增的 &phi;-node 的 `getNumOperands` 应该就是 0，那么直接用 0 就好了，为啥还要进行一次调用？

处理完 &phi;-node 后，就是替换 load 和 store 的部分，这里很直观，跟我们自己实现的差不多，就不再赘述了。

最后是“递归”的部分。首先将 BB 和 Pred 都更新为第一个 succ，然后将其他的后继 BB 都添加到 work list 中（这一步要跟外面的 while 循环配合才能完成使命），最后，跳回 `RenamePass` 的开始。本质上完成了一个先序 DFS 遍历。

**递归转迭代**

我们只讨论直接递归（关于直接递归和间接递归，参见 [Recursion: Direct vs Indirect](https://www.baeldung.com/cs/recursion-direct-vs-indirect)）。首先明确一点，递归的轨迹是一棵树（自然，可以退化成线性序列），这样我们讨论的问题就变成了如何将递归的树遍历算法转换为迭代形式。通常，遍历一棵树有三种形式：先序遍历、后序遍历和中序遍历，其中中序遍历是针对二叉树的特殊情况。

先来看先序遍历。LLVM 已经为我们做出了示范：

* 将参数打包，保存在一个数据结构中
* 无限循环，通过 break 或者 return 结束
* 在循环的末尾设置好下一次“递归”的“参数”，将其他的部分保存起来

至于中序和后序，我也暂时没弄明白，尬笑……

## 总结

本文首先介绍了引入 mem2reg 的[背景](#sec:background)，然后展示了若干简单的[实验](#实验)加深对 mem2reg 功能的理解，接下来讲解了[如何实现自己的 mem2reg](#自己实现一个-mem2reg)，最后，本文详细地讲解了 [LLVM 的实现](#llvm-的实现)。

总体而言，mem2reg 是 LLVM 中一个比较简单的 pass，不过通过学习这个 pass，我们了解了不少思想、相关的实现技术和需要注意的细节。

## 参考

* [[[bib:opt-no-effect,1]]] [LLVM opt mem2reg has no effect](https://stackoverflow.com/questions/46513801/llvm-opt-mem2reg-has-no-effect)
* [[[bib:writing-an-llvm-pass-new-pm,2]]] [Writing an LLVM Pass (with the new pass manager)](https://llvm.org/docs/WritingAnLLVMNewPMPass.html)
* [[[bib:llvm-progman,3]]] [LLVM Programmer’s Manual](https://llvm.org/docs/ProgrammersManual.html:)
* [[[bib:llvm-langref,4]]] [LLVM Language Reference](https://llvm.org/docs/LangRef.html)
* [[[bib:c11-annex-j.2,5]]] [J.2 Undefined behavior](http://port70.net/~nsz/c/c11/n1570.html#J.2)
* [[[bib:llvm-langref-volatile,6]]] [Volatile Memory Accesses](https://llvm.org/docs/LangRef.html#volatile-memory-accesses)
* [[[bib:liveness-analysis,7]]] [Live-variable analysis - Wikipedia](https://en.wikipedia.org/wiki/Live-variable_analysis)

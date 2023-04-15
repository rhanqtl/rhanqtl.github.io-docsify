---
title: "Local Analysis"
date: 2023-03-02T10:25:27+08:00
tags: []
summary: \-

draft: true
---

Local analysis 是优化中重要的一环，其中包括：

+ Local Value Numbering
+ Dead Code Elimination
+ Common Sub-expression Elimination
+ Constant Propagation
+ Constant Folding

其中最核心的就是 local value numbering，它为我们提供了一个框架，可以对其进行扩展然后实现后几个优化。

# Local Value Numbering

对于 BB 中的每一条指令，假定其格式为 `T ← L Op R`（扩展成支持更多的格式是比较容易的，这里的重点在于思想，所以就使用二元运算）。

基本算法如下：

1. 在表中查找 `L` 和 `R` 的编号（“value number”）
   1. 分别记为 `VN(L)` 和 `VN(R)`
   2. 如果没有找到，就分配新的编号
2. 构造 `(Op, VN(L), VN(R))` 元组，查找表中是否存在该元组
   1. 如果存在，将当前指令替换为对对应值的拷贝
   2. 如果不存在，分配新的编号，并将其插入表中

```python
from collections import defaultdict

def _make_counter():
    cnt = 0
    def f():
        ans = cnt
        cnt += 1
        return ans
    return f

def do_lvn(bb: [Instr]) -> [Instr]:
    """
    Perform local value numbering on a basic block and returns the transformed basic block.

    Args:
        - bb: the basic block to be transformed

    Returns:
        - The transformed basic block
    """
    new_bb = []
    var2num = defaultdict()
    for i in bb:

```

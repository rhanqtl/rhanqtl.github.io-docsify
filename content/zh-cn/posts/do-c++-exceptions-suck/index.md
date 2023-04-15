---
title: "C++ 的异常机制真的很烂吗？"
date: 2023-02-28T09:23:11+08:00
tags: []
summary: \-

draft: true
---

最近在学习 LLVM，发现 LLVM 完全不使用异常[^llvm-programmers-manual]，因此就去 Google 了一下。反对异常的人通常有这么几个论点：

+ 开启异常会导致编译产物变大。[^exceptions-just-say-no]中给出了详细的数据，增量不到 10%（但是注意这是 2008 年的文章，其中的数据现在可能不适用了）
+ 性能会严重下降。[^exceptions-just-say-no]从游戏开发的视角给出了性能下降的数据，FPS 从大约 85 降到了 67，下降了 18。
+ 不可控的开销。
+ 写出异常安全的代码很困难。

这跟我之前读到的 Bjarne Stroustrup 在 HOPL IV 的论文[^bjarne-hopl4]中的观点不一致，Bjarne 认为不应该回避异常，有条件的使用是没问题的（异常本就是为异常比率 1/10000 这种情况设计的）。因此，我决定研究一下异常到底有多大的代价、应该怎么用。


# 参考

[^llvm-programmers-manual]: https://llvm.org/docs/ProgrammersManual.html

[^exceptions-just-say-no]: http://www.codercorner.com/blog/?p=33

[^bjarne-hopl4]: https://dl.acm.org/doi/10.1145/3386320

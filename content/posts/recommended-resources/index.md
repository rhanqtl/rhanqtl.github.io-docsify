---
title: "推荐课程"
date: 2022-12-28 11:09:06
draft: false
summary: \-
---

# 编译器

## 入门

### 书

- Crafting Interpreters
  - 就实现技术而言，涵盖了 Tree-Walk 解释器和虚拟机
  - 就语言特性而言，涵盖了字面量、变量、运算符、控制流、FP、OOP

## 工具

### 书

- [Flex and Bison](https://www.oreilly.com/library/view/flex-bison/9780596805418/?_gl=1*1m4okjo*_ga*NzM3Mjc1NDUzLjE2NzUyMjEyODY.*_ga_092EL089CH*MTY3NTgyNTcyMy4zLjAuMTY3NTgyNTcyOC41NS4wLjA.) by John Levine
  - Flex 和 Bison 大概是如今（截止到 2023 年）最为广泛使用的词法分析和语法分析器生成器
  - 中文版的翻译大体上还可以，只是有一些地方有点奇怪（例如“语法糖”翻译成“语法甜头”）
  - 建议：
    - 第 4 章没有什么特别新的知识，可以放在后面看
    - 第 5、6 章的内容属于手册性质，而且内容与第 9 章有重叠，可以放在最后看或者跳过
    - 第 7 章需要一些 parsing 算法的知识（尤其是 LALR 和二义性），读起来会轻松一点
  - 有点过时，有条件建议直接看 Flex 和 [Bison](https://www.gnu.org/software/bison/manual/) 的官方手册，信息密度更大

# PLT

## 课程

### Programming Languages (Coursera)

+ Part A: https://www.coursera.org/learn/programming-languages
+ Part B: https://www.coursera.org/learn/programming-languages-part-b
+ Part C: https://www.coursera.org/learn/programming-languages-part-c

# 程序设计

## 设计模式

### 书

- Head First Design Patterns 不错的入门书籍，不过没有完全覆盖 23 个设计模式
- GoF 设计模式，经典之作
- Game Programming Patterns / Design Patterns Revisited

### 在线资源

- refactoring.guru
- Peter Norvig 关于动态语言中设计模式的使用的[演讲](http://www.norvig.com/design-patterns/design-patterns.pdf)

# 其它

## 课程

### The Missing Semester

+ 英文官网: https://missing.csail.mit.edu/
+ 中文网站（不确定是不是官网）: https://missing-semester-cn.github.io/

教授 CS 从业人员使用的各种工具。课程内容一览：

![](images/missing-semester-schedule.png)

## 书

+ 七周七语言 - 以七个编程语言为例展示了各种编程范式（面向对象、函数式编程、逻辑编程），对于开阔眼界有帮助，可惜的是都是浅尝辄止
+ Ruby Under a Microscope - 主要是讲解了 MRI 一些比较浅显的知识，包括怎样进行词法分析和语法分析、各种语义在 VM 中是如何实现的，对于了解 Ruby 有一定帮助；此外还涉猎了一点 JRuby 和 Rubinius 的实现。

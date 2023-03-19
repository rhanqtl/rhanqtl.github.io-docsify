---
title: "在 Bison 中如何构造列表"
date: 2023-02-11T21:02:57+08:00
tags: [编译器, Bison]
summary: 本文介绍在 Bison 中如何生成列表

draft: false
---

# 前言 #

生成 AST 时免不了处理列表（例如，函数调用的参数）：

```c++
struct Expr {};
struct CallExpr : public Expr {
  std::shared_ptr<Expr> fn;
  std::vector<std::shared_ptr<Expr>> args;
};
```

如果是手写 parser，可以这样：

```text
// assume that '(' is consumed
until current token is ')'
    expr ← parse an expression
    if lookahead token is not ')'
        consume a ','
    end
end
```

但是 Bison 并没有直接提供对列表表示的支持（例如，`*` 和 `+` 符号[^lists-in-antlr]），只能使用递归（有点像命令式语言跟函数式语言，不是吗？），本文介绍如何实现。

[^lists-in-antlr]: ANTLR 中支持，所以可以如[这个答案](https://stackoverflow.com/a/759055)中这样实现

# 依靠递归创建列表

Bison 支持两种递归：左递归和右递归，两种方式都可以创建列表，但是顺序不同、需要的内存也不同。

我们以列表字面量为例：

1. 一个列表字面量以一个 `[` 开始、中间是以 `,` 分隔的任意数量的表达式，最后以 `]` 结尾；
2. 表达式可以是浮点数，也可以是列表字面量

## 左递归

文法（省略语义动作）：

```bison
line: "\n"
    | list "\n" ;

list: NUMBER
    | list "," NUMBER
    ;
```

处理过程大概是这样的：

| 序号 | 栈                    | token 序列                 | 动作   |
|:-----|:----------------------|:---------------------------|:-------|
| 0    | $start                | 42 ',' 3.14 ',' 2.718 '\n' | -      |
| 1    | $start 42             | ',' 3.14 ',' 2.718 '\n'    | shift  |
| 2    | $start list           | ',' 3.14 ',' 2.718 '\n'    | reduce |
| 3    | $start list ','       | 3.14 ',' 2.718 '\n'        | shift  |
| 4    | $start list ',' 3.14  | ',' 2.718 '\n'             | shift  |
| 5    | $start list           | ',' 2.718 '\n'             | reduce |
| 6    | $start list ','       | 2.718  '\n'                | shift  |
| 7    | $start list ',' 2.718 | '\n'                       | shift  |
| 8    | $start list '\n'      |                            | reduce |
| 9    | $start line           |                            | reduce |

可以看到：

1. 整个序列是*从前向后* 构造；
2. 基本上类似于手写的、基于迭代的方式；
3. 所需内存比较稳定。

## 右递归

文法（省略动作）：

```bison
line: "\n"
    | list "\n"
    ;

list: NUMBER
    | NUMBER "," list
    ;
```

处理过程大概是这样的：

| 序号 | 栈                           | token 序列                 | 动作   |
|:-----|:-----------------------------|:---------------------------|:-------|
| 0    | $start                       | 42 ',' 3.14 ',' 2.718 '\n' | -      |
| 1    | $start 42                    | ',' 3.14 ',' 2.718 '\n'    | shift  |
| 2    | $start 42 ','                | 3.14 ',' 2.718 '\n'        | shift  |
| 3    | $start 42 ',' 3.14           |',' 2.718 '\n'              | shift  |
| 4    | $start 42 ',' 3.14 ','       | 2.718 '\n'                 | shift  |
| 5    | $start 42 ',' 3.14 ',' 2.718 | '\n'                       | shift  |
| 6    | $start 42 ',' 3.14 ',' list  | '\n'                       | reduce |
| 7    | $start 42 ',' list           | '\n'                       | reduce |
| 8    | $start list                  | '\n'                       | reduce |
| 9    | $start list '\n'             |                            | shift  |
| 10   | $start line                  |                            | reduce |

可以看到：

1. 整个序列是*从后向前* 构造；
2. 所需内存与列表长度成正比。

## 小结

如果是构造双向链表，两种方式都可以，不过应该更偏向于左递归，因为内存量少；如果构造列表（Java 的 ArrayList、Python 的 `list`），最好还是使用左递归；如果构造单向链表，可能右递归会容易处理一些（当然，也可以使用左递归 + 头插，最后翻转一下）。

> 这有点像函数式语言中的 `foldl` 和 `foldr`

Bison 的手册建议总是使用左递归。

# 结论

本文介绍了如何在 Bison 中创建列表。

完整代码见 [leftrec.yy](./code/leftrec.yy)、[rightrec.yy](./code/rightrec.yy)、[list.hpp](./code/list.hpp) 和 [Makefile](./code/Makefile).

# 参考

1. [3.3.3 Recursive Rules](https://www.gnu.org/software/bison/manual/html_node/Recursion.html)

---
title: "文法"
date: 2023-01-18T20:27:54+08:00
tags: []
summary: \-

draft: true
---

# 左递归

```antlr
expr : expr ('+'|'-'|'*'|'/') expr
     | NUMBER ;

NUMBER : [0-9]+ ;
```

左递归会使得文法无法正常工作

```antlr
expr : NUMBER ('+'|'-'|'*'|'/') expr ;
```

# 优先级和结合性

```antlr
expr   : term NEWLINE ;
term   : factor ( ('+'|'-') factor )* ;
factor : negate ( ('*'|'/') negate )* ;
negate : '-' negate
       | NUMBER
       | '(' expr ')'
       ;
```

## 为什么方法调用的优先级比函数调用高？

考虑如下的情况：

```c++
struct A {
 public:
  void f() {}
};

void f() {}

int main() {
  auto o = A();
  o.f();
}
```

如果函数调用的优先级更高，AST 就会是这样：

```text
 DOT
 / \
o  CALL
    |
    f
```

这显然不符合我们的认知，因为 `o` 跟 `f` 是绑定在一起的，我们期望的应该是这样：

```text
 CALL
  |
 DOT
 / \
o   f
```

```antlr
field : IDENT ( '.' IDENT )+ ;
method_call : field '(' ')' ;
```

这样只能够支持 `o.a.b.c.d` 或者 `o.a.b.c.d()`，如果我们想支持 `o.a().b().c.d()`，应该怎么做呢？

我们先来画一下 AST：

```text
                      CALL
                       |
                      DOT
                    /     \
                  DOT    <IDENT 'd'>
                /     \
              CALL   <IDENT 'c'>
               |
              DOT
            /     \
          CALL   <IDENT 'b'>
           |
          DOT
      /         \
<IDENT 'o'>   <IDENT 'a'>
```

```text
             METHOD_CALL
                /   \
             FIELD   d
             /  \
    METHOD_CALL  c
         /   \
METHOD_CALL   b
    /  \
   o    a
```

```antlr
field : IDENT '.' field
      | method_call ( '.)
```

# 错误产生式

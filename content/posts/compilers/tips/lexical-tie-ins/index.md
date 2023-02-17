---
title: "Bison 小技巧：tie-in"
date: 2023-02-10T15:02:57+08:00
tags: [编译器, Bison]
summary: \-

draft: true
---

# 前言

实现 C++ 编译器时，有一个比较恼人的点是嵌套模板的使用：

```c++
vector<vector<int>>
```

在 c++11 以前，必须在两个 `<` 之间插入空白符[^c++-nested-template-before-11]：

[^c++-nested-template-before-11]: [cpp.sh 上的示例](https://cpp.sh/?source=%23include+%3Ciostream%3E%0A%0Astruct+S+%7B%0A++++void+f()+%7B%0A++++++++this()%3B%0A++++%7D%0A++++%0A++++void+operator()()+const+%7B%0A++++++++std%3A%3Acout+%3C%3C+%22aha%22+%3C%3C+std%3A%3Aendl%3B%0A++++%7D%0A%7D%3B%0A%0Aint+main()+%7B%0A++++S+s%3B%0A++++s.f()%3B%0A%7D)

```c++
vector<vector<int> >
```

因为 `>>` 会默认被解析成右移运算符，这对于强迫症来说是难以接受的。C++11 给出的办法是，在存在尚未匹配的 `<` 的情况下，如果遇到 `>>`，则将其拆分为 `>`.

# Tie-in

前面提到的情况实际上就是 parser 中的上下文依赖，Bison 手册提供了相应的解决方案，称为“tie-in”。换言之，就是 parser 对外提供一些访问当前状态的接口，scanner 会根据不同的状态在相同的 lexeme 提供不同的 token 类别：

```bison
/* example.y */
type:
    IDENT       {
                  printf("%s", $<sval>1);
                  if (unmatched_open_bracket == 0) {
                    putchar('\n');
                  }
                }
  | IDENT       { printf("%s [ ", $<sval>1); }
      LT        { unmatched_open_bracket++; }
      type GT   {
                  unmatched_open_bracket--;
                  printf(" ]");
                  if (unmatched_open_bracket == 0) {
                    putchar('\n');
                  }
                }
  ;
```

> 这里使用了所谓的“midrule action”[^bison-midrule-actions]。

[^bison-midrule-actions]: [3.4.8 Actions in Midrule](https://www.gnu.org/software/bison/manual/bison.html#Midrule-Actions)

词法则是这样的：

```flex
/* example.l */
"<"     { return LT; }
">"     { return GT; }
">>"    {
          /* 如果当前没有未匹配的 <，认为是右移运算符 */
          if (unmatched_open_bracket == 0) {
            return RSHIFT;
          }
          /* 放回一个 >，下次用 */
          unput('>');
          return GT;
        }
```

# 错误处理


# 结论

本文介绍了一种在 parser 中处理上下文依赖的技术，称为“tie-in”，并使用 Bison 和 Flex 给出了示例实现。

完整的代码见 [tie_in.y](./code/tie_in.y)、[tie_in.l](./code/tie_in.l) 和 [Makefile](./code/Makefile).


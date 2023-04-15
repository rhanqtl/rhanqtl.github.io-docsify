---
title: "用 Flex 和 Bison 实现一个计算器"
date: 2023-02-04T11:27:54+08:00
tags: [编译器, Lex, Yacc, Flex, Bison]
summary: \-

draft: false
---

> 本文需要你有一定的 C/C++ 基础和编译原理基础，包括：了解 C/C++ 基本的编译模型，词法分析（会写正则表达式）、语法分析（至少能够定义文法，了解 BNF 更好）

# 词法规则定义

Flex 文件包含三个部分：一些 C 语言的声明定义、匹配 token 的规则（正则表达式）和匹配成功时的动作，以及 C 函数定义，三个部分之间用 `%%` 分开，如下：

```text
%{
  ... prologue ...
%}

... Bison declarations ...

%%

... grammar ...

%%

... epilogue ...
```

词法规则的第一部分如下：

```flex
%{
  #include <stdlib.h>

  #include "calc.tab.h"

  void yyerror(const char *fmt, ...);
%}

%option noyywrap nodefault

number  [0-9]+(\.[0-9]+)?([eE][-+]?[0-9]+)?
```

其中 `%{ ... %}` 内的部分会被原样拷贝到生成的 `.c` 文件中：
- `#include "calc.tab.h"` 是 Bison 生成的头文件，其中包含 token 的定义和 C 函数的声明
- `yyerror` 是报告错误的函数，定义在语法规则文件中
- `%option noyywrap` 表示不需要默认定义 `yywrap` 函数，这个函数时早期的遗留，现在几乎不使用
- `%option nodefault` 表示禁用默认的 `main` 函数

最后的 `number ...` 相当于给正则表达式起了个名字，后面可以通过名字引用。注意，不支持符号，这里跟常见的编程语言一致，认为负数实际上是对字面量应用取反运算符。

此法规则的第二部分如下：

```flex
%%

{number}    {
              yylval = atof(yytext);
              return NUM;
            }

[-+*/^()]   { return yytext[0]; }

\n          {
              yycolumn = 1;
              return yytext[0];
            }
[ \t\r\f]   ;

.           yyerror("invalid character");

%%
```

每一条规则都包含一个模式和一组动作。其中：
+ 第一个规则的 `number` 就是前一部分定义的正则表达式，其动作表示我们要将当前的 lexeme 转换为 `double` 赋给保存语义值的 yylval 变量，并返回 `NUM` 表示当前匹配到了 `NUM` 这个 token；
+ 第二条规则匹配到的都是单字符的运算符，不需要额外的名字，表示原样返回对应的字符；
+ 第四条规则的动作部分只有一个 `;`，表示将相关字符忽略掉，具体的，这里就是忽略除 `\n` 之外的空白符。
+ 最后一条规则表示任何没有匹配的字符都要报错

> Flex 有一个重要的限制是不支持 Unicode，如果想要支持 Unicode 只能自己实现词法分析器然后与语法分析器对接起来（通过 `yylex` 函数）。


# 语法规则定义

为了实现运算符的优先级和结合性，对于数学表达式，教科书上通常会给出这样的文法：

```
expr    → term
term    → term '+' factor | term '-' factor
factor  → factor '*' unary | factor '/' unary
unary   → primary | '-' unary
primary → NUM | '(' expr ')'
```

但是这种文法需要引入若干额外的名字（毕竟起名字是一件很难的事），而且关于优先级和结合性的信息是隐含的，不便于理解，因此 Bison 提供了一种简便的方式：

```bison
...

%left '+' '-'
%left '*' '/'
%nonassoc NEG
%right '^'

%%

expr : expr '+' expr
     | expr '-' expr
     | expr '*' expr
     | expr '/' expr
     | expr '^' expr
     | '-' expr %prec NEG

...
```

注意中间的 `%left` 这部分，`%left` 表示其后的运算符是左结合的，而从上到下的顺序表示优先级从高到低。

Bison 文件的总体结构与 Flex 文件相同，所不同的是具体的内容。

语法规则定义的第一部分如下：

```
%{
  #include <stdio.h>
  #include <math.h>

  const char *PROMPT = "=> ";

  int yylex(void);
  void yyerror(const char *format, ...);
%}

%define api.value.type {double}

%token NUM

%left '+' '-'
%left '*' '/'
%nonassoc NEG
%right '^'
```

与 Flex 文件相同，`%{ ... %}` 中的部分是一些 C 语言的头文件包含、常量定义和函数声明；`%define ...` 表示将 词法分析中保存语义值的变量 `yylval` 的类型 YYSTYPE 定义为 `double`；`%token NUM` 表示定义一个名为 `NUM` 的 token 类型；最后的几行就是前面提到的优先级和结合性的定义。

> 你可能会好奇为什么 token 类别是由语法分析器定义的。我没有看到任何关于这一点的解释，但是有一个合理的推测：Flex 和 Bison 不是一起开发的，Bison 甚至不要求词法分析器一定是 Flex 生成的；语法分析过程需要了解 token 类别，这有一种做法是让词法分析器定义或者使用单独的头文件，但是如你所见，Bison 可以对 token 定义优先级，这一点使得无论如何 `.y` 文件中都会出现一些相关的定义，不如就直接由 Bison 定义。

第二部分如下：

```bison
%%

input : %empty
      | input line { printf(PROMPT); }
      ;

line : '\n'
     | expr '\n'  { printf("%.17g\n", $1); }
     ;

expr : NUM                { $$ = $1; }
     | '(' expr ')'       { $$ = $2; }
     | expr '+' expr      { $$ = $1 + $3; }
     | expr '-' expr      { $$ = $1 - $3; }
     | expr '*' expr      { $$ = $1 * $3; }
     | expr '/' expr      { $$ = $1 / $3; }
     | expr '^' expr      { $$ = pow($1, $3); }
     | '-' expr %prec NEG { $$ = -$2; }
     ;
```

其中顶层规则 `input` 表示输入可以为空字符串（`%empty` 是 Bison 内置的表示空字符串的符号），也可以由若干个 `line` 组成，其动作在下一个部分解释；规则 `line` 表示一行可以直接给出换行，也可以有一个表达式；规则 `expr` 定义了我们支持的运算，其中 `$$` 表示产生式左侧所对应的语义值，<code>$<i>n</i></code> 表示产生式右侧第 `n` 个终结符或非终结符的语义值。

这里注意 `expr` 的倒数第二个产生式，因为前一部分并没有定义一元 `-` 运算符的优先级，而仅定义了 `NEG` 这个优先级，这里需要用 `%prec NEG` 将该产生式与对应的优先级关联起来。

第三部分如下：

```bison
%%

int main(int argc, char *argv) {
  printf(PROMPT);
  return yyparse();
}

void yyerror(const char *fmt, ...) {
  fprintf(stderr, "%s\n", fmt);
}
```

`main` 函数中首先打印提示符，然后调用 `yyparse` 函数开始解析，由于在语法分析完成之前（即，遇到 EOF）函数 `yyparse` 都不会返回，这里不能在 `while` 循环体中打印提示符，而是需要在解析完成一行（即，一个表达式后）打印新的提示符（因为这时已经在 `line` 中打印了表达式的值，所以不会干扰）；函数 `yyerror` 就是报告错误函数的实现。

# 改进：错误恢复

现在的计算器在出现语法错误时会直接崩溃，这不是我们期望的结果，一个健壮的计算器应该能够报告错误并从中恢复，为此，我们需要修改词法和语法的定义。

在词法规则定义中加入：

```flex
%{
  /* ... */
  int yycolumn = 1;

  #define YY_USER_ACTION \
    do { \
      yylloc.first_line = yylloc.last_line = yylineno; \
      yylloc.first_column = yycolumn; \
      yylloc.last_column = yycolumn + yyleng - 1; \
      yycolumn += yyleng;   \
    } while (0);
%}
```

其中：
- 由于 Flex 只能够自动维护行号（即 `yylineno`），所以这里定义一个变量 `yycolumn` 来手动维护列号
- Flex 在匹配到 token 并设置好 `yytext` 和 `yyleng` 后、在执行关联的语义动作之前，会展开 `YY_USER_ACTION` 钩子，给用户一些扩展空间，这里就是我们的定义，其中 `yylloc` 是用于表示 token 位置的结构体，其类型默认定义为

```c
typedef struct YYLTYPE {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
```

> 可以通过如下方式覆盖默认的 `YYLTYPE` 定义：
>
> ```bison
> %{
>   #include "location.h"
> %}
>
> %define api.location.type {location_t}
> ```
>
> 在 Flex 文件中也需要修改：
>
> ```flex
> %{
>   // 顺序不能颠倒，否则 calc.tab.h 中找不到 `location_t` 这个符号
>   #include "location.h"
>   #include "calc.tab.h"
> %}
> ```
>
> 原因见 [StackOverflow](https://stackoverflow.com/questions/10386567/yylloc-undefined-in-this-scope)
>
> 在另一个名为 `location.h` 的文件中，可以如下定义：
>
> ```c
> typedef struct __location {
>   const char * filename;
>   int first_line;
>   int first_column;
>   int last_line;
>   int last_column;
> } location_t;
> ```

在语法规则定义中加入：

```bison
%locations
/* ... */
```

表示开启位置特性

在 `line` 加入一条产生式：

```bison
line : error \n { yyerrok; }
```

`error` 是 Bison 保留的关键字，用于表示错误情况，动作中的 `yyerrok` 表示忽略错误，恢复到正常状态。

将除法的产生式修改为：

```bison
expr : expr '/' expr { if ($3 != 0) {
                         $$ = $1 / $3;
                       } else {
                         $$ = INT_MIN;
                         fprintf(stderr, "%d:%d-%d:%d: divide by zero\n",
                             @3.first_line, @3.first_column,
                             @3.last_line, @3.last_column);
                       }
                     }
```

我们会首先判断除数是否为 0，如果不为 0，正常进行除法运算，否则报错。其中 `@3` 是包含了产生式右侧第三个符号的位置的结构体（类型为 `YYLTYPE`）。

修改 `yyerror` 的定义：

```c
void yyerror(const char *fmt, ...) {
  extern char *yytext;
  fprintf(stderr, "%d:%d-%d:%d: \"%s\"\n  error: %s\n",
      yylloc.first_line, yylloc.first_column,
      yylloc.last_line, yylloc.last_column, yytext, fmt);
}
```

由于 `yytext` 是词法分析器中的变量，这里需要将其声明为 `extern`，但是并不建议直接使用，因为语法分析过程可能会 look ahead 导致 `yytext` 并不是出错的 token。

这样我们就有了一个健壮的，能够报告错误的计算器。

# 改进：读取/写入文件

在语法规则定义中加入：

```bison
%{
  /* ... */
  #include <stdbool.h>

  const char *filename = "<interactive>";
  bool interactive = true;
%}
```

修改 `input` 规则：

```bison
input : %empty
      | input line { if (interactive) {
                       printf(PROMPT);
                     }
                   }
      ;
```

修改 `main` 函数：

```c
int main(int argc, char *argv[]) {
  extern FILE *yyin;
  extern FILE *yyout;

  if (argc > 1) {
    filename = argv[1];
    yyin = fopen(filename, "r");
    interactive = false;
  }
  if (argc > 2) {
    yyout = fopen(argv[2], "w");
  }
  if (interactive) {
    printf(PROMPT);
  }
  int ret = yyparse();
  if (!interactive) {
    fclose(yyin);
    fclose(yyout);
  }
  return ret;
}
```

此外，还需要将所有的输出改为 `fprintf(yyout, ...)`

> 不过输出到文件应该不是太重要，完全可以用 I/O 重定向实现，这里只是为了介绍 `yyout`.

# 结论

本文首先介绍了计算器的基本词法规则定义和基本语法规则定义，详细解释了各部分的含义。然后，借助 Flex 和 Bison 提供的功能，本文改进了该计算器，使得其更加健壮且能够报告出错位置。最后，本文为计算器增加了读取并解释文件以及将结果输出到文件的功能。

源码见 [calc.y](./src/src/calc.y)、[calc.l](./src/src/calc.l)、[location.h](./src/include/location.h) 和 [Makefile](./src/Makefile).

# 参考

1. [3.5.1 Data Type of Locations - Bison Manual](https://www.gnu.org/software/bison/manual/html_node/Location-Type.html)


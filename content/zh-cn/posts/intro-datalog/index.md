---
title: "Datalog 简介"
date: 2023-03-28T15:54:13+08:00
tags: ["声明式编程", "逻辑编程", "Datalog"]
summary: \-

draft: true
---

```plantuml
@startuml

abstract class Token {}
class Variable extends Token {}
class Identifier extends Token {}
class String extends Token {}

class Constant extends Identifier, String {}

class PredicateSymbol extends Identifier, String {}
class Term extends Variable, Constant {}

class Literal{}
Literal -- PredicateSymbol
Literal -- Term

class Clause {}
Clause -- Literal
Clause -- Literal

' Assertion;
' Retraction;
' Query;
' Fact;
' Rule;

' Clause -- Assertion [label="\".\""];
' Clause -- Retraction [label="\"~\""];
' Clause -- Query [label="\"?\""];

' Clause -- Fact [label="w/o body"];
' Clause -- Rule [label="w/ body"];

@enduml
```

# 安装 DrRacket

DrRacket 内置了 Datalog 的语言支持，很好用，还有一点简介[^drracket-datalog]。

我使用的是 WSL2 + WSLg，可以在 WSL 中启动 GUI 程序

# Datalog 基础

Datalog 有四类 token：标点符号（punctuation）、变量（variable）、标识符（Identifier）和字符串（string）。

[^drracket-datalog]: https://docs.racket-lang.org/datalog/datalog.html

---
title: "LeetCode 640. 求解方程"
date: 2022-08-20T11:40:17+08:00
draft: false
summary: \-
---

<pre>
equation   ::= side "=" side
side       ::= operand 
             | operand operator side
operand    ::= number | number "x" | "x"
number     ::= one_nine digits
one_nine   ::= 1 | 2 | ... | 9
digits     ::= 0 | one_nine
operator   ::= "+" | "-"
<pre>
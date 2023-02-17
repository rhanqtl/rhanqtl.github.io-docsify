---
title: "词法作用域及其实现"
date: 2023-01-11T14:35:22+08:00
tags: [编译器, 解释器, 作用域, 词法作用域, 环境]
summary: \-

draft: true
---

# 作用域

常见的作用域分为两种：动态作用域和词法（静态）作用域。

## 动态作用域

动态作用域是指，对于一个符号，只有在运行时才能够准确地确定其所指的对象。使用这种作用域的语言包括 [Lua](https://www.lua.org/) 和 [JavaScript](http://www.ecmascript.org/)。

示例如下：

```lua
-- example.lua

function do_stuff()
  print a
end

do_stuff()
-- nil

a = 42

do_stuff()
-- 42
```

不难发现，动态作用域容易出错，难于对程序进行推导和优化。

## 词法作用域

词法作用域也称为“静态作用域”，顾名思义是与动态作用域相对的一种作用域。词法作用域要求，一个表达式中引用的符号必须是在编译期可以访问到的（换言之，通过阅读代码就可以知道引用关系）。

示例：

```ruby
# example.rb

def do_stuff
  p a
end

# ERROR!
# do_stuff

a = 42

# ERROR!
# do_stuff
```

```text
>>> a = 42
>>> def do_stuff
...   p a
...   p b
... end
>>> do_stuff
(irb):3:in `do_stuff`: undefined local variable or method `a' for main:Object (NameError)
>>> b = 3.14
>>> def do_stuff
...   p a
...   p b
... end
>>> do_stuff
(irb):3:in `do_stuff`: undefined local variable or method `a' for main:Object (NameError)
```

不过，即使同样生成使用词法作用域，Scheme、SML 和 Ocaml 的表现同 Ruby 和 Python 也不一样：

```ocaml
let f () = !x
```

会报错：

```text
Error: Unbound value x
```

Scheme 和 SML 与此类似。

这种差别的原因在于 Ruby 和 Python 使用隐式声明（implicit declaration），例如，在静态语言中，使用一个变量一般需要声明 + 初始化（有时也合称为“定义”），例如：

```rust
fn main(){
  // 声明
  let mut x;
  // 赋值
  x = 42;
}
```

而 Ruby 和 Python 对于赋值语句，如果左边的变量会事先没有定义，就会隐式地声明该变量。

```ruby
def make_counter
  cnt = 0
  def counter
    cnt += 1
    cnt
  end
end

cnt = make_counter
puts cnt()
# => 1
puts cnt()
# => 2
```

# 实现词法作用域

## 环境

作用域是一个概念，而环境就是其在运行时的实现。

从概念上讲，环境就是一个包含从名字到对象的映射。例如，对于如下的程序：

```ruby

```

## Upvalue

## 备份栈帧

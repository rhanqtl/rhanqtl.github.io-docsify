---
title: "Prolog 简介"
date: 2023-03-28T11:20:02+08:00
tags: ["程序设计语言", "Prolog"]
summary: \-

draft: true
---

> 最近在学习 NJU 的[《软件分析》](https://www.bilibili.com/video/BV1b7411K7P4/?share_source=copy_web&vd_source=5dbfbb1d06ff90087bd485b9d4e6d613)课程，有[一节](https://www.bilibili.com/video/BV1wa411k7Uv/?share_source=copy_web&vd_source=5dbfbb1d06ff90087bd485b9d4e6d613)讲了使用 Datalog[^datalog] 做程序分析，Google 一下发现 Datalog 源自 Prolog[^prolog]，刚巧之前看《七周七语言》[^7-weeks-7-langs]了解过 Datalog，就想先复习一下。

# 安装 SWI-Prolog

Prolog 主流的实现有两个：GNU Prolog 和 SWI-Prolog[^swi-prolog]，GNU 的实现更加符合 ISO 标准，但可惜的是官网已经挂掉了，尽管 GitHub 有手册的 [TeX 源码](https://github.com/didoudiaz/gprolog/tree/master/doc)，但是~~我太懒了~~，不想折腾，所以本文使用 SWI-Prolog。

我用的是 Arch WSL，直接运行如下的命令就可以：

```sh
pacman -S swi-prolog
```

安装成功之后进入 REPL：

```sh
swipl
```

# 基础

[^datalog]: https://en.wikipedia.org/wiki/Datalog

[^prolog]: https://en.wikipedia.org/wiki/Prolog

[^gprolog]: https://github.com/didoudiaz/gprolog/tree/master/doc

[^swi-prolog]: https://swi-prolog.org

[^7-weeks-7-langs]: https://book.douban.com/subject/10555435/

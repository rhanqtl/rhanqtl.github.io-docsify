---
title: 效率神器 tmux
date: 2022-07-28T11:06:53+08:00
draft: true
summary: ~
---

# 简介

摘录自官方 Wiki [^1]：

> tmux is a terminal multiplexer. It lets you switch easily between several programs in one terminal, detach them (they keep running in the background) and reattach them to a different terminal.

tmux 可以实现 nohup [^2] 的功能，但是强大得多，例如能够交互式工作 [^3]。

tmux 的结构如下：

# {% plantuml %}
# tmux ||--|{ session ||--|{ window ||-- |{ pane
# {% endplantuml %}

tmux 的外观如下图所示：

{% asset_img tmux-show.png "tmux 效果图" %}

当前只有一个 session，ID 为 `0`，其中有 13 个 window，序号分别为 `0` 到 `12`，其中 10 号 window 有单独的名字 `mysql-server`，其余 window 都运行着 Zsh，2 号 window 的 `*` 表示当前处于该窗口。可以看到 2 号 window 有 3 个 pane。

# 基本流程

> **符号约定**
> 
> 在需要出现快捷键的上下文中，`C` 表示 Ctrl，`M` 表示 Alt，`S` 表示 Shift。
> 如果使用连字符，表示要同时按下，例如 `C-a` 表示要同时按下 Ctrl 和 a，否则有先后顺序，例如 `C a` 表示先按 Ctrl 后按 a，两个规则可以嵌套，例如 `C-b M-a` 表示先同时按 Ctrl 和 b，松开后再同时按 Alt 和 a


此处使用 Python 内置的 http.server 为例

## 启动服务

1. `tmux new -s http-server` 创建一个 ID 为 `http-server` 的 session
2. `python -m http.server 8080` 启动 HTTP 服务器
   1. 此时通过快捷键 `C-b "` 可以在当前 window 创建一个新的 pane，可以在其中 `ss -lnpt | grep 8080` 看到 HTTP 服务器
3. `C-b d` 断开与当前 session 的连接（detach）
   1. 此时通过 `curl localhost:8080` 依然可以获取到 HTML 文档，说明服务并没有因为退出 tmux 而结束


## 重新连接到 session

1. ~tmux ls~ 可以看到 =http-server: 1 windows (created Fri Jul  1 16:29:40 2022)= 这样的输出行
2. ~tmux attach-session -t http-server~ 可以重新连接（attach）到 =http-server= 这个 session，可以看到之前的命令和输出还在

# 快捷键

## session 相关

| 快捷键 | 功能 | 备注 |
|:---|:---|:---|
| `C-b d` | 断开连接 |  |


## window 相关

| 快捷键 | 功能 | 备注 |
|:---|:---|:---|
| `C-b c` | 创建新的 window | |
| `C-b <N>` | 切换到 0 号 window | 只能在 0 到 9 之间切换 |
| `C-b w` | 从列表中选择 window | 上下功能比较符合直觉，在 window 项上左右方向键可以收起或展开 pane 列表 |
| `C-b %` | 将当前 pane 水平切分 | 助记：可以将 `%` 看作是用一条垂直线把一个大圆（当前 pane）切割成两个小圆（两个小 pane） |
| `C-b "` | 将当前 pane 垂直切分 | |
| `C-b ,` | 重命名当前 window | |


## pane 相关

| 快捷键 | 功能 | 备注 |
|:---:|:---:|:---:|
| `C-b x` | 关闭当前 pane |  |
| `C-b UP` | 导航到上面的 pane | 循环 |
| `C-b DOWN` | 导航到下面的 pane | 循环 |
| `C-b LEFT` | 导航到左面的 pane | 循环 |
| `C-b RIGHT` | 导航到右面的 pane | 循环 |

## 其他

| 快捷键 | 功能 |
|:---|:---|
| `C-b ?` | 展示快捷键列表（key bindings） |

# 参考文献

[^1]: https://github.com/tmux/tmux/wiki

[^2]: https://linux.die.net/man/1/nohup

[^3]: https://qr.ae/pvPumn
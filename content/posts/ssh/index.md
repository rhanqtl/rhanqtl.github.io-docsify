---
title: "SSH 学习笔记"
date: 2022-07-28T11:57:17+08:00
draft: true
summary: "~"
---

# SSH 是什么


# SSH key

参见 <>

# authorized_keys 文件和自动认证

# SSH 配置文件

`ssh` 命令可以从命令行接收配置，也可以使用配置文件的方式。配置文件有两个位置，分别是 `/etc/ssh/ssh_config` 和 `~/.ssh/config`[^2]。


[^digitalocean-ssh-essentials]: [SSH Essentials: Working with SSH Servers, Clients, and Keys](https://www.digitalocean.com/community/tutorials/ssh-essentials-working-with-ssh-servers-clients-and-keys)

# SSH 小技巧

## 免密码登录

`ssh-copy-id`

使用密钥对：

```plaintext
ssh-copy-id -i <identity-file> <user>@<host>
```

`identity-file` 是公钥，例如最常见的 `~/.ssh/id_rsa.pub`

使用密码：

```plaintext
ssh-copy-id [ -p <port> ] <user>@<host>
```

会要求输入密码，后续的登陆就可以不再输入密码

测试：

```bash
ssh <user>@<host>
```

1. https://www.ssh.com/academy/ssh/copy-id#copy-the-key-to-a-server
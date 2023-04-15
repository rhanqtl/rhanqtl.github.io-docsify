---
title: "Docker 基础"
date: 2022-12-29T09:06:53+08:00
draft: true
summary: \-
---

# 创建镜像

## 从容器创建

这里我们想要创建一个带 SSH 服务和 Python3 的镜像

首先，拉取 [`alpine`](https://hub.docker.com/_/alpine) 镜像并创建一个容器

```
docker run -itd --name ssh-demo alpine
```

进入容器（alphine 只有 sh）

```
docker exec -it ssh-demo sh
```

安装 `openssh-server` 和 `python3`

```
apk update && apk add openssh-server python3
```
---
title: "记一次 V2Ray 问题排查经历"
date: 2022-07-28T11:00:44+08:00
draft: true
summary: "排查 V2Ray 将访问 GitHub 的流量路由到 Blackhole 出站端点的问题"
---

# 缘起

最近手痒，将 Manjaro[^manjaro] + KDE[^kde] 换为了 Arch Linux[^arch-linux] + Awesome[^awesome-wm]，安装了 Google Chrome[^google-chrome] 并且配置了科学上网（使用 V2ray[^v2ray] 搭建）之后，发现大部分网页能够正常访问（包括走代理的），但是却无法访问 GitHub[^github]。

# 症状

访问 GitHub 时，提示 connection refused，错误码为 `ERR_CONNECTION_CLOSED`

{% asset_img todo.jpg "Google Chrome 错误提示" %}

# 排查过程

## 确认代理服务器是否正常

1. 使用 `ping`[^ping] 访问代理服务器的 IP 地址，正常
2. 使用手机经过代理访问 GitHub，正常

以上的步骤说明并不是代理服务器出了问题，这就将排查问题的范围缩小到了本机，包括本机网络（因为之前遇见过 Linux 可以走通代理但是 Windows 无法走通代理的情况）和 V2ray 客户端。

## 本机网络

并没有检查

## V2ray 客户端

V2ray 提供了日志记录的能力[^v2ray-logging-config]，配置如下：

```json
{
    "log": {
        "loglevel": "info",
        "access": "/var/log/v2ray/access.log",
        "error": "/var/log/v2ray/error.log"
    }
}
```

出口规则大概是这样的：

```json
"outbounds": [
    {
        "tag": "socks-proxy",
        "protocol": "vmess",
        "settings": {
            "vnext": [
                // ... 省略
            ]
        }
    },
    {
        "tag": "discarded",
        "protocol": "blackhole",
        "settings": {}
    }
]
```

Blackhole（黑洞）是一个出站数据协议，它会阻碍所有数据的出站，配合路由（Routing）一起使用，可以达到禁止访问某些网站的效果[^v2ray-protocol-blackhole]。

> **小插曲**
> 
> 在修改了 V2ray 的配置文件后重新启动 systemd 服务时还有一个小插曲，通过 `systemctl status` 查询 V2ray 服务的状态时显示 `failed` 并且有 `exit-code: 23` 的字样，通过 `journalctl -u v2ray` 发现是 `{access,error}.log` 没有权限（即，`Permission denied` 错误），因为 V2ray 服务是以用户 `nobody` 的身份运行的，而这两个日志文件的所有者是 `root`，将这两个文件的所有者改成 `nobody` 就好了。

access.log 有如下行：

```plaintext
//github.com [discarded]
```

也就是说，对 GitHub 的访问被本地代理服务器的路由规则分派（dispatch）到了 `discarded` 这个出站规则上：

<a id="v2ray-routing-config" />

```json
    "routing": {
        "domainStrategy": "IPOnDemand",
        "rules": [
            {
                "type": "field",
                "ip": ["geoip:private"],
                "outboundTag": "discarded"
            },
            {
                "type": "field",
                "domain": ["geosite:category-ads"],
                "outboundTag": "discarded"
            }
        ]
    }
```

修改了路由规则之后就能够正常访问 GitHub 了。

# 深层原因

## V2Ray 的 geoip 是什么？

GeoIP[^v2ray-geoip] 可以理解为一份 IP 地址列表，跟 V2Ray 的路由规则（`$.routing`）配合使用可以实现将对不同地址的请求分流。

V2Ray 中在设置路由规则时可以在 `$.routing.ip` 中以 `geoip:<tag>` 形式指定预定义的列表，这个列表由一个 `.dat` 文件存储，采用 Protocol Buffer 的格式进行编码[^v2ray-geoip-encoding]。

## 查看 GeoIP 文件的内容

如<a name="v2ray-routing-config">前文</a>所述，既然 GitHub 被路由到了 `discarded` 这个出站协议，那么 `github.com` 这个域名应该在 `geoip:private` 中，因此我们需要确认这一点。

V2Ray 官方提供了用于操纵 geoip 的命令行工具。

[^manjaro]: https://manjaro.org/

[^kde]: https://kde.org/

[^arch-linux]: https://archlinux.org/

[^awesome-wm]: https://awesomewm.org/

[^google-chrome]: https://www.google.com/intl/zh-CN/chrome/

[^v2ray]: https://www.v2ray.com/

[^github]: github.com

[^ping]: https://en.wikipedia.org/wiki/Ping_(networking_utility)

[^v2ray-logging-config]: https://v2ray.com/chapter_02/01_overview.html#logobject

[^v2ray-protocol-blackhole]: https://v2ray.com/chapter_02/protocols/blackhole.html

[^v2ray-geoip]: https://github.com/v2fly/geoip

[^v2ray-geoip-encoding]: https://github.com/v2fly/v2ray-core/blob/master/app/router/routercommon/common.proto
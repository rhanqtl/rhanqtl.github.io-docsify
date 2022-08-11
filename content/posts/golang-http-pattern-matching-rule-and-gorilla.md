---
title: "Go 标准库 net/http 模式匹配规则与 gorilla/mux"
date: 2022-08-10T02:20:17+08:00
draft: true
summary: "~"
---

# 前言

近期在阅读代码时看到了如下的代码：

```go
r := mux.NewRouter()

r.HandleFunc("/a", handle1)
r.HandleFunc("/b/c/d/e", handle2)
r.HandleFunc("/b/c/d", handle3)
r.PathPrefix("/b/c/d/").HandlerFunc(handle4)
r.HandleFunc("/b/f", handle5)
r.HandleFunc("/b/c/g", handle6)
r.PathPrefix("/").HandlerFunc(handle7)

http.Handle("/", r)
```

于是对如何匹配的产生了好奇，就做了一番实验。

# 实验

经过试验，如下

- `GET /a` `GET /b/c/d/e`：毫无疑问会分别经过 `handle1` `handle2`
- `GET /b/c/d`：经过 `handle3`
- `GET /b/c/d/`（注意末尾的 `/`）：经过 `handle4`
- `GET /b/c/d/z` 和 `GET /b/c/d/z/`：经过 `handle4`
- `GET /` `GET /x/y/z`：经过 `handle7`


如果将最后一行改成：

```go
http.Handle("/api", r)
```

则只能**精确**匹配 path 为 `/api` 的请求（没有末尾的 `/`），其他的都会 404

而如果将左后一行改成：

```go
http.Handle("/api/", r)
```

则所有包涵 `/api/` 前缀的会走到 `handle7`，`/api` 会返回 `Moved permanently`，其他的路径全部 404

类似地，如果将最后一行改成：

```go
http.Handle("/b/", r)
```

则所有包含 `/b/` 前缀的路径能够正常处理，根据具体情况经过 `handle[2-6]`，其他的路径 404

# 原因

这跟 Go 标准库 `net/http` 匹配路由的规则有关系。下面的代码是用于选择路由的：

```go
func (mux *ServeMux) match(path string) (h Handler, pattern string) {
    // 如果最后一行为 http.Handle(pattern, r)
    // 那么 mux.m 包含 pattern => ... 的键值对
	// Check for exact match first.
	v, ok := mux.m[path]
	if ok {
		return v.h, v.pattern
	}

    // 如果最后一行为 http.Handle(pattern, r) 并且 pattern 以 "/" 结束，那么 es 会包含
    // { pattern: pattern, ... } 的项
	// Check for longest valid match.  mux.es contains all patterns
	// that end in / sorted from longest to shortest.
	for _, e := range mux.es {
		if strings.HasPrefix(path, e.pattern) {
			return e.h, e.pattern
		}
	}
	return nil, ""
}
```

区别就在于最后对于 `es` 的处理，如果 `http.Handle(pattern, ...)` 的末尾有 `/`（这在标准库中被称作“sub-tree”），就能够使用模糊匹配。

# 参考

1. https://pkg.go.dev/net/http#ServeMux
2. `net/http` 源码
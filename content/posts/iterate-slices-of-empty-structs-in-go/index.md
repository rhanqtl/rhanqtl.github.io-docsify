---
title: "Go 中遍历 struct{} 切片"
date: 2022-12-30T22:27:43+08:00
draft: true
summary: "最近看了一篇关于 Go 语言高性能编程的文章，在自己做实验时注意到遍历 struct{} 切片的不同方式居然会有不一样的性能表现，于是就想一探究竟。"
---

# 现象

废话不多说直接上代码：

```go
package main

import "testing"

type Item struct{}

func BenchmarkIndexStructSlice(b *testing.B) {
 var items [1024]Item
 for i := 0; i < b.N; i++ {
  var tmp Item
  for j := 0; j < len(items); j++ {
   tmp = items[j]
  }
  _ = tmp
 }
}

func BenchmarkRangeIndexStructSlice(b *testing.B) {
 var items [1024]Item
 for i := 0; i < b.N; i++ {
  var tmp Item
  for k := range items {
   tmp = items[k]
  }
  _ = tmp
 }
}

func BenchmarkRangeStructSlice(b *testing.B) {
 var items [1024]Item
 for i := 0; i < b.N; i++ {
  var tmp Item
  for _, item := range items {
   tmp = item
  }
  _ = tmp
 }
}
```

使用 `go test -bench=. range_test.go` 的输出如下：

```text
goos: linux
goarch: amd64
cpu: Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
BenchmarkIndexStructSlice-8              1866397               591.5 ns/op
BenchmarkRangeIndexStructSlice-8         2014816               683.3 ns/op
BenchmarkRangeStructSlice-8              1973628               588.3 ns/op
PASS
ok      command-line-arguments  5.518s
```

而使用 `go test -bench=. -gcflags='-N' range_test.go` 的输出如下：

```text
goos: linux
goarch: amd64
cpu: Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
BenchmarkIndexStructSlice-8               542804              1883 ns/op
BenchmarkRangeIndexStructSlice-8          728491              1658 ns/op
BenchmarkRangeStructSlice-8               802605              1511 ns/op
PASS
ok      command-line-arguments  3.508s
```




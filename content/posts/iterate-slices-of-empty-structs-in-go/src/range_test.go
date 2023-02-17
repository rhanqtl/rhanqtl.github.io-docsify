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
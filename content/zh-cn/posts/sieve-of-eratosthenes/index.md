---
title: "埃氏筛法及其实现"
date: 2023-04-08T15:23:42+08:00
tags: []
summary: \-

draft: false
---

# 埃氏筛法是什么

埃氏筛法是一种计算素数的方法。某种意义上而言，该算法不是检验一个数是否符合素数的条件，而是将所有的合数过滤掉，因此得名“筛法”。

## 流程

输入为 `n`，表示需要找出所有不大于 `n` 的素数。

具体流程是这样的：

1. 创建列表 `[2, 3, ..., n]`
2. 令 `p = 2` 为最小的素数
3. 枚举所有 `2p`、`3p` 等等，将它们标记为合数（注意 `p` 本身不能被标记）
4. 找出比 `p` 大的最小的未被标记的数，如果有，记为 `k`，令 `p = k`，回到步骤 3，否则，结束循环

还有另一种思路：

1. 令列表 `primes = []`，用于记录所有的素数
2. 对于 `2 ..= n`[^range-notation] 范围中的每个数 `k`，如果 `primes` 为空或者其中的任何一个数都不能整除 `k`，将 `k` 加入到 `primes` 中

[^range-notation]: 我们参考 Rust 的符号，用 `a .. b` 表示集合 `{ x | a <= x < b }`，用 `a ..= b` 表示集合 `{ x | a <= x <= b }`

## 示例

假设我们有序列

<pre>
    2 3 4 5 6 7 8 9 10 11 12 13 14 15
</pre>

第一轮，将所有 `2n` 过滤掉：

<pre>
    2 3 5 7 9 11 13 15
</pre>

第二轮，将所有 `3n` 过滤掉：

<pre>
    2 3 5 7 11 13
</pre>

第三轮，将所有 `5n` 过滤掉：

<pre>
    2 3 5 7 11 13
</pre>

依此类推。

# 实现

## Python

### 返回列表

```python
def primes(n):
    primes = []
    for i in range(2, n + 1):
        if all(i % d != 0 for d in primes):
            primes.append(i)
    return primes
```

### 生成器

只需要对上面的程序进行小小的改动：

```python
def primes(n):
    primes = []
    for i in range(2, n + 1):
        if all(i % d != 0 for d in primes):
            primes.append(i)
            yield i
```

无穷序列：

```python
import itertools

def primes(n = None):
    primes = []
    # Python 的 range 不支持无穷序列，用 itertools.count 解决 [2]
    rng = range(2, n+1) if n is not None else itertools.count(start=2)
    for i in rng:
        if all(i % p != 0 for p in primes):
            primes.append(i)
            yield i
```

## 并发版本

用 Go 实现

```go
func main() {
	var channels []chan int
	for i := 0; i < 5; i++ {
		channels = append(channels, make(chan int))
	}

	var wg sync.WaitGroup
	wg.Add(6)

  // Enumerate
	go func() {
    out := channels[0]
		for i := 2; i <= 11; i++ {
			out <- i
		}
		close(out)
		wg.Done()
	}()

  // Sieve-ers
	for i := 0; i < 5; i++ {
		var out chan int
		if i != 4 {
			out = channels[i+1]
		} else {
			out = nil
		}
		go func(in <-chan int, out chan<- int) {
			p := <-in
			println(p)
			for n := range in {
				if n%p != 0 && out != nil {
					out <- n
				}
			}
			if out != nil {
				close(out)
			}
			wg.Done()
		}(channels[i], out)
	}

	wg.Wait()
}
```


# 参考

1. https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
2. https://stackoverflow.com/questions/9884213/looping-from-1-to-infinity-in-python

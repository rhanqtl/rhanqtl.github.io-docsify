---
title: "Missing Semester 作业记录"
date: 2022-12-28T14:25:46+08:00
draft: true
summary: \-
---

> 在公司的最后几天，有点心不在焉，学不进去，就来看看 Missing Semester 的作业

# 第 01 讲

https://missing.csail.mit.edu/2020/course-shell/

**Ex. 2**

```
mkdir /tmp/missing
```

**Ex. 3**

```
$ which touch
$ whereis touch
$ whatis touch
$ man touch
```

**Ex. 4**

```
$ touch /tmp/missing/semester
```

Ex 6. 

没有 x 权限

Ex. 7

因为 semester 是 sh 的参数，只要 sh 是可执行的并且 semester 可读就可以

Ex. 9

```
chmod u+x semester
```


**Ex. 10**

```
./semester | grep 'last modified' > ~/last-modified.txt
```

**Ex. 11**

```
$ cat /sys/power/state
```

# 第 02 讲

https://missing.csail.mit.edu/2020/shell-tools/

**Ex. 1**


```
$ ls -aGthl
```

Ex. 2

见 ![macro.sh](./src/lec02-shell-tools/macro.sh)

Ex. 3

见 ![maybe_fail_runner.sh](./src/lec02-shell-tools/maybe_fail_runner.sh)

Ex. 4

目录结构：

```text
.
|-- a.html
|-- b
|   |-- c
|   |   `-- d.html
|   `-- d.txt
|-- e.rb
`-- f
    |-- g.html
    |-- h.html
    `-- i
```

```bash
find . -name '*.html' | xargs -d'\n' zip pages.zip
```

`-d` 用于制定定界符，使得文件名中包含空格的文件能够正确压缩

```
find . -name '*.html' -print0 | xargs -0 zip pages.zip
```

`-print0` 会打印以 `NUL` 结尾的字符串，`xargs` 的 `-0` 表示每一项都以 `NUL` 结尾

可以使用 `unzip -l pages.zip` 查看文件内容

Ex. 5

```bash
find . -type f -printf "%CF %CT %p\n" | sort -k1,2 -r
```

其中：
+ `-printf` 表示自定义输出格式，其中 `%CF %CT` 表示最后一次修改的日期时间，`%p` 表示文件名称
+ `-k1,2` 表示使用前两个部分，也就是日期时间，格式化符确保了能够直接按照字符串方式比较

# 第 04 讲

Ex. 2

```bash
egrep '^.*a.*a.*a.*$' /usr/share/dict/words  # find words containing at least 3 a's
  | grep -v "'s$"                            # filter out words ending with 's
```

```bash
# 如果要直接运行，需要去掉注释拼成一行
egrep '^.*a.*a.*a.*$' /usr/share/dict/words 
  | grep -v "'s$" 
  | grep -o '..$'                            # print last 2 characters, -o for "only matching"
  | sort | uniq -c                           # count unique items
  | sort -k1 -r                              # sort by count reversely
  | head -3                                  # print top 3 suffixes
```

![ex02_check.rb](./src/lec04-data-wrangling/ex02_check.rb) 可以用来检查

可以将 `head -3` 替换为 `wc -l` 得到组合数（150）

可以使用 `diff <(...) <(permute 2)` 得到未出现的组合，其中 permute 见 ![](./src/lec04-data-wrangling/perm.sh)


Ex. 03

见 `sed` 的 `-I` 和 `-i`


Ex. 04

可以使用 GNU [datamash](https://www.gnu.org/software/datamash/)

# 第 05 讲

## Job control

Ex. 2

```bash
sleep 60 &
wait $!; ls
```

```bash
function pidwait() {
  local pid=$1
  while kill -0 $pid; do
    sleep 1
  done
}
```

## Aliases

Ex. 1

```
alias dc=cd
```

## Remote Machines

SSH 相关内容见 TODO

# 参考

1. [find(1)](https://man7.org/linux/man-pages/man1/find.1.html)
2. [How to Find and Sort Files Based on Modification Date and Time in Linux](https://www.tecmint.com/find-and-sort-files-modification-date-and-time-in-linux/)
3. [sort(1)](https://man7.org/linux/man-pages/man1/sort.1.html)
4. [Shebang (Unix) - Wikipedia](https://en.wikipedia.org/wiki/Shebang_(Unix))
5. https://www.man7.org/linux/man-pages/man1/wait.1p.html
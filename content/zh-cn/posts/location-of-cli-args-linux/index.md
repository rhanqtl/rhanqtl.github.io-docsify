---
title: "Linux 下程序命令行参数的位置"
date: 2023-02-23T09:56:59+08:00
tags: []
summary: \-

draft: true
---

写程序时常常会用到命令行参数，但是从来没有深究过命令行参数从何而来，本文是阅读了 [^linking-loading-libraries] 之后的整理。全文以 C 语言为例。

# Linux 上可执行文件的入口点

在学习 C 语言是，我们知道 `main` 是程序的入口函数，但在 Linux 中，`main` 却不是真正的入口。

在编译 C 代码时可以用 `--verbose` 查看完整的过程：

```text
$ gcc -fno-builtin --verbose hello.c
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/lto-wrapper
Target: x86_64-pc-linux-gnu
Configured with: /build/gcc/src/gcc/configure --enable-languages=c,c++,ada,fortran,go,lto,objc,obj-c++,d --enable-bootstrap --prefix=/usr --libdir=/usr/lib --libexecdir=/usr/lib --mandir=/usr/share/man --infodir=/usr/share/info --with-bugurl=https://bugs.archlinux.org/ --with-build-config=bootstrap-lto --with-linker-hash-style=gnu --with-system-zlib --enable-__cxa_atexit --enable-cet=auto --enable-checking=release --enable-clocale=gnu --enable-default-pie --enable-default-ssp --enable-gnu-indirect-function --enable-gnu-unique-object --enable-libstdcxx-backtrace --enable-link-serialization=1 --enable-linker-build-id --enable-lto --enable-multilib --enable-plugin --enable-shared --enable-threads=posix --disable-libssp --disable-libstdcxx-pch --disable-werror
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 12.2.1 20230201 (GCC)
COLLECT_GCC_OPTIONS='-fno-builtin' '-v' '-mtune=generic' '-march=x86-64' '-dumpdir' 'a-'
 /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/cc1 -quiet -v hello.c -quiet -dumpdir a- -dumpbase hello.c -dumpbase-ext .c -mtune=generic -march=x86-64 -version -fno-builtin -o /tmp/ccGPBtT0.s
GNU C17 (GCC) version 12.2.1 20230201 (x86_64-pc-linux-gnu)
        compiled by GNU C version 12.2.1 20230201, GMP version 6.2.1, MPFR version 4.2.0, MPC version 1.3.1, isl version isl-0.25-GMP

GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
ignoring nonexistent directory "/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../../x86_64-pc-linux-gnu/include"
#include "..." search starts here:
#include <...> search starts here:
 /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/include
 /usr/local/include
 /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/include-fixed
 /usr/include
End of search list.
GNU C17 (GCC) version 12.2.1 20230201 (x86_64-pc-linux-gnu)
        compiled by GNU C version 12.2.1 20230201, GMP version 6.2.1, MPFR version 4.2.0, MPC version 1.3.1, isl version isl-0.25-GMP

GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
Compiler executable checksum: 99df1fbb4c4d7eeb8d93f0979bfab9ce
COLLECT_GCC_OPTIONS='-fno-builtin' '-v' '-mtune=generic' '-march=x86-64' '-dumpdir' 'a-'
 as -v --64 -o /tmp/ccVDfyBE.o /tmp/ccGPBtT0.s
GNU assembler version 2.40 (x86_64-pc-linux-gnu) using BFD version (GNU Binutils) 2.40
COMPILER_PATH=/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/:/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/:/usr/lib/gcc/x86_64-pc-linux-gnu/:/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/:/usr/lib/gcc/x86_64-pc-linux-gnu/
LIBRARY_PATH=/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/:/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../../lib/:/lib/../lib/:/usr/lib/../lib/:/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../:/lib/:/usr/lib/
COLLECT_GCC_OPTIONS='-fno-builtin' '-v' '-mtune=generic' '-march=x86-64' '-dumpdir' 'a.'
 /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/collect2 -plugin /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/liblto_plugin.so -plugin-opt=/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/lto-wrapper -plugin-opt=-fresolution=/tmp/ccF2y5l6.res -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lc -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s --build-id --eh-frame-hdr --hash-style=gnu -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 -pie /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../../lib/Scrt1.o /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../../lib/crti.o /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/crtbeginS.o -L/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1 -L/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../../lib -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../.. /tmp/ccVDfyBE.o -lgcc --push-state --as-needed -lgcc_s --pop-state -lc -lgcc --push-state --as-needed -lgcc_s --pop-state /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/crtendS.o /usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../../lib/crtn.o
COLLECT_GCC_OPTIONS='-fno-builtin' '-v' '-mtune=generic' '-march=x86-64' '-dumpdir' 'a.'
```

用 `objdump` 查看编译产物 `a.out`，可以看到 `main` 前面还有一段内容：

```text
$ objdump -s -d a.out

a.out:     file format elf64-x86-64


Disassembly of section .init:

0000000000001000 <_init>:
    1000:       f3 0f 1e fa             endbr64
    1004:       48 83 ec 08             sub    $0x8,%rsp
    1008:       48 8b 05 c1 2f 00 00    mov    0x2fc1(%rip),%rax        # 3fd0 <__gmon_start__@Base>
    100f:       48 85 c0                test   %rax,%rax
    1012:       74 02                   je     1016 <_init+0x16>
    1014:       ff d0                   call   *%rax
    1016:       48 83 c4 08             add    $0x8,%rsp
    101a:       c3                      ret

-- section .plt omitted --

Disassembly of section .text:

0000000000001040 <_start>:
    1040:       f3 0f 1e fa             endbr64
    1044:       31 ed                   xor    %ebp,%ebp
    1046:       49 89 d1                mov    %rdx,%r9
    1049:       5e                      pop    %rsi
    104a:       48 89 e2                mov    %rsp,%rdx
    104d:       48 83 e4 f0             and    $0xfffffffffffffff0,%rsp
    1051:       50                      push   %rax
    1052:       54                      push   %rsp
    1053:       45 31 c0                xor    %r8d,%r8d
    1056:       31 c9                   xor    %ecx,%ecx
    1058:       48 8d 3d da 00 00 00    lea    0xda(%rip),%rdi        # 1139 <main>
    105f:       ff 15 5b 2f 00 00       call   *0x2f5b(%rip)        # 3fc0 <__libc_start_main@GLIBC_2.34>
    1065:       f4                      hlt
    1066:       66 2e 0f 1f 84 00 00    cs nopw 0x0(%rax,%rax,1)
    106d:       00 00 00
    1070:       48 8d 3d a1 2f 00 00    lea    0x2fa1(%rip),%rdi        # 4018 <__TMC_END__>
    1077:       48 8d 05 9a 2f 00 00    lea    0x2f9a(%rip),%rax        # 4018 <__TMC_END__>
    107e:       48 39 f8                cmp    %rdi,%rax
    1081:       74 15                   je     1098 <_start+0x58>
    1083:       48 8b 05 3e 2f 00 00    mov    0x2f3e(%rip),%rax        # 3fc8 <_ITM_deregisterTMCloneTable@Base>
    108a:       48 85 c0                test   %rax,%rax
    108d:       74 09                   je     1098 <_start+0x58>
    108f:       ff e0                   jmp    *%rax
    1091:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    1098:       c3                      ret
    1099:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    10a0:       48 8d 3d 71 2f 00 00    lea    0x2f71(%rip),%rdi        # 4018 <__TMC_END__>
    10a7:       48 8d 35 6a 2f 00 00    lea    0x2f6a(%rip),%rsi        # 4018 <__TMC_END__>
    10ae:       48 29 fe                sub    %rdi,%rsi
    10b1:       48 89 f0                mov    %rsi,%rax
    10b4:       48 c1 ee 3f             shr    $0x3f,%rsi
    10b8:       48 c1 f8 03             sar    $0x3,%rax
    10bc:       48 01 c6                add    %rax,%rsi
    10bf:       48 d1 fe                sar    %rsi
    10c2:       74 14                   je     10d8 <_start+0x98>
    10c4:       48 8b 05 0d 2f 00 00    mov    0x2f0d(%rip),%rax        # 3fd8 <_ITM_registerTMCloneTable@Base>
    10cb:       48 85 c0                test   %rax,%rax
    10ce:       74 08                   je     10d8 <_start+0x98>
    10d0:       ff e0                   jmp    *%rax
    10d2:       66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)
    10d8:       c3                      ret
    10d9:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    10e0:       f3 0f 1e fa             endbr64
    10e4:       80 3d 2d 2f 00 00 00    cmpb   $0x0,0x2f2d(%rip)        # 4018 <__TMC_END__>
    10eb:       75 33                   jne    1120 <_start+0xe0>
    10ed:       55                      push   %rbp
    10ee:       48 83 3d ea 2e 00 00    cmpq   $0x0,0x2eea(%rip)        # 3fe0 <__cxa_finalize@GLIBC_2.2.5>
    10f5:       00
    10f6:       48 89 e5                mov    %rsp,%rbp
    10f9:       74 0d                   je     1108 <_start+0xc8>
    10fb:       48 8b 3d 0e 2f 00 00    mov    0x2f0e(%rip),%rdi        # 4010 <__dso_handle>
    1102:       ff 15 d8 2e 00 00       call   *0x2ed8(%rip)        # 3fe0 <__cxa_finalize@GLIBC_2.2.5>
    1108:       e8 63 ff ff ff          call   1070 <_start+0x30>
    110d:       c6 05 04 2f 00 00 01    movb   $0x1,0x2f04(%rip)        # 4018 <__TMC_END__>
    1114:       5d                      pop    %rbp
    1115:       c3                      ret
    1116:       66 2e 0f 1f 84 00 00    cs nopw 0x0(%rax,%rax,1)
    111d:       00 00 00
    1120:       c3                      ret
    1121:       66 66 2e 0f 1f 84 00    data16 cs nopw 0x0(%rax,%rax,1)
    1128:       00 00 00 00
    112c:       0f 1f 40 00             nopl   0x0(%rax)
    1130:       f3 0f 1e fa             endbr64
    1134:       e9 67 ff ff ff          jmp    10a0 <_start+0x60>

0000000000001139 <main>:
    1139:       55                      push   %rbp
    113a:       48 89 e5                mov    %rsp,%rbp
    113d:       48 83 ec 10             sub    $0x10,%rsp
    1141:       89 7d fc                mov    %edi,-0x4(%rbp)
    1144:       48 89 75 f0             mov    %rsi,-0x10(%rbp)
    1148:       48 8d 05 b5 0e 00 00    lea    0xeb5(%rip),%rax        # 2004 <_IO_stdin_used+0x4>
    114f:       48 89 c7                mov    %rax,%rdi
    1152:       b8 00 00 00 00          mov    $0x0,%eax
    1157:       e8 d4 fe ff ff          call   1030 <printf@plt>
    115c:       b8 00 00 00 00          mov    $0x0,%eax
    1161:       c9                      leave
    1162:       c3                      ret

Disassembly of section .fini:

0000000000001164 <_fini>:
    1164:       f3 0f 1e fa             endbr64
    1168:       48 83 ec 08             sub    $0x8,%rsp
    116c:       48 83 c4 08             add    $0x8,%rsp
    1170:       c3                      ret
```

# 参考

[^linking-loading-libraries]: 程序员自我修养：链接、装载与库

<h1 id="0">0 目录</h1>

* [12.1 MIPS启动代码](#12.1)
* [12.2 MIPS如何管理Cache](#12.2)
* [12.3 MIPS异常处理](#12.3)
* [12.4 MIPS中断处理](#12.4)
* [12.5 MIPS代码调优](#12.5)

---

This chapter is based on real workable chunks of software, which often means we sacrifice clarity and are less than concise; but then, real software is like that. We’re going to cover the following:

* MIPS启动代码： getting the CPU and system going, to the point where you can run a C program
* MIPS-Cache管理： an example of the system you might need to be able to invalidate and/or write back cache locations to make your I/O system work
* MIPS异常处理：grisly details from low-level interrupt to confortable C handler
* MIPS中断处理：not particularly efficient, but showing the essential features; builds on the structures of the previous section
* MIPS代码调优：a simple function rewritten in assembler with many tricks to improve bandwidth

<h2 id="12.1">12.1 MIPS启动代码</h2>
<h2 id="12.2">12.2 MIPS-Cache管理</h2>
<h2 id="12.3">12.3 MIPS异常处理</h2>

The exception-handling routines are once again taken (sometimes simplified) with permission from the Algorithmics SDE-MIPS — note again that the code is copyrighted and not freely reusable for commercial purposes. In this case, the mechanism is called xcption; it connects MIPS machine-level exceptions through to C interrupt handlers and POSIX-like signal handlers.

<h2 id="12.4">12.4 MIPS中断处理</h2>
<h2 id="12.5">12.5 MIPS代码调优</h2>

The following example is the heavily used C library function memcpy() tuned heroically. This is freely redistributable code from a BSD release, used with thanks to the University of California.

    #include <mipe/asm.h>
    #include <mips/regdef.h>
    /* 如果架构可以则用64位操作 */
    #if __mips >= 3
    #define L 1d
    #define LL ldl
    #define LR ldr
    #define S sd
    #define SL sdl
    #define SR sdr
    #define RS 8
    #else
    #define L 1w
    #define LL lwl
    #define LR lwr
    #define S sw
    #define SL swl
    #define SR swr
    #define RS 4
    #endif

    /* 根据大小端移动字节 */
    #ifdef MIPSEL
    # define LHI LR
    # define LLO LL
    # define SHI SR
    # define SLO SL
    #endif
    #ifdef MIPSEB
    # define LHI LL
    # define LLO LR
    # define SHI SL
    # defins SLO SR
    #endif

Let’s review these definitions. The strange instructions like ldr and ldl are for unaligned accesses; these two act in a pair to load a doubleword from an arbitrarily aligned location.

What we have defined are the following:

| 符号 | 意义 |
| ---- | ---- |
| L    | 加载一个WORD |
| S    | 存储一个WORD |
| RS   | 一个WORD的大小，按字节 |
| LHI  | 以非对齐加载WORD高字节 |
| LLO  | 以非对齐加载WORD低字节 |
| SHI  | 以非对齐存储WORD高字节 |
| SLO  | 以非对齐存储WORD低字节 |

We’ll also use free registers. The t0-t9 registers are by definition free for our use in a subroutine; so are the argument registers a0-a3 and the returnvalue registers v0 and v1.

    /* memcpy(to, from, n) */
    LEAF(memcpy)
    .set noreorder          # 不允许编译器进行指令序优化
    move v0, a0             # save to for return
    beq a2, zero, .ret
    sltu t2, a2, 12         # check for small copy
    bne t2, zero, .smallcpy # do a small bcopy

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
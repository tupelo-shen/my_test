<h1 id="0">0 目录</h1>

* [7.1 浮点数基本介绍](#7.1)
* [7.2 IEEE754标准及其背景知识](#7.2)
* [7.12 按需初始化和使能FPU](#7.12)

---

本章我们讨论一下MIPS架构中的FPU浮点单元。

<h2 id="7.1">7.1 浮点数基本介绍</h2>

浮点数在计算机中是如何表示的，可以自行google或者其它方法。

<h2 id="7.2">7.2 IEEE754标准及其背景知识</h2>

Because floating point deals with the approximate representations of numbers (in the same way as decimals do), computer implementations used to differ in the details of their behavior with regard to very small or large numbers. This meant that numerical routines, identically coded, might behave differently. In some sense these differences shouldn’t have mattered: You only got different answers in circumstances where no implementation could really produce a “correct” answer.

The use of calculators shows the irritating consequences of this: If you take the square root of a whole number and square it, you will rarely get back the whole number you put in, but rather something with lots of nines. Numerical routines are intrinsically hard to write and hard to prove correct. Many heavily used functions (common trigonometric operations, for example) are calculated by repeated approximation. Such a routine might reliably converge to the correct result on one CPU and loop forever on another when fed a difficult value.

The ANSI/IEEE Std 754 — 1985 IEEE Standard for Binary Floating-Point Arithmetic (usually referred to simply as the IEEE 754 standard) was introduced to bring order to this situation. The standard defines exactly what result will be produced by a small class of basic operations, even under extreme situations, ensuring that programmers can obtain identical results from identical inputs regardless of what machine they are using. Its approach is to require as much precision as is possible within each supported data format.

Perhaps IEEE754 has too many options, but it is a huge improvement on the chaos that motivated it; since it became a real international standard in 1985, it has become the basis for all new implementations.

The operations regulated by IEEE754 include every operation that MIPS FPAs can do in hardware, plus some that must be emulated by software. IEEE754 legislates the following:

<h2 id="7.3">7.3 IEEE754浮点数如何存储</h2>

IEEE recommends a number of different binary formats for encoding floatingpoint numbers, at several different sizes. But all of them have some common ingenious features, which are built on the experience of implementers in the early chaotic years.

The first thing is that the exponent is not stored as a signed binary number, but biased so that the exponent field is always positive: The exponent value 1 represents the tiniest (most negative) legitimate exponent value; for the 64-bit IEEE format the exponent field is 11 bits long and can hold numbers from 0 to 2,047. The exponent values 0 and 2,047 (all ones, in binary) are kept back for special purposes we’ll come to in a moment, so we can represent a range of exponents from −1,022 to +1,023.

For a number:

    mantissa × 2^exponent

we actually store the binary representation of:

    exponent + 1,023

in the exponent field.

The biased exponent (together with careful ordering of the fields) has the useful effect of ensuring that FP comparisons (equality, greater than, less than, etc.) have the same result as is obtained from comparing two signed integers composed of the same bits. FP compare operations can therefore be provided by cheap, fast, and familiar logic.

<h3 id="7.3.1">7.3.1 IEEE尾数和规格化</h3>

The IEEE format uses a single sign bit separate fromthe mantissa (0 for positive, 1 for negative). So the stored mantissa only has to represent positive numbers. All properly represented numbers in IEEE format are normalized, so:

    1 ≤ mantissa < 2

This means that the most significant bit of the mantissa (the single binary digit before the point) is always a 1, so we don’t actually need to store it. The IEEE standard calls this the hidden bit.

So now the number 93,000,000, whose normalized representation has a binary mantissa of 1.01100010110001000101 and a binary exponent of 26, is represented in IEEE 64-bit format by setting the fields:

    mantissafield = 01100010110001000101000 . . .
    exponentfield = 1049 = 10000011001

Looking at it the other way, a 64-bit IEEE number with an exponent field of E and a mantissa field of m represents the number f, where:

    f = 1.m × 2^(E−1023)

(provided that you accept that 1.m represents the binary fraction with 1 before the point and the mantissa field contents after it).

7.3.2 Reserved Exponent Values for Use with Strange Values

The smallest and biggest exponent field values are used to represent otherwiseillegal quantities.

E == 0 is used to represent zero (with a zero mantissa) and denormalized forms, for numbers too small to represent in the standard form. The denormalized number with E zero and mantissa m represents f, where:

    f = 0.m × 2^(−1022)

As denormalized numbers get smaller, precision is progressively lost. No MIPS FPU built to date is able to cope with either generating denormalized results or computing with them, and operations creating or involving them will be punted to the software exception handler. Modern MIPS FPUs can be configured to replace a denormalized result with a zero and keep going.

E == 111. . . 1 (i.e., the binary representation of 2,047 in the 11-bit field used for an IEEE double) is used to represent the following:

* With the mantissa zero, it is the illegal values +inf, −inf (distinguished by the usual sign bit).

* With the mantissa nonzero, it is a NaN. For MIPS, the most significant bit of the mantissa determines whether the NaN is quiet (MS bit 0) or signaling (MS bit 1). This choice is not part of the IEEE standard and is opposite to the convention used by most other IEEE-compatible architectures.3

<h3 id="7.3.3">7.3.3 MIPS架构浮点数据格式</h3>

MIPS架构采用了IEEE754的两种浮点数：

* 单精度

    在内存中占用32位的存储空间。MIPS的编译器使用单精度表示C语言的float类型。

* 双精度

    在内存中占用64位的存储空间。MIPS编译器使用双精度表示C语言的double类型。

浮点数在内存和寄存器中的布局，参考下图：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_7_1.PNG">

表示浮点数的C语言结构体，如下所示：

    #if BYTE_ORDER == BIG_ENDIAN /* 大端模式 */
    struct ieee754dp_konst {
        unsigned sign:1;
        unsigned bexp:11;
        unsigned manthi:20; /* 无法直接定义52位 */
        unsigned mantlo:32;
    };
    struct ieee754sp_konst {
        unsigned sign:1;
        unsigned bexp:8;
        unsigned mant:23;
    };
    #else           /* 小端模式 */
    struct ieee754dp_konst {
        unsigned mantlo:32;
        unsigned manthi:20;
        unsigned bexp:11;
        unsigned sign:1;
    };
    struct ieee754sp_konst {
        unsigned mant:23;
        unsigned bexp:8;
        unsigned sign:1;
    };
    #endif

<h2 id="7.4">7.4 IEEE754浮点数如何存储</h2>
<h2 id="7.5">7.5 IEEE754浮点数如何存储</h2>
<h2 id="7.6">7.6 IEEE754浮点数如何存储</h2>
<h2 id="7.7">7.7 IEEE754浮点数如何存储</h2>
<h2 id="7.8">7.8 IEEE754浮点数如何存储</h2>
<h2 id="7.9">7.9 IEEE754浮点数如何存储</h2>
<h2 id="7.10">7.10 IEEE754浮点数如何存储</h2>
<h2 id="7.11">7.11 IEEE754浮点数如何存储</h2>
<h2 id="7.12">7.12 按需初始化和使能FPU</h2>

通常情况下，一旦复位会将CPU的SR寄存器设置为禁止所有可选配的协处理器，包括FPU（协处理器1）。SR的CU1比特位可以设置FPU工作。对于遵循MIPS-III及以后的FPU，可以使用32个64位寄存器；对于MIPS-I兼容模式，可以使用32个32位寄存器（偶数号的寄存器可用）。

可以读取FPU的实现寄存器FIR；如果值为零，说明不支持FPU，应该将CU1设为0，禁止使用FPU。

一旦将CU1设为1，就应该配置FPU的控制状态寄存器FCSR（$31），设置自己的舍入模式和trap使能状态。除了最近舍入原则和禁止所有trap之外，别的情况较少使用。基于MIPS-III的CPU，还有一个选择，就是设置FS标志，使非常小的值舍为0，并保留一个trap给浮点仿真使用。这种做法与IEEE并不兼容，但这是因为硬件无法生成非规格化的结果。

一旦FPU开始工作，需要在中断和进程上下文切换的时候保存和恢复FPU相关寄存器。当然了，对于裸机程序，这一步就不需要了。但是因为这一步需要消耗时间，为了优化性能，可以效仿一些UNIX系统的做法：

* 当运行新任务的时候，默认禁止FPU。因为新任务现在还没有访问FPU，不必保存和恢复寄存器。

* 当发生`CU1不可用`的trap时，标志任务为使用FPU的任务，然后在返回之前使能FPU。

* 禁止内核和中断上下文中使用FPU。这样，在中断处理程序中不需要保存和恢复FPU寄存器，只有当在进程上下文切换的时候需要为使用FPU的任务保存和恢复寄存器。

# 7.13 Floating-Point Emulation
<h2 id="7.13">7.13 浮点仿真</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
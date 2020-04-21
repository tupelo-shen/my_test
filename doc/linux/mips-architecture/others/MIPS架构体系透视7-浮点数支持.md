<h1 id="0">0 目录</h1>

* [7.1 浮点数基本介绍](#7.1)
* [7.2 IEEE754标准及其背景知识](#7.2)
* [7.12 按需初始化和使能FPU](#7.12)

---

You are increasingly unlikely to meet a MIPS floating-point coprocessor — always known as floating-point accelerator, or FPA — in the flesh. In newer MIPS CPUs the FPA is either part of the CPU or isn’t there at all.

In 1987 the MIPS FPA set a new benchmark for performance for microprocessor math performance in affordable workstations. Unlike the CPU, which was mostly a rather straightforward implementation relying on its basic architecture for its performance, the FPA was a heroic silicon design brisling with innovation and ingenuity. Of course, now everyone has learned how to do it!

Since then the MIPS FPA has been pulled onward by Silicon Graphics’s need for math performance that would once have been the preserve of supercompeters. I expect to see a lot more embedded applications that need very high floating-point performance in the next few years, so even the most abstruse and high-end features may move rapidly down the MIPS family.

# 7.1 浮点数基本介绍
<h2 id="7.1">7.1 浮点数基本介绍</h2>

Floating-point math retains a great deal of mystery. You probably have a very clear idea of what it is for, but you may be hazy about the details. This section describes the various components of the data and what they mean. In so doing we are bound to tell most of you things you already know; please skip ahead but keep an eye on the text!

People who deal with numbers that may be very large or very small are used to using exponential (scientific) notation; for example, the distance from the earth to the sun is 93 × 106 miles. The number is defined by 93, themantissa, and 6, the exponent.

The same distance can be written 9.3 × 107 miles. Numerical analysts like to use the second form; a decimal exponent with a mantissa between 1.0 and 9.999. . . ... is called normalized.1 The normalized form is useful for computer representation, since we don’t have to keep separate information about the position of the decimal point.

Computer-held floating-point numbers are an exponential form, but in base 2 instead of base 10. Both mantissa and exponent are held as binary fields. Just changing the exponent into a power of two, the distance quoted above is 1.38580799102783203125 × 2^26 miles. The mantissa can be expressed as a binary “decimal”, which is just like a real decimal; for example, 

    1.38580799102783203125 = 1 + 3 × 10^(-1) + 8 × 10^(-2) + 5 × 10^(-3) + ...

is the same value as binary

    1.01100010110001000101 = 1 + 0 × 2^(-1) + 1 × 2^(-2) + 1 × 2^(-3) + ...

However, neither the mantissa nor the exponent are stored just like this in standard formats — and to understand why, we need to review a little history.

#7.2 IEEE754标准及其背景知识
<h2 id="7.2">7.2 IEEE754标准及其背景知识</h2>

Because floating point deals with the approximate representations of numbers (in the same way as decimals do), computer implementations used to differ in the details of their behavior with regard to very small or large numbers. This meant that numerical routines, identically coded, might behave differently. In some sense these differences shouldn’t have mattered: You only got different answers in circumstances where no implementation could really produce a “correct” answer.

The use of calculators shows the irritating consequences of this: If you take the square root of a whole number and square it, you will rarely get back the whole number you put in, but rather something with lots of nines. Numerical routines are intrinsically hard to write and hard to prove correct. Many heavily used functions (common trigonometric operations, for example) are calculated by repeated approximation. Such a routine might reliably converge to the correct result on one CPU and loop forever on another when fed a difficult value.

The ANSI/IEEE Std 754 — 1985 IEEE Standard for Binary Floating-Point Arithmetic (usually referred to simply as the IEEE 754 standard) was introduced to bring order to this situation. The standard defines exactly what result will be produced by a small class of basic operations, even under extreme situations, ensuring that programmers can obtain identical results from identical inputs regardless of what machine they are using. Its approach is to require as much precision as is possible within each supported data format.

Perhaps IEEE754 has too many options, but it is a huge improvement on the chaos that motivated it; since it became a real international standard in 1985, it has become the basis for all new implementations.

The operations regulated by IEEE754 include every operation that MIPS FPAs can do in hardware, plus some that must be emulated by software. IEEE754 legislates the following:

# 7.12 Initialization and Enabling on Demand
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
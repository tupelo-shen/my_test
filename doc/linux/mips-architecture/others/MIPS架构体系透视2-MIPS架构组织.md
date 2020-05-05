<h1 id="0">0 目录</h1>

* [1.1 流水线](#1.1)
    - [1.1.1 制约流水线效率的因素](#1.1.1)
    - [1.1.2 制约流水线效率的因素](#1.1.2)
* [1.2 MIPS架构5级流水线](#1.2)
* [1.3 RISC和CISC对比](#1.3)
* [1.4 MIPS架构的发展](#1.4)
* [1.5 MIPS架构和CISC架构的对比](#1.5)

---

The rather grandiose word architecture is used in computing to describe the abstract machine you program, rather than the actual implementation of that machine. That’s a useful distinction—and one worth defending from the widespread misuse of the termin marketing hype. The abstract description may be unfamiliar, but the concept isn’t. If you drive a stick-shift car you’ll find the gas pedal on the right and the clutch on the left, regardless of whether the car is front-wheel drive or rear-wheel drive. The architecture (which pedal is where) is deliberately kept the same, although the implementation is different.

Of course, if you’re a rally driver concerned with going very fast along slippery roads, it’s suddenly going to matter a whole lot which wheels are driven. Computers are like that too—once your performance needs are extreme or unusual, the details of the implementation may become important to you. 

In general, a CPU architecture consists of an instruction set and some knowledge about registers. The terms instruction set and architecture are pretty close to synonymous, and you can get both at once in the acronym ISA (Instruction Set Architecture).

These days the MIPS ISA is best defined by the MIPS32 and MIPS64 architecture specifications published by MIPS Technologies Inc. MIPS32 is a subset of MIPS64 for CPUs with 32-bit general-purpose registers. “The MIPS32 and MIPS64 architecture specifications” is a bit of a mouthful, so we’ll abbreviate it to “MIPS32/64.”

Most of the companies making MIPS CPUs are now making them compatible with these specifications—and those that are not strictly compatible are generally reducing differences as they can.

Before MIPS32/64, a number of versions of the MIPS architecture were written down. But those older definitions formally applied only to the instructions and resources used by higher-level software—they regarded the CPU control mechanisms necessary to operating systems as implementation dependent. That meant that portable operating system work depended on a gentleman’s agreement to keep the unspecified areas of the MIPS architecture stable, too. The way that worked out was that each architecture version was naturally associated with a “parent” implementation:

* MIPS I: 

    The instruction set used by the original 32-bit processors (R2000/3000); every MIPS CPU there ever was will run these instructions, so it is still common as a lingua franca.

* MIPS II: 

    A minor upgrade defined for the MIPS R6000, which didn’t get beyond preproduction. But something very like it has been widely used for 32-bit MIPS CPUs for the embedded market. MIPS II is the immediate parent of MIPS32.

* MIPS III: 

    The 64-bit instruction set introduced by the R4000.

* MIPS IV: 

    Adds a few useful instructions (mostly floating point) to MIPS III, appearing in two different implementations (R10000 and R5000).

* MIPS V:

    Adds some surprising two operations at once (“SIMD”) floatingpoint operations—but no MIPS V CPU was built. Most of it reappeared as an optional part of MIPS64 called “paired-single.”

* MIPS32, MIPS64: 

    Standards promulgated by MIPS Technologies Inc. after its demerger from Silicon Graphics in 1998. For the first time, the standards encompassed the “CPU control” functions known as coprocessor 0. MIPS32 is a superset of MIPS II, while MIPS64—which includes 64-bit instructions—is a superset of MIPS IV (and contains much of MIPS V as an optional extension).

    Most MIPS CPUs designed since 1999 are compatible with these standards, so we’ll use MIPS32/64 as the base architectures in this book. To be more precise, our base is Release 2 of the MIPS32/64 specifications, as published in 2003. Earlier MIPS32/64 CPUs might be referred to as Release 1.

The architecture levels define everything the original company documentation chose to define; up to MIPS V, that was typically rather more than enough to ensure the ability to run the same UNIX application and less than enough to ensure complete portability of code that uses OS or low-level features. By contrast, if a CPU conforms to MIPS32/64, it should run a reasonable portable operating system.

Early MIPS CPUs (starting with the R3000) had CPU control instructions and registers that are fairly different from MIPS32; they’re very old now, and these features aren’t detailed in this book.

Quite a few implementations add some of their own new instructions and interesting features. It’s not always easy to get software or tools (particularly compilers) that take advantage of implementation-specific features. We’ll describe most that I find interesting.

There are two levels of detail at whichwe can describe the MIPS architecture. The first (this chapter) is the kind of view you’d get if you were writing a user program for a workstation but chose to look at your code at the assembly level. That means that the whole normal working of the CPU would be visible.

In the later chapters we’ll take on everything, including all the gory details that a high-level operating system hides—CPU control registers, interrupts, traps, cache manipulation, and memory management. But at least we can cut the task into smaller pieces.

CPUs are often much more compatible at the user level than when everything is exposed. All known MIPS CPUs can run MIPS I user-level code.

#### Bringing Order to Instruction Set Extensions—ASEs

We’ve kept stressing that being RISC has very little to do with keeping the
instruction set small. In fact, RISC simplicity makes it temptingly easy to extend
the instruction set with new twists on three-operand register-to-register
calculations.

When MIPS CPUs began to be used in embedded systems, new instructions
aimed at helping out some particular application began to mushroom.
MIPS32/64 has included some of those inventions. But it also provides some
regulations, in the form of recognized ASEs (Application-Specific instruction
set Extensions). ASEs are optional extensions to MIPS32/64 whose presence is
marked in a standard way through configuration registers. There are already
quite a lot of them:

MIPS16e: This extension long predates MIPS32 and MIPS Technologies.
It was pioneered by LSI Logic in the 1990s, with a view to reducing the
size of MIPS binary programs. MIPS16 encodes a subset of user-privilege
MIPS instructions (and a few special ones added in) in 16-bit op-codes.
Such an instruction set is evidently “too small” and programs compile
into significantly more MIPS16 than regular MIPS instructions; however,
the half-size instructions lead to much smaller code. The MIPS16
instruction set was organized and slightly augmented to be an extension
of MIPS32, and the augmented version is called MIPS16e. This book
has few details about MIPS16e: It’s a large subject and rarely met up
with even at assembly level, but there are some words on the subject in
section B.1.

MDMX: Another old extension, this one championed by SGI. It adds a
large set of SIMD arithmetic operations using the FP (coprocessor 1) register
set. They’re SIMD because each operation runs in parallel on each
of a short array of integer values packed into the 64-bit registers.Many of
the operations are “saturating”; when a result would otherwise overflow,
the destination is filled with the nearest representable value. Saturating
small-integer SIMD operations accelerates a variety of audio and video
“media stream” computations. They’re also broadly characteristic of the
specialized CPUs called DSPs (Digital Signal Processors).
MDMX resembles the early versions of Intel’s MMX extension. But
MDMX was never implemented by SGI, and few implementations
survive: Broadcom’s CPUs seem to be the only ones.

SmartMIPS: A very small extension to the architecture aimed at improving
the performance of encryption operations believed to be key to the
smart card market; it is coupled to a bunch of other security- and sizerelated
tweaks to the CPU control system. One day the 32-bit smart card
market will be huge, but it hasn’t happened yet.

MT: A rather small extension in terms of instructions but a hugely significant
one: This adds hardware multithreading to MIPS cores. It was first
seen in MIPS Technologies’ 34-K family, launched in 2005. Appendix A
gives an overview of the MIPS MT system.

DSP: Like MDMX, this is a set of instructions held to be useful for
audio/video processing, with saturating and SIMD arithmetic on small
integers to the fore—but it already looks more useful in practice than
MDMX. It was also new in 2005, available in MIPS Technologies’ 24-K
and 34-K families. The DSP ASE was made public just in time to include
a sketchy description here in section B.2.

There are some other optional parts of the MIPS32/64 specification that
may not be regarded as primarily instruction set extensions:

* Floating point: The oldest are the best. Floating point has been an
optional part of the MIPS instruction set since the earliest days. But it fits
within the confines of the “coprocessor 1” encodings. It’s well described
in this book.

* CP2: The second coprocessor encoding region is free for brave customers.
But it’s a lot of design and testing work, and few try it. There’s nothing
more about it here.

* CorExtend: MIPS Technologies’ best shot at a relatively easy-to-use userdefinable
instruction set. The idea was much hyped in 2002/2003, with
announcements from companies such as ARM and Tensilica.

* EJTAG:An optional systemto improve the debugging facilities, described
in section 12.1.

* Paired single:An extension to the floating-point unit that provides SIMD
operations, each of which works simultaneously on two single-precision
(32-bit) sets of values.

* MIPS-3D: Usually associated with paired single, adds a handful of
instructions to help out with the floating-point matrix operations
heavily used by 3D scene rendering.



<h2 id="2.1">2.1 MIPS汇编语言的风格</h2>

Assembly language is the human-writable (and readable) version of the CPU’s
raw binary instructions, and Chapter 9 tells you more about how to understand
it. Readers who have never seen any assembly language will find some parts of
this book mystifying—but there’s no time like now to start.

For readers familiar with assembly language, but not the MIPS version, here
are some examples of what you might see:

        # this is a comment
    entrypoint: # that’s a label
        addu $1, $2, $3 # (registers) $1 = $2 + $3

Like most assembly languages, it is line oriented. The end of a line delimits
instructions, and the assembly’s native comment convention is that it ignores
any text on a line beyond a “#” character. But it is possible to put more than one
instruction on a line, separated by semicolons.

A label is a word followed by a colon “:”—word is interpreted loosely, and
labels can contain all sorts of strange characters. Labels are used to define entry
points in code and to name storage locations in data sections.

MIPS assembly programs interpret a rather stark language, full of register
numbers.Most programmers use the C preprocessor and some standard header
files so they can write registers by name; the names of the general-purpose registers
reflect their conventional use (which we’ll talk about in section 2.2). If you
use the C preprocessor, you can also use C-style comments.

A lot of instructions are three-operand, as shown. The destination register
is on the left (watch out, that’s opposite to the Intel x86 convention). In general,
the register result and operands are shown in the same order you’d use to write
the operation in C or any other algebraic language, so:

    subu $1, $2, $3

means exactly:

    $1 = $2 - $3;

That should be enough for now.

<h2 id="2.2">2.2 寄存器</h2>

There are 32 general-purpose registers for your programto use: $0 to $31. Two, and only two, behave differently than the others:

* $0

    Always returns zero, no matter what you store in it.

* $31 

    Is always used by the normal subroutine-calling instruction (jal) for the return address. Note that the call-by-register version (jalr) can use any register for the return address, though use of anything except $31 would be eccentric.

In all other respects, all these registers are identical and can be used in any instruction (you can even use $0 as the destination of instructions, though the resulting data will disappear without a trace).

In the MIPS architecture the program counter is not a register, and it is probably better for you not to think of it that way—in a pipelined CPU there are multiple candidates for its value, which gets confusing. The return address of a jal is the next instruction but one in sequence:

    ...
    jal printf
    mov $4, $6
    xxx         # return here after call

That makes sense, because the instruction immediately after the call is the
call’s delay slot—remember, the rules say itmust be executed before the branch
target. The delay slot instruction of the call is rarely wasted, because it is typically
used to set up a parameter.

There are no condition codes; nothing in the status register or other CPU
internals is of any consequence to the user-level programmer.

There are two register-sized result ports (called hi and lo) associated with
the integermultiplier. They are not general-purpose registers, nor are they useful
for anything except multiply and divide operations. However, there are
instructions defined that insert an arbitrary value back into these ports—after
some reflection, you may be able to see that this is required when restoring the
state of a program that has been interrupted.

The floating-point math coprocessor (floating-point accelerator, or FPU),
if available, adds 32 floating-point registers; in simple assembly language, they
are called $f0 to $f31.

Actually, for early 32-bit machines (conforming to MIPS I), only the 16
even-numbered registers were usable for math. But each even-numbered register
can be used for either a single-precision (32-bit) or double-precision (64-
bit) number; when you do double-precision arithmetic, register $f1 holds the
remaining bits of the register identified as $f0, and so on. In MIPS I programs
you only see odd-numbered math registers when you move data between
integer and FPU registers or load/store floating-point register values—and even
then the assembler helps you forget that complication.

MIPS32/64 CPUs have 32 genuine FP registers. But you may still meet software
that avoids using the odd-numbered registers, preferring to maintain software
compatibility with old CPUs. There’s a mode bit in one of the control
registers to make a modern FPU behave like an old one.

<h2 id="2.2.1">2.2.1 Conventional Names and Uses of General-Purpose Registers</h2>

We’re a couple of pages into an architecture description and here we are talking
about software. But I think you need to know this now.

Although the hardware makes few rules about the use of registers, their
practical use is governed by a forest of conventions. The hardware cares nothing
for these conventions, but if you want to be able to use other people’s subroutines,
compilers, or operating systems, then you had better fit in.

With the conventional uses of the registers go a set of conventional names.
Given the need to fit in with the conventions, use of the conventional names is
pretty much mandatory. The common names are listed in Table 2.1.

Somewhere about 1996 Silicon Graphics began to introduce compilers that
use new conventions. The new conventions can be used to build programs that
use 32-bit or 64-bit addressing, and in those two cases they are called, respectively,
“n32” and “n64.” We’ll ignore them for now, but there is a terse but
fairly complete description of them in section 11.2.8.

**Conventional Assembly Names and Usages for Registers**

* at: This register is reserved for the synthetic instructions generated by
the assembler. Where you must use it explicitly (such as when saving
or restoring registers in an exception handler), there’s an assembly
directive to stop the assembly from using it behind your back—but then
some of the assembler’s macro instructions won’t be available.

* v0, v1: Used when returning non-floating-point values from a subroutine.
If you need to return anything too big to fit in two registers, the
compiler will arrange to do it in memory. See section 11.2.1 for details.

* a0–a3: Used to pass the first four non-FP parameters to a subroutine.
That’s an occasionally false oversimplification—see section 11.2.1 for the
grisly details.

* t0–t9: By convention, subroutines may use these values without preserving
them. This makes them a good choice for “temporaries” when
evaluating expressions—but the compiler/programmer must remember
that the values stored in them may be destroyed by a subroutine call.




<h2 id="2.3">2.3 整数、乘法硬件单元及寄存器</h2>

<h2 id="2.4">2.4 加载与存储：寻址方式</h2>

<h2 id="2.5">2.5 存储器与寄存器的数据类型</h2>

<h2 id="2.6">2.6 汇编语言的合成指令</h2>

<h2 id="2.7">2.7 MIPS I到MIPS64指令集</h2>

<h2 id="2.8">2.8 基本地址空间</h2>

<h2 id="2.9">2.9 流水线可见性</h2>
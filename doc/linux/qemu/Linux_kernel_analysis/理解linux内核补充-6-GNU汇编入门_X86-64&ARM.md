<h1 id="0">0 目录</h1>

* [1 引言](#1)
* [2 开源汇编工具](#2)
* [3 X86汇编](#3)
    - [3.1 寄存器和数据类型](#3.1)
    - [3.2 寻址模式](#3.2)
    - [3.3 基本算术运算](#3.3)
    - [3.4 比较和跳转](#3.4)
    - [3.5 栈](#3.5)
    - [3.6 函数调用](#3.6)
    - [3.7 定义叶子函数](#3.7)
    - [3.8 定义复杂函数](#3.8)
* [4 ARM汇编](#4)
    - [4.1 寄存器和数据类型](#4.1)
    - [4.2 寻址模式](#4.2)
    - [4.3 基本算术运算](#4.3)
    - [4.4 比较和跳转](#4.4)
    - [4.5 栈](#4.5)
    - [4.6 函数调用](#4.6)
    - [4.7 定义叶子函数](#4.7)
    - [4.8 定义复杂函数](#4.8)
    - [4.9 ARM-64位](#4.9)
* [5 参考](#5)

---

<h2 id="1">1 引言</h2>

In order to build a compiler, you must have a working knowledge of at least one kind of assembly language. And, it helps to see two or more variations of assembly, so as to fully appreciate the distinctions between architectures. Some of these differences, such as register structure, are quite fundamental, while some of the differences are merely superficial.

We have observed that many students seem to think that assembly language is rather obscure and complicated. Well, it is true that the complete manual for a CPU is extraordinarily thick, and may document hundreds of instructions and obscure addressing modes. However, it’s been our experience
that it is really only necessary to learn a small subset of a given assembly language (perhaps 30 instructions) in order to write a functional compiler. Many of the additional instructions and features exist to handle special cases for operating systems, floating point math, and multi-media computing. You can do almost everything needed with the basic subset.

We will look at two different CPU architectures that are in wide use today: X86 and ARM. The Intel X86 is a CISC architecture that has evolved since the 1970s from 8-bit to 64-bit and is now the dominant chip in personal computers, laptops, and high performance servers. The ARM processor is a RISC architecture began life as a 32-bit chip for the personal computer market, and is now the dominant chip for low-power and embedded devices such as mobile phones and tablets.

This chapter will give you a working knowledge of the basics of each architecture, but you will need a good reference to look up more details. We recommend that you consult the Intel Software Developer Manual [1] and the ARM Architecture Reference Manual [3] for the complete details. (Note that each section is meant to be parallel and self-contained, so some explanatory material is repeated for both X86 and ARM.)

<h2 id="2">2 开源汇编工具</h2>

A given assembly language can have multiple dialects for the same CPU, depending on whether one uses the assembler provided by the chip vendor, or other open source tools. For consistency, we will give examples in the assembly dialect supported by the GNU compiler and assembler, which are known as `gcc` and `as` (or sometimes `gas`).

A good way to get started is to view the assembler output of the compiler for a C program. To do this, run `gcc` with the `-S` flag, and the compiler will produce assembly output rather than a binary program. On Unix-like systems, assembly code is stored in files ending with `.s`, which indicates “source” file.

If you run `gcc -S hello.c -o hello.s` on this C program:

    #include <stdio.h>
    
    int main( int argc, char *argv[] )
    {
        printf("hello %s\n","world");
        return 0;
    }

then you should see output similar to this in `hello.s`

    .file "test.c"
    .data
    .LC0:
            .string "hello %s\n"
    .LC1:
            .string "world"
    .text
    .global main
    main:
            PUSHQ %rbp
            MOVQ %rsp, %rbp
            SUBQ $16, %rsp
            MOVQ %rdi, -8(%rbp)
            MOVQ %rsi, -16(%rbp)
            MOVQ $.LC0, %rax
            MOVQ $.LC1, %rsi
            MOVQ %rax, %rdi
            MOVQ $0, %rax
            CALL printf
            MOVQ $0, %rax
            LEAVE
            RET

(There are many valid ways to compile `hello.c` and so the output of your compiler may be somewhat different.)

Regardless of the CPU architecture, the assembly code has three different kinds of elements:

1. **Directives**

Directives begin with a dot and indicate structural information useful to the assembler, linker, or debugger, but are not in and of themselves assembly instructions. For example, `.file` simply records the name of the original source file to assist the debugger. `.data` indicates the start of the data segment of the program, while `.text` indicates the start of the program segment. `.string` indicates a string constant within the data section, and `.global main` indicates that the label `main` is a global symbol that can be accessed by other code modules.

2. **Labels**

Labels end with a colon and indicate by their position the association between names and locations. For example, the label `.LC0`: indicates that the immediately following string should be called `.LC0`. The label `main`: indicates that the instruction `PUSHQ %rbp` is the first instruction of the main function. By convention, labels beginning with a dot are temporary local labels generated by the compiler, while other symbols are user-visible functions and global variables. The labels do not become part of the resulting machine code `per se`, but they are present in the resulting object code for the purposes of linking, and in the eventual executable, for purposes of debugging.

3. **Instructions** 

Instructions are the actual assembly code like (PUSHQ %rbp), typically indented to visually distinguish them from directives and labels. Instructions in GNU assembly are not case sensitive, but we will generally uppercase them, for consistency.

To take this `hello.s` and turn it into a runnable program, just run `gcc`, which will figure out that it is an assembly program, assemble it, and link it with the standard library:

    % gcc hello.s -o hello
    % ./hello
    hello world

It is also interesting to compile the assembly code into object code, and then use the `nm` utility to display the symbols (”names”) present in the code:

    % gcc hello.s -c -o hello.o
    % nm hello.o
    0000000000000000    T main
                        U printf

> <font color="blue">nm -> 是names的缩写，nm命令主要是用来列出某些文件中的符号（换句话说就是一些函数和全局变量）。</font>

This displays the information available to the linker. `main` is present in the text (T) section of the object, at location zero, and `printf` is undefined (U), since it must be obtained from the standard library. But none of the labels like `.LC0` appear because they were not declared as .global.

As you are learning assembly language, take advantage of an existing compiler: write some simple functions to see what `gcc` generates. This can give you some starting points to identify new instructions and techniques to use.

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3">3 X86汇编语言</h2>

X86 is a generic term that refers to the series of microprocessors descended from (or compatible with) the Intel 8088 processor used in the original IBM PC, including the 8086, 80286, ’386, ’486, and many others. Each generation of CPUs added new instructions and addressing modes from 8-bit to 16-bit to 32-bit, all while retaining backwards compatibility with old code. A variety of competitors (such as AMD) produced compatible chips that implemented the same instruction set.

However, Intel broke with tradition in the 64-bit generation by introducing a new brand (Itanium) and architecture (IA64) that was not backwards compatible with old code. Instead, it implemented a new concept known as Very Long Instruction Word (VLIW) in which multiple concurrent operations were encoded into a single word. This had the potential for significant speedups due to instruction-level parallelism but represented a break with the past.

AMDstuck with the old ways and produced a 64-bit architecture (AMD64)
that was backwards compatible with both Intel and AMD chips. While the
technical merits of both approaches were debatable, the AMD approach
won in the marketplace, and Intel followed by producing its own 64-bit
architecture (Intel64) that was compatible with AMD64 and its own previous
generation of chips. X86-64 is the generic name that covers both
AMD64 and Intel64 architectures.

X86-64 is a fine example of CISC (complex instruction set computing).
There are a very large number of instructions with many different submodes,
some of them designed for very narrow tasks. However, a small
subset of instructions will let us accomplish a lot.

<h3 id="3.1">3.1 寄存器和数据类型</h3>

X86-64 has sixteen (almost) general purpose 64-bit integer registers:

| 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
| -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
|%rax|%rbx|%rcx|%rdx|%rsi|%rdi|%rbp|%rsp| %r8| %r9|%r10|%r11|%r12|%r13|%r14|%r15|

These registers are *almost* general purpose because earlier versions of the processors intended for each register to be used for a specific purpose, and not all instructions could be applied to every register. The names of the lower eight registers indicate the purpose for which each was originally intended: for example, %rax is the accumulator.

> A Note on AT&T Syntax versus Intel Syntax
> 
> Note that the GNU tools use the traditional AT&T syntax, which is used across many processors on Unix-like operating systems, as opposed to the Intel syntax typically used on DOS and Windows systems. The following instruction is given in AT&T syntax:
> 
>       MOVQ %RSP, %RBP
>       
> MOVQ is the name of the instruction, and the percent signs indicate that RSP and RBP are registers. In the AT&T syntax, the source is always given first, and the destination is always given second.
> 
> In other places (such as the Intel manual), you will see the Intel syntax, which (among other things) dispenses with the percent signs and reverses the order of the arguments. For example, this is the same instruction in the Intel syntax:
> 
>       MOVQ RBP, RSP
>       
> When reading manuals and web pages, be careful to determine whether you are looking at AT&T or Intel syntax: look for the percent signs!


As the design developed, new instructions and addressing modes were added to make the various registers almost equal. A few remaining instructions, particularly related to string processing, require the use of `%rsi` and `%rdi`. In addition, two registers are reserved for use as the stack pointer (`%rsp`) and the base pointer (`%rbp`). The final eight registers are numbered and have no specific restrictions.

The architecture has expanded from 8 to 64 bits over the years, and so each register has some internal structure. The lowest 8 bits of the `%rax` register are an 8-bit register `%al`, and the next 8 bits are known as `%ah`. The low 16 bits are collectively known as `%ax`, the low 32-bits as `%eax`, and the whole 64 bits as `%rax`.

图1: X86 寄存器结构

<img id="Figure_1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/assembly_language_1.PNG">

The numbered registers %r8-%r15 have the same structure, but a slightly different naming scheme:

图2: X86 寄存器结构

<img id="Figure_2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/assembly_language_2.PNG">

To keep things simple, we will focus our attention on the 64-bit registers. However, most production compilers use a mix of modes: a byte can represent a boolean; a longword is usually sufficient for integer arithmetic, since most programs don’t need integer values above 2^32; and a quadword is needed to represent a memory address, enabling up to 16EB (exa-bytes) of virtual memory.

<h3 id="3.2">3.2 寻址模式</h3>

The `MOV` instruction moves data between registers and to and from memory in a variety of different modes. A single letter suffix determines the size of data to be moved:

| Suffix | Name     | Size              |
| ------ | -------- | ----------------- |
| B      | BYTE     | 1 byte (8 bits)   |
| W      | WORD     | 2 bytes (16 bits) |
| L      | LONG     | 4 bytes (32 bits) |
| Q      | QUADWORD | 8 bytes (64 bits) |

MOVB moves a byte, MOVW moves a word, MOVL moves a long, MOVQ moves a quad-word. Generally, the size of the locations you are moving to and from must match the suffix. In some cases, you can leave off the suffix, and the assembler will infer the right size. However, this can have unexpected consequences, so we will make a habit of using the suffix.

The arguments to MOV can have one of several addressing modes.

* A **global value** is simply referred to by an unadorned name such as `x` or `printf`, which the assembler translates into an absolute address or an address computation.

* An **immediate value** is a constant value indicated by a dollar sign such as $56, and has a limited range, depending on the instruction in use.

* A **register value** is the name of a register such as `%rbx`.

* An **indirect value** refers to a value by the address contained in a register. For example, `(%rsp)` refers to the value pointed to by `%rsp`.

* A **base-relative value** is given by adding a constant to the name of a register. For example, `-16(%rcx)` refers to the value at the memory location sixteen bytes below the address indicated by `%rcx`. This mode is important for manipulating stacks, local values, and function parameters, where the start of an object is given by a register.

* A **complex address** is of the form D(RA,RB,C) which refers to the value at address *RA + RB * C + D*. Both *RA* and *RB* are general purpose registers, while C can have the value 1, 2, 4, or 8, and D can be any integer displacement. This mode is used to select an item within an array, where RA gives the base of the array, RB gives the index into the array, C gives the size of the items in the array, and D is an offset relative to that item.

Here is an example of using each kind of addressing mode to load a 64-bit value into `%rax`:

| Mode          | Example                       |
| ------------- | ----------------------------- |
| Global Symbol | MOVQ x, %rax                  |
| Immediate     | MOVQ $56, %rax                |
| Register      | MOVQ %rbx, %rax               |
| Indirect      | MOVQ (%rsp), %rax             |
| Base-Relative | MOVQ -8(%rbp), %rax           |
| Complex       | MOVQ -16(%rbx,%rcx,8), %rax   |

For the most part, the same addressing modes may be used to store data into registers and memory locations. There are some exceptions. For example, it is not possible to use base-relative for both arguments of MOV: `MOVQ -8(%rbx), -8(%rbx)`. To see exactly what combinations of addressing modes are supported, you must read the manual pages for the instruction in question.

In some cases, you may want to load the address of a variable instead of its value. This is handy when working with strings or arrays. For this purpose, use the `LEA` (load effective address) instruction, which can perform the same address computations as MOV:

| Mode          | Example                       |
| ------------- | ----------------------------- |
| Global Symbol | LEAQ x, %rax                  |
| Base-Relative | LEAQ -8(%rbp), %rax           |
| Complex       | LEAQ -16(%rbx,%rcx,8), %rax   |

<h3 id="3.3">3.3 基本算术运算</h3>

You will need four basic arithmetic instructions for your compiler: integer addition, subtraction, multiplication, and division.

ADD and SUB have two operands: a source and a destructive target. For example, this instruction:

    ADDQ %rbx, %rax

adds `%rbx` to `%rax`, and places the result in `%rax`, overwriting what might have been there before. This requires a little care, so that you don’t accidentally clobber a value that you might want to use later. For example, you could translate c = a+b+b; like this:

    MOVQ a, %rax
    MOVQ b, %rbx
    ADDQ %rbx, %rax
    ADDQ %rbx, %rax
    MOVQ %rax, c

The `IMUL` instruction is a little unusual, because multiplying two 64-bit integers results in a 128-bit integer, in the general case. IMUL takes its argument, multiplies it by the contents of `%rax`, and then places the low 64 bits of the result in `%rax` and the high 64 bits in `%rdx`. (This is implicit: `%rdx` is not mentioned in the instruction.)

`IMUL`乘法指令有点不一样，
For example, suppose that you wish to translate c = b*(b+a);, where a, and b, and c are global integers. Here is one possible translation:

    MOVQ a, %rax
    MOVQ b, %rbx
    ADDQ %rbx, %rax
    IMULQ %rbx
    MOVQ %rax, c

The IDIV instruction does the same thing, except backwards: it starts
with a 128 bit integer value whose low 64 bits are in %rax and high 64 bits
in %rdx, and divides it by the value given in the instruction. The quotient
is placed in %rax and the remainder in %rdx. (If you want to implement
the modulus instruction instead of division, just use the value of %rdx.)

To set up a division, you must make sure that both registers have the
necessary sign-extended value. If the dividend fits in the lower 64 bits, but
is negative, then the upper 64 bits must all be ones to complete the twoscomplement
representation. The CQO instruction serves the very specific
purpose of sign-extending %rax into %rdx for division.

For example, to divide a by five:

    MOVQ a, %rax    # set the low 64 bits of the dividend
    CQO             # sign-extend %rax into %rdx
    IDIVQ $5        # divide %rdx:%rax by 5,
                    # leaving result in %rax

The instructions INC and DEC increment and decrement a register destructively. For example, the statement a = ++b could be translated as:

    MOVQ b, %rax
    INCQ %rax
    MOVQ %rax,b
    MOVQ %rax, a

The instructions AND, OR, and XOR perform destructive bitwise boolean
operations on two values. Bitwise means that the operation is applied to
each individual bit in the operands, and stored in the result.

So, `AND $0101B $0110B` would yield the result `$0100B`. In a similar way, the NOT instruction inverts each bit in the operand. For example, the bitwise C expression `c = (a & ˜b)`; could be translated like this:

    MOVQ a, %rax
    MOVQ b, %rbx
    NOTQ %rbx
    ANDQ %rax, %rbx
    MOVQ %rbx, c

Be careful here: these instructions do not implement logical boolean operations
according to the C representation that you are probably familiar
with. For example, if you define “false” to be the integer zero, and “true”
to be any non-zero value. In that case, `$0001` is true, but `NOT $0001B` is
`$1110B`, which is also true! To implement that correctly, you need to use
CMP with conditionals described below.2

Like the MOV instruction, the various arithmetic instructions can work
on a variety of addressing modes. However, for your compiler project, you
will likely find it most convenient to use MOV to load values in and out of
registers, and then use only registers to perform arithmetic.

<h3 id="3.4">3.4 比较和跳转</h3>

Using the JMP instruction, we may create a simple infinite loop that counts up from zero using the `%rax` register:


        MOVQ $0, %rax
loop:   INCQ %rax
        JMP loop

To define more useful structures such as terminating loops and if-then statements, we must have a mechanism for evaluating values and changing program flow. In most assembly languages, these are handled by two different kinds of instructions: compares and jumps.

All comparisons are done with the CMP instruction. CMP compares two different registers and then sets a few bits in the internal `EFLAGS` register, recording whether the values are the same, greater, or lesser. You don’t need to look at the EFLAGS register directly. Instead a selection of
conditional jumps examine the EFLAGS register and jump appropriately:

| Instruction | Meaning                 |
| ----------- | ----------------------- |
| JE          | Jump if Equal           |
| JNE         | Jump if Not Equal       |
| JL          | Jump if Less            |
| JLE         | Jump if Less or Equal   |
| JG          | Jump if Greater         |
| JGE         | Jump if Greater or Equal|

For example, here is a loop to count `%rax` from zero to five:


        MOVQ $0, %rax
loop:   INCQ %rax
        CMPQ $5, %rax
        JLE loop

And here is a conditional assignment: if global variable *x* is greater than zero, then global variable *y* gets ten, else twenty:


            MOVQ x, %rax
            CMPQ $0, %rax
            JLE .L1
    .L0:
            MOVQ $10, $rbx
            JMP .L2
    .L1:
            MOVQ $20, $rbx
    .L2:
            MOVQ %rbx, y

Note that jumps require the compiler to define target labels. These labels must be unique and private within one assembly file, but cannot be seen outside the file unless a `.global` directive is given. Labels like `.L0`, `.L1`, etc, can be generated by the compiler on demand.

<h3 id="3.5">3.5 栈</h3>

The stack is an auxiliary data structure used primarily to record the function call history of the program along with local variables that do not fit in registers. By convention, the stack grows *downward* from high values to low values. The `%rsp` register is known as the **stack pointer** and keeps track of the bottom-most item on the stack.

To push `%rax` onto the stack, we must subtract 8 (the size of `%rax` in bytes) from `%rsp` and then write to the location pointed to by `%rsp`:

    SUBQ $8, %rsp
    MOVQ %rax, (%rsp)

Popping a value from the stack involves the opposite:

    MOVQ (%rsp), %rax
    ADDQ $8, %rsp

To discard the most recent value from the stack, just move the stack pointer the appropriate number of bytes :

    ADDQ $8, %rsp

Of course, pushing to and popping from the stack referred to by `%rsp` is so common, that the two operations have their own instructions that behave exactly as above:

    PUSHQ %rax
    POPQ %rax

Note that, in 64-bit code, PUSH and POP are limited to working with 64-bit values, so a manual MOV and ADD must be used if it is necessary to move smaller items to/from the stack.

<h3 id="3.6">3.6 函数调用</h3>

Prior to the 64-bit architecture described here, a simple stack calling convention was used: arguments were pushed on the stack in reverse order, then the function was invoked with CALL. The called function looked for the arguments on the stack, did its work, and returned the result in `%eax`. The caller then removed the arguments from the stack.

However, 64-bit code uses a register calling convention, in order to exploit the larger number of available registers in the X86-64 architecture. [3] This convention is known as the **System V ABI** [2] and is written out in a lengthy technical document. The complete convention is quite complicated, but this summary handles the basic cases:

System V ABI调用习惯总结：

1. 前6个参数（包括指针和其它可以存储为整形的类型）以此保存在寄存器`%rdi`、`%rsi`、`%rdx`、`%rcx`、`%r8`和`%r9`。

2. 前8个浮点型参数以此存储在寄存器%xmm0-%xmm7。

3. 超过这些寄存器的参数被压入栈。

4. 如果函数接受可变数量的参数(如printf)，则必须将`%rax`寄存器设置为浮点参数的数量。

5. 函数的返回值存储在`%rax`。

In addition, we also need to know how the remaining registers are handled. A few are caller saved, meaning that the calling function must save those values before invoking another function. Others are callee saved, meaning that a function, when called, must save the values of those registers, and restore them on return. The argument and result registers need not be saved at all. Figure 10.4 shows these requirements.

To invoke a function, we must first compute the arguments and place them in the desired registers. Then, we must push the two caller-saved registers (%r10 and %r11) on the stack, to save their values. We then issue the CALL instruction, which pushes the current instruction pointer on to the stack then jumps to the code location of the function. Upon return from the function, we pop the two caller-saved registers off of the stack, and look for the return value of the function in the `%rax` register.

Here is an example. The following C program:
    
    int x=0;
    int y=10;
    int main()
    {
        x = printf("value: %d\n",y);
    }

could be translated to this:

    .data
    x:
            .quad 0
    y:
            .quad 10
    str:
            .string "value: %d\n"
    .text
    .global main
    main:
            MOVQ $str, %rdi         # first argument in %rdi: string
            MOVQ y, %rsi            # second argument in %rsi: y
            MOVQ $0, %rax           # there are zero float args
            PUSHQ %r10              # save the caller-saved regs
            PUSHQ %r11
            CALL printf             # invoke printf
            POPQ %r11               # restore the caller-saved regs
            POPQ %r10
            MOVQ %rax, x            # save the result in x
            RET                     # return from main function


System V ABI寄存器分配：

| 寄存器 | 目的    | 谁保存 |
| ------ | ------- | ------ |
| %rax   | 结果    | not saved    |
| %rbx   | 临时    | callee saves |
| %rcx   | 参数4   | not saved    |
| %rdx   | 参数3   | not saved    |
| %rsi   | 参数2   | not saved    |
| %rdi   | 参数1   | not saved    |
| %rbp   | 基址指针| callee saves |
| %rsp   | 栈指针  | callee saves |
| %r8    | 参数5   | not saved    |
| %r9    | 参数6   | not saved    |
| %r10   | 临时    | CALLER saves |
| %r11   | 临时    | CALLER saves |
| %r12   | 临时    | callee saves |
| %r13   | 临时    | callee saves |
| %r14   | 临时    | callee saves |
| %r15   | 临时    | callee saves |

<h3 id="3.7">3.7 定义叶子函数</h3>

Because function arguments are passed in registers, it is easy to write a leaf function that computes a value without calling any other functions. For example, code for the following function:

    square: function integer ( x: integer ) =
    {
        return x*x;
    }

Could be as simple as this:

    .global square
    square:
            MOVQ %rdi, %rax         # copy first argument to %rax
            IMULQ %rax              # multiply it by itself
                                    # result is already in %rax
            RET                     # return to caller

Unfortunately, this won’t work for a function that wants to invoke other functions, because we haven’t set up the stack properly. A more complex approach is needed for the general case.

<h3 id="3.8">3.8 定义复杂函数</h3>

A complex function must be able to invoke other functions and compute expressions of arbitrary complexity, and then return to the caller with the original state intact. Consider the following recipe for a function that accepts three arguments and uses two local variables:

    .global func
    func:
        pushq %rbp          # save the base pointer
        movq %rsp, %rbp     # set new base pointer
        pushq %rdi          # save first argument on the stack
        pushq %rsi          # save second argument on the stack
        pushq %rdx          # save third argument on the stack
        subq $16, %rsp      # allocate two more local variables
        pushq %rbx          # save callee-saved registers
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
        ### 函数体 ###
        popq %r15           # restore callee-saved registers
        popq %r14
        popq %r13
        popq %r12
        popq %rbx
        movq %rbp, %rsp     # reset stack pointer
        popq %rbp # recover previous base pointer
        ret # return to the caller

There is a lot to keep track of here: the arguments given to the function, the information necessary to return, and space for local computations. For this purpose, we use the base register pointer `%rbp`. Whereas the stack pointer `%rsp` points to the end of the stack where new data will be pushed, the base pointer `%rbp` points to the start of the values used by this function. The space between `%rbp` and `%rsp` is known as the stack frame for this function call.

There is one more complication: each function needs to use a selection of registers to perform computations. However, what happens when one function is called in the middle of another? We do not want any registers currently in use by the caller to be clobbered by the called function. To prevent this, each function must save and restore all of the registers that it uses by pushing them onto the stack at the beginning, and popping them off of the stack before returning. 根据System V ABI寄存器分配, each function must preserve the values of %rsp, %rbp, %rbx, and %r12-%r15 when it completes.

Here is the stack layout for `func`, defined above:

<img id="Figure_3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/assembly_language_3.PNG">

图3 X86-64栈布局示例

Note that the base pointer `(%rbp)` locates the start of the stack frame. So, within the body of the function, we may use base-relative addressing against the base pointer to refer to both arguments and locals. The arguments to the function follow the base pointer, so argument zero is at -8(%rbp), argument one at -16(%rbp), and so forth. Past those are local variables to the function at -32(%rbp) and then saved registers at -48(%rbp). The stack pointer points to the last item on the stack. If we use the stack for additional purposes, data will be pushed to further negative values. (Note that we have assumed all arguments and variables are 8 bytes large: different types would result in different offsets.)

Here is a complete example that puts it all together. Suppose that you have a B-minor function defined as follows:

    compute: function integer ( a: integer, b: integer, c: integer ) = 
    {
        x:integer = a+b+c;
        y:integer = x*5;
        return y;
    }

A complete translation of the function is on the next page. The code given is correct, but rather conservative. As it turned out, this particular function didn’t need to use registers %rbx and %r15, so it wasn’t necessary to save and restore them. In a similar way, we could have kept the arguments in registers without saving them to the stack. The result could have been computed directly into `%rax` rather than saving it to a local variable. These optimizations are easy to make when writing code by hand, but not so easy when writing a compiler.

For your first attempt at building a compiler, your code created will (probably) not be very efficient if each statement is translated independently. The preamble to a function must save all the registers, because it does not know a priori which registers will be used later. Likewise, a statement that computes a value must save it back to a local variable, because it does not know beforehand whether the local will be used as a return value. We will explore these issues later in Chapter 12 on optimization.

完整的X86示例

    .global compute
    compute:
        ##################### preamble of function sets up stack
        pushq %rbp              # save the base pointer
        movq %rsp, %rbp         # set new base pointer to rsp
        pushq %rdi              # save first argument (a) on the stack
        pushq %rsi              # save second argument (b) on the stack
        pushq %rdx              # save third argument (c) on the stack
        subq $16, %rsp          # allocate two more local variables
        pushq %rbx              # save callee-saved registers
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
        ######################## body of function starts here
        movq -8(%rbp), %rbx     # load each arg into a register
        movq -16(%rbp), %rcx
        movq -24(%rbp), %rdx
        addq %rdx, %rcx         # add the args together
        addq %rcx, %rbx
        movq %rbx, -32(%rbp)    # store the result into local 0 (x)
        movq -32(%rbp), %rbx    # load local 0 (x) into a register.
        movq $5, %rcx           # load 5 into a register
        movq %rbx, %rax         # move argument in rax
        imulq %rcx              # multiply them together
        movq %rax, -40(%rbp)    # store the result in local 1 (y)
        movq -40(%rbp), %rax    # move local 1 (y) into the result
        #################### epilogue of function restores the stack
        popq %r15               # restore callee-saved registers
        popq %r14
        popq %r13
        popq %r12
        popq %rbx
        movq %rbp, %rsp         # reset stack to base pointer.
        popq %rbp               # restore the old base pointer
        ret                     # return to caller


<h2 id="4">4 ARM汇编</h2>

最新的ARM架构是ARMv7-A（32位）和ARMv8-A（64位）。本文着重介绍32位架构，最后讨论一下64位体系架构的差异。

ARM是一个精简指令计算机（RISC）架构。相比X86，ARM使用更小的指令集，这些指令更易于流水线操作或并行执行，从而降低芯片复杂度和能耗。但由于一些例外，ARM有时候被认为是部分RISC架构。比如，一些ARM指令执行时间的差异使流水线不完美，为预处理而包含的桶形移位器引入了更复杂的指令，还有条件指令减少了一些潜在指令的执行，导致跳转指令的使用减少，从而降低了处理器的能耗。我们侧重于编写编译器常用到的指令，更复杂的内容和程序语言的优化留到以后再研究。

<h3 id="4.1">4.1 寄存器和数据类型</h3>

32位ARM架构拥有16个通用目的寄存器，r0~r15，使用约定如下所示：

| 名称 | 别名 | 目的 |
| ---- | ---- | ---- |
| r0   | -    | 通用目的寄存器         |
| r1   | -    | 通用目的寄存器         |
| ...  | -    | -                      |
| r10  | -    | 通用目的寄存器         |
| r11  | fp   | 栈帧指针，栈帧起始地址 |
| r12  | ip   | 内部调用临时寄存器     |
| r13  | sp   | 栈指针                 |
| r14  | lr   | 链接寄存器（返回地址） |
| r15  | pc   | 程序计数器             |

除了通用目的寄存器，还有2个寄存器：当前程序状态寄存器(CPSR)和程序状态保存寄存器(SPSR)，它们不能被直接访问。这两个寄存器保存着比较运算的结果，以及与进程状态相关的特权数据。用户态程序不能直接访问，但是可以通过一些操作的`副作用`修改它们。

ARM使用下面的后缀表示数据大小。它们与X86架构不同！如果没有后缀，汇编器假设操作数是unsigned word类型。有符号类型提供正确的符号位。任何word类型寄存器不会再有细分且被命名的寄存器。

| 后缀   | 数据类型        | 大小              |
| ------ | --------------- | ----------------- |
| B      | Byte            | 8 位     |
| H      | Halfword        | 16 位    |
| W      | WORD            | 32 位    |
| -      | Double Word     | 64 位    |
| SB     | Signed Byte     | 8 位     |
| SH     | Signed Halfword | 16 位    |
| SW     | Signed Word     | 32 位    |
| -      | Double Word     | 64 位    |

<h3 id="4.2">4.2 寻址模式</h3>

与X86不同，ARM使用两种不同的指令分别搬运寄存器之间、寄存器与内存之间的数据。MOV拷贝寄存器之间的数据和常量，而LDR和STR指令拷贝寄存器和内存之间的数据。

MOV指令可以把一个立即数或者寄存器值搬运到另一个寄存器中。ARM中，用`#`表示立即数，这些立即数必须小于等于16位。如果大于16位，就会使用LDR指令代替。大部分的ARM指令，目的寄存器在左，源寄存器在右。（STR是个例外）。具体格式如下：

| 模式    | 示例       |
| ------- | ---------- |
| 立即数  | MOV r0, #3 |
| 寄存器  | MOV r1, r0 |

MOV指令后面添加标识数据类型的字母，确定传输的类型和如何传输数据。如果没有指定，汇编器假定为word。

从内存中搬运数据使用LDR和STR指令，它们把源寄存器和目的寄存器作为第一个参数，要访问的内存地址作为第二个参数。简单情况下，使用寄存器给出地址并用中括号`[]`标记：

    LDR Rd, [Ra]
    STR Rs, [Ra]

`Rd`，表示目的寄存器；`Rs`，表示源寄存器；`Ra`，表示包含内存地址的寄存器。（必须要注意内存地址的类型，可以使用任何内存地址访问字节数据，使用偶数地址访问半字数据等。）

ARM寻址模式

| 模式    | 示例       |
| ------- | ---------- |
| 文本                    | LDR Rd, =0xABCD1234 |
| 绝对地址                | LDR Rd, =label      |
| 寄存器间接寻址          | LDR Rd, [Ra]        |
| 先索引-立即数           | LDR Rd, [Ra, #4]    |
| 先索引-寄存器           | LDR Rd, [Ra, Ro]    |
| 先索引-立即数&Writeback | LDR Rd, [Ra, #4]!   |
| 先索引-寄存器&Writeback | LDR Rd, [Ra, Ro]!   |
| 后索引-立即数           | LDR Rd, [Ra], #4    |
| 后索引-寄存器           | LDR Rd, [Ra], Ro    |

如上表所示，LDR和STR支持多种寻址模式。首先，LDR能够加载一个32位的文本值（或绝对地址）到寄存器。（完整的解释请参考下一段内容）。与X86不同，ARM没有可以从一个内存地址拷贝数据的单指令。为此，首先需要把地址加载到一个寄存器，然后执行一个寄存器间接寻址：

    LDR r1, =x
    LDR r2, [r1]

为了方便高级语言中的指针、数组、和结构体的实现，还有许多其它可用的寻址模式。比如，先索引模式可以添加一个常数（或寄存器）到基址寄存器，然后从计算出的地址加载数据：

    LDR r1, [r2, #4] ;  # 载入地址 = r2 + 4
    LDR r1, [r2, r3] ;  # 载入地址 = r2 + r3

有时候可能需要在把计算出的地址中的内容读取后，再把该地址写回到基址寄存器中，这可以通过在后面添加感叹号`!`实现。

    LDR r1, [r2, #4]! ; # 载入地址 = r2 + 4 然后 r2 += 4
    LDR r1, [r2, r3]! ; # 载入地址 = r2 + r3 然后 r2 += r3

后索引模式做相同的工作，但是顺序相反。首先根据基址地址执行加载，然后基址地址再加上后面的值：

    LDR r1, [r2], #4 ;  # 载入地址 = r2 然后 r2 += 4
    LDR r1, [r2], r3 ;  # 载入地址 = r2 然后 r2 += r3

通过先索引和后索引模式，可以使用单指令实现像我们经常写的C语句`b = a++`。STR使用方法类似。

在ARM中，绝对地址以及其它长文本更为复杂些。因为每条指令都是32位的，因此不可能将32位的地址和操作码（opcode）一起添加到指令中。因此，长文本存储在一个文本池中，它是程序代码段中一小段数据区域。使用与PC寄存器相关的加载指令，比如LDR，加载文本类型数据，这样的文本池可以引用靠近load指令的±4096个字节数据。这导致有一些小的文本池散落在程序中，由靠近它们的指令使用。

ARM汇编器隐藏了这些复杂的细节。在绝对地址和长文本的前面加上等号`=`，就代表向汇编器表明，标记的值应该存储在一个文本池中，并使用与PC寄存器相关的指令代替。

例如，下面的指令，把x的地址加载到r1中，然后取出x的值，存入r2寄存器中。

    LDR r1, =x
    LDR r2, [r1]

下面的代码展开后，将会从相邻的文本池中加载x的地址，然后加载x的值，存入r2寄存器中。也就是，下面的代码与上面的代码是一样的。

    LDR r1, .L1
    LDR r2, [r1]
    B   .end
    .L1:
        .word x
    .end:

<h3 id="4.3">4.3 基本算术运算</h3>

ARM的`ADD`和`SUB`指令，使用3个地址作为参数。目的寄存器是第一个参数，第二、三个参数作为操作数。其中第三个参数可以是一个8位的常数，或者带有移位的寄存器。使能进位的加、减法指令，将CPSR寄存器的C标志位写入到结果中。这4条指令如果分别后缀S，代表在完成时是否设置条件标志（包括进位），这是可选的。

| 指令     | 示例       |
| -------- | ---------- |
| 加       | ADD Rd, Rm, Rn |
| 带进位加 | ADC Rd, Rm, Rn |
| 减       | SUB Rd, Rm, Rn |
| 带进位减 | SBC Rd, Rm, Rn |

乘法指令的工作方式与加减指令类似，除了将2个32位的数字相乘能够产生一个64位的值之外。普通的MUL指令舍弃了结果的高位，而UMULL指令把结果分别保存在2个寄存器中。有符号的指令SMULL，在需要的时候会把符号位保存在高寄存器中。

| 指令         | 示例       |
| ------------ | ---------- |
| 乘法         | MUL Rd, Rm, Rn |
| 无符号长整形 | UMULL RdHi, RdLo, Rm, Rn |
| 有符号长整形 | SMULL RdHi, RdLo, Rm, Rn |

ARM没有除法指令，因为它不能再单个流水线周期中执行。因此，需要除法的时候，调用外部标准库中的函数。

逻辑指令在结构上和算术指令非常相似，如下图所示。特殊的是MVN指令，执行按位取反然后将结果保存到目的寄存器。

| 指令       | 示例       |
| ---------- | ---------- |
| 位与       | AND Rd, Rm, Rn |
| 位或       | ORR Rd, Rm, Rn |
| 位异或     | EOR Rd, Rm, Rn |
| 位置0      | BIC Rd, RM, Rn |
| 取反并移动 | MVN Rd, Rn |

<h3 id="4.4">4.4 比较和跳转</h3>

比较指令CMP比较2个值，将比较结果写入CPSR寄存器的N（负）和Z（零）标志位，供后面的指令读取使用。如果比较一个寄存器值和立即数，立即数必须作为第二个操作数：

    CMP Rd, Rn
    CMP Rd, #imm

另外，也可以在算术指令后面添加`S`标志，以相似的方式更新CPSR寄存器的相应标志位。比如，SUBS指令是两个数相减，保存结果，并更新CPSR。

ARM跳转指令

| 操作码 | 意义         | 操作码 | 意义       |
| ------ | ------------ | ------ | ---------- |
| B      | 无条件跳转   | BL     | 设置lr寄存器为下一条指令的地址并跳转 |
| BX     | 跳转并切换状态|BLX    | BL+BX指令的组合 |
| BEQ    | 相等跳转     | BVS    | 溢出标志设置跳转 |
| BNE    | 不等跳转     | BVC    | 溢出标志清除跳转 |
| BGT    | 大于跳转     | BHI    | 无符号>跳转 |
| BGE    | 大于等于跳转 | BHS    | 无符号>=跳转 |
| BLT    | 小于跳转     | BLO    | 无符号<跳转 |
| BLE    | 小于等于跳转 | BLS    | 无符号<=跳转 |
| BMI    | 负值跳转     | BPL    | >= 0跳转 |

各种跳转指令参考CPSR寄存器中之前的值，如果设置正确就跳到相应的地址（标签表示）执行。无条件跳转指令就是一个简单的`B`。

比如，从0累加到5：

            MOV r0, #0
    loop:   ADD r0, r0, 1
            CMP r0, #5
            BLT loop

再比如，如果x大于0，则给y赋值为:10；否则，赋值为20：

            LDR r0, =x
            LDR r0, [r0]
            CMP r0, #0
            BGT .L1
    .L0:
            MOV r0, #20
            B .L2
    .L1:
            MOV r0, #10
    .L2:
            LDR r1, =y
            STR r0, [r1]

BL指令用来实现函数调用。BL指令设置lr寄存器为下一条指令的地址，然后跳转到给定的标签（比如绝对地址）处执行，并将lr寄存器的值作为函数结束时的返回地址。BX指令跳转到寄存器中给定的地址处，最常用于通过跳转到lr寄存器而从函数调用中返回。

BLX指令执行的动作跟BL指令一样，只是操作对象换成了寄存器中给定的地址值，常用于调用函数指针，虚函数或其它间接跳转的场合。

ARM指令集的一个重要特性就是条件执行。每条指令中有4位表示16中可能的条件，如果条件不满足，指令被忽略。上面各种类型的跳转指令只是在最单纯的B指令上应用了各种条件而已。这些条件几乎可以应用到任何指令。

例如，假设下面的代码片段，哪个值小就会自加1：

    if(a<b) { a++; } else { b++; }

代替使用跳转指令和标签实现这个条件语句，我们可以前面的比较结果对每个加法指令设置条件。无论那个条件满足都被执行，而另一个被忽略。如下面所示（假设a和b分别存储在寄存器r0和r1中）：

    CMP r0, r1
    ADDLT r0, r0, #1
    ADDGE r1, r1, #1

<h3 id="4.5">4.5 栈</h3>

栈是一种辅助数据结构，主要用来存储函数调用历史以及局部变量。按照约定，栈的增长方向是从髙地址到地地址。`sp`寄存器保存栈指针，用来追踪栈顶内容。

为了把寄存器r0压入栈中，首先，`sp`减去寄存器的大小，然后把`r0`存入`sp`指定的位置：

    SUB sp, sp, #4
    STR r0, [sp]

或者，可以使用一条单指令完成这个操作，如下所示：

    STR r0, [sp, #-4]!

这儿，使用了先索引并`write-back`的寻址方式。也就是说，`sp`先减4，然后把`r0`的内容存入`sp-4`指向的地址处，然后再把`sp-4`写入到`sp`中。

ARM调用习惯总结

1. 前4个参数存储在r0、r1、r2 和r3中；
2. 其余的参数按照相反的顺序存入栈中；
3. 如果需要，调用者必须保存r0-r3和r12；
4. 调用者必须始终保存r14，即链接寄存器；
5. 如果需要，被调用者必须保存r4-r11；
6. 结果存到r0寄存器中。

PUSH伪指令可以压栈的动作，还可以把任意数量的寄存器压入栈中。使用花括号`{}`列出要压栈的寄存器列表：

    PUSH {r0,r1,r2}

出栈的动作正好与压栈的动作相反：

    LDR r0, [sp]
    ADD sp, sp, #4

使用后索引模式

    LDR r0, [sp], #4

使用`POP`指令弹出一组寄存器：

    POP {r0,r1,r2}

与X86不同的是，任何数据项(从字节到双word)都可以压入栈，只要遵守数据对齐即可。

<h3 id="4.6">4.6 函数调用</h3>

《[The ARM-Thumb Procedure Call Standard](http://infocenter.arm.com/help/topic/com.arm.doc.espc0002/ATPCS.pdf.)》描述了ARM的寄存器调用约定，其摘要如下：

> ARM寄存器分配：

| 寄存器 | 目的     | 谁保存 |
| ------ | -------  | ------ |
| r0   | 参数0      | not saved    |
| r1   | 参数1      | CALLER saves |
| r2   | 参数2      | CALLER saves |
| r3   | 参数3      | CALLER saves |
| r4   | 临时       | callee saves |
| ...  | ...        | ...          |
| r10  | 临时       | callee saves |
| r11  | 栈帧指针   | callee saves |
| r12  | 内部过程   | CALLER saves    |
| r13  | 栈指针     | callee saves    |
| r14  | 链接寄存器 | CALLER saves |
| r15  | 程序计数器 | 保存在r14    |

为了调用一个函数，把参数存入r0-r3寄存器中，保存lr寄存器中的当前值，然后使用`BL`指令跳转到指定的函数。返回时，恢复lr寄存器的先前值，并检查r0寄存器中的结果。

比如，下面的C语言代码段：

    int x=0;
    int y=10;
    int main() {
        x = printf("value: %d\n",y);
    }

其编译后的ARM汇编格式为:

    .data
        x: .word 0
        y: .word 10
        S0: .ascii "value: %d\012\000"
    .text
        main:
            LDR r0, =S0         @ 载入S0的地址
            LDR r1, =y          @ 载入y的地址
            LDR r1, [r1]        @ 载入y的值
            PUSH {ip,lr}        @ 保存ip和lr寄存器的值
            BL printf           @ 调用printf函数
            POP {ip,lr}         @ 恢复寄存器的值
            LDR r1, =x          @ 载入x的地址
            STR r0, [r1]        @ 把返回的结果存入x中
    .end

<h3 id="4.7">4.7 定义叶子函数</h3>

因为使用寄存器传递函数参数，所以编写一个不调用其它函数的叶子函数非常简单。比如下面的代码：

    square: function integer ( x: integer ) =
    {
        return x*x;
    }

它的汇编代码可以非常简单：

    .global square
    square:
            MUL r0, r0, r0  @ 参数本身相乘
            BX lr           @ 返回调用者

但是，很不幸，对于想要调用其他函数的函数，这样的实现就无法工作，因为我们没有正确建立函数使用的栈。所以，需要一种更为复杂的方法。

<h3 id="4.8">4.8 定义复杂函数</h3>

A complex function must be able to invoke other functions and compute expressions of arbitrary complexity, and then return to the caller with the original state intact. Consider the following recipe for a function that accepts three arguments and uses two local variables:

    func:
            PUSH {fp}           @ save the frame pointer
            MOV fp, sp          @ set the new frame pointer
            PUSH {r0,r1,r2}     @ save the arguments on the stack
            SUB sp, sp, #8      @ allocate two more local variables
            PUSH {r4-r10}       @ save callee-saved registers
            @@@ body of function goes here @@@
            POP {r4-r10}        @ restore callee saved registers
            MOV sp, fp          @ reset stack pointer
            POP {fp}            @ recover previous frame pointer
            BX lr               @ return to the caller

Through this method, we ensure that we are able to save all the values
in the registers into the stack and ensure that no data will be lost. The
stack, once this has been done, looks very similar to that of the X86 stack,
just with some extra callee-saved registers stored on the stack.

Here is a complete example that puts it all together. Suppose that you
have a B-minor function defined as follows:

    compute: function integer
            ( a: integer, b: integer, c: integer ) =
    {
        x: integer = a+b+c;
        y: integer = x*5;
        return y;
    }


<img id="Figure_4" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/assembly_language_4.PNG">

图4 ARM栈布局示例

A complete translation of the function is on the next page. Note that
this is one of many ways to construct a valid stack frame for a function
definition. Other approaches are valid, as long as the function uses the
stack frame consistently. For example, the callee could first push all of the
argument and scratch registers on the stack, and then allocate space for
local variables below that. (The reverse process must be used on function
exit, of course.)

Another common approach is for the callee PUSH {fp,ip,lr,pc}
on to the stack, before pushing arguments and local variables. While not
strictly required for implementing the function, it provides additional debugging
information in the form of a stack backtrace so that a debugger
can look backwards through the call stack and easily reconstruct the current
execution state of the program.

The code given is correct, but rather conservative. As it turned out,
this particular function didn’t need to use registers r4 and r5, so it wasn’t
necessary to save and restore them. In a similar way, we could have kept
the arguments in registers without saving them to the stack. The result
could have been computed directly into r0 rather than saving it to a local
variable. These optimizations are easy to make when writing code by
hand, but not so easy when writing a compiler.

For your first attempt at building a compiler, your code created will
(probably) not be very efficient if each statement is translated independently.
The preamble to a function must save all the registers, because it
does not know a priori which registers will be used later. Likewise, a statement
that computes a value must save it back to a local variable, because

ARM完整示例

    .global compute
    compute:
        @@@@@@@@@@@@@@@@@@ preamble of function sets up stack
        PUSH {fp}               @ save the frame pointer
        MOV fp, sp              @ set the new frame pointer
        PUSH {r0,r1,r2}         @ save the arguments on the stack
        SUB sp, sp, #8          @ allocate two more local variables
        PUSH {r4-r10}           @ save callee-saved registers
        @@@@@@@@@@@@@@@@@@@@@@@@ body of function starts here
        LDR r0, [fp,#-12]       @ load argument 0 (a) into r0
        LDR r1, [fp,#-8]        @ load argument 1 (b) into r1
        LDR r2, [fp,#-4]        @ load argument 2 (c) into r2
        ADD r1, r1, r2          @ add the args together
        ADD r0, r0, r1
        STR r0, [fp,#-20]       @ store the result into local 0 (x)
        LDR r0, [fp,#-20]       @ load local 0 (x) into a register.
        MOV r1, #5              @ move 5 into a register
        MUL r2, r0, r1          @ multiply both into r2
        STR r2, [fp,#-16]       @ store the result in local 1 (y)
        LDR r0, [fp,#-16]       @ move local 1 (y) into the result
        @@@@@@@@@@@@@@@@@@@ epilogue of function restores the stack
        POP {r4-r10}            @ restore callee saved registers
        MOV sp, fp              @ reset stack pointer
        POP {fp}                @ recover previous frame pointer
        BX lr                   @ return to the caller

it does not know beforehand whether the local will be used as a return
value. We will explore these issues later in Chapter 12 on optimization

<h3 id="4.9">4.9 ARM-64位</h3>

The 64-bit ARMv8-A architecture provides two execution modes: The A32
mode supports the 32-bit instruction set described above, and the A64
mode supports a new 64-bit execution model. This permits a 64-bit CPU
with a supporting operating system to execute a mix of 32-bit and 64-bit
programs simultaneously. Though not binary compatible with A32, the
A64 model follows much of the same architectural principles, with a few
key changes:

Word Size. A64 instructions are still a fixed size of 32 bits, however,
registers and address computations are 64 bits.

Registers. A64 has 32 64-bit registers, named x0-x31. x0 is a dedicated
zero register: when read, it always provides the value zero, when
written, there is no effect. x1-x15 are general purpose registers, x16 and
x17 are for interprocess communication, x29 is the frame pointer, x30 is
the link register and x31 is the stack pointer. (The program counter is not
directly accessible from user code.) Instead of using a data type suffix, a
32-bit value may be indicated by naming a register as w#.

Instructions. A64 instructions are largely the same as A32, using the
same mnemonics, with a few differences. Conditional predicates are no
longer part of every instruction. Instead, all conditional codes must perform
an explicit CMP and then a conditional branch. The LDM/STM instructions
and pseudo-instructions PUSH/POP are not available and must
be replaced with a sequence of explicit loads and stores. (This can be made
more efficient by using LDP/STP which load and store pairs of registers.

Calling Convention. To invoke a function, the first eight arguments
are placed in registers x0-x7, and the remainder are pushed on to the
stack. The caller must preserve registers x9-x15 and x30 while the callee
must preserve x19-x29. The (scalar) return value is placed in x0, while
extended return values are pointed to by x8.

<h2 id="5">5 参考</h2>

This chapter has given you a brief orientation to the core features of the X86 and ARM architectures, enough to write simple programs in the most direct way. However, you will certainly need to look up specific details of individual instructions in order to better understand their options and limitations. Now you are ready to read the detailed reference manuals and keep them handy while you construct your compiler:

1. Intel64 and IA-32 Architectures Software Developer Manuals. Intel Corp., 2017. http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html
2. System V Application Binary Interface, Jan Hubicka, Andreas Jaeger, Michael Matz, and Mark Mitchell (editors), 2013. https://software.intel.com/sites/default/files/article/402129/mpx-linux64-abi.pdf
3. ARM Architecture Reference Manual ARMv8. ARM Limited, 2017.
https://static.docs.arm.com/ddi0487/bb/DDI0487B_b_armv8_arm.pdf.
4. The ARM-THUMB Procedure Call Standard. ARM Limited, 2000.
http://infocenter.arm.com/help/topic/com.arm.doc.espc0002/ATPCS.pdf.

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
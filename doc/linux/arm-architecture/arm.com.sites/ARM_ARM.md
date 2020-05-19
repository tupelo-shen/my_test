# 目录

* [0 前言](#1)
    - [0.1 关于本手册](#1.1)
    - [0.2 架构版本以及变体](#1.2)
    - [0.3 约定](#1.3)
* [4 ARM指令集](#4)
    - [4.1 指令集编码](#4.1)
* [5 ARM寻址模式](#5)
    - [5.1 寻址模式1-数据处理操作数](#5.1)
    - [5.2 寻址模式2-load/store](#5.2)
---

* [B1 内存和系统架构简介](#B1)
    - [B1.1 关于内存](#B1.1)
    - [B1.2 内存架构体系](#B1.2)
    - [B1.3 L1 Cache](#B1.3)
    - [B1.4 L2 Cache](#B1.4)
    - [B1.5 Write buffer](#B1.5)
    - [B1.6 紧耦合内存-TCM](#B1.6)
    - [B1.7 内存架构体系](#B1.7)
* [B2 内存序模型](#B2)
    - [B2.1 关于内存序模型](#B2.1)
    - [B2.4 ARMv6内存属性](#B2.4)
* [B3 系统控制协处理器](#B3)
* [B4 虚拟内存系统架构](#B4)
* [B5 受保护内存系统架构](#B5)
* [B6 Cache和Write buffer](#B6)
* [B7 紧耦合内存-TCM](#B7)

---

<h1 id="0">0 前言</h1>

This preface describes the versions of the ARM® architecture and the contents of this manual, then lists the conventions and terminology it uses.

<h2 id="0.1">0.1 关于本手册</h2>

The purpose of this manual is to describe the ARM instruction set architecture, including its high code density Thumb® subset, and three of its standard coprocessor extensions:

* The standard System Control coprocessor (**coprocessor 15**), which is used to control memory system components such as caches, write buffers, Memory Management Units, and Protection Units.

* The Vector Floating-point (**VFP**) architecture, which uses coprocessors 10 and 11 to supply a high-performance floating-point instruction set.

* The debug architecture interface (**coprocessor 14**), formally added to the architecture in ARM v6 to provide software access to debug features in ARM cores, (for example, breakpoint and watchpoint control).

The 32-bit ARM and 16-bit Thumb instruction sets are described separately in Part A. The precise effects of each instruction are described, including any restrictions on its use. This information is of primary importance to authors of compilers, assemblers, and other programs that generate ARM machine code.

Assembler syntax is given for most of the instructions described in this manual, allowing instructions to be specified in textual form.

However, this manual is not intended as tutorial material for ARM assembler language, nor does it describe ARM assembler language at anything other than a very basic level. To make effective use of ARM assembler language, consult the documentation supplied with the assembler being used.

The memory and system architecture definition is significantly improved in ARM architecture version 6 (the latest version). Prior to this, it usually needs to be supplemented by detailed implementation-specific information from the technical reference manual of the device being used.

<h2 id="0.2">0.2 架构版本以及变体</h2>

The ARM instruction set architecture has evolved significantly since it was first developed, and will continue to be developed in the future. Six major versions of the instruction set have been defined to date, denoted by the version numbers 1 to 6. Of these, the first three versions including the original 26-bit architecture (the 32-bit architecture was introduced at ARMv3) are now OBSOLETE. All bits and encodings that were used for 26-bit features become RESERVED for future expansion by ARM Ltd. Versions can be qualified with variant letters to specify collections of additional instructions that are included as an architecture extension. Extensions are typically included in the base architecture of the next version number, ARMv5T being the notable exception. Provision is also made to exclude variants by prefixing the variant letter with x, for example the xP variant described below in the summary of version 5
features.

The valid architecture variants are as follows (variant in brackets for legacy reasons only):

    ARMv4, ARMv4T, ARMv5T, (ARMv5TExP), ARMv5TE, ARMv5TEJ, and ARMv6

The following architecture variants are now OBSOLETE:

    ARMv1, ARMv2, ARMv2a, ARMv3, ARMv3G, ARMv3M, ARMv4xM, ARMv4TxM, ARMv5, ARMv5xM, and ARMv5TxM

Details on OBSOLETE versions are available on request from ARM.

The ARM and Thumb instruction sets are summarized by architecture variant in ARM instructions and architecture versions on page A4-286 and Thumb instructions and architecture versions on page A7-125 respectively. The key differences introduced since ARMv4 are listed below.

<h2 id="0.3">0.3 约定</h2>

<h3 id="0.3.1">0.3.1 指令的伪码描述</h3>

A form of pseudo-code is used to provide precise descriptions of what instructions do. This pseudo-code is written in a *typewriter* font, and uses the following conventions for clarity and brevity:

* Indentation is used to indicate structure. For example, the range of statements that a for statement loops over, goes from the for statement to the next statement at the same or lower indentation level as the for statement (both ends exclusive).

* 注释使用C语言风格，/* 和 */。

* English text is occasionally used outside comments to describe functionality that is hard to describe otherwise.

* All keywords and special functions used in the pseudo-code are described in the Glossary.

* Assignment and equality tests are distinguished by using = for an assignment and == for an equality test, as in the C language.

* Instruction fields are referred to by the names shown in the encoding diagram for the instruction. When an instruction field denotes a register, a reference to it means the value in that register, rather than the register number, unless the context demands otherwise. For example, a Rn == 0 test is checking whether the value in the specified register is 0, but a Rd is R15 test is checking whether the specified register is register 15.

* When an instruction uses an addressing mode, the pseudo-code for that addressing mode generates one or more values that are used in the pseudo-code for the instruction. For example, the AND instruction described in AND on page A4-8 uses ARM addressing mode 1 (see Addressing Mode 1 - Data-processing operands on page A5-2). The pseudo-code for the addressing mode generates two values shifter_operand and shifter_carry_out, which are used by the pseudo-code for the AND instruction.

<h3 id="1.3.2">1.3.2 汇编语法描述</h3>

This manual contains numerous syntax descriptions for assembler instructions and for components of assembler instructions. These are shown in a **typewriter** font, and are as follows:

* <>

    Any item bracketed by < and > is a short description of a type of value to be supplied by the user in that position. A longer description of the item is normally supplied by subsequent text. Such items often correspond to a similarly named field in an encoding diagram for an instruction. When the correspondence simply requires the binary encoding of an integer value or register number to be substituted into the instruction encoding, it is not described explicitly. For example, if the assembler syntax for an ARM instruction contains an item <Rn> and the instruction encoding diagram contains a 4-bit field named Rn, the number of the register specified in the assembler syntax is encoded in binary in the instruction field.

    If the correspondence between the assembler syntax item and the instruction encoding is more complex than simple binary encoding of an integer or register number, the item description indicates how it is encoded.

* {}

    Any item bracketed by { and } is optional. A description of the item and of how its presence or absence is encoded in the instruction is normally supplied by subsequent text.

* |

    This indicates an alternative character string. For example, LDM|STM is either LDM or STM. spaces Single spaces are used for clarity, to separate items. When a space is obligatory in the assembler syntax, two or more consecutive spaces are used.

* +/-

    This indicates an optional + or - sign. If neither is coded, + is assumed.

* *

    When used in a combination like \<immed_8\> * 4, this describes an immediate value which must be a specified multiple of a value taken from a numeric range. In this instance, the numeric range is 0 to 255 (the set of values that can be represented as an 8-bit immediate) and the specified multiple is 4, so the value described must be a multiple of 4 in the range 4*0 = 0 to 4*255 = 1020.

All other characters must be encoded precisely as they appear in the assembler syntax. Apart from { and }, the special characters described above do not appear in the basic forms of assembler instructions documented in this manual. The { and } characters need to be encoded in a few places as part of a variable item. When this happens, the long description of the variable item indicates how they must be used.

<h1 id="2">2 ARM体系结构简介</h1>

<h1 id="3">3 编程者模型</h1>

<h1 id="4">4 ARM指令集</h1>

<h2 id="4.1">4.1 指令集编码</h2>

Figure A3-1 shows the ARM instruction set encoding.

All other bit patterns are UNPREDICTABLE or UNDEFINED. See Extending the instruction set on page A3-32 for a description of the cases where instructions are UNDEFINED.

An entry in square brackets, for example [1], indicates that more information is given after the figure.

<h3 id="4.1.6">4.1.6 BIC指令</h3>

![图4_1_6_1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_4_1_6_1.JPG)

* BIC（位清除）

    将一个值与另一个按位AND操作。第一个值来自寄存器，第二个值可以是立即数，也可以是寄存器值，还可以在操作之前进行移位操作。
    BIC指令可以根据结果，有选择地更新<cond>条件标志。

* 语法

        BIC{<cond>}{S} <Rd>, <Rn>, <shifter_operand>

    在这里，

    * cond

        是执行指令的条件。关于各种条件的定义请参考[4.2 条件域](#4.2)，如果 <cond> 被省略，则总是执行该指令。

    * S

        Causes the S bit, bit[20], in the instruction to be set to 1 and specifies that the instruction updates the CPSR. If S is omitted, the S bit is set to 0 and the CPSR is not changed by the instruction. Two types of CPSR update can occur when S is specified:

        * If <Rd> is not R15, the N and Z flags are set according to the result of the operation, and the C flag is set to the carry output bit generated by the shifter (see Addressing Mode 1 - Data-processing operands on page A5-2). The V flag and the rest of the CPSR are unaffected.

        * If <Rd> is R15, the SPSR of the current mode is copied to the CPSR. This form of the instruction is UNPREDICTABLE if executed in User mode or System mode, because these modes do not have an SPSR.

    * Rd

        指定目的寄存器。

    * Rn

        指定包含第一个操作数的寄存器。

    * shifter_operand

        Specifies the second operand. The options for this operand are described in Addressing Mode 1 - Data-processing operands on page A5-2, including how each option causes the I bit (bit[25]) and the shifter_operand bits (bits[11:0]) to be set in the instruction.
        指定第二个操作数。

        If the I bit is 0 and both bit[7] and bit[4] of shifter_operand are 1, the instruction is not BIC. Instead, see Extending the instruction set on page A3-32 to determine which instruction it is.


<h3 id="4.1.38">4.1.38 MRS指令</h3>

![图4_1_38_1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_4_1_38_1.JPG)

* MRS

    将当前模式下的CPSR或SPSR的值移动到通用目的寄存器中。在通用目的寄存器中，就可以使用常用的数据处理指令对这些值进行检查或操控。

* 语法

        MRS{<cond>} <Rd>, CPSR
        MRS{<cond>} <Rd>, SPSR

    在这里，

    * <cond>

        是执行指令的条件。关于各种条件的定义请参考[4.2 条件域](#4.2)，如果 <cond> 被省略，则总是执行该指令。

    * <Rd>

        指定目的寄存器。

* 架构版本

    所有。

* 异常

    没有。

* 伪代码

        if ConditionPassed(cond) then
            if R == 1 then
                Rd = SPSR
            else
                Rd = CPSR

* 使用

    MRS通常有三种用途：

    * 作为用于更新PSR的Read/Modify/Write/操作的一部分。
    * When an exception occurs and there is a possibility of a nested exception of the same type occurring, the SPSR of the exception mode is in danger of being corrupted. To deal with this, the SPSR value must be saved before the nested exception can occur, and later restored in preparation for the exception return. The saving is normally done by using an MRS instruction followed by a store instruction. Restoring the SPSR uses the reverse sequence of a load instruction followed by an MSR instruction.
    * In process swap code, the programmers’ model state of the process being swapped out must be saved, including relevant PSR contents, and similar state of the process being swapped in must be restored. Again, this involves the use of MRS/store and load/MSR instruction sequences.

* 注意

    * User mode SPSR

        在用户模式或系统模式下访问SPSR是不可预料的。

<h3 id="4.1.99">4.1.99 STR指令</h3>

![图4_1_99_1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_4_1_99_1.JPG)

* STR

    存储寄存器内容（WORD）到内存中。

* 语法

        STR{<cond>} <Rd>, <addressing_mode>

    在这儿，

    * `<cond>`

        是执行指令的条件。关于各种条件的定义请参考[4.2 条件域](#4.2)，如果 <cond> 被省略，则总是执行该指令。

    * `<Rd>`

        指定操作的源寄存器。如果指定R15、则存储的值是实现定义的。

    * `<addressing_mode>`

        具体的描述可以参考[]

* 架构版本

    所有。

* 异常

    数据会被抛弃。

* 伪代码

        MemoryAccess(B-bit, E-bit)
        processor_id = ExecutingProcessor()
        if ConditionPassed(cond) then
            Memory[address,4] = Rd
            if Shared(address) then                 /* from ARMv6 */
                physical_address = TLB(address)
                ClearExclusiveByAddress(physical_address,processor_id,4)

* 使用

    结合合适的寻址模式，STR将32位的数据从通用寄存器写入到内存中。使用PC作为基址寄存器允许基于PC的相对寻址，这是实现位置无关代码的关键。（PIC，position-independent code）。

<h2 id="4.2">4.2 条件域</h2>

Most ARM instructions can be conditionally executed, which means that they only have their normal effect on the programmers’ model state, memory and coprocessors if the N, Z, C and V flags in the CPSR satisfy a condition specified in the instruction. If the flags do not satisfy this condition, the instruction acts as a NOP: that is, execution advances to the next instruction as normal, including any relevant checks for interrupts and Prefetch Aborts, but has no other effect.

Prior to ARMv5, all ARM instructions could be conditionally executed. A few instructions have been introduced subsequently which can only be executed unconditionally. See Unconditional instruction extension space on page A3-41 for details.

Every instruction contains a 4-bit condition code field in bits 31 to 28:

![图4-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_TMP_4_2.JPG)

This field contains one of the 16 values described in Table A3-1 on page A3-4. Most instruction mnemonics can be extended with the letters defined in the mnemonic extension field.

If the always (AL) condition is specified, the instruction is executed irrespective of the value of the condition code flags. The absence of a condition code on an instruction mnemonic implies the AL condition code.

<h3 id="4.2.1">4.2.1 条件码0b1111</h3>

If the condition field is 0b1111, the behavior depends on the architecture version:

* In ARMv4, any instruction with a condition field of 0b1111 is UNPREDICTABLE.
* In ARMv5 and above, a condition field of 0b1111 is used to encode various additional instructions which can only be executed unconditionally (see Unconditional instruction extension space on page A3-41). All instruction encoding diagrams which show bits[31:28] as cond only match instructions in which these bits are not equal to 0b1111.

<h2 id="4.4">4.4 数据处理指令</h2>

ARM has 16 data-processing instructions, shown in Table A3-2.

Table A3-2 Data-processing instructions

| Opcode | Mnemonic | Operation | Action |
| - | - | - | - |
| 0000 | AND | Logical AND | Rd := Rn AND shifter_operand |

<h3 id="4.4.1">4.4.1 指令编码</h3>

<h1 id="5">5 ARM寻址模式</h1>

本章主要讨论ARM指令的5种寻址模式。

<h2 id="5.1">5.1 寻址模式1-数据处理操作数</h2>

数据处理指令中计算`<shifter_operand>`的方式有11种。其语法格式如下：

    <opcode>{<cond>}{S} <Rd>, <Rn>, <shifter_operand>

在这儿，`<shifter_operand>`可以是下面的任意一种：

1. `#立即数`
2. `<Rm>`
3. `<Rm>, LSL #移位立即数`
4. `<Rm>, LSL <Rs>`
5. `<Rm>, LSR #移位立即数`
6. `<Rm>, LSR <Rs>`
7. `<Rm>, ASR #移位立即数`
8. `<Rm>, ASR <Rs>`
9. `<Rm>, ROR #移位立即数`
10. `<Rm>, ROR <Rs>`
11. `<Rm>, RRX`

<h3 id="5.1.1">5.1.1 指令编码</h3>

下面的图片展示了数据处理指令下寻址模式的编码：

32位立即数
![图5_1_1_1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_5_1_1_1.JPG)

立即数移位
![图5_1_1_2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_5_1_1_2.JPG)

寄存器移位
![图5_1_1_3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_5_1_1_3.JPG)

* opcode

    指定操作

* S位

    表示指令需要更新条件标志

* Rd

    指定目标计数器

* Rn

    指定第一个源操作数寄存器

* 位[11:0]

    位[11:0]合称移位操作数。具体的内容可以参考[移位操作数](#5.1.2)

* 位[25]

    作为I位被引用，用来区分立即数移位操作数还是寄存器移位操作数。

如果以下3位具有下面的值，说明指令不是数据处理指令，但是位于算术或load/store指令扩展空间中。

    bit[25] == 0
    bit[4] == 1
    bit[7] == 1

<h3 id="5.1.2">5.1.2 移位操作数</h3>

除了产生一个移位操作数，移位器还会产生一个进位，有一些指令将其写入C标志（进位标志）。默认的寄存器操作数（比如没有指定移位的Rm）使用寄存器移位操作，将立即数设为0，也就是不移位。

移位操作数具有下面三种形式：

* **立即数**

    立即数是对一个8位的常数进行偶数移位（移位位数为0,2,4,8...26,28,30），存于32位的Word中。也就是说一个立即数如果小于0xFF（255）那么直接用前7~0位表示即可，此时不用移位，11~8位的`rotate_imm`设为0即可；如果前8位`immed_8` 的数值大于 255，那么就看这个数是否能有 `immed_8` 中的某个数移位 `2 * Rotate_imm` 位形成的。如果能，那么就是合法立即数；否则非法。

    一些合法的值：

        0xFF,0x104,0xFF0,0xFF00,0xFF000,0xFF000000,0xF000000F

    一些不合法的值：

        0x101,0x102,0xFF1,0xFF04,0xFF003,0xFFFFFFFF,0xF000001F

    示例:

        MOV R0, #0 ;            把立即数0移动到R0
        ADD R3, R3, #1 ;        将寄存器3的值+1
        CMP R7, #1000 ;         R7的值与立即数1000相比较
        BIC R9, R8, #0xFF00 ;   清除R8寄存器的位8-15，并将结果存储到R9

* **寄存器操作数**

    寄存器操作数就是一个寄存器的值。寄存器的值直接作为数据处理指令的操作数。例如：

        MOV R2, R0 ;           将R0的值移动到R2
        ADD R4, R3, R2 ;       将R2的值加到R3上，结果存储于R4
        CMP R7, R8 ;           比较R7和R8的值

* **移位寄存器操作数**

    移位寄存器操作数有5种类型，如下所示：

    | 指令名称 | 功能描述 |
    | --- | -------- |
    | ASR | 算术右移 |
    | LSL | 逻辑左移 |
    | LSR | 逻辑右移 |
    | ROR | 循环右移 |
    | RRX | 循环右移扩展 |

    要移位的位数，既可以使用立即数，也可以使用寄存器值表示。参考下面的例子：

        MOV R2, R0, LSL #2 ;            逻辑左移2位，结果写入R2,相当于R2=R0x4
        ADD R9, R5, R5, LSL #3 ;        R9 = R5 + R5 x 8 或 R9 = R5 x 9
        RSB R9, R5, R5, LSL #3 ;        R9 = R5 x 8 - R5 或 R9 = R5 x 7
        SUB R10, R9, R8, LSR #4 ;       R10 = R9 - R8 / 16
        MOV R12, R4, ROR R3 ;           R12 = R4 循环右移后的值，位数由寄存器R3中的控制

<h2 id="5.2">5.2 寻址模式2-load/store</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="7">7 TCM</h1>

紧密耦合内存（Tightly Coupled Memory）。



<h1 id="B1">B1 内存和系统架构简介</h1>

<h2 id="B1.1">B1.1 关于内存</h2>

多年来，ARM架构已经在演进。在此期间，有超过10亿ARM芯片发售，其中大多数是基于ARMv4或者ARMv5。内存系统的要求也发生了相当大的变化，从简单的直接内存映射，到系统使用多种手段去优化它们的内存资源的使用，比如：

1. 多种类型的内存
2. cache
3. write buffer
4. 虚拟内存和其它的内存映射技术

说起内存系统，首先，应该提及它的2个属性，可缓存的（cacheable）和可缓冲的（bufferable）。从字面上，就可以理解它的底层硬件原理。在ARMv6架构版本之前，内存访问顺序模型（在后面的文章中统称为内存序模型）没有定义。对此，为了更高性能及其相关实现的需求，ARMv6架构新引入了虚拟内存系统和弱序内存模型，其中弱序模型包含一个附加的内存屏障（memory barrier）命令。

内存可以按照下面3种基本类型进行划分：

* 强序（strongly ordered），比如，Cache、Write buffer。

* 设备序（device），比如，非Cache、Write Buffer。

* 正常序（normal），比如，Cache、Write buffer。

对于这些基本类型的内存，可以通过可缓存性、共享属性及访问机制进行进一步划分。虚拟内存实现机理向前兼容的，关于这一点，在后面的兼容模型中会描述。一些早期的功能会被抛弃，因此，在新的设计中不推荐使用了。


协处理器15，后面统一使用CP15代替，仍然是虚拟内存系统控制，识别配置和其它内存配置控制的控制方式。CP15是ARMv6的一个规定。

内存系统和内存序模型，分别在以下几个章节进行描述：

1. 介绍

    本章

2. 内存架构体系

    对于Cache理论和紧耦合内存的概念作一概述。

3. 内存序模型

    ARMv6架构的内存属性和内存序规则。

4. 系统控制协处理器

    对协处理器的功能作一概述。

5. 虚拟内存系统架构(VMSA)

    A sophisticated system to control virtual-to-physical address mapping, access permissions to memory, and other memory attributes, based on the use of a Memory Management Unit (MMU). The revised ARMv6 model, and the model used by earlier architecture variants, are described.

6. 受保护的内存系统架构(PMSA)

    An alternative, simpler protection mechanism suitable for many applications that do not require the full facilities provided by the MMU memory system. The revised ARMv6 and earlier architecture variant models are described.

7. Cache和Write Buffer

    Mechanisms provided to control cache and write buffer functionality in a memory hierarchy.

8. L1 紧耦合内存

    ARMv6 provision including the associated DMA and Smartcache models.

9. 快速上下文切换扩展

    Describes the Fast Context Switch Extension. This facilitated fast switching between up to 128 processes executing in separate process blocks, each of size up to 32 MB. This is supported in ARMv6 only for backwards compatibility, and its use is deprecated.

<h2 id="B1.2">B1.2 内存架构体系</h2>

好的设计肯定是多方面的平衡，以达到系统的最佳性能和损耗。对于内存也是如此：

* 内存类型，比如ROM、Flash、DRAM、SRAM、基于硬盘的存储介质
* 大小 - 容量和芯片的体积大小
* 访问速度 - 内核读写某个区域的时钟周期
* 架构 - Harvard(独立的指令和数据存储)或冯·诺依曼(统一的内存)

通常，内存访问时间越短，对于可以使用的内存种类越受限制，因为，它需要与处理器内核紧密耦合。甚至片上系统内存，由于它的类型和大小，功率限制，及物理布局的走线长度等的影响，可能有不同的时序要求。Cache提供了一种手段，使得应用程序中的各个线程可以共享这个快速而又昂贵的系统内存资源。

Where a system is designed with different types of memory in a layered model, this is referred to as a memory hierarchy. Systems can employ caches at multiple levels. The outer layers trade increased latency for increasing size. All the caches in the system must adhere to a memory coherency policy, which is part of the system architecture. Such layered systems usually number the layers - level 1, level 2 ... level n- with the increasing numbers representing increased access times for layers further from the core.

IO can also be provided at the different layers, that is, some no-wait-state register-based peripherals at level 1, out to memory mapped peripherals on remote system buses.

图B1-1展示了一个内存架构体系的例子

![ARM_ARM_B_1_1.PNG](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_B_1_1.PNG)

The ARMv6 specifies the Level 1 (L1) subsystem, providing cache, Tightly-Coupled Memory (TCM), and an associated TCM-L1 DMA system. The architecture permits a range of implementations, with software visible configuration registers to allow identification of the resources that exist. Options are provided to support the L1 subsystem with a Memory Management Unit (VMSAv6) or a simpler Memory Protection Unit (PMSAv6).

Some provision is also made for multiprocessor implementations and Level 2 (L2) caches. However, these are not fully specified in this document. To ensure future compatibility, it is recommended that Implementors of L2 caches and closely-coupled multiprocessing systems work closely with ARM.

VMSAv6 describes Inner and Outer attributes which are defined for each page-by-page. These attributes are used to control the caching policy at different cache levels for different regions of memory. Implementations can use the Inner and Outer attributes to describe caching policy at other levels in an IMPLEMENTATION DEFINED manner. See sections Memory region attributes on page B4-11 for the architecture details. All levels of cache need appropriate cache management and must support:

* cache cleaning (write-back caches only)
* cache invalidation (all caches).

ARM processors and software are designed to be connected to a byte-addressed memory. Prior to ARMv6, addressing was defined as word invariant. Word and halfword accesses to the memory ignored the byte alignment of the address, and accessed the naturally-aligned value that was addressed, that is, a memory access ignored address bits 0 and 1 for word access, and ignored bit 0 for halfword accesses. The endianness of the ARM processor normally matched that of the memory system, or was configured to match it before any non-word accesses occurred.

ARM处理器和软件被设计能够按字节访问内存。ARMv6之前，寻址方式被定义为按word访问内存。

ARMv6引入了：

* a byte-invariant address model
* 支持未对齐 word 和 half-word 访问
* additional control features for loading and storing data in a little or big endian manner.

See Endian support on page A2-30 for details.

<h2 id="B1.3">B1.3 L1 Cache</h2>

Before ARMv6, ARM caches were normally implemented as virtually addressed caches, with virtual indexing and virtual address tags. With this model, physical pages were only mapped into a single virtual page, otherwise the result was UNPREDICTABLE. These implementations did not provide coherence between multiple virtual copies of a single physical page.

ARMv6 specifies a cache architecture where the expected behavior is that normally associated with physically tagged caches. The ARMv6 L1 cache architecture is designed to reduce the requirement for cache clean and/or invalidation on a context switch, and to support multiple virtual address aliases to a particular memory location. Flexibility on the size, associativity or organization of the caches within this subsystem is provided in the Coprocessor System Control Register (CP15). The cache organization may be a Harvard architecture with separate instruction and data caches, or a von Neumann architecture with a single, unified cache.

cache的组织架构可以是具有独立指令和数据cache的哈佛结构，也可以是使用一个缓存的冯诺依曼结构。

In a Harvard architecture, an implementation does not need to include hardware support for coherency between the Instruction and Data caches. Where such support would be required, for example, in the case of self-modifying code, the software must make use of the cache cleaning instructions to avoid such problems.

An ARMv6 L1 cache must appear to software to behave as follows:
对于软件来说，ARMv6 L1 cache必须具备以下功能：

* the entries in the cache do not need to be cleaned and/or invalidated by software for different virtual to physical mappings
* 对于不同的虚拟地址到物理地址的映射，缓存中的条目不需要由软件进行清理和/或失效。

* aliases to the same physical address may exist in memory regions that are described in the page tables as being cacheable, subject to the restrictions for 4KB small pages outlined in Restrictions on Page Table Mappings on page B6-11.

Caches can be implemented with virtual or physical addressing (including indexing) provided these behavior requirements are met. ARMv6 L1 cache management uses virtual addresses, which is consistent with earlier architecture guidelines and implementations.

For architecture details on the L1 cache see [Chapter B6 Caches and Write Buffers](#B6).


<h2 id="B1.4">B1.4 L2 Cache</h2>

L1 缓存总是和内核紧密相关，但是L2缓存既可以与内核紧密相关也可以实现为系统总线上的外设映射的内存:


A recommended minimum set of L2 cache commands is defined for configuration and control. Closely-coupled L2 caches must be managed through the System Control Coprocessor. It is IMPLEMENTATION DEFINED whether they use virtual or physical addresses for control functions. Memory mapped L2 caches must use physical address based control.

Further levels of cache are possible, but their control is not mandated within ARMv6 except that they must comply with:

* the inner and outer attribute model described in Memory region attributes on page B4-11.
* coherency needs associated with managing multi-level caches through the System Control Coprocessor interface, see Considerations for additional levels of cache on page B6-12.

For architecture details on the L2 cache see section L2 cache.

<h2 id="B1.5">B1.5 Write buffer</h2>

The term write buffer can cover a number of different behaviors. The effects of these behaviors on different uses of memory mapped space needs to be understood by the programmer to avoid unexpected results. For this reason, the term bufferable is no longer used as an attribute to describe the required behavior of a memory system.

A write buffer exists to decouple a write transaction from the execution of subsequent memory transactions.
In addition, particular buffer implementations may perform additional tasks such as the re-ordering of
memory transfers, the merging of multiple writes into proximate locations, or the forwarding of write data
to subsequent reads. These buffering behaviors are becoming more cache-like in nature. The memory
attributes Strongly Ordered, Device, and Normal described in Strongly Ordered memory attribute on
page B2-12 are designed to allow the programmer to describe the required behavior, leaving the
Implementor free to choose whatever structures are optimal for a given system, provided that the behavior
for each memory attribute is correctly fulfilled.

For writes to buffered areas of memory, precise aborts can only be signaled to the processor as a result of
conditions that are detectable at the time the data is placed in the write buffer. Conditions that can only be
detected when the data is later written to main memory, such as an ECC error from main memory, must be
handled by other methods, by raising an interrupt or an imprecise abort.

B1.6 Tightly Coupled Memory

<h2 id="B1.6">B1.6 紧耦合内存-TCM</h2>

The Tightly Coupled Memory (TCM) is an area of memory that can be implemented alongside the L1 cache,as part of the level 1 memory subsystem. The TCM is physically addressed, with each bank occupying a unique part of the physical memory map. See SmartCache Behavior on page B7-6 for an optional, smartcache, ARMv6 usage model. In keeping with the L1 cache, the TCM may be structured as a Harvard architecture with separate instruction and data TCM, or as a Von Neumann architecture with a unified TCM.

The TCM is designed to provide low latency memory that can be used by the processor without the unpredictability that is a feature of caches. Such memory can be used to hold critical routines, such as interrupt handling routines or real-time tasks, where the indeterminacy of a cache would be highly undesirable. Other example uses are:

* scratchpad data
* data types whose locality properties are not well suited to caching
* critical data structures such as Interrupt stacks.

关于TCM的更多详细内容，请参考[第B7章 紧耦合内存-TCM](#B7)。

<h3 id="B1.6.1">B1.6.1 紧耦合内存和Cache的对比</h3>

The TCM is designed to be used as part of the physical memory map of the system, and is not expected to be backed by a level of external memory with the same physical addresses. For this reason, the TCM behaves differently from the caches for regions of memory which are marked as being Write-Through cacheable. In such regions, no external writes occur in the event of a write to memory locations contained in the TCM.

TCM被设计称为系统物理内存映射的一部分，且它并不期望通过具有相同物理地址的外部内存来支持。因为这个原因，TCM的行为与被标记为Write-through的内存区域的Cache不同。在这些区域，当写入到TCM中包含的内存位置时，不会发生外部写操作。

It is an architectural requirement that memory locations are contained either in the TCM or the cache, not in both. In particular, no coherency mechanisms are supported between the TCM and the cache. This means that it is important when allocating the base address of the TCM to ensure that the TCM address range does not overlap with any valid cache entries.

根据体系结构的要求，内存区域要么包含在TCM中，要么包含在缓存中，但不能同时包含在两者中。特别是，在TCM和Cache之间没有一致性支持。这意味着在分配TCM的基地址时，确保TCM地址范围不与任何有效的Cache所包含的区域重叠，这非常重要。

<h3 id="B1.6.2">B1.6.2 紧耦合内存的DMA支持</h3>

ARMv6 includes a DMA model with register support for its configuration. This is the only mechanism other than the associated processor core that can read and write the TCM. Up to two DMA channels are provided for. This allows chained operations, see Level 1 (L1) DMA model on page B7-8 for architectural details.

<h2 id="B1.7">B1.7 Asynchronous exceptions</h2>

Many exceptions are synchronous events related to instruction execution in the core. However, the following exceptions cause asynchronous events to occur:

1. Reset on page A2-18
2. Interrupts
3. Imprecise aborts on page B1-11.

<h2 id="B1.8">B1.8 Semaphores</h2>

The Swap (SWP) and Swap Byte (SWPB) instructions need to be used with care to ensure that expected behavior is observed. Two examples are as follows:

* Systems with multiple bus masters that use the Swap instructions to implement semaphores to control interaction between different bus masters.

    In this case, the semaphores must be placed in an uncached region of memory, where any buffering of writes occurs at a point common to all bus masters using the mechanism. The Swap instruction then causes a locked read-write bus transaction.

    This type of semaphore can be externally aborted.

* Systems with multiple threads running on a uniprocessor that use the Swap instructions to implement semaphores to control interaction of the threads.

    In this case, the semaphores can be placed in a cached region of memory, and a locked read-write bus transaction might or might not occur. The Swap and Swap Byte instructions are likely to have better performance on such a system than they do on a system with multiple bus masters (as described above).

    This type of semaphore has UNPREDICTABLE behavior if it is externally aborted.

From ARMv6, load and store exclusive instructions (LDREX and STREX) are the preferred method of implementing semaphores for system performance reasons. The new mechanism is referred to as synchronization primitives, and requires data monitor logic within the memory system that monitors access to the requested location from all sources in the shared memory model case. The instructions provide a degree of decoupling between the load and store elements, with the store only being successful if no other resource has written to the location since its associated load. See Synchronization primitives on page A2-44 for more details.

<h1 id="B2">B2 内存序模型</h1>

<h2 id="B2.1">B2.1 关于内存序模型</h2>

The architecture prior to ARMv6 did not attempt to define the acceptable memory ordering of explicit memory transactions, describing the regions of memory according to the hardware approaches that had previously been used to implement such memory systems. Thus regions of memory had been termed as being one of Write-Through Cacheable, Write-Back Cacheable, Non-Cacheable Bufferable or Non-Cacheable, Non-Bufferable. These terms are based on the previous hardware implementations of cores and the exact properties of the memory transactions could not be rigorously inferred from the memory names. Implementations have chosen to interpret these names in different ways, leading to potentially incompatible uses.

ARMv6之前的架构没有定义显式内存事务可接受的内存序，而是根据之前被用来实现此类内存系统的硬件方法描述内存区域。因而，内存区域可以分为Write-Through可缓存的，Write-Back可缓存的，非缓存非缓冲的，非缓存的，或非缓冲的。这些术语是基于以前内核硬件实现的，

In a similar manner, the order in which memory accesses could be presented to memory was not defined,and in particular there was no definition of what order could be relied upon by an observer of the memory transactions generated by a processor. As implementations and systems become more complicated, these undefined areas of the architecture move from being simply based on a standard default to having the potential of presenting significant incompatibilities between different implementations; at processor core and system level.

ARMv6 introduces a set of memory types - Normal, Device, and Strongly Ordered - with memory access properties defined to fit in a largely backwards compatible manner to the defacto meanings of the original memory regions. A potential incompatibility has been introduced with the need for a software polling policy when it is necessary for the program to be aware that memory accesses to I/O space have completed, and all side effects are visible across the whole system. This reflects the increasing difficulty of ensuring linkage between the completion of memory accesses and the execution of instructions within a complex high-performance system.

A shared memory attribute to indicate whether a region of memory is shared between multiple processors (and therefore requires an appearance of cache transparency in an ordering model) is also introduced. Implementations remain free to choose the mechanisms to implement this functionality.

The key issues with the memory order model are slightly different depending on the target audience:

* for software programmers, the key factor is that side effects are only architecturally visible after software polling of a location that indicates that it is safe to proceed。

* for silicon Implementors, the Strongly Ordered and Device memory attributes defined in this chapter place certain restrictions on the system designer in terms of what they are allowed to build, and when to indicate completion of a transaction.

Additional attributes and behaviors relate to the memory system architecture. These features are defined in other areas of this manual:

* Virtual memory systems based on an MMU described in Chapter [B4 虚拟内存系统架构](#B4).
* Protected memory systems based on an MPU described in Chapter B5 Protected Memory System Architecture.
* Caches and write buffers described in Chapter B6 Caches and Write Buffers.
* Tightly Coupled Memory (TCM) described in Chapter B7 Tightly Coupled Memory

Some attributes are described in relation to an MMU for ARMv6. In general, these can also be applied to an MPU based system.

<h2 id="B2.4">B2.4 ARMv6内存属性</h2>

ARMv6架构定义了一组属性，这些属性反映了系统内存映射关系中所有的内存和设备特征。也将内存区域的访问顺序定义为了内存属性。

描述内存区域，有3个互斥的主内存类型：

* 正常内存
* 设备内存
* 强序内存

正常内存具有下面的特性：

* 可以重复写，没有任何副作用
* 重复的读操作，返回最后一个写入的读取位置的值
* 如果读写被中断，可以重新启动
* 多字节访问不需要是原子的，且能够重新操作
* 支持未对齐访问
* 在访问目标系统内存之前，读写操作可以被合并
* 读操作可以预取附加的内存位置，并且没有副作用

通常情况下，系统外设（I/O）遵守不同的访问规则；在ARMv6可以被定义为强序或设备内存。I/O访问的例子有：

* 可以连续读写队列值的FIFO
* 中断控制器的寄存器，访问可以用作改变控制器本身状态的中断确认
* 内存控制器配置寄存器，用于设置正常内存区域的时序（正确性）
* 内存映射的外围设备，其中访问这样的内存区域会在系统内造成副作用。

为了保证系统的正确性，访问规则比正常内存的要更为严格：

*  读写会有副作用
*  读写操作不能被重复
*  必须维护读写事务的编号，大小和顺序

另外，共享属性主要是针对多核系统的定义，如果被指定为共享，那就是多核共享这块内存，如果指定为非共享，就是某个内核独享。比如，具有DMA功能的智能外设。

表B2-2 对内存属性做了一个总结

| 内存基本类型|共享属性|其它属性|描述|
|----------|----------|----------|----------|
| 强序        | -      | -        | 所有对强序内存的访问都是按程序顺序执行的。所有的强序访问都被假定为共享的。|
| 设备序      | 共享    | -        | 被设计用于多个内核共享访问映射为外设的内存 |
| 设备序      | 非共享  | -        | 被设计用来访问单个内核访问的映射为外设的内存 |
| 正常序      | 共享    | 非缓存的/Write-Through缓存/Write-Back缓存 | 被设计用于处理几个内核共享的正常序内存 |
| 正常序      | 非共享   | 非缓存的/Write-Through缓存/Write-Back缓存 | 被设计用于处理单个内核使用的正常序内存 |

<h3 id="B2.4.1">B2.4.1 正常内存属性</h3>

该属性在MMU中的每一页都有定义，可以进一步定义为共享或非共享，用来描述系统中使用的大部分内存。该属性就是用来描述正常内存的，这样的内存存储没有任何副作用。正常内存可以读写或者是只读的。

对于可写的正常内存，除非改变物理地址的映射，否则：

* 从某个特定的位置加载数据，将会为相同的处理器加载那个位置上最近存储的数据。
* 如果从某个特定的位置加载2次，中间没有store的操作，将会返回相同的数据。

对于只读的正常内存：

* 如果从某个特定的位置加载2次，将会为每一次加载返回相同的数据。

对于正常内存的访问符合内存序模型的`弱序模型`。对于`弱序模型`可以在描述内存序问题的标准文本中找到。可以参考[第2章 共享内存-多核的内存一致性模型](#A2)。


<h4 id="B2.4.1.1">B2.4.1.1 非共享正常内存</h4>

<h4 id="B2.4.1.2">B2.4.1.2 共享正常内存</h4>

<h4 id="B2.4.1.3">B2.4.1.3 可缓存的Write-Through，可缓存的Write-Back和非可缓存的内存</h4>

<h3 id="B2.4.2">B2.4.2 设备内存属性</h3>

设备内存属性就是为，访问那个内存位置会产生副作用，或者为load操作返回的数据依赖加载的编号的内存位置而定义的属性。内存映射外设和I/O内存位置就是典型的设备内存的示例。MMU中的每一个页也都定义了设备属性。

从处理器到设备内存的访问按照指令定义的大小和顺序进行。对于这些内存位置的访问数量是由程序指定的。当程序中只指定了一次访问的时候，硬件的实现不能重复访问这些区域，也就是说，访问是不可重启的。比如说，一种实现就是，为了允许中断抛弃很缓慢的访问，在中断的前后可以重复访问。像这种实现优化就不能对设备内存使用。

> 通俗地讲，就是对于设备内存区域的访问，就是由软件支配的。硬件本身不会做操作。

另外，设备内存位置也是不可缓存的。尽管可以缓存对设备内存的写入操作，但是，只有在保证访问的次数，顺序和大小正确的时候，才能合并。多次访问同一地址，不能改变访问的次数，在这种情况下不允许合并访问。

访问这种设备内存，以便控制正常内存的时候，要求使用内存屏障保证正确的执行。一个实例就是，对内存控制器的配置寄存器进行编程，控制相关的内存。

<h4 id="B2.4.2.1">B2.4.2.1 共享属性</h4>

为MMU中的每一个页面定义了共享属性。这些区域被称为共享设备或非共享设备。

标记为非共享设备的内存区域只能被单个内核访问。一个系统支持共享和非共享设备的示例是，为其私有外设支持一个私有总线，而系统外设位于主系统总线（共享）上。这样的系统可能对私有外设具有更可控的访问时间预测（比如，watchdog定时器或中断控制器）。

<h3 id="B2.4.3">B2.4.3 强序内存属性</h3>

MMU中的每一页都定义了强序属性。对强序内存的访问就像是，在访问的前后插入了一个DMB内存屏障一样。具体的参考[B2.6.1 数据内存屏障（DMB）-CP15寄存器7](B2.6.1)

对强序内存的访问就是按照程序的指定行为进行访问。硬件的实现不能重复访问这些位置。

标记为强序的内存，不能被缓存，总是被视为共享内存。

<h3 id="B2.4.4">B2.4.4 内存访问的限制</h3>

（略）。其实就是对上面通用说明没有覆盖的地方进行补充说明。

<h3 id="B2.4.5">B2.4.5 向后兼容</h3>

（略）。

<h2 id="B2.6">B2.6 内存屏障</h2>

内存屏障，是处理器应用到指令或指令序列上，用来同步CPU核的load/store指令退出的事件的。内存屏障用来消除load/store指令的竞争性或者冲洗掉前面的指令。ARMv6提供了3种内存屏障指令，位于系统控制协处理器中，用于实现本章描述的内存序模型，并要求这些指令在特权模式和用户模式必须可用。

* DMB

    数据内存屏障（DataMemoryBarrier）

* DSB

    数据同步屏障（DataSynchronizationBarrier，也称为DataWriteBarrier）

* PrefetchFlush

    预取指令冲洗

这些指令可以单独使用，也可以结合Cache和内存管理操作一起使用，还可以只在特权模式下使用。在早期版本的中，对内存屏障的支持是在实现时由芯片厂商自定义的。

显式内存屏障影响CPU发起的load/store指令对内存的读写操作。由L1 DMA读写，预取指令或硬件页表访问造成的读写操作不属于显式访问。

<h3 id="B2.6.1">B2.6.1 数据内存屏障（DMB）-CP15寄存器7</h3>

DMB的行为如下：

* 前面的数据对后面的数据可见
* DMB对CPU上执行的其它指令的顺序没有影响

因此，DMB确保指令前后的显式操作顺序，而无法保证这些操作的完成是否有竞争性。

<h3 id="B2.6.2">B2.6.2 数据同步屏障（DSB）-CP15寄存器7</h3>

DSB就是一种特殊的内存屏障，它的行为如下：

* 该指令之前的所有显式内存操作必须完成
* 该指令之前的所有Cache，分支预测器和TLB表上的操作必须完成

另外，在DSB指令后的所有指令必须在DSB指令完成后才能执行。


<h3 id="B2.6.3">B2.6.3 预取指令冲洗-CP15寄存器7</h3>

PrefetchFlush指令会冲洗处理器中的流水线，以便在该指令执行后，从Cache或者内存中获取流水线刷新后的所有指令。它保证了上下文切换时的操作的效果，比如，改变ASID（应用程序空间标识符）、完成TLB维护操作或分支预测器维护操作，以及在PrefetchFlush之前执行的所有对改变CP15寄存器的更改，对在该指令执行后获取的指令是可见的。

另外，预取刷新操作确保在预取刷新操作之后按程序顺序出现的任何分支都被写入分支预测逻辑，并且上下文在预取刷新操作之后是可见的。这是确保正确执行指令流所必须的。

<h1 id="B3">B3 系统控制协处理器</h1>

关于协处理器CP15，全名是系统控制协处理器。包含以下内容：

* 关于CP15
* 寄存器
* 寄存器0-ID code
* 寄存器1-控制寄存器
* 寄存器2-15

<h2 id="B3.1">B3.1 关于协处理器的介绍</h2>

All of the standard memory and system facilities are controlled by coprocessor 15 (CP15), which is known as the System Control coprocessor. Some facilities also use other methods of control, and these are described in the chapters relating to those facilities. For example, the Memory Management Unit described in Chapter B4 Virtual Memory System Architecture is also controlled by page tables in memory.

ARMv6 systems shall include a System Control Coprocessor, with support for automatic interrogation of cache, tightly coupled memory, and coprocessor provision. It also provides the control mechanism for memory management (MMU and MPU support as applicable).

Prior to ARMv6, CP15 instructions are UNDEFINED when CP15 is not implemented. However, CP15 has become a de facto standard for processor ID, cache control, and memory management (MMU and MPU support) in implementations since ARMv4. This manual should be read in conjunction with the relevant implementation reference manual to determine the exact details of CP15 support in a particular part.

This chapter describes the overall design of the System Control coprocessor and how its registers are accessed. Detailed information is given about some of its registers. Other registers are allocated to facilities described in detail in other chapters and are only summarized in this chapter.

<h2 id="B3.2">B3.2 协处理器中的寄存器</h2>

The System Control coprocessor can contain up to 16 primary registers, each of which is 32 bits long. Additional fields in the register access instructions are used to further refine the access, increasing the number of physical 32-bit registers in CP15. The 4-bit primary register number is used to identify registers in descriptions of the System Control coprocessor, because it is the primary factor determining the function of the register.

CP15 registers can be read-only, write-only or read/write. The detailed descriptions of the registers specify:

* the types of access that are allowed
* the functionality invoked by each type of access
* whether a primary register identifies more than one physical register, and if so, how they are distinguished
* any other details that are relevant to the use of the register.

<h3 id="B3.2.1">B3.2.1 操作协处理器的指令</h3>

协处理器相关的指令是：

* MCR

    写ARM通用寄存器值到协处理器中。


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="B4">B4 虚拟内存系统架构</h1>

本章描述基于MMU的虚拟内存系统架构，包括以下内容：

* 关于VMSA
* 内存访问序列
* 内存访问控制
* 内存区域属性
* Aborts
* 错误地址和错误状态寄存器
* 硬件页表转换
* 页表和微页表的支持
* CP15寄存器

<h2 id="B4.1">B4.1 关于VMSA</h2>

复杂的操作系统，通常使用虚拟内存系统为不同的进程，提供独立的、受保护的地址空间。进程是在内存管理单元（MMU）的控制下，动态分配的内存和其它内存映射的系统资源。MMU通过自身持有的TLB（转换旁路缓冲，Translation Lookaside Buffers，在这儿虽然命名为一个buffer的样子，但其实它是一个Cache）中的虚拟地址到物理地址的映射和相关的内存属性对系统内存实现细粒度的控制。TLB的内容是通过对在内存中维护的一组地址转换表进行转换查找来管理的。

执行一次完整转换表查找的过程被称为`转换表遍历`，是由硬件自动完成的，在执行时间上有很大的开销，至少是一个主内存访问，或者两个的周期。通过缓存遍历转换表的结果，TLB减少了内存访问的平均时间成本。在硬件实现上，对于冯诺依曼结构，一个统一的TLB；对于哈佛结构，独立的指令和数据TLB。

在ARMv6上，VMSA得到了显著的加强，称为VMSAv6版本。为了防止上下文切换时的TLB失效，将每一个虚拟地址到物理地址的映射与某一个进程空间、或全局空间关联起来。任意时刻，只有全局和当前进程的地址空间的映射被使能。通过改变ASID（应用空间ID），使能的虚拟到物理的地址映射集合可以被改变。VMSAv6添加了许多不同的内存类型的定义，请参考[ARMv6内存属性](#B2.4)和其它属性。


与每个TLB条目相关联的内存属性包括:

* 内存访问权限控制

    它控制程序对内存区域的读写访问权限，非、只读、读/写。当不允许访问时，将向处理器发出内存中止的信号。允许的访问级别可能受到程序是在用户模式还是特权模式下运行以及使用域的影响。

* 内存区域的属性

    描述了内存的属性，比如设备内存(VMSAv6)，不可缓存的，直写（write-through）和后写（write-back）

* 虚拟到物理地址的映射

    ARM处理器产生的地址称为虚拟地址。 MMU允许可以被映射到不同的物理地址。物理地址标识正在访问的内存地址。这可以导致多种方式管理物理内存的分配。这可以用于以多种方式管理物理内存的分配。例如，它可以用于将内存分配给具有潜在冲突地址映射的不同进程，或者允许具有稀疏地址映射的应用程序使用物理内存的连续区域。

> 由于快速上下文切换扩展(FCSE，参见[B8章](#B8))，本章中对虚拟地址的所有引用都指修改过的虚拟地址，除非另有明确说明。当FCSE被禁止的时候，虚拟地址和修改后的虚拟地址是相等的。FCSE存在仅仅是为了向后兼容，在新系统的设计中被废弃。

<h3 id="B4.1.1">B4.1.1 VMSAv6引入的主要变化</h3>

The following list summarizes the changes introduced in VMSAv6:

* Entries can be associated with an application space identifier, or marked as a global mapping. This eliminates the requirement for TLB flushes on most context switches.
* Access permissions extended to allow both privileged read only, and privileged/user read-only modes to be simultaneously supported. The use of the System (S) and ROM (R) bits to control access permission determination are only supported for backwards compatibility.
* Memory region attributes to mark pages shared by multiple processors.
* The use of Tiny pages, and the fine page table second level format is now obsolete.

<h2 id="B4.2">B4.2 内存访问序列</h2>

When the ARM CPU generates a memory access, the MMU performs a lookup for a mapping for the requested modified virtual address in a TLB. From VMSAv6 this also includes the current ASID. Implementations can use either Harvard or unified TLBs. If the implementation has separate instruction and data TLBs, it uses:

* the instruction TLB for an instruction fetch
* the data TLB for all other accesses.

If no global mapping, or mapping for the currently selected ASID (VMSAv6), for the modified virtual address can be found in the appropriate TLB then a translation table walk is automatically performed by hardware.



<h3 id="B4.2.1">B4.2.1 TLB命中过程</h3>

每一个TLB项包含一个修改后的虚拟地址，页大小，和物理地址，以及一组内存属性。通过ASID，它被指向某一进程空间，或全局空间。通过CP15的寄存器13选择当前选择的进程空间。

如果MVA的第31~N位和TLB项匹配，就说明要么是全局的，要么ASID与当前的ASID匹配。其中，N是TLB项的页面大小的log2.

在任何时候，如果超过2项命中，则TLB的行为不可预知（UNPREDICTABLE）。操作系统必须保证至多1个TLB项命中，通常是通过更改全局页面映射时通过刷新其TLB来保证。

TLB可以存储的项数取决于内存的块大小：

* 超级扇区，16M的内存块大小
* 扇区，1M的内存块大小
* 大页面，64K内存块大小
* 小页，4K的内存块大小

> 微页的使用(1KB)在VMSAv6架构中不支持

<h2 id="B4.7">B4.7 硬件页表转换</h2>

MMU支持以页或扇区访问内存：

* 超级扇区，16M的内存块大小
* 扇区，1M的内存块大小
* 大页，64K内存块大小
* 小页，4K的内存块大小
* 微页，1K的内存块大小（VMSAv6不支持）

内存中的转换表有2级：

* 一级转换表

    持有扇区和超级扇区转换，并且指向二级转换表

* 二级转换表

    持有大、小页的转换。

The MMU translates modified virtual addresses generated by the CPU into physical addresses to access external memory, and also derives and checks the access permission. Translations occur as the result of a TLB miss, and start with a first-level fetch. A section-mapped access only requires a first-level fetch, whereas a page-mapped access also requires a second-level fetch.



协处理器CP15的EE位控制查找的页表的大小端格式。

> As the fine page table format and support for tiny pages is now OBSOLETE, definition of these features has been moved into a separate section, Fine page tables and support of tiny pages on page B4-35.

<h3 id="B4.7.1">B4.7.1 转译表基地址</h3>

当TLB没有命中时，就会发生转译过程。转译表基地址寄存器（TTBR，CP15的寄存器2），持有一级转译表的物理基地址。

VMSAv6之前，存在一个简单的TTBR。只有TTBR的位\[31:14\]有意义，其它位都为0。因此，一级页表以16K为边界。

VMSAv6引入了一个附加的TTBR，称为TTBR1。现在就有3个相关寄存器，TTBR0，TTBR1和TTBCR。如果发生TLB未命中，MVA的高bit位决定了是否使用一级转换表基地址还是二级转换表基地址。具体参考[VMSAv6中的页表转换](#4.7.3)

TTBR1预计将用于操作系统和I/O地址，不会基于上下文发生改变。TTBR0预计将用于与进程有关的地址空间。当TTBCR被设为0，所有使用TTBR0的方法都将与之前的版本兼容。TTBR1表的大小是16K，但是TTBR0表的大小范围从128字节~16K，这依赖于TTBCR的N值，N可以取值0~7。所有的转译表必须自然对齐。

VMSAv6 还引入了一个控制位，在TTBR的最低有效位，具体查看[VMSAv6中的页表转换](#4.7.3)。

<h3 id="B4.7.2">B4.7.2 一级提取</h3>

将转换表基地址寄存器（Translation Table Base register）的位\[31:14\]，和MVA的位[31:20]以及2个值为0的位，组成一个32位的物理地址，如图B4-3所示。通过这个物理地址就可以选出一个4字节的转换表里的一项，也就是一个扇区的一级描述符或二级页表的指针。

![ARM_ARM_B_4_3.PNG](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_B_4_3.PNG)

<h3 id="B4.7.3">B4.7.3 VMSAv6的页表转换</h3>

VMSAv6 supports two page table formats:

* A backwards-compatible format supporting sub-page access permissions. These have been extended so certain page table entries support extended region types.
* A new format, not supporting sub-page access permissions, but with support for the VMSAv6 features. These features are:
    - extended region types
    - global and process specific pages
    - more access permissions
    - marking of shared and nonshared regions
    - marking of execute-never regions.

Subpages are described in Second-level descriptor - Coarse page table format on page B4-31.

<h3 id="B4.7.4">B4.7.4 一级描述符</h3>

一级表中的每一项都是一个描述符，描述了与其关联的1M大小的修改后虚拟地址（MVA）是怎样的被映射的。一级页表的每一项中的位\[1:0\]决定了一级描述符的类型：

* 如果位\[1:0\] 等于0b00，相关的虚拟地址没有映射的物理地址，试图发起这样的访问，会产生[转译错误](#B4.5)。位\[31:2\]由软件使用，表示自己的目的，而硬件在这时候会自动忽略这些位。恰当的时候，位\[31:2\]会继续取得对描述符的访问权限。

* 如果位\[1:0\] = 0b10，表示该项是一个与其相关的MVA的扇区描述符。

* 如果位\[1:0\] = 0b01，给出一个粗二级表的物理地址，指定与其相关的1M大小的MVA地址范围如何被映射。粗二级表每个表的大小为1K，可以映射大页和小页。

* 如果位\[1:0\] = 0b11，给出一个细二级表的物理地址（VMSAv6之前），VMSAv6保留。

表B4-1 一级描述符格式(VMSAv6, 子页使能)
![ARM_ARM_BT_4_1.PNG](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_BT_4_1.PNG)

表B4-2 一级描述符格式 (VMSAv6,子页禁用))
![ARM_ARM_BT_4_2.PNG](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_BT_4_2.PNG)

<h3 id="B4.7.5">B4.7.5 扇区和超级扇区</h3>

如果bit\[1:0\]等于0b10，一级描述符就是一个1M或16M的扇区描述。超级扇区是可选的。

![ARM_ARM_B_4_4.PNG](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_B_4_4.PNG)


<h3 id="B4.7.6">B4.7.6 粗页表描述符</h3>

如果一级描述符是一个粗页表描述符，各个域具有下面的意义：

| 位域 | 功能描述 |
| ---- | ------- |
| Bits[1:0]   | 标识描述符的类型（0b01表示粗页表描述符） |
| Bits[4:2]   | 未定义，SBZ |
| Bits[8:5]   | domain域，指明16种可能的domain域 |
| Bit[9]      | 未定义 |
| Bits[31:10] | 是一个页表基地址，指向粗二级页表 |

![ARM_ARM_B_4_5.PNG](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_B_4_5.PNG)

<h2 id="B4.9">B4.9 CP15</h2>

<h1 id="B5">B5 受保护的内存系统架构（PMSA）</h1>

用一张图进行阐述

![ARM_ARM_B_5_1.PNG](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARM_ARM_B_5_1.PNG)


<h1 id="B6">B6 Cache和Write buffer</h1>

本章介绍Cache和Write buffer，它们对于基于MMU的内存系统和基于MPU的内存系统都是常用的概念。本章包含以下内容：

* 关于Cache和Write buffer
* Cache结构
* Cache的类型
* L1 Cache
* 其它级别的Cache的考虑
* CP15


<h1 id="B7">B7 紧耦合内存-TCM</h1>

<h2 id="B7.6">B7.6 SmartCache Behavior</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
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
* [B2 介绍内存和系统架构](#B2)
* [B3 介绍内存和系统架构](#B3)
* [B4 介绍内存和系统架构](#B4)
* [B5 介绍内存和系统架构](#B5)
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

说起内存系统，应该首先描述它的2个属性，cacheable和bufferable。它们的名称就是底层硬件的原理的反应，没有任何其它意思。In addition, the order model of the memory accesses made was not defined. An implicit model evolved from early implementations, which were much simpler systems than those being developed today.

To meet the demands of higher performance systems and their associated implementations, ARMv6 introduces new disciplines for virtual memory systems and a weakly-ordered memory model including an additional memory barrier command.

Memory behavior is now classified by type:

* strongly ordered
* device
* normal.

These basic types can be further qualified by cacheable and shared attributes as well as access mechanisms. As in the second edition of the *ARM Architecture Reference Manual*, general requirements are described in keeping with the diversity of needs, however, emphasis is given to the ARMv6 virtual memory model and its absolute requirements. The virtual memory support mechanisms associated with earlier variants are described in the backwards compatibility model. Some earlier features are deprecated, and therefore not recommended for use in new designs.

Coprocessor 15 (CP15) remains the primary control mechanism for virtual memory systems, as well as identification, configuration and control of other memory configurations and system features. CP15 provision is a requirement of ARMv6.

内存系统和内存序模型，分别在以下几个章节进行描述：

1. 介绍

    本章

2. 内存架构体系

    An overview including basic cache theory and the concept of tightly coupled memory.

3. 内存序模型

    Memory attributes and order rules introduced with ARMv6.

4. The System Control coprocessor

    An overview of the features and support provided.

5. Virtual Memory System Architecture (VMSA)

    A sophisticated system to control virtual-to-physical address mapping, access permissions to memory, and other memory attributes, based on the use of a Memory Management Unit (MMU). The revised ARMv6 model, and the model used by earlier architecture variants, are described.

6. Protected Memory System Architecture (PMSA)

    An alternative, simpler protection mechanism suitable for many applications that do not require the full facilities provided by the MMU memory system. The revised ARMv6 and earlier architecture variant models are described.

7. Caches and Write buffers

    Mechanisms provided to control cache and write buffer functionality in a memory hierarchy.

8. L1 Tightly Coupled Memory Support

    ARMv6 provision including the associated DMA and Smartcache models.

9. Fast Context Switch Extension

    Describes the Fast Context Switch Extension. This facilitated fast switching between up to 128 processes executing in separate process blocks, each of size up to 32 MB. This is supported in ARMv6 only for backwards compatibility, and its use is deprecated.

<h2 id="B1.2">B1.2 内存架构体系</h2>

Good system design is a balance of many trade-offs to achieve the overall system performance and cost goals. An important part of this decision process is the memory provision:

* types of memory, for example ROM, Flash, DRAM, SRAM, disk based storage

* size - capacity and silicon area

* access speed - core clock cycles required to read or write a location

* architecture - Harvard (separate instruction and data memories) or Von Neumann (unified memory).

As a general rule, the faster the memory access time, the more constrained the amount of resource available, because it needs to be closely coupled to the processor core, that is, on the same die. Even on-chip memory may have different timing requirements because of its type or size, power constraints, and the associated critical path lengths to access it in the physical layout. Caches provide a means to share the fastest, most expensive system memory resources between the currently active process threads in an application.

Where a system is designed with different types of memory in a layered model, this is referred to as a memory hierarchy. Systems can employ caches at multiple levels. The outer layers trade increased latency for increasing size. All the caches in the system must adhere to a memory coherency policy, which is part of the system architecture. Such layered systems usually number the layers - level 1, level 2 ... level n- with the increasing numbers representing increased access times for layers further from the core.

IO can also be provided at the different layers, that is, some no-wait-state register-based peripherals at level 1, out to memory mapped peripherals on remote system buses.

Figure B1-1 shows an example memory hierarchy.

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

L1 caches are always tightly coupled to the core, but L2 caches can be either:

* tightly coupled to the core
* implemented as memory mapped peripherals on the system bus.

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

<h1 id="B4">B4 内存序模型</h1>

This chapter describes the Virtual Memory System Architecture (VMSA) based on a Memory ManagementUnit (MMU). It contains the following sections:

<h2 id="B4.1">B4.1 关于VMSA</h2>

Complex operating systems typically use a virtual memory system to provide separate, protected address spaces for different processes. Processes are dynamically allocated memory and other memory mapped system resources under the control of a Memory Management Unit (MMU). The MMU allows fine-grained control of a memory system through a set of virtual to physical address mappings and associated memory properties held within one or more structures known as `Translation Lookaside Buffers` (TLBs) within the MMU. The contents of the TLBs are managed through hardware translation lookups from a set of translation tables maintained in memory.

The process of doing a full translation table lookup is called a `translation table walk`. It is performed automatically by hardware, and has a significant cost in execution time, at least one main memory access, and often two. TLBs reduce the average cost of a memory access by caching the results of translation table walks. Implementations can have a unified TLB (von Neumann architecture) or separate Instruction and Data TLBs (Harvard architecture).

The VMSA has been significantly enhanced in ARMv6. This is referred to as VMSAv6. To prevent the need for a TLB invalidation on a context switch, each virtual to physical address mapping can be marked as being associated with a particular application space, or as global for all application spaces. Only global mappings and those for the current application space are enabled at any time. By changing the `Application Space IDentifier` (ASID), the enabled set of virtual to physical address mappings can be altered. VMSAv6 has added definitions for different memory types (see `ARMv6 memory attributes - introduction` on page B2-8),and other attributes (see `Memory access control` on page B4-8). For backwards compatibility there is an XP control bit in the System Control Coprocessor, CP15 register 1, as defined in Register 1: Control register on page B4-40.

The set of memory properties associated with each TLB entry includes:

* Memory access permission control

    This controls whether a program has no-access, read-only access, or read/write access to the memory area. When an access is not permitted, a memory abort is signaled to the processor. The level of access allowed can be affected by whether the program is running in User mode, or a privileged mode, and by the use of domains.

* Memory region attributes

    These describe properties of a memory region. Examples include device (VMSAv6), non-cacheable, write-through, and write-back.

* Virtual-to-physical address mapping

    An address generated by the ARM® processor is called a virtual address. The MMU allows this address to be mapped to a different physical address. This physical address identifies which main memory location is being accessed.

    This can be used to manage the allocation of physical memory in many ways. For example, it can be used to allocate memory to different processes with potentially conflicting address maps, or to allow an application with a sparse address map to use a contiguous region of physical memory.

> Because of the Fast Context Switch Extension (FCSE, see Chapter B8), all references to virtual address in this chapter are made to the modified virtual address that it generates, except where explicitly stated otherwise. The virtual address and modified virtual address are equal when the FCSE mechanism is disabled (PID == zero). The FCSE is only present in ARMv6 for backwards compatibility. Its use in new systems is deprecated.

B4.2.1 TLB match process

Each TLB entry contains a modified virtual address, a page size, a physical address, and a set of memory properties. It is marked as being associated with a particular application space, or as global for all application spaces. Where an ASID is used, register 13 in CP15 determines the currently selected application space.

A TLB entry matches if bits 31-N of the modified virtual address match, and it is either marked as global, or the ASID matches the current ASID, where N is log2 of the page size for the TLB entry.

If two or more entries match at any time (including global and ASID specific entries), the behavior of a TLB is UNPREDICTABLE. The operating system must ensure that no more than one TLB entry can match at any time, typically by flushing its TLBs when global page mappings are changed.

A TLB can store entries based on the following block sizes:

* Supersections consist of 16MB blocks of memory
* Sections consist of 1MB blocks of memory
* Large pages consist of 64KB blocks of memory
* Small pages consist of 4KB blocks of memory.

> The use of Tiny (1KB) pages is not supported in VMSAv6.


<h1 id="B6">B6 Cache和Write buffer</h1>

<h1 id="B7">B7 紧耦合内存-TCM</h1>

<h2 id="B7.6">B7.6 SmartCache Behavior</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
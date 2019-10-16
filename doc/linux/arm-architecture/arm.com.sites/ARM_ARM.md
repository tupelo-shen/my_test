<h1 id="0">0 目录</h1>
* [1 前言](#1)
    - [1.1 关于Cortex-A8](#1.1)
    - [1.2 ARMv7-A架构](#1.2)
    - [1.3 处理器的组成](#1.3)
* [4 ARM指令集](#4)
    - [4.1 指令集编码](#4.1)
* [5 ARM寻址模式](#5)
    - [5.1 寻址模式1-数据处理操作数](#5.1)
    - [5.2 寻址模式2-load/store](#5.2)
---

<h1 id="1">1 前言</h1>

This preface describes the versions of the ARM® architecture and the contents of this manual, then lists the conventions and terminology it uses.

<h2 id="1.1">1.1 关于本手册</h2>

The purpose of this manual is to describe the ARM instruction set architecture, including its high code density Thumb® subset, and three of its standard coprocessor extensions:

* The standard System Control coprocessor (**coprocessor 15**), which is used to control memory system components such as caches, write buffers, Memory Management Units, and Protection Units.

* The Vector Floating-point (**VFP**) architecture, which uses coprocessors 10 and 11 to supply a high-performance floating-point instruction set.

* The debug architecture interface (**coprocessor 14**), formally added to the architecture in ARM v6 to provide software access to debug features in ARM cores, (for example, breakpoint and watchpoint control).

The 32-bit ARM and 16-bit Thumb instruction sets are described separately in Part A. The precise effects of each instruction are described, including any restrictions on its use. This information is of primary importance to authors of compilers, assemblers, and other programs that generate ARM machine code.

Assembler syntax is given for most of the instructions described in this manual, allowing instructions to be specified in textual form.

However, this manual is not intended as tutorial material for ARM assembler language, nor does it describe ARM assembler language at anything other than a very basic level. To make effective use of ARM assembler language, consult the documentation supplied with the assembler being used.

The memory and system architecture definition is significantly improved in ARM architecture version 6 (the latest version). Prior to this, it usually needs to be supplemented by detailed implementation-specific information from the technical reference manual of the device being used.

<h2 id="1.2">1.2 架构版本以及变体</h2>

The ARM instruction set architecture has evolved significantly since it was first developed, and will continue to be developed in the future. Six major versions of the instruction set have been defined to date, denoted by the version numbers 1 to 6. Of these, the first three versions including the original 26-bit architecture (the 32-bit architecture was introduced at ARMv3) are now OBSOLETE. All bits and encodings that were used for 26-bit features become RESERVED for future expansion by ARM Ltd. Versions can be qualified with variant letters to specify collections of additional instructions that are included as an architecture extension. Extensions are typically included in the base architecture of the next version number, ARMv5T being the notable exception. Provision is also made to exclude variants by prefixing the variant letter with x, for example the xP variant described below in the summary of version 5
features.

The valid architecture variants are as follows (variant in brackets for legacy reasons only):

    ARMv4, ARMv4T, ARMv5T, (ARMv5TExP), ARMv5TE, ARMv5TEJ, and ARMv6

The following architecture variants are now OBSOLETE:

    ARMv1, ARMv2, ARMv2a, ARMv3, ARMv3G, ARMv3M, ARMv4xM, ARMv4TxM, ARMv5, ARMv5xM, and ARMv5TxM

Details on OBSOLETE versions are available on request from ARM.

The ARM and Thumb instruction sets are summarized by architecture variant in ARM instructions and architecture versions on page A4-286 and Thumb instructions and architecture versions on page A7-125 respectively. The key differences introduced since ARMv4 are listed below.

<h2 id="1.3">1.3 约定</h2>

<h3 id="1.3.1">1.3.1 指令的伪码描述</h3>

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

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
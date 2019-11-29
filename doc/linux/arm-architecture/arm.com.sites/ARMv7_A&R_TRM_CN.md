<h1 id="0">0 目录</h1>
* [A1 ARM体系结构简介](#1)
    - [1.1 关于ARM架构](#1.1)
    - [1.2 指令集](#1.2)
    - [1.3 架构版本、配置和变体](#1.3)
    - [1.4 架构相关扩展](#1.4)
    - [1.5 ARM内存模型](#1.5)
* [A2 应用层编程者模型](#2)
    - [2.1 关于应用层编程者模型](#2.1)
    - [2.2 ARM数据类型和算法](#2.2)
    - [2.3 ARM寄存器](#2.3)
    - [2.4 应用编程状态寄存器（APSR）](#2.4)
    - [2.5 执行状态寄存器](#2.5)
    - [2.6 高级SIMD和浮点扩展](#2.6)
    - [2.7 浮点数据类型和算法](#2.7)
    - [2.8 polynomial arithmetic over {0,1}](#2.8)
    - [2.9 协处理器](#2.9)
    - [2.10 Thumb扩展环境](#2.10)
    - [2.11 Jazelle直接字节码扩展](#2.11)
    - [2.12 异常、debug和检查](#2.12)
* [A3 应用层级-内存模型](#3)
    - [3.1 地址空间](#3.1)
    - [3.2 内存排列](#3.2)
    - [3.3 大小端](#3.3)
    - [3.4 同步和信号量](#3.4)
    - [3.5 内存类型和属性以及内存顺序模型](#3.5)
    - [3.6 访问权限](#3.6)
    - [3.7 虚拟和物理地址](#3.7)
    - [3.8 内存访问顺序](#3.8)
    - [3.9 Cache和内存体系结构](#3.9)
* [A4 指令集](#4)
    - [4.1 关于指令集](#4.1)
    - [4.2 统一汇编语言](#4.2)
    - [4.3 分支指令](#4.3)
    - [4.4 数据处理指令](#4.4)
    - [4.5 状态寄存器访问指令](#4.5)
    - [4.6 Load/Store指令](#4.6)
    - [4.7 Load/Store多条指令](#4.7)
    - [4.8 混合指令](#4.8)
    - [4.9 异常产生和处理指令](#4.9)
    - [4.10 协处理器指令](#4.10)
    - [4.11 高级SIMD和浮点数Load/Store指令](#4.11)
    - [4.12 高级SIMD和浮点数寄存器转移指令](#4.12)
    - [4.13 高级SIMD数据处理指令](#4.13)
    - [4.14 浮点处理指令](#4.14)
* [A5 ARM指令集编码](#5)
    - [5.1 ARM指令集编码](#5.1)
    - [5.2 数据处理和混合指令](#5.2)
    - [5.3 Load/Store字和无符号字节](#5.3)
    - [5.4 多媒体指令](#5.4)
    - [5.5 分支、分支带有链接和块数据转移指令](#5.5)
    - [5.6 协处理器指令和SVC(Supervisor Call-硬件会产生一个中断，请求系统服务)](#5.6)
    - [5.7 非条件指令](#5.7)
* [A6 Thumb指令集编码](#6)
* [A7 高级SIMD和浮点指令集编码](#7)
* [A8 指令描述](#8)
* [A9 ThumbEE指令集](#9)
* [B10 系统级-编程模型](#10)
    - [10.1 关于系统级编程模型](#10.1)
    - [10.2 系统级概念和术语](#10.2)
    - [10.3 ARM处理器模型和ARM内核寄存器](#10.3)
    - [10.4 指令集状态](#10.4)
    - [10.5 安全扩展](#10.5)
    - [10.6 大物理地址扩展](#10.6)
    - [10.7 虚拟化扩展](#10.7)
    - [10.8 异常处理](#10.8)
    - [10.9 异常描述](#10.9)
    - [10.10 协处理器和系统控制](#10.10)
    - [10.11 高级SIMD和浮点支持](#10.11)
    - [10.12 Thumb执行环境](#10.12)
    - [10.13 Jazelle直接字节码执行](#10.13)
    - [10.14 Traps to the hypervisor](#10.14)
* [B11 通用内存架构](#11)
    - [11.1 关于内存系统架构](#11.1)
    - [11.2 缓存和分支预测期](#11.2)
    - [11.3 IMPLEMENTION DEFINED memory system features](#11.3)
    - [11.4 Pseudocode details of general memory system operations](#11.4)
* [B12 虚拟内存系统架构（VMSA）](#12)
    - [12.1 关于VMSA](#12.1)
    - [12.2 禁止MMU对VMSA行为的影响](#12.2)
    - [12.3 转换表](#12.3)
    - [12.4 安全和非安全地址空间](#12.4)
    - [12.5 短描述符转换表格式](#12.5)
    - [12.6 长描述符转换表格式](#12.6)
    - [12.7 内存访问控制](#12.7)
    - [12.8 内存区域属性](#12.8)
    - [12.9 转换后备缓冲区（TLB）](#12.9)
    - [12.10 TLB维护要求](#12.10)
    - [12.11 关于内存系统架构](#12.11)
    - [12.12 关于内存系统架构](#12.12)
    - [12.13 关于内存系统架构](#12.13)
    - [12.14 关于内存系统架构](#12.14)
    - [12.15 关于内存系统架构](#12.15)
    - [12.16 关于内存系统架构](#12.16)
    - [12.17 关于内存系统架构](#12.17)
    - [12.18 关于内存系统架构](#12.18)
    - [12.19 关于内存系统架构](#12.19)
* [B13 VMSA中的系统控制寄存器](#13)
* [B14 受保护的内存系统架构（PMSA）](#14)
* [B15 PMSA实现中的系统控制寄存器](#15)
* [B16 CPUID身份标识方案](#16)
* [B17 通用定时器](#17)
* [B18 系统指令](#18)

---

<h1 id="1">1 ARM体系结构简介</h1>

本章主要介绍ARM架构。

<h2 id="1.1">1.1 关于ARM架构</h2>

The ARM architecture supports implementations across a wide range of performance points. The architectural simplicity of ARM processors leads to very small implementations, and small implementations mean devices can have very low power consumption. Implementation size, performance, and very low power consumption are key attributes of the ARM architecture.

The ARM architecture is a Reduced Instruction Set Computer (RISC) architecture, as it incorporates these RISC architecture features:

* a large uniform register file
* a load/store architecture, where data-processing operations only operate on register contents, not directly on memory contents
* simple addressing modes, with all load/store addresses being determined from register contents and instruction fields only.

In addition, the ARM architecture provides:

* instructions that combine a shift with an arithmetic or logical operation
* auto-increment and auto-decrement addressing modes to optimize program loops
* Load and Store Multiple instructions to maximize data throughput
* conditional execution of many instructions to maximize execution throughput.

These enhancements to a basic RISC architecture mean ARM processors achieve a good balance of high performance, small program size, low power consumption, and small silicon area.

This Architecture Reference Manual defines a set of behaviors to which an implementation must conform, and a set of rules for software to use the implementation. It does not describe how to build an implementation.

Except where the architecture specifies differently, the programmer-visible behavior of an implementation must be the same as a simple sequential execution of the program. This programmer-visible behavior does not include the execution time of the program.

The ARM architecture includes definitions of:

* An associated debug architecture, see Debug architecture versions on page A1-31 and Part C of this manual.
* Associated trace architectures, that define trace macrocells that implementers can implement with the associated processor. For more information see the ARM Embedded Trace Macrocell Architecture Specification and the ARM CoreSight™ Program Flow Trace Architecture Specification.

<h2 id="1.2">1.2 指令集</h2>

The ARM instruction set is a set of 32-bit instructions providing comprehensive data-processing and control functions.

The Thumb instruction set was developed as a 16-bit instruction set with a subset of the functionality of the ARM instruction set. It provides significantly improved code density, at a cost of some reduction in performance. A processor executing Thumb instructions can change to executing ARM instructions for performance critical segments, in particular for handling interrupts.

ARMv6T2 introduced Thumb-2 technology. This technology extends the original Thumb instruction set with many 32-bit instructions. The range of 32-bit Thumb instructions included in ARMv6T2 permits Thumb code to achieve performance similar to ARM code, with code density better than that of earlier Thumb code.

<h3 id="1.2.1">1.2.1 执行环境的支持</h3>

Two additional instruction sets support execution environments:

* The architecture can provide hardware acceleration of Java bytecodes. For more information, see:
    - Jazelle direct bytecode execution support on page A2-97, for application level information
    - Jazelle direct bytecode execution on page B1-1241, for system level information.

    The Virtualization Extensions do not support hardware acceleration of Java bytecodes. That is, they support only a trivial implementation of the Jazelle® extension.
* The ThumbEE instruction set is a variant of the Thumb instruction set that minimizes the code size overhead of a Just-In-Time (JIT) or Ahead-Of-Time (AOT) compiler. JIT and AOT compilers convert execution environment source code to a native executable. For more information, see:
    - Thumb Execution Environment on page A2-95, for application level information
    - Thumb Execution Environment on page B1-1240, for system level information.

    From the publication of issue C.a of this manual, ARM deprecates any use of the ThumbEE instruction set.


<h2 id="1.3">1.3 架构版本、配置和变体</h2>

The ARM architecture has evolved significantly since its introduction, and ARM continues to develop it. Seven　major versions of the architecture have been defined to date, denoted by the version numbers 1 to 7. Of these, the　first three versions are now obsolete.

ARMv7 提供了3个配置文件：

* **ARMv7-A**   面向应用的配置文件:
    - 实现了具有多个工作模式的传统ARM架构。
    - 具有MMU内存管理单元，支持虚拟内存系统架构（VMSA）。因此，ARMv7-A的实现又可以称为VMSAv7实现。
    - 支持ARM和Thumb指令集。

* **ARMv7-R** 面向实时应用的配置文件:
    - 实现了具有多个工作模式的传统ARM架构。
    - 具有MPU-内存保护单元，支持受保护的内存系统架构（PMSA）。因此，ARMv7-R的实现又可以称为PMSAv7实现。
    - 支持ARM和Thumb指令集。

* **ARMv7-M** 面向微控制器的配置文件，在[ARMv7-M Architecture Reference Manual]()手册中进行描述:
    - 使用寄存器的硬件堆栈，为低延迟的中断处理提供编程模型，同时，支持使用高级语言编写中断处理程序。
    - 实现ARMv7 PMSA的变体。
    - 支持Thumb指令集的变体。

本参考手册只描述了ARMv7-A和ARMv7-R配置：

* 附录描述了ARMv4-ARMv6架构与ARMv7的不同。
* 对于使用M-架构的技术参考手册，请参考其独立手册。

<h3 id="1.3.1">1.3.1 Debug架构版本</h3>

Before ARMv6, the debug implementation for an ARM processor was IMPLEMENTATION DEFINED. ARMv6 defined the first debug architecture.

The debug architecture versions are:

* **v6 Debug**

    Introduced with the original ARMv6 architecture definition.

* **v6.1 Debug**

    Introduced to ARMv6K with the OPTIONAL Security Extensions, described in Architecture extensions on page A1-33. A VMSAv6 implementation that includes the Security Extensions must implement v6.1 Debug.

* **v7 Debug**

    First defined in issue A of this manual, and required by any ARMv7-R implementation

    An ARMv7-A implementation that does not include the Virtualization Extensions must implement either v7 Debug or v7.1 Debug.

    For more information about the Virtualization Extensions, see Architecture extensions on page A1-33.

* **v7.1 Debug**

    First defined in issue C.a of this manual, and required by any ARMv7-A implementation that includes the Virtualization Extensions.

For more information, see:

* [Chapter C1 Introduction to the ARM Debug Architecture](), for v7 Debug and v7.1 Debug
* [About v6 Debug and v6.1 Debug on page D13-2550], for v6 Debug and v6.1 Debug.

<h2 id="1.4">1.4 架构扩展</h2>

[Instruction set architecture extensions](#1.4.1) summarizes the extensions that mainly affect the Instruction Set Architecture(ISA), either extending the instructions implemented in the ARM and Thumb instruction sets, or implementing an additional instruction set.

[Architecture extensions](#1.4.2) on page A1-33 describes other extensions to the architecture.

<h3 id="1.4.1">1.4.1 指令集相关扩展</h3>

This manual describes the following extensions to the ISA:

* **Jazelle**

    Is the Java bytecode execution extension that extended ARMv5TE to ARMv5TEJ. From ARMv6, the architecture requires at least the trivial Jazelle implementation, but a Jazelle implementation is still often described as a Jazelle extension.

    The Virtualization Extensions require that the Jazelle implementation is the trivial Jazelle implementation.

* **ThumbEE**

    Is an extension that provides the ThumbEE instruction set, a variant of the Thumb instruction set that is designed as a target for dynamically generated code. In the original release of the ARMv7 architecture, the ThumbEE extension was:

    * A required extension to the ARMv7-A profile.
    * An optional extension to the ARMv7-R profile.

    From publication of issue C.a of this manual, ARM deprecates any use of ThumbEE instructions. However, ARMv7-A implementations must continue to include ThumbEE support, for backwards compatibility.

<h3 id="1.4.2">1.4.2 架构扩展</h3>

本手册也描述了针对ARMv7架构进行的扩展：

* **Security Extensions**

    Are an OPTIONAL set of extensions to VMSAv6 implementations of the ARMv6K architecture, and to the ARMv7-A architecture profile, that provide a set of security features that facilitate the development of secure applications.

* **Multiprocessing Extensions**

    Are an OPTIONAL set of extensions to the ARMv7-A and ARMv7-R profiles, that provides a set of features that enhance multiprocessing functionality.

* **Large Physical Address Extension**

    Is an OPTIONAL extension to VMSAv7 that provides an address translation system supporting physical addresses of up to 40 bits at a fine grain of translation. The Large Physical Address Extension requires implementation of the Multiprocessing Extensions.

* **Virtualization Extensions**

    Are an OPTIONAL set of extensions to VMSAv7 that provides hardware support for virtualizing the Non-secure state of a VMSAv7 implementation. This supports system use of a virtual machine monitor, also called a hypervisor, to switch Guest operating systems.

    The Virtualization Extensions require implementation of:
    * the Security Extensions
    * the Large Physical Address Extension
    * the v7.1 Debug architecture, see Scope of part C of this manual on page C1-2022.
    If an implementation that includes the Virtualization Extensions also implements:
    * The Performance Monitors Extension, then it must implement version 2 of that extension, PMUv2, see About the Performance Monitors on page C12-2302.
    * A trace macrocell, that trace macrocell must support the Virtualization Extensions. In particular, if the trace macrocell is:
        - an Embedded Trace Macrocell (ETM), the macrocell must implement ETMv3.5 or later, see the Embedded Trace Macrocell Architecture Specification
        - a Program Trace Macrocell (PTM), the macrocell must implement PFTv1.1 or later, see the CoreSight Program Flow Trace Architecture Specification.

    In some tables in this manual, an ARMv7-A implementation that includes the Virtualization Extensions is described as ARMv7VE, or as v7VE.

* **Generic Timer Extension**

    Is an OPTIONAL extension to any ARMv7-A or ARMv7-R, that provides a system timer, and a low-latency register interface to it.

    This extension is introduced with the Large Physical Address Extension and Virtualization Extensions, but can be implemented with any earlier version of the ARMv7 architecture. The Generic Timer Extension does not require the implementation of any of the extensions described in this subsection.

    For more information see Chapter B8 The Generic Timer.

* **Performance Monitors Extension**

The ARMv7 architecture:

* reserves CP15 register space for IMPLEMENTATION DEFINED performance monitors
* defines a recommended performance monitors implementation.

From issue C.a of this manual, this recommended implementation is called the Performance Monitors Extension.

The Performance Monitors Extension does not require the implementation of any of the extensions described in this subsection.

If an ARMv7 implementation that includes v7.1 Debug also includes the Performance Monitors Extension, it must implement PMUv2.

For more information see Chapter C12 The Performance Monitors Extension.

<h2 id="1.5">1.5 ARM内存模型</h2>

ARM指令集能够访问的地址空间是2^32 (4G)字节大小。

该架构提供以下功能：

* 发生未对齐内存访问时报异常
* 限制应用程序对指定内存区域的访问
* 将指令传递的虚拟地址转换成物理地址
* 在大小端之间进行切换
* 控制对内存的访问顺序
* 控制Cache
* 同步多个处理器对共享内存的访问


<h1 id="2">2 应用级编程者模型</h1>

本章从应用的视角来看待ARM编程者模型。具体内容请参考下面的各个小节：

<h2 id="2.1">2.1 关于编程者模型</h2>

本章描述了应用开发所需要的编程者模型信息。

The information in this chapter is distinct from the system information required to service and support application execution under an operating system, or higher level of system software. However, some knowledge of that system information is needed to put the Application level programmers' model into context.

依赖于实现的架构扩展不同，支持多个执行特权等级，编号从PL0开始向上增加，PL0是最低特权等级，通常被描述为非特权等级。应用级编程者模型就是指执行在PL0等级的软件的编程者模型。

System software determines the privilege level at which application software runs. When an operating system supports execution at both PL1 and PL0, an application usually runs unprivileged. This:

* permits the operating system to allocate system resources to an application in a unique or shared manner
* provides a degree of protection from other processes and tasks, and so helps protect the operating system from malfunctioning applications.

This chapter indicates where some system level understanding is helpful, and if appropriate it gives a reference to
the system level description in Chapter B1 System Level Programmers’ Model, or elsewhere.

The Security Extensions extend the architecture to provide hardware security features that support the development of secure applications, by providing two Security states. The Virtualization Extensions further extend the architecture to provide virtualization of operation in Non-secure state. However, application level software is generally unaware of these extensions. For more information, see The Security Extensions on page B1-1156 and The Virtualization Extensions on page B1-1162.

<h3 id="2.1.1">2.1.1 指令集、算术运算、寄存器</h3>

The ARM and Thumb instruction sets both provide a wide range of integer arithmetic and logical operations, that operate on register file of sixteen 32-bit registers, the ARM core registers. As described in ARM core registers on page A2-45, these registers include the special registers SP, LR, and PC. ARM core data types and arithmetic on page A2-40 gives more information about these operations.

<h2 id="2.2">2.2 ARM核数据类型和算术</h2>

All ARMv7-A and ARMv7-R processors support the following data types in memory:

| 名称 | 位 |
| --- | --- |
| 字节 | 8 |
| 半字 | 16 |
| 字 | 32 |
| DWORD | 64 |

处理器寄存器大小为32位。指令集包含指令，支持处理寄存器保存下面的数据类型。

* 32位指针
* 无符号或有符号32位整数
* unsigned 16-bit or 8-bit integers, held in zero-extended form
* signed 16-bit or 8-bit integers, held in sign-extended form
* two 16-bit integers packed into a register
* four 8-bit integers packed into a register
* unsigned or signed 64-bit integers held in two registers.

Load and store operations can transfer bytes, halfwords, or words to and from memory. Loads of bytes or halfwords zero-extend or sign-extend the data as it is loaded, as specified in the appropriate load instruction.

The instruction sets include load and store operations that transfer two or more words to and from memory. Software can load and store doublewords using these instructions.

<h3 id="2.2.1">2.2.1 整数运算</h3>

The instruction set provides a wide variety of operations on the values in registers, including bitwise logical operations, shifts, additions, subtractions, multiplications, and many others. The pseudocode described in Appendix D16 Pseudocode Definition defines these operations, usually in one of three ways:

* By direct use of the pseudocode operators and built-in functions defined in Operators and built-in functions on page D16-2653.
* By use of pseudocode helper functions defined in the main text. These can be located using the table in Appendix D17 Pseudocode Index.
* By a sequence of the form:
    - Use of the SInt(), UInt(), and Int() built-in functions defined in [Converting bitstrings to integers on page D16-2655]() to convert the bitstring contents of the instruction operands to the unbounded integers that they represent as two's complement or unsigned integers.
    - Use of mathematical operators, built-in functions and helper functions on those unbounded integers to calculate other such integers.
    - Use of either the bitstring extraction operator defined in Bitstring extraction on page D16-2654 or of the saturation helper functions described in Pseudocode details of saturation on page A2-44 to convert an unbounded integer result into a bitstring result that can be written to a register.


<h2 id="2.3">2.3 Thumb-2EE架构</h2>

<h3 id="2.3.1">2.3.1 写PC寄存器</h3>

In ARMv7, many data-processing instructions can write to the PC. Writes to the PC are handled as follows:

* In Thumb state, the following 16-bit Thumb instruction encodings branch to the value written to the PC:
    - encoding T2 of ADD (register, Thumb) on page A8-310
    - encoding T1 of MOV (register, Thumb) on page A8-486.
    The value written to the PC is forced to be halfword-aligned by ignoring its least significant bit, treating that bit as being 0.

* The B, BL, CBNZ, CBZ, CHKA, HB, HBL, HBLP, HBP, TBB, and TBH instructions remain in the same instruction set state and branch to the value written to the PC.

    The definition of each of these instructions ensures that the value written to the PC is correctly aligned for the current instruction set state.

<h2 id="2.11">2.11 处理器的地址</h2>


<h1 id="3">3 内存模型</h1>

本章从应用的角度讲述如何理解内存模型。

<h2 id="3.1">3.1 地址空间</h2>

<h2 id="3.2">3.2 排列支持</h2>

<h2 id="3.3">3.3 大小端支持</h2>

<h2 id="3.4">3.4 同步和信号量</h2>

<h2 id="3.5">3.5 内存类型和属性以及内存顺序模型</h2>

<h2 id="3.6">3.6 访问权限</h2>

<h2 id="3.7">3.7 虚拟和物理地址</h2>

<h2 id="3.8">3.8 内存访问顺序</h2>

<h2 id="3.9">3.9 Cache和内存体系结构</h2>




<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

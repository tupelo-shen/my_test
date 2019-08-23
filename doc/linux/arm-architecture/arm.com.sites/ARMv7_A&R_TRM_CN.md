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
* [B11 通用内存系统架构特点](#11)
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

ARMv7 provides three profiles:

* **ARMv7-A**   Application profile, described in this manual:
    - Implements a traditional ARM architecture with multiple modes.
    - Supports a Virtual Memory System Architecture (VMSA) based on a Memory Management Unit (MMU). An ARMv7-A implementation can be called a VMSAv7 implementation.
    - Supports the ARM and Thumb instruction sets.
* **ARMv7-R** Real-time profile, described in this manual:
    - Implements a traditional ARM architecture with multiple modes.
    - Supports a Protected Memory System Architecture (PMSA) based on a Memory Protection Unit (MPU). An ARMv7-R implementation can be called a PMSAv7 implementation.
    - Supports the ARM and Thumb instruction sets.
* **ARMv7-M** Microcontroller profile, described in the ARMv7-M Architecture Reference Manual:
    - Implements a programmers' model designed for low-latency interrupt processing, with hardware stacking of registers and support for writing interrupt handlers in high-level languages.
    - Implements a variant of the ARMv7 PMSA.
    - Supports a variant of the Thumb instruction set.

Parts A, B, and C of this Architecture Reference Manual describe the ARMv7-A and ARMv7-R profiles:

* Appendixes describe how the ARMv4-ARMv6 architecture versions differ from ARMv7.
* Separate Architecture Reference Manuals define the M-profile architectures, see Additional reading on page xxiii.

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

<h3 id="1.4.1">1.4.1 指令集架构相关扩展</h3>

This manual describes the following extensions to the ISA:

* **Jazelle**

    Is the Java bytecode execution extension that extended ARMv5TE to ARMv5TEJ. From ARMv6, the architecture requires at least the trivial Jazelle implementation, but a Jazelle implementation is still often described as a Jazelle extension.

    The Virtualization Extensions require that the Jazelle implementation is the trivial Jazelle implementation.

* **ThumbEE**

    Is an extension that provides the ThumbEE instruction set, a variant of the Thumb instruction set that is designed as a target for dynamically generated code. In the original release of the ARMv7 architecture, the ThumbEE extension was:

    * A required extension to the ARMv7-A profile.
    * An optional extension to the ARMv7-R profile.

    From publication of issue C.a of this manual, ARM deprecates any use of ThumbEE instructions. However, ARMv7-A implementations must continue to include ThumbEE
support, for backwards compatibility.



<h1 id="2">2 编程者模型</h1>

<h2 id="2.1">2.1 关于编程者模型</h2>


<h2 id="2.2">2.2 Thumb-2架构</h2>



<h2 id="2.3">2.3 Thumb-2EE架构</h2>



<h2 id="2.11">2.11 处理器的地址</h2>



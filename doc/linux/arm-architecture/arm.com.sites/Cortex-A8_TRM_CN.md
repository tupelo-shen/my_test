<h1 id="0">0 目录</h1>
* [1 引言](#1)
    - [1.1 关于ARM920T](#1.1)
    - [1.2 功能框图](#1.2)
* [2 编程者模型](#2)
    - [2.1 关于编程者模型](#2.1)
    - [2.2 关于ARM9TDMI编程者模型](#2.2)
    - [2.3 CP15寄存器映射](#2.3)
* [3 内存管理单元-MMU](#3)
    - [3.1 关于MMU](#3.1)
    - [3.2 MMU可编程寄存器](#3.2)
    - [3.3 地址变换](#3.3)
    - [3.9 MMU和Cache的交互](#3.9)
* [4 Cache、写缓冲区、和PA TAG RAM](#4)
    - [4.1 关于Cache和write buffer](#4.1)
    - [4.2 ICache](#4.2)
    - [4.3 DCache & 写缓存](#4.3)


---

<h1 id="1">1 引言</h1>

<h2 id="1.1">1.1 关于Cortex-A8</h2>

The Cortex-A8 processor is a high-performance, low-power, cached application processor that provides full virtual memory capabilities. The features of the processor include:

* full implementation of the ARM architecture v7-A instruction set
* configurable 64-bit or 128-bit high-speed Advanced Microprocessor Bus Architecture (AMBA) with Advanced Extensible Interface (AXI) for main memory interface supporting up to 22 outstanding transactions
* a pipeline for executing ARM integer instructions
* a NEON pipeline for executing NEON and VFP instruction sets
* dynamic branch prediction with branch target address cache, global history　buffer, and 8-entry return stack
* Memory Management Unit (MMU) and separate instruction and data Translation　Look-aside Buffers (TLBs) of 32 entries each
* Level 1 instruction and data caches of 16KB or 32KB configurable size
* Level 2 cache of 0KB, 64KB through 2MB configurable size
* Level 2 cache with parity and Error Correction Code (ECC) configuration option
* Level 2 cache configuration option that supports one, two, or four tag banks
* Embedded Trace Macrocell (ETM) support for non-intrusive debug
* ARMv7 debug with watchpoint and breakpoint registers and a 32-bit Advanced Peripheral Bus (APB) interface to the CoreSight debug system.



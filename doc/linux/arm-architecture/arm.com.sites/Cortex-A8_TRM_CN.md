<h1 id="0">0 目录</h1>
* [1 引言](#1)
    - [1.1 关于Cortex-A8](#1.1)
    - [1.2 ARMv7-A架构](#1.2)
    - [1.3 处理器的组成](#1.3)
* [2 编程者模型](#2)
    - [2.1 关于编程者模型](#2.1)
    - [2.2 Thumb-2架构](#2.2)
    - [2.3 Thumb-2EE架构](#2.3)
    - [2.4 Jazelle扩展](#2.4)
    - [2.5 TrustZone技术](#2.5)
    - [2.6 NEON技术](#2.6)
    - [2.7 VFPv3架构](#2.7)
    - [2.8 处理器工作状态](#2.8)
    - [2.9 数据类型](#2.9)
    - [2.10 内存存储格式](#2.10)
    - [2.11 处理器的地址](#2.11)
    - [2.12 工作模式](#2.12)
    - [2.13 寄存器](#2.13)
    - [2.14 程序状态寄存器](#2.14)
    - [2.15 异常](#2.15)
    - [2.16 TrustZone软件考虑](#2.16)
    - [2.17 TrustZone硬件考虑](#2.17)
    - [2.18 控制协处理器](#2.18)
* [3 系统控制协处理器](#3)
    - [3.1 关于协处理器](#3.1)
    - [3.2 协处理器的寄存器](#3.2)
* [4 非对齐数据和混合字节序数据的支持](#4)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [5 程序流预测](#5)
    - [5.1 关于程序流预测](#5.1)
    - [5.2 预测指令](#5.2)
    - [5.3 非预测指令](#5.3)
    - [5.4 最优性能](#5.4)
    - [5.5 使能程序流预测](#5.5)
    - [5.6 操作系统和预测器上下文](#5.6)
* [6 内存管理单元](#6)
    - [6.1 关于MMU](#6.1)
    - [6.2 内存访问序列](#6.2)
    - [6.3 16M大小的supersection支持](#6.3)
    - [6.4 控制MMU](#6.4)
    - [6.5 外部中止](#6.5)
    - [6.6 TLB锁定](#6.6)
    - [6.7 MMU 软件可访问的寄存器](#6.7)
* [7 Level1-内存系统](#7)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [8 Level2-内存系统](#8)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [9 外部存储器接口](#9)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [10 时钟，复位，和电源控制](#10)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [11 NEON&VFPLite编程者模型](#11)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [12 嵌入式跟踪宏单元](#12)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [13 交叉触发接口](#13)
    - [4.1 aaa](#4.1)
    - [4.2 bbb](#4.2)
* [16 指令采样周期](#16)
    - [16.1 关于指令采样周期](#16.1)
    - [16.2 Instruction-specific scheduling for ARM instructions](#16.2)

---

<h1 id="1">1 引言</h1>

<h2 id="1.1">1.1 关于Cortex-A8</h2>

Cortex-A8处理器是款高性能、低功耗、带有高速缓存的应用处理器，并且提供完整的虚拟地址功能。其主要特点如下：

* ARMv7-A架构指令集的完整实现
* 可配置的64位或128位高速高级微处理器总线架构（AMBA），其带有内存接口使用的高级扩展接口（AXI），可以支持22个未处理事务
* 执行ARM整数指令集的流水线
* NEON流水线，执行NEON和VFP指令集
* dynamic branch prediction with branch target address cache, global history　buffer, and 8-entry return stack
*
* 内存管理单元（MMU）和独立的指令和数据转译后备缓冲区（TLB），每一个具有32项
* Level 1 instruction and data caches of 16KB or 32KB configurable size
* Level 2 cache of 0KB, 64KB through 2MB configurable size
* Level 2 cache with parity and Error Correction Code (ECC) configuration option
* Level 2 cache configuration option that supports one, two, or four tag banks
* Embedded Trace Macrocell (ETM) support for non-intrusive debug
* ARMv7 debug with watchpoint and breakpoint registers and a 32-bit Advanced Peripheral Bus (APB) interface to the CoreSight debug system.

<h2 id="1.2">1.2 ARMv7-A架构</h2>

Cortex实现了ARMv7-A架构，包含以下功能：

1. ARM Thumb-2架构，整体代码密度可与Thumb相媲美，性能可与ARM指令相媲美。有关ARM和Thumb指令集的详细信息，请参阅[<<ARM架构技术参考手册>>Thumb2补充说明]。
2. Thumb-2扩展环境（Thumb-2EE）架构，执行环境加速。
3. TrustZone技术，增强安全。
4. NEON技术，加速多媒体应用的性能，比如3D图形处理。
5. 用于浮点数计算的VFPv3架构，与IEEE754标准兼容。

<h2 id="1.3">1.3 处理器构成</h2>

主要构成：

* 指令获取
* 指令解码
* 指令执行
* 加载和存储（load/store）
* L2 Cache
* NEON技术
* ETM

图1-1 展示了处理器结构

![图1-1 处理器结构](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Cortex-A8-Picture1-1.JPG)

<h3 id="1.3.1">1.3.1 指令获取</h3>

指令获取单元预测指令流，从L1 Cache中提取指令，把提取到的指令写入buffer中，以备解码流水线使用。

<h3 id="1.3.2">1.3.2 指令解码</h3>

指令解码单元解码并序列化ARM和Thumb-2指令，这些指令包括debug控制协处理器-CP14和系统控制协处理器-CP15的指令。关于CP14请参考 [第12章 debug](#12)。

指令解码单元能够处理的序列：

1. 异常
2. debug事件
3. 复位初始化
4. 内存内嵌自测（MBIST-Memory Built-In SelfTest）
5. 等待中断
6. 其它异常事件

<h3 id="1.3.3">1.3.3 指令执行</h3>



<h2 id="1.4">1.4 处理器对外接口</h2>

* AMBA AXI接口
* APB CoreSight 接口
* ATB CoreSight 接口
* DFT接口

<h1 id="2">2 编程者模型</h1>

<h2 id="2.1">2.1 关于编程者模型</h2>

该段描述了编写功能代码所需要的基本细节，并不涉及内部微架构的细节。

Cortex-A8实现ARMv7-A架构，主要包括：

1. 32位ARM指令集
2. 16位和32位Thumb2指令集
3. Thumb-2EE指令集
4. TrustZone技术
5. NEON技术

更多的细节请参考其它ARM架构参考手册。

<h2 id="2.2">2.2 Thumb-2架构</h2>

Thumb-2架构是16位Thumb指令集的增强版。添加了能够与16位指令自由混合使用的32位指令。32位指令的增加使得Thumb-2架构涵盖了ARM指令集的功能。这样，32位的指令集使得Thumb-2即继承了早期版本的Thumb架构的代码密度，同时也具备了已有ARM指令集的性能。

Thumb指令集和ARM指令集之间最主要的区别是大多数的32位Thumb指令是非条件指令，而ARM指令是条件的。Thumb-2引入了一个条件执行指令，*IT*，即逻辑 *if-then-else*函数，应用在这些指令上，使其成为条件指令。

可以通过设置CPSR寄存器的T位为1、J位为0，使处理器处于Thumb状态，这时候就可以将Thumb-2指令作为Thumb指令执行了。

除了32位Thumb指令之外，还有一些16位Thumb指令和32位ARM指令，作为Thumb-2架构的一部分。

主要的增强有：

* Thumb指令集添加了32位指令集：
    - 提供在Thumb状态的异常处理
    - 提供协处理器的访问
    - 包括数字信号处理（DSP）和多媒体指令
    - 在单个16位指令限制了编译器可用功能的情况下提高性能
* 增加16位IT指令，该指令后面的1到4条Thumb指令的代码块，可以是条件的。
* 增加16位CZB（Compare with Zero and Branch）指令，用一条指令取代原先的2条指令，提高代码密度。

32位ARM Thumb2指令集的格式如下图2-1所示：

![图2-1 32位ARM Thumb2指令集的格式](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Cortex-A8-Picture2-1.JPG)

hw1（第一个半字）决定了指令的长度和功能。如果处理器解码认为是32位，则处理器会再从指令地址中取出hw2（第二个半字）。

总结起来就是，Thumb-2指令集给了平衡性能和代码大小的灵活性。比如，对于很多应用的临界循环代码，比如快速中断和DSP算法，可以使用32位的多媒体指令；对于其他应用，就可以使用更小的16位经典Thumb指令。

<h2 id="2.3">2.3 Thumb-2EE架构</h2>

Thumb-2EE是Thumb-2架构的一个变体。它专门为生成动态代码而设计。

Thumb-2EE相比使用ARM或Thumb-2指令的代码，编译后的二进制文件具有更高的代码密度。

Thumb-2EE提供了一种处理器状态，ThumbEE状态，可以通过CPSR寄存器的T和J位进行设置。更多详细的细节请参考[《Architecture Reference Manual, Thumb-2 Execution Environment supplement》]()。

<h3 id="2.3.1">2.3.1 指令</h3>

在ThumbEE状态，处理器使用的指令集几乎与Thumb-2相同，只是某些指令行为不一样，还有一些被移除，或者，添加了一些新的指令。

与Thumb-2的主要不同的地方是：

* 在Thumb和ThumbEE状态都添加了状态迁移指令。
* 跳转到服务程序的新指令
* 在执行Load和Store指令的时候，对null指针的检查
* ThumbEE状态具有检查数组越界的额外指令
* 对Load，Store和branch指令做了一些其它的修正

<h2 id="2.4">2.4 Jazelle扩展</h2>

Cortex-A8实现了Jazelle扩展，但是一个简单版本。这意味着处理器不会加速任何字节码的执行，而是完全由软件服务程序执行。

在该Jazelle扩展的实现中：

* 不支持Jazelle状态
* BJX指令表现出与BX指令相同的行为

关于Jazelle扩展的详细内容可以参考《[ARM Architecture Reference Manual](https://www.scss.tcd.ie/~waldroj/3d1/arm_arm.pdf)》

<h2 id="2.5">2.5 TrustZone技术</h2>

处理器支持TrustZone安全扩展为软件提供了一个安全的环境。TrustZone技术增强了arm架构原有的硬件安全功能，更方便地开发安全应用。

该功能基于下面这些基本原则：

* 处理器定义了一组核心操作，可以用来在安全状态和非安全状态之间切换，而大部分代码运行在非安全状态。只有可信的代码运行在安全状态。
* 定义一些安全内存，只有内核处在安全状态，可以访问安全内存。
* 进入安全状态被严格控制。
* 只有在编程点才可以退出安全状态。
* 严格限制debug功能。
* 复位进入安全状态。

对于异常的处理和其它版本的ARM架构类似。对于某些异常只有运行在安全状态的代码可以处理。

关于TrustZone工作原理的详细内容可以参考《[ARM Architecture Reference Manual](https://www.scss.tcd.ie/~waldroj/3d1/arm_arm.pdf)》

<h3 id="2.5.1">2.5.1 TrustZone模型</h3>

TrustZone技术的立足点是，把处理器的运行环境分为两部分：安全状态和非安全状态，且安全数据不会泄漏到非安全状态。软件安全监控代码，运行在Monitor模式，负责连接2种状态，管理控制程序流，作用就像一个守门员。因此，系统具有安全和非安全外设，它们分别由安全设备和非安全设备驱动控制。图2-7展示了安全状态和非安全状态之间的关系。因此，操作系统（OS）分割成了安全OS和非安全OS，安全OS包含安全内核，非安全OS包含非安全内核。

![图2-1 安全和非安全状态之间的区别](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/p2-7.PNG)

正常情况下，非安全操作时，OS已正常方式运行任务。当用户请求执行安全操作时，请求会通过安全外设驱动发送给安全内核，此时的内核工作在特权模式。然后，调用安全Monitor把操作发送安全状态。

安全系统的这种方法，意味着工作在非安全状态的系统OS，只有几个固定的入口点，通过安全monitor可以进入到安全状态。而安全状态的代码基础就是安全内核和安全设备驱动，它们的代码规模很小，且简单易维护和验证。

更多的关于TrustZone工作原理请参考[ARM Architecture Reference Manual, Security Extensions supplement]()

<h2 id="2.10">2.10 内存存储格式</h2>

处理器把内存看成字节的线性集合，升序排列。比如，字节0-3占据第一个存储字位置，字节4-7占据第二个存储字。但是，字节的排序既可以是大端模式，也可以是小端模式。

另外，处理器支持混合字节序，未对齐数据访问。

<h3 id="2.10.1">2.10.1 大端格式</h3>

一个32位值的最高字节存储在32字的最低字节位置。

<h3 id="2.10.2">2.10.2 小端模式</h3>

一个32位值的最低字节存储在32字的最低字节位置。

<h2 id="2.11">2.11 处理器的地址</h2>

Cortex-A8系统中具有3种不同类型的地址：

* 虚拟地址（VA）
* 修正后的虚拟地址（MVA）
* 物理地址（PA）

其地址类型和ARM920T的地址类型相似，只是因为Cortex-A8处理器采用了ARMv7-A架构，从而引入了TrustZone技术。导致处理器核可能处于2种状态：安全状态和非安全状态。所以，转译表也分为安全转译表和非安全转译表。

表2-7 处理器中的地址类型

| 处理器地址 | Cache | TLB | AXI总线 |
| ---------- | -------------- | --------------------- | -------|
| 虚拟地址   | 虚拟索引物理tag | 转换虚拟地址到物理地址 | 物理地址|

This is an example of the address manipulation that occurs when the processor requests an instruction.

1. The processor issues the VA of the instruction as the Secure or Nonsecure VA according to the state of the processor.
2. The lower bits of the VA indexes the instruction cache. The VA is translated using the Secure or Nonsecure Process ID, CP15 c13, to the MVA, and then to PA in the Translation Lookaside Buffer (TLB). The TLB performs the translation in parallel with the cache lookup. The translation uses secure descriptors if the core is in the Secure state. Otherwise it uses the nonsecure ones.
3. If the TLB performs a successful protection check on the MVA, and the PA tag is in the instruction cache, the instruction data is returned to the processor. For information on unsuccessful protection checks, see Aborts on page 2-37.
4. The PA is passed to the L2 cache. If the L2 cache contains the physical address of the requested instruction, the L2 cache supplies the instruction data.
5. The PA is passed to the AXI bus interface to perform an external access, in the event of a cache miss. The external access is always Nonsecure when the core is in the Nonsecure state. In the Secure state, the external access is Secure or Nonsecure according to the NS attribute value in the selected descriptor.

<h2 id="2.13">2.13 寄存器</h2>

总共具有40个寄存器：

* 33个通用目的寄存器（32位）
* 7个状态寄存器（32位）

这些寄存器并不是同时都可以访问。处理器工作状态和模式决定了用户可以访问哪些寄存器。

<h3 id="2.13.1">2.13.1 状态寄存器</h3>

ARM状态下，具有16个数据寄存器和1或2个状态寄存器，可以在任何时候访问。在特权模式，和模式相关的一些影子寄存器（banked register）可用，而不是直接使用正常的寄存器。

图2-10展示了在每一种模式下可用的寄存器

![图2-10 安全和非安全状态之间的区别](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/p2-10.PNG)

Thumb和ThumbEE状态下与ARM状态访问的寄存器组相同。但是，对于某些寄存器，16位指令只能提供有限的访问。而32位的Thumb2和Thumb-2EE就没有这样的限制。

r0-r13表示通用寄存器，可以用来存储数据或地址值。

寄存器r14和r15具有下面的特殊功能：

* 链接寄存器

    - 寄存器r14，被用作子程序的链接寄存器（LR）
    - 寄存器r14存储执行带有链接指令的分支时返回的地址，比如指令（BL或BLX）
    - 在其他的时候，可以把r14作为通用目的寄存器
    - 类似地，当处理器接收到中断和异常，或者在中断或异常例程中执行BL或BLX指令时，相应的存储寄存器r14_mon，r14_svc，r14_irq，r14_fiq，r14_abt和r14_und将保存返回值。

* 程序计数器寄存器r15，存储着程序计数器PC:

    + ARM状态下，按word对齐
    + Thumb状态下，按half-word对齐
    + ThumbEE状态下，按half-word对齐

One of the status registers, the Current Program Status Register (CPSR), contains condition code flags, status bits, and current mode bits.

In privileged modes, another register, one of the Saved Program Status Registers (SPSR), is accessible. This contains the condition code flags, status bits, and current mode bits saved as a result of the exception that caused entry to the current mode. Typically, this is used when returning after handling an exception.

Banked registers have a mode identifier that indicates which mode they relate to. Table 2-9 shows these mode identifiers.

FIQ mode has seven banked registers mapped to r8–r14, that is, r8_fiq through r14_fiq. As a result many FIQ handlers do not have to save any registers.

The Monitor, Supervisor, Abort, IRQ, and Undefined modes have alternative mode-specific registers mapped to r13 and r14, that permits a private stack pointer and link register for each mode.

Monitor,Supervisor,Abort,IRQ和Undifined模式具有映射到r13和r14的与模式有关的替代寄存器，从而允许每种模式具有专有堆栈指针和链接寄存器。


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="5">5 程序流预测</h1>

This chapter describes how the processor performs branch prediction.

<h2 id="5.1">5.1 关于程序流预测</h2>

The processor contains program flow prediction hardware, also known as **branch prediction**. With program flow prediction disabled, all taken branches incur a 13-cycle penalty. With program flow prediction enabled, all mispredicted branches incur a 13-cycle penalty.

To avoid this penalty, the branch prediction hardware operates at the front of the instruction pipeline. The branch prediction hardware consists of:

* a 512-entry 2-way set associative Branch Target Buffer (BTB)
* a 4096-entry Global History Buffer (GHB)
* an 8-entry return stack.

An unpredicted branch executes in the same way as a branch that is predicted as not taken. Incorrect or invalid prediction of the branch prediction or target address causes the pipeline to flush, invalidating all of the following instructions.

<h2 id="5.2">5.2 预测指令</h2>

This section shows the instructions that the processor predicts. Unless otherwise specified, the list applies to ARM, Thumb-2, and Thumb-2EE instructions. See the ARM Architecture Reference Manual for more information about instructions or addressing modes.

The flow prediction hardware predicts the following instructions:

| 指令名称 | 说明|
| -------- | -------- |
| B conditional |  |
| B unconditional | |
| BL |
| BLX(1) immediate | The BL and BLX(1) instructions act as function calls and push the return address and ARM or Thumb state onto the return stack. |
| BLX(2) register | The BLX(2) instruction acts as a function call and pushes the return address and ARM or Thumb state onto the return stack. |
| BX | The BX r14 instruction acts as a function return and pops the return address and ARM or Thumb state from the return stack. |
| LDM(1) with PC in the register list in ARM state | The LDM instruction with r13 specified as the base register acts as a function return and pops the return address and ARM or Thumb state from the return stack. |
| POP with PC in register list in Thumb state | The POP instruction acts as a function return and pops the return address and ARM or Thumb state from the return stack. |
| LDM with PC in register list in Thumb or ThumbEE state | The LDM instruction with r13 specified as the base register, or r9 specified as the base register in ThumbEE state acts as a function return and pops the return address and ARM or Thumb state from the return stack. |

In ARM state, the second operand of a data-processing instruction can be a 32-bit
immediate value, an immediate shift value, or a register shift value. An instruction
with an immediate shift value or a register shift value is predicted. An instruction
with a 32-bit immediate value is not predicted. For example:

    MOV pc, r10, LSL r3 is predicted
    ADD pc, r0, r1, LSL #2 is predicted
    ADD pc, r4, #4 is not predicted.

There is no restriction on the opcode predicted, but a majority of opcodes do not make sense for branch-type instructions. Usually only MOV, ADD, and SUB are useful.

> Instructions with the S suffix are not predicted. They are typically used to return from exceptions and have side effects that can change privilege mode and security state.

<h3 id="5.2.1">5.2.1 返回堆栈预测</h3>

The return stack stores the address and the ARM or Thumb state of the instruction after
a function-call type branch instruction. This address is equal to the link register value
stored in r14.

The following instructions cause a return stack push if predicted:

* BL immediate
* BLX(1) immediate
* BLX(2) register
* HBL (ThumbEE state)
* HBLP (ThumbEE state).

The following instructions cause a return stack pop if predicted:

    - BX r14
    - MOV pc, r14
    - LDM r13, {…pc}
    - LDR pc, [r13]
    - LDM r9, {..pc}            ; (只有ThumbEE状态)
    - LDR pc, [r9]              ; (只有ThumbEE状态)。

The LDR instruction can use any of the addressing modes, as long as r13 is the base register. Additionally, in ThumbEE state you can also use r9 as a stack pointer so the LDR and LDM instructions with pc as a destination and r9 as a base register are also treated as a return stack pop.

Because return-from-exception instructions can change processor privilege mode and security state, they are not predicted. This includes the LDM(3) instruction, and the MOVS pc, r14 instruction.

<h2 id="5.3">5.3 非预测指令</h2>

<h2 id="5.4">5.4 最优性能</h2>

You can avoid certain code constructs to maximize branch prediction performance. For example:

* Using conditional Undefined instructions in normal code to enter the undefined handler as a means of doing emulation.
* Coding more than two likely taken branches per fetch. This can only happen in Thumb state. Unless used as a jump table where each branch is its own basic block, use NOPs for padding.
* Coding more than three branches per fetch that are likely to be executed in sequence.

    In Thumb state, it is possible to pack four branches in a single fetch, for example, in a multiway branch:

        BVS overflow
        BGT greater_than
        BLT less_than
        B equal

    This is a sequence of more than three branches with three conditional branches, and the fourth branch is likely to be reached. Avoid this kind of sequence, or use NOPs to break up the branch sequence.


<h2 id="5.5">5.5 使能程序流预测</h2>

<h2 id="5.6">5.6 操作系统和预测器上下文</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="6">6 内存管理单元</h1>

本章描述了MMU。

<h2 id="6.1">6.1 关于MMU</h2>

The MMU works with the L1 and L2 memory system to translate virtual addresses to physical addresses. It also controls accesses to and from external memory. See the ARM Architecture Reference Manual for a full architectural description of the MMU.

The processor implements the ARMv7-A MMU enhanced with TrustZone features to provide address translation and access permission checks. The MMU controls table walk hardware that accesses translation tables in main memory. The MMU enables fine-grained memory system control through a set of virtual-to-physical address mappings and memory attributes held in instruction and data TLBs.

The MMU features include the following:

* full support for Virtual Memory System Architecture version 7 (VMSAv7)
* separate, fully-associative, 32-entry data and instruction TLBs
* support for 32 lockable entries using the lock-by-entry model
* TLB entries that support 4KB, 64KB, 1MB, and 16MB pages
* 16 domains
* global and application-specific identifiers to prevent context switch TLB flushes
* extended permissions check capability
* round-robin replacement policy
* CP15 TLB preloading instructions to enable locking of TLB entries.


<h2 id="6.2">6.2 内存访问序列</h2>

When the processor generates a memory access, the MMU:

1. Performs a lookup for the requested virtual address and current ASID and security state in the relevant instruction or data TLB.
2. Performs a hardware translation table walk if the lookup in step 1 misses.

The MMU might not find global mapping, mapping for the currently selected ASID, or a matching NSTID for the virtual address in the TLB. The hardware does a translation table walk if the translation table walk is enabled by the PD0 or PD1 bit in the TTB Control Register. If translation table walks are disabled, the processor returns a Section Translation fault.

If the MMU finds a matching TLB entry, it uses the information in the entry as follows:

1. The access permission bits and the domain determine if the access is enabled. If the matching entry does not pass the permission checks, the MMU signals a memory abort. See the [ARM Architecture Reference Manual]() for a description of abort types and priorities, and for a description of the Instruction Fault Status Register (IFSR) and Data Fault Status Register (DFSR).
2. The memory region attributes specified in the CP15 c10 registers control the cache and write buffer, and determine if the access is secure or nonsecure, cached or noncached, and device or shared.
3. The MMU translates the virtual address to a physical address for the memory access.

If the MMU does not find a matching entry, a hardware table walk occurs.

<h3 id="6.2.1">6.2.1 TLB匹配过程</h3>

Each TLB entry contains a virtual address, a page size, a physical address, and a set of memory attributes.

A TLB entry matches when these conditions are true:

1. its virtual address matches that of the requested address
2. its NSTID matches the secure or nonsecure state of the MMU request
3. its ASID matches the current ASID or is global

The behavior of a TLB if two or more entries match at any time, including global and ASID-specific entries, is Unpredictable. The operating system must ensure that only one TLB entry matches at any time. Entries with different NSTIDs can never be hit simultaneously.

<h2 id="6.3">6.3 16MB supersection支持</h2>

处理器支持supersection，16M的内存块组成。对于物理地址的可选扩展位\[39:32\]，处理器并不支持。

图6-1 展示了supersection描述符的格式

![p6-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/p6-1.PNG)

> Each translation table entry for a supersection must be repeated 16 times in consecutive memory locations in the level 1 translation tables, and each of the 16 repeated entries must have identical translation and permission information. See the [ARM Architecture Reference Manual]() for more information.

<h2 id="6.4">6.4 MMU和内存系统的交互</h2>

可以通过CP15的c1寄存器，使能MMU，处理器会冲刷掉管道流水线上的所有指令。处理器重新预取指令，MMU根据主内存中的转换表描述符，执行虚拟地址到物理地址的映射。

通过CP15的c1寄存器禁止MMU后，处理器也会冲刷掉管道中所有的指令。处理器重新预取指令并使用直接地址映射。此时，PA=VA。

下面是使能MMU的例子：

    MRC p15, 0, r1, c1, c0, 0       ; read CP15 Register 1
    ORR r1, r1, #0x1
    MCR p15, 0, r1, c1, c0, 0       ; enable MMUs
    Fetch translated
    Fetch translated
    Fetch translated
    Fetch translated

下面是禁止MMU的例子：

    MRC p15, 0, r1, c1, c0, 0       ; read CP15 Register 1
    BIC r1, r1, #0x1
    MCR p15, 0, r1, c1, c0, 0       ; disabled
    Fetch flat
    Fetch flat
    Fetch flat
    Fetch flat

<h2 id="6.5">6.5 外部中止</h2>

外部内存错误定义为那些发生在内存系统范围之内，但是MMU又检测不到的错误。正常情况下，外部内存错误极少，并很可能对正在运行的处理器造成致命的影响。当请求传到处理器外部时，外部中止是由AXI标记的错误引起的。 通过将安全配置寄存器中的EA位置1， 可以将外部中断配置为捕获到监视器。

<h2 id="6.6">6.6 TLB锁定</h2>

正如 [ARM Architecture Reference Manual]()一文中描述的那样，TLB支持lock-by-entry模型。
The TLB supports the TLB lock-by-entry model as described in the ARM Architecture Reference Manual. CP15 preload TLB instructions support loading entries into the TLB to be locked. Any preload operation first looks in the TLB to determine if the entry hits within the TLB array. If the entry misses, a hardware translation table walk loads that entry into the TLB array. See c10, TLB Lockdown Registers on page 3-124 and c10, TLB preload operation on page 3-126 for more information.

<h2 id="6.7">6.7 MMU 软件可访问的寄存器</h2>



<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="7">7 L1级内存系统</h1>

本章描述了L1级内存系统。

<h2 id="7.1">7.1 L1级内存系统</h2>

L1级内存系统由遵循哈弗结构的独立指令和数据cache组成。主要提供下面的功能：

1. 64字节的固定Line长度
2. 支持16K或32K的cache
3. 两个具有32项的完全关联的ARMv7-A MMU
4. 具有奇偶校验的数据数组，用于错误检测
5. 虚拟索引，物理标记的Cache
6. 4路组关联缓存结构
7. 随机替换策略
8. 对于NEON代码，执行非阻塞Cache行为
9. 阻塞整数代码
10. MBIST

<h2 id="7.2">7.2 Cache组织架构</h2>

<h1 id="16">16 指令采样周期</h1>

本章描述了Cortex-A8处理器的指令周期。

<h2 id="16.1">16.1 关于指令采样周期</h2>

This chapter provides the information to estimate how much execution time particular code sequences require. The complexity of the processor makes it impossible to guarantee precise timing information with hand calculations. The timing of an instruction is often affected by other concurrent instructions, memory system activity, and additional events outside the instruction flow. Detailed descriptions of all possible instruction interactions and all possible events taking place in the processor is beyond the scope of this document. Only a cycle-accurate model of the processor can produce precise timings for a particular instruction sequence.

This chapter provides a framework for doing basic timing estimations for instruction sequences. The framework requires three main information components:

* **Instruction-specific scheduling information**

    This includes the number of micro-operations for each main instruction and the source and destination requirements for each micro-operation. The processor can issue a series of micro-operations to the execution pipeline for each ARM instruction executed. Most ARM instructions execute only one micro-operation. More complex ARM instructions such as load multiples can consist of several micro-operations.

* **Dual issue restriction criteria**

    This is the set of rules used to govern which instruction types can dual issue and under what conditions. This information is provided for dual issue of ARM instructions and NEON instructions.

* **Other pipeline-dependent latencies**

    In addition to the time taken for the scheduling and issuing of instructions, there are other sources of latencies that effect the time of a program sequence. The two most common examples are a branch mispredict and a memory system stall such as a data cache miss of a load instruction. These cases are the most difficult to predict and often must be ignored or estimated using statistical analysis techniques. Fortunately, you can ignore most of these additional latencies when creating an optimal hand scheduling for a code sequence. Hand scheduling is the most useful application of this cycle timing information.

<h2 id="16.2">16.2 Instruction-specific scheduling for ARM instructions</h2>

The tables in this section provide information for determining the best-case instruction scheduling for a sequence of instructions. The information includes:

* when source registers are required
* when destination registers are available
* which register, such as Rn or Rm, is meant for each source or destination
* the minimum number of cycles required for each instruction
* any additional instruction issue requirements or restrictions.

When a source register is required or a destination register is available depends on the availability of forwarding paths to route the required data from the correct source to the correct destination.

关于表的特殊注意事项如下所示：

* \[Rd\]作为源寄存器时，表示如果是条件指令，目的寄存器应该作为一个源寄存器。

* {} 大括号作用于源寄存器上，表示只有包含累加器操作数的指令才会要求该寄存器。

* source requirements are always given for the first cycle in a multi-cycle instruction

* destination available is always given with respect to the last cycle in a multi-cycle instruction

* multiply instructions issue to pipeline 0 only

* flags from the CPSR Register are updated internally in the E2 stage

* () (parentheses) on a destination register indicate the destination is required only if writeback is enabled.

<h2 id="16.2.1">16.2.1 Example of how to read ARM instruction tables</h2>

This section provides examples of how to read ARM instruction tables described in the chapter. See the [ARM Architecture Reference Manual]() for assembly syntax of instructions.

示例16-1 展示了如何从表16-1读取ADDEQ数据处理指令

    ADDEQ R0, R1, R2 LSL#10

This is a conditional general data-processing instruction of type shift by immediate. Source1, in this case R1, is required in E2 and Source2, in this case R2, is required in E1. Because the instruction is conditional, the destination register R0 is also required as a source register and must be available in E2. The result, stored in R0 for this case, is available in E2 for the next subsequent instruction that requires this register as a source operand. Assuming no data hazards, the instruction takes a minimum of one cycle to execute as indicated by the value in the Cycles column.

这是一个通用的条件执行指令，R1和R2是源寄存器，RO是目的寄存器。首先，将寄存器R2中的数据左移#10（立即数）位，然后与R1寄存器中的值相加，将其值存入R0。又因为该指令是条件指令，所以R0还是源寄存器，这样又可以作为下一条指令的源操作数。假设数据正常，该指令至少执行一个周期。

Example 16-2 shows how to read an SMLAL multiply instruction from Table 16-4 on page 16-8.

    SMLAL R0, R1, R2, R3

This is a multiply accumulate instruction. Source1, in this case R2, and Source2, in this case R3, are both required in E1. Because this is an accumulate multiply instruction, the result registers, R0 and R1, in this case are both required as source registers in E1. The result, stored in R0 and R1, for this case is available in E5 for the next subsequent instruction that requires one or both of these registers as a source operand. Assuming no data hazards, the instruction takes a minimum of three cycles to execute as indicated by the value in the Cycles column.

Example 16-3 shows how to read an LDR PC load instruction from Table 16-9 on page 16-11.

    LDR PC, [R13,#4]

This is a load instruction of type immediate offset. However, it is also a branch instruction because the PC is the destination. Source1, in this case R13, is required in E1. Because writeback is enabled on this load instruction, Source1, in this case R13, is also required as a result destination register for writing back the new address. This result is available in E2 for the next subsequent instruction that requires this register as a source operand. Assuming no data hazards, the instruction takes a minimum of one cycle to execute as indicated by the value in the Cycles column. To complete the timing calculation for this instruction, we use information for the branch instructions as shown in Table 16-11 on page 16-13. In this table, we can see that the instruction is unconditional, therefore no flags are required as a source in E3 for branch resolution. The Cycles column of Table 16-11 on page 16-13 indicates to add one cycle to the total execution time for all load instructions that are branches. Assuming no data hazards, the instruction takes a minimum of two cycles instead of one cycle.

<h2 id="16.2.2">16.2.2 Data-processing instructions</h2>

Data-processing instructions are divided into the following subcatagories:

* Data-processing instructions with a destination

        AND, EOR, SUB, RSB, ADD, ADC, SBC, RCSC, ORR, BIC

* Data-processing without a destination

        TST, TEQ, CMP, CMN

* Move instructions

        MOV, MVN

The data-processing instruction tables exclude cases where the PC is the destination. Branch instructions on page 16-12 describes these cases.

表16-1 显示了使用目的寄存器的数据处理指令的操作

| Shift type | Cycles | Source1 | Source2 | Source3 | Source4 | Result1 | Result2 |
| ---------- | ------ | ------- | ------- | ------- | ------- | ------- | ------- |
| Immediate  | 1      | Rn:E2   | [Rd:E2] | -       | -       | Rd:E2   | - |
| Register   | 1      | Rn:E2   | Rm:E2   | [Rd:E2] | -       | Rd:E2   | - |
| Shift by immediate, non-RRX   | 1 | Rn:E2 | Rm:E1 | [Rd:E2] | -       | Rd:E2 | - |
| Shift by immediate, RRX       | 1 | Rn:E2 | Rm:E1 | [Rd:E2] | -       | Rd:E2 | - |
| Shift by register             | 1 | Rn:E2 | Rm:E1 | Rs:E1   | [Rd:E2] | Rd:E2 | - |

Table 16-9 shows the operation of load instructions.

| 地址模式 | 周期 | 源寄存器1 |

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>



<h1 id="0">0 目录</h1>
* [1 引言](#1)
    - [1.1 关于Cortex-A8](#1.1)
    - [1.2 ARMv7-A架构](#1.2)
    - [1.3 处理器的组成](#1.3)
* [2 编程者模型](#2)
    - [2.1 关于编程者模型](#2.1)
    - [2.2 Thumb-2架构](#2.2)
    - [2.11 处理器的地址](#2.11)
    -
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

![图1-1 处理器结构](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Cortex-A8-Picture1-1.PNG)

<h3 id="1.3.1">1.3.1 指令获取</h3>

指令获取单元预测指令流，从L1 Cache中提取指令，把提取到的指令写入buffer中，以备解码流水线使用。

<h3 id="1.3.2">1.3.2 指令解码</h3>

<h2 id="1.4">1.4 处理器对外接口</h2>

* AMBA AXI接口
* APB CoreSight 接口
* ATB CoreSight 接口
* DFT接口

<h1 id="2">2 编程者模型</h1>

<h2 id="2.1">2.1 关于编程者模型</h2>

该段描述了编写功能代码所需要的基本细节，并不涉及内部微架构的细节。

ARMv7-A架构主要包括：

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

![图2-1 32位ARM Thumb2指令集的格式](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Cortex-A8-Picture2-1.PNG)

hw1（第一个半字）决定了指令的长度和功能。如果处理器解码认为是32位，则处理器会再从指令地址中取出hw2（第二个半字）。

总结起来就是，Thumb-2指令集给了平衡性能和代码大小的灵活性。比如，对于很多应用的临界循环代码，比如快速中断和DSP算法，可以使用32位的多媒体指令；对于其他应用，就可以使用更小的16位经典Thumb指令。

<h2 id="2.3">2.3 Thumb-2EE架构</h2>

Thumb-2EE是Thumb-2架构的一个变体。

<h2 id="2.11">2.11 处理器的地址</h2>

Cortex-A8系统中具有3种不同类型的地址：

* 虚拟地址（VA）
* 修正后的虚拟地址（MVA）
* 物理地址（PA）

其地址类型和ARM920T的地址类型相似，只是因为Cortex-A8处理器采用了ARMv7-A架构，从而引入了TrustZone技术。导致处理器核可能处于2种状态：安全状态和非安全状态。所以，转译表也分为安全转译表和非安全转译表。

表2-7 处理器中的地址类型

| 处理器地址 | Cache |

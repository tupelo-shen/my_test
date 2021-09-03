[TOC]

## 1 虚拟化

虚拟化扩展是`ARMv7-A`架构的一个可选项。基于该虚拟化扩展的实现都必须包含`安全扩展`、`大物理地址扩展`和`多处理器扩展`。

在实现时，虚拟化扩展提供了一组硬件特性，支持对`VMSAv7`实现的非安全状态进行虚拟化。虚拟化系统的基本模型包括:

* `hypervisor`，运行在非安全的`Hyp`模式下，负责切换客户机操作系统；

* 一定数量的客户机操作系统，它们都运行在非安全的`PL1`和`PL0`模式下；

* 对于每个客户机操作系统来说，应用程序都运行在`User`模式下；

    > 运行在虚机上的OS，它认为自己就是运行在一个真正的ARM处理器上，而不会意识到实际运行在一个虚机上，也不会意识到其它虚机的存在。

虚拟化的另一种描述就是：客户机OS，包括运行在OS上的应用和任务，都运行在一个虚拟机上；而底层，有一个`hypervisor`负责虚拟机的调度切换。

为了识别虚拟机，`hypervisor`赋予每个虚拟机一个标识符，称为`virtual machine identifier`（`VMID`）。

为了实现虚拟化扩展，对ARM架构的许多特性进行了扩展。这些关键特性包括：

* 为了客户机OS的管理，`Hyp`模式只在非安全状态中实现。`Hyp`模式运行在自己的非安全虚拟地址空间中，与`PL0`和`PL1`模式下的虚拟地址空间不同。

* 虚拟机扩展需要提供控制：

    - 为少量标识寄存器提供虚拟值。当客户机OS或其应用程序读取标识寄存器的时候，返回这些虚拟值；
    - 能够陷入到其它各种操作中，包括访问寄存器、内存管理等。这种捕获会在`Hyp`模式下产生一个异常。

    这些控制都是运行在`Hyp`模式下的软件配置的，也就是`hypervisor`。

* 通过安全扩展，虚拟化扩展将中断、异步的数据`Abort`路由到正确的地方，比如：

    - 当前的客户机OS
    - 当前没有运行的客户机OS
    - `hypervisor`
    - 安全监控软件（`monitor`）

* 如果实现中包含虚拟化扩展，需要为内存访问提供独立的转换机制：

    - 安全模式，提供安全的`PL1&0`转换机制；
    - 非安全`hyp`模式，提供非安全PL2转换机制；
    - 非安全PL1和PL0模式，提供非安全PL1&0转换机制。

    关于各种模式下的转换机制可以参考[ARMv7_A&R_技术参考手册_B_3_1_虚拟存储系统架构-VMSA]中的内容。

* 在非安全的PL1&0转换机制中，地址转换分为两个阶段：

    - 阶段1：映射虚拟地址（VA）到中间物理地址（IPA）。通常，`Guest OS`配置和控制这个阶段，并且相信IPA就是真实的物理地址。
    - 阶段2：将IPA映射到PA。通常，`hypervisor`控制这个阶段，`Guest OS`完全意识不到这个地址转换的存在。

    关于更多地址转换的内容，可以参考[ARMv7_A&R_技术参考手册_B_3_1_虚拟存储系统架构-VMSA]一文。

## 2 虚拟化扩展对工作模式和异常模型的影响

本段主要讨论虚拟化扩展对工作模式和异常模型的影响。一个包含虚拟化扩展的实现，应该：

* 实现一个新的非安全模式：`Hyp`模式。在[ARMv7_A&R_技术参考手册_B_1_3_ARM工作模式和核心寄存器]()一文中，总结了`hyp`模式与其它工作模式的不同之处。

* 实现新的异常，包括：`Hypervisor Call`异常、`Hyp Trap`异常、虚拟IRQ异常、虚拟FIQ异常、虚拟Abort异常。HVC异常和`Hyp Trap`异常被传递给`hyp`模式。虚拟异常被传递给非安全IRQ、FIQ或Abort模式。通俗的讲，虚拟异常就是`hypervisor`给`Guest OS`传递异常信号使用的。

* 实现一个新寄存器`HVBAR`，保存异常向量表的基地址。异常向量表是`hyp`模式的向量表。

* Provides controls that can be used to route IRQs, FIQs, and asynchronous aborts, to Hyp mode. This is possible only if Secure software has not routed the exception to Monitor mode, and applies only to exceptions taken from a Non-secure mode.

    For more information see Asynchronous exception routing controls on page B1-1175.

* Provides controls that can be used to route some synchronous exceptions, taken from Non-secure modes, to Hyp mode. For more information see Routing general exceptions to Hyp mode on page B1-1192 and Routing Debug exceptions to Hyp mode on page B1-1194.

* Provide mechanisms to trap processor functions to Hyp mode, using the Hyp Trap exception, see Traps to the hypervisor on page B1-1248.

    When an operation is trapped to Hyp mode, the hypervisor typically either:

    - emulates the required operation, so the application running in the Guest OS is unaware of the trap to Hyp mode
    - returns an error to the Guest OS.

* Implements enhanced exception reporting for exceptions taken to Hyp mode, see Reporting exceptions taken to the Non-secure PL2 mode on page B3-1420. These exceptions are reported using the HSR, see Use of the HSR on page B3-1424,

* Implements a new exception return instruction, ERET, for return from Hyp mode. For more information see Hyp mode on page B1-1141.

## 3 虚拟异常

虚拟扩展引入了3个虚拟异常：

* 虚拟`IRQ`异常，对应物理`IRQ`异常；
* 虚拟`FIQ`异常，对应物理`FIQ`异常；
* 虚拟`Abort`异常，对应物理异步外部数据`abort`异常。

运行在`hyp`模式下的代码可以使用这些虚拟异常给其它非安全模式发送信号。非安全`PL1`或`PL0`模式不会区分出是虚拟异常还是真实的物理异常。

虚拟异常的使用场景之一就是：运行在非安全PL1或PL0模式下的处理器，发生物理IRQ、FIQ或者异步中止（`abort`）时，将其路由给`hyp`模式。`hyp`模式下的异常处理程序决定是直接在`hyp`模式下处理该异常，还是将其转给`Guest OS`进行处理。如果异常请求由`Guest OS`进行处理，那么将会为该`Guest OS`挂起该中断标志位。当`hypervisor`切换到那个`Guest OS`后，它会使用正确的虚拟异常，发送一个挂起的虚拟异常给那个`Guest OS`。

关于更多虚拟化扩展中的虚拟异常，可以参考[ARMv7_A&R_技术参考手册_B_1_8_异常处理]()中的虚拟异常部分。
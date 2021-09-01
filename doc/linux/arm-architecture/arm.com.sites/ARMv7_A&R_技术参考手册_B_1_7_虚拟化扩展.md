[TOC]

## 1 虚拟化

虚拟化扩展是`ARMv7-A`架构的一个可选项。基于该虚拟化扩展的实现都必须包含`安全扩展`、`大物理地址扩展`和`多处理器扩展`。

在实现时，虚拟化扩展提供了一组硬件特性，支持对`VMSAv7`实现的非安全状态进行虚拟化。虚拟化系统的基本模型包括:

* `hypervisor`，运行在非安全的`Hyp`模式下，负责切换客户机操作系统；

* 一定数量的客户机操作系统，它们都运行在非安全的`PL1`和`PL0`模式下；

* 对于每个客户机操作系统来说，应用程序都运行在`User`模式下；

    > 运行在虚机上的OS，它认为自己就是运行在一个真正的ARM处理器上，而不会意识到实际运行在一个虚机上，也不会意识到其它虚机的存在。

虚拟化的另一种描述就是：

* 客户机OS，包括运行在OS之下的应用和任务，都运行在一个虚拟机上；
* 一个`hypervisor`负责虚拟机的调度切换。

`hypervisor`赋予每个虚拟机一个标识符，称为`virtual machine identifier`（`VMID`）。

为了实现虚拟化扩展，对ARM架构的许多特性进行了扩展。这些关键特性包括：

* 为了客户机OS的管理，`Hyp`模式只在非安全状态中实现。`Hyp`模式运行在自己的非安全虚拟地址空间中，与`PL0`和`PL1`模式下的虚拟地址空间不同。

* 虚拟机扩展需要提供控制：

    - 为少量标识寄存器提供虚拟值。当客户机OS或其应用程序读取标识寄存器的时候，返回这些虚拟值；
    - 能够陷入到其它各种操作中，包括访问其它寄存器，或内存管理操作。这种被捕获操作就会在`Hyp`模式下产生一个异常。

    这些控制都是运行在`Hyp`模式下的软件配置的，也就是`hypervisor`。

* 通过安全扩展，虚拟化扩展控制中断和异步数据`Abort`异常路由到正确的接收方，比如：

    - 当前的客户机OS
    - 当前没有运行的客户机OS
    - `hypervisor`
    - 安全监控软件（`monitor`）

* When an implementation includes the Virtualization Extensions, it provides independent translation regimes for memory accesses from:

    - Secure modes, the Secure PL1&0 translation regime
    - Non-secure Hyp mode, the Non-secure PL2 translation regime
    - Non-secure PL1 and PL0 modes, the Non-secure PL1&0 translation regime.

    Figure B3-1 on page B3-1309 shows these translation regimes.

* In the Non-secure PL1&0 translation regime, address translation occurs in two stages:

    - Stage 1 maps the Virtual Address (VA) to an Intermediate Physical Address (IPA). Typically, the Guest OS configures and controls this stage, and believes that the IPA is the Physical Address (PA)
    - Stage 2 maps the IPA to the PA. Typically, the hypervisor controls this stage, and a Guest OS is completely unaware of this translation.

    For more information, see About address translation on page B3-1311.

Impact of the Virtualization Extensions on the modes and exception model gives more information about many of these features.

## 1 虚拟化扩展对工作模式和异常模型的影响

This section summarizes the effect of the Virtualization Extensions on the modes and exception model. An implementation that includes the Virtualization Extensions:

* 实现一个新的非安全模式：`Hyp`模式。在[ARMv7_A&R_技术参考手册_B_1_3_ARM工作模式和核心寄存器]()一文中，总结了`hyp`模式与其它工作模式的不同之处。

* 需要实现新的异常，包括：

    - Hypervisor Call (HVC) exception on page B1-1212
    - Hyp Trap exception on page B1-1209
    - Virtual IRQ exception on page B1-1221
    - Virtual FIQ exception on page B1-1223
    - Virtual Abort exception on page B1-1218.

    The Hypervisor Call and Hyp Trap exceptions are always taken to Hyp mode. The virtual exceptions are taken to Non-secure IRQ, FIQ, or Abort mode, see The virtual exceptions on page B1-1164.

* Implements a new register that holds the exception vector base address for exceptions taken to Hyp mode, the HVBAR.

* Provides controls that can be used to route IRQs, FIQs, and asynchronous aborts, to Hyp mode. This is possible only if Secure software has not routed the exception to Monitor mode, and applies only to exceptions taken from a Non-secure mode.

    For more information see Asynchronous exception routing controls on page B1-1175.

* Provides controls that can be used to route some synchronous exceptions, taken from Non-secure modes, to Hyp mode. For more information see Routing general exceptions to Hyp mode on page B1-1192 and Routing Debug exceptions to Hyp mode on page B1-1194.

* Provide mechanisms to trap processor functions to Hyp mode, using the Hyp Trap exception, see Traps to the hypervisor on page B1-1248.

    When an operation is trapped to Hyp mode, the hypervisor typically either:

    - emulates the required operation, so the application running in the Guest OS is unaware of the trap to Hyp mode
    - returns an error to the Guest OS.

* Implements enhanced exception reporting for exceptions taken to Hyp mode, see Reporting exceptions taken to the Non-secure PL2 mode on page B3-1420. These exceptions are reported using the HSR, see Use of the HSR on page B3-1424,

* Implements a new exception return instruction, ERET, for return from Hyp mode. For more information see Hyp mode on page B1-1141.

## 2 虚拟异常

虚拟扩展引入了3个虚拟异常：

* 虚拟IRQ异常，对应物理IRQ异常；
* 虚拟FIQ异常，对应物理FIQ异常；
* 虚拟Abort异常，对应物理异步外部数据abort异常。

Software executing in Hyp mode can use these to signal exceptions to the other Non-secure modes. A Non-secure PL1 or PL0 mode cannot distinguish a virtual exception from the corresponding physical exception.

A usage model for these exceptions is that physical IRQs, FIQs and asynchronous aborts that occur when the processor is in a Non-secure PL1 or PL0 mode are routed to Hyp mode. The exception handler, executing in Hyp mode, determines whether the exception can be handled in Hyp mode or requires routing to a Guest OS. When an exception requires handling by a Guest OS it is marked as pending for that Guest OS. When the hypervisor switches to a particular Guest OS, it uses the appropriate virtual exception to signal any pending virtual exception to that Guest OS.

For more information see Virtual exceptions in the Virtualization Extensions on page B1-1197.

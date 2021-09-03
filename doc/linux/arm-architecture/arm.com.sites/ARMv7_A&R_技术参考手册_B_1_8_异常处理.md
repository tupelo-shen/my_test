异常是必要的，它可以打断处理器的执行，而去处理某个事件，比如，外部产生的中断或者内部尝试执行一个为定义的指令。从异常的来源看，异常分为同步和异步。

通常，当发生异常时，在处理异常之前，处理器状态会立即保留。这意味着，当异常事件被处理后，可以恢复原来的状态，并从发生异常的地方恢复程序执行。

异常可能会并发产生，也可能会嵌套产生。

本文的内容包含：

[TOC]

## 1 异常向量表和异常基地址

When an exception is taken, processor execution is forced to an address that corresponds to the type of exception. This address is called the exception vector for that exception.

A set of exception vectors comprises eight consecutive word-aligned memory addresses, starting at an exception base address. These eight vectors form a `vector table`. For the IRQ and FIQ exceptions only, when the exceptions are taken to IRQ mode and FIQ mode, software can change the exception vectors from the vector table values by setting the `SCTLR.VE` bit to `1`, see `Vectored interrupt` support on page B1-1168.

The number of possible exception base addresses, and therefore the number of vector tables, depends on the implemented architecture profile and extensions, as follows:

* 实现不包含安全扩展时：

    This section applied to all ARMv7-R implementations.

    An implementation that does not include the Security Extensions has a single vector table, the base address of which is selected by SCTLR.V, see SCTLR, System Control Register, VMSA on page B4-1707 or SCTLR, System Control Register, PMSA on page B6-1932:

    - **V == 0**
    
        异常基地址 `= 0x00000000`。这称为正常向量表，或低向量表。

    - **V == 1**
    
        异常基地址 `= 0xFFFF0000`。这称为高向量表，或者`Hivecs`。

    > ARM deprecates using the Hivecs setting, SCTLR.V == 1, in ARMv7-R. ARM recommends that Hivecs is used only in ARMv7-A implementations.

* 实现包含安全扩展时：

    如果包含安全扩展，则应该包含下面的向量表：

    - One for exceptions taken to Secure Monitor mode. This is the Monitor vector table, and is in the address space of the Secure PL1&0 translation regime.

    - One for exceptions taken to Secure PL1 modes other than Monitor mode. This is the Secure vector table, and is in the address space of the Secure PL1&0 translation regime.

    - One for exceptions taken to Non-secure PL1 modes. This is the Non-secure vector table, and is in the address space of the Non-secure PL1&0 translation regime.

    For the Monitor vector table, MVBAR holds the exception base address.

    For the Secure vector table:

    - the Secure SCTLR.V bit determines the exception base address:
        
        + V == 0    The Secure VBAR holds the exception base address.
        + V == 1    Exception base address = 0xFFFF0000, the Hivecs setting.

    For the Non-secure vector table:
    
    - the Non-secure SCTLR.V bit determines the exception base address:
        + V == 0    The Non-secure VBAR holds the exception base address.
        + V == 1    Exception base address = 0xFFFF0000, the Hivecs setting.

* 实现包含虚拟化扩展时：

    An implementation that includes the Virtualization Extensions must include the Security Extensions, and also includes an additional vector table. Therefore, it has the following vector tables:

    - One for exceptions taken to Secure Monitor mode. This is the Monitor vector table, and is in the address space of the Secure PL1&0 translation regime.
    - One for exceptions taken to Secure PL1 modes other than Monitor mode. This is the Secure vector table, and is in the address space of the Secure PL1&0 translation regime.
    - One for exceptions taken to Hyp mode, the Non-secure PL2 mode. This is the Hyp vector table, and is in the address space of the Non-secure PL2 translation regime.
    - One for exceptions taken to Non-secure PL1 modes. This is the Non-secure vector table, and is in the address space of the Non-secure PL1&0 translation regime.

    `Monitor`向量表、安全向量表和非安全向量表的异常基址的确定方法与包含安全扩展但不包含虚拟化扩展的实现相同。

    对于`Hyp`向量表，`HVBAR`寄存器保存异常基地址。

The following subsections give more information:

* The vector tables and exception offsets
* Vectored interrupt support on page B1-1168
* Pseudocode determination of the exception base address on page B1-1168.

#### 1.2 向量表和异常偏移

表B1-3定义了向量表中的表项。在表中：

* `hyp`列定义了进入`hyp`模式的异常向量；
* `Monitor`列定义了进入`Monitor`模式的异常向量；
* `secure`和`Non-secure`分别定义了安全和非安全模式的向量表。它们是用来进入PL1模式的向量表，而不是所谓的Monitor模式。表B1-4 每个异常发生时，应该进入的异常模式。

For more information about determining the mode to which an exception is taken, see Processor mode for taking
exceptions on page B1-1173.

The Virtualization Extensions provide a number of additional exceptions, some of which are not shown explicitly
in the vector tables. For more information, see Offsets of exceptions introduced by the Virtualization Extensions.

表B1-3 向量表

| offset | hyp       | monitor    | secure     | Non-secure |
| ------ | ----------| ---------- | ---------- | ---------- |
| 0x00   | 未使用    | 未使用     | 复位       | 复位       |
| 0x04   | 未定义指令| 未使用     | 未定义指令 | 未定义指令 |
| 0x08   | HVC调用   | SMC调用    | SVC调用    | SVC调用    |
| 0x0C   | 预取abort | 预取abort  | 预取abort  | 预取abort  |
| 0x10   | 数据abort | 数据abort  | 数据abort  | 数据abort  |
| 0x14   | Hyp陷阱   | 未使用     | 未使用     | 未使用     |
| 0x18   | IRQ中断   | IRQ中断    | IRQ中断    | IRQ中断    |
| 0x1C   | FIQ中断   | FIQ中断    | FIQ中断    | FIQ中断    |

> HVC = Hypervisor Call
> 
> SMC = Secure Monitor Call
> 
> SVC = Supervisor Call

表B1-4 安全或非安全向量表下，异常进入的模式

| exception  | PL1 Mode taken to |
| ---------- | ----------------- |
| 复位       | Supervisor        |
| 未定义指令 | Undefined         |
| SVC调用    | Supervisor        |
| 预取abort  | Abort             |
| 数据abort  | Abort             |
| IRQ中断    | IRQ               |
| FIQ中断    | FIQ               |

For more information about use of the vector tables see Overview of exception entry on page B1-1171.

#### 1.3 Offsets of exceptions introduced by the Virtualization Extensions

The Virtualization Extensions introduce the following new exceptions. The processor enters the handlers for these exceptions using the following vector table entries shown in Table B1-3:

* Hypervisor Call

    If taken from Hyp mode, shown explicitly in the Hyp mode vector table. Otherwise, see Use of offset 0x14 in the Hyp vector table on page B1-1168.

* Hyp Trap

    Shown explicitly in the Hyp mode vector table.

* Virtual Abort

    Entered through the Data Abort vector in the Non-secure vector table.

* Virtual IRQ

    Entered through the IRQ vector in the Non-secure vector table.

* Virtual FIQ    

    Entered through the FIQ vector in the Non-secure vector table.


#### 1.4 Use of offset 0x14 in the Hyp vector table

The vector at offset 0x14 in the Hyp vector table is used for exceptions that cause entry to Hyp mode. This means it
is:

* Always used for the Hyp Trap exception.

* Used for the following exceptions, when the exception is not taken from Hyp mode:

    - Hypervisor Call
    - Supervisor Call, when caused by execution of an SVC instruction in Non-secure User mode when HCR.TGE is set to 1
    - Undefined Instruction
    - Prefetch Abort
    - Data Abort.

    Table B1-3 on page B1-1167 shows the offsets used for these exceptions when they are taken from Hyp mode.

* Never used for IRQ exceptions, Virtual IRQ exceptions, FIQ exceptions, or Virtual FIQ exceptions.

For more information, see Processor mode for taking exceptions on page B1-1173.

#### 1.5 Pseudocode determination of the exception base address

For an exception taken to a PL1 mode other than Monitor mode, the ExcVectorBase() function determines the exception base address:

```c
// ExcVectorBase()
// ===============

bits(32) ExcVectorBase()
    if SCTLR.V == '1' then // Hivecs selected, base = 0xFFFF0000
        return Ones(16):Zeros(16);
    elsif HaveSecurityExt() then
        return VBAR;
    else
        return Zeros(32);
```

#### 1.6 Vectored interrupt support

At reset, any implemented vectored interrupt mechanism is disabled, and the IRQ and FIQ exception vectors are at fixed offsets from the exception base address that is being used. With this configuration, an FIQ or IRQ handler typically starts with an instruction sequence that determines the cause of the interrupt and then branches to an appropriate routine to handle it.

If an implementation supports vectored interrupts, enabling this feature means an interrupt controller can prioritize interrupts and provide the address of the required interrupt handler directly to the processor, for use as the  interrupt vector. For interrupts taken to PL1 modes other than Monitor mode, vectored interrupt behavior is enabled by setting the SCTLR.VE bit to 1, see either:

* SCTLR, System Control Register, VMSA on page B4-1707
* SCTLR, System Control Register, PMSA on page B6-1932.

The hardware that supports vectored interrupts is IMPLEMENTATION DEFINED, and an implementation might not include any support for this operation.

In an implementation that includes the Security Extensions:

* The SCTLR.VE bit is Banked between Secure and Non-secure states to provide independent control of whether vectored interrupt support is enabled.

* Interrupts can be routed to Monitor mode, by setting either or both of the SCR.IRQ and SCR.FIQ bits to 1. When an interrupt is routed to Monitor mode it uses the vector in the vector table at the Monitor exception base address held in MVBAR, regardless of the value of either Banked copy of the SCTLR.VE bit.

The Virtualization Extensions do not support this vectoring of the IRQ and FIQ exceptions when these exceptions are routed to Hyp mode. When an interrupt is routed to Hyp mode, it uses the vector in the vector table at the Hyp exception base address held in HVBAR, regardless of the value of either Banked copy of the SCTLR.VE bit.

From the introduction of the Virtualization Extensions, ARM deprecates any use of the SCTLR.VE bit.

## 2 异常优先级

## 3 异常概述

On taking an exception:

1. The hardware determines the mode to which the exception must be taken, see Processor mode for taking exceptions on page B1-1173.

2. A link value, indicating the preferred return address for the exception, is saved. This is a possible return address for the exception handler, and depends on:

    * the exception type
    * whether the exception is taken to a PL1 mode or a PL2 mode
    * for some exceptions taken to a PL1 mode, the instruction set state when the exception is taken.

    Where the link value is saved depends on whether the exception is taken to a PL1 mode or a PL2 mode.

    For more information see Link values saved on exception entry on page B1-1172.

3. The value of the CPSR is saved in the SPSR for the mode to which the exception must be taken. The value saved in SPSR.IT[7:0] is always correct for the preferred return address.

4. In an implementation that includes the Security Exceptions:

    * if the exception taken from Monitor mode, SCR.NS is cleared to 0
    * otherwise, taking the exception leaves SCR.NS unchanged.

5. The CPSR is updated with new context information for the exception handler. This includes:

    * Setting CPSR.M to the processor mode to which the exception is taken.
    * Setting the appropriate CPSR mask bits. This can disable the corresponding exceptions, preventing uncontrolled nesting of exception handlers.
    * Setting the instruction set state to the state required for exception entry.
    * Setting the endianness to the required value for exception entry.
    * Clearing the CPSR.IT[7:0] bits to 0.

    For more information, see Processor state on exception entry on page B1-1182.

6. The appropriate exception vector is loaded into the PC, see Exception vectors and the exception base address on page B1-1165.

7. Execution continues from the address held in the PC.

For an exception taken to a PL1 mode, on exception entry, the exception handler can use the SRS instruction to store
the return state onto the stack of any mode at the same privilege level, and the CPS instruction to change mode. For
more information about the instructions, see SRS (Thumb) on page B9-2004, SRS (ARM) on page B9-2006, CPS
(Thumb) on page B9-1978, and CPS (ARM) on page B9-1980.

Later sections of this chapter describe each of the possible exceptions, and each of these descriptions includes a
pseudocode description of the processor state changes when it takes that exception. Table B1-5 gives an index to
these descriptions:

## 3.1 Link values saved on exception entry

On exception entry, a link value for use on return from the exception, is saved. This link value is based on the preferred return address for the exception, as shown in Table B1-6:

Table B1-6 Exception return addresses

| 异常           | 首选返回地址             | 模式  |
| -------------- | ------------------------ | --------- |
| 未定义指令     | 未定义指令的地址         | 
| SVC            | SVC指令之后的指令地址    | 
| SMC            | SMC指令之后的指令地址    | 
| HVC            | HVC指令之后的指令地址    |
| 预取Abort      | abort的预取指令的地址    |
| 数据Abort      | 产生abort的指令地址      |
| 虚拟Abort      | 要执行的下一条指令的地址 |
| Hyp Trap       | 陷入指令的地址           |
| IRQ或FIQ       | 要执行的下一条指令的地址 |
| 虚拟IRQ或FIQ   | 要执行的下一条指令的地址 |

## 4 异常模式

## 5 异常时的处理器状态

## 6 异步异常的屏蔽

## 7 异步异常的总结

## 8 路由异常到hyp模式

## 9 路由debug异常到hyp模式

## 10 异常返回

In the ARM architecture, exception return requires the simultaneous restoration of the PC and CPSR to values that are consistent with the desired state of execution on returning from the exception. Typically, exception return involves returning to one of:

* the instruction after the instruction boundary at which an asynchronous exception was taken（异步信号导致的异常）

* the instruction following an SVC, SMC, or HMC instruction, for an exception generated by one of those instructions（系统调用指令导致的异常）

* the instruction that caused the exception, after the reason for the exception has been removed（同步异常信号）

* the subsequent instruction, if the instruction that caused the exception has been emulated in the exception handler（模拟异常）

The ARM architecture defines a preferred return address for each exception other than Reset, see Link values saved on exception entry on page B1-1172. The values of the SPSR.IT[7:0] bits generated on exception entry are always correct for this preferred return address, but might require adjustment by the exception handler if returning elsewhere.

In some cases, to calculate the appropriate preferred return address, a subtraction must be performed on the link value saved on taking the exception. The description of each exception includes any value that must be subtracted from the link value, and other information about the required exception return.


## 11 虚拟化扩展中的虚拟异常

## 12 低中断延迟配置

## 13 事件-event

## 14 等待中断


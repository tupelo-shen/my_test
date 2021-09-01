[TOC]

The following sections describe the ARM processor modes and the ARM core registers:

[TOC]

## 1 工作模式

Table B1-1 shows the processor modes defined by the ARM architecture. In this table:

* the Processor mode column gives the name of each mode and the abbreviation used, for example, in the ARM core register name suffixes used in ARM core registers on page B1-1143
* the Privilege level column gives the privilege level of software executing in that mode, see Privilege level on page B1-1136
* the Encoding column gives the corresponding CPSR.M field
* the Security state column applies only to processors that implement the Security Extensions.

Table B1-1 ARM processor modes

| 模式           | 编码     | 特权级别        | 是否实现    | 安全态         |
| -------------- | -------- | --------------- | ----------- | -------------- |
| User      <br>usr | 10000 | PL0             | Always      | Both           |
| FIQ       <br>fiq | 10001 | PL1             | Always      | Both           |
| IRQ       <br>irq | 10010 | PL1             | Always      | Both           |
| Supervisor<br>svc | 10011 | PL1             | Always      | Both           |
| Monitor   <br>mon | 10110 | PL1             | 安全扩展    | Secure only    |
| Abort     <br>abt | 10111 | PL1             | Always      | Both           |
| Hyp       <br>hyp | 11010 | PL2             | 虚拟化扩展  | Non-secure only|
| Undefined <br>und | 11011 | PL1             | Always      | Both           |
| System    <br>sys | 11111 | PL1             | Always      | Both           |

Mode changes can be made under software control, or can be caused by an external or internal exception.

#### 1.1 ARM处理器工作模式的注意事项

* User mode

    An operating system runs applications in User mode to restrict the use of system resources. Software executing in User mode executes at PL0. Execution in User mode is sometimes described as unprivileged execution. Application programs normally execute in User mode, and any program executed in User mode:

    - makes only unprivileged accesses to system resources, meaning it cannot access protected system resources

    - makes only unprivileged access to memory

    - cannot change mode except by causing an exception, see Exception handling on page B1-1165.

* System mode

    Software executing in System mode executes at PL1. System mode has the same registers available as User mode, and is not entered by any exception.

* Supervisor mode

    Supervisor mode is the default mode to which a Supervisor Call exception is taken.

    Executing a SVC (Supervisor Call) instruction generates an Supervisor Call exception, that is taken to Supervisor mode.

    A processor enters Supervisor mode on Reset.

* Abort mode

    Abort mode is the default mode to which a Data Abort exception or Prefetch Abort exception is taken.

* Undefined mode

    Undefined mode is the default mode to which an instruction-related exception, including any attempt to execute an UNDEFINED instruction, is taken.

* FIQ mode

    FIQ mode is the default mode to which an FIQ interrupt is taken.

* IRQ mode

    IRQ mode is the default mode to which an IRQ interrupt is taken.

* Hyp mode

    Hyp mode is the Non-secure PL2 mode, implemented as part of the Virtualization Extensions. Hyp mode is entered on taking an exception from Non-secure state that must be taken to PL2

    The Hypervisor Call exception and Hyp Trap exception are exceptions that are implemented as part of the Virtualization Extensions, and that are always taken in Hyp mode.

    > This means that Hypervisor Call exceptions and Hyp Trap exceptions cannot be taken from Secure state.

    In a Non-secure PL1 mode, executing a HVC (Hypervisor Call) instruction generates a Hypervisor Call exception.

    For more information, see Hyp mode on page B1-1141.

* Monitor mode

    Monitor mode is the mode to which a Secure Monitor Call exception is taken.

    In a PL1 mode, executing an SMC (Secure Monitor Call) instruction generates a Secure Monitor Call exception.

    Monitor mode is a Secure mode, meaning it is always in the Secure state, regardless of the value of the `SCR.NS` bit.

    Software running in Monitor mode has access to both the Secure and Non-secure copies of system registers. This means Monitor mode provides the normal method of changing between the Secure and Non-secure security states.

    > It is important to distinguish between:
    > 
    > * **Monitor mode**
    > 
    >   This is a processor mode that is only available when an implementation includes the Security Extensions. It is used in normal operation, as a mechanism to transfer between Secure and Non-secure state, as described in this section.
    > 
    > * **Monitor debug-mode**
    > 
    >   This is a debug mode and is available regardless of whether the implementation includes the Security Extensions. For more information, see About the ARM Debug architecture on page C1-2023.

    Monitor mode is implemented only as part of the Security Extensions. For more information, see The Security Extensions on page B1-1156.

* Secure and Non-secure modes

    In a processor that implements the Security Extensions, most mode names can be qualified as Secure or Non-secure, to indicate whether the processor is also in Secure state or Non-secure state.

    For example:

    - if a processor is in Supervisor mode and Secure state, it is in Secure Supervisor mode
    - if a processor is in User mode and Non-secure state, it is in Non-secure User mode.

    > As indicated in the appropriate Mode descriptions:
    > 
    > * Monitor mode is a Secure mode, meaning it is always in the Secure state
    > * Hyp mode is a Non-secure mode, meaning it is accessible only in Non-secure state.

Figure B1-1 shows the modes, privilege levels, and security states, for an implementation that includes the Security Extensions and the Virtualization Extensions.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARMv7_A_picture_B_1_1.PNG">

#### 1.2 Hyp模式

Hyp mode is a Non-secure mode, implemented only as part of the Virtualization Extensions. It provides the usual method of controlling almost all of the functionality of the Virtualization Extensions.

> The alternative method of controlling this functionality is by accessing the Hyp mode controls from Secure Monitor mode, with the `SCR.NS` bit set to 1.

This section summarizes how Hyp mode differs from the other modes, and references where the features of Hyp mode are described in more detail:

* Software executing in Hyp mode executes at PL2, see Mode, state, and privilege level on page B1-1135.

* Hyp mode is accessible only in Non-secure state. When the processor is in Secure state, setting `CPSR.M` to `0b11010`, the encoding for Hyp mode, has no meaning. Therefore, in Secure state, the effect of attempting to set `CPSR.M` to `0b11010` is `UNPREDICTABLE`. For more information see The Current Program Status Register (`CPSR`) on page B1-1147.

* In Non-debug state, the only mechanisms for changing to Hyp mode are:

    - an exception taken from a Non-secure PL1 or PL0 mode
    - an exception return from Secure Monitor mode.

* In Hyp mode, the only exception return is execution of an `ERET` instruction, see ERET on page B9-1982.

* In Hyp mode, the CPACR has no effect on the execution of coprocessor, floating-point, or Advanced SIMD instructions. The HCPTR controls execution of these instructions in Hyp mode.

* If software running in Hyp mode executes an SVC instruction, the Supervisor Call exception generated by the instruction is taken to Hyp mode, see SVC (previously SWI) on page A8-720.

* The effect of an exception return with the restored CPSR specifying Hyp mode is UNPREDICTABLE if either:

    - SCR.NS is set to 0
    - the return is from a Non-secure PL1 mode.

* The instructions described in the following sections are UNDEFINED if executed in Hyp mode:

    - SRS (Thumb) on page B9-2004
    - SRS (ARM) on page B9-2006
    - RFE on page B9-2000
    - LDM (exception return) on page B9-1986
    - LDM (User registers) on page B9-1988
    - STM (User registers) on page B9-2008
    - SUBS PC, LR and related instructions (ARM) on page B9-2012
    - SUBS PC, LR (Thumb) on page B9-2010, when executed with a nonzero constant.

    > In Thumb state, `ERET` is encoded as `SUBS PC, LR, #0`, and therefore this is a valid instruction.

* The unprivileged Load unprivileged and Store unprivileged instructions LDRT, LDRSHT, LDRHT, LDRBT, STRT,
STRHT, and STRBT, are UNPREDICTABLE if executed in Hyp mode.

复位上电后，非安全`PL1`模式下，`HVC`指令未定义，也就是说，默认情况下，`hyp`模式被禁用。为了允许使用`HVC`指令进入`hyp`模式，安全软件必须通过设置`SCR.HCE`标志位为1使能该指令。如果在`hyp`模式下，设置`SCR.HCE`为0，则`HVC`指令是不可预测的(`UNPREDICTABLE`)。

#### 1.3 判断工作模式的伪代码

The BadMode() function tests whether a 5-bit mode number corresponds to one of the permitted modes:

```c
// BadMode()
// =========
boolean BadMode(bits(5) mode)
    case mode of
        when '10000' result = FALSE;                // User mode
        when '10001' result = FALSE;                // FIQ mode
        when '10010' result = FALSE;                // IRQ mode
        when '10011' result = FALSE;                // Supervisor mode
        when '10110' result = !HaveSecurityExt();   // Monitor mode
        when '10111' result = FALSE;                // Abort mode
        when '11010' result = !HaveVirtExt();       // Hyp mode
        when '11011' result = FALSE;                // Undefined mode
        when '11111' result = FALSE;                // System mode
        otherwise result = TRUE;
    return result;
```

The following pseudocode functions provide information about the current mode:

```c
// CurrentModeIsNotUser()
// ======================
boolean CurrentModeIsNotUser()
    if BadMode(CPSR.M) then UNPREDICTABLE;
    if CPSR.M == '10000' then return FALSE; // User mode
    return TRUE;                            // Other modes

// CurrentModeIsUserOrSystem()
// ===========================
boolean CurrentModeIsUserOrSystem()
    if BadMode(CPSR.M) then UNPREDICTABLE;
    if CPSR.M == '10000' then return TRUE;  // User mode
    if CPSR.M == '11111' then return TRUE;  // System mode
    return FALSE;                           // Other modes

// CurrentModeIsHyp()
// ==================
boolean CurrentModeIsHyp()
    if BadMode(CPSR.M) then UNPREDICTABLE;
    if CPSR.M == '11010' then return TRUE;  // Hyp mode
    return FALSE;                           // Other modes
```

## 2 ARM核寄存器

> 什么是`Banked registers`?
> 
> 1. ARMv8文档描述：
>   
>   A register that has multiple instances, with the instance that is in use depending on the PE mode, Security state, or other PE state
>   
>   翻译成中文就是：有多个实例的寄存器，具体使用哪个实例，依赖于PE模式、安全状态或其它PE状态。
>   
> 2. stackoverflow描述：
> 
>   In ARM there is a concept of Banked Register. While reading many questions and their answer and various other resources about what is Banked mean here. Then I got this definition: Register banking refers to providing multiple copies of a register at the same address. Not all registers can be seen at once.
>   
>   翻译成中文就是：ARM有一个`Banked Register`的寄存器概念。阅读许多问题和答案以及一些其它资源时，Banked到底是什么意思呢？我得到了如下的定义：Banked register指的就是，在同一地址，可以访问一个寄存器的多个备份。一次只能看见它们中的一个。

ARM core registers on page A2-45 describes the application level view of the ARM core registers. This view provides 16 ARM core registers, R0 to R12, the stack pointer (SP), the link register (LR), and the program counter (PC). These registers are selected from a larger set of registers, that includes Banked copies of some registers, with the current register selected by the execution mode. The implementation and banking of the ARM core registers depends on whether or not the implementation includes the Security Extensions, or the Virtualization Extensions. Figure B1-2 on page B1-1144 shows the full set of Banked ARM core registers, the Program Status Registers CPSR and SPSR, and the ELR_hyp Special register.

> * The architecture uses system level register names, such as R0_usr, R8_usr, and R8_fiq, when it must identify a specific register. The application level names refer to the registers for the current mode, and usually are sufficient to identify a register.
>
> * The Security Extensions and Virtualization Extensions are supported only in the ARMv7-A architecture profile.
>
> * The Virtualization Extensions require implementation of the Security Extensions.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARMv7_A_picture_B_1_2.PNG">

As described in Processor mode for taking exceptions on page B1-1173, on taking an exception the processor changes mode, unless it is already in the mode to which it must take the exception. Each mode that the processor might enter in this way has:

* A Banked copy of the stack pointer, for example SP_irq and SP_hyp.

* A register that holds a preferred return address for the exception. This is:

    - for each PL1 mode, a Banked copy of the link register, for example LR_und and LR_mon
    - for the PL2 mode, Hyp mode, the special register ELR_hyp.

* A saved copy of the CPSR, made on exception entry, for example SPSR_irq and SPSR_hyp.

In addition FIQ mode has Banked copies of the ARM core registers R8 to R12.

User mode and System mode share the same ARM core registers.

User mode, System mode, and Hyp mode share the same LR.

For more information about the application level view of the SP, LR, and PC, and the alternative descriptions of them as R13, R14 and R15, see ARM core registers on page A2-45.

#### 2.1 ARM核寄存器操作的伪代码

The following pseudocode gives access to the ARM core registers:

```c
// The names of the Banked core registers.
enumeration RName {RName_0usr, RName_1usr, RName_2usr, RName_3usr, RName_4usr, RName_5usr,
RName_6usr, RName_7usr, RName_8usr, RName_8fiq, RName_9usr, RName_9fiq,
RName_10usr, RName_10fiq, RName_11usr, RName_11fiq, RName_12usr, RName_12fiq,
RName_SPusr, RName_SPfiq, RName_SPirq, RName_SPsvc,
RName_SPabt, RName_SPund, RName_SPmon, RName_SPhyp,
RName_LRusr, RName_LRfiq, RName_LRirq, RName_LRsvc,
RName_LRabt, RName_LRund, RName_LRmon,
RName_PC};
// The physical array of Banked core registers.
//
// _R[RName_PC] is defined to be the address of the current instruction. The
// offset of 4 or 8 bytes is applied to it by the register access functions.
array bits(32) _R[RName];
// RBankSelect()
// =============
RName RBankSelect(bits(5) mode, RName usr, RName fiq, RName irq,
RName svc, RName abt, RName und, RName mon, RName hyp)
if BadMode(mode) then
UNPREDICTABLE;
else
case mode of
when '10000' result = usr; // User mode
when '10001' result = fiq; // FIQ mode
when '10010' result = irq; // IRQ mode
when '10011' result = svc; // Supervisor mode
when '10110' result = mon; // Monitor mode
when '10111' result = abt; // Abort mode
when '11010' result = hyp; // Hyp mode
when '11011' result = und; // Undefined mode
when '11111' result = usr; // System mode uses User mode registers
return result;
// RfiqBankSelect()
// ================

RName RfiqBankSelect(bits(5) mode, RName usr, RName fiq)
return RBankSelect(mode, usr, fiq, usr, usr, usr, usr, usr, usr);
// LookUpRName()
// =============
RName LookUpRName(integer n, bits(5) mode)
assert n >= 0 && n <= 14;
case n of
when 0 result = RName_0usr;
when 1 result = RName_1usr;
when 2 result = RName_2usr;
when 3 result = RName_3usr;
when 4 result = RName_4usr;
when 5 result = RName_5usr;
when 6 result = RName_6usr;
when 7 result = RName_7usr;
when 8 result = RfiqBankSelect(mode, RName_8usr, RName_8fiq);
when 9 result = RfiqBankSelect(mode, RName_9usr, RName_9fiq);
when 10 result = RfiqBankSelect(mode, RName_10usr, RName_10fiq);
when 11 result = RfiqBankSelect(mode, RName_11usr, RName_11fiq);
when 12 result = RfiqBankSelect(mode, RName_12usr, RName_12fiq);
when 13 result = RBankSelect(mode, RName_SPusr, RName_SPfiq, RName_SPirq,
RName_SPsvc, RName_SPabt, RName_SPund, RName_SPmon, RName_SPhyp);
when 14 result = RBankSelect(mode, RName_LRusr, RName_LRfiq, RName_LRirq,
RName_LRsvc, RName_LRabt, RName_LRund, RName_LRmon, RName_LRusr);
return result;

// Rmode[] - non-assignment form
// =============================
bits(32) Rmode[integer n, bits(5) mode]
assert n >= 0 && n <= 14;
// In Non-secure state, check for attempted use of Monitor mode ('10110'), or of FIQ
// mode ('10001') when the Security Extensions are reserving the FIQ registers. The
// definition of UNPREDICTABLE does not permit this to be a security hole.
if !IsSecure() && mode == '10110' then UNPREDICTABLE;
if !IsSecure() && mode == '10001' && NSACR.RFR == '1' then UNPREDICTABLE;
return _R[LookUpRName(n,mode)];
// Rmode[] - assignment form
// =========================
Rmode[integer n, bits(5) mode] = bits(32) value
assert n >= 0 && n <= 14;
// In Non-secure state, check for attempted use of Monitor mode ('10110'), or of FIQ
// mode ('10001') when the Security Extensions are reserving the FIQ registers. The
// definition of UNPREDICTABLE does not permit this to be a security hole.
if !IsSecure() && mode == '10110' then UNPREDICTABLE;
if !IsSecure() && mode == '10001' && NSACR.RFR == '1' then UNPREDICTABLE;
// Writes of non word-aligned values to SP are only permitted in ARM state.
if n == 13 && value<1:0> != '00' && CurrentInstrSet() != InstrSet_ARM then UNPREDICTABLE;
_R[LookUpRName(n,mode)] = value;
return;
// R[] - non-assignment form
// =========================
bits(32) R[integer n]
assert n >= 0 && n <= 15;
if n == 15 then
offset = if CurrentInstrSet() == InstrSet_ARM then 8 else 4;
result = _R[RName_PC] + offset;
else
result = Rmode[n, CPSR.M];
return result;
// R[] - assignment form
// =====================
R[integer n] = bits(32) value
assert n >= 0 && n <= 14;
Rmode[n, CPSR.M] = value;
return;
// SP - non-assignment form
// ========================
bits(32) SP
return R[13];
// SP - assignment form
// ====================
SP = bits(32) value
R[13] = value;
// LR - non-assignment form
// ========================
bits(32) LR
return R[14];

// LR - assignment form
// ====================
LR = bits(32) value
R[14] = value;
// PC - non-assignment form
// ========================
bits(32) PC
return R[15];
// BranchTo()
// ==========
BranchTo(bits(32) address)
_R[RName_PC] = address;
return;

```

## 3 程序状态寄存器-PSR

The Application level programmers’ model provides the Application Program Status Register, see The Application Program Status Register (APSR) on page A2-49. This is an application level alias for the Current Program Status Register (CPSR). The system level view of the CPSR extends the register, adding system level information.

Every mode that an exception can be taken to has its own saved copy of the CPSR, the Saved Program Status Register (SPSR), as shown in Figure B1-2 on page B1-1144. For example, the SPSR for Monitor mode is called SPSR_mon.

#### 3.1 CPSR-当前程序状态寄存器

The Current Program Status Register (CPSR) holds processor status and control information:

* the APSR, see The Application Program Status Register (APSR) on page A2-49
* the current instruction set state, see Instruction set state register, ISETSTATE on page A2-50
* the execution state bits for the Thumb If-Then instruction, see IT block state register, ITSTATE on page A2-51
* the current endianness, see Endianness mapping register, ENDIANSTATE on page A2-53
* the current processor mode
* interrupt and asynchronous abort disable bits.

The non-APSR bits of the CPSR have defined reset values. These are shown in the TakeReset() pseudocode function, see Reset on page B1-1205.

Writes to the CPSR have side-effects on various aspects of processor operation. All of these side-effects, except for those on memory accesses associated with fetching instructions, are synchronous to the CPSR write. This means they are guaranteed:

* not to be visible to earlier instructions in the execution stream
* to be visible to later instructions in the execution stream.

The privilege level and address space of memory accesses associated with fetching instructions depend on the current privilege level and security state. Writes to CPSR.M can change one of both of the privilege level and security state. The effect, on memory accesses associated with fetching instructions, of a change of privilege level or security state is:

* Synchronous to the change of privilege level or security state, if that change is caused by an exception entry or exception return.

* Guaranteed not to be visible to any memory access caused by fetching an earlier instruction in the execution stream.

* Guaranteed to be visible to any memory access caused by fetching any instruction after the next context synchronization operation in the execution stream.

    > See Context synchronization operation for the definition of this term.

* Might or might not affect memory accesses caused by fetching instructions between the mode change instruction and the point where the mode change is guaranteed to be visible.

* See Exception return on page B1-1194 for the definition of exception return instructions.

#### 3.2 SPSR-保存的程序状态寄存器

The purpose of an SPSR is to record the pre-exception value of the CPSR. On taking an exception, the CPSR is copied to the SPSR of the mode to which the exception is taken. Saving this value means the exception handler can:

* on exception return, restore the CPSR to the value it had immediately before the exception was taken

* examine the value that the CPSR had when the exception was taken, for example to determine the instruction set state and privilege level in which the instruction that caused an Undefined Instruction exception was executed.


Figure B1-2 on page B1-1144 shows the banking of the SPSRs.

The SPSRs are UNKNOWN on reset. Any operation in a Non-secure PL1 or PL0 mode makes SPSR_hyp UNKNOWN.

#### 3.3 CPSR和SPSR的格式

The CPSR and SPSR bit assignments are:

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARMv7_A_picture_B_1_3.PNG">

* Condition flags, bits[31:28]

    Set on the result of instruction execution. The flags are:

    - N, bit[31] Negative condition flag
    - Z, bit[30] Zero condition flag
    - C, bit[29] Carry condition flag
    - V, bit[28] Overflow condition flag.

    The condition flags can be read or written in any mode, and are described in The Application Program Status Register (APSR) on page A2-49.

TODO: 未完待续。

## 4 ELR_hyp

`hyp`模式不提供LR寄存器的副本。取而代之的是，当发生异常进入`hyp`模式时，返回地址保存在`ELR_hyp`寄存器中，一个专门的32位特殊寄存器。

* `ELR_hyp`是专门为虚拟化扩展实现的。

* `ELR_hyp`只能使用`MRS`或`MSR`指令进行访问，具体可以参见指令集相关内容。

* `ERET`指令使用`ELR_hyp`中的值作为异常的返回地址。

* 任何非安全的PL1或PL0模式下，`ELR_hyp`寄存器是`UNKNOWN`的。


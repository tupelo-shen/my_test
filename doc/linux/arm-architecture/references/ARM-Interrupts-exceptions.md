[TOC]

Exception and Interrupt Handling in ARM

## 0 缩略语

| 简称 | 意义 |
| ---- | ---- |
| ISR  | Interrupt Service Routine |
| SWI  | Software Interrupt |
| IRQ  | Interrupt Request |
| FIQ  | Fast Interrupt Request |
| ARM  | Advanced RISC Machines |
| RISC | Reduced Instruction Set Computers |
| SVC  | Supervisor |
| CPSR | Current Program Status Register |
| SPSR | Saved Program Status Register |
| LDR  | Load Register |
| STR  | Store Register |
| DMA  | Direct Memory Access |

## 1 介绍

Exceptions are so important in embedded systems, without exception the development of systems would be a very complex task. With exceptions we can detect bugs in the application, errors in memory access and finally debug it by placing breakpoints and building the program with debugging information.

Interrupts which are kinds of exceptions are essential in embedded systems. It enables the system to deal with external events by receiving interrupt signals telling the CPU that there is something to be done instead of the alternative way of doing the same operation by the pooling mechanism which wastes the CPU time in looping forever checking some flags to know that the event occurred.

Due to the fact that systems are going more complex day after day, we have nowadays systems with more than one interrupt source. That is why an interrupt handling scheme is needed to define how different cases will be handled. We may need priorities to be assigned to different interrupts and in some other cases we may need nested handling capabilities. 

We introduce the ARM processor itself to see its different modes of operation and then we have an overview of the register set. This is because dealing with interrupts and exceptions causes the ARM core to switch between these modes and copy some of the registers into other registers to safe the core state before switching to the new mode. In the next chapter we introduce exceptions and see how the ARM processor handles exceptions. In the third chapter we define interrupts and discuss mechanisms of interrupt handling on ARM. In the forth chapter we provide a set of standard interrupt handling schemes. And finally some remarks regarding these schemes and which one is suitable to which application. The main source of information provided in this paper is mainly the book “ARM System Developer’s Guide” [1].

#### 1.1 ARM工作模式

The ARM processor internally has 7 different modes of operation, they are as follows; User mode: It is used for normal program execution state, FIQ mode: This mode is used for interrupts requiring fast response and low latency like for example data transfer with DMA, IRQ mode: This mode is used for general interrupt services, Supervisor mode: This mode is used when operating system support is needed where it works as protected mode, Abort mode: selected when data or instruction fetch is aborted, system mode: Operating system privilege mode for users and undefined mode: When undefined instruction is fetched. The following table summarizes the 7 modes:

| 工作模式 | 描述 |
| -------- | ---- |
| User (usr)        | 用户态程序工作模式 |
| FIQ (fiq)         | 快速中断工作模式 |
| IRQ (irq)         | 通用中断工作模式 |
| Supervisor (svc)  | 操作系统的保护模式 |
| Abort (abt)       | 读取数据或指令抛弃时进入的工作模式 |
| Undefined (und)   | 调用未定义的指令进入的工作模式 |
| System (sys)      | 操作系统特权模式 |

#### 1.2 ARM寄存器

Register structure in ARM depends on the mode of operation. For example we have 16 (32-
bit) registers named from R0 to R15 in ARM mode (usr).

Registers R0 to R12 are general purpose registers, R13 is stack pointer (SP), R14 is
subroutine link register and R15 is program counter (PC).

R16 is the current program status register (CPSR) this register is shared between all modes
and it is used by the ARM core all the time and it plays a main role in the process of switching
between modes.

In other modes some of these 16 registers are visible and can be accessed and some others are
not visible and can’t be accessed. Also some registers are available with the same name but as
another physical register in memory which is called (banked), existence of such banked
registers decreases the effort needed when context switching is required since the new mode
has its own physical register space and no need to store the old mode’s register values.

So in ARM7 we have a total of 37 physical registers and the following figure shows the
ARM7 register set.


As we can see the banked registers are marked with the gray colour. We can notice that in the
FIQ mode there are more banked registers, this is to speed up the context switching since
there will be no need to store many registers when switching to the FIQ mode. We may need
only to store the values of registers r0 to r7 if the FIQ handler needs to use those registers, but
registers r8_fiq to r14_fiq are specific registers for the FIQ mode and can’t be accesses by any
other mode (they become invisible in other modes).

## 2 ARM异常

An exception is any condition that needs to halt normal execution of the instructions [1]. As an
example of exceptions the state of resetting ARM core, the failure of fetching instructions or
memory access, when an external interrupt is raised or when a software interrupt instruction is
executed. There is always software associated with each exception, this software is called
exception handler. Each of the ARM exceptions causes the ARM core to enter a certain mode
automatically also we can switch between different modes manually by modifying the CPSR
register. The following table summarises different exceptions and the associated mode of
operation on ARM processor.

| Exception              | Mode|
| ---------------------- | --- |
| 快速中断请求           | FIQ |
| 中断请求               | IRQ |
| SWI和RESET             | SVC |
| 取指和内存访问失败     | ABT |
| 未定义的指令           | UND |

#### 2.1 向量表

It is a table of instructions that the ARM core branches to when an exception is raised. These instructions are places in a specific part in memory and its address is related to the exception type. It always contains a branching instruction in one of the following forms:

* B <Add>

    This instruction is used to make branching to the memory location with address “Add” relative to the current location of the pc.

* LDR pc, [pc, #offset]

    This instruction is used to load in the program counter register its old value + an offset value equal to “offset”.

* LDR pc, [pc, #-0xff0]

    This instruction is used only when an interrupt controller is available, to load a specific ISR address from the vector table. The vector interrupt controller (VIC) is placed at memory address 0xfffff000 this is the base address of the VIC. The ISR address is always located at 0xfffff030.

* MOV pc, #immediate

    Load in the program counter the value “immediate”.

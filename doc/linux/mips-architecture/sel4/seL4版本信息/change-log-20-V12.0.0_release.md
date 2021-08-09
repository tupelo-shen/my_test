[TOC]

## 1 变化

#### 1.1 通用变化

* 因为项目已经转移给`seL4`基金会，所以更新License许可。使用`SPDX`标签标记工程项目中的每个文件。内核代码遵循`GPLv2`，用户层代码遵循`BSD-2-Clause`许可协议。

* 更新贡献指南：

    - `seL4`基金会要求使用`DCO`流程，代替`CLA`。
    - `CLA`：全称是`Contributor License Agreement`。主要是要开发者放弃著作权。
    - `DCO`：全称是`Developer Certificate Of Origin`。主要是改善CLA条款的缺陷而产生的。一般集成在git中使用。

* 为基于`RISC-V`架构的64位的`HiFive`未正式发布的平台提供功能正确性的验证（`RISCV64_verified.cmake`中没有启动`fastpath`和`FPU`使能）。

* 更新说明文件：

    - 回收操作被移除；
    - 添加关于验证版本的更多信息；
    - 更新关于`seL4`实时应用的注释；

* `seL4`手册的改进：

    - 修复了`aarch64`架构下`seL4_ARM_PageDirectory`对象相关API文档：传递给`seL4_ARM_PageDirectory_Map`的是一个虚拟空间`vspace`能力，而不是上层的`页目录`能力；
    - 增加了`ARM`架构下对象调用的文档覆盖率；
    - 对内核服务和对象一章中的系统调用重新进行了整理；
    - 增加了MCS系统和非MCS系统之间关于Recv和Wait系统调用的不同行为的讨论；
    - 在`exceptions`一章中，提及了`grantreply`权限；
    - 记录`schedcontext`的 `size_bits`的意义；
    - 删除系统临界性的内容；
    - 将`SchedContext`添加到对象大小的讨论中；
    - 修复初始线程的`CNode`的`guard`位大小；
    - 更新`BootInfo`结构表；
    - 更新`UntypedDesc`表中的填充字段；

* 更新`seL4_DebugSnapshot`，以提供正在运行系统的能力布局的CapDL转储，支持`ARM`、`x86_64`和`riscv32`配置。

* KernelBenchmarksTrackUtilisation:

    - 添加对`SMP`的支持；
    - 对于每个线程，还可以跟踪调度次数、内核入口的数量、在内核中花费的周期数以及给每个线程添加绑核的总数；

* 添加2个新的基准利用率的系统调用：

    - `seL4_BenchmarkDumpAllThreadsUtilisation`：以JSON格式的形式，打印系统的总、每个线程的利用率统计信息。这包括线程总的调度周期、总的调度次数、总的内核态花费周期、总的进入内核次数、当前核上所有线程的总数；
    - `seL4_BenchmarkResetAllThreadsUtilisation`: 复位当前核上每个用户线程的统计数据；

* 添加`seL4_DebugPutString`函数（libsel4库），底层调用`seL4_DebugPutChar()`函数打印以`NULL`结束的字符串；

* 引入新的config标志，`KernelInvocationReportErrorIPC`，将userError格式化字符串写入到`IPC`缓存中。另一个配置bool型标志，用来控制是否打印错误信息到IPC缓存中，可以在运行时进行设置。`KernelInvocationReportErrorIPC`配置选项，用来控制是否将IPC缓存中的内核错误信息打印出来；

* 修复`clh_lock_acquire`（SMP内核锁）中的屏障。加强`clh_lock_acquire`，以便在`atomic_exchange`上使用`release`，使NODE成为公共的。否则，在ARM或者RISC-V上，存储node值，将它的状态设为`CLHState_Pending`后的一段时间内该node可见，那么该值也就可见。在这个窗口期内，下一个线程想要申请lock，将会看到的是旧的状态（`CLHState_Granted`），进入临界区，导致互斥冲突；

* 在`libsel4`头文件中用`#pragma once`指令替换所有`#ifdef`头保护；

* gcc.cmake:
    - 添加了颜色化打印输出的选项。在环境中设置`GCC_COLORS`将导致向gcc提供`-fdiagnostics-color=always`。通常，在正常构建期间，当`ninja`重定向其`stderr`时，gcc会抑制颜色化输出；
    - 记住，`CROSS_COMPILER_PREFIX`是跨`CMake`调用的。在有些上下文中，这个变量可能未设置；
    - 在`Red Hat`发行版上增加了对Arm交叉编译器的支持。

* `Fastpath`优化：

    - Reorganise the code layout on Arm.
    - bitfield_gen: explicit branch predictions.
    - Optimize instruction cache access for fastpath.

* 对于所有的内核配置都支持`Clang`编译器，支持LLVM版本9-11；

* `hardware_gen.py`：为`hardware_gen`脚本添加`elfloader`输出目标。这将生成头文件，描述平台的CPU配置以及设备信息，如`elfloader`的兼容性字符串和内存区域，这些都与内核自己的定义一致。

* Fix bootinfo allocation bug when user image pushed against page directory boundary. The bootinfo is mapped in at the end of the user image in the initial thread’s vspace. The kernel initialisation code wasn’t calculating the bootinfo size correctly which could lead to a kernel fault when trying to map the bootinfo in when the parent page table object hadn’t been allocated.

* 在`<sel4/types.h>`中使用`RetypeMaxObjects`的`autoconf`定义。这确保了定义与内核的配置保持一致。

* 修正定时器和时钟频率：

    - Remove beaglebone kernel timer prescaling. Previously the timer frequency was incorrectly set to to half (12MHz) its configured frequency (24MHz).
    - Set TX1 kernel timer frequency config to 12MHz and not 19.2MHz as this is the standard frequency of the input clock source (m_clock).
    - Set KZM kernel timer frequency config to 35MHz and not 18.6MHz based on sampling the timer frequency.
    - imx31: add missing dts entry for the epit2 timer.
    - Set non-mcs i.MX6 kernel timer frequency config to 498MHz and not 400MHz as this is based on the frequency of the input clock source (PLL1)
    - `Zynq7000`：设置内核定时器频率为`320MHz`；
    - `Qemu-arm-virt`：设置内核定时器频率为`6.25MHz`；

* 不要在`libsel4`中为枚举生成数据符号，因为它们最终会存储在`bss`段，在较新的编译器版本中导致链接错误。

* 更新`seL4_UntypedDesc`中的`padding`字段，使结构按照WORD对齐。之前，这个结构体在64位平台上没有正确的字对齐。此更改删除了`padding1`和`padding2`字段，并将它们替换为一个根据平台不同的可变字节数的`padding`字段。

* 添加GitHub行为脚本。这些脚本直接在GitHub上复制内部CI检查；

#### 1.2 MCS

* Stop scheduling contexts from being bound to tcb’s that already have scheduling contexts.
* Fix x86 KERNEL_TIMER_IRQ definition. Previously, MCS preemption point handling would check the wrong interrupt on x86 platforms.
* smp: tcb affinity modification bug. When changing the affinity of a thread on a remote core, the reschedule operation wasn’t being performed.
* Allow replyGrant for fault handlers. The MCS kernel so far insisted on full grant rights for fault handler caps, but replyGrant is sufficient and consistent with the default kernel config.
* All scheduling contexts compare their time with time in assigned core instead of currently executing core.
* Prevent recursion on timeout faults by suspending a passive server that receives a timeout fault.
* Add KernelStaticMaxBudgetUs to bound the time the user provides to configure scheduling contexts to avoid malicious or erroneous overflows of the scheduling math. Make the default max period/budget 1 hour.
* rockpro64: enable mcs configurations

#### 1.3 ARM

* `arm`：添加`seL4_BenchmarkFlushL1Caches`系统调用，用来手动刷新基准配置中的L1级Cache；

* 添加新的fault类型，用于运行在`Arm hypervisor`模式时：`seL4_Fault_VPPIEvent`：

    - The kernel can keep track of IRQ state for each VCPU for a reduced set of PPI IRQs and deliver IRQ events as VCPU faults for these interrupt numbers.
    - Additionally a new VCPU invocation is introduced: seL4_ARM_VCPU_AckVPPI. This is used to acknowledge a virtual PPI that was delivered as a fault.

* 虚拟ARM定时器和VTimer中断，以便支持跨vCPU共享：
    
    - A VCPU will now save and restore VTimer registers for the generic timer and also deliver a VTimer IRQ via a seL4_Fault_VPPIEvent fault. This enables multiple VCPUs bound to the same physical core to share this device.

* 增加了构建配置选项，用于在`ARM hypervisor`模式下运行时，是否在vCPU上设置`WFE/WFI`陷阱；

* `arm`：为vCPU对象增加`VMPIDR`和`VMPIDR_EL2`寄存器，用于在aarch32和aarch64架构上编程vCPU的`虚拟化多处理器ID寄存器`；

* `arm`、`vcpu`、`smp`：为虚拟IRQ添加远程IPI调用支持。 Remote IPI call support for VIRQS. Injecting a VIRQ into a vcpu running on a different core will IPI the remote core to perform the IRQ injection.

* `zynqmp`: 禁用硬件调试API，因为平台不支持内核硬件调试API；
* `zynqmp`：为`aarch32 hyp`添加支持；
* `Gicv3`: include cluster id when sending ipis.

* qemu-arm-virt:

    - 基于`KernelMaxNumNodes`配置选项生成平台`dtb`；
    - 预留RAM的前512M空间，作为`device untyped`内存区，虚拟化配置时使用；

###### 1.3.1 Aarch32

* Moved TPIDRURO (PL0 Read-Only Thread ID register) to TCB register context from VCPU registers. This means changes to this register from user level have to go via seL4_TCB_Write Registers instead of seL4_ARM_VCPU_WriteRegs.
* aarch32: Restrict cache flush operation address range in hyp mode. It’s required that cache flushing in hyp mode is performed through the kernel window mapping as the kernel is unable to flush addresses outside of this mapping without causing an access fault.
* arm_hyp: Move PGD definitions out of libsel4 as they don’t correspond to any public interfaces and are only used internally by the kernel to manage its own address space.

###### 1.3.2 Aarch64

* `aarch64`: Fix a bug where saving ELR_EL1 when managing a VCPU object was reading from ELR_EL1 instead of writing to it.

* `aarch64`: Fix a bug where saving FAR_EL1 when managing a VCPU object was only writing to the low 32 bits of the 64-bit FAR_EL1 register.

* `aarch64`: Add missing faults to seL4_getArchFault. seL4_getArchFault is a libsel4 helper that constructs fault messages out of the IPC buffer message registers but it wasn’t aware of all possible fault types.

* `aarch64`、`vcpu`: Add CNTKCTL_EL1 register to `vcpu_t`. This register tracks timer delegation to EL0 from EL1 and needs to be switched for different VCPUs.

* `aarch64`: Adds missing vcpu cases for some aarch64-specific functions on capabilities.

* `cortex-a53`、`hyp`: Reduce seL4_UserTop when on a cortex-a53 platform and KernelArmHypervisorSupport is set.
    - This is because the kernel uses the last slot in the top level VSpace object for storing the assigned VMID and so any addresses that are addressed by the final slot are not accessible. This would apply to any CPU that have 40bit stage 2 translation input address.

* `Arm SMMUv2`内核API和`TX2 smmuv2`驱动。 This supports using an SMMU to provide address translation and memory protection for device memory access to platforms that implement a compatible Arm SMMUv2 System mmu. The kernel implementation supports using an SMMU to restrict memory access of VM guest pass-through devices, or for isolating devices and their drivers’ memory accesses to the rest of a running system.

#### 1.4 x86

* 修正`apic_init`中的打印错误；
* x86_64: Fix PCID feature constant to use the correct bit.
* Fix interrupt flag reset upon nested interrupt resume, `c_nested_interrupt`. This fixes an issue where ia32 kernels would crash if receiving a nested interrupt.

#### 1.5 RISC-V

* Functional correctness of seL4/RISCV now formally verified at the C level.
* Hifive: Enable seL4_IRQControl_GetTrigger object method. This method allocates an IRQ handler by ID and whether it is level or edge triggered. Note: HiFive PLIC interrupts are all positive-level triggered.
* Add search for additonal gcc riscv toolchains if the first one cannot be found.
* Add support for rocketchip soc. Support Rocketchip SoC maps to Xilinx ZC706 board and ZCU102 board
* Add support for polarfire soc.
* Clear reservation state on slowpath exit as the RISC-V ISA manual requires supervisor code to execute a dummy sc instruction to clear reservations “during a preemptive context switch”.
* Pass DTB through to userlevel in extra bootinfo fields similar to on Arm.
* Use full width of scause to prevent large exception numbers to be misinterpreted as syscalls.
* Fix page map bug.
    - Previously, it was possible in decodeRISCVFrameInvocation for the rwx rights of the new PTE to become 000 after masking with cap rights. This would turn the frame PTE into a page table PTE instead, and allow the user to create almost arbitrary mappings, including to kernel data and code. The defect was discovered in the C verification of the RISC-V port.
* Remove seL4_UserException_FLAGS. This field was unused and was never set to anything by the kernel.
* Add FPU config options for RISCV
    - Two options, KernelRiscvExtD and KernelRiscvExtF, are added to represent the D and F floating-point extensions. KernelHaveFPU is enabled when the floating-point extensions are enabled. The compiler ABI will also be changed to lp64d for hardfloat builds.
* Add RISCV64_MCS_verified.cmake config for in-progress MCS verification
* riscv32: Remove incorrectly provided constants for 512MiB ‘huge pages’ which is not part of the specification.
* riscv: Lower .boot alignment to 4KiB. This makes the final kernel image more compact.

## 2 升级说明

* The project’s licensing updates don’t change the general licensing availability of the sources. More information can be found: https://github.com/seL4/seL4/blob/master/LICENSE.md

* Any references to the padding1 or padding2 fields in seL4_UntypedDesc require updating to the new padding field. It is expected that these fields are unused.

* Any platforms that have had changed kernel timer frequencies will see different scheduling behavior as kernel timer ticks will occur at different times as kernel ticks are configured in microseconds but then converted to timer ticks via the timer frequency. Any time-sensitive programs may need to be re-calibrated.

* Any riscv32 programs that were using the constants RISCVGigaPageBits or seL4_HugePageBits will see a compilation error as the constants have been deleted as they aren’t supported in the riscv32 spec.

* Any riscv programs that refer to the seL4_UserException_FLAGS field will need to remove this reference. This field was never initialised by the kernel previously and has now been removed.

* Any riscv programs using the LR/SC atomics instructions will see reservations invalidated after kernel entries. It is expected that this will not require any changes as reservations becoming invalid is normal behavior.

* On cortex-a53 platforms when KernelArmHypervisorSupport is set have 1 less GiB of virtual memory addresses available or 2GiB less on TX2 with the SMMU feature enabled. This is captured by the change in definition of the seL4_UserTop constant that holds the larges virtual address accessible by user level.

* On aarch32 the TPIDRURO register(PL0 Read-Only Thread ID register) has been removed from the VCPU object and added to the TCB object. A VCPU is typically bound to a TCB so after updating the access, a thread with a VCPU attached will still support having a TPIDRURO managed.

* On Arm hypervisor configurations, PPI virtual timer interrupts are now delivered via seL4_Fault_VPPIEvent faults and it is not possible to allocate an interrupt handler for these interrupts using the normal interrupt APIs. A VPPI interrupt is received via receiving a fault message on a VCPU fault handler, and acknowledged by an invocation on the VCPU object. VPPI interrupts that target a particular VCPU can only be generated while the VCPU thread is executing.

## 3 完整的修改日志

可以使用`git log`命令获取：`git log 11.0.0..12.0.0`。

## 4 更多详细信息

参考`V12.0.0`的手册。
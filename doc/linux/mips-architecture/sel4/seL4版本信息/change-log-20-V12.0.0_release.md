[TOC]

## 1 变化

#### 1.1 通用变化

* Update licensing to reflect project transfer to seL4 Foundation. SPDX tags are now used to identify the licenses for each file in the project. Generally, kernel-level code is licensed under GPLv2 and user-level code under the 2-clause BSD license.
* Update contribution guidelines:
    - the seL4 foundation requires DCO process instead of a CLA
* Functional correctness verification for the RISC-V 64-bit HiFive Unleashed platform configuration (RISCV64_verified.cmake with no fastpath or FPU enabled)
* Update caveats file:
    - The recycle operation has been removed
    - More detail on what versions are verified
    - Update comments on real time use of seL4
* Improve seL4 manual.
    - Fix aarch64 seL4_ARM_PageDirectory object API docs: seL4_ARM_PageDirectory_Map is passed a vspace cap not an upper page directory cap.
    - Increase documentation coverage for Arm object invocations
    - Rework introduction of system calls in Kernel Services and Objects chapter.
    - Improve discussion of Receive and Wait syscall behaviour between MCS and non-MCS systems.
    - Explicitly mention grantreply rights in the exceptions section.
    - Document schedcontext size_bits meaning.
    - Remove metion of system criticality.
    - Add SchedContext to object size discussion.
    - Fix initial thread’s CNode guard size.
    - Update BootInfo struct table.
    - Update padding field in UntypedDesc table
* Update seL4_DebugSnapshot to provide a CapDL dump of the capability layout of a running system for Arm, x86_64 and riscv32 configurations.
* KernelBenchmarksTrackUtilisation:
    - Add feature support for SMP configurations
    - For each thread also track number of times scheduled, number of kernel entries and amount of cycles spent inside the kernel and also add core-wide totals for each.
* Add 2 new benchmark utilization syscalls
    - seL4_BenchmarkDumpAllThreadsUtilisation: Prints a JSON formatted record of total and per-thread utilisation statistics about the system. This currently includes a thread’s total cycles scheduled, total number of times scheduled, total cycles spent in the kernel and total number of times entering the kernel and then totals of each for all threads on the current core.
    - seL4_BenchmarkResetAllThreadsUtilisation: Resets the current counts of every user thread on the current core.
* Added seL4_DebugPutString libsel4 function for printing a null-terminated string via calling seL4_DebugPutChar().
* Introduced a new config flag, KernelInvocationReportErrorIPC, to enable userError format strings to be written to the IPC buffer. Another config bool has been introduced to toggle printing the error out and this can also be set at runtime. LibSel4PrintInvocationErrors is a libsel4 config used to print any kernel error messages reported in the IPC buffer.
* Repair barriers in clh_lock_acquire (SMP kernel lock). Strengthen the clh_lock_acquire to use release on the atomic_exchange that makes the node public. Otherwise, (on ARM & RISCV) the store to the node value which sets its state to CLHState_Pending can become visible some time after the node is visible. In that window of time, the next thread which attempts to acquire the lock will still see the old state (CLHState_Granted) and enters the critical section, leading to a mutual exclusion violation.
* Replace all #ifdef header guards with #pragma once directives in libsel4 header files
* gcc.cmake:
    - Add option for coloured gcc output. Setting GCC_COLORS in the environment will result in `-fdiagnostics-color=always` being provided to gcc. Ordinarily gcc would suppress coloured output when ninja redirects its stderr during normal builds.
    - Remember CROSS_COMPILER_PREFIX across CMake invocations. The variable would become unset in certain contexts.
    - Add support for Arm cross-compilers on Red Hat distros.
* Fastpath optimisation:
    - Reorganise the code layout on Arm.
    - bitfield_gen: explicit branch predictions.
    - Optimize instruction cache access for fastpath.
* Extend Clang support to all kernel configurations. Support targets LLVM versions between 9 and 11.
* hardware_gen.py: Add elfloader output target for hardware_gen script. This generates header files describing the platform’s CPU configuration as well as device information such as compatibility strings and memory regions to the elfloader that are consistent with the kernel’s own definitions.
* Fix bootinfo allocation bug when user image pushed against page directory boundary. The bootinfo is mapped in at the end of the user image in the initial thread’s vspace. The kernel initialisation code wasn’t calculating the bootinfo size correctly which could lead to a kernel fault when trying to map the bootinfo in when the parent page table object hadn’t been allocated.
* Use autoconf definition for RetypeMaxObjects in <sel4/types.h>. This ensures that the definition stays consistent with what the kernel is configured with.
* Fix up timers and clock frequencies
    - Remove beaglebone kernel timer prescaling. Previously the timer frequency was incorrectly set to to half (12MHz) its configured frequency (24MHz).
    - Set TX1 kernel timer frequency config to 12MHz and not 19.2MHz as this is the standard frequency of the input clock source (m_clock).
    - Set KZM kernel timer frequency config to 35MHz and not 18.6MHz based on sampling the timer frequency.
    - imx31: add missing dts entry for the epit2 timer.
    - Set non-mcs i.MX6 kernel timer frequency config to 498MHz and not 400MHz as this is based on the frequency of the input clock source (PLL1)
    - Zynq7000: Set kernel timer frequency to 320MHz.
    - Qemu-arm-virt: Set kernel timer frequency to 6.25MHz.
* Do not generate data symbols for enums in libsel4 as they end up as bss symbols and cause linker errors on newer compiler versions.
* Update padding field definition in seL4_UntypedDesc to make the struct word aligned. Previously, this struct wasn’t correctly word aligned on 64-bit platforms. This change removes the padding1 and padding2 fields and replaces them with a padding field that is a variable number of bytes depending on the platform.
* Add GitHub actions scripts. These scripts replicate internal CI checks directly on GitHub

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

* arm: Add seL4_BenchmarkFlushL1Caches syscall to manually flush L1 caches in benchmark configurations.
* New fault type when running in Arm hypervisor mode: seL4_Fault_VPPIEvent
    - The kernel can keep track of IRQ state for each VCPU for a reduced set of PPI IRQs and deliver IRQ events as VCPU faults for these interrupt numbers.
    - Additionally a new VCPU invocation is introduced: seL4_ARM_VCPU_AckVPPI. This is used to acknowledge a virtual PPI that was delivered as a fault.
* Virtualise Arm Timer and VTimer interrupts to support sharing across VCPUs.
    - A VCPU will now save and restore VTimer registers for the generic timer and also deliver a VTimer IRQ via a seL4_Fault_VPPIEvent fault. This enables multiple VCPUs bound to the same physical core to share this device.
* Build config option for whether WFE/WFI traps on VCPUs when running in Arm hypervisor mode
* Arm: Add VMPIDR and VMPIDR_EL2 registers to VCPU objects for programming a VCPU’s ‘Virtualization Multiprocessor ID Register’ on aarch32 and aarch64.
* Arm, vcpu, smp: Remote IPI call support for VIRQS. Injecting a VIRQ into a vcpu running on a different core will IPI the remote core to perform the IRQ injection.
* zynqmp: Disable hardware debug APIs as the platform doesn’t support kernel hardware debug API.
* zynqmp: Add support for aarch32 hyp
* Gicv3: include cluster id when sending ipis.
* qemu-arm-virt:
    - Generate platform dtb based on KernelMaxNumNodes config value.
    - Reserve the first 512MiB of Ram as device untyped for use in virtualization configurations.

###### 1.3.1 Aarch32

* Moved TPIDRURO (PL0 Read-Only Thread ID register) to TCB register context from VCPU registers. This means changes to this register from user level have to go via seL4_TCB_Write Registers instead of seL4_ARM_VCPU_WriteRegs.
* aarch32: Restrict cache flush operation address range in hyp mode. It’s required that cache flushing in hyp mode is performed through the kernel window mapping as the kernel is unable to flush addresses outside of this mapping without causing an access fault.
* arm_hyp: Move PGD definitions out of libsel4 as they don’t correspond to any public interfaces and are only used internally by the kernel to manage its own address space.

###### 1.3.2 Aarch64

* aarch64: Fix a bug where saving ELR_EL1 when managing a VCPU object was reading from ELR_EL1 instead of writing to it.
* aarch64: Fix a bug where saving FAR_EL1 when managing a VCPU object was only writing to the low 32 bits of the 64-bit FAR_EL1 register.
* aarch64: Add missing faults to seL4_getArchFault. seL4_getArchFault is a libsel4 helper that constructs fault messages out of the IPC buffer message registers but it wasn’t aware of all possible fault types.
* aarch64,vcpu: Add CNTKCTL_EL1 register to `vcpu_t`. This register tracks timer delegation to EL0 from EL1 and needs to be switched for different VCPUs.
* aarch64: Adds missing vcpu cases for some aarch64-specific functions on capabilities.
* cortex-a53,hyp: Reduce seL4_UserTop when on a cortex-a53 platform and KernelArmHypervisorSupport is set.
    - This is because the kernel uses the last slot in the top level VSpace object for storing the assigned VMID and so any addresses that are addressed by the final slot are not accessible. This would apply to any CPU that have 40bit stage 2 translation input address.
* Arm SMMUv2 kernel API and TX2 smmuv2 driver. This supports using an SMMU to provide address translation and memory protection for device memory access to platforms that implement a compatible Arm SMMUv2 System mmu. The kernel implementation supports using an SMMU to restrict memory access of VM guest pass-through devices, or for isolating devices and their drivers’ memory accesses to the rest of a running system.

#### 1.4 x86

* Fix printf typo in apic_init.
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
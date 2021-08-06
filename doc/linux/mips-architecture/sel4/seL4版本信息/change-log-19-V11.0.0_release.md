[TOC]

## 1 变化

* Add GrantReply access right for endpoint capabilities.
* seL4_Call is permitted on endpoints with either the Grant or the GrantReply access rights.
* Capabilities can only be transferred in a reply message if receiver’s endpoint capability has the Grant right.
* `seL4_CapRights_new` now takes 4 parameters
* seL4_CapRightsBits added to libsel4. seL4_CapRightsBits is the number of bits to encode seL4_CapRights.
* `seL4_UserTop` added
    - a new constant in libsel4 that contains the first virtual address unavailable to user level.
* Add Kernel log buffer to aarch64
* Support added for Aarch64 hypervisor mode (EL2) for Nvidia TX1 and TX2. This is not verified.
* Support for generating ARM machine header files (memory regions and interrupts) based on a device tree.
* Support added for ARM kernel serial driver to be linked in at build time based on the device tree compatibility string.
* Support added for compiling verified configurations of the kernel with Clang 7.
* RISC-V: handle all faults
    - Pass all non-VM faults as user exceptions.
* arm-hyp: pass ESR in handleUserLevelFault
* aarch64: return ESR as part of user level fault
* Created new seL4_nbASIDPoolsBits constant to keep track of max nb of ASID pools.
* Support added for Hardkernel ODROID-C2.
* Added extended bootinfo header for device tree (SEL4_BOOTINFO_HEADER_FDT).
* Support added for passing a device tree from the bootloader to the root task on ARM.
* Add seL4_VSpaceBits, the size of the top level page table.
* The root cnode size is now a minimum of 4K.
* Hifive board support and RISC-V external interrupt support via a PLIC driver.
* Update seL4_FaultType size to 4bits.
* Fix seL4_MappingFailedLookupLevel() for EPTs on x86.
    - add SEL4MAPING_LOOKUP_NO[EPTPDPT, EPTPD, EPTPT] which now correspond to the value returned by seL4_MappingFailedLookupLevel on X86 EPT mapping calls.
* BeagleBone Black renamed from am335x to am335x-boneblack.
* Supported added for BeagleBone Blue (am335x-boneblue).
* Remove IPC Buffer register in user space on all platforms
* Add managed TLS register for all platforms
* Add configurable system call allowing userspace to set TLS register without capability on all platforms.
* Non-hyp support added for Arm GICv3 interrupt controller.
* Add initial support for i.MX8M boards.
    - Support for i.MX8M Quad evk AArch64 EL2 and EL1, AArch32 smode only is accessible via the imx8mq-evk platform.
    - Support for i.MX8M Mini evk AArch64 EL2 and EL1, AArch32 smode only is accessible via the imx8mm-evk platform.
* Add FVP platform with fixed configuration. This currently assumes A57 configuration described in `tools/dts/fvp.dts`.
* Arm SMP invocation IRQControl_GetTriggerCore added
    - Used to route a specify which core an IRQ should be delivered on.
* Kernel log buffer: Specify on which core an IRQ was delivered.
* Add new seL4_DebugSendIPI syscall to send arbitrary SGIs on ARM when SMP and DEBUG_BUILD are activated.
* Support for aarch64-hyp configurations with 40-bit physical addresses (PA) added.
    - The aarch64 api now refers to VSpaces rather than PageGlobalDirectories, as depending on the PA the top level translation structure can change.
    - all `seL4_ARM_PageGlobalDirectory` invocations are now `seL4_ARM_VSpace` invocations.
    - new constants `seL4_ARM_VSpaceObject` and `seL4_VSpaceIndexBits`.
* 合并MCS内核特性：
    - 正在验证中...
    - The goals of the MCS kernel is to provide strong temporal isolation and a basis for reasoning about time.
* Moved aarch64 kernel window
    - aarch64 kernel window is now placed at 0, meaning the kernel can access memory below where the kernel image is mapped.
* aarch64: Moved TPIDRRO_EL0 (EL0 Read-Only Thread ID register) to TCB register context from VCPU registers. This means changes to this register from user level have to go via seL4_TCB_Write Registers instead of seL4_ARM_VCPU_WriteRegs.

* Merge ARCH_Page_Remap functionality into ARCH_Page_Map. Remap was used for updating the mapping attributes of a page without changing its virtual address. Now ARCH_Page_Map can be performed on an existing mapping to achieve the same result. The ARCH_Page_Remap invocation has been removed from all configurations.

* riscv64: Experimental SMP support for RISCV64 on HiFive.

* Support added for QEMU ARM virt platform, with 3 CPUs: cortex-a15, cortex-a53 and cortex-a57
    - PLATFORM=qemu-arm-virt
    - ARM_CPU={cortex-a15, cortex-a53, cortex-a57}
    - QEMU_MEMORY=1024 (default)

* Support added for rockpro64.

* RISCV: Add support for Ariane SoC

* Unify device untyped initialisation across x86, Arm and RISC-V
    - Access to the entire physical address range is made available via untypes.
    - The kernel reserves regions that user level is not able to access and doesn’t hand out untypeds for it.
    - Ram memory is part of this reservation and is instead handed out as regular Untypeds.
    - Memory reserved for use by the kernel or other reserved regions are not accessible via any untypeds.
    - Devices used by the kernel are also not accessible via any untypeds.

## 2 升级说明

* Usages of Endpoints can now use seL4_Call without providing Grant rights by downgrading the Grant to GrantReply

* The kernel no longer reserves a register for holding the address of a thread’s IPC buffer. It is now expected that the location of the IPC buffer is stored in a `__thread` local variable and a thread register is used to refer to each thread’s thread local variables. The sel4runtime is an seL4 runtime that provides program entry points that setup the IPC buffer address and serves as a reference for how the IPC buffer is expected to be accessed.

* All `seL4_ARM_PageGlobalDirectory` invocations need to be replaced with `seL4_ARM_VSpace`.

* Usages of ARCH_Page_Remap can be replaced with ARCH_Page_Map and require the original mapping address to be provided.

* Device untypeds are provided to user level in different sizes which may require more initial processing to break them down for each device they refer to.

## 3 完整的修改日志

可以使用`git log`命令获取：`git log 10.1.1..11.0.0`。

## 4 更多详细信息

参考`V11.0.0`的手册。
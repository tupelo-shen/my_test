[TOC]

原文地址:[https://genode.org/documentation/articles/arm_virtualization](https://genode.org/documentation/articles/arm_virtualization)

Recent high end ARM CPUs include support for hardware virtualization. Due to limitations of former ARM architectures, virtualizing the hardware tended to be slow and expensive. Some privileged instructions did not necessarily trap when executed in non-privileged mode. This effectively prevented a hypervisor to run the kernel code of the guest operating system unmodified in non-privileged mode and to handle privileged instructions using a trap-and-emulate approach. Instead of modifying the architecture in a way to close the virtualization holes, meaning to always trap when privileged parts of the CPU state shall be modified, ARM decided to stay backwards compatible and extended the ARM v7 architecture to support virtualization. ARM went a similar path like Intel when VT was introduced for the x86 architecture.

Although ARM's virtualization extensions seem to be very similar to Intel's approach, the devil is in the details. We were looking forward to explore them for quite some time and are happy to share the insights of our research with you. When starting with this line of work on ARM virtualization, one of the most interesting questions for us was: How can we integrate virtualization into Genode without increasing the trusted computing base (TCB) of applications that run beside virtual machines? Moreover, how can we make the virtual machine monitor (VMM) of one virtual machine (VM) independent of others, similar to the approach taken by the [NOVA OS virtualization architecture](http://hypervisor.org/)?

## Overall Architecture

The ARM virtualization extensions are based on the security extensions, commonly known as TrustZone. For more information about TrustZone, refer to our [previously published article](http://genode.org/documentation/articles/trustzone). To realize the switching between different virtual machines, a new privilege level was introduced within the normal world of the processor, including one new processor execution mode - the "hyp" mode.

<img src="1_1">

The figure above presents an overview of all privilege levels and modes an ARM processor may execute code in. As depicted, virtual machines can be executed in TrustZone's "normal world" only. Processor instructions that support VM separation and world switches can be used in either hyp or monitor mode only. Thereby, the first design question was whether the hypervisor-related code shall be executed in the normal world solely, or if at least parts of the hypervisor should run in the "secure world"? Which of both TrustZone worlds should host normal user-land applications, not related to virtualization?

Until now, Genode used to be executed in either the normal world or the secure world, depending on the platform. For instance, when using the Pandaboard, the secure world is already locked by OEM firmware during the boot process, and Genode is restricted to the normal world. In contrast, on the ARNDALE board, the Genode system is free to use both the secure and normal world. We decided to ignore the secure world here and to execute everything in the normal world. Thereby, Genode is able to accommodate all possible platforms including those where the TrustZone features are already locked by the vendor.

Starting point of the current work was Genode's integrated ARM kernel that we call "base-hw" ("hw" representing Genode running on bare hardware as opposed to running it on a third-party kernel). On this platform, Genode's core process runs partly in kernel mode (PL1) but most code is running in user mode (PL0). The rationale behind this design is explained in our TrustZone article. We broadened this approach by extending the core process with hypervisor-specific code running in hyp mode (PL2), thereby gaining a binary that has a global view on the hardware resources yet is executed in three different privilege levels. The concept of the base-hw platform hosting different virtual machines from a bird's perspective is depicted in the following figure.

<img src="1_2">

Ideally, the part of the core process, which runs in hyp mode comprises hypervisor-specific code only. This includes the code to switch between different virtual machines as well as the Genode world denoted as Dom0 in the picture. To keep its complexity as low as possible, the hypervisor should stay free from any device emulation. If possible its functionality should come down to reloading general purpose and system registers, and managing guest-physical to host-physical memory translations.

In contrast to the low-complexity hypervisor, the user-level VMM can be complex without putting the system's security at risk. It contains potentially complex device-emulation code and assigns hardware resources such as memory and interrupts to the VM. The VMM is an ordinary application running unprivileged and can be re-instantiated per VM. By instantiating one VMM per VM, different VMs are well separated from each other. Even in the event that one VMM breaks, the other VMs stay unaffected. Of course, a plain user-land application is not able to directly use the hardware virtualization extensions. These extensions are available in hyp mode only, which is exclusive to the kernel. Hence an interface between VMM and the kernel is needed to share the state of a virtual machine. We faced a similar problem when building a VMM for our former TrustZone experiments. It was natural to build upon the available solution, extending it wherever necessary. Core provides a so called VM service. Each VM corresponds to a session of this service. The session provides the following interface:

* **CPU state**

    The CPU-state function returns a dataspace containing the virtual machine's state. It is initially filled by the VMM before starting the VM, gets updated by the hypervisor whenever it switches away from the VM, and can be used by the VMM to interpret the behavior of the guest OS. Moreover, it can be updated after the virtual machine monitor emulated instructions for the VM. This mechanism can be compared to the VMCS structure in the x86 architecture.

* **Exception handler**

    The second function is used to register a signal handler that gets informed whenever the VM produces a virtualization fault.

* **Run**

    The run function starts or resumes the execution of the VM.

* **Pause**

    The pause function removes the VM from the kernel's scheduler.

Given this high-level architecture the remainder of the article covers the technical challenges to realize it. We start with explaining what had to be done to initially bootstrap the platform from secure to normal world. We encountered memory as the first hardware resource we had to address. Section [Memory Virtualization](https://genode.org/documentation/articles/arm_virtualization#Memory_Virtualization) explains our approach to virtualizing memory. The most substantial part is the virtualization of the CPU, which is covered in section [CPU Virtualization](https://genode.org/documentation/articles/arm_virtualization#CPU_Virtualization). It is followed by the explanation of the pitfalls and positive surprises while virtualizing interrupts and time in sections [Virtualizing Interrupts](https://genode.org/documentation/articles/arm_virtualization#Virtualizing_Interrupts) and [Virtual Time](https://genode.org/documentation/articles/arm_virtualization#Virtual_Time). Finally, the article closes with a summary of the current state.

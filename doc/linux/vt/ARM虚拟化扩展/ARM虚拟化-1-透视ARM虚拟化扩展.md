[TOC]

原文地址:[https://genode.org/documentation/articles/arm_virtualization](https://genode.org/documentation/articles/arm_virtualization)

高端ARM架构CPU已经包含了硬件虚拟化的支持。由于早期ARM架构的限制，虚拟化硬件性能不高且代价高昂。当运行在非特权模式下时，一些特权指令不能被捕获。这有效阻止了`hypervisor`未经修改地在非特权模式下运行客户机操作系统的内核代码，也不能使用`异常捕获-模拟指令（trap-and-emulate）`的方式，处理那些特权指令。为了处理这种情况，ARM没有选择修改CPU架构去实现CPU所有的特权指令都能够被捕获，从而修复之前架构关于虚拟化的缺陷，而是选择向后兼容，扩展ARMv7架构，实现虚拟化的支持。在这一点上，ARM选择了和Intel的`VTx`虚拟化技术一样的方式。

尽管ARM虚拟化扩展和Intel的方法相似，但是细节却不同。本文就是对这些细节的一些探索。现在，我们提出一些有意思的问题：如何在不增加运行在虚机侧的应用程序的可信计算基（TCB）的前提下，将虚拟化技术集成到Genode操作系统框架中？更重要的是，如何保证虚拟机的VMM各自独立，互不干扰？方法是否与[NOVA OS 虚拟化架构](http://hypervisor.org/)相似？

## 1 总体架构

ARM虚拟化扩展是基于`TrustZone`安全扩展的。关于`TrustZone`，可以参考一篇文章[https://genode.org/documentation/articles/trustzone](https://genode.org/documentation/articles/trustzone)。为了实现在不同虚机之间进行切换，ARM架构引入了一个新的特权级别-`hyp`模式。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/ARM%E8%99%9A%E6%8B%9F%E5%8C%96%E6%89%A9%E5%B1%95/images/arm_virtual_extension_1_1.png">

上图展示了`ARMv7`架构处理器的所有特权级别和工作模式。虚拟机只能运行在TrustZone的非安全空间中。那些用来支持虚拟机隔离和切换的指令只能在`hyp`或`monitor`模式下可用。因此，第一个设计问题来了：hypervisor的代码是应该运行在非安全模式下，还是其中的部分代码可以运行在安全模式下？与虚拟化无关的用户态程序是应该运行在安全模式还是非安全模式？

到目前为止，Genode代码既可以在非安全模式，也可以在安全模式下执行，具体却决于平台。比如，使用`Pandaboard`开发板时，引导阶段，安全空间已经被OEM厂商的固件程序锁住，那Genode只能运行在非安全模式下。相反，在`ARNDALE`开发板上，Genode系统可以自由使用安全和非安全空间。所以，假设我们忽略安全和非安全的概念，只讨论非安全的情况。因此，Genode能够适应所有可能的平台，包括那些`TrustZone`特性已经被供应商锁定的平台。

我们讨论的平台是`Genode+ARM核`。也就是说，Genode直接运行在裸机之上，而不是运行在第三方内核上。英文原文中，称为`base-hw`，我们将其理解为`Genode`基础硬件平台。在这样的平台上，Genode的Core进程的部分代码运行在内核模式（PL1），但是，大部分代码运行在用户模式（PL0）。这样设计的根本原因，我们在[TrustZone](http://genode.org/documentation/articles/trustzone)文章中解释过。在此基础上，我们通过在`hyp`模式下（PL2）运行`hypervisor`专有代码，这样我们得到的系统既可以访问全局的硬件资源，又可以运行在三个特权级别下。下图就是一个托管不通虚拟机的基础平台的鸟瞰图。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/ARM%E8%99%9A%E6%8B%9F%E5%8C%96%E6%89%A9%E5%B1%95/images/arm_virtual_extension_1_2.png">

理想情况下，core进程运行在hyp模式下的代码，只是与hypervisor相关的代码。这部分代码完成虚机的切换，包括称为Dom0的Genode系统。为了尽可能的降低hypervisor的复杂性，hypervisor不管任何设备的模拟。如果可能，最好就是只加载通用目的和系统寄存器，完成客户机物理地址到主机的物理地址转换。

相比于极其简单的hypervisor，用户态的VMM可以很复杂，而不会影响系统的安全。它可以包含负责的设备模拟，分配硬件资源比如内存和中断给VM。VMM就是运行在非特权级别下的普通程序，可以为每个VM实例化一个VMM。通过实例化单独的VMM，不同的VM彼此之间实现隔离。即使是在一个VM奔溃的情况下，其它VMs也不会受影响。当然了，普通的用户态程序是不能直接使用硬件虚拟化扩展的。这些扩展，只有在`hyp`模式才可用，所以是内核独有的。为了共享虚拟机的状态，需要在VMM和内核之间建立接口。Core进程提供VM虚拟机服务。每个VM对应于该服务的一个会话。这个会话提供下面所示的接口：

* **CPU state**

    该功能返回虚拟机状态的数据。启动VM时，VMM会给虚拟机一个初始状态，VM进行切换时，hypervisor就会更新CPU的状态数据。VMM用这些数据来解释客户机OS的行为。更重要的是，它可以在VMM为VM模拟了指令后更新。这种机制可以与x86架构里的VMCS结构进行比较。

* **Exception handler**

    注册异常处理程序，当VM产生虚拟化Fault时，该异常处理程序就会被通知。

* **Run**

    run功能可以用于启停虚拟机。

* **Pause**

    pause功能将VM从内核的调度器中移除。

接下来，我们主要讨论实现它的技术难点。

## 2 引导进入Genode的Dom0

为了开始实机环境的虚拟化扩展探索，我们选择使用ARNDALE开发板，该平台包含一个`Samsung`的`Exynos5 SoC`芯片，由2个`Cortex A15`的CPU核组成。选择这个开发板的目的是，已经有许多设备驱动程序可用。更重要的是，该开发板还提供UART和JTAG连接器，这在调试程序的时候是非常方便的。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/ARM%E8%99%9A%E6%8B%9F%E5%8C%96%E6%89%A9%E5%B1%95/images/arm_virtual_extension_1_3.png">

上电复位后，ARNDALE开发板首先被引导到TrustZone的安全空间中。如果我们想在非安全空间中运行整个系统，第一步就是引导进入`hyp`模式。在离开安全模式之前，我们需要做如下的调整，允许非安全空间下可以访问所有的硬件资源。非安全访问控制寄存器（`NSACR`必须被配置，允许访问所有的协处理器，并允许调整辅助控制寄存器（`ACTLR`）的多核处理器相关标志位。还有，所有中断必须在中断控制器上被标记为非安全的（`Non-Secure`），保证能够被正常接收。

当我们在实现上述内容的时候，发现了一个问题：私有化中断的问题。我们知道，ARM的通用中断控制器（`GIC`）被模块化了两部分：一个通用模块，称为分配器（`distributor`）；每个核都与之有对应的接口。正常情况下，分配器调整的所有属性涉及到所有的CPU核，包括决定中断是应该被安全还是非安全模式接收的中断类型等。但是，每个CPU核还有32个私有中断。在我们的第一次实验中，引导CPU核将所有中断标记为非安全模式下的中断。刚开始，一切都是好的。但是，当第二个核收到了来自于第一个核的核间中断（IPI）时，而IPI是一个CPU核私有中断。我们知道，在初始化阶段，每个CPU核应该自己初始化自己的安全分类，而不仅仅是靠第一个CPU核。

在解决了中断控制器的问题之后，我们接下来配置`安全配置寄存器（SCR）`：

* 使能HVC调用

* 禁用SMC调用

* 没有异常陷入到安全空间，有效地锁定安全空间

* 切到非安全空间

完成这些步骤之后，CPU进入了`hypervisor`模式。在启动常规内核的引导过程之前，CPU必须跳出`hypervisor`的特权级PL2，进入内核的运行特权级PL1。在hypervisor模式下运行未经修改的已有内核代码是不行的。有一些不兼容性会阻止在PL2模式下运行PL1模式的代码。例如，在hypervisor模式下，使用LDM指令访问特定于用户模式的寄存器或从异常返回时，结果是为定义的。为了从运行内核的低特权级重新进入hypervisor模式，必须要做一些准备工作。首先，应该准备一个异常向量表，该表就是与hypervisor相关的异常绑定的处理函数。发生异常，就会陷入这些函数中执行。可以通过`hyp向量表基地址寄存器（HVBAR）`设置hypervisor的异常向量表。这样，就可以愉快地在hypervisor模式之外，引导内核了。

## 3 内存虚拟化

ARM虚拟化通过2个阶段的地址转换扩展了之前的MMU硬件单元，因此，有效地实现了嵌套分页。更重要的，ARM架构为了在hypervisor模式下使用虚拟内存，为其引入了专用的MMU。

为了克服32位可寻址物理地址的限制，ARM提供了一种新的页表格式，将物理可寻址范围扩展到40位。这就是所谓的`大物理地址扩展（LPAE）`，也是虚拟化扩展需要的。第2阶段的客户机物理地址到主机物理地址的页表，以及hypervisor的页表都需要这种新格式的页表机制。当使用普通MMU，实现虚拟地址到物理地址或者客户机虚拟地址到客户机物理地址的转换时，可以决定是使用新的页表格式，还是沿用旧页表格式。

因为我们计划在不同的特权级别上运行Core进程，从而共享一个内存，好像使用相同的页表更为有利。因为hypervisor模式要求新页表，所以，我们决定在先前已有的内核上实现新的页表。这种方法的好处是，我们可以使用内核作为验证新页表的测试平台。更重要的是，它支持比较测量。旧页表格式最多有2级深度，而新页表格式有3级。因此，可以做一些性能比较。

`Genode`基础硬件平台集成了这种新页表格式后，事实上，确实有轻微的性能损失。作为运行时测试，我们只在Genode本身的基础上，编译Genode的Core进程。在编译的过程中又会产生许多进程，它们会不断重复使用地址空间。这就强调了TLB表项的替换以及MMU的页表遍历功能。尽管这种测试大量使用了MMU，但是，它代表了一种真实的使用场景，而不是人为的一种微基准测试。因此，它有希望更精准的证明有效性能负载。如表1所示，使用新的3级页表格式，相对于编译测试，运行时负荷小于1%。

为了是现在所有不同的特权级别上始终为Core进程提供一致的内存视图，也就是先前在用户态和内核态共享的页表，也必须给hypervisor专用的MMU使用。因此，除了[引导进入Genode的Dom0]()一节中描述的准备工作之外，我们还需要设置页表基地址，使用寄存器`HTTBR`（`hyp translation table base register`）。另外，我们还需要设置page属性，使用寄存器`HTCR`（`hyp translation control register`）和寄存器`HSCTLR`（`hyp memory attribute indirection registers`）。最后，我们为hypervisor使能MMU和Cache，使用寄存器`HSCTLR`（`hyp system control register`）。

最后，我们创建一个`1:1`的客户机物理地址到主机物理地址的转换页表，并使能正常Genode系统的第二阶段的页表转换，进行嵌套分页测试。寄存器`VTTBR`（`virtualization translation table base register`）、寄存器`VTCR`（`virtualization translation control register`）、寄存器`HCR`（`hyp configuration register`）、被用来设置第2阶段的页表地址，分别设置页表属性和使能嵌套分页。很自然地，在hypervisor模式下可以独占使用。是能了第二阶段的转换后，我们重新进行了测试，发现使用了嵌套分页的方式，比没有嵌套分页的旧页表格式运行负载增加了，大约为`3.7%`。

表1 测试结果

| 页表格式 | 持续时间(ms) | 负载 |
| -------- | ------------ | ---- |
| 2 level, 1 stage  | 214 | |
| 3 level, 1 stage  | 216 | 0.9% |
| 3 level, 2 stages | 222 | 3.7% |

考虑到使用嵌套分页时的性能下降，我们决定只有在切换到客户机OS时，启用虚拟MMU，只要是在Genode本身中运行时，将其禁止。使用这种方法，我们在开发的后期观察到了一个有趣的副作用。

为了在不同进程之间切换时，减少TLB维护操作的必要性，ARM架构引入了`地址空间标识符-ASID`的概念，过去，我们用它标记TLB中的项，也就是我们常见的`TAG`标识符。因此，只有被标记为当前有效的ASID的TLB项才是有效的。现在MMU不仅在地址空间之间共享，还在虚拟机之间共享，又添加了一个额外的`虚拟机标识符-VMID`。可以在`VTTBR`寄存器中进行设置，该寄存器负责设置第2阶段的页表。对于我们需要记住的就是，不管是否启动嵌套分页，VMID是用来标记TLB项的即可。因此，保留`VMID=0`给`Dom0`使用，其余的255个VMID用于识别其它客户机。

## 4 CPU虚拟化

完成内存虚拟化后，接下来的步骤是实现关于CPU状态的全局切换。
After experimenting successfully with memory virtualization, the next consequential step was to realize the world-switch regarding the CPU state. We started with a very simple VMM that used the VM session interface described in section Overall Architecture to provide the VM's initial register set to the hypervisor. The register set, denoted as VM state in the following, at first comprised merely the general purpose registers (r0-r15), the "current program status register" (CPSR), and "banked" copies of some of these registers related to the different execution modes.

At first, the VMM requests the dataspace containing the VM state via its VM session from core. It prepares the state by providing corresponding reset values. Using the VM session, it registers a signal handler and starts the execution of the VM. After that, the VMM waits for virtualization events that are delivered to its signal handler.

When a VM session is opened, the core process creates a new VM object. This object comprises the dataspace for the VM state, the VMID, and the initially empty second-stage page-table used for guest-physical to host-physical translations. When the session's client issues a run call, core adds the VM object to the scheduler. If the scheduler selects the VM to run, all mode-specific banked registers are reloaded and a hypervisor call is triggered using the "hvc" instruction. The hypervisor responds to this call by first checking whether the virtualization MMU is enabled. If disabled it infers that the call was triggered by the Genode host system, not by a VM. In that case, the hypervisor assumes a host-to-VM switch.

For the actual switch, the virtualization MMU is enabled, the VM's second stage table and VMID are loaded into the VTTBR, and the trap behavior is configured. Whenever a co-processor is accessed, including access to the system co-processor (CP15) that contains all system registers, the VM enters the hypervisor. This trap control is managed via the "hyp coprocessor trap register" (HCPTR), the "hyp system trap register" (HSTR), and the HCR. Finally, the hypervisor loads all the VM's general-purpose registers and the CPSR, and drops its privilege level to actually execute the VM. The detailed process of VM creation and execution is depicted in the figure below.

When the virtual machine is trapped, the "hyp syndrome register" (HSR) is updated by the hardware. The HSR contains the type of exception and additional information specific to the exception. Among such syndromes are an attempt to access the system's co-processor, a page fault related to the second stage page tables, and a hypervisor call. Additional exception-specific information encoded into the HSR are for instance the source and destination registers when accessing a system register. In addition to the HSR, there are other system registers containing information about the guest-virtual address that caused a page fault (HDFAR and HIFAR) as well as the page-aligned guest-physical address that produced a fault (HPFAR).

The information provided via the HSR, HDFAR, HIFAR, HPFAR, and the general-purpose registers principally suffices for the emulation of virtual devices. There is no need to walk the page tables of the guest OS, access its memory, and decode instructions to gain the relevant information. It was a smart decision by ARM to encode the exception related information already known by the hardware into these hypervisor-specific trap registers. It does not only simplify the VMM software but also alleviates the need of the VMM to access the VM's dedicated memory. Such accesses may suffer from ARM's weak cache coherency.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/ARM%E8%99%9A%E6%8B%9F%E5%8C%96%E6%89%A9%E5%B1%95/images/arm_virtual_extension_1_4.png">

Given the above insights, we decided to extend the VM state by HSR, HDFAR, HIFAR, and HPFAR. When switching from a VM to the host system, these fields of the VM state are updated within hypervisor mode. After core signaled that the VM is trapped, the VMM can use the updated state information to handle the exception.

In our experiments, until now the VMM just started a virtual machine with a given set of registers. No physical memory was reserved for the VM, not to mention the missing load of the guest OS binary to memory. As we don't want to trust the VMM more than any other user-land application, we can't let the VMM control the VM's memory via the second stage table directly. If a VMM had direct access to the nested pages, it might provide access to any memory area to its VM including memory containing the hypervisor itself. Therefore, we had to extend the VM session interface with the facility to populate the guest's physical memory. To enable a VMM to assign host-physical memory to the guest-physical address space, it first needs to proof that it has appropriate rights to access the host-physical memory. This proof is provided in form of a dataspace capability. The kernel adds the host-physical memory referenced by the dataspace to the second-level page table at a guest-physical address specified by the VMM.

To test the preliminary world-switch routine as well as the VM's physical memory-control functions, we implemented a very simplified guest OS kernel. This kernel does not do anything except from accessing some of its binary data and afterwards provoking a hypervisor call. The altered VMM first copies the simple kernel image into the dataspace it previously requested via its own RAM session. Then it uses the VM session interface to attach the same dataspace to the guest-physical memory at the address the test kernel is linked to. The actual incorporation into the second-stage page tables of the VM is done by the core process. After that the VMM prepares the VM state and starts the virtual machine. Whenever a virtualization event is signaled to the VMM, it dumps the whole VM state as debug output and halts the VM.

The first trap the VM went into was the very first instruction. But in contrast to our assumptions, the reason was not a misconfiguration related to the second-stage page tables. The information gained by the hypervisor-specific fault registers helped us to identify the problem. The exception got raised by the MMU that tried to resolve the first instruction's address. As we had not considered the "system control register" (SCTRL) in the world switch at that point, all of its properties applied to the VM execution, too. This includes the MMU and caches being enabled. By adding the SCTRL register to the VM state, setting the correct reset value (no MMU) in the VMM, and reloading the SCTRL register whenever switching between the VM and the Genode host system, we were able to run our simple kernel test to the point where it calls the hypervisor explicitly.

At this point, we had a solid foundation to start virtualizing a real guest OS. Naturally, we choose an open-source system as candidate to be able to easily determine what the OS is trying to do when raising virtualization events. Linux was our first candidate as it supports the target hardware best and we were most experienced with it. We picked ARM's Versatile Express Cortex A15 development board as virtual hardware platform to provide it to the Linux guest. Being the reference platform of ARM, documented predominantly well, and with QEMU having support for it, the platform seemed promising.

We used the standard configuration for Versatile Express platforms to compile the latest vanilla Linux kernel. Nowadays in the embedded world, the hardware description is separated from the Linux source code by using so-called "device tree binaries" (DTBs). To avoid the need to virtualize the entire hardware at once, we created a minimal device tree for the Versatile Express board that only contains one CPU core, the interrupt controller, timer, one UART device, and the necessary clock and bus configuration. After successful tests of the minimal DTB together with the unmodified Linux kernel on QEMU, we used our VMM to load both kernel and DTB into memory, and start its execution.

From then on, we went step by step from one virtualization event to the next. As the hypervisor prepared the machine to trap the VM whenever a system register or non-existing physical address is accessed, we could clearly see what the Linux guest is doing in what order. By incrementally emulating system registers, optionally adding them to the VM state where necessary, and disabling trap behavior where appropriate, we completed the CPU virtualization step by step. For instance, all it takes for emulating certain identification registers is returning meaningful values. But for other registers, the access must be forwarded to real hardware. For instance, a change of MMU attributes like the current page table, cannot be emulated but needs to be propagated to the hardware. Those system registers need to be saved and restored by the hypervisor. Therefore, they were added to the VM state successively. When system registers have to be reloaded anyway, in general they also can be accessed by the guest VM directly without the need to trap. Moreover, there are system registers used to maintain TLB, instruction-, and data-caches, which use the currently active VMID and ASID to e.g. flush cache lines associated with the current process respectively VM. Those operations are uncritical and should be done directly by the VM.

Fortunately, ARM enabled fine-grained control regarding the trap behavior of the system co-processor registers. For other co-processors, it is a binary decision whether the VM shall be trapped or not whenever accessing them. The trap behavior with respect to the system coprocessor is divided into 14 different sections, which are controlled via the "hyp system trap register" (HSTR). One of these sections for instance comprises all TLB-maintenance-related system registers. The principle development procedure was to first enable all sections to trap the VM as soon as it accesses one of its registers. If all system registers within a section are reloaded by the hypervisor anyway, the trap behavior of that section can be disabled and direct access can be granted to the VM. As the VMM records each register trap, we could also identify hot spots in the system register set, e.g., the SCTRL register.

Without optimizing at all, and by just giving direct access to TLB and cache maintenance registers, we identified a minimal system register set that needed to be reloaded by the hypervisor. Table 2 shows the minimal register set.


| Section | Abbreviation | Name |
| ------- | ------------ | ---- |
| 1       | SCTRL        | System Control Register  |
| 1       | CPACR        | Coprocessor Access Control Register | 
| 2       | TTBCR        | Translation Table Base Control Register |
| 2       | TTBR0        | Translation Table Base Register 0 |
| 2       | TTBR1        | Translation Table Base Register 1 |
| 3       | DACR         | Domain Access Control Register |
| 5       | DFSR         | Data Fault Status Register |
| 5       | IFSR         | Instruction Fault Status Register |
| 5       | ADFSR        | Auxiliary DFSR |
| 5       | AIFSR        | Auxiliary IFSR |
| 6       | DFAR         | Data Fault Address Register |
| 6       | IFAR         | Instruction Fault Address Register |
| 10      | PRRR         | Primary Region Remap Register |
| 10      | NMRR         | Normal Memory Remap Register |
| 13      | CIDR         | Context ID Register |
| 13      | TPIDR 0-2    | Software Thread ID Registers | 

At least these 16 system registers in addition to the 37 core registers had to be reloaded by the hypervisor on each switch from host to guest system and vice versa.

## 5 中断虚拟化

At a very early step in development, we recognized that if the hypervisor does not configure the trap behavior accordingly, device interrupts under control of the Genode host system where received within the VM. To that effect, the hypervisor has to direct all interrupts to itself whenever switching to a VM. To minimize the overhead, this trap behavior is disabled again when switching back to the host system. Correspondingly, when interrupts occur while normal Genode applications are executed, the hypervisor code isn't involved at all.

During the completion of the CPU model described in the previous section and following the VM's execution step by step, we crossed Linux' initialization routine of the interrupt controller. In addition to the CPU, ARM introduced virtualization extensions for its interrupt controller, too. As explained in section Bootstrap into Genode's "Dom0", ARM's generic interrupt controller (GIC) is split into a core-local CPU interface and one global distributor interface.

With hardware virtualization present, for each core, a "virtual CPU interface" (GICV) is available. Each GICV is accommodated by a dedicated "virtual CPU control interface" (GICH). The GICV is meant as transparent replacement for the normal CPU interface and is used directly by the VM. The control interface is preserved to the hypervisor respectively VMM to manage interrupts appearing at the GICV.

In contrast to the CPU interface, there is no special virtualization support for the global distributor implemented in hardware. Therefore, the VMM has to emulate this device for the virtual machine. Whereas the CPU interface is frequently accessed by the guest, the distributor is supposedly rarely touched, mostly during initialization. Therefore, providing optimization via special hardware support for the CPU interface suggests itself.

At the beginning, we implemented an almost empty shape of a virtual GIC. Without backing the physical address where normally the Distributor interface resides with memory, the VM always triggers a virtualization event when trying to access it. For all GIC registers accessed by Linux, the VMM records their states but does not implement any logic. Thereby, Linux first successfully went through the initialization of the distributor.

To make use of the new virtualization features of the interrupt controller, we decided to provide the Virtual CPU interface to the VM instead of emulating the CPU interface. Therefore, we had to back the physical address range where the VM normally expects the CPU interface, with the GICV's memory-mapped I/O (MMIO) registers. Although, we already extended the VM session interface to enable the VMM to provide arbitrary dataspaces to a VM, this is not appropriate for attaching the GICV's MMIO registers. Given that more than one VMM might run in parallel to control different VMs and there is only one physical GICV, we cannot grant the VMM direct access to it. Otherwise, different VMMs would interfere. On the other hand, only the VMM knows where the CPU interface shall be placed in guest-physical memory, dependent on the platform it emulates. To tackle this problem, the VM session got further extended by a specific function to attach the interrupt controller's CPU interface.

After adding the core-local interrupt controller interface to the memory layout of the VM, the Linux kernel successfully finalized the GIC initialization and continued its boot process to the point where it issued a "wait-for-interrupt" (WFI) instruction. This normally puts the CPU into a light sleep until the next interrupt occurs. When configured accordingly (via the HCR), a VM that executes a WFI instruction is trapped. The VMM recognizes that the VM is waiting for interrupts and stops its execution until the next interrupt occurs. To further follow the initialization of the Linux system, the next logical step was to fill the GIC model with life and inject interrupts to the VM.

The first result was a very simple interrupt controller model. It ignores multi-processor support, priorities, and the assignment of different security levels (TrustZone) for interrupts. Whereas the former is definitely a must-have feature when supporting more than one CPU for the VM, priorities and the security extensions are not used by our Linux guest kernel anyway. As all-encompassing priority support would make the state-machine of the GIC much more complex, it is desirable to omit it if possible. The current GIC model holds state for the individual interrupts, whether they are enabled or not, and whether the interrupt controller itself is in charge.

To signal an interrupt to the VM, the GICV's control interface can be used. It is under exclusive control by the core process (hypervisor), not the VMM. This design prevents interferences between different virtual machines and monitors. Our first intuitive approach, following the micro-kernel paradigm, was to minimize complexity within the hypervisor by just reloading the GICH registers when switching between different VMs. We introduced a shadow copy of the GICH registers into the VM state. The VMM operates on the shadow copy only. When the hypervisor performs a world-switch, it loads the shadow GICH state into the real GICH.

When dealing with the very first interrupts, limitations of this approach appeared. The first interrupt source that got implemented was the virtual timer that is used by the Linux guest OS for scheduling. Due to reasons explained in the following section, the interrupt of the virtual timer needs to be shared by different VMs and thus is handled directly within the kernel. Therefore, the timer interrupt either had to be injected by the kernel directly or its occurrence had to be signaled to the VMM, which in turn would inject the interrupt via the shadowed GICH registers. The first approach requires synchronization between kernel and VMM regarding the GICH register set of the VM, which is undesirable as the kernel must never depend on user-land behavior. On the other hand, as we show below, it is not sufficient to inject the interrupt when a device is handled by the VM directly, like in the case of the timer.

Each time a device interrupt is received by Genode's host kernel, the kernel masks the interrupt at the distributor until the corresponding device driver handled it. So the interrupt does not re-occur in the mean time. In a monolithic kernel, one can omit this masking. The kernel calls the top half handler of the interrupt, which is a small routine that acknowledges the interrupt directly at the corresponding device so that the interrupt signal at the interrupt controller vanishes. In a micro-kernel-based system like Genode where device drivers run like normal applications unprivileged in user land, this is not the case. It is in general not prior known when a device driver will have handled a related interrupt at the device. Therefore, the kernel for the time being masks the interrupt at the controller until the associated device driver signals that the interrupt got handled. For the virtualization case, this means if a VM uses a device directly by itself, for instance the virtual timer, the completion of the interrupt handling needs to be recognized. Otherwise the host kernel could not unmask the corresponding interrupt again. Fortunately, ARM has made provisions for this situation with the virtualization extensions. If configured accordingly, an injected interrupt can trigger a special maintenance interrupt as soon as the guest OS marks that interrupt as handled.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/ARM%E8%99%9A%E6%8B%9F%E5%8C%96%E6%89%A9%E5%B1%95/images/arm_virtual_extension_1_5.png">

To sum up, the VMM implements the interrupt handling of the VM. Whenever an interrupt of a physical device under control of the VM occurs, the kernel signals that interrupt to the VMM. The VMM injects the interrupt via the GICH registers in the VM state and resumes the VM. The kernel, in turn, reloads the GICH registers when switching to the VM. Within the VM the interrupt occurs. The guest OS kernel handles the interrupt at the related device and acknowledges the interrupt at the interrupt controller's Virtual CPU interface. This triggers a maintenance interrupt at the hypervisor, which forwards it to the VMM. The VMM responds to the maintenance interrupt by acknowledging the original interrupt at the kernel. The kernel unmasks the interrupt again and the whole process is completed. The sequence diagram in the figure above illustrates the delivery of a device interrupt dedicated to one VM. Assuming this procedure occurs while the VM is running, it will take four world switches and at least four kernel-VMM switches until the VM is able to proceed. This indicates high interrupt latencies.

Although the overhead of handling interrupts that are directly assigned to a VM seems to be quite significant, we decided to start with it and optimize later if necessary. The advantage of this pure architecture is that the hypervisor does nothing more than saving and restoring the registers of the Virtual CPU control interface from the VM state. Thereby, we strictly follow the principle of minimizing the common TCB.

The VMM controls the GICH registers and injects interrupts whenever necessary. If an interrupt shall be associated with a VM directly, the VMM gains access to it like any other device driver would do in Genode: It opens an IRQ session at the core process. In contrast to that, virtual timer and maintenance interrupts of the interrupt controller cannot be obtained via an IRQ session. They are shared between all VMMs. When they occur, only the VMM of the currently active VM shall receive it. Such an interrupt is signaled by the hypervisor to the VMM implicitly via the VM state. When the VMM recognizes that the VM was stopped due to an interrupt, it reads the interrupt information out of the VM state.

In contrast to devices under direct control of a VM, a VMM might provide emulated devices. These devices normally use services of the host system as backend. Section Interacting with the guest OS describes an example in form of an UART device that uses Genode's terminal service. If such emulated devices need to inject interrupts into the VM, the process is less complex. The successful handling of such virtual interrupts in contrast to physical ones does not need to explicitly signaled to the VMM.

## 6 Virtual Time

As has been mentioned in the previous section, one of the first devices that are used by the Linux kernel during initialization is the timer. Virtualizing time is a critical operation. When implementing a timer by the trap-emulate approach, significant performance penalties due to the frequent access are inevitable. To circumvent these foreseeable problems, ARM added virtualization support for its core-local generic timer, which is accessed via co-processor fourteen (CP14).

When starting to enable ARM's generic timer, we encountered serious problems. When originally enabling support for the ARNDALE board, we failed to use ARM's timer but used Samsung's own multi-core timer instead. The documentation of Samsung's Exynos 5 SoC, the one used in the ARNDALE board, does not even mentioned the generic timer at all. By following an interesting discussion on the ARM Linux kernel mailing list, involving a developer from Samsung, it turned out, that Samsung's multi-core timer and ARM's generic timer in fact use the same clock on this SoC and that certain bits in the Samsung timer need to be enabled so that the ARM timer runs successfully. Another minor issue was the frequency the timer runs at. It needs to be configured in the secure world. After sailing around these cliffs, we could finally continue examining the timer's virtualization support.

ARM's generic timer is extended by a virtual timer, which is coupled with the normal physical one. It counts with the same frequency but has an offset with respect to the physical reference that can be set. When switching between different virtual machines, the register containing the actual counter value and the control register of the virtual timer need to be saved and restored. Apart from that, the VM has direct access to the counter and always gets actual values. No trapping is needed.

When the counter of the virtual timer reaches zero, an interrupt occurs, if this is enabled in the control register. However, in contrast to other devices that can be associated with exactly one device driver or VM, the virtual timer is potentially shared among several VMs. Therefore, the virtual timer interrupt plays a special role. It should be delivered only to the VMM with the currently active VM. This process was described in depth in the previous section.

If a VM gets interrupted, it's virtual time counter is stored and the virtual time stops. This is acceptable for relatively short periods of time, e.g., during the handling of a virtualization event. But if a VM gets preempted permanently, for instance after the guest OS signaled that it waits for interrupts, virtual time needs to proceed. At this point the ARM's virtual timer hardware is of little use. We cannot program the timer for an inactive VM and use it for another active VM at the same time. Instead, another time resource needs to be used to monitor time progress of an inactive VM. Therefore, whenever the VMM recognizes that the VM will stop execution for a longer time, it uses the last virtual timer counter value from the VM state to program Genode's timer service. When the timer service signals that the time period expired, the VMM resets the timer counter and injects a virtual timer interrupt.

## 7 与客户机通信

An essential device model needed during the first steps was the UART. At first, we implemented a very simple device model in the VMM without interrupt support that did not provide any characters to the VM (RX direction) but merely printed characters transmitted by the VM (TX direction). By enabling early debug messages in the Linux kernel, it was possible to again and again compare these messages with the output of the Linux kernel running within QEMU. This helped a lot to detect problems by identifying differences in the output.

After the boot process was finished, we liked to use the UART device to first communicate with the guest OS interactively. Therefore, the device model was extended and the former print backend was replaced by Genode's terminal service. By injecting an interrupt each time a character is received from the terminal and trapping each time the VM tries to read from or write a character to the UART registers, we gained a functional though slow serial console. This performance limitation, however, is not special to our virtualization approach but due to the working of UART devices, which are ungrateful for virtualization in general.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/ARM%E8%99%9A%E6%8B%9F%E5%8C%96%E6%89%A9%E5%B1%95/images/arm_virtual_extension_1_6.png">

## 8 I/O MMU for device virtualization

When hosting virtual machines, the direct assignment of a physical device such as a USB controller, a GPU, or a dedicated network card to the guest OS running in the virtual machine can be useful in two ways. First, if the guest OS is the sole user of the device, the direct assignment of the device maximizes the I/O performance of the guest OS using the device. Second, the guest OS may be equipped with a proprietary device driver that is not present in the host OS otherwise. In this case, the guest OS may be used as a runtime executing the device driver and providing a driver interface to the host OS. In both cases the guest OS should not be considered as trustworthy. In contrary, it bears the risk to subvert the isolation between components and virtual machines. A misbehaving guest OS could issue DMA requests referring to the physical memory used by other components and even the host OS kernel and thereby break out of its virtual machine.

Accompanied with the virtualization support, ARM introduced an "I/O memory management unit" (IOMMU) fitting their architecture that is called "System MMU" (SMMU). In general, an IOMMU translates device-visible virtual addresses to physical addresses analogously to traditional MMUs used by a CPU. It helps to protect against malicious or faulty devices, as well as device drivers. Moreover, it simplifies device virtualization. When a device dedicated to a specific guest OS initiates DMA transfers, it will potentially access wrong address ranges. A DMA capable device is programmed by the guest OS' driver to use certain physical addresses therefore. But the guest-physical memory view differs from the actual host-physical one. Here the IOMMU comes into play. It translates the device memory requests from guest-physical to host-physical addresses.

While exploring the ARM virtualization support, the following question arose: How to provide direct device access to a virtual machine (VM)? From our prior experiences, we had a notion of how to separate device drivers from the rest of the system via IOMMUs of the x86 platform. But on ARM, we had no platforms equipped with such technology yet. Therefore, a further question was: How can ARM's IOMMU be integrated in a microkernel-based operating system like Genode?

As it turned out, the Samsung Exynos5 SoC of the ARNDALE board doesn't make use of ARM's SMMU but uses its own derivative SysMMU. Nevertheless, given the reference-manual description, both IOMMUs are quite similar. Although Samsung's SysMMU seems to be less complex than the ARM variant.

## 9 Guest OS DMA attack

Before exploring the SysMMU available on our development platform, we decided to implement an attack used as test vehicle. A simple driver that uses a DMA capable device to read or override Genode's kernel binary seemed to be an appropriate example. When looking for a proper device, we naturally hit on the DMA engine available on the Samsung SoC. This DMA engine is used by some peripherals that do not have a dedicated DMA engine on their own. Moreover, it includes several DMA channels to perform memory-to-memory copy transactions. Therefore, it seemed to perfectly fit our demands. However, after studying the functional description of the DMA engine superficially, it turned out to be more complex than anticipated. As we did not want to implement overly complex test drivers, an alternative approach was chosen. Given our minimal virtualization environment, we were able to run a simple Linux guest OS within a VM. If we would extend the virtual environment with direct access to the DMA engine, the Linux kernel could perform the attack, as it already contains a driver for the corresponding device.

The first step was to add the DMA engine to the "device tree binary" (DTB) of the Linux guest. As described in section CPU Virtualization, the virtual environment delivered by the VMM differ from the underlying hardware platform. Instead of providing a virtual ARNDALE board, we use a kernel and minimal DTB built for ARM's Versatile Express Cortex A15 development board. Luckily, the original DTB hardware description of this board already contained the same DMA engine like the one on the ARNDALE board. Therefore, we only needed to add the original DMA-engine description to our minimal DTB again.

After booting the Linux guest OS, the kernel was not able to initialize the DMA engine. As it turned out, the platform driver of the Genode host OS, responsible among others for clock and power domains, disabled the device when initializing clock and power controllers. After fixing this little problem, the kernel was finally able to identify and initialize the DMA engine.

To execute the actual attack, we adapted a DMA engine debug test that is already present in the Linux kernel. It uses the generic DMA API of the kernel to run a number of tests on the corresponding engine. We altered the test module in a way that it tries to copy the Genode OS host kernel to some empty Linux guest memory. At that stage, we had no IOMMU support in place. Therefore, we calculated the offset of guest-physical to host-physical memory to the addresses handed over to the DMA API. Nevertheless, the expected effect did not occur. The target memory always remained untouched. But the DMA engine reported that it completed the transaction successfully. After further inspecting the DMA engine's device driver of the Linux kernel, it turned out that the engine fetches micro-code from memory to execute its operations. The micro-code is written by the driver, and its physical address is provided to the device by the driver too. In other words: to be able to execute a DMA transaction, the engine already initiates a DMA transaction before.

After applying the guest-to-host physical memory offset to the instruction pointer's address of the DMA engine device driver, the attack finally worked as expected. We were able to read out and overwrite the host OS kernel from the guest OS.

## 10 Limiting DMA transactions

To prevent DMA attacks driven from the guest OS, the next step was to put the SysMMU into operation. As it turned out, there is no single SysMMU in the SoC, but a bunch of different ones sitting in front of different devices. As the the SysMMU is not capable to differentiate requests from its source bus, the SoC is required to use a dedicated SysMMU in each case a device shall be separated from others. On the other hand, the use of one SysMMU per device allows for a largely simplified SysMMU compared to Intel's IOMMU.

On Intel, the IOMMU relies on the PCI device's bus-device-function triplet as unique identification for DMA transactions, and it can manage different translation tables for different devices. Without having practical experiences and by only studying the reference manuals, ARM's SMMU works similar and can distinguish different devices according to the AXI bus ID. The different approaches are depicted in the following figure.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/ARM%E8%99%9A%E6%8B%9F%E5%8C%96%E6%89%A9%E5%B1%95/images/arm_virtual_extension_1_7.png">

IOMMU approaches: Intel VT-d (above) and Samsung Exynos5 (below)

The Samsung Exynos5 SoC that we used for our experiments comprises more than 30 SysMMUs in it. We identified the one situated between that part of the DMA engine, which is responsible for memory-to-memory transactions, and the memory controller itself.

To test that we target the right SysMMU, we configured it to block any request. Samsung's SysMMU has three different modes: "disable", "enable", and "block". In disable mode it just lets all transactions pass without any translation. This is the default when the system was reset. In block mode the SysMMU just blocks any request from the source bus. In enable mode it uses a translation table to look up device-virtual to physical translations, and a TLB to cache the mappings. Putting the DMA engine's SysMMU into block mode led to timeouts of all DMA transactions of the Linux guest OS, which met the expected behavior.

The next step was to put the SysMMU into enable mode. Thereby, we first did not define a valid translation table. To be informed about translation faults or other failures, each SysMMU provides a dedicated interrupt. When trying to determine the interrupt number of the DMA engine's SysMMU, a new issue arose. In contrast to most other ordinary device interrupts, the ones originating from the SysMMUs are grouped. That means they partially share the same interrupt line. To be able to distinguish the different sources within one interrupt group, a simplified interrupt controller for each group exists, the so called interrupt combiner. The interrupt combiners are connected to the global interrupt controller. Genode's integrated ARM kernel did not made use of these combiners, as they were not necessary for the device drivers already present. Unfortunately, it is not enough to merely enable the interrupt combiners. A namespace of virtual interrupt numbers have to be introduced to consider the different interrupts of one group. Instead of solving that problem by design, we chose the pragmatic path. At this point, we only enabled the interrupt for the corresponding SysMMU and ignored all other interrupts of the same group. The interrupt is primarily used for error detection and not necessarily needed for the correct functioning of the SysMMU. Therefore, it seemed feasible to use an interim solution for the time being.

Now that interrupts could principally be delivered to the SysMMU driver, the device signaled a bus error. This was expected. As we did not set an address to a translation table for the device, it used an invalid bus address to access its table. After setting up a valid portion of memory initialized with zeroes and propagating its physical address to the SysMMU as translation table, we finally received an interrupt indicating a translation-fault correctly. Finally, we had to set up a valid translation table containing the guest-physical to host-physical translations of the Linux guest OS memory. The translation table format of Samsung's SysMMU is a compatible but simplified form of ARM's two-level page-table format. In contrast to the original model, the SysMMU derivative does not include some memory attributes such as cache-policy attributes. After putting the populated translation table into effect, the Linux guest OS was finally able to do DMA memory-to-memory transactions within its own memory areas, but failed to access ranges outside of it. Moreover, all patches to the Linux kernel's DMA driver, regarding the offset calculation of guest-to-host physical memory, naturally became superfluent.

In the current example implementation, the driver for the DMA engine's SysMMU was implemented as part of the VMM. This was done for the convenience of implementation. Nevertheless, in the final design of a secure virtualization environment, we do not want to trust the VMM more than any other application or driver. Since we identified the setup of the SysMMU translation table to be critical to all programs and devices of the system, the programming of the SysMMUs has to be done by trusted components exclusively. Therefore, we aspire to incorporate IOMMUs for ARM within Genode's trusted platform driver in the future. As all device drivers in Genode depend on that driver, it is the natural place for implementing DMA protection. Note that the SysMMU handling is still outside the host kernel but implemented in user mode.

## 11 成果

The present work examines ARM's virtualization extensions as well as I/O protection mechanisms. It shows how it can be combined with a component-based OS architecture like Genode. The resulting implementation is a proof-of-concept and not yet incorporated in the mainline Genode OS framework. Although it is still a prototype, we can already draw some conclusions.

In our work, we followed the paradigm of minimizing the common TCB as far as possible by implementing nearly all virtualization-related functionality into the unprivileged VMM. The complexity of the privileged hypervisor is almost negligible. In fact, Genode's core/kernel had been supplemented by merely 600 lines of code (LOC) to support this virtualization architecture.

The further addition of features to the VMM such as sophisticated device models will not increase the complexity of the common TCB. The only omission of the current version of the hypervisor is the lack of on-demand switching of FPU contexts when switching between VMs and Genode. However, as the base-hw platform already comprises support for FPU switching for normal Genode applications, it is assumed that FPU virtualization won't add much complexity.

Another open issue is support for symmetric multi-processing (SMP) in VMs. In general, SMP is already supported by the base-hw platform on Cortex A15 CPUs. Having more than one CPU in a VM is mostly a question of enhancing the VMM accordingly. We envision to represent each virtual CPU by a dedicated VM session. The VM objects implemented in the kernel are almost identically to normal threads with regard to scheduling and can be assigned to different cores. For this, no additional code has to be implemented in the kernel. Apart from that, the VMM has to guarantee synchronicity with regard to commonly used device models when dealing with several VM sessions.

Most our the ARM-virtualization efforts described throughout this article (except for the I/O protection support) have been integrated into the [version 15.02 of the Genode OS framework](http://genode.org/documentation/release-notes/15.02#Virtualization_on_ARM).


## 12 Acknowledgements

The study was conducted by Genode Labs GmbH during the year 2014. The work was supported by funding from the German Federal Office for Information Security (BSI). We thank the BSI for the opportunity to explore and document technologies like ARM virtualization in the level of detail as presented.

## 13 参考文章

1. [ARM 虚拟化技术简介](https://blog.csdn.net/tony_vip/article/details/105890376)
2. 
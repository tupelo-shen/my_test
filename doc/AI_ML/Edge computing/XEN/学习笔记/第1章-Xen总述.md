[TOC]

本章，我们首先解释Xen与其它虚拟技术有哪些不同之处。然后，了解一些Xen是如何工作，如何把各个组件组合到一起的一些底层细节。

# 1 虚拟化原理

这个故事我们还是要从计算机说起，即使最新的、运行多任务的计算机系统，一次也只能运行一条指令。你可能会说，“不对啊，我怎么感觉我的计算机同时在运行好多任务呢？我可以同时查看时间，听音乐，下载文件，与朋友聊天等等”。这当然是真的。但是，这是计算机系统在底层以极快的速度在不同的任务间进行切换，而你无法感知这种延时而已。就像电影其实是一幅幅静止的图像，给你造成运动的错误一样，计算机上的任务也是如此无缝衔接的交织在一起，看起来好像是同时运行的。

Virtualization just extends this metaphor a bit. Ordinarily, this multiplexing takes place under the direction of the operating system, which acts to supervise tasks and make sure that each receives its fair share of CPU time. Because the operating system must therefore schedule tasks to run on the CPU, this aspect of the operating system is called a scheduler. With Xen virtualization, the same process occurs, with entire operating systems taking the place of tasks. The scheduling aspect is handled by the Xen kernel, which runs on a level superior to the “supervising” guest operating systems, and which we thus call the hypervisor.

Of course, it’s not quite so simple—operating systems, even ones that have been modified to be Xen-friendly, use a different, more comprehensive, set of assumptions than applications, and switching between them is almost by definition going to involve more complexity.

So let’s look at an overview of how virtualization is traditionally done and how Xen’s design is new and different. A traditional virtual machine is designed to mimic a real machine in every way, such that it’s impossible to tell from within the virtual machine that it isn’t real. To preserve that illusion, fully virtualized machines intercept attempts to access hardware and emulate that hardware’s functionality in software—thus maintaining perfect compatibility with the applications inside the virtual machine. This layer of indirection makes the virtual machine very slow.

Xen bypasses this slowdown using an approach called paravirtualization— para as a prefix means similar to or alongside. As the name suggests, it’s not “real” virtualization in the traditional sense because it doesn’t try to provide a seamless illusion of a machine. Xen presents only a partial abstraction of the underlying hardware to the hosted operating system, exposing some aspects of the machine as limitations on the guest OS, which needs to know that it’s running on Xen and should handle certain hardware interactions accordingly.

> Newer processors incorporate support for hardware virtualization, allowing unmodified operating systems to run under Xen. See [Chapter 12](https://wiki.prgmr.com/mediawiki/index.php/Chapter_12:_HVM:_Beyond_Paravirtualization) for details.

Most of these limitations—by design—aren’t noticeable to the system’s users. To run under Xen, the guest OS kernel needs to be modified so that, for example, it asks Xen for memory rather than allocating it directly. One of the design goals for Xen was to have these changes occur in the hardwaredependent bits of the guest operating system, without changing the interface between the kernel and user-level software.

This design goal reduces the difficulty of moving to Xen by ensuring that existing binaries will work unmodified on the Xen guest OS and that the virtual machine will, in most regards, act exactly like a real one, at least from the perspective of the system’s end users.

# 2 虚拟化技术-全虚拟化

Not all virtualization methods use Xen’s approach. Virtualization software come in three flavors. At one extreme you have full virtualization, or emulation, in which the virtual machine is a software simulation of hardware, real or fictional—as long as there’s a driver, it doesn’t matter much. Products in this category include VMware and QEMU.

> And what, you ask, is this fictional hardware? Apart from the obvious “not real” answer, one good example is the VTPM driver. TPM (Trusted Platform Module) hardware is relatively uncommon, but it has some potential applications with signing code—for example, making sure that the running kernel is the correct one, rather than a fake put on by a rootkit or virus. Xen therefore makes a virtual TPM available to the domUs.

With full virtualization, an unmodified[3] OS “hosts” a userspace program that emulates a machine on which the “guest” OS runs. This is a popular approach because it doesn’t require the guest OS to be changed in any way. It also has the advantage that the virtualized architecture can be completely different from the host architecture—for example, QEMU can simulate a MIPS processor on an IA-32 host and a startling array of other chips.

However, this level of hardware independence comes at the cost of an enormous speed penalty. Unaccelerated QEMU is an order of magnitude slower than native execution, and accelerated QEMU or VMware ESX server can only accelerate the emulated machine if it’s the same architecture as the underlying hardware. In this context, for normal usage, the increased hardware versatility of a full emulator isn’t a significant advantage over Xen.

VMware is currently the best-known vendor of full-virtualization products, with a robust set of tools, broad support, and a strong brand. Recent versions of VMware address the speed problem by running instructions in place where possible and dynamically translating code when necessary. Although this approach is elegant and doesn’t require guest OS modification, it’s not as fast as Xen, making it less desirable for production setups or for a full-time work environment.

# 3 虚拟化技术-OS虚拟化

On the other extreme is OS-level virtualization, where what’s being virtualized is the operating environment, rather than the complete machine. FreeBSD jails and Solaris Containers take this approach.

OS virtualization takes the position that the operating system already provides, or at least can be made to provide, sufficient isolation to do everything that a normal VM user expects—install software systemwide, upgrade system libraries in the guest without affecting those in the host, and so forth. Thus, rather than emulating physical hardware, OS virtualization emulates a complete OS userspace using operating system facilities.

FreeBSD jails and Solaris Containers (or Zones) are two popular implementations of OS-level virtualization. Both derive from the classic Unix chroot jail. The idea is that the jailed process can only access parts of the filesystem that reside under a certain directory—the rest of the filesystem, as far as this process can tell, simply doesn’t exist. If we install an OS into that directory, it can be considered a complete virtual environment. Jails and Zones expand on the concept by also restricting certain system calls and providing a virtual network interface to enhance isolation between virtual machines. Although this is incredibly useful, it’s neither as useful or as versatile as a full-fledged virtual machine would be. Because the jails share a kernel, for example, a kernel panic will bring down all the VMs on the hardware.

However, because they bypass the overhead of virtualizing hardware, virtualized machines can be about as fast as native execution—in fact, they are native.

OS virtualization and Xen complement each other, each being useful in different situations, possibly even simultaneously. One can readily imagine, for example, giving a user a single Xen VM, which he then partitions into multiple Zones for his own use.

# 4 半虚拟化-Xen方法

Finally, somewhere between the two, there’s paravirtualization, which relies on the operating system being modified to work in concert with a sort of “super operating system,” which we call the hypervisor. This is the approach Xen uses.

## 4.1 半虚拟化工作原理

Xen works by introducing a very small, very compact and focused piece of software that runs directly on the hardware and provides services to the virtualized operating systems.[4]

Xen’s approach to virtualization does away with most of the split between host OS and guest OS. Full virtualization and OS-level virtualization have a clear distinction—the host OS is the one that runs with full privileges. With Xen, only the hypervisor has full privileges, and it’s designed to be as small and limited as possible.

Instead of this “host/guest” split, the hypervisor relies on a trusted guest OS (domain 0, the driver domain, or more informally, dom0) to provide hardware drivers, a kernel, and a userland. This privileged domain is uniquely distinguished as the domain that the hypervisor allows to access devices and perform control functions. By doing this, the Xen developers ensure that the hypervisor remains small and maintainable and that it occupies as little memory as possible. Figure 1-1 shows this relationship.

Figure 1-1: Shown here is the hypervisor with domains. Note that the hypervisor

runs directly on the hardware but doesn’t itself mediate access to disk and network devices. Instead, dom0 interacts directly with disk and network devices, servicing requests from the other domains. In this diagram, domU 1 also acts as

a driver domain for an unnamed PCI device.

NOTE: See also “Safe Hardware Access with the Xen Virtual Machine Monitor,” Fraser et al.5 Also, non-dom0 driver domains can exist—however, they’re not recommended on current hardware in the absence of an IOMMU (I/O Memory Management Unit) and therefore will not be covered here. For more on IOMMU development, see Chapter 12.

Domain 0’s privileged operations broadly fall into two categories. First, dom0 functions as an area from which to administer Xen. From the dom0, the administrator can control the other domains running on the machine— create, destroy, save, restore, etc. Network and storage devices can also be manipulated—created, presented to the kernel, assigned to domUs, etc.

Second, dom0 has uniquely privileged access to hardware. The domain 0 kernel has the usual hardware drivers and uses them to export abstractions of hardware devices to the hypervisor and thence to virtual machines. Think of the machine as a car, with the dom0 as driver. He’s also a passenger but has privileges and responsibilities that the other passengers don’t.

If you're looinkg to buy these articles make it way easier.

# 5 put it together

In general, all of these implementation details demonstrate Xen’s focus on simplicity and code reuse. Where possible, the Xen developers have chosen to focus on providing and managing channels between physical devices and virtual devices, letting Linux userspace tools and kernel mechanisms handle arbitration and device access. Also, the actual work is offloaded as much as possible to the dom0 so as to reduce the complexity of the hypervisor and maximize device support.

For the administrator, this means that Xen can be administered and monitored, by and large, with standard tools, and that most interactions with Xen take place at the level of the dom0. When Xen is installed and domains are running, the Xen domains act like normal, physical machines, running unmodified userspace programs, with some caveats. Let’s move on to the next chapter to see how to set this up in practice.

Now I know who the brniay one is, I'll keep looking for your posts.


[^1]: [baidu](http://xueshu.baidu.com/)
[^5]: [参考文章](https://wiki.prgmr.com/mediawiki/index.php/Chapter_1:_Xen:_A_High-Level_Overview#Footnotes)
[TOC]

# 1 什么是Xen hypervisor？

`xen hypervisor`是`type-1`或者`baremetal`虚拟化管理程序，可以支持多个VM的虚拟化并运行不同的操作系统。它是唯一的`type-1`类型的开源虚拟化管理程序。可以作为商业软件或开源软件的基础构建工具，比如，服务器虚拟化、IaaS服务、桌面虚拟化、安全应用、甚至是嵌入式设备的虚拟化。亚马逊的AWS云服务最早就是基于Xen构建，现在已经转向KVM。

主要特点：

1. 占用空间小（大约1MB），采用微内核架构设计，与客户机的接口少。因此，比其它虚拟化管理程序更健壮和安全。

2. 与操作系统无关：大部分时候使用Linux作为控制管理程序（运行在Dom0上）。当然，也可以运行其它系统，比如`NetBSD`和`Open Solaris`。

3. 驱动隔离：可以让驱动程序运行在虚拟机中。如果驱动程序崩溃，可以重新引导该虚拟机，不会影响系统中其它的虚拟机。

4. 半虚拟化：使用半虚拟化+HVM的组合方式，可以优化虚拟机的运行。这使得客户机的运行速度比单纯使用HVM快得多。此外，还可以在不支持硬件虚拟化扩展的机器上运行。

本文主要介绍Xen工程的架构相关知识，方便用户在选择虚拟化方案的时候，作出最佳选择。

# 2 Xen架构

下面是Xen的架构图。从图上可以看出，hepervisor直接运行在硬件上，只负责处理CPU、内存、定时器和中断。它是通过bootloader程序引导后运行的第一个程序。在其之上，运行了许多虚拟机。每一个运行的虚拟机实例称为`domain`或者`guest`。其中，有一个特殊的Domain，称为Dom0，包含系统中所有的硬件驱动。Dom0提供控制和管理Xen系统的程序和服务。值得注意的是，也可以通过[Dom0 Disaggregation](https://wiki.xen.org/wiki/Dom0_Disaggregation)，将一些服务和设备驱动运行在某个专用的虚拟机上，当然，这不是正常的操作方式。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/650px-Xen_Arch_Diagram_v2">

各个组件介绍：

* hypervisor：

    直接运行在硬件之上的程序，负责管理CPU、内存和中断。其本身不管I/O功能，比如网络和存储。

* guest domain/Virtual Machines：

    是虚拟出的运行环境，每一个都可以运行操作系统或者应用程序。guest虚拟机与硬件隔离：换句话说，它们没有特权访问硬件和I/O功能。因此，也称为非特权域（简称DomU）。

* 控制域（Domain 0）：

    特殊的虚拟机，有特权直接访问硬件，处理所有的I/O访问请求并与其它虚拟机进行交互。Xen hypervisor如果没有Dom0，就没有什么用。它是系统运行后，启动的第一个VM。在一个标准的Xen系统中，Dom0包含下面功能：

    - 系统服务：管理设置的工具`XenStore/XenBus (XS)`，向用户提供接口的工具栈`Toolstack (TS)`，基于QEMU的设备模拟

    - 本机设备驱动：Dom0上本地物理设备驱动，用于访问物理硬件。

    - 虚拟设备驱动：Dom0上包含的虚拟设备驱动，英文名称称为`backend`。这儿的概念应该与互联网的服务器后端概念相似。

    - 工具栈：允许用户管理虚拟机的创建、销毁和配置。可以通过命令行、图形界面接口或者`OpenStack或CloudStack`之类的云管理工具调用。

* 支持的操作系统：

    Dom0要求支持Xen的内核。半虚拟化客户机要求支持PV的操作系统。Linux大于3.0的版本支持Xen，并且包含hypervisor程序和工具。Linux2.6.24之前的版本不支持PV，其它的都可以作为PV客户机。

# 3 客户机类型

下图展示了Xen支持的客户机的演化史：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/750px-GuestModes">

对于ARM主机来说，只有一种guest类型；而对于x86主机来说，支持三种类型的guest：

* PV客户机：

    PV是首先由Xen引入的一种软件虚拟化技术。PV不要求CPU支持硬件虚拟化扩展，但是，要求guest操作系统支持Xen。也就是说，guest操作系统是定制的。PV客户机主要用于旧的硬件和旧客户机镜像以及特殊场景，例如，特殊的客户机类型、特定的工作负载（如unikernel）、在不使用嵌套硬件虚拟化支持的情况下在另一个hypervisor中运行Xen（比如使用VirtualBox之类的虚拟软件搭建一个虚拟机作为主机，然后在其上运行Xen）。

* HVM客户机：

    HVM客户机使用主机CPU的硬件虚拟化扩展来虚拟化客户机。HVM需要主机CPU支持Intel VT或AMD-V硬件扩展。Xen使用QEMU设备模型去模拟PC外围硬件，包括BIOS，IDE硬盘控制器、VGA图形适配器、USB控制器、网络适配器等等。当HVM客户机中的PV接口和驱动可用的时候，HVM就使用PV接口和驱动（通常发生在Linux和BSD客户机中）。在Windows系统上，驱动可以在[download page](https://xenproject.org/downloads/windows-pv-drivers.html)下载。如果可用，HVM客户机会使用硬件和软件加速，比如本地APIC、Posted中断，Viridian（Hyper-V）以及PV接口，使客户机的运行更快。通常，HVM使Linux、Windows和BSD上的最佳性能选项。

* PVH客户机：

    PVH使类似于HVM的轻量级客户机。使用主机CPU的虚拟化扩展来虚拟化客户机。与HVM不同的是，PCH不用QEMU模拟设备，而是使用PV驱动程序进行I/O操作、并使用本机操作系统接口用于虚拟化定时器，虚拟化中断和引导。PVH客户机需要支持PVH功能。这种方法与Xen虚拟化ARM的方法类似，只是ARM CPU提供了对虚拟化计时器和中断的硬件支持。

    PVH有两个版本：PVHv1和PVHv2，Linux4.11及其以后的内核支持PVHv2（Xen4.10）。

> 客户机的类型可以通过配置文件进行选择，Xen4.9使用builder，Xen4.10使用type字段。

ARM主机只支持一种虚拟化方案，并且不使用QEMU。



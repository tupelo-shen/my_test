<h1 id="0">0 目录</h1>

* [1 简介](#1)
    - [1.1 Linux与其它类Unix内核对比](#1.1)
    - [1.2 硬件依赖](#1.2)
    - [1.3 Linux版本](#1.3)
    - [1.4 操作系统基本概念](#1.4)
    - [1.5 Unix文件系统综述](#1.5)
    - [1.6 Unix内核综述](#1.6)
* [2 内存寻址](#2)
    - [2.1 内存地址](#2.1)
    - [2.2 内存分段](#2.2)
    - [2.3 Linux中的分段机制](#2.3)
    - [2.4 内存分页](#2.4)
    - [2.5 Linux中的分页机制](#2.5)
* [3 进程](#3)
    - [3.1 进程、轻量级进程和线程](#3.1)
    - [3.2 进程描述符](#3.2)
    - [3.3 进程切换](#3.3)
    - [3.4 创建进程](#3.4)
    - [3.5 销毁进程](#3.5)
* [4 中断和异常](#4)
    - [4.1 中断信号的角色](#4.1)
    - [4.2 中断和异常](#4.2)
    - [4.3 嵌套中断和异常](#4.3)
    - [4.4 初始化中断描述符表](#4.4)
    - [4.5 异常处理](#4.5)
    - [4.6 中断处理](#4.6)
    - [4.7 软件中断和Tasklet](#4.7)
    - [4.8 工作队列](#4.8)
    - [4.9 中断和异常的返回](#4.9)
* [5 内核同步](#5)
    - [5.1 内核服务如何请求](#5.1)
    - [5.2 同步原语](#5.2)
    - [5.3 内核数据结构的同步访问](#5.3)
    - [5.4 防止竞态条件的示例](#5.4)
* [6 内核计时](#6)
    - [6.1 时钟和定时器电路](#6.1)
    - [6.2 内核计时架构](#6.2)
    - [6.3 更新时间和日期](#6.3)
    - [6.4 更新系统数据](#6.4)
    - [6.5 软件定时器和延时函数](#6.5)
    - [6.6 与计时相关的系统调用](#6.6)
* [7 进程调度](#7)
    - [7.1 调度策略](#7.1)
    - [7.2 调度算法](#7.2)
    - [7.3 调度器使用的数据结构](#7.3)
    - [7.4 调度器使用的函数](#7.4)
    - [7.5 多处理器系统的运行队列平衡](#7.5)
    - [7.6 调度相关的系统调用](#7.6)
* [8 内存管理](#8)
    - [8.1 页帧管理](#8.1)
    - [8.2 内存管理](#8.2)
    - [8.3 非连续内存管理](#8.3)
* [9 进程地址空间](#9)
    - [9.1 进程地址空间](#9.1)
    - [9.2 内存描述符](#9.2)
    - [9.3 内存区域](#9.3)
    - [9.4 页错误异常处理程序](#9.4)
    - [9.5 创建和删除进程地址空间](#9.5)
    - [9.6 管理堆](#9.6)
* [10 系统调用](#10)
    - [10.1 POSIX API和系统调用](#10.1)
    - [10.2 系统调用处理程序和服务例程](#10.2)
    - [10.3 进入和退出系统调用](#10.3)
    - [10.4 参数传递](#10.4)
    - [10.5 内核包装例程](#10.5)
* [11 信号量](#11)
    - [11.1 信号量的角色](#11.1)
    - [11.2 创建信号量](#11.2)
    - [11.3 传递信号量](#11.3)
    - [11.4 与信号处理相关的系统调用](#11.4)
* [12 虚拟文件系统](#12)
    - [12.1 虚拟文件系统的角色](#12.1)
    - [12.2 VFS中的数据结构](#12.2)
    - [12.3 文件系统类型](#12.3)
    - [12.4 文件系统处理](#12.4)
    - [12.5 遍历路径](#12.5)
    - [12.6 VFS系统调用的实现](#12.6)
    - [12.7 文件加锁](#12.7)
* [13 I/O架构和设备驱动](#13)
    - [13.1 I/O架构](#13.1)
    - [13.2 设备驱动模型](#13.2)
    - [13.3 设备文件](#13.3)
    - [13.4 设备驱动](#13.4)
    - [13.5 字符设备驱动](#13.5)
* [14 块设备驱动](#14)
    - [14.1 块设备处理](#14.1)
    - [14.2 通用块设备层](#14.2)
    - [14.3 I/O调度器](#14.3)
    - [14.4 块设备驱动](#14.4)
    - [14.5 打开块设备文件](#14.5)
* [15 页缓存](#15)
    - [15.1 页缓存](#15.1)
    - [15.2 页缓存中存储block块](#15.2)
    - [15.3 更新页到硬盘](#15.3)
    - [15.4 sync()、fsync()、和fdatasync()系统调用](#15.4)
* [16 访问文件](#16)
    - [16.1 读写文件](#16.1)
    - [16.2 内存映射](#16.2)
    - [16.3 直接I/O传送](#16.3)
    - [16.4 异步I/O](#16.4)
* [17 页帧回收](#17)
    - [17.1 页帧回收算法](#17.1)
    - [17.2 反向映射](#17.2)
    - [17.3 实现PFRA](#17.3)
    - [17.4 交换](#17.4)
* [18 Ext2和Ext3文件系统](#18)
    - [18.1 Ext2的一般特性](#18.1)
    - [18.2 Ext2硬盘数据结构](#18.2)
    - [18.3 Ext2内存数据结构](#18.3)
    - [18.4 创建Ext2文件系统](#18.4)
    - [18.5 Ext2方法](#18.5)
    - [18.6 管理Ext2硬盘空间](#18.6)
    - [18.7 Ext3文件系统](#18.7)
* [19 进程通信](#19)
    - [19.1 管道](#19.1)
    - [19.2 FIFO](#19.2)
    - [19.3 System V IPC](#19.3)
    - [19.4 POSIX消息队列](#19.4)
* [20 程序执行](#20)
    - [20.1 可执行文件](#20.1)
    - [20.2 可执行文件格式](#20.2)
    - [20.3 执行域](#20.3)
    - [20.4 exec函数](#20.4)
* [21 系统启动](#21)
    - [21.1 BIOS](#21.1)
    - [21.2 Bootloader](#21.2)
        + [21.2.1 从硬盘引导Linux](#21.2.1)
    - [21.3 setup()函数](#21.3)
    - [21.4 setup_32()函数](#21.4)
    - [21.5 start_kernel()](#21.5)
* [22 模块化](#22)
    - [22.1 是否选择模块化](#22.1)
    - [22.2 实现模块](#22.2)
    - [22.3 加载和卸载模块](#22.3)
    - [22.4 根据需要加载模块](#22.4)

---

In order to thoroughly  understand what makes Linux tick and why it works so well on a wide variety of systems, you need to delve deep into the heart of the kernel. The kernel handles all interactions between the CPU and the external world, and determines which programs will share processor time, and in what order. It manages limited memory so well that hundreds of processes can share the system efficiently. It expertly organizes data transfers so that the CPU isn't kept waiting any longer than necessary for the relatively slow hard disks.

*Understanding the Linux Kernel*, Third Edition, takes you on a guided tour of the most significant data structures, algorithms, and programming tricks used in the kernel. Probing beyond superficial features, the authors offer valuable insights for those who want to know how things really work inside their machine. Important Intel-specific features are discussed. Relevant segments of code are dissected line by line. But the book covers more than just the functioning of the code; it explains the theoretical underpinnings [[support with evidence or authority or make more certain or confirm]]() of why Linux does things the way it does.

The third edition of the book covers version 2.6, which has seen signigican changes to nearly every kernel subsystem. particularly in the areas of memory management and block devices. The book focuses on the following topics:

* Memory management, including file buffering, process swapping, and Direct Memory Access(DMA)
* The virtual Filesystem layer and the Second and Third Extended Filesystem
* Process creation and scheduling
* Signals, interrupts, and the essential interfaces to device drivers
* Timing
* Synchronization within the kernel
* Interprocess Communication(IPC)
* Program execution

*Understanding the Linux kernel* will acquaint you with all the inner workings of Linux, but it's more than just an academic exercise. You'll learn what conditions bring out Linux's best performance, and you'll see how it meets the challenge of providing good system response during process scheduling, file access, and memory management in a wide variety of environments. This book will help you make the most of your Linux system.

<h1 id="1">1 简介</h1>

Linux is a member of the large family of Unix-like operating systems. A relative newcomer experiencing sudden spectacular[[*惊人的*]]() popularity starting in the late 1990s, Linux joins such well-known commercial Unix operating systems as System V Release 4(SVR4), developed by AT&T (now owned by the SCO Group); the 4.4 BSD release from the University of California at Berkeley (4.4BSD); Digital UNIX from Digital Equipment Corporation (now Hewlett-Packard); AIX from IBM; HP-UX from Hewlett-Packard; Solaris from Sun Microsystems; and Mac OS X from Apple Computer, Inc. Beside Linux, a few other opensource Unix-like kernels exist, such as FreeBSD, NetBSD, and OpenBSD.

Linux was initially developed by Linus Torvalds in 1991 as an operating system for IBM-compatible personal computers based on the Intel 80386 microprocessor. Linus remains deeply involved with improving Linux, keeping it up-to-date with various hardware developments and coordinating the activity of hundreds of Linux developers around the world. Over the years, developers have worked to make Linux available on other architectures, including Hewlett-Packard’s Alpha, Intel’s Itanium, AMD’s AMD64, PowerPC, and IBM’s zSeries.

One of the more appealing benefits to Linux is that it isn’t a commercial operating system: its source code under the *GNU General Public License (GPL)† *is open and available to anyone to study (as we will in this book); if you download the code (the official site is [http://www.kernel.org](http://www.kernel.org)) or check the sources on a Linux CD, you will be able to explore, from top to bottom, one of the most successful modern operating systems. This book, in fact, assumes you have the source code on hand and can apply what we say to your own explorations.

Technically speaking, Linux is a true Unix kernel, although it is not a full Unix operating system because it does not include all the Unix applications, such as filesystem utilities, windowing systems and graphical desktops, system administrator commands, text editors, compilers, and so on. However, because most of these programs are freely available under the GPL, they can be installed in every Linux-based system.

Because the Linux kernel requires so much additional software to provide a useful environment, many Linux users prefer to rely on commercial distributions, available on CD-ROM, to get the code included in a standard Unix system. Alternatively, the code may be obtained from several different sites, for instance [http://www.kernel.org](http://www.kernel.org). Several distributions put the Linux source code in the */usr/src/linux* directory. In the rest of this book, all file pathnames will refer implicitly to the Linux source code directory.

<h2 id="1.1">1.1 Linux与其它类Unix内核对比</h2>

The various Unix-like systems on the market, some of which have a long history and show signs of archaic practices, differ in many important respects. All commercial variants were derived from either SVR4 or 4.4BSD, and all tend to agree on some common standards like IEEE’s Portable Operating Systems based on Unix (POSIX) and X/Open’s Common Applications Environment (CAE).

The current standards specify only an application programming interface (API)—that is, a well-defined environment in which user programs should run. Therefore, the standards do not impose any restriction on internal design choices of a compliant kernel.

To define a common user interface, Unix-like kernels often share fundamental design ideas and features. In this respect, Linux is comparable with the other Unix-like operating systems. Reading this book and studying the Linux kernel, therefore, may help you understand the other Unix variants, too.

The 2.6 version of the Linux kernel aims to be compliant with the IEEE POSIX standard. This, of course, means that most existing Unix programs can be compiled and executed on a Linux system with very little effort or even without the need for patches to the source code. Moreover, Linux includes all the features of a modern Unix operating system, such as virtual memory, a virtual filesystem, lightweight processes, Unix signals, SVR4 interprocess communications, support for Symmetric Multiprocessor (SMP) systems, and so on.

When Linus Torvalds wrote the first kernel, he referred to some classical books on Unix internals, like Maurice Bach’s *The Design of the Unix Operating System* (Prentice Hall, 1986). Actually, Linux still has some bias toward the Unix baseline described in Bach’s book (i.e., SVR2). However, Linux doesn’t stick to any particular variant. Instead, it tries to adopt the best features and design choices of several different Unix kernels.

The following list describes how Linux competes against some well-known commercial Unix kernels:

* Monolithic kernel
    
    It is a large, complex do-it-yourself program, composed of several logically different components. In this, it is quite conventional; most commercial Unix variants are monolithic. (Notable exceptions are the Apple Mac OS X and the GNU Hurd operating systems, both derived from the Carnegie-Mellon’s Mach, which follow a microkernel approach.)

* Compiled and statically linked traditional Unix kernels

    Most modern kernels can dynamically load and unload some portions of the kernel code (typically, device drivers), which are usually called modules. Linux’s support for modules is very good, because it is able to automatically load and unload modules on demand. Among the main commercial Unix variants, only the SVR4.2 and Solaris kernels have a similar feature.

* 内核线程
    
    Some Unix kernels, such as Solaris and SVR4.2/MP, are organized as a set of kernel threads. A kernel thread is an execution context that can be independently scheduled; it may be associated with a user program, or it may run only some kernel functions. Context switches between kernel threads are usually much less expensive than context switches between ordinary processes, because the former usually operate on a common address space. Linux uses kernel threads in a very limited way to execute a few kernel functions periodically; however, they do not represent the basic execution context abstraction. (That’s the topic of the next item.)

* 支持多线程应用程序

    Most modern operating systems have some kind of support for multithreaded applications—that is, user programs that are designed in terms of many relatively independent execution flows that share a large portion of the application data structures. A multithreaded user application could be composed of many lightweight processes (LWP), which are processes that can operate on a common address space, common physical memory pages, common opened files, and so on. Linux defines its own version of lightweight processes, which is different from the types used on other systems such as SVR4 and Solaris. While all the commercial Unix variants of LWPare based on kernel threads, Linux regards lightweight processes as the basic execution context and handles them via the nonstandard `clone()` system call.

* 可抢占式内核
    
    When compiled with the “Preemptible Kernel” option, Linux 2.6 can arbitrarily interleave execution flows while they are in privileged mode. Besides Linux 2.6, a few other conventional, general-purpose Unix systems, such as Solaris and Mach 3.0, are fully preemptive kernels. SVR4.2/MP introduces some fixed preemption points as a method to get limited preemption capability.

* 支持多处理器
    
    Several Unix kernel variants take advantage of multiprocessor systems. Linux 2.6 supports symmetric multiprocessing (SMP) for different memory models, including NUMA: the system can use multiple processors and each processor can handle any task—there is no discrimination among them. Although a few parts of the kernel code are still serialized by means of a single “big kernel lock,” it is fair to say that Linux 2.6 makes a near optimal use of SMP.

* 文件系统
    
    Linux’s standard filesystems come in many flavors. You can use the plain old Ext2 filesystem if you don’t have specific needs. You might switch to Ext3 if you want to avoid lengthy filesystem checks after a system crash. If you’ll have to deal with many small files, the ReiserFS filesystem is likely to be the best choice. Besides Ext3 and ReiserFS, several other journaling filesystems can be used in Linux; they include IBM AIX’s Journaling File System (JFS) and Silicon Graphics IRIX’s XFS filesystem. Thanks to a powerful object-oriented Virtual File System technology (inspired by Solaris and SVR4), porting a foreign filesystem to Linux is generally easier than porting to other kernels.

* STREAMS
    
    Linux has no analog to the STREAMS I/O subsystem introduced in SVR4, although it is included now in most Unix kernels and has become the preferred interface for writing device drivers, terminal drivers, and network protocols.

This assessment suggests that Linux is fully competitive nowadays with commercial
operating systems. Moreover, Linux has several features that make it an exciting
operating system. Commercial Unix kernels often introduce new features to gain a
larger slice of the market, but these features are not necessarily useful, stable, or productive.
As a matter of fact, modern Unix kernels tend to be quite bloated. By contrast,
Linux—together with the other open source operating systems—doesn’t suffer
from the restrictions and the conditioning imposed by the market, hence it can freely
evolve according to the ideas of its designers (mainly Linus Torvalds). Specifically,
Linux offers the following advantages over its commercial competitors:

* **Linux是免费的** 
    
    You can install a complete Unix system at no expense other than the hardware (of course).

* **Linux可完全定制化**
    
    Thanks to the compilation options, you can customize the kernel by selecting only the features really needed. Moreover, thanks to the GPL, you are allowed to freely read and modify the source code of the kernel and of all system programs.

* **Linux可以运行在低端也不贵的硬件设备上** 
    
    You are able to build a network server using an old Intel 80386 system with 4 MB of RAM.

* **Linux功能强大**
    
    Linux systems are very fast, because they fully exploit the features of the hardware components. The main Linux goal is efficiency, and indeed many design choices of commercial variants, like the STREAMS I/O subsystem, have been rejected by Linus because of their implied performance penalty.

* **Linux开发者都是及其优秀的**
    
    Linux systems are very stable; they have a very low failure rate and system maintenance time.

* **Linux内核可以小而紧凑**
    
    It is possible to fit a kernel image, including a few system programs, on just one 1.44 MB floppy disk. As far as we know, none of the commercial Unix variants is able to boot from a single floppy disk.

* **Linux与许多常见的系统兼容**
    
    Linux lets you directly mount filesystems for all versions of MS-DOS and Microsoft Windows, SVR4, OS/2, Mac OS X, Solaris, SunOS, NEXTSTEP, many BSD variants, and so on. Linux also is able to operate with many network layers, such as Ethernet (as well as Fast Ethernet, Gigabit Ethernet, and 10 Gigabit Ethernet), Fiber Distributed Data Interface (FDDI), High Performance Parallel Interface (HIPPI), IEEE 802.11 (Wireless LAN), and IEEE 802.15 (Bluetooth). By using suitable libraries, Linux systems are even able to directly run programs written for other operating systems. For example, Linux is able to execute some applications written for MS-DOS, Microsoft Windows, SVR3 and R4, 4.4BSD, SCO Unix, Xenix, and others on the 80x86 platform.

* **Linux具有良好的支持**
    
    Believe it or not, it may be a lot easier to get patches and updates for Linux than for any proprietary operating system. The answer to a problem often comes back within a few hours after sending a message to some newsgroup or mailing list. Moreover, drivers for Linux are usually available a few weeks after new hardware products have been introduced on the market. By contrast, hardware manufacturers release device drivers for only a few commercial operating systems—usually Microsoft’s. Therefore, all commercial Unix variants run on a restricted subset of hardware components.

With an estimated installed base of several tens of millions, people who are used to certain features that are standard under other operating systems are starting to expect the same from Linux. In that regard, the demand on Linux developers is also increasing. Luckily, though, Linux has evolved under the close direction of Linus and his subsystem maintainers to accommodate the needs of the masses.

<h2 id="1.2">1.2 硬件依赖</h2>

Linux tries to maintain a neat distinction between hardware-dependent and hardware-independent source code. To that end, both the arch and the include directories include 23 subdirectories that correspond to the different types of hardware platforms supported. The standard names of the platforms are:

* alpha
    
    Hewlett-Packard’s Alpha workstations (originally Digital, then Compaq; no longer manufactured)

* arm, arm26

    ARM processor-based computers such as PDAs and embedded devices

* cris

    “Code Reduced Instruction Set” CPUs used by Axis in its thin-servers, such as web cameras or development boards

* frv
    
    Embedded systems based on microprocessors of the Fujitsu’s FR-V family

* h8300
    
    Hitachi h8/300 and h8S RISC 8/16-bit microprocessors

* i386
    
    IBM-compatible personal computers based on 80x86 microprocessors

* ia64
    
    Workstations based on the Intel 64-bit Itanium microprocessor

* m32r
    
    Computers based on the Renesas M32R family of microprocessors

* m68k, m68knommu

    Personal computers based on Motorola MC680×0 microprocessors

* mips

    Workstations based on MIPS microprocessors, such as those marketed by Silicon Graphics

* parisc

    Workstations based on Hewlett Packard HP 9000 PA-RISC microprocessors

* ppc, ppc64

    Workstations based on the 32-bit and 64-bit Motorola-IBM PowerPC microprocessors

* s390

    IBM ESA/390 and zSeries mainframes

* sh, sh64
    
    Embedded systems based on SuperH microprocessors developed by Hitachi and STMicroelectronics

* sparc, sparc64
    
    Workstations based on Sun Microsystems SPARC and 64-bit Ultra SPARC microprocessors

* um
    
    User Mode Linux, a virtual platform that allows developers to run a kernel in User Mode

* v850
    
    NEC V850 microcontrollers that incorporate a 32-bit RISC core based on the Harvard architecture

* x86_64

    Workstations based on the AMD’s 64-bit microprocessors—such Athlon and Opteron—and Intel’s ia32e/EM64T 64-bit microprocessors

<h2 id="1.3">1.3 Linux版本</h2>

Up to kernel version 2.5, Linux identified kernels through a simple numbering scheme. Each version was characterized by three numbers, separated by periods. The first two numbers were used to identify the version; the third number identified the release. The first version number, namely 2, has stayed unchanged since 1996. The second version number identified the type of kernel: if it was even, it denoted a stable version; otherwise, it denoted a development version.

As the name suggests, stable versions were thoroughly checked by Linux distributors and kernel hackers. A new stable version was released only to address bugs and to add new device drivers. Development versions, on the other hand, differed quite significantly from one another; kernel developers were free to experiment with different solutions that occasionally lead to drastic kernel changes. Users who relied on development versions for running applications could experience unpleasant surprises when upgrading their kernel to a newer release.

During development of Linux kernel version 2.6, however, a significant change in the version numbering scheme has taken place. Basically, the second number no longer identifies stable or development versions; thus, nowadays kernel developers introduce large and significant changes in the current kernel version 2.6. A new kernel 2.7 branch will be created only when kernel developers will have to test a really disruptive change; this 2.7 branch will lead to a new current kernel version, or it will be backported to the 2.6 version, or finally it will simply be dropped as a dead end.

The new model of Linux development implies that two kernels having the same version but different release numbers—for instance, 2.6.10 and 2.6.11—can differ significantly even in core components and in fundamental algorithms. Thus, when a new kernel release appears, it is potentially unstable and buggy. To address this problem, the kernel developers may release patched versions of any kernel, which are identified by a fourth number in the version numbering scheme. For instance, at the time this paragraph was written, the latest “stable” kernel version was 2.6.11.12. Please be aware that the kernel version described in this book is Linux 2.6.11.


<h2 id="1.4">1.4 操作系统基本概念</h2>

Each computer system includes a basic set of programs called the operating system. The most important program in the set is called the kernel. It is loaded into RAM when the system boots and contains many critical procedures that are needed for the system to operate. The other programs are less crucial utilities; they can provide a wide variety of interactive experiences for the user—as well as doing all the jobs the user bought the computer for—but the essential shape and capabilities of the system are determined by the kernel. The kernel provides key facilities to everything else on the system and determines many of the characteristics of higher software. Hence, we often use the term “operating system” as a synonym for “kernel.”

The operating system must fulfill two main objectives:

* Interact with the hardware components, servicing all low-level programmable elements included in the hardware platform.

* Provide an execution environment to the applications that run on the computer system (the so-called user programs).

Some operating systems allow all user programs to directly play with the hardware components (a typical example is MS-DOS). In contrast, a Unix-like operating system hides all low-level details concerning the physical organization of the computer from applications run by the user. When a program wants to use a hardware resource, it must issue a request to the operating system. The kernel evaluates the request and, if it chooses to grant the resource, interacts with the proper hardware components on behalf of the user program.

To enforce this mechanism, modern operating systems rely on the availability of specific hardware features that forbid user programs to directly interact with low-level hardware components or to access arbitrary memory locations. In particular, the hardware introduces at least two different execution modes for the CPU: a nonprivileged mode for user programs and a privileged mode for the kernel. Unix calls these User Mode and Kernel Mode, respectively.

In the rest of this chapter, we introduce the basic concepts that have motivated the design of Unix over the past two decades, as well as Linux and other operating systems. While the concepts are probably familiar to you as a Linux user, these sections try to delve into them a bit more deeply than usual to explain the requirements they place on an operating system kernel. These broad considerations refer to virtually all Unix-like systems. The other chapters of this book will hopefully help you understand the Linux kernel internals.

<h3 id="1.4.1">1.4.1 多用户系统</h3>

A multiuser system is a computer that is able to concurrently and independently execute several applications belonging to two or more users. Concurrently means that applications can be active at the same time and contend for the various resources such as CPU, memory, hard disks, and so on. Independently means that each application can perform its task with no concern for what the applications of the other users are doing. Switching from one application to another, of course, slows down each of them and affects the response time seen by the users. Many of the complexities of modern operating system kernels, which we will examine in this book, are present to minimize the delays enforced on each program and to provide the user with responses that are as fast as possible.

Multiuser operating systems must include several features:

* An authentication mechanism for verifying the user’s identity

* protection mechanism against buggy user programs that could block other applications running in the system

* A protection mechanism against malicious user programs that could interfere with or spy on the activity of other users

* An accounting mechanism that limits the amount of resource units assigned to each user

To ensure safe protection mechanisms, operating systems must use the hardware protection associated with the CPU privileged mode. Otherwise, a user program would be able to directly access the system circuitry and overcome the imposed bounds. Unix is a multiuser system that enforces the hardware protection of system resources.

<h3 id="1.4.2">1.4.2 用户和组的概念</h3>

In a multiuser system, each user has a private space on the machine; typically, he owns some quota of the disk space to store files, receives private mail messages, and so on. The operating system must ensure that the private portion of a user space is visible only to its owner. In particular, it must ensure that no user can exploit a system application for the purpose of violating the private space of another user.

All users are identified by a unique number called the *User ID*, or *UID*. Usually only a restricted number of persons are allowed to make use of a computer system. When one of these users starts a working session, the system asks for a login name and a password. If the user does not input a valid pair, the system denies access. Because the password is assumed to be secret, the user’s privacy is ensured.

To selectively share material with other users, each user is a member of one or more user groups, which are identified by a unique number called a *user group ID*. Each file is associated with exactly one group. For example, access can be set so the user owning the file has read and write privileges, the group has read-only privileges, and other users on the system are denied access to the file.

Any Unix-like operating system has a special user called *root* or *superuser*. The system administrator must log in as root to handle user accounts, perform maintenance tasks such as system backups and program upgrades, and so on. The root user can do almost everything, because the operating system does not apply the usual protection mechanisms to her. In particular, the root user can access every file on the system and can manipulate every running user program.

<h3 id="1.4.3">1.4.3 进程</h3>

All operating systems use one fundamental abstraction: the *process*. A process can be defined either as “an instance of a program in execution” or as the “execution context” of a running program. In traditional operating systems, a process executes a single sequence of instructions in an address space; the address space is the set of memory addresses that the process is allowed to reference. Modern operating systems allow processes with multiple execution flows—that is, multiple sequences of instructions executed in the same address space.

Multiuser systems must enforce an execution environment in which several processes can be active concurrently and contend for system resources, mainly the CPU. Systems that allow concurrent active processes are said to be multiprogramming or multiprocessing. It is important to distinguish programs from processes; several processes can execute the same program concurrently, while the same process can execute several programs sequentially.

On uniprocessor systems, just one process can hold the CPU, and hence just one execution flow can progress at a time. In general, the number of CPUs is always restricted, and therefore only a few processes can progress at once. An operating system component called the scheduler chooses the process that can progress. Some operating systems allow only nonpreemptable processes, which means that the scheduler is invoked only when a process voluntarily relinquishes the CPU. But processes of a multiuser system must be preemptable; the operating system tracks how long each process holds the CPU and periodically activates the scheduler.

Unix is a multiprocessing operating system with preemptable processes. Even when no user is logged in and no application is running, several system processes monitor the peripheral devices. In particular, several processes listen at the system terminals waiting for user logins. When a user inputs a login name, the listening process runs a program that validates the user password. If the user identity is acknowledged, the process creates another process that runs a shell into which commands are entered. When a graphical display is activated, one process runs the window manager, and each window on the display is usually run by a separate process. When a user creates a graphics shell, one process runs the graphics windows and a second process runs the shell into which the user can enter the commands. For each user command, the shell process creates another process that executes the corresponding program.

Unix-like operating systems adopt a process/kernel model. Each process has the illusion that it’s the only process on the machine, and it has exclusive access to the operating system services. Whenever a process makes a system call (i.e., a request to the kernel, see [Chapter 10](#10)), the hardware changes the privilege mode from User Mode to Kernel Mode, and the process starts the execution of a kernel procedure with a strictly limited purpose. In this way, the operating system acts within the execution context of the process in order to satisfy its request. Whenever the request is fully satisfied, the kernel procedure forces the hardware to return to User Mode and the process continues its execution from the instruction following the system call.

<h3 id="1.4.4">1.4.4 内核架构</h3>

As stated before, most Unix kernels are monolithic: each kernel layer is integrated into the whole kernel program and runs in Kernel Mode on behalf of the current process. In contrast, microkernel operating systems demand a very small set of functions from the kernel, generally including a few synchronization primitives, a simple scheduler, and an interprocess communication mechanism. Several system processes that run on top of the microkernel implement other operating system–layer functions, like memory allocators, device drivers, and system call handlers.

Although academic research on operating systems is oriented toward microkernels, such operating systems are generally slower than monolithic ones, because the explicit message passing between the different layers of the operating system has a cost. However, microkernel operating systems might have some theoretical advantages over monolithic ones. Microkernels force the system programmers to adopt a modularized approach, because each operating system layer is a relatively independent program that must interact with the other layers through well-defined and clean software interfaces. Moreover, an existing microkernel operating system can be easily ported to other architectures fairly easily, because all hardware-dependent components are generally encapsulated in the microkernel code. Finally, microkernel operating systems tend to make better use of random access memory (RAM) than monolithic ones, because system processes that aren’t implementing needed functionalities might be swapped out or destroyed.

To achieve many of the theoretical advantages of microkernels without introducing performance penalties, the Linux kernel offers modules. A module is an object file whose code can be linked to (and unlinked from) the kernel at runtime. The object code usually consists of a set of functions that implements a filesystem, a device driver, or other features at the kernel’s upper layer. The module, unlike the external layers of microkernel operating systems, does not run as a specific process. Instead, it is executed in Kernel Mode on behalf of the current process, like any other statically linked kernel function.

使用模块主要有以下优点：

* 模块化方法

    Because any module can be linked and unlinked at runtime, system programmers must introduce well-defined software interfaces to access the data structures handled by modules. This makes it easy to develop new modules.

* 平台独立

    Even if it may rely on some specific hardware features, a module doesn’t depend on a fixed hardware platform. For example, a disk driver module that relies on the SCSI standard works as well on an IBM-compatible PC as it does on Hewlett-Packard’s Alpha.

* 节省内存使用

    A module can be linked to the running kernel when its functionality is required and unlinked when it is no longer useful; this is quite useful for small embedded systems.

* 没有性能损失

    Once linked in, the object code of a module is equivalent to the object code of the statically linked kernel. Therefore, no explicit message passing is required when the functions of the module are invoked.

<h2 id="1.5">1.5 Unix文件系统综述</h2>

The Unix operating system design is centered on its filesystem, which has several interesting characteristics. We’ll review the most significant ones, since they will be mentioned quite often in forthcoming chapters.

<h3 id="1.5.1">1.5.1 文件</h3>

A Unix file is an information container structured as a sequence of bytes; the kernel does not interpret the contents of a file. Many programming libraries implement higher-level abstractions, such as records structured into fields and record addressing based on keys. However, the programs in these libraries must rely on system calls offered by the kernel. From the user’s point of view, files are organized in a tree-structured namespace, as shown in Figure 1-1.

Unix文件是由字节序列构成的信息容器；内核不解释文件的内容。许多编程库在其上实现了更高级的抽象，比如record结构，由基于键值对的方式构成。但是，这些程序还是依赖于内核提供的系统调用接口。从用户的观点来看，文件就像组织在一个树形结构中，如图1-1所示。

<img id="Figure_1-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_1_1.PNG">

图1-1 目录树示例

All the nodes of the tree, except the leaves, denote directory names. A directory node
contains information about the files and directories just beneath it. A file or directory
name consists of a sequence of arbitrary ASCII characters,* with the exception of
/ and of the null character \0. Most filesystems place a limit on the length of a filename,
typically no more than 255 characters. The directory corresponding to the
root of the tree is called the root directory. By convention, its name is a slash (/).
Names must be different within the same directory, but the same name may be used
in different directories.

Unix associates a current working directory with each process (see the section “The
Process/Kernel Model” later in this chapter); it belongs to the process execution context,
and it identifies the directory currently used by the process. To identify a specific
file, the process uses a pathname, which consists of slashes alternating with a
sequence of directory names that lead to the file. If the first item in the pathname is
a slash, the pathname is said to be absolute, because its starting point is the root
directory. Otherwise, if the first item is a directory name or filename, the pathname
is said to be relative, because its starting point is the process’s current directory.

While specifying filenames, the notations “.” and “..” are also used. They denote the
current working directory and its parent directory, respectively. If the current working
directory is the root directory, “.” and “..” coincide.


<h3 id="1.5.2">1.5.2 硬链接和软链接</h3>

A filename included in a directory is called a file hard link, or more simply, a link.
The same file may have several links included in the same directory or in different
ones, so it may have several filenames.

The Unix command:

    $ ln p1 p2

is used to create a new hard link that has the pathname `p2` for a file identified by the pathname p1.

Hard links have two limitations:

* It is not possible to create hard links for directories. Doing so might transform the directory tree into a graph with cycles, thus making it impossible to locate a file according to its name.

* Links can be created only among files included in the same filesystem. This is a serious limitation, because modern Unix systems may include several filesystems located on different disks and/or partitions, and users may be unaware of the physical divisions between them.

To overcome these limitations, soft links (also called symbolic links) were introduced
a long time ago. Symbolic links are short files that contain an arbitrary pathname of
another file. The pathname may refer to any file or directory located in any filesystem;
it may even refer to a nonexistent file.

The Unix command:

    $ ln -s p1 p2

creates a new soft link with pathname `p2` that refers to pathname `p1`. When this command
is executed, the filesystem extracts the directory part of `p2` and creates a new
entry in that directory of type symbolic link, with the name indicated by `p2`. This new
file contains the name indicated by pathname `p1`. This way, each reference to `p2` can
be translated automatically into a reference to `p1`.

<h3 id="1.5.3">1.5.3 文件类型</h3>

Unix系统具有如下的文件类型：

* 常规文件
* 目录
* 符号链接
* 块设备文件
* 字符设备文件
* 管道和命名管道（也称为FIFO）
* 套接字（Socket）

The first three file types are constituents of any Unix filesystem. Their implementation is described in detail in Chapter 18.

Device files are related both to I/O devices, and to device drivers integrated into the kernel. For example, when a program accesses a device file, it acts directly on the I/O device associated with that file (see [Chapter 13](#13)).

Pipes and sockets are special files used for interprocess communication (see the section “Synchronization and Critical Regions” later in this chapter; also see [Chapter 19](#19)).

<h3 id="1.5.4">1.5.4 文件描述符和Inode</h3>

Unix makes a clear distinction between the contents of a file and the information about a file. With the exception of device files and files of special filesystems, each file consists of a sequence of bytes. The file does not include any control information, such as its length or an end-of-file (EOF) delimiter.

All information needed by the filesystem to handle a file is included in a data structure called an inode. Each file has its own inode, which the filesystem uses to identify the file.

While filesystems and the kernel functions handling them can vary widely from one Unix system to another, they must always provide at least the following attributes, which are specified in the POSIX standard:

* File type (see the previous section)
* Number of hard links associated with the file
* File length in bytes
* Device ID (i.e., an identifier of the device containing the file)
* Inode number that identifies the file within the filesystem
* UID of the file owner
* User group ID of the file
* Several timestamps that specify the inode status change time, the last access time, and the last modify time
* Access rights and file mode (see the next section)

<h3 id="1.5.5">1.5.5 访问权限和文件模式</h3>

The potential users of a file fall into three classes:

* The user who is the owner of the file
* The users who belong to the same group as the file, not including the owner
* All remaining users (others)

There are three types of access rights—`read`, `write`, and `execute`—for each of these three classes. Thus, the set of access rights associated with a file consists of nine different binary flags. Three additional flags, called `suid` (Set User ID), `sgid` (Set Group ID), and `sticky`, define the file mode. These flags have the following meanings when applied to executable files:

* suid
    
    A process executing a file normally keeps the User ID (UID) of the process owner. However, if the executable file has the suid flag set, the process gets the UID of the file owner.

* sgid
    
    A process executing a file keeps the user group ID of the process group. However, if the executable file has the sgid flag set, the process gets the user group ID of the file.

* sticky
    
    An executable file with the sticky flag set corresponds to a request to the kernel to keep the program in memory after its execution terminates.

When a file is created by a process, its owner ID is the UID of the process. Its owner user group ID can be either the process group ID of the creator process or the user group ID of the parent directory, depending on the value of the sgid flag of the parent directory.

<h3 id="1.5.6">1.5.6 操作文件的系统调用</h3>

When a user accesses the contents of either a regular file or a directory, he actually
accesses some data stored in a hardware block device. In this sense, a filesystem is a
user-level view of the physical organization of a hard disk partition. Because a process
in User Mode cannot directly interact with the low-level hardware components,
each actual file operation must be performed in Kernel Mode. Therefore, the Unix
operating system defines several system calls related to file handling.

All Unix kernels devote great attention to the efficient handling of hardware block
devices to achieve good overall system performance. In the chapters that follow, we
will describe topics related to file handling in Linux and specifically how the kernel
reacts to file-related system calls. To understand those descriptions, you will need to
know how the main file-handling system calls are used; these are described in the
next section.

<h4 id="1.5.6.1">1.5.6.1 打开文件</h4>

Processes can access only “opened” files. To open a file, the process invokes the system call:

    fd = open(path, flag, mode)

The three parameters have the following meanings:

* path

    Denotes the pathname (relative or absolute) of the file to be opened.

* flag
    
    Specifies how the file must be opened (e.g., read, write, read/write, append). It also can specify whether a nonexisting file should be created.

* mode

    Specifies the access rights of a newly created file.

This system call creates an “open file” object and returns an identifier called a `file` descriptor. An open file object contains:

* Some file-handling data structures, such as a set of flags specifying how the file has been opened, an offset field that denotes the current position in the file from which the next operation will take place (the so-called file pointer), and so on.

* Some pointers to kernel functions that the process can invoke. The set of permitted functions depends on the value of the flag parameter.

We discuss open file objects in detail in Chapter 12. Let’s limit ourselves here to describing some general properties specified by the POSIX semantics.

* A file descriptor represents an interaction between a process and an opened file, while an open file object contains data related to that interaction. The same open file object may be identified by several file descriptors in the same process.

* Several processes may concurrently open the same file. In this case, the filesystem assigns a separate file descriptor to each file, along with a separate open file object. When this occurs, the Unix filesystem does not provide any kind of synchronization among the I/O operations issued by the processes on the same file. However, several system calls such as `flock()` are available to allow processes to synchronize themselves on the entire file or on portions of it (see [Chapter 12](#12)).

To create a new file, the process also may invoke the `creat()` system call, which is handled by the kernel exactly like `open()`.

<h4 id="1.5.6.2">1.5.6.2 访问打开的文件</h4>
<h4 id="1.5.6.3">1.5.6.3 关闭文件</h4>
<h4 id="1.5.6.2">1.5.6.2 重命名和删除文件</h4>

<h2 id="1.6">1.6 Unix内核综述</h2>

Unix kernels provide an execution environment in which applications may run. Therefore, the kernel must implement a set of services and corresponding interfaces. Applications use those interfaces and do not usually interact directly with hardware resources.

<h3 id="1.6.1">1.6.1 进程/内核模型</h3>

As already mentioned, a CPU can run in either User Mode or Kernel Mode. Actually, some CPUs can have more than two execution states. For instance, the 80×86 microprocessors have four different execution states. But all standard Unix kernels use only Kernel Mode and User Mode.

When a program is executed in User Mode, it cannot directly access the kernel data structures or the kernel programs. When an application executes in Kernel Mode, however, these restrictions no longer apply. Each CPU model provides special instructions to switch from User Mode to Kernel Mode and vice versa. A program usually executes in User Mode and switches to Kernel Mode only when requesting a service provided by the kernel. When the kernel has satisfied the program’s request, it puts the program back in User Mode.

Processes are dynamic entities that usually have a limited life span within the system. The task of creating, eliminating, and synchronizing the existing processes is delegated to a group of routines in the kernel.

The kernel itself is not a process but a process manager. The process/kernel model assumes that processes that require a kernel service use specific programming constructs called system calls. Each system call sets up the group of parameters that identifies the process request and then executes the hardware-dependent CPU instruction to switch from User Mode to Kernel Mode.

Besides user processes, Unix systems include a few privileged processes called `kernel threads` with the following characteristics:

* They run in Kernel Mode in the kernel address space.
* They do not interact with users, and thus do not require terminal devices.
* They are usually created during system startup and remain alive until the system is shut down.

On a uniprocessor system, only one process is running at a time, and it may run
either in User or in Kernel Mode. If it runs in Kernel Mode, the processor is executing
some kernel routine. Figure 1-2 illustrates examples of transitions between User
and Kernel Mode. Process 1 in User Mode issues a system call, after which the process
switches to Kernel Mode, and the system call is serviced. Process 1 then resumes
execution in User Mode until a timer interrupt occurs, and the scheduler is activated
in Kernel Mode. A process switch takes place, and Process 2 starts its execution in
User Mode until a hardware device raises an interrupt. As a consequence of the interrupt,
Process 2 switches to Kernel Mode and services the interrupt.

<img id="Figure_1-2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_1_2.PNG">

图1-2 用户模式和内核模式之间的转换

Unix kernels do much more than handle system calls; in fact, kernel routines can be activated in several ways:

* A process invokes a system call.
* The CPU executing the process signals an exception, which is an unusual condition such as an invalid instruction. The kernel handles the exception on behalf of the process that caused it.
* A peripheral device issues an interrupt signal to the CPU to notify it of an event such as a request for attention, a status change, or the completion of an I/O operation. Each interrupt signal is dealt by a kernel program called an interrupt handler. Because peripheral devices operate asynchronously with respect to the CPU, interrupts occur at unpredictable times.
* A kernel thread is executed. Because it runs in Kernel Mode, the corresponding program must be considered part of the kernel.

<h3 id="1.6.2">1.6.2 进程实现</h3>

To let the kernel manage processes, each process is represented by a *process descriptor* that includes information about the current state of the process.

When the kernel stops the execution of a process, it saves the current contents of several processor registers in the process descriptor. These include:

* 程序计数器(PC)和堆栈指针寄存器(SP)
* 通用目的寄存器
* 浮点寄存器
* 处理器控制寄存器(Processor Status Word)，包含CPU状态信息
* 内存管理寄存器，用于跟踪进程访问的RAM

When the kernel decides to resume executing a process, it uses the proper process descriptor fields to load the CPU registers. Because the stored value of the program counter points to the instruction following the last instruction executed, the process resumes execution at the point where it was stopped.

When a process is not executing on the CPU, it is waiting for some event. Unix kernels distinguish many wait states, which are usually implemented by queues of process descriptors; each (possibly empty) queue corresponds to the set of processes waiting for a specific event.

<h3 id="1.6.3">1.6.3 可重入内核</h3>
<h3 id="1.6.4">1.6.4 进程地址空间</h3>
<h3 id="1.6.5">1.6.5 同步和临界区</h3>
<h3 id="1.6.6">1.6.6 信号量和进程间通信</h3>
<h3 id="1.6.7">1.6.7 进程管理</h3>
<h3 id="1.6.8">1.6.8 内存管理</h3>
<h3 id="1.6.9">1.6.9 设备驱动</h3>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="2">2 内存寻址</h1>

This chapter deals with addressing techniques. Luckily, an operating system is not forced to keep track of physical memory all by itself; today’s microprocessors include several hardware circuits to make memory management both more efficient and more robust so that programming errors cannot cause improper accesses to memory outside the program.

本章讲述寻址技术。幸运的是，操作系统不需要自己跟踪管理物理内存。今天的微处理器都包含一些硬件电路，使得内存管理更为高效，也更为健壮，所以，编程错误就不会导致程序外的内存访问不当。

As in the rest of this book, we offer details in this chapter on how 80×86 microprocessors address memory chips and how Linux uses the available addressing circuits. You will find, we hope, that when you learn the implementation details on Linux’s most popular platform you will better understand both the general theory of paging and how to research the implementation on other platforms.

在本书的其余部分，我们将以x86微处理器为例，讲述它如何寻址内存芯片和Linux如何使用这些寻址单元。希望通过讲解这个最为流行的平台上的Linux实现，我们可以理解分页机制的通用理论，并能够将这些理论应用到别的平台上。

This is the first of three chapters related to memory management; [Chapter 8](#8) discusses how the kernel allocates main memory to itself, while [Chapter 9](#9) considers how linear addresses are assigned to processes.

这是关于内存管理相关的3章中的第1章。在[第8章](#8)中讨论内核如何给自身分配内存，而[第9章](#9)如何分配线性地址给进程。

<h2 id="2.1">2.1 内存地址</h2>

程序员通常使用内存地址访问内存单元中的内容。但是，对于x86处理器，需要区分下面3种地址（[ARM处理器是没有逻辑地址这个概念的，也就是说它们指令中的寻址方式是不同的。]()）：

1. 逻辑地址
    
    在机器语言指令中用来指定算子或指令的地址。这类地址加强了x86架构的分段架构，但迫使基于MS-DOS和Windows系统编程的程序员将他们的程序也分段。每个逻辑地址由一个段地址和偏移量组成，偏移量描述段地址偏离实际物理地址的距离。

2. 线性地址（也被称为虚拟地址）
    
    32位系统的寻址空间是4GB，也就是寻址范围是0x00000000到0xffffffff。

3. 物理地址
    
    用来描述实际物理内存芯片的地址空间。也就是说，这些地址是发送到内存总线上的电信号。物理地址可以是32位，也可以是36位。

内存管理单元（MMU）负责把逻辑地址转换成物理地址：首先，段管理单元把逻辑地址转换成线性地址；然后，页管理单元负责把线性地址转换成物理地址。如图2-1所示，

<img id="Figure_2-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_1.PNG">

图2-1 逻辑地址的转换过程

在多核系统中，所有CPU共享内存，这意味着可能对内存发生并发访问。但是，对内存的读写操作必须是串行的，所以，添加了一个称为内存仲裁的硬件单元，负责CPU对内存访问权限的仲裁：如果内存空闲就让CPU访问，如果内存正在被另一个CPU访问就延缓该CPU的请求。即使单处理器系统也会使用内存仲裁，因为还有直接内存访问-DMA方式，同样会造成对内存访问的并发。多核系统中，内存仲裁单元的结构非常复杂，因为它有多个输入端口。比如，双核奔腾处理器，维护着一个2端口的仲裁单元，要求2个CPU在使用公共的总线之前先要交换同步消息。从编程的角度来说，仲裁单元被隐藏了，因为是硬件负责的，无需程序干预。

<h2 id="2.2">2.2 内存分段</h2>

Starting with the 80286 model, Intel microprocessors perform address translation in two different ways called real mode and protected mode. We’ll focus in the next sections on address translation when protected mode is enabled. Real mode exists mostly to maintain processor compatibility with older models and to allow the operating system to bootstrap (see Appendix A for a short description of real mode).

<h3 id="2.2.1">2.2.1 段选择器和段寄存器</h3>

A logical address consists of two parts: a segment identifier and an offset that specifies the relative address within the segment. The segment identifier is a 16-bit field called the Segment Selector (see Figure 2-2), while the offset is a 32-bit field. We’ll describe the fields of Segment Selectors in the section “Fast Access to Segment Descriptors” later in this chapter.

<img id="Figure_2-2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_2.PNG">

图2-2 段选择器格式

To make it easy to retrieve segment selectors quickly, the processor provides segmentation registers whose only purpose is to hold Segment Selectors; these registers are called cs, ss, ds, es, fs, and gs. Although there are only six of them, a program can reuse the same segmentation register for different purposes by saving its content in memory and then restoring it later.

Three of the six segmentation registers have specific purposes:

* cs 
    
    The code segment register, which points to a segment containing program instructions

* ss 
    
    The stack segment register, which points to a segment containing the current program stack

* ds 

    The data segment register, which points to a segment containing global and static data

The remaining three segmentation registers are general purpose and may refer to arbitrary data segments.

The `cs` register has another important function: it includes a 2-bit field that specifies the Current Privilege Level (CPL) of the CPU. The value 0 denotes the highest privilege level, while the value 3 denotes the lowest one. Linux uses only levels 0 and 3, which are respectively called Kernel Mode and User Mode.

<h3 id="2.2.2">2.2.2 段描述符</h3>

Each segment is represented by an 8-byte Segment Descriptor that describes the segment characteristics. Segment Descriptors are stored either in the Global Descriptor Table (GDT) or in the Local Descriptor Table (LDT).

Usually only one GDT is defined, while each process is permitted to have its own LDT if it needs to create additional segments besides those stored in the GDT. The address and size of the GDT in main memory are contained in the `gdtr` control register, while the address and size of the currently used LDT are contained in the `ldtr` control register.

Figure 2-3 illustrates the format of a Segment Descriptor; the meaning of the various fields is explained in Table 2-1.

Table 2-1. Segment Descriptor fields

| Field name | Description |
| ---------- | ----------- |
| Base   | Contains the linear address of the first byte of the segment. |
| G      | 粒度，如果=0，段按字节表示；否则，按4K的倍数表示。|
| Limit  | 该段最后一个内存单元的偏移量，相当于长度。如果G=0，段的大小为1~1MB；否则为4K~4GB。|
| S      | 系统标志：如果被清除，则该段是一个系统段，用来存储关键数据结构，比如LDT；否则，就是正常代码段或数据段。|
| Type   | 段类型和访问权限。|
| DPL    | 描述符特权级别：用来限制对段的访问。它表示访问段所要求的最小CPU特权级别。因此，如果一个段的特权级别被设置为0，则只有CPL也是0的时候才能访问，也就是说只有内核模式才能访问。如果将DPL设置为3，任何CPL的值都可以访问该段，也就是说，什么模式都可以访问该段。|
| P      | 段当前标志:如果段当前没有存储在主存中，则等于0。Linux总是将这个标志(第47位)设置为1，因为它从不将整个段交换到磁盘。 |
| D or B | Called D or B depending on whether the segment contains code or data. Its meaning is slightly different in the two cases, but it is basically set (equal to 1) if the addresses used as segment offsets are 32 bits long, and it is cleared if they are 16 bits long (see the Intel manual for further details). |
| AVL    | May be used by the operating system, but it is ignored by Linux. |

There are several types of segments, and thus several types of Segment Descriptors. The following list shows the types that are widely used in Linux.

* *代码段描述符*
* *数据段描述符*
* *任务状态段描述符*
* *LDT描述符*


<img id="Figure_2-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_3.PNG">

图2-3 段描述符格式

<h3 id="2.2.3">2.2.3 快速访问段描述符</h3>

We recall that logical addresses consist of a 16-bit Segment Selector and a 32-bit Offset, and that segmentation registers store only the Segment Selector.

To speed up the translation of logical addresses into linear addresses, the 80×86 processor
provides an additional nonprogrammable register—that is, a register that cannot
be set by a programmer—for each of the six programmable segmentation
registers. Each nonprogrammable register contains the 8-byte Segment Descriptor
(described in the previous section) specified by the Segment Selector contained in the
corresponding segmentation register. Every time a Segment Selector is loaded in a segmentation
register, the corresponding Segment Descriptor is loaded from memory
into the matching nonprogrammable CPU register. From then on, translations of logical
addresses referring to that segment can be performed without accessing the GDT

or LDT stored in main memory; the processor can refer only directly to the CPU register
containing the Segment Descriptor. Accesses to the GDT or LDT are necessary
only when the contents of the segmentation registers change (see Figure 2-4).

<img id="Figure_2-4" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_4.PNG">

图2-4 段选择器和段描述符

Any Segment Selector includes three fields that are described in Table 2-2.

Table 2-2. Segment Selector fields
| Field name | Description |
| ---------- | ----------- |
| index | Identifies the Segment Descriptor entry contained in the GDT or in the LDT (described further in the text following this table). |
| TI    | Table Indicator: specifies whether the Segment Descriptor is included in the GDT (TI = 0) or in the LDT (TI = 1). |
| RPL   | Requestor Privilege Level: specifies the Current Privilege Level of the CPU when the corresponding Segment Selector is loaded into the cs register; it also may be used to selectively weaken the processor privilege level when accessing data segments (see Intel documentation for details). |

Because a Segment Descriptor is 8 bytes long, its relative address inside the GDT or
the LDT is obtained by multiplying the 13-bit index field of the Segment Selector by
8. For instance, if the GDT is at 0x00020000 (the value stored in the gdtr register) and
the index specified by the Segment Selector is 2, the address of the corresponding
Segment Descriptor is 0x00020000 + (2 × 8), or 0x00020010.

The first entry of the GDT is always set to 0. This ensures that logical addresses with
a null Segment Selector will be considered invalid, thus causing a processor exception.
The maximum number of Segment Descriptors that can be stored in the GDT is
8,191 (i.e., 2^13–1).

<h3 id="2.2.4">2.2.4 段硬件单元</h3>

Figure 2-5 shows in detail how a logical address is translated into a corresponding
linear address. The segmentation unit performs the following operations:

* Examines the TI field of the Segment Selector to determine which Descriptor Table stores the Segment Descriptor. This field indicates that the Descriptor is either in the GDT (in which case the segmentation unit gets the base linear address of the GDT from the gdtr register) or in the active LDT (in which case the segmentation unit gets the base linear address of that LDT from the ldtr register).

* Computes the address of the Segment Descriptor from the `index` field of the Segment Selector. The `index` field is multiplied by 8 (the size of a Segment Descriptor), and the result is added to the content of the `gdtr` or `ldtr` register.

* Adds the offset of the logical address to the Base field of the Segment Descriptor, thus obtaining the linear address.


<img id="Figure_2-5" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_5.PNG">

图2-5 转换逻辑地址

Notice that, thanks to the nonprogrammable registers associated with the segmentation registers, the first two operations need to be performed only when a segmentation register has been changed.

<h2 id="2.3">2.3 Linux中的分段机制</h2>

Segmentation has been included in 80×86 microprocessors to encourage programmers to split their applications into logically related entities, such as subroutines or global and local data areas. However, Linux uses segmentation in a very limited way. In fact, segmentation and paging are somewhat redundant, because both can be used to separate the physical address spaces of processes: segmentation can assign a different linear address space to each process, while paging can map the same linear address space into different physical address spaces. Linux prefers paging to segmentation for the following reasons:

* Memory management is simpler when all processes use the same segment register values—that is, when they share the same set of linear addresses.

* One of the design objectives of Linux is portability to a wide range of architectures; RISC architectures in particular have limited support for segmentation.

The 2.6 version of Linux uses segmentation only when required by the 80×86 architecture.

All Linux processes running in User Mode use the same pair of segments to address instructions and data. These segments are called *user code segment* and *user data segment*, respectively. Similarly, all Linux processes running in Kernel Mode use the same pair of segments to address instructions and data: they are called *kernel code segment* and *kernel data segment*, respectively. Table 2-3 shows the values of the Segment Descriptor fields for these four crucial segments.

Table 2-3. Values of the Segment Descriptor fields for the four main Linux segments
| Segment     | Base       | G | Limit   | S | Type | DPL | D/B | P |
| ----------- | ---------- | - | -----   | - | ---- | --- | --- | - |
| user code   | 0x00000000 | 1 | 0xfffff | 1 | 10   | 3   | 1   | 1 |
| user data   | 0x00000000 | 1 | 0xfffff | 1 | 2    | 3   | 1   | 1 |
| kernel code | 0x00000000 | 1 | 0xfffff | 1 | 10   | 0   | 1   | 1 |
| kernel data | 0x00000000 | 1 | 0xfffff | 1 | 2    | 0   | 1   | 1 |

The corresponding Segment Selectors are defined by the macros `__USER_CS`, `__USER_DS`, `__KERNEL_CS`, and `__KERNEL_DS`, respectively. To address the kernel code segment, for instance, the kernel just loads the value yielded by the `__KERNEL_CS` macro into the `cs` segmentation register.
Notice that the linear addresses associated with such segments all start at 0 and reach the addressing limit of 232 –1. This means that all processes, either in User Mode or
in Kernel Mode, may use the same logical addresses.

Another important consequence of having all segments start at `0x00000000` is that in
Linux, logical addresses coincide with linear addresses; that is, the value of the Offset
field of a logical address always coincides with the value of the corresponding linear
address.

As stated earlier, the Current Privilege Level of the CPU indicates whether the processor
is in User or Kernel Mode and is specified by the `RPL` field of the Segment Selector
stored in the `cs` register. Whenever the CPL is changed, some segmentation registers
must be correspondingly updated. For instance, when the `CPL` is equal to 3 (User
Mode), the `ds` register must contain the Segment Selector of the user data segment,
but when the CPL is equal to 0, the `ds` register must contain the Segment Selector of
the kernel data segment.

A similar situation occurs for the `ss` register. It must refer to a User Mode stack
inside the user data segment when the CPL is 3, and it must refer to a Kernel Mode
stack inside the kernel data segment when the CPL is 0. When switching from User
Mode to Kernel Mode, Linux always makes sure that the `ss` register contains the Segment
Selector of the kernel data segment.

When saving a pointer to an instruction or to a data structure, the kernel does not
need to store the Segment Selector component of the logical address, because the `ss`
register contains the current Segment Selector. As an example, when the kernel
invokes a function, it executes a `call` assembly language instruction specifying just
the Offset component of its logical address; the Segment Selector is implicitly selected
as the one referred to by the `cs` register. Because there is just one segment of type
“executable in Kernel Mode,” namely the code segment identified by `__KERNEL_CS`, it
is sufficient to load `__KERNEL_CS` into `cs` whenever the CPU switches to Kernel Mode.
The same argument goes for pointers to kernel data structures (implicitly using the `ds`
register), as well as for pointers to user data structures (the kernel explicitly uses the
`es` register).

Besides the four segments just described, Linux makes use of a few other specialized segments. We’ll introduce them in the next section while describing the Linux GDT.

<h3 id="2.3.1">2.3.1 Linux GDT</h3>

单处理器系统只有一个GDT，而多处理器系统中每一个CPU都有一个GDT。所有的GDT存储在`cpu_gdt_table`数组中，而GDT的地址和大小存储在`cpu_gdt_descr`数组中，其中GDT的大小用来初始化`gdtr`寄存器。如果查看源代码，这些符号的定义位于文件`arch/i386/kernel/head.S`中，说明根据处理器架构不同，这些定义也是不同的。本书中所有的宏、函数和其它符号都来源于源代码，可以轻松找到。

GDT的布局可以参考图2-6.每一个GDT包含18个段描述符和14个空、未使用和保留的项。之所以插入未使用的项是因为，段描述符通常在[cache]()中是按照32字节的line一起访问的。

<img id="Figure_2-6" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_6.PNG">

图2-6 全局描述符表-GDT

这18个段描述符指向下面这些段：

* 4个用户和内核代码和数据段

* 任务状态段(TSS)，系统中的每个CPU都不相同。
different for each processor in the system. The linear address space corresponding to a TSS is a small subset of the linear address space corresponding to the kernel data segment. The Task State Segments are sequentially stored in the `init_tss` array; in particular, the Base field of the TSS descriptor for the Nth CPU points to the Nth component of the `init_tss` array. The G (granularity) flag is cleared, while the Limit field is set to 0xeb, because the TSS segment is 236 bytes long. The Type field is set to 9 or 11 (available 32-bit TSS), and the DPL is set to 0, because processes in User Mode are not allowed to access TSS segments. You will find details on how Linux uses TSSs in the section “Task State Segment” in Chapter 3.

* A segment including the default Local Descriptor Table (LDT), usually shared by all processes (see the next section).

* Three Thread-Local Storage (TLS) segments: this is a mechanism that allows multithreaded applications to make use of up to three segments containing data local to each thread. The *set_thread_area()* and *get_thread_area()* system calls, respectively, create and release a TLS segment for the executing process.

* Three segments related to Advanced Power Management (APM): the BIOS code makes use of segments, so when the Linux APM driver invokes BIOS functions to get or set the status of APM devices, it may use custom code and data segments.

* Five segments related to Plug and Play (PnP) BIOS services. As in the previous case, the BIOS code makes use of segments, so when the Linux PnP driver invokes BIOS functions to detect the resources used by PnP devices, it may use custom code and data segments.

* A special TSS segment used by the kernel to handle “Double fault” exceptions (see “Exceptions” in Chapter 4).

As stated earlier, there is a copy of the GDT for each processor in the system. All copies of the GDT store identical entries, except for a few cases. First, each processor has its own TSS segment, thus the corresponding GDT’s entries differ. Moreover, a few entries in the GDT may depend on the process that the CPU is executing (LDT and TLS Segment Descriptors). Finally, in some cases a processor may temporarily modify an entry in its copy of the GDT; this happens, for instance, when invoking an APM’s BIOS procedure.

<h3 id="2.3.2">2.3.2 Linux LDT</h3>

Most Linux User Mode applications do not make use of a Local Descriptor Table, thus the kernel defines a default LDT to be shared by most processes. The default Local Descriptor Table is stored in the `default_ldt` array. It includes five entries, but only two of them are effectively used by the kernel: a call gate for iBCS executables, and a call gate for Solaris/x86 executables (see the section “Execution Domains” in Chapter 20). Call gates are a mechanism provided by 80×86 microprocessors to change the privilege level of the CPU while invoking a predefined function; as we won’t discuss them further, you should consult the Intel documentation for more details.

In some cases, however, processes may require to set up their own LDT. This turns out to be useful to applications (such as Wine) that execute segment-oriented Microsoft Windows applications. The `modify_ldt()` system call allows a process to do this.

Any custom LDT created by modify_ldt() also requires its own segment. When a processor starts executing a process having a custom LDT, the LDT entry in the CPU-specific copy of the GDT is changed accordingly.

User Mode applications also may allocate new segments by means of modify_ldt(); the kernel, however, never makes use of these segments, and it does not have to keep track of the corresponding Segment Descriptors, because they are included in the custom LDT of the process.

<h2 id="2.4">2.4 内存分页</h2>

分页单元将线性地址转换成物理地址。分页单元主要的任务就是根据线性地址的访问权限检查请求的访问类型。如果访问无效，则产生一个页错误异常（具体看[第4章](#4)和[第8章](#8)）。

为了效率，将线性地址按照固定长度组织，称为页；一个页的连续线性地址映射为连续的物理地址。通过这种方式，内核可以按页指定物理地址和访问权限，而不是对整个地址进行操作，增加了灵活性。基于传统习惯，我们使用 *page*这个词表示一组线性地址及其存储内容。

分页单元把所有的RAM分为固定长度的`页帧`（有时，也称为物理页）。每一个页帧都包含一个页，也就是说，页帧的长度和页的长度是一致的。页帧是内存的组成部分，是指一段存储区域。弄清楚`页`和`页帧`这两个概念是非常重要的；前者是一块数据，可能存储在任何页帧或硬盘上。

映射线性地址到物理地址的数据结构，称为页表；它们存储在内存中，使能分页单元之前必须被正确初始化。

从80386架构开始，所有的x86架构处理器都支持分页机制；通过控制寄存器cr0的PG标志位进行设置。当PG=0，线性地址就是物理地址。

> <font color="red">注：</font>
> <font color="red">也就是分段机制已经被废弃了，所以暂时不深究了。</font>

<h3 id="2.4.1">2.4.1 常规分页</h3>

从80386架构开始，英特尔的分页单元将页的大小划分为4KB。32位的线性地址被分为3个域：

* 目录 - 最高有效10位

* 表 - 中间10位

* 偏移量 - 最低有效12位

线性地址的转换分为2步，每一步都依赖一个转换表。第一级转换表称为`页目录`，第二级称为`页表`。

> 在后面的讨论中，小写的`page table`表示存储着线性地址和物理地址映射关系的任何页，而大写的`Page Table`术语表示最后一级页表中的页。

2级页表的方案的目的是减少每个进程页表所占的RAM数量。如果只使用1级页表，为了按照4K大小的页访问，那就需要多达2^20项，每项占用4字节，需要4M的RAM，而且还有那么多进程，想想就很可怕。另外，即使一个进程不使用所有的线性地址空间，一级页表也还是这么大。这大大降低了RAM的实际使用率。而使用2级页表可以减少内存的使用，因为第二级页表只是进程实际使用的虚拟地址空间。（第二级页表是进程在创建的时候动态申请的）

每个激活的进程必须有一个页目录。但是，无需一次为进程的所有二级页表分配内存。只当进程需要某个二级页表的时候再给其分配内存，这样更有效率。

页目录的物理地址被存储在控制寄存器`cr3`。线性地址中的 *Directory*域决定了在页目录中的第几项，而该项的内容又指向恰当的二级页表。*Table*域，决定了在页表中的第几项，而该项的内容就是对应的页帧的物理地址。*Offset*域决定了在页帧中的相对位置（参考图2-7）。*Offset*域所占的长度是12位，正好是每页4096个字节的大小。

<img id="Figure_2-7" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_7.PNG">

图2-7 80×86处理器分页框图

*Directory*和 *Table*域的长度都是10位，所以所能寻址的项数都是1024项。结果就是，一个页目录可以寻址的大小为`1024 × 1024 × 4096=2^32`，正与期望的32位地址相同。

页目录和页表中的项具有相同的结构。每一项包含下面这些内容：

* Present标志
    
    如果被设置，说明指向的页（或二级页表）被包含在内存中；如果该标志为0，内存中没有该页，该项中的其它位由操作系统作其它目的使用。如果需要执行地址转换的二级页表或页目录中的项中的`Present`标志被清除，分页单元就将该线性地址存储到控制寄存器`cr2`中并产生异常14-页错误异常。（我们将在[第17章 Linux如何使用该标志](#17)中看到。）

    <font color="blue"> 通俗地讲，使用该标志表示要访问的地址在内存中存不存在，不存在的话，就会产生页错误异常。</font>

* 包含页帧物理地址中最重要的20位的域
    
    因为每个页帧具有4KB的容量，它的物理地址肯定是4096的倍数，所以，所包含的物理地址的低12位总是0。如果该域是指向页目录的，页帧中包含的是一个页表；如果它指向一个页表，页帧中包含的是数据所在的页。

* 访问标志
    
    每次分页单元访问相应的页帧时，设置该标志。操作系统在选择要换出的页时，使用该标志。分页单元绝不会复位该标志，应该由操作系统完成。

* Dirty标志
    
    只对页表项有用。每次对页帧实行写操作的时候，设置该标志。因为和Accessed标志一样，也是操作系统在选择要换出的页时使用该标志。分页单元绝不会复位该标志，应该由操作系统完成。

* Read/Write标志

    页或页表的访问权限（读写、只读）。具体参考本章后面的[硬件保护机制](#2.4.3)。

* User/Supervisor标志

    访问页或页表所需要的特权级别。具体参考本章后面的[硬件保护机制](#2.4.3)。

* PCD和PWT标志

    控制硬件Cache处理页或页表的方式。具体参考本章后面的[硬件Cache](#2.4.7)。

* 页大小标志

   只对页目录有效。如果被设置，每项指向2MB或4MB大小的页帧（后面会有介绍）。

* 全局标志

   只对页表项有用。这个标记是在Pentium Pro中引入的，用于防止频繁使用的页面从TLB缓存中被刷新(请参阅本章后面的[转换后备缓存-TLB](#2.4.8)一节)。只有在设置了寄存器cr4的页全局使能(PGE)标志的情况下才能工作。


<h3 id="2.4.2">2.4.2 扩展分页</h3>

从奔腾系列开始，80×86架构处理器引入了扩展分页，允许页帧的大小为4M，从而代替4K（看图2-8）。扩展分页用来转换大段连续的线性地址到相应的物理内存。内核无需中间的二级页表即可以保存内存，且保护TLB中的项不被频繁的刷新。（参考后面的[转换后备缓存-TLB](#2.4.8)一节）

<img id="Figure_2-8" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_8.PNG">

图2-8 扩展分页

正如前面提到的，通过设置页目录项中的页大小标志来使能扩展分页。这时，分页单元把32位线性地址分成2部分：

* 目录 - 最高有效10位

* 偏移量 - 剩下的22位
  
使能了扩展分页的页目录项和普通的分页几乎相同，除了：

* 必须设置`页大小`标志。
* 只有物理地址20位中的最高10位有效。这是因为低22位全被设为0。
* 扩展分页和常规分页共存。通过cr4寄存器中的PSE标志进行使能。

<h3 id="2.4.3">2.4.3 硬件保护</h3>

分页单元使用与分段单元不一样的保护机制。尽管80x86允许4个可能的特权级别访问一个段，但是分页机制只使用了2个特权级别访问页和页表，特别级别的设定已经在[常规分页](#2.4.1)中提到，使用`User/Supervisor`标志。当这个标志为0，页只能被CPL低于3的特权级别访问（对于Linux，这意味着处理器此时处于内核模式）；当该标志为1，总是可以访问页。

另外，代替分段机制时的三种访问权限（读、写、和执行），分页机制只有两种访问权限（读和写）。如果页目录或二级页表的读/写标志等于0，相应的二级页表或页是只读的；否则，可被读写。

<h3 id="2.4.4">2.4.4 常规分页的示例</h3>

我们使用一个简单的示例阐明常规分页的工作机理。假定内核分配线性地址0x20000000~0x2003ffff给正在运行的进程。这段地址实际是64个页。我们不关心页帧的物理地址；事实上，有些也有可能不在内存范围内。我们感兴趣的只是二级页表项的其余域。

我们从该线性地址的最高有效10位开始，它们被分页单元解释为页面目录域。该段地址以2开始，后面紧跟0，所以最高10位的值相同，都是0x080或128。所以，所有地址的目录域都指向该进程页表目录的第129项。该项中肯定包含分配给进程的二级页表的物理地址（如图2-9所示）。如果没有其它线性地址分配给进程，页目录其余的1023项都会被填充0。

<img id="Figure_2-9" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_9.PNG">

图2-9 常规分页示例

赋给中间10位的值，范围是0~0x03f，或者说0~63。也就是说，只有二级页表的前64项是合法的。其余的960项被填充0。

假设该进程想要读取线性地址0x20021406处的字节值。分页单元的处理过程如下：

1. 使用目录域的值0x80锁定页目录中的第0x80项，其中的内容指向该进程的页所对应的二级页表。

2. 页表域的值0x21用来锁定二级页表的第0x21项，它指向想要访问页的页帧。

3.最后使用偏移值0x406选择想要的页帧，在偏移量0x406处的值。

如果二级页表第0x21项的Present标志被清除，想要访问的地址所在页不在内存中。这时，分页单元发送一个页错误异常。如果进程想要访问地址0x20000000~0x2003ffff范围之外的地址，也会发生相同的页错误异常。上面提到二级页表的其余未用的项被填充0，尤其是，Present标志全被清除。

<h3 id="2.4.5">2.4.5 物理地址扩展(PAE)分页机制</h3>

处理器能够访问的RAM数量是受到总线宽度的影响的。从80386到奔腾系列，这些旧英特尔处理器都是使用32位物理地址。理论上，可以使用4GB的RAM；实际上，因为用户模式进程的线性地址要求，内核不能直接寻址超过1GB的RAM，比如，在32位系统上，页上层目录和页中层目录被消除了，这将在[Linux分页机制](#2.5)中提到。

但是，大型服务器需要同时运行成千上万个进程，这往往要求RAM大于4GB。这就要求英特尔扩展32位80x86架构支持的RAM数量。

英特尔通过增加CPU的地址线，32位扩展到36位，来满足这个要求。从奔腾Pro开始，所有的英特尔处理器能够寻址2^36=64GB的RAM。但是，增加了物理地址的范围，也需要新分页硬件单元，完成将32位线性地址转换成36位的物理地址。

通过奔腾Pro处理器，英特尔引入一种机制，称为物理地址扩展（PAE）。奔腾III处理器还引入了另外一种机制，称为页大小扩展（PSE-36），但是Linux并没有使用，所以，我们不再介绍。

PAE可以通过设置cr4寄存器中PAE标志激活。页目录项中的页大小（PS）标志能够使能大页访问（PAE被使能时，是2M）。

为了支持PAE，英特尔改变了硬件分页单元：

* 64G的物理内存被分割成2^24个不同的页帧，页表项的物理地址域从20位扩展到24位。因为基于PAE的最后一级页表中的项必须是12位（前面的[常规分页](#2.4.1)中已经讲述过），再加上24位的物理地址，总位数达到36位，页表项的大小必须从32位扩展到64位才能满足要求。结果，4K页大小的PAE页表中包含512项而不是之前说的1024项。

* 多出来的一级页表称为页目录指针表（PDPT），包含4个64位的项。

* cr3寄存器控制27位页目录指针表基地址。因为PDPT存储在物理内存的第一个4G空间中，而且排列是32字节的倍数（2^5），所以27足够表达这个表的基地址了。

* 当最小寻址单元是4KB页大小时（页目录项中的PS标志被清除），32位的线性地址按照下面的方式解释：

    - cr3 - 指向一个PDPT

    - 位31–30 - 指向PDPT中四个可能项中的一个

    - 位29–21 - 指向页目录中的512项中的一个

    - 位20–12 - 指向页表中的512项中的一个

    - 位11–0 - 4K页中的偏移

* 当最小寻址单元是2MB页大小时（页目录项中的PS标志被设置），32位的线性地址按照下面的方式解释：

    - cr3 - 指向一个PDPT

    - 位31–30 - 指向PDPT中4项中的一个

    - 位29–21 - 指向页目录中的512项中的一个

    - 位20–0 - 2M页中的偏移

总之，一旦设置了cr3，可能的寻址范围达到4GB的RAM。如果我们想要寻址更大的RAM，我们必须改变cr3的值或者改变PDPT中的内容。但是，PAE的问题是，线性空间仍然是32位的。也就是说，每个进程最大的访问空间仍然是4G，这迫使编程者不断使用相同的线性地址访问不同的物理内存区域。我们将会在后面的段落中描述启用PAE，物理内存大于4G的情况下，Linux如何初始化页表。但是，PAE毕竟使内核可以利用的内存达到了64G，这可以显著增加系统中的进程数量。

<h3 id="2.4.6">2.4.6 64位架构分页机制</h3>

正如我们前面讲述的，32位系统通常使用2级页表。但是，2级页表对于采用64位架构的计算机却不合适。我们做一个假想性的实验来解释为什么：

假设标准页大小还是4KB。因为4KB覆盖2^12地址，所以`Offset`域为12位。这还剩余52位线性地址分配给`Table`和`Directory`域。如果只使用64位中的48位进行寻址（这样的话，可以有256TB的地址空间），剩余48-12=36位分配给`Table`和`Directory`域。如果我们给这2个域各保留18位，那么每个进程的页面目录和最后一级页表应该包含2^18项-也就是说，超过256000项。这太耗费内存了。

基于上面的原因，64位处理器都使用多级页表。具体依赖于处理器。表2-4总结了Linux支持的64位系统，它们的硬件分页单元的主要特性。

表2-4. 64位架构中的页表结构

| 平台名称 | 页大小 | 使用的地址位数 | 页表数量 | 线性地址分割 |
| -------- | ------ | -------------- | -------- | ------------ |
| alpha    | 8 KB*  | 43             | 3        | 10 + 10 + 10 + 13  |
| ia64     | 4 KB   | 39             | 3        | 9 + 9 + 9 + 12     |
| ppc64    | 4 KB   | 41             | 3        | 10 + 10 + 9 + 12   |
| sh64     | 4 KB   | 41             | 3        | 10 + 10 + 9 + 12   |
| x86_64   | 4 KB   | 48             | 4        | 9 + 9 + 9 + 9 + 12 |

> *该架构支持不同的页大小，在这儿，我们选择Linux采用的典型大小。因为该架构已经不再研发，可以不研究。

在后面的[Linux的分页机制](#2.5)中我们会讲到，Linux设计了一个通用分页模型，适合大部分硬件系统。

<h3 id="2.4.7">2.4.7 硬件Cache</h3>

今天，微处理器的时钟频率都是几GHz，但是DRAM的时钟频率却不能匹配。时钟频率的不匹配导致从RAM读取数据都会变慢，影响CPU指令的执行效率。

为了减少CPU和RAM之间的速度不匹配，引入了Cache。Cache的设计遵循局部性原理，由于程序的循环结构和相关数据一般保存成线型数组，接近最近使用的地址有很高的可能性接下来被使用。因此，引入一个小容量、读取速度快的内存，包含最近使用的代码和数据就很有意义。一种新的硬件单元被引入80x86架构，称为`line`。它包含几十个连续的字节，在慢速DRAM和Cache之间以快速模式传输，一般使用快速片上静态RAM（SRAM）实现Cache。

Cache被分成若干行，通常称为`Cache line`或者`Cache行`。一种极端情况就是，Cache中的行和内存中的行是一一对应、直接映射的关系。另一种情况是，Cache是全相关的，意味着内存中的任何行可以存储到Cache中的任意行。但是，大部分Cache使用的是N路相关，内存中的任意行可以存储到Cache的N行中的任意一个。比如，一个2路相关的Cache中，内存的一行可以被存储到该Cache中的2个行中的任意一个。

如果2-10所示，Cache硬件单元位于分页管理单元和主内存之间，包含一个Cache内存和Cache控制器。Cache内存用来存储来自主内存的实际行。Cache控制器存储着Cache内存中行数，也就是有多少项，使用数组管理。每项包含一个TAG和描述Cache行状态的一些标志。TAG由一些位组成，这些位允许Cache控制器识别当前该行映射的内存位置。内存的物理地址通常分为3组：最高的一些位对应TAG，中间的一些位对应Cache控制器的index，最低的那些位用来标识在行内的偏移量。

<img id="Figure_2-10" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_10.PNG">

图2-10 硬件cache

When accessing a RAM memory cell, the CPU extracts the subset index from the physical address and compares the tags of all lines in the subset with the high-order bits of the physical address. If a line with the same tag as the high-order bits of the address is found, the CPU has a `cache hit`; otherwise, it has a `cache miss`.

为了访问RAM内存单元，CPU从物理地址中抽取
When a cache hit occurs, the cache controller behaves differently, depending on the
access type. For a read operation, the controller selects the data from the cache line
and transfers it into a CPU register; the RAM is not accessed and the CPU saves time,
which is why the cache system was invented. For a write operation, the controller
may implement one of two basic strategies called write-through and write-back. In a
write-through, the controller always writes into both RAM and the cache line, effectively
switching off the cache for write operations. In a write-back, which offers more
immediate efficiency, only the cache line is updated and the contents of the RAM are
left unchanged. After a write-back, of course, the RAM must eventually be updated.
The cache controller writes the cache line back into RAM only when the CPU executes
an instruction requiring a flush of cache entries or when a FLUSH hardware
signal occurs (usually after a cache miss).

When a cache miss occurs, the cache line is written to memory, if necessary, and the correct line is fetched from RAM into the cache entry.

多处理器系统为每一个处理器提供一个cache，因此需要外部提供同步cache内容的硬件电路。如图2-11所示，每一个CPU都有自己局部的cache。这个更新增加了耗时：无论何时CPU修改了它的cache，它必须检查其它的cache是否包含相同的数据；否则，它必须通知其它CPU更新成正确的值。这个操作称为 *cache snooping*。幸运的是，这些都是硬件完成的，内核无需关心。

> ARM架构类似的电路称为 *Snoop Control Unit-SCU*。

<img id="Figure_2-11" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_11.PNG">

图2-11 双核架构中的cache

Cache technology is rapidly evolving. For example, the first Pentium models included a single on-chip cache called the L1-cache. More recent models also include other larger, slower on-chip caches called the L2-cache, L3-cache, etc. The consistency between the cache levels is implemented at the hardware level. Linux ignores these hardware details and assumes there is a single cache.

The CD flag of the cr0 processor register is used to enable or disable the cache circuitry.
The NW flag, in the same register, specifies whether the write-through or the
write-back strategy is used for the caches.

Another interesting feature of the Pentium cache is that it lets an operating system
associate a different cache management policy with each page frame. For this purpose,
each Page Directory and each Page Table entry includes two flags: PCD (Page
Cache Disable), which specifies whether the cache must be enabled or disabled while
accessing data included in the page frame; and PWT (Page Write-Through), which
specifies whether the write-back or the write-through strategy must be applied while
writing data into the page frame. Linux clears the PCD and PWT flags of all Page Directory
and Page Table entries; as a result, caching is enabled for all page frames, and
the write-back strategy is always adopted for writing.

<h3 id="2.4.8">2.4.8 转换后备缓存-TLB</h3>

Besides general-purpose hardware caches, 80×86 processors include another cache
called Translation Lookaside Buffers (TLB) to speed up linear address translation.
When a linear address is used for the first time, the corresponding physical address is
computed through slow accesses to the Page Tables in RAM. The physical address is
then stored in a TLB entry so that further references to the same linear address can
be quickly translated.

In a multiprocessor system, each CPU has its own TLB, called the local TLB of the
CPU. Contrary to the hardware cache, the corresponding entries of the TLB need not
be synchronized, because processes running on the existing CPUs may associate the
same linear address with different physical ones.

When the cr3 control register of a CPU is modified, the hardware automatically
invalidates all entries of the local TLB, because a new set of page tables is in use and
the TLBs are pointing to old data.

<h2 id="2.5">2.5 Linux中的分页机制</h2>

Linux采用了适用于32位和64位架构的通用分页模型。正如在之前的[64位架构分页](#2.4.6)所解释的那样，2级页表对于32位系统而言足够了，而64位系统架构要求更多数量的分级页表。版本2.6.11之前，Linux分页模型由3级页表组成；从2.6.11开始，采用4级页表模型。如图2-12所示，它们分别称为：

* 页全局目录
* 页上层目录
* 页中间目录
* 最后一级页表

页全局目录包含页上层目录的地址，这些地址又包含页中层目录的地址，这些页中间目录又包含几个页表，而每个页表又指向一个页帧。也就是说，线性地址被分割成5部分。图2-12没有展示这些位，因为每个部分的大小依赖于计算机的架构。

<img id="Figure_2-12" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_12.PNG">

图2-12 linux分页模型

对于没有进行PAE物理地址扩展的32位架构来说，2级页表足够了。Linux通过填充0，把页上层目录和页中间目录域给消除了。但本质上，页上层目录和页中间目录还是存在的，所以在32位和64位架构上能运行相同的代码。内核通过设置页上层目录和页中间目录中的项数为1，将其位置保留，并将其映射到页全局目录中。

对于使能了PAE扩展内存的32位系统，则是使用3级页表结构。Linux的页全局目录对应于80x86的页目录指针表，页上层目录被消除，页中间目录对应80x86的页目录，Linux的页表对应于80x86的页表。

最后，对于64位架构来说，使用3级页表结构还是4级页表结构，完全依赖于硬件将线性地址分割成几部分。（参见表2-4）

Linux对进程的处理严重依赖于分页单元。事实上，将线性地址自动转换成物理地址，使下面的目标成为可能：

* 为每个进程分配不同的物理地址空间，确保有效地防止寻址错误。

* 区分页（数据）和页帧（内存中的物理地址）。这保证了相同的页被保存在同一个页帧中，然后再写入硬盘中，而后可以再加载到不同的页帧中。这是虚拟物理内存的基本构成要素。（参见[第17章](#17)）

在本章的其余部分，为了具体起见，我们将提到80×86处理器使用的分页硬件单元。

正如我们将在[第9章](#9)中看到的，每个进程拥有自己的页全局目录和一组页表。当发生进程切换时，Linux保存cr3控制寄存器，其存储在先前执行的进程描述符中；然后把下一个要执行的进程的描述符的值存入cr3寄存器中。然后，新进程继续在CPU上执行，分页单元指向正确的页表。

尽管分页机制很复杂，但是将线性地址映射成物理地址现在变成了一个硬件任务，无需我们操心。我们需要关心的是，如何检索内核查找地址所需的信息，如何操作这些页表。本节后面的部分介绍了这些函数和宏。

<h3 id="2.5.1">2.5.1 线性地址域</h3>

下面的宏简化了页表的处理：

* PAGE_SHIFT
    
    指定`Offset`域的长度，也就是位数。对于80x86处理器，该宏的值是12。因为一个页中的所有地址必须用`Offset`域能够索引到，所以页的大小就是2^12，也就是4096个字节。PAGE_SIZE会使用该宏返回一个页的大小。PAGE_MASK是0xfffff000，作为`Offset`域的掩码。

* PMD_SHIFT
    
    线性地址`Offset`和`Table`域的位的总长度；换句话说，页中间目录项能够映射的区域的大小。PMD_SIZE宏使用该宏计算出页中间目录项映射区域的大小，也就是一个页表的大小。PMD_MASK宏是`Offset`和`Table`域的掩码。

    当PAE禁止时，PMD_SHIFT的值是22（12位的`Offset`和10位的`Table`），PMD_SIZE产生的大小是2^22或4MB，PMD_MASK的值是0xffc00000。相反，如果PAE被使能，PMD_SHIFT的值是21（12位的`Offset`和9位的`Table`），PMD_SIZE产生的大小是2^21或2MB，PMD_MASK的值是0xffe00000。

    大页不使用最后一级页表，LARGE_PAGE_SIZE是大页的大小，等于PMD_SIZE(2PMD_SHIFT)，同时，LARGE_PAGE_MASK等于PMD_MASK

* PUD_SHIFT
    
    确定页上层目录项可以映射的区域大小的对数。PUD_SIZE计算页上层目录中的一项可以映射的区域大小。PUD_MASK是`Table`、`Offset`、`Middle Air`3个域的位掩码。在80x86系统上，PUD_SHIFT总是等于PMD_SHIFT，而PUD_SIZE等于4M或2M。

* PGDIR_SHIFT
    
    确定页全局目录项能够映射的区域大小的对数。PGDIR_SIZE计算一个页全局目录项映射区域的大小。PGDIR_MASK是对应位`Offset`、`Table`、`Middle Air`和`Upper Air` 域的位掩码。

    当PAE禁止时，PGDIR_SHIFT 的值是22（12位的`Offset`和10位的`Table`），PGDIR_SIZE 产生的大小是2^22或4MB，PGDIR_MASK的值是0xffc00000。相反，如果PAE被使能，PGDIR_SHIFT的值是30（12位的`Offset`+9位的`Table`+9位的`Middle Air`），PGDIR_SIZE产生的大小是2^30或1GB，PGDIR_MASK的值是0xc0000000。

* PTRS_PER_PTE、PTRS_PER_PMD、PTRS_PER_PUD和PTRS_PER_PGD
    
    分别计算各个表中的项数。当PAE禁止时，分别是1024、1、1和1024当PAE使能时，分别是512、512、1和4。

<h3 id="2.5.2">2.5.2 处理页表的相关函数</h3>

pte_t, pmd_t, pud_t, and pgd_t describe the format of, respectively, a Page Table, a
Page Middle Directory, a Page Upper Directory, and a Page Global Directory entry.
They are 64-bit data types when PAE is enabled and 32-bit data types otherwise.
pgprot_t is another 64-bit (PAE enabled) or 32-bit (PAE disabled) data type that represents
the protection flags associated with a single entry.

Five type-conversion macros—__pte, __pmd, __pud, __pgd, and __pgprot—cast an
unsigned integer into the required type. Five other type-conversion macros—pte_
val, pmd_val, pud_val, pgd_val, and pgprot_val—perform the reverse casting from
one of the four previously mentioned specialized types into an unsigned integer.

The kernel also provides several macros and functions to read or modify page table entries:

* pte_none, pmd_none, pud_none, and pgd_none yield the value 1 if the corresponding entry has the value 0; otherwise, they yield the value 0.

* pte_clear, pmd_clear, pud_clear, and pgd_clear clear an entry of the corresponding page table, thus forbidding a process to use the linear addresses mapped by the page table entry. The ptep_get_and_clear() function clears a Page Table entry and returns the previous value.

* set_pte, set_pmd, set_pud, and set_pgd write a given value into a page table entry; set_pte_atomic is identical to set_pte, but when PAE is enabled it also ensures that the 64-bit value is written atomically.

* pte_same(a,b) returns 1 if two Page Table entries a and b refer to the same page and specify the same access privileges, 0 otherwise.

* pmd_large(e) returns 1 if the Page Middle Directory entry e refers to a large page (2 MB or 4 MB), 0 otherwise.

The pmd_bad macro is used by functions to check Page Middle Directory entries
passed as input parameters. It yields the value 1 if the entry points to a bad Page
Table—that is, if at least one of the following conditions applies:

* The page is not in main memory (Present flag cleared).
* The page allows only Read access (Read/Write flag cleared).
* Either Accessed or Dirty is cleared (Linux always forces these flags to be set for every existing Page Table).

The pud_bad and pgd_bad macros always yield 0. No pte_bad macro is defined,
because it is legal for a Page Table entry to refer to a page that is not present in main
memory, not writable, or not accessible at all.

The pte_present macro yields the value 1 if either the Present flag or the Page Size
flag of a Page Table entry is equal to 1, the value 0 otherwise. Recall that the Page
Size flag in Page Table entries has no meaning for the paging unit of the microprocessor;
the kernel, however, marks Present equal to 0 and Page Size equal to 1 for
the pages present in main memory but without read, write, or execute privileges. In
this way, any access to such pages triggers a Page Fault exception because Present is
cleared, and the kernel can detect that the fault is not due to a missing page by
checking the value of Page Size.

The pmd_present macro yields the value 1 if the Present flag of the corresponding
entry is equal to 1—that is, if the corresponding page or Page Table is loaded in
main memory. The pud_present and pgd_present macros always yield the value 1.

The functions listed in Table 2-5 query the current value of any of the flags included
in a Page Table entry; with the exception of pte_file(), these functions work properly
only on Page Table entries for which pte_present returns 1.

表2-5. 页标志读取函数

| 函数名称      | 描述        |
| ------------- | ----------- |
| pte_user()    | Reads the User/Supervisor flag |
| pte_read()    | Reads the User/Supervisor flag (pages on the 80×86 processor cannot be protected against reading) |
| pte_write()   | Reads the Read/Write flag |
| pte_exec()    | Reads the User/Supervisor flag (pages on the 80x86 processor cannot be protected against code execution) |
| pte_dirty()   | Reads the Dirty flag |
| pte_young()   | Reads the Accessed flag |
| pte_file()    | Reads the Dirty flag (when the Present flag is cleared and the Dirty flag is set, the page belongs to a non-linear disk file mapping; see Chapter 16) |

Another group of functions listed in Table 2-6 sets the value of the flags in a Page Table entry.

表2-6. 页标志设置函数

| 函数名称      | 描述        |
| ------------- | ----------- |
| mk_pte_huge()      | 设置页表项中的Page Size和 Present标志 |
| pte_wrprotect()    | 清除读写标志 |
| pte_rdprotect()    | 清除User/Supervisor标志 |
| pte_exprotect()    | 清除User/Supervisor标志 |
| pte_mkwrite()      | 设置Read/Write标志 |
| pte_mkread()       | 设置User/Supervisor标志 |
| pte_mkexec()       | 设置User/Supervisor标志 |
| pte_mkclean()      | 清除Dirty标志 |
| pte_mkdirty()      | 设置Dirty标志 |
| pte_mkold()        | 清除Accessed标志(使该页变旧) |
| pte_mkyoung()      | 设置Accessed标志(使该页变年轻) |
| pte_modify(p,v)    | 设置页表项p为指定的值v |
| ptep_set_wrprotect()    | 同pte_wrprotect()，但是操作对象是指向页表项的指针 |
| ptep_set_access_flags() | 如果Dirty标志被设置, 设置一个页的访问权限为指定值并调用 flush_tlb_page() （参考[转换后备缓存-TLB](#2.4.8)） |
| ptep_mkdirty()                | 同pte_mkdirty()，但是操作对象是指向页表项的指针 |
| ptep_test_and_clear_dirty()   | 同pte_mkclean()但是操作对象是指向页表项的指针并返回该标志旧值 |
| ptep_test_and_clear_young()   | 同pte_mkold()但是操作对象是指向页表项的指针并返回该标志旧值 |

Now, let’s discuss the macros listed in Table 2-7 that combine a page address and a
group of protection flags into a page table entry or perform the reverse operation of
extracting the page address from a page table entry. Notice that some of these macros refer to a page through the linear address of its “page descriptor” (see the section
“Page Descriptors” in Chapter 8) rather than the linear address of the page itself.

Table 2-7. Macros acting on Page Table entries

| 函数名称      | 描述        |
| ------------- | ----------- |
| pgd_index(addr) | Yields the index (relative position) of the entry in the Page Global Directory that maps the linear address addr. |
| pgd_offset(mm, addr) | Receives as parameters the address of a memory descriptor cw (see Chapter 9) and a linear address addr. The macro yields the linear address of the entry in a Page Global Directory that corresponds to the address addr; the Page Global Directory is found through a pointer within the memory descriptor. |
| pgd_offset_k(addr) | Yields the linear address of the entry in the master kernel Page Global Directory that corresponds to the address addr (see the later section “Kernel Page Tables”). |
| pgd_page(pgd) | Yields the page descriptor address of the page frame containing the Page Upper Directory referred to by the Page Global Directory entry pgd. In a twoor three-level paging system, this macro is equivalent to pud_page() applied to the folded Page Upper Directory entry. |
| pud_offset(pgd, addr) | Receives as parameters a pointer pgd to a Page Global Directory entry and a linear address addr. The macro yields the linear address of the entry in a Page Upper Directory that corresponds to addr. In a two- or three-level paging system, this macro yields pgd, the address of a Page Global Directory entry. |
| pud_page(pud) | Yields the linear address of the Page Middle Directory referred to by the Page Upper Directory entry pud. In a two-level paging system, this macro is equivalent to pmd_page() applied to the folded Page Middle Directory entry. |
| pmd_index(addr) | Yields the index (relative position) of the entry in the Page Middle Directory that maps the linear address addr. |
| pmd_offset(pud, addr) | Receives as parameters a pointer pud to a Page Upper Directory entry and a linear address addr. The macro yields the address of the entry in a Page Middle Directory that corresponds to addr. In a two-level paging system, it yields pud, the address of a Page Global Directory entry. |
| pmd_page(pmd) | Yields the page descriptor address of the Page Table referred to by the Page Middle Directory entry pmd. In a two-level paging system, pmd is actually an entry of a Page Global Directory. |
| mk_pte(p,prot) | Receives as parameters the address of a page descriptor p and a group of access rights prot, and builds the corresponding Page Table entry. |
| pte_index(addr) | Yields the index (relative position) of the entry in the Page Table that maps the linear address addr. |
| pte_offset_kernel(dir, addr) | Yields the linear address of the Page Table that corresponds to the linear address addr mapped by the Page Middle Directory dir. Used only on the master kernel page tables (see the later section “Kernel Page Tables”). |
| pte_offset_map(dir, addr) | Receives as parameters a pointer dir to a Page Middle Directory entry and a linear address addr; it yields the linear address of the entry in the Page Table that corresponds to the linear address addr. If the Page Table is kept in high memory, the kernel establishes a temporary kernel mapping (see the section “Kernel Mappings of High-Memory Page Frames” in Chapter 8), to be released by means of pte_unmap. The macros pte_offset_map_nested and pte_unmap_nested are identical, but they use a different temporary kernel mapping. |
| pte_page(x) | Returns the page descriptor address of the page referenced by the Page Table entry x. |
| pte_to_pgoff(pte) | Extracts from the content pte of a Page Table entry the file offset corresponding to a page belonging to a non-linear file memory mapping (see the section “Non-Linear Memory Mappings” in Chapter 16). |
| pgoff_to_pte(offset) | Sets up the content of a Page Table entry for a page belonging to a non-linear file memory mapping. |

The last group of functions of this long list was introduced to simplify the creation and deletion of page table entries.

When two-level paging is used, creating or deleting a Page Middle Directory entry is
trivial. As we explained earlier in this section, the Page Middle Directory contains a
single entry that points to the subordinate Page Table. Thus, the Page Middle Directory
entry is the entry within the Page Global Directory, too. When dealing with Page
Tables, however, creating an entry may be more complex, because the Page Table
that is supposed to contain it might not exist. In such cases, it is necessary to allocate
a new page frame, fill it with zeros, and add the entry.

If PAE is enabled, the kernel uses three-level paging. When the kernel creates a new
Page Global Directory, it also allocates the four corresponding Page Middle Directories;
these are freed only when the parent Page Global Directory is released.

When two or three-level paging is used, the Page Upper Directory entry is always
mapped as a single entry within the Page Global Directory.

As usual, the description of the functions listed in Table 2-8 refers to the 80×86
architecture.

Table 2-8. Page allocation functions

| 函数名称      | 描述        |
| ------------- | ----------- |
| pgd_alloc(mm) | Allocates a new Page Global Directory; if PAE is enabled, it also allocates the three children Page Middle Directories that map the User Mode linear addresses. The argument mm (the address of a memory descriptor) is ignored  on the 80x86 architecture. |
| pgd_free( pgd) | Releases the Page Global Directory at address pgd; if PAE is enabled, it also releases the three Page Middle Directories that map the User Mode linear addresses. |
| pud_alloc(mm, pgd, addr) | In a two- or three-level paging system, this function does nothing: it simply returns the linear address of the Page Global Directory entry pgd. |
| pud_free(x) | In a two- or three-level paging system, this macro does nothing. |
| pmd_alloc(mm, pud, addr) | Defined so generic three-level paging systems can allocate a new Page Middle Directory for the linear address addr. If PAE is not enabled, the function simply returns the input parameter pud—that is, the address of the entry in the Page Global Directory. If PAE is enabled, the function returns the linear address of the Page Middle Directory entry that maps the linear address addr. The argument cw is ignored. |
| pmd_free(x ) | Does nothing, because Page Middle Directories are allocated and deallocated together with their parent Page Global Directory. |
| pte_alloc_map(mm, pmd, addr) | Receives as parameters the address of a Page Middle Directory entry pmd and a linear address addr, and returns the address of the Page Table entry corresponding to addr. If the Page Middle Directory entry is null, the function allocates a new Page Table by invoking pte_alloc_one(). If a new Page Table is allocated, the entry corresponding to addr is initialized and the User/Supervisor flag is set. If the Page Table is kept in high memory, the kernel establishes a temporary kernel mapping (see the section “Kernel Mappings of High-Memory Page Frames” in Chapter 8), to be released by pte_unmap. |
| pte_alloc_kernel(mm, pmd, addr) | If the Page Middle Directory entry pmd associated with the address addr is null, the function allocates a new Page Table. It then returns the linear address of the Page Table entry associated with addr. Used only for master kernel page tables (see the later section “Kernel Page Tables”). |
| pte_free( pte) | Releases the Page Table associated with the pte page descriptor pointer. |
| pte_free_kernel(pte) | Equivalent to pte_free(), but used for master kernel page tables. |
| clear_page_range(mmu,start,end) | Clears the contents of the page tables of a process from linear address start to end by iteratively releasing its Page Tables and clearing the Page Middle Directory entries. |

<h3 id="2.5.3">2.5.3 物理内存布局</h3>

在初始化阶段，内核必须构建一个物理地址映射，指定哪些物理地址范围是内核可用的，哪些不可用（因为它们映射硬件设备的I/O共享内存，或因为相应的页帧包含BIOS数据）。

内核将下列页帧视作保留：

* 可用物理地址范围之外的页帧
* 包含内核代码和初始化数据的页帧

在保留页帧中的页永远不能动态分配或交换到硬盘中。

一般来说，Linux内核一般从内存的地址0x00100000处开始，也就是说，保留了1MB的空间。具体需要的页帧数量取决于内核大小。通常，内核不大于3M。

为什么内核保留1MB的空间呢？PC架构一些特性需要被考虑。比如：

* 页帧0是BIOS用来存储开机自检（Power-On Self-Test (POST)）时检测到的系统硬件配置的。更重要的是，很多笔记本电脑即使在初始化完成后，也会将数据写入这个页帧。

* 物理地址0x000a0000到0x000fffff保留给BIOS服务程序的，用来映射ISA显卡的内部内存。这个区域是所有IBM兼容的PC中一个众所周知的保留区域，640K到1M地址范围：物理地址存在，但是保留，操作系统不能使用相关的页帧。

* 第一个1M内存中的其它页帧，可能被其它计算机架构保留。比如，IBM的ThinkPad就将0xa0页帧映射到0x9f处。

在早期的系统引导（boot）中，内核查询BIOS并了解物理内存的大小。在最近的计算机中，内核调用BIOS处理程序构建一个物理地址范围和它们相应内存类型的列表。

然后，内核执行`machine_specific_memory_setup()`函数，构建物理地址映射（参见表2-9）。当然了，如果BIOS列表可用，内核将以这个列表为基础构建这个映射表；否则，内核使用默认设置构建映射表：从0x9f（LOWMEMSIZE()）到0x100（HIGH_MEMORY）被标记为保留。

表2-9. BIOS提供的物理地址映射的示例

| Start      | End        | Type   |
| ---------- | ---------- | ------ |
| 0x00000000 | 0x0009FFFF | Usable |
| 0x000F0000 | 0x000FFFFF | Reserved |
| 0x00100000 | 0x07FEFFFF | Usable |
| 0x07FF0000 | 0x07FF2FFF | ACPI data |
| 0x07FF3000 | 0x07FFFFFF | ACPI NVS |
| 0xFFFF0000 | 0xFFFFFFFF | Reserved |

具有128M内存的计算机，典型的内存布局如表2-9所示。具有128 MB RAM的计算机的典型配置如表2-9所示。物理地址范围从0x07ff0000到0x07ff2fff，它存储有关BIOS在POST阶段写入的系统硬件设备的信息;在初始化阶段，内核将这些信息复制到合适的内核数据结构中，然后认为这些页帧是可用的。相反，从0x07ff3000到0x07ffffff的物理地址范围映射到硬件设备的ROM芯片。从0xffff0000开始的物理地址范围被标记为保留的，因为它被硬件映射到BIOS的ROM芯片(参见附录A)。在该表中，由于BIOS没有提供0x000a0000到0x000effff的信息，为了安全起见，Linux假定这些范围是不可用的。

内核可能不会看到BIOS报告的所有物理内存:例如，如果内核没有使用PAE支持进行编译，那么它只能处理4gb RAM，即使实际上有更多的物理内存可用。setup_memory()函数在machine_specific_memory_setup()之后立即调用:它分析物理内存区域表，并初始化几个描述内核物理内存布局的变量。这些变量如表2-10所示。

表2-10. 描述内核物理内存布局的变量

| 变量名称      | 描述        |
| ------------- | ----------- |
| num_physpages     | 最高可用页帧编号 |
| totalram_pages    | 可用页帧总数 |
| min_low_pfn       | 内存中内核镜像之后第一个可用的页帧编号 |
| max_pfn           | 最后一个可用页帧编号 |
| max_low_pfn       | 内核直接映射的最后一页帧的页帧编号(low memory) |
| totalhigh_pages   | 内核没有映射的页帧总数 (high memory) |
| highstart_pfn     | 内核没有映射的第一个页帧编号 |
| highend_pfn       | 内核没有映射的最后一个页帧编号 |

因为RAM的前1M空间使用不连续，所以，Linux跳过这一段内存空间。Linux使用硬件架构没有保留的内存空间存储动态分配的页。

图2-13展示了Linux如何填充前3M的RAM空间。假设内核小于3M。

内核代码从符号`_text`-对应物理地址0x00100000-开始，到符号`_etext`处结束。内核数据分成2部分：初始化数据段和未初始化数据段。初始化数据段从`_etext`之后开始，在`_edata`处结束。未初始化数据段紧随其后，在`_end`处结束。

上面提到的这些符号在Linux源代码中没有定义，是在编译时产生的。

<img id="Figure_2-13" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_13.PNG">

图2-13 Linux内核前768个页帧

<h3 id="2.5.4">2.5.4 进程页表</h3>

进程的线性地址空间可以分为2部分：

* 线性地址空间0x00000000到0xbfffffff可以被用户空间或内核空间寻址。

* 线性地址空间0xc0000000到0xffffffff只被内核空间寻址。

运行在用户模式的进程发出的线性地址小于0xc0000000，运行在内核模式的进程，执行内核代码，发出的线性地址大于0xc0000000。但是，有些情况下，内核可以访问用户模式的线性地址空间，以检索或者存储数据。

Linux源代码中使用宏PAGE_OFFSET代表值0xc0000000，这是内核所在进程的线性地址空间的偏移量。在本书中，往往直接使用这个值0xc0000000。

页全局目录中的第一项中的内容映射的小于0xc0000000的线性地址，当然了，这跟具体的进程有关（PAE禁止时，前768项；PAE使能时，前3项）。也就是，运行在用户模式的进程，其全局页表的内容映射小于0xc0000000的地址空间。与此相反，全局页表中剩余的项，对于所有的进程都是一样的，等于相应的主内核页全局目录中的项（参考下一节）。

<h3 id="2.5.5">2.5.5 内核页表</h3>

The kernel maintains a set of page tables for its own use, rooted at a so-called master kernel Page Global Directory. After system initialization, this set of page tables is never directly used by any process or kernel thread; rather, the highest entries of the master kernel Page Global Directory are the reference model for the corresponding entries of the Page Global Directories of every regular process in the system.

我们将在[第8章 非连续内存区域的线性地址 ](#8)一节中解释内核如何确保将对主内核页全局目录的更改传播到页全局目录，这些页全局目录实际上由进程使用。

我们现在描述内核如何初始化它自己的页表。这分为2步。事实上，内核镜像刚刚加载到内存后，CPU仍然运行在实时模式，此时，分页单元还没使能。

第一步，内核创建有限的地址空间，存储内核代码和数据段，初始页表和存储动态数据结构的128K空间。这个最小的空间正好在RAM中安装内核代码并初始化它的核心数据结构。

第二步，内核充分利用已有的RAM，正确地建立页表。下面让我们详细讨论这个建立的过程。

<h4 id="2.5.5.1">2.5.5.1 临时内核页表</h4>

临时页全局目录在内核编译期间静态初始化，而临时页表在startup_32()汇编函数中初始化，该函数位于 *arch/i386/kernel/head.S*文件中。我们不再提及页上层目录和页中间目录，因为它们等同于页全局目录。在这一阶段，PAE支持还没有使能。

临时页全局目录存储在变量`swapper_pg_dir`中。而临时页表存储在pg0页帧开始的物理地址中，其位于内核未初始化数据段结束处之后（[图2-13](Figure_2-13)中的符号`_end`处）。为了简单，我们假设内核的各个段，临时页表和128KB内存区域，都在RAM的前8M空间之内。为了映射这8M的RAM，需要2个页表。

第一步的分页目的是允许在实时模式和保护模式下，能够简单地对这8M空间进行寻址。因此，内核必须创建一个映射，将线性地址0x00000000~0x007fffff和线性地址0xc0000000~0xc07fffff都映射到物理地址0x00000000~0x007fffff上。换句话说，内核在初始化的第一步，通过使用与物理地址相同的线性地址或者从0xc0000000开始的8M线性空间访问RAM的前8M空间。

内核通过填充`swapper_pg_dir`中的0、1、0x300（768）和0x301（769）这几项，其余位置填充0，来创建想要的映射。后面2项的范围正好是0xc0000000~0xc07fffff。初始化如下：

* 0和0x300设置为pg0的物理地址，1和0x301设置成pg1的地址
* 4项的Present、Read/Write和User/Supervisor标志被设置
* 4项的Accessed、Dirty、PCD、PWD和PS标志被清除

startup_32()汇编函数也会使能分页单元。通过把`swapper_pg_dir`的物理地址写入cr3寄存器并设置cr0寄存器的PG标志实现。代码片段如下：

    movl $swapper_pg_dir-0xc0000000, %eax
    movl %eax, %cr3                         /* 设置页表指针 */
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0                         /* 设置PG标志位 */

<h4 id="2.5.5.2">2.5.5.2 内核页表（RAM < 896M）</h4>

由内核页表提供的映射必须能够把从`0xc0000000`开始的线性地址转换成从0开始的物理地址。

`__pa`能够把一个从PAGE_OFFSET(0xC0000000)开始的线性地址转换成物理地址，而`__va`实现逆操作。都是位于内核空间的转换。

主内核页全局目录仍然在`swapper_pg_dir`变量中，它由`paging_init()`函数进行初始化：

1. 调用pagetable_init()建立页表项；
2. 写swapper_pg_dir的物理地址到cr3寄存器中；
3. 如果内核编译选择支持PAE，设置cr4寄存器中的PAE标志；
4. 调用`__flush_tlb_all()`函数使所有TLB无效。

pagetable_init()函数所做的操作依赖于RAM的数量和CPU模型。让我们从最简单的一种情况说起，假设我们的计算机内存小于896MB，32位的物理地址寻址足够了，没有必要激活PAE功能。

swapper_pg_dir页全局目录重新初始化，代码片段如下：

    pgd = swapper_pg_dir + pgd_index(PAGE_OFFSET); /* 768 */
    phys_addr = 0x00000000;
    while (phys_addr < (max_low_pfn * PAGE_SIZE)) {
        pmd = one_md_table_init(pgd); /* returns pgd itself */
        set_pmd(pmd, __pmd(phys_addr | pgprot_val(__pgprot(0x1e3))));
        /* 0x1e3 == Present, Accessed, Dirty, Read/Write, Page Size, Global */
        phys_addr += PTRS_PER_PTE * PAGE_SIZE; /* 0x400000 */
        ++pgd;
    }

We assume that the CPU is a recent 80×86 microprocessor supporting 4 MB pages and “global” TLB entries. Notice that the User/Supervisor flags in all Page Global Directory entries referencing linear addresses above 0xc0000000 are cleared, thus denying processes in User Mode access to the kernel address space. Notice also that the Page Size flag is set so that the kernel can address the RAM by making use of large pages (see the section “Extended Paging” earlier in this chapter).

The identity mapping of the first megabytes of physical memory (8 MB in our example) built by the startup_32() function is required to complete the initialization phase of the kernel. When this mapping is no longer necessary, the kernel clears the corresponding page table entries by invoking the zap_low_mappings() function.

Actually, this description does not state the whole truth. As we’ll see in the later section
“Fix-Mapped Linear Addresses,” the kernel also adjusts the entries of Page
Tables corresponding to the “fix-mapped linear addresses.”

<h4 id="2.5.5.3">2.5.5.3 内核页表（896M < RAM < 4096M）</h4>

In this case, the RAM cannot be mapped entirely into the kernel linear address space.
The best Linux can do during the initialization phase is to map a RAM window of
size 896 MB into the kernel linear address space. If a program needs to address other
parts of the existing RAM, some other linear address interval must be mapped to the
required RAM. This implies changing the value of some page table entries. We’ll
discuss how this kind of dynamic remapping is done in Chapter 8.

To initialize the Page Global Directory, the kernel uses the same code as in the previous
case.


<h4 id="2.5.5.4">2.5.5.4 内核页表（RAM > 4096M）</h4>

Let’s now consider kernel Page Table initialization for computers with more than
4 GB; more precisely, we deal with cases in which the following happens:

* The CPU model supports Physical Address Extension (PAE).
* The amount of RAM is larger than 4 GB.
* The kernel is compiled with PAE support.

Although PAE handles 36-bit physical addresses, linear addresses are still 32-bit
addresses. As in the previous case, Linux maps a 896-MB RAM window into the kernel
linear address space; the remaining RAM is left unmapped and handled by
dynamic remapping, as described in Chapter 8. The main difference with the previous case is that a three-level paging model is used, so the Page Global Directory is
initialized by a cycle equivalent to the following:

    pgd_idx = pgd_index(PAGE_OFFSET); /* 3 */
    for (i=0; i<pgd_idx; i++)
        set_pgd(swapper_pg_dir + i, __pgd(__pa(empty_zero_page) + 0x001));
        /* 0x001 == Present */
    pgd = swapper_pg_dir + pgd_idx;
    phys_addr = 0x00000000;
    for (; i<PTRS_PER_PGD; ++i, ++pgd) {
        pmd = (pmd_t *) alloc_bootmem_low_pages(PAGE_SIZE);
        set_pgd(pgd, __pgd(__pa(pmd) | 0x001)); /* 0x001 == Present */
        if (phys_addr < max_low_pfn * PAGE_SIZE)
            for (j=0; j < PTRS_PER_PMD /* 512 */
                    && phys_addr < max_low_pfn*PAGE_SIZE; ++j) {
                set_pmd(pmd, __pmd(phys_addr | pgprot_val(__pgprot(0x1e3))));
                /* 0x1e3 == Present, Accessed, Dirty, Read/Write,  Page Size, Global */
                phys_addr += PTRS_PER_PTE * PAGE_SIZE; /* 0x200000 */
            }
    }
    swapper_pg_dir[0] = swapper_pg_dir[pgd_idx];

The kernel initializes the first three entries in the Page Global Directory corresponding
to the user linear address space with the address of an empty page (empty_zero_
page). The fourth entry is initialized with the address of a Page Middle Directory
(pmd) allocated by invoking alloc_bootmem_low_pages(). The first 448 entries in the
Page Middle Directory (there are 512 entries, but the last 64 are reserved for noncontiguous
memory allocation; see the section “Noncontiguous Memory Area Management”
in Chapter 8) are filled with the physical address of the first 896 MB of RAM.

Notice that all CPU models that support PAE also support large 2-MB pages and global
pages. As in the previous cases, whenever possible, Linux uses large pages to
reduce the number of Page Tables.

The fourth Page Global Directory entry is then copied into the first entry, so as to
mirror the mapping of the low physical memory in the first 896 MB of the linear
address space. This mapping is required in order to complete the initialization of
SMPsystems: when it is no longer necessary, the kernel clears the corresponding
page table entries by invoking the zap_low_mappings() function, as in the previous
cases.

<h3 id="2.5.6">2.5.6 固定映射的线性地址</h3>

We saw that the initial part of the fourth gigabyte of kernel linear addresses maps the
physical memory of the system. However, at least 128 MB of linear addresses are
always left available because the kernel uses them to implement noncontiguous
memory allocation and fix-mapped linear addresses.

Noncontiguous memory allocation is just a special way to dynamically allocate and
release pages of memory, and is described in the section “Linear Addresses of Noncontiguous
Memory Areas” in Chapter 8. In this section, we focus on fix-mapped linear
addresses.

Basically, a fix-mapped linear address is a constant linear address like 0xffffc000
whose corresponding physical address does not have to be the linear address minus
0xc000000, but rather a physical address set in an arbitrary way. Thus, each fixmapped
linear address maps one page frame of the physical memory. As we’ll see in
later chapters, the kernel uses fix-mapped linear addresses instead of pointer variables
that never change their value.

Fix-mapped linear addresses are conceptually similar to the linear addresses that
map the first 896 MB of RAM. However, a fix-mapped linear address can map any
physical address, while the mapping established by the linear addresses in the initial
portion of the fourth gigabyte is linear (linear address X maps physical address
X –PAGE_OFFSET).

With respect to variable pointers, fix-mapped linear addresses are more efficient. In
fact, dereferencing a variable pointer requires one memory access more than dereferencing
an immediate constant address. Moreover, checking the value of a variable
pointer before dereferencing it is a good programming practice; conversely, the check
is never required for a constant linear address.

Each fix-mapped linear address is represented by a small integer index defined in the
enum fixed_addresses data structure:

    enum fixed_addresses {
        FIX_HOLE,
        FIX_VSYSCALL,
        FIX_APIC_BASE,
        FIX_IO_APIC_BASE_0,
        [...]
        __end_of_fixed_addresses
    };

Fix-mapped linear addresses are placed at the end of the fourth gigabyte of linear
addresses. The fix_to_virt() function computes the constant linear address starting
from the index:

    inline unsigned long fix_to_virt(const unsigned int idx)
    {
        if (idx >= __end_of_fixed_addresses)
        __this_fixmap_does_not_exist();
        return (0xfffff000UL - (idx << PAGE_SHIFT));
    }


Let’s assume that some kernel function invokes fix_to_virt(FIX_IO_APIC_BASE_0).
Because the function is declared as “inline,” the C compiler does not generate a call
to fix_to_virt(), but inserts its code in the calling function. Moreover, the check on
the index value is never performed at runtime. In fact, FIX_IO_APIC_BASE_0 is a constant
equal to 3, so the compiler can cut away the if statement because its condition
is false at compile time. Conversely, if the condition is true or the argument of fix_
to_virt() is not a constant, the compiler issues an error during the linking phase
because the symbol __this_fixmap_does_not_exist is not defined anywhere. Eventually,
the compiler computes 0xfffff000-(3<<PAGE_SHIFT) and replaces the fix_to_
virt() function call with the constant linear address 0xffffc000.

To associate a physical address with a fix-mapped linear address, the kernel uses the
set_fixmap(idx,phys) and set_fixmap_nocache(idx,phys) macros. Both of them initialize
the Page Table entry corresponding to the fix_to_virt(idx) linear address
with the physical address phys; however, the second function also sets the PCD flag of
the Page Table entry, thus disabling the hardware cache when accessing the data in
the page frame (see the section “Hardware Cache” earlier in this chapter). Conversely,
clear_fixmap(idx) removes the linking between a fix-mapped linear address
idx and the physical address.

<h3 id="2.5.7">2.5.7 处理Cache和TLB</h3>

The last topic of memory addressing deals with how the kernel makes an optimal use
of the hardware caches. Hardware caches and Translation Lookaside Buffers play a
crucial role in boosting the performance of modern computer architectures. Several
techniques are used by kernel developers to reduce the number of cache and TLB
misses.

<h4 id="2.5.7.1">2.5.7.1 处理Cache</h4>

As mentioned earlier in this chapter, hardware caches are addressed by cache lines.
The L1_CACHE_BYTES macro yields the size of a cache line in bytes. On Intel models
earlier than the Pentium 4, the macro yields the value 32; on a Pentium 4, it yields
the value 128.

To optimize the cache hit rate, the kernel considers the architecture in making the
following decisions.

* The most frequently used fields of a data structure are placed at the low offset within the data structure, so they can be cached in the same line.

* When allocating a large set of data structures, the kernel tries to store each of them in memory in such a way that all cache lines are used uniformly.

Cache synchronization is performed automatically by the 80×86 microprocessors, thus the Linux kernel for this kind of processor does not perform any hardware cache flushing. The kernel does provide, however, cache flushing interfaces for processors that do not synchronize caches.

<h4 id="2.5.7.2">2.5.7.2 处理TLB</h4>

Processors cannot synchronize their own TLB cache automatically because it is the
kernel, and not the hardware, that decides when a mapping between a linear and a
physical address is no longer valid.

Linux 2.6 offers several TLB flush methods that should be applied appropriately,
depending on the type of page table change (see Table 2-11).

Table 2-11. Architecture-independent TLB-invalidating methods

| Method name | Description | Typically used when |
| ----------- | ----------- | ------------------- |
| flush_tlb_all | Flushes all TLB entries (including those that refer to global pages, that is, pages whose Global flag is set) | Changing the kernel page table entries |
| flush_tlb_kernel_range | Flushes all TLB entries in a given range of linear addresses (including those that refer to global pages) | Changing a range of kernel page table entries |
| flush_tlb | Flushes all TLB entries of the non-global pages owned by the current process | Performing a process switch |
| flush_tlb_mm | Flushes all TLB entries of the non-global pages owned by a given process | Forking a new process |
| flush_tlb_range | Flushes the TLB entries corresponding to a linear address interval of a given process | Releasing a linear address interval of a process |
| flush_tlb_pgtables | Flushes the TLB entries of a given contiguous subset of page tables of a given process | Releasing some page tables of a process |
| flush_tlb_page | Flushes the TLB of a single Page Table entry of a given process | Processing a Page Fault |

Despite the rich set of TLB methods offered by the generic Linux kernel, every microprocessor
usually offers a far more restricted set of TLB-invalidating assembly language
instructions. In this respect, one of the more flexible hardware platforms is
Sun’s UltraSPARC. In contrast, Intel microprocessors offers only two TLB-invalidating
techniques:

* All Pentium models automatically flush the TLB entries relative to non-global pages when a value is loaded into the cr3 register.

* In Pentium Pro and later models, the invlpg assembly language instruction invalidates a single TLB entry mapping a given linear address.

Table 2-12 lists the Linux macros that exploit such hardware techniques; these macros
are the basic ingredients to implement the architecture-independent methods
listed in Table 2-11.

Table 2-12. TLB-invalidating macros for the Intel Pentium Pro and later processors

| Method name | Description | Typically used when |
| ----------- | ----------- | ------------------- |
| __flush_tlb() | Rewrites cr3 register back into itself | flush_tlb, flush_tlb_mm, flush_tlb_range |
| __flush_tlb_global() | Disables global pages by clearing the PGE flag of cr4, rewrites cr3 register back into itself, and sets again the PGE flag | flush_tlb_all, flush_tlb_kernel_range |
| __flush_tlb_single(addr) | Executes invlpg assembly language instruction with parameter addr | flush_tlb_page |

Notice that the flush_tlb_pgtables method is missing from Table 2-12: in the 80×86
architecture nothing has to be done when a page table is unlinked from its parent
table, thus the function implementing this method is empty.

The architecture-independent TLB-invalidating methods are extended quite simply
to multiprocessor systems. The function running on a CPU sends an Interprocessor
Interrupt (see “Interprocessor Interrupt Handling” in Chapter 4) to the other CPUs
that forces them to execute the proper TLB-invalidating function.

As a general rule, any process switch implies changing the set of active page tables.
Local TLB entries relative to the old page tables must be flushed; this is done automatically
when the kernel writes the address of the new Page Global Directory into
the cr3 control register. The kernel succeeds, however, in avoiding TLB flushes in the
following cases:


* When performing a process switch between two regular processes that use the
same set of page tables (see the section “The schedule() Function” in
Chapter 7).

* When performing a process switch between a regular process and a kernel
thread. In fact, we’ll see in the section “Memory Descriptor of Kernel Threads”
in Chapter 9, that kernel threads do not have their own set of page tables; rather,
they use the set of page tables owned by the regular process that was scheduled
last for execution on the CPU.

Besides process switches, there are other cases in which the kernel needs to flush
some entries in a TLB. For instance, when the kernel assigns a page frame to a User
Mode process and stores its physical address into a Page Table entry, it must flush
any local TLB entry that refers to the corresponding linear address. On multiprocessor
systems, the kernel also must flush the same TLB entry on the CPUs that are
using the same set of page tables, if any.

To avoid useless TLB flushing in multiprocessor systems, the kernel uses a technique
called lazy TLBmode . The basic idea is the following: if several CPUs are using
the same page tables and a TLB entry must be flushed on all of them, then TLB
flushing may, in some cases, be delayed on CPUs running kernel threads.
In fact, each kernel thread does not have its own set of page tables; rather, it makes
use of the set of page tables belonging to a regular process. However, there is no need
to invalidate a TLB entry that refers to a User Mode linear address, because no kernel
thread accesses the User Mode address space.*

When some CPUs start running a kernel thread, the kernel sets it into lazy TLB
mode. When requests are issued to clear some TLB entries, each CPU in lazy TLB
mode does not flush the corresponding entries; however, the CPU remembers that its
current process is running on a set of page tables whose TLB entries for the User
Mode addresses are invalid. As soon as the CPU in lazy TLB mode switches to a regular
process with a different set of page tables, the hardware automatically flushes
the TLB entries, and the kernel sets the CPU back in non-lazy TLB mode. However,
if a CPU in lazy TLB mode switches to a regular process that owns the same set of
page tables used by the previously running kernel thread, then any deferred TLB
invalidation must be effectively applied by the kernel. This “lazy” invalidation is
effectively achieved by flushing all non-global TLB entries of the CPU.

Some extra data structures are needed to implement the lazy TLB mode. The cpu_
tlbstate variable is a static array of NR_CPUS structures (the default value for this
macro is 32; it denotes the maximum number of CPUs in the system) consisting of
an active_mm field pointing to the memory descriptor of the current process (see
Chapter 9) and a state flag that can assume only two values: TLBSTATE_OK (non-lazy
TLB mode) or TLBSTATE_LAZY (lazy TLB mode). Furthermore, each memory descriptor
includes a cpu_vm_mask field that stores the indices of the CPUs that should
receive Interprocessor Interrupts related to TLB flushing. This field is meaningful
only when the memory descriptor belongs to a process currently in execution.

When a CPU starts executing a kernel thread, the kernel sets the state field of its
cpu_tlbstate element to TLBSTATE_LAZY; moreover, the cpu_vm_mask field of the active
memory descriptor stores the indices of all CPUs in the system, including the one
that is entering in lazy TLB mode. When another CPU wants to invalidate the TLB
entries of all CPUs relative to a given set of page tables, it delivers an Interprocessor
Interrupt to all CPUs whose indices are included in the cpu_vm_mask field of the corresponding
memory descriptor.

When a CPU receives an Interprocessor Interrupt related to TLB flushing and verifies
that it affects the set of page tables of its current process, it checks whether the
state field of its cpu_tlbstate element is equal to TLBSTATE_LAZY. In this case, the kernel
refuses to invalidate the TLB entries and removes the CPU index from the cpu_
vm_mask field of the memory descriptor. This has two consequences:

* As long as the CPU remains in lazy TLB mode, it will not receive other Interprocessor Interrupts related to TLB flushing.

* If the CPU switches to another process that is using the same set of page tables as the kernel thread that is being replaced, the kernel invokes __flush_tlb() to invalidate all non-global TLB entries of the CPU.

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="3">3 进程</h1>

The concept of a process is fundamental to any multiprogramming operating system. A process is usually defined as an instance of a program in execution; thus, if 16 users are running vi at once, there are 16 separate processes (although they can share the same executable code). Processes are often called tasks or threads in the Linux source code.

In this chapter, we discuss static properties of processes and then describe how process switching is performed by the kernel. The last two sections describe how processes can be created and destroyed. We also describe how Linux supports multithreaded applications—as mentioned in Chapter 1, it relies on so-called lightweight processes (LWP).

<h2 id="1">3.1 进程、轻量级进程和线程</h2>

The term “process” is often used with several different meanings. In this book, we
stick to the usual OS textbook definition: a process is an instance of a program in
execution. You might think of it as the collection of data structures that fully
describes how far the execution of the program has progressed.

Processes are like human beings: they are generated, they have a more or less significant
life, they optionally generate one or more child processes, and eventually they
die. A small difference is that sex is not really common among processes—each process
has just one parent.

From the kernel’s point of view, the purpose of a process is to act as an entity to
which system resources (CPU time, memory, etc.) are allocated.

When a process is created, it is almost identical to its parent. It receives a (logical)
copy of the parent’s address space and executes the same code as the parent, beginning
at the next instruction following the process creation system call. Although the
parent and child may share the pages containing the program code (text), they have separate copies of the data (stack and heap), so that changes by the child to a memory
location are invisible to the parent (and vice versa).

While earlier Unix kernels employed this simple model, modern Unix systems do
not. They support multithreaded applications—user programs having many relatively
independent execution flows sharing a large portion of the application data
structures. In such systems, a process is composed of several user threads (or simply
threads), each of which represents an execution flow of the process. Nowadays, most
multithreaded applications are written using standard sets of library functions called
pthread (POSIX thread) libraries.

Older versions of the Linux kernel offered no support for multithreaded applications.
From the kernel point of view, a multithreaded application was just a normal process.
The multiple execution flows of a multithreaded application were created, handled,
and scheduled entirely in User Mode, usually by means of a POSIX-compliant
pthread library.

However, such an implementation of multithreaded applications is not very satisfactory.
For instance, suppose a chess program uses two threads: one of them controls
the graphical chessboard, waiting for the moves of the human player and showing
the moves of the computer, while the other thread ponders the next move of the
game. While the first thread waits for the human move, the second thread should
run continuously, thus exploiting the thinking time of the human player. However, if
the chess program is just a single process, the first thread cannot simply issue a
blocking system call waiting for a user action; otherwise, the second thread is
blocked as well. Instead, the first thread must employ sophisticated nonblocking
techniques to ensure that the process remains runnable.

Linux uses lightweight processes to offer better support for multithreaded applications.
Basically, two lightweight processes may share some resources, like the address
space, the open files, and so on. Whenever one of them modifies a shared resource,
the other immediately sees the change. Of course, the two processes must synchronize
themselves when accessing the shared resource.

A straightforward way to implement multithreaded applications is to associate a
lightweight process with each thread. In this way, the threads can access the same set
of application data structures by simply sharing the same memory address space, the
same set of open files, and so on; at the same time, each thread can be scheduled
independently by the kernel so that one may sleep while another remains runnable.
Examples of POSIX-compliant pthread libraries that use Linux’s lightweight processes
are LinuxThreads, Native POSIX Thread Library (NPTL), and IBM’s Next
Generation Posix Threading Package (NGPT).

POSIX-compliant multithreaded applications are best handled by kernels that support
“thread groups.” In Linux a thread group is basically a set of lightweight processes
that implement a multithreaded application and act as a whole with regards to

some system calls such as getpid(), kill(), and _exit(). We are going to describe
them at length later in this chapter.

<h2 id="1">3.2 进程描述符</h2>
<h2 id="1">3.3 进程切换</h2>
<h2 id="1">3.4 创建进程</h2>
<h2 id="1">3.5 销毁进程</h2>
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
<h1 id="2">4 中断和异常</h1>
<h2 id="1">4.1 中断信号的角色</h2>
<h2 id="1">4.2 中断和异常</h2>
<h2 id="1">4.3 嵌套中断和异常</h2>
<h2 id="1">4.4 初始化中断描述符表</h2>
<h2 id="1">4.5 异常处理</h2>
<h2 id="1">4.6 中断处理</h2>
<h2 id="1">4.7 软件中断和Tasklet</h2>
<h2 id="1">4.8 工作队列</h2>
<h2 id="1">4.9 中断和异常的返回</h2>
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
<h1 id="2">5 内核同步</h1>
<h2 id="1">5.1 内核服务如何请求</h2>
<h2 id="1">5.2 同步原语</h2>
<h2 id="1">5.3 内核数据结构的同步访问</h2>
<h2 id="1">5.4 防止竞态条件的示例</h2>
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
<h1 id="2">6 内核计时</h1>
<h2 id="1">6.1 时钟和定时器电路</h2>
<h2 id="1">6.2 内核计时架构</h2>
<h2 id="1">6.3 更新时间和日期</h2>
<h2 id="1">6.4 更新系统数据</h2>
<h2 id="1">6.5 软件定时器和延时函数</h2>
<h2 id="1">6.6 与计时相关的系统调用</h2>
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
<h1 id="2">7 进程调度</h1>
<h2 id="1">7.1 调度策略</h2>
<h2 id="1">7.2 调度算法</h2>
<h2 id="1">7.3 调度器使用的数据结构</h2>
<h2 id="1">7.4 调度器使用的函数</h2>
<h2 id="1">7.5 多处理器系统的运行队列平衡</h2>
<h2 id="1">7.6 调度相关的系统调用</h2>
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
<h1 id="2">8 内存管理</h1>
<h2 id="1">8.1 页帧管理</h2>
<h2 id="1">8.2 内存管理</h2>
<h2 id="1">8.3 非连续内存管理</h2>
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
<h1 id="2">9 进程地址空间</h1>
<h2 id="1">9.1 进程地址空间</h2>
<h2 id="1">9.2 内存描述符</h2>
<h2 id="1">9.3 内存区域</h2>
<h2 id="1">9.4 页错误异常处理程序</h2>
<h2 id="1">9.5 创建和删除进程地址空间</h2>

<h2 id="9.6">9.6 管理堆</h2>

Each Unix process owns a specific memory region called the heap, which is used to satisfy the process’s dynamic memory requests. The start_brk and brk fields of the memory descriptor delimit the starting and ending addresses, respectively, of that region.

The following APIs can be used by the process to request and release dynamic memory:


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="10">10 系统调用</h1>

Operating systems offer processes running in user mode a set of interfaces to interact with hardware devices such as the CPU, disks, and printers. Putting extralayer between the application and the hardware has several advantages. First, it makes programming easier by freeing users from studying low-level programming characteristics of hardware devices. Second, it greatly increases system security, because the kernel can check the accuracy of the request at the interface level before attempting to satisfy it. Last but not least, these interfaces make programs more portable, because the can be compiled and executed correctly on every kernel that offers the same set of interfaces.

Unix systems implement most interfaces between User Mode processes and hardware devices by means of system calls issued to the kernel. This chapter examines in detail how Linux implements system calls that User Mode programs issue to the kernel.

<h2 id="10.1">10.1 POSIX API和系统调用</h2>

Let's start by stressing the difference between an application programmer interface(API) and a system call. The former is a function definition that specifies how to obtain a given service, while the latter is an explicit request to the kernel made via a software interrupt.

Unix systems include several libraries of functions that provide APIs to programmers. Some of the APIs defined by the *libc* standard C library refer to *wrapper routines*(routines whose only purpose is to issue a system call). Usually, each system call has a corresponding wrapper routine, which defines the API that application programs should employ.

The converse is not true, by the way - an API does not necessarily correspond to a specific system call. First of all, the API offer its services directly in User Mode.(For something abstact such as math functions, there may be no reason to make system calls.) Second, a single API function could make several system calls. Moreover, several API functions could make the same system call, but wrap extra functionality aound it. For instance, in Linux, the *malloc()*, *calloc()*, and *free()* APIs are implemented in the *libc* library. The code in this library keeps track of the allocation and deallocation requests and uses the *brk*() system call to enlarge or shrink the process heap (see the section "Managing the Heap" in [Chapter 9](#9))

The POSIX standard refers to APIs and not to system calls. A system can be certified as POSIX-compliant if it offers the proper set of APIs to the application programs, no matter how the corresponding functions are implemented. As a matter of fact, several non-Unix systems have been certified as POSIX-compliant, because they offer all traditional Unix services in User Mode libraries.

From the programmer’s point of view, the distinction between an API and a system call is irrelevant — the only things that matter are the function name, the parameter types, and the meaning of the return code. From the kernel designer’s point of view, however, the distinction does matter because system calls belong to the kernel, while User Mode libraries don’t.

Most wrapper routines return an integer value, whose meaning depends on the corresponding system call. A return value of –1 usually indicates that the kernel was unable to satisfy the process request. A failure in the system call handler may be caused by invalid parameters, a lack of available resources, hardware problems, and so on. The specific error code is contained in the *errno* variable, which is defined in the libc library.

Each error code is defined as a macro constant, which yields a corresponding positive integer value. The POSIX standard specifies the macro names of several error codes. In Linux, on 80×86 systems, these macros are defined in the header file *include/asm-i386/errno.h*. To allow portability of C programs among Unix systems, the *include/asm-i386/errno.h* header file is included, in turn, in the standard */usr/include/errno.h* C library header file. Other systems have their own specialized subdirectories of header files.

核心笔记：

1. POSIX API和系统调用的区别：
    
    `POSIX API`可以理解为一些标准C函数库，而系统调用是在其上进行的封装，以满足用户态和内核态切换需要。系统只要是使用符合`POSIX`标准定义的函数实现系统调用，就是与`POSIX`兼容的。

2. 为什么这么设计？不直接使用API？
    
    如果使用API，那么用户态程序可以直接访问硬件资源，这与`进程/内核`模型的设计理念不符。使用系统调用有2个好处：

    * 用户态和内核态隔离开来，访问更加安全
    * 便于用户态程序的移植

3. 对于系统调用，用户可能更关心返回值。
    
    其返回值用一个整数表示，其宏定义位于 *include/arch-specific/errno.h*头文件中。在这儿，`arch-specific`与系统平台的架构相关。用户态的 标准C库的 */usr/include/errno.h*头文件中也应该包含这些错误返回码。

<h2 id="10.2">10.2 系统调用处理程序和服务例程</h2>

When a User Mode process invokes a system call, the CPU switches to Kernel Mode
and starts the execution of a kernel function. As we will see in the next section, in the
80×86 architecture a Linux system call can be invoked in two different ways. The net
result of both methods, however, is a jump to an assembly language function called
the *system call handler*.

Because the kernel implements many different system calls, the User Mode process
must pass a parameter called the *system call number* to identify the required system
call; the *eax* register is used by Linux for this purpose. As we’ll see in the section
“Parameter Passing” later in this chapter, additional parameters are usually passed
when invoking a system call.

All system calls return an integer value. The conventions for these return values are
different from those for wrapper routines. In the kernel, positive or 0 values denote a
successful termination of the system call, while negative values denote an error condition.
In the latter case, the value is the negation of the error code that must be
returned to the application program in the *errno* variable. The *errno* variable is not
set or used by the kernel. Instead, the wrapper routines handle the task of setting this
variable after a return from a system call.

The system call handler, which has a structure similar to that of the other exception
handlers, performs the following operations:

* Saves the contents of most registers in the Kernel Mode stack (this operation is common to all system calls and is coded in assembly language).

* Handles the system call by invoking a corresponding C function called the *system call service routine*.

* Exits from the handler: the registers are loaded with the values saved in the Kernel Mode stack, and the CPU is switched back from Kernel Mode to User Mode (this operation is common to all system calls and is coded in assembly language).

The name of the service routine associated with the *xyz()* system call is usually *sys_xyz()*; there are, however, a few exceptions to this rule.

Figure 10-1 illustrates the relationships between the application program that invokes a system call, the corresponding wrapper routine, the system call handler, and the system call service routine. The arrows denote the execution flow between the functions. The terms “SYSCALL” and “SYSEXIT” are placeholders for the actual assembly language instructions that switch the CPU, respectively, from User Mode to Kernel Mode and from Kernel Mode to User Mode.

<img id="Figure_10-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_10_1.PNG">

图10-1 系统调用的过程

To associate each system call number with its corresponding service routine, the kernel uses a *system call dispatch table*, which is stored in the *sys_call_table* array and has *NR_syscalls* entries (289 in the Linux 2.6.11 kernel). The Nth entry contains the service routine address of the system call having number N.

The *NR_syscalls* macro is just a static limit on the maximum number of implementable system calls; it does not indicate the number of system calls actually implemented. Indeed, each entry of the dispatch table may contain the address of the *sys_ni_syscall()* function, which is the service routine of the “nonimplemented” system calls; it just returns the error code -ENOSYS.



<h2 id="10.3">10.3 进入和退出系统调用</h2>

Native applications* can invoke a system call in two different ways:

* By executing the int $0x80 assembly language instruction; in older versions of the Linux kernel, this was the only way to switch from User Mode to Kernel Mode.

* By executing the sysenter assembly language instruction, introduced in the Intel Pentium II microprocessors; this instruction is now supported by the Linux 2.6 kernel.

Similarly, the kernel can exit from a system call—thus switching the CPU back to User Mode—in two ways:

* By executing the *iret* assembly language instruction.

* By executing the sysexit assembly language instruction, which was introduced in the Intel Pentium II microprocessors together with the sysenter instruction.

However, supporting two different ways to enter the kernel is not as simple as it might look, because:

* The kernel must support both older libraries that only use the int $0x80 instruction and more recent ones that also use the sysenter instruction.

* A standard library that makes use of the sysenter instruction must be able to cope with older kernels that support only the int $0x80 instruction.

* The kernel and the standard library must be able to run both on older processors that do not include the sysenter instruction and on more recent ones that include it.

We will see in the section “Issuing a System Call via the sysenter Instruction” later in this chapter how the Linux kernel solves these compatibility problems.

<h3 id="10.3.1">10.3.1 使用中断$0x80指令发起系统调用</h3>

The “traditional” way to invoke a system call makes use of the *int* assembly language instruction, which was discussed in the section “Hardware Handling of Interrupts and Exceptions” in [Chapter 4](#4).

The vector 128—in hexadecimal, 0x80—is associated with the kernel entry point. The *trap_init()* function, invoked during kernel initialization, sets up the Interrupt Descriptor Table entry corresponding to vector 128 as follows:

    set_system_gate(0x80, &system_call);

The call loads the following values into the gate descriptor fields (see the section “Interrupt, Trap, and System Gates” in [Chapter 4](#4)):

* Segment Selector
* Offset
* Type
* DPL(Descriptor Privilege Level)
    
    Set to 3. This allows processes in User Mode to invoke the exception handler (see the section “Hardware Handling of Interrupts and Exceptions” in Chapter 4).

Therefore, when a User Mode process issues an *int $0x80* instruction, the CPU switches into Kernel Mode and starts executing instructions from the *system_call* address.

<h4 id="10.3.1.1">10.3.1.1 system_call()函数</h4>

The *system_call()* function starts by saving the system call number and all the CPU registers that may be used by the exception handler on the stack—except for eflags,
cs, eip, ss, and esp, which have already been saved automatically by the control unit
(see the section “Hardware Handling of Interrupts and Exceptions” in Chapter 4).
The SAVE_ALL macro, which was already discussed in the section “I/O Interrupt Handling”
in Chapter 4, also loads the Segment Selector of the kernel data segment in ds
and es:

<h4 id="10.3.1.2">10.3.1.2 从system_call退出</h4>

<h3 id="10.3.2">10.3.2 使用sysenter指令发起系统调用</h3>

<h2 id="10.4">10.4 参数传递</h2>

<h2 id="10.5">10.5 内核包装例程</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="2">11 信号量</h1>
<h2 id="1">11.1 信号量的角色</h2>
<h2 id="1">11.2 创建信号量</h2>
<h2 id="1">11.3 传递信号量</h2>
<h2 id="1">11.4 与信号处理相关的系统调用</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="2">12 虚拟文件系统</h1>
<h2 id="1">12.1 虚拟文件系统的角色</h2>
<h2 id="1">12.2 VFS中的数据结构</h2>
<h2 id="1">12.3 文件系统类型</h2>
<h2 id="1">12.4 文件系统处理</h2>
<h2 id="1">12.5 遍历路径</h2>
<h2 id="1">12.6 VFS系统调用的实现</h2>
<h2 id="1">12.7 文件加锁</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="2">13 I/O架构和设备驱动</h1>
<h2 id="1">13.1 I/O架构</h2>
<h2 id="1">13.2 设备驱动模型</h2>
<h2 id="1">13.3 设备文件</h2>
<h2 id="1">13.4 设备驱动</h2>
<h2 id="1">13.5 字符设备驱动</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="14">14 块设备驱动</h1>
<h2 id="14.1">14.1 块设备处理</h2>
<h2 id="14.2">14.2 通用块设备层</h2>
<h2 id="14.3">14.3 I/O调度器</h2>
<h2 id="14.4">14.4 块设备驱动</h2>
<h2 id="14.5">14.5 打开块设备文件</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="15">15 页缓存</h1>
<h2 id="15.1">15.1 页缓存</h2>
<h2 id="15.2">15.2 页缓存中存储block块</h2>
<h2 id="15.3">15.3 更新页到硬盘</h2>
<h2 id="15.4">15.4 sync()、fsync()、和fdatasync()系统调用</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="16">16 访问文件</h1> 
<h2 id="16.1">16.1 读写文件</h2> 
<h2 id="16.2">16.2 内存映射</h2> 
<h2 id="16.3">16.3 直接I/O传送</h2> 
<h2 id="16.4">16.4 异步I/O</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="17">17 页帧回收</h1> 
<h2 id="17.1">17.1 页帧回收算法</h2> 
<h2 id="17.2">17.2 反向映射</h2> 
<h2 id="17.3">17.3 实现PFRA</h2> 
<h2 id="17.4">17.4 交换</h2> 

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="18">18 Ext2和Ext3文件系统</h1> 
<h2 id="18.1">18.1 Ext2的一般特性</h2> 
<h2 id="18.2">18.2 Ext2硬盘数据结构</h2> 
<h2 id="18.3">18.3 Ext2内存数据结构</h2> 
<h2 id="18.4">18.4 创建Ext2文件系统</h2> 
<h2 id="18.5">18.5 Ext2方法</h2>
<h2 id="18.6">18.6 管理Ext2硬盘空间</h2> 
<h2 id="18.7">18.7 Ext3文件系统</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="19">19 进程通信</h1> 
<h2 id="19.1">19.1 管道</h2> 
<h2 id="19.2">19.2 FIFO</h2> 
<h2 id="19.3">19.3 System V IPC</h2> 
<h2 id="19.4">19.4 POSIX消息队列</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="20">20 程序执行</h1> 
<h2 id="20.1">20.1 可执行文件</h2> 
<h2 id="20.2">20.2 可执行文件格式</h2> 
<h2 id="20.3">20.3 执行域</h2> 
<h2 id="20.4">20.4 exec函数</h2> 

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="21">21 系统启动</h1> 

This appendix explains what happens right after users switch on their computers—that is, how a Linux kernel image is copied into memory and executed. In short, we discuss how the kernel, and thus the whole system, is “bootstrapped.”

本章主要讲解计算机打开后到底会发生什么-也就是说，内核镜像如何被拷贝到内存上并执行的。

Traditionally, the term bootstrap refers to a person who tries to stand up by pulling his own boots. In operating systems, the term denotes bringing at least a portion of the operating system into main memory and having the processor execute it. It also denotes the initialization of kernel data structures, the creation of some user processes, and the transfer of control to one of them.

所谓`引导`，就是如何把操作系统加载到主内存并让处理器执行它。还要完成内核数据结构的初始化，创建一些进程并将控制权交给其中一个的过程。

Computer bootstrapping is a tedious, long task, because initially, nearly every hardware device, including the RAM, is in a random, unpredictable state. Moreover, the bootstrap process is highly dependent on the computer architecture; as usual in this book, we refer to the 80×86 architecture.

计算机引导是一个冗长、复杂的任务，因为几乎所有的硬件，包括RAM，处于一个混乱无序的状态。更重要的是，引导过程严重依赖计算机硬件架构。本书专注于x86架构。

<h2 id="21.1">21.1 BIOS</h2>

The moment after a computer is powered on, it is practically useless because the RAM chips contain random data and no operating system is running. To begin the boot, a special hardware circuit raises the logical value of the RESET pin of the CPU. After RESET is asserted, some registers of the processor (including cs and eip) are set to fixed values, and the code found at physical address `0xfffffff0` is executed. This address is mapped by the hardware to a certain read-only, persistent memory chip that is often called Read-Only Memory (ROM). The set of programs stored in ROM is traditionally called the Basic Input/Output System (BIOS) in the 80×86 architecture, because it includes several interrupt-driven low-level procedures used by all operating systems in the booting phase to handle the hardware devices that make up the computer. Some operating systems, such as Microsoft’s MS-DOS, rely on BIOS to implement most system calls.

Once in protected mode (see the section [“Segmentation in Hardware” in Chapter 2](#2.2)), Linux does not use BIOS any longer, but it provides its own device driver for every hardware device on the computer. In fact, the BIOS procedures must be executed in real mode, so they cannot share functions even if that would be beneficial.

The BIOS uses Real Mode addresses because they are the only ones available when the computer is turned on. A Real Mode address is composed of a *seg* segment and an *off* offset; the corresponding physical address is given by *seg*\*16+*off*. As a result, no Global Descriptor Table, Local Descriptor Table, or paging table is needed by the CPU addressing circuit to translate a logical address into a physical one. Clearly, the code that initializes the GDT, LDT, and paging tables must run in Real Mode.

BIOS使用实模式地址。实模式地址由 *seg*的段地址和 *off*的偏移量组成，所以实际的物理地址就是 *seg*\*16+*off*。没有GDT（全局描述符表）、LDT（局部描述符表）或页表。很明显，运行在实模式的代码需要初始化GDT、LDT和页表。

Linux is forced to use BIOS in the bootstrapping phase, when it must retrieve the kernel image from disk or from some other external device. The BIOS bootstrap procedure essentially performs the following four operations:

如果Linux内核镜像存储在硬盘或者其它外部设备，则必须在引导阶段使用BIOS检索内核。BIOS引导程序主要执行下面4步：

1. Executes a series of tests on the computer hardware to establish which devices are present and whether they are working properly. This phase is often called <font color="blue">Power-On Self-Test (POST)</font>. During this phase, several messages, such as the BIOS version banner, are displayed.

    Recent 80×86, AMD64, and Itanium computers make use of the *Advanced Configuration and Power Interface (ACPI)* standard. The bootstrap code in an ACPI-compliant BIOS builds several tables that describe the hardware devices present in the system. These tables have a vendor-independent format and can be read by the operating system kernel to learn how to handle the devices.

    第一步，完成硬件上电自检测，专业术语称为 *Power-On Self-Test (POST)*。

2. <font color="blue">Initializes the hardware devices</font>. This phase is crucial in modern PCI-based architectures, because it guarantees that all hardware devices operate without conflicts on the IRQ lines and I/O ports. At the end of this phase, a table of installed PCI devices is displayed.
    
    第二步，初始化硬件。这一步对于现代基于PCI的硬件架构非常重要，可以检查IRQ中断线和I/O端口是否有冲突。完成这一步，会建立一个已安装的PCI设备的表。

3. Searches for an operating system to boot. Actually, depending on the BIOS setting, the procedure may try to access (in a predefined, customizable order) the first sector (boot sector) of every floppy disk, hard disk, and CD-ROM in the system.
    
    搜索需要引导的操作系统。检索的对象依赖于BIOS的设置，比如软盘、硬盘和CD-ROM中的第一个扇区，也称为引导扇区。

4. As soon as a valid device is found, it copies the contents of its first sector into RAM, starting from physical address 0x00007c00, and then jumps into that address and executes the code just loaded.
    
    只要找到合法的设备，就会把第一个扇区的内容拷贝到RAM，从物理地址0x00007c00开始，然后跳转到此地址并开始执行刚刚加载的代码。

The rest of this appendix takes you from the most primitive starting state to the full glory of a running Linux system.

本章余下的部分，将会从最原始的启动阶段到Linux系统运行起来，进行一一阐述。

<h2 id="21.2">21.2 Bootloader</h2> 

The boot loader is the program invoked by the BIOS to load the image of an operating system kernel into RAM. Let’s briefly sketch how boot loaders work in IBM’s PC architecture.

`boot loader`是BIOS为了加载操作系统内核到RAM而调用的程序。让我们简要概括一下在IBM的PC里boot loader是如何工作的。

To boot from a floppy disk, the instructions stored in its first sector are loaded in RAM and executed; these instructions copy all the remaining sectors containing the kernel image into RAM.

为了从软盘引导，存储在它的第一个扇区的代码被加载到RAM并执行；这些代码会被内核镜像加载到RAM中。

Booting from a hard disk is done differently. The first sector of the hard disk, named the Master Boot Record (MBR), includes the partition table* and a small program, which loads the first sector of the partition containing the operating system to be started. Some operating systems, such as Microsoft Windows 98, identify this partition by means of an active flag included in the partition table;† following this approach, only the operating system whose kernel image is stored in the active partition can be booted. As we will see later, Linux is more flexible because it replaces the rudimentary<font color="blue">基本的，初级的</font> program included in the MBR with a  sophisticated program—the “boot loader”—that allows users to select the operating system to be booted.

硬盘引导与软盘引导不太一样。硬盘的第一个扇区，称为`MBR`，包含分区表和一小段程序，它会加载分区中的第一个扇区里的操作系统。一些操作系统，比如Windows 98，会通过分区表中一个激活标志位判断该分区是哪个。使用这种方法，这有把操作系统的内核存储到该激活分区中才能被引导。后面我们还会介绍，Linux实际更为灵活，因为它替换了MBR中的这个简单程序，而使用一个更为复杂的程序，称为`boot loader`，允许用户可以选择将要引导的操作系统。

Kernel images of earlier Linux versions—up to the 2.4 series—included a minimal “boot loader” program in the first 512 bytes; thus, copying a kernel image starting from the first sector made the floppy bootable. On the other hand, kernel images of Linux 2.6 no longer include such boot loader; thus, in order to boot from floppy disk, a suitable boot loader has to be stored in the first disk sector. Nowadays, booting from a floppy is very similar to booting from a hard disk or from a CD-ROM.

早期Linux版本（直到2.4系列）的内核镜像在开始的512字节，包含一个最小的`boot loader`程序；因而，从第一个扇区开始复制内核镜像是可以从软盘启动的。但是，从Linux2.6版本开始，不再包含这样的引导程序；因而为了从软盘引导，必须选择一个合适的引导程序存储在软盘的第一个扇区。如今，从软盘引导和从硬盘或CD-ROM引导非常类似了。

<h2 id="21.2.1">21.2.1 从硬盘引导Linux</h2>

A two-stage boot loader is required to boot a Linux kernel from disk. A well-known Linux boot loader on 80×86 systems is named LInux LOader (LILO). Other boot loaders for 80×86 systems do exist; for instance, the GRand Unified Bootloader (GRUB) is also widely used. GRUB is more advanced than LILO, because it recognizes several disk-based filesystems and is thus capable of reading portions of the boot program from files. Of course, specific boot loader programs exist for all architectures supported by Linux.

从硬盘引导Linux内核，引导程序需要完成两个阶段的工作。x86系统上，一个著名的Linux引导程序叫做`LILO`。还有一些其它的引导程序，比如`GRUB`。它比`LILO`更高级，可以识别几个基于硬盘的文件系统，因此有能力从文件中读取boot程序。当然了，Linux支持的所有架构都有自己特定的引导程序。

LILO may be installed either on the MBR (replacing the small program that loads the boot sector of the active partition) or in the boot sector of every disk partition. In both cases, the final result is the same: when the loader is executed at boot time, the user may choose which operating system to load.

LILO即可以安装在主引导记录`MBR`上（替换加载到激活分区的引导扇区的小引导程序），也可以安装在硬盘分区的引导扇区上。两种情况下，最终结果是一样的：当引导程序执行起来后，用户可以选择需要加载的操作系统。

Actually, the LILO boot loader is too large to fit into a single sector, thus it is broken into two parts. The MBR or the partition boot sector includes a small boot loader, which is loaded into RAM starting from address 0x00007c00 by the BIOS. This small program moves itself to the address 0x00096a00, sets up the Real Mode stack (ranging from 0x00098000 to 0x000969ff), loads the second part of the LILO boot loader into RAM starting from address 0x00096c00, and jumps into it.

实际上，LILO引导程序太大，无法在一个扇区上安装，因而它被分为2部分。MBR或者分区上的引导扇区包含一个小引导程序，它被BIOS加载到RAM，起始地址是0x00007c00。这个小引导程序将自身搬运到地址0x00096a00处，建立实模式堆栈（范围0x00098000~0x000969ff），然后加载LILO引导程序的第2部分到RAM中，起始地址是0x00096c00，然后跳转到该地址并执行。

In turn, this latter program reads a map of bootable operating systems from disk and offers the user a prompt so she can choose one of them. Finally, after the user has chosen the kernel to be loaded (or let a time-out elapse so that LILO chooses a default), the boot loader may either copy the boot sector of the corresponding partition into RAM and execute it or directly copy the kernel image into RAM.

接下来，引导程序的第二部分读取可引导操作系统的映射表，提供给用户可选择的提示。最后，用户选择了要加载的内核之后（如果超时，LILO选择默认），引导程序即可以拷贝相应分区的引导扇区内容到RAM中并执行，也可以直接拷贝内核到RAM中。

Assuming that a Linux kernel image must be booted, the LILO boot loader, which relies on BIOS routines, performs essentially the following operations:

要加载Linux内核镜像，LILO基本上要执行下面几步：

1. Invokes a BIOS procedure to display a “Loading” message.
    
    调用BIOS程序显示正在加载的消息。

2. Invokes a BIOS procedure to load an initial portion of the kernel image from disk: the first 512 bytes of the kernel image are put in RAM at address 0x00090000, while the code of the setup() function (see below) is put in RAM starting from address 0x00090200.
    
    调用BIOS程序从硬盘加载内核镜像的开始部分：把内核的头512字节存放到RAM地址0x00090000处，同时，把`setup()`函数加载到RAM地址0x00090200处。

3. Invokes a BIOS procedure to load the rest of the kernel image from disk and puts the image in RAM starting from either low address 0x00010000 (for small kernel images compiled with `make zImage`) or high address 0x00100000 (for big kernel images compiled with make bzImage). In the following discussion, we say that the kernel image is “loaded low” or “loaded high” in RAM, respectively. Support for big kernel images uses essentially the same booting scheme as the other one, but it places data in different physical memory addresses to avoid problems with the ISA hole mentioned in the section “Physical Memory Layout” in Chapter 2.
    
    调用BIOS程序从硬盘加载内核其余部分：如果内核镜像较小（使用`make zImage`编译），存放到低地址0x00010000处；如果内核镜像较大（使用`make bzImage`编译），则存放到高地址0x00100000处。大内核镜像的引导原理和其它引导基本上是一样的，只是为了避免和ISA空洞（可以查看[物理内存布局](#2.5.3)）产生冲突，把数据放在了不同的物理地址空间上。

4. Jumps to the setup() code.
    
    跳转到`setup()`函数。

<h2 id="21.3">21.3 setup()函数</h2>

The code of the `setup()` assembly language function has been placed by the linker at offset 0x200 of the kernel image file. The boot loader can therefore easily locate the code and copy it into RAM, starting from physical address 0x00090200.

汇编函数 `setup()`的代码由链接器将其链接到到内核镜像文件偏移量0x200处。因此，引导程序能够轻松地索引代码并将其拷贝到物理地址0x00090200处。

The `setup()` function must initialize the hardware devices in the computer and set up the environment for the execution of the kernel program. Although the BIOS already initialized most hardware devices, Linux does not rely on it, but reinitializes the devices in its own manner to enhance portability and robustness. `setup()` performs essentially the following operations:

`setup()`函数负责初始化硬件设备，为内核执行提供环境。尽管BIOS已经初始化了大部分硬件，但是为了系统的可移植性和健壮性，Linux按照自己的方式重新初始化硬件设备。`setup()`执行以下步骤：

1. In ACPI-compliant systems, it invokes a BIOS routine that builds a table in RAM describing the layout of the system’s physical memory (the table can be seen in the boot kernel messages by looking for the “BIOS-e820” label). In older systems, it invokes a BIOS routine that just returns the amount of RAM available in the system.
    
    遵循ACPI的系统，调用BIOS服务程序在RAM中构建一张关于系统物理地址布局的表（该表可以通过在内核引导信息中搜索`BIOS-e820`字样找到。在较旧的系统上，只是调用BIOS服务程序返回系统中可用的RAM数量。

2. Sets the keyboard repeat delay and rate. (When the user keeps a key pressed past a certain amount of time, the keyboard device sends the corresponding keycode over and over to the CPU.)
    
    设置键盘重复计数延时和速率。（当用户按下按键后一段时间，键盘设备就会向CPU连续不断地发送相应的键码。

3. Initializes the video adapter card.
    
    初始化视频适配器卡。

4. Reinitializes the disk controller and determines the hard disk parameters.
    
    重新初始化硬盘控制器并设置硬盘参数。

5. Checks for an IBM Micro Channel bus (MCA).
    
    检查IBM微通道总线（MCA）。

6. Checks for a PS/2 pointing device (bus mouse).
    
    检查PS/2所指向的设备。

7. Checks for Advanced Power Management (APM) BIOS support.
    
    检查高级电源管理（APM）BIOS支持。

8. If the BIOS supports the *Enhanced Disk Drive Services (EDD)*, it invokes the proper BIOS procedure to build a table in RAM describing the hard disks available in the system. (The information included in the table can be seen by reading the files in the *firmware/edd* directory of the *sysfs* special filesystem.)
    
    如果BIOS支持 *Enhanced Disk Drive Services (EDD)*，它会调用正确的BIOS程序构建一张表，用来描述系统中可用的硬盘。（表中的信息可以通过读取 *firmware/edd*和 *sysfs*文件目录下的文件查看。）

9. If the kernel image was loaded low in RAM (at physical address 0x00010000), the function moves it to physical address 0x00001000. Conversely, if the kernel image was loaded high in RAM, the function does not move it. This step is necessary because to be able to store the kernel image on a floppy disk and to reduce the booting time, the kernel image stored on disk is compressed, and the decompression routine needs some free space to use as a temporary buffer following the kernel image in RAM.

    如果内核镜像载入低内存区（物理地址0x00010000），该函数会将其移动到物理地址0x00001000处。相反，如果内核镜像载入高内存区（物理地址0x00100000），该函数不会搬运它。这一步是必要的，因为为了能够在软盘上存储内核镜像，同时也是为了减少引导时间，存储在软盘上的内核镜像被压缩，解压缩服务程序需要一些紧跟在内核镜像后面的空间作为临时缓存。

10. Sets the A20 pin located on the 8042 keyboard controller. The A20 pin is a hack introduced in the 80286-based systems to make physical addresses compatible with those of the ancient 8088 microprocessors. Unfortunately, the A20 pin must be properly set before switching to Protected Mode, otherwise the 21st bit of every physical address will always be regarded as zero by the CPU. Setting the A20 pin is a messy operation.
    
    设置A20管脚，用于8042键盘控制器。该管脚的存在是为了和基于80286的系统进行兼容。

11. Sets up a provisional Interrupt Descriptor Table (IDT) and a provisional Global Descriptor Table (GDT).
    
    建立临时中段描述表（IDT）和临时GDT（全局描述表）。

12. Resets the floating-point unit (FPU), if any.
    
    复位浮点单元（FPU），当然前提是支持的话。

13. Reprograms the Programmable Interrupt Controllers (PIC) to mask all interrupts, except IRQ2 which is the cascading interrupt between the two PICs.
    
    重新设置可编程中断控制器（PIC）屏蔽所有中断，除了IRQ2，它是专门用于2个PIC级联的。

14. Switches the CPU from Real Mode to Protected Mode by setting the PE bit in the cr0 status register. The PG bit in the cr0 register is cleared, so paging is still disabled.
    
    设置cr0寄存器的PE标志位，切换CPU从实模式到保护模式。同时，cr0寄存器中的PG位被清除，所以此时分页机制仍然是禁止的。

15. Jumps to the startup_32() assembly language function.
    
    跳转到汇编函数`startup_32()`处。

<h2 id="21.4">21.4 setup_32()函数</h2> 

There are two different startup_32() functions; the one we refer to here is coded in the `arch/i386/boot/compressed/head.S` file. After `setup()` terminates, the function has been moved either to physical address 0x00100000 or to physical address 0x00001000, depending on whether the kernel image was loaded high or low in RAM.

内核代码中有2个不同的`startup_32()`函数，首先，调用的这个位于`arch/i386/boot/compressed/head.S`文件中。`setup()`函数执行完之后，函数即可以跳转到物理地址0x00100000处，也可以跳转到物理地址0x00001000处，这完全依赖于内核是被加载到高内存区域还是低内存区域。

This function performs the following operations:

这个函数执行的主要内容如下：

1. Initializes the segmentation registers and a provisional stack.

    初始化段寄存器和临时堆栈。

2. Clears all bits in the eflags register.
    
    清除`eflags`寄存器中的所有位。

3. Fills the area of uninitialized data of the kernel identified by the `_edata` and `_end` symbols with zeros (see the section “Physical Memory Layout” in Chapter 2).

    将内核中未初始化的数据填充为0，也就是符号`_edata`和`_end`指定的区域（参见[第2章 物理内存分布](#2.5.3)）。

4. Invokes the decompress_kernel() function to decompress the kernel image. The “Uncompressing Linux...” message is displayed first. After the kernel image is decompressed, the “OK, booting the kernel.” message is shown. If the kernel image was loaded low, the decompressed kernel is placed at physical address 0x00100000. Otherwise, if the kernel image was loaded high, the decompressed kernel is placed in a temporary buffer located after the compressed image. The decompressed image is then moved into its final position, which starts at physical address 0x00100000.

    调用解压缩函数`decompress_kernel()`函数解压内核镜像。

    首先，会显示：

        Uncompressing Linux...

    然后，解压缩完成后，显示：

        OK, booting the kernel.

    如果内核镜像被加载到低地址空间，解压缩后的内核被放置到地址0x00100000处。否则，如果内核镜像被加载到高地址空间，解压缩后的内核被放置到压缩内核镜像的后面的临时缓存中。然后，解压缩后的镜像会被移动到最后的位置，也就是开始地址0x00100000处。

5. Jumps to physical address 0x00100000.
    
    然后，跳转到物理地址0x00100000处。

The decompressed kernel image begins with another startup_32() function included in the `arch/i386/kernel/head.S` file. Using the same name for both the functions does not create any problems (besides confusing our readers), because both functions are executed by jumping to their initial physical addresses.

解压缩后的内核从另一个`startup_32()`函数开始执行，该函数位于`arch/i386/kernel/head.S`文件中。使用相同的函数名称不会产生问题（可能影响我们阅读代码），因为它们都是跳到各自的初始物理地址处执行。

The second startup_32() function sets up the execution environment for the first Linux process (process 0). The function performs the following operations:

第二个函数`startup_32()`函数建立第一个Linux进程（进程0）的运行环境。函数执行下面的操作：

1. Initializes the segmentation registers with their final values.
    
    使用最终的值，初始化段寄存器。

2. Fills the bss segment of the kernel (see the section “Program Segments and Process Memory Regions” in Chapter 20) with zeros.
    
    初始化内核的`bss`段为0（参见[第20章 程序段和进程内存区域](#20.1.5)）

3. Initializes the provisional kernel Page Tables contained in `swapper_pg_dir` and `pg0` to identically map the linear addresses to the same physical addresses, as explained in the section “Kernel Page Tables” in Chapter 2.

    初始化包含在`swapper_pg_dir`和`pg0`中的临时内核页表，以便将线性地址映射到相同的物理地址，如[第2章 内核页表](#2.5.5)所解释的那样。

4. Stores the address of the Page Global Directory in the cr3 register, and enables paging by setting the PG bit in the cr0 register.

    保存页全局目录（PGD）地址到cr3寄存器中，通过设置cr0寄存器中的PG标志，使能分页机制。

5. Sets up the Kernel Mode stack for process 0 (see the section “Kernel Threads” in Chapter 3).

    为进程0设定内核模式堆栈（参见[第3章 内核线程](#3.4.1)）。

6. Once again, the function clears all bits in the eflags register.

    再一次，该函数清除`eflags`中的所有位。

7. Invokes `setup_idt()` to fill the IDT with null interrupt handlers (see the section “Preliminary Initialization of the IDT” in Chapter 4).

    调用`setup_idt()`，使用空中断处理函数初始化IDT表（参见[第4章 IDT表的空初始化](#4.4.2)）。

8. Puts the system parameters obtained from the BIOS and the parameters passed to the operating system into the first page frame (see the section “Physical Memory Layout” in Chapter 2).

    把从BIOS获取的系统参数和要传递给操作系统的参数保存到第一个页帧中（参见[第2章 物理内存布局](#2.5.3)）。

9. Identifies the model of the processor.

    识别CPU模型。

10. Loads the `gdtr` and `idtr` registers with the addresses of the GDT and IDT tables.

    把GDT和IDT表的地址保存到`gdtr`和`idtr`寄存器中。

11. Jumps to the start_kernel() function.

    跳转到`start_kernel()`函数。

<h2 id="21.5">21.5 start_kernel()</h2>

The start_kernel() function completes the initialization of the Linux kernel. Nearly every kernel component is initialized by this function; we mention just a few of them:

`start_kernel()`函数完成Linux内核的初始化。内核的各个组成部分几乎都在这个函数中完成初始化，我们只提及它们中的一些。

1. The scheduler is initialized by invoking the sched_init() function (see Chapter 7).

    调用`sched_init()`函数（参见[第7章 进程调度](#7)，初始化调度器`scheduler`。


2. The memory zones are initialized by invoking the build_all_zonelists() function (see the section “Memory Zones” in Chapter 8).

    调用`build_all_zonelists()`函数（参见[第8章中的内存区](#8.1.3)），初始化内存区。

3. The Buddy system allocators are initialized by invoking the page_alloc_init() and mem_init() functions (see the section “The Buddy System Algorithm” in Chapter 8).
    
    调用`page_alloc_init()`和`mem_init()`函数（参见[第8章中的伙伴系统算法](#8.1.7)），初始化`伙伴系统分配器`。

4. The final initialization of the IDT is performed by invoking trap_init() (see the section “Exception Handling” in Chapter 4) and init_IRQ() (see the section “IRQ data structures” in Chapter 4).

    调用`trap_init()`（[第4章中的异常处理](#4.5)）和`init_IRQ()`函数（[第4章中的IRQ数据结构](#4.6.1.2)）完成`IDT`最终的初始化

5. The `TASKLET_SOFTIRQ` and `HI_SOFTIRQ` are initialized by invoking the softirq_init() function (see the section “Softirqs” in Chapter 4).

    调用`softirq_init()`函数（[第4章中的IRQ数据结构](#4.7.1)）完成`TASKLET_SOFTIRQ`和`HI_SOFTIRQ`的初始化。

6. The system date and time are initialized by the time_init() function (see the section “The Linux Timekeeping Architecture” in Chapter 6).

    调用`time_init()`函数（[第6章中的Linux计时架构](#6.2)）完成系统日期和时间的初始化。

7. The slab allocator is initialized by the kmem_cache_init() function (see the section “General and Specific Caches” in Chapter 8).

    调用`kmem_cache_init()`函数（[第8章中的通用和特定Cache](#8.2.4)）完成`slab`分配器初始化

8. The speed of the CPU clock is determined by invoking the calibrate_delay() function (see the section “Delay Functions” in Chapter 6).

    调用`calibrate_delay()`函数（[第6章中的延时函数](#6.5.3)）决定CPU时钟速率。

9. The kernel thread for process 1 is created by invoking the kernel_thread() function. In turn, this kernel thread creates the other kernel threads and executes the /sbin/init program, as described in the section “Kernel Threads” in Chapter 3.

    进程1的内核线程通过调用`kernel_thread()`函数创建。这个内核线程会以此创建其它内核线程并执行`/sbin/init`程序，如[第3章内核线程这一节](3.4.2)所描述的那样。

Besides the “Linux version 2.6.11...” message, which is displayed right after the beginning of start_kernel(), many other messages are displayed in this last phase, both by the init program and by the kernel threads. At the end, the familiar login prompt appears on the console (or in the graphical screen, if the X Window System is launched at startup), telling the user that the Linux kernel is up and running.

除了`Linux version 2.6.11...`这条提示信息之外，`start_kernel()`函数执行之后，还会显示一些其它的信息，这些信息都是由init程序和内核线程打印的。最后，熟悉的登陆提示信息就会出现在控制台上（或者在图形界面上，如果X Window系统启动的话），告诉用户Linux内核已经启动并正在运行。

<h1 id="22">22 模块化</h1> 
<h2 id="22.1">22.1 是否选择模块化</h2> 
<h2 id="22.2">22.2 实现模块</h2> 
<h2 id="22.3">22.3 加载和卸载模块</h2> 
<h2 id="22.4">22.4 根据需要加载模块</h2> 

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

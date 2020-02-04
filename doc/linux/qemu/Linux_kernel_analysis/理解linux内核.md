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

* **Linux is cost-free.** 
    
    You can install a complete Unix system at no expense other than the hardware (of course).

* **Linux is fully customizable in all its components.**
    
    Thanks to the compilation options, you can customize the kernel by selecting only the features really needed. Moreover, thanks to the GPL, you are allowed to freely read and modify the source code of the kernel and of all system programs.

* **Linux runs on low-end, inexpensive hardware platforms.** 
    
    You are able to build a network server using an old Intel 80386 system with 4 MB of RAM.

* **Linux is powerful.**
    
    Linux systems are very fast, because they fully exploit the features of the hardware components. The main Linux goal is efficiency, and indeed many design choices of commercial variants, like the STREAMS I/O subsystem, have been rejected by Linus because of their implied performance penalty.

* **Linux developers are excellent programmers.**
    
    Linux systems are very stable; they have a very low failure rate and system maintenance time.

* **The Linux kernel can be very small and compact.**
    
    It is possible to fit a kernel image, including a few system programs, on just one 1.44 MB floppy disk. As far as we know, none of the commercial Unix variants is able to boot from a single floppy disk.

* **Linux is highly compatible with many common operating systems.**
    
    Linux lets you directly mount filesystems for all versions of MS-DOS and Microsoft Windows, SVR4, OS/2, Mac OS X, Solaris, SunOS, NEXTSTEP, many BSD variants, and so on. Linux also is able to operate with many network layers, such as Ethernet (as well as Fast Ethernet, Gigabit Ethernet, and 10 Gigabit Ethernet), Fiber Distributed Data Interface (FDDI), High Performance Parallel Interface (HIPPI), IEEE 802.11 (Wireless LAN), and IEEE 802.15 (Bluetooth). By using suitable libraries, Linux systems are even able to directly run programs written for other operating systems. For example, Linux is able to execute some applications written for MS-DOS, Microsoft Windows, SVR3 and R4, 4.4BSD, SCO Unix, Xenix, and others on the 80x86 platform.

* **Linux is well supported.**
    
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

Unix files may have one of the following types:

* Regular file
* Directory
* Symbolic link
* Block-oriented device file
* Character-oriented device file
* Pipe and named pipe (also called FIFO)
* Socket

The first three file types are constituents of any Unix filesystem. Their implementation is described in detail in Chapter 18.

Device files are related both to I/O devices, and to device drivers integrated into the kernel. For example, when a program accesses a device file, it acts directly on the I/O device associated with that file (see [Chapter 13](#13)).

Pipes and sockets are special files used for interprocess communication (see the section “Synchronization and Critical Regions” later in this chapter; also see [Chapter 19](#19)).

<h3 id="1.5.4">1.5.4 文件描述符和Inode</h3>

Unix makes a clear distinction between the contents of a file and the information
about a file. With the exception of device files and files of special filesystems, each
file consists of a sequence of bytes. The file does not include any control information,
such as its length or an end-of-file (EOF) delimiter.

All information needed by the filesystem to handle a file is included in a data structure
called an inode. Each file has its own inode, which the filesystem uses to identify
the file.

While filesystems and the kernel functions handling them can vary widely from one
Unix system to another, they must always provide at least the following attributes,
which are specified in the POSIX standard:

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

As in the rest of this book, we offer details in this chapter on how 80×86 microprocessors address memory chips and how Linux uses the available addressing circuits. You will find, we hope, that when you learn the implementation details on Linux’s most popular platform you will better understand both the general theory of paging and how to research the implementation on other platforms.

This is the first of three chapters related to memory management; [Chapter 8](#8) discusses how the kernel allocates main memory to itself, while [Chapter 9](#9) considers how linear addresses are assigned to processes.

<h2 id="2.1">2.1 内存地址</h2>

程序员通常将内存地址作为访问内存单元内容的方式。但是，对于80x86处理器，必须区分下面3种地址（[ARM处理器是没有逻辑地址这个概念的，也就是说它们指令中的寻址方式是不同的。]()）：

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

To speed up the translation of logical addresses into linear addresses, the 80 × 86 processor
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

分页单元把所有的RAM分为固定长度的`页帧`（有时，也称为物理页）。每一个页帧都包含一个页，也就是说，页帧的长度和页的长度是一致的。页帧是内存的组成部分，因而是指一段存储区域。弄清楚`页`和`页帧`这两个概念是非常重要的；前者是一块数据，可能存储在任何页帧或硬盘上。

映射线性地址到物理地址的数据结构，称为`页表`；他们存储在内存中，在使能分页单元之前必须被正确初始化。

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

The amount of RAM supported by a processor is limited by the number of address
pins connected to the address bus. Older Intel processors from the 80386 to the Pentium
used 32-bit physical addresses. In theory, up to 4 GB of RAM could be installed
on such systems; in practice, due to the linear address space requirements of User
Mode processes, the kernel cannot directly address more than 1 GB of RAM, as we
will see in the later section “Paging in Linux.”

However, big servers that need to run hundreds or thousands of processes at the same
time require more than 4 GB of RAM, and in recent years this created a pressure on
Intel to expand the amount of RAM supported on the 32-bit 80 × 86 architecture.

Intel has satisfied these requests by increasing the number of address pins on its processors
from 32 to 36. Starting with the Pentium Pro, all Intel processors are now
able to address up to 236 = 64 GB of RAM. However, the increased range of physical
addresses can be exploited only by introducing a new paging mechanism that translates
32-bit linear addresses into 36-bit physical ones.

With the Pentium Pro processor, Intel introduced a mechanism called Physical
Address Extension (PAE). Another mechanism, Page Size Extension (PSE-36), was
introduced in the Pentium III processor, but Linux does not use it, and we won’t discuss
it further in this book.

PAE is activated by setting the Physical Address Extension (PAE) flag in the cr4 control
register. The Page Size (PS) flag in the page directory entry enables large page
sizes (2 MB when PAE is enabled).

Intel has changed the paging mechanism in order to support PAE.

* The 64 GB of RAM are split into 224 distinct page frames, and the physical
address field of Page Table entries has been expanded from 20 to 24 bits.
Because a PAE Page Table entry must include the 12 flag bits (described in the
earlier section “Regular Paging”) and the 24 physical address bits, for a grand
total of 36, the Page Table entry size has been doubled from 32 bits to 64 bits. As
a result, a 4-KB PAE Page Table includes 512 entries instead of 1,024.

* A new level of Page Table called the Page Directory Pointer Table (PDPT) consisting
of four 64-bit entries has been introduced.

* The cr3 control register contains a 27-bit Page Directory Pointer Table base
address field. Because PDPTs are stored in the first 4 GB of RAM and aligned to
a multiple of 32 bytes (25), 27 bits are sufficient to represent the base address of
such tables.

* When mapping linear addresses to 4 KB pages (PS flag cleared in Page Directory entry), the 32 bits of a linear address are interpreted in the following way:
    - cr3
        
        Points to a PDPT

    - bits 31–30
        
        Point to 1 of 4 possible entries in PDPT

    - bits 29–21
        
        Point to 1 of 512 possible entries in Page Directory

    - bits 20–12
        
        Point to 1 of 512 possible entries in Page Table

    - bits 11–0
        
        Offset of 4-KB page

* When mapping linear addresses to 2-MB pages (PS flag set in Page Directory entry), the 32 bits of a linear address are interpreted in the following way:
    - cr3
        
        Points to a PDPT

    - bits 31–30
        
        Point to 1 of 4 possible entries in PDPT

    - bits 29–21
        
        Point to 1 of 512 possible entries in Page Directory

    - bits 20–0
        
        Offset of 2-MB page

To summarize, once cr3 is set, it is possible to address up to 4 GB of RAM. If we
want to address more RAM, we’ll have to put a new value in cr3 or change the content
of the PDPT. However, the main problem with PAE is that linear addresses are
still 32 bits long. This forces kernel programmers to reuse the same linear addresses
to map different areas of RAM. We’ll sketch how Linux initializes Page Tables when
PAE is enabled in the later section, “Final kernel Page Table when RAM size is more
than 4096 MB.” Clearly, PAE does not enlarge the linear address space of a process,
because it deals only with physical addresses. Furthermore, only the kernel can modify
the page tables of the processes, thus a process running in User Mode cannot use
a physical address space larger than 4 GB. On the other hand, PAE allows the kernel
to exploit up to 64 GB of RAM, and thus to increase significantly the number of processes
in the system.

<h3 id="2.4.6">2.4.6 64位架构分页</h3>

As we have seen in the previous sections, two-level paging is commonly used by 32-
bit microprocessors*. Two-level paging, however, is not suitable for computers that
adopt a 64-bit architecture. Let’s use a thought experiment to explain why:

Start by assuming a standard page size of 4 KB. Because 1 KB covers a range of 210
addresses, 4 KB covers 212 addresses, so the Offset field is 12 bits. This leaves up to
52 bits of the linear address to be distributed between the Table and the Directory
fields. If we now decide to use only 48 of the 64 bits for addressing (this restriction
leaves us with a comfortable 256 TB address space!), the remaining 48-12 = 36 bits
will have to be split among Table and the Directory fields. If we now decide to reserve
18 bits for each of these two fields, both the Page Directory and the Page Tables of
each process should include 218 entries—that is, more than 256,000 entries.

For that reason, all hardware paging systems for 64-bit processors make use of additional paging levels. The number of levels used depends on the type of processor. Table 2-4 summarizes the main characteristics of the hardware paging systems used by some 64-bit platforms supported by Linux. Please refer to the section “Hardware Dependency” in Chapter 1 for a short description of the hardware associated with the platform name.

Table 2-4. Paging levels in some 64-bit architectures

| Platform name | Page size | Number of address bits used | Number of paging levels | Linear address splitting |
| --- | --- | --- | --- | --- |
| alpha  | 8 KB*| 43 | 3 | 10 + 10 + 10 + 13  |
| ia64   | 4 KB | 39 | 3 | 9 + 9 + 9 + 12     |
| ppc64  | 4 KB | 41 | 3 | 10 + 10 + 9 + 12   |
| sh64   | 4 KB | 41 | 3 | 10 + 10 + 9 + 12   |
| x86_64 | 4 KB | 48 | 4 | 9 + 9 + 9 + 9 + 12 |

> *This architecture supports different page sizes; we select a typical page size adopted by Linux.

As we will see in the section “Paging in Linux” later in this chapter, Linux succeeds in providing a common paging model that fits most of the supported hardware paging systems.

<h3 id="2.4.7">2.4.7 硬件Cache</h3>

Today’s microprocessors have clock rates of several gigahertz, while dynamic RAM
(DRAM) chips have access times in the range of hundreds of clock cycles. This
means that the CPU may be held back considerably while executing instructions that
require fetching operands from RAM and/or storing results into RAM.

Hardware cache memories were introduced to reduce the speed mismatch between
CPU and RAM. They are based on the well-known locality principle, which holds
both for programs and data structures. This states that because of the cyclic structure
of programs and the packing of related data into linear arrays, addresses close to
the ones most recently used have a high probability of being used in the near future.
It therefore makes sense to introduce a smaller and faster memory that contains the
most recently used code and data. For this purpose, a new unit called the line was
introduced into the 80 × 86 architecture. It consists of a few dozen contiguous bytes
that are transferred in burst mode between the slow DRAM and the fast on-chip
static RAM (SRAM) used to implement caches.

The cache is subdivided into subsets of lines. At one extreme, the cache can be direct mapped, in which case a line in main memory is always stored at the exact same location in the cache. At the other extreme, the cache is fully associative, meaning that any line in memory can be stored at any location in the cache. But most caches are to some degree N-way set associative, where any line of main memory can be stored in any one of N lines of the cache. For instance, a line of memory can be stored in two different lines of a two-way set associative cache.

As shown in Figure 2-10, the cache unit is inserted between the paging unit and the
main memory. It includes both a hardware cache memory and a cache controller. The
cache memory stores the actual lines of memory. The cache controller stores an array
of entries, one entry for each line of the cache memory. Each entry includes a tag and
a few flags that describe the status of the cache line. The tag consists of some bits
that allow the cache controller to recognize the memory location currently mapped
by the line. The bits of the memory’s physical address are usually split into three
groups: the most significant ones correspond to the tag, the middle ones to the cache
controller subset index, and the least significant ones to the offset within the line.

<img id="Figure_2-10" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_10.PNG">

图2-10 硬件cache

When accessing a RAM memory cell, the CPU extracts the subset index from the
physical address and compares the tags of all lines in the subset with the high-order
bits of the physical address. If a line with the same tag as the high-order bits of the
address is found, the CPU has a `cache hit`; otherwise, it has a `cache miss`.

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

Besides general-purpose hardware caches, 80 × 86 processors include another cache
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

Linux adopts a common paging model that fits both 32-bit and 64-bit architectures. As explained in the earlier section “Paging for 64-bit Architectures,” two paging levels are sufficient for 32-bit architectures, while 64-bit architectures require a higher number of paging levels. Up to version 2.6.10, the Linux paging model consisted of three paging levels. Starting with version 2.6.11, a four-level paging model has been adopted.* The four types of page tables illustrated in Figure 2-12 are called:

* Page Global Directory
* Page Upper Directory
* Page Middle Directory
* Page Table

The Page Global Directory includes the addresses of several Page Upper Directories,
which in turn include the addresses of several Page Middle Directories, which in
turn include the addresses of several Page Tables. Each Page Table entry points to a
page frame. Thus the linear address can be split into up to five parts. Figure 2-12
does not show the bit numbers, because the size of each part depends on the computer
architecture.

<img id="Figure_2-12" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_12.PNG">

图2-12 linux分页模型

For 32-bit architectures with no Physical Address Extension, two paging levels are sufficient. Linux essentially eliminates the Page Upper Directory and the Page Middle Directory fields by saying that they contain zero bits. However, the positions of the Page Upper Directory and the Page Middle Directory in the sequence of pointers are kept so that the same code can work on 32-bit and 64-bit architectures. The kernel keeps a position for the Page Upper Directory and the Page Middle Directory by setting the number of entries in them to 1 and mapping these two entries into the proper entry of the Page Global Directory.

For 32-bit architectures with the Physical Address Extension enabled, three paging
levels are used. The Linux’s Page Global Directory corresponds to the 80 × 86’s Page
Directory Pointer Table, the Page Upper Directory is eliminated, the Page Middle
Directory corresponds to the 80 × 86’s Page Directory, and the Linux’s Page Table
corresponds to the 80 × 86’s Page Table.

Finally, for 64-bit architectures three or four levels of paging are used depending on
the linear address bit splitting performed by the hardware (see Table 2-4).

Linux’s handling of processes relies heavily on paging. In fact, the automatic translation
of linear addresses into physical ones makes the following design objectives
feasible:

* Assign a different physical address space to each process, ensuring an efficient protection against addressing errors.

* Distinguish pages (groups of data) from page frames (physical addresses in main
memory). This allows the same page to be stored in a page frame, then saved to
disk and later reloaded in a different page frame. This is the basic ingredient of
the virtual memory mechanism (see Chapter 17).

In the remaining part of this chapter, we will refer for the sake of concreteness to the
paging circuitry used by the 80 × 86 processors.

As we will see in Chapter 9, each process has its own Page Global Directory and its
own set of Page Tables. When a process switch occurs (see the section “Process
Switch” in Chapter 3), Linux saves the cr3 control register in the descriptor of the
process previously in execution and then loads cr3 with the value stored in the
descriptor of the process to be executed next. Thus, when the new process resumes
its execution on the CPU, the paging unit refers to the correct set of Page Tables.

Mapping linear to physical addresses now becomes a mechanical task, although it is
still somewhat complex. The next few sections of this chapter are a rather tedious list
of functions and macros that retrieve information the kernel needs to find addresses
and manage the tables; most of the functions are one or two lines long. You may
want to only skim these sections now, but it is useful to know the role of these functions
and macros, because you’ll see them often in discussions throughout this book.


<h3 id="2.5.1">2.5.1 线性地址域</h3>

下面的宏简化了页表的处理：

* PAGE_SHIFT
    
    Specifies the length in bits of the Offset field; when applied to 80 × 86 processors, it yields the value 12. Because all the addresses in a page must fit in the Offset field, the size of a page on 80 × 86 systems is 212 or the familiar 4,096 bytes; the PAGE_SHIFT of 12 can thus be considered the logarithm base 2 of the total page size. This macro is used by PAGE_SIZE to return the size of the page. Finally, the PAGE_MASK macro yields the value 0xfffff000 and is used to mask all the bits of the Offset field.

* PMD_SHIFT
    
    The total length in bits of the Offset and Table fields of a linear address; in other words, the logarithm of the size of the area a Page Middle Directory entry can map. The PMD_SIZE macro computes the size of the area mapped by a single entry of the Page Middle Directory—that is, of a Page Table. The PMD_MASK macro is used to mask all the bits of the Offset and Table fields.

    When PAE is disabled, PMD_SHIFT yields the value 22 (12 from Offset plus 10 from Table), PMD_SIZE yields 2^22 or 4 MB, and PMD_MASK yields 0xffc00000. Conversely, when PAE is enabled, PMD_SHIFT yields the value 21 (12 from Offset plus 9 from Table), PMD_SIZE yields 2^21 or 2 MB, and PMD_MASK yields 0xffe00000.

    Large pages do not make use of the last level of page tables, thus LARGE_PAGE_SIZE, which yields the size of a large page, is equal to PMD_SIZE (2PMD_SHIFT) while LARGE_PAGE_MASK, which is used to mask all the bits of the Offset and Table fields in a large page address, is equal to PMD_MASK.

* PUD_SHIFT
    
    Determines the logarithm of the size of the area a Page Upper Directory entry can map. The PUD_SIZE macro computes the size of the area mapped by a single entry of the Page Global Directory. The PUD_MASK macro is used to mask all the bits of the Offset, Table, Middle Air, and Upper Air fields.

    On the 80 × 86 processors, PUD_SHIFT is always equal to PMD_SHIFT and PUD_SIZE is equal to 4 MB or 2 MB.

* PGDIR_SHIFT
    
    Determines the logarithm of the size of the area that a Page Global Directory entry can map. The PGDIR_SIZE macro computes the size of the area mapped by a single entry of the Page Global Directory. The PGDIR_MASK macro is used to mask all the bits of the Offset, Table, Middle Air, and Upper Air fields.

    When PAE is disabled, PGDIR_SHIFT yields the value 22 (the same value yielded by PMD_SHIFT and by PUD_SHIFT), PGDIR_SIZE yields 2^22 or 4 MB, and PGDIR_MASK yields 0xffc00000. Conversely, when PAE is enabled, PGDIR_SHIFT yields the value 30 (12 from Offset plus 9 from Table plus 9 from Middle Air), PGDIR_SIZE yields 2^30 or 1 GB, and PGDIR_MASK yields 0xc0000000.

* PTRS_PER_PTE, PTRS_PER_PMD, PTRS_PER_PUD, and PTRS_PER_PGD
    
    Compute the number of entries in the Page Table, Page Middle Directory, Page Upper Directory, and Page Global Directory. They yield the values 1,024, 1, 1, and 1,024, respectively, when PAE is disabled; and the values 512, 512, 1, and 4, respectively, when PAE is enabled.

<h3 id="2.5.2">2.5.2 页表处理</h3>

pte_t, pmd_t, pud_t, and pgd_t describe the format of, respectively, a Page Table, a
Page Middle Directory, a Page Upper Directory, and a Page Global Directory entry.
They are 64-bit data types when PAE is enabled and 32-bit data types otherwise.
pgprot_t is another 64-bit (PAE enabled) or 32-bit (PAE disabled) data type that represents
the protection flags associated with a single entry.

Five type-conversion macros—_ _pte, _ _pmd, _ _pud, _ _pgd, and _ _pgprot—cast an
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

Table 2-5. Page flag reading functions

| Function name | Description |
| ------------- | ----------- |
| pte_user()    | Reads the User/Supervisor flag |
| pte_read()    | Reads the User/Supervisor flag (pages on the 80 × 86 processor cannot be protected against reading) |
| pte_write()   | Reads the Read/Write flag |
| pte_exec()    | Reads the User/Supervisor flag (pages on the 80x86 processor cannot be protected against code execution) |
| pte_dirty()   | Reads the Dirty flag |
| pte_young()   | Reads the Accessed flag |
| pte_file()    | Reads the Dirty flag (when the Present flag is cleared and the Dirty flag is set, the page belongs to a non-linear disk file mapping; see Chapter 16) |

Another group of functions listed in Table 2-6 sets the value of the flags in a Page Table entry.

Table 2-6. Page flag setting functions

| Function name | Description |
| ------------- | ----------- |
| mk_pte_huge() | Sets the Page Size and Present flags of a Page Table entry |
| pte_wrprotect()    | Clears the Read/Write flag |
| pte_rdprotect()    | Clears the User/Supervisor flag |
| pte_exprotect()    | Clears the User/Supervisor flag |
| pte_mkwrite()    | Sets the Read/Write flag |
| pte_mkread()    | Sets the User/Supervisor flag |
| pte_mkexec()    | Sets the User/Supervisor flag |
| pte_mkclean()    | Clears the Dirty flag |
| pte_mkdirty()    | Sets the Dirty flag |
| pte_mkold()    | Clears the Accessed flag (makes the page old) |
| pte_mkyoung()    | Sets the Accessed flag (makes the page young) |
| pte_modify(p,v)   | Sets all access rights in a Page Table entry p to a specified value v |
| ptep_set_wrprotect()    | Like pte_wrprotect(), but acts on a pointer to a Page Table entry |
| ptep_set_access_flags()    | If the Dirty flag is set, sets the page’s access rights to a specified value and invokes flush_tlb_page() (see the section “Translation Lookaside Buffers (TLB)” later in this chapter) |
| ptep_mkdirty()    | Like pte_mkdirty() but acts on a pointer to a Page Table entry |
| ptep_test_and_clear_dirty()    | Like pte_mkclean() but acts on a pointer to a Page Table entry and returns the old value of the flag |
| ptep_test_and_clear_young()    | Like pte_mkold() but acts on a pointer to a Page Table entry and returnsthe old value of the flag |

Now, let’s discuss the macros listed in Table 2-7 that combine a page address and a
group of protection flags into a page table entry or perform the reverse operation of
extracting the page address from a page table entry. Notice that some of these macros refer to a page through the linear address of its “page descriptor” (see the section
“Page Descriptors” in Chapter 8) rather than the linear address of the page itself.

Table 2-7. Macros acting on Page Table entries

| Function name | Description |
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

As usual, the description of the functions listed in Table 2-8 refers to the 80 × 86
architecture.

Table 2-8. Page allocation functions

| Function name | Description |
| ------------- | ----------- |
| pgd_alloc(mm) | Allocates a new Page Global Directory; if PAE is enabled, it also allocates the three children Page Middle Directories that map the User Mode linear addresses. The argument mm (the address of a memory descriptor) is ignored  on the 80x86 architecture. |
| pgd_free( pgd) | Releases the Page Global Directory at address pgd; if PAE is enabled, it also releases the three Page Middle Directories that map the User Mode linear addresses. |
| pud_alloc(mm, pgd, addr) | In a two- or three-level paging system, this function does nothing: it simply returns the linear address of the Page Global Directory entry pgd. |
| pud_free(x) | In a two- or three-level paging system, this macro does nothing. |
| pmd_alloc(mm, pud, addr) | Defined so generic three-level paging systems can allocate a new Page Middle Directory for the linear address addr. If PAE is not enabled, the function simply returns the input parameter pud—that is, the address of the entry in the Page Global Directory. If PAE is enabled, the function returns the linear address of the Page Middle Directory entry that maps the linear address addr. The argument cw is ignored. |
| pmd_free(x ) | Does nothing, because Page Middle Directories are allocated and deallocated together with their parent Page Global Directory. |
| pte_alloc_map(mm, pmd, addr) | Receives as parameters the address of a Page Middle Directory entry pmd and a linear address addr, and returns the address of the Page Table entry corresponding to addr. If the Page Middle Directory entry is null, the function allocates a new Page Table by invoking pte_alloc_one( ). If a new Page Table is allocated, the entry corresponding to addr is initialized and the User/Supervisor flag is set. If the Page Table is kept in high memory, the kernel establishes a temporary kernel mapping (see the section “Kernel Mappings of High-Memory Page Frames” in Chapter 8), to be released by pte_unmap. |
| pte_alloc_kernel(mm, pmd, addr) | If the Page Middle Directory entry pmd associated with the address addr is null, the function allocates a new Page Table. It then returns the linear address of the Page Table entry associated with addr. Used only for master kernel page tables (see the later section “Kernel Page Tables”). |
| pte_free( pte) | Releases the Page Table associated with the pte page descriptor pointer. |
| pte_free_kernel(pte) | Equivalent to pte_free(), but used for master kernel page tables. |
| clear_page_range(mmu,start,end) | Clears the contents of the page tables of a process from linear address start to end by iteratively releasing its Page Tables and clearing the Page Middle Directory entries. |

<h3 id="2.5.3">2.5.3 物理内存布局</h3>

During the initialization phase the kernel must build a physical addresses map that specifies which physical address ranges are usable by the kernel and which are unavailable (either because they map hardware devices’ I/O shared memory or because the corresponding page frames contain BIOS data).

The kernel considers the following page frames as *reserved*:

* Those falling in the unavailable physical address ranges
* Those containing the kernel’s code and initialized data structures

A page contained in a reserved page frame can never be dynamically assigned or swapped to disk.

As a general rule, the Linux kernel is installed in RAM starting from the physical address 0x00100000—i.e., from the second megabyte. The total number of page frames required depends on how the kernel is configured. A typical configuration yields a kernel that can be loaded in less than 3 MB of RAM.

Why isn’t the kernel loaded starting with the first available megabyte of RAM? Well, the PC architecture has several peculiarities that must be taken into account. For example:

* Page frame 0 is used by BIOS to store the system hardware configuration detected during the Power-On Self-Test (POST); the BIOS of many laptops, moreover, writes data on this page frame even after the system is initialized.

* Physical addresses ranging from 0x000a0000 to 0x000fffff are usually reserved to
BIOS routines and to map the internal memory of ISA graphics cards. This area
is the well-known hole from 640 KB to 1 MB in all IBM-compatible PCs: the
physical addresses exist but they are reserved, and the corresponding page
frames cannot be used by the operating system.

* Additional page frames within the first megabyte may be reserved by specific
computer models. For example, the IBM ThinkPad maps the 0xa0 page frame
into the 0x9f one.

In the early stage of the boot sequence (see Appendix A), the kernel queries the BIOS
and learns the size of the physical memory. In recent computers, the kernel also
invokes a BIOS procedure to build a list of physical address ranges and their corresponding
memory types.

Later, the kernel executes the machine_specific_memory_setup() function, which
builds the physical addresses map (see Table 2-9 for an example). Of course, the kernel
builds this table on the basis of the BIOS list, if this is available; otherwise the
kernel builds the table following the conservative default setup: all page frames with
numbers from 0x9f (LOWMEMSIZE()) to 0x100 (HIGH_MEMORY) are marked as reserved.

Table 2-9. Example of BIOS-provided physical addresses map

| Start      | End        | Type   |
| ---------- | ---------- | ------ |
| 0x00000000 | 0x0009ffff | Usable |
| 0x000f0000 | 0x000fffff | Reserved |
| 0x00100000 | 0x07feffff | Usable |
| 0x07ff0000 | 0x07ff2fff | ACPI data |
| 0x07ff3000 | 0x07ffffff | ACPI NVS |
| 0xffff0000 | 0xffffffff | Reserved |

A typical configuration for a computer having 128 MB of RAM is shown in
Table 2-9. The physical address range from 0x07ff0000 to 0x07ff2fff stores information
about the hardware devices of the system written by the BIOS in the POST
phase; during the initialization phase, the kernel copies such information in a suitable
kernel data structure, and then considers these page frames usable. Conversely,
the physical address range of 0x07ff3000 to 0x07ffffff is mapped to ROM chips of the hardware devices. The physical address range starting from 0xffff0000 is marked
as reserved, because it is mapped by the hardware to the BIOS’s ROM chip (see
Appendix A). Notice that the BIOS may not provide information for some physical
address ranges (in the table, the range is 0x000a0000 to 0x000effff). To be on the safe
side, Linux assumes that such ranges are not usable.

The kernel might not see all physical memory reported by the BIOS: for instance, the
kernel can address only 4 GB of RAM if it has not been compiled with PAE support,
even if a larger amount of physical memory is actually available. The setup_memory( )
function is invoked right after machine_specific_memory_setup(): it analyzes the table
of physical memory regions and initializes a few variables that describe the kernel’s
physical memory layout. These variables are shown in Table 2-10.

Table 2-10. Variables describing the kernel’s physical memory layout

| Variable name | Description |
| ------------- | ----------- |
| num_physpages | Page frame number of the highest usable page frame |
| totalram_pages | Total number of usable page frames |
| min_low_pfn | Page frame number of the first usable page frame after the kernel image in RAM |
| max_pfn | Page frame number of the last usable page frame |
| max_low_pfn | Page frame number of the last page frame directly mapped by the kernel (low memory) |
| totalhigh_pages | Total number of page frames not directly mapped by the kernel (high memory) |
| highstart_pfn | Page frame number of the first page frame not directly mapped by the kernel |
| highend_pfn | Page frame number of the last page frame not directly mapped by the kernel |

To avoid loading the kernel into groups of noncontiguous page frames, Linux prefers
to skip the first megabyte of RAM. Clearly, page frames not reserved by the PC
architecture will be used by Linux to store dynamically assigned pages.

Figure 2-13 shows how the first 3 MB of RAM are filled by Linux. We have assumed
that the kernel requires less than 3 MB of RAM.

The symbol _text, which corresponds to physical address 0x00100000, denotes the
address of the first byte of kernel code. The end of the kernel code is similarly identified
by the symbol _etext. Kernel data is divided into two groups: initialized and
uninitialized. The initialized data starts right after _etext and ends at _edata. The
uninitialized data follows and ends up at _end.

The symbols appearing in the figure are not defined in Linux source code; they are
produced while compiling the kernel.*

<img id="Figure_2-13" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_13.PNG">

图2-13 Linux内核前768个页帧

<h3 id="2.5.4">2.5.4 进程页表</h3>

The linear address space of a process is divided into two parts:

* Linear addresses from 0x00000000 to 0xbfffffff can be addressed when the process runs in either User or Kernel Mode.

* Linear addresses from 0xc0000000 to 0xffffffff can be addressed only when the process runs in Kernel Mode.

When a process runs in User Mode, it issues linear addresses smaller than
0xc0000000; when it runs in Kernel Mode, it is executing kernel code and the linear
addresses issued are greater than or equal to 0xc0000000. In some cases, however, the
kernel must access the User Mode linear address space to retrieve or store data.

The PAGE_OFFSET macro yields the value 0xc0000000; this is the offset in the linear
address space of a process where the kernel lives. In this book, we often refer directly
to the number 0xc0000000 instead.

The content of the first entries of the Page Global Directory that map linear
addresses lower than 0xc0000000 (the first 768 entries with PAE disabled, or the first
3 entries with PAE enabled) depends on the specific process. Conversely, the remaining
entries should be the same for all processes and equal to the corresponding
entries of the master kernel Page Global Directory (see the following section).

<h3 id="2.5.5">2.5.5 内核页表</h3>

The kernel maintains a set of page tables for its own use, rooted at a so-called master
kernel Page Global Directory. After system initialization, this set of page tables is
never directly used by any process or kernel thread; rather, the highest entries of the
master kernel Page Global Directory are the reference model for the corresponding
entries of the Page Global Directories of every regular process in the system.

We explain how the kernel ensures that changes to the master kernel Page Global
Directory are propagated to the Page Global Directories that are actually used by
processes in the section “Linear Addresses of Noncontiguous Memory Areas” in
Chapter 8.

We now describe how the kernel initializes its own page tables. This is a two-phase
activity. In fact, right after the kernel image is loaded into memory, the CPU is still
running in real mode; thus, paging is not enabled.

In the first phase, the kernel creates a limited address space including the kernel’s
code and data segments, the initial Page Tables, and 128 KB for some dynamic data
structures. This minimal address space is just large enough to install the kernel in
RAM and to initialize its core data structures.

In the second phase, the kernel takes advantage of all of the existing RAM and sets
up the page tables properly. Let us examine how this plan is executed.

<h4 id="2.5.5.1">2.5.5.1 临时内核页表</h4>

A provisional Page Global Directory is initialized statically during kernel compilation,
while the provisional Page Tables are initialized by the startup_32() assembly
language function defined in arch/i386/kernel/head.S. We won’t bother mentioning
the Page Upper Directories and Page Middle Directories anymore, because they are
equated to Page Global Directory entries. PAE support is not enabled at this stage.

The provisional Page Global Directory is contained in the swapper_pg_dir variable.
The provisional Page Tables are stored starting from pg0, right after the end of the
kernel’s uninitialized data segments (symbol _end in Figure 2-13). For the sake of
simplicity, let’s assume that the kernel’s segments, the provisional Page Tables, and
the 128 KB memory area fit in the first 8 MB of RAM. In order to map 8 MB of RAM,
two Page Tables are required.

The objective of this first phase of paging is to allow these 8 MB of RAM to be easily
addressed both in real mode and protected mode. Therefore, the kernel must create a
mapping from both the linear addresses 0x00000000 through 0x007fffff and the linear
addresses 0xc0000000 through 0xc07fffff into the physical addresses 0x00000000
through 0x007fffff. In other words, the kernel during its first phase of initialization
can address the first 8 MB of RAM by either linear addresses identical to the physical
ones or 8 MB worth of linear addresses, starting from 0xc0000000.

The Kernel creates the desired mapping by filling all the swapper_pg_dir entries with
zeroes, except for entries 0, 1, 0x300 (decimal 768), and 0x301 (decimal 769); the latter
two entries span all linear addresses between 0xc0000000 and 0xc07fffff. The 0,
1, 0x300, and 0x301 entries are initialized as follows:

* The address field of entries 0 and 0x300 is set to the physical address of pg0, while the address field of entries 1 and 0x301 is set to the physical address of the page frame following pg0.
* The Present, Read/Write, and User/Supervisor flags are set in all four entries.
* The Accessed, Dirty, PCD, PWD, and Page Size flags are cleared in all four entries.

The startup_32( ) assembly language function also enables the paging unit. This is
achieved by loading the physical address of swapper_pg_dir into the cr3 control register
and by setting the PG flag of the cr0 control register, as shown in the following
equivalent code fragment:

    movl $swapper_pg_dir-0xc0000000, %eax
    movl %eax, %cr3                         /* set the page table pointer.. */
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0                         /* ..and set paging (PG) bit */

<h4 id="2.5.5.2">2.5.5.2 内核页表（RAM < 896M）</h4>

The final mapping provided by the kernel page tables must transform linear
addresses starting from `0xc0000000` into physical addresses starting from 0.

The `__pa` macro is used to convert a linear address starting from PAGE_OFFSET to the
corresponding physical address, while the `__va` macro does the reverse.

The master kernel Page Global Directory is still stored in `swapper_pg_dir`. It is initialized
by the `paging_init()` function, which does the following:

1. Invokes pagetable_init() to set up the Page Table entries properly.
2. Writes the physical address of swapper_pg_dir in the cr3 control register.
3. If the CPU supports PAE and if the kernel is compiled with PAE support, sets the PAE flag in the cr4 control register.
4. Invokes __flush_tlb_all() to invalidate all TLB entries.

The actions performed by pagetable_init() depend on both the amount of RAM
present and on the CPU model. Let’s start with the simplest case. Our computer has
less than 896 MB* of RAM, 32-bit physical addresses are sufficient to address all the
available RAM, and there is no need to activate the PAE mechanism. (See the earlier
section “The Physical Address Extension (PAE) Paging Mechanism.”)

The swapper_pg_dir Page Global Directory is reinitialized by a cycle equivalent to the
following:

    pgd = swapper_pg_dir + pgd_index(PAGE_OFFSET); /* 768 */
    phys_addr = 0x00000000;
    while (phys_addr < (max_low_pfn * PAGE_SIZE)) {
        pmd = one_md_table_init(pgd); /* returns pgd itself */
        set_pmd(pmd, _ _pmd(phys_addr | pgprot_val(_ _pgprot(0x1e3))));
        /* 0x1e3 == Present, Accessed, Dirty, Read/Write, Page Size, Global */
        phys_addr += PTRS_PER_PTE * PAGE_SIZE; /* 0x400000 */
        ++pgd;
    }

We assume that the CPU is a recent 80 × 86 microprocessor supporting 4 MB pages
and “global” TLB entries. Notice that the User/Supervisor flags in all Page Global
Directory entries referencing linear addresses above 0xc0000000 are cleared, thus
denying processes in User Mode access to the kernel address space. Notice also that
the Page Size flag is set so that the kernel can address the RAM by making use of
large pages (see the section “Extended Paging” earlier in this chapter).

The identity mapping of the first megabytes of physical memory (8 MB in our example)
built by the startup_32() function is required to complete the initialization phase
of the kernel. When this mapping is no longer necessary, the kernel clears the corresponding
page table entries by invoking the zap_low_mappings() function.

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
        set_pgd(swapper_pg_dir + i, _ _pgd(_ _pa(empty_zero_page) + 0x001));
        /* 0x001 == Present */
    pgd = swapper_pg_dir + pgd_idx;
    phys_addr = 0x00000000;
    for (; i<PTRS_PER_PGD; ++i, ++pgd) {
        pmd = (pmd_t *) alloc_bootmem_low_pages(PAGE_SIZE);
        set_pgd(pgd, _ _pgd(_ _pa(pmd) | 0x001)); /* 0x001 == Present */
        if (phys_addr < max_low_pfn * PAGE_SIZE)
            for (j=0; j < PTRS_PER_PMD /* 512 */
                    && phys_addr < max_low_pfn*PAGE_SIZE; ++j) {
                set_pmd(pmd, _ _pmd(phys_addr | pgprot_val(_ _pgprot(0x1e3))));
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
because the symbol _ _this_fixmap_does_not_exist is not defined anywhere. Eventually,
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

Cache synchronization is performed automatically by the 80 × 86 microprocessors, thus the Linux kernel for this kind of processor does not perform any hardware cache flushing. The kernel does provide, however, cache flushing interfaces for processors that do not synchronize caches.

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

Notice that the flush_tlb_pgtables method is missing from Table 2-12: in the 80 × 86
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
same set of page tables (see the section “The schedule( ) Function” in
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

* If the CPU switches to another process that is using the same set of page tables as the kernel thread that is being replaced, the kernel invokes _ _flush_tlb() to invalidate all non-global TLB entries of the CPU.

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
80 × 86 architecture a Linux system call can be invoked in two different ways. The net
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
<h2 id="21.1">21.1 BIOS</h2> 
<h2 id="21.2">21.2 Bootloader</h2> 
<h2 id="21.3">21.3 setup()函数</h2> 
<h2 id="21.4">21.4 setup_32()函数</h2> 
<h2 id="21.5">21.5 start_kernel()</h2> 
<h1 id="22">22 模块化</h1> 
<h2 id="22.1">22.1 是否选择模块化</h2> 
<h2 id="22.2">22.2 实现模块</h2> 
<h2 id="22.3">22.3 加载和卸载模块</h2> 
<h2 id="22.4">22.4 根据需要加载模块</h2> 

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

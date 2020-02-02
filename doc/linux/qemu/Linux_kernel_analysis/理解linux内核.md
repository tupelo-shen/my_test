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
| G      | Granularity flag: if it is cleared (equal to 0), the segment size is expressed in bytes; otherwise, it is expressed in multiples of 4096 bytes. |
| Limit  | ----------- |
| S      | ----------- |
| Type   | ----------- |
| DPL    | ----------- |
| P      | ----------- |
| D or B | ----------- |

<img id="Figure_2-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_3.PNG">

图2-3 段描述符格式

<img id="Figure_2-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_3.PNG">

图2-3 段描述符格式

<img id="Figure_2-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_2_3.PNG">

图2-5 转换一个逻辑地址

<h2 id="2.3">2.3 Linux中的分段机制</h2>
<h2 id="2.4">2.4 内存分页</h2>
<h2 id="2.5">2.5 Linux中的分页机制</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="3">3 进程</h1>
<h2 id="1">3.1 进程、轻量级进程和线程</h2>
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

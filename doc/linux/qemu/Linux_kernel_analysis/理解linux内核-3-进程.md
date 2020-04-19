<h1 id="0">0 目录</h1>

* [3 进程](#3)
    - [3.1 进程、轻量级进程和线程](#3.1)
    - [3.2 进程描述符](#3.2)
        + [3.2.1 进程状态](#3.2.1)
        + [3.2.2 识别进程](#3.2.2)
        + [3.2.3 进程之间关系](#3.2.3)
        + [3.2.4 如何组织进程](#3.2.4)
        + [3.2.5 进程资源限制](#3.2.5)
    - [3.3 进程切换](#3.3)
        + [3.3.1 硬件上下文](#3.3.1)
        + [3.3.2 任务状态段-TSS](#3.3.2)
        + [3.3.3 执行进程切换](#3.3.3)
        + [3.3.4 保存和加载FPU、MMX和XMM寄存器](#3.3.4)
    - [3.4 创建进程](#3.4)
        + [3.4.1 clone()、fork()和vfork()系统调用](#3.4.1)
        + [3.4.2 内核线程](#3.4.2)
    - [3.5 销毁进程](#3.5)
        + [3.5.1 终止进程](#3.5.1)
        + [3.5.2 移除进程](#3.5.2)

---

<h1 id="3">3 进程</h1>

The concept of a process is fundamental to any multiprogramming operating system. A process is usually defined as an instance of a program in execution; thus, if 16 users are running vi at once, there are 16 separate processes (although they can share the same executable code). Processes are often called tasks or threads in the Linux source code.

进程的概念是多任务操作系统的基础。一个进程通常是正在执行的程序的一个实例，比如说，如果16个用户同时运行vi编辑器，就会有16个独立的进程（尽管它们共享相同的可执行代码）。在Linux源代码中，进程经常被称为任务或线程。

In this chapter, we discuss static properties of processes and then describe how process switching is performed by the kernel. The last two sections describe how processes can be created and destroyed. We also describe how Linux supports multithreaded applications—as mentioned in Chapter 1, it relies on so-called lightweight processes (LWP).

在本章中，我们将讨论进程的静态属性，然后描述内核如何实施进程切换。最后两节描述进程的创建和销毁。我们还会描述Linux如何支持多线程程序，正如我们在第1章提到的，它依赖与所谓的轻进程（LWP）。

<h2 id="3.1">3.1 进程、轻量级进程和线程</h2>

The term “process” is often used with several different meanings. In this book, we stick to the usual OS textbook definition: a process is an instance of a program in execution. You might think of it as the collection of data structures that fully describes how far the execution of the program has progressed.

术语"进程"经常被赋予不同的意义。在本书中，我们坚持操作系统教科书中的定义：进程时正在执行程序的实例。你可以认为进程就是一个数据结构的集合，完整地描述了程序执行的进度。

> <font color="blue">补充：
>
> 现代操作系统的概念中，进程就是线程的容器，线程是内核调度的最小执行单元。一个正在执行的程序可以有多个进程，而进程又可以有多个线程。线程的划分粒度更小，导致并发性更高。另外，进程可以拥有独立的内存单元，而线程共享内存，从而极大提高了程序的运行效率。
>
> </font>

Processes are like human beings: they are generated, they have a more or less significant life, they optionally generate one or more child processes, and eventually they die. A small difference is that sex is not really common among processes—each process has just one parent.

进程像极了人类：它们被创造，拥有一个或轻或重的人生，它们可以任意创建一个或多个子进程，最后死亡。一个细微的不同就是，进程没有性别之分，每个进程只有一个父进程。

From the kernel’s point of view, the purpose of a process is to act as an entity to which system resources (CPU time, memory, etc.) are allocated.

从内核的角度看，进程就是分配系统资源（CPU时间、内存等）的实体单元。

When a process is created, it is almost identical to its parent. It receives a (logical) copy of the parent’s address space and executes the same code as the parent, beginning at the next instruction following the process creation system call. Although the parent and child may share the pages containing the program code (text), they have separate copies of the data (stack and heap), so that changes by the child to a memory location are invisible to the parent (and vice versa).

当进程被创建时，几乎与父进程相同。它拷贝父进程的地址空间，执行与父进程一样的代码，从创建进程的系统调用的下一条指令开始执行。尽管父进程和子进程共享代码段，但是它们具有独立的数据拷贝（堆栈），以至于子进程修改内存的某个位置，对父进程是不可见的，等等。

While earlier Unix kernels employed this simple model, modern Unix systems do not. They support  multithreaded applications—user programs having many relatively independent execution flows sharing a large portion of the application data structures. In such systems, a process is composed of several user threads (or simply threads), each of which represents an execution flow of the process. Nowadays, most multithreaded applications are written using standard sets of library functions called pthread (POSIX thread) libraries.

早期的Unix内核采用这种简单的模型，但是现代Unix系统已经不是了。它们支持多线程程序：用户程序具有许多相对独立的执行流，它们共享大部分的数据结构。在这样的系统中，进程由多个用户线程组成（或单线程），每个线程代表进程的一个执行流。现在，大多数多线程程序都是使用标准线程库（POSIX-thread）的库函数编写的。

Older versions of the Linux kernel offered no support for multithreaded applications. From the kernel point of view, a multithreaded application was just a normal process. The multiple execution flows of a multithreaded application were created, handled, and scheduled entirely in User Mode, usually by means of a POSIX-compliant pthread library.

旧版本的Linux内核不支持多线程程序。它们认为，从内核的角度看，一个多线程程序就是一个普通的进程。完全可以通过使用与POSIX兼容的pthread库，实现在用户态下创建，处理和调度多线程程序的多个执行流。

However, such an implementation of multithreaded applications is not very satisfactory. For instance, suppose a chess program uses two threads: one of them controls the graphical chessboard, waiting for the moves of the human player and showing the moves of the computer, while the other thread ponders the next move of the game. While the first thread waits for the human move, the second thread should run continuously, thus exploiting the thinking time of the human player. However, if the chess program is just a single process, the first thread cannot simply issue a blocking system call waiting for a user action; otherwise, the second thread is blocked as well. Instead, the first thread must employ sophisticated nonblocking techniques to ensure that the process remains runnable.

但是，这样的多线程程序实现不是很令人满意。比如，假设一个国际象棋游戏程序，使用2个线程：其中的一个线程控制棋盘，等待用户的动作并显示计算机的动作，而另一个线程思考游戏的下一步动作。当第一个线程等待人类用户的动作时，第二个线程还应该继续运行，从而充分利用人类选手的思考时间。但是，如果象棋游戏程序就是一个单进程程序的话，第一个线程不能简单地发出一个阻塞系统调用等待用户动作；否则，第二个线程也被阻塞。除非第一个线程必须采用复杂的非阻塞技术才能保证进程继续运行。

Linux uses lightweight processes to offer better support for multithreaded applications. Basically, two lightweight processes may share some resources, like the address space, the open files, and so on. Whenever one of them modifies a shared resource, the other immediately sees the change. Of course, the two processes must synchronize themselves when accessing the shared resource.

Linux采用轻进程（LWP）的方法提供更好的多线程程序支持。大体上，两个轻进程共享某些资源，像地址空间，打开的文件等等。当一个轻进程修改了某个共享资源，另一个立即能看到变化。当然了，两个轻进程必须在访问共享资源的时候实现同步。

A straightforward way to implement multithreaded applications is to associate a lightweight process with each thread. In this way, the threads can access the same set of application data structures by simply sharing the same memory address space, the same set of open files, and so on; at the same time, each thread can be scheduled independently by the kernel so that one may sleep while another remains runnable. Examples of POSIX-compliant pthread libraries that use Linux’s lightweight processes are LinuxThreads, Native POSIX Thread Library (NPTL), and IBM’s Next Generation Posix Threading Package (NGPT).

实现多线程程序的最直接方式就是将一个轻进程和一个线程关联起来。这种方法，线程们通过共享相同的资源，比如，内存地址空间，打开的文件等，能够访问相同的应用程序的数据结构。同时，每一个线程可以被内核独立调度，这样，当一个线程休眠的时候，另一个仍然可以运行。使用了Linux轻进程技术并与POSIX兼容的pthread库的例子有LinuxThreads、Native POSIX Thread Library (NPTL)和IBM的下一代Posix线程包（NGPT）。

POSIX-compliant multithreaded applications are best handled by kernels that support “thread groups.” In Linux a thread group is basically a set of lightweight processes that implement a multithreaded application and act as a whole with regards to some system calls such as getpid(), kill(), and _exit(). We are going to describe them at length later in this chapter.

与POSIX兼容的多线程程序最好是由支持`线程组`的内核处理。Linux中，线程组基本上是由一组轻进程（LWP）组成的，他们实现一个多线程程序，并作为一个整体，处理一些系统调用诸如getpid()、kill()和_exit()。在本章的后面我们将描述它们。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3.2">3.2 进程描述符</h2>

To manage processes, the kernel must have a clear picture of what each process is doing. It must know, for instance, the process’s priority, whether it is running on a CPU or blocked on an event, what address space has been assigned to it, which files it is allowed to address, and so on. This is the role of the process descriptor—a task_struct type structure whose fields contain all the information related to a single process.* As the repository of so much information, the process descriptor is rather complex. In addition to a large number of fields containing process attributes, the process descriptor contains several pointers to other data structures that, in turn, contain pointers to other structures. Figure 3-1 describes the Linux process descriptor schematically.

为了管理进程，内核必须清楚的知道每个进程在做什么。比如，进程优先级，正在CPU上运行还是阻塞在某个事件上，分配给它的进程地址空间，允许访问的文件等等。对于这些，内核使用进程描述符记录这一切，数据结构是task_struct，在这个结构中，包含单个进程的所有信息。包含了这么多信息，进程描述符相当复杂。除了大量包含进程属性的成员之外，还包含了几个指向其它数据结构的指针，进一步，这些其它数据结构又包含指向更多数据结构的指针。图3-1概要性地描述了Linux进程描述符。

The six data structures on the right side of the figure refer to specific resources owned by the process. Most of these resources will be covered in future chapters. This chapter focuses on two types of fields that refer to the process state and to process parent/child relationships.

图中右边的6个数据结构都是进程拥有的具体资源。这些资源的大部分会在后面的章节中覆盖到。本章主要关注两部分，分别是进程状态和父、子进程的关系。

<img id="Figure_3-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_1.PNG">

图3-1 Linux进程描述符示意图

<h3 id="3.2.1">3.2.1 进程状态</h3>

As its name implies, the state field of the process descriptor describes what is currently happening to the process. It consists of an array of flags, each of which describes a possible process state. In the current Linux version, these states are mutually exclusive, and hence exactly one flag of state always is set; the remaining flags are cleared. The following are the possible process states:

1. TASK_RUNNING-运行态

    The process is either executing on a CPU or waiting to be executed.

2. TASK_INTERRUPTIBLE-阻塞态（可中断）

    The process is suspended (sleeping) until some condition becomes true. Raising a hardware interrupt, releasing a system resource the process is waiting for, or delivering a signal are examples of conditions that might wake up the process (put its state back to TASK_RUNNING).


3. TASK_UNINTERRUPTIBLE-阻塞态（不可中断）

    Like TASK_INTERRUPTIBLE, except that delivering a signal to the sleeping process leaves its state unchanged. This process state is seldom used. It is valuable, however, under certain specific conditions in which a process must wait until a given event occurs without being interrupted. For instance, this state may be used when a process opens a device file and the corresponding device driver starts probing for a corresponding hardware device. The device driver must not be interrupted until the probing is complete, or the hardware device could be left in an unpredictable state.

4. TASK_STOPPED-终止态（不可中断）

    Process execution has been stopped; the process enters this state after receiving a SIGSTOP, SIGTSTP, SIGTTIN, or SIGTTOU signal.

5. TASK_TRACED-跟踪态

    Process execution has been stopped by a debugger. When a process is being monitored by another (such as when a debugger executes a ptrace() system call to monitor a test program), each signal may put the process in the TASK_TRACED state.

Two additional states of the process can be stored both in the `state` field and in the `exit_state` field of the process descriptor; as the field name suggests, a process reaches one of these two states only when its execution is terminated:

其实，进程还有2个状态，分别存储在进程描述符的state和exit_state两个成员中；它们都是进程终止后的状态：

1. EXIT_ZOMBIE

    Process execution is terminated, but the parent process has not yet issued a `wait4()` or `waitpid()` system call to return information about the dead process.* Before the wait()-like call is issued, the kernel cannot discard the data contained in the dead process descriptor because the parent might need it. (See the section “Process Removal” near the end of this chapter.)

    僵尸进程标志。进程执行中止，但是父进程还没有发起wait4()或waitpid()系统调用返回关于这个死亡进程的信息。在收到信息之前，内核不能抛弃这个死亡进程描述符中的数据，因为父进程可能还需要。（参见本章最后的[移除进程](#3.5.2)一节）

2. EXIT_DEAD

    The final state: the process is being removed by the system because the parent process has just issued a `wait4()` or `waitpid()` system call for it. Changing its state from EXIT_ZOMBIE to EXIT_DEAD avoids race conditions due to other threads of execution that execute wait()-like calls on the same process (see Chapter 5).

    死亡进程标志。父进程已经发出wait4()或waitpid()系统调用，进程将要被移除。将进程的状态从EXIT_ZOMBIE更改为EXIT_DEAD可以避免竞态条件，因为其他执行的线程也可能给相同的线程发送wait()类似的调用。（见[第5章](#5)）

The value of the `state` field is usually set with a simple assignment. For instance:

改变state的值非常简单，如下所示：

    p->state = TASK_RUNNING;

The kernel also uses the `set_task_state` and `set_current_state` macros: they set the state of a specified process and of the process currently executed, respectively. Moreover, these macros ensure that the assignment operation is not mixed with other instructions by the compiler or the CPU control unit. Mixing the instruction order may sometimes lead to catastrophic results (see Chapter 5).

另外，内核还提供了两个宏set_task_state和set_current_state：分别设定一个指定的进程和当前正在执行的进程。更重要的是，这些宏保证编译器或CPU控制单元，不会将赋值操作和其它指令混合。有时候，混合的指令序会导致灾难性的后果（见[第5章](#5)）。

<h3 id="3.2.2">3.2.2 识别进程</h3>

As a general rule, each execution context that can be independently scheduled must have its own process descriptor; therefore, even lightweight processes, which share a large portion of their kernel data structures, have their own `task_struct` structures.

作为通用规则，每一个可以被独立调度的执行上下文都必须具有自己的进程描述符。因此，即使共享大部分的内核数据结构的轻进程，也都有自己的task_struct结构。

The strict one-to-one correspondence between the process and process descriptor makes the 32-bit address† of the `task_struct` structure a useful means for the kernel to identify processes. These addresses are referred to as process descriptor pointers. Most of the references to processes that the kernel makes are through process descriptor pointers.

因为进程和进程描述符之间严格遵循一对一原则，所以，可以使用task_struct结构体的32位地址作为标识进程的有用手段。这些地址被称为进程描述符指针。内核对进程的大多数引用都是通过进程描述符指针实现的。

On the other hand, Unix-like operating systems allow users to identify processes by means of a number called the Process ID (or PID), which is stored in the `pid` field of the process descriptor. PIDs are numbered sequentially: the PID of a newly created process is normally the PID of the previously created process increased by one. Of course, there is an upper limit on the PID values; when the kernel reaches such limit, it must start recycling the lower, unused PIDs. By default, the maximum PID number is 32,767 (PID_MAX_DEFAULT - 1); the system administrator may reduce this limit by writing a smaller value into the `/proc/sys/kernel/pid_max` file (`/proc` is the mount point of a special filesystem, see the section “Special Filesystems” in Chapter 12). In 64-bit architectures, the system administrator can enlarge the maximum PID number up to 4,194,303.

另一方面，类Unix操作系统提供了进程ID（PID）作为标识进程的一种手段，其存储在进程描述符的pid成员中。PID按顺序编号：新创建的进程ID通常是在前一个PID的基础上加1得到的结果。当然了，PID值有上限，当内核达到这个上限，它会回滚，重用未使用的PID号。默认情况下，PID的最大值是32767（PID_MAX_DEFAULT-1）；系统管理员可以减小这个值，方法就是写一个较小的值到/proc/sys/kernel/pid_max文件中（/proc是一个特殊文件系统挂载点，可以查看[第12章的特殊文件系统](12.3.1)一节）；在64位系统中，可以把这个值扩大到4194303。

When recycling PID numbers, the kernel must manage a `pidmap_array` bitmap that denotes which are the PIDs currently assigned and which are the free ones. Because a page frame contains 32,768 bits, in 32-bit architectures the `pidmap_array` bitmap is stored in a single page. In 64-bit architectures, however, additional pages can be added to the bitmap when the kernel assigns a PID number too large for the current bitmap size. These pages are never released.

为了重新回收利用PID号，内核必须知道哪些PID被使用，哪些是空闲的，它是通过一个pidmap_array的位图结构进行管理的。因为一个页帧刚好包含32768位，所以在32位架构里，pidmap_array位图被存储在一个单页中。在64位架构中，会添加额外的页到位图中，这样就可以保存更多的PID号。这些页永远不会被释放。

Linux associates a different PID with each process or lightweight process in the system. (As we shall see later in this chapter, there is a tiny exception on multiprocessor systems.) This approach allows the maximum flexibility, because every execution context in the system can be uniquely identified.

Linux为每一个进程或轻进程关联一个不同的PID（在本章的后面，我们可以多处理器系统中有细微的差异）。这种方法带来了最大程度的灵活性，因为系统中每一个执行上下文都可以被唯一标识。

On the other hand, Unix programmers expect threads in the same group to have a common PID. For instance, it should be possible to a send a signal specifying a PID that affects all threads in the group. In fact, the POSIX 1003.1c standard states that all threads of a multithreaded application must have the same PID.

另一方面，Unix编程者可能期望一组线程具有共同的PID。比如，它可能发送信号给指定的PID，而这个PID能够影响组内所有的线程。事实上，POSIX 1003.1c标准声明，多线程程序中的所有线程具有相同的PID。

To comply with this standard, Linux makes use of thread groups. The identifier shared by the threads is the PID of the thread group leader, that is, the PID of the first lightweight process in the group; it is stored in the tgid field of the process descriptors. The getpid() system call returns the value of tgid relative to the current process instead of the value of pid, so all the threads of a multithreaded application share the same identifier. Most processes belong to a thread group consisting of a single member; as thread group leaders, they have the tgid field equal to the pid field, thus the getpid() system call works as usual for this kind of process.

为了遵循这个标准，Linux使用了线程组的概念。所有线程共享的标识符是线程组组长的PID，也就是组内第一个轻进程的PID。它被存储在进程描述符tgid成员中。getpid()系统调用返回当前进程的tgid值而不是pid的值，所以，多线程程序中的所有线程共享同一个标识符。大部分进程属于单个线程构成的线程组；作为线程组的组长，它们的tgid值等于pid值，因而， getpid()系统调用对于这种进程可以正常工作。

Later, we’ll show you how it is possible to derive a true process descriptor pointer efficiently from its respective PID. Efficiency is important because many system calls such as kill() use the PID to denote the affected process.

稍后，我们将展示如何从各自的PID中有效地派生出一个真正的进程描述符。效率非常重要，因为许多系统调用，如kill()都会使用PID通知受影响的进程。

> <font color="blue">读书笔记：
>
> 如何标识进程：
>
> 1. 进程的表示方法有两种：task_struct结构体的地址和进程ID（PID）
> 2. 为了兼容POSIX，Linux提出了"线程组"的概念，同一个组内的轻进程都具有自己的task_struct结构体，
> 但是这个组的第一个线程作为组长，获得和进程ID相同的tgid，这样对于系统调用getid()来说，线程组内的
> 所有线程就具有相同的PID了。
>
> </font>

<h4 id="3.2.2.1">3.2.2.1 处理进程描述符</h4>

Processes are dynamic entities whose lifetimes range from a few milliseconds to months. Thus, the kernel must be able to handle many processes at the same time, and process descriptors are stored in dynamic memory rather than in the memory area permanently assigned to the kernel. For each process, Linux packs two different data structures in a single per-process memory area: a small data structure linked to the process descriptor, namely the `thread_info` structure, and the Kernel Mode process stack. The length of this memory area is usually 8,192 bytes (two page frames). For reasons of efficiency the kernel stores the 8-KB memory area in two consecutive page frames with the first page frame aligned to a multiple of 2^13; this may turn out to be a problem when little dynamic memory is available, because the free memory may become highly fragmented (see the section “The Buddy System Algorithm” in Chapter 8). Therefore, in the 80×86 architecture the kernel can be configured at compilation time so that the memory area including stack and `thread_info` structure spans a single page frame (4,096 bytes).

进程是一个动态的实体，它的生命周期从几毫秒到几个月。因而，内核必须能够同时处理多个进程，且进程描述符应该存储在动态内存区，而不是永久分配给内核的内存区。对于每个进程，Linux在它们的内存区打包了两个不同的数据结构：一个链接到进程描述符的小数据结构，又称为thread_info结构体；和内核态栈。这个区域的长度通常是8192字节（两个页帧）。因为效率的原因，内核分配连续的两个页帧，也就是第一个页帧的地址总是2^13的倍数。但是结果证明，分配小的动态内存时会带来问题，因为空闲的内存变得高度碎片化（参见第8章的[伙伴系统算法](#8.1.7)一节）。<font color="red">因此，在x86架构中，在编译时配置内核，使得包含栈和thread_info结构的内存区域跨越单个页帧（4096字节）。这句话的意思是说，可以从不是2^13的倍数地址分配这种内存吗？？？</font>

In the section “Segmentation in Linux” in Chapter 2, we learned that a process in Kernel Mode accesses a stack contained in the kernel data segment, which is different from the stack used by the process in User Mode. Because kernel control paths make little use of the stack, only a few thousand bytes of kernel stack are required. Therefore, 8 KB is ample space for the stack and the `thread_info` structure. However, when stack and `thread_info` structure are contained in a single page frame, the kernel uses a few additional stacks to avoid the overflows caused by deeply nested interrupts and exceptions (see Chapter 4).

在第二章的[Linux分段]()一节中，我们已经了解，内核态访问的包含在内核数据段中的栈，和用户态进程使用的栈是不同的。因为内核的控制路径很少使用栈，只需要几千字节的内核栈。因此，8KB的内存空间对于栈和thread_info结构来说足够了。但是，当栈和thread_info结构包含在单个页帧中时，内核使用一些额外的栈去避免由于深度嵌套中断和异常可能造成的溢出（参见[第4章](#4)）。

Figure 3-2 shows how the two data structures are stored in the 2-page (8 KB) memory area. The `thread_info` structure resides at the beginning of the memory area, and the stack grows downward from the end. The figure also shows that the `thread_info` structure and the `task_struct` structure are mutually linked by means of the fields `task` and `thread_info`, respectively.

图3-2展示了两个数据结构在8KB内存区是如何分布的。thread_info结构位于这块内存的开始，而栈是向下增长的。图中还展示了thread_info和task_struct两个结构之间的相互链接关系，分别通过task和thread_info两个成员实现。

<img id="Figure_3-2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_2.PNG">

图3-2 thread_info结构和进程内核栈在连续两个页帧的布局

The `esp` register is the CPU stack pointer, which is used to address the stack’s top location. On 80×86 systems, the stack starts at the end and grows toward the beginning of the memory area. Right after switching from User Mode to Kernel Mode, the kernel stack of a process is always empty, and therefore the `esp` register points to the byte immediately following the stack.

esp寄存器是CPU栈指针，用来寻址栈顶位置。x86系统中，栈位于高地址，向下增长。刚刚从用户态切换到内核态时，进程的内核栈总是空的，esp寄存器总是指向紧跟栈后的字节处。

The value of the `esp` is decreased as soon as data is written into the stack. Because the `thread_info` structure is 52 bytes long, the kernel stack can expand up to 8,140 bytes.

只要数据写入栈中，esp的值将会被减。因为thread_info结构体的大小是52个字节，内核栈能够扩展到8140个字节。

The C language allows the `thread_info` structure and the kernel stack of a process to be conveniently represented by means of the following union construct:

使用union联合类型结构实现thread_info和进程内核栈的结构：

    union thread_union {
        struct thread_info thread_info;
        unsigned long stack[2048];          /* 1024是4KB大小 */
    };

The `thread_info` structure shown in Figure 3-2 is stored starting at address 0x015fa000, and the stack is stored starting at address 0x015fc000. The value of the `esp` register points to the current top of the stack at 0x015fa878.

图3-2中，thread_info结构体的开始地址是0x015fa000，栈的开始地址是0x015fc000。esp寄存器的值是指向当前栈顶位置0x015fa878的指针。

The kernel uses the `alloc_thread_info` and `free_thread_info` macros to allocate and release the memory area storing a `thread_info` structure and a kernel stack.

内核使用alloc_thread_info和free_thread_info两个宏，为thread_info和栈，分配和释放内存区域。

<h4 id="3.2.2.2">3.2.2.2 标识当前进程</h4>

<s>
The close association between the `thread_info` structure and the Kernel Mode stack just described offers a key benefit in terms of efficiency: the kernel can easily obtain the address of the `thread_info` structure of the process currently running on a CPU from the value of the `esp` register. In fact, if the `thread_union` structure is 8 KB (2^13 bytes) long, the kernel masks out the 13 least significant bits of `esp` to obtain the base address of the `thread_info` structure; on the other hand, if the `thread_union` structure is 4 KB long, the kernel masks out the 12 least significant bits of `esp`. This is done by the `current_thread_info() `function, which produces assembly language instructions like the following:

正如刚刚描述的，thread_info结构和内核栈这种紧密的联系，从效率上讲，提供了一个重要的优点：内核可以轻松从esp指针中获取正在运行进程的thread_info结构的地址。如下面的代码所示，因为thread_union联合体的大小是8K，所以屏蔽掉栈esp寄存器的低13位，就能获取的thread_info的基地址。

</s>

> <font color="blue">注意：
>   在2007年的一次更新(2.6.22之后)中加入了stack内核栈指针, 替代了原来的thread_info的指针。
>
>  进程描述符task_struct结构中没有直接指向thread_info结构的指针，而是用一个void指针类型的成员表示，
>  然后通过类型转换来访问thread_info结构。
>
>  stack指向了内核栈的地址(其实也就是`thread_info`和`thread_union`的地址), 根据上面的描述，很容易
>  通过类型转换获取`thread_info`的地址。
>
>  相关代码在`include/linux/sched.h`中，如下所示。`task_thread_info` 用于通过`task_struct`来查找其`thread_info`的信息, 只需要一次指针类型转换即可。
>
>       #define task_thread_info(task)  ((struct thread_info *)(task)->stack)
>
>  根据上面的代码，`task_thread_info`实际上是一个宏，在这儿，task是task_struct类型的对象
>
> </font>

<s>

    movl $0xffffe000,%ecx /* or 0xfffff000 for 4KB stacks */
    andl %esp,%ecx
    movl %ecx,p

After executing these three instructions, p contains the `thread_info` structure pointer of the process running on the CPU that executes the instruction.

执行完这3条指令后，p就是thread_info结构体的指针。

Most often the kernel needs the address of the process descriptor rather than the address of the thread_info structure. To get the process descriptor pointer of the process currently running on a CPU, the kernel makes use of the current macro, which is essentially equivalent to `current_thread_info()->task` and produces assembly language instructions like the following:

大部分时候，内核需要的是进程描述符的地址而不是thread_info结构体的地址。为了获取当前正在运行的进程的进程描述符的指针，内核使用current宏，等价于`current_thread_info()->task`，产生的汇编指令跟上面的代码类似。

    movl $0xffffe000,%ecx /* or 0xfffff000 for 4KB stacks */
    andl %esp,%ecx
    movl (%ecx),p

Because the task field is at offset 0 in the thread_info structure, after executing these three instructions p contains the process descriptor pointer of the process running on the CPU.

因为task是thread_info结构体的第一个成员，偏移量为0，所以执行完这些指令之后，p就是进程描述符的指针。

The current macro often appears in kernel code as a prefix to fields of the process descriptor. For example, current->pid returns the process ID of the process currently running on the CPU.

Another advantage of storing the process descriptor with the stack emerges on multiprocessor systems: the correct current process for each hardware processor can be derived just by checking the stack, as shown previously. Earlier versions of Linux did not store the kernel stack and the process descriptor together. Instead, they were forced to introduce a global static variable called current to identify the process descriptor of the running process. On multiprocessor systems, it was necessary to define current as an array—one element for each available CPU.

</s>

<h4 id="3.2.2.3">3.2.2.3 双向链表</h4>

Before moving on and describing how the kernel keeps track of the various processes in the system, we would like to emphasize the role of special data structures that implement doubly linked lists.

在我们讨论内核如何追踪各种进程之前，我们先来看一个特殊的数据结构，`双向链表`。

For each list, a set of primitive operations must be implemented: initializing the list, inserting and deleting an element, scanning the list, and so on. It would be both a waste of programmers’ efforts and a waste of memory to replicate the primitive operations for each different list.

内核中肯定存在着大量表，方便管理数据。但是对于这些表的每一种操作必须实现为原子操作：初始化、插入/删除元素、遍历列表等等。而为这些表重复实现原子操作，不仅是编程者精力的一种浪费，也会更加消耗内存。

Therefore, the Linux kernel defines the `list_head` data structure, whose only fields `next` and `prev` represent the forward and back pointers of a generic doubly linked list element, respectively. It is important to note, however, that the pointers in a `list_head` field store the addresses of other `list_head` fields rather than the addresses of the whole data structures in which the `list_head` structure is included; see Figure 3-3 (a).

因此，Linux内核定义了`list_head`链表结构，只有2个成员`next`和`prev`指针，分别是链表结构前后元素的索引。但是，值得注意的是，`list_head`里的指针指向的是其它`list_head`结构的地址，而不是包含`list_head`结构的整个数据结构的地址，见图3-3（a）。

A new list is created by using the `LIST_HEAD`(list_name) macro. It declares a new variable named list_name of type `list_head`, which is a dummy first element that acts as a placeholder for the head of the new list, and initializes the prev and next fields of the `list_head` data structure so as to point to the list_name variable itself; see Figure 3-3 (b).

可以使用宏`LIST_HEAD(list_name)`创建新的`list_head`列表，名称是`list_name`。它的第一个元素是空的，其占位符的作用，并初始化`next`和`prev`指针指向自己，见图3-3（b）

<img id="Figure_3-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_3.PNG">

图3-3 内嵌着`list_head`数据结构的双向链表

Several functions and macros implement the primitives, including those shown in Table 3-1.

下表是一些操作函数和宏，实现是原子的。

<img id="Figure_3_1_T" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_1_T.PNG">

The Linux kernel 2.6 sports another kind of doubly linked list, which mainly differs from a `list_head` list because it is not circular; it is mainly used for hash tables, where space is important, and finding the the last element in constant time is not. The list head is stored in an hlist_head data structure, which is simply a pointer to the first element in the list (NULL if the list is empty). Each element is represented by an hlist_node data structure, which includes a pointer `next` to the next element, and a pointer `pprev` to the `next` field of the previous element. Because the list is not circular, the `pprev` field of the first element and the `next` field of the last element are set to NULL. The list can be handled by means of several helper functions and macros similar to those listed in Table 3-1: hlist_add_head(), hlist_del(), hlist_empty(), hlist_entry, hlist_for_each_entry, and so on.

从Linux 2.6版本开始，还支持另外一种双向链表，但它不是环形链表。这种链表有两种数据结构`hlist_head`和`hlist_node`，其主要用于哈希表（hash），空间很重要，但是查找最后一个元素的时间复杂度不是O(1)。`hlist_head`是指向链表表头的指针，如果链表是空的，则其成员first指针等于null。链表中每个元素使用`hlist_node`结构表示，`hlist_node`包含2个指针，指向下一个元素的`next`指针和指向前一个元素的`next`指针的`pprev`指针。由于不是环形结构，链表中第一个元素的`pprev`指针指向表头的`first`指针，最后一个元素的`next`指针设为null。这种hash链表结构也有类似于`list_head`那样的处理函数和宏：hlist_add_head()、hlist_del()、hlist_empty()、hlist_entry、hlist_for_each_entry等等。

<h4 id="3.2.2.4">3.2.2.4 进程列表</h4>

The first example of a doubly linked list we will examine is the process list, a list that links together all existing process descriptors. Each `task_struct` structure includes a tasks field of type `list_head` whose `prev` and `next` fields point, respectively, to the previous and to the next task_struct element.

我们将研究的第一个双向链表的结构是进程表，也就是把所有已存在的进程描述符串起来的链表。如果查看linux内核源代码，会发现每一个`task_struct`中都包含`struct list_head`类型的`tasks`成员，它的prev和next指针分别指向前一个和后一个task_struct结构。

The head of the process list is the init_task task_struct descriptor; it is the process descriptor of the so-called process 0 or swapper (see the section “Kernel Threads” later in this chapter). The tasks->prev field of init_task points to the tasks field of the process descriptor inserted last in the list.

该进程链表的头是一个叫`init_task`的进程描述符，它是所谓的进程0或swapper（参见本章后面的"[内核线程](#3.4.2)"一节）的进程描述符。`init_task`的`tasks->prev`指向最后插入该链表的进程描述符的`tasks`成员。

The SET_LINKS and REMOVE_LINKS macros are used to insert and to remove a process descriptor in the process list, respectively. These macros also take care of the parenthood relationship of the process (see the section “How Processes Are Organized” later in this chapter).

内核提供了简单操作进程列表的专用宏`SET_LINKS`和`REMOVE_LINKS`，分别用来插入和删除一个进程描述符。这些宏还负责处理进程的父子关系（参见本章后面的"[进程是如何组织的](#3.2.4)"一节）。

Another useful macro, called for_each_process, scans the whole process list. It is defined as:

其它有用的宏，比如`for_each_process`，遍历整个进程列表。定义如下：

    #define for_each_process(p) \
        for (p=&init_task; (p=list_entry((p)->tasks.next, \
                                struct task_struct, tasks) \
                                ) != &init_task; )

The macro is the loop control statement after which the kernel programmer supplies the loop. Notice how the init_task process descriptor just plays the role of list header. The macro starts by moving past init_task to the next task and continues until it reaches init_task again (thanks to the circularity of the list). At each iteration, the variable passed as the argument of the macro contains the address of the currently scanned process descriptor, as returned by the list_entry macro.

宏展开后，就是一个for循环。可以注意的是，`init_task`进程描述符仅仅是扮演了一个链表表头的作用。该宏就是把`init_task`传递给下一个任务，直到再次转到`init_task`（因为是环形链表）。每一次的迭代过程中，通过宏传递进来的参数`p`都会被`list_entry`宏的值赋值，等于当前扫描到的进程描述符的地址。

> <font color="blue">注意：
>
> 其实，理解了双向链表的操作，对于这一部分的理解就是水到渠成的事了。
>
> </font>

<h4 id="3.2.2.5">3.2.2.5 运行态进程列表</h4>

When looking for a new process to run on a CPU, the kernel has to consider only the runnable processes (that is, the processes in the TASK_RUNNING state).

当我们要查找将要在CPU上运行的进程时，必须只考虑在运行态的进程（也就是说处于`TASK_RUNNING`状态的进程）

Earlier Linux versions put all runnable processes in the same list called runqueue. Because it would be too costly to maintain the list ordered according to process priorities, the earlier schedulers were compelled to scan the whole list in order to select the “best” runnable process.

早期的linux内核把所有可运行的进程存放在一个称为`runqueue`的链表中。因为按照进程优先级分别组织链表的话，维护成本太高，早期的内核调度器被迫遍历整个链表，以便选出最好的可运行进程。

Linux 2.6 implements the runqueue differently. The aim is to allow the scheduler to select the best runnable process in constant time, independently of the number of runnable processes. We’ll defer to Chapter 7 a detailed description of this new kind of runqueue, and we’ll provide here only some basic information.

从Linux 2.6版本之后的内核实现不同的`runqueue`。目的就是可以在恒定时间内选出最佳的可运行进程，而无关于可运行进程的数量。详细的阐述会在[第7章](#7)介绍，这儿只是提供基本的概念。

The trick used to achieve the scheduler speedup consists of splitting the runqueue in many lists of runnable processes, one list per process priority. Each task_struct descriptor includes a run_list field of type list_head. If the process priority is equal to k (a value ranging between 0 and 139), the run_list field links the process descriptor into the list of runnable processes having priority k. Furthermore, on a multiprocessor system, each CPU has its own runqueue, that is, its own set of lists of processes. This is a classic example of making a data structures more complex to improve performance: to make scheduler operations more efficient, the runqueue list has been split into 140 different lists!

让调度器加速的小技巧就是将可运行的进程按照优先级分割成多个链表，每个链表对应一个优先级。每个`task_struct`结构中包含一个`list_head`类型的`run_list`成员。假设进程优先级等于k（范围0-139），run_list就相当于把具有优先级k的可运行进程连接起来组成一个链表。进一步，如果是多核系统，每个CPU都有自己的runqueue，也就是自己的一组进程链表。这是增加数据结构的复杂度，以提高性能的经典案例：为了是调度器更加高效，将runqueue分割成140个不同的链表！

As we’ll see, the kernel must preserve a lot of data for every runqueue in the system; however, the main data structures of a runqueue are the lists of process descriptors belonging to the runqueue; all these lists are implemented by a single `prio_array_t` data structure, whose fields are shown in Table 3-2.

正如我们看到的，内核必须为系统中的每一个runqueue保存大量的数据。但是，runqueue中主要的数据结构还是属于它的进程描述符的链表；这些链表是通过一个数组结构prio_array_t实现的。

表 3-2 prio_array_t数据结构的各个成员

<img id="Figure_3_2_T" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_2_T.PNG">

The enqueue_task(p,array) function inserts a process descriptor into a runqueue list; its code is essentially equivalent to:

    list_add_tail(&p->run_list, &array->queue[p->prio]);
    __set_bit(p->prio, array->bitmap);
    array->nr_active++;
    p->array = array;

The prio field of the process descriptor stores the dynamic priority of the process, while the array field is a pointer to the `prio_array_t` data structure of its current runqueue. Similarly, the `dequeue_task(p,array)` function removes a process descriptor from a runqueue list.

<font color="blue">补充：

这一部分的内容，从原理上来说，新旧内核类似的。但是，在Linux4.4.23版本中，任务优先级已经被分为2组，0-99为实时调度进程优先级；100-139为普通进程优先级（也就是用户态进程）。实时进程的优先级当然要高于普通进程的优先级。

根据这种划分，内核的调度方法也分为了2种：完全公平调度(cfs)和实时调度(rt)。因为它们的调度方法不同，内核为了调度器的统一处理，使用类似于面向对象的概念，抽象出一个调度类(struct sched_class)，声明了统一的回调函数接口。具体的接口则分别由公平调度实例(fair_sched_class)和实时调度实例(rt_sched_class)实现。

数据结构`prio_array_t`已经改成了

    struct rt_prio_array {
        DECLARE_BITMAP(bitmap, MAX_RT_PRIO+1); /* include 1 bit for delimiter */
        struct list_head queue[MAX_RT_PRIO];
    };

对于向runqueue队列中插入一个进程描述符的函数`enqueue_task(p,array)`也已经修改为

    void (*enqueue_task) (struct rq *rq, struct task_struct *p, int flags);

具体的实现由各个实例实现。

</font>

<h3 id="3.2.3">3.2.3 进程之间关系</h3>

我们已经或多或少知道，进程具有父子关系，不仅如此，还有兄弟关系。所以，进程描述符中必须有几个成员是记录这种关系的（P是创建的进程），具体可以参考下表。进程0和1是由内核创建的，后面我们会看到，进程1（init）是所有其它进程的祖先。

表3-3 进程中用来表述父子、兄弟关系的成员

| 成员名称 | 描述 |
| -------- | ---- |
| real_parent | 指向创建P，如果不存在指向进程1。（比如，在shell中启动了一个后台进程，然后退出shell，则后台进程的父进程就是init）。|
| parent      | 指向P的当前父进程。（当子进程结束时，必须发送信号通知的那个进程）；通常等于real_parent。偶尔会有不同的时候，比如当另一个进程发送ptrace()系统调用去监控进程P时（具体可以查看第20章的[执行跟踪](#20.1.5)）。|
| children    | 包含P创建的所有子进程的列表的表头。|
| sibling     | 包含指向兄弟关系的进程链表中的下一个元素和前一个元素的指针，这些进程的父进程都是P。|

图3-4 阐述了进程的父子、兄弟关系。进程P0以此创建了P1、P2和P3。继而，进程P3创建了P4。

更进一步讲，进程之间还有其它关系：一个进程可以是进程组的组长或者login会话的组长（参见第一章的进程管理），还可以是线程组的组长（参见本章前面的[标识进程](#3.2.2)），还可以追踪其它进程的执行（参见第20章的[执行跟踪](#20.1.5)）。表3-4列出了描述进程P和其它进程之间关系的数据成员。

表3-4 进程描述符中建立非父子兄弟关系的数据成员

| 成员名称 | 描述 |
| -------- | ---- |
| group_leader    | 进程P的进程组组长的进程描述符 |
| signal->pgrp    | 进程P的进程组组长的PID |
| tgid            | 进程P的线程组组长的PID |
| signal->session | 进程P的login会话组组长的PID |
| ptrace_children | 正在被调试器追踪的进程P的所有子进程的列表的表头 |
| ptrace_list     | 包含指向正在被调试器追踪所有进程的real_parent列表中的元素的指针，分别指向下一个或者前一个元素。当追踪进程P时使用。|

<h4 id="3.2.3.1">3.2.3.1 PID哈希表和链表</h4>

在多种情况下，内核必须能够根据PID得到进程描述符的指针。比如，kill()系统调用，假设进程P1想要发送给进程P2一个信号，它指定P2进程的PID作为参数调用kill()。内核能够根据PID溯源到进程描述符的指针，然后从P2的进程描述符记录待处理（也就是挂起-pending）信号的数据结构的指针。

顺序扫描进程列表，逐个检查进程描述符的pid成员，这当然是可行的，但却不是最有效的。为了加速查找过程，内核引入了4个哈希表。为什么是4个哈希表呢？这当然是因为进程描述符的PID有4类，如表3-5所示，每一种PID对应一个哈希表。

表3-5 进程描述符中的四种哈希表以及对应的数据结构

| 哈希表类型 | 成员名称 | 描述 |
| ---------- | -------- | ---- |
| PIDTYPE_PID | pid     | 进程PID |
| PIDTYPE_TGID| tgid    | 线程组组长的PID |
| PIDTYPE_PGID| pgrp    | 进程组组长的PID |
| PIDTYPE_SID | session | session组长的PID |

4个哈希表都是在内核初始化阶段动态分配，它们的地址存储在pid_hash的数组中。哈希表的大小依赖于RAM的数量。比如，系统的RAM位512M，每一个哈希表被存储在4个页帧中，大约是2048项（4*4*1024/4/2=2048）。

使用pid_hashfn宏将PID值转换成哈希表的索引值，其定义为：

    #define pid_hashfn(x) hash_long((unsigned long) x, pidhash_shift)

其中，pidhash_shift参数是哈希表索引所占的位数，在我们的例子中需要2048项，也就是2^11，所以pidhash_shift=11。hash_long()函数被许多哈希函数使用；对于32位架构等于：

    unsigned long hash_long(unsigned long val, unsigned int bits)
    {
        unsigned long hash = val * 0x9e370001UL;
        return hash >> (32 - bits);
    }

因为在我们的示例中，pidhash_shift等于11，所以pid_hashfn宏产生的值永远落在0-2047这个区间内。

> 魔幻常数
>
> 在上面的代码中，你肯定会想0x9e370001UL这个值是如何得来的。hash函数通常是索引值乘以一个合适的大数，因此，结果会溢出，将其余下的值存入32位的变量，这个变量可以看做是求模运算的结果。Knuth认为，选取一段数值范围中黄金比例的质数为这个大数是最合适的。所以，0-2^23之间的黄金比例附近最合适的质数，我们选取0x9e370001UL，它还可以方便地被加、减法，还有移位实现。因为它等于`2^31 + 2^29 - 2^25 + 2^22 - 2^19 – 2^16 + 1`。

正如计算机科学课程中所讲的，哈希函数是无法保证PID和哈希表索引之间的一对一关系的。两个PID对应哈希表中的同一个索引，就成为 *冲突*。

为了解决这个冲突问题，Linux决定使用一个双向链表存储这些冲突的PID，把这个双向链表的表头存入哈希表中，通过这种方法，完美地解决了这个冲突。图3-5，展示了一个带有两个双向链表的PID哈希表：PID为2890和29384的进程都被存入到哈希表的第200个元素处的双向链表中，而PID为29385的进程被装入到了哈希表的第1466个元素里。

这种带链表的哈希表优于从PID到表索引的线性转换，这是因为，对于任何给定的32位系统中，进程的数量通常远少于32768个（最大允许进程数）。如果定义一个32768项的表，这对于内存空间都是一种浪费，因为大部分项根本没用。

当然了，实际用在PID哈希表中的数据结构非常复杂，因为它们要跟踪进程之间的各种关系。比如，假设内核需要检索属于某个线程组的所有进程，也就是所有的进程其tgid成员都等于某个相同的进程ID。如果根据这个给定的线程组ID，也就是线程组组长的PID，遍历整个PID哈希表，仅是返回一个进程描述符，也就是线程组组长的进程描述符。那为了快速检索整个线程组的所有进程，内核就需要为每个线程组维护一个进程表。对于寻找一个给定的login会话组或者进程组中的所有进程，道理是一样的。

<img id="Figure_3_5" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_5.PNG">

Figure 3-5. 一个简单的PID哈希表和链表

PID哈希表的数据结构就解决了这所有的问题，因为它允许给包含在哈希表中的任何一个PID定义一个进程表。核心数据结构就是在进程描述符的pids成员中嵌入4个pid成员结构，组成一个数组，数组每个成员对应一种哈希表。每个pid成员的数据成员如表3-6所示：

表3-6 `pid`数据结构的各个成员

| 类型              | 名称      | 描述 |
| ----------------- | --------- | ---- |
| int               | nr        | PID值 |
| struct hlist_node | pid_chain | 用于hash表中的链表结构中，用于指向下一个和前一个元素 |
| struct list_head  | pid_list  | 每个PID表的头 |

我们用下面的图3-6，展示一个类型为PIDTYPE_TGID的哈希表。pid_hash数组的第二项存储着哈希表的地址，也就是由hlist_head结构的组成的一个数组，这个数组存储着链表的表头。开始于哈希表第71项的链表，存储着2个进程描述符，其PID分别是246和4351，使用双向链表表示。这些PID值存储在进程描述符的pid结构成员的nr成员中（顺便说一下，因为线程组的ID和它的组长的PID相同，所以这些值也是线程组的ID）。接下来，我们看一个线程组4351，它对应着一组链表：链表的头被存储在进程描述符的pid_list结构成员中，通过pid_list结构的next和prev指针分别指向该链表的下一个和前一个元素。通过这种方式，我们就实现了检索某个线程组中的所有进程。其它3类哈希表的检索与此类似，就不再一一展开了。

图3-6展示了一个基于PIDTYPE_TGID类型的哈希表的示例。pid_hash数组中的第2项存储着该哈希表的地址，也就是hlist_head类型的数组结构，用于保存具有相同tpid值的链表的表头。tgid哈希表的第71项出来的分链表中，有PID分别为246和4351的进程描述符。

<img id="Figure_3_6" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_6.PNG">

图3-6 PID哈希表

下面的函数和宏用来处理PID哈希表：

* do_each_task_pid(nr, type, task)
* while_each_task_pid(nr, type, task)

    遍历与nr指定的PID相关的每一个PID列表，type是哈希表类型，task指向当前刚被遍历过的进程描述符。

* find_task_by_pid_type(type, nr)

    type类型的哈希表中查找PID等于nr的进程。函数返回匹配的进程描述符指针，如果不匹配返回NULL。

* find_task_by_pid(nr)

    作用等同于find_task_by_pid_type(PIDTYPE_PID, nr)。

* attach_pid(task, type, nr)

    往类型为type的PID哈希表中插入进程描述符，task指向要插入的进程描述符，nr是PID哈希表的索引。如果已经有一个PID等于nr的进程描述符在哈希表中了，则将task插入到该PID对应的链表中。

* detach_pid(task, type)

    从类型为type的PID列表中删除task指向的进程描述符。执行完删除操作后，如果PID链表没有变为空，则函数执行中止；否则，该函数还会从类型为type的哈希表中删除对应的进程描述符。

* next_thread(task)

    返回类型为PIDTYPE_TGID的哈希表中紧跟在task之后的轻进程的进程描述符地址。因为链表是环形的，如果是作用到常规进程上，该宏返回进程本身的描述符地址。


<h3 id="3.2.4">3.2.4 如何组织进程</h3>

使用runqueue列表组织所有运行态（TASK_RUNNING）的进程。对于其它状态下的进程，根据类型不同做不同的处理。下面是处理的基本原则：

* 处于TASK_STOPPED、EXIT_ZOMBIE或EXIT_DEAD等终止态的进程不需要使用特定列表进行管理。因为终止态的进程只能通过PID或者通过特定父进程的子进程链表进行查找。

* 而处于TASK_INTERRUPTIBLE或TASK_UNINTERRUPTIBLE状态的进程被细分为许多类，每一类对应一种event。对于这种情况，内核必须在进程描述符中提供一种数据结构，用于管理这类进程信息。我们称这些额外的数据结构为等待队列。

<h4 id="3.2.4.1">3.2.4.1 等待队列</h4>

等待队列在内核中有多种用途，特别是进行中断处理，进程同步和定时处理时，非常有用。在这儿，我们只讨论进程等待某些事件发生，比如硬盘操作终止，系统资源被释放，或timeout等事件。等待队列实现条件等待某些事件：进程等待某个事件，把自己暂时存入某个等待队列中，放弃对CPU的控制权；当等待的条件发生后，会被内核唤醒。

等待队列使用双向链表实现，包含指向进程描述符的指针。每个等待队列都有一个表头，数据类型为wait_queue_head_t，其定义如下：

    struct __wait_queue_head {
        spinlock_t lock;
        struct list_head task_list;
    };
    typedef struct __wait_queue_head wait_queue_head_t;

因为等待队列可能同时被中断处理或内核函数修改，必须对这个双向链表实行加锁处理，防止并发情况的出现，从而产生不可预知的后果（参见[第5章](#5)）。等待队列中，使用的是自旋锁lock，其类型是spinlock_t。而成员task_list是等待进程的链表的表头。

等待队列中的元素类型为wait_queue_t，其定义如下：

    struct __wait_queue {
        unsigned int        flags;
        struct task_struct  *task;
        wait_queue_func_t   func;
        struct list_head    task_list;
    };
    typedef struct __wait_queue wait_queue_t;

等待队列中的每一个元素表示一个正在休眠的进程，它正在等待某些事件的发生；其进程描述符的地址存储在task成员中。类型为list_head的成员task_list包含指向这个链表中其它元素的指针。

但是，唤醒等待队列中所有的休眠进程，多数时候可能不方便。比如，如果有多于2个进程正在等待某些资源，并进行独占式访问，这种情况下，应该只唤醒其中一个进程，允许其占用资源，而其它进程继续休眠。（这避免了 *惊群效应*的出现，所谓的惊群效应就是多个进程被唤醒，只为了竞争一个资源，而这个资源只能被其中一个访问，结果就是其余的进程再次被休眠。）

因此，这就产生了两种休眠进程：独占式进程（等待队列中该进程的flags相应标志位被置1）和非独占进程（标志位被清零），当事件发生时，独占式进程有选择地被唤醒，非独占式进程总是全被唤醒。等待每次只能分配给一个进程使用的资源的进程，就是独占式进程；否则，就是非独占式。比如，一组进程正在等待硬盘传输完成的事件，一旦传输完成，所有等待的进程立即被唤醒。接下来，我们看看wait_queue_t中的func成员是如何将等待队列中休眠的进程唤醒的。

<h4 id="3.2.4.2">3.2.4.2 处理等待队列</h4>

1. 初始化

    声明等待队列表头，使用宏DECLARE_WAIT_QUEUE_HEAD(name)，静态声明一个名称为name的变量并初始化lock和task_list成员。init_waitqueue_head()函数动态创建一个等待队列表头变量并完成初始化。

    函数init_waitqueue_entry(q,p)初始化等待队列中的一项，q的类型是wait_queue_t，代码内容如下：

        q->flags = 0;
        q->task = p;
        q->func = default_wake_function;

    非独占式进程P将会被default_wake_function()函数唤醒，该函数是try_to_wake_up()函数的封装函数，具体可以参考[第7章](#7)。

    另外，DEFINE_WAIT声明一个新的wait_queue_t变量，并使用当前正在运行的进程描述符初始化它，而func成员被初始化为autoremove_wake_function()唤醒函数。这个函数调用default_wake_function()唤醒正在休眠的地址，然后从等待队列中删除这个进程。最后，内核开发者可以init_waitqueue_func_entry()函数可以自定义一个唤醒函数。

2. 插入等待队列

    将进程插入等待队列中，可以使用的函数有：

    * add_wait_queue()

        将非独占进程插入到等待队列的第一个位置处。

    * add_wait_queue_exclusive()

        将独占进程插入到等待队列的最后位置处。

    * remove_wait_queue()

        从队列中移除一个进程。

    * waitqueue_active()

        检查等待队列是否为空

3. 休眠

    想要等待某个条件的进程，可以调用下面任何一个函数进入休眠状态：

    - sleep_on()

        sleep_on()函数直接操作当前进程：

            void sleep_on(wait_queue_head_t *wq)
            {
                wait_queue_t wait;
                init_waitqueue_entry(&wait, current);
                current->state = TASK_UNINTERRUPTIBLE;
                add_wait_queue(wq,&wait);               /* 指向等待队列的表头 */
                schedule();
                remove_wait_queue(wq, &wait);
            }

        函数设置当前进程的状态为TASK_UNINTERRUPTIBLE，并把它插入到指定的等待队列中。然后调用调度器，继续其它进程的执行。当休眠的进程被唤醒时，调度器继续sleep_on()函数的执行，执行`remove_wait_queue(wq, &wait);`这条语句，将进程从等待队列中删除。

    - interruptible_sleep_on()

        与sleep_on()函数相同，唯一的不同是，将进程的状态设为TASK_INTERRUPTIBLE，这表示进程可以被信号唤醒，比如中止信号等等。

    - sleep_on_timeout()
    - interruptible_sleep_on_timeout()

        这两个函数和前面的2个函数非常类似，只是允许调用者可以定义个时间间隔，如果定时时间到，则进程被内核唤醒。实现方式就是使用schedule_timeout()函数代替schedule()函数。

    - prepare_to_wait()
    - prepare_to_wait_exclusive()
    - finish_wait()

        这几个函数是在内核2.6版本中引入的，提供了一些其它休眠进程的方法。它们的使用方式，通常如下：

            DEFINE_WAIT(wait);
            prepare_to_wait_exclusive(&wq, &wait, TASK_INTERRUPTIBLE);
                                        /* wq 等待队列的表头 */
            ...
            if (!condition)
                schedule();
            finish_wait(&wq, &wait);

        prepare_to_wait()和prepare_to_wait_exclusive()函数设置进程的状态为第三个参数传递进来的值，然后设置等待队列中的进程的独占标志，0-非独占，1-独占，然后插入到等待队列中，wq是等待队列的表头。

        只要进程被唤醒，继续执行finish_wait()函数，再次将进程的状态设为TASK_RUNNING状态，从等待队列中删除该进程。（这儿有个需要注意的问题就是，condition必须在schedule()之前变为true。

    - wait_event
    - wait_event_interruptible

        等待某个条件被验证，否则一直休眠。wait_event(wq,condition)的代码与下面的内容类似：

            DEFINE_WAIT(__wait);
            for (;;) {
                prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);
                if (condition)
                    break;
                schedule();
            }
            finish_wait(&wq, &__wait);

    我们发现，当需要测试某个条件，判断条件不能被验证时，将进程休眠且该操作必须是原子的，这种环境下，是无法使用类sleep_on()函数的。这种情况下是会产生竞态条件的，这种使用是不鼓励的。更主要的是，为了往等待队列中插入一个独占进程，内核必须使用prepare_to_wait_exclusive()或者直接调用add_wait_queue_exclusive()。其它的插入等待队列的函数都是非独占的。最后，使用DEFINE_WAIT或者finish_wait()将唤醒的进程从等待队列中删除。

4. 唤醒

    内核唤醒等待队列中的进程后，将它们设为运行态。可调用的方法有wake_up、wake_up_nr、wake_up_all、wake_up_interruptible、wake_up_interruptible_nr、wake_up_interruptible_all、wake_up_interruptible_sync、和wake_up_locked。基本上，从名字就可以看出它们的用处：

    * 所有的宏都可唤醒处于TASK_INTERRUPTIBLE状态的休眠进程；如果宏名称不包含interruptible的宏，也能唤醒TASK_UNINTERRUPTIBLE状态的进程。

    * 所有的宏都可以唤醒所要求的状态下的非独占进程

    * 包含nr字符串的宏，唤醒所要求的状态下的独占进程，nr就是指定PID

    * 不包含sync的宏会检查被唤醒进程的优先级是否高于当前正在运行的进程，如果有必要调用schedule()，将正在运行的进程休眠。包含sync的宏，不会执行优先级检查，这样的结果就是，高优先级的进程可能会有一定的延时。

    * wake_up_locked宏与wake_up相似，不同的是，它可以在wait_queue_head_t中的自旋锁已经被占用的情况下调用。

    比如，宏wake_up的代码大概是下面这样的：

        void wake_up(wait_queue_head_t *q)
        {
            struct list_head *tmp;
            wait_queue_t *curr;

            list_for_each(tmp, &q->task_list) {
                curr = list_entry(tmp, wait_queue_t, task_list);
                if (curr->func(curr, TASK_INTERRUPTIBLE|TASK_UNINTERRUPTIBLE,
                        0, NULL) && curr->flags)
                    break;
            }
        }

    遍历q指向的双向链表q->task_list中所有的进程。对于每一项，list_entry计算wait_queue_t类型的变量的地址。这个变量的func成员存储着唤醒函数的地址。如果进程被唤醒，且如果进程是独占的（curr->flags=1），则循环终止。因为所有的非独占进程都存储在链表的开头部分，所有的独占进程都在链表的尾端，该函数总是唤醒所有的非独占进程，然后唤醒一个独占进程，如果存在的话。

<h3 id="3.2.5">3.2.5 进程资源限制</h3>

每个进程都需要进行资源限制，避免把系统搞垮（比如对CPU的使用，硬盘空间的占用等等）。基于这个目的，Linux内核在每个进程的进程描述符中还应该包含资源限制的数据结构，Linux使用了一个数组成员，该数组成员的包含关系为current->signal->rlim，数组的定义如下所示：

    struct rlimit rlim[RLIM_NLIMITS];

其中，rlimit结构的定义为：

    struct rlimit {
        __kernel_ulong_t    rlim_cur;
        __kernel_ulong_t    rlim_max;
    };

__kernel_ulong_t等于无符号长整形。RLIM_NLIMITS的大小为16，也就是说，目前对进程资源的限制有16种，分别如下所示：

1. RLIMIT_AS

    进程空间的最大值，单位是字节。当进程使用malloc()或者相关函数扩大自己的地址空间时，就会检查这个值。

2. RLIMIT_CORE

    最大核心转储文件大小，单位是字节。当进程被中止时，内核会检查这个值，然后进程的当前目录下创建一个core文件。（最常见的情况就是，我们的程序有bug而崩溃的时候，会在该目录下产生一个core文件。）当然了，如果这个值的大小为0，是不会产生core文件的。

3. RLIMIT_CPU

    进程占用CPU的最大时间，单位是秒（S）。如果超过这个时间，内核会发送一个SIGXCPU信号，如果进程还是没有终止，再发送SIGKILL信号。

4. RLIMIT_DATA

    最大堆大小，单位是字节。内核在扩大进程的堆空间之前，检查这个值。

5. RLIMIT_FSIZE

    最大文件大小，单位是字节。如果进程尝试扩大文件超过这个值，内核发送一个SIGXFSZ信号。

6. RLIMIT_LOCKS

    文件锁的最大数量（目前不强制）。

7. RLIMIT_MEMLOCK

    非交换内存的最大值，单位是字节。当内核调用mlock()或mlockall()系统调用尝试给一个页帧加锁时会检查该值。

8. RLIMIT_MSGQUEUE

    POSIX消息队列的最大字节数。

9. RLIMIT_NOFILE

    打开的文件描述符最大数量。当新打开一个文件或复制文件描述符时，内核都会检查这个值。

10. RLIMIT_NPROC

    用户可以拥有的最大进程数量。

11. RLIMIT_RSS

    进程可以拥有的页帧数量，也就是物理帧的数量（目前不强制）。

12. RLIMIT_SIGPENDING

    进程挂起信号的最大数量。

13. RLIMIT_STACK

    最大栈空间，单位是字节。在扩展进程的用户态栈时，内核会检查这个值。

14. RLIMIT_NICE

    优先级的完美值。进程可通过setpriority()或nice()设置。

15. RLIMIT_RTPRIO

    最大实时优先级。进程可通过sched_setscheduler和sched_setparam设置。

16. RLIMIT_RTTIME

    实时任务的timeout，单位是uS。

结构体成员rlim_cur表示对当前进程的资源限制。比如`current->signal->rlim[RLIMIT_CPU].rlim_cur`是指当前正在运行的进程的CPU时间限制。

成员rlim_max表示资源限制允许的最大值。可以通过getrlimit()和setrlimit()系统调用进行设置，用户可以增加rlim_max的值到rlim_max。但是，超级用户（更准确地将，具有CAP_SYS_RESOURCE能力的用户）可以增加rlim_max的值，或者将rlim_cur设为超过rlim_max的值。

> 这就是为什么当我们的程序崩溃时，却发现没有core文件，这是因为系统默认是关闭的。所以需要调用命令
>
>       ulimit -c unlimited // 设置core文件大小为不限制大小
>
> 然后才能看到core文件的原因。

但是，我们查看源码的时候会发现，大部分的资源限制都被设为RLIM_INFINITY（0xffffffff），这意味对资源没有用户限制（当然了，本身还要受到硬件的限制：比如可用的RAM，硬盘实际空间等等）。这是因为我们想要保留软件设置的自由度，如果代码中直接写死对硬件资源的限制，软件操作的空间就会变小。

通过上面的方法，系统管理员可以更改对资源的限制。当用户登陆到系统时，内核创建一个超级用户拥有的进程，通过它调用setrlimit()减小rlim_max和rlim_cur的值；然后执行login shell，成为用户态进程（实际就是进程init）。用户新创建的进程继承它父进程的rlim数组内容，所以，用用也不能覆盖掉由超级用户赋值的限制值。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3.3">3.3 进程切换</h2>

进程切换，又称为任务切换、上下文切换、或者任务调度。下面我们就来研究Linux的进程切换。

<h3 id="3.3.1">3.3.1 硬件上下文</h3>

尽管每个进程都有自己的地址空间，但是所有的进程却共享CPU寄存器。所以，在恢复进程执行之前，内核必须保证该进程在挂起时的寄存器值重新加载到CPU的寄存器中。

这些需要加载到CPU寄存器中的值就成为硬件上下文。硬件上下文是进程执行上下文的一个子集，进程执行上下文包含进程执行所需要的所有信息。在Linux中，进程的硬件上下文一部分存储在进程描述符中，而其它部分存储在内核态的栈中。

在下面的描述中，我们假设，局部变量prev指向将要被替换掉的进程，而next指向将要替换的进程。因此，我们就可以说，进程切换就是保存prev进程的硬件上下文，然后加载next进程的硬件上下文。因为进程的切换非常频繁，所以最小化保存和加载硬件上下文内容的时间就显得非常重要了。

旧版本的linux利用x86架构提供的硬件支持，并通过远程调转指令（GNU-`ljump`；Intel-`jmp far`）进行进程切换，跳转到下一个进程的任务状态段（TSS）描述符。执行这条跳转指令的同时，CPU自动执行硬件上下文切换，保存旧的硬件上下文，加载新的硬件上下文。但是，linux2.6版本以后，通过软件进行进程切换，原因如下：

* 通过一连串的mov指令，一步步执行切换，可以更好地控制加载数据的合法性。由其是ds和es段寄存器中的值，有可能会被恶意用户篡改。如果使用远程跳转指令是无法进程数据检查的。

* 新旧方法所要求的时间是大致相同的。但是，优化硬件上下文的切换是不可能的，因为都是由CPU完成的，而Linux是使用软件代替硬件上下文切换的，所以有优化的空间，一遍提高执行时间。

进程切换只能发生在内核态。在进行进程切换之前，用户态进程使用的所有寄存器内容都已经包含在内核态的栈中了。这其中就包含指定用户态进程栈指针地址的ss和esp这对寄存器内容。

<h3 id="3.3.2">3.3.2 任务状态段-TSS</h3>

x86架构包含一个特殊的段寄存器，称为任务状态段（TSS），用来保存硬件上下文内容。尽管Linux不使用硬件上下文切换，但还是给每个不同CPU建立一个TSS。这么做，基于两个原因：

* 当x86架构的CPU从用户态到内核态时，会从TSS中获取内核态的栈地址（参见第4章的[中断和异常的硬件处理](#4.2.4)和第10章的[通过系统进入指令发送系统调用](#10.3.2)）

* 用户态进程想要访问I/O端口的时候，CPU需要访问存储在TSS中的I/O权限位，判断进程是否被允许访问这个I/O端口。那么，当用户态进程执行in或out指令时，I/O控制单元到底做了什么呢？

    1. 检查eflags寄存器中IOPL位（2位）。如果等于3，也就是超级用户权限，也就是进程对于这个I/O端口来说就是一个超级用户，那么，直接执行I/O指令。否则，继续执行检查。
    2. 访问tr寄存器，确定当前的TSS，以及正确的I/O访问权限。
    3. 它检查I/O端口对应的访问权限位。如果清零，指令被执行；否则，控制单元发出**常规保护**的异常。

内核中使用tss_struct结构体描述TSS。init_tss数组为系统中的每一个CPU包含一个tss_struct结构。每一次进程切换，内核更新TSS相关内容，使CPU控制单元能够安全地检索自己想要的信息。因而，TSS反映了当前运行在CPU上的进程的特权级别，但是当进程不运行的时候，无需维护这些信息。

每个TSS具有8个字节长度的任务状态段描述符（TSSD）。这个描述符包含一个32位的基地址，指向TSS的起始地址
以及20位的Limit域，表示页的大小。TSSD的S标志被清零，说明这是一个系统段（参见第2章的[段描述符](#2.2.2)）。

Type域设置为9或者11都可以，表明该段是一个TSS段即可。Intel最初的设计中，系统中的每个进程都应该引用自己的TSS：Type域的低第2个有效位称为Busy位，如果被设为1，进程正在CPU上执行；设为0，没有执行。在Linux的设计中，每个CPU就只有一个TSS，所以，Busy位总是设为1。换句话说，Linux中Type域一般为11。

创建的这些TSSD存储在全局描述符表（GDT）中，该表的基地址存储在CPU的gdtr寄存器中。每个CPU的tr寄存器包含对应TSS的TSSD选择器，还包含两个隐藏的、不可编程的域：TSSD的Base和Limit域。使用这种方法，CPU可以直接寻址TSS，而不必非得访问GDT中TSS的地址。

<h4 id="3.3.2.1">3.3.2.1 线程域</h4>

每当进程切换时，将要被替换掉的进程硬件上下文内容都应该被保存到某个地址。显然不能保存在TSS中，因为Linux为每个CPU就建立了一个TSS，而不是为每个进程建立TSS。

因而，进程描述符中添加了一个类型为thread_struct的结构，通过它，内核保存将要被切换出的进程的硬件上下文。后面我们会看到，该数据结构包含了大部分的CPU寄存器，除了通用目的寄存器，比如eax、ebx等，它们被存储在内核态的栈中。

<h3 id="3.3.3">3.3.3 执行进程切换</h3>

1. 进程切换的时机：

    * 中断处理程序中直接调用schedule()函数，实现进程调度。
    * 内核线程，是一个特殊的进程，只有内核态没有用户态。所以即可以主动调用schedule()函数进行调度，也可以被中断处理程序调用。
    * 内核态进程没法直接主动调度，因为schedule()是一个内核函数，不是系统调用。所以只能在中断处理程序进行调度。

2. 关键代码梳理

    * 首先，schedule()函数会调用`next = pick_next_task(rq, prev);`，所做的工作就是根据调度算法策略，选取要执行的下一个进程。

    * 其次，根据调度策略得到要执行的进程后，调用`context_switch(rq, prev, next);`，完成进程上下文切换。其中，最关键的`switch_to(prev,next, prev);`切换堆栈和寄存器的状态。

我们假设prev指向被切换掉的进程描述符，next指向将要执行的进程描述符。我们将会在第7章发现，prev和next正是schedule()函数的局部变量。

<h4 id="3.3.3.1">3.3.3.1 switch_to宏</h4>

进程硬件上下文的切换是由宏`switch_to`完成的。该宏的实现与硬件架构是息息相关的，要想理解它需要下一番功夫。下面是基于X86架构下的该宏实现的汇编代码：

    #define switch_to(prev, next, last)                             \
    do {                                                            \
        /*
         * 进程切换可能会改变所有的寄存器，所以我们通过未使用的输出变量显式地修改它们。
         * EAX和EBP没有被列出，是因为EBP是为当前进程访问显式地保存和恢复的寄存器，
         * 而EAX将会作为函数__switch_to()的返回值。
         */
        unsigned long ebx, ecx, edx, esi, edi;                      \
                                                                    \
        asm volatile("pushfl\n\t"               /* save    flags */ \
                 "pushl %%ebp\n\t"              /* save    EBP   */ \
                 "movl %%esp,%[prev_sp]\n\t"    /* save    ESP   */ \
                 "movl %[next_sp],%%esp\n\t"    /* restore ESP   */ \
                 "movl $1f,%[prev_ip]\n\t"      /* save    EIP   */ \
                 "pushl %[next_ip]\n\t"         /* restore EIP   */ \
                 __switch_canary                                    \
                 __retpoline_fill_return_buffer                     \
                 "jmp __switch_to\n"            /* regparm call  */ \
                 "1:\t"                                             \
                 "popl %%ebp\n\t"               /* restore EBP   */ \
                 "popfl\n"                      /* restore flags */ \
                                                                    \
                 /* 输出参数 */                                     \
                 : [prev_sp] "=m" (prev->thread.sp),                \
                   [prev_ip] "=m" (prev->thread.ip),                \
                   "=a" (last),                                     \
                                                                    \
                   /* 列出所有可能会修改的寄存器  */                \
                   "=b" (ebx), "=c" (ecx), "=d" (edx),              \
                   "=S" (esi), "=D" (edi)                           \
                                                                    \
                   __switch_canary_oparam                           \
                                                                    \
                   /* 输入参数 */                                   \
                 : [next_sp]  "m" (next->thread.sp),                \
                   [next_ip]  "m" (next->thread.ip),                \
                                                                    \
                   /* 为函数__switch_to()设置寄存器参数 */          \
                   [prev]     "a" (prev),                           \
                   [next]     "d" (next)                            \
                                                                    \
                   __switch_canary_iparam                           \
                                                                    \
                 : /* reloaded segment registers */                 \
                "memory");                                          \
    } while (0)

> 上面是一段GCC内嵌汇编代码，关于其详细的语法使用方法可以参考[GCC内嵌汇编使用手册](https://ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html)。


* 首先，该宏具有3个参数，`prev`、`next`和`last`
    * `prev`和`next`这2个参数很容易理解，分别指向新旧进程的描述符地址；
    * `last`，是一个输出参数，用来记录是从哪个进程切换来的。

* 为什么需要`last`参数呢？

    当进程切换涉及到3个进程的时候，3个进程分别假设为A、B、C。假设内核决定关掉A进程，激活B进程。在schedule函数中，prev指向A的描述符，而next指向B的描述符。只要switch_to宏使A失效，A的执行流就会冻结。后面，当内核想要重新激活A，必须关掉C进程，就要再执行一次switch_to宏，此时prev指向C，next指向A。当A进程想要继续执行之前的执行流时，会查找原先的内核态栈，发现prev等于A进程描述符，next等于B进程描述符。此时，调度器失去了对C进程的引用。保留这个引用非常有用，我们后面再讨论。

图3-7分别展示了进程A、B和C内核态栈的内容，及寄存器eax的值。还展示了last的值，随后被eax中的值覆盖。

<img id="Figure_3_7" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_3_7.PNG">

`switch_to`宏的处理过程如下：

1. 将新旧进程描述符存放到CPU寄存器中：

        movl prev, %eax
        movl next, %edx

2. 保存旧进程的内核态栈，比如`eflags`和`ebp`寄存器的内容。

        pushfl
        pushl %ebp

3. 保存旧进程栈指针`esp`到`prev->thread.esp`中

        movl %esp,484(%eax)

    操作数`484(%eax)`表明目的地址是寄存器`eax`中的地址加上`484`。

4. 将新进程的栈指针加载到`esp`寄存器中。

    新进程的栈指针位于`next->thread.esp`中。从现在起，内核在新进程的内核态栈上操作，所以，这条指令才是执行旧进程切换到新进程的开始。因为内核态栈的地址和进程描述符的地址紧密相关，那么改变内核栈意味着改变了当前的进程。

        movl 484(%edx), %esp

5. 保存标签`1`的地址->`prev->thread.eip`。

    标签`1`标记进程当前执行的指令。这条指令意味着，再恢复进程A执行的时候，就从标签`1`处的地址中的指令开始执行。

        movl $1f, 480(%eax)

6. 加载新进程的指令流。

        pushl 480(%edx)

    意义和第5步差不多，就是执行顺序相反。

7. 跳转到`__switch_to()`函数执行，是一个C函数。

        jmp __switch_to

8. 至此，进程A被进程B取代：开始执行B进程的指令。第一步应该是先弹出`eflags`和ebp寄存器的值。

        1:
            popl %ebp
            popfl

9. 拷贝eax寄存器的内容（第1步加载的）到last变量中。

        movl %eax, last

    也就是说，last记录了被取代的进程。

<h4 id="3.3.3.2">3.3.3.2 __switch_to()函数</h4>

实际上大部分的进程切换工作是由__switch_to()函数完成的，它的参数是prev_p和next_p，分别指向旧进程和新进程。这个函数和普通的函数有些差别，因为__switch_to()函数从eax和edx寄存器中获取prev_p和next_p这两个参数（在分析switch_to宏的时候已经讲过），而不是像普通函数那样，从栈中获取参数。为了强制函数从寄存器中获取参数，内核使用`__attribute__`和`regparm`进行声明。这是gcc编译器对C语言的一个非标准扩展。__switch_to()函数定义在`include/asm-i386/system.h`文件中：

    __switch_to(struct task_struct *prev_p,
            struct task_struct *next_p)
            __attribute__(regparm(3));

这个函数执行的内容：

1. 执行__unlazy_fpu()宏，保存旧进程的FPU、MMX和XMM寄存器

    __unlazy_fpu(prev_p);

2. 执行smp_processor_id()宏，获取正在执行代码的CPU的ID。从thread_info结构的cpu成员中获取。

3. 加载新进程的`next_p->thread.esp0`到当前CPU的TSS段中的esp0成员中。通过调用sysenter汇编指令从用户态切换到内核态引起的任何特权级别的改变都会导致将这个地址拷贝到esp寄存器中。

        init_tss[cpu].esp0 = next_p->thread.esp0;

4. 将新进程的线程本地存储（TLS）段加载到当前CPU的GDT中。3个段选择器存储在进程描述符的tls_array数组中。

        cpu_gdt_table[cpu][6] = next_p->thread.tls_array[0];
        cpu_gdt_table[cpu][7] = next_p->thread.tls_array[1];
        cpu_gdt_table[cpu][8] = next_p->thread.tls_array[2];

5. 存储fs和gs段寄存器的内容到旧进程的prev_p->thread.fs和prev_p->thread.gs中。汇编指令如下：

        movl %fs, 40(%esi)
        movl %gs, 44(%esi)

    寄存器esi指向prev_p->thread结构。gs寄存器用来存放TLS段的地址。fs寄存器实际上windows使用。

6. 加载新进程的fs或gs寄存器内容。数据来源是新进程的thread_struct描述符中对应的值。汇编语言如下：

        movl 40(%ebx),%fs
        movl 44(%ebx),%gs

    ebx寄存器指向next_p->thread结构。

7. 载入新进程的调式寄存器中的信息。

        if (next_p->thread.debugreg[7]){
            loaddebug(&next_p->thread, 0);
            loaddebug(&next_p->thread, 1);
            loaddebug(&next_p->thread, 2);
            loaddebug(&next_p->thread, 3);
            /* no 4 and 5 */
            loaddebug(&next_p->thread, 6);
            loaddebug(&next_p->thread, 7);
        }

8. 更新TSS中的I/O权限位（如果有必要的话）。也就是如果新旧进程对I/O访问有自己特殊的要求的话就需要更改。

        if (prev_p->thread.io_bitmap_ptr || next_p->thread.io_bitmap_ptr)
            handle_io_bitmap(&next_p->thread, &init_tss[cpu]);

9. `__switch_to()`函数结束。

        return prev_p;

    相应的汇编语言就是：

        movl %edi,%eax
        ret

    因为switch_to总是假设eax寄存器保存旧进程的进程描述符的地址。所以，这里把prev_p变量再次写入到eax寄存器中。

    ret指令把栈上要返回的地址写入到eip寄存器中。其实，栈上的返回地址就是标签为`1`处的指令地址，这是由switch_to压栈的。如果新进程从来没挂起过，因为是第一次执行，然后就会跳转到ret_from_fork()函数返回的起始地址处（这部分等讲进程的创建时再细说）。至此，完成了进程的切换。

<h3 id="3.3.4">3.3.4 保存和加载FPU、MMX和XMM寄存器</h3>

从英特尔80486DX开始，算术浮点单元（FPU）就已经被集成到CPU中了。但是之所以还继续使用数学协处理器，是因为以前使用专用芯片进行浮点运算，所以这算是旧习惯的沿用吧。为了与旧CPU架构模型兼容，指令的使用方式与整数运算一样，只是使用了转义指令，也就是在原有的指令基础上加上前缀，组成新的指令，这些前缀的范围是0xd8-0xdf。使用这些指令可以操作CPU中的浮点寄存器。很显然，使用这些浮点运算指令的进程在进程切换的时候，需要保存属于它的硬件上下文中的浮点寄存器内容。

随后的奔腾系列处理器，因特尔引入了一组新的汇编指令。它们被称为MMX指令，用来支持多媒体应用的加速执行。MMX指令也是作用于FPU单元的浮点寄存器。这样的设计缺点是，内核开发者无法混合使用转义浮点指令和MMX指令；优点是内核开发者可以使用相同的进程切换代码来保存浮点单元和MMX的状态。

MMX指令之所以能够加速多媒体应用的执行，是因为它们在处理器中专门引入了单指令多数据（SIMD）指令流水线。奔腾III扩展了SIMD指令：引入了SSE扩展（单指令多数据流扩展），包含8个128位的寄存器，称为XMM寄存器，通过它们可以大大增加浮点数的处理。这些寄存器是独立的，和FPU和MMX寄存器没有重叠，所以SSE扩展和FPU/MMX指令可以混合使用。奔腾4又又引入了新的扩展：SSE2扩展，是在SSE基础上的扩展，支持更高精度的浮点数。SSE2扩展和SSE扩展使用相同的XMM寄存器。

X86微处理器不会自动在TSS中保存FPU、MMX和XMM寄存器。但是，从硬件上，支持内核只保存所需要的寄存器。具体方法就是在cr0寄存器中包含一个TS（任务切换）标志，标志设置的时机如下所示：

* 每次执行硬件上下文切换，TS标志被置。
* 每次执行ESCAPE、MMX、SSE或SSE2指令，同时TS标志被置，则控制单元就会发出`Device not available`的异常。

从上面可以看出，只有执行浮点运算的时候才需要保存FPU、MMX和XMM相关寄存器。假设进程A正在使用协处理器，当进程A切换到进程B的时候，内核设置TS标志，且把浮点寄存器保存到进程A的任务状态段（TSS）中。如果进程B没有使用协处理器，内核不需要恢复浮点寄存器的内容。但只要进程B想要执行浮点运算或多媒体指令，CPU就会发出`Device not available`异常，这个异常对应的处理程序就会把浮点寄存器中的值加载到进程B的TSS段中。

Linux内核是使用什么数据结构表示FPU、MMX和XMM这些需要保存的寄存器值呢？基于x86架构的Linux内核使用i387_union类型的变量thread.i387存储这些值，该变量位于进程描述符中。i387_union如下所示：

    union i387_union {
        struct i387_fsave_struct fsave;
        struct i387_fxsave_struct fxsave;
        struct i387_soft_struct soft;
    };

如代码所示，这个联合体包含三个不同类型的数据结构。没有协处理器的CPU模型使用i387_soft_struct类型数据结构，这是Linux为了兼容那些使用软件模拟协处理器的旧芯片。故我们在此，不做过多描述。带有协处理器和MMX单元的CPU模型使用i387_fsave_struct数据类型。带有SSE和SSE2扩展的CPU模型使用i387_fxsave_struct数据类型。

除此之外，进程描述符还包含另外2个标志：

* TS_USEDFPU标志

    位于thread_info描述符的status成员中。表示正在进行的进程是否使用FPU、MMX或XMM寄存器。

* PF_USED_MATH标志

    位于task_struct描述符中的flags成员中。表示存储在thread.i387中的数据是否有意义。该标志被清除的时候有两种情况：

    - 调用execve()系统调用，启动新进程的时候。因为控制单元绝不会再返回到之前的程序中，所以存储在thread.i387中的数据就没有了意义。

    - 用户态正在执行的程序开始执行信号处理程序的时候。因为信号处理程序相对于正在执行的程序流来说是异步的，此时的浮点寄存器对于信号处理程序也就没有了意义。但是需要内核为进程保存thread.i387中的浮点寄存器值，等到信号处理程序终止时再恢复这些寄存器值。也就是说，允许信号处理程序使用协处理器。

<h4 id="3.3.4.1">3.3.4.1 保存FPU寄存器</h4>

我们在分析进程切换的时候，知道主要的工作都是在__switch_to()宏中完成的。而在__switch_to()宏中，执行__unlazy_fpu宏，并将先前进程的进程描述符作为参数进行传递。这个宏会检查旧进程的TS_USEDFPU标志：如果标志被设置，说明旧进程使用了FPU、MMX、SSE或SSE2指令。因此，内核必须保存相关的硬件上下文，如下所示：

    if (prev->thread_info->status & TS_USEDFPU)
        save_init_fpu(prev);

函数save_init_fpu()完成保存这些寄存器的基本工作，如下所示：

1. 将FPU寄存器的内容保存到旧进程的描述符中，然后重新初始化FPU。如果CPU还使用了SSE/SSE2扩展，还需要保存XMM寄存器的内容，然后重新初始化SSE/SSE2单元。下面的2条GNU伪汇编语言实现：
    
        asm volatile( "fxsave %0 ; fnclex"
            : "=m" (prev->thread.i387.fxsave) );

    开启SSE/SSE2扩展，使用下面这条汇编语言：

        asm volatile( "fnsave %0 ; fwait"
            : "=m" (prev->thread.i387.fsave) );

2. 清除旧进程的TS_USEDFPU标志：

        prev->thread_info->status &= ~TS_USEDFPU;

3. 设置cr0协处理器的TS标志。使用stts()宏实现，该宏转换成汇编语言如下所示：

        movl %cr0, %eax
        orl $8,%eax
        movl %eax, %cr0

<h4 id="3.3.4.2">3.3.4.2 加载FPU寄存器</h4>

新进程恢复执行的时候，浮点寄存器不能立即恢复。但是通过__unlazy_fpu()宏已经设置了cr0协处理器中的TS标志。所以，新进程第一次尝试执行ESCAPE、MMX或SSE/SSE2指令的时候，控制单元就会发出`Device not available`异常，内核中相关的异常处理程序就会执行math_state_restore()函数加载浮点寄存器等。新进程被标记为当前进程。

    void math_state_restore()
    {
        asm volatile ("clts");  /* 清除TS标志 */
        if (!(current->flags & PF_USED_MATH))
            init_fpu(current);
        restore_fpu(current);
        current->thread.status |= TS_USEDFPU;
    }

该函数还会清除TS标志，以至于后来再执行FPU、MMX或SSE/SSE2指令的时候，不会再发出`Device not available`异常。如果PF_USED_MATH标志等于0，说明thread.i387中的内容没有意义了，init_fpu()就会复位thread.i387，并设置当前进程的PF_USED_MATH为1。然后，restore_fpu()就会把正确的值加载到FPU寄存器中。这个加载过程需要调用汇编指令fxrstor或frstor，使用哪一个取决于CPU是否支持SSE/SSE2扩展。最后，设置TS_USEDFPU标志，表示使用了浮点运算单元。

<h4 id="3.3.4.3">3.3.4.3 在内核中使用FPU、MMX和SSE/SSE2单元</h4>

当然了，内核中也可以使用FPU、MMX或SSE/SSE2硬件单元（虽然，大部分时候没有意义）。这样做的话，应该避免干扰当前用户进程执行的任何浮点运算。因此：

* 在使用协处理器之前，内核必须调用kernel_fpu_begin()，继而调用save_init_fpu()，保存用户进程的浮点相关寄存器的内容。然后，清除cr0协处理器中的TS标志。

* 使用完了之后，内核必须调用kernel_fpu_end()，设置TS标志。

之后，如果用户进程再执行协处理器指令的时候，math_state_restore()就会像进程切换时那样，恢复这些寄存器的内容。

但是，需要特别指出的是，如果当前用户进程正在使用协处理器时，kernel_fpu_begin()的执行时间相当长，甚至抵消了使用FPU、MMX或SSE/SSE2这些硬件单元带来的加速效果。事实上，内核只在几处地方使用它们，通常是搬动或清除大内存块或当计算校验的时候。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3.4">3.4 创建进程</h2>

Unix操作系统创建进程来响应用户请求。比如，每当用户在shell命令行中键入命令时，创建一个shell副本，然后执行用户的请求。

传统的Unix系统使用相同的方法创建所有进程：在子进程中复制父进程的所有资源。但是，这种创建进程的方法效率比较低，因为它拷贝父进程的整个地址空间。子进程极少需要读取或修改所有从父进程继承的资源。大部分时候，都是立即发出execve()系统调用和并清空所有拷贝的地址空间。

现代Unix系统采用三种方法解决这个问题：

* **写时复制技术**（Copy-On-Write）允许父进程和子进程共享相同的物理地址空间。不论哪个进程尝试写这个共同的物理地址时，内核都会给这个尝试的进程分配一个新的物理地址空间。 尝试写的进程就在新的地址空间上进行操作。

* **轻量级进程允许父进程和子进程共享内核数据结构**，比如页表（因此，共享整个用户态地址空间），打开的文件和信号处理等。

* vfork()创建一个进程，和父进程共享内存地址空间。先阻止父进程的执行，直到子进程退出或者执行一个新程序之后，父进程才执行。

<h3 id="3.4.1">3.4.1 clone()、fork()和vfork()系统调用</h3>

Linux内核用于创建进程的系统调用有3个，它们的实现分别为：fork、vfork、clone。它们的作用如下表所示：

| 调用 | 描述 |
| --- | ---- |
| clone | 创建轻量级进程（也就是线程），pthread库基于此实现 |
| vfork | 父子进程共享资源，子进程先于父进程执行 |
| fork  | 创建父进程的完整副本 |

<font color="blue">
进程的4要素:

* 执行代码
* 私有堆栈空间
* 进程控制块（task_struct）
* 有独立的内存空间

</font>
下面我们来看一下3个函数的区别：

1. clone()

    创建轻量级进程，其拥有的参数是：

    1. fn

        指定新进程执行的函数。当从函数返回时，子进程终止。函数返回一个退出码，表明子进程的退出状态。

    2. arg

        指向fn()函数的参数。

    3. flags

        一些标志位，低字节是表示当子进程终止时发送给父进程的信号，通常是SIGCHLD信号。其余的3个字节是一组标志，如下表所示：

        | 名称 | 描述 |
        | ------------- | -----------------------|
        | CLONE_VM      | 共享内存描述符和所有的页表 |
        | CLONE_FS      | 共享文件系统 |
        | CLONE_FILES   | 共享打开的文件 |
        | CLONE_SIGHAND | 共享信号处理函数，阻塞和挂起的信号等 |
        | CLONE_PTRACE  | debug用，父进程被追踪，子进程也追踪 |
        | CLONE_VFORK   | 父进程挂起，直到子进程释放虚拟内存资源 |
        | CLONE_PARENT  | 设置子进程的父进程是调用者的父进程，也就是创建兄弟进程 |
        | CLONE_THREAD  | 共享线程组，设置相应的线程组数据 |
        | CLONE_NEWNS   | 设置自己的命令空间，也就是有独立的文件系统 |
        | CLONE_SYSVSEM | 共享System V IPC可撤销信号量操作 |
        | CLONE_SETTLS  | 为轻量级进程创建新的TLS段 |
        | CLONE_PARENT_SETTID | 写子进程PID到父进程的用户态变量中 |
        | CLONE_CHILD_CLEARTID | 设置时，当子进程exit或者exec时，给父进程发送信号 |

    4. child_stack

        指定子进程的用户态栈指针，存储在子进程的esp寄存器中。父进程总是给子进程分配一个新的栈。

    5. tls

        指向为轻量级进程定义的TLS段数据结构的地址。只有CLONE_SETTLS标志设置了才有意义。

    6. ptid

        指定父进程的用户态变量地址，用来保存新建轻量级进程的PID。只有CLONE_PARENT_SETTID标志设置了才有意义。

    7. ctid

        指定新进程保存PID的用户态变量的地址。只有CLONE_CHILD_SETTID标志设置了才有意义。

    clone()其实是一个C库中的封装函数，它建立新进程的栈并调用sys_clone()系统调用。sys_clone()系统调用没有参数fn和arg。事实上，clone()把fn函数的指针保存到子进程的栈中return地址处，指针arg紧随其后。当clone()函数终止时，CPU从栈上获取return地址并执行fn(arg)函数。

    下面我们看一个C代码示例，看看clone()函数的使用：

        #include <stdio.h>
        #include <stdlib.h>
        #include <malloc.h>
        #include <linux/sched.h>
        #include <signal.h>
        #include <sys/types.h>
        #include <unistd.h>

        #define FIBER_STACK 8192

        int a;
        void *stack;

        int do_something()
        {
            printf("This is the son, and my pid is:%d,
                and a = %d\n", getpid(), ++a);
            free(stack);
            exit(1);
        }

        int main()
        {
            void * stack;
            a = 1;

            /* 为子进程申请系统堆栈 */
            stack = malloc(FIBER_STACK);

            if(!stack)
            {
                printf("The stack failed\n");
                exit(0);
            }
            printf("Creating son thread!!!\n");

            clone(&do_something, (char *)stack + FIBER_STACK, CLONE_VM|CLONE_VFORK, 0);//创建子线程
            printf("This is the father, and my pid is: %d, and a = %d\n", getpid(), a);

            exit(1);
        }

    上面的代码就相当于实现了一个vfork()，只有子进程执行完并释放虚拟内存资源后，父进程执行。执行结果是：

        Creating son thread!!!
        This is the son, and my pid is:3733, and a = 2
        This is the father, and my pid is: 3732, and a = 2

    它们现在共享堆栈，所以a的值是相等的。

2. fork()

    linux将fork实现为这样的clone()系统调用，其flags参数指定为SIGCHLD信号并清除所有clone标志，child_stack参数是当前父进程栈的指针。父进程和子进程暂时共享相同的用户态堆栈。然后采用 **写时复制技术**，不管是父进程还是子进程，在尝试修改堆栈时，立即获得刚才共享的用户态堆栈的一个副本。也就是称为了一个单独的进程。

        #include <stdio.h>
        #include <stdlib.h>
        #include <sys/types.h>
        #include <unistd.h>

        int main(void)
        {
            int count = 1;
            int child;

            child = fork();
            if(child < 0){
                perror("fork error : ");
            }
            else if(child == 0){    // child process
                printf("This is son, his count is: %d (%p). and his pid is: %d\n",
                        ++count, &count, getpid());
            }
            else{                   // parent process
                printf("This is father, his count is: %d (%p), his pid is: %d\n",
                        count, &count, getpid());
            }
            return EXIT_SUCCESS;
        }

    上面代码的执行结果：

        This is father, his count is: 1 (0xbfdbb384), his pid is: 3994
        This is son, his count is: 2 (0xbfdbb384). and his pid is: 3995

    可以看出，父子进程的PID是不一样的，而且堆栈也是独立的（count计数一个是1，一个是2）。

3. vfork()

    将vfork实现为这样的clone()系统调用，flags参数指定为SIGCHLD|CLONE_VM|CLONE_VFORK信号，child_stack参数等于当前父进程栈指针。

    vfork其实是一种过时的应用，vfork也是创建一个子进程，但是子进程共享父进程的空间。在vfork创建子进程之后，阻塞父进程，直到子进程执行了exec()或exit()。vfork最初是因为fork没有实现COW机制，而在很多情况下fork之后会紧接着执行exec，而exec的执行相当于之前的fork复制的空间全部变成了无用功，所以设计了vfork。而现在fork使用了COW机制，唯一的代价仅仅是复制父进程页表的代价，所以vfork不应该出现在新的代码之中。

    实际上，vfork创建的是一个线程，与父进程共享内存和堆栈空间：

    我们看下面的示例代码：

        #include <stdio.h>
        #include <stdlib.h>
        #include <sys/types.h>
        #include <unistd.h>

        int main()
        {
            int count = 1;
            int child;
            printf("Before create son, the father's count is:%d\n", count);
            if(!(child = vfork())) {
                printf("This is son, his pid is: %d and the count is: %d\n",
                        getpid(), ++count);
                exit(1);
            }
            else {
                printf("This is father, pid is: %d and count is: %d\n",
                        getpid(), count);
            }
        }

    执行结果为：

        Before create son, the father's count is:1
        This is son, his pid is: 3564 and the count is: 2
        This is father, pid is: 3563 and count is: 2

    从运行结果看，vfork创建的子进程（线程）共享了父进程的count变量，所以，子进程修改count，父进程的count值也改变了。

    另外由vfork创建的子进程要先于父进程执行，子进程执行时，父进程处于挂起状态，子进程执行完，唤醒父进程。除非子进程exit或者execve才会唤起父进程，看下面程序：

        #include <stdio.h>
        #include <stdlib.h>
        #include <sys/types.h>
        #include <unistd.h>

        int main()
        {
            int count = 1;
            int child;
            printf("Before create son, the father's count is:%d\n", count);
            if(!(child = vfork())) {

                int i;
                for(i = 0; i < 100; i++)
                {
                    printf("Child process & i = %d\n", i);
                    count++;
                    if(i == 20)
                    {
                        printf("Child process & pid = %d;count = %d\n",
                                getpid(), ++count);
                        exit(1);
                    }
                }
            }
            else {
                printf("Father process & pid = %d ;count = %d\n",
                        getpid(), count);
            }
        }

    执行结果为：

        Before create son, the father's count is:1
        Child process & i = 0
        Child process & i = 1
        Child process & i = 2
        Child process & i = 3
        Child process & i = 4
        Child process & i = 5
        Child process & i = 6
        Child process & i = 7
        Child process & i = 8
        Child process & i = 9
        Child process & i = 10
        Child process & i = 11
        Child process & i = 12
        Child process & i = 13
        Child process & i = 14
        Child process & i = 15
        Child process & i = 16
        Child process & i = 17
        Child process & i = 18
        Child process & i = 19
        Child process & i = 20
        Child process & pid = 3755;count = 23
        Father process & pid = 3754 ;count = 23

    从上面的结果可以看出，父进程总是等子进程执行完毕后才开始继续执行。

<h4 id="3.4.1.1">3.4.1.1 _do_fork()函数</h4>

不论是clone()、fork()还是vfork()，它们最核心的部分还是调用_do_fork()（一个与体系无关的函数），完成创建进程的工作。它具有如下参数：

> `_do_fork`和`do_fork`在进程的复制的时候并没有太大的区别, 他们就只是在进程tls复制的过程中实现有细微差别

下面是_do_fork的源代码:

    long _do_fork(unsigned long clone_flags,
              unsigned long stack_start,
              unsigned long stack_size,
              int __user *parent_tidptr,
              int __user *child_tidptr,
              unsigned long tls)
    {
        struct task_struct *p;
        int trace = 0;
        long nr;

        /*
         * 当从kernel_thread调用或CLONE_UNTRACED被设置时，不需要报告event
         * 否则，报告使用哪种fork类型的event
         */
        if (!(clone_flags & CLONE_UNTRACED)) {
            if (clone_flags & CLONE_VFORK)
                trace = PTRACE_EVENT_VFORK;
            else if ((clone_flags & CSIGNAL) != SIGCHLD)
                trace = PTRACE_EVENT_CLONE;
            else
                trace = PTRACE_EVENT_FORK;

            if (likely(!ptrace_event_enabled(current, trace)))
                trace = 0;
        }
        /* 拷贝进程描述符 */
        p = copy_process(clone_flags, stack_start, stack_size,
                 child_tidptr, NULL, trace, tls, NUMA_NO_NODE);
        /*
         * 在唤醒新线程之前，先检查指针是否合法
         * 因为如果线程创建后立即退出的话，线程指针可能会非法
         */
        if (!IS_ERR(p)) {
            struct completion vfork;
            struct pid *pid;

            trace_sched_process_fork(current, p);

            /* 分配PID */
            pid = get_task_pid(p, PIDTYPE_PID);
            nr = pid_vnr(pid);

            /* 把新进程PID写入到父进程的用户态变量中。 */
            if (clone_flags & CLONE_PARENT_SETTID)
                put_user(nr, parent_tidptr);

            /* 如果实现的是vfork调用，则完成completion机制，确保父进程后续运行 */
            if (clone_flags & CLONE_VFORK) {
                p->vfork_done = &vfork;
                init_completion(&vfork);
                get_task_struct(p);
            }

            /* 唤醒新进程 */
            wake_up_new_task(p);

            /* fork过程完成，子进程开始运行，并告知ptracer */
            if (unlikely(trace))
                ptrace_event_pid(trace, pid);

            /*
             * 如果实现的vfork调用，则将父进程插入等待队列，并挂起父进程
             * 直到子进程释放自己的内存空间，也就是，保证子进程先行于父进程
             */
            if (clone_flags & CLONE_VFORK) {
                if (!wait_for_vfork_done(p, &vfork))
                    ptrace_event_pid(PTRACE_EVENT_VFORK_DONE, pid);
            }
            put_pid(pid);
        } else {
            nr = PTR_ERR(p);
        }
        return nr;
    }

<h4 id="3.4.1.2">3.4.1.2 copy_process()函数</h4>

copy_process函数实现进程创建的大部分工作：创建旧进程的副本，比如进程描述符和子进程运行需要的其它内核数据结构。下面是一段伪代码：

    static struct task_struct *copy_process(unsigned long clone_flags,
                        unsigned long stack_start,
                        unsigned long stack_size,
                        int __user *child_tidptr,
                        struct pid *pid,
                        int trace,
                        unsigned long tls,
                        int node)
    {
        // 1. 检查参数clone_flags，一些标志组合是否合理
        // ...

        // 2. 其它的安全检查
        retval = security_task_create(clone_flags);

        // 3. 分配一个新的task_struct结构，与父进程完全相同，只是stack不同
        p = dup_task_struct(current, node);

        // 4. 检查该用户的进程数是否超过限制
        if (atomic_read(&p->real_cred->user->processes) >=
                task_rlimit(p, RLIMIT_NPROC)) {
            // 检查该用户是否具有相关权限，不一定是root
            if (p->real_cred->user != INIT_USER &&
                // ...
        }

        // 5. 检查进程数量是否超过max_threads，后者取决于内存的大小
        if (nr_threads >= max_threads)
        // ...

        // 6. 进程描述符相关变量初始化

        // 7. 完成调度器相关数据结构的初始化
        retval = sched_fork(clone_flags, p);

        // 8. 拷贝所有的进程信息
        shm_init_task(p);
        retval = copy_semundo(clone_flags, p);
        retval = copy_files(clone_flags, p);
        retval = copy_fs(clone_flags, p);
        retval = copy_sighand(clone_flags, p);
        retval = copy_signal(clone_flags, p);
        retval = copy_mm(clone_flags, p);
        retval = copy_namespaces(clone_flags, p);
        retval = copy_io(clone_flags, p);

        // 9. 设置子进程的pid
        retval = copy_thread_tls(clone_flags, stack_start, stack_size, p, tls);

        // ...

        // 10. 设置子进程的PID
        p->pid = pid_nr(pid);

        // 11. 根据是创建线程还是进程设置线程组组长、进程组组长等等信息
        // ...

        // 12. 将pid加入PIDTYPE_PID这个散列表
        if (likely(p->pid)) {
            // ..
            attach_pid(p, PIDTYPE_PID);
            nr_threads++;
        }

        // 释放资源，善后处理
        return p;
    err:
        // 错误处理
    }

现在，我们已经有了一个可运行的子进程，但是，实际上还没有运行。至于何时运行取决于调度器。在未来的某个进程切换时间点上，调度器把子进程描述符中的thread成员中的值加载到CPU上，赋予子进程CPU的使用权。esp寄存器加载thread.esp的值（也就是获取了子进程的内核态栈的地址），eip寄存器加载ret_from_fork()函数的返回地址（子进程执行的下一条指令）。ret_from_fork()是一个汇编函数，它调用schedule_tail()，继而调用finish_task_switch()函数，完成进程切换，然后把栈上的值加载到寄存器中，强迫CPU进入用户模式。基本上，新进程的执行恰恰在fork()、vfork()或clone()系统调用结束之时。该系统调用的返回值保存在寄存器eax中：对于子进程是0，对于父进程来说就是子进程的PID。

要想理解这一点，应该要重点理解一下copy_thread_tls()函数，其与早期的copy_thread()函数非常类似，只是在末尾添加了向子线程添加tls的内容。

在copy_thread_tls()函数中，我们可以看到这样的代码：

    childregs->ax = 0;
    p->thread.ip = (unsigned long) ret_from_fork;

这就是为什么子进程为什么返回PID=0的原因。

<h3 id="3.4.2">3.4.2 内核线程</h3>

Traditional Unix systems delegate some critical tasks to intermittently running processes, including flushing disk caches, swapping out unused pages, servicing network connections, and so on. Indeed, it is not efficient to perform these tasks in strict linear fashion; both their functions and the end user processes get better response if they are scheduled in the background. Because some of the system processes run only in Kernel Mode, modern operating systems delegate their functions to kernel threads, which are not encumbered with the unnecessary User Mode context. In Linux, kernel threads differ from regular processes in the following ways:

* Kernel threads run only in Kernel Mode, while regular processes run alternatively in Kernel Mode and in User Mode.

* Because kernel threads run only in Kernel Mode, they use only linear addresses greater than PAGE_OFFSET. Regular processes, on the other hand, use all four gigabytes of linear addresses, in either User Mode or Kernel Mode.

<h4 id="3.4.2.1">3.4.2.1 创建内核线程</h4>

The kernel_thread( ) function creates a new kernel thread. It receives as parameters the address of the kernel function to be executed (fn), the argument to be passed to that function (arg), and a set of clone flags (flags). The function essentially invokes do_fork() as follows:

    do_fork(flags|CLONE_VM|CLONE_UNTRACED, 0, pregs, 0, NULL, NULL);

The CLONE_VM flag avoids the duplication of the page tables of the calling process: this duplication would be a waste of time and memory, because the new kernel thread will not access the User Mode address space anyway. The CLONE_UNTRACED flag ensures that no process will be able to trace the new kernel thread, even if the calling process is being traced.

The pregs parameter passed to do_fork() corresponds to the address in the Kernel Mode stack where the copy_thread() function will find the initial values of the CPU registers for the new thread. The kernel_thread() function builds up this stack area so that:

* The ebx and edx registers will be set by copy_thread() to the values of the parameters fn and arg, respectively.
* The eip register will be set to the address of the following assembly language fragment:

        movl %edx,%eax
        pushl %edx
        call *%ebx
        pushl %eax
        call do_exit

Therefore, the new kernel thread starts by executing the fn(arg) function. If this function terminates, the kernel thread executes the _exit() system call passing to it the return value of fn() (see the section “Destroying Processes” later in this chapter).

<h4 id="3.4.2.2">3.4.2.2 进程0</h4>

The ancestor of all processes, called process 0, the idle process, or, for historical reasons, the swapper process, is a kernel thread created from scratch during the initialization phase of Linux (see Appendix A). This ancestor process uses the following statically allocated data structures (data structures for all other processes are dynamically allocated):

* A process descriptor stored in the init_task variable, which is initialized by the INIT_TASK macro.
* A thread_info descriptor and a Kernel Mode stack stored in the init_thread_union variable and initialized by the INIT_THREAD_INFO macro.
* The following tables, which the process descriptor points to:
    - init_mm
    - init_fs
    - init_files
    - init_signals
    - init_sighand
    The tables are initialized, respectively, by the following macros:
    - INIT_MM
    - INIT_FS
    - INIT_FILES
    - INIT_SIGNALS
    - INIT_SIGHAND
* The master kernel Page Global Directory stored in swapper_pg_dir (see the section“Kernel Page Tables” in Chapter 2).

The start_kernel() function initializes all the data structures needed by the kernel, enables interrupts, and creates another kernel thread, named process 1 (more commonly referred to as the init process):

    kernel_thread(init, NULL, CLONE_FS|CLONE_SIGHAND);

The newly created kernel thread has PID 1 and shares all per-process kernel data structures with process 0. When selected by the scheduler, the init process starts executing the init() function.

After having created the init process, process 0 executes the cpu_idle( ) function, which essentially consists of repeatedly executing the hlt assembly language instruction with the interrupts enabled (see Chapter 4). Process 0 is selected by the scheduler only when there are no other processes in the TASK_RUNNING state.

In multiprocessor systems there is a process 0 for each CPU. Right after the power-on, the BIOS of the computer starts a single CPU while disabling the others. The swapper process running on CPU 0 initializes the kernel data structures, then enables the other CPUs and creates the additional swapper processes by means of the copy_process() function passing to it the value 0 as the new PID. Moreover, the kernel sets the cpu field of the thread_info descriptor of each forked process to the proper CPU index.

<h4 id="3.4.2.3">3.4.2.3 进程1</h4>

The kernel thread created by process 0 executes the init( ) function, which in turn completes the initialization of the kernel. Then init( ) invokes the execve() system call to load the executable program init. As a result, the init kernel thread becomes a regular process having its own per-process kernel data structure (see Chapter 20). The init process stays alive until the system is shut down, because it creates and monitors the activity of all processes that implement the outer layers of the operating system.

<h4 id="3.4.2.4">3.4.2.4 其它内核线程</h4>

Linux uses many other kernel threads. Some of them are created in the initialization
phase and run until shutdown; others are created “on demand,” when the kernel
must execute a task that is better performed in its own execution context.

A few examples of kernel threads (besides process 0 and process 1) are:

* keventd (also called events)

    Executes the functions in the keventd_wq workqueue (see Chapter 4).

* kapmd

    Handles the events related to the Advanced Power Management (APM).

* kswapd

    Reclaims memory, as described in the section “Periodic Reclaiming” in Chapter 17.

* pdflush

    Flushes “dirty” buffers to disk to reclaim memory, as described in the section “The pdflush Kernel Threads” in Chapter 15.

* kblockd

    Executes the functions in the kblockd_workqueue workqueue. Essentially, it periodically activates the block device drivers, as described in the section “Activating the Block Device Driver” in Chapter 14.

* ksoftirqd

    Runs the tasklets (see section “Softirqs and Tasklets” in Chapter 4); there is one of these kernel threads for each CPU in the system.


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3.5">3.5 销毁进程</h2>
<h3 id="3.5.1">3.5.1 终止进程</h3>
<h4 id="3.5.1.1">3.5.1.1 do_group_exit()</h4>
<h4 id="3.5.1.2">3.5.1.2 do_exit()</h4>
<h3 id="3.5.2">3.5.2 移除进程</h3>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
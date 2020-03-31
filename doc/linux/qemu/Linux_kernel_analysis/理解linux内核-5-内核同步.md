<h1 id="0">0 目录</h1>

* [5 内核同步](#5)
    - [5.1 内核服务如何请求](#5.1)
    - [5.2 同步原语](#5.2)
    - [5.3 内核数据结构的同步访问](#5.3)
    - [5.4 防止竞态条件的示例](#5.4)


<h1 id="5">5 内核同步</h1>

我们可以把内核想象成一个服务器，专门响应各种请求。这些请求可以是CPU上正在运行的进程发起的请求，也可以是外部的设备发起的中断请求。所以说，内核并不是串行运行，而是交错执行。既然是交错执行，就会产生`竞态条件`，我们可以采用同步技术消除这种竞态条件。

We start this chapter by reviewing when, and to what extent, kernel requests are executed in an interleaved fashion. We then introduce the basic synchronization primitives implemented by the kernel and describe how they are applied in the most common cases. Finally, we illustrate a few practical examples.

<h2 id="5.1">5.1 内核服务如何请求</h2>

为了更好地理解内核是如何工作的，我们把内核比喻成一个酒吧服务员，他响应两种请求服务：一种是来自顾客，另外一种来自多个老板。这个服务员采用的策略是：

1. 如果老板呼叫服务员，而服务员恰巧空闲，则立即服务老板；

2. 如果老板呼叫服务员，而服务员恰巧正在服务一名顾客。则服务员停止为顾客服务，而是去服务老板。

3. 如果老板呼叫服务员，而服务员恰巧在服务另一个老板，则服务员停止服务第一个老板，转而服务第二个。当他服务完第二个老板，再回去服务第一个老板。

4. 老板让服务员停止为顾客服务转而为自己服务。在处理完老板的最后一个请求后，服务员也可能会决定是临时性地放弃之前的顾客，而迎接新顾客。

上面的服务员就非常类似于处于内核态的代码执行。如果CPU被用户态程序占用，服务员被认为是空闲的。老板的请求就类似于中断请求，而顾客请求就对应于用户进程发出的系统调用或异常。后面描述中，异常处理程序指的是系统调用和常规异常的处理程序。

仔细研究，就会发现，前3条规则其实与内核中的异常和中断嵌套执行的规则是一样的。第4条规则就对应于内核抢占。

<h3 id="5.1.1">5.1.1 内核抢占</h3>

给内核抢占下一个完美定义很难。在这儿，我们只是尝试着给其下一个定义：如果一个进程正运行在内核态，此时，发生了进程切换我们就称其为抢占式内核。当然了，Linux内核不可能这么简单：

* 不论是抢占式内核还是非抢占式内核，进程都有可能放弃CPU的使用权而休眠等待某些资源。我们称这类进程切换是有计划的进程切换。但是抢占式内核和非抢占式的区别就在于对于异步事件的响应方式不同-比如，抢占式内核的中断处理程序可以唤醒更高优先级的进程，而非抢占式内核不会。我们称这类进程切换为强迫性的进程切换。

* 我们已经知道所有的进程切换动作都由`switch_to`宏完成。不论是抢占式还是非抢占式，当进程完成内核活动的某个线程并调用调度器时就会发生进程切换。但是，在非抢占式内核中，除非即将切换到用户态时，否则不会发生进程替换。

因此，抢占式内核主要的特性就是运行在内核态的进程可以被其它进程打断而发生替换。

让我们举例说明抢占式内核和非抢占式内核的区别：

假设进程A正在执行异常处理程序（内核态），这时候中断请求IRQ发生，相应的处理程序唤醒高优先级的进程B。如果内核是可抢占式的，就会发生进程A到进程B的替换。异常处理程序还没有执行完，只有当调度器再一次选择进程A执行的时候才会继续。相反，如果内核是非抢占式的，除非进程A完成异常处理或者自愿放弃CPU的使用权，否则不会发生进程切换。

再比如，考虑正在执行异常处理程序的进程，它的CPU使用时间已经超时。如果内核是抢占式的，进程被立即切换；但是，如果内核是非抢占式的，进程会继续执行，知道进程完成异常处理或自动放弃CPU的使用权。

实施内核抢占的动机就是减少用户态进程的调度延时，也就是减少`可运行状态`到`真正运行时`的延时。需要实时调度的任务（比如外部的硬件控制器等）需要内核具有抢占性，因为减少了被其它进程延时的风险。

Linux内核是从2.6版本开始的，相比那些旧版本的非抢占性内核而言，没有什么显著的变化。当`thread_info`描述符中的`preempt_count`成员的值大于0，内核抢占就被禁止。这个值分为3部分，也就是说可能有3种情况导致该值大于0：

1. 内核正在执行中断服务例程（ISR）；
2. 延时函数被禁止（当内核执行软中断或tasklet时总是使能状态）；
3. 内核抢占被禁止。

通过上面的规则可以看出，内核只有在执行异常处理程序（尤其是系统调用）的时候才能够被抢占，而且内核抢占也没有被禁止。所以，CPU必须使能中断，内核抢占才能被执行。

A few simple macros listed in Table 5-1 deal with the preemption counter in the　prempt_count field.

Table 5-1. Macros dealing with the preemption counter subfield

| Macro | Description |
| ----- | ----------- |
| preempt_count() | Selects the preempt_count field in the thread_info descriptor |
| preempt_disable() | Increases by one the value of the preemption counter |
| preempt_enable_no_resched() | Decreases by one the value of the preemption counter |
| preempt_enable() | Decreases by one the value of the preemption counter, and invokes preempt_schedule() if the TIF_NEED_RESCHED flag in the thread_info descriptor is set |
| get_cpu() | Similar to preempt_disable(), but also returns the number of the local CPU |
| put_cpu() | Same as preempt_enable() |
| put_cpu_no_resched() | Same as preempt_enable_no_resched() |

The preempt_enable() macro decreases the preemption counter, then checks whether the TIF_NEED_RESCHED flag is set (see Table 4-15 in Chapter 4). In this case, a process switch request is pending, so the macro invokes the preempt_schedule() function, which essentially executes the following code:

    if (!current_thread_info->preempt_count && !irqs_disabled()) {
        current_thread_info->preempt_count = PREEMPT_ACTIVE;
        schedule();
        current_thread_info->preempt_count = 0;
    }

The function checks whether local interrupts are enabled and the preempt_count field of current is zero; if both conditions are true, it invokes schedule() to select another process to run. Therefore, kernel preemption may happen either when a kernel control path (usually, an interrupt handler) is terminated, or when an exception handler reenables kernel preemption by means of preempt_enable(). As we’ll see in the section “Disabling and Enabling Deferrable Functions” later in this chapter, kernel preemption may also happen when deferrable functions are enabled.

We’ll conclude this section by noticing that kernel preemption introduces a nonnegligible overhead. For that reason, Linux 2.6 features a kernel configuration option that allows users to enable or disable kernel preemption when compiling the kernel.

<h3 id="5.1.2">5.1.2 什么时候需要同步？</h3>

Chapter 1 introduced the concepts of race condition and critical region for processes.
The same definitions apply to kernel control paths. In this chapter, a race
condition can occur when the outcome of a computation depends on how two or
more interleaved kernel control paths are nested. A critical region is a section of code
that must be completely executed by the kernel control path that enters it before
another kernel control path can enter it.

Interleaving kernel control paths complicates the life of kernel developers: they must
apply special care in order to identify the critical regions in exception handlers, interrupt
handlers, deferrable functions, and kernel threads. Once a critical region has
been identified, it must be suitably protected to ensure that any time at most one kernel
control path is inside that region.

Suppose, for instance, that two different interrupt handlers need to access the same
data structure that contains several related member variables—for instance, a buffer
and an integer indicating its length. All statements affecting the data structure must be
put into a single critical region. If the system includes a single CPU, the critical region
can be implemented by disabling interrupts while accessing the shared data structure,
because nesting of kernel control paths can only occur when interrupts are enabled.

On the other hand, if the same data structure is accessed only by the service routines
of system calls, and if the system includes a single CPU, the critical region can be
implemented quite simply by disabling kernel preemption while accessing the shared
data structure.

As you would expect, things are more complicated in multiprocessor systems. Many
CPUs may execute kernel code at the same time, so kernel developers cannot assume
that a data structure can be safely accessed just because kernel preemption is disabled
and the data structure is never addressed by an interrupt, exception, or softirq handler.

We’ll see in the following sections that the kernel offers a wide range of different synchronization
techniques. It is up to kernel designers to solve each synchronization
problem by selecting the most efficient technique.

<h3 id="5.1.3">5.1.3 什么时候不需要同步？</h3>

Some design choices already discussed in the previous chapter simplify somewhat
the synchronization of kernel control paths. Let us recall them briefly:

* All interrupt handlers acknowledge the interrupt on the PIC and also disable the
IRQ line. Further occurrences of the same interrupt cannot occur until the handler
terminates.

* Interrupt handlers, softirqs, and tasklets are both nonpreemptable and nonblocking,
so they cannot be suspended for a long time interval. In the worst case,
their execution will be slightly delayed, because other interrupts occur during
their execution (nested execution of kernel control paths).

* A kernel control path performing interrupt handling cannot be interrupted by a
kernel control path executing a deferrable function or a system call service routine.

* Softirqs and tasklets cannot be interleaved on a given CPU.

* The same tasklet cannot be executed simultaneously on several CPUs.

Each of the above design choices can be viewed as a constraint that can be
exploited to code some kernel functions more easily. Here are a few examples of
possible simplifications:

* Interrupt handlers and tasklets need not to be coded as reentrant functions.
* Per-CPU variables accessed by softirqs and tasklets only do not require synchronization.
* A data structure accessed by only one kind of tasklet does not require synchronization.

The rest of this chapter describes what to do when synchronization is necessary—i.e., how to prevent data corruption due to unsafe accesses to shared data structures.

<h2 id="5.2">5.2 同步原语</h2>

We now examine how kernel control paths can be interleaved while avoiding race conditions among shared data. Table 5-2 lists the synchronization techniques used by the Linux kernel. The “Scope” column indicates whether the synchronization technique applies to all CPUs in the system or to a single CPU. For instance, local interrupt disabling applies to just one CPU (other CPUs in the system are not affected); conversely, an atomic operation affects all CPUs in the system (atomic operations on several CPUs cannot interleave while accessing the same data structure).

表5-2 Linux内核使用的一些同步技术

| 技术 | 描述 | 范围 |
| --------- | ----------- | ----- |
| Per-CPU变量 | 用于在CPU之间拷贝数据 | 所有CPU |
| 原子操作 | 针对计数器的原子RMW指令 | 所有CPU |
| 内存屏障 | 避免指令乱序 | 本地CPU或所有CPU |
| 自旋锁 | 忙等待 | 所有CPU |
| 信号量 | 阻塞等待（休眠） | 所有CPU |
| Seqlock |   | 所有CPU |
| 中断禁止 | 禁止响应中断 | 本地CPU |
| 软中断禁止 | 禁止处理可延时函数 | 本地CPU |
| 读-拷贝-更新（RCU） | 通过指针实现无锁访问共享资源 | 所有CPU |

Let’s now briefly discuss each synchronization technique. In the later section “Synchronizing Accesses to Kernel Data Structures,” we show how these synchronization techniques can be combined to effectively protect kernel data structures.

<h3 id="5.2.1">5.2.1 Per-CPU变量</h3>

The best synchronization technique consists in designing the kernel so as to avoid
the need for synchronization in the first place. As we’ll see, in fact, every explicit synchronization
primitive has a significant performance cost.

The simplest and most efficient synchronization technique consists of declaring kernel
variables as per-CPU variables. Basically, a per-CPU variable is an array of data
structures, one element per each CPU in the system.

A CPU should not access the elements of the array corresponding to the other CPUs;
on the other hand, it can freely read and modify its own element without fear of race
conditions, because it is the only CPU entitled to do so. This also means, however,
that the per-CPU variables can be used only in particular cases—basically, when it
makes sense to logically split the data across the CPUs of the system.

The elements of the per-CPU array are aligned in main memory so that each data
structure falls on a different line of the hardware cache (see the section “Hardware
Cache” in Chapter 2). Therefore, concurrent accesses to the per-CPU array do not
result in cache line snooping and invalidation, which are costly operations in terms
of system performance.

While per-CPU variables provide protection against concurrent accesses from several
CPUs, they do not provide protection against accesses from asynchronous functions
(interrupt handlers and deferrable functions). In these cases, additional
synchronization primitives are required.

Furthermore, per-CPU variables are prone to race conditions caused by kernel preemption,
both in uniprocessor and multiprocessor systems. As a general rule, a kernel
control path should access a per-CPU variable with kernel preemption disabled.
Just consider, for instance, what would happen if a kernel control path gets the
address of its local copy of a per-CPU variable, and then it is preempted and moved
to another CPU: the address still refers to the element of the previous CPU.

Table 5-3 lists the main functions and macros offered by the kernel to use per-CPU variables.

Table 5-3. Functions and macros for the per-CPU variables


<h3 id="5.2.2">5.2.2 原子操作</h3>

Several assembly language instructions are of type “read-modify-write”—that is,
they access a memory location twice, the first time to read the old value and the second
time to write a new value.

Suppose that two kernel control paths running on two CPUs try to “read-modifywrite”
the same memory location at the same time by executing nonatomic operations.
At first, both CPUs try to read the same location, but the memory arbiter (a
hardware circuit that serializes accesses to the RAM chips) steps in to grant access to
one of them and delay the other. However, when the first read operation has completed,
the delayed CPU reads exactly the same (old) value from the memory location.
Both CPUs then try to write the same (new) value to the memory location;
again, the bus memory access is serialized by the memory arbiter, and eventually
both write operations succeed. However, the global result is incorrect because both
CPUs write the same (new) value. Thus, the two interleaving “read-modify-write”
operations act as a single one.

The easiest way to prevent race conditions due to “read-modify-write” instructions is
by ensuring that such operations are atomic at the chip level. Every such operation
must be executed in a single instruction without being interrupted in the middle and
avoiding accesses to the same memory location by other CPUs. These very small
atomic operations can be found at the base of other, more flexible mechanisms to create
critical regions.

Let’s review 80 × 86 instructions according to that classification:

* Assembly language instructions that make zero or one aligned memory access are atomic.

* Read-modify-write assembly language instructions (such as inc or dec) that read data from memory, update it, and write the updated value back to memory are atomic if no other processor has taken the memory bus after the read and before the write. Memory bus stealing never happens in a uniprocessor system.

* Read-modify-write assembly language instructions whose opcode is prefixed by the lock byte (0xf0) are atomic even on a multiprocessor system. When the control unit detects the prefix, it “locks” the memory bus until the instruction is finished. Therefore, other processors cannot access the memory location while the locked instruction is being executed.

* Assembly language instructions whose opcode is prefixed by a rep byte (0xf2, 0xf3, which forces the control unit to repeat the same instruction several times) are not atomic. The control unit checks for pending interrupts before executing a new iteration.

When you write C code, you cannot guarantee that the compiler will use an atomic instruction for an operation like a=a+1 or even for a++. Thus, the Linux kernel provides a special atomic_t type (an atomically accessible counter) and some special functions and macros (see Table 5-4) that act on atomic_t variables and are implemented as single, atomic assembly language instructions. On multiprocessor systems, each such instruction is prefixed by a lock byte.

Table 5-4. Atomic operations in Linux


<h3 id="5.2.3">5.2.3 优化和内存屏障</h3>

When using optimizing compilers, you should never take for granted that instructions
will be performed in the exact order in which they appear in the source code.
For example, a compiler might reorder the assembly language instructions in such a
way to optimize how registers are used. Moreover, modern CPUs usually execute
several instructions in parallel and might reorder memory accesses. These kinds of
reordering can greatly speed up the program.

When dealing with synchronization, however, reordering instructions must be
avoided. Things would quickly become hairy if an instruction placed after a synchronization
primitive is executed before the synchronization primitive itself. Therefore,
all synchronization primitives act as optimization and memory barriers.

An optimization barrier primitive ensures that the assembly language instructions corresponding to C statements placed before the primitive are not mixed by the compiler with assembly language instructions corresponding to C statements placed after the primitive. In Linux the barrier() macro, which expands into asm volatile("":::"memory"), acts as an optimization barrier. The asm instruction tells the compiler to insert an assembly language fragment (empty, in this case). The volatile keyword forbids the compiler to reshuffle the asm instruction with the other instructions of the program. The memory keyword forces the compiler to assume that all memory locations in RAM have been changed by the assembly language instruction; therefore, the compiler cannot optimize the code by using the values of memory locations stored in CPU registers before the asm instruction. Notice that the optimization barrier does not ensure that the executions of the assembly language instructions are not mixed by the CPU—this is a job for a memory barrier.

A memory barrier primitive ensures that the operations placed before the primitive
are finished before starting the operations placed after the primitive. Thus, a memory
barrier is like a firewall that cannot be passed by an assembly language instruction.

In the 80×86 processors, the following kinds of assembly language instructions are
said to be “serializing” because they act as memory barriers:

* All instructions that operate on I/O ports
* All instructions prefixed by the lock byte (see the section “Atomic Operations”)
* All instructions that write into control registers, system registers, or debug registers (for instance, cli and sti, which change the status of the IF flag in the eflags register)

<h3 id="5.2.4">5.2.4 自旋锁</h3>
<h3 id="5.2.5">5.2.5 信号量</h3>
<h3 id="5.2.6">5.2.6 Seqlock</h3>
<h3 id="5.2.7">5.2.7 中断禁止</h3>
<h3 id="5.2.8">5.2.8 软中断禁止</h3>
<h3 id="5.2.9">5.2.9 读-拷贝-更新（RCU）</h3>

<h2 id="5.3">5.3 内核数据结构的同步访问</h2>

<h2 id="5.4">5.4 防止竞态条件的示例</h2>
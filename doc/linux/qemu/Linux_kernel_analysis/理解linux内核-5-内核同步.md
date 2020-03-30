<h1 id="0">0 目录</h1>

* [5 内核同步](#5)
    - [5.1 内核服务如何请求](#5.1)
    - [5.2 同步原语](#5.2)
    - [5.3 内核数据结构的同步访问](#5.3)
    - [5.4 防止竞态条件的示例](#5.4)


<h1 id="5">5 内核同步</h1>

You could think of the kernel as a server that answers requests; these requests can come either from a process running on a CPU or an external device issuing an interrupt request. We make this analogy to underscore that parts of the kernel are not run serially, but in an interleaved way. Thus, they can give rise to race conditions, which must be controlled through proper synchronization techniques. A general introduction to these topics can be found in the section “An Overview of Unix Kernels” in Chapter 1.

We start this chapter by reviewing when, and to what extent, kernel requests are executed in an interleaved fashion. We then introduce the basic synchronization primitives implemented by the kernel and describe how they are applied in the most common cases. Finally, we illustrate a few practical examples.

<h2 id="5.1">5.1 内核服务如何请求</h2>

To get a better grasp of how kernel’s code is executed, we will look at the kernel as a waiter who must satisfy two types of requests: those issued by customers and those issued by a limited number of different bosses. The policy adopted by the waiter is the following:

1. If a boss calls while the waiter is idle, the waiter starts servicing the boss.

2. If a boss calls while the waiter is servicing a customer, the waiter stops servicing the customer and starts servicing the boss.

3. If a boss calls while the waiter is servicing another boss, the waiter stops servicing the first boss and starts servicing the second one. When he finishes servicing the new boss, he resumes servicing the former one.

4. One of the bosses may induce the waiter to leave the customer being currently serviced. After servicing the last request of the bosses, the waiter may decide to drop temporarily his customer and to pick up a new one.

The services performed by the waiter correspond to the code executed when the CPU is in Kernel Mode. If the CPU is executing in User Mode, the waiter is considered idle.

Boss requests correspond to interrupts, while customer requests correspond to system calls or exceptions raised by User Mode processes. As we shall see in detail in Chapter 10, User Mode processes that want to request a service from the kernel must issue an appropriate instruction (on the 80×86, an int $0x80 or a sysenter instruction). Such instructions raise an exception that forces the CPU to switch from User Mode to Kernel Mode. In the rest of this chapter, we will generally denote as “exceptions” both the system calls and the usual exceptions.

The careful reader has already associated the first three rules with the nesting of kernel control paths described in “Nested Execution of Exception and Interrupt Handlers” in Chapter 4. The fourth rule corresponds to one of the most interesting new features included in the Linux 2.6 kernel, namely kernel preemption.

<h2 id="5.1.1">5.1.1 内核抢占</h2>

It is surprisingly hard to give a good definition of kernel preemption. As a first try,　we could say that a kernel is preemptive if a process switch may occur while the　replaced process is executing a kernel function, that is, while it runs in Kernel Mode.　Unfortunately, in Linux (as well as in any other real operating system) things are　much more complicated:

* Both in preemptive and nonpreemptive kernels, a process running in Kernel Mode　can voluntarily relinquish the CPU, for instance because it has to sleep waiting for　some resource. We will call this kind of process switch a planned process switch.　However, a preemptive kernel differs from a nonpreemptive kernel on the way a　process running in Kernel Mode reacts to asynchronous events that could induce　a process switch—for instance, an interrupt handler that awakes a higher priority　process. We will call this kind of process switch a forced process switch.

* All process switches are performed by the switch_to macro. In both preemptive　and nonpreemptive kernels, a process switch occurs when a process has finished　some thread of kernel activity and the scheduler is invoked. However, in nonpreemptive　kernels, the current process cannot be replaced unless it is about to switch　to User Mode (see the section “Performing the Process Switch” in Chapter 3).

Therefore, the main characteristic of a preemptive kernel is that a process running in　Kernel Mode can be replaced by another process while in the middle of a kernel　function.

Let’s give a couple of examples to illustrate the difference between preemptive and　nonpreemptive kernels.

While process A executes an exception handler (necessarily in Kernel Mode), a higher　priority process B becomes runnable. This could happen, for instance, if an IRQ occurs　and the corresponding handler awakens process B. If the kernel is preemptive, a forced　process switch replaces process A with B. The exception handler is left unfinished and　will be resumed only when the scheduler selects again process A for execution. Conversely,　if the kernel is nonpreemptive, no process switch occurs until process A either　finishes handling the exception handler or voluntarily relinquishes the CPU.

For another example, consider a process that executes an exception handler and　whose time quantum expires (see the section “The scheduler_tick( ) Function” in　Chapter 7). If the kernel is preemptive, the process may be replaced immediately;　however, if the kernel is nonpreemptive, the process continues to run until it finishes　handling the exception handler or voluntarily relinquishes the CPU.

The main motivation for making a kernel preemptive is to reduce the dispatch latency　of the User Mode processes, that is, the delay between the time they become runnable　and the time they actually begin running. Processes performing timely scheduled　tasks (such as external hardware controllers, environmental monitors, movie players,　and so on) really benefit from kernel preemption, because it reduces the risk of　being delayed by another process running in Kernel Mode.

Making the Linux 2.6 kernel preemptive did not require a drastic change in the kernel　design with respect to the older nonpreemptive kernel versions. As described in　the section “Returning from Interrupts and Exceptions” in Chapter 4, kernel preemption　is disabled when the preempt_count field in the thread_info descriptor referenced　by the current_thread_info() macro is greater than zero. The field encodes　three different counters, as shown in Table 4-10 in Chapter 4, so it is greater than　zero when any of the following cases occurs:

1. The kernel is executing an interrupt service routine.
2. The deferrable functions are disabled (always true when the kernel is executing a softirq or tasklet).
3. The kernel preemption has been explicitly disabled by setting the preemption counter to a positive value.

The above rules tell us that the kernel can be preempted only when it is executing an　exception handler (in particular a system call) and the kernel preemption has not　been explicitly disabled. Furthermore, as described in the section “Returning from　Interrupts and Exceptions” in Chapter 4, the local CPU must have local interrupts　enabled, otherwise kernel preemption is not performed.

A few simple macros listed in Table 5-1 deal with the preemption counter in the　prempt_count field.

Table 5-1. Macros dealing with the preemption counter subfield

| Macro | Description |
| ----- | ----------- |
| preempt_count() | Selects the preempt_count field in the thread_info descriptor |
| preempt_disable() | Increases by one the value of the preemption counter |
| preempt_enable_no_resched() | Decreases by one the value of the preemption counter |
| preempt_enable() | Decreases by one the value of the preemption counter |

<h2 id="5.2">5.2 同步原语</h2>

<h2 id="5.3">5.3 内核数据结构的同步访问</h2>

<h2 id="5.4">5.4 防止竞态条件的示例</h2>
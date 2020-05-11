<h1 id="0">0 目录</h1>

* [1.1 流水线](#1.1)
    - [1.1.1 制约流水线效率的因素](#1.1.1)
    - [1.1.2 制约流水线效率的因素](#1.1.2)
* [1.2 MIPS架构5级流水线](#1.2)
* [1.3 RISC和CISC对比](#1.3)
* [1.4 MIPS架构的发展](#1.4)
* [1.5 MIPS架构和CISC架构的对比](#1.5)

---

Why should a book about computer architecture devote several chapters to a particular operating system?

Well, this is what a CPU is for. A CPU “architecture” is the description of what a useful CPU does, and a useful CPU runs programs under the control of an operating system. Although many operating systems run on MIPS, the great thing about Linux is that it’s public. Anyone can download the source code, so anyone can see how it works.

Any operating system is just a bunch of programs. Ingenious programs, and—perhaps more than most software—built on a set of ideas that have been refined and figured out over the years. An operating system is supposed to be particularly reliable (doesn’t crash) and secure (doesn’t let some program do things the OS hasn’t been told to let it do).

Correct usage sees “Linux” as the name of the operating system kernel originally written by Linus Torvalds, a kernel whose subsequent history is, well, history. Most of the (much larger) rest of the system came from projects organized under the “GNU” banner of the Free Software Foundation. Everybody sometimes forgets and calls the whole thing “Linux.”

Both sides of this process emerged as a reaction to the seminal work on the UNIX operating system developed by Bell Laboratories in the 1970s. Probably because Bell saw it as of no commercial value, it distributed the software widely to academic institutions under terms that were then unprecedently “open.” But it wasn’t “open source”—many programmers worked on UNIX at university, only to find that their contributions were either lost or were now owned by Bell Labs (and their many successors). Frustration with this process eventually drove people to write “really free” replacements.

The last key part was the kernel. Kernels are quite difficult programs, but the delay was cultural: OS kernels were seen as something for academic groups, and those groups wanted to go beyond UNIX, not to recreate it. The post-UNIX fashion was for a small, modular operating system built of clearly separated components, but no OS built on that basis ever found a significant user base.

Linux won out because it was a much more pragmatic project. Linus and his fellow developers wanted something that worked (on x86 desktops, in the first instance). When the Linux kernel was in competition with offshoots of the finally free BSD4.4 system, BSD protagonists insisted with some justification on their superior engineering. But the Linux community had arrived at an understanding of a far more “open” development style. Linux evolved quickly. Sometimes, it evolved quickly because Linux people were perfectly happy to adapt BSD code. It wasn’t long before Linux triumphed, and the engineering got better, too.

<h2 id="13.1">13.1 组成</h2>

To get to grips with any artifact you need to attach some good working meaning to the terms used by its experts, and you are particularly likely to be confused by terms you already know, butwith not quite the same meaning. The UNIX/Linux heritage is long enough that there are lots of magic words:

1. Thread:

    The best general definition of “thread” I know is “a set of computer instructions being run in the order specified by the programmer.” The Linux kernel has an explicit notion of a thread (for each thread there’s a struct thread struct). It’s almost the same thing, but by the terms of my definition a low-level interrupt handler (for example) is a distinct thread that happens to have borrowed the environment of the interrupted thread to run with. Both definitions are valuable, and we’ll say “Linux thread” when necessary.

    Linux loves threads (there are currently 134 on the desktop machine I’m typing this on). Most of those threads correspond to an active application program—but there are quite a few special-purpose threads that run only in the kernel, and some applications have multiple threads.

    One of the kernel’s basic jobs is scheduling—picking which Linux thread to run next. See the bullet on the scheduler, below.

2. File:

    A named chunk of data. In GNU/Linux, most of the interactions a program makes with the world beyond its process are done by reading and writing files. Files can just be things you write data to and get it back later. But there are also special files that lead to device drivers: Read one of those and the data comes from a keyboard, write another and your data is interpreted as digital audio and sent out to a loudspeaker. The Linux kernel likes to avoid too many new system calls, so special /proc files are also used to allow applications to get information about the kernel.

3. User mode and system calls:

    Linux applications run in user mode, the lower-privilege state of MIPS CPUs. In user mode, the software can’t directly access the parts of the address space where the kernel lives, and all the locations it can address are mapped to pages the kernel has agreed to let the application playwith. In user mode, you can’t run the coprocessor zero CPU control instructions.

    GNU/Linux application code that runs in user mode is frequently referred to as userland.

    To obtain any service from the kernel (most often, to read or write a file) the application makes a system call. A system call is a deliberately planted exception, interpreted by the kernel’s exception handler. The exception switches to high-privilege mode.

    Through the system call, Linux application threads run quite happily in the kernel in high-privilege mode (but of course they’re running trusted code there). When it’s done, the return from exception code involves an eret, which makes sure that the change back to user mode and the return to user mode code are done simultaneously.

4. Interrupt context:

    Linux tries not to disable interrupts too much. When Linux is running, at any moment there’s an active thread on a CPU:2 So an interrupt borrows what appears to be the context of that thread until it finishes its business and returns. Code called from an interrupt handler is in interrupt context, and there are many things such code should not do. It can’t do anything that might have to wait for some other software activity, for example. If your keyboard input routine is going to log all keystrokes to a file,3 then you can’t do that by calling the file output routine from the interrupt handler.

    There are decent ways to do that: You can get the keyboard interrupt to arrange to wake some Linux thread that obtains and logs the input, for example.

5. Interrupt service routine (ISR):

    The lowest-level interrupt code in the device driver is generally called an ISR. In Linux you’re encouraged to keep this code short: If there’s lots of work to do, you can consider using some kind of “bottom half,” as described in the next chapter.

6. The scheduler:

    A kernel subroutine. The OS maintains a list of threads that are ready to run (they’re not blocked on an incomplete I/O transfer, for example), and that list is in priority order. The priority is dynamic, and is recalculated periodically—mostly to ensure that long-running computations don’t hog the CPU and prevent it from responding to events. Applications can lower their own priority to volunteer for a life in the background but can’t usually raise it. 

    After any interrupt is handled, the scheduler will be called. If the scheduler finds another thread is more worthy of running, it parks the current thread and runs the winner.

    Older Linux kernels were not preemptive: once a thread was running in the kernel it was allowed to run until it either volunteered for rescheduling (by waiting on something) or until control was just about to pass back into userland—only then would the kernel contemplate a thread switch.

    A nonpreemtive kernel is easier to program. Your kernel code sequence might have to worry about interrupt handlers running unexpectedly while it was in flight, but you knew it could never be unexpectedly caught halfway through something by some other mainstream kernel code. But it led to excessive delays and inadequate responsiveness. 

    The luxurious freedom from interference from parallel threads is lost when you have an SMP kernel (where two CPUs are simultaneously threading the same kernel). To make the SMP kernel work properly, hundreds of possible interactions need to be tracked down and protected with appropriate locks. The SMP locks are (in almost all cases) exactly where you need them to be to permit the scheduler to stop a running kernel thread and run another: That’s called kernel preemption.

    It’s now an important kernel programming discipline to recognize code sequences where preemptionmust be temporarily inhibited. The macros used to mark the start and end of that code have definitions that change according to kernel configuration to work correctly on uniprocessor or SMP systems.

7. Memory map/address space:

    The map of memory locations available to a particular Linux thread. The address space of a thread is defined through a mm struct, pointed to by the thread. For Linux OS ported to the MIPS architecture (hereinafter, “Linux/MIPS”) on a 32-bit processor, the high half of the address space (addresseswith bit 31 set) can be read and written only in kernel-privilege mode. The kernel code/data is normally in the corner of this, known as kseg0, which means the kernel itself does not depend on addresses translated through the TLB.

    The user part of the address space is mapped differently for each application—only threads that collaborate in an explicitly multithreaded application share the user address space (i.e., they point to the same mm struct). But all Linux threads share the same kernel map.

    A thread running a conventional single-threaded application runs in an address space that is distinct from all other threads and is exactly what older UNIX-like systems called a “process.”

    At any given time, much of an application’s address space may not in fact be mapped, or even not represented by any data present in physical memory at all. An attempt to access that will cause a TLB exception, which will be handled by the OS, which will load any missing data and set up an appropriate mapping before it returns to the application. That is, of course, virtual memory.

8. Thread group:

    The collection of threads within the same memory map is called a thread group. Where a group has two or more members, those threads are cooperating to run the same program. The thread group is another good approximation in Linux to what is called a “process” in old UNIX systems.

    High memory: Physical memory above 512 MB (whether real read/write memory or memory-mapped I/O locations) is not directly accessible through the kseg0 (cached) or kseg1 (uncached) windows. On a 32-bit CPU physical addresses above the low 512 MB are “high memory” in the Linux sense and can only be accessed through TLB mappings. With a MIPS CPU, you can create a few permanent mappings by defining “wired” TLB entries, protected from replacement. But Linux tries to avoid using resources that will quickly run out, so mainstream kernel code avoids wired entries completely. For Linux/MIPS, high-memory mappings are maintained dynamically by TLB entries created on demand.

9. Libraries and applications:

    Long ago, applications running on UNIX-like systems were monolithic pieces of code, which were loaded as required. You built them by compiling some source code and gluing in some library functions—prebuilt binaries provided with your toolchain. But there are two things wrong with that. One is that the library code is often bigger than the application that attaches to it, bloating all the programs. The other is that if a supplier fixes a bug in a library function, you don’t get full benefit from the fix until every software maintainer rebuilds his or her application. 

    Instead, the application is built without the library functions. The names of the missing libraries are built into the application, so the loader can find the required libraries and stitch them in when the application is loaded. So long as the library continues to provide identical functions, everything should be fine (there’s a library version–tracking system to allow libraries to evolve functionally, too, but that’s beyond our scope). 

    That carries a penalty. When you link a program at load time out of pieces (each of which may get separately updated), the exact address of the components is unpredictable at build time. You can’t predict in advance which locations will be available for loading a particular library. The runtime loader can do no better than to load each library in the next space available, so even the starting address for a library is unpredictable. A library binary has to be position-independent code or PIC—itmust run correctly wherever its code and data are positioned in virtual address space.

    We’ll discuss PIC code and the construction of application memory spaces in Chapter 16.

<h2 id="13.2">13.2 内核分层架构</h2>

From one point of view, the kernel is a set of subroutines called from exception handlers. The raw post-exception “exception mode” environment on a MIPS CPU is all-powerful and very low-overhead but tricky to program. So with each entry to the kernel you get something like a foreshortened bootstrap process, as each “layer” constructs the environment necessary for the next one. Moreover, as you exit from the kernel you pass through the same layers again, in reverse order, passing briefly through exception mode again before the final eret which returns you to userland.

Different environments in the kernel are built by more or less elaborate software which makes up for the limitations of the exception handler environment. Let’s list a few starting at the bottom, as the kernel is entered:

<h3 id="13.2.1">13.2.1 异常模式下的MIPS-CPU</h3>

Immediately after taking an exception, the CPU has SR(EXL) set—it’s in exception mode. Exception mode forces the CPU into kernel-privilege mode and disables interrupts, regardless of the setting of other SR bits. Moreover, the CPU cannot take a nested exception in exception mode except in a very peculiar way.

The first few instructions of an exception handler usually save the values of the CPU’s general-purpose registers, whose values are likely to be important to the software that was running before the exception. They’re saved on the kernel stack of the process that was running when the interrupt hit. It’s in the nature of MIPS that the store operations that save the register require you to use at least one general-purpose register first, which is why the registers called k0 and k1 are reserved for the use of exception handlers.

The handler also saves the values of some key CP0 registers: SR will be changed in the next section of the exception handler, but the whole at-exception value should be kept intact for when we return. Once that’s done, we’re ready to leave exception mode by changing SR, though we are going to leave interrupts disabled.

A CISC CPU like an x86 has no equivalent of exception mode; the work done in MIPS exception mode is done by hardware (really by invisible microcode). An x86 arrives at an interrupt or trap handler with registers already saved.

The software run in MIPS exception mode can be seen as producing a virtual machine that looks after saving the interrupted user program’s state immediately after an exception and then restores it while preparing for the eret, which will take us back again. Programmers need to be very careful what they do in exception mode. Exceptions are largely beyond the control of the software locks that make the kernel thread-safe, so exception code may only interact very carefully with the rest of the kernel.

In the particular case of the exception used to implement a system call, it’s not really necessary to save GP registers at all (so long as the exception handler doesn’t overwrite the s0–s8 “saved” registers, that is). In a system call or any noninterrupt exception, you can call straight out to code running in thread context.

Some particularly simple exception handlers never leave exception mode. Such code doesn’t even have to save the registers (it just avoids using most of them). An example is the “TLB refill” exception handler described in section 14.4.8.

It’s also possible—though currently unusual—to have an interrupt handler that runs briefly at exception level, does its minimal business, and returns. But such an interrupt handler has no real visibility at the OS level, and at some point will have to cause a Linux-recognized interrupt to get higher-level software working on its data.


<h3 id="13.2.2">13.2.2 禁止中断或部分中断</h3>

As we’ll see in the next chapter, an interrupt routine exits exception mode but continues to run with at least some interrupts disabled.

Running with all interrupts disabled is a costly but effective way of getting a single CPU to be nonpreemptive (the longest time software spends with interrupts disabled determines your worst-case interrupt latency, and every device driver with a real-time constraint must budget for it). And of course it doesn’t prevent re-entrance where there’s a second CPU at work.

The simplest, shortest kind of ISR may opt to run to completion without ever re-enabling interrupts—Linux can support this and calls it a fast interrupt handler. You get that behavior by setting the flag SA INTERRUPT when registering the ISR. But most run for a while with higher-priority interrupts enabled.

Potentially, you can get a stack of interrupts interrupting interrupts. Infinite recursion (and stack overflow and an inevitable crash) can’t happen because Linux makes sure you can stack up at most one entry at each distinct interrupt level. The amount of data saved at each level must be small enough that the maximum stack of interrupt save information will not overfill a thread’s kernel stack.

<h3 id="13.2.3">13.2.3 中断上下文</h3>

After an interrupt, even after the interrupt handler has re-enabled most interrupts and built a full C environment, interrupt code is still limited because it’s borrowing the state (and kernel stack) of whichever thread happened to be interrupted.

Servicing an interrupt is someone’s business, certainly, but it has no systematic relationship with the thread that is executing when the interrupt happens. An interrupt borrows the kernel stack of its victim thread and runs parasitically on that thread’s environment. The software is in interrupt context, and to prevent unreasonable disruption, interrupt-context code is restricted in what it can do.

One vital job done by the kernel is the scheduler, which determines which thread the OS should run next. The scheduler is a subroutine, called by a thread; in some cases it’s called by a thread in interrupt context. Once the interruptcontext part of an interrupt handler can get to the point where the hardware’s immediate needs are met, it can (and often does) schedule a thread that will complete the interrupt-handling job, this time in thread context.

<h3 id="13.2.4">13.2.4 异常模式下的MIPS-CPU</h3>

You can arrive in the kernel in thread context either when an application has made a voluntary system call or a forced call for resources on a virtual memory exception (and the system call or VM exception has emerged from its lower layers), or as a result of a reschedule—which is, in turn, always either caused by an interrupt or by another thread voluntarily rescheduling itself because it’s waiting for some event.

System calls are a sort of “subroutine call with security checks.” But a range of other exceptions—notably virtual memory maintenance exceptions—are very much the same, even though the application didn’t know this particular system call was necessary until it got the exception.

Not every thread is an application thread. Special threads with no attached application can be used to schedule work in the kernel in process context for device management and other kernel functions.

Thread context is the “normal” state of the kernel, and much effort is spent making sure that most kernel execution time is spent in this mode. An interrupt handler’s “bottom half ” code, which is scheduled into a work queue (see section 14.1), is in thread context, for example.
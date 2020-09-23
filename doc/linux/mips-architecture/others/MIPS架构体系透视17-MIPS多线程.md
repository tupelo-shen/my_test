[TOC]

Multithreading hardware was 2004–2005’s “Next Big Thing” in computers. Not so in 2006: Virtualization looks like this year’s NBT, a crown it last held around 1975–1980. The crest of the multithreading hype wave has gone past, and multithreading still looks pretty important.

So let’s attempt to define what it is, and then why it’s useful.We’ll use “multithreading” without the hyphen, and use the abbreviation “MT” to refer to the MIPS32/64 architecture extension (the MT ASE).

# 17.1 What Is Multithreading?

A thread is `a sequence of instructions executed in the order the programmer intended`.

That seems a bit simple, really, though it leaves a “thread” as a thing that is probably pretty tangled (there are muddied paths following the call tree of subroutines and loops within loops). But when a conventional CPU takes an interrupt, that isn’t the same thread (at least, not until and unless the interrupt handler returns). And what operating systems variously call “processes,” “tasks,” or “jobs” are all separate threads.

You only need to define a thread when you intend to have more than one (otherwise it’s just the program the CPU is running). It’s easy to see why you might want more than one thread on a timeshared computer where different users were impatient to get their programs run.

Sometimes you want a computer to run a single program, but one that must reconcile competing needs to respond. Some problems are inherently concurrent. You can (in theory)write an explicit program using some kind of multiway select call to wait for one of a number of events, figure out the event, and call the appropriate subroutine. Or maybe you can invent a concurrent programming language.

But explicitly concurrent programs are very hard to write, and no concurrent language ever took off. In the 1970s, research into practical ways to design, write, and test programs that dealt with concurrent events arrived at a conclusion. Researchers homed in on the idea of usingmultiple simple threads that influence each other only through simple, stylized interfaces. 

Furious internecine wars were fought over whether those interfaces should be based on message passing or something simpler like semaphores: The wars have long since been abandoned, and everybody won. Modern OSs contain a variety of communication mechanisms, though different programming communities seem to favor different subsets.

The word thread emerged as a term distinct from “process” in order to free the thread from other baggage: UNIX processes each had their own address space, but threads were things that might share an address space. The POSIX standard 1003.1 (known as “PThreads”) proposed a rich cross-OS programming interface for threads that explicitly share code and data, and PThreads has been substantially accepted by a wide range of modern operating systems.

## 17.1.1 Which Resources Do You Need to Run Two Threads at Once?

Two complete CPUs will do it, of course. But that seems a lot: What’s the minimum you need?

You can get a good clue about this by looking at the information kept about each thread by a multithreading OS (on a conventional CPU). Look at a thread when it’s not running, and you’ll find a PC (the address the thread will execute from when it next starts up) and saved values of all the programmer-visible registers. For user-privilege programs on MIPS, that’s the general-purpose registers and the hi/lo accumulator of themultiplier. The OS will need to keep some kind of identifier as to which thread is running, as well as a flag to say whether it’s currently holding kernel privilege (as itwill be when executing a system call). A MIPS OS maintaining multiple address spaces will need the ASID value too, because it’s used by the hardware when translating addresses.

That’s about it: PC, general-purpose registers, thread ID, kernel mode, ASID. An OS calls this the thread context, and at a minimum each thread must have its own copy of those registers and fields.

There’s another angle you can take. If you want someone to buy your multithreading hardware, it helps if your hardware can run existing software. Dual-processor x86 PC hardware (particularly the closely related x86 small-server hardware) has been around for a few years, and Microsoft Windows, Linux, and other UNIX OSs already know how to exploit that hardware. So when Intel introduced multithreading in its “Hyperthreading” CPUs, it augmented the hardware so that a system with one multithreading CPU would run software built for a dual-CPU system, unchanged. To do the same thing for MIPS, you’d need to provide separate copies of every register to each “virtual processor.”

That’s two different approaches to multithreading. Which should MIPS adopt? Before we come to that, let’s take a belated look at why it’s worth doing at all.

# 17.2 Why Is MT Useful?

Conventional CPUs in 2005–2006 are hitting the “memory wall.” The time taken to run an instruction has gone down much faster than the time taken to access memory. How much faster? Intel’s 32-bit x86 CPUs have gone from running at 16 MHz in 1985 to about 3 GHz in 2005—200 times faster in 20 years. During that time memory access time has dropped too, but by rather less than a factor of four, from about 180 ns in 1985 to 50 ns now.[^1]

Roughly speaking, that means a memory read that stopped a CPU for four cycles in 1985 now stops it for 150 cycles. Vast improvements in caching and clever CPU designs that run as far as possible ahead of the returning data have covered up the problem, but the scope for further improvement is limited. It’s normal for a CPU to spend well over half its time idle, waiting for data from memory.

Embedded systems don’t run at anything like 3 GHz, but they have simpler caches and CPU designs. Contemporary embedded systems, too, tend to have CPUs that are stalled out 50 percentage of the time.

There’s another reason why multithreading hardware might be useful for embedded systems. If you have some very demanding I/O service requirement, a thread dedicated to that requirement can respond instantly. You can “park” the thread reading some value from the I/O system, and as soon as the data is ready it will leap back to life. There’s no interrupt overhead and—more importantly—potentially no OS overhead.Who says the I/O thread must run the standard OS code?

So multithreading might be a good thing. How do we do it? And is it a good bargain? As usual for embedded systems the biggest cost factors are silicon area and power consumption.

# 17.3 How to Do Multithreading for MIPS

RISC CPUs like to leave the software in charge, rather than moving policy decisions into hardware. RISC principles favor flexible and general-purpose solutions. That motivates a number of features of MIPS MT:

* Expose thread registers to each other:

    This gives the OS total control over multithreading. `mftr` and `mttr` are privileged instructions (“move from/to thread register”) that allow OS software to get at the registers of a different TC.

* Start up single-threaded:

    This respects the principle of least surprise for bootstrap software. OS or bootstrap software can wake more threads when it feels ready.

* Provide both sorts of multithreading:

    Both minimal-overhead thread engines and complete “virtual CPUs” can be mixed and matched. The basic MT feature is the minimal thread engine, known as a TC (inspired by “thread context”). If we stopped there, everything in the CPU beyond the immediate thread context would be shared.

    Instead, we permit MT CPUs to replicate everything required to make a MIPS32/64-compliant CPU. One of these CPU-like sets of registers and other resources is called a VPE: One or more TCs affiliate to the VPE and share its registers and resources, and the TC(s) in the VPE make a “virtual processing element,” which leads to the acronym. A CPU can implement more than one VPE to produce something that looks (to software) like multiple CPUs.

    MIPS Technologies’ first MT product, the 34-K CPU, can have up to five TCs distributed at will between two VPEs.

    Many things can be shared between the VPEs while leaving them software-equivalent to a MIPS32 CPU, of course: This is not a real dual CPU, it only looks like one. The caches, main pipeline, control logic, arithmetic/logic units, and system interface are all shared. The TLB can either be hard-partitioned between the VPEs, or shared (the shared-TLB version involves some localized changes to the OS).

Each instruction being run by a machine has a TC number, which selects its particular context. Whenever the instruction accesses some state—reads or writes a general-purpose register, for example—it uses its TC number to extend the register-number field, which is already defined inside the instruction. An instruction sees a different set of registers depending on the TC number: It’s very simple, and it just works.

MIPS MT is not quite that simple, because of the TC/VPE trick mentioned above. So this instruction might be for TC #5 (it uses general-purpose registers from the fifth bank) or VPE #1 (it gets CP0 registers from the first bank). Again, this should just work.What’s more complicated, of course, is to get those CPU resources working, which can’t simply be reduced to registers. But that’s not architecture, it’s implementation; you’ll have to read CPU manuals for that stuff.

The MT specification doesn’t require it, but effective MT CPUs need to be able to change threads very quickly, or time lost in thread changes will eat away at the potential throughput gain. Other things being equal, it will usually be best to mix threads on as fine grain as is possible: In a single-issue CPU, that means issuing an instruction from a different thread in each clock period so long as more than one thread is ready.

## 17.3.1 New CP0 Registers for MT

They come in three groups: those provided for each TC, those provided for each VPE, and a couple that exist per-CPU. The latter are registers that provide an inventory of the CPU’s resources (how many TCs? how many VPEs? and so on)and allow you to share them out; they are not described further here.

The per-TC registers include:

* TCHalt: 

    A 1-bit register, write 1 to halt the target. With a target TC halted, its state is stable and it’s safe to write its registers.

* TCRestart:

    The thread “PC”—when the thread is halted, this is the address of the first instruction it will run when it next runs. When not halted, it may not make much sense.

* TCStatus:

    Per-TC “legacy” fields—kernel/user status, ASID, and instruction set options flags (such as the one that enables floating point). There’s also a flag to indicate that the instruction the thread stopped before is in a branch delay slot (in which case TCRestart points at the preceding branch).

* TCBind:

    Contains the ID of the VPE to which this TC is affiliated (writable in a 34-K family CPU), and a read-only ID for this TC.

* TCContext:

    A pure software read/write register, typically used to store an OS-specific thread ID.

The per-VPE register VPEControl is for control fields you might reasonably change during the running of an OS, while VPEConf0-1 have configuration fields you’d most likely set once and then leave alone.

## 17.3.2 Exceptions and Interrupts

An exception in a single-threaded MIPS architecture CPU is usually quite disruptive in the pipeline and is commonly implemented by discarding a lot of execution state (pipelines get flushed and instructions discarded). An exception on a MIPS MT machine happens within a thread context—and other threads (at least those on separate VPEs) expect to continue undisturbed. So you’d expect there to be some difficulties when we redefine exceptions on a multithreading machine.

Bear in mind that an OS is a program (a set of threads, in fact). It’s likely not to matter which TC happens to execute some part of it. As we observed at the beginning of this appendix, each exception handler is a thread in its own right. So to run an exception handler, we need to borrow a TC to interrupt its thread and run this one instead.

There are two types of exceptions. Interrupts are asynchronous—they happen for reasons unconnectedwith any particular instruction. But most other exceptions are synchronous—they’re associated with a particular instruction. That’s what we’ll look at first.

Synchronous exception handlers are run by the TC whose instruction caused the exception. The TC immediately ceases work on its normal thread and starts fetching instructions from the appropriate exception handler. 

The MIPS MT ASE requires that once a TC enters exception state, all the other TCs within the same VPE are suspended. None of the other TC’s instructions may be executed until the the exception handler leaves exception mode: that is, until SR(EXL) is cleared either by the eret at the end of the exception handler or as a result of the exception handler branching off to some less restrictively coded part of the OS. It’s already good practice for exception handlers to minimize the amount of time spent in exception mode, and most OS code does that.

But if your application needs to maximize concurrency, you should consider minimizing exceptions—you may be able to use a thread blocked on an ITC access or yield condition instead. And, of course, arrange that exception handlers (as soon as they can) save the state necessary that they can drop back out of exception mode.

## 17.3.3 MIPS MT and Interrupts

In the MIPS architecture, interrupt management is by CP0 registers (in particular, Cause and SR). Those registers are replicated per-VPE, not per-TC, so interrupt masking and steering are managed per-VPE. Even the interrupt signal wiring into the core is per-VPE.

Each interrupt input may be connected to just one VPE or to all of them, so which VPE receives an interrupt comes down to how the system is wired up, but possibly also to the software configuration. If you connect and unmask an interrupt on multiple VPEs, any number of them may take the interrupt exception—you probably don’t want that to happen, so either don’t connect or don’t enable some of them.

The interrupt exception may be taken by any available TC associated with the VPE.

The MIPS architecture already provides multiple ways to refuse an interrupt exception. An interrupt to any thread from this VPE can be prevented by exception mode, a global interrupt-enable flag (whichmay be zero), and per-interrupt mask bits: that’s SR(EXL), SR(IE), and SR(IM)—and that list is not exhaustive.

The MIPSMTspecification adds yet another reason not to take an interrupt. You can now set a new per-TC CP0 register field TCStatus(IXMT) (interrupt exempt), which will prevent the particular TC from being used for an interrupt exception.

## 17.3.4 Thread Priority Hints

Some application developers express interest in being able to steer the provision of CPU bandwidth to one thread rather than another. The MIPS MT specification provides for this, though it doesn’t say how it should be done. A scheduling policy manager (often PM) is a piece of logic outside the CPU, customizable for a particular application, that generates a 2-bit priority for each TC. Inside the core, a runnable higher-priority TC will always be preferred over a runnable lower-priority one.

It’s too early to comment on how successful developers will be at using this scheme. 

## 17.3.4 User-Privilege Dynamic Thread Creation—the Fork Instruction

An interesting but commercially unexplored application of multithreading hardware is to provide another way of discovering and exploiting parallelism in sequential algorithms. For example, a loop might be optimized by having two threads running its odd- and even-numbered iterations, respectively.

If that’s to be done under the aegis of a protected OS, this requires a very efficient mechanism for firing off a new thread on demand. MIPS MT’s fork instruction provides such a mechanism.

So `fork d,s,t` is a user-privileged instruction. If all is well, it starts a thread on a ready-to-fork TC, which starts execution at the instruction pointed to by s. The child thread’s d receives the value from the parent’s t.

The OS enables this by maintaining a pool of ready-to-fork but otherwise idle TCs (they’re distinguished by a flag bit TCStatus(DA)). After the new thread has done its work in the application, it terminates and returns the TC to the pool using a `yield $0` instruction.

`fork` is a forward-looking feature: At the time of writing, no OS yet supports such a pool of TCs. It’s there so that the MIPS MT architecture can be well placed as multithreading ideas spread.

# 17.4 MT in Action

It’s early days yet, but what are people doing with multithreading and, particularly, with MIPS MT? 

Broadly speaking, there are two different kinds of systems being explored and/or developed. One seeks to use modified standard SMP OS code (perhaps quite lightly modified) to provide something well suited to provide multithreading efficiency to a relatively conventional multiple-thread workload; the other seeks to build “underneath” the OS to provide a low-level, ultra-responsive “real-time” environment for novel applications.

## 17.4.1 SMP Linux

It’s possible to port an SMP Linux to an MT CPU with two or more TCs in the role of the SMP system’s CPUs. If you’ve got an application that already uses the Linux implementation of the PThreads threading API (it’s called NPTL), then you can just drop that application onto such a Linux and have it exploit multithreading.

SMP systems need locks and semaphores that work between the CPUs, and for MIPS systems, those are built using the load-linked/store-conditional instruction pair (ll/sc, described in section 8.5.2). So this wouldn’t work unless the MT CPU preserved the ll/sc semantics between TCs: It does. The 34-K family hardware does that by keeping track of an address used by an ll issued by any TC, and breaking the link if any other TC’s store modifies an address sharing the same doubleword. The link bit is also broken if some management software takes over the TCand reschedules it bywriting its TCRestart register.

It’s very easy to do the SMP port if the TCs are in separate VPEs, because a TC alone in a VPE, by design, looks like a separate CPU. But MIPS Technologies has also demonstrated it using TCs in the same VPE. There are some difficulties here but they appear not to cause much loss of performance.

It’s important to note, though, that only some applications benefit directly. You need an application that uses the Linux environment, exploits threading, and where for significant periods of time there are not only two threads running, but two threads that divide up the CPU’s efforts fairly evenly between them.

## 17.4.2 Highly Responsive Programming with MT

Another approach being taken by MT pioneers is to use dedicated TCs to provide close-to-hardware intelligence, whose response time can be independent of whatever might be going on in the OS running on another TC in the other VPE. Such a system promises the best of both worlds: a convenient programming environment for the high-level code, but instant response to events that make the hardware simpler at the low end.

Low-end code on such a system is much in need of efficient synchronization mechanisms. Even an I/O read is snail-paced compared with the execution speed of a microprocessor. MIPS MT defines two mechanisms: yield-on-signal and gating storage.

The yield instruction, as hinted above, has a double life. When it’s not doing the opposite of fork to provide user-privilege thread termination, it provides a way to cause a thread to wait for a hardware event, defined as the assertion of an external signal sampled into the CPU.

The 34-K CPU can sample any of 15 external signals. A yield instruction waits until one of any selection of them is asserted, based on an input argument register whose value is interpreted as a 15-bit vector: A “1” bit makes it sensitive to the correspondingly numbered input signal. A low-level thread waiting on a yield instruction starts instantly when one of its signals is asserted.

But you might want a synchronization mechanism that also transfers data and is useful between software threads. In that case, the gating storage interface helps. MIPSMTdefines gating storage as a special physical address region where both load and store instructions become blocking. A thread that reads or writes a gating storage location blocks on that load or store until data is successfully transferred.Moreover, an incomplete operation can be terminated (causing any waiting thread to receive an exception).

The gating storage interface is suitable for hardware interfaces where a high-speed data flow may be interrupted by error or end-of-block conditions.

The 34-K CPU design is licensed togetherwith a gating storage system called ITC (for interthread communication memory): It provides software-to-software communication using “empty/full” storage or short FIFOs. For more information, look at the CPU manuals from MIPS Technologies.


[^1]: Memory bandwidth has risen at a much higher rate, but “Money can buy bandwidth. Latency requires bribing God.”—in this case, technology stands in for money.

[^2]: yield fed with a nonzero value from its source register operand has a quite distinct application.
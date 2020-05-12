<h1 id="0">0 目录</h1>

* [5.1 精确异常](#5.1)
    - [5.1.1 非精确异常-历史上的MIPS架构CPU的乘法器](#5.1.1)
* [5.2 异常发生的时机](#5.2)
* [5.3 异常向量：异常处理开始的地方](#5.3)
* [5.4 异常处理：基本过程](#5.4)
* [5.5 从异常返回](#5.5)
* [5.6 嵌套异常](#5.6)
* [5.7 异常处理例程](#5.7)
* [5.8 中断](#5.8)
    - [5.8.1 MIPS架构CPU上的中断资源](#5.8.1)
    - [5.8.2 在软件中实现中断优先级](#5.8.2)
    - [5.8.3 原子性以及对SR的原子修改](#5.8.3)
    - [5.8.4 使能中断状态下的临界区：MIPS式的信号量](#5.8.4)
    - [5.8.5 MIPS32/64架构CPU的中断向量化和EIC中断](#5.8.5)
    - [5.8.6 影子寄存器](#5.8.6)
* [5.9 启动](#5.9)
    - [5.9.1 检测识别CPU](#5.9.1)
    - [5.9.2 引导步骤](#5.9.2)
    - [5.9.3 启动应用程序](#5.9.3)
* [5.10 指令仿真](#5.10)

---

MIPS架构中，中断、异常、系统调用以及其它可以中断程序正常执行流的event统称为`异常（exception）`，统一由异常处理机制进行处理。

> <font color="blue">
> 异常和中断概念在不同架构上的含义的区别：
> 
> 1. MIPS架构将所有可以中断程序执行流的`event`称为异常；
> 2. X86架构将所有可以中断程序执行流的`event`称为中断，我们日常所见的狭义上的中断，也就是外部中断，称之为`异步中断`；而狭义上的异常称为`同步中断`；
> 3. ARM架构将这两个概念合起来使用-`异常中断`类似于MIPS架构的`异常`概念。
> 
> 在阅读相关书籍的时候，请注意区分这些概念。
> </font>

MIPS架构所涉及的event，都有哪些呢？

* 外部事件

    Some event outside the CPU core—that is, from some real “wire” input signal. These are interrupts.1 Interrupts are used to direct the attention of the CPU to some external event: an essential feature of an OS that attends to more than one different event at a time.
    来自CPU核外的外部中断。

    Interrupts are the only exception conditions that arise from something independent of the CPU’s normal instruction stream. Since you can’t avoid interrupts just by being careful, there have to be software mechanisms to inhibit the effect of interrupts when necessary.

* Memory translation exceptions:

    These happen when an address needs to be translated, but no valid translation is available to the hardware or perhaps on a write to a write-protected page.

    The OS must decide whether such an exception is an error or not. If the exception is a symptom of an application program stepping outside its permitted address space, it will be fixed by terminating the application to protect the rest of the system. The more common benign memory translation exceptions can be used to initiate operating system functions as complex as a complete demand-paged virtual memory system or as simple as extending the space available for a stack.

* Other unusual program conditions for the kernel to fix: 

    Notable among these are conditions resulting fromfloating-point instructions, where the hardware is unable to cope with some difficult and rare combination of operation and operands and is seeking the services of a software emulator. This category is fuzzy, since different kernels have different ideas about what they’re willing to fix. An unaligned load may be an error on one system and something to be handled in software on another.

* Program or hardware-detected errors:

    This includes nonexistent instructions, instructions that are illegal at user-privilege level, coprocessor instructions executed with the appropriate SR flag disabled, integer overflow, address alignment errors, and accesses outside kuseg in user mode.

* Data integrity problems:

    Many MIPS CPUs continually check data on the bus or data coming from the cache for a per-byte parity or for wordwide error-correcting code. Cache or parity errors generate an exception in CPUs that support data checking.

* System calls and traps:

    These are instructions whose whole purpose is to generate recognizable exceptions; they are used to build software facilities in a secure way (system calls, conditional traps planted by careful code, and breakpoints).

Some things do not cause exceptions, though you’d expect them to. For example, you will have to use other mechanisms to detect bus errors on write cycles. That’s because most modern systems queue up writes: Any writeassociated error would happen much too late to relate to the instruction that caused it. Systems that do inform you about errors on writes must use some outside-the-CPU hardware, probably signaled with an interrupt.

In this chapter, we’ll look at how MIPS CPUs decide to take exceptions and what the software has to do to handle them cleanly. We’ll explain why MIPS exceptions are called “precise,” discuss exception entry points, and discuss some software conventions.

Hardware interrupts fromoutside theCPUare the most commonexceptions for embedded applications, the most time critical, and the ones most likely to cause subtle bugs. Special problems can arise with a nested exception, one that happens before you have finished handling an earlier exception.

The way that a MIPS CPU starts up after system reset is implemented as a kind of exception and borrows functions from exceptions—so that’s described in this chapter too.

At the end of the chapter, we’ll look at a couple of related subjects: how to emulate an instruction (as needed by an instruction set extension mechanism) and how to build semaphores to provide robust task-to-task communication in the face of interrupts. Chapter 14 describes in some detail how interrupts are handled in a real, grown-up OS for MIPS.

<h2 id="5.1">5.1 精确异常</h2>

You will see the phrase precise exceptions used in the MIPS documentation. It is a useful feature, but to understand why, you need to meet its alternative.

In a CPU tuned for the best performance by pipelining (or by more complicated tricks for overlapping instruction execution), the architecture’s sequential model of execution is an illusion maintained by clever hardware. Unless the hardware is designed cleverly, exceptions can cause this illusion to unravel.

When an exception suspends its thread of execution, a pipelined CPU has several instructions in different phases of completion. Since we want to be able to return fromthe exception and carry onwithout disruption to the interrupted flow of execution, each instruction in the pipeline must be either completed or made as though we never saw it.2 Moreover, we need to be able to remember which instruction falls in each of those categories.

A CPU architecture features precise exceptions when it prescribes a solution to this problem that makes life as easy as possible for the software. In a preciseexception CPU, on any exception we are pointed at one instruction (the exception victim). All instructions preceding the exception victim in execution sequence are complete; but it’s as if the exception victim and any successors were never started.3 When exceptions are precise, the software that handles exceptions can ignore all the timing effects of the CPU’s implementation.

The MIPS architecture comes close to prescribing that all exceptions are precise. Here are the ingredients:

* Unambiguous proof of guilt:

    After any exception, the CPU control register EPC points to the correct place to restart execution after the exception is dealt with. In most cases, it points to the exception victim, but if the victim was in a branch delay slot, EPC points to the preceding branch instruction: Returning to the branch instruction will re-execute the victim instruction, but returning to the victim would cause the branch to be ignored. When the victim is in a branch delay slot, the cause register bit Cause(BD) is set.

    It may seem obvious that it should be easy to find the victim, but on some heavily pipelined CPUs it may not be possible.

* Exceptions appear in instruction sequence:

    This would be obvious for a nonpipelined CPU, but exceptions can arise at several different stages of execution, creating a potential hazard. For example, if a load instruction suffers an address exception, this won’t happen until a pipeline stage where the address translation would have been complete—and that’s usually late. If the next instruction hits an address problem on an instruction fetch (right at the start of the pipeline), the exception event affecting the second-in-sequence instruction will actually happen first. To avoid this problem, an exception that is detected early is not acted on immediately; the event is just noted and passed along the pipeline. In most CPU designs, one particular pipeline stage is designated as the place you detect exceptions. If an older instruction’s late-detected event reaches this finish line while our exception note is making its way down the pipeline, the exception note just gets discarded. In the case above, the instruction-fetch address problem is forgotten—it will likely happen again when we finish handling the victim instruction’s problem and re-execute the victim and subsequent instructions.

* Subsequent instructions nullified:

    Because of the pipelining, instructions lying in sequence after the victim at EPC have been started. But you are guaranteed that no effects produced by these instructions will be visible in the registers or CPU state, and no effect at all will occur that will prevent execution, properly restarted at EPC, from continuing just as if the exception had not happened.

    The MIPS implementation of precise exceptions is quite costly, because it limits the scope for pipelining. That’s particularly painful in the FPU, because floating-point operations often take many pipeline stages to run. The instruction following a MIPS FP instruction can’t be allowed to commit state (or reach its own exception-determination point) until the hardware can be sure that the FP instruction won’t produce an exception.


5.1.1 Nonprecise Exceptions—The Multiplier in Historic MIPS CPUs

<h3 id="5.1.1">5.1.1 非精确异常-历史上的MIPS架构CPU的乘法器</h3>

MIPS’s original multiply/divide instructions are started by instructions like mult or div, which take two register operands and feed them into the multiplier machine. The program then issues an mflo instruction (and sometimes also mfhi, for a 64-bit result or to obtain the remainder) to get the results back into a general-purpose register. The CPU stalls on mflo if the computation is not finished.

In MIPS implementations, a multiply takes 4–10 cycles, but divide may take 15–30.

By dividing these long-latency instructions into two stages (“launch” and “get results”), the instruction set encourages the use of amultiply machine separately pipelined from the regular integer unit. Later CPUs provided a richer set of instructions, including multiply-accumulate instructions, which give software different ways of exploiting the pipeline—see section 8.5.5 for details.

On modern CPUs conforming to MIPS32/64, the instructions behave themselves. But older CPUs can showa problem. To make the hardware simpler, the original architecture allowed amultiply/divide operation to be unstoppable, even by an exception. That’s not normally a problem, but suppose we have a code sequence like the following, in which we’re retrieving one multiply unit result and then immediately firing off another operation:

    mflo $8
    mult $9, $10

If we take an exception whose restart address is the mflo instruction, then the first execution of mflo will be nullified under the precise-exception rules, and the register $8 will be left as though the mflo had never happened. Unfortunately, the mult will already have been started too, and since the multiply unit knows nothing of the exception it will continue to run. Before the exception returns, the computation will most likely have finished and the mflo will now deliver the result of the mult that should have followed it.

We can avoid this problem (inherent in most pre-MIPS32 CPUs), by interposing at least two4 nonmultiply instructions between the mflo/mfhi on the one hand and the mult (or any other instruction that will write new values to hi/lo) on the other.

<h2 id="5.2">5.2 异常发生的时机</h2>

Since exceptions are precise, the programmer’s view of when an exception happens is unambiguous: the last instruction executed before the exception was the one before the exception victim. And, if the exception wasn’t an interrupt, the victim is the instruction that caused it.

On an interrupt in a typical MIPS CPU, the last instruction to be completed before interrupt processing starts will be the one that has just finished its MEM stage when the interrupt is detected. The exception victim will be the one that has just finished its ALU stage. However, take care: MIPS architects don’t make promises about exact interrupt latencies, and signals may be resynchronized through one or more clock stages before reaching the CPU core.

5.3 Exception Vectors:Where Exception Handling Starts
<h2 id="5.3">5.3 异常向量表：异常处理开始的地方</h2>

Most CISC processors have hardware (or concealed microcode) that analyzes an exception, dispatching the CPU to different entry points according to what kind of exception happened. When even interrupts are handled at different entry points according to the interrupt input signal(s) activated, that’s called vectored interrupts. Historically, MIPS CPUs did very little of this. If that seems a serious omission, consider the following.

First, vectored interrupts are not as useful in practice as we might hope. In most operating systems, interrupt handlers share code (for saving registers and such like), and it is common for CISC microcode to spend time dispatching to different interrupt entry points, where OS software loads a code number and spends a little more time jumping back to a common handler.

Second, it’s difficult to envisionmuch exception analysis being done by pure hardware rather than microcode; on a RISC CPU, ordinary code is fast enough to be used in preference.

Here and elsewhere, you should bear in mind just howfastCPUs of the RISC generation are compared with their peripherals. A useful interrupt routine is going to have to read/write some external registers, and on an early 21st-century CPU, that external bus cycle is likely to take 50–200 internal clock cycles. It’s easy to write interrupt dispatch code on a MIPS CPU that will be faster than a single peripheral access, so this is unlikely to be a performance bottleneck. That’s probably emphasized by the fact that a vectored interrupt option in the 2003 revision of MIPS32 has found little use yet.

However, even in MIPS not all exceptions were ever equal, and differences have grown as the architecture has developed. So we can make some distinctions:

* TLB refill of user-privilege address:

    There is one particularly frequent exception in a protected OS, related to the address translation system (see Chapter 6). The TLB hardware only holds a modest number of address translations, and in a heavily used system running a virtual memory OS, it’s common for the application program to run on to an address whose translation is not recorded in the TLB—an event called a TLB miss (because the TLB is used as a software-managed cache).

    The use of software to handle this condition was controversial when RISC CPUs were introduced, and MIPS CPUs provide significant support for a preferred scheme for TLB refill. The hardware helps out enough that the exception handler for the preferred refill scheme can run in as few as 13 clock cycles.

    As part of this, common classes of TLB refill are given an entry point different from all other exceptions, so that the finely tuned refill code doesn’t have to waste time figuring out what kind of exception has happened.

* TLB refill for 64-bit address spaces:

    Memory translation for tasks wanting to take advantage of the larger programaddress space available on 64-bit CPUs uses a slightly different register layout and a different TLB refill routine; MIPS calls this an XTLB refill (“X” for extended, I guess). Again, a desire to keep this very efficient makes a separate entry point useful.

* Uncached alternative entry points:

    For good performance on exceptions, the interrupt entry point must be in cached memory, but this is highly undesirable during system bootstrap; from reset or power-up, the caches are unusable until initialized. If you want a robust and self-diagnosing start-up sequence, you have to use uncached read-only memory entry points for exceptions detected in early bootstrap. In MIPS CPUs there is no uncached “mode”—there are uncached program memory regions instead—so there’s a mode bit SR(BEV) that reallocates the exception entry points into the uncached, start-up-safe kseg1 region.

* Parity/ECC error: 

    MIPS32 CPUs may detect a data error (usually in data arriving from main memory, but often not noticed until it’s used from cache) and take a trap. It would be silly to vector through a cached location to handle a cache error, so regardless of the state of SR(BEV) the cache error exception entry point is in uncached space.

* Reset:

    For many purposes, it makes sense to see reset as another exception, particularly when many CPUs use the same entry point for cold reset (where the CPU is completely reconfigured; indistinguishable from power-up) and warm reset (where the software is completely reinitialized). In fact, nonmaskable interrupt (NMI) turns out to be a slightly weaker version of warm reset, differing only in that it waits for the current instruction and any pending load/store to finish before taking effect.

* Interrupt:

    As an option in MIPS32 (and some earlier CPUs from IDT and PMC-Sierra), you can set the CPU to dispatch interrupt exceptions to a separate entry point. This is convenient, though little used: Perhaps software authors can’t bring themselves to special-case their OS for a feature that is not universally available.

    Further, in some of these CPUs you can enable vectored interrupt operation—multiple entry points to be used by different interrupts. This is a more substantial change; as explained elsewhere in this chapter, the MIPS tradition was that interrupts were only prioritized in software. But if you have two active interrupts and have to choose an interrupt entry point, the hardware must decide which has the higher priority. This change is therefore significantly more disruptive to software, since the software loses control over interrupt priority; your OS maintainer and hardware engineers will have to liaise closely.

All exception entry points lie in untranslated regions of the MIPS memory map, in kseg1 for uncached entry points and in kseg0 for cached ones. The uncached entry points used when SR(BEV) is set are fixed, but when SR(BEV) is clear, the EBase register can be programmed to shift all the entry points—together—to some other block. It’s particularly useful to be able to move the interrupt base when your CPU is part of a multiprocessor system sharing the kseg0 memory but wants to have separate exception entry points from the other CPUs in the system.

In these areas the nominal 32-bit addresses given in Table 5.1 extend to a 64-bit memory map by sign extension: The program address 0x8000.0000 in the 32-bit view is the same as 0xFFFF.FFFF.8000.0000 in the 64-bit view.

Table 5.1 describes the entry points with just 32-bit addresses—you need to accept that BASE stands for the exception base address programmed by the EBase register.

Presumably the default 128-byte (0x80) gap between the original exception vectors occurs because the original MIPS architects felt that 32 instructions would be enough to code the basic exception routine, saving a branch instruction without wasting too much memory! Modern programmers are rarely so frugal.

Here’s what a MIPS CPU does when it decides to take an exception:

1. It sets up EPC to point to the restart location.

2. It sets SR(EXL),which forces the CPU into kernel (high-privilege) mode and disables interrupts.

3. Cause is set up so that software can see the reason for the exception. On address exceptions, BadVAddr is also set. Memory management system exceptions set up some of the MMU registers too; more details are given in Chapter 6.

4. The CPU then starts fetching instructions from the exception entry point, and everything else is up to software.

Very short exception routines can run entirely with SR(EXL) set (in exception mode, as we’ll sometimes say) and need never touch the rest of SR. For more conventional exception handlers, which save state and pass control over to more complex software, exception level provides a cover under which system software can save essential state—including the old SR value—in safety.

With a couple of tweaks this mechanism can allow a minimal nested exception within the primitive TLB miss handler, but we’ll talk more about how that’s done when we get to it.

<h2 id="5.4">5.4 异常处理：基本过程</h2>

Any MIPS exception handler routine has to go through the same stages:

* Bootstrapping:

    On entry to the exception handler, very little of the state of the interrupted program has been saved, so the first job is to make yourself enough room to do whatever it is you want without overwriting something vital to the software that has just been interrupted. Almost inevitably, this is done by using the k0 and k1 registers (which are conventionally reserved for the use of low-level exception handling code) to reference a piece of memory that can be used for other register saves.

* Dispatching different exceptions:

    Consult Cause(ExcCode), whose possible values are listed inTable 3.2. It tells youwhy the exception happened, and allows an OS to define separate functions for the different causes.

* Constructing the exception processing environment:

    Complex exceptionhandling routines will probably be written in a high-level language, and you will want to be able to use standard library routines. You will have to provide a piece of stack memory that isn’t being used by any other piece of software and save the values of any CPU registers that might be both important to the interrupted program and that called subroutines are allowed to change.

    Some operating systems may do this before dispatching different exceptions.

* Processing the exception:

    You can do whatever you like now.

* Preparing to return:

    The high-level function is usually called as a subroutine and therefore returns into the low-level dispatch code. Here, saved registers are restored, and the CPU is returned to its safe (kernel mode, exceptions off) state by changing SR back to its postexception value.

* Returning from an exception:

    The end-of-exception processing is another area where the CPU has changed, and its description follows in section 5.5.

<h2 id="5.5">5.5 从异常返回</h2>

The return of control to the exception victim and the change (if required) back from kernel to a lower-privilege level must be done at the same time (“atomically,” in the jargon of computer science). It would be a security hole if you ran even one instruction of application code at kernel-privilege level; on the other hand, the attempt to run a kernel instruction with user privileges would lead to a fatal exception.

MIPS CPUs have an instruction, `eret`, that does the whole job; it both clears the SR(EXL) bit and returns control to the address stored in EPC.6

<h2 id="5.6">5.6 嵌套异常</h2>

In many cases, you will want to permit (or will not be able to avoid) further exceptions occurring within your exception processing routine; these are called nested exceptions.

Naïvely done, this would cause chaos; vital state from the interrupted program is held in EPC and SR, and you might expect another exception to overwrite them. Before you permit anything but a very peculiar nested exception, you must save those registers’ contents. Moreover, once exceptions are re-enabled, you can no longer rely on the reserved-for exception-handler general-purpose registers k0 and k1.

An exception handler that is going to survive a nested exception must use some memory locations to save register values. The data structure used is often called an exception frame;multiple exception frames from nested exceptions are usually arranged on a stack.

Stack resources are consumed by each exception, so arbitrarily deep nesting of exceptions cannot be tolerated. Most systems award each kind of exception a priority level and arrange that while an exception is being processed, only higher-priority exceptions are permitted. Such systems need have only as many exception frames as there are priority levels.

You can avoid all exceptions; interrupts can be individually masked by software to conform to your priority rules, masked all at once with the SR(IE) bit, or implicitly masked (for later CPUs) by the exception-level bit. Other kinds of exceptions can be avoided by appropriate software discipline. For example, privilege violations can’t happen in kernel mode (used by most exception processing software), and programs can avoid the possibility of addressing errors and TLB misses. It’s essential to do so when processing higher-priority exceptions.

<h2 id="5.7">5.7 异常处理例程</h2>

The following MIPS32 code fragment is as simple as an exception routine can be. It does nothing except increment a counter on each exception:

        .set noreorder
        .set noat
    xcptgen:
        la      k0, xcptcount       # get address of counter
        lw      k1, 0(k0)           # load counter
        addu    k1, 1               # increment counter
        sw      k1, 0(k0)           # store counter
        eret                        # return to program
        .set at
        .set reorder

This doesn’t look very useful: Whichever condition caused the exception will still probably be active on its return, so it might just go round and round. And the counter xcptcount had better be in kseg0 so that you can’t get a TLB Miss exception when you read or write it.

<h2 id="5.8">5.8 中断</h2>

The MIPS exception mechanism is general purpose, but democratically speaking there are two exception types that happen far more often than all the rest put together. One is the TLB miss when an application running under a memory-mapped OS like UNIX steps outside the (limited) boundaries of the on-chip translation table; we mentioned that before and will come back to it in Chapter 6. The other popular exceptions are interrupts, occurring when a device outside the CPU wants attention. Since we’re dealing with an outside world that won’t wait for us, interrupt service time is often critical.

Embedded-system MIPS users are going to be most concerned about interrupts, which iswhy they get a special section.We’ll talk about the following:

* Interrupt resources in MIPS CPUs: This describes what you’ve got to work with.
* Implementing interrupt priority: All interrupts are equal to MIPS CPUs, but in your system you probably want to attend to some of them before the others.
* Critical regions, disabling interrupts, and semaphores: It’s often necessary to prevent an interrupt from occurring during critical operations, but there are particular difficulties about doing so on MIPS CPUs. We look at solutions.

<h3 id="5.8.1">5.8.1 MIPS架构CPU上的中断资源</h3>

MIPS CPUs have a set of eight independent7 interrupt bits in their Cause register. On most CPUs you’ll find five or six of these are signals from external logic into the CPU, while two of them are purely software accessible. The on-chip counter/timer (made of the Count and Compare registers, described in section 3.3.5) will be wired to one of them; it’s sometimes possible to share the counter/timer interrupt with an external device, but rarely a good idea to do so.

An active level on any input signal is sensed in each cycle and will cause an exception if enabled.

The CPU’s willingness to respond to an interrupt is affected by bits in SR. There are three relevant fields:

* The global interrupt enable bit SR(IE)must be set to 1, or no interrupt will be serviced.
* TheSR(EXL)(exception level) and SR(ERL)(error level) bitswill inhibit interrupts if set (as one of them will be immediately after any exception).
* The status register also has eight individual interrupt mask bits SR(IM), one for each interrupt bit in Cause. Each SR(IM) bit should be set to 1 to enable the corresponding interrupt so that programs can determine exactly which interrupts can happen and which cannot.

> What Are the Software Interrupt Bits For?
> Why on earth should the CPU provide 2 bits in the Cause register that, when set, immediately cause an interrupt unless masked?
>
> The clue is in “unless masked.” Typically this is used as a mechanism for high-priority interrupt routines to flag actions that will be performed by lower-priority interrupt routines once the system has dealt with all high-priority business. As the high-priority processing completes, the software will open up the interrupt mask, and the pending software interrupt will occur.
>
> There is no absolute reason why the same effect should not be simulated by system software (using flags in memory, for example) but the soft interrupt bits are convenient because they fit in with an interrupt-handling mechanism that already has to be provided, at very low hardware cost.

To discover which interrupt inputs are currently active, you look inside the Cause register. Note that these are exactly that—current levels—and do not necessarily correspond to the signal pattern that caused the interrupt exception in the first place. The active input levels in Cause(IP) and the masks in SR(IM) are helpfully aligned to the same bit positions, in case you want to “and” them together. The software interrupts are at the lowest positions, and the hardware interrupts are arranged in increasing order.

In architectural terms, all interrupts are equal.8When an interrupt exception is taken, an older CPU uses the “general” exception entry point—though MIPS 32/64 CPUs and some other modern CPUs offer an optional distinct exception entry point reserved for interrupts, which can save a few cycles. You can select this with the Cause(IV) register bit.

Interrupt processing proper begins after you have received an exception and discovered from Cause(ExcCode) that it was a hardware interrupt. Consulting Cause(IP), we can find which interrupt is active and thus which device is signaling us. Here is the usual sequence:

* Consult the Cause register IP field and logically “and” it with the current interrupt masks in SR(IM) to obtain a bit map of active, enabled interrupt requests. There may be more than one, any of which would have caused the interrupt.

* Select one active, enabled interrupt for attention. Most OSs assign the different inputs to fixed priorities and deal with the highest priority first, but it is all decided by the software.

* You need to save the old interrupt mask bits in SR(IM), but you probably already saved the whole SR register in the main exception routine.

* Change SR(IM) to ensure that the current interrupt and all interrupts your software regards as being of equal or lesser priority are inhibited.

* If you haven’t already done it in the main exception routine, save the state (user registers, etc.) required for nested exception processing.

* Nowchange yourCPUstate to that appropriate to the higher-level part of the interrupt handler, where typically some nested interrupts and exceptions are permitted.

In all cases, set the global interrupt enable bit SR(IE) to allow higherpriority interrupts to be processed. You’ll also need to change the CPU privilege-level field SR(KSU) to keep the CPU in kernel mode as you clear exception level and, of course, clear SR(EXL) itself to leave exception mode and expose the changes made in the status register.

* Call your interrupt routine.

* On return you’ll need to disable interrupts again so you can restore the preinterrupt values of registers and resume execution of the interrupted task. To do that you’ll set SR(EXL). But in practice you’re likely to do this implicitly when you restore the just-after-exception value of the whole SR register, before getting into your end-of-exception sequence.

When making changes to SR, you need to be careful about changes whose effect is delayed due to the operation of the pipeline—“CP0 hazards.” See section 3.4 for more details and how to program around the hazards.

<h3 id="5.8.2">5.8.2 在软件中实现中断优先级</h3>

The MIPS CPU(until you use the new vectored interrupt facilities) has a simple minded approach to interrupt priority; all interrupts are equal.

If your system implements an interrupt priority scheme, then:

* At all times the software maintains a well-defined interrupt priority level (IPL) at which the CPU is running. Every interrupt source is allocated to one of these levels.

* If the CPU is at the lowest IPL, any interrupt is permitted. This is the state in which normal applications run.

* If the CPU is at the highest IPL, then all interrupts are barred.

Not only are interrupt handlers run with the IPL set to the level appropriate to their particular interrupt cause, but there’s provision for programmers to raise and lower the IPL. Those parts of the application side of a device driver that communicate with the hardware or the interrupt handler will often need to prevent device interrupts in their critical regions, so the programmer will temporarily raise the IPL to match that of the device’s interrupt input.

In such a system, high-IPL interrupts can continue to be enabled without affecting the lower-IPL code, so we’ve got the chance to offer better interrupt response time to some interrupts, usually in exchange for a promise that their interrupt handlers will run to completion in a short time.

Most UNIX systems have between four and six IPLs.

While there are other ways of doing it, the simplest schemes have the following characteristics:

* Fixed priorities: At any IPL, interrupts assigned to that and lower IPLs are barred, but interrupts of higher IPLs are enabled. Different interrupts at the same IPL are typically scheduled first come, first served.

* IPL relates to code being run: Any given piece of code always executes at the same IPL.

* Simple nested scheduling (above IPL 0): Except at the lowest level, any interrupted code will be returned to as soon as there are no more active interrupts at a higher level. At the lowest level there’s quite likely a scheduler that shares the CPU out among various tasks, and it’s common to take the opportunity to reschedule after a period of interrupt activity

On a MIPS CPU a transition between interrupt levels must (at least) be accompanied by a change in the status register SR, since that register contains all the interrupt control bits. On some systems, interrupt level transitions will require doing something to external interrupt control hardware, and most OSs have some global variables to change, but we don’t care about that here; for now we’ll characterize an IPL by a particular setting of the SR interrupt fields.

In the MIPS architecture SR (like all coprocessor registers) is not directly accessible for bit setting and clearing. Any change in the IPL, therefore, requires a piece of code that reads, modifies, and writes back the SR in separate operations:

        mfc0 t0, SR
    1:
        or t0, things_to_set
        and t0, ˜(things_to_clear)
    2:
        mtc0 t0, SR
        ehb

(The last instruction, ehb, is a hazard barrier instruction; see section 3.4.) In general, this piece of code may itself be interrupted, and a problem arises: Suppose we take an interrupt somewhere between label 1 and 2 and that interrupt routine itself causes a change in SR? Then when we write our own altered value of SR at label 2, we’ll lose the change made by the interrupt routine.

It turns out that we can only get away with the code fragment above—some version of which is prettymuch universal in MIPS implementations of OSs—in systems where we can rely on the IPL being constant in any particular piece of code (and where we don’t make any other running changes to SR).With those conditions, even if we get interrupted in the middle of our read-modify-write sequence, it will do no harm; when the interrupt returns it will do so with the same IPL, and therefore the same SR value, as before.

Where this assumption breaks down, we need the following discussion.

<h3 id="5.8.3">5.8.3 原子性以及对SR的原子修改</h3>

In systemswith more than one thread of control—including a single application with interrupt handlers—you will quite often find yourself doing something during which you don’t want to be caught halfway. In more formal language, you may want a set of changes to be made atomically, so that some cooperating task or interrupt routine in the system will see either none of them made or all of them, but never anything in between.10 The code implementing the atomic change is sometimes called a critical region.

On a uniprocessor that runs multiple threads in software, a thread switch that surprises the current thread can only happen as the consequence of some interrupt or other. So in a uniprocessor system, any critical region can be simply protected by disabling all interrupts around it; this is crude but effective.

But as we saw above, there’s a problem: The interrupt-disabling sequence (requiring a read-modify-write sequence on SR) is itself not guaranteed to be atomic. I know of four ways of fixing this impasse and one way to avoid it.

You can fix it if you know that all CPUs running your software implement MIPS32 Release 2: In that case, you can use the di instruction instead of the mfc0. di atomically clears the SR(IE) bit, returning the original value of SR in a general-purpose register.11 But until MIPS32 Release 2 compliance becomes the rule rather than the exception, you may need to look further.

A more general fix is to insist that no interrupt may change the value of SR held by any interruptible code; this requires that interrupt routines always restore SR before returning, just as they’re expected to restore the state of all the user-level registers. If so, the nonatomic RMW sequence above doesn’t matter; even if an interrupt gets in,the old value of SR you’reusing will still be correct.This approach is generally used in UNIX-like OS kernels for MIPS and goes well with the interrupt priority system in which every piece of code is associated with a fixed IPL.

But sometimes this restriction is too much. For example, when you’ve sent the last waiting byte on a byte-at-a-time output port, you’d like to disable the ready-to-send interrupt (to avoid eternal interrupts) until you have some more data to send. And again, some systems like to rotate priorities between different interrupts to ensure a fair distribution of interrupt service attention.

Another solution is to use a system call to disable interrupts (probably you’d define the system call as taking separate bit-set and bit-clear parameters and get it to update the status register accordingly). Since a syscall instruction works by causing an exception, it disables interrupts atomically. Under this protection your bit-set and bit-clear can proceed cheerfully. When the system call exception handler returns, the global interrupt enable status is restored (once again atomically).

A system call sounds pretty heavyweight, but it actually doesn’t need to take long to run; however, you will have to untangle this system call from the rest of the system’s exception-dispatching code.

The third solution—which all substantial systems should use for at least some critical regions—is to use the load-linked and store-conditional instructions to build critical regions without disabling interrupts at all, as described below. Unlike anything described above, that mechanism extends correctly to multiprocessor or hardware-multithreading systems.

<h3 id="5.8.4">5.8.4 使能中断状态下的临界区：MIPS式的信号量</h3>

A semaphore12 is a coding convention to implement critical regions (though extended semaphores can do more tricks than that). The semaphore is a shared memory location used by concurrently running processes to arrange that some resource is only accessed by one of them at once.

Each atomic chunk of code has the following structure:13

    wait(sem);
    /* do your atomic thing */
    signal(sem);

Think of the semaphore as having two values: 1 meaning “in use” and 0 meaning “available.” The signal() is simple; it sets the semaphore to 0.

wait() checks for the variable to have the value 0 and won’t continue until it does. It then sets the variable to 1 and returns. That should be easy, but you can see that it’s essential that the process of checking the value of sem and setting it again is itself atomic.High-level atomicity (for threads calling wait()) is dependent on being able to build low-level atomicity, where a test-and-set operation can operate correctly in the face of interrupts (or, on a multiprocessor, in the face of access by other CPUs).

Most mature CPU families have some special instruction for this: 680x0 CPUs—and many others—have an atomic test-and-set instruction; x86 CPUs have an “exchange register with memory” operation that can be made atomic with a prefix “lock” instruction.

For large multiprocessor systems this kind of test-and-set process becomes expensive; essentially, all shared memory access must be stopped while first the semaphore user obtains the value and completes the test-and-set operation, and, second, the set operation percolates through to every cached copy in the system. This doesn’t scale well to largemultiprocessors, because you’re holding up lots of probably unrelated traffic just to make sure you held up the occasional thing that mattered.

It’smuch more efficient to allow the test-and-set operation to run without a prior guarantee of atomicity so that the “set” succeeds only if it was atomic. Software needs to be told whether the set succeeded: Then unsuccessful test-and-set sequences can be hidden inside the wait() function and retried as necessary.15

This is what MIPS has, using the ll (load-linked) and sc (store-conditional) instructions in sequence. sc will only write the addressed location if the hardware confirms that there has been no competing access since the last ll and will leave a 1/0 value in a register to indicate success or failure.

In most implementations this information is pessimistic: Sometimes sc will fail even though the location has not been touched; CPUs will fail the sc when there’s been any exception serviced since the ll,16 and most multiprocessors will fail on any write to the same “cache line”-sized block of memory. It’s only important that the sc should usually succeed when there’s been no competing access and that it always fails when there has been one such.

Here’s wait() for the binary semaphore sem:

    wait:
        la t0, sem
    TryAgain:
        ll t1, 0(t0)
        bne t1, zero, WaitForSem
        li t1, 1
        sc t1, 0(t0)
        beq t1, zero, TryAgain
        /* got the semaphore... */
        jr ra

ll/sc was invented for multiprocessors, but even in a uniprocessor system, this kind of operation can be valuable, because it does not involve shutting out interrupts. It avoids the interrupt-disabling problem described above and contributes to a coordinated effort to reduce worst-case interrupt latency, very desirable in embedded systems.

<h3 id="5.8.5">5.8.5 MIPS32/64架构CPU的中断向量化和EIC中断</h3>

Release 2 of the MIPS32 specification—first seen in the 4KE and 24K family CPU cores from MIPS Technologies—adds two new features that can make interrupt handling more efficient. The savings are modest and probably wouldn’t be important in a substantial OS, but MIPS CPUs are also used in very low level embedded environments where these kinds of improvements are very welcome. Those features are vectored interrupts and a way of providing a large number of distinguishable interrupts to the CPU, called EIC mode.

If you switch on the vectored interrupt feature, an interrupt exception will start at one of eight addresses according to the interrupt input signal that caused it. That’s slightly ambiguous: There’s nothing to stop two interrupts being active at once, so the hardware uses the highest-numbered interrupt signal, which is both active and enabled. Vectored interrupts are set up by programming IntCtl(VS), which gives you a few choices as to the spacing between the different entry points (a zero causes all the interrupts to use the same entry point, as was traditional).

Embedded systems often have a very large number of interrupt events to signal, far beyond the six hardware inputs of traditional MIPS CPUs. In EIC mode, those six formerly independent signals become a 6-bit binary number: Zero means no interrupt, but that leaves 63 distinct interrupt codes. In EIC mode, each nonzero code has its own interrupt entry point, allowing an appropriately designed interrupt controller to dispatch theCPUdirectly to handle up to 63 events.

Vectored interrupts (whether with traditional or EIC signaling) are likely to be most helpful in circumstances where one or two interrupt events in your system are particularly frequent or time-critical. The small number of cycles saved are likely to be lost in fitting into the interrupt-handling discipline of a more sophisticated OS, so don’t be surprised to find that your favorite OS does not use these features.

<h3 id="5.8.6">5.8.6 影子寄存器</h3>

Even with interrupt vectors, an interrupt routine is burdened with the need to avoid trashing the register values of the code it interrupted, and must load addresses for itself before it can do any useful work.

Release 2 of the MIPS32/64 specification permits CPUs to provide one or more distinct sets of general-purpose registers: The extra register sets are called shadow registers. Shadow registers can be used for any kind of exception but are most useful for interrupts.

An interrupt handler using a shadow register set has no need to save the interrupted program’s register values and may keep its own state between invocations in its own registers (if more than one interrupt handler uses the shadow set, they’d better agree who gets to use which register).

An interrupt handler using vectored interrupts and a shadow register set can be unburdened by housekeeping and can run phenomenally fast. But again, that advantage can be lost by the discipline of an OS (in particular because the OS is likely to disable all interrupts for periods of time that far exceed our superfast interrupt handler’s run time). Some applications that would benefit from shadow registers might get the same kind of benefit more cleanly by using a multithreading CPU, but that’s a much bigger story—see Appendix A.

<h2 id="5.9">5.9 启动</h2>

In terms of its software-visible effect on the CPU, reset is almost the same as an exception, though one from which we’re not going to return. In the original MIPS architecture, this is mostly a matter of economy of implementation effort and documentation, but later CPUs have offered several different levels of reset, from a cold reset through to a nonmaskable interrupt. In MIPS, reset and exception conditions shade imperceptibly into each other.

We’re recycling mechanisms from regular exceptions, following reset EPC points to the instruction that was being executed when reset was detected, and most register values are preserved. However, reset disrupts normal operation, and a register being loaded or a cache location being stored to or refilled at the moment reset occurred may be trashed.

It is possible to use the preservation of state through reset to implement some useful postmortemdebugging, but your hardware engineer needs to help; the CPU cannot tell you whether reset occurred to a running system or from power-up. But postmortem debugging is an exercise for the talented reader; we will focus on starting up the system from scratch.

The CPU responds to reset by starting to fetch instructions from 0xBFC0.0000. This is physical address 0x1FC0.0000 in the uncached kseg1 region.

Following reset, enough of the CPU’s control register state is defined that the CPU can execute uncached instructions. “Enough state” is interpreted minimally; note the following points:

* Only three things are guaranteed in SR: The CPU is in kernel mode; interrupts are disabled; and exceptions will vector through the uncached entry points—that is, SR(BEV) = 1. In modern CPUs, the first two conditions (andmore beside) are typically guaranteed by setting the exception-mode bit SR(EXL), and this is implied by treating reset as an exception.

* The caches will be in a random, nonsensical state, so a cached load might return rubbish without reading memory.

* The TLB will be in a random state and must not be accessed until initialized (in some CPUs the hardware has only minimal protection against the possibility that there are duplicate matches in the TLB, and the result could be a TLB shutdown,which can be amended only by a further reset).

The traditional start-up sequence is as follows:

1. Branch to the main ROM code.Why do a branch now?

    * The uncached exception entry points start at 0xBFC0.0100, which wouldn’t leave enough space for start-up code to get to a “natural break”—so we’re going to have to do a branch soon, anyway.
    
    * The branch is a very simple test to see if the CPU is functioning and is successfully reading instructions. If something terrible goes wrong with the hardware, the MIPS CPU is most likely to keep fetching instructions in sequence (and next most likely to get permanent exceptions).
    
        If you use test equipment that can track the addresses of CPU reads and writes, it will show the CPU’s uncached instruction fetches from reset; if the CPU starts up and branches to the right place, you have strong evidence that the CPU is getting mostly correct data from the ROM.
    
        By contrast, if your ROM program plows straight in and fiddles with SR, strange and undiagnosable consequences may result from simple faults.

2. Set the status register to some known and sensible state. Now you can load and store reliably in uncached space.

3. You will probably have to run using registers only until you have initialized and (most likely) run a quick check on the integrity of some RAM memory. This will be slow (we’re still running uncached from ROM), so you will probably confine your initialization and check to a chunk of memory big enough for the ROM program’s data.

4. You will probably have to make some contact with the outside world (a console port or diagnostic register) so you can report any problem with the initialization process.

5. You can now assign yourself some stack and set up enough registers to be able to call a standard C routine.

6. Now you can initialize the caches and run in comfort. Some systems can run code from ROM cached and some can’t; on most MIPS CPUs, a memory supplying the cache must be able to provide 4/8-word bursts, and your ROM subsystem may or may not oblige.

5.9.1 Probing and Recognizing Your CPU
<h3 id="5.9.1">5.9.1 检测识别CPU</h3>

You can identify your CPU implementation number and a manufacturerdefined revision level from the PRId(Imp) and PRId(Rev) fields. However, it’s best to rely on this information as little as possible; if you rely on PRId, you guarantee that you’ll have to change your program for any future CPU, even though there are no new features that cause trouble for your program.

Whenever you can probe for a feature directly, do so. Where you can’t figure out a reliable direct probe, then for CPUs that are MIPS32/64-compliant, there’s a good deal of useful information encoded in the various Config registers described in section 3.3.7. Only after exhausting all other options should you consider relying on PRId.

Nonetheless, your boot ROM and diagnostic software should certainly display PRId in some readable form. And should you ever need to include a truly unpleasant software workaround for a hardware bug, it’s good practice to make it conditional on PRId, so your workaround is not used on future, fixed, hardware.

Since we’ve recommended that you probe for individual features, here are some examples of how you could do so:

* Have we got FP hardware? Your MIPS32/64 CPU should tell you through the Config1(FP) register bit. For older CPUs one recommended technique is to set SR(CU1) to enable coprocessor 1 operations and to use a cfc1 instruction from coprocessor 1 register 0, which is defined to hold the revision ID. A nonzero value in the ProcessorID field (bits 8–15) indicates the presence of FPU hardware— you’ll most often see the same value as in the PRId(Imp) field. A skeptical programmer17 will probably follow this up by checking that it is possible to store and retrieve data from the FPU registers. Unless your system supports unconditional use of the floating-point unit, don’t forget to reset SR(CU1) afterward.

* Cache size: On a MIPS32/64-compliant CPU, it probably is better to rely on the values encoded in the configuration registers Config1-2. But you can also deduce the size by reading and writing cache tags and looking for when the cache size wraps around.

    Have we got a TLB? That’s memory translation hardware. The Config (MT) bit tells you whether you have it. But you could also read and write values to Index or look for evidence of a continuously counting Random register. If it looks promising, you may want to check that you can store and retrieve data in TLB entries.

* CPU clock rate: It is often useful to work out your clock rate. You can do this by running a loop of known length, cached, that will take a fixed large number of CPU cycles and then comparing with “before” and “after” values of a counter outside the CPU that increments at known speed. Do make sure that you are really running cached, or you will get strange results—remember that some hardware can’t run cached out of ROM.

    The Linux kernel does this when it boots and reports a number called BogoMIPS, to emphasize that any relationship between the number and CPU performance is bogus.

    If your CPU is compliant to MIPS32/64 Release 2 (the 2003 specification), the rdhwr instruction gives you access to the divider between the main CPU clock and the speed with which the Count register increments. It’s simpler to compare the counter speed with some external reference.

Some maintenance engineer will bless you one day if you make the CPU ID, clock rate, and cache sizes available, perhaps as part of a sign-on message.

<h3 id="5.9.2">5.9.2 引导步骤</h3>

Start-up code suffers from the clash of two opposing but desirable goals. On the one hand, it’s robust to make minimal assumptions about the integrity of the hardware and to attempt to check each subsystem before using it (think of climbing a ladder and trying to check each rung before putting your weight on it). On the other hand, it’s desirable to minimize the amount of tricky assembly code. Bootstrap sequences are almost never performance sensitive, so an early change to a high-level language is desirable. But high-level language code tends to require more subsystems to be operational.

After you have dealt with the MIPS-specific hurdles (like setting up SR so that you can at least perform loads and stores), the major question is how soon you can make some read/write memory available to the program, which is essential for calling functions written in C.

Sometimes diagnostic suites include bizarre things like the code in the original PC BIOS, which tests each 8086 instruction in turn. This seems to me like chaining your bicycle to itself to foil thieves. If any subsystem is implemented inside the same chip as the CPU, you don’t lose much by trusting it.

<h3 id="5.9.3">5.9.3 启动应用程序</h3>

To be able to start a C application (presumably with its instructions coming safely from ROM) you need writable memory, for three reasons.

First, you need stack space. Assign a large enough piece of writable memory and initialize the sp register to its upper limit (aligned to an eight-byte boundary). Working out how large the stack should be can be difficult: A generous guess will be more robust.

Then you may need some initialized data. Normally, m= the C data area is initialized by the program loader to set up any variables that have been allocated values. Most compilation systems that purport to be usable for embedded applications permit read-only data (implicit strings and data items declared const) to be managed in a separate segment of object code and put into ROM memory.

Initialized writable data can be used only if your compilation system and runtime system cooperate to arrange to copy writable data initializations from ROM into RAM before calling main().

Last, C programs use a different segment of memory for all static and extern data items that are not explicitly initialized—an area sometimes called “BSS,” for reasons long lost. Such variables should be cleared to zero, which is readily achieved by zeroing the whole data section before starting the program.

If your program is built carefully, that’s enough. However, it can get more complicated: Take care that your MIPS program is not built to use the global pointer register gp to speed access to nonstack variables, or you’ll need to do more initialization.

<h2 id="5.10">5.10 指令仿真</h2>

Sometimes an exception is used to invoke a software handler that will stand in for the exception victim instruction, as when you are using software to implement a floating-point operation on a CPU that doesn’t support FP in hardware. Debuggers and other system tools may sometimes want to do this too.

To emulate an instruction, you need to find it, decode it, and find its operands. A MIPS instruction’s operands were in registers, and by now those preexception register values have been put away in an exception frame somewhere. Armed with those values, you do the operation in software and patch the results back into the exception frame copy of the appropriate result register. You then need to fiddle with the stored exception return address so as to step over the emulated instruction and then return. We’ll go through these step by step.

Finding the exception-causing instruction is easy; it’s usually pointed to by EPC, unless it was in a branch delay slot, in which case Cause(BD) is set and the exception victim is at the address EPC + 4.

To decode the instruction, you need some kind of reverse-assembly table. A big decode-oriented table of MIPS instructions is part of the widely available GNU debugger gdb, where it’s used to generate disassembly listings. So long as the GNU license conditions aren’t a problem for you, that will save you time and effort.

To find the operands, you’ll need to know the location and layout of the exception frame, which is dependent on your particular OS (or exception handling software, if it’s too humble to be called an OS).

You’ll have to figure out for yourself how to do the operation, and once again you need to be able to get at the exception frame, to put the results back in the saved copy of the right register.

There’s a trap for the unwary in incrementing the stored EPC value to step over the instruction you’ve emulated. If your emulated instruction was in a branch delay slot, you can’t just return to the “next” instruction—that would be as if the branch was not taken, and you don’t know that.

So when your emulation candidate is in a branch delay slot, you also have to emulate the branch instruction, testing for whether the branch should be taken or not. If the branch should be taken, you need to compute its target and return straight there from the exception.

Fortunately, all MIPS branch instructions are simple and free of side effects.


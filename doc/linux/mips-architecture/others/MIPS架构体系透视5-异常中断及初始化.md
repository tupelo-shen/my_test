<h1 id="0">0 目录</h1>

* [5.1 精确异常](#5.1)
    - [5.1.1 非精确异常-历史上的MIPS架构CPU的乘法器](#5.1.1)
* [5.2 异常发生的时机](#5.2)
* [5.3 异常向量表：异常处理开始的地方](#5.3)
* [5.4 异常处理的基本过程](#5.4)
* [5.5 嵌套异常](#5.5)
* [5.6 异常处理例程](#5.6)
* [5.7 中断](#5.7)
    - [5.7.1 MIPS架构CPU上的中断资源](#5.7.1)
    - [5.7.2 在软件中实现中断优先级](#5.7.2)
    - [5.7.3 原子性以及对SR的原子修改](#5.7.3)
    - [5.7.4 使能中断状态下的临界区：MIPS式的信号量](#5.7.4)
    - [5.7.5 MIPS32/64架构CPU的中断向量化和EIC中断](#5.7.5)
    - [5.7.6 影子寄存器](#5.7.6)
* [5.8 启动](#5.8)
    - [5.8.1 检测识别CPU](#5.8.1)
    - [5.8.2 引导步骤](#5.8.2)
    - [5.8.3 启动应用程序](#5.8.3)
* [5.9 指令仿真](#5.9)

---

MIPS架构中，中断、异常、系统调用以及其它可以中断程序正常执行流的事件统称为`异常（exception）`，统一由异常处理机制进行处理。

> <font color="blue">
> 异常和中断概念在不同架构上的含义区别：
> 
> 1. MIPS架构将所有可以中断程序执行流的事件称为异常；
> 2. X86架构将所有可以中断程序执行流的事件称为中断，我们日常所见的狭义上的中断，也就是外部中断，称之为`异步中断`；而狭义上的异常称为`同步中断`；
> 3. ARM架构将这两个概念合起来使用-`异常中断`类似于MIPS架构的`异常`概念。
> 
> 在阅读相关书籍的时候，请注意区分这些概念。
> </font>

MIPS架构所涉及的event，都有哪些呢？

* 外部事件

    Some event outside the CPU core—that is, from some real “wire” input signal. These are interrupts.1 Interrupts are used to direct the attention of the CPU to some external event: an essential feature of an OS that attends to more than one different event at a time.
    来自CPU核外的外部中断。

    Interrupts are the only exception conditions that arise from something independent of the CPU’s normal instruction stream. Since you can’t avoid interrupts just by being careful, there have to be software mechanisms to inhibit the effect of interrupts when necessary.

* 内存转换异常

    These happen when an address needs to be translated, but no valid translation is available to the hardware or perhaps on a write to a write-protected page.

    The OS must decide whether such an exception is an error or not. If the exception is a symptom of an application program stepping outside its permitted address space, it will be fixed by terminating the application to protect the rest of the system. The more common benign memory translation exceptions can be used to initiate operating system functions as complex as a complete demand-paged virtual memory system or as simple as extending the space available for a stack.

* 其它需要内核修复的非常情况 

    Notable among these are conditions resulting fromfloating-point instructions, where the hardware is unable to cope with some difficult and rare combination of operation and operands and is seeking the services of a software emulator. This category is fuzzy, since different kernels have different ideas about what they’re willing to fix. An unaligned load may be an error on one system and something to be handled in software on another.

* 程序或硬件检测到的错误

    This includes nonexistent instructions, instructions that are illegal at user-privilege level, coprocessor instructions executed with the appropriate SR flag disabled, integer overflow, address alignment errors, and accesses outside kuseg in user mode.

* 数据完整性问题

    Many MIPS CPUs continually check data on the bus or data coming from the cache for a per-byte parity or for wordwide error-correcting code. Cache or parity errors generate an exception in CPUs that support data checking.

* 系统调用和陷阱

    These are instructions whose whole purpose is to generate recognizable exceptions; they are used to build software facilities in a secure way (system calls, conditional traps planted by careful code, and breakpoints).

Some things do not cause exceptions, though you’d expect them to. For example, you will have to use other mechanisms to detect bus errors on write cycles. That’s because most modern systems queue up writes: Any writeassociated error would happen much too late to relate to the instruction that caused it. Systems that do inform you about errors on writes must use some outside-the-CPU hardware, probably signaled with an interrupt.

In this chapter, we’ll look at how MIPS CPUs decide to take exceptions and what the software has to do to handle them cleanly. We’ll explain why MIPS exceptions are called “precise,” discuss exception entry points, and discuss some software conventions.

Hardware interrupts fromoutside theCPUare the most commonexceptions for embedded applications, the most time critical, and the ones most likely to cause subtle bugs. Special problems can arise with a nested exception, one that happens before you have finished handling an earlier exception.

The way that a MIPS CPU starts up after system reset is implemented as a kind of exception and borrows functions from exceptions—so that’s described in this chapter too.

At the end of the chapter, we’ll look at a couple of related subjects: how to emulate an instruction (as needed by an instruction set extension mechanism) and how to build semaphores to provide robust task-to-task communication in the face of interrupts. Chapter 14 describes in some detail how interrupts are handled in a real, grown-up OS for MIPS.

<h2 id="5.1">5.1 精确异常</h2>

在MIPS架构的文档中，我们经常看到一个术语"精确异常"，英文称之为`precise exception`。那到底什么是精确异常，什么是非精确异常呢？

在通过流水线获取最佳性能的CPU中，体系结构的顺序执行模型其实是硬件巧妙维护的假象。如果硬件设计不够完美，异常就可能导致该假象暴露。

当异常中断正在执行的线程时，CPU的流水线中肯定还有几条处于不同阶段尚未完成的指令。如果我们想要从异常返回时，继续不受破坏地执行被打断的程序执行流，那么流水线中的每条指令都必须要执行完，从异常返回时，仿佛什么都没有发生才行。

一个CPU体系结构具备精确异常的特性，必须满足任何异常发生时，都必须确定的指向某条指令，这条指令就是产生异常的指令。而在该指令之前的指令必须都执行完，异常指令和后续指令好像都没有发生。所以，当说异常是精确异常时，处理异常的软件就可以忽略CPU实现的时序影响。

MIPS架构的异常基本上都是精确异常。 Here are the ingredients:

* 明确的罪证

    异常处理完成后，CPU的EPC寄存器指向重新执行的正确地址。大部分情况下，指向异常指令所在的地址。但是，如果异常发生在分支延时槽上的指令时，EPC寄存器指向前面的分支指令：如果指向异常指令，分支指令会被忽略；而指向分支指令，可以重新执行异常。当异常发生在分支延时槽时，Cause寄存器的BD标志位会被设置。

* 异常尽量出现在指令序列中，而不是流水线的某个阶段

    异常可能会发生在流水线的各个阶段，这带来了一个潜在的危险。比如，一个load指令直到流水线的地址转换完成阶段才会发生异常，通常这已经晚了。如果下一条指令在取指时发生地址异常（刚好在流水线的开始阶段），此时，第二条指令的异常首先发生，这与我们的构想不一致。

    为了避免这个问题，异常检测到后不是立即执行，这个事件只是被记录并沿着流水线向下传递。在大多数的CPU设计中，都会标记一个特殊的流水线阶段作为检测异常的地方。如果较久指令后面才检测到的异常到达这个检测点，异常记录就会被立即抛弃。这保证了永远执行最新的异常。对于上面的问题，第二条指令带来的取指问题就会被忽略。反正当我们继续执行时，它还会发生。

* 后续指令无效

    因为流水线的原因，当异常发生时，异常指令后面的指令已经开始执行了。但是硬件保证这些指令产生的效果不会影响寄存器和CPU的状态，从而保证这些指令好像没有执行一样。

MIPS实现精确异常的代价高昂，因为它限制了流水线的作用范围。尤其是FPU硬件单元。我们前面讲过，浮点指令不能遵守MIPS架构的5级流水线，需要更多级的流水线才能完成。所以，浮点单元一般都有自己独立的流水线。这种现状导致跟在MIPS浮点指令后的指令必须在确认浮点指令不会产生异常后才能提交自己的状态。

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

<h2 id="5.3">5.3 异常向量表：异常处理开始的地方</h2>

我们知道，CPU使用硬件或者软件分析异常，然后根据类型将CPU派发到不同的入口点。这个过程就是中断响应。如果通过硬件，直接根据中断输入信号就能在不同的入口点处理中断，称为向量化中断。比如，常见的ARM架构的Cortex-M系列基本上就是采用向量化中断的方式。历史上，MIPS架构CPU很少使用向量化中断的方式，主要是基于一下几个方面的考虑。

* 首先，向量化中断在实践中并没有我们想象的那么有用。大部分操作系统中，中断处理程序共享代码（为了节约寄存器之类的目的），因此，常见的作法就是硬件或者微代码将CPU派发到不同的入口点，在这儿，OS再跳转到共同处理程序，根据中断编号进行处理。

* 其次，由硬件所做的异常分析，相比软件而言非常有限。而且现在的CPU来说，代码的执行速度也足够快。

总结来说，高端CPU的时钟频率肯定远远快于外设，所以写一个中断通用处理程序完全可以满足性能要求。所以，自从在MIPS32架构上添加了向量化中断之后，几乎没有人使用。

但是，MIPS架构上，并不是所有的异常都是平等的，他们之间也是有优先级区分的，总结如下：

* 用户态地址的TLB重填异常

    There is one particularly frequent exception in a protected OS, related to the address translation system (see Chapter 6). The TLB hardware only holds a modest number of address translations, and in a heavily used system running a virtual memory OS, it’s common for the application program to run on to an address whose translation is not recorded in the TLB—an event called a TLB miss (because the TLB is used as a software-managed cache).

    The use of software to handle this condition was controversial when RISC CPUs were introduced, and MIPS CPUs provide significant support for a preferred scheme for TLB refill. The hardware helps out enough that the exception handler for the preferred refill scheme can run in as few as 13 clock cycles.

    As part of this, common classes of TLB refill are given an entry point different from all other exceptions, so that the finely tuned refill code doesn’t have to waste time figuring out what kind of exception has happened.

* 64位地址空间的TLB重填异常

    Memory translation for tasks wanting to take advantage of the larger programaddress space available on 64-bit CPUs uses a slightly different register layout and a different TLB refill routine; MIPS calls this an XTLB refill (“X” for extended, I guess). Again, a desire to keep this very efficient makes a separate entry point useful.

* 初始化时的中断向量入口点（不使用Cache访问）

    For good performance on exceptions, the interrupt entry point must be in cached memory, but this is highly undesirable during system bootstrap; from reset or power-up, the caches are unusable until initialized. If you want a robust and self-diagnosing start-up sequence, you have to use uncached read-only memory entry points for exceptions detected in early bootstrap. In MIPS CPUs there is no uncached “mode”—there are uncached program memory regions instead—so there’s a mode bit SR(BEV) that reallocates the exception entry points into the uncached, start-up-safe kseg1 region.

* 奇偶/ECC错误异常 

    MIPS32 CPUs may detect a data error (usually in data arriving from main memory, but often not noticed until it’s used from cache) and take a trap. It would be silly to vector through a cached location to handle a cache error, so regardless of the state of SR(BEV) the cache error exception entry point is in uncached space.

* 复位

    For many purposes, it makes sense to see reset as another exception, particularly when many CPUs use the same entry point for cold reset (where the CPU is completely reconfigured; indistinguishable from power-up) and warm reset (where the software is completely reinitialized). In fact, nonmaskable interrupt (NMI) turns out to be a slightly weaker version of warm reset, differing only in that it waits for the current instruction and any pending load/store to finish before taking effect.

* 中断

    As an option in MIPS32 (and some earlier CPUs from IDT and PMC-Sierra), you can set the CPU to dispatch interrupt exceptions to a separate entry point. This is convenient, though little used: Perhaps software authors can’t bring themselves to special-case their OS for a feature that is not universally available.

    Further, in some of these CPUs you can enable vectored interrupt operation—multiple entry points to be used by different interrupts. This is a more substantial change; as explained elsewhere in this chapter, the MIPS tradition was that interrupts were only prioritized in software. But if you have two active interrupts and have to choose an interrupt entry point, the hardware must decide which has the higher priority. This change is therefore significantly more disruptive to software, since the software loses control over interrupt priority; your OS maintainer and hardware engineers will have to liaise closely.

为了效率，所有异常入口点都位于不需要地址映射的内存区域，不经过Cache的kseg1空间，经过cache的kseg0空间。当SR（BEV）等于1时，异常入口地址位于kseg1，且是固定的；当SR(BEV=0)时，就可以对EBase寄存器进行编程来平移所有入口点，比如说，kseg0的某个区域。当使用多处理器系统时，想使各个CPU的异常入口点不同时，这个功能就很用了。

In these areas the nominal 32-bit addresses given in Table 5.1 extend to a 64-bit memory map by sign extension: The program address 0x8000.0000 in the 32-bit view is the same as 0xFFFF.FFFF.8000.0000 in the 64-bit view.

Table 5.1 describes the entry points with just 32-bit addresses—you need to accept that BASE stands for the exception base address programmed by the EBase register.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_5_1.PNG">

Presumably the default 128-byte (0x80) gap between the original exception vectors occurs because the original MIPS architects felt that 32 instructions would be enough to code the basic exception routine, saving a branch instruction without wasting too much memory! Modern programmers are rarely so frugal.

下面是发生异常时，MIPS架构CPU处理的过程：

1. 设置EPC寄存器指向重新开始的地址；

2. 设置SR(EXL)标志位，强迫CPU进入内核态并禁止中断；

3. 设置Cause寄存器，软件可以读取它获知异常原因。地址转换异常时，还要设置BadVaddr寄存器。内存管理系统异常还要设置一些MMU相关寄存器；

4. 然后CPU开始从异常入口点取指令，之后就取决于软件如何处理了。

Very short exception routines can run entirely with SR(EXL) set (in exception mode, as we’ll sometimes say) and need never touch the rest of SR. For more conventional exception handlers, which save state and pass control over to more complex software, exception level provides a cover under which system software can save essential state—including the old SR value—in safety.

With a couple of tweaks this mechanism can allow a minimal nested exception within the primitive TLB miss handler, but we’ll talk more about how that’s done when we get to it.

<h2 id="5.4">5.4 异常处理：基本过程</h2>

MIPS异常处理程序的基本步骤：

1. 保存被中断程序的状态：

    在异常处理程序的入口点，需要保存少量的被中断程序的状态。所以，第一步工作就是为保存这些状态提供必要的空间。MIPS架构习惯上保留k0和k1寄存器，用它们指向某段内存，用来保存某些需要保存的寄存器。

2. 派发异常：

    查询Cause寄存器的ExcCode域，获取异常码。通过异常码，允许OS定义不同的函数处理不同的异常。

3. 构建异常处理程序的运行环境：

    复杂的异常处理例程一般使用高级语言（比如，C语言）实现。所以，需要建立一段堆栈空间，保存被中断程序可能使用的任何寄存器，从而允许被调用的C异常处理例程可以修改这些寄存器。

    某些操作系统可能在派发异常之前进行这一步的处理。

4. 执行异常处理（一般使用C语言实现）：

    做你想做的任何事情。

5. 准备返回工作：

    需要从C代码返回到派发异常的通用代码中。在这儿，恢复被保存的寄存器，另外，通过修改SR寄存器到刚发生异常时的值，CPU也从异常模式返回到内核态。

6. 从异常返回：

    从异常状态返回时，有可能从内核态向低级别的运行态进行切换。为了系统安全的原因，这步工作必须是一个原子操作。基于这个目的，MIPS架构的CPU提供了一条指令，`eret`，完成从异常的返回：它清除SR(EXL)标志位，返回到EPC寄存器保存的地址处开始执行。

<h2 id="5.5">5.5 嵌套异常</h2>

In many cases, you will want to permit (or will not be able to avoid) further exceptions occurring within your exception processing routine; these are called nested exceptions.

Naïvely done, this would cause chaos; vital state from the interrupted program is held in EPC and SR, and you might expect another exception to overwrite them. Before you permit anything but a very peculiar nested exception, you must save those registers’ contents. Moreover, once exceptions are re-enabled, you can no longer rely on the reserved-for exception-handler general-purpose registers k0 and k1.

An exception handler that is going to survive a nested exception must use some memory locations to save register values. The data structure used is often called an exception frame;multiple exception frames from nested exceptions are usually arranged on a stack.

Stack resources are consumed by each exception, so arbitrarily deep nesting of exceptions cannot be tolerated. Most systems award each kind of exception a priority level and arrange that while an exception is being processed, only higher-priority exceptions are permitted. Such systems need have only as many exception frames as there are priority levels.

You can avoid all exceptions; interrupts can be individually masked by software to conform to your priority rules, masked all at once with the SR(IE) bit, or implicitly masked (for later CPUs) by the exception-level bit. Other kinds of exceptions can be avoided by appropriate software discipline. For example, privilege violations can’t happen in kernel mode (used by most exception processing software), and programs can avoid the possibility of addressing errors and TLB misses. It’s essential to do so when processing higher-priority exceptions.

<h2 id="5.6">5.6 异常处理例程</h2>

The following MIPS32 code fragment is as simple as an exception routine can be. It does nothing except increment a counter on each exception:

        .set noreorder
        .set noat
    xcptgen:
        la      k0, xcptcount       # 得到计数器的地址
        lw      k1, 0(k0)           # 加载计数器
        addu    k1, 1               # 增加计数值
        sw      k1, 0(k0)           # 存储计数器
        eret                        # 返回到程序
        .set at
        .set reorder

This doesn’t look very useful: Whichever condition caused the exception will still probably be active on its return, so it might just go round and round. And the counter xcptcount had better be in kseg0 so that you can’t get a TLB Miss exception when you read or write it.

<h2 id="5.7">5.7 中断</h2>

The MIPS exception mechanism is general purpose, but democratically speaking there are two exception types that happen far more often than all the rest put together. One is the TLB miss when an application running under a memory-mapped OS like UNIX steps outside the (limited) boundaries of the on-chip translation table; we mentioned that before and will come back to it in Chapter 6. The other popular exceptions are interrupts, occurring when a device outside the CPU wants attention. Since we’re dealing with an outside world that won’t wait for us, interrupt service time is often critical.

Embedded-system MIPS users are going to be most concerned about interrupts, which iswhy they get a special section.We’ll talk about the following:

* Interrupt resources in MIPS CPUs: This describes what you’ve got to work with.
* Implementing interrupt priority: All interrupts are equal to MIPS CPUs, but in your system you probably want to attend to some of them before the others.
* Critical regions, disabling interrupts, and semaphores: It’s often necessary to prevent an interrupt from occurring during critical operations, but there are particular difficulties about doing so on MIPS CPUs. We look at solutions.

<h3 id="5.7.1">5.7.1 MIPS架构CPU上的中断资源</h3>

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

<h3 id="5.7.2">5.7.2 在软件中实现中断优先级</h3>

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

<h3 id="5.7.3">5.7.3 原子性以及对SR的原子修改</h3>

In systemswith more than one thread of control—including a single application with interrupt handlers—you will quite often find yourself doing something during which you don’t want to be caught halfway. In more formal language, you may want a set of changes to be made atomically, so that some cooperating task or interrupt routine in the system will see either none of them made or all of them, but never anything in between.10 The code implementing the atomic change is sometimes called a critical region.

On a uniprocessor that runs multiple threads in software, a thread switch that surprises the current thread can only happen as the consequence of some interrupt or other. So in a uniprocessor system, any critical region can be simply protected by disabling all interrupts around it; this is crude but effective.

But as we saw above, there’s a problem: The interrupt-disabling sequence (requiring a read-modify-write sequence on SR) is itself not guaranteed to be atomic. I know of four ways of fixing this impasse and one way to avoid it.

You can fix it if you know that all CPUs running your software implement MIPS32 Release 2: In that case, you can use the di instruction instead of the mfc0. di atomically clears the SR(IE) bit, returning the original value of SR in a general-purpose register.11 But until MIPS32 Release 2 compliance becomes the rule rather than the exception, you may need to look further.

A more general fix is to insist that no interrupt may change the value of SR held by any interruptible code; this requires that interrupt routines always restore SR before returning, just as they’re expected to restore the state of all the user-level registers. If so, the nonatomic RMW sequence above doesn’t matter; even if an interrupt gets in,the old value of SR you’reusing will still be correct.This approach is generally used in UNIX-like OS kernels for MIPS and goes well with the interrupt priority system in which every piece of code is associated with a fixed IPL.

But sometimes this restriction is too much. For example, when you’ve sent the last waiting byte on a byte-at-a-time output port, you’d like to disable the ready-to-send interrupt (to avoid eternal interrupts) until you have some more data to send. And again, some systems like to rotate priorities between different interrupts to ensure a fair distribution of interrupt service attention.

Another solution is to use a system call to disable interrupts (probably you’d define the system call as taking separate bit-set and bit-clear parameters and get it to update the status register accordingly). Since a syscall instruction works by causing an exception, it disables interrupts atomically. Under this protection your bit-set and bit-clear can proceed cheerfully. When the system call exception handler returns, the global interrupt enable status is restored (once again atomically).

A system call sounds pretty heavyweight, but it actually doesn’t need to take long to run; however, you will have to untangle this system call from the rest of the system’s exception-dispatching code.

The third solution—which all substantial systems should use for at least some critical regions—is to use the load-linked and store-conditional instructions to build critical regions without disabling interrupts at all, as described below. Unlike anything described above, that mechanism extends correctly to multiprocessor or hardware-multithreading systems.

<h3 id="5.7.4">5.7.4 使能中断状态下的临界区：MIPS式的信号量</h3>

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

<h3 id="5.7.5">5.7.5 MIPS32/64架构CPU的中断向量化和EIC中断</h3>

Release 2 of the MIPS32 specification—first seen in the 4KE and 24K family CPU cores from MIPS Technologies—adds two new features that can make interrupt handling more efficient. The savings are modest and probably wouldn’t be important in a substantial OS, but MIPS CPUs are also used in very low level embedded environments where these kinds of improvements are very welcome. Those features are vectored interrupts and a way of providing a large number of distinguishable interrupts to the CPU, called EIC mode.

If you switch on the vectored interrupt feature, an interrupt exception will start at one of eight addresses according to the interrupt input signal that caused it. That’s slightly ambiguous: There’s nothing to stop two interrupts being active at once, so the hardware uses the highest-numbered interrupt signal, which is both active and enabled. Vectored interrupts are set up by programming IntCtl(VS), which gives you a few choices as to the spacing between the different entry points (a zero causes all the interrupts to use the same entry point, as was traditional).

Embedded systems often have a very large number of interrupt events to signal, far beyond the six hardware inputs of traditional MIPS CPUs. In EIC mode, those six formerly independent signals become a 6-bit binary number: Zero means no interrupt, but that leaves 63 distinct interrupt codes. In EIC mode, each nonzero code has its own interrupt entry point, allowing an appropriately designed interrupt controller to dispatch theCPUdirectly to handle up to 63 events.

Vectored interrupts (whether with traditional or EIC signaling) are likely to be most helpful in circumstances where one or two interrupt events in your system are particularly frequent or time-critical. The small number of cycles saved are likely to be lost in fitting into the interrupt-handling discipline of a more sophisticated OS, so don’t be surprised to find that your favorite OS does not use these features.

<h3 id="5.7.6">5.7.6 影子寄存器</h3>

Even with interrupt vectors, an interrupt routine is burdened with the need to avoid trashing the register values of the code it interrupted, and must load addresses for itself before it can do any useful work.

Release 2 of the MIPS32/64 specification permits CPUs to provide one or more distinct sets of general-purpose registers: The extra register sets are called shadow registers. Shadow registers can be used for any kind of exception but are most useful for interrupts.

An interrupt handler using a shadow register set has no need to save the interrupted program’s register values and may keep its own state between invocations in its own registers (if more than one interrupt handler uses the shadow set, they’d better agree who gets to use which register).

An interrupt handler using vectored interrupts and a shadow register set can be unburdened by housekeeping and can run phenomenally fast. But again, that advantage can be lost by the discipline of an OS (in particular because the OS is likely to disable all interrupts for periods of time that far exceed our superfast interrupt handler’s run time). Some applications that would benefit from shadow registers might get the same kind of benefit more cleanly by using a multithreading CPU, but that’s a much bigger story—see Appendix A.

<h2 id="5.8">5.8 启动</h2>

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

5.8.1 Probing and Recognizing Your CPU
<h3 id="5.8.1">5.8.1 检测识别CPU</h3>

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

<h3 id="5.8.2">5.8.2 引导步骤</h3>

Start-up code suffers from the clash of two opposing but desirable goals. On the one hand, it’s robust to make minimal assumptions about the integrity of the hardware and to attempt to check each subsystem before using it (think of climbing a ladder and trying to check each rung before putting your weight on it). On the other hand, it’s desirable to minimize the amount of tricky assembly code. Bootstrap sequences are almost never performance sensitive, so an early change to a high-level language is desirable. But high-level language code tends to require more subsystems to be operational.

After you have dealt with the MIPS-specific hurdles (like setting up SR so that you can at least perform loads and stores), the major question is how soon you can make some read/write memory available to the program, which is essential for calling functions written in C.

Sometimes diagnostic suites include bizarre things like the code in the original PC BIOS, which tests each 8086 instruction in turn. This seems to me like chaining your bicycle to itself to foil thieves. If any subsystem is implemented inside the same chip as the CPU, you don’t lose much by trusting it.

<h3 id="5.8.3">5.8.3 启动应用程序</h3>

To be able to start a C application (presumably with its instructions coming safely from ROM) you need writable memory, for three reasons.

First, you need stack space. Assign a large enough piece of writable memory and initialize the sp register to its upper limit (aligned to an eight-byte boundary). Working out how large the stack should be can be difficult: A generous guess will be more robust.

Then you may need some initialized data. Normally, m= the C data area is initialized by the program loader to set up any variables that have been allocated values. Most compilation systems that purport to be usable for embedded applications permit read-only data (implicit strings and data items declared const) to be managed in a separate segment of object code and put into ROM memory.

Initialized writable data can be used only if your compilation system and runtime system cooperate to arrange to copy writable data initializations from ROM into RAM before calling main().

Last, C programs use a different segment of memory for all static and extern data items that are not explicitly initialized—an area sometimes called “BSS,” for reasons long lost. Such variables should be cleared to zero, which is readily achieved by zeroing the whole data section before starting the program.

If your program is built carefully, that’s enough. However, it can get more complicated: Take care that your MIPS program is not built to use the global pointer register gp to speed access to nonstack variables, or you’ll need to do more initialization.

<h2 id="5.9">5.9 指令仿真</h2>

Sometimes an exception is used to invoke a software handler that will stand in for the exception victim instruction, as when you are using software to implement a floating-point operation on a CPU that doesn’t support FP in hardware. Debuggers and other system tools may sometimes want to do this too.

To emulate an instruction, you need to find it, decode it, and find its operands. A MIPS instruction’s operands were in registers, and by now those preexception register values have been put away in an exception frame somewhere. Armed with those values, you do the operation in software and patch the results back into the exception frame copy of the appropriate result register. You then need to fiddle with the stored exception return address so as to step over the emulated instruction and then return. We’ll go through these step by step.

Finding the exception-causing instruction is easy; it’s usually pointed to by EPC, unless it was in a branch delay slot, in which case Cause(BD) is set and the exception victim is at the address EPC + 4.

To decode the instruction, you need some kind of reverse-assembly table. A big decode-oriented table of MIPS instructions is part of the widely available GNU debugger gdb, where it’s used to generate disassembly listings. So long as the GNU license conditions aren’t a problem for you, that will save you time and effort.

To find the operands, you’ll need to know the location and layout of the exception frame, which is dependent on your particular OS (or exception handling software, if it’s too humble to be called an OS).

You’ll have to figure out for yourself how to do the operation, and once again you need to be able to get at the exception frame, to put the results back in the saved copy of the right register.

There’s a trap for the unwary in incrementing the stored EPC value to step over the instruction you’ve emulated. If your emulated instruction was in a branch delay slot, you can’t just return to the “next” instruction—that would be as if the branch was not taken, and you don’t know that.

So when your emulation candidate is in a branch delay slot, you also have to emulate the branch instruction, testing for whether the branch should be taken or not. If the branch should be taken, you need to compute its target and return straight there from the exception.

Fortunately, all MIPS branch instructions are simple and free of side effects.


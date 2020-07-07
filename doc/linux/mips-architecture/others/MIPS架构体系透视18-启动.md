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
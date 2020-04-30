<h1 id="0">0 目录</h1>

* [1.1 流水线](#1.1)
    - [1.1.1 制约流水线效率的因素](#1.1.1)
    - [1.1.2 制约流水线效率的因素](#1.1.2)
* [1.2 MIPS架构5级流水线](#1.2)
* [1.3 RISC和CISC对比](#1.3)
* [1.4 有代表性的MIPS架构芯片](#1.4)
* [1.5 MIPS架构和CISC架构的对比](#1.5)

---

众多RISC精简指令集架构中，MIPS架构是最优雅的"舞者"。就连它的竞争者也为其强大的影响力所折服。DEC公司的Alpha指令集（现在已被放弃）和HP的Precision都受其影响。虽说，优雅不足以让其在残酷的市场中固若金汤，但是，MIPS架构还是以最简单的设计成为每一代CPU架构中，执行效率最快的那一个。

作为一个从学术项目孵化而来的成果，简单的架构是MIPS架构商业化的需要。它拥有一个小的设计团队，制造和商业推广交给它的半导体合作伙伴们。结果就是，在半导体行业领域中，它拥有广泛的合作制造商：LSI逻辑、LSI、东芝、飞利浦、NEC和IDT等都是它的合作伙伴。值得一提的是，国内的龙芯买断了它的指令集架构，成为芯片国产化的佼佼者。

在低端CPU领域，MIPS架构昙花一现。目前主要应用于嵌入式行业，比如路由器领域，几乎占据了大部分的市场。

MIPS架构的CPU是RISC精简指令CPU中的一种，诞生于RISC学术研究最为活跃的时期。RISC（精简指令集计算机）这个缩略语是1986年-1989年之间诞生的所有新CPU架构的一个标签TAG，它为新诞生的这些高性能架构提供了想法上的创新。有人形象的说，"RISC是1984年之后诞生的所有计算机架构的基础"。虽说，略显夸张，但也是无可争议的事实，谁也无法忽略RISC精简指令集的那些先驱们所做的贡献。

MIPS领域最无法忽视的贡献者是Stanford大学的MIPS项目。之所以命名成MIPS，即可以代表`microcomputer without interlocked pipeline stages`-无互锁流水线的微处理器的英文意思，又可以代表`millions of instructions per second`每秒执行百万指令的意思，真是一语双关。看起来，MIPS架构主要研究方向还是CPU的流水线架构，让它如何更高效地工作。那接下来，我们就从流水线开始讲起。

> 流水线的互锁是影响CPU指令执行效率的关键因素之一。

<h2 id="1.1">1.1 流水线</h2>

假设有一家餐馆，我们称之为Evie的炸鱼薯条店。在其中，每一名顾客排队取餐（炸鳕鱼、炸薯片、豌豆糊、一杯茶）。店员装好盘子后，顾客找桌子坐下来就餐。这是，我们常见的餐馆就餐方式。

Evie的薯条是当地最好的小吃。所以，每当赶大集的时候，长长的队伍都要延伸到店外。所以，当隔壁的木屐店关门的时候，Evie盘下来，扩大了店面，增加了桌子。但是，这还是于事无补。因为忙碌的市民根本没空坐下来喝杯茶。（因为Evie还没有找到排长队的根本原因，......）

Evie炸鳕鱼和Bert的薯条是店里的招牌，来的顾客都会点这两样。但是他们只有一个柜台，所以，当有一名顾客执行点一杯茶的时候，而恰好他又在点鳕鱼和薯条的顾客后面，那他只能等待了.....。终于有一天，Evie想起了一个绝妙的主意：他们把柜台延长，Evie、Bert、Dionysus和Mary四名店员一字排开；每当顾客进来，Evie负责鳕鱼装盘，Bert添加薯条，Dionysus盛豌豆糊，Mary倒茶并结账。这样，一次可以有四名顾客同时被服务，多么天才的想法啊。

再也没有长长的排队人群，Evie的店收入增加了......。

这就是流水线，将重复的工作分成若干份，每个人负责其中一份。虽然每名顾客总的服务时间延长，但是，同时有四名顾客被服务，提高了整个取餐付账的效率。下图1-1就是Evie店里的流水线结构图。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_1_1.PNG">

那么，我们将CPU执行一条指令分解成取指令、解码、查找操作数、执行运算、存储结果五步操作的话，是不是跟上面Evie的店里的流水线就极其类似了呢。本质上讲，等待执行的程序就是一个个指令的队列，等待CPU一个个执行。

流水线当然不是RSIC指令集的新发明，CSIC复杂指令集也采用流水线的设计。差异就是，RSIC重新设计指令集，使流水线更有效率。那到底什么是制约流水线效率的关键因素呢？

<h3 id="1.1.1">1.1.1 制约流水线效率的因素</h3>

我们都知道那个著名的"木桶效应"：决定木桶的装水量的是最短的那个木头，而不是最长的。同样的，如果我们保证指令执行的每一步都占用相同时间的话，那么这个流水线将是一个完美的高效流水线。但现实往往是残酷的，CPU的工作频率远远大于读写内存的工作频率（都不是一个量级）。

让我们回到Evie的店中。顾客Cyril是一个穷人，经常缺钱，所以在Mary没有收到他的钱之前，Evie就不会为他服务。那么，现在Cyril就卡在Evie的位置处，知道Mary处理完前面三名顾客，再给他结完账，Evie才会继续为他服务。所以，在这个取餐的流水线上，Cyril就是一个麻烦制造者，因为他需要一个他人正在使用资源（Mary结账）。（这种情况可以对应CPU存储指令使用锁总线的方式保证对内存的独占访问。）

有时候，Daphne和Lola分别买一部分食物，然后彼此之间共享。比如说，Lola买不买薯条，取决于Daphne是否购买一杯茶。因为光吃薯条不喝点饮料的话，也许Daphne会噎着或者齁着。那么，Lola就会在售卖员Bert处着急等待Daphne在Mary处买一杯茶，这中间就发生了时间上的空隙，我们将其称为流水线上的间隙。（这是不是很像条件分支？）

当然了，不是所有的依赖关系都是坏的结果。假设有一名顾客Frank，总是模仿Fred点餐，也许Frank是Fred的粉丝呢。这其实蕴涵着通过Cache命中提高存取内存和分支预测工作效率的基础。

你当然在想，把这些麻烦的顾客剔除出去，不就是一个效率超高的流水线吗？但是，Evie还要在这儿生活，怎么可能得罪顾客呢。计算机处理器的行业大佬Intel现在也面临着这样的问题：不可能不兼容以前的软件，完全另起炉灶搞一个新的架构的话，可能会流失很多客户。于是，只能在旧的架构上缝缝补补又十年啊。这也给了RSIC指令集发展的大好机会。

<h3 id="1.1.2">1.1.2 流水线和Cache</h3>

计算机CPU处理速度和内存读取速度的匹配问题是提高CPU工作效率的关键，也就是"木桶效应"的那个短板。So，为了加速对内存的访问，CPU设计中引入了Cache。所谓的Cache，就是一个小的高速内存，用来拷贝内存中的一段数据。Cache中的最小数据单元是line，每个line对应一小段内存地址（常见的line大小为64字节）。每个Line不仅包含从主内存读取的数据，还包括其地址信息（TAG）和状态信息。当CPU想要访问内存中的数据时，先由内存管理单元搜索Cache，如果数据存在，则立即返回给CPU，这称为Cache命中；如果不存在，则称为Cache未命中，此时，内存管理单元再去主内存中查找相关数据，返回给CPU并在Cache中留下备份。Cache当然不知道CPU下一步需要什么数据，所以它只能保留CPU最近使用过的数据。如果需要为新拷贝的主内存数据，它就会选择合适的数据丢弃，这涉及到Cache替换策略算法。

Cache大约9成时间能够提供CPU想要的数据，所以大大提高了CPU读取数据的速率，从而提高了流水线的工作效率。

因为指令不同于数据，是只读属性，所以，MIPS架构采用哈弗结构，将数据Cache和指令Cache分开。这样就可以同时读取指令和读写变量了。

<h2 id="1.2">1.2 MIPS架构5级流水线</h2>

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_1_2.PNG">

图1.2: MIPS-5级流水线

MIPS本身就是基于流水线优化设计的架构，所以，将MIPS指令分为5个阶段，每个阶段占用固定的时间，在此，固定的时间其实就是处理器的时钟周期（有2个指令花费半个时钟周期，所以，MIPS的5级流水线实际上占据4个时钟周期）。

所有的指令都是严格遵守流水线的各个阶段，即使某个阶段什么也不做。这样做的结果就是，只要Cache命中，每个时钟周期CPU都会启动一条指令。

让我们看看每一个阶段都做了什么：

* **取指令-IF**
    
    从I-Cache中取要执行的指令。

* **读寄存器-RD** 
 
    取CPU寄存器中的值。

* **算术、逻辑运算-ALU**

    执行算术或逻辑运算。（浮点运算和乘除运算在一个时钟周期内无法完成，我们以后再写文章专门讲解FPU相关的知识）

* **读写内存-MEM**

    也就是读写D-Cache。至于这儿为什么说读写数据缓存，是因为内存的读写速度实在太慢了，无法满足CPU的需要。所以出现了D-Cache这种高速缓存。但即便如此，在读写D-Cache期间，平均每4条指令就会有3条指令什么也做不了。但是，每条指令在这个阶段都应该是独占数据总线的，不然会造成访问D-Cache冲突。这就是内存屏障和总线锁存在的理由。

* **写回寄存器-Writeback**

    将结果写入寄存器。

当然，上面的流水线只是一个理论上的模型而已。但实际上，有一些MIPS架构的CPU会有更长的流水线或者其它的不同。但是，5级流水线架构确是这一切的出发点和基础。

流水线的严格规定，限制了指令可以做的事情。

首先，所有的指令具有相同的长度（32位），读取指令使用相同的时间。这降低了流水线的复杂度，比如，指令中没有足够的位用来编码复杂的寻址模式。

这种限制当然也有不利：基于X86架构的程序，平均指令长度也只有大约3个字节多一点。所以，MIPS架构占用更多的内存空间。

第二，MIPS架构的流水线设计排除了对内存变量进行任何操作的指令的实现。内存数据的获取只能在阶段4，这对于算术逻辑单元有点延迟。内存访问只能通过load或store指令进行。（MIPS架构的汇编也是最简单易懂的代码之一）

尽管有这些问题，但是MIPS架构的设计者也在思考，如何使CPU可以被编译器更加简单高效地优化。一些编译器高效优化的要求和流水线的设计要求是兼容的，所以MIPS架构的CPU具有32个通用寄存器，使用具有三个操作数的算术/逻辑指令。那些复杂的特殊目的的指令也是编译器不愿意产生的。通俗地讲，编译器能不用复杂指令就不用复杂指令。

<h2 id="1.3">1.3 RISC和CISC对比</h2>

We can now have a go at defining what we mean by these overused terms. For me, RISC is an adjective applied to machine architectures/instruction sets. In the mid-1980s, it became attached to a group of relatively new architectures in which the instruction set had been cunningly and effectively specified to make pipelined implementations efficient and successful. It’s a useful term because of the great similarity of approach apparent in SPARC, MIPS, PowerPC, HP Precision, DEC Alpha, and (to a lesser extent) in ARM.

By contrast to this rather finely aimed description, CISC (Complex Instruction Set Computing) is used negatively to describe architectures whose definition has not been shaped by those insights about pipelined implementations. The RISC revolution was so successful that no post-1985 architecture has abandoned the basic RISC principles;4 thus, CISC architectures are inevitably those born before 1985. In this book you can reasonably assume that something said about CISC is being said to apply to both Intel’s x86 family and Motorola’s 680x0.

Both terms are corrupted when they are applied not to instruction sets but to implementations. It’s certainly true that Intel accelerated the performance of its far-from-RISC x86 family by applying implementation tricks pioneered by RISC builders. But to describe these implementations as having a RISC core is misleading.

<h2 id="1.4">1.4 有代表性的MIPS架构芯片</h2>

It’s time to take a tour through the evolution of MIPS processors and the systems that use them, over the span of the past 20 years or so. We’ll look at events in the order they occurred, roughly speaking, with a few scenic detours. Along the way, we’ll see that although the MIPS architecture was originally devised with UNIX workstations in mind, it has since found its way into all sorts of other applications—many of which could hardly have been foreseen during the early years. You’ll get to know some of these names much better in the chapters that follow.

And although much has happened to the instruction set as well as the silicon, the user-level software from a 1985 R2000 would run perfectly well and quite efficiently on any modern MIPS CPU. That’s possibly the best backward compatibility achievement of any popular architecture.

<h2 id="1.5">1.5 MIPS和CISC的对比</h2>

Programmers who have some assembly-language-level knowledge of earlier architectures—particularly those brought up on x86 or 680x0 CISC instruction sets—may get some surprises from the MIPS instruction set and register model.We’ll try to summarize them here, so you don’t get sidetracked later into doomed searches for things that don’t quite exist, like a stack with push/pop instructions!

We’ll consider the following: constraints on MIPS operations imposed to make the pipeline efficient; the radically simple load/store operations; possible operations that have been deliberately omitted; unexpected features of the instruction set; and the points where the pipelined operation becomes visible to the programmer.

The Stanford group that originally dreamed up MIPS was paying particular attention to the short, simple pipeline it could afford to build. But it’s a testament to the group’s judgment that many of the decisions that flowed from that have proven to make more ambitious implementations easier and faster, too.

1.5.1 Constraints on MIPS Instructions

<h3 id="1.5.1">1.5.1 MIPS指令集的限制</h3>

* All instructions are 32 bits long:

    That means that no instruction can fit into only two or three bytes of memory (so MIPS binaries are typically 20 percent to 30 percent bigger than for 680x0 or 80x86) and no instruction can be bigger.

    It follows that it is impossible to incorporate a 32-bit constant into a single instruction (there would be no instruction bits left to encode the operation and the target register). The MIPS architects decided to make space for a 26-bit constant to encode the target address of a jump or jump to subroutine; but that’s only for a couple of instructions. Other instructions find room only for a 16-bit constant. It follows that loading an arbitrary 32-bit value requires a two-instruction sequence, and conditional branches are limited to a range of 64-K instructions.

* Instruction actions must fit the pipeline:

    Actions can only be carried out in the right pipeline phase and must be complete in one clock. For example, the register write-back phase provides for just one value to be stored in the register file, so instructions can only change one register.

    Integer multiply and divide instructions are too important to leave out but can’t be done in one clock. MIPS CPUs have traditionally provided them by dispatching these operations into a separately pipelined unit we’ll talk about later.

* Three-operand instructions:

    Arithmetical/logical operations don’t have to specify memory locations, so there are plenty of instruction bits to define two independent sources and one destination register. Compilers love three-operand instructions, which give optimizers much more scope to improve code that handles complex expressions.

* The 32 registers:

    The choice of the number of registers is largely driven by software requirements, and a set of 32 general-purpose registers is easily the most popular in modern architectures. Using 16 would definitely not be as many as modern compilers like, but 32 is enough for a C compiler to keep frequently accessed data in registers in all but the largest and most intricate functions. Using 64 or more registers requires a bigger instruction field to encode registers and also increases context-switch overhead.

* Register zero:

    $0 always returns zero, to give a compact encoding of that useful constant.

* No condition codes:

    One feature of the MIPS instruction set that is radical even among the 1985 RISCs is the lack of any condition flags. Many architectures have multiple flags for “carry,” “zero,” and so on. CISC architectures typically set these flags according to the result written by any or a large subset of machine instructions, while some RISC architectures retain flags (though typically they are only set explicitly, by compare instructions).

    The MIPS architects decided to keep all this information in the register file: Compare instructions set general-purpose registers and conditional branch instructions test general-purpose registers. That does benefit a pipelined implementation, in that whatever clever mechanisms are built in to reduce the effect of dependencies on arithmetical/logical operations will also reduce dependencies in compare/branch pairs.

    We’ll see later that efficient conditional branching (at least in one favorite simple pipeline organization) means that the decision about whether to branch or not has to be squeezed into only half a pipeline stage; the architecture helps out by keeping the branch decision tests very simple. So MIPS conditional branches test a single register for sign/zero or a pair of registers for equality.

<h3 id="1.5.2">1.5.2 寻址和内存访问</h3>

* Memory references are always plain register loads and stores:

    Arithmetic on memory variables upsets the pipeline, so it is not done. Every memory reference has an explicit load or store instruction. The large register file makes this much less of a problem than it sounds.

* Only one data-addressing mode:

    Almost all loads and stores select the memory location with a single base register value modified by a 16-bit signed displacement (a limited register-plus-register address mode is available for floating-point data).

* Byte-addressed:

    Once data is in a register of a MIPS CPU, all operations always work on the whole register. But the semantics of languages such as C fit badly on a machine that can’t address memory locations down to byte granularity, so MIPS gets a complete set of load/store operations for 8- and 16-bit variables (we will say byte and halfword). Once the data has arrived in a register it will be treated as data of full register length, so partial-word load instructions come in two flavors—sign-extend and zero-extend.

* Load/stores must be aligned:

    Memory operations can only load or store data from addresses aligned to suit the data type being transferred. Bytes can be transferred at any address, but halfwords must be even-aligned and word transfers aligned to four-byte boundaries. Many CISC microprocessors will load/store a four-byte itemfromany byte address, but the penalty is extra clock cycles.

    However, the MIPS instruction set architecture (ISA) does include a couple of peculiar instructions to simplify the job of loading or storing at improperly aligned addresses.

* Jump instructions:

    The limited 32-bit instruction length is a particular problem for branches in an architecture that wants to support very large programs. The smallest opcode field in a MIPS instruction is 6 bits, leaving 26 bits to define the target of a jump. Since all instructions are four-byte aligned in memory, the two least significant address bits need not be stored, allowing an address range of 228 = 256 MB. Rather than make this branch PC relative, this is interpreted as an absolute address within a 256-MB segment. That’s inconvenient for single programs larger than this, although it hasn’t been much of a problem yet! Branches out of segment can be achieved by using a jumpregister instruction, which can go to any 32-bit address.

    Conditional branches have only a 16-bit displacement field—giving a 218-byte range, since instructions are four-byte aligned—which is interpreted as a signed PC-relative displacement. Compilers can only code a simple conditional branch instruction if they know that the target will be within 128 KB of the instruction following the branch.

<h3 id="1.5.3">1.5.3 MIPS没有的特性</h3>

* No byte or halfword arithmetic:

    All arithmetical and logical operations are performed on 32-bit quantities. Byte and/or halfword arithmetic requires significant extra resources and many more opcodes, and it is rarely really useful. The C language’s semantics cause most calculations to be carried out with int precision, and for MIPS int is a 32-bit integer. However, where a program explicitly does arithmetic as short or char, a MIPS compiler must insert extra code to make sure that the results wrap and overflow as they would on a native 16- or 8-bit machine.

* No special stack support:

    Conventional MIPS assembly usage does define one of the registers as a stack pointer, but there’s nothing special to the hardware about sp. There is a recommended format for the stack frame layout of subroutines, so that you can mix modules from different languages and compilers; you should almost certainly stick to these conventions, but they have no relationship to the hardware. A stack pop wouldn’t fit the pipeline, because it would have two register values to write (the data from the stack and the incremented pointer value).

* Minimal subroutine support:

    There is one special feature: jump instructions have a jump and link option, which stores the return address into a register. $31 is the default, so for convenience and by convention $31 becomes the return address register.

    This is less sophisticated than storing the return address on a stack, but it has some significant advantages. Two examples will give you a feeling for the argument: First, it preserves a pure separation between branch and memory-accessing instructions; second, it can aid efficiency when calling small subroutines that don’t need to save the return address on the stack at all.

* Minimal interrupt handling:

    It is hard to see how the hardware could do less. It stashes away the restart location in a special register, modifies the machine state just enough to let you find out what happened and to disallow further interrupts, then jumps to a single predefined location in low memory. Everything else is up to the software.

* Minimal exception handling:

    Interrupts are just one sort of exception (the MIPS word exception covers all sorts of events where the CPU may want to interrupt normal sequential processing and invoke a software handler). An exception may result from an interrupt, an attempt to access virtual memory that isn’t physically present, or many other things. You go through an exception, too, on a deliberately planted trap instruction like a system call that is used to get into the kernel in a protected OS. All exceptions result in control passing to the same fixed entry point.7

    On any exception, a MIPS CPU does not store anything on a stack, write memory, or preserve any registers for you.

    By convention, two general-purpose registers are reserved so that exception routines can bootstrap themselves (it is impossible to do anything on a MIPS CPU without using some registers). For a program running in any system that takes interrupts or traps, the values of these registers may change at any time, so you’d better not use them.

<h3 id="1.5.4">1.5.4 MIPS架构流水线的好处</h3>

So far, this has all been what you might expect from a simplified CPU. However, making the instruction set pipeline friendly has some stranger effects as well, and to understand them we’re going to draw some pictures.

* Delayed branches:

    The pipeline structure of the MIPS CPU (Figure 1.3) means that when a jump/branch instruction reaches the execute phase and a new program counter is generated, the instruction after the jump will already have been started. Rather than discard this potentially useful work, the architecture dictates that the instruction after a branch must always be executed before the instruction at the target of the branch. The instruction position following any branch is called the branch delay slot. If nothing special was done by the hardware, the decision to branch or not, together with the branch target address, would emerge at the end of the ALU pipestage—by which time, as Figure 1.3 shows, you’re too late to present an address for an instruction in even the next-but-one pipeline slot.

    But branches are important enough to justify special treatment, and you can see fromFigure 1.3 that a special path is provided through the ALU to make the branch address available half a clock cycle early. Together with the odd half-clock-cycle shift of the instruction fetch stage, that means that the branch target can be fetched in time to become the next but one,so the hardware runs the branch instruction, then the branch delay slot instruction, and then the branch target—with no other delays.

    It is the responsibility of the compiler system or the assembly programming wizard to allowfor and even to exploit the branch delay; it turns out that it is usually possible toarrange that the instruction in the branchdelay slot does useful work. Quite often, the instruction that would otherwise have been placed before the branch can be moved into the delay slot.

    This can be a bit tricky on a conditional branch, where the branch delay instruction must be (at least) harmless on both paths. Where nothing useful can be done, the delay slot is filled with a nop instruction. Many MIPS assemblers will hide this odd feature from you unless you explicitly ask them not to.

* Late data from load (load delay slot):

    Another consequence of the pipeline is that a load instruction’s data arrives from the cache/memory system after the next instruction’s ALU phase starts—so it is not possible to use the data from a load in the following instruction. (See Figure 1.4 for how this works.)

    The instruction position immediately after the load is called the load delay slot, and an optimizing compiler will try to do something useful with it. The assembler will hide this from you but may end up putting in a nop. On modern MIPS CPUs the load result is interlocked: If you try to use the result too early, the CPU stops until the data arrives. But on early MIPS CPUs, there were no interlocks, and the attempt to use data in the load delay slot led to unpredictable results.

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
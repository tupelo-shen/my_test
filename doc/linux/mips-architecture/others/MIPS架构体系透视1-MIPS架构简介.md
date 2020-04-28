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

<img src="">

那么，我们将CPU执行一条指令分解成取指令、解码、查找操作数、执行运算、存储结果五步操作的话，是不是跟上面Evie的店里的流水线就极其类似了呢。本质上讲，等待执行的程序就是一个个指令的队列，等待CPU一个个执行。

流水线当然不是RSIC指令集的新发明，CSIC复杂指令集也采用流水线的设计。差异就是，RSIC重新设计指令集，使流水线更有效率。那到底什么是制约流水线效率的关键因素呢？

<h3 id="1.1.1">1.1.1 制约流水线效率的因素</h3>

我们都知道那个著名的"木桶效应"：决定木桶的装水量的是最短的那个木头，而不是最长的。同样的，如果我们保证指令执行的每一步都占用相同时间的话，那么这个流水线将是一个完美的高效流水线。但现实往往是残酷的，CPU的工作频率远远大于读写内存的工作频率（都不是一个量级）。

让我们回到Evie的店中。顾客Cyril是一个穷人，经常缺钱，所以在Mary没有收到他的钱之前，Evie就不会为他服务。那么，现在Cyril就卡在Evie的位置处，知道Mary处理完前面三名顾客，再给他结完账，Evie才会继续为他服务。所以，在这个取餐的流水线上，Cyril就是一个麻烦制造者，因为他需要一个他人正在使用资源（Mary结账）。
Daphne and Lola always come in together (in that order) and share their meals. Lola won’t have chips unless Daphne gets some tea (too salty without something to drink). Lola waits on tenterhooks in front of Bert until Daphne gets to Mary, and so a gap appears in the pipeline. This is a dependency (and the gap is called a pipeline bubble).

Not all dependencies are a problem. Frank always wants exactly the same meal as Fred, but he can follow him down the counter anyway — if Fred gets chips, Frank gets chips. . .

If you could get rid of awkward customers, you could make a more efficient pipeline. This is hardly an option for Evie, who has to make her living in a town of eccentrics. Intel is faced with much the same problem: The appeal of its CPUs relies on the customer being able to go on running all that old software. But with a new CPU you get to define the instruction set, and you can define many of the awkward customers out of existence. In Section 1.2 we’ll show how MIPS did that, but first we’ll come back to computer hardware in general with a discussion of caching.

<h3 id="1.1.2">1.1.2 流水线和缓存</h3>

We said earlier that efficient pipeline operation requires every stage to take the same amount of time. But a 1996 CPU can add two 64-bit numbers about 10 times quicker than it can fetch a piece of data from memory.

So effective pipelining relies on another technique to speed most memory accesses by a factor of 10-the use of caches. A cache is a small, very fast, local memory that holds copies of memory data. Each piece of data is kept with a record of its main memory address (the cache tag) and when the CPU wants data the cache gets searched and, if the requisite data is available, it’s sent back quickly. Since we’ve no way to guess what data the CPU might be about to use, the cache merely keeps copies of data the CPU has had to fetch from main memory in the recent past; data is discarded from the cache when its space is needed for more data arriving from memory.

Even a simple cache will provide the data the CPU wants more than 90% of the time, so the pipeline design need only allow enough time to fetch data from the cache: A cache miss is a relatively rare event and we can just stop the CPU when it happens.

<img src="">

Figure 1.2: MIPS five-stage pipeline

The MIPS architecture was planned with separate instruction and data caches, so it can fetch an instruction and read or write a memory variable simultaneously.

CISC architectures have caches too, but they’re most often afterthoughts, fitted in as a feature of the memory system. A RISC architecture makes more sense if you regard the caches as very much part of the CPU and tied firmly into the pipeline.

<h2 id="1.2">1.2 MIPS架构5级流水线</h2>

The MIPS architecture is made for pipelining, and Figure 1.2 shows the pipeline of most MIPS CPUs. So long as the CPU runs from the cache, the execution of every MIPS instruction is divided into five phases (called pipestages), with each pipestage taking a fixed amount of time. The fixed amount of time is usually a processor clock rycle (though some actions take only half a clock, so the MIPS five-stage pipeline actually occupies only four clock cycles).

All instructions are rigidly defined so they can follow the same sequence of pipestages, even where the instruction does nothing at some stage. The net result is that, so long as it keeps hitting the cache, the CPU starts an instruction every clock cycle.

Let’s look at Figure 1.2 and consider what happens in each pipestage.

* **IF-取指令**
    
    从I-Cache中取要执行的指令。

* **RD-读寄存器** 
 
    取CPU寄存器中的值。

* **ALU-算术逻辑单元**

    执行算术或逻辑运算。（浮点运算和乘除运算在一个时钟周期内无法完成，我们以后再写文章专门讲解FPU相关的知识）

* **MEM-读写内存**

    也就是读写D-Cache。至于这儿为什么说读写数据缓存，是因为内存的读写速度实在太慢了，无法满足CPU的需要。所以出现了D-Cache这种高速缓存。但即便如此，在读写D-Cache期间，平均每4条指令就会有3条指令什么也做不了。但是，每条指令在这个阶段都应该是独占数据总线的，不然会造成访问D-Cache冲突。这就是内存屏障和总线锁存在的理由。

* **WB-写回寄存器**

    将结果写入寄存器。

You may have seen other pictures of the MIPS pipeline that look slightly different; it has been common practice to simplify the picture by drawing each pipestage as if it takes exactly one clock cycle. Some later MIPS CPUs have longer or slightly different pipelines, but the pipeline with five stages in four cycles is where the architecture started, and implementations keep returning to something very close to it.

The tyranny of the rigid pipeline limits the kinds of things instructions can do. Firstly, it forces all instructions to be the same length (exactly one machine word of 32 bits), so that they can be fetched in a constant time. This itself discourages complexity; there are not enough bits in the instruction to encode really complicated addressing modes, for example.

This limitation has an immediate disadvantage; in a typical program built for an architecture like x86, the average size of instructions is only just over 3 bytes. MIPS code will use more memory space.

Secondly, the pipeline design rules out the implementation of instructions that do any operation on memory variables. Data from cache or memory is obtained only in phase 4, which is much tao late to be available to the ALU. Memory accesses occur only as simple load or store instructions that move the data to or from registers (you will see this described as a load/store architecture).

The RISC CPUs launched around 1987 worked because these restrictions don’t cause much trouble. An 87 or later RISC is characterized by an instruction set designed for efficient pipelining and the use of caches.

However, the MIPS project architects also attended to the best thinking of the time about what makes a CPU an easy target for efficient optimizing compilers. Many of those requirements are quite compatible with the pipeline requirements, so MIPS CPUs have 32 general-purpose registers and threeoperand arithmetical/logical instructions. Happily, the complicated specialpurpose instructions that particularly upset pipelines are often those that compilers are unwilling to generate.


<h2 id="1.3">1.3 RISC和CISC对比</h2>
<h2 id="1.4">1.4 有代表性的MIPS架构芯片</h2>
<h2 id="1.5">1.5 MIPS架构和CISC架构的对比</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
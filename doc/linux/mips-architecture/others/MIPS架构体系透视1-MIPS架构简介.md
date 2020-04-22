<h1 id="0">0 目录</h1>

* [1.1 流水线](#1.1)
    - [1.1.1 制约流水线效率的因素](#1.1.1)
    - [1.1.2 制约流水线效率的因素](#1.1.2)
* [1.2 MIPS架构5级流水线](#1.2)
* [1.3 RISC和CISC对比](#1.3)
* [1.4 有代表性的MIPS架构芯片](#1.4)
* [1.5 MIPS架构和CISC架构的对比](#1.5)

---

MIPS is the most elegant among the effective RISC architectures; even the competition thinks so, as evidenced by the strong MIPS influence seen in later architectures like DEC’s Alpha and HP’s Precision. Elegance by itself doesn’t get you far in a competitive marketplace, but MIPS microprocessors have usually managed to be among the fastest of each generation by remaining among the simplest.

Relative simplicity was a commercial necessity for MIPS, which spun off from an academic project as a small design group using multiple semiconductor partners to make and market the chips. As a result the architecture has the largest range of active manufacturers in the industry - working from ASIC cores (LSI Logic, Toshiba, Philips, NEC) through low-cost CPUs (iDT, LSI) and from low-end 64-bit (IDT, NKK, NEC) to the top (NEC, Toshiba, and IDT).

At the low end the CPU is 1.5 mm2 (rapidly disappearing from sight in the “system on a chip”); at the high end the R10000 is nearly an inch square and generates 30W of heat — and when first launched was probably the fastest CPU on the planet. And although MIPS looks like an outsider, sales volumes seem healthy enough: 44M MIPS CPUs were shipped in 1997, mostly into embedded applications.

The MIPS CPU is one of the RISC CPUs, born out of a particularly fertile period of academic research and development. RISC(reduced instruction set computer) is an attractive acronym that, like many such, probably obscures reality more than it reveals it. But it does serve as a useful tag for a number of new CPU architectures launched between 1986 and 1989, which owe their remarkable performance to ideas developed a few years earlier by a couple of seminal research projects. Someone commented that “a RISC is any computer architecture defined after 1984”; although meant as a jibe at the industry’s use of the acronym, the comment is also true — no computer defined after 1984 can afford to ignore the RISC pioneers’ work.

One of these pioneering projects was the MIPS project at Stanford. Theproject name MIPS (named for the key phrase microcomputer without interlocked pipeline stages) is also a pun on the familiar unit ”millions of instructions per second” The Stanford group’s work showed that pipelining, although a well-known technique, had been drastically underexploited by earlier architectures and could be much better used, particularly when combined with 1980 silicon design.

1.1 Pipelines
<h2 id="1.1">1.1 流水线</h2>

Once upon a time in a small town in the north of England, there was Evie’s fish and chip shop. Inside, each customer got to the head of the queue and asked for his or her meal (usually fried cod, chips, mushy peas,1 and a cup of tea). Then each customer waited for the plate to be filled before going to sit down.

Evie’s chips were the best in town, and every market day the lunch queue stretched out of the shop. So when the clog shop next door shut down, Evie rented it and doubled the number of tables. But they couldn’t fill them! The queue outside was as long as ever, and the busy townsfolk had no time to sit over their cooling tea.

They couldn’t add another serving counter; Evie’s cod and Bert’s chips were what made the shop. But then they had a brilliant idea. They lengthened the counter and Evie, Bert, Dionysus, and Mary stood in a row. As customers came in, Evie gave them a plate with their fish, Bert added the chips, Dionysus spooned out the mushy peas, and Mary poured the tea and took the money. The customers kept walking; as one customer got his peas, the next was already getting chips and the one after that fish. Less hardy folk don’t eat mushy peas — but that’s no problem, those customers just got nothing but a vacant smile from Dionysus.

The queue shortened and soon they bought the shop on the other side as well for extra table space. ..

That’s a pipeline. Divide any repetitive job into a number of sequential parts and arrange that the work moves past the workers, with each specialist doing his or her part for each unit of work in turn. Although the total time any customer spends being served has gone up, there are four customers being served at once and about three times as many customers being served in that market day lunch hour. Figure 1.1 shows Evie’s organization, as drawn by her son Einstein in a rare visit to non-virtual reality.2

Seen as a collection of instructions in memory, a program ready to run doesn’t look much like a queue of customers. But when you look at it from the CPU’s point of view, things change. The CPU fetches each instruction from memory, decodes it, finds any operands it needs, performs the appropriate action, and stores any results — and then it goes and does the same thing all over again. The program waiting to be run is a queue of instructions waiting to flow through the CPU one at a time.

The use of pipelining is not new with RISC microprocessors. What makes the difference is the redesign of everything — starting with the instruction set — to make the pipeline more efficient.1 So how do you make a pipeline efficient? Actually, that’s probably the wrong question. The right one is, what makes a pipeline inefficient?

<h3 id="1.1.1">1.1.1 制约流水线效率的因素</h3>

lt’s not good if one stage takes much longer than the others. The organization
of Evie’s shop depends on Mary’s ability to pour tea with one hand while
giving change with the other — if Mary takes longer than the others, the
whole queue will have to slow down to match her.

In a pipeline, you try to make sure that every stage takes roughly the
same amount of time. A circuit design often gives you the opportunity to
trade the complexity of logic off against its speed, so designers can assign
work to different stages until everything is just right.

The hard problem is not difficult actions, it’s awkward customers. Back in
the chip shop Cyril is often short of cash, so Evie won’t serve him until Mary
has counted his money. When Cyril arrives, he’s stuck at Evie’s position
until Mary has finished with the three previous customers and can check his
pile of old bent coins. Cyril is trouble because when he comes in he needs a
resource (Mary’s counting) that is being used by previous customers. He’s a
resource conflict.

Daphne and Lola always come in together (in that order) and share their
meals. Lola won’t have chips unless Daphne gets some tea (too salty without
something to drink). Lola waits on tenterhooks in front of Bert until Daphne
gets to Mary, and so a gap appears in the pipeline. This is a dependency (and
the gap is called a pipeline bubble).

Not all dependencies are a problem. Frank always wants exactly the same
meal as Fred, but he can follow him down the counter anyway — if Fred gets
chips, Frank gets chips. . .

If you could get rid of awkward customers, you could make a more efficient
pipeline. This is hardly an option for Evie, who has to make her living in a
town of eccentrics. Intel is faced with much the same problem: The appeal
of its CPUs relies on the customer being able to go on running all that old
software. But with a new CPU you get to define the instruction set, and you
can define many of the awkward customers out of existence. In Section 1.2
we’ll show how MIPS did that, but first we’ll come back to computer hardware
in general with a discussion of caching.

<h3 id="1.1.2">1.1.2 流水线和缓存</h3>

We said earlier that efficient pipeline operation requires every stage to take
the same amount of time. But a 1996 CPU can add two 64-bit numbers about
10 times quicker than it can fetch a piece of data from memory.

So effective pipelining relies on another technique to speed most memory
accesses by a factor of 10-the use of caches. A cache is a small, very fast,
local memory that holds copies of memory data. Each piece of data is kept
with a record of its main memory address (the cache tag) and when the CPU
wants data the cache gets searched and, if the requisite data is available, it’s
sent back quickly. Since we’ve no way to guess what data the CPU might be
about to use, the cache merely keeps copies of data the CPU has had to fetch
from main memory in the recent past; data is discarded from the cache when
its space is needed for more data arriving from memory.

Even a simple cache will provide the data the CPU wants more than 90%
of the time, so the pipeline design need only allow enough time to fetch data
from the cache: A cache miss is a relatively rare event and we can just stop
the CPU when it happens.

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
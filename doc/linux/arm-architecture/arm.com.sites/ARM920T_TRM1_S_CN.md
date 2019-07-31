<h1 id="0">0 目录</h1>
* [1 引言](#1)
    - [1.1 关于ARM920T](#1.1)
    - [1.2 功能框图](#1.2)
* [2 编程者模型](#2)
    - [2.1 关于编程者模型](#2.1)
        + [2.1.1 访问权限和域](#2.1.1)
    - [2.2 关于ARM9TDMI编程者模型](#2.2)
    - [2.3 CP15寄存器映射](#2.3)
* [3 内存管理单元-MMU](#3)
    - [3.1 关于MMU](#3.1)
        + [3.1.1 访问权限和域](#3.1.1)
    - [3.2 MMU可编程寄存器](#3.2)
    - [3.3 地址变换](#3.3)
* [4 Cache、写缓冲区、和PA TAG RAM](#4)
    - [4.2 ICache](#4.2)
        + [4.2.1 ICache组织结构](#4.2.1)
        + [4.2.2 使能和禁止ICache](#4.2.2)
    - [4.3 DCache & 写缓存](#4.3)


---

<h1 id="1">1 引言</h1>

<h2 id="1.1">1.1 关于ARM920T</h2>

ARM920T处理器是通用目的微处理器ARM9TDMI家族的一员，它包含：

1. ARM9TDMI（核）
2. ARM940T（core plus cache and protection unit）
3. ARM920T（core plus cache and MMU）

ARM9TDMI处理器核是基于5级流水线的哈弗结构，包括取指、解码、执行、内存和写这5个步骤。它可以作为一个单独的核嵌入到其它更为复杂的系统中。独立的核提供一个总线接口，允许用户设计自己的cache和内存系统。

ARM9TDMI处理器家族支持32位ARM和16位Thumb指令集，允许用户在高性能和高代码密度两方面作平衡。

ARM920T处理器是一种哈佛缓存架构处理器，面向多任务应用，支持内存管理，高性能，低功耗。ARM920T处理器实现了一个增强ARM架构v4 MMU，提供指令和数据地址的转译以及访问权限的检查。

ARM920T处理器支持ARM debug架构，增加了辅助硬件和软件debug的逻辑。ARM920T处理器也支持协处理器，并 输出指令和数据总线，该总线带有简单的握手信号。

对于系统其余部分，ARM920T使用统一的地址和数据总线作为通信接口。该接口可实现高级微控制器总线架构（AMBA）高级系统总线（ASB）或高级高性能总线（AHB）方案，AHB既可以作为完全兼容的AMBA总线主机，也可以作为产品测试时的从机。ARM920T处理器还具有跟踪ICE模式，其允许采用类似与传统ICE操作模式的方法。

ARM920T处理器支持添加嵌入式跟踪宏单元（ETM），用于实时跟踪指令和数据。

<h2 id="1.2">1.2 功能框图</h2>


图1-1展示了ARM920T处理器的功能框图：

![ARM920T功能框图](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/picture1-1.PNG)

图示说明：

1. 关于ARM9TDMI核，可以参考《ARM9TDMI 技术参考手册》中的描述。
2. 关于寄存器13和协处理器15在[第2章 编程者模型](#2)中进行描述。
3. 指令和数据MMU在[第3章 内存管理单元](#3)中进行描述。
4. 指令和数据cache，写缓冲区，write-back PA TAG RAM将在[第4章 Cache、写缓冲区、和PA TAG RAM](#4)中进行描述。
5. AMBA总线接口将在[第6章 总线接口单元](#6)进行描述。
6. 外部协处理器接口将在[第7章 协处理器接口](#7)进行描述。
7. 跟踪接口端口将在[第8章 跟踪接口](#8)进行描述。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="2">2 编程者模型</h1>

本章描述了ARM920T的寄存器，提供了对其编程时所需要的信息。

<h2 id="2.1">2.1 关于编程者模型</h2>

ARM920T处理器采用ARM9TDMI完整内核，其实现了ARM v4T架构。支持ARM和Thumb指令集，并包含嵌入式ICE JTAG软件调试功能。

ARM920T处理器的编程模型是在ARM9TDMI核的编程模型的基础上进行添加和修改而成的。

* ARM920T处理器包含2个协处理器： 
    - CP14， 允许软件访问debug通信通道。可以使用指令MCR和MRC指令访问这些寄存器。这些将会在Debug通信通道部分进行描述。
    - 系统控制协处理器，CP15，提供寄存器控制cache，MMU，保护系统，时钟模式以及ARM920T的其它系统选项、比如大小端的选择。同上，也可以使用MCR和MRC指令访问这些寄存器。
* ARM920T还有一个外部协处理器接口，允许在同一块芯片上连接一个紧密耦合的协处理器，比如，一个浮点计算单元。用户可以使用适当的协处理器指令访问连接扫外部协处理器接口的任何协处理器提供的寄存器和操作。
* 用于指令提取和数据加载、存储的内存访问可以被cache或者缓存。Cache和write buffer的操作可以参考[第4章 Cache、写缓冲区、和PA TAG RAM](#4)。
* 位于主内存中的MMU页表，描述了虚拟到物理地址的映射，访问权限以及cache和write buffer的配置。


<h2 id="2.2">2.2 关于编程者模型</h2>

本章描述了ARM920T的寄存器，提供了对其编程时所需要的信息。

<h2 id="2.3">2.3 CP15寄存器映射</h2>

CP15定义了16个寄存器。其内容见表2-2。

|register   |Read           |WRITE|
|--------   | ------------- |------------|
| 0         | ID code       | 不可预料 |
| 0         | Cache类型      | 不可预料 |
| 1         | Control       | Control |
| 2         | 转换表基地址   | 转换表基地址 |
| 3         | 域访问控制     | 域访问控制 |
| 4         | 不可预料       | 不可预料 |
| 省略... |

> 寄存器位置0处，可以当做2个寄存器，具体的行为依赖于opcode_2的值。

<h3 id="2.3.1">2.3.1 访问权限和域</h3>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>



<h1 id="3">3 内存管理单元-MMU</h1>

<h2 id="3.1">3.1 关于MMU</h2>

ARM920T处理器提供了一个增强型的ARMv4 MMU，用来对指令和数据进行转换以及访问权限的检查。MMU由存储在主内存中的一组2级页表控制，由CP15寄存器1中的M位进行使能，提供了一种简单的地址转换和保护机制。MMU中的指令和数据转换表可以独立地进行加锁和刷新操作。

主要功能：

1. 标准ARMv4 MMU映射大小，域和访问保护机制
2. 映射大小：段（1MB）、大页面（64KB）、小页面（4KB）和微小页面（1KB）
3. 提供对段访问的权限
4. 大页面和小页面的访问权限可以分别为其子页面进行指定（这些子页面就是每个page的1/4）
5. 16 domains implemented in hardware
6. 64 entry instruction TLB and 64 entry data TLB
7. hardware page table walks
8. round-robin replacement algorithm (also called cyclic)
9. invalidate whole TLB, using CP15 register 8
10. invalidate TLB entry, selected by MVA, using CP15 register 8
11. independent lockdown of instruction TLB and data TLB, using CP15 register 10.

<h3 id="3.1.1">3.1.1 访问权限和域</h3>

对于大页面和小页面，可以为每一个子页面定义访问权限（对于小页面来说就是1KB，对于大页面来说就是16KB）。段和微小页面具有单独一组访问权限。

所有的内存区域都有一个关联的域。域是内存访问的主要控制机制。它定义了访问能够进行的必要条件。域决定了是否：

1. 访问权限用于限定访问
2. 访问可以无条件的进行
3. 访问无条件中止

在后2种情况下，访问权限的属性被忽略。

有16种域。使用域访问控制寄存器进行配置。

<h3 id="3.1.2">3.1.2 转换项</h3>

每个TLB缓存着64个转换项。在CPU内存访问期间，TLB提供保护信息给访问控制逻辑单元。

如果TLB包含一个MVA中转换后的项，则由访问控制逻辑单元决定是否允许访问：

1. 如果允许访问，且需要片外访问，则MMU输出对应于MVA的正确的物理地址
2. 如果允许访问，且不要求片外访问，则Cache响应此次访问
3. 如果不允许访问，则MMU通知CPU放弃此次访问

如果TLB未命中（也就是不包含VA中的项），则转换表

<h2 id="3.2">3.2 MMU可编程寄存器</h2>

<h2 id="3.3">3.3 地址转换</h2>

<h1 id="4">4 Cache、写缓冲区、和PA TAG RAM</h1>

<h2 id="4.2">4.2 ICache</h2>

ARM920T 包含一个16KB的ICache，它有512个line，每个line大小32字节（8个Word）。

ICache中的指令可以被锁住，以避免被linefill覆盖掉。锁指令这种操作的粒度是Cache的1/64，也就是64个字（256字节）。

所有指令的访问都应由MMU进行权限和转换的检查。由MMU放弃的指令读取不会导致linefill或指令读取出现在AMBA ASB总线接口上。

---
> 为了明确，在下面的文章中，I位（CP15寄存器1的位12）被称为ICR位；来自与被访问的地址相关的MMU转换表描述符相关的C位称为CTT位。

---

<h3 id="4.2.1">4.2.1 ICache组织结构</h3>

参考下面的图，了解ICache的基本结构：

![16KBICache](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/%E5%8F%AF%E5%AF%BB%E5%9D%8016KBICache.png)

ICache被分为8个段，每个段包含64个line，每个line包含8个字（word）。每个段中的line编号为0-63，通过编号对其进行索引。该索引独立于MVA。段编号通过MVA中的位 *\[7:5\]*进行选择。

MVA中的 *\[4:2\]*指定了line中要访问的word。对于半字操作，MVA的位\[1\]指定访问哪个半字，高16位还是低16位。对于字节操作，MVA中的位\[1:0\]决定。

每个cache line中的位\[31:8\]称为 *TAG*。当某个line被linefill操作时， *TAG*和8个字节的数据一起被存储在cache中。

对cache的遍历，就是比较MVA的位\[31:8\]和存储在cache line中的 *TAG*，如果匹配就命中，否则丢弃。正是因此，cache才会被称为 *虚拟地址寻址*。

<h3 id="4.2.2">4.2.2 使能和禁止ICache</h3>

重置时，ICache条目全部失效，ICache被禁用。对ICR位写1，使能；对ICR位写0，禁止。

当ICache被禁止时，cache的内容将被忽略，并且所有指令的获取将以独立的非顺序访问的形式出现在AMBA ASB总线接口上。ICache通常是在启用MMU的情况下使用。在这种情况下，相关MMU转换表描述符中的CTT位指示内存中的哪块区域是可缓存的。

如果cache被禁止，所有cache内容被忽略，但是不会发生变化。也就是说，cache被重新使能时，内容还是禁止之前的内容。所以，当cache中的内容和内存中的内容不一致的时候，必须在使能之前冲洗之前的数据。

**这就是为什么在裸机程序，比如说uboot中需要禁止并冲洗cache的原因**

可以使用MCR指令，同时往CP15 寄存器1的M位和ICR位写1，使能MMU和ICache。

<h3 id="4.2.3">4.2.3 ICache操作</h3>

如果ICache被禁止，每条指令的读取都将以独立的非顺序内存访问的形式出现在AMBA ASB接口上，这样操作，导致较低的总线和内存性能。进而，导致指令读取和CPU的处理速度无法匹配，不能将性能发挥到最优。因此，在复位后必须尽可能的使能ICache。

如果ICache被使能，无论相关的MMU转换表描述符中的CTT位设置与否，对ICache的遍历都会进行：

* 如果请求的指令在cache中发现，就称为cache命中。如果指令在cache中，且CTT=1，表明该内存地址是一个可缓存的区域。然后就将cache中的指令返回给ARM9TDMI CPU核。

* 如果请求的指令在cache中没有，则称为 *cache未命中*。如果发生cache未命中且CTT=1，然后执行一个8个字的linefill，可能替换另一个entry。要替换的entry是从没锁定的条目中选择的，使用的算法是随机或者round-robin替代策略。如果CTT=0，表明是一个非缓存区域，就会以单独的非顺序内存访问形式出现在AMBA ASB接口上。

---

> 如果CTT等于0，表明是一个不可cache的内存区域，然后cache遍历就会导致 *cache未命中*。 cache命中的唯一方法就是，软件改变了MMU转换表描述符中的CTT位，并且没有使Cache中的数据失效。这是一个可编程的错误。而且

---

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="4.3">4.3 DCache&写缓冲区</h2>

<h3 id="4.3.2">4.3.2 DCache&写缓冲区的操作</h3>


<h3 id="4.3.3">4.3.3 DCache组织结构</h3>

DCache的结构模型和ICache相同。可以参考[ICache组织结构](#4.2.1)。


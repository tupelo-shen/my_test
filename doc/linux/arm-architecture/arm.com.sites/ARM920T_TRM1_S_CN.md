<h1 id="0">0 目录</h1>
* [1 引言](#1)
    - [1.1 关于ARM920T](#1.1)
    - [1.2 功能框图](#1.2)
* [2 编程者模型](#2)
    - [2.1 关于编程者模型](#2.1)
    - [2.2 关于ARM9TDMI编程者模型](#2.2)
    - [2.3 CP15寄存器映射](#2.3)
* [3 内存管理单元-MMU](#3)
    - [3.1 关于MMU](#3.1)
    - [3.2 MMU可编程寄存器](#3.2)
    - [3.3 地址变换](#3.3)
    - [3.9 MMU和Cache的交互](#3.9)
* [4 Cache、写缓冲区、和PA TAG RAM](#4)
    - [4.1 关于Cache和write buffer](#4.1)
    - [4.2 ICache](#4.2)
    - [4.3 DCache & 写缓存](#4.3)


---

<h1 id="1">1 引言</h1>

<h2 id="1.1">1.1 关于ARM920T</h2>

ARM920T处理器是通用目的微处理器ARM9TDMI家族的一员，它包含：

1. ARM9TDMI（核）
2. ARM940T（内核+Cache+保护单元）
3. ARM920T（内核+Cache+MMU）

ARM9TDMI处理器核是基于5级流水线的哈弗结构，包括取指、解码、执行、内存和写这5个步骤。它可以作为一个单独的核嵌入到其它更为复杂的系统中。独立的核提供一个总线接口，允许用户设计自己的cache和内存系统。

ARM9TDMI处理器家族支持32位ARM和16位Thumb指令集，允许用户在高性能和高代码密度两方面作平衡。

ARM920T处理器是一种哈佛缓存架构处理器，面向多任务应用，支持内存管理，高性能，低功耗。ARM920T处理器实现了一个增强ARM架构v4 MMU，提供指令和数据地址的转译以及访问权限的检查。

ARM920T处理器支持ARM debug架构，增加了辅助硬件和软件debug的逻辑。ARM920T处理器也支持协处理器，并 输出指令和数据总线，该总线带有简单的握手信号。

对于系统其余部分，ARM920T使用统一的地址和数据总线作为通信接口。该接口可实现高级微控制器总线架构（AMBA）高级系统总线（ASB）或高级高性能总线（AHB）方案，AHB既可以作为完全兼容的AMBA总线主机，也可以作为产品测试时的从机。ARM920T处理器还具有跟踪ICE模式，其允许采用类似与传统ICE操作模式的方法。

ARM920T处理器支持添加嵌入式跟踪宏单元（ETM），用于实时跟踪指令和数据。

<h2 id="1.2">1.2 功能框图</h2>


图1-1展示了ARM920T处理器的功能框图：

![ARM920T功能框图](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/%E5%9B%BE1-1ARM920T%E5%8A%9F%E8%83%BD%E6%A1%86%E5%9B%BE.PNG)

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

<h3 id="2.3.1">2.3.1 ARM920T地址</h3>

ARM920T系统中具有3种不同类型的地址：

* 虚拟地址（VA）
* 修正后的虚拟地址（MVA）
* 物理地址（PA）

下面是一个ARM9TDMI核请求指令时发生的地址操作的示例（参见图2-10）。

1. 指令的虚拟地址VA（IVA）由ARM9TDMI核发起。
2. 根据ProcID翻译成指令MVA（IMVA）。指令缓存（ICache）和MMU看到的是IMVA。
3. 如果IMMU在IMVA之上执行的保护检查没有中止，并且IMVA tag也在ICache中，则指令数据就会返回到ARM9TDMI核中。
4. 如果ICache未命中（IMVA 的Tag在ICache中不存在），IMMU执行转换，将其转换为指令物理地址（IPA）。这个地址会被发送给 *AMBA*总线执行外部访问。

表2-3 ARM920T中的地址类型

| Domain   | ARM9TDMI | Caches and TLBs | AMBA bus |
|----------| ------------- |------------|------------|
| 地址     | 虚拟地址（VA） | 修正后虚拟地址（MVA） | 物理地址（PA） |

<h3 id="2.3.2">2.3.2 访问CP15的寄存器</h3>

下表2-4是本段内容中使用的术语或缩略语

表2-4 CP15缩略语

| 术语 | 缩略语 | 描述 |
| ---------- | ------------- | ------------ |
| 不可预知 | UNP | 对于读，读取的值可能是任何值；对于写，写入的值可能导致不可预知的行为 |
| 应该是0 | SBZ | 所有位应为0 |

需要注意的是，对CP15读写任何数据，都不会对其造成永久的损害。

除了寄存器1的V位之外，所有CP15的寄存器包含状态位的都由BnRES设置为0，而V位取值为VINITHI。

用户只能在特权模式下使用MRC和MCR指令访问CP15寄存器。这些指令的汇编格式如下所示

    MCR/MRC{cond} P15,opcode_1,Rd,CRn,CRm,opcode_2

* cond 指令运行时的条件码，当cond忽略时，指令无条件运行。
* opcode_1 协处理器将运行的操作的操作码。对于CP15协处理器，opcode_1永远为0，不为零时的操作不可预知。
* Rd 作为目的寄存器的ARM寄存器。其值与对应的协处理器中的值进行交换。Rd不能为PC寄存器，如果为PC，指令操作结果不可预知。
* CRn 指定要访问的协处理器。
* CRm字段和 opcode_2字段指定寻址寄存器时的特定操作。通过L位区分MRC（L=1）和MCR（L=0）。如果不是特定操作，请将CRm指定为C0，opcode_2指定为0。

<h3 id="2.3.3">2.3.3 寄存器 0- ID寄存器</h3>

只读存储器，返回32位的设备ID码。

可以通过设置opcode_2为除了1之外的任何值读取CP15寄存器0（当读取时，CRm字段的值应为0），获取设备ID。例如，

    MRC p15,0,Rd,c0,c0,0 ; #返回设备ID

设备ID的具体内容参考下面表2-5，

表2-5 寄存器0 - 设备ID

| 寄存器位 | 功能 | 值（16进制） |
| ---------- | ------------- | ------------ |
| 31:24 | 实施者 | 0x41 |
| 23:20 | 规范版本 | 0x1 |
| 19:16 | 架构（ARMv4T） | 0x2 |
| 15:4 | Part number | 0x920 |
| 3:0 | Layout revision | Revision |

<h3 id="2.3.4">2.3.4 寄存器 0 - cache类型寄存器</h3>

这是一个只读寄存器，包含cache的大小和架构，允许操作系统建立如何执行缓存清理和锁定等操作。所有的ARMv4T和后面带有缓存的处理器都包含这个寄存器，允许RTOS厂商实现面向未来的操作系统版本。

通过设置opcode_2为1，就可以读取CP15寄存器0-cache类型寄存器。例如：

    MRC p15,0,Rd,c0,c0,1 ; # 返回Cache的细节

寄存器的各个位表示如下所示：

* ctype \[28:25\] - Cache类型
* S位 \[24\] - 指定Cache是一个统一的Canche，还是指令和数据Cache是分开的。
* Dsize \[23:12\] - 指定数据Cache的大小，line长度和关联性
* Isize \[11:0\] - 指定指令Cache的大小，line长度和关联性
* 其余位为 0

Dsize和Isize具有相同的格式，具体如下：

* size - 和M位一起决定Cache的大小
* assoc - 和M位一起决定Cache的相关性
* M位 - 乘法位
* len - Cache line的长度

表2-6 Cache type寄存器格式

| 功能 | 寄存器位 | 值 |
| ---------- | ------------- | ------------ |
| 保留 | 31:29 | 0b000 |
| Cache类型 | 28:25 | 0b0110 |
| S位 | 24 | 0b1 = 哈佛架构 |
| Dsize（保留） | 23：21 | 0b000 |
| Dsize（size） | 20:18 | 0b101 = 16KB |
| Dsize（assoc） | 17:15 | 0b110 = 64-way |
| Dsize（M位） | 14 | 0b0 |
| Dsize（len） | 13:12 | 0b10 = 每个line具有8个word（32个字节） |
| Isize（保留） | 11:9 | 0b000 |
| Isize（size） | 8:6 | 0b101 = 16KB |
| Isize（assoc） | 5:3 | 0b110 = 64-way |
| Isize（M位） | 2 | 0b0 |
| Isize（len） | 1:0 | 0b10 = 每个line具有8个word（32个字节） |

位\[28:25\]表示Cache的主要类型。在这儿，0x6表示缓存提供：

* Cache-clean-step操作
* Cache-flush-step操作
* lockdown工具

Cache的大小由M位和size字段决定。M位表明是数据和指令Cache。

表2-7 Cache大小编码（M=0）

| size字段 | Cache大小 |
| ----- | ------------- |
| 0b000 | 512B |
| 0b001 | 1KB |
| 0b010 | 2KB |
| 0b011 | 4KB |
| 0b100 | 8KB |
| 0b101 | 16KB |
| 0b110 | 32KB |
| 0b111 | 64KB |

<h3 id="2.3.11">2.3.11 寄存器7 - Cache操作寄存器</h3>

寄存器7是一个只写寄存器，用来管理ICache和DCache。具体的功能参考下表2-15：

| 指令 | 数据（Rd） | 功能 |
| ------------- | ------------- | ------------- |
| MCR p15,0,Rd,c7,c7,0  | 0 | 失效ICache和DCache |
| MCR p15,0,Rd,c7,c5,0  | 0 | 失效ICache |
| MCR p15,0,Rd,c7,c5,1  | MVA虚拟地址 | 失效ICache中的一项 |
| MCR p15,0,Rd,c7,c13,1 | MVA虚拟地址 | 预取ICache的line |
| MCR p15,0,Rd,c7,c6,0  | 0 | 失效DCache |
| MCR p15,0,Rd,c7,c6,1  | MVA虚拟地址 | 失效DCache中的一项 |
| MCR p15,0,Rd,c7,c10,1 | MVA虚拟地址 | 清空DCache的一项 |
| MCR p15,0,Rd,c7,c14,1 | MVA虚拟地址 | 清空和失效DCache的一项 |
| MCR p15,0,Rd,c7,c10,2 | 索引 | 清空DCache的一项 |
| MCR p15,0,Rd,c7,c14,2 | 索引 | 清空和失效DCache的一项 |
| MCR p15,0,Rd,c7,c10,4 | 0 | 耗尽write buffer |
| MCR p15,0,Rd,c7,c0,4  | 0 | 等待中断发生 |

<h3 id="2.3.12">2.3.12 寄存器8 - TLB操作寄存器</h3>

寄存器8是一个只写寄存器，常常用来管理转译后备缓冲区（TLB），指令TLB和数据TLB。

通过设置opcode_2和CRm字段的值，来定义TLB的操作，并选择其功能。

表2-17 展示了使用寄存器8可以实施的TLB操作

| 功能 | 数据 | 指令 |
| ---------- | ------------- | ------------- |
| 无效TLB                 | SBZ       | MCR p15,0,Rd,c8,c7,0 |
| 无效I TLB               | SBZ       | MCR p15,0,Rd,c8,c5,0 |
| 无效I TLB（单项，使用MVA） | MVA格式   | MCR p15,0,Rd,c8,c5,1 |
| 无效D TLB               | SBZ       | MCR p15,0,Rd,c8,c6,0 |
| 无效D TLB（单项，使用MVA） | MVA格式   | MCR p15,0,Rd,c8,c6,1 |



<h3 id="2.3.16">2.3.16 寄存器13 - FCSE PID寄存器</h3>

寄存器13的英文全称是 *Fast Context Switch Extension(FCSE) Process Identifier(PID)* 寄存器。复位时，其值为0x0。

读取该寄存器，返回的就是FCSE PID的值。写入该寄存器时，只修改位[31:25]。位[24:0]应该为0。具体的位定义如下：

![图2-9 寄存器 13](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure2-9.PNG)

可以使用下面的指令访问寄存器13：

    MRC p15, 0, Rd, c13, c0, 0 ;read FCSE PID
    MCR p15, 0, Rd, c13, c0, 0 ;write FCSE PID

# 使用FCSE PID

ARM9TDMI发出的范围在0~32MB的地址，由CP15的寄存器13-FCSE PID进行转译。Cache和MMU看到的地址就是地址A+（FCSE_PID x 32MB)的地址。具体的可以参考[处理器功能框图](#1.2)。

![图2-10 使用寄存器 13的地址映射图](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure2-10.PNG)



---
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="3">3 内存管理单元-MMU</h1>

<h2 id="3.1">3.1 关于MMU</h2>

ARM920T处理器提供了一个增强型的ARMv4 MMU，用来对指令和数据进行转换以及访问权限的检查。MMU由存储在主内存中的一组2级页表控制，由CP15寄存器1中的M位进行使能，提供了一种简单的地址转换和保护机制。MMU中的指令和数据转换表可以独立地进行加锁和刷新操作。

主要功能：

1. 标准ARMv4 MMU映射大小，域和访问保护机制
2. 映射大小：段（1MB）、大页面（64KB）、小页面（4KB）和微小页面（1KB）
3. 提供对段访问的权限
4. 大页面和小页面的访问权限可以分别为其子页面进行指定（这些子页面就是每个page的1/4）
5. 硬件实现的16种域，用于控制访问权相是否有效
6. 64项指令TLB和64项数据TLB
7. 硬件页表遍历
8. round-robin 替代算法(也称为cyclic算法)
9. 使用CP15 寄存器8可以使整个TLB失效
10. 使用CP15的寄存器8，通过MVA，可以有选择的失效某一个TLB项
11. 使用CP15寄存器10独立锁定指令TLB和数据TLB

<h3 id="3.1.1">3.1.1 访问权限和域</h3>

对于大页面和小页面，可以为每一个子页面定义访问权限（对于小页面来说就是1KB，对于大页面来说就是16KB）。段和微页具有单独一组访问权限。

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

如果TLB未命中（也就是不包含VA中的项），则硬件就会遍历物理内存中的转译表检索转译信息。当检索到时，这个转译信息就会被写入TLB中，可能会覆盖掉已经存在的值。

当MMU被关闭，比如上电复位时，没有地址映射发生且所有的区域被标记为不可缓存的。具体的内容可以参考[《cache和write buffer》](#4.3)。

<h2 id="3.2">3.2 MMU编程可访问的寄存器</h2>

表3-1列出了对MMU操作有影响的CP15寄存器

| 寄存器 | 编号 | 位 | 寄存器描述 |
| --------------- | ------------- | ------------- | ------------- |
| 控制寄存器       | 1      | M、A、S、R | 使能MMU（M）；使能数据地址排列检查（A位）；控制访问保护机制（S位和R位） |
| 转译表基本寄存器 | 2      | 31:14 | 转译表的基地址对应的内存中维护的物理地址。该基地址必须位于16KB的边界上并且对于2个TLB都是通用的 |
| Domain访问      | 3      | 31:0 | ------------- |
| 错误状态寄存器   | 5（I/D） | 7:0 | ------------- |
| 错误地址寄存器   | 6（D）   | 31:0 | ------------- |
| TLB操作寄存器   | 8       | 31:0 | ------------- |
| TLB锁定寄存器   | 10（I/D  | 31:20和0 | ------------- |

<h2 id="3.3">3.3 地址转译</h2>

MMU负责把虚拟地址转换成物理地址，用于访问外部的存储设备，这些虚拟地址是由CPU核或CP15寄存器13产生的。还可以使用TLB派生和检查访问权限。

MMU页表遍历硬件被用来向TLB中追加项目。转译信息包含地址转移数据和访问权限数据，它们位于物理内存的一个转译表中。MMU提供了遍历转译表并将其加载到TLB中的逻辑单元。

这个转换的过程，包括1~2级的硬件转译表遍历，权限检查以及处理。具体几级取决于地址被标记为段映射访问还是页映射访问。

对于页映射访问，有3种大小：

* 大页
* 小页
* 微页

而对于段映射访问，只有1种大小。

<h3 id="3.3.1">3.3.1 转译表基地址</h3>

当TLB中不包含所请求的MVA的转译项时，就会启动硬件转译过程。转译表基地址（TTB）寄存器指向物理内存的基地址，其包含段或页的描述符。如图3-1所示， TTB寄存器的低14位都被设置为0，页表必须以16KB为边界。TTB寄存器的格式为：

![图3-1 TTB寄存器格式](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure3-1.PNG)

如图3-2所示，转译表包含多达4096个32位的项，每一项都描述了1MB的虚拟地址。这就允许虚拟地址的寻址范围达到4GB。


![图3-2 页表转译过程](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure3-2.PNG)


<h3 id="3.3.2">3.3.2 一级提取</h3>

图3-3 访问转译表的1级描述符

![图3-3 访问转译表的1级描述符](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure3-3.PNG)

如图3-3所示，TTB寄存器的位\[31:14\]连接上MVA的位\[31:20\]组成了一个30位的地址。

这个地址的每一项是4字节的内容。这既是段也是页表的一级描述符。

<h3 id="3.3.3">3.3.3 一级描述符</h3>

![图3-4 一级描述符](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure3-4.PNG)

一级描述符可以是段描述符，粗页表描述符，细页表描述符或者是非法的。参考图3-4。

由图可以看出，端描述符提供1MB内存块的基地址。

页表描述符时分为2种：粗页表描述符和细页表描述符：

* 粗页表把1MB的空间分成了256项，每一项4KB大小；
* 细页表把1MB的空间分成了1024项，每一项1KB大小。

表3-2 一级描述符各位的意义

| 段 | 粗页表 | 细页表 | 描述 |
| --------------- | ------------- | ------------- | ------------- |
| 31:20 | 31:10 | 31:19 | 这些位都是直接跟物理地址相关的 |
| 19:12 | - | - | 应该为0 |
| 11:10 | - | - | 访问权限位。Domain访问控制和错误检查序列中已经解释了这些访问权限的解释 |
| 9  | 9 | 11:9 | 应该是0 |
| 8:5 | 8:5 | 8:5 | Domain控制位 |
| 4 | 4 | 4 | 必须为1 |
| 3:2 | - | - | 这些位，C和B表示时回写cache、直写cache、非cache的buffer和非cache非buffer |
| - | 3:2 | 3:2 | 应该为0 |
| 1:0 | 1:0 | 1:0 | 用来解释页的大小和合法性，如表3-3中所解释的 |

<h3 id="3.3.4">3.3.4 段描述符</h3>

![图3-5 段描述符地址格式](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure3-5.PNG)

解释：

表3-4 段描述符各位

| 位 | 描述 |
| --------------- | ------------- |
| 31:20 | 物理地址相关位 |
| 19:12 | 总是0 |
| 11:10 | 指明该段的访问权限 |
| 9 | 总是0 |
| 8:5 | 指明16个域中的一个 |
| 4 | 应该是1，向后兼容 |
| 3:2 | 这些位，C和B表示时回写cache、直写cache、非cache的buffer和非cache非buffer |
| 1:0 | 必须是0b10 |


<h3 id="3.3.7">3.3.7 转译段地址的过程</h3>

![图3-6 转译段地址的过程](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure3-6.PNG)

> 注意：
> 必须在产生物理地址之前，检查一级描述符的访问权限

<h3 id="3.3.8">3.3.8 二级描述符</h3>

二级描述符的格式如下图3-9所示：

![图3-9 二级描述符](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/Figure3-9.PNG)

二级描述符描述了微页、小页、大页或非法地址：

* 大页描述符提供了64KB内存块的基地址
* 小页描述符提供了4KB内存块的基地址
* 微页描述符提供了1KB内存块的基地址

粗页表提供的基地址既可以是小页、也可以是大页的基地址。大页描述符必须在16个连续的项中是重复的，这样才能保证64KB的内存块。小页描述符仅在每一项中使用一次即可。

细页表可以提供大页、小页和微页的基地址。大页描述符必须在连续的64项中是重复的。小页描述符在连续的4项中是重复的。微页描述符仅在每一项中使用。

二级描述符各位的赋值参考下表3-7所示：

| 大页            | 小页           | 微页          | 描述          |
| --------------- | ------------- | ------------- | ------------- |
| 31:16           | 31:12         | 31:10         | 这些位都是直接跟物理地址相关的 |
| 15:12           | -             | 9:6           | 应该为0 |
| 11:4            | 11:4          | 5:4           | 访问权限位。[域访问控制]和[错误检查序列]都阐述了如何解释这些访问权限位 |
| 3:2             | 3:2           | 3:2           | 这些位，C和B表示回写cache、直写cache、非cache的buffer和非cache非buffer |
| 1:0             | 1:0           | 1:0           | 这些位反映了页的大小和合法性，比如大页、小页、微页、不合法等 |

<h2 id="3.9">3.9 MMU和Cache的交互</h2>

可以使用CP15的控制寄存器的位0，禁止和使能MMU。

<h3 id="3.9.1">3.9.1 使能MMU</h3>

为了使能MMU：

1. 设置TTB和域访问控制寄存器
2. 按需设置1级和2级页表
3. 设置控制寄存器的位0使能MMU

可以使用一条简单简单的MCR指令同时使能ICache和DCache。

<h3 id="3.9.2">3.9.2 禁止MMU</h3>

清除控制寄存器的位0，就可以禁止MMU了。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

---

<h1 id="4">4 Cache、写缓冲区、和PA TAG RAM</h1>

<h2 id="4.1">4.1 关于Cache和write buffer</h2>

ARM920T一级内存系统包含一个ICache和一个DCache，一个write buffer，和一个物理地址TAG RAM，这些都是为了减少内存读写带宽和延时对性能的影响。

ARM920T实现了一个独立的16KB的ICache和一个独立的16KB的DCache。

这2个Cache具有下面的特色：

1. 虚拟寻址的64路关联Cache
2. 每个line具有8个word（32个字节），带有一个合法位和2个脏位，允许半line回写
3. 直写和回写Cache操作（回写也称拷贝备份Cache），通过MMU转译表中的B位和C位进行选择
4. 伪随机或round-robin替代算法，具体的算法可以使用CP15的寄存器1的RR位进行选择
5. 低功耗的CAM-RAM实现
6. Cache可以被锁定的最小粒度为Cache的1/64分之一，也就是64个字（256个字节）
7. 为了避免在回写数据期间TLB未命中，且为了减少中断等待时间，除了存储在cache CAM中的VA TAG之外，还在PA TAG RAM中存储每个数据cache项对应的物理地址，以在进行cache line 回写时使用。这意味着MMU不参与cache回写操作， 消除了与回写地址相关的TLB未命中的可能性
8. 为了提供整个数据cache清理工作和小块虚拟内存的清理和失效工作的高效率，进行Cache的维护。后者允许在发生小块代码更改时有效地维护ICache的一致性，比如，自修正代码和对异常向量的更改

而write buffer具有下面的功能：

1. 有一个16字的数据buffer
2. 有一个4地址的地址buffer
3. 可以通过软件控制清理buffer，使用CP15的MCR指令（参见[Drain write buffer](#4.8))



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

ARM920T包含一个16KB的DCache和一个write buffer，用以提高数据的访问性能。DCache拥有512个line，每个line拥有32字节（8个字），排列为64路关联缓存组，使用经过CP15的寄存器13转换后的MVA，其来源是ARM9TDMI CPU核请求的地址。


<h3 id="4.3.2">4.3.2 DCache&写缓冲区的操作</h3>


<h3 id="4.3.3">4.3.3 DCache组织结构</h3>

DCache的结构模型和ICache相同。可以参考[ICache组织结构](#4.2.1)。

<h2 id="4.8">4.8 Drain write buffer</h2>

用户可以通过软件控制，在write buffer耗尽之前不再执行后续的指令。具体的方法见下面：

1. 存储不可缓存的内存
2. 从不可缓存的内存中加载内容
3. 耗尽write buffer的MCR指令：

        MCR p15,0,Rd,c7,c10,4

在执行不太可控的活动之前，write buffer也会耗尽，你必须将其视为实现定义的：

* 从不可缓存的内存中提取内容
* DCache linefill
* ICache linefill



---
<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
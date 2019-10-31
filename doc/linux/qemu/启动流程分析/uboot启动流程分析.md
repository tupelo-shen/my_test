<h1 id="0">0 目录</h1>

* [1 简介](#1)
    - [1.1 uboot是什么？](#1.1)
    - [1.2 存储器](#1.2)
* [2 uboot链接文件](#2)
    - [2.1 u-boot.lds文件分析](#2.1)
    - [2.2 System.map文件](#2.2)
    - [2.3 链接文件语法](#2.3)
* [3 uboot启动流程第1阶段](#3)
    - [3.1 start.S文件分析](#3.1)
    - [3.2 lowlevel_init.S文件分析](#3.2)
* [4 uboot启动流程第2阶段](#4)
    - [4.1 初始化](#4.1)
    - [4.2 加载内核](#4.2)
* [A 参考资料](#A)
    - [A.1 arm公司官方提供的ARMv7-A体系结构文档](#A.1)
    - [A.2 Uboot中start.S源码的指令级的详尽解析_v1.6.pdf](#A.2)
    - [A.3 ARM指令集快速查询手册.pdf](#A.3)
    - [A.4 ARM指令详解[ARM标准].pdf](#A.4)
    - [A.5 程序员的自我修养—链接、装载与库.pdf](#A.5)

---

<h1 id="1">1 基本概念</h1>

<h2 id="1.1">1.1 uboot是什么？</h2>

当我们厌倦了裸机程序，而想要采用操作系统的时候，`uboot`就是不得不引入的一段程序。所以，`uboot`就是一段引导程序，在加载系统内核之前，完成硬件初始化，内存映射，为后续内核的引导提供一个良好的环境。`uboot`是`bootloader`的一种，全称为`universal boot loader`。

<h2 id="1.2">1.2 存储器</h2>

在了解Uboot之前，我们先来了解一下代码的载体，看看代码到底存储在什么地方！

1. norflash/nandflash、SRAM和SDRAM作为存储器有何异同？

    1. norflash - 是非易失性存储器（也就是掉电保存）

        NOR flash带有SRAM接口，有足够的的地址引脚进行寻址，可以很容易地读取其内部的每一个字节（
        注意是 *Read* ！因为flash不是任意写入，而是遵循 *disable write protect -> erase -> write* 。这是flash的特性决定的，
        其电路只能从 *1->0*，而不能 *0->1*翻转。擦除过程就是将flash中的某一个扇区恢复为 `0xFFFFFFFF`，然后再写入数据。另外，代码
        指令可以直接在norflash上运行。

        （**重要！！！上电后可以读取norflash中的数据但是不可以进行写操作**）

    2. nandflash - 是非易失性存储器（也就是掉电保存）

        它也是非易失闪存（掉电不丢失）的一种，但是它虽然有数据总线，但是没有地址总线，所以cpu不能直接从nandflash中取指运行，由于它价格便宜，所以常常用来存储大量数据，和我们常说的硬盘类似。


    3. SRAM - 静态随机访问存储器 - Static Random Access Memory

        *static* 是指只要不掉电，存储在SRAM中的数据就不会丢失。这一点与DRAM不同，DRAM需要进行周期性刷新操作。然而，我们不应将
        SRAM和只读存储器（ROM）、Flash Memory相混淆，因为SRAM是一种易失性存储器，它只有在电源保持连续供应的情况下才能够保持数据。
        *Random Access* 指的是存储器的内容可以任意顺序进行访问，而不管前一次访问的是哪一个位置。

        （**重要！！！上电后就可以读写SRAM中的数据，而无需初始化操作**）

    4. SDRAM - 同步动态随机存取存储器 - Synchronous Dynamic Random Access Memory

        需要不断的刷新，才能保存数据。而且是行列地址复用，许多都有页模式。

        （**重要！！！需要对DDR控制器进行初始化<配置寄存器>，才能去读写SDRAM中的数据**）

2. 存储器在ARM处理器上的作用。

    1. NOR Flash - 程序存储器

        作为ARM处理器的程序存储器。因为我们必须将程序保存在一个掉电后还能保存数据的地方。上电后，NOR Flash就相当于一个随机读取的只读存储器。

        那么，既然NOR Flash可以运行程序，我们为什么不直接在其上运行程序呢？

        一般情况下，我们编译的程序中，*.text*段，*.rodata*段都是只读的，这没有问题。但是，*.data*段（数据段）和 *.bss*（未初始化的全局变量和静态变量）在程序的运行过程中变量的值是需要改变的（改变一个变量的值，需要改变对应物理地址上
        存储的内容），很可惜，NOR Flash只能直接读取，而无法进行写操作。

        那么，为了解决这个问题，就需要SRAM了。

    2. SRAM - 提供运行环境

        其重要特性就是：容量小，512KB；运行程序速度快；价格贵。

        可以作为程序运行时的堆栈空间。把ARM处理器的堆栈指针sp设置在SRAM中，为程序提供运行环境。

        SRAM作为运行程序的场所是可以的，但是由于其昂贵的价格，极小的容量，而现在程序一般都非常庞大，所以满足不了我们的需求。正因为此，SDRAM应运而生。

    3. SDRAM - 运行程序

        它的重要特性是容量大：512M；程序运行速度快；价格便宜。

        但是其掉电易失数据，上电后必须初始化DDR控制器，否则无法进行数据的读写。所以在运行系统内核之前必须对其进行初始化，这就是在NOR Flash 和 SRAM上搭建的程序的作用。

        那它主要都完成什么工作呢？

            1. 完成对处理器时钟的初始化
            2. DDR的初始化
            3. 给 gd_t *gd_ptr 赋值(用于存储uboot镜像的长度，重定位地址，重定位偏移量等信息)

        在uboot搬运到DDR中运行前进行最小系统的初始化，之后就将uboot搬运到DDR中运行。那么，此时NOR Flash和SRAM的任务就完成了（也就是没有用了）。

总结：

从norflash启动可以省事多了，不仅如此，我们自己编写的裸机程序需要调试，一般也是直接烧写到norflash中进行的，因为只要我们将编译好的可执行文件放到norflash的开始，开发板上电以后就会从norflash的第一条指令开始取指执行，我们后面写裸机程序的调试就是用这种方式进行的。
从norflash启动虽然从开发的角度会很方便（其实也方便不了多少），但是从产品的角度却增加了它的成本，毕竟norflash还是相对较贵的，我们明明只要一块nandflash就足够启动整个开发板了，就没必要在产品中添加一块norflash了，只要代码改改就能省下不少成本，何乐不为。而且nandflash对产品是必不可少的，因为后面还要存放内核和文件系统，起码需要几十兆的空间，用norflash来存储也不现实。

也许你会想，能不能只用norflash，不用nandflash和SDRAM行不行呢，毕竟norflash即可以存储，也可以运行程序的啊，从理论来说是可以的，但是了解一下他们的市场价格、运行速度和工作原理，应该就会知道答案了。

<h1 id="2">2 uboot链接文件</h1>

前面我们已经了解了嵌入式设备的存储介质，以及它们的用途。但是，对于编译后的代码如何在内存上分配，处理器是不知道，这需要我们人为地指定。这时候，后缀为lds的链接文件出现了，它规定了编译后的代码各个段是如何分配的。理解了它，对于我们对u-boot也是大有帮助的。下面我们直接看代码内容，如果需要了解语法，请参考[2.3 链接文件语法](#2.3)。

<h2 id="2.1">2.1 u-boot.lds文件分析</h2>

文件位置：*u-boot/board/mini2440/u-boot.lds*。

    OUTPUT_FORMAT("elf32-littlearm","elf32-littlearm","elf32-littlearm")/*指定输出可执行文件是elf格式,32位ARM指令,小端*/
    OUTPUT_ARCH(arm)                                                    /*指定输出可执行文件的platform为ARM*/
    ENTRY(_start)                                                       /*指定输出可执行文件的起始地址为_start*/
    SECTIONS
    {
        /* 指定可执行文件（image）的全局入口点，通常这个地址都放在ROM(flash)0x0位置。*/
        /* 必须使编译器知道这个地址，通常都是修改此处来完成 */
        . = 0x00000000;                                 /* 从0x0位置开始 */

        . = ALIGN(4);                                   /* 代码以4字节对齐 */
        .text :
        {
            cpu/arm920t/start.o (.text)                 /* 从start.S文件中的代码开始 */
            cpu/arm920t/s3c24x0/nand_read.o (.text)     /* 关于nand读取的代码 */
            *(.text)                                    /* 其它代码部分*/
        }

        . = ALIGN(4);
        .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) } /*指定只读数据段*/

        . = ALIGN(4);
        .data : { *(.data) }                            /* 指定读/写数据段，RW段 */

        . = ALIGN(4);
        .got : { *(.got) }                              /*指定got段, got段是uboot自定义的一个段, 非标准段*/

        . = .;
        __u_boot_cmd_start = .;                         /*把__u_boot_cmd_start赋值为当前位置, 即起始位置*/
        .u_boot_cmd : { *(.u_boot_cmd) }                /*指定u_boot_cmd段, uboot把所有的uboot命令放在该段.*/
        __u_boot_cmd_end = .;                           /*把__u_boot_cmd_end赋值为当前位置,即结束位置*/

        . = ALIGN(4);
        __bss_start = .;                                /*把__bss_start赋值为当前位置,即bss段的开始位置*/
        .bss (NOLOAD) : { *(.bss) . = ALIGN(4); }       /*指定bss段,告诉加载器不要加载这个段*/
        __bss_end = .;                                  /*把_end赋值为当前位置,即bss段的结束位置*/
    }

通过上面的分析可以看出

1. 由于在链接脚本中规定了文件start.o(对应于start.S)作为整个uboot的起始点，因此启动uboot时会执行首先执行start.S。
2. 一般来说，内存空间可分为代码段、数据段、全局变量段、未初始化变量区、栈区、堆区等.其中，栈区由指针SP决定，堆区实质上是由C代码实现的，其它段则由编译器决定.从上面的分析可以看出，从0x00000000地址开始，编译器首先将代码段放在最开始的位置，然后是数据段，然后是bss段(未初始化变量区).


<h2 id="2.2">2.2 System.map文件</h2>

我们来看一下 *System.map* 这个文件：

    33f80000 T _start
    33f80020 t _undefined_instruction
    ......(中间省略)
    33f80fd8 T start_armboot

如何设置从0x33f80000开始呢?~这是链接的时候指定的。在u-boot根目录下面的config.mk中有下面一句

    LDFLAGS += -Bstatic -T $(LDSCRIPT) -Ttext $(TEXT_BASE) $(PLATFORM_LDFLAGS)

其中的-Ttext $(TEXT_BASE),这句指明了代码段的起始地址。而 *TEXT_BASE*在 *board/mini2440/config.mk* 中定义 *TEXT_BASE = 0x33F80000*

### 为什么是0x33F80000呢?~

我们先来看一下S3C2440的内存分布图：

![uboot_pic_1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/%E5%90%AF%E5%8A%A8%E6%B5%81%E7%A8%8B%E5%88%86%E6%9E%90/images/uboot_pic_1.PNG)

如上图所示，如果选择NAND flash为引导ROM时，为了支持NAND Flash的BootLoader，S3C2440A配备了一个内置的SRAM 缓冲器，叫做“Steppingstone”， 具体如下图所示。引导启动时，NAND Flash 存储器的开始4K 字节将被自动加载到Steppingstone 中并且执行自动加载到Steppingstone的引导代码。

![uboot_pic_2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/%E5%90%AF%E5%8A%A8%E6%B5%81%E7%A8%8B%E5%88%86%E6%9E%90/images/uboot_pic_2.PNG)

所以说，虽然我们在链接文件u-boot.lds中指定了起始地址是0x00000000，但是在config.mk的编译构造文件中还指定了起始地址是0x33f80000。因为我们的u-boot代码在上面的4k代码执行完成后，还是要跳转到0x33f80000处执行的。所以，链接文件中的默认0x00000000起始地址不需要起作用。

查看S3C2440的datasheet，如下图所示，

![uboot_pic_3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/%E5%90%AF%E5%8A%A8%E6%B5%81%E7%A8%8B%E5%88%86%E6%9E%90/images/uboot_pic_3.PNG)

可以看出64M空间对应的地址为0x30000000~0x33FFFFFF。TEXT_BASE=0x33F80000 即为程序加载起始地址，可以使用的空间大小即为 0x33F80000 到 0x33FFFFFF 共 512K ，如果你u-boot包含的功能太多，觉得不够用，你可以把 0x33F80000调小一点， 即和往低地址移一些，移的过程中注意内存页对齐就行了，一般是4KB.

<h2 id="2.3">2.3 链接文件语法</h2>

下面是一些比较难懂的lds文件的语法，先记录如下。对于常用语法可以自行查阅，网上资料还是蛮多的。

### 2.3.1 .got 语法

在了解这个语法之前，我们先来了解一些其他概念：

1. 加载时重定位

    基本思路是：在链接时，对所有绝对地址的引用不作重定位，而把这一步推迟到装载时再完成。一旦模块装载地址确定，即目标地址确定，那么系统就对程序中所有的绝对地址引用进行重定位。

    设函数foobar相对于代码段的起始地址是0x100,当模块被装载到0x10000000时，我们假设代码段位于模块的最开始，即代码段的装载地址也是0x10000000,那么我们就可以确定foobar的地址为0x10000100。这时候，系统遍历模块中的重定位表，把所有对foobar的地址引用都重定位至0x10000100。

2. 地址无关代码（position-independent-code，PIC），又称为地址无关可执行文件（position-independent-executable，PIE）

    加载时重定位解决了动态模块中有绝对地址引用的问题，但是又带来了指令部分无法在多个进程间共享的问题。

    具体想法就是把程序模块中共享的指令部分在加载时不需要因为加载地址的改变而改变。把指令中那些需要被修改的部分分离出来，跟数据部分放在一起，这样指令部分就可以保持不变，而数据部分可以在每个进程中拥有一个副本。这种方案就是目前的地址无关代码（PIC）技术。

    具体方法：先分析模块中各种类型的地址引用方式，把共享对象模块中地址引用按照是否跨模块分为两类：模块内部引用和模块外部引用。按照不同的引用方式又可以分为指令引用和数据访问。

        1. 第一种是模块内部的函数调用、跳转等。
        2. 第二种是模块内部的数据访问，比如模块中定义的全局变量、静态变量。
        3. 第三种是模块外部的函数调用、跳转等。
        4. 第四种是模块外部的数据访问，比如其它模块中定义的全局变量。

3. 全局偏移表（GOT）

    对于类型三，我们需要用到代码地址无关（PIC）技术，基本的思想就是把跟地址相关部分放到数据段里面。 ELF的做法是在数据段里建立一个指向这些变量的指针数组，称为全局偏移表（GOT），当代码需要引用该全局变量时，可以通过GOT中相对应的项间接引用。由于GOT本身是放在数据段的，所以它可以在模块装载时被修改，并且每个进程都可以有独立的副本，相互不受影响。

    对于跨模块的数据访问也是如此，不做细致分析了。

<h1 id="3">3 uboot启动流程第1阶段</h1>

主要脉络： *部分硬件初始化 -> 加载完整的uboot到RAM -> 跳转到第2阶段入口开始执行*

第1阶段主要用到的文件是：

* start.S文件，位于 *u-boot/cpu/arm920t/start.S*
* lowlevel_init.S文件，位于 *u-boot/board/smdk2410/lowlevel_init.S*

<h2 id="3.1">3.1 start.S文件分析</h2>

文件位置：

### 1. 启动-_start

查看下面的代码：

    _start:                                 # 异常处理向量表
    b start_code
    ldr pc, _undefined_instruction          # 未定义指令异常：0x00000004
    ldr pc, _software_interrupt             # 软中断异常:0x00000008
    ldr pc, _prefetch_abort                 # 预取异常：0x0000000C
    ldr pc, _data_abort                     # 数据异常:0x00000010
    ldr pc, _not_used                       # 未使用：0x00000014
    ldr pc, _irq                            # 外部中断请求IRQ:0x00000018
    ldr pc, _fiq                            # 快束中断请求FIQ:0x0000001C

从上面的内容可以看出，除第1行代码之外，其余代码都是跳转到特定位置去执行中断服务子程序。

正常情况下，程序的执行流程是不会走到中断处理流程中去的，而是直接跳转到reset处开始执行。那我们接下来就看reset处的代码都干了什么。

### 2. reset-设置超级管理模式

设置CPU为SVC32模式，即超级管理权限模式

    start_code:
        mrs r0,cpsr             # 将程序状态寄存器读取到通用寄存器R0
        bic r0,r0,#0x1f         # 清除当前的工作模式
        orr r0,r0,#0xd3         # 设置超级管理员权限
        msr cpsr,r0             # 将结果写回到CPSR寄存器

cpsr 是ARM体系结构中的程序状态寄存器，其结构如下：

| M[4:0] | CPU模式 | 可访问寄存器 | 说明 |
|--------|----------|---------------------| ----- |
| 0b10000 | user        | pc,R14~R0,CPSR | 正常ARM程序执行状态
| 0b10001 | FIQ         | PC,R14_FIQ-R8_FIQ,R7~R0,CPSR,SPSR_FIQ | 为支持数据传输或通道处理设计 |
| 0b10010 | IRQ         | PC,R14_IRQ-R13_IRQ,R12~R0,CPSR,SPSR_IRQ | 用于一般用途的中断处理 |
| 0b10011 | SUPERVISOR  | PC,R14_SVC-R13_SVC,R12~R0,CPSR,SPSR_SVC | 操作系统保护模式 |
| 0b10111 | ABORT       | PC,R14_ABT-R13_ABT,R12~R0,CPSR,SPSR_ABT | 数据或指令预取中止后进入 |
| 0b11011 | UNDEFINED   | PC,R14_UND-R8_UND,R12~R0,CPSR,SPSR_UND | 执行未定义指令时进入 |
| 0b11111 | SYSTEM      | PC,R14-R0,CPSR(ARM V4以及更高版本） | 操作系统的特权用户模式 |

---

> I、F、T三位如果写1即禁用，所以reset后面的4句操作的结果为设置CPU为SUPERVISOR模式且禁用中断。那为什么选择这个模式呢？
>
> 首先，可以排除的就是ABORT和UNDEFINED模式，看上去就不像正常模式。
>
> 其次，对于快速中断fiq和中断irq来说，此处uboot初始化的时候，也还没啥中断要处理和能够处理，而且即使是注册了终端服务程序后，
> 能够处理中断，那么这两种模式，也是自动切换过去的，所以，此处也不应该设置为其中任何一种模式。
>
> 于usr模式，由于此模式无法直接访问很多的硬件资源，而uboot初始化，就必须要去访问这类资源，所以此处可以排除，不能设置为用户usr模式。
>
> 而svc模式本身就属于特权模式，本身就可以访问那些受控资源，而且，比sys模式还多了些自己模式下的影子寄存器，所以，相对sys模式来说，
> 可以访问资源的能力相同，但是拥有更多的硬件资源。

### 3. 关闭看门狗

    ldr     r0, =pWTCON         # 取得看门狗寄存器的地址
    mov     r1, #0x0            # 将R1寄存器清0
    str     r1, [r0]            # 将看门狗寄存器清0，即将看门狗禁止，包括定时器定时，溢出中断及溢出复位等

### 4. 关中断

    mov r1, #0xffffffff         # 设R1寄存器为0xFFFF FFFF
    ldr r0, =INTMSK             # 读取中断屏蔽寄存器的地址
    str r1, [r0]                # 将中断屏蔽寄存器中的位全设1,屏蔽所有中断

    ldr r1, =INTSUBMSK_val      # 设R1寄存器为0xFFFF
    ldr r0, =INTSUBMSK          # 读取辅助中断屏蔽寄存器的地址
    str r1, [r0]                # 将辅助中断屏蔽寄中的11个中断信号屏蔽掉，本人觉得INTSUBMS_val应设成7ff

### 5. 设置时钟

### 6. 关闭MMU，设置ARM时序

    #ifndef CONFIG_SKIP_LOWLEVEL_INIT
    cpu_init_crit:
        // 使I/D cache失效： 协处理寄存器操作，将r0中的数据写入到协处理器p15的c7中，c7对应cp15的cache控制寄存器
        mov r0, #0
        mcr p15, 0, r0, c7, c7, 0   /* flush v3/v4 cache */
        mcr p15, 0, r0, c8, c7, 0   /* flush v4 TLB */ 使TLB操作寄存器失效：将r0数据送到cp15的c8、c7中。C8对应TLB操作寄存器

        /*
         * 禁用MMU和缓存
         */
        mrc p15, 0, r0, c1, c0, 0   // 将c1、c0的值写入到r0中
        bic r0, r0, #0x00002300 @ clear bits 13, 9:8 (--V- --RS)
        bic r0, r0, #0x00000087 @ clear bits 7, 2:0 (B--- -CAM)
        orr r0, r0, #0x00000002 @ set bit 2 (A) Align
        orr r0, r0, #0x00001000 @ set bit 12 (I) I-Cache
        mcr p15, 0, r0, c1, c0, 0   // 将设置好的r0值写入到协处理器p15的c1、c0中，关闭MMU

        /*
         * 在重加载之前，我们必须设置RAM的时序，因为内存的时序依赖于板子，
         * 在board目录下可以发现lowlevel_init.S文件
         */
        mov ip, lr                  // 将lr寄存器内容保存到ip寄存器中，用于子程序调用返回
    #if defined(CONFIG_AT91RM9200DK) || defined(CONFIG_AT91RM9200EK) || defined(CONFIG_AT91RM9200DF)
    #else
        bl  lowlevel_init           // 跳转到`lowlevel_init`地址执行
    #endif
        mov lr, ip
        mov pc, lr
    #endif /* CONFIG_SKIP_LOWLEVEL_INIT */

#### 1. Cache是什么呢？

Cache是处理器内部的一个高速缓存单元，为了应对处理器和外部存储器之间的速度不匹配而设立的。其速度要比内存的读写速度快好多，接近处理器的工作速度，一般处理器从内存中读取数据到Cache中，到下次再用到数据时，会先去cache中查找，如果cache中存在的话，就不会访问内存了，用以提高系统性能。

#### 2. 系统引导时为什么关闭Cache？

从上面的解释中，可以看出，在系统未初始化完成时，代码还没有转移到内存中，也就是说，我们还没有用到内存，先将MMU和Cache关闭，以免发生不可预料的错误。

#### 3. 怎样使Cache中的数据无效？

见上面的代码。


<h2 id="3.2">3.2 lowlevel_init.S文件分析</h2>

<h3 id="3.2.1">3.2.1 RAM初始化</h3>

这一步主要完成RAM的初始化，也就是通过写控制RAM的寄存器，对寄存器的存取方式进行控制。主要代码位于文件`lowlevel_init.S`中。

`lowlevel_init.S`文件内容如下：

    lowlevel_init:
        /* memory control configuration */
        /* make r0 relative the current location so that it */
        /* reads SMRDATA out of FLASH rather than memory ! */
        ldr r0, =SMRDATA                    // 读取下面标号为SMRDATA处的地址到R0中
        ldr r1, _TEXT_BASE                  // 程序的加载地址 TEXT_BASE = 0x33F80000 到 R1中
        sub r0, r0, r1                      // 计算SMRDATA的相对地址保存到R0中，
                                            /* SMRDATA为虚拟地址,而TEXT_BASE为虚拟地址的起始地址
                                             * TEXT_BASE为0x33F8 0000,SMRDATA为0x33F8 06C8
                                             * 而现在程序运行在起始地址为0x0000 0000的地方
                                             * 所以需要计算以0x0000 0000为标准的相对地址 */
        ldr r1, =BWSCON                     // 取得带宽与等待状态控制寄存器地址到R1中，也就是控制内存的寄存器的首地址
        add r2, r0, #13*4                   // R2保存要操作的寄存器的个数，在这儿是13
    0:
        ldr     r3, [r0], #4                // 数据处理后R0自加4，[R0]->R3，R0+4->R0
        str     r3, [r1], #4                // 将这些数据写入到控制内存的寄存器中。
        cmp     r2, r0                      // 循环从Flash中读取13个Word大小的值到内存中
        bne     0b

        mov pc, lr                          // 返回函数lowlevel_init()的调用地方

        .ltorg
    /* the literal pools origin */

    SMRDATA:
        .word (0+(B1_BWSCON<<4)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
        .word ((B0_Tacs<<13)+(B0_Tcos<<11)+(B0_Tacc<<8)+(B0_Tcoh<<6)+(B0_Tah<<4)+(B0_Tacp<<2)+(B0_PMC))
        .word ((B1_Tacs<<13)+(B1_Tcos<<11)+(B1_Tacc<<8)+(B1_Tcoh<<6)+(B1_Tah<<4)+(B1_Tacp<<2)+(B1_PMC))
        .word ((B2_Tacs<<13)+(B2_Tcos<<11)+(B2_Tacc<<8)+(B2_Tcoh<<6)+(B2_Tah<<4)+(B2_Tacp<<2)+(B2_PMC))
        .word ((B3_Tacs<<13)+(B3_Tcos<<11)+(B3_Tacc<<8)+(B3_Tcoh<<6)+(B3_Tah<<4)+(B3_Tacp<<2)+(B3_PMC))
        .word ((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC))
        .word ((B5_Tacs<<13)+(B5_Tcos<<11)+(B5_Tacc<<8)+(B5_Tcoh<<6)+(B5_Tah<<4)+(B5_Tacp<<2)+(B5_PMC))
        .word ((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))
        .word ((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))
        //设置REFRESH,在S3C2440中11～17位是保留的，也即(Tchr<<16)无意义
        .word ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT)
        .word 0x32      // 设置BANKSIZE,对于容量可以设置大写，多出来的空内存会被自动检测出来
        .word 0x30      // 设置MRSRB6
        .word 0x30      // 设置MRSRB7

<h3 id="3.2.2">3.2.2 Uboot代码加载</h3>

    #ifndef CONFIG_SKIP_RELOCATE_UBOOT
        adr r0, _start          /* r0保存当前程序的位置 */
    relocate:                   /* 将uboot代码重定位到RAM中 */
        teq r0, #0              /* 测试是否从地址0开始运行 */
        bleq    may_resume      /* yes -> do low-level setup */

        adr r0, _start          /* 上面的代码有可能会破会r0中的值 */
        ldr r1, _TEXT_BASE      /* 测试从Flash还是RAM中运行程序，它们的地址是不一样的 */
        cmp r0, r1              /* 在debug期间不需要重定位，直接在Flash中运行代码 */
        beq     done_relocate

        ldr r2, _armboot_start
        ldr r3, _bss_start
        sub r2, r3, r2          /* 根据前面分析的uboot.lds文件可知，r3-r2就是uboot代码的大小，将其存入寄存器r2中 */
        add r2, r0, r2          /* r0是程序的起始地址，加上uboot代码的大小就是uboot代码的结束地址 */

    copy_loop:
        ldmia   r0!, {r3-r10}   /* 从源地址[r0]处开始拷贝 */
        stmia   r1!, {r3-r10}   /* 拷贝到目标地址[r1]处 */
        cmp r0, r2              /* 直到源代码结束地址[r2] */
        ble copy_loop

<h3 id="3.2.3">3.2.3 建立堆栈</h3>

设置堆栈，其中，`_TEXT_BASE=0x33F80000`，而 `CFG_MALLOC_LEN`， `CFG_GBL_DATA_SIZE`， `CONFIG_STACKSIZE_IRQ`， `CONFIG_STACKSIZE_FIQ`在文件`uboot/include/configs/mini2440.h`文件中定义。

    /* 建立堆栈 */
    stack_setup:
        ldr r0, _TEXT_BASE                                          /* upper 128 KiB: relocated uboot   */
        sub r0, r0, #CFG_MALLOC_LEN                                 /* malloc area */
        sub r0, r0, #CFG_GBL_DATA_SIZE                              /* bdinfo */
    #ifdef CONFIG_USE_IRQ
        sub r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
    #endif
        sub sp, r0, #12                                             /* leave 3 words for abort-stack    */

<h3 id="3.2.4">3.2.4 清除bss段</h3>

    clear_bss:
        ldr r0, _bss_start                                          /* find start of bss segment        */
        ldr r1, _bss_end                                            /* stop here                        */
        mov     r2, #0x00000000                                     /* clear                            */

    clbss_l:str r2, [r0]                                            /* clear loop...                    */
        add r0, r0, #4
        cmp r0, r1
        ble clbss_l

<h3 id="3.2.5">3.2.5 跳转到uboot第2阶段</h3>

        ldr pc, _start_armboot

    _start_armboot: .word start_armboot

 初始化外设完成之后，程序跳转到u-boot第2阶段的入口函数 `start_armboot` 。 `ldr pc，_start_armboot` 为绝对跳转命令，pc值等于`_start_armboot`的连接地址，程序跳到SDRAM中执行。在此之前程序都是在flash中运行的，绝对跳转必须在初始SDRAM，执行代码重定位之后才能进行。

<h1 id="4">4 uboot启动流程第2阶段</h1>

第2阶段，uboot完成进一步的硬件初始化，并设置了uboot下的命令行、环境变量、并跳转到内核中。其主要用到的文件是：

* board.c文件，位于 *u-boot/lib_arm/board.c*
* main.c文件，位于 *u-boot/common/main.c*

<h2 id="4.1">4.1 初始化</h2>

    void start_armboot (void)
    {
        init_fnc_t **init_fnc_ptr;
        char *s;
    #ifndef CFG_NO_FLASH
        ulong size;
    #endif
    #if defined(CONFIG_VFD) || defined(CONFIG_LCD)
        unsigned long addr;
    #endif

        /* 在上面的代码中gd的值绑定到寄存器r8中了 */
        gd = (gd_t*)(_armboot_start - CFG_MALLOC_LEN - sizeof(gd_t));
        /* 为GCC >= 3.4以上的编译进行代码优化，而插入内存barrier */
        __asm__ __volatile__("": : :"memory");

        memset ((void*)gd, 0, sizeof (gd_t));
        gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));
        memset (gd->bd, 0, sizeof (bd_t));

        monitor_flash_len = _bss_start - _armboot_start;

        for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
            if ((*init_fnc_ptr)() != 0) {
                hang ();
            }
        }

首先，我们先来分析`init_fnc_t **init_fnc_ptr;`这行代码。

要分析这行代码，首先看指针数组`init_fnc_t *init_sequence[]`

    typedef int (init_fnc_t) (void);

    init_fnc_t *init_sequence[] = {
        cpu_init,               /* 与CPU相关的初始化 */
        board_init,             /* 与板子初始化相关的初始化 */
        interrupt_init,         /* 中断初始化 */
        env_init,               /* 初始化环境变量 */
        init_baudrate,          /* 初始化波特率设置 */
        serial_init,            /* serial通信相关初始化 */
        console_init_f,         /* console初始化的第一部分 */
        display_banner,         /* say that we are here */
        // ...根据配置，还有一些其它的初始化
        dram_init,              /* 配置可用的RAM块 */
        display_dram_config,
        NULL,
    };

根据这儿的分析，我们就可以知道`init_fnc_ptr`就是一个函数指针。在后面的for循环中，将函数指针数组的首地址`init_sequence`赋值给`init_fnc_ptr`，然后循环，对所有的硬件进行初始化。

而对于代码`gd = (gd_t*)(_armboot_start - CFG_MALLOC_LEN - sizeof(gd_t));`确实有些抽象。而要分析它，必须看一下下面这个宏定义：

    DECLARE_GLOBAL_DATA_PTR;        //在board.c最上面

而它的定义如下：

    #define DECLARE_GLOBAL_DATA_PTR     register volatile gd_t *gd asm ("r8")

这个声明，告诉编译器使用寄存器r8来存储gd_t类型的指针gd，即这个定义声明了一个指针，并且指明了它的存储位置。也就是说，我们声明了一个寄存器变量，它的初始值为`_armboot_start - CFG_MALLOC_LEN - sizeof(gd_t)`，也就是`0x33F80000-(0x20000+2048*1024)-0x24`。也就是说，gd指向了一段可用的内存区域，而这段内存区域相当于u-boot的全局变量。

那指针gd指向的数据结构到底是什么呢？为什么要设置这个数据结构呢？那么接下来让我们看一下这个数据结构吧。

    typedef struct  global_data {
        bd_t            *bd;
        unsigned long   flags;
        unsigned long   baudrate;
        unsigned long   have_console;   /* serial_init() 函数被调用 */
        unsigned long   reloc_off;      /* Relocation Offset */
        unsigned long   env_addr;       /* Address  of Environment struct */
        unsigned long   env_valid;      /* Checksum of Environment valid? */
        unsigned long   fb_base;        /* base address of frame buffer */
    #ifdef CONFIG_VFD
        unsigned char   vfd_type;       /* display type */
    #endif
        void            **jt;           /* jump table */
    } gd_t;

这个数据结构是系统初始化的最小全局变量值，主要在boot引导的早期使用，直到我们设置好内存控制器并能够访问RAM。注意，保证`CFG_GBL_DATA_SIZE`大于`sizeof（gd_t）`。

对于其中的变量，我们进行重点分析。`bd_t *bd;`这句话中，结构体`bd_t`的内容如下：

    typedef struct bd_info {
        int             bi_baudrate;    /* 串口控制台波特率 */
        unsigned long   bi_ip_addr;     /* IP 地址 */
        unsigned char   bi_enetaddr[6]; /* Ethernet adress */
        struct environment_s *bi_env;
        ulong           bi_arch_number; /* 该板唯一的ID */
        ulong           bi_boot_params; /* 存放参数的地方 */
        struct                          /* RAM配置 */
        {
            ulong start;
            ulong size;
        }               bi_dram[CONFIG_NR_DRAM_BANKS];
        // ......
    } bd_t;

从上面的代码中，可以看出，在该结构体内，存放着boot引导前期，系统初始化的最小集合。要么通过串口打印，要么通过网络打印进行人机交互。

        // ...... 此处，如果有LCD等设备，为其准备内存空间
        /* armboot_start在与板子相关的链接脚本文件中定义，初始化动态内存 */
        mem_malloc_init (_armboot_start - CFG_MALLOC_LEN);

        // 其他存储空间的初始化

        env_relocate ();        /* 初始化环境变量 */

        gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");  /* 得到IP地址 */

        /* 获取MAC地址 */
        {
            int i;
            ulong reg;
            char *s, *e;
            char tmp[64];

            i = getenv_r ("ethaddr", tmp, sizeof (tmp));
            s = (i > 0) ? tmp : NULL;

            for (reg = 0; reg < 6; ++reg) {
                gd->bd->bi_enetaddr[reg] = s ? simple_strtoul (s, &e, 16) : 0;
                if (s)
                    s = (*e) ? e + 1 : e;
            }
            // ...
        }

        devices_init ();        /* 获取设备列表 */

        jumptable_init ();

        console_init_r ();      /* 将完全初始化的控制台作为一个设备 */

        enable_interrupts ();   /* 使能异常中断 */

        /* 如果有必要，执行网卡的初始化工作 */

        /* main_loop() can return to retry autoboot, if so just run it again. */
        for (;;) {
            main_loop ();
        }

        /* 不可到达 */
    }

<h2 id="4.2">4.2 加载内核</h2>

首先，我们将`main_loop`函数进行简化，如下图所示。这个流程非常简单明了了。

    void main_loop (void)
    {
        static char lastcommand[CFG_CBSIZE] = { 0, };
        int len;
        int rc = 1;
        int flag;

        char *s;
        int bootdelay;

        s = getenv ("bootdelay");               // 获取延时时间
        bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

        s = getenv ("bootcmd");                 // 获取引导命令

        // 判断延时时间是否到，到了的话直接执行引导命令；如果在此期间有按键按下，则进入For循环
        if (!nobootdelay && bootdelay >= 0 && s && !abortboot (bootdelay)) {
            run_command (s, 0);
        }

        // 主循环，监控命令，并根据命令作出相应的处理
        for (;;) {
            len = readline (CFG_PROMPT);
            flag = 0;                                       /* assume no special flags for now */
            if (len > 0)
                strcpy (lastcommand, console_buffer);
            else if (len == 0)
                flag |= CMD_FLAG_REPEAT;
            // ......其它代码
            if (len == -1)
                puts ("<INTERRUPT>\n");
            else
                rc = run_command (lastcommand, flag);       /* 执行命令 */

            if (rc <= 0) {
                lastcommand[0] = 0;                         /* 非法的或不可重复的命令 */
            }
        }
    }

下面我们来看`run_command`函数的内容，看看到底在执行什么。经简化可得，这个函数有2种流程，一种是按照hush表查找命令，另一种就是下面这个，简单粗暴：

    int run_command(const char *cmd, int flag)
    {
        if (builtin_run_command(cmd, flag) == -1)
            return 1;
        return 0;
    }

那么，我们再来看函数`builtin_run_command`：

    static int builtin_run_command(const char *cmd, int flag)
    {
        //合法性校验
        while (*str) {
            //特殊字符解析
            }
        process_macros (token, finaltoken); //宏展开，即完全解析命令

        //命令执行过程
        if (cmd_process(flag, argc, argv, &repeatable))
            rc = -1;
        return rc ? rc : repeatable;
    }

最后，我们再来看一下函数`cmd_process`:

    cmd_process(int flag, int argc, char * const argv[],
                       int *repeatable)
    {
        cmd_tbl_t *cmdtp;

        cmdtp = find_cmd(argv[0]); //查找命令
        if (cmdtp == NULL) {
            printf("Unknown command '%s' - try 'help'\n", argv[0]);
            return 1;
        }

        if (argc > cmdtp->maxargs)
            rc = CMD_RET_USAGE;

        /* If OK so far, then do the command */
        if (!rc) {
            rc = cmd_call(cmdtp, flag, argc, argv); //真正的执行命令
            *repeatable &= cmdtp->repeatable;
        }
        return rc;
    }

至此，uboot的使命便完成了，将执行的权利交给linux内核。


---

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

---

<h1 id="A">A 参考资料</h1>

<h2 id="A.1">A.1 arm公司官方提供的ARMv7-A体系结构文档</h2>

[《ARM® Architecture Reference Manual ARM®v7-A and ARM®v7-R》](https://developer.arm.com/docs/ddi0406/c/arm-architecture-reference-manual-armv7-a-and-armv7-r-edition)

这是ARM官方文档，具有相当的权威性。但是如果实在不愿意看英文文档，也可以借鉴杜春雷的《ARM体系结构与编程》。

<h2 id="A.2">A.2 Uboot中start.S源码的指令级的详尽解析_v1.6.pdf</h2>

[Uboot中start.S源码的指令级的详尽解析_v1.6.pdf](https://www.crifan.com/files/doc/docbook/uboot_starts_analysis/release/html/uboot_starts_analysis.html)

文章对Uboot中的Start.S源码进行了逐行解读，而且还建立了相关的论坛，是一个不错的资料。

<h2 id="A.3">A.3 ARM指令集快速查询手册.pdf</h2>

（工具手册，快速查找各种arm指令用法）

<h2 id="A.4">A.4 ARM指令详解[ARM标准].pdf</h2>

（arm汇编的一些规范和常用形式，很多例子非常有用：子程序调用、散转、数据块复制等。对写裸机程序很有帮助，同样对理解uboot和内核启动代码有很大帮助）

<h2 id="A.5">A.5 程序员的自我修养—链接、装载与库.pdf</h2>

（里面会介绍ELF文件  程序链接的过程  最重要的部分就是帮助理解uboot的核心重定位！这个真的很重要！！）
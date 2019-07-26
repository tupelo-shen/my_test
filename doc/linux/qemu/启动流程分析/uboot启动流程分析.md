<h1 id="0">0 目录</h1>

* [1 简介](#1)
    - [1.1 uboot是什么？](#1.1)
    - [1.2 存储器](#1.2)
* [2 uboot链接文件](#2)
    - [2.1 u-boot.lds文件分析](#2.1)
    - [2.2 链接文件语法](#2.2)
* [3 uboot启动流程第一阶段](#3)
    - [3.1 start.S文件分析](#3.1)
    - [3.2 ARM汇编指令](#3.2)
        + [3.2.1 常用命令](#3.2.1)
        + [3.2.2 协处理器相关](#3.2.2)
* [4 uboot启动流程第二阶段](#4)
    - [4.1 uboot是什么？](#4.1)
    - [4.2 存储器](#4.2)
* [A 参考资料](#A)
    - [A.1 arm公司官方提供的ARMv7-A体系结构文档](#A.1)
    - [A.2 Uboot中start.S源码的指令级的详尽解析_v1.6.pdf](#A.2)
    - [A.3 ARM指令集快速查询手册.pdf](#A.3)
    - [A.4 ARM指令详解[ARM标准].pdf](#A.4)
    - [A.5 程序员的自我修养—链接、装载与库.pdf](#A.5)

---

<h1 id="1">1 基本概念</h1>

<h2 id="1.1">1.1 uboot是什么？</h2>

当我们厌倦了裸机程序，而想要采用操作系统的时候，uboot就是不得不引入的一段程序。所以，uboot就是一段引导程序，在加载系统内核之前，完成硬件初始化，内存映射，为后续内核的引导提供一个良好的环境。uboot是bootloader的一种，全称为universal boot loader。

<h2 id="1.2">1.2 存储器</h2>

1. norflash/nandflash、SRAM和SDRAM作为存储器有何异同？
    1. norflash - 是非易失性存储器（也就是掉电保存）

        NOR flash带有SRAM接口，有足够的的地址引脚进行寻址，可以很容易地读取其内部的每一个字节（
        注意是 *Read* ！因为flash不是任意写入，而是遵循 *disable write protect -> erase -> write* 。这是flash的特性决定的，
        其电路只能从 *1->0*，而不能 *0->1*翻转。擦除过程就是将flash中的某一个扇区恢复为 *0xFFFFFFFF*，然后再写入数据。另外，代码
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

前面我们已经了解了嵌入式设备的存储介质，以及它们的用途。但是，对于编译后的代码如何在内存上分配，处理器是不知道，这需要我们人为地指定。这时候，后缀为lds的链接文件出现了，它规定了编译后的代码各个段是如何分配的。理解了它，对于我们对u-boot也是大有帮助的。下面我们直接看代码内容，如果需要了解语法，请参考[2.2 链接文件语法](#2.2)。

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

### 2.3.1 .got 语法

在了解这个语法之前，我们先来了解一些其他概念：

1. 加载时重定位

    基本思路是：在链接时，对所有绝对地址的引用不作重定位，而把这一步推迟到装载时再完成。一旦模块装载地址确定，即目标地址确定，那么系统就对程序中所有的绝对地址引用进行重定位。

    设函数foobar相对于代码段的起始地址是0x100,当模块被装载到0x10000000时，我们假设代码段位于模块的最开始，即代码段的装载地址也是0x10000000,那么我们就可以确定foobar的地址为0x10000100。这时候，系统遍历模块中的重定位表，把所有对foobar的地址引用都重定位至0x10000100。

2. 地址无关代码

    加载时重定位解决了动态模块中有绝对地址引用的问题，但是又带来了指令部分无法在多个进程间共享的问题。

    具体想法就是把程序模块中共享的指令部分在加载时不需要因为加载地址的改变而改变。把指令中那些需要被修改的部分分离出来，跟数据部分放在一起，这样指令部分就可以保持不变，而数据部分可以在每个进程中拥有一个副本。这种方案就是目前的地址无关代码（PIC）技术。

    具体方法：先分析模块中各种类型的地址引用方式，把共享对象模块中地址引用按照是否跨模块分为两类：模块内部引用和模块外部引用。按照不同的引用方式又可以分为指令引用和数据访问。

        1. 第一种是模块内部的函数调用、跳转等。
        2. 第二种是模块内部的数据访问，比如模块中定义的全局变量、静态变量。
        3. 第三种是模块外部的函数调用、跳转等。
        4. 第四种是模块外部的数据访问，比如其它模块中定义的全局变量。

3. 全局偏移表（GOT）

对于类型三，我们需要用到代码地址无关（PIC）技术，基本的思想就是把跟地址相关部分放到数据段里面。

ELF的做法是在数据段里建立一个指向这些变量的指针数据，称为全局偏移表（GOT），当代码需要引用该全局变量时，可以通过GOT中相对应的项间接引用。

由于GOT本身是放在数据段的，所以它可以在模块装载时被修改，并且每个进程都可以有独立的副本，相互不受影响。

对于跨模块的函数调用也是如此，不做细致分析了。

<h1 id="3">3 uboot启动流程第一阶段</h1>

主要脉络： *部分硬件初始化 -> 加载完整的uboot到RAM -> 跳转到第二阶段入口开始执行*

<h2 id="3.1">3.1 start.S文件分析</h2>

文件位置： *u-boot/cpu/arm920t/start.S*

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

    start_code:
        # 设置CPU为SVC32模式，即超级管理权限模式
        mrs r0,cpsr
        bic r0,r0,#0x1f
        orr r0,r0,#0xd3
        msr cpsr,r0

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
        /*
         * flush v4 I/D caches
         */
        mov r0, #0
        mcr p15, 0, r0, c7, c7, 0   /* flush v3/v4 cache */
        mcr p15, 0, r0, c8, c7, 0   /* flush v4 TLB */

        /*
         * 禁用MMU和缓存
         */
        mrc p15, 0, r0, c1, c0, 0
        bic r0, r0, #0x00002300 @ clear bits 13, 9:8 (--V- --RS)
        bic r0, r0, #0x00000087 @ clear bits 7, 2:0 (B--- -CAM)
        orr r0, r0, #0x00000002 @ set bit 2 (A) Align
        orr r0, r0, #0x00001000 @ set bit 12 (I) I-Cache
        mcr p15, 0, r0, c1, c0, 0

        /*
         * 在重加载之前，我们必须设置RAM的时序，因为内存的时序依赖于板子，
         * 在board目录下可以发现lowlevel_init.S文件
         */
        mov ip, lr
    #if defined(CONFIG_AT91RM9200DK) || defined(CONFIG_AT91RM9200EK) || defined(CONFIG_AT91RM9200DF)
    #else
        bl  lowlevel_init
    #endif
        mov lr, ip
        mov pc, lr
    #endif /* CONFIG_SKIP_LOWLEVEL_INIT */

第242~246行，使I/D cache失效： 协处理寄存器操作，将r0中的数据写入到协处理器p15的c7中，c7对应cp15的cache控制寄存器

第247行，使TLB操作寄存器失效：将r0数据送到cp15的c8、c7中。C8对应TLB操作寄存器

第252行，将c1、c0的值写入到r0中

第257行，将设置好的r0值写入到协处理器p15的c1、c0中，关闭MMU

第264行，将lr寄存器内容保存到ip寄存器中，用于子程序调用返回

第265行，跳转到lowlevel_init入口地址执行，lowlevel_init在lowlevel_init.S文件中，代码如下：

#### 1. Cache是什么呢？

Cache是处理器内部的一个高速缓存单元，为了应对处理器和外部存储器之间的速度不匹配而设立的。其速度要比内存的读写速度快好多，接近处理器的工作速度，一般处理器从内存中读取数据到Cache中，到下次再用到数据时，会先去cache中查找，如果cache中存在的话，就不会访问内存了，用以提高系统性能。

#### 2. 系统引导时为什么关闭Cache？

从上面的解释中，可以看出，在系统未初始化完成时，代码还没有转移到内存中，也就是说，我们还没有用到内存，先将MMU和Cache关闭，以免发生不可预料的错误。

#### 3. 怎样使Cache中的数据无效？



<h2 id="3.2">3.2 ARM汇编指令</h2>

<h3 id="3.2.1">3.2.1 常用命令</h3>

1. bl - 跳转指令

    除了包含b指令单纯的跳转功能，还执行，在跳转之前，把r15寄存器=PC=CPU地址；把下一条要执行的指令赋值给r14，也就是r14=lr。然后跳转到对应的位置，开始执行。执行完毕后，再用 *mov pc, lr* 再跳转到调用之前的地址。 所以，就是调用子程序的过程。

<h3 id="3.2.2">3.2.2 协处理器相关</h3>

1. MCR - ARM寄存器数据传到协处理器寄存器

    协处理器的格式为：

        MCR [协处理器编号]，[协处理器操作码1]，[源寄存器]，[目的寄存器1]，[目的寄存器2]，[协处理器操作码2]

            其中协处理器操作码1 和协处理器操作码2 为协处理器将要执行的操作，

            源寄存器为ARM 处理器的寄存器，目的寄存器1 和目的寄存器2 均为协处理器的寄存器。


<h1 id="4">4 uboot启动流程第二阶段</h1>


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
* [1 查找](#1)
    - [1.1 uboot是什么？](#1.1)
    - [1.2 存储器](#1.2)
* [2 uboot启动流程第一阶段](#2)
    - [2.1 start.S文件分析](#2.1)
    - [2.2 存储器](#2.2)
* [3 uboot启动流程第二阶段](#2)
    - [2.1 uboot是什么？](#2.1)
    - [2.2 存储器](#2.2)
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

1. norflash、SRAM和SDRAM作为存储器有何异同？
    1. norflash - 是非易失性存储器（也就是掉电保存）

        NOR flash带有SRAM接口，有足够的的地址引脚进行寻址，可以很容易地读取其内部的每一个字节（
        注意是 *Read* ！因为flash不是任意写入，而是遵循 *disable write protect -> erase -> write* 。这是flash的特性决定的，
        其电路只能从 *1->0*，而不能 *0->1*翻转。擦除过程就是将flash中的某一个扇区恢复为 *0xFFFFFFFF*，然后再写入数据。另外，代码
        指令可以直接在norflash上运行。

        （**重要！！！上电后可以读取norflash中的数据但是不可以进行写操作**）

    2. SRAM - 静态随机访问存储器 - Static Random Access Memory

        *static* 是指只要不掉电，存储在SRAM中的数据就不会丢失。这一点与DRAM不同，DRAM需要进行周期性刷新操作。然而，我们不应将
        SRAM和只读存储器（ROM）、Flash Memory相混淆，因为SRAM是一种易失性存储器，它只有在电源保持连续供应的情况下才能够保持数据。
        *Random Access* 指的是存储器的内容可以任意顺序进行访问，而不管前一次访问的是哪一个位置。

        （**重要！！！上电后就可以读写SRAM中的数据，而无需初始化操作**）

    3. SDRAM - 同步动态随机存取存储器 - Synchronous Dynamic Random Access Memory

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

        它的重要特性是容量大：512；程序运行速度快；价格便宜。

        但是其掉电易失数据，上电后必须初始化DDR控制器，否则无法进行数据的读写。所以在运行系统内核之前必须对其进行初始化，这就是在NOR Flash 和 SRAM上搭建的程序的作用。

        那它主要都完成什么工作呢？

            1. 完成对处理器时钟的初始化
            2. DDR的初始化
            3. 给 gd_t *gd_ptr 赋值(用于存储uboot镜像的长度，重定位地址，重定位偏移量等信息)

        在uboot搬运到DDR中运行前进行最小系统的初始化，之后就将uboot搬运到DDR中运行。那么，此时NOR Flash和SRAM的任务就完成了（也就是没有用了）。

<h1 id="2">2 uboot启动流程第一阶段</h1>

主要脉络： *部分硬件初始化 -> 加载完整的uboot到RAM -> 跳转到第二阶段入口开始执行*

<h2 id="2.1">2.1 start.S文件分析</h2>

# 1. 启动-_start

查看下面的代码：

    .globl _start                           //定义一个全局标号_star
    _start: b    reset                      //标号_start处的内容为跳转到reset标号开始执行
    //将_undefined_instruction标号表示的内容作为地址，加载到PC中，
    //这种用法可以实现执行流程的跳转
        ldr    pc, _undefined_instruction
        ldr    pc, _software_interrupt
        ldr    pc, _prefetch_abort
        ldr    pc, _data_abort
        ldr    pc, _not_used
        ldr    pc, _irq
        ldr    pc, _fiq
    //以上七条ldr pc,x加上b reset共八条指令组成中断向量表
          …
    //_undefined_instruction标号表示定义了一个word类型的变量undefined_instruction
    _undefined_instruction: .word undefined_instruction
    …
    //exception handlers                    //异常处理
    .align    5                             //5字节对齐

    //可知undefined_instruction的真正用途是指向此处代码，即异常处理的具体实现
    undefined_instruction:
    get_bad_stack
    bad_save_user_regs
    bl    do_undefined_instruction

从上面的内容可以看出，除第1行代码之外，其余代码都是跳转到特定位置去执行中断服务子程序。

正常情况下，程序的执行流程是不会走到中断处理流程中去的，而是直接跳转到reset处开始执行。那我们接下来就看reset处的代码都干了什么。

# 2. reset

    reset:
        /*
         * set the cpu to SVC32 mode -> 设置CPU为SVC32模式，即管理模式
         */
        mrs    r0, cpsr                 // 读出
        bic    r0, r0, #0x1f            // 低五位清0
        orr    r0, r0, #0xd3            // 与D3做与操作
        msr    cpsr,r0                  // 写回

cpsr 是ARM体系结构中的程序状态寄存器，其结构如下：

| M[4:0] | cpu mode | re

<h1 id="3">3 uboot启动流程第二阶段</h1>


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
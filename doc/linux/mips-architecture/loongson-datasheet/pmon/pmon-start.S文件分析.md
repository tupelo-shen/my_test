应用程序在硬件平台上跑起来，分为两个主要阶段：汇编程序运行阶段和C程序运行阶段。为什么这么说呢？因为C语言的运行需要堆栈的支持。所以，在开始阶段，堆栈等的最基本初始化工作由start.S中的汇编代码实现。但是，为了尽量使移植更为简单。程序开发人员也在努力减少start.S需要完成的工作。一旦堆栈构建完成，就可以运行C语言程序了。

本文主要是分析start.S文件。为了方便调试，加上串口打印函数是非常有必要的。上面提到的2个阶段需要分别实现打印函数。

# 1. 串口打印函数

PMON源代码中的start.S文件一开始，就定义了一个`TTYDEBUG`宏，用来输出DEBUG信息。具体的宏定义如下：

    #define TTYDBG(x) \
        .rdata;98: .asciz x; .text; la a0, 98b; bal stringserial; nop

将其展成可读样式：

        .rdata;             # 标记数据段
    98: .asciz x;           # 字符串x，98为字符串x的首地址标号
        .text;              # 标记代码段
        la a0, 98b;         # 将字符串x的首地址加载到a0寄存器中
        bal stringserial;   # 跳转到子函数stringserial处执行
        nop                 # 在此添加nop操作，是因为分支指令可以造成流水线的延时，
                            # 其实，可以在此处添加一条合理的指令，提高流水线执行效率。
                            # 但是，这个需要考虑的情况过于复杂，最简单的方式就是添加nop操作

那接下来就看stringserial函数的内容了：

    LEAF(stringserial)
        move    a2, ra      # 把返回地址保存到a2寄存器中      
        addu    a1, a0, s0  # s0寄存器被约定为load offset
        lbu     a0, 0(a1)   # 取首字节到a0寄存器中
    1:
        beqz    a0, 2f      # 如果a0寄存器的值等于0，
                            # 则向前跳转到标签2处执行
        nop                 # 填充分支延时
        bal     tgt_putchar # 跳转到tgt_putchar处执行
        addiu   a1, 1       # 指向下一个字符
        b       1b          # 向后跳转到标签1处执行
        lbu     a0, 0(a1)   # 取出下一个字符
    2:
        j       a2          # 返回到调用该函数的地方继续执行
        nop
    END(stringserial)

上面的代码中，需要注意的是下面这两句语句：

    b       1b          # 向后跳转到标签1处执行
    lbu     a0, 0(a1)   # 取出下一个字符

按照正常的逻辑，这两条语句的顺序好像是反了，其实不然。这与mips架构的五级流水线有着深入的关系。因为分支指令`b 1b`后面会造成一个延时槽，为了提高效率，可以插入一条其它指令（不能对前后的上下文有影响）。所以，把取字符的指令放到了后面。

继续深入`tgt_putchar`函数：

    LEAF(tgt_putchar)
        .set noat
        move    AT, ra                  # 保存返回地址
        la      v0, COM1_BASE_ADDR      # 加载串口1的基地址
        bal     1f                      # 跳转到标签1处执行
                                        # 并把返回地址存入ra寄存器中
        nop
        jr      AT                      # 返回调用tgt_putchar处继续执行
        nop
      
    1:
        lbu v1, NSREG(NS16550_LSR)(v0)  # NSREG(NS16550_LSR)计算串口芯片的寄存器偏移
        and v1, LSR_TXRDY               # 设置TXRDY标志
        beqz    v1, 1b                  # v1寄存器等于0，则重新设置标志
        nop
                             
        sb  a0, NSREG(NS16550_DATA)(v0) # 加载数据到数据寄存器
        j   ra
        nop
        .set at
    END(tgt_putchar)

至此，字符串打印函数就分析完了。

`start.S`文件中还有一个16进制打印函数`hexserial`。代码如下：

    LEAF(hexserial)
        move    a2, ra      # 保存返回地址
        move    a1, a0      # 保存要显示的值到寄存器a1中
        li      a3, 7       # 循环次数
    1:
        rol     a0, a1, 4   # 循环左移4位
        move    a1, a0      # 保存新值
        and     a0, 0xf     # 
        la      v0, hexchar # 加载hexchar地址到v0中
        addu    v0, s0      # s0记录uncache的偏移量
        addu    v0, a0      # v0=v0+s0+a0
    #ifdef BOOT_FROM_NAND
    #ifdef NAND_ECC_MODE
        addiu   v0, v0,0x400 
    #endif
    #endif
        bal     tgt_putchar # 跳转到tgt_putchar执行
        lbu     a0, 0(v0)   # 将要显示的字符存入a0寄存器

        bnez    a3, 1b      # 如果a3不等于0，跳转到1处执行
        addu    a3, -1      # a3寄存器全写1

        j       a2          # 返回调用处执行
        nop
    END(hexserial)

`hexchar`定义如下：

    hexchar:
        .ascii  "0123456789abcdef"

# 2 代码执行流程

#### 2.1 上电

当整个板子上电后，CPU将从0xbfc00000地址处取指令执行。（这是硬件完成的，将该地址硬链接到flash上。）

而我们知道，最终的二进制文件`gzrom.bin`中的开头部分就是`start.S`文件中的代码。为什么是这样呢？

通过makefile我们就会知道到底是怎么回事了。

1. 通过执行：

        gcc -DSTARTADDR=${GZROMSTARTADDR} -DOUT_FORMAT=\"${OUT_FORMAT}\" -DOUT_ARCH=mips -Umips -E -P ld.script.S  > ld.script

    我们可以知道，这里的链接文件是通过gcc的特性生成的。将ld.scripts.S编译生成ld.script。

    链接脚本的内容如下所示：

        OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips",
              "elf32-tradlittlemips")
        OUTPUT_ARCH(mips)
        ENTRY(_start)
        SECTIONS
        {
            . = 0xffffffff8f900000;
            .text :
            {
                _ftext = . ;
                *(.text)
                *(.rodata)
                *(.rodata1)
                *(.reginfo)
                *(.init)
                *(.stub)
                *(.gnu.warning)
            } =0
            ......(此处省略)
        }

    我们可以知道，在内存中的起始地址定义为`0xffffffff8f900000`，如果是32位系统就是`0x8f900000`。

2. 通过

        ${LD} -T ld.script -e start -o gzrom ${START} zloader.o

    将`${START} zloader.o`以ld.script脚本指定的形式链接成gzrom文件，入口地址位start的地址。

3. 通过

        ${CROSS_COMPILE}objcopy -O binary gzrom gzrom.bin

    将elf格式的gzrom转换成bin格式的gzrom.bin文件。

#### 2.2 _start入口：

        /*
         + 程序开始的地方：
         + 1. 告诉编译器不要对后面的代码进行优化
         + 2. 声明了3个全局标签
         */
        .set    noreorder
        .globl  _start
        .globl  start
        .globl  __main

        /* 程序入口：ld.script使用 */
    _start:
    start:
        /* 定义堆栈stack，位于pmon程序代码之下的RAM地址中。大小为16k. */
        .globl  stack
        stack = start - 0x4000

        /*set all spi cs to 1, default input*/
        li      v0,0xbfff0225
        li      v1,0xff
        sb      v1,(v0)

#### 2.3 初始化寄存器

    /* 
     * 初始化处理器的工作状态
     * 1. 禁止中断和异常处理，保留boot异常
     * 2. 设置BEV=1，也就是说异常的入口地址为0xBFC00000；
     *    BEV=0，异常地址为0x80000000.
     * 3. 初始化通用寄存器
     * 4. 通过bal locate跳转到真正程序开始的地方
     */
    /* NOTE!! Not more that 16 instructions here!!! Right now it's FULL! */
    mtc0    zero, COP_0_STATUS_REG  /* 禁止中断 */
    mtc0    zero, COP_0_CAUSE_REG   /* 禁止异常处理 */
    li      t0, SR_BOOT_EXC_VEC     /* Exception to Boostrap Location */
                                    /* 设置状态寄存器的BEV标志位，这样是让CP0 */
                                    /* 运行在没有TLB的模式，并且一旦发生异常，*/
                                    /* 就进入ROM的0xbfc00000位置重启 */ 
    mtc0    t0, COP_0_STATUS_REG    /* 将0x00400000写入SR寄存器中，BEV标志等于1*/
                                    /* CPU使用kseg1内存区作为物理内存的映射 */

    bal     initregs                /* 初始化通用寄存器 */
    nop

    .set    mips32
    mfc0    t0, $16, 6      #Store fill
    .set    mips3
    li  t1, 0xfffffeff
    and t0, t1, t0
    .set    mips32
    mtc0    t0, $16, 6      #Store fill
    .set    mips3

    /* spi speedup */
    li      t0, 0xbfff0220
    li      t1, 0x47
    sb      t1, 0x4(t0)

    bal     locate                  /* 获取当前要执行的地址 */
    nop

    /*
     * 与0xa0000000进行or操作，也就是说，从ROM加载时，不会改变返回地址ra的值。
     * 保证物理内存映射到kseg1地址空间
     */
    uncached:
        or  ra, UNCACHED_MEMORY_ADDR/* 0xa0000000 */
        j   ra
        nop

#### 2.4 设置异常向量表和重启

        /*
         *  Reboot vector usable from outside pmon.
         */
        .align  8
    ext_map_and_reboot:
        bal     CPU_TLBClear
        nop

        li      a0, 0xc0000000
        li      a1, 0x40000000
        bal     CPU_TLBInit
        nop
        la      v0, tgt_reboot
        la      v1, start
        subu    v0, v1
        lui     v1, 0xffc0
        addu    v0, v1
        jr      v0
        nop

        /*
         *  Exception vectors here for rom, before we are up and running. Catch
         *  whatever comes up before we have a fully fledged exception handler.
         */
        .align  9           /* bfc00200 */
        move    k0, ra      #save ra
        la      a0, v200_msg
        bal     stringserial
        nop
        b       exc_common

        .align  7           /* bfc00280 */
        move    k0, ra  #save ra
        la      a0, v280_msg
        bal     stringserial
        nop
        b       exc_common

        /* Cache error */
        .align  8           /* bfc00300 */
        PRINTSTR("\r\nPANIC! Unexpected Cache Error exception! ")
        mfc0    a0, COP_0_CACHE_ERR
        bal     hexserial
        nop
        b       exc_common

    /* General exception */
        .align  7           /* bfc00380 */
        move    k0, ra      #save ra
        la  a0, v380_msg
        bal stringserial
        nop
        b   exc_common
        
        .align  8           /* bfc00400 */
        move    k0, ra      #save ra
        la  a0, v400_msg
        bal stringserial
        nop

    #if 1
        b   exc_common
        nop

        /* Debug exception */
        .align  7           /* bfc00480 */
        #include "exc_ejtag.S"
    #endif


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
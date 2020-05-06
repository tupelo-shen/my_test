<h1 id="0">0 目录</h1>

* [2.1 MIPS汇编语言的风格初探](#2.1)
* [2.2 寄存器](#2.2)
* [2.3 RISC和CISC对比](#2.3)
* [2.4 MIPS架构的发展](#2.4)
* [2.5 MIPS架构和CISC架构的对比](#2.5)

---

架构这个词，英文是architecture，牛津词典对其解释为`the design and structure of a computer system`。所以，这个词体现的是设计和结构，也就是说，是一个抽象机器或通用模型概念上的描述，而不是一个真实机器的实现。这就好比一辆手动挡车，无论是前轮驱动还是后轮驱动，它的油门总是在右，离合器在左。这里，油门和离合器的位置就相当于架构，前轮还是后轮驱动是具体实现。所以，相同的架构，实现未必相同。

当然了，如果你是一个拉力赛车手，在湿滑的路上高速行驶时，前轮驱动还是后轮驱动就很重要了。计算机也是一样，如果对某个方面有特殊的需求，实现的细节就很重要了。

通常，CPU架构由指令集和寄存器组成。术语-指令集和架构在语义上非常接近，所以，有时候你也会见到这两个词的组合缩写-指令集架构（ISA）。

对于MIPS指令集架构描述最好的，肯定是MIPS公司出版的MIPS32和MIPS64架构规范。MIPS32是MIPS64的一个子集，用于描述具有32位通用目的寄存器的CPU。为了简单，我们缩写为`MIPS32/64`。

生产MIPS架构CPU的公司，尽量兼容`MIPS32/64`规范。

在`MIPS32/64`规范之前，已经发布了多版的MIPS架构。但是，这些旧架构只是规定了软件使用的指令和资源，并没有定义操作系统所需要的CPU控制机制，而是将其认为应该在实现时定义。通俗地讲，早期版本的MIPS架构对CPU控制单元的硬件实现不做约束，由芯片制造商在实现时自己实现。这意味着，对于可移植操作系统需要做更多的工作，去适配因此而带来的差异。好消息是，几乎每一个版本的MIPS架构，都有一个作为所有实现的`父版本`存在。

* MIPS I: 

    最早的32位处理器（R2000/3000）使用的指令集，几乎每一个MIPS架构CPU都可以运行这些指令。

* MIPS II: 

    为没有投产的MIPS-R6000机器定义的指令集。MIPS-II是MIPS32的前身。

* MIPS III: 

    为R4000引入的64位指令集。

* MIPS IV: 

    在MIPS-III基础上添加了浮点指令，R10000和R5000硬件实现中使用。

* MIPS V:

    添加了2个奇怪的SIMD浮点操作指令，但是没有具体的CPU实现。大多是作为MIPS64架构的可选部分-`单精度对（paired-single）`-出现。

* MIPS32、MIPS64： 

    1998年，由从`Silicon Graphics`公司分拆出来的`MIPS Technologies Inc.`公司制定的标准。该标准第一次纳入了CPU控制的功能，由协处理器0实现。MIPS32是MIPS-II的超集，MIPS64是MIPS-IV的超集（还以可选的方式包含了MIPS-V的大部分）。

    大多数1999年之后设计的MIPS架构CPU都兼容这些标准。所以，在后面的描述中，我们使用`MIPS32/64`作为基础架构。到目前为止，`MIPS32/64`规范已经发布到了第6版。

#### 指令集扩展的规范化—ASE

我们一直强调，RISC和保持指令集小没有关系。事实上，RISC的简单性，更容易让人进行扩展。

随着MIPS架构的CPU出现在嵌入式系统中，许多新的指令如雨后春笋般地冒出来。MIPS32/64吸收了一些，同时也提供了一种扩展机制ASE（`Application-Specific instruction set Extensions`）。ASE作为MIPS32/64的扩展存在，可以通过配置寄存器进行选择。下面是一些选项：

* MIPS16e:

    类似于ARM架构的thumb指令的一种扩展。是一种旧扩展。基本不用。

* MDMX:

    类似于英特尔的MMX扩展的早期版本。但是，MDMX从来没有实现。

* SmartMIPS:

    为提高MIPS架构的加密性能而扩展的一个模块。这个功能扩展还是比较有用的，尤其是在当下这个特别注重数据安全的时代。

* MT:

    将硬件多线程技术添加到MIPS核中。2005年，第一次出现在MIPS公司的34-K系列产品中。

* DSP:

    音视频处理指令，将饱和和SIMD算法运用到小整数上。看上去比MDMX更有用。2005年，开始在MIPS公司的24-K和34-K系列中推出。

MIPS32/64规范还有一些可选项，它们不能被看作为指令集的扩展：

* 浮点单元：

    协处理器1控制。

* CP2:
    
    协处理器2，用户自定义。很少有人使用。

* CorExtend:

    用户自定义指令集。2002/2003年大肆炒作的一个概念，ARM和Tensilica公司也宣布支持。

* EJTAG:

    调试工具。

* 单精度对:

    浮点单元的扩展，提供SIMD操作。每条指令可以同时操作2个单精度值。

* MIPS-3D: 

    通常和单精度对结合使用，提供了一些指令，用于3D场景渲染时的浮点矩阵运算。

<h2 id="2.1">2.1 MIPS汇编语言的风格初探</h2>

本部分对汇编语言只做一个简单的介绍，详细的理解后面会再展开。

我们或多或少地已经接触过汇编语言，下面是MIPS架构的一小段汇编代码：

        # 注释
    entrypoint:         # 标签
        addu $1, $2, $3 # 基于寄存器的加法，等价于 $1 = $2 + $3

跟大部分的汇编语言一样，基于行的分割语言。原生注释符号是`#`，编译器会忽略掉`#`后面的所有文本。但是可以在一行中插入多条语句，使用`;`进行分割。

标签（label）使用`:`开始，可以包含各类符号。标签可以定义代码的入口点和数据存储的开始位置。

MIPS汇编程序可以使用数字标记的通用寄存器，也可以使用C语言的预处理器和一些标准头文件，这样就可以使用寄存器的别称（关于别称请参考下一节）。当然了，如果使用C预处理器，注释也可以使用C风格。

大多数指令是三目运算指令，目的寄存器在左边（与X86相反）。

    subu $1, $2, $3

代表的表达式是：

    $1 = $2 - $3;

目前，了解这些就足够了。

<h2 id="2.2">2.2 寄存器</h2>

MIPS有32个通用寄存器（$0-$31），各寄存器的功能及汇编程序中使用约定如下：

下表描述32个通用寄存器的别名和用途

| 寄存器  | 别名    | 使用 |
| ------- | ------- | ---- |
| $0      | $zero   | 常量0 |
| $1      | $at     | 保留给汇编器 |
| $2-$3   | $v0-$v1 | 函数返回值 |
| $4-$7   | $a0-$a3 | 函数调用参数 |
| $8-$15  | $t0-$t7 | 临时寄存器 |
| $16-$23 | $s0-$s7 | 保存寄存器 |
| $24-$25 | $t8-$t9 | 临时寄存器 |
| $26-$27 | $k0-$k1 | 保留给系统 |
| $28     | $gp     | 全局指针 |
| $29     | $sp     | 堆栈指针 |
| $30     | $fp     | 帧指针 |
| $31     | $ra     | 返回地址 |

##详细的寄存器使用说明

* $0:即$zero,该寄存器总是返回零，为0这个有用常数提供了一个简洁的编码形式。
    
    比如，下面的代码：

        move $t0,$t1（伪指令常用）
    
    实际为：

        add $t0,$0,$t1
    
    使用伪指令可以简化任务，汇编程序提供了比硬件更丰富的指令集。

* $1:即$at，该寄存器为汇编保留。
    
    由于I型指令的立即数字段只有16位，在加载大常数时，编译器或汇编程序需要       把大常数拆开，然后重新组合到寄存器里。比如加载一个32位立即数需要 lui（装入高位立即数）和addi两条       指令。像MIPS程序拆散和重装大常数由汇编程序来完成，汇编程序必需一个临时寄存器来重组大常数，这也是为汇编 保留$at的原因之一。

* $2..$3:($v0-$v1)用于子程序的非浮点结果或返回值。

    对于子程序如何传递参数及如何返回，MIPS范围有一套约       定，堆栈中少数几个位置处的内容装入CPU寄存器，其相应内存位置保留未做定义，当这两个寄存器不够存       放返回值时，编译器通过内存来完成。

* $4..$7:($a0-$a3)用来传递前四个参数给子程序，不够的用堆栈。

    a0-a3和v0-v1以及ra一起来支持子程序/过程调用，分别用以传递参数，返回结果和存放返回地址。当需要使用更多的寄存器时，就需要堆栈了,MIPS编译器总是为参数在堆栈中留有空间以防有参数需要存储。

* $8..$15:($t0-$t7)临时寄存器，子程序可以使用它们而不用保留。

* $16..$23:($s0-$s7)保存寄存器，在过程调用过程中需要保留（被调用者保存和恢复，还包括$fp和$ra）。

    MIPS提供了临时寄存器和保存寄存器，这样就减少了寄存器溢出（spilling,即将不常用的变量放到存储器的过程),  编译器在编译一个叶（leaf)过程（不调用其它过程的过程）的时候，总是在临时寄存器分配完了才使用需要       保存的寄存器。

* $24..$25:($t8-$t9)同($t0-$t7)

* $26..$27:($k0,$k1)为操作系统/异常处理保留，至少要预留一个。 

    异常（或中断）是一种不需要在程序中显示调用的过程。MIPS有个叫异常程序计数器（exception program counter,EPC)的寄存器，属于CP0寄存器，用于保存造成异常的那条指令的地址。查看控制寄存器的唯一方法是把它复制到通用寄存器里，指令mfc0 (move from system control)可以将EPC中的地址复制到某个通用寄存器中，通过跳转语句（jr)，程序可以返 回到造成异常的那条指令处继续执行。MIPS程序员都必须保留两个寄存器$k0和$k1，供操作系统使用。

    发生异常时，这两个寄存器的值不会被恢复，编译器也不使用k0和k1,异常处理函数可以将返回地址放到这两个中的任何一个，然后使用jr跳转到造成异常的指令处继续执行。

* $28:($gp)为了简化静态数据的访问，MIPS软件保留了一个寄存器：全局指针gp(global pointer,$gp)。

    全局指针只存储静态数据区中的运行时决定的地址，在存取位于gp值上下32KB范围内的数据时，只需要一条以gp为基指针的指令即可。在编译时，数据须在以gp为基指针的64KB范围内。

* $29:($sp)堆栈指针寄存器。

    MIPS硬件并不直接支持堆栈，你可以把它用于别的目的，但为了使用别人的程序或让别人使用你的程序， 还是要遵守这个约定的，但这和硬件没有关系。

* $30:($fp)存放栈帧指针寄存器。

    为支持MIPS架构的GNU C编译器保留的，MIPS公司自己的C编译器没有使用，而把这个寄存器当作保存寄存器使用（$s8),这节省了调用和返回开销，但增加了代码生成的复杂性。

* $31:($ra)存放返回地址。

    MIPS有个jal(jump-and-link,跳转并链接)指令，在跳转到某个地址时，把下一条指令的 地址放到$ra中，用于支持子程序调用。例如，调用程序把参数放到$a0~$a3,然后使用jal指令跳转到子程序执行；被调用过程完成后，把结果放到$v0,$v1寄存器中，然后使用`jr $ra`返回。

## MIPS 指令总结

下表是MIPS指令总结表，供以后查阅使用：

| 指令 | 举例 | 功能描述 |
| ---- | ---- | -------- |
| LB   | LB R1, 0(R2) | 从存储器中读取一个字节的数据到寄存器中 |
| LH   | LH R1, 0(R2) | 从存储器中读取半个字的数据到寄存器中 |
| LW   | LW R1, 0(R2) | 从存储器中读取一个字的数据到寄存器中 |
| LD   | LD R1, 0(R2) | 从存储器中读取双字的数据到寄存器中 |
| L.S  | L.S R1, 0(R2)| 从存储器中读取单精度浮点数到寄存器中 |
| L.D  | L.D R1, 0(R2)| 从存储器中读取双精度浮点数到寄存器中 |
| LBU  | LBU R1, 0(R2)| 功能与LB指令相同，但读出的是无符号数据 |
| LHU  | LHU R1, 0(R2)| 功能与LH指令相同，但读出的是无符号数据 |
| LWU  | LWU R1, 0(R2)| 功能与LW指令相同，但读出的是无符号数据 |
| SB   | SB R1, 0(R2) | 把一个字节的数据从寄存器存储到存储器中 |
| SH   | SH R1，0(R2) | 把半个字节的数据从寄存器存储到存储器中 |
| SW   | SW R1, 0(R2) | 把一个字的数据从寄存器存储到存储器中   |
| SD   | SD R1, 0(R2) | 把两个字节的数据从寄存器存储到存储器中 |
| S.S  | S.S R1, 0(R2)| 把单精度浮点数从寄存器存储到存储器中   |
| S.D  | S.D R1, 0(R2)| 把双精度数据从存储器存储到存储器中     |
| DADD | DADD R1,R2,R3| 把两个定点寄存器的内容相加             |
| DADDI| DADDI R1,R2,#3|把一个寄存器的内容加上一个立即数       |
| DADDU| DADDU R1,R2,R3|不带符号的加                           |
| DADDIU|DADDIU R1,R2,#3| 把一个寄存器的内容加上一个无符号的立即数|
| ADD.S| ADD.S F0,F1,F2|把一个单精度浮点数加上一个双精度浮点数，结果是单精度浮点数|
| ADD.D| ADD.D F0,F1,F2|把一个双精度浮点数加上一个单精度浮点数，结果是双精度浮点数|
| ADD.PS|ADD.PS F0,F1,F2|两个单精度浮点数相加，结果是单精度浮点数|
| DSUB | DSUB R1,R2,R3 | 两个寄存器的内容相减，也就是定点数的减 |
| DSUBU| DSUBU R1,R2,R3| 不带符号的减 |
| SUB.S| SUB.S F1,F2,F3| 一个双精度浮点数减去一个单精度浮点数，结果为单精度 |
| SUB.D| SUB.D F1,F2,F3| 一个双精度浮点数减去一个单精度浮点数，结果为双精度浮点数 |
| SUB.PS| SUB.SP F1,F2,F3 | 两个单精度浮点数相减 |
| DDIV | DDIV R1,R2,R3 | 两个定点寄存器的内容相除，也就是定点除 |
| DDIVU| DDIVU R1,R2,R3 | 不带符号的除法运算 |
| DIV.S| DIV.S F1,F2,F3 | 一个双精度浮点数除以一个单精度浮点数，结果为单精度浮点数 |
| DIV.D| DIV.D F1,F2,F3 | 一个双精度浮点数除以一个单精度浮点数，结果为双精度浮点数 |
| DIV.PS|DIV.PS F1,F2,F3| 两个单精度浮点数相除，结果为单精度 |
| DMUL | DMUL R1,R2,R3| 两个定点寄存器的内容相乘，也就是定点乘 |
| DMULU| DMULU R1,R2,R3 | 不带符号的乘法运算 |
| MUL.S| DMUL.S F1,F2,F3| 一个双精度浮点数乘以一个单精度浮点数，结果为单精度浮点数 |
| MUL.D| DMUL.D F1,F2,F3| 一个双精度浮点数乘以一个单精度浮点数，结果为双精度浮点数 |
| MUL.PS|DMUL.PS F1,F2,F3|两个单精度浮点数相乘，结果为单精度浮点数|
| AND  | AND R1,R2,R3 | 与运算，两个寄存器中的内容相与 |
| ANDI | ANDI R1,R2,#3 | 一个寄存器中的内容与一个立即数相与 |
| OR   | OR R1,R2,R3  | 或运算，两个寄存器中的内容相或 |
| ORI  | ORI R1,R2,#3  | 一个寄存器中的内容与一个立即数相或 |
| XOR  | XOR R1,R2,R3 | 异或运算，两个寄存器中的内容相异或 |
| XORI | XORI R1,R2,#3 | 一个寄存器中的内容与一个立即数异或 |
| BEQZ | BEQZ R1,0      | 条件转移指令，当寄存器中内容为0时转移发生 |
| BENZ | BNEZ R1,0      | 条件转移指令，当寄存器中内容不为0时转移发生 |
| BEQ  | BEQ R1,R2      | 条件转移指令，当两个寄存器内容相等时转移发生 |
| BNE  | BNE R1,R2      | 条件转移指令，当两个寄存器中内容不等时转移发生 |
| J    | J name         | 直接跳转指令，跳转的地址在指令中 |
| JR   | JR R1          | 使用寄存器的跳转指令，跳转地址在寄存器中 |
| JAL  | JAL R1 name    | 直接跳转指令，并带有链接功能，指令的跳转地址在指令中，跳转发生时要把返回地址存放到R31这个寄存器中 |
| JALR | JALR R1 | 使用寄存器的跳转指令，并且带有链接功能，指令的跳转地址在寄存器中，跳转发生时指令的放回地址放在R31这个寄存器中|
| MOV.S| MOV.S F0,F1    | 把一个单精度浮点数从一个浮点寄存器复制到另一个浮点寄存器 |
| MOV.D| MOV.D F0,F1    | 把一个双精度浮点数从一个浮点寄存器复制到另一个浮点寄存器 |
| MFC0 | MFC0 R1,R2     | 把一个数据从通用寄存器复制到特殊寄存器 |
| MTC0 | MTC0 R1,R2     | 把一个数据从特殊寄存器复制到通用寄存器 |
| MFC1 | MFC1 R1,F1     | 把一个数据从定点寄存器复制到浮点寄存器 |
| MTC1 | MTC1 R1,F1     | 把一个数据从浮点寄存器复制到定点寄存器 |
| LUI  | LUI R1,#42     | 把一个16位的立即数填入到寄存器的高16位，低16位补零 |
| DSLL | DSLL R1,R2,#2  | 双字逻辑左移 |
| DSRL | DSRL R1,R2,#2  | 双字逻辑右移 |
| DSRA | DSRA R1,R2,#2  | 双字算术右移 |
| DSLLV| DSLLV R1,R2,#2 | 可变的双字逻辑左移 |
| DSRLV| DSRLV R1,R2,#2 | 可变的双字罗伊右移 |
| DSRAV| DSRAV R1,R2,#2 | 可变的双字算术右移 |
| SLT  | SLT R1,R2,R3   | 如果R2的值小于R3，那么设置R1的值为1，否则设置R1的值为0 |
| SLTI | SLTI R1,R2,#23 | 如果寄存器R2的值小于立即数，那么设置R1的值为1，否则设置寄存器R1的值为0 |
| SLTU | SLTU R1,R2,R3  | 功能与SLT一致，但是带符号的 |
| SLTUI| SLTUI R1,R2,R3 | 功能与SLT一致，但不带符号 |
| MOVN | MOVN R1,R2,R3  | 如果第三个寄存器的内容为负，那么复制一个寄存器的内容到另外一个寄存器 |
| MOVZ | MOVZ R1,R2,R3  | 如果第三个寄存器的内容为0，那么复制一个寄存器的内容到另外一个寄存器 |
| TRAP | ---- | 根据地址向量转入管态 |
| ERET | ---- | 从异常中返回到用户态 |
| MADD.S | ---- | 一个双精度浮点数与单精度浮点数相乘加，结果为单精度 |
| MADD.D | ---- | 一个双精度浮点数与单精度浮点数相乘加，结果为双精度 |
| MADD.PS | ---- | 两个单精度浮点数相乘加，结果为单精度 |


<h2 id="2.3">2.3 整数、乘法硬件单元及寄存器</h2>

The MIPS architects decided that integermultiplication was important enough to deserve a hardwired instruction. This was not universal in 1980s, RISCs. One alternative was to implement a multiply step that fits in the standard integer execution pipeline but mandates software routines for every nontrivial multiplication; early SPARC CPUs did just that.

Another way of avoiding the complexity of the integer multiplier would be to perform integer multiplication in the floating-point unit—a good solution used in Motorola’s short-lived 88000 family—but that would compromise the optional nature of the MIPS floating-point coprocessor.

Instead, a MIPS CPU has a special-purpose integer multiply unit, which is not quite integrated with the main pipeline. The multiply unit’s basic operation is tomultiply two register-sized values together to produce a twice-registersized result, which is stored inside the multiply unit. The instructions mfhi, mflo retrieve the result in two halves into specified general registers. 

Since multiply results are not returned so fast as to be automatically available for any subsequent instruction, the multiply result registers are and always were interlocked. An attempt to read out the results before the multiplication is complete results in the CPU being stopped until the operation completes.

The integermultiply unit will also performan integer division between values in two general-purpose registers; in this case the lo register stores the result (quotient) and the hi register stores the remainder.

You don’t get a multiply unit answer out in one clock: multiply takes 4–12 clock cycles and division 20–80 clock cycles (it depends on the implementation, and for some implementations it depends on the size of the operands). Some CPUs have fully or partially pipelined multiply units—that is, they can start a multiply operation every one or two clocks, even though the result will not arrive for four or five clocks.

MIPS32/64 includes a three-operand mul instruction, which returns the low half of themultiply result to a general-purpose register. But that instruction must stall until the operation is finished; highly tuned software will still use separate instructions to start themultiply and retrieve the results. MIPS32/64CPUs (and most other CPUs still on the market) also havemultiply-accumulate operations, where products from successive multiply operations are accumulated inside the lo/hi pair.

Integer multiply and divide operations never produce an exception: not even divide by zero (though the result you get fromthat is unpredictable).Compilers will often generate additional instructions to check for and trap on errors, particularly on divide by zero.

Instructions mthi, mtlo are defined to set up the internal registers from general-purpose registers. They are essential to restore the values of hi and lo when returning from an exception but probably not for anything else.

<h2 id="2.4">2.4 加载与存储：寻址方式</h2>

As mentioned previously, there is only one addressing mode.1 Any load or store machine instruction can be written:

    lw $1, offset($2)

You can use any registers for the destination and source. The offset is a signed, 16-bit number (and so can be anywhere between −32768 and 32767); the program address used for the load is the sum of $2 and the offset. This address mode is normally enough to pick out a particular member of a C structure (offset being the distance between the start of the structure and the member required). It implements an array indexed by a constant; it is enough to reference function variables from the stack or frame pointer and to provide a reasonable-sized global area around the gp value for static and extern variables.

The assembler provides the semblance of a simple direct addressing mode to load the values of memory variables whose address can be computed at link time.

More complex modes such as double-register or scaled index must be implemented with sequences of instructions.

<h2 id="2.5">2.5 存储器与寄存器的数据类型</h2>

MIPS CPUs can load or store between one and eight bytes in a single operation.
Naming conventions used in the documentation and to build instruction
mnemonics are as follows.

2.5.1 Integer Data Types

Byte and halfword loads come in two flavors. Sign-extending instructions lb
and lh load the value into the least significant bits of the 32-bit register but fill
the high-order bits by copying the sign bit (bit 7 of a byte, bit 15 of a halfword).
This correctly converts a signed integer value to a 32-bit signed integer, as shown
in the following chart.

| 数据类型 | 字节数 | 助记符 |
| -------- | ------ | ------ |
| dword    | 8      | ld     |
| word     | 4      | lw     |
| halfword | 2      | lh     |
| byte     | 1      | lb     |


The unsigned instructions lbu and lhu zero-extend the data; they load the value into the least significant bits of a 32-bit register and fill the high-order bits with zeros.

For example, if the byte-wide memory location whose address is in t1
contains the value 0xFE (−2, or 254 if interpreted as unsigned), then:

    lb t2, 0(t1)
    lbu t3, 0(t1)

will leave t2 holding the value 0xFFFF.FFFE (−2 as signed 32-bit value) and t3 holding the value 0x0000 00FE (254 as signed or unsigned 32-bit value).

The above description relates to MIPS machines considered as 32-bit CPUs, but many have 64-bit registers. It turns out that all partial-word loads (even unsigned ones) sign-extend into the top 32 bits; this behavior looks bizarre but is helpful, as is explained in section 2.7.3.

Subtle differences in the way shorter integers are extended to longer ones are a historical cause of C portability problems, and the modern C standards have very definite rules to remove possible ambiguity. On machines like the MIPS, which cannot do 8- or 16-bit precision arithmetic directly, expressions involving short or char variables require the compiler to insert extra instructions to make sure things overflow when they should; this is nearly always undesirable and rather inefficient.When porting code that uses small integer variables to a MIPS CPU, you should consider identifying which variables can be safely changed to int.

2.5.2 Unaligned Loads and Stores

Normal loads and stores in the MIPS architecture must be aligned; halfwords
may be loaded only from two-byte boundaries and words only from four-byte
boundaries. A load instruction with an unaligned address will produce a trap.
Because CISC architectures such as the MC680x0 and Intel x86 do handle
unaligned loads and stores, you may come across this as a problem when porting
software; in extremity, youmay even decide to install a trap handler that will
emulate the desired load operation and hide this feature fromthe application—
but that’s going to be horribly slow unless the references are very rare.

All data items declared by C code will be correctly aligned.

Where you knowin advance that you want to code a transfer froman address
whose alignment is unknown and that may turn out to be unaligned, the architecture
does allow for a two-instruction sequence (much more efficient than a
series of byte loads, shifts, and adds). The operation of the constituent instructions
is obscure and hard to grasp, but they are normally generated by the
macro-instruction ulw (unaligned load word). They’re described fully in
section 8.5.1.

A macro-instruction ulh (unaligned load half) is also provided and is synthesized
by two loads, a shift, and a bitwise “or” operation.

By default, a C compiler takes trouble to align all data correctly, but there are
occasions (e.g., when importing data from a file or sharing data with a different
CPU) when being able to handle unaligned integer data efficiently is a requirement.
Most compilers permit you to flag a data type as potentially unaligned
and will generate (reasonably efficient) special code to cope—see section 11.1.5.

2.5.3 Floating-Point Data in Memory

Loads into floating-point registers from memory move data without any interpretation—
you can load an invalid floating-point number (in fact, an arbitrary
bit pattern) and no FP error will result until you try to do arithmetic with it.

On 32-bit processors, this allows you to load single-precision values by a
load into an even-numbered floating-point register, but you can also load a
double-precision value by a macro instruction, so that on a 32-bit CPU the
assembly instruction:

    l.d $f2, 24(t1)

is expanded to two loads to consecutive registers:

    lwc1 $f2, 24(t1)
    lwc1 $f3, 28(t1)

On a 64-bit CPU, l.d is the preferred alias for the machine instruction ldc1, which does the whole job.

Any C compiler that complies with the MIPS/SGI rules aligns eight-bytelong
double-precision floating-point variables to eight-byte boundaries. The
32-bit hardware does not require this alignment, but it’s done for forward compatibility:
64-bit CPUs will trap if asked to load a double from a location that
is not eight-byte aligned.

<h2 id="2.6">2.6 汇编语言的合成指令</h2>

MIPS machine code might be rather dreary to write; although there are
excellent architectural reasons why you can’t load a 32-bit constant value into
a register with a single instruction, assembly programmers don’t want to think
about it every time. So the GNU assembler (and other good MIPS assemblers)
will synthesize instructions for you. You just write an li (load immediate)
instruction and the assembler will figure out when it needs to generate two
machine instructions.

This is obviously useful, but it can be abused. Some MIPS assemblers end
up hiding the architecture to an extent that is not really necessary. In this book,
we will try to use synthetic instructions sparingly, and we will tell you when it
happens. Moreover, in the instruction tables, we will consistently distinguish
between synthetic and machine instructions.

It ismy feeling that these features are there to help human programmers and
that serious compilers should generate instructions that are one-for-one with
machine code.3 But in an imperfect world many compilers will in fact generate
synthetic instructions.

Helpful things the assembler does include the following:

* A 32-bit load immediate: You can code a load with any value (including a
memory location that will be computed at link time), and the assembler
will break it down into two instructions to load the high and low half of
the value.

* Load from memory location: You can code a load froma memory-resident
variable. The assemblerwill normally replace this by loading a temporary
register with the high-order half of the variable’s address, followed by a
load whose displacement is the low-order half of the address. Of course,
this does not apply to variables defined inside C functions, which are
implemented either in registers or on the stack.

* Efficient access to memory variables: Some C programs contain many references
to static or extern variables, and a two-instruction sequence
to load/store any of them is expensive. Some compilation systems, with
the help of some runtime support, get around this. Certain variables
are selected at compile/assemble time (most commonly the assembler
selects variables that occupy eight or less bytes of storage) and are kept
together in a single section of memory that must end up smaller than
64 KB. The runtime system then initializes one register—$28 or gp by
convention—to point to the middle of this section.

Loads and stores to these variables can now be coded as a single
gp-relative load or store.

* More types of branch conditions: The assembler synthesizes a full set of
branches conditional on an arithmetic test between two registers.

* Simple or different forms of instructions: Unary operations such as not
and neg are produced as a nor or sub with the zero-valued register $0.
You can write two-operand forms of three-operand instructions and the
assembler will put the result back into the first-specified register.

* Hiding the branch delay slot: In normal coding the assembler will move
the instruction you wrote before the branch around into the delay slot if
it can see it is safe to do so. The assembler can’t seemuch, so it is not very
good at filling branch delays. An assembly directive .set noreorder
is available to tell the assembler that you’re in control and it must not
move instructions about.

* Hiding the load delay: Some assemblers may detect an attempt to use the
result of a load in the immediately succeeding instruction and maymove
an instruction up or back in sequence if it can.

* Unaligned transfers: The unaligned load/store instructions (ulh, ulw,
etc.) will fetch halfword and word quantities correctly, even if the target
address turns out to be unaligned.

* Other pipeline corrections: Some instructions (such as those that use the
integer multiply unit) have additional constraints on some old CPUs. If
you have such an old CPU, you may find that your assembler helps out.

In general, if you really want to correlate assembly source language (not
enclosed by a .set noreorder)with instructions stored in memory, you need
help. Use a disassembler utility.

<h2 id="2.7">2.7 MIPS I到MIPS64指令集</h2>

<h2 id="2.8">2.8 基本地址空间</h2>

<h2 id="2.9">2.9 流水线可见性</h2>
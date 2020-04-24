
本章旨在帮助读者阅读MIPS汇编代码。本文中专注于32位MIPS指令集。

本文主要的目标读者是习惯于C语言编程，但是，有时候不得不读懂一些汇编代码甚至做一些小范围的改动的开发者，比如操作系统移植时启动代码start.S文件的阅读与修改。如果想要深入研究汇编程序如何编写，请参考所使用的MIPS工具链的说明文档。

阅读MIPS汇编代码，不仅仅需要熟悉各个机器指令，因为它还包括许多MACRO，这些宏由GNU工具链识别，将其展开成真正的机器指令。这些宏的存在是为了更方便地编写汇编程序。另外，MIPS汇编器还提供了许多伪指令或伪操作，用来管理代码布局、控制指令序列以及实施优化等。

通常，编程人员会将汇编代码以更具有可读性的源文件（后缀`.S`）传递给预处理器，由预处理器进行宏展开、别名替换等等操作，然后形成真正的预处理后的汇编文件（后缀`.s`表示）。

<h2 id="9.1">9.1 简单示例</h2>

下面是C函数库中的strcmp()函数实现。通过这个示例，我们将展示一些基本的汇编语法和手动优化代码的内容。

    int strcmp (char *a0, char *a1)
    {
        char t0, t1;

        while (1) {
            t0 = a0[0];
            a0 += 1;
            t1 = a1[0];
            a1 += 1;
            if (t0 == 0)
                break;
            if (t0 != t1)
                break;
        }

        return (t0 - t1);
    }

在这个初始版的strcmp实现函数中，每次迭代过程需要执行2个if语句和2个读取内存操作（访问数组）。这每一个操作会产生一个延时点，比如说读取内存时，其它指令无法读取内存，但是可以在CPU上执行。而在这个while循环中，却没有足够的不需要分支预测和存取内存的操作填充这个时间段的CPU执行。所以，这其实没有最大化CPU的执行效率。而且上面的代码，每次迭代过程只能比较一次字符串。

下面，我们对上面的代码进行简单的改进。最大的变化就是，单次迭代可以执行2次比较，而且还把其中的一次存取操作放到循环的最后：

    int strcmp (char *a0, char *a1) 
    {
        char t0, t1, t2;
    
        /* 第1次迭代的读取a0操作放在循环之外 */
        t0 = a0[0];
        while (1) {
            /* 第1个字节 */
            t1 = a1[0];
            if (t0 == 0)
                break;
            a0 += 2;
            if (t0 != t1)
                break;
            /* 第2个字节 */
            t2 = a0[-1];    /* 此处的a0已被增加 */
            t1 = a1[1];     /* 不再增加a1 */
            if (t2 == 0)
                /* 汇编代码中的标签t21处 */
                return t2-t1;
            a1 += 2;
            if (t1 != t2)
                /* 汇编代码中的标签t21处 */
                return t2-t1;
            t0 = a0[0];
        }
        /* 汇编代码中的标签t01处 */
        return t0-t1;
    }

将上面的代码展开为MIPS汇编代码：

    #include <mips/asm.h>
    #include <mips/regdef.h>
    
    LEAF(strcmp)
        .set noreorder
        lbu t0, 0(a0)
    1:  
        lbu t1, 0(a1)
        beq t0, zero,.t01   # 读取造成延时
        addu a0, a0, 2      # 分支造成延时
        bne t0, t1, .t01
        lbu t2, -1(a0)      # 分支造成延时
        lbu t1, 1(a1)       # 读取造成延时
        beq t2, zero,.t21
        addu a1, a1, 2      # 分支造成延时
        beq t2, t1, 1b
        lbu t0, 0(a0)       # 分支造成延时
    .t21: j ra
        subu v0, t2, t1     # 分支造成延时
    .t01: j ra
        subu v0, t0, t1     # 分支造成延时
    .set reorder
    END(strcmp)

我们先来分析上面汇编代码的每一部分的作用：

* **#include:** 

    它的作用和其它高级语言的#include是一样的，可以将包含的文件在本文件中展开，进行文本替换。

    - mips/asm.h 文件定义了宏LEAF和END；
    - mips/regdef.h 文件定义了MIPS架构32个通用寄存器$0-$31的别称，比如上面的t0和a0等。

* **宏LEAF：** 

    定义如下：

        #define LEAF(name) \
            .text; \
            .globl name; \
            .ent name; \
        name:

    宏LEAF与下面的END一起使用，定义相当于`叶子函数`的子程序，供其它汇编程序调用。我们知道`非叶子函数`需要做更多工作，比如保存变量，保存返回地址等等。除非是有特殊目的，一般不用在汇编程序中实现这样的函数，用C语言写更好。LEAF包含的内容：

    - .text 
    
        将后面的代码添加到目标文件用`.txt`标记的文本段。

    - .globl
    
        将name标记为global全局符号。供整个程序调用。

    - .ent

        没有实际作用，仅仅是告诉编译器从此处开始。

    - name

        标签，汇编子程序真正开始的地方。

* **宏END：** 

    定义如下：

        #define END(name) \
            .size name,.-name; \
            .end name

    - .size
    
        出现在符号表中的大小。

    - .end

        标记结束


* **.set伪指令：**

    设置汇编器的工作方式。默认情况下，汇编器尝试填充分支指令和存取指令造成的空闲时间，通过重新排列指令。也就是说，大部分时候都不需要关心汇编代码中的指令执行顺序所带来的性能问题。
    
    - `.set noreorder`和`.set reorder`：告知汇编器是否重新对指令进行顺序进行排序。
    
    - `.set push`和`.set pop`：分别是保存所有设置、弹出所有设置。

    - `.set at`和`.set noat`：是否允许汇编程序中使用at寄存器。

    - `.set mipsn`：n，是一个从0到5的数字，或是数字32或64。指定使用的指令集。
        + `.set mips0`，使用原本的指令集；
        + `.set mips3`，使用`MIPS IV`中的指令（64位兼容32位）；
        + `.set mips32`，使用32位指令集；
        + `.set mips64`，使用64位指令集；

* **标签1：**

    同其它汇编语言一样，就是一个程序调转的地址别名。

    - 1f：后面的f表示forward，代表向前跳转；
    - 1b：后面的b表示back，代表向后跳转。

* **指令：** 

    真正的可执行指令。因为使用了伪指令`.set noreorder`，所以分支、存取指令导致的延时就会被加入进来。

<h2 id="9.2">9.2 语法概述</h2>

上面我们对汇编代码已经有了一些感官上的认识，下面我们就系统地学习一下汇编的语法。

<h3 id="9.2.1">9.2.1 代码风格，分隔符和标识符</h3>

我们都比较熟悉C代码，基本规则差不多，只需要注意一些特别的地方即可。

* 必须有行结束符，每行可以有多个指令语句，只要使用分隔符`;`分割即可。

* 行尾`#`注释。如果使用C预处理器，也可以使用`/*注释内容*/`这种C风格的注释。

标签和变量的标识符可以是C语言中任何合法的字符，甚至可以包含`$`和`.`。

* 常用1-99的数字标记LABEL。

* 强烈建议使用MIPS惯用寄存器命名。但你需要包含头文件`mips/regdef.h`；如果选择直接使用通用目的寄存器名称，则使用$3这之类的命名方式。通用寄存器的编号从0-31。

* 常量和字符串可以按照C风格使用。

<h2 id="9.3">9.3 指令的通用规则</h2>

MIPS汇编器对一些常用的实现作了处理，形成了自己的伪指令。通俗地讲，就是程序开发者以更友好的方式写代码，汇编器将其拆解成多条具体的机器指令。

<h3 id="9.3.1">9.3.1 算术、逻辑指令</h3>

MIPS架构算术、逻辑指令是三目操作运算指令，也就是说，它们具有2个输入和一个输出。例如：表达式`d = s + t`写成汇编形式为`addu d,s,t`。

但是复杂指令集的风格一般是2个操作数。为此，汇编器将目标寄存器d作为源寄存器s使用。如果编写代码时，省略s，写成`addu d,t`，汇编器生成最终的机器码时，会将其展开为`addu d,d,t`。

像neg、not等单寄存器指令都是一个或者多个三寄存器指令合成的。汇编器希望这些指令最多使用2个寄存器，所以，

    negu d,s    =>  subu d,zero,s
    not d       =>  nor d,zero,d      

最常用的寄存器到寄存器操作是`move d,s`。汇编器将这种最常见的操作转换为`or d,zero,s`。

<h3 id="9.3.2">9.3.2 立即数运算</h3>

在汇编或者机器指令中，编入指令中的常数称为立即数。许多算术和逻辑运算使用16位立即数替换t。但是，有时候16位立即数不能满足我们的要求，我们需要对其进行扩展，扩展方式有两种：符号扩展或零扩展到32位。怎样选择取决于操作，通常，算术操作使用符号扩展，逻辑操作使用零扩展。

虽然使用立即数的操作和不使用立即数的操作指令不相同，比如addu和addiu(立即数)。但是，编程人员无需关注这些细节，汇编器会自动检测是否为立即数，从而选择正确的机器指令：

    addu $2, $4, 64 => addiu $2, $4, 64

但是，如果立即数太大，16位立即数无法满足，则需要汇编器额外帮助。它自动把常数加载到临时寄存器`at/$1`中，然后使用它执行操作：

    addu $4, 0x12345 => li at, 0x12345
                        addu $4, $4, at

`li`指令，载入立即数专用伪指令，所以它不是实际的机器指令。

当32位的立即数值位于±32K之内时，汇编器使用单条指令addiu加上$0寄存器实现li指令语句；如果立即数的16-31位都为0，则使用ori指令实现li指令；如果立即数的0-15位都为0，则可以直接使用lui指令将数据搬运到寄存器中。其它情况，根据需要选择使用lui还是ori指令来实现。

    li $3, -5       =>  addiu $3, $0, -5
    li $4, 0x8000   =>  ori $4, $0, 0x8000
    li $5, 0x120000 =>  lui $5, 0x12
    li $6, 0x12345  =>  lui $6, 0x1
                        ori $6, $6, 0x2345

但是，将伪汇编指令展开为多条指令时，如果此时使用了`.set noreorder`伪指令，则会非常麻烦。如果在一个分支延迟槽中使用多指令宏，汇编器会发出警告。

<h3 id="9.3.3">9.3.3 64/32位指令</h3>

我们在前面看到，MIPS体系结构扩展到64位(第2.7.3节)时非常注意确保MIPS32程序的行为保持不变，即使它们在MIPS64机器上运行;在MIPS64机器中，MIPS32指令的执行总是将任何GP寄存器的32位上半部分设置为所有1或所有0(反映第31位的值)。

MIPS64架构完全兼容MIPS32架构，执行MIPS32指令时，总是使用通用寄存器的高32位，也就是偶数号寄存器。

许多32位指令可以直接在64位系统上被使用，比如按位逻辑操作，但是，算术操作却不能。加减乘除、移位都需要实现新的指令。新指令就是在旧指令的基础上前缀d（表示double）来实现的。比如，旧加法指令addu，新指令就是daddu。

<h2 id="9.4">9.4 寻址模式</h2>

As noted previously, the hardware supports only one addressing mode, base reg+offset, where offset is in the range −32768 to 32767. However, the assembler will synthesize code to access data at addresses specified in various other ways:

1. Direct: A data label or external variable name supplied by you
2. Direct+index: An offset from a labeled location specified with a register
3. Constant: Just a large number, interpreted as an absolute 32-bit address
4. Register indirect: Just register+offset with an offset of zero

When these methods are combined with the assembler’s willingness to do simple constant arithmetic at compile time and the use of a macro processor, you are able to do most of what you might want. Here are some examples:

    lw $2, ($3)     =>  lw $2, 0($3)
    lw $2, 8+4($3)  =>  lw $2, 12($3)
    lw $2, addr     =>  lui at, %hi(addr)
                        lw $2, %lo(addr)(at)
    sw $2, addr($3) =>  lui at, %hi(addr)
                        addu at, at, $3
                        sw $2, %lo(addr)(at)

The symbol addr in the above examples can be any of the following:

* A relocatable symbol—the name of a label or variable (whether in this module or elsewhere)

* A relocatable symbol ± a constant expression (the assembler/linker can handle this at system build time)

* A 32-bit constant expression (e.g., the absolute address of a device register)

The constructs %hi() and %lo() represent the high and low 16 bits of the address. This is not quite the straightforward division into low and high halfwords that it looks, because the 16-bit offset field of an lw is interpreted as signed. So if the addr value is such that bit 15 is a 1, then the %lo(addr) value will act as negative, and we need to increment %hi(addr) to compensate:

| addr        | %hi(addr) | %lo(addr) |
| ----------- | --------- | --------- |
| 0x1234.5678 | 0x1234    | 0x5678    |
| 0x1000.8000 | 0x1001    | 0x8000    |

The la (load address) macro instruction provides a similar service for addresses to that provided for integer constants by li:

    la $2, 4($3)    =>  addiu $2, $3, 4
    la $2, addr     =>  lui at, %hi(addr)
                        addiu $2, at, %lo(addr)
    la $2, addr($3) =>  lui at, %hi(addr)
                        addiu $2, at, %lo(addr)
                        addu $2, $2, $3

In principle, la could avoid messing around with apparently negative %lo( ) values by using an ori instruction. But load/store instructions have a signed 16-bit address offset, and as a result the linker is already equipped with the ability to fix up addresses into two parts that can be added correctly. So la uses the add instruction to avoid the linker having to understand two different fix-up types.

9.4.1 Gp-Relative Addressing

A consequence of the way the MIPS instruction set is crammed into 32-bit operations is that accesses to compiled-in locations usually require at least two instructions, for example:

    lw $2, addr     =>  lui at, %hi(addr)
                        lw $2, %lo(addr)(at)

In programs that make a lot of use of global or static data, this can make the compiled code significantly fatter and slower.

Early MIPS compilers introduced a fix for this, which has been carried into most MIPS toolchains. It’s usually called gp-relative addressing. This technique requires the cooperation of the compiler, assembler, linker, and start-up code to pool all of the “small” variables and constants into a single memory region; then it sets register $28 (known as the global pointer or gp register) to point to the middle of this region. (The linker creates a special symbol, gp, whose address is the middle of this region. The address of gpmust then be loaded into the gp register by the start-up code, before any load or store instructions are used.) So long as all the variables together take up no more than 64 KB of space, all the data items are now within 32 KB of the midpoint, so a load turns into:

    lw $2, addr     =>  lw $2, addr - _gp(at)

The problem is that the compiler and assembler must decide which variables can be accessed via gp at the time the individual modules are compiled. The usual test is to include all objects of less than a certain size (eight bytes is the usual default). This limit can usually be controlled by the “-G n” compiler/assembler option; specifying “-G 0”will switch this optimization off altogether.

While it is a useful trick, there are some pitfalls to watch out for. You must take special care when writing assembly code to declare global data items consistently and correctly:

* Writable, initialized small data items must be put explicitly into the
    
    .sdata section.

* Global common datamust be consistently declared with the correct size:

    .comm smallobj, 4
    .comm bigobj, 100

* Small external variables should also be explicitly declared:

    .extern smallext, 4

Most assemblers will not act on a declaration unless it precedes the use  of the variable.

In C, you must declare global variables correctly in all modules that use them. For external arrays, either omit the size, like this:

    extern int extarray[];

or give the correct size:

    int cmnarray[NARRAY];

Sometimes the way programs are run means this method can’t be used. Some real-time operating systems (and many PROM monitors) are built with a separately linked chunk of code implementing the kernel, and applications invoke kernel functions with long-range subroutine calls. There’s no costeffective method by which you could switch back and forth between the two different values of gp that will be used by the application and OS, respectively. In this case either the applications or the OS (but not necessarily both)must be built with -G 0.

When the -G 0 option has been used for compilation of any set of modules, it is usually essential that all libraries linked in with them should be compiled that way. If the linker is confronted with modules that disagree on whether a named variable should be put in the small or regular data sections, it’s likely to give you peculiar and unhelpful error messages.

<h2 id="9.5">9.5 Object File and Memory Layout</h2>


This chapter concludes with a brief look at the way programs are typically laid out in system memory and notes some important points about the relationship between the memory layout and the object files produced by the toolchain. It’s very useful to have a basic understanding of the way your code should appear after it’s loaded into the system’s memory, especially if you’re going to face the task of getting MIPS code to run for the first time on newly developed system hardware.

The conventional code and data sections defined by MIPS conventions are illustrated (for ROMable programs) in Figure 9.1.

Within an assembly program the sections are selected as described in the groupings that follow.

    .text, .rdata, and .data

Simply put the appropriate section name before the data or instructions, as shown in this example:

        .rdata
    msg:.asciiz "Hello world!\n"


FIGURE 9.1 ROMable program’s object code segments and typical memory layout.

        .data
    table:
        .word 1
        .word 2
        .word 3
        .text
    func:sub sp, 64
        ...

* .lit4 and .lit8 Sections: Floating-Point Implicit Constants

are given as arguments to the li.s or li.d macro instructions. Some assemblers and linkers will save space by combining identical constants. .lit4 and .lit8 may be included in the “small data” region if the application is built to use gp-relative addressing.

* .bss, .comm, and .lcomm Data

This section name is also not used as a directive. It is used to collect all static or global uninitialized data declared in C modules. It’s a feature of C thatmultiple same-named definitions in different modules are acceptable so long as not more than one of them is initialized, and the .bss section is often used for data that is not initialized anywhere. FORTRAN programmers would recognize this as what is called common data—that’s where the name of the directive comes from.

You always have to specify a size for the data (in bytes).When the programis linked, the item will get enough space for the largest size. If any module declares it in an initialized data section, all the sizes are used and that definition is used:

    .comm dbgflag, 4    # global common variable, 4 bytes
    .lcomm sum, 4       # local common variable, 8 bytes
    .lcomm array, 100   # local common variable, 100 bytes


“Uninitialized” is actually a misnomer. In C, static or global variables that are not explicitly initialized should be set to zero before the program starts—a job for the operating system or start-up code.

* .sdata, Small Data, and .sbss

These sections are used as alternatives to the .data and .bss sections above by toolchains that want to separate out smaller data objects. Toolchains for MIPS processors do this because the resulting small-object section is compact enough to allow an efficient access mechanism that relies on maintaining a data pointer in a reserved register gp, as described in section 9.4.1.

Note that the .sbss is not a legal directive; the toolchain allocates a data item to the .sbss section if the item is declared with .comm or .lcomm and is of size smaller than the -G threshold value fed to the assembly program.

The implicit-constant sections .lit4 and .lit8 may be included in the small data region, according to the threshold setting.

When gp-relative addressing is used, gp will be initialized to point somewhere close to the midpoint of the “small data” region.

* .section


Start an arbitrarily named section and supply control flags (which are object code specific and probably toolkit specific). See your toolkit manuals, and always use the specific section name directives for the common sections.


9.5.1 Practical Program Layout, Including Stack and Heap

The programlayout illustrated in Figure 9.1 is suitable in most practical systems in which the code is stored in ROMand runs on a bare CPU (that is, without the services of any intermediate software such as an operating system). The readonly sections are likely to be located in an area of memory remote from the read/write sections.

The stack and heap are significant as areas of the system’s address space, but it’s important to understand that they’re not known to the assembler or linker in the same way as, for example, the .text or .data sections. Typically, the stack and the heap are initialized and maintained by the runtime system. The stack is defined by setting the sp register to the top of available memory (aligned to an eight-byte boundary). The heap is defined by a global pointer variable used by functions such as malloc() functions; it’s often initialized to the end symbol, which the linker has calculated as the highest location used by declared variables.

* Special Symbols

Figure 9.1 also shows a number of special symbols that are automatically defined by the linker to allow programs to discover the start and end of their various sections. They are descended from conventions that grew up in UNIX-like OSs, and some are peculiar to the MIPS environment. Your toolkit might or might not define all of them; those marked with a √ in the following list are pretty certain to be there:

| Symbol  | Standard? | Value |
| ------- | --------- | ----- |
| ftext   |           | Start of text (code) segment |
| etext   | √         | End of text (code) segment |
| fdata   |           | Start of initialized data segment |
| edata   | √         | End of initialized data segment |
| fbss    |           | Start of uninitialized data segment |
| end     | √         | End of uninitialized data segment |

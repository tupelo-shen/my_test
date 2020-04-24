
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

MIPS架构的寻址模式非常简单，就是寄存器+偏移量的方式，偏移量的范围是−32768~32767（也就是16位的立即数）。如果编程人员想要使用其它寻址方式，汇编器将会使用寄存器+偏移量的方式进行组合实现。这些其它寻址方式如下所示：

1. 直接寻址：也就是访问某个标签，其指向某个数据或者变量。
2. 直接寻址+索引：使用寄存器指定偏移量。
3. 常量寻址：直接作为32位地址使用。
4. 寄存器间接寻址：寄存器+偏移量，但是此处的偏移量等于0。

我们直接看下面的示例：

    #源码实现       =>  MIPS汇编器实现
    lw $2, ($3)     =>  lw $2, 0($3)
    lw $2, 8+4($3)  =>  lw $2, 12($3)
    lw $2, addr     =>  lui at, %hi(addr)
                        lw $2, %lo(addr)(at)
    sw $2, addr($3) =>  lui at, %hi(addr)
                        addu at, at, $3
                        sw $2, %lo(addr)(at)

符号addr可以是下面任何一种：

* 一个可重定位的符号，比如标签和变量名称；

* 一个可重定位的符号+常量表达式（汇编器或链接器会解析）；

* 32位常量表达式（比如设备配置寄存器的绝对地址）。

`%hi()`和`%lo()`代表地址的高16位和低16位。它并不是直接把一个32位的WORD分成2个16位的半字。因为lw指令把16位的偏移量解释为带符号的立即数。也就是说，如果bit15等于1的话，%lo(addr)的值就会是负值。所以，我们需要增加%hi(addr)进行补偿，如下所示：

| addr        | %hi(addr) | %lo(addr) |
| ----------- | --------- | --------- |
| 0x1234.5678 | 0x1234    | 0x5678    |
| 0x1000.8000 | 0x1001    | 0x8000    |

`la`宏指令实现加载地址，与`li`宏指令及其类似，只是一个加载地址，一个加载立即数：

    la $2, 4($3)    =>  addiu $2, $3, 4
    la $2, addr     =>  lui at, %hi(addr)
                        addiu $2, at, %lo(addr)
    la $2, addr($3) =>  lui at, %hi(addr)
                        addiu $2, at, %lo(addr)
                        addu $2, $2, $3

原则上，`la`指令可以通过使用`ori`指令避免`%lo()`为负值的时候。但是`load/store`指令使用一个带符号位的16位地址偏移量（这样在访问内存的时候更方便），导致linker链接器已经使用了这种修复地址的技术。所以，`la`指令为了避免linker需要理解两种不同的修复地址的方法，而选择使用add指令实现。

9.4.1 gp相对寻址

MIPS指令集使用32操作数的结果就是，访问某个地址通常需要两条指令实现：

    lw $2, addr     =>  lui at, %hi(addr)
                        lw $2, %lo(addr)(at)

如果在程序中，大量使用global或static数据，会使编译后的代码非常臃肿，执行效率低下。

早期的MIPS编译器引入一种小技巧修复这个问题，称为`gp相对寻址`（gp->global pointer）。这个技术需要编译器、汇编器、链接器和启动代码的配合才能实现。启动代码start_up.S中把所有较小的变量和常数存入一段内存区域；然后设置寄存器$28（被称为gp指针或gp寄存器）指向这段内存区域的中间位置（链接器会创建一个特殊的符号gp，指向该内存区域的中间位置。启动代码执行load或store指令之前，必须把gp的值加载到gp寄存器中）。但是要求所有的变量所占的空间不超过64KB，也就是上下各32KB。现在，访问某个变量的指令就变成了下面这样：

    lw $2, addr     =>  lw $2, addr - _gp(at)

可以看出，上面的实现，最终只会生成一条机器指令。显然，这可以节省代码量。但是，这里存在的问题是在编译各个模块的时候，编译器和汇编器必须决定哪些变量可以通过gp访问。通常要求所包含的对象小于一定的字节数（默认是8个字节）。这个限制可以通过编译汇编选项`-G n`进行控制，如果n等于0，则是将这个优化选项关闭。

虽然这是一个非常有用的小技巧，但是也有许多小陷阱需要留意。下面是一些避免陷阱的一些措施：

* 可写的、已初始化过的数据项显式地存放到`.sdata`数据段。

* 全局通用数据必须正确声明大小：

    .comm smallobj, 4
    .comm bigobj, 100

* 对外可见的变量必须使用`.extern`进行声明：

    .extern smallext, 4

    大部分的汇编器都是在使用变量之前处理声明，除此之外，不予理会。

程序的运行方式决定了这种方法是否可行。许多实时操作系统使用一段独立的代码实现内核，应用程序通过大范围的子程序调用接口调用内核函数。没有一个有效的方法，可以在内核代码和应用程序代码的gp之间来回切换。这种情况下，应用程序或者OS必须至少一个使用`-G 0`进行编译。

如果使用了`-G 0`选项编译了某个模块，那么与该模块相关的所有链接库也都得需要使用`-G 0`选项进行编译。否则，会给出一些稀奇古怪的错误信息。

<h2 id="9.5">9.5 目标文件和内存布局</h2>

本段我们主要对MIPS架构常见的内存布局做个简要的介绍，也对内存布局和目标文件之间的关系提出了几个重要的点。了解代码加载到系统内存中的方式对我们很有帮助，尤其是，代码第一次在系统硬件上运行时。

MIPS架构常见的内存布局如图9-1所示。汇编程序中，使用下面这些标记各个段：

    .text, .rdata, 和 .data

应该在数据和指令之前添加正确的段标识符，比如：

        .rdata
    msg:.asciiz "Hello world!\n"
        .data
    table:
        .word 1
        .word 2
        .word 3
        .text
    func:sub sp, 64
        ...


<img src="">

图9-1 程序的各个目标代码段和内存布局

* `.lit4`和`.lit8`段：浮点常数数据段

    主要是传递给li.s或li.d宏指令的参数。有些汇编器和链接器会组合相同的常数以节省空间。如果使能了`-G n`编译选项，也有可能使用gp相对寻址，将`.lit4`和`.lit8`浮点常数段存放到全局的`小数据`那个特殊的数据段中。
    
* `.bss`、`.comm`和`.lcomm`数据段
    
    未初始化数据段。用来存储C代码中所有的静态和全局未初始化的数据。FORTRAN语言常常成为common data，这也是.comm的由来。

* `.sdata`、小数据段和`.sbss`

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

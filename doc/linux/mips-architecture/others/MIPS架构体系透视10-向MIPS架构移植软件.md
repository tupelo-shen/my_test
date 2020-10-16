[TOC]

> 站在巨人的肩膀上，才能看得更远。
> 
> If I have seen further, it is by standing on the shoulders of giants.
> 
<p align="right">牛顿</p>

科学巨匠尚且如此，何况芸芸众生呢。我们不可能每个软件都从头开始搞起。大部分时候，我们都是利用已有的软件，不管是应用软件，还是操作系统。所以，对于MIPS架构来说，完全可以把在其它架构上运行的软件拿来为其所用。

但是，这是一个说简单也简单，说复杂也复杂的工作。为什么这么说呢？如果你要采用的软件，其可移植性比较好的话，可能只需要使用支持MIPS架构的编译器重新编译以便就可以了；如果程序只是为特定的硬件平台编写的话（大部分嵌入式软件都是如此），可能处处是坑。比如说`Linux`系统，在编写应用或者系统软件的时候，一般都会考虑可移植性。所以说，基于Linux的软件一般都可以直接编译使用。但是，像现在流行的一些实时操作系统，比如、`μC/OS`、`Free-RTOS`、`RT-Thread`或其它一些基于微内核的系统，它们的程序一般不通用，需要修改才能在其它平台上运行。

而且，越往底层越难移植，几乎所有嵌入式系统上的驱动程序都不能直接使用。而且，嵌入式系统软件通常好几年才会发生一次重大设计更新，所以，如果坚持考虑软硬件上的接口兼容并不合理，尤其是考虑到成本效益的时候。

本文就是总结一些在移植代码或者编写代码时，应该需要特别关注的一些点。

# 1 MIPS架构移植软件时常见的问题

以下是一些比较常见的问题：

1. 大小端 

    计算机的世界分为大端（`big-endian`）和小端（`little-endian`）两个阵营。为了二者兼容，MIPS架构一般都可以配置到底使用大端还是小端模式。所以，我们应该彻底理解这个问题，不要在这个问题上栽跟头。

2. 内存布局和对齐 

    大部分时候，我们可以假定C声明的数据结构在内存中的布局是不可移植的。比如，使用C的结构体表示从输入文件或者网络上接收的数据的时候。还有，对于指针或者union型数据，通过不同方法引用的时候，也会存在风险。但是，内存布局海域一些其它的一些约定有关（比如寄存器的使用，参数传递和堆栈等）。

3. 需要显式管理Cache

    对于嵌入式系统来说，大部分时候采用的都是微处理器，可能并没有实现Cache硬件。但是，随着半导体技术的发展，现在的高端工业处理器一般都带有Cache，只是对于系统软件来说是不可见的而已（比如大部分处理器把Cahce可能带有的副作用都由硬件进行处理，软件不需要管理）。但是，大部分MIPS架构的CPU为了保持硬件的简单，而将一些Cache的副作用暴漏给软件，需要软件进行处理。关于这部分内容，我们后面会进行阐述。

4. 内存访问顺序 

    在大部分的嵌入式或者消费电子产品中，一般都挂载了许多子系统，这些子系统一般通过一条总线，比如PCIe总线、AHB总线、APB总线等进行通信。虽然方便了我们对系统进行扩展，但是也带来了不可预知的问题。比如，CPU和I/O设备之间的信息需要缓存处理，招致不可见的延时；或者它们被拆分成几个数据流，扔到总线上，但是对于到达目的地的顺序却没有保障。关于这部分内容，我们后面会进行阐述。

5. 编程语言
    
    对于语言，当然大部分时候使用C语言了。但是，对于MIPS架构来说，有些事情可能使用汇编语言编写更好。讲解这部分内容的时候，主要涉及inline汇编、内存映射寄存器和MIPS架构可能出现的各种缺陷。

# 2 字节序：WORD、BYTE和BIT

The word endianness was introduced to computer science by Danny Cohen (Cohen 1980). In an article of rare humor and readability, Cohen observed that computer architectures had divided up into two camps, based on an arbitrary choice of the way in which byte addressing and integer definitions are related in communications systems.

`WORD`最早是由`Danny Cohen`在1980年引入计算机科学的。在他的文章中，以其独有的幽默和智慧指出，通信系统分为两大阵营，分别是字节寻址访问和整数寻址访问。

In Jonathan Swift’s Gulliver’s Travels, the “little-endians” and “big-endians” fought a war over the correct end at which to start eating a boiled egg. Swift was satirizing 18th-century religious disputes, and neither of his sides can see that their difference is entirely arbitrary. Cohen’s joke was appreciated, and the word has stuck. The problem is not just relevant to communications; it has implications for portability too.

在乔纳森·斯威夫特（Jonathan Swift）的《格列佛游记》（Gulliver’s Travels）中，`little-endians`派和`big-endians`派就如何吃一个煮熟的鸡蛋展开了一场战争。斯威夫特讽刺的是18世纪的宗教争端问题，双方都不知道他们的分歧是完全武断的。科恩的笑话很受欢迎，这个词也就流传了下来。这个问题不仅仅体现在通信上，对于代码的可移植性也有影响。

Computer programs are always dealing with sequence and order of different types of data: iterating in order over the characters in a string, the words in an array, or the bits in a binary representation. C programmers live with a pervasive assumption that all these variables are stored in a memory that is itself visible as a sequence of bytes—memcpy() will copy any data type. And C’s I/O system models all I/O operations as bytes; you can also read() and write() any chunk of memory containing any data type.

计算机程序总是在处理不同类型的数据序列：迭代字符串中的字符，数组中的WORD类型元素，以及二进制表示的BIT位。C程序员普遍认为，所有这些变量都存储在内存中，而内存本身是作为字节序列可见的-比如，memcpy()函数能够复制任何数据，不论什么数据类型。而且，使用C语言编写的I/O系统也将I/O操作以字节进行建模，你才能够使用read()和write()之类的函数读写包含任何数据类型的内存块。

So one computer can write out some data, and another computer can read it; suddenly, we’re interested in whether the second computer can understand what the first one wrote.

这样，一个计算机写数据，另一个计算机读数据。那么，我们不禁想，第二台计算机是如何理解第一台计算所写的数据的呢？

We understand that we need to be careful with padding and alignment (details in section 11.1). And it’s probably too much to expect that complex data types like floating-point numbers will always transfer intact. But we’d hope at least to see simple twos complement integers coming across[^1] OK; the curse[^2] of endianness is that they don’t. The 32-bit integer whose hexadecimal value was written as 0x1234.5678 quite often reads in as 0x7856.3412—it’s been “byte-swapped.” To understand why, let’s go back a bit.

另外，我们不止一次地被提醒，要小心数据填充和对齐。因为这对于数据搬运会产生很大的影响。比如说，因为填充的原因，想要完整准确地传递float型数据就变得很难，所以，浮点数据存在精度问题。但是，我们期望至少能够正确表述整形数据，而"字节序"就是个拦路虎。比如说，一个32位整型数，用16进制进行表示为`0x12345678`，而读进来却为`0x78563412`，发生了字节交换。想要理解为什么，我们需要追溯一下字节序的发展历史。

[^1]: come across，偶然相遇，无意中出现，讲的清楚明白，给人...印象；
[^2]: curse，诅咒，咒骂；

## 2.1 位、字节、字和整形

A 32-bit binary integer is represented by a sequence of bits, with each bit having a different significance. The least significant bit is “ones,” then “twos,” then “fours”—just as a decimal representation is “ones,” “tens,” and “hundreds.” When your memory is byte-addressable, your 32-bit integer occupies four bytes. There are two reasonable choices about how the integer and bytewise view tie up: Some computers put the least significant (LS) bits “first” (that is, in lower addressed memory bytes) and some put the most significant (MS) bit first—and Cohen called them little-endian and big-endian, respectively. When I first got to know about computers in 1976, DEC’s minicomputers were little-endian and IBM mainframes were big-endian; neither camp was about to give way.

我们知道一个32位的int型数据，是由32个比特位组成的，它们每一位都有自己的意义，就像我们熟悉的10进制那样，每一位分别表示`个`、`十`、`百`、`千`、...以此类推，对于二进制，bit0代表1，bit1代表2，bit3代表4，bit4代表8，...。对于一个可以按字节访问的内存来说，32位整数占据4个字节。如何从比特位的视角表述整形数，有两种选择：一派，将低有效位（LS）放在前，也就是存储在内存的低地址里；而另一派，将高有效位（`MS`）放在前。科恩将其分别称为小端和大端。最早的时候，DEC的微型计算机是小端，而IBM的大型机是大端。彼时，两个阵营互不妥协。

It’s worth stressing that the curse of choice only appeared once you could address bytes. Pioneering computers through to the late 1960s were generally organized around a single word size: Instructions, integers, and memory width were all the same word size. Such a computer has no endianness: It has word order in memory and bit order inside words, and those are unrelated.

有一个细节需要特殊提一下，大小端字节序的问题只有能够按字节访问的时候才会有。1960年代之前的电脑都是按照WORD大小进行组织：包括指令，整型数和内存宽度都是WORD大小。所以，不存在字节序的大小端问题。

Just as with opening a boiled egg, both camps have good arguments.

We’re used to writing decimals with the most significant digits to the left, and (reading left to right as usual) we say numbers that way: Shakespeare might have said “four and twenty,” but we say “twenty-four.” So if you write down numbers, it’s natural to put the most significant bits first. Bytes first appeared as a convenient way of packing characters into words, before memory was byte addressable. A 1970s vintage IBM programmer had spent most of his or her career poring over vast dump listings, and each set of characters represented a word, which was a number. Little-endian numbers look ridiculous. They were instinctive big-endians. But with numbers written MS to the left and byte addresses increasing in the same direction, it would have been inconsistent to have numbered the bits from right to left: So IBM labeled the MS bit of a word bit 0. Their world looked like Figure 10.1.

我们再读写10进制数据的时候，习惯于从左到右，高有效位在左，低有效位在右。BYTE最早引入计算机，是为了方便将CHAR型字符打包成WORD，然后进行数据的交互。1970年代，一位IBM的老工程师花费了大量的时间，研究大量的内存dump列表，每个WORD大小的数据代表一组字符。这样看起来，使用小端字节序没有必要。大端字节序更有利于使用和阅读。但是，将数字的高有效位写在左端，字节顺序也是自左向右增加，这样和从右到左对bit位进行编号的行为不一致。于是，IBM将一个高有效位标记为bit0。看起来如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_1.PNG">

But it’s also natural to number the bits according to their arithmetic significance within integer data types—that is, to assign bit number n to the position that has arithmetic significance 2n. It’s then consistent to store bits 0–7 in byte 0, and you’ve become a little-endian. Having words appear backward in dumps is a shame, but the little-endian view made particular sense to people who’d gotten used to thinking of memory as a big array of bytes. Intel, in particular, is little-endian. So its words, bytes, and bits look like Figure 10.2.

但是，根据整型数据类型的算术意义对bit位进行编号更自然，也就是说，标记为`N`的bit位，其算术意义就是`2^N`。这样，就可以把bit0-7存储在`字节0`中。显然，这种方式就变成了小端模式。显然，这种方式不利于阅读，但是对于将内存看成是一个字节型的大数组的人来说，就会非常有意义。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_2.PNG">

You’ll notice that these diagrams have exactly the same contents: only the MS/LS are interchanged, as well as the order of the fields. IBM big-endians see words broken into bytes, while little-endians see bytes built into words. Both these systems seemed unarguably right to different people: There’s lots of merit in both, but you have to choose.

通过上面的讨论，可以看出，两幅图中，内容都是相同的，只是最高有效位（MS）和最低有效位（LS）进行了互换，当然，bit位的顺序也发生了互换。IBM主导的大端模式，看到的是被分割成字节的WORD；而Intel主导的小端模式看到的是构建WORD的字节序列。毋庸置疑的是，对于不同的人群，它们都非常有用。它们都有自己的优点，就看你怎么选择了。

But let’s get back to our observation about the problem above. Our mangled word started as 0x1234.5678, which is 00010010 00110100 01010110 01111000 in binary. If you transfer it naively to a system with the opposite endianness, you’d surely expect to see all the bits reversed. In that case you’d receive the number 00011110 01101010 00101100 01001000, which is hex 0x1E6A.2C48. But we said we’d read hex 0x7856.3412.

让我们回到上面的问题。16进制数据`0x12345678`，二进制形式为`00010010 00110100 01010110 01111000`。如果传送给一个具有相反字节序的系统，你肯定期望看到所有的位是相反的：`00011110 01101010 00101100 01001000`，16进制为`0x1E6A2C48`。但是，为什么我们上边却说是`0x78563412`。

It’s true that complete bit reversal could arise in some circumstances; there are communication links that send the MS bit first, and some that send the LS bit first. But sometime in the 1970s 8-bit bytes emerged as a universal base unit both inside computers and in computer communications systems (where they were called “octets”). Typically, communication systems build all their messages out of bytes, and only the lowest-level hardware engineers know which bit goes first.

的确，在某些情况下完全可以实现上面的位反转：有些通讯链路先发送最高有效位，另一些则先发送最低有效位。但是，在上世纪70年代，更多地使用8位的字节作为计算机内部和计算机通信系统的基本单元。通常，通信系统使用字节构建消息流，由硬件决定哪一位首先被发送出去。

Meanwhile, every microprocessor system got to use 8-bit peripheral controllers (wider controllers were reserved for the high-end stuff), and all those peripherals have 8-bit ports numbered 0 through 7, and the most significant  bit is 7. Somehow, without a shot apparently fired, every byte was little-endian, and has been ever since.

与此同时，每个微控制器系统都使用8位宽的外设控制器（更宽的控制器是为高端设备预留的），这些外设一般都使用8位端口，bit0-bit7，最高有效位是bit7。对此，没有任何争议，每个字节都采用小端字节序。从那以后，一直保持到现在。

Early microprocessor systems were 8-bit CPUs on 8-bit buses with 8-bit memory systems, so they had no endianness. Intel’s 8086 was a 16-bit little-endian system. When Motorola introduced the 68000 microprocessor around 1978, they greatly admired IBM’s mainframe architecture. Either in admiration for IBM or to differentiate themselves from Intel, they thought they should be big-endians too. But Motorola couldn’t oppose the prevailing bits within-bytes convention—every 8-bit Motorola peripheral would have had to be connected to a 68000 with its data bus bit-twisted. As a result, the 68000 family looks like Figure 10.3, with the bits and bytes numbered in opposite directions.

而早期的微处理器系统，都是8位CPU，使用8位总线和一个8位的内存进行通信，所以，根本不存在字节序问题。Intel的8086是一个16位的小端系统。当摩托罗拉在1978年左右推出68000微处理器时，他们推崇IBM的大型机架构。不管是处于对IBM的敬仰，还是为了区别于Intel，他们选择了大端模式。但是，它们无法违反8位外设控制器的习惯，于是，每一个8位的摩托罗拉的外设通过交错的数据总线与68000进行连接。这就是，我们为什么说收到`0x78563412`数据的原因。于是，68000家族系列使用如下图所示的字节序：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_3.PNG">

The 68000 and its successors went on to be used for most successful UNIX servers and workstations (notably with Sun). When MIPS and other RISCs emerged in the 1980s, their designers needed to woo system designers with the right endianness, so they designed CPUs that could swing either way. But from the 68000 onward, big-endian has meant 68000-style big-endian, with bits and bytes going opposite ways. When you set up a MIPS CPU to be big-endian, it looks like Figure 10.3. And that’s where the trouble really starts.

68000及其后继产品被大多数成功的UNIX服务器和工作站所使用（尤其是SUN公司）。所以，当MIPS架构和其它RISC指令集架构的CPU在1980年代出现时，他们的设计者为了兼容大小端字节序，都设置了配置选项，可以自由选择使用大小端模式。但是，从68000开始，大端模式就指68000风格的大端字节序，其bit位和字节序相反。当你配置MIPS架构CPU为大端模式时，就如上图所示。

One small difficulty comes when you read hardware manuals for your CPU and see register diagrams. Everyone’s convinced that registers are (first and foremost) 32-bit integers, so they’re invariably drawn with the MS bit (bit 31, remember) first. This has some consequences for programmers and hardware designers alike. That picture motivates the difference between “shift-left” and “shift-right” instructions, determines the bit-number arguments of bitfield instructions, and even refers to the labeling of the bitfields that make up MIPS instructions.

选择不同的大小端模式，可能会影响你阅读CPU和寄存器手册。尤其是对于位操作指令，向左移动和向右移动的区别，位操作指令的参数位置等。

Once you get over that, there is serious software trouble when porting software or moving data between incompatible machines; there is hardware trouble when connecting incompatible components or buses. We’ll take the software and hardware problems separately.

理解了这些，就要面对大小端模式对于软硬件的影响：软件的话，比如移植软件和数据通信；硬件的话，如不兼容组件或总线之间的连接问题。对此，我们分别进行阐述。

## 2.2 软件和字节序

Here’s a software-oriented definition of endianness: A CPU/compiler system where the lowest addressed byte of a multi-byte integer holds the least significant bits is called little-endian; a system where the lowest addressed byte of a multibyte integer holds the most significant bits is called big-endian. You can very easily find out which sort of CPU you have by running a piece of deliberately nonportable code:

对于软件来说，字节序的定义如下：如果CPU或编译器中，一个整型数的最低寻址字节存储的是最低有效位，那么就是小端模式；如果最低寻址字节存储的是最高有效位，那么就是大端模式。可以通过下面的代码，验证你的CPU是大端还是小端模式。

    #include <stdio.h>

    main ()
    {
        union {
            int     as_int;
            short   as_short[2];
            char    as_char[4];
        } either;
        
        either.as_int = 0x12345678;
        if (sizeof(int) == 4 && either.as_char[0] == 0x78) {
            printf ("Little endian\n");
        }
        else if (sizeof(int) == 4 && either.as_char[0] == 0x12) {
            printf ("Big endian\n");
        }
        else {
            printf ("Confused\n");
        }
    }

Strictly speaking, software endianness is an attribute of the compiler toolchain, which could always—if it worked hard enough—produce the effect of either endianness. But on a byte-addressable CPU like MIPS with native 32-bit arithmetic it would be unreasonably inefficient to buck the hardware; thus we talk of the endianness of the CPU.

严格说来，软件字节序是编译器工具链的一个属性，如果你想的话，可以产生任何字节序。但是对于像MIPS架构这样的可字节寻址的CPU，内部使用32位算术运算，这会导致硬件效率降低；因此，我们主要谈论的是CPU的字节序。

Of course, the question of byte layout within the address space applies to other data types besides integers; it affects any item that occupies more than a single byte, such as floating-point data types, text strings, and even the 32-bit op-codes that represent machine instructions. For some of these noninteger data types, the idea of arithmetic significance applies only in a limited way, and for others it has no meaning at all.

当然了，内存地址空间中字节布局的问题也同样适用于其它数据类型。比如浮点数据类型，文本字符串，甚至是机器指令的32位操作码。对于这些非整型数据类型来说，算术意义大部分情况下根本没有意义。

When a language deals in software-constructed data types bigger than the hardware can manage, then their endianness is purely an issue of software convention—they can be constructed with either endianness. I hope that modern compiler writers appreciate that it’s a good idea to be consistent with the hardware’s own convention.

当软件要处理的数据类型大于硬件能够管理的数据类型时，字节序问题完全就成为软件的一种约定了，可以是任何字节序。当然了，最好还是与硬件本身的约定保持一致。

### 2.2.1 字节序和可移植性

So long as binary data items are never imported into an application from elsewhere, and so long as you avoid accessing the same piece of data under two different integer types (as we deliberately did above), your CPU’s endianness is invisible (and your code is portable). Modern C compilers will try to watch out for you: If you do this by accident, you’ll probably get a compiler error or warning.

只要应用程序不从外界获取数据，或避免使用不同的整型数据类型访问同一个数据块（如上面我们故意那样做的那样），CPU的字节序对你的应用程序就是不可见的，也就是说，你的代码是可移植的。如果你这样干了，现在的C编译器都会努力在这方面给出warning或者error。

You may not be able to live within those limitations, however; you may have to deal with foreign data delivered into your system from elsewhere, or with memory-mapped hardware registers. For either of these, you need to know exactly how your compiler accesses memory.

但是，应用程序不可能接收这些限制。你可能必须处理外部发送过来的数据，或者需要把硬件寄存器映射到内存上，便于访问。不管哪种应用，你都需要准确知道编译器如何访问内存。

This all seems fairly harmless, but experience shows that of all data-mapping problems, endianness is uniquely confusing. I think this is because it is difficult even to describe the problem without taking a side. The origin of the two alternatives lies in two different ways of drawing the pictures and describing the data; both are natural in different contexts.

这好像没有什么，但是经验告诉我们，字节序是最容易混淆的。因为很难描述这个问题。两种方案起源于勾画和描述数据的不同方式，它们在各自的视角都没有什么问题。

As we saw above, big-endians typically draw their pictures organized around words. So that gives us a big-endian picture of the data structure we used in Figure 10.4. It would look a lot prettier with the IBM convention of labeling the MS bit as bit 0, but that’s no longer done.

如上所述，大端模式通常围绕WORD来组织其数据结构。如下图1所示。虽然按照IBM约定，将最高有效位（MS）标记为位0更为美观，但是，现在已经不在那样做了。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_4.PNG">

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_5.PNG">

But little-endians are likely to emphasize a software-oriented, abstract view of computer memory as an array of bytes. So the same data structure looks like Figure 10.5. Little-endians don’t think of computer data as primarily numeric, so they tend to put all the low numbers (bits, bytes, or whatever) on the left.

而小端模式更主要从软件方面抽象数据结构，将计算机的内存视为一个字节类型的数组。如上图2所示。小端模式没有将数据看作是数值型的，所以倾向于把低有效位存放在左边。

It’s very difficult to achieve a real grasp of endianness without drawing pictures, but many people find themselves struggling to set aside the conventions they’re used to; for example, if you’re used to numbering the bits from right to left, it can take a real effort of will to number them from left to right (a picture of a little-endian structure as drawn by someone with big-endian habits can look very illogical). This is the essence of the subject’s capacity to confuse: It’s difficult even to think about an unfamiliar convention without getting caught up in the ones you know.

所以，软件大小端的字节序问题，归根结底就是一个习惯的问题：究竟习惯于从左到右，还是从右到左对bit位进行编号。每个人的习惯不同，这也是字节序问题容易混淆的根源。

## 2.3 硬件和字节序

We saw previously that a CPU’s native endianness only shows up when it offers direct support both for word-length numbers and a finer-resolution, byte-sized memory system. Similarly, your hardware system acquires a recognizable endianness when a byte-addressed system is wired up with buses that are multiple bytes wide.

前面我们已经看见，CPU内部的字节序问题，只有在能够同时提供WORD字长的数据和按字节访问的内存系统中才会出现。同样，当系统与具有多字节宽度的总线进行连接时，也会存在字节序问题。

When you transfer multibyte data across the bus, each byte of that data has its own individual address. If the lowest-address byte in the data travels on the eight bus lines (“byte lane”) with the lowest bit numbers, the bus is little-endian. But if the lowest-address byte in the data travels on the byte lane with the highest bit numbers, the bus is big-endian.

当通过总线传输多个字节数据时，数据中的每个字节都有自己的存储地址。如果总线上传输的数据的低地址字节，被编为低bit位编号，那么这条总线就是小端模式；反之，如果使用高bit位编号对数据的低地址字节进行编号，那么就是大端模式总线。

There’s no necessary connection between the “native” endianness of a CPU and the endianness of its system interface considered as a bus. However, I don’t know of any CPUs where the software and interface endianness are different, so we can talk about “the endianness of a CPU” and mean both internal organization and system interface.

Byte-addressable CPUs announce themselves as either big- or little-endian every time they transfer data. Intel and DEC CPUs are little-endian; Motorola 680x0 and IBM CPUs are big-endian. MIPS CPUs can be either, as configured from power-up; most other RISCs have followed the MIPS lead and chosen to make endianness configurable—a boon when updating an existing system with a new CPU.

可字节寻址的CPU在它们传送数据的时候会声明是大端还是小端字节序。英特尔和DEC的CPU是小端模式；摩托罗拉680x0和IBM的CPU是大端模式。MIPS架构CPU可以支持大小端两种模式，需要上电时进行配置。许多其它RISC指令集架构的CPU也都遵循MIPS架构的思路，选择大小端可配置的方式：这在使用一个新的CPU替换已经存在的系统时是个优点，如果旧系统遵循小端模式，新的CPU也配置为小端模式；反之亦然。

Hardware engineers can hardly be blamed for connecting up different buses by matching up the bit numbers. But trouble strikes when your system includes buses, CPUs, or peripherals whose endianness doesn’t match. In this case the choice is not a happy one; the system designer must choose the lesser of two evils:

* Bit number consistent/byte sequence scrambled: 

    Most obviously, the designer can wire up the two buses according to their bit numbers, which will have the effect of preserving bit numbering within aligned “words.” But since the relationship between bit numbers and bytes-within-words is different on the two buses, the two sides will see the sequence of bytes in memory differently. 

    Any data that is not of bus-width size and bus-width aligned will get mangled when transferred between the connected buses, with bytes swapped within each bus-width-sized unit. This looks and feels worse than the software problem. With wrong-endianness data in software, you have no problem finding data type boundaries; it’s just that the data doesn’t make sense. With this hardware problem the boundaries are scrambled too (unless the data are, by chance, aligned on bus-width “word” boundaries).

    There’s a catch here. If the data being passed across the interface is always aligned word-length integers, then bit-number-consistent wiring will conceal the endianness difference, avoiding the need for software conversion of integers. But hardware engineers very rarely know exactly which data will be passed across an interface over the lifetime of a system, so be cautious.

* Byte address consistent/integers scrambled: 

    The designer can decide to preserve byte addressing by connecting byte lanes that correspond to the same byte-within-word address, even though the bit-numbering of the data lines in the byte lane doesn’t match at all. Then at least the whole system can agree on the data seen as an array of bytes.

However, there are presumably going to be components with mismatched software endianness in the system. So your consistent byte addressing is guaranteed to expose their disagreement about the representation of multibyte integers. And—in particular—even a bus-width-aligned integer (the “natural” unit of transfer)will appear byte-swapped when moved to the other endianness.

For most purposes, byte address scrambling is much more harmful, and we’d recommend “byte address consistent” wiring. When dealing with data representation and transfer problems, programmers will usually fall back on C’s basic model of memory as an array of bytes, with other data types built up from that. When your assumptions about memory order don’t work out, it’s very hard to see what’s going on.

Unfortunately, a bit number consistent/byte address scrambled connection looks much more natural on a schematic; it can be very hard to persuade hardware engineers to do the right thing.

Not every connection in a system matters. Suppose we have a 32-bit-wide memory system bolted directly to a CPU. The CPU’s system interface may not include a byte-within-word address—the address bus does not specify address bits 1 and 0. Instead, many CPUs have four “byte enables,” which show that data is being transferred on particular byte lanes. The memory array is wired to the whole bus, and on a write the byte enables tell the memory array which of four possible byte locations within the word will actually get written. Internally, the CPU associates each of the byte lanes with a byte-within-word address, but that has no effect on the operation of the memory system. Effectively, the memory/CPU combination acts together and inherits the endianness of the CPU; where byte-within-word 0 actually goes in memory doesn’t matter, so long as the CPU can read it back again[^3].

It’s very important not to be seduced by this helpful characteristic of a RAM memory into believing that there’s no intrinsic endianness in a simple CPU/RAM system. You can spot the endianness of any transfer on a wide bus. Here’s a sample list of conditions in which you can’t just ignore the CPU’s endianness when building a memory system:

* If your system uses firmware that’s preprogrammed into ROM memory, the hardware address and byte lane connection assignments within the system need to match those assumed in the way the ROM was programmed, and the data contained in the ROM needs to match the CPU’s configured endianness. In effect[^4], the contents of the ROM are being delivered into your system from somewhere outside it. If the code is to be executed directly from the ROM, it’s especially important to get the endianness right, because it’s impossible for the CPU to apply any corrective software byte-swapping to the op-codes as it fetches them.
* 如果你的系统使用的是预先烧录到ROM内存中的固件时，硬件地址总线和字节数据通道与系统的连接方式必须与ROM编程时假设的方式是一致的。通俗的讲，现在是ROM，程序数据是预先写入到ROM中的，也就是大小端方式固定了，那么它与系统总线的连接必须是一致的大小端方式。尤其是对于指令来说，这很重要，因为它决定了取出的指令中操作码的字节序。

* When a DMA device gets to transfer data directly into memory, then its notions of ordering will matter.

* When a CPU interface does not in fact use byte enables, but instead issues byte-within-word addresses with a byte-width code (quite common for MIPS CPUs), then at least the hardware that decodes the CPU’s read and write requests must know which endianness the CPU is using. This can be particularly tricky if the CPU allows endianness to be software configured.

The next section is for you to tell your hardware engineer about how to set up a byte address consistent system—and even how to make that system configurable with the CPU, if some of your users might set up the MIPS CPU both ways.

[^3]: 熟悉硬件的工程师可能意识到了一个更为通用的原则：可写存储器的一个性质就是，不管连接到它的地址和数据总线怎么排列，都能继续工作。一个具体数据存储在哪里并不重要，重要的是你给出相同的读取地址时，能够正确读取之前写入的数据即可。
[^4]: in effect， 实际上，生效

### 2.3.1 连接字节序不一致的总线

Suppose we’ve got a 64-bit MIPS CPU configured big-endian, and we need to connect it to a little-endian 32-bit bus such as PCI.

假设我们有一个64位的CPU，配置为大端模式，将其与一个小端模式的32位PCI总线相连。下图展示了如何连线，以获得CPU和PCI两端看上去都一致的字节地址。

Figure 10.6 shows how we’d wire up the data buses to achieve the recommended outcome of consistent byte addresses as seen by the big-endian CPU and the little-endian bus.

The numbers called “byte lane” show the byte-within-bus-width part of the address of the byte data traveling there. Writing in the byte lane numbers is the key to getting one of these connections right.

Since the CPU bus is 64 bits wide and the PCI bus 32 bits, you need to be able to connect each half of the wide bus to the narrow bus according to the “word” address—that’s address bit 2, since address bits 1 and 0 are the byte-within-32-bit-word address. The CPU’s 64-bit bus is big-endian, so its high-numbered bits carry the lower addresses, as you can see from the byte lane numbers.

因为CPU是64位，而PCI总线是32位，所以，根据32位WORD宽的地址中的bit2，将64位总线分成两组，与32位PCI总线进行连接。比特位1和比特位0是每个WORD中的其中一个字节地址。CPU的64位总线是大端模式，高编号的位携带的是低地址，这个从字节通道的编号能够看出来。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_6.PNG">

You may find yourself staring at the numbering of the connections around the bus switch for quite some time before they really make sense. Such are the joys of endianness.

And note that I only showed data. PCI is a multiplexed bus, and in some clock cycles those “byte lanes” are carrying an address. In address cycles, PCI bus wire 31 is carrying the most significant bit of the address. The address-time connection from your MIPS-based system should not be swapped.

### 2.3.2 建立字节序可配置的连接

Suppose you want to build a board or bus switch device that allows you to configure a MIPS CPU to run with either endianness. How can we generalize the advice above?

We’d suggest that, if you can persuade your hardware designer, you should put a programmable byte lane swapper between the CPU and the I/O system. The way this works is shown diagrammatically in Figure 10.7; note that this is only a 32-bit configurable interface and it’s an exercise for you to generalize it to a 64-bit CPU connection.

We call this a byte lane swapper, not a byte swapper, to emphasize that it does not alter its behavior on a per-transfer basis, and in particular to indicate that it is not switched on and off for transfers of different sizes. There are circumstances where it can be switched on and off for transfers to different address regions—mapping some part of the system as bit number consistent/byte address scrambled—but that’s for you to make work.

上面的方法毕竟是固定的，一旦完成硬件设计就无法改动了。如果我们想要实现一个类似于总线开关设备，用它进行切换，让CPU既可以工作在大端模式，也可以工作在小端模式，如下图所示。

在这儿，我们称这个总线开关设备为字节通道交换器，而不是字节交换器。主要是想强调这个设备不管是开，还是关，都不会影响传输数据的大小。有了这个设备，我们就可以根据需要，认为关闭或者打开它，存取字节一致或者不一致的数据。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_7.PNG">

What a byte lane swapper does achieve is to ensure that whether your CPU is set up to be big- or little-endian, the relationship between the CPU and the now mismatched external bus or device can still be one where byte sequnce is preserved.

字节通道交换器所做的就是无论你的CPU设置为大端模式还是小端模式，CPU和不匹配的外设总线或设备之间，数据总能按照想要的序列进行交换。

You normally won’t put the byte-lane swapper between the CPU and its local memory—this is just as well, because the CPU/local memory connection is fast and wide, which would make the byte swapper expensive.

正常情况下，在CPU和内存之间不需要添加字节通道交换器。因为它们之间的连接本身就是快速且是并联的，添加字节通道交换器的代价比较昂贵。

As mentioned above, so long as you can decode the CPU’s system interface successfully, you can treat the CPU/local memory as a unit and install the byte swapper between the CPU/memory unit and the rest of the system. In this case, the relationship between bit number and byte order inside the local memory changes with the CPU, but this fact is not visible from the rest of the world.

综上所述，只要能够成功解码CPU的系统接口，就可以将CPU和本地内存视为一个单元。然后，在CPU/内存单元和系统其它部分之间添加字节通道交换器。这样，无论CPU配置成什么工作模式，字节序不再是一个问题了。

### 2.3.3 False Cures and False Prophets for Endianness Problems

Every design team facing up to endianness for the first time goes through the stage of thinking that the troubles reflect a hardware deficiency to be solved. It’s never that simple. Here are a few examples.

每个团队在第一次遇到字节序问题时，都可能会思考：这个问题可能反映了一个需要解决的硬件缺陷。然而，事情往往没有那么简单。比如下面的2个例子，有时候必须需要编程人员的干预。

* 可配置的I/O控制器： 

    Some newer I/O devices and system controllers can themselves be configured into big-endian and little-endian modes. You’re going to have to read the manual very carefully before using such a feature, particularly if you mean to use it not as a static (design time) option but rather as a jumper (reset time) option.

    一些新的I/O设备和系统控制器本身就可以自由配置成大端或者小端模式。想要使用这些特性之前，必须仔细阅读芯片手册。尤其是，硬件设计为可以使用跳线帽进行选择，而不是固定在某种工作模式下时。

    It is quite common for such a feature to affect only bulk data transfers, leaving the programmer to handle other endianness issues, such as access to bit-coded device registers or shared memory control fields. Also, the controller designer probably didn’t have the benefit of this book—and confusion about endianness is widespread.

    这些特性一般在大块数据传输时使用，其余的字节序问题，比如访问位编码的设备寄存器或者共享内存的控制位等问题，留给编程人员进行单独处理。

* 可以根据传输类型进行字节交换的硬件： 

    If you’re designing in some byte-swap hardware, it seems appealing to try to solve the whole problem. If we just swapped byte data to preserve its addresses, but left words alone, couldn’t we prevent the whole software problem? The answer is no, there aren’t any hardware fixes for the software problem. For example, many of the transfers in a real system are of data cache lines. They may contain an arbitrary mixture of data sizes and alignments; if you think about it for a moment, you’ll see that there simply isn’t any way to know where the boundaries are, which means there’s no way to determine the required swap configuration.

    如果你正在尝试设计一些字节交换硬件，意图解决整个问题。可以肯定的告诉你，这条路行不通。软件问题没有任何一个可以一劳永逸的硬件解决方案。比如，一个实际系统中的许多传输都是以数据高速缓存作为单位的。他们可能包含不同大小和对其格式的任意数据组合。可能无法知道数据的边界在哪里，也就意味着没有办法确定所需的字节交换配置。

有条件的字节交换除了增加混乱之外，没有什么多大用处。除了无条件的字节通道交换器之外，任何做法都是用来骗人的东西。

## 2.4 为MIPS架构CPU写支持任意字节序的软件

You may want to create binary code that will run correctly on MIPS CPUs with either endianness—probably for a particular board that may be run either way or to create a portable device driver that may run on boards of either configuration. It’s a bit tricky, and you will probably only do a tiny part of your bootstrap code like this, but here are some guidelines.

The MIPS CPU doesn’t have to do too much to change endianness. The only parts of the instruction set that recognize objects smaller than 32 bits are partial-word loads and stores. On a MIPS CPU with a 32-bit bus, the instruction:

    lbu t0, 1(zero)

takes the byte at byte program address 1, loads it into the least significant bits (0 through 7) of register t0, and fills the rest of the register with zero bits. This description is endianness independent. However, in big-endian mode the data loaded into the register will be taken from bits 16–23 of the CPU data bus; in little-endian mode, the byte is loaded from bits 8–15 of the CPU data bus.

取地址1处的字节，加载到寄存器t0的最低有效位上（0-7），其余部分填充0。这条指令本身描述是与字节序无关的。但是，大端模式下，数据将从CPU数据总线的位16-23进行读取；小端模式下，将从CPU数据总线的位8-15位进行加载。

Inside the MIPS CPU, there’s data-steering hardware that the CPU uses to direct all the active bytes in a transfer from their respective byte lanes at the interface to the correct positions within the internal registers. This steering logic has to accommodate all permutations of load size, address, and alignment (including the load/store left/right instructions described in section 8.5.1).

MIPS架构CPU内部，有个硬件单元负责把有效的字节从它们各自的字节通道中，加载到内存寄存器的正确位置上。这个负责操纵数据加载的硬件逻辑能够适应所有的加载大小、地址和对齐方式的组合（包括`load/store`和左右移位指令等）。

It is the change in the relationship between the active byte lane and the address on partial-word loads and stores that characterizes the MIPS CPU’s endianness. When you reconfigure your MIPS CPU’s endianness, it’s that steering logic between data and register whose behavior changes.

正是这个特性使得MIPS架构的CPU能够配置大小端工作模式。当你重新配置MIPS架构CPU的字节序时，正是改变了这个操纵数据加载的硬件逻辑单元的行为。

Complementing the chip’s configurability, most MIPS toolchains can produce code of either endianness, based on a command-line option.

为了配合CPU大小端的可配置性，大部分的MIPS工具链都能够在编译flag中添加一个选项，编译产生任何字节序的代码。

If you set a MIPS CPU to the wrong endianness for its system, then a couple of things will happen.

如果你设置了MIPS架构的CPU与系统不匹配的字节序，将会发生一些预料不到的事情。

First, if you change nothing else, the software will crash quickly, because on any partial-word write the memory system will pick up garbage data from the wrong part of the CPU bus. At the same time as reconfiguring the CPU, we’d better reconfigure the logic that decodes CPU cycles.2

首先，软件可能会迅速崩溃，因为对于字节的读取可能会获取垃圾数据。在重新配置CPU的同时，最好重新配置解码CPU的时钟逻辑[^5]。

If you fix that, you’ll find that the CPU’s view of byte addressing becomes scrambled with respect to the rest of the system; in terms of the description above, we’ve implicitly opted for a connection that keeps the bit numbers consistent, rather than the byte addresses.

Of course, data written by the CPU after a change of endianness will seem fine to the CPU itself; if we allow changes of endianness only at reset time, then volatile memory that is private to the CPU won’t give us any trouble.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_8.PNG">

Note also that the CPU’s view of bit numbering within aligned bus-width words continues to match the rest of the system. This is the choice we described earlier as bit number consistent and that we suggested you should generally avoid. But in this particular case it has a useful side effect, because MIPS instructions are encoded as bitfields in 32-bit words. An instruction ROM that makes sense to a big-endian CPU will make sense to a little-endian CPU too, allowing us to share a bootstrap. Nothing works perfectly—in this case, any data in the ROM that doesn’t consist of aligned 32-bit words will be scrambled. Many years ago, Algorithmics’ MIPS boards had just enough bi-endian code in their boot ROM to detect that the main ROM program does not match the CPU’s endianness and to print the helpful message:

    Emergency - wrong endianness configured.

The word Emergency is held as a C string, null-terminated. You should now know enough to understand why the ROM start-up code contains the enigmatic lines:

    .align 4
    .ascii "remEcneg\000\000\000y"

That’s what the string Emergency (with its standard C terminating null and two bytes of essential padding) looks like when viewed with the wrong endianness. It would be even worse if it didn’t start on a four-byte-aligned location. Figure 10.8 (drawn from the point of view of a confirmed big-endian) shows what is going on.

You’ve seen that writing bi-endian code is possible, but be aware that when you’re ready to load it into ROM, you’ll be asking your tools to do something they weren’t designed to handle. Typically, big-endian tools pack instruction words into the bytes of a load file with the most significant bits first, and littleendian tools work the other way around. You’ll need to think carefully about the result you need to achieve, and examine the files you generate to make sure everything went according to plan.

[^5]: 有些CPU接口的字节传输采用独立的字节通道选通信号，就不会发生这种问题。

## 2.5 可移植性和大小端无关代码

按照约定，大部分的MIPS工具链定义`BYTE_ORDER`作为字节序选择的宏定义选择的符号。

    #if BYTE_ORDER == BIG_ENDIAN
    /* 大端模式版本代码... */
    #else
    /* 小端模式版本代码... */
    #endif

So if you really need to, you can put in different code to handle each case. But it’s better—wherever possible—to write endianness-independent code. Particularly in a well-controlled situation (such as when writing code for a MIPS system that may be initialized with the CPU in either mode), you can get rid of a lot of dependencies by good thinking.

如果确实需要，你可以选择使用上面的模板编写不同的分支，分别处理大端模式和小端模式的代码。但是，还是尽量编写与字节序无关的代码，CPU处于哪种模式下，就编写哪种模式下的代码。

All data references that pick up data from an external source or device are potentially endianness dependent. But according to how your system is wired, you may be able to produce code that works both ways. There are only two ways of wiring the wrong endianness together: One preserves byte addresses and the other bit numbers. For some particular peripheral register access in a particular range of systems, there’s a good chance that the endianness change consistently sticks to one of these.

所有从外部数据源或设备接收数据的引用都有潜在的字节序问题。但是，根据系统的布线方式，你能够生成双向工作的代码。在不同的字节序之间接线只有两种方式：一种保持字节地址不变，另一种保持位编号不变。在系统特定范围内访问具体的外设寄存器，字节序可以保持与二者之一保持一致。

If your device is typically mapped to be byte address compatible, then you should program it strictly with byte operations. If ever, for reasons of efficiency or necessity, you want to transfer more than one byte at a time, you need to write endianness-conditional code that packs or unpacks that data.

如果你的外设通常被映射为字节地址兼容，那么你应该按照字节操作进行编程。如果为了效率或者处于不得已，想要一次传输多个字节，你需要编写根据字节序进行打包和解包的代码。

If your device is compatible at the word (32-bit) level—for example, it consists of registers wired (by however devious and indirect a route) to a fixed set of MIPS data bus bits—then program it with bus-width read/write operations. That will be 32-bit or 64-bit loads and stores. If the device registers are not wired to MIPS data bus bits starting at 0, you’ll probably want to shift the data after a read and before a write. For example, 8-bit registers on a 32-bit bus in a system originally conceived as big-endian are commonly wired via bits 31–24.

如果你的外设与32位WORD兼容，通常按照总线宽度进行读写操作。那就是32位或64位的读写操作。

## 2.6 字节序和外来数据（省略）

This chapter is about programming, not a treatise on I/O and communications, so we’ll keep this section brief. Any data that is not initialized in your code, chosen libraries, and OS is foreign. It may be data you read from some memory-mapped piece of hardware, data put into memory by DMA, data in a preprogrammed ROM that isn’t part of your program, or you may be trying to interpret a byte stream obtained from an “abstract” I/O device under your OS.

The first stage is to figure out what this data looks like in memory; with C, that can usually be accomplished by mapping out what its contents are as an array of unsigned char. Even if you know your data and compiler well enough to guess which C structure will successfully map to the data, fall back to the array of bytes when something is not as you expect; it’s far too easy to miss what is really going on if your data structure is incorrect.

Apart from endianness, the data may consist of data types that are not supported by your compiler/CPU; it may have similar types but with completely different encodings; it may have familiar data but be incorrectly aligned; or, falling under this section’s domain, it may have the wrong endianness.

If the chain along which the data has reached you has preserved byte order at each stage, the worst that will happen is that integer data will be represented with an opposite order, and it’s easy enough to build a “swap” macro to restore the two, four, or eight bytes of an integer value.

But if the data has passed over a bit number consistent/byte address scrambled interface, it can be more difficult. In these circumstances, you need to locate the boundaries corresponding to the width of the bus where the data got swapped; then, taking groups of bytes within those boundaries, swap them without regard to the underlying data type. If you do it right, the result should now make sense, with the correct byte sequence, although you may still need to cope with the usual problems in the data—including, possibly, the need to swap multibyte integer data again.

# 3 高速缓存可见性带来的问题

In section 4.6, you learned about the operations you can use to get your caches initialized and operating correctly. This section alerts you to some of the problems that can come up and explains what you can do to deal with them.

在之前的文章《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/) 》中，我们已经了解了初始化和正确操作Cache的方法。本段主要讲解一些可能出现的问题，并解释如何处理这些问题。

Most of the time, the caches are completely invisible to software, serving only to accelerate the system as they should. But especially if you need to deal with DMA controllers and the like, it can be helpful to think of the caches as independent buffer memories, as shown in Figure 10.9.

大部分时候，Cache对软件都是不可见的，只是一个加速系统执行的工具。但是，当你需要处理DMA控制器及其类似的事物时，考虑把Cache作为一个独立的内存的buffer会很有帮助，如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_9.PNG">

It’s important to remember that transfers between cache and memory always work with blocks of memory that fit the cache line structure—typically 16- or 32-byte-aligned blocks—so the cache may read or write a block because the CPU makes direct reference to just one of the byte addresses contained within it; even if the CPU never touches any of the other bytes in the cache line, it’ll still include all the bytes in the next transfer of that cache line.

重要的是，Cache和内存之间的传输总是以16字节或32字节对齐的内存块作为传输单元。即使CPU只是读取一个字节，仍然会加载这样的内存块到Cache行中。

In an ideal system, we could always be certain that the state of the memory is up-to-date with all the operations requested by the CPU, and that every valid cache line contains an exact copy of the appropriate memory location. Unfortunately, practical systems can’t always live up to this ideal. We’ll assume that you initialize your caches after any reset, and that you avoid (rather than try to live with) the dreaded cache aliases described in section 4.12. Starting from those assumptions, how does a real system’s behavior fall short of the ideal?

理想情况下，内存的状态与CPU请求的所有操作都是最新的，每个有效的Cache行都保存一份正确的内存备份。不幸的是，实际的系统根本就达不到这种理想的情况。假设每次复位之后都初始化缓存，并且也不存在《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/)》一文中提到的Cache重影问题。Cache和内存之间还是会存在数据不一致的问题。如下：

* Cache中的旧数据：

    When your CPU writes to memory in cached space, it updates the cached copy (and may also write memory at the same time). But if a a memory location is updated in any other way, any cached copies of its contents continue to hold the old value, and are now out-of-date. That can happen when a DMA controller writes data. Or, when the CPU writes new instructions for itself, the I-cache may continue to hold whatever was at the same location before. It’s important for programmers to realize that the hardware generally doesn’t deal with these conditions automatically.

    当CPU向Cache的内存区写入数据时，它会更新Cache中的备份，同时写入内存。但是，如果通过其它方式更新了内存，那么Cache中的备份就有可能成为旧数据。比如，DMA控制器写内存，或者，CPU往内存中写入新指令，I-Cache继续保持原先的指令等。所以，编程人员应该注意，硬件是不会自动处理这些情况的。

* 内存中的旧数据： 

    When the CPU writes some data into a (writeback) cache line, that data is not immediately copied to memory. If the data is read later by the CPU, it gets the cached copy and is fine; but if something that isn’t the CPU reads memory, it may get the old value. That can happen to an outbound DMA transfer.

    当CPU写数据到Cache行中（回写），数据不会立即复制到内存中。稍后，CPU读取数据，直接读取Cache拷贝，一切没有问题。但是，如果不是CPU读取数据，而是其它控制器直接从内存读取数据，就会获取旧值。比如，向外传送的DMA。

The software weapons you have to fight problems caused by visible caches are a couple of standard subroutines that allow you to clean up cache/memory inconsistency, built on MIPS cache instructions. They operate on cache locations corresponding to a specified area of memory and can write-back up-to-date cache data or invalidate cache locations (or both).

为此，MIPS架构提供了Cache指令，可以根据需要调用它们，消除这种内存和Cache的不一致性。这些指令可以将最新的Cache数据写回到内存中，或者根据内存的最新状态失效对应的Cache行中的内容。

Well, of course, you can always map data uncached. In fact, there are some circumstances when you will do just that: Some network device controllers, for example, have a memory-resident control structure where they read and write bytes and bit flags, and it’s a lot easier if you map that control structure uncached. The same is true, of course, of memory-mapped I/O registers, where you need total control over what gets read and written. You can do that by accessing those registers through pointers in kseg1 or some other uncached space; if you use cached space for I/O, bad things will happen!

当然了，我们可以把数据映射到非Cache的内存区，比如`kseg1`区域。比如，网络控制器，映射一段非Cache的内存区保存读写的数据和标志位；这样可以方便快速的读取数据，因为不需要同步Cache和内存中的数据。相同的，内存映射的I/O寄存器，最好也映射到非Cache的内存区，通过`kseg1`或其它非Cache内存区中的指针进行访问。如果为I/O使用经过Cache的内存区，可能发生坏事情。

If (unusually) you need to use the TLB to map hardware register accesses, you can mark the page translation as uncached. That’s useful if someone has built hardware whose I/O registers are not in the low 512 MB of the physical memory space.

如果你需要使用TLB映射硬件寄存器，从而进行访问，你可以标记页转换为非Cache内存区（当然了，这不经常使用）。当I/O寄存器的内存地址不在低512M物理地址空间的时候，该方法是非常有用的。

It’s possible that you might want to map a memory-like device (a graphics frame buffer, perhaps) through cached space so as to benefit from the speed of the block reads and writes that the CPU only uses to implement cache refills and write-backs. But you’d have to explicitly manage the cache by invalidation and write-back on every such access. Some embedded CPUs provide strange and wonderful cache options that can be useful for that kind of hardware—check your manual.

还有的使用情况就是映射类似图像帧缓冲区为使用Cache的内存区，充分利用CPU的Cache充填和回写的block读写速度，提高像素帧的刷新频率。但是，每次图像帧的访问，都需要失效和回写Cache，显式地管理Cache。有一些嵌入式CPU，可能会提供一些奇怪但好用地Cache选项，请仔细检查对应芯片的手册。

## 3.1 Cache管理和DMA数据

This is a common source of errors, and the most experienced programmers will sometimes get caught out. Don’t let that worry you too much: Provided you think clearly and carefully about what you’re trying to achieve, you’ll be able to get your caches to behave as they should while your DMA transfers work smoothly and efficiently.

Cache管理和DMA数据传输是一个很容易出错的地方，即使很有经验的编程者也常常会犯错。对此，不要犯怵；只要清晰地知道自己想干什么以及怎么干，你就能让你的Cache和DMA传输正常工作。

When a DMA device puts data into memory, for example, on receipt of network data, most MIPS systems don’t update the caches—even though some cache lines may currently be holding addresses within the region just updated by the DMA transfer. If the CPU subsequently reads the information in those cache lines, it’ll pick up the old, stale version in the cache; as far as the CPU can tell, that’s still marked valid, and there’s no indication that the memory has a newer version.

比如，当从网络上接收到数据后，DMA设备会直接把数据存进内存，大部分MIPS系统不会更新Cache--即使某些Cache行中持有的地址落在DMA传输更新的内存区域中。随后，如果CPU读取这些Cache行的数据，将会读取Cache中旧的、过时的数据；就CPU而言，没有被告知内存中已经有了新数据，Cache中的数据仍然是合法的。

To avoid this, your software must actively invalidate any caches’ lines that fall within the address range covered by your DMA buffer, before there’s any chance that the CPU will try to refer to them again. This is much easier to manage if you round out all your DMA buffers so they start and end exactly at cache line boundaries.

为了避免这种情况，你的程序必须在CPU尝试读取落在DMA缓冲区对应地址范围的数据前，主动失效对应Cache行中的内容。应该将DMA缓冲区的边界和Cache行的边界对齐，这样更容易管理。

For outbound transfers, before you allow a DMA device to transfer data from memory—such as a packet that you’re sending out via a network interface—you must make absolutely sure that none of the data to be sent is still just sitting in the cache. After your software has finished writing out the information to be transferred by DMA, it must force the write-back of all cache lines currently holding information within the address range that the DMA controller will use for the transfer. Only then can you safely initiate the DMA transfer.

对于通过DMA向外传输数据，比如网络通信，你必须在允许DMA设备传输数据之前，完全确保Cache中的数据都已经更新到对应的内存发送区域里了。也就是说，在你的程序写完需要由DMA发送的数据信息之后，必须强制写回所有的落在DMA控制器映射的内存地址范围的Cache行中的内容到内存中。只有这样，才能安全启动DMA传输。

On some MIPS CPUs, you can avoid the need for explicit write-back operations by configuring your caches to use write-through rather than write-back behavior, but this cure is really worse than the disease — write-through tends to be much slower overall and will also raise your system’s power consumption.

有些MIPS架构CPU，为了避免显式的回写操作，配置为直写式Cache。但是，这种方案有一个缺点，直写式Cache会造成总体性能上更慢，也会增加系统的电源功耗。

You really can get rid of the explicit invalidations and write-backs by accessing all the memory used for all DMA transfers via an uncached address region. This isn’t recommended either, because it’ll almost certainly degrade your system’s overall performance far more than you’d like.Even if your software’s access to the buffers is purely sequential, caching the DMA buffer regions will mean that information gets read and written in efficient cache-line-sized bursts rather than single transfers. The best general advice is to cache everything, with only the following exceptions:

当然，你也可以通过映射DMA的传输数据区到非Cache内存地址区，避免显式的失效和回写操作。这也是不推荐的一种方式，因为从整体上会降低系统的性能。因为使用Cache读写内存的速度肯定要快于直接从内存读取数据。最好的建议就是使用Cache，只有下面的情况避免使用Cache：

* I/O寄存器： 

    Perhaps obvious, but worth pointing out. MIPS has no dedicated input/output instructions, so all device registers must be mapped somewhere in the address space, and very strange things will happen if you accidentally let them be cached.

    MIPS架构没有专门的输入、输出指令，所以，所有的外设寄存器都必须被映射到一段内存地址空间上，如果使用Cache，会发生一些奇怪的事情。

* DMA描述符数组：

    Sophisticated DMA controllers share control/status information with the CPU using small descriptor data structures held in memory. Typically, the CPU uses these to create a long list of information to be transferred, and only then tells the DMA controller to begin its work. If your system uses descriptors, you’ll want to access the memory region that contains them through an uncached address region.

    复杂的DMA控制器和CPU共享控制和状态信息，这些信息保存在内存中的描述符数据结构中。通常，CPU使用这些描述符结构体创建一个待发送数据信息的列表，然后，只需告诉DMA控制器开始工作即可。如果你的系统使用描述符结构，请将其映射到非Cache内存地址区域。

A portable OS like Linux must deal with a range of caches from the most sophisticated and invisible to the crude and simple, so it has a fairly well-defined API (set of stable function calls) for driver writers to use and some terse documentation on how to use them. See section 15.1.1.

移植性比较好的操作系统，比如Linux，不管是复杂的、不可见的Cache，还是简单的Cache，都能很好的适配。所以，Linux一般提供一组很完备的API，供驱动编写者使用。

## 3.2 Cache管理和写指令数据：自修改代码

If your code ever tries to write instructions into memory, then execute them, you’ll need to make sure you allow for cache behavior.

如果你想在程序的执行过程中，产生新的代码，然后跳转到新代码中执行。必须确保正确的Cache行为。

This can surprise you on two levels. First, if you have a write-back D-cache, the instructions that your program writes out may not find their way to main memory until something triggers a write-back of the relevant cache lines. The instructions that your program wrote out could just be sitting in the D-cache at the time you try to execute them, and the CPU’s fetches simply can’t access them there. So the first step is to do write-back operations on the cache lines at which you write the instructions; that at least ensures they reach main memory.

因为如果你不注意，可能会在两个阶段给你带来意想不到的结果：

* 首先，如果你使用的是回写式D-Cache，你写的指令数据在没有触发相关Cache行的回写操作之前，一直停留在Cache中，并没有写入到内存中。如果，此时CPU尝试执行这些新的代码指令，因为仍然在D-Cache中，CPU无法访问到它们。所以，当CPU写完新指令数据后，首先要做的就是执行回写操作，保证数据写入到内存中。

The second surprise (regardless of which type of D-cache you have) is that even after writing out the new instructions to some region of main memory, your CPU’s I-cache may still hold copies of the information that used to be held in those addresses. Before you tell the CPU to execute the newly written instructions, it’s essential that your software first invalidates all the lines in the I-cache that contain information at the affected address range.

* 其次，不管你使用的是哪种类型的D-Cache，在你把指令数据写入到内存中后，你的I-Cache里仍然持有着这些地址之前的数据。所以，在CPU执行新写的代码指令之前，软件首先应该失效I-Cache中的相关行。

Of course, you could avoid the need for these explicit write-back and invalidate operations by writing and then executing the new instructions within an uncached address region; but that gives up the advantages of caching and is almost always a mistake.

当然了，你也可以使用非Cache区域保存新的代码指令，然后执行它们。但是，这毕竟放弃了Cache的加速效果不是。

The general-purpose cache management instructions described in section 4.9 are CP0 instructions, only usable by kernel-privilege software. That doesn’t matter when cache operations are related to DMA operations, which are also entirely kernel matters. But it does matter with applications like writing instructions and executing them (think of a modern application using a “just-in-time” interpreted/translated language).

我们在《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/)》一文中描述的Cache管理指令都是协处理器CP0指令，只有特权级的代码才能使用。一般情况下，DMA操作也是内核完成的，这些都没有异议存在。但是，当用户态的应用程序也想要这样写指令，然后执行的话（比如，现在的即时性的解释性语言），却无法访问这些指令。

So MIPS32/64 provides the synci instruction, which does a D-side writeback and an I-side invalidate of one cache-line-sized piece of your new code. Find out how in section 8.5.11.

所以，MIPS32/64提供了`synci`指令，它可以执行D-Cache的回写操作和I-Cache的失效操作。具体可以参考MIPS指令集参考。

## 3.3 Cache管理和非Cache或直写数据

If you mix cached and uncached references that map to the same physical range of addresses, you need to think about what this means for the caches. Uncached writes will update only the copy of a given address in main memory, possibly leaving what’s now a stale copy of that location’s contents in the D-cache—or the I-cache. Uncached loads will pick up whatever they find in main memory—even if that information is, in fact, stale with respect to an up-to-date copy present only in cache.

如果你混合使用Cache和非Cache程序地址访问同一段物理内存空间，你需要清楚这意味什么。使用非Cache程序地址往物理内存中写入数据，可能会造成D-Cache或I-Cache中保留一份过时的拷贝（相同地址）。使用非Cache程序地址直接从内存中加载数据，可能是旧数据，而最新的数据还停留在Cache中。

Careful use of cached and uncached references to the same physical region may be useful, or even necessary, in the low-level code that brings your system into a known state following a reset. But for running code, you probably don’t want to do that. For each region of physical memory, decide whether your software should access it cached or uncached, then be absolutely consistent in treating it that way.

上电复位后，在引导系统进入一个已知状态的底层代码中，使用Cache和非Cache程序地址引用同一段物理地址空间是非常有用，甚至是有非常有必要的。但是，对于运行中的代码，一般不要这样做。而且，不管是使用Cache程序地址，还是使用非Cache程序地址访问物理内存，一定要保证它的一致性。

## 3.4 Cache重影和页着色

There’s more about the hardware origin of cache aliases in section 4.12. The problem occurs with L1 caches that are virtually indexed but physically tagged, and where the index range is big enough to span two or more page sizes. The index range is the size of one “set” of the cache, so with common 4-KB pages you can get aliases in an 8-KB direct-mapped cache or a 32-KB four-way set-associative cache.

我们在《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/)》一文中已经描述了Cache重影的根源。L1级Cache使用虚拟地址作为索引，而使用物理地址作为Tag标签，如果索引的范围大于、等于2个page页，就可能发生Cache重影。索引范围等于一组Cache的大小，所以，使用4KB大小的page页的话，在8KB大小的直接映射Cache或着32KB大小的4路组相关的Cache上就可能会发生Cache重影。

发生Cache重影会有什么后果呢？在进程上下文切换的时候，必须首先清空Cache，要不然，上个进程映射的物理地址，可能与新进程映射的物理地址相同，导致同一物理地址在Cache上有2份拷贝，可能会导致意想不到的后果。再比如，使用共享内存的时候，多个进程的虚拟地址都可能引用这个数据，如果发生Cache重影，那么也会导致共享内存中的数据不正确。

The “page color” of a location is the value of those one or more virtual address bits that choose a page-sized chunk within the appropriate cache set. Two virtual pointers to the same physical data can produce an alias only if they have a different page color. So long as all pointers to the same data have the same color, all is well—all the data, even though at different virtual addresses, will be stored in the same physical portion of the cache and will be correctly identified by the (common) physical tag.

为此，聪明的软件工程师们想了一个巧妙地技巧：`页着色技术`，又称为Cache着色，其实都是一回事，叫法不一样而已。具体的做法就是，假定page页的大小是4K，然后给每一个page页分配一个颜色（此处的颜色就是一种区分叫法而已，没有任何实际动作），使用虚拟地址的某几个比特位来标记颜色。当然，也可以选择使用物理地址中的某些比特位标记颜色。相同颜色的虚拟地址对应一组Cache。所以，两个虚拟地址想要指向同一个物理地址的数据，必须具有不同的页颜色。也就是说，页着色技术要求页分配程序把任一给定的物理地址映射到具有相同颜色的虚拟地址上。

> 颜色数是否与Cache的way数相等？应该是相等的。

It’s quite common in Linux (for example) for a physical page to be accessible at multiple virtual locations (shared libraries are routinely shared between programs at different virtual addresses).

比如说，Linux操作系统，多个虚拟地址可能都会访问一个物理页（共享库）。

Most of the time, the OS is able to overalign virtual address choices for shared data—the sharing processes may not use the same address, but we’ll make sure their different virtual addresses are a multiple of, say, 64KB apart, so the different virtual addresses have the same color. That takes up a bit more virtual memory, but virtual memory is fairly cheap.

大部分时候，操作系统OS对于共享数据的虚拟地址的对齐肯定满足要求-共享进程也可以不使用相同的地址，但是，我们必须保证不同的虚拟地址必须是64K的倍数，所以不同的虚拟地址具有相同的颜色。也就避免了Cache重影。这可能消耗更多的虚拟内存，但是虚拟内存又不值钱，对吧？😀

It’s easy to think that cache aliases are harmless so long as the data is “readonly” (it must have been written once, but that was before there were aliases to it): We don’t care if there are multiple copies of a read-only page. But they’re only mostly harmless. It is possible to tolerate aliases to read-only data, particularly in the I-cache: But you need to make sure that cache management software is aware that data that has been invalidated at one virtual address may still be cached at another.

想象一下，加入数据都是只读的，Cache重影还会有影响吗？当然是没有什么问题了。但是，必须保证你的程序知道，在失效某个数据的时候，Cache的其它地方还有一份拷贝。

With the widespread use of virtual-memory OSs (particularly Linux) in the embedded and consumer computing markets, MIPS CPUs are increasingly being built so that cache aliases can’t happen. It’s about time this long-lasting bug was fixed.

随着带有虚拟内存管理的操作系统OS在嵌入式和消费者电子产品市场的广泛应用，越来越多的MIPS架构CPU，在硬件层面就消除了Cache重影。相信随着时间的推移，这个问题也许就不存在了吧。

Whatever you need to do, the cache primitive operations required for a MIPS32/64 CPU are described in section 4.9.1.

# 4 内存访问的排序和重新排序

Programmers tend to think of their code executing in a well-behaved sequence: The CPU looks at an instruction, updates the state of the system in the appropriate ways, then goes on to the next instruction. But our program can run faster if we allow the CPU to break out of this purely sequential form of execution, so that operations aren’t necessarily constrained to take place in strict program order. This is particularly true of the read and write transactions performed at the processor’s interface, triggered by its execution of load and store instructions.

程序员往往认为他们的代码是顺序执行的：CPU执行指令，更新系统的状态，然后继续下一条指令。但是，如果允许CPU乱序执行，而不是这种串行方式执行，效率可能更高。这对于执行load和store这种存储指令尤其重要。

From the CPU’s point of view, a store requires only an outbound write request: Present the memory address and data, and leave the memory controller to get on with it. Practical memory and I/O devices are relatively slow, and in the time the write is completed the CPU may be able to run tens or hundreds of instructions.

从CPU的角度来看，执行store操作就是发送一个write请求：给出内存地址和数据，其余的交给内存控制器完成。实际的内存和I/O设备相对较慢，等write操作完成，CPU可能已经完成了几十条甚至几百条指令。

Reads are different, of course: They require two-way communication in the form of an outbound request and an inbound response. When the CPU needs to know the contents of a memory location or a device register, there’s probably not much it can do until the system responds with the information.

read操作又有不同：它需要发送一个read请求，然后等待对请求的响应。当CPU需要知道内存或者设备寄存器中的内容时，没有得到请求响应前，可能啥也做不了。

In the quest for higher performance, that means we want to make reads as fast as possible, even at the expense of making writes somewhat slower. Taking this thinking a step further, we can even make write requests wait in a queue, and pass any subsequent read requests to memory ahead of the buffered writes. From the CPU’s point of view, this is a big advantage; by starting the read transaction immediately, it gets the response back as soon as possible. The writes will have to be done sometime, and the queue is of finite size: But it’s likely that after this read is done there will be a period while the CPU is running from cache. And if the queue fills up, we’ll just have to stop while some writes happen: That’s certainly no worse than if we’d done the writes in sequence.

如果想要追求更高的性能，就意味着我们需要让read尽可能地快，甚至不惜让write操作变得更慢。进一步考虑，我们可以让write操作排队等待，把随后地任何read操作请求提前到write请求队列之前执行。从CPU地角度来看，这是一个大优势：尽可能快地启动read操作，就越早得到read操作的响应。然后，在某个时刻把执行write操作，而且write请求队列的大小是固定的。但是，这个write操作可能需要写Cache一段时间。如果这个队列满了，可能需要停下来等待一段时间，等待所有的write完成操作。但是，这肯定要比顺序执行，效率更高。这就是现代CPU一般都具有一个write buffer的原因。

You can probably see a problem here: Some programs may write a location and then read it back again. If the read overtakes the write, we may get stale data from memory and our program will malfunction.Most of the time we can fix it with extra hardware that checks an outgoing read request against the addresses of entries in the write queue and doesn’t allow the read to overtake a matching write.4

看到这儿，你可能会有一个疑问：某些程序可能会写入一个地址，然后再将其读回来，这时候会怎么样呢？如果read提前到write之前执行，我们可能从内存中读取的是旧值，从而导致程序发生故障。通常，CPU会提供额外的硬件，比较read操作的地址和write队列中的地址，如果有相同的项，就不允许这样的read操作提前到write操作之前执行。

In systems where tasks that could be really concurrent (that is, they might be running on different CPUs) share variables, the problem of ordering reads and writes becomes more dangerous. It’s true that much of the time the tasks have no expectation of mutual ordering.Ordering matters when the tasks are deliberately using shared memory for synchronization and communication, but in this case the software will be using carefully crafted OS synchronization operations (locks and semaphores, for example).

上面的讨论没有考虑真正的并发系统，比如多核系统。并发执行的任务间共享变量，对其执行read和write操作会非常危险。比如使用共享变量进行同步和通信的时候，内存访问次序就会非常重要。这种情况下，软件一般会采用精心的设计，比如锁和信号量，进行同步操作。

But there are some shared-memory communication tricks—often good, cheap, efficient ones—that don’t need so many semaphores or locks but are disrupted by arbitrary cycle reordering. Suppose, for example, we have two tasks: one is writing a data structure, the other is reading it. They use the data structure in turn, as shown in Figure 10.10.

但是，使用共享内存，还有一些技巧，往往效果更好，开销也更小。因为不需要使用信号量或者锁。但是，可能会被乱序执行打断。假设，我们有2个任务，如下图所示：一个读取数据结构，一个写数据结构。它们可以交替使用这个数据结构。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_10.PNG">

For correct operation,we need to know that when the reader sees the updated value in the key field, we can guarantee that all the other updates will be visible to the reader as well.

为了能够正确执行，我们需要知道，对于reader任务来说，当什么时候reader任务看见关键域中的值发生了更新时，能够保证其它所有的更新对reader任务可见。

Unless we discard all the performance advantages of decoupling reads and writes from the CPU, it’s not practical for hardware to conceal all ordering issues from the programmer. The MIPS architecture provides the sync instruction for this purpose: You’re assured that (for all participants in the shared memory) all accesses made before the sync will precede those made afterward. It’s worth dwelling on the limited nature of that promise: It only relates to ordering, and only as seen by participants in uncached or cache-coherent memory accesses.

当然，硬件可以实现所有的内存访问顺序问题，从而将它们对程序员不可见，但是也就放弃了解耦read和write操作带来的性能优势。MIPS架构提供了`sync`指令实现这个目的，它可以确保sync指令之前的访问先于之后的执行。但是，这种保障指令有其局限性：只与内存的访问顺序有关，只能被非Cache或具有Cache一致性的内存访问的参与者看见。

To make the example above reliable on a suitable system, the writer should include sync just before writing keyfield, and the reader should have a sync just after reading keyfield. See section 8.5.9 for details. But there’s a lot more to this subject; if you’re building such a system, you’re strongly recommended to use an OS that provides suitable synchronization mechanisms, and read up on this subject.

对于上面的示例，为了让其在合适的系统上可靠地运行，writer任务应该在写关键域的值之前，调用一条sync指令；reader任务应该在读关键域的值之后插入一条sync指令。对于sync指令的详细使用方法，可以参考《MIPS指令集参考大全》一文。

Different architectures make different promises about ordering. At one extreme, you can require all CPU and system designers to contrive that all the writes and reads made by one CPU appear to be in exactly the same order from the viewpoint of another CPU: That’s called “strongly ordered.” There are weaker promises too (such as “all writes remain in order”); but the MIPS architecture takes the radical position that no guarantees are made at all.

不同的体系架构对执行顺序作出了不同的规定。一类极端情况就是，要求所有的CPU和系统设计人员努力保证一个CPU的全部读和写操作，从另一个CPU的角度看上去顺序完全相同，这叫做`强序`。也有一类情况就是`弱序`，比如只要求所有的写操作保持顺序不变。而MIPS架构更为激进，完全就是`无序`访问内存。这就要求我们系统开发人员必须手动保证内存的访问顺序是正确的。

## 4.1 访存顺序和写缓存

Let’s escape from the lofty theory and describe something rather more practical. The idea of holding outbound requests in a write buffer turns out to work especially well in practice because of the way store instructions tend to be bunched together. For a CPU running compiled MIPS code, it’s typical to find that only about 10 percentage of the instructions executed are stores; but these accesses tend to come in bursts—for example, when a function prologue saves a group of register values.

前面讨论了这么多理论，接下来让我们讨论点实际的内容吧。把write操作缓存到一个队列中（也就是硬件中常常讨论的write buffer）的思想在实践中证明非常有效。因为，store指令往往是多条指令扎堆出现。比如，一个运行MIPS代码的CPU，实际上运行的store指令大约占所有指令的10%左右；但是，往往是突发式访问，比如函数的调用过程中，首先需要压栈操作一组寄存器的值。

Most of the time the operation of the write buffer is completely transparent to software. But there are some special situations in which the programmer needs to be aware of what’s happening:

但是，一般情况下，写缓存（write buffer）都是硬件保证的，对于软件来说不用管理。但是，也有一些特殊的情况，程序员需要知道怎样处理：

1. I/O寄存器访问的时序 

    This affects all MIPS CPUs. After the CPU executes a store to update an I/O device register, the outbound write request is liable to incur some delay in the write buffer, on its way to the device. Other events, such as inbound interrupts, may take place after the CPU executes the store instruction, but before the write request takes effect within the I/O device. This can lead to surprising behavior: For example, the CPU may receive an interrupt from a device “after” you have told it not to generate interrupts. To give another example: If an I/O device needs some software-implemented delay to recover after a write, you must ensure that the write buffer is empty before you start counting out that delay— ensuring also that the CPU waits while the write buffer empties. It’s good practice to define a subroutine that does this job, and it’s traditionally given the name wbflush(). See section 10.4.2 for hints on implementing it.

    这个问题，对于所有架构CPU都存在。比如，CPU发出一个store指令，更新I/O设备寄存器的值，write请求可能会在写缓存中延迟一段时间。这时候，可能会发生其它事件，比如中断。但是此时写入的值还未更新到对应的I/O设备寄存器中。这可能导致一些奇怪的行为：比如，你想禁止产生中断，但是CPU发出write操作之后，CPU还有可能会收到中断。

2. read操作抢先于write操作执行 

    The MIPS32/64 architecture permits this behavior, discussed above. If your software is to be robust and portable, it should not assume that read and write order is preserved. Where you need to guarantee that two cycles happen in some particular order, you need the sync instruction described in section 8.5.9.

    上面已经讨论过，MIPS32/64架构允许这种操作。如果想要软件更加健壮和具有可移植性，就不应该假定read和write操作顺序会被保持。如果想要保证前后两个指令周期是按照特定顺序执行，就需要插入sync指令。

3. 字节汇集

    Some write buffers watch for partial-word writes within the same memory word (or even writes within the same cache line) and will combine those partial writes into a single operation. To avoid unpleasant symptoms when uncached writes are combined into a word-width, it’s a good idea to map your I/O registers such that each register is in a separate word location (i.e., 8-bit registers should be at least four bytes apart).

    有些写缓存会汇集不足WORD大小的write操作，凑成一个WORD大小的write操作，然后再执行（有些写缓存甚至会攒一个Cache行，然后再写入）。所以，为了避免对于非Cache的内存区也做相同的操作，最好的办法就是把I/O寄存器（比如，一个8位的寄存器）映射到一个单独的WORD大小的地址上。

## 4.2 实现wb flush

Most write queues can be emptied out by performing an uncached store to any location and then performing an operation that reads the same data back. A write queue certainly can’t permit the read to overtake the write—it would return stale data. Put a sync instruction between the write and the read, and that should be effective on any system compliant with MIPS32/64.

This is effective, but not necessarily efficient; you can minimize the overhead by loading from the fastest memory available. Perhaps your system offers something system-specific but faster. Use it after reading the following note!

> Write buffers are often implemented within the CPU, but may also be implemented outside it; any system controller or memory interface that boasts of a write-posting feature introduces another level of write buffering to your system. Write buffers outside the CPU can give you just the same sort of trouble as those inside it. Take care to find out where all the write buffers are located in your system, and to allow for them in your programming.

# 5 Writing it in C

You probably already write almost everything in C or in C++. MIPS’s lack of special I/O instructions means that I/O register accesses are just normal loads and stores with appropriately chosen addresses; that’s convenient, but I/O register accesses are usually somewhat constrained, so you need to make sure the compiler doesn’t get too clever. MIPS’s use of large numbers of CP0 registers also means that OS code can benefit from well-chosen use of C asm() operations.

## 5.1 Wrapping Assembly Code with the GNU C Compiler


TheGNUC Compiler (“GCC”) allows you to enclose snippets of assembly code within C source files. GCC’s feature is particularly powerful, but other modern compilers probably could support the example here. But their syntax is probably quite different, so we’ll just discuss GCC here. If you want low-level control over something that extends beyond a handful of machine instructions, such as a library function that carries out some clever computation, you’ll really need to get to gripswithwriting pure MIPS assembly; but if you just want to insert a short sequence that consists of one or a few specific MIPS instructions, the asm() directive can achieve the desired result quite simply. Better still, you can leave it to the compiler to manage the selection of registers according to its own conventions.

As an example, the following code makes GCC use the three-operand form of multiply, available on more recent MIPS CPUs. If you just use the normal C language * multiplication operator, the work could end up being done by the original formof themultiply instruction that accepts only two source operands, implicitly sending its double-length result to the hi/lo register pair.5

The C function mymul() is exactly like the three-operand mul and delivers the less significant half of the double-length result; the more significant half is simply discarded, and it’s up to you to ensure that overflows are either avoided or irrelevant.

    static int __inline__ mymul(int a, int b)
    {
        int p;

        asm(
            "mul %0, %1, %2"
            : "=r" (p)
            : "r" (a), "r" (b)
        );

        return p;
    }

The function itself is declared inline, which instructs the compiler that a use of this function should be replaced by a copy of its logic (which permits local register optimization to apply). Adding static means that the function need not be published for other modules to use, so no binary of the function itself will be generated. It very often makes sense to wrap an asm() like this: You’d probably usually then put the whole definition in an include file. You could use a C preprocessor macro, but the inlined function is a bit cleaner.

The declarations inside the asm() parentheses tell GCC to emit a MIPS mul line to the assemblerwith three operands on the command line—onewill be the output and two will be inputs.

Onthe line below,we tellGCCabout operand %0, the product: first, that this value will be write-only (meaning that there’s no need to preserve its original value) with the “=” modifier; the “r” tells GCC that it’s free to choose any of the general-purpose registers to hold this value. Finally, we tell GCC that the operand we wrote as %0 corresponds to the C variable p.

On the third line of the asm() construct, we tell GCC about operands %1 and %2.Again,we allowGCCto put these in any of the general-purpose registers, and tell it that they correspond to the C variables a and b.

At the end of the example function, the result we obtained from the multiply instruction is returned to the C caller.

GCC allows considerable control over the specification of the operands; you can tell it that certain values are both read and written and that certain hardware registers are left with meaningless values as a side effect of a particular assembly sequence.You can dig out the details fromthe MIPS-specific sections of the GCC manual.

## 5.2 Memory-Mapped I/O Registers and “Volatile”

Most of you will be writing code that accesses I/O registers in C—you certainly shouldn’t be using assembly code in the absence of any pressing need to do so, and since all I/O registers in MIPSmust be memory-mapped, it is never difficult to access them from C. Having said that, you should keep in mind that as compilers advance, or if you make significant use of C++, it can become harder to predict exactly the low-level instruction sequences that’ll end up in your code. Here are some well-worn hints.

I might write a piece of code that is intended to poll the status register of a serial port and to send a character when it’s ready:

    unsigned char *usart_sr = (unsigned char *) 0xBFF00000;
    unsigned char *usart_data = (unsigned char *) 0xBFF20000;
    #define TX_RDY 0x40
    void putc (ch)
    char ch;
    {
        while ((*usart_sr & TX_RDY) == 0);
        *usart_data = ch;
    }


I’d be upset if this sent two characters and then looped forever, but that would be quite likely to happen. The compiler sees the memory-mapped I/O reference implied by *usart sr as a loop-invariant fetch; there are no stores in the while loop so it seems safe to pull the load out of the loop. Your compiler has recognized that your C program is equivalent to:

    void putc(ch)
    char ch;
    {
        tmp = (*usart_sr & TX_RDY);
        while (tmp);
        *usart_data = ch;
    }

You could prevent this particular problem by defining your registers as follows:

    volatile unsigned char *usart_sr =
    (unsigned char *) 0xBFF00000;
    volatile unsigned char *usart_data =
    (unsigned char *) 0xBFF20000;

A similar situation can exist if you examine a variable that is modified by an interrupt or other exception handler.Again, declaring the variable as volatile should fix the problem. I won’t guarantee that this will always work: The C bible describes the operation of volatile as implementation dependent. I suspect, though, that compilers that ignore the volatile keyword are implicitly not allowed to optimize away loads. Many programmers have trouble using volatile. The thing to remember is that it behaves just like any other C type modifier—just like unsigned in the example above. You need to avoid syndromes like this:

    typedef char * devptr;
    volatile devptr mypointer;

You’ve now told the compiler that it must keep loading the pointer value from the variable devptr, but you’ve said nothing about the behavior of the register you’re using it to point at. It would be more useful to write the code like this:

    typedef volatile char * devptr;
    devptr mypointer;

Once you’ve dealt with this, the most common reason that optimized code breaks will be that you have tried to drive the hardware too fast. There are often timing constraints associated with reads and writes of hardware registers, and you’ll often have to deliberately slow your code to fit in.

What’s the main lesson of this section? While it’s easier to write and maintain hardware driver code in C than in assembly, it’s important to use this option responsibly. In particular, you’ll need to understand enough about the way the toolchain converts your high-level source code into lowlevel machine instructions to make sure you get the system behavior that you intended.

## 5.3 Miscellaneous Issues When Writing C for MIPS Applications

* Negative pointers:When running simpleunmappedcode on a MIPS CPU, all pointers are in the kseg0 or kseg1 areas, so any data pointer’s 32-bit value has the top bit set and looks “negative.” Unmapped programs on most other architectures are dealing with physical addresses, which are usually a lot smaller than 2 GB! Such pointer values could cause trouble when pointer values are being compared, if the pointer were implicitly converted to a signed integer type. Any implicit conversions between integer and pointer types (quite common in C) should be made explicit and should specify an unsigned integer type (you should use unsigned long for this). Most compilers will warn about pointer-to-integer conversions, though you may have to specify an option.

* Signed versus unsigned characters: In early C compilers, the char type used for strings was usually equivalent to signed char; this is consistent with the convention for larger integer values. However, as soon as you have to deal with character encodings using more than 7-bit values, this is dangerous when converting or comparing. Modern compilers usually make char equivalent to unsigned char instead. If you discover that your old program depends on the default signextension of char types, good compilers offer an option that will restore the traditional convention.

* Moving from 16-bit int: A significant number of programs are being moved up from 16-bit x86 or other CPUs where the standard int is a 16-bit value. Such programs may rely, much more subtly than you think, on the limited size and overflow characteristics of 16-bit values. Although you can get correct operation by translating such types into short, that will be inefficient. In most cases you can let variables quietly pick up the MIPS int size of 32 bits, but you should be particularly aware of places where signed comparisons are used to catch 16-bit overflow.

* Programming that depends on the stack: Some kind of function invocation stack and data stack are implicit in C’s block structure. Despite the MIPS hardware’s complete lack of stack support, MIPS C compilers implement a fairly conventional stack structure. Even so, if your program thinks it knows what the stack looks like, it won’t be portable. If possible, don’t just replace the old assumptions with new ones: Two of the most common motivations for stack abuse are now satisfied with respectable and standards-conforming macro/library operations, which may tackle what your software was trying to do before:

    - stdargs: Use this include-file-based macro package to implement routines with a variable number of parameters whose type need not be predefined at compile time.

    - alloca(): To allocate memory at run time, use this library function, which is “on the stack” in the sense that it will be automatically freed when the function allocating thememory returns. Some compilers implement alloca() as a built-in function that actually extends the stack; otherwise, there are pure-library implementations available. But don’t assume that suchmemory is actually at an address with some connection with the stack.


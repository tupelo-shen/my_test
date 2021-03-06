[TOC]

> 站在巨人的肩膀上，才能看得更远。
> 
> If I have seen further, it is by standing on the shoulders of giants.
> 
<p align="right">牛顿</p>

科学巨匠尚且如此，何况芸芸众生呢。我们不可能每个软件都从头开始搞起。大部分时候，我们都是利用已有的软件，不管是应用软件，还是操作系统。所以，对于MIPS架构来说，完全可以把在其它架构上运行的软件拿来为其所用。

但是，这是一个说简单也简单，说复杂也复杂的工作。为什么这么说呢？如果你要采用的软件，其可移植性比较好的话，可能只需要使用支持MIPS架构的编译器重新编译一遍就可以了；如果程序只是为特定的硬件平台编写的话（大部分嵌入式软件都是如此），可能处处是坑。而像`Linux`系统，在编写应用或者系统软件的时候，一般都会考虑可移植性。所以说，基于Linux的软件一般都可以直接编译使用。但是，像现在流行的一些实时操作系统，比如、`μC/OS`、`Free-RTOS`、`RT-Thread`或其它一些基于微内核的系统，它们的程序一般不通用，需要修改才能在其它平台上运行。

而且，越往底层越难移植，几乎所有嵌入式系统上的驱动程序都不能直接使用。而且，嵌入式系统软件通常好几年才会发生一次重大设计更新，所以，如果坚持考虑软硬件上的接口兼容并不合理，尤其是考虑到成本效益的时候。

本文就是总结一些在移植代码或者编写代码时，应该需要特别关注的一些点。

# 1 MIPS架构移植软件时常见的问题

以下是一些比较常见的问题：

1. 大小端 

    计算机的世界分为大端（`big-endian`）和小端（`little-endian`）两个阵营。为了二者兼容，MIPS架构一般都可以配置到底使用大端还是小端模式。所以，我们应该彻底理解这个问题，不要在这个问题上栽跟头。

2. 内存布局和对齐 

    大部分时候，我们可以假定C声明的数据结构在内存中的布局是不可移植的。比如，使用C的结构体表示从输入文件或者网络上接收的数据的时候。还有，对于指针或者union型数据，通过不同方法引用的时候，也会存在风险。但是，内存布局还与一些其它的约定有关（比如寄存器的使用，参数传递和堆栈等）。

3. 显式管理Cache

    对于嵌入式系统来说，大部分时候采用的都是微处理器，可能并没有实现硬件Cache。但是，随着半导体技术的发展，现在的高端工业处理器一般都带有Cache，只是对于系统软件来说是不可见而已（比如，大部分处理器把Cahce可能带有的副作用都由硬件进行处理，软件不需要管理）。但是，大部分MIPS架构的CPU为了保持硬件的简单，而将一些Cache的副作用暴漏给软件，需要软件进行处理。关于这部分内容，我们后面会进行阐述。

4. 内存访问序 

    在大部分的嵌入式或者消费电子产品中，一般都挂载了许多子系统，这些子系统一般通过一条总线，比如PCIe总线、AHB总线、APB总线等进行通信。虽然方便了我们对系统进行扩展，但是也带来了不可预知的问题。比如，CPU和I/O设备之间的信息需要缓存处理，招致不可见的延时；或者它们被拆分成几个数据流，扔到总线上，但是对于到达目的地的顺序却没有保障。关于这部分内容，我们后面会进行阐述。

5. 编程语言
    
    对于语言，当然大部分时候使用C语言了。但是，对于MIPS架构来说，有些事情可能使用汇编语言编写更好。讲解这部分内容的时候，主要涉及inline汇编、内存映射I/O寄存器和MIPS架构可能出现的各种缺陷。

# 2 字节序：WORD、BYTE和BIT

`WORD`最早是由`Danny Cohen`在1980年引入计算机科学的。在他的文章中，以其独有的幽默和智慧指出，通信系统分为两大阵营，分别是字节寻址访问和整数寻址访问。

在乔纳森·斯威夫特（Jonathan Swift）的《格列佛游记》（Gulliver’s Travels）中，`little-endians`派和`big-endians`派就如何吃一个煮熟的鸡蛋展开了一场战争。斯威夫特讽刺的是18世纪的宗教争端问题，双方都不知道他们的分歧是完全武断的。科恩的笑话很受欢迎，这个词也就流传了下来。这个问题不仅仅体现在通信上，对于代码的可移植性也有影响。

计算机程序总是在处理不同类型的数据序列：迭代字符串中的字符，数组中的WORD类型元素，以及二进制表示的BIT位。C程序员普遍认为，所有这些变量以字节为单位在内存中顺序排列的-比如，memcpy()函数能够复制任何数据，不论什么数据类型。而且，使用C语言编写的I/O系统也将I/O操作以字节进行建模，你才能够使用read()和write()之类的函数读写包含任何数据类型的内存块。

这样，一个计算机写数据，另一个计算机读数据。那么，我们不禁想，第二台计算机是如何理解第一台计算所写的数据的呢？

另外，我们不止一次地被提醒，要小心数据填充和对齐。因为这对于数据搬运会产生很大的影响。比如说，因为填充的原因，想要完整准确地传递float型数据就变得很难，所以，浮点数据存在精度问题。但是，我们期望至少能够正确表述整形数据，而"字节序"就是个拦路虎。比如说，一个32位整型数，用16进制进行表示为`0x12345678`，而读进来却为`0x78563412`，发生了字节交换。想要理解为什么，我们需要追溯一下字节序的发展历史。

[^1]: come across，偶然相遇，无意中出现，讲的清楚明白，给人...印象；
[^2]: curse，诅咒，咒骂；

## 2.1 位、字节、字和整形

我们知道一个32位的int型数据，是由32个比特位组成的，它们每一位都有自己的意义，就像我们熟悉的10进制那样，每一位分别表示`个`、`十`、`百`、`千`、...以此类推，对于二进制，bit0代表1，bit1代表2，bit3代表4，bit4代表8，...。对于一个可以按字节访问的内存来说，32位整数占据4个字节。如何从比特位的视角表述整形数，有两种选择：一派，将低有效位（LS）放在前，也就是存储在内存的低地址里；而另一派，将高有效位（`MS`）放在前。科恩将其分别称为小端和大端。最早的时候，DEC的微型计算机是小端，而IBM的大型机是大端。彼时，两个阵营互不妥协。

有一个细节需要特殊提一下，大小端字节序的问题只有能够按字节访问的时候才会有。1960年代之前的电脑都是按照WORD大小进行组织：包括指令，整型数和内存宽度都是WORD大小。所以，不存在字节序的大小端问题。

我们在读写10进制数据的时候，习惯于从左到右，高有效位在左，低有效位在右。BYTE最早引入计算机，是为了方便将CHAR型字符打包成WORD，然后进行数据的交互。1970年代，一位IBM的老工程师花费了大量的时间，研究大量的内存dump列表，每个WORD大小的数据代表一组字符。这样看起来，使用小端字节序没有必要。大端字节序更有利于使用和阅读。但是，将数字的高有效位写在左端，字节顺序也是自左向右增加，这样和从右到左对bit位进行编号的行为不一致。于是，IBM将一个高有效位标记为bit0。看起来如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_1.PNG">

但是，根据数据的算术意义对bit位进行编号更自然，也就是说，标记为`N`的bit位，其算术意义就是`2^N`。这样，就可以把bit0-7存储在`字节0`中。显然，这种方式就变成了小端模式。显然，这种方式不利于阅读，但是对于习惯于将内存看成是一个字节型的大数组的人来说，就会非常有意义。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_2.PNG">

通过上面的讨论，可以看出，两幅图中，内容都是相同的，只是最高有效位（MS）和最低有效位（LS）进行了互换，当然，bit位的顺序也发生了互换。IBM主导的大端模式，看到的是被分割成字节的WORD；而Intel主导的小端模式看到的是构建WORD的字节序列。毋庸置疑的是，对于不同的人群，它们都非常有用。它们都有自己的优点，就看你怎么选择了。

让我们回到上面的问题。假设一个16进制类型的数据`0x12345678`，二进制形式为`00010010 00110100 01010110 01111000`。如果传送给一个具有相反字节序的系统，你肯定期望看到所有的位是相反的：`00011110 01101010 00101100 01001000`，16进制为`0x1E6A2C48`。但是，为什么我们上边却说是`0x78563412`。

的确，在某些情况下完全可以实现上面的位反转：有些通讯链路先发送最高有效位，另一些则先发送最低有效位。但是，在上世纪70年代，更多地使用8位的字节作为计算机内部和计算机通信系统的基本单元。通常，通信系统使用字节构建消息流，由硬件决定哪一位首先被发送出去。

与此同时，每个微控制器系统都使用8位宽的外设控制器（更宽的控制器是为高端设备预留的），这些外设一般都使用8位端口，bit0-bit7，最高有效位是bit7。对此，没有任何争议，每个字节都采用小端字节序。从那以后，一直保持到现在。

而早期的微处理器系统，都是8位CPU，使用8位总线和一个8位的内存进行通信，所以，根本不存在字节序问题。Intel的8086是一个16位的小端系统。当摩托罗拉在1978年左右推出68000微处理器时，他们推崇IBM的大型机架构。不管是处于对IBM的敬仰，还是为了区别于Intel，他们选择了大端模式。但是，它们无法违反8位外设控制器的习惯，于是，每一个8位的摩托罗拉的外设通过交错的数据总线与68000进行连接。这就是，我们为什么说收到`0x78563412`数据的原因。于是，68000家族系列使用如下图所示的字节序：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_3.PNG">

68000及其后继产品被大多数成功的UNIX服务器和工作站所使用（尤其是SUN公司）。所以，当MIPS架构和其它RISC指令集架构的CPU在1980年代出现时，他们的设计者为了兼容大小端字节序，都设置了配置选项，可以自由选择使用大小端模式。但是，从68000开始，大端模式就指68000风格的大端字节序，其bit位和字节序相反。当你配置MIPS架构CPU为大端模式时，就如上图所示。

选择不同的大小端模式，可能会影响你阅读CPU和寄存器手册。尤其是对于位操作指令，向左移动和向右移动的区别，位操作指令的参数位置等。

理解了这些，就要面对大小端模式对于软硬件的影响：软件的话，比如移植软件和数据通信；硬件的话，如不兼容组件或总线之间的连接问题。对此，我们分别进行阐述。

## 2.2 软件和字节序

对于软件来说，字节序的定义如下：如果CPU或编译器中，一个整型数的最低寻址字节存储的是最低8位，那么就是小端模式；如果最低寻址字节存储的是最高8位，那么就是大端模式。可以通过下面的代码，验证你的CPU是大端还是小端模式。

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

严格说来，软件字节序是编译器工具链的一个属性。只要你愿意，可以产生任何字节序的代码。但是对于像MIPS架构这样的可字节寻址的CPU，内部使用32位算术运算，这会导致硬件效率降低；因此，我们接下来，主要谈论的是CPU的字节序。

当然了，内存地址空间中字节布局的问题也同样适用于其它数据类型。比如浮点数据类型，文本字符串，甚至是机器指令的32位操作码。对于这些非整型数据类型来说，算术意义根本没有存在的价值。

当软件要处理的数据类型大于硬件能够管理的数据类型时，字节序问题完全就成为软件的一种约定了，可以是任何字节序。当然了，最好还是与硬件本身的约定保持一致。

### 2.2.1 可移植性和字节序

只要应用程序不从外界获取数据，或避免使用不同的整型数据类型访问同一个数据块（如上面我们故意那样做的那样），CPU的字节序对你的应用程序就是不可见的。也就是说，你的代码是可移植的。

但是，应用程序不可能接受这些限制。你可能必须处理外部发送过来的数据，或者需要把硬件寄存器映射到内存上，便于访问。不管哪种应用，你都需要准确知道编译器如何访问内存。

这好像没有什么，但是经验告诉我们，字节序是最容易混淆的，因为很难描述这个问题。大小端两种方案起源于勾画和描述数据的不同方式，它们在各自的视角都没有什么问题。

如上所述，大端模式通常围绕WORD来组织其数据结构。如下图1所示。虽然按照IBM约定，将最高有效位（MS）标记为位0更为美观，但是，现在已经不在那样做了。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_4.PNG">

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_5.PNG">

而小端模式更主要从软件方面抽象数据结构，将计算机的内存视为一个字节类型的数组。如上图2所示。小端模式没有将数据看作是数值型的，所以倾向于把低有效位存放在左边。

所以，软件大小端的字节序问题，归根结底就是一个习惯的问题：究竟习惯于从左到右，还是从右到左对bit位进行编号。每个人的习惯不同，这也是字节序问题容易混淆的根源。

## 2.3 硬件和字节序

前面我们已经看见，CPU内部的字节序问题，只有在能够同时提供WORD字长的数据和按字节访问的内存系统中才会出现。同样，当系统与具有多字节宽度的总线进行连接时，也会存在字节序问题。

当通过总线传输多个字节数据时，数据中的每个字节都有自己的存储地址。如果总线上传输数据的低地址字节，被编为低编号，那么这条总线就是小端模式；反之，如果使用高编号对数据的低地址字节进行编号，那么就是大端模式总线。

可字节寻址的CPU，在它们传送数据的时候会声明是大端还是小端字节序。英特尔和DEC的CPU是小端模式；摩托罗拉680x0和IBM的CPU是大端模式。MIPS架构CPU可以支持大小端两种模式，需要上电时进行配置。许多其它RISC指令集架构的CPU也都遵循MIPS架构的思路，选择大小端可配置的方式：这在使用一个新的CPU替换已经存在的系统时是个优点，如果旧系统遵循小端模式，新的CPU也配置为小端模式；反之亦然。

假设硬件工程师按照比特位的顺序把系统串联在一起，这本身没有什么错。但是，如果你的系统包含总线、CPU和外设，而它们的字节序不匹配时，会很麻烦。只能哪种方式更简单一些，使用哪一种。

* 位顺序一致/字节序被打乱 

    很显然，设计者可以按照位顺序的方式，把两条总线接到一起。这样，每个WORD的位顺序没有变化，但是位编号和字节是不同的，那么，两边内存中的字节序列也是不同的。

    任何小于总线宽度或没有按照总线宽度进行排列的数据，在总线上传输时，都会被破坏顺序，并按照总线宽度发生字节交换。这看上去要比软件问题严重。软件产生错误字节序的数据，根据数据类型仍能找到，因为没有破坏数据类型的边界；这是这个数据已经没有意义。但是，硬件却会打乱数据类型的边界（除非，数据恰好以总线宽度对其）。

    这儿有一个问题。如果通过总线接口进行传输的数据，总是按照WORD大小对齐，然后按照比特位的编号进行接线，那么就会隐藏大小端字节序的问题。也就避免了软件再对其进行转换。但是，硬件工程师很难知道，设计的系统上的接口以后会传输什么数据。所以，应该小心应对这个问题。

* 字节地址一致/整数被打乱 

    设计者可以按字节地址进行连线，也就是保证两端的相同字节存储在相同的地址。这样，字节通道内的比特位的顺序必然不一致。至少，整个系统可以把数据看作字节数组，只是数组元素的比特位是相反的。

对于大多数情况下，字节地址乱序副作用更明显。所以，我们推荐使用“字节地址一致”方法进行连线。因为在处理、传输数据时，程序员更希望将内存看作为字节数组。其它数据类型一般也是据此构建的。

不幸的是，有时候使用位编号一致，好像在原理图上更为自然。想要说服硬件工程师修改他们的原理图是一件很难的事情哦。这个大家都懂的😀！！！

并不是每个系统内的连接都很重要。假设，我们有一个32位地址范围内的内存系统，直接与CPU相连。CPU的接口可以不包含字节寻址，只需将地址总线上的低2位置0即可。与此相反的是，大部分CPU使能字节存取。在内部，CPU将每个字节通道和内存中的字节地址相关联。我们可以想象得到，无论连线如何，内存系统都不受影响。内存按照任何一种连线被写入，只要再按照同样的方式读取出来就可以。虽然，大部分情况下，内存一般都继承了CPU的大小端模式。但是，这个连接无关于字节序[^3]。

但是，上面的例子是个陷阱，千万不要以为简单的CPU/RAM架构不存在大小端的字节序问题。下面我们列举在构建内存系统时不能忽略CPU字节序问题的情况：

* 如果你的系统使用的是预先烧录到ROM内存中的固件时，硬件地址总线和字节数据通道与系统的连接方式必须与ROM编程时假设的方式是一致的。通俗的讲，现在是ROM，程序数据是预先写入到ROM中的，也就是大小端方式固定了，那么它与系统总线的连接必须是一致的大小端方式。尤其是对于指令来说，这很重要，因为它决定了取出的指令中操作码的字节序。

* 使用DMA直接传输数据到内存中时，字节序就很重要。

* 当CPU没有使能字节地址寻址，而使用一个字节大小的码表示该字节在WORD地址中的位置时（这在MIPS架构CPU中很常见），那么硬件必须能够正确解析CPU想要读写的是哪个字节，也就是必须知道CPU正在使用的大小端模式。这个需要仔细阅读芯片手册。

下面我们将分析硬件工程师如何构建一个字节地址一致的系统。

[^3]: 熟悉硬件的工程师可能意识到了一个更为通用的原则：可写存储器的一个性质就是，不管连接到它的地址和数据总线怎么排列，都能继续工作。一个具体数据存储在哪里并不重要，重要的是你给出相同的读取地址时，能够正确读取之前写入的数据即可。
[^4]: in effect， 实际上，生效

### 2.3.1 建立连接字节序不一致的总线

假设我们有一个64位的CPU，配置为大端模式，将其与一个小端模式的32位PCI总线相连。下图展示了如何连线，以获得CPU和PCI两端看上去都一致的字节地址。

因为CPU是64位，而PCI总线是32位，所以，根据32位WORD宽的地址中的bit2，将64位总线分成两组，与32位PCI总线进行连接。比特位1和比特位0是每个WORD中的其中一个字节地址。CPU的64位总线是大端模式，高编号的位携带的是低地址，这个从字节通道的编号能够看出来。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_6.PNG">

### 2.3.2 建立字节序可配置的连接

上面的方法毕竟是固定的，一旦完成硬件设计就无法改动了。如果我们想要实现一个类似于总线开关设备，用它进行切换，让CPU既可以工作在大端模式，也可以工作在小端模式，如下图所示。

在这儿，我们称这个总线开关设备为字节通道交换器，而不是字节交换器。主要是想强调这个设备不管是开，还是关，都不会影响传输数据的大小。有了这个设备，我们就可以根据需要，认为关闭或者打开它，存取字节一致或者不一致的数据。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_7.PNG">

字节通道交换器所做的就是无论你的CPU设置为大端模式还是小端模式，CPU和不匹配的外设总线或设备之间，数据总能按照想要的序列进行交换。

正常情况下，在CPU和内存之间不需要添加字节通道交换器。因为它们之间的连接本身就是快速且是并联的，添加字节通道交换器的代价比较昂贵。

综上所述，我们将CPU和内存视为一个整体。然后，在CPU和系统其它部分之间增加一个字节通道交换器。这样，无论CPU配置成什么工作模式，字节序就不再是一个问题了。

### 2.3.3 对字节序问题的一些错误认知

每当遇到大小端的字节序问题时，我们的第一反应往往是：这个问题可能是一个硬件缺陷。然而，事情往往没有那么简单。比如下面的2个例子，有时候必须需要编程人员的干预。

* 可配置的I/O控制器：

    一些新的I/O设备和系统控制器本身就可以自由配置成大端或者小端模式。想要使用这些特性之前，必须仔细阅读芯片手册。尤其是，可以使用跳线帽进行选择时，而不是固定在某种工作模式下时。

    这些特性一般在大块数据传输时使用，其余的字节序问题，比如访问位编码的设备寄存器或者共享内存的控制位等问题，留给编程人员进行单独处理。

* 可以根据传输类型进行字节交换的硬件： 

    如果你正在尝试设计一些字节交换硬件，意图解决整个问题。可以肯定的告诉你，这条路行不通。软件问题没有任何一个可以一劳永逸的硬件解决方案。比如，一个实际系统中的许多传输都是以数据高速缓存作为单位的。他们可能包含不同大小和对其格式的任意数据组合。可能无法知道数据的边界在哪里，也就意味着没有办法确定所需的字节交换配置。

有条件的字节交换除了增加混乱之外，没有什么多大用处。除了无条件的字节通道交换器之外，任何做法都是用来骗人的东西。

## 2.4 在MIPS架构上编写支持任意字节序的软件

你可能会想，我是否可以写一个正确运行在MIPS CPU上的程序，不论它被配置为大端模式，还是小端模式。或者编写一个可以运行在任意配置的板子上的驱动程序。很遗憾，这是一个很棘手的问题。最多也就是在引导程序中的某一小部分里可以这样写。下面是一些指导原则。

MIPS架构指令集中能够实现字节加载的指令如下所示：

    lbu t0, 1(zero)

上面这条语句的作用是：取字节地址1处的字节，加载到寄存器t0的最低有效位上（0-7），其余部分填充0。这条指令本身描述是与字节序无关的。但是，大端模式下，数据将从CPU数据总线的位16-23进行读取；小端模式下，将从CPU数据总线的位8-15位进行加载。

MIPS CPU内部，有个硬件单元负责把有效的字节从它们各自的字节通道中，加载到内存寄存器的正确位置上。这个负责操纵数据加载的硬件逻辑能够适应所有的加载大小、地址和对齐方式的组合（包括`load/store`和左右移位指令等）。

正是这个特性使得`MIPS CPU`能够配置大小端工作模式。当你重新配置`MIPS CPU`的字节序时，正是改变了这个操纵数据加载的硬件逻辑单元的行为。

为了配合CPU大小端的可配置性，大部分的MIPS工具链都能够在编译flag中添加一个选项，编译产生任何字节序的代码。

如果你设置了MIPS架构的CPU与系统不匹配的字节序，将会发生一些预料不到的事情。比如，软件可能会迅速崩溃，因为对于字节的读取可能会获取垃圾数据。在重新配置CPU的同时，最好重新配置解码CPU的时钟逻辑[^5]。

我们这儿选择位编号一致的方法，而不是字节地址一致的方法。之所以选择位编号一致的方法是因为，MIPS的指令都是按位进行编码的（32位指令集宽度）。这样的话，存放代码指令的ROM，不管是大端模式的CPU，还是小端模式的CPU都有意义。从而，可以共享这段引导程序。这不是完美无缺的，如果ROM包含非32位对齐的任何数据都将会被打乱。许多年前，Algorithmics公司的MIPS主板的ROM中，就使用了这种适应大小端模式的代码检测，主ROM程序是否与CPU的大小端模式匹配，如果不匹配，就会打印下面的帮助信息：

    Emergency - wrong endianness configured.

单词`Emergency`被存放在一个C字符串中。现在，我们已经能够理解为什么ROM程序的开头，往往会有下面这么几行奇幻的代码了。

    .align 4
    .ascii "remEcneg\000\000\000y"

上面定义了一个文本字符串`Emergency`，包含标准C的终止符null和2个字节的填充。下图以大端模式为视角，展示了这个单词在内存中的布局。如果使用了小端模式，就会打印上面的帮助信息。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_8.PNG">

通过上面的示例，我们可以看出编写适应大小端模式的代码是可能的。但是，要注意当把代码加载到ROM中时，加载工具应该区分大端模式和小端模式，确保能够把数据写入到正确的位置上。

[^5]: 有些CPU接口的字节传输采用独立的字节通道选通信号，就不会发生这种问题。

## 2.5 可移植性和大小端无关代码

按照约定，大部分的MIPS工具链定义`BYTE_ORDER`作为字节序选择的宏定义选择的符号。

    #if BYTE_ORDER == BIG_ENDIAN
    /* 大端模式版本代码... */
    #else
    /* 小端模式版本代码... */
    #endif

如果确实需要，你可以选择使用上面的模板编写不同的分支，分别处理大端模式和小端模式的代码。但是，还是尽量编写与字节序无关的代码，CPU处于哪种模式下，就编写哪种模式下的代码。

所有从外部数据源或设备接收数据的引用都有潜在的字节序问题。但是，根据系统的布线方式，你能够生成双向工作的代码。在不同的字节序之间接线只有两种方式：一种保持字节地址不变，另一种保持位编号不变。在系统特定范围内访问具体的外设寄存器，字节序可以保持与二者之一保持一致。

如果你的外设通常被映射为字节地址兼容，那么你应该按照字节操作进行编程。如果为了效率或者处于不得已，想要一次传输多个字节，你需要编写根据字节序进行打包和解包的代码。

如果你的外设与32位WORD兼容，通常按照总线宽度进行读写操作。那就是32位或64位的读写操作。

# 3 高速缓存可见性带来的问题

在之前的文章《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/) 》中，我们已经了解了初始化和正确操作Cache的方法。本段主要讲解一些可能出现的问题，并解释如何处理这些问题。

大部分时候，Cache对软件都是不可见的，只是一个加速系统执行的工具。也就是编程人员无需干预。但是，当需要处理DMA控制器及其类似的事物时，考虑把Cache作为一个独立的内存缓存会很有帮助，如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_9.PNG">

我们知道，Cache和内存之间的传输总是以16字节或32字节对齐的内存块作为传输单元。即使CPU只是读取一个字节，仍然会加载这样的内存块到Cache行中。

理想情况下，内存的状态与CPU请求的所有操作都是最新的，每个有效的Cache行都保存一份正确的内存备份。不幸的是，实际的系统根本就达不到这种理想的情况。假设每次复位之后都初始化缓存，并且也不存在《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/)》一文中提到的Cache重影问题。Cache和内存之间还是会存在数据不一致的问题。如下：

* Cache中的旧数据：

    当CPU向Cache的内存区写入数据时，它会更新Cache中的备份，同时写入内存。但是，如果通过其它方式更新了内存，那么Cache中的备份就有可能成为旧数据。比如，DMA控制器写内存，或者，CPU往内存中写入新指令，I-Cache继续保持原先的指令等。所以，编程人员应该注意，硬件是不会自动处理这些情况的。

* 内存中的旧数据： 

    当CPU写数据到Cache行中（回写），数据不会立即复制到内存中。稍后，CPU读取数据，直接读取Cache拷贝，一切没有问题。但是，如果不是CPU读取数据，而是其它控制器直接从内存读取数据，就会获取旧值。比如，向外传送的DMA。

为此，MIPS架构提供了Cache指令，可以根据需要调用它们，消除这种内存和Cache的不一致性。这些指令可以将最新的Cache数据写回到内存中，或者根据内存的最新状态失效对应的Cache行中的内容。

当然了，我们可以把数据映射到非Cache的内存区，比如`kseg1`区域。比如，网络控制器，映射一段非Cache的内存区保存读写的数据和标志位；这样可以方便快速的读取数据，因为不需要同步Cache和内存中的数据。相同的，内存映射的I/O寄存器，最好也映射到非Cache的内存区，通过`kseg1`或其它非Cache内存区中的指针进行访问。如果为I/O使用经过Cache的内存区，可能发生坏事情。

如果你需要使用TLB映射硬件寄存器，从而进行访问，你可以标记页转换为非Cache内存区（当然了，这不经常使用）。当I/O寄存器的内存地址不在低512M物理地址空间的时候，该方法是非常有用的。

还有的使用情况就是映射类似图像帧缓冲区为使用Cache的内存区，充分利用CPU的Cache充填和回写的block读写速度，提高像素帧的刷新频率。但是，每次图像帧的访问，都需要失效和回写Cache，显式地管理Cache。有一些嵌入式CPU，可能会提供一些奇怪但好用地Cache选项，请仔细检查对应芯片的手册。

## 3.1 Cache管理和DMA数据

Cache管理和DMA数据传输是一个很容易出错的地方，即使很有经验的编程者也常常会犯错。对此，不要犯怵；只要清晰地知道自己想干什么以及怎么干，就能让Cache和DMA传输正常工作。

比如，当从网络上接收到数据后，DMA设备会直接把数据存进内存，大部分MIPS系统不会更新Cache--即使某些Cache行中持有的地址落在DMA传输更新的内存区域中。随后，如果CPU读取这些Cache行的数据，将会读取Cache中旧的、过时的数据；就CPU而言，没有被告知内存中已经有了新数据，Cache中的数据仍然是合法的。

为了避免这种情况，你的程序必须在CPU尝试读取落在DMA缓冲区对应地址范围的数据前，主动失效对应Cache行中的内容。应该将DMA缓冲区的边界和Cache行的边界对齐，这样更容易管理。

对于通过DMA向外传输数据，比如网络通信，你必须在允许DMA设备传输数据之前，完全确保Cache中的数据都已经更新到对应的内存发送区域里了。也就是说，在你的程序写完需要由DMA发送的数据信息之后，必须强制写回所有的落在DMA控制器映射的内存地址范围的Cache行中的内容到内存中。只有这样，才能安全启动DMA传输。

有些MIPS架构CPU，为了避免显式的回写操作，配置为直写式Cache。但是，这种方案有一个缺点，直写式Cache会造成总体性能上更慢，也会增加系统的电源功耗。

当然，你也可以通过映射DMA的传输数据区到非Cache内存地址区，避免显式的失效和回写操作。这也是不推荐的一种方式，因为从整体上会降低系统的性能。因为使用Cache读写内存的速度肯定要快于直接从内存读取数据。最好的建议就是使用Cache，只有下面的情况避免使用Cache：

* I/O寄存器： 

     MIPS架构没有专门的I/O指令。 所以，所有的外设寄存器都必须被映射到一段内存地址空间上。如果使用Cache，会发生一些奇怪的事情。

* DMA描述符数组：

    复杂的DMA控制器和CPU共享控制和状态信息，这些信息保存在内存中的描述符数据结构中。通常，CPU使用这些描述符结构体创建一个待发送数据信息的列表，然后，只需告诉DMA控制器开始工作即可。如果你的系统使用描述符结构，请将其映射到非Cache内存地址区域。

移植性比较好的操作系统，比如Linux，不管是复杂的、不可见的Cache，还是简单的Cache，都能很好的适配。即，Linux一般提供一组很完备的API，供驱动编写者使用。

## 3.2 Cache管理和写指令数据

假设，我们想在程序的执行过程中，产生新的代码，然后跳转到新代码中执行。常见的示例有在线更新程序。必须确保正确的Cache行为。

如果不注意，这个过程中，可能会在两个阶段带来非预期的结果：

* 首先，如果你使用的是回写式D-Cache，你写的指令数据在没有触发相关Cache行的回写操作之前，一直停留在Cache中，并没有写入到内存中。如果，此时CPU尝试执行这些新的代码指令，因为仍然在D-Cache中，CPU无法访问到它们。所以，当CPU写完新指令数据后，首先要做的就是执行回写操作，保证数据写入到内存中。

* 其次，不管你使用的是哪种类型的D-Cache，在你把指令数据写入到内存中后，你的I-Cache里仍然持有着这些地址之前的数据。所以，在CPU执行新写的代码指令之前，软件首先应该失效I-Cache中的相关行。

当然了，你也可以使用非Cache区域保存新的代码指令，然后执行它们。但是，这毕竟放弃了Cache的加速效果不是。

我们在《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/)》一文中描述的Cache管理指令都是协处理器CP0指令，只有特权级的代码才能使用。一般情况下，DMA操作也是内核完成的，这些都没有异议存在。但是，当用户态的应用程序也想要这样写指令，然后执行的话（比如，现在的即时性的解释性语言），却无法访问这些指令。

所以，MIPS32/64提供了`synci`指令，它可以执行D-Cache的回写操作和I-Cache的失效操作。具体可以参考MIPS指令集参考。

## 3.3 Cache管理和非Cache或直写数据

如果你混合使用Cache和非Cache程序地址访问同一段物理内存空间，你需要清楚这意味什么。使用非Cache程序地址往物理内存中写入数据，可能会造成D-Cache或I-Cache中保留一份过时的拷贝（相同地址）。使用非Cache程序地址直接从内存中加载数据，可能是旧数据，而最新的数据还停留在Cache中。

上电复位后，在引导系统进入一个已知状态的底层代码中，使用Cache和非Cache程序地址引用同一段物理地址空间是非常有用，甚至是有非常有必要的。但是，对于运行中的代码，一般不要这样做。而且，不管是使用Cache程序地址，还是使用非Cache程序地址访问物理内存，一定要保证它的一致性。

## 3.4 Cache重影和页着色

我们在《[MIPS高速缓存机制](https://tupelo-shen.github.io/2020/06/10/MIPS%E6%9E%B6%E6%9E%84%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A34-Cache%E6%9C%BA%E5%88%B6/)》一文中已经描述了Cache重影的根源。L1级Cache使用虚拟地址作为索引，而使用物理地址作为Tag标签，如果索引的范围大于、等于2个page页，就可能发生Cache重影。索引范围等于一组Cache的大小，所以，使用4KB大小的page页的话，在8KB大小的直接映射Cache或着32KB大小的4路组相关的Cache上就可能会发生Cache重影。

发生Cache重影会有什么后果呢？在进程上下文切换的时候，必须首先清空Cache，要不然，上个进程映射的物理地址，可能与新进程映射的物理地址相同，导致同一物理地址在Cache上有2份拷贝，可能会导致意想不到的后果。再比如，使用共享内存的时候，多个进程的虚拟地址都可能引用这个数据，如果发生Cache重影，那么也会导致共享内存中的数据不正确。

为此，聪明的软件工程师们想了一个巧妙地技巧：`页着色技术`，又称为Cache着色，其实都是一回事，叫法不一样而已。具体的做法就是，假定page页的大小是4K，然后给每一个page页分配一个颜色（此处的颜色就是一种区分叫法而已，没有任何实际动作），使用虚拟地址的某几个比特位来标记颜色。当然，也可以选择使用物理地址中的某些比特位标记颜色。相同颜色的虚拟地址对应一组Cache。所以，两个虚拟地址想要指向同一个物理地址的数据，必须具有不同的页颜色。也就是说，页着色技术要求页分配程序把任一给定的物理地址映射到具有相同颜色的虚拟地址上。

> 颜色数是否与Cache的way数相等？应该是相等的。

比如说，Linux操作系统，多个虚拟地址可能都会访问一个物理页（共享库）。

大部分时候，操作系统OS对于共享数据的虚拟地址的对齐肯定满足要求-共享进程也可以不使用相同的地址，但是，我们必须保证不同的虚拟地址必须是64K的倍数，所以不同的虚拟地址具有相同的颜色。也就避免了Cache重影。这可能消耗更多的虚拟内存，但是虚拟内存又不值钱，对吧？😀

想象一下，加入数据都是只读的，Cache重影还会有影响吗？当然是没有什么问题了。但是，必须保证你的程序知道，在失效某个数据的时候，Cache的其它地方还有一份拷贝。

随着带有虚拟内存管理的操作系统OS在嵌入式和消费者电子产品市场的广泛应用，越来越多的MIPS架构CPU，在硬件层面就消除了Cache重影。相信随着时间的推移，这个问题也许就不存在了吧。

# 4 内存访问的排序和重新排序

程序员往往认为他们的代码是顺序执行的：CPU执行指令，更新系统的状态，然后继续下一条指令。但是，如果允许CPU乱序执行，而不是这种串行方式执行，效率可能更高。这对于执行load和store这种存储指令尤其重要。

从CPU的角度来看，执行store操作就是发送一个write请求：给出内存地址和数据，其余的交给内存控制器完成。实际的内存和I/O设备相对较慢，等write操作完成，CPU可能已经完成了几十条甚至几百条指令。

read操作又有不同：它需要发送一个read请求，然后等待对请求的响应。当CPU需要知道内存或者设备寄存器中的内容时，没有得到请求响应前，可能啥也做不了。

如果想要追求更高的性能，就意味着我们需要让read尽可能地快，甚至不惜让write操作变得更慢。进一步考虑，我们可以让write操作排队等待，把随后地任何read操作请求提前到write请求队列之前执行。从CPU地角度来看，这是一个大优势：尽可能快地启动read操作，就越早得到read操作的响应。然后，在某个时刻把执行write操作，而且write请求队列的大小是固定的。但是，这个write操作可能需要写Cache一段时间。如果这个队列满了，可能需要停下来等待一段时间，等待所有的write完成操作。但是，这肯定要比顺序执行，效率更高。这就是现代CPU一般都具有一个write buffer的原因。

看到这儿，你可能会有一个疑问：某些程序可能会写入一个地址，然后再将其读回来，这时候会怎么样呢？如果read提前到write之前执行，我们可能从内存中读取的是旧值，从而导致程序发生故障。通常，CPU会提供额外的硬件，比较read操作的地址和write队列中的地址，如果有相同的项，就不允许这样的read操作提前到write操作之前执行。

上面的讨论没有考虑真正的并发系统，比如多核系统。并发执行的任务间共享变量，对其执行read和write操作会非常危险。比如使用共享变量进行同步和通信的时候，内存访问次序就会非常重要。这种情况下，软件一般会采用精心的设计，比如锁和信号量，进行同步操作。

但是，使用共享内存，还有一些技巧，往往效果更好，开销也更小。因为不需要使用信号量或者锁。但是，可能会被乱序执行打断。假设，我们有2个任务，如下图所示：一个读取数据结构，一个写数据结构。它们可以交替使用这个数据结构。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_10.PNG">

为了能够正确执行，我们需要知道，对于reader任务来说，当什么时候reader任务看见关键域中的值发生了更新时，能够保证其它所有的更新对reader任务可见。

当然，硬件可以实现所有的内存访问顺序问题，从而将它们对程序员不可见，但是也就放弃了解耦read和write操作带来的性能优势。MIPS架构提供了`sync`指令实现这个目的，它可以确保sync指令之前的访问先于之后的执行。但是，这种保障指令有其局限性：只与内存的访问顺序有关，只能被非Cache或具有Cache一致性的内存访问的参与者看见。

对于上面的示例，为了让其在合适的系统上可靠地运行，writer任务应该在写关键域的值之前，调用一条sync指令；reader任务应该在读关键域的值之后插入一条sync指令。对于sync指令的详细使用方法，可以参考《MIPS指令集参考大全》一文。

不同的体系架构对执行顺序作出了不同的规定。一类极端情况就是，要求所有的CPU和系统设计人员努力保证一个CPU的全部读和写操作，从另一个CPU的角度看上去顺序完全相同，这叫做`强序`。也有一类情况就是`弱序`，比如只要求所有的写操作保持顺序不变。而MIPS架构更为激进，完全就是`无序`访问内存。这就要求我们系统开发人员必须手动保证内存的访问顺序是正确的。

## 4.1 访存顺序和写缓存

前面讨论了这么多理论，接下来让我们讨论点实际的内容吧。把write操作缓存到一个队列中（也就是硬件中常常讨论的write buffer）的思想在实践中证明非常有效。因为，store指令往往是多条指令扎堆出现。比如，一个运行MIPS代码的CPU，实际上运行的store指令大约占所有指令的10%左右；但是，往往是突发式访问，比如函数的调用过程中，首先需要压栈操作一组寄存器的值。

但是，一般情况下，写缓存（英文称为`write buffer`）都是硬件保证的，对于软件来说不用管理。但是，也有一些特殊的情况，程序员需要知道怎样处理：

1. I/O寄存器访问的时序 

    这个问题，对于所有架构CPU都存在。比如，CPU发出一个store指令，更新I/O设备寄存器的值，write请求可能会在写缓存中延迟一段时间。这时候，可能会发生其它事件，比如中断。但是此时写入的值还未更新到对应的I/O设备寄存器中。这可能导致一些奇怪的行为：比如，你想禁止产生中断，但是CPU发出write操作之后，CPU还有可能会收到中断。

2. read操作抢先于write操作执行 

    上面已经讨论过，MIPS32/64架构允许这种操作。如果想要软件更加健壮和具有可移植性，就不应该假定read和write操作顺序会被保持。如果想要保证前后两个指令周期是按照特定顺序执行，就需要插入sync指令。

3. 字节汇集

    有些写缓存会汇集不足WORD大小的write操作，凑成一个WORD大小的write操作，然后再执行（有些写缓存甚至会攒一个Cache行，然后再写入）。所以，为了避免对于非Cache的内存区也做相同的操作，最好的办法就是把I/O寄存器（比如，一个8位的寄存器）映射到一个单独的WORD大小的地址上。

## 4.2 写缓存的flush

通过对非Cache内存区的任意位置执行write操作，然后再read，可以清空写缓存（大部分都是这样实现的）。当然，写缓存不允许read操作发生在write之前，这样导致返回旧值。所以，必须在write和read操作之间，插入sync指令。对于兼容`MIPS32/64`规范的任何系统，这应该都是有效的。

但是，有效不等于高效。通过提高内存的读写速度也可以降低整体的负荷。有些特定的系统可能会提供更快的内存或者写缓存。

任何具有回写功能的处理器或者内存接口，都引入了写缓存。只是，有的在CPU内部实现，有的在CPU外部实现。不管是在CPU内部，还是在CPU外部，麻烦是相同的。在编程的时候，一定要仔细确认你的系统中，写缓存的位置，善加利用。

# 5 如何使用C语言调用汇编指令

你可能习惯了C或C++语言。MIPS架构缺乏特殊的I/O操作指令，这意味着，要想访问I/O寄存器，只能使用load或者store之类的指令，通过恰当的操作来实现。但是，I/O寄存器的访问有一些限制，因此，必须确保编译器不能太聪明，编译出了违背我们意愿的结果。另外，MIPS架构使用了大量的CP0寄存器，我们也可以使用C语言的伪汇编`asm()`方法进行操作。

## 5.1 封装汇编代码

对于GCC编译器，几乎是家喻户晓，其允许在C文件中封装汇编代码。当然了，其它编译器也支持，只是语法上不同罢了。在这儿，我们只以GCC进行举例；至于其它的编译器，请自行google或者baidu。如果，想要写一个高效计算的库函数之类的，可以使用纯MIPS汇编语言进行编写；但是，如果只是想在某个C文件中，插入一小段汇编语言，可以使用`asm()`伪指令实现。甚至，你可以让编译器根据一些约定，自行选择使用的寄存器。

比如说，下面的这段代码，调用乘法指令`mul`，就可以在绝大数的MIPS架构CPU上运行。我们可以注意到，`mul`指令后面跟着三个源操作数。如果我们直接使用C语言的`*`乘法操作符，生成的乘法汇编指令一般只使用两个操作数，而且隐含地将生成的double类型的结果保存到`hi/lo`寄存器中。

下面这段伪汇编代码实现的`mymul`乘法函数，使用了三目乘法指令`mul`，只保存double型结果的低有效部分到p变量中，高有效部分被抛弃。由我们自己决定如何避免溢出或者其它不相干的事情。

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

函数本身被声明为inline内联函数，这意味着应该使用该函数逻辑代码的拷贝去替代调用这个函数的地方的代码（这允许局部寄存器优化）。使用static进行限定，不允许其它模块文件调用该函数，所以，不会生成这个函数本身的二进制代码。封装asm()代码时，经常会这样干。然后，将这个伪汇编代码放到某个include文件中。当然，也可以使用C语言预处理宏来进行定义，但是，使用inline函数更简洁一些。

上面的代码，告知GCC，传递给汇编器一个MIPS的`mul`指令，具有三个操作数，一个是输出，两个是输入。

`%0`的意思就是指向索引为0的变量，也就是`p`。首先，我们使用`=`修改符指明这个值是`write-only`的；其次，通过符号`r`告诉GCC，可以自由选择任何一个通用寄存器保存这个值。

`asm()`中的第3行代码，告诉GCC，操作数`%1`和`%2`分别是`a`和`b`，并且允许GCC将其保存到任何通用目的寄存器中。

示例函数的最后，就是表明，把结果返回给调用者。

从上面的示例可以看出，GCC允许对操作数进行相当自由的控制。你可以告诉某个值可读可写，某些寄存器可能会留下毫无意义的值等。详细的使用方法可以参考GCC手册中关于MIPS架构的部分章节内容。

## 5.2 内存映射的I/O寄存器和volatile

因为在MIPS架构中，将所有的I/O寄存器映射到内存上，可以很容易使用C语言编写代码进行访问。所以，不到迫不得已，不要使用汇编语言操作这些I/O寄存器。我们已经说过，随着编译器的发展，或者在你的代码中使用了大量的C++代码，很难预测最终生成的汇编指令的顺序。下面我们将再谈论一些老生常谈的问题。

下面是一段代码，用来轮询串口的状态寄存器。如果准备就绪，就发送一个字符：

    unsigned char *usart_sr = (unsigned char *) 0xBFF00000;
    unsigned char *usart_data = (unsigned char *) 0xBFF20000;
    #define TX_RDY 0x40
    void putc (ch)
    char ch;
    {
        while ((*usart_sr & TX_RDY) == 0);
        *usart_data = ch;
    }

这段代码，编译器很可能将映射到内存上的寄存器变量`usart_sr`，视作一个不变的变量；而在while循环中也没有存储按位与表达式的结果的地方，编译器可能会自作主张的将其保存到一个临时变量中。最终，上面的代码可能等效于下面的代码。结果可能就是一直发送某个字符，也可能一直无法输出。

    void putc(ch)
    char ch;
    {
        tmp = (*usart_sr & TX_RDY);
        while (tmp);
        *usart_data = ch;
    }

为了避免这种情况，我们必须让编译器意识到，`usart_sr`是一个随时变化的值的指针，不能被优化。方法就是添加限定符`volatile`，如下所示：

    volatile unsigned char *usart_sr =
    (unsigned char *) 0xBFF00000;
    volatile unsigned char *usart_data =
    (unsigned char *) 0xBFF20000;

相似的情况，也可能发生在中断或者异常处理程序中要修改的变量身上。同样的，可以使用`volatile`进行限定。但是，你需要避免像下面的代码那样使用`volatile`：

    typedef char * devptr;
    volatile devptr mypointer;

本意是想告诉编译器，重新从`char *`类型的指针处加载数值，但是使用上面的方式，没有起到任何作用。应该如下所示，进行声明：

    typedef volatile char * devptr;
    devptr mypointer;

通过上面的讨论过程，我们可以看出使用C编写驱动程序要更容易一些，代码的阅读性也更好。但是，你需要充分理解硬件行为和工具链生成机器指令的方式，保证系统按照想要的行为进行工作。

## 5.3 在MIPS架构上使用C编写程序时的一些其它问题

* 负指针

    当在MIPS架构上运行比较简单的程序时，一般直接运行在非映射内存区，也就是`kseg0`或`kseg1`区域时，所有32位数据指针的最高位都置1，看起来像是一个负数。而在其它架构上，运行这种程序一般都在低于2G的内存地址上，也就是直接对应物理地址。所以，MIPS架构的这种负指针，如果对其进行比较运算的话，指针可能会隐式地被转为一个有符号的整数类型。所以，在进行指针和某个整数进行比较的时候，一定要显式地指定为无符号整数类型，比如`unsigned long`。大部分的编译器都会对指针向integer类型进行转换时给出警告。

* 有符号与无符号字符类型

    早期的C编译器，char类型一般用于string，通常是signed char类型；这与为了获取更大整数值的约定是一致的。但是，当处理超过127的字符编码时，比如转换或者比较，就会很危险。现代编译器一般都将char型等同于unsigned char类型。如果发现你的旧代码依赖于char类型的默认符号扩展，一定检查编译器是否有选项，恢复这个传统的约定。

* 16位int类型数据的使用 

    当我们从16位的机器架构的程序，比如x86或者ARM等，移植到MIPS架构上时，一定要注意最大值、溢出和符号位扩展。笨方法就是，直接将这些程序的int型替换成short类型，但这需要时间和耐心😊。大部分时候，可以直接使用MIPS架构的32位int类型替换。但是，需要特别注意的是signed类型比较时的bit16的溢出问题。

    还有就是，使用两个16位整型数拼凑成一个32位整型数时，一定要使用无符号16位整型数。笔者在移植ARM架构的操作系统到MIPS架构上时，就是使用了`signed short`类型的2个变量拼接成一个32位整数时，由于符号位扩展的原因（高16位全部被填充为1）导致高位数一直无法生效。

* 堆栈的使用

    尽管MIPS架构缺乏对堆栈的支持，但是MIPS-C编译器还是实现了一个常规的栈结构，主要就是按照某种约定，指定通用寄存器作一些特殊的用途，比如使用哪几个寄存器传递函数参数，使用哪个寄存器作为stack指针寄存器等等。话虽如此，不要想当然的认为，堆栈就可以安全的移植了。必要的时候，使用下面的2个方法-宏和库函数-解决堆栈的问题：

    - stdargs:

        使用头文件，定义宏，允许函数接收可变参数。

    - alloca(): 

        使用这个函数动态分配内存。有些编译器实现alloca()为内嵌函数，来扩展堆栈；也可以使用单纯的库函数实现。但是，不要假设堆栈和其分配的内存有什么关系。



# 1 引言

现代CPU中，为了提高CPU的执行效率，高速缓存必不可少。关于Cache工作原理可以参考我之前的文章[<Linux内核2-Cache基本原理>](https://tupelo-shen.github.io/2020/04/30/Linux%E5%86%85%E6%A0%B82-Cache%E5%9F%BA%E6%9C%AC%E5%8E%9F%E7%90%86/)

与ARM等架构相同，MIPS架构CPU也是采用多级cache。我们这里关心的是L1级缓存：I-cache和D-cache。通过这种哈弗结构，指令和数据读取可以同时进行。

# 2 Cache工作原理

从概念上讲，Cache是一个关联性内存，数据存入其中，可以通过关键字进行查找。对于高速Cache而言，关键字就是完整的内存地址。因为想要查询高速Cache，就必须与每一个关键字进行比较，所以合理使用Cache非常重要。

下图是一个直接映射的Cache，这是MIPS架构早期使用的Cache原型。这种原型的好处就是简单直接好理解，也是后来Cache发展的垫脚石。

它的工作方式就是，使用地址中的低位作为index，索引在Cache中的位置，也就是位于哪一行。当CPU发出某个地址后，使用地址中的高位与Cache中的tag位进行比较，如果相同，则称为"命中"；否则，"未命中"。命中，则将Cache中的数据拷贝到CPU寄存器中；如果没有命中，则重新从内存中读取数据，并将其加载到对应的Cache位置中。如果每行包含多个Word数据的话，则使用地址的最低几位进行区分。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_4_1.PNG">

这样直接映射的Cache有优点也有缺点：优点是，一一直接映射，遍历时间肯定短，可以让CPU跑的更快。但同时也带来了缺点，假设你正在交替使用2个数据，而且这2个数据共享Cache中的同一个位置（它们地址中的低位刚好一样）。那么，这2个数据会不断地将对方从Cache中替换出来，效率将急剧下降。

> 关于全关联Cache和直接映射Cache的概念可以参考文章[Linux内核2-Cache基本原理](https://tupelo-shen.github.io/2020/04/30/Linux%E5%86%85%E6%A0%B82-Cache%E5%9F%BA%E6%9C%AC%E5%8E%9F%E7%90%86/)


如果是全关联高速缓存不会存在这种问题，但是遍历时间变长，而且设计复杂。

折中的方案就是，使用一个2路关联Cache，效果相当于运行了两个直接映射的并行Cache。内存中的数据可以映射到这两路Cache中的任意一个上。如图4.2所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_4_2.PNG">

如图所示，对于一个地址，需要比较两次。四路组相关联的Cache在片上高速缓存中也比较常见（比如ARM架构）。

在多路相关联的Cache中，替换Cache中的哪个位置具有多路选择。理想的方法肯定是替换"最近最少使用"的缓存line，但是要维护严格个LRU原则，意味着每次读取Cache时，需要更新每个Cache line的LRU位。而且对于超过4路组相关联的Cache，维护严格的LRU信息变得不切实际。实际的缓存都是使用"最近最少填充"这样的折衷算法来选择要替换的缓存line。

当然，这也是有代价的。相比直接映射的Cache，多路组相关联的Cache在缓存芯片和控制器间需要更多的总线进行连接。这意味太大的Cache无法集成到单个芯片中，这也是放弃直接映射Cache的因素之一。

当运行一段时间后，Cache肯定就会填满，再获取新的内存数据时，需要从Cache中替换数据出去。这时候就要考虑Cache和内存的一致性问题。如果Cache中的数据比内存中的新，就需要将这些数据写回到内存中。

这就带来一个问题：Cache如何处理写操作。

# 3 早期MIPS架构CPU的write-though缓存

上面的讨论中，好像假定从内存中读取数据。但是CPU不只是读，还有写。write-though缓存就是不管三七二十一，数据总是写入内存中。如果数据需要在Cache中有一个备份，那么这个备份也要更新。这样做，我们不用管Cache和内存的数据是否一致。直接把Cache中替换出的数据丢弃即可。

如果写数据很多，我们等到所有的写操作完成，会降低CPU的执行速度。不过这个问题可以修正，通过使用一个称为write-buffer（写缓冲器）的FIFO方式的缓存区保存所有要写入内存的数据。然后由内存控制器读取数据，并完成写操作。

早期的MIPS处理器有一个直接映射的Cache和一个write-buffer。只要主存系统能够很好地消化这些以CPU平均速率产生的写操作即可。但是，CPU的发展速度太快了。很快，CPU的速度就超过了主内存系统可以合理消化CPU所有写操作的临界点了。

# 4 MIPS CPU的回写高速缓存

尽管早期的MIPS架构的CPU使用简单的透写Cache，但是，CPU的频率提升太快了，很快透写Cache就无法满足CPU写操作的需求，大大拖慢了系统的运行速速。

解决方案就是把要写的数据保存到Cache中。且在对应的Cache行中标记dirty位。后面根据这个标志位再写回到相应的内存中。如果当前Cache中没有要写入地址对应的数据，我们可以直接把数据写到内存中，不用管高速缓存。

# 5 高速缓存设计中的一些其它考虑

上世纪的80年代后期和90年代初期，研究人员开始研究如何构建Cache。下面我们看看，Cache的发展都经历了哪些历程：

* 关于物理寻址/虚拟寻址：

    我们知道，CPU正在运行OS时，应用程序的数据和指令地址都会被转换成物理地址，继而访问实际的物理内存。

    如果Cache单纯地工作在物理地址上，很容易管理（后面我们再介绍为什么）。但是，虚拟地址需要尽快的启动Cache遍历才能使系统运行的更快些。虚拟地址的问题在于，它们不是唯一的：运行在不同地址空间的应用程序可以共享某段物理内存而存储不同的数据。每当我们在不同地址空间进行上下文切换时，都需要重新初始化整个Cache。在过去的许多年，对于小容量的Cache，这都是常见的处理方式。但是，对于大容量的Cache，这不太高效，而且还要在Cache的tag中包含一个标识地址空间的标识符。 

    对于L1级Cache，许多MIPS架构CPU使用虚拟地址作为快速索引，使用物理地址作为Cache中的tag，而不是使用虚拟地址+地址空间标识符作为cache行的tag标签。这样的设计，Cache行中的物理地址是唯一的，CPU在遍历Cache的同时也就完成了虚拟地址到物理地址的转换，

    采用物理地址作为tag，还有一个微妙的问题：不同任务中的地址不同（虚拟地址不同）。因为使用虚拟地址作为Cache索引，所以相同的物理地址有可能在Cache中存在不同的项。这样的现象称为Cache重影（`Cache alias`）。许多MIPS架构的CPU硬件上没有这种检测机制，避免Cache重影，而是留待OS内存管理者去解决这个问题。

* 关于Cache行大小的选择：

    Cache行大小指的是一个tag标签对应的32位长度的数据个数。早期MIPS的缓存使用一个tag对应一个word的方式，但是，通常每个tag对应多个word数据更有利，尤其是内存管理系统支持burst读取方式时。现在MIPS架构CPU倾向于使用4个word大小或者8个word大小的Cache行。但是对于L2和L3级Cache来说，它们会使用更大的行。如果发生Cache丢失，整个行都会被填充。

* 关于指令和数据Cache： 

    MIPS架构L1级Cache总是分为I-Cache和D-Cache。这是因为指令和数据的性质决定的，指令是只读的，数据是可读写的，分开为两个Cache可以提高读写效率。但是，L2级缓存很少这样划分，毕竟硬件过于复杂且昂贵。

# 6 管理Cache

在之前的文章中，我们已经知道MIPS架构的CPU有两个固定大小的512MB的内存空间可以映射到物理内存上。其中，一个称为kseg0，另一个称为kseg1。通常，OS运行在kseg0上，而使用kseg1构建那些不需要经过Cache的物理内存访问。如果物理内存的实际空间大于512MB，其超出部分使用这两个内存空间是无法访问的。当然，你可以设置TLB（转换表）进行地址映射，每个TLB项都有标志表明是否经过Cache。

要想在程序中使用Cache，必须经过正确的配置，保证有无Cache，对于物理内存的访问，尤其是DMA访问，都必须是正确的。

X86架构和ARM架构，Cache的管理是硬件实现的，无需编程人员的干预Cache的一致性。不幸的是，MIPS架构因其设计理念不同，Cache还需要编程人员通过代码保证其一致性。

上电后，Cache的内容是随机的，必须进行初始化才能够使用。一般情况下，引导程序负责这部分初始化工作，这是一个非常复杂的配置过程。一旦CPU运行起来，只有三种情况需要CPU进行干预，如下所示：

* DMA设备从内存读取数据之前：

    假设一个外设使用DMA方式从内存读取数据，这一步就非常重要了。如果D-Cache是write-back类型的，有程序在DMA访问之前修改了D-Cache中的内容，还没有写回到内存中；恰巧这些数据是DMA访问的数据。这种情况下，CPU无法知晓它应该从Cache上获取最新数据。

    所以，在DMA设备启动从内存读取数据之前，如果所访问的数据在Cache中，必须写回到内存中，不论是不是被修改过。

* DMA写数据到内存中：

    如果DMA设备想要加载数据到内存中，失效Cache中相关数据是非常重要的。否则，CPU读到的就是过时的数据。必须在CPU使用DMA数据之前失效Cache项，但常见做法是DMA启动之前就失效Cache。

* 指令写入：

    当CPU写指令代码到内存中，然后再执行它们。所以，再运行之前，必须保证所有的指令都写回到内存中，并且保证失效对应的I-Cache项。MIPS架构CPU的D-Cache和I-Cache没有任何关系。

    在最新的CPU上，一个用户特权级的指令`synci`保证必要的同步，是你刚刚保存的指令可以运行。

现代CPU都在朝着硬件管理的方向发展，所以，这一部分仅供参考。

# 7 L2和L3两级Cache

L1级Cache一般小而快速，与CPU核紧密相关；L2级和L3级Cache在容量和访问速度上介于内存和L1级Cache之间。目前为止，一般就使用到L2级缓存。

# 8 Loongson2k1000的Cache配置

lonngson2k1000的结构示意图如下所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_4_3.PNG">

一级交叉开关连接两个CPU核（C0和C1）、两个二级Cache（S0和S1）以及IO子网络（Cache访问路径）。二级交叉开关连接两个二级Cache、内存控制器（MC）、启动模块（SPI或者LIO）以及IO子网络（Uncache访问路径）。IO子网络连接一级交叉开关，以减少处理器访问延迟。IO子网络中包括需要DMA的模块（PCIE、GMAC、SATA、USB、HDA/I2S、NAND、SDIO、DC、GPU、VPU、CAMERA 和加解密模块）和不需要DMA的模块，需要DMA的模块可以通过Cache或者Uncache方式访问内存。

# 9 对MIPS32/64高速缓存的编程

兼容MIPS32/64架构的CPU一般具有write-back功能的高速Cache。要想对Cache进行编程，硬件必须具有下面的操作可能：

* 失效某个Cache区域：
    
    将某个地址范围内的数据清除出Cache，这样下一次引用的时候就会从内存中重新读取数据。

    指令`cache HitInvalidate`形式和`load/store`指令一样，给出一个虚拟地址。将虚拟地址所引用的Cache行失效。在地址范围内每个一个Cache行大小的地址上重复该命令。

* 回写某段内存： 

    将该地址范围内的已经被修改的Cache行写回到主内存中。`cache HitWritebackInvalidate`将虚拟地址引用的数据所在的Cache行写回到主内存中，并失效这个Cache行。

* 失效整个Cache： 

    指令为`cache IndexInvalidate`。其虚拟地址作为索引参数，按照Cache行大小从0开始递增。

* 初始化Cache：
    
    设置tag通常涉及到对CP0 TagLo寄存器清零，以及对每一个Cache行执行`Cache IndexStoreTag`指令操作。数据校验位的预填充可以使用指令`cache Fill`。

#### 9.1 Cache指令

Cache指令的使用方式跟load/store指令类似，使用通用寄存器+16位有符号地址偏移的方式。但是添加了一个操作域（5位），用来决定操作那个Cache，如何查找line，以及对line做什么处理。你可以使用汇编直接编写相应的操作，最好还是使用宏定义之类的C方法，更为方便和易于阅读。

操作域中的高2位指定操作哪个Cache：
    
    0 = L1 I-cache
    1 = L1 D-cache
    2 = L3 cache, if fitted
    3 = L2 cache, if fitted

将Cache相关命令可以分为3类：

* hit型操作：

    给出一个地址，从Cache中查找，如果找到则执行操作；否则什么也不干。

* 寻址型操作：

    如果寻址的数据不在Cache中，则从内存中加载对应的数据到Cache中。

* 索引型操作：

    根据需要用虚拟地址的低位选出在Cache行中的一个字节，然后是Cache某路中的Cache行地址，然后就是第几路。具体格式如下，所示

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_4_4.PNG">

一旦选中了某行，CPU所能做的操作如下表4-2所示。但是，与MIPS32/64兼容的CPU必须实现基本的三个操作：索引型失效、`Index Store Tag`、和`Hit Writeback invalidate`。

表4-2 Cache行上的可用操作

| 取值 | 命令 | 功能 |
| ---- | ---- | ---- |
| 0    | Index invalidate | 失效行。如果Cache行被修改过，<br> 则将数据写回到内存。<br> 当初始化Cache的时候，<br>这是最好的方法。<br> 如果Cache被奇偶保护，<br>还需要填充正确的校验位。<br> 参见下面的Fill命令。|
| 1    | Index Load Tag | 读取Cache行的Tag，<br>数据加载到TagLo/TagHi寄存器。<br> 几乎不用 |
| 2    | Index Store Tag | 设置tag。<br> 初始化时必须<br>每行都执行该操作。|
| 3    | Hit invalidate | 失效但不回写数据。<br> 有可能会丢失数据。 |
| 4    | Hit Writeback <br> invalidate | 失效且回写数据。<br> 这是常用失效Cache方法。 |
| 5    | Fill | 填充Cache行的数据位。<br> 奇偶保护时使用。 |
| 6    | Hit writeback | 强制回写内存。|
| 7    | Fetch and Lock | 正常的加载Cache操作。 |

#### 9.2 Cache初始化和Tag/Data寄存器

为了诊断和维护，最好能够读写Cache的Tag标签。为此，MIPS32/64定义了一对32位寄存器：`TagLo`和`TagHi`，使用它们对Cache的Tag部分进行管理。大部分时候TagHi寄存器不是必须的：除非你的物理内存地址空间超过36位；否则，只使用TagLo寄存器就足够了。

> 有些寄存器可能使用不同的寄存器分别与I-Cache和D-Cache进行通信，比如ITagLo等。

这两个寄存器反映了Cache的Tag标签位，具体实现依赖于CPU的实现。唯一可以保证的事情是：如果寄存器中的值全为0，代表一个合法的数据Tag，表示相应的Cache行中不含有效数据。所以，这是一个初始化Cache的方法：设置寄存器的值为0，存储Tag值到Cache中，就可以使Cache从未知状态进入初始化状态。

MIPS32/64定义了store和load两种Cache操作，但它们是可选的，具体依赖于CPU实现。

Tag包含Cache索引之外的所有必须位。所以，L1级Cache的Tag长度等于物理地址位数减去L1级Cache索引的位数（对于一个16K大小的4路组相关联Cache来说，索引的位数是12位。）。TagLo（PTagLo）可以容纳24位，这样一个高速Cache最多可以支持36位。

TagLo（PState）包含状态位。

####9.3 CacheErr、ERR和ErrorEPC寄存器：Memory/Cache错误处理

CPU的Cache是内存系统的重要组成部分。对于高可靠性系统来说，监视Cache数据完整性是非常有必要的。

数据完整性检查理想上来说，应该是end-to-end：数据产生或者进入系统时就一起计算校验位，一起存放；在数据使用之前，再次执行检查。这样的检查不仅能够发现存储器的错误而且能够发现复杂的总线错误，和数据与CPU的交互过程中产生的错误。

基于这个原因，MIPS架构CPU通常在Cache中提供错误检查。如同主内存一样，既可以使用简单的奇偶校验，也可以是复杂的ECC错误纠错码。

MIPS架构的CPU内存系统是以8位宽度为最小处理单元，所以内存模块提供64位数据检查和8位校验位。也可以将数据校验简单理解为，系统每8个字节产生一个额外的位。

一个字节的错误数据有50%的概率产生正确的校验位，所以，在64位总线上的随机垃圾数据及其校验位每256次中将有一次逃脱检测。有些系统要求比这更高。

ECC的计算更为复杂，因为它涉及整个64位数据和8位校验位。ECC是排除内存系统中随机错误的一个强大工具。

理想情况下，存储系统采用什么样的校验机制，Cache也应该采用相同的机制。根据CPU的不同，可以采用奇偶位、纠错码ECC，或者什么都不做。

如果发生错误，CPU产生特殊的错误陷阱。这时，如果Cache包含坏数据，异常向量位于非Cache的地址空间上（Cache发生了错误，还在上面运行是不是很愚蠢？）。如果系统使用ECC，硬件要么自己纠正错误，要么提供给软件足够多的信息去修复数据。

CacheErr寄存器的域取决于具体实现，需要查阅CPU手册。

####9.4 设置Cache大小和配置

对于兼容MIPS32/64架构的CPU，Cache大小，Cache结构，行大小都可以通过协处理器0（CP0）的Config1-2寄存器获取。

#### 9.5 初始化程序

通用的初始化程序示例：

1. 开辟一些内存用来填充Cache，至于内存中存储什么数据没有关系。如果开启了奇偶校验或ECC功能，还需要填充正确的奇偶校验位。一个很好的技巧就是保留内存的低32K空间，用来填充Cache，直到完成Cache初始化。如果使用不经过Cache的数据填充，自然包含正确的校验位。

    对于较大的L2级Cache，这个32K的空间可能不够，需要做一些其它的处理。

2. 将TagLo寄存器设为0，对Cache进行初始化。`cache IndexStoreTag`指令使用TagLo寄存器强制失效一个Cache行并清除Tag奇偶位。

3. 禁止中断

4. 先初始化I-Cache，然后是D-Cache。下面是I-Cache初始化的C代码部分。 

        for (addr = KSEG0; addr < KSEG0 + size; addr += lnsize)
            /* 清除Tag并失效 */
            Index_Store_Tag_I (addr);
        for (addr = KSEG0; addr < KSEG0 + size; addr += lnsize)
            /* 填充一次，数据校验是正确的 */
            Fill_I (addr);
        for (addr = KSEG0; addr < KSEG0 + size; addr += lnsize)
            /* 再次失效，谨慎但并非严格必要 */
            Index_Store_Tag_I (addr);

5. D-Cache初始化代码。D-Cache侧没有fill等价的操作，所以只能使用Cache空间对其进行加载，完全依赖于正常的miss处理过程。

        /* 清除所有的Tag */
        for (addr = KSEG0; addr < KSEG0 + size; addr += lnsize)
            Index_Store_Tag_D (addr);
        /* 加载每行（使用Cached地址空间）*/
        for (addr = KSEG0; addr < KSEG0 + size; addr += lnsize)
            junk = *addr;
        /* 清除所有Tag */
        for (addr = KSEG0; addr < KSEG0 + size; addr += lnsize)
            Index_Store_Tag_D (addr);

#### 9.6 失效或回写Cache对应的存储区

这一步时，注意访问的地址参数一定是合法的，比如一些I/O设备内存的地址或物理内存地址。

几乎总是使用命中型操作来失效Cache或者回写内存。大于大块地址空间，也许使用索引型操作更快。这个需要自己选择。

简单示例：

    PI_cache_invalidate (void *buf, int nbytes)
    {
        char *s;
        for (s = (char *)buf; s < buf+nbytes; s += lnsize)
            Hit_Invalidate_I (s);
    }

只要buf是程序地址，就没有必要使用特殊地址。如果直接使用物理地址p来进行失效，只要p在物理空间的低512M空间内，加上常量0x80000000就对应kseg0地址空间：

    PI_cache_invalidate (p + 0x80000000, nbytes);

# 10 Cache效率问题

（这一部分的内容主要是芯片设计者应该考虑的问题）

据研究表明，CPU的性能很大程度上取决于其Cache的性能。尤其是嵌入式系统，需要在Cache和内存性能上节约。CPU大概有50%-65%的时间在等待Cache重填。

而系统等待Cache重填的时间取决于两个因素：

* 平均指令的Cache未命中率：

    Cache未命中数除以执行的指令数。其实，每千条指令的Cache未命中数是一个更有用的度量标准。

* Cache未命中/重填的开销：

    内存系统重填Cache并重启CPU执行所花费的时间。

我们为什么把Cache的未命中率定义为`平均指令的Cache未命中率`，而不是`平均CPU访问内存的未命中数`。那是因为Cache未命中率的影响因素有很多，有一些甚至无法预料。比如，x86架构CPU的寄存器个数比较少，同样的程序，基于x86架构编译就比MIPS架构编译多产生load和store操作（比如，函数传递较多参数时）。但是，x86编译器会将额外的load和store操作作用到堆栈上，代替使用寄存器。那么这一小段内存的使用率就会非常高，Cache的效率也就更高。所以，总体上来说，每千条指令的未命中数受影响的差别没那么大。

下面我们列举一些提高系统运行的方法：

* 降低Cache未命中数

    - 增大Cache。但是代价高昂。64K大小的Cache可能比CPU其它所有部分（不包含FPU浮点单元）所占据的硅面积都大。
    - 增加Cache的组关联度。但是4路组Cache之后，再增加对性能几乎影响不大。
    - 再增加一级Cache。代价也很大。这也是目前都是两级Cache的原因。
    - 通过软件降低Cache未命中数。小程序容易实现，规模化的程序很难实现。

* 降低Cache重填的开销

* 尽可能快的重启CPU执行

* 不到万不得已不要停止CPU执行：

* 多线程化CPU

    Cache-miss延时对程序性能的不利影响可以被抑制但是无法避免。所以，更为有效的方法就是在CPU上运行多个线程，这样可以在彼此等待期间使用空闲的CPU资源。

# 11 软件对Cache效率的影响

在运行大量应用程序的复杂系统中，Cache性能是一个性能平衡的过程，不太好优化软件提高执行效率。但是对于单个应用程序的嵌入式系统中，可以通过一些特殊的处理提高其性能。在分析之前，我们先把Cache未命中数按照产生的原因进行分类：

* 第一次访问 

    必然都是cache-miss。

* 替换 

    不可避免，在程序的运行过程中，需要不断地从Cache中替换、重填数据。

* 抖动

    在四路组相关联的Cache中（更多路的情况在MIPS架构CPU中很少见），有四个位置可以保存特定内存位置上的数据。（对于直接映射Cache，仅有一个）。
    
    假设你的程序大量使用几段地址的代码，这些地址有一个特点就是它们的低位都非常接近，也就是使用相同的Cache行。那么如果这几段地址的个数大于Cache的组关联度时，它们之间就会频繁发生互相替换的情况，从而降低系统性能。（大多数研究表明，大于四路组关联的Cache，性能基本上不再提高）。

通过上面的了解，解决Cache效率的可以分为下面几个方面：

* 使程序更加精简

    这应该是一个程序员的至高境界。但是很难做到。大部分时候也就是通过编译优化选项实现，但是过度优化往往会增大程序。

* 使频繁调用的代码更加短小

    一种方法是，分析代码在典型应用下的使用最频繁的函数，然后按照函数的执行时间递减顺序在内存中排列函数。这意味着频繁使用的函数不会相互争夺Cache的位置。

* 强制一些重要常用的代码常驻Cache 

    该方法怀疑中。

总之一句话，软件要提高Cache的效率太难了，让硬件去实现吧。

# 12 Cache重影

这个问题是怎么产生的呢？这是因为，对于MIPS架构的CPU的L1级缓存来说，通常使用虚拟地址作为索引，物理地址作为Tag标签。这样可以提高性能：如果我们使用物理地址作为索引，只有等通过TLB转换成实际的物理地址后，我们才能遍历Cache。但是这导致了Cache重影。

大部分CPU转换地址的单元是4K大小。这意味虚拟地址的低12位无需转换。只要你的Cache不超过4KB，虚拟地址的索引和物理地址的索引是相同的，这是OK的。其实，只要是Cache的跨度单元不超过4KB就没问题。在组相关联的Cache中，每个索引访问几个内存点（一路对应一个内存点）。所以，即使是16kB大小的Cache，虚拟地址的索引也是与物理地址的索引相同的，不会发生Cache重影。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_4_5.PNG">

但是设想一下，如果Cache的索引范围达到8K的情形（32k四路组相关联的Cache就是这样）。你可能会从两个不同的虚拟地址访问同一个物理页：例如虚拟地址可能是连着的两页-地址分别从0和4K处开始。这样，如果程序访问地址0处的数据，就会加载该数据到某个Cache索引为0的位置。如果此时再从另一个地址4K处访问同一个物理内存上的数据，再次从内存中取出数据加载到Cache索引为4K的位置。现在，对于同一数据在Cache中存在两个备份，彼此之间无法知晓。这就是Cache重影。如果仅仅是只读还好。但是，如果正在写的数据重影就很可能导致灾难性的后果。

MIPS架构的L2级Cache总是使用物理地址索引和Tag，所以不会存在Cache重影。

总结来说，重影的出现是L1级Cache在设计之初就引入的一个潜在问题。但是，如果使用了L2级Cache，就不会出现问题。如果没有使用L2级Cache，则只要保证虚拟地址到物理地址的映射间隔是64K的整数倍既可以。



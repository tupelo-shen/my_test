本章我们从硬件底层开始，首先研究TLB机制以及如何设置。在此基础上分别研究裸机程序和操作系统下内存管理机制。

# 1 TLB/MMU硬件

TLB是把程序地址或者虚拟地址转换成物理地址的硬件电路。地址转换是实现安全OS的安全特性的关键。

基于MIPS架构的CPU，转换页表项的大小是4K，我们称之为页（page）。虚拟地址的低12位是在物理内存上的偏移量，换句话说，虚拟地址的低12位等于物理地址的低12位。其实，MIPS架构的CPU完全支持访问更大的页（大于4K），这对于特殊场景下的应用很有用。尤其是现在AI人工智能这么火，许多应用直接把算法和模型数据存放到内存上，需要使用上百G、甚至更大的内存。这时，就需要访问更大的页。但是，本文不讨论更大的页表转换。

转换表中的每一项包含一个VPN（虚拟地址页编号）和一个PFN（物理页帧编号）。当程序给出一个虚拟地址后，和TLB中的每一个VPN进行比较，如果匹配，就给出对应的PFN。具体的比较复杂，由硬件电路实现。所以，通常TLB只有16到64项。

转换表中的每一项除了VPN之外，还包含一些标志位。这些标志位允许OS控制实际的物理地址的属性，比如只读（read-only），或指定数据可以被缓存，也就是存储到Cache中。

现代MIPS架构CPU为了效率，在上面的基础上进行扩展，将每个TLB项包含两个独立的物理页帧，由两个连续的虚拟地址页进行映射。也就是说，每一个TLB项，包含1个VPN和2个PFN，因为虚拟地址是连续的，所以VPN自动加1访问下一个物理内存页。

图6-1是一个兼容MIPS32/64规范的TLB项定义。它们的控制使用协处理器0（CP0）寄存器实现。图中的标签就是寄存器的名称。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_6_1.PNG">

在实际的使用过程中，TLB太小了，肯定不能包含所有虚拟地址到物理地址的映射。所以，软件只是把TLB作为最近常用地址转换的一个缓存。当需要的地址转换不在TLB中时，产生异常，异常处理程序计算并加载正确的地址转换关系。这样做，效率肯定包含所有的地址映射效率高，但这是一个综合平衡的结果。

# 2 TLB/MMU寄存器

上面描述了TLB的工作机制，那么想要控制它实现我们想要的地址转换，就必须有控制它的寄存器和相应的指令。相关寄存器位于协处理器0（CP0）中。下面我们会一一描述这些寄存器：

1. EntryHi：在协处理器0中的编号为`10`。

2. EntryLo0-1：在协处理器0中的编号为`2/3`。

3. PageMask：在协处理器0中的编号为`5`。

    1-3项描述的这些寄存器一起构成了一个TLB项所需要的一切。所有对TLB的读写都要经过它们。其中，EntryHi存有VPN和ASID。ASID域具有双重职责，因为它还记录了当前进程的地址空间标识符。64位CPU中，EntryHi扩展到64位，但是对于32位软件仍然保持32位布局不变。兼容MIPS32/64规范的CPU每一项映射2个连续的VPN到2个不同的PFN上，物理内存页的PFN和存取权限标志由EntryLo0和EntryLo1两个寄存器单独指定。PageMask可以用来创建映射大于4K的页。

4. Index：在协处理器0中的编号为`0`。

    对TLB项的索引。操作指令靠这个寄存器寻址TLB项。

5. Random：在协处理器0中的编号为`1`。

    产生伪随机数（实际上是一个自由运行的计数器）。表示`tlbwr`指令写新TLB项时随机指定的位置。当在异常处理中，重新填充TLB时，随机替换TLB表项使用。可以节省时间。

6. Context：在协处理器0中的编号为`4`。
7. XContext：在协处理器0中的编号为`20`。

    6-7两项描述的寄存器是辅助寄存器，用来加速TLB重填陷进的处理过程。
    These are convenience registers, provided to speed up the processing of  TLB refill traps. The high-order bits are read/write; the low-order bits are taken from the VPN of the address that couldn’t be translated. The register fields are laid out so that, if you use the favored arrangement of memory-held copies of memory translation records, then following a TLB refill trap Context will contain a pointer to the page table record needed to map the offending address. See section 6.2.4. XContext does the same job for traps from processes using more than 32 bits of effective address space; a straightforward extension of the Context layout to larger spaces would be unworkable because of the size of the resulting data structures. Some 64-bit CPU software is happy with 32-bit virtual address spaces; but for when that’s not enough, 64-bit CPUs are equipped with “mode bits” SR(UX), SR(KX), which can be set to invoke an alternative TLB refill handler; in turn that handler can use XContext to support a huge but manageable page table format.

####2.1 TLB关键域描述1

下面对关键的域进行描述：

1. VPN2（虚拟页编号）

    程序地址或虚拟地址的高位（低位0-13略去）。其中，位0-12属于页内偏移，但是位12并不参与查找。每一页映射大小为4K的页，位13自动在两个可能的输出值之间进行选择。

    `refill`异常发生后，将自动设置此域，以匹配无法转译的程序地址或虚拟地址。如果想要不同的TLB项或尝试TLB探测时，必须手动进行设定。

    MIPS32/64规范中允许EntryHi的最大虚拟地址区域，达到64位，然而当前的通用CPU只能实现40位。如果你想要知道可用的虚拟地址多大，可以将EntryHi寄存器全写1，然后重新读回，还为1的位就是有效位。

    VPN2中超过CPU实际使用的高位地址必须全写0或者1，和R域的最高有效位要匹配。也就是说，核心态使用地址高位必须全为1，否则全为0。

    如果使用的是32位指令集，这一切自动发生，不需要我们管理。因为这种工作模式下，所有的寄存器包含的值都是一个32位数的64位有符号扩展。你可以把它理解成就是一个32位寄存器。

    从图6-2中，可以看出还有一些位填充为0：这些位并不是没用，有些CPU可以配置支持1KB大小的页，这样VPN2的位需要向下扩展2位。

2. ASID（地址空间标识符）

    这一部分的作用同ARM架构的ASID作用是一样的。可以保证应用程序的地址空间互相隔离，不受彼此的影响。

    异常不会影响该域，所以refill异常之后，该域对当前的进程仍然是有效的。支持多进程的操作系统使用该域表示当前有效的地址空间。但是在使用指令`tlbr`指令检查TLB项的时候必须十分小心：该操作会重写整个EntryHi寄存器，所以执行之后，必须恢复正确的当前ASID的值。

3. R（64位版本才有）

    | R值 | 区域名 | 描述 |
    | --- | ------ | ---- |
    |  0  | xuseg  | 用户态可访问的虚拟存储器的低地址区 |
    |  1  | xsseg  | 管理态可访问的空间（管理态是可选的）|
    |  2  | xkphys | Core专用的大物理内存窗口 |
    |  3  | xkseg  | 核心态空间 |

4. PageMask：

    寄存器允许设置TLB域来映射更大的页。

####2.2 TLB关键域描述2-EntryLo0-1

图6-3分别展示了64位和32位版本的EntryLo寄存器：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_6_3.PNG">

1. PFN

    保存物理地址的高位，和VPN2中的高位是映射关系。MIPS32架构的CPU外部物理内存的接口限制到2^32字节的范围，但是EntryLo潜在支持多达2^38字节的物理范围（26位的PFN，支持2^26个物理页，每个大小4K）。

2. C

    A 3-bit field originally defined for cache-coherent multiprocessor systems to set the “cache algorithm” (or “cache coherency attribute”— some manuals call this field CCA). An OS will typically know that some pages will not need to have changes tracked automatically through multiple caches—pages known to be used only by one CPU, or known to be read-only, don’t need so much care. It can make the system more efficient to turn off the cache snooping and interaction for accesses to these pages, and this field is used by the OS to note that the page is, for example, cacheable but doesn’t need coherency management (“cacheable noncoherent”).

    But the field has also been used in CPUs aimed at embedded applications, when it selects how the cache works—for example, marking some particular page to be managed “write-through” (that is, all writes made there are sent straight to main memory as well as to any cached copy). The only universally supported values of this field denote “uncached” (2) and “cacheable noncoherent” (3).

3. D（脏位）

    写内存使能标志位。1，写使能允许；0，如果尝试写，产生陷阱。

4. V（合法标志位）

    如果为0，尝试访问该地址都导致异常。用来设置某个物理地址不可访问。

5. G（全局标志）

    置1时，TLB项只匹配VPN域内容，不管ASID域的内容是否匹配EntryHi中的值。这样提供了一种机制，可以实现所有进程共享某个地址空间。

    需要注意的是，虽然有两个EntryLo寄存器，但是只有一个G位，如果EntryLo0的G位和EntryLo1的不同，那就会坏事。

6. 未使用的PFN位和填充为0的位

    无论填写0或1，硬件统统忽略。

#### 2.3 选择TLB项的寄存器

1. Index寄存器

    取值范围0~总项数-1。具体读一项的时候，手动设置Index；如果使用tlbp搜索某个TLB项时，Index会自动增加。Index不需要使用很多位，目前为止，MIPS架构的CPU没有超过128项。bit31有特殊意义，当检测到未匹配项的时候由tlbp置位。使用bit31，看起来像负数，很容易测试。

2. Random寄存器

    保存到TLB中的索引，CPU每执行一次指令就计数一次（向下递减计数）。写数据指令tlbwr使用这个值作为写入TLB中的位置。可以用来实现随机替换策略。

    正常使用时，不需要读写Random寄存器。硬件复位时，将Random设置为最大值-TLB项的最高编号，每个时钟周期递减计数，递减到最小值后又回绕到最大值，周而复始。

3. Wired寄存器

    有时候，我们可能需要一些永久转换的地址项，基于MIPS架构的OS文档中一般称为wired。设置了Wired寄存器后，凡是索引值小于该值的表项不受随机替换的影响。写入Wired寄存器时，Random寄存器被复位到TLB顶部。

#### 2.4 页表存取辅助寄存器-Context和XContext寄存器

当给出的虚拟地址不在TLB表中时，CPU发生异常，未能转换的虚拟地址已经在BadVAddr寄存器中了。虚拟地址中对应VPN域的位也会被写入到EntryHi（VPN2）中，从而为`未命中`的地址建立新的TLB项。

When the CPU takes an exception because a translation isn’t in the TLB, the virtual address whose translation wasn’t available is already in BadVAddr. The page-resolution address is also reflected in EntryHi(VPN2), which is thereby preset to exactly the value needed to create a new entry to translate the missed address.

为了进一步加速这种异常的处理过程，Context或XContext寄存器用来
But to further speed the processing of this exception, the Context or XContext register repackages the page-resolution address in a format that can act as a ready-made pointer to a memory-based page table.

MIPS32架构的CPU只有Context寄存器，可以帮助填充32位的虚拟地址。MIPS64架构的CPU增加了XContext寄存器，用来扩展虚拟地址空间（达到40位。如图6-4所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_6_4.PNG">

Note that XContext is the only register in which the MIPS64 definition does not exactly define field boundaries: the XContext(BadVPN2) field grows on CPUs supporting virtual address regions bigger than 240 bits and pushes the R and PTEBase fields left (the latter is squashed to fit). Section 6.2.1, which describes Figure 6.2, explains how to find out how many bits are used in your CPU. The fields are:

1. Context(PTEBase): 

    用于页表管理。保存想要保存的内存页表的基地址。该基地址的低22位为0，也就是以4M为边界。虽然在物理内存或者未映射的内存上提供对齐很低效，但是这样设计的目的是把该表存储到kseg2映射区域内。

2. Context(BadVPN2)/XContext(BadVPN2): 

    Following a TLB-related exception, this holds the page address, which is just the high-order bits of BadVAddr. Why the “2” in the name? Recall that in the MIPS32/MIPS64 TLB, each entry maps an adjacent pair of virtual-address pages onto two independent physical pages.

    The BadVPN2 value starts at bit 4, so as to precalculate a pointer into a table of 16-byte entries whose base address is in PTEBase. If the OS maintains this table so that the entry implicitly accessed by a particular virtual page address contains exactly the right EntryLo0-1 data to create a TLB entry translating that page, then you minimize the work a TLB miss exception handler has to carry out; you can see that in section 6.5. If you’re only translating 32-bit addresses and don’t need too many bits of software-only state in the page table, you could get by with an eight-byte page-table entry. This turns out to be one of the reasons why Linux doesn’t use the Context registers in the prescribed manner. The XContext(BadVPN2) field may be larger than is shown in Figure 6.4 if your CPU can handle more than 240 bits of user virtual address space.When that happens, the R and PTEBase fields are pushed along to make space.

3. XContext(PTEBase): 

    The page table base for 64-bit address regions must be aligned so that all the bits below those specified by XContext (PTEBase) are zero: That’s 8 GB aligned. That sounds intolerable, but there is a suitable large, mapped, kernel-only-accessible region (“xkseg”) in the basic MIPS64 memory map.

4. XContext(R): 

    TLB misses can come from any mapped region of the CPU’s memory map, not just from user space. All regions lie within one overarching 64-bit space, but are much smaller than is required to pack it full (you might like to refer to Figure 2.2 in section 2.8). Usable 64-bit virtual addresses are divided into four “xk. . . ” segments within which you can use a 62-bit in-segment address.

    So as to save space in XContext, the miss address as shown here is kept as a separate 40-bit in-region page address (BadVPN2) and a 2-bit mappedregion selector XContext(R), defined as follows:

    | R值 | 区域名 | 描述 |
    | --- | ------ | ---- |
    |  0  | xuseg  | 用户态可访问的虚拟内存的低地址区 |
    |  1  | xsseg  | 管理态可访问的空间（管理态是可选的）|
    |  2  |        | 对应未映射的地址段，未使用 |
    |  3  | xkseg  | 内核态映射空间（包含老kseg2）|

Note that not all operating systems use Context/XContext as originally envisaged—notably, Linux doesn’t.We’ll discuss why later.

# 3 TLB/MMU指令

相关指令：

* tlbr和tlbwi

    读写TLB表项，也就是在TLB项和EntryHi和EntryLo0-1寄存器之间搬运数据。使用Index寄存器中的值作为索引，

    正常情况下，这两个指令应该不常用。如果你确实修改了某个TLB项，记得恢复ASID域的值，因为该域的值也会被覆盖掉。

* tlbwr

    拷贝EntryHi、EntryLo和PageMask寄存器的内容到由Random寄存器随机索引的某个TLB项中。当你选择随机替换策略时，这可以节省你自己产生随机数的时间。实践中，tlbwr经常被TLB重填异常处理程序调用，写一个新的TLB项，而tlbwi指令可以在任何地方使用。

* tlbp

    遍历TLB表。搜索TLB表，查看是否有与EntryHi寄存器中的VPN和ASID相匹配的项。如果有，把对应项的索引写入到Index寄存器中；如果没有，则设置Index寄存器的bit31，这个值看上去是一个负值，更好判断。

程序应该保证只能一个TLB项匹配，否则会发生可怕的错误。

需要注意的是，tlbp不会从TLB中读取数据，必须在后面执行指令tlbr读取数据。

在大部分的CPU中，TLB地址转换都被纳入流水线的操作流程中，以便提高效率。这是，TLB的这些指令操作不能完全适配标准的管道流水线。所以，在使用了上面这些指令后，立马使用相关虚拟地址的指令可能会产生危险，这个问题我们之前的文章分析过。为了避免这个问题，通常在kseg0非转换区域进行TLB的维护工作。

# 4 TLB编程

TLB表的设置过程是：将想要的值写入到EntryHi和EntryLo寄存器中，然后使用tlbwr或tlbwi指令拷贝到相应的TLB表中。

处理TLB重填异常的时候，硬件自动将虚拟地址的VPN和ASID域写入到EntryHi寄存器中。

一定要小心不要创建两个相同的虚拟地址映射关系。如果TLB包含重复的项，尝试转换这个地址的时候，潜在的会破坏CPU芯片。一些CPU为了在这种情况保护自身，会关闭TLB硬件单元，并设置相应的SR（TS）标志位。此时，TLB只能复位才能工作。

系统软件一般不会读取TLB表项。但是，如果确实需要读取它们，则使用tlbp遍历匹配到需要的虚拟地址对应的TLB项，把对应的索引值写入到Index寄存器。然后使用tlbr指令读取TLB项相应的值到EntryHi和EntryLo0-1寄存器中。使用过程中，不要忘记保存和恢复EntryHi寄存器，因为ASID域非常重要。

有时候在阅读相关CPU文档的时候，可能会看到带有"ITLB"和"DTLB"字样的指令，它们分别执行指令和数据地址的转换。它们主要执行L1级Cache中地址的转译工作，这些操作完全由硬件进行管理，软件无需干预。当你写入到主TLB表中某一项时，它们会自动失效。

#### 4.1 重填过程

如果程序试图访问任何需要转译的地址（通常是用户态使用的地址空间kuseg和内核态使用的kseg2段），如果TLB表中没有对应的转换映射，CPU就会发出一个TLB重填异常。

我们知道TLB一般很小，而应用程序所需要的地址空间都很大，无法一次在TLB表完全展现。所以，内核OS一般都在内存中维护着一些页表，它们保存着虚拟地址到物理地址的映射关系，我们称这些表为虚拟内存映射表。把TLB作为一个内存映射表的一个缓存。为了提高效率，这些页表中的数据项直接就是按照TLB表项的内容进行排列组合的数据；为了更快访问这些页表，把这些页表的位置和结构保存到Context或XContext寄存器中，作为访问这些页表的指针。

MIPS架构系统一般在kseg0段运行OS代码，这段地址不需要地址转换。所以，TLB未命中一般发生在用户态程序中。为了加速异常处理程序的执行，提供了几个特殊的硬件特性。首先，重填异常处理程序位于内存的低地址区，不会被其它异常使用；其次，使用一些小技巧保证虚拟内存映射表存储于内核虚拟地址空间上（kseg2或64位中对应的内核虚拟地址空间中），这样，这些页表所在的物理内存就不需要映射到用户态虚拟地址空间上了；最后，Context或XContext寄存器可以直接从内存中的虚拟地址映射表中访问正确的映射关系了。

当然了，这些过程都不是强制的。在小型的嵌入式系统上，TLB完全可以映射到固定的物理内存或者进行很少的转换，这时候，TLB就不需要作为一个缓存而存在了。

甚至有些大型OS也不使用上面这种处理方式，比如Linux。因为它与Linux对于虚拟内存的管理策略不同。因为Linux内核的地址映射对所有进程都相同。后面我们再专门分析，基于MIPS架构的Linux内存管理方式。

#### 4.2 使用ASID

ASID设计的目的就是将内存区域进行安全划分，保证不同进程的地址空间安全。使用方法就是设置TLB项中的ASID域为对应的值，并且EntryLo0-1(G)标志位为0，就可以只访问EntryHi(ASID)匹配的项了。ASID占用8位，允许同时映射多达256个不同的地址空间，而不用在进程切换的时候清除TLB。如果ASID用尽，需要把不需要的进程从TLB中清除。抛弃其所有的映射，就可以把ASID重新指定给其它进程了。

#### 4.3 Random和Wired寄存器

一般情况下，TLB使用随机替换原则。所以，为了效率MIPS架构CPU提供了一个Random寄存器来简化实现。

但是，有时候确实需要一些TLB项常驻TLB表中。MIPS架构提供Wired寄存器实现这个需求。写入Wired寄存器一个值，0~wired-1范围的值Random寄存器就不会再产生。它仍然递减，到了wired寄存器的值时，就会返回到最大值。通过这种方式将TLB索引在0到wired-1中的项永久保留在TLB表中。

# 5 对硬件友好的页表和重填机制

There’s a particular translation mechanism that the MIPS architects undoubtedly had in mind for user addresses in a UNIX-like OS. It relies upon building a page table in memory for each address space. The page table consists of a linear array of entries, indexed by the VPN, whose format is matched to the bitfields of the EntryLo register. The paired TLBs need 2 × 64-bit entries, 16 bytes per entry.
类Unix的OS为MIPS架构提供了一种特殊的地址转换机制。把所有的地址空间划分为一个线性数组，使用VPN索引，与EntryLo寄存器的位域匹配。这样成对的TLB项需要16个字节保存，2*64位。

这种处理方式减少了重填异常处理程序的负荷，但是带来了其它问题。因为每8K的用户空间地址占用一个16字节的表项，整个2GB的用户空间就占用4MB大小的页表，这是一个相当大的内存空间。我们知道，用户空间的地址一般是在底部填充代码和数据，顶部是堆栈（向下增长），这样中间有一个巨大的空隙。MIPS架构借鉴了DEC的VAX体系结构的启发，把页表存入内核态的虚拟地址空间（kseg2或xkseg）。这样的话，

* 节省了物理空间：

    中间不用的空闲不需要为其提供物理内存分配。
    
* 
That minimizes the load on the critical refill exception handler but opens up other problems. Since each 8 KB of user address space takes 16 bytes of table space, the entire 2 GB of user space needs a 4-MB table, which is an embarrassingly large chunk of data.4 Of course, most user address spaces are only filled at the bottom (with code and data) and at the top (with a downward growing stack) with a huge gap in between. The solution MIPS adopted is inspired by DEC’s VAX architecture, and is to locate the page table itself in virtual memory in a kernel-mapped (kseg2 or xkseg) region. This neatly solves two problems at once:

* It saves physical memory: 

    Since the unused gap in the middle of the page table will never be referenced, no physical memory need actually be allocated for those entries.

* It provides an easy mechanism for remapping a new user page table when changing context, without having to find enough virtual addresses in the OS to map all the page tables at once. Instead, you have a different kernel memory map for each different address space, and when you change the ASID value, the kseg2 pointer to the page table is now automatically remapped onto the correct page table. It’s nearly magic.

The MIPS architecture supports this kind of linear page table in the form of the Context register (or XContext for extended addressing in 64-bit CPUs).

If you make your page table start at a 4-MB boundary (since it is in virtual memory, any gap created won’t use up physical memory space) and set up the Context PTEBase field with the high-order bits of the page table starting the address, then, following a user refill exception, the Context register will contain the address of the entry you need for the refill with no further calculation needed.

So far so good: But this scheme seems to lead to a fatal vicious circle, where a TLB refill exception handler may itself get a TLB refill exception, because the kseg2 mapping for the page table isn’t in the TLB. But we can fix that, too.

If a nested TLB refill exception happens, it happens with the CPU already in exception mode. In MIPS CPUs, a TLB refill from exception mode is directed to the general exception entry point, where it will be detected and can be handled specially.

Moreover, an exception from exception mode behaves strangely: It does not change the restart location EPC, sowhen the “inner” exception returns, it returns straight to the nonexception TLB miss point. It’s rather as if the hardware started processing one exception, then changed its mind and processed another: but the second exception is no longer nested, it has usurped the first one.We’ll see how that works when we look at an example.

#### 5.1 TLB未命中处理

A TLB miss exception always uses a dedicated entry point unless the CPU is already handling an exception—that is, unless SR(EXL) is set.

Here is the code for a TLB miss handler for a MIPS32 CPU (or a MIPS64 CPU handling translations for a 32-bit address space):

        .set    noreorder
        .set    noat
    TLBmiss32:
        mfc0    k1, C0_CONTEXT      # (1)
        lw      k0, 0(k1)           # (2)
        lw      k1, 8(k1)           # (3)
        mtc0    k0, C0_ENTRYLO0     # (4)
        mtc0    k1, C0_ENTRYLO1     # (5)
        ehb                         # (6)
        tlbwr                       # (7)
        eret                        # (8)
        .set at
        .set reorder

Following is a line-by-line analysis of the code:

* (1)行

    The k0-1 general-purpose registers are conventionally reserved for the use of low-level exception handlers; we can just go ahead and use them.

* (2-5)行

    There are a pair of physical-side (EntryLo) descriptions in each TLB entry (you might like to glance back at the TLB entry diagram, Figure 6.1). The layout of the MIPS32/64 Context register shown in Figure 6.4 reserves 16 bytes for each paired entry (eight bytes of space for each physical page), even though MIPS32’s EntryLo0-1 are 32-bit registers. This is for compatibility with the 64-bit page table and to provide some spare fields in the page table to keep software-only information.

    Interleaving the lw/mtc0 sequences herewill save time: FewMIPSCPUs can keep going without pause if you use loaded data in the very next instruction.

    These loads are vulnerable to a nested TLB miss if the kseg2 address’s translation is not in the page table.We’ll talk about that later.

* (6)行

    It’s no good writing the entry with tlbwr until it will get the right data from EntryLo1. The MIPS32 architecture does not guarantee this will be ready for the immediately following instruction, but it does guarantee that the sequence will be safe if the instructions are separated by an ehb (execution hazard barrier) instruction—see section 3.4 for more information about hazard barriers.

* (7)

    This is random replacement of a translation pair as discussed.

* (8)

    MIPS32 (and all MIPS CPUs later than MIPS I) have the eret instruction, which returns from the exception to the address in EPC and unsets SR(EXL).

So what happens when you get another TLB miss? The miss from exception
level invokes not the special high-speed handler but the general-purpose
exception entry point. We’re already in exception mode, so we don’t alter the
exception return register EPC.

The Cause register and the address-exception registers (BadVAddr,
EntryHi, and even Context and XContext) will relate to the TLB miss
on the page table address in kseg2. But EPC still points back at the instruction
that caused the original TLB miss.

The exception handler will fix up the kseg2 page table miss (so long as this
was a legal address) and the general exception handler will return into the user
program. Of course, we haven’t done anything about the translation for the
user address that originally caused the user-space TLB miss, so it will immediately
miss again. But the second time around, the page table translation will be
available and the user miss handler will complete successfully. Neat.

#### 5.2 XTLB未命中处理

MIPS64 CPUs have two special entry points. One—shared with MIPS32 CPUs—is used to handle translations for processes using only 32 bits of address space; an additional entry point is provided for programs marked as using the bigger address spaces available with 64-bit pointers.

The status register has three fields, SR(UX), SR(SX), and SR(KX), that select which exception handler to use based on the CPU privilege level at the time of the failed translation.

With the appropriate status bit set (SR(UX) for user mode), a TLB miss exception uses a different vector, wherewe should have a routine thatwill reload translations for a huge address space. The handler code (of an XTLB miss handler for a CPU with 64-bit address space) looks much like the 32-bit version, except for the 64-bit-wide registers and the use of the XContext register in place of Context:

        .set    noreorder
        .set    noat
    TLBmissR4K:
        dmfc0   k1, C0_XCONTEXT
        ld      k0, 0(k1)
        ld      k1, 8(k1)
        dmtc0   k0, C0_ENTRYLO0
        dmtc0   k1, C0_ENTRYLO1
        ehb
        tlbwr
        eret
        .set    at
        .set    reorder

Note, though, that the resulting page table structure in kernel virtual memory is far bigger and will undoubtedly be in the giant xkseg region.

I should remind you again that this system is not compulsory, and in fact is not used by the MIPS version of Linux (which is overwhelmingly the most popular translated-address OS for MIPS applications). It’s a rather deeply ingrained design choice in the Linux kernel that the kernel’s own code and data are not remapped by a context switch, but exactly that is required for the kseg2/xkseg page table trick described here. See section 14.4.8 for how it’s done.


# 6 MIPS架构中TLB的使用场景

If you’re using a full-scale OS like Linux, then it will use the TLB behind your back, and you’ll rarely notice. With a less ambitious OS or runtime system, you may wonder whether it’s useful. But, because the MIPS TLB provides a general purpose  address translation service, there are a number of ways you might take advantage of it.

The TLB mechanism permits you to translate addresses (at page granularity) from any mapped address to any physical address and therefore to relocate regions of program space to any location in your machine’s address map. There’s no need to support a TLB refill exception or a separate memory-held page table if your mapping requirements are modest enough that you can accommodate all the translations you need in the TLB.

The TLB also allows you to define some address as temporarily or permanently unavailable, so that accesses to those locations will cause an exception that can be used to run some operating system service routine. By using user-privilege programs you can give some software access only to those addresses you want it to have, and by using address space IDs in the translation entries, you can efficiently manage multiple mutually inaccessible user programs. You can write-protect some parts of memory.

The applications for this are endless, but here’s a list to indicate the range:

* Accessing inconvenient physical address ranges: 

    Hardware registers for a MIPS system are most conveniently located in the physical address range 0–512 MB, where you can access them with a corresponding pointer from the kseg1 region. But where the hardware can’t stay within this desirable area, you can map an arbitrary page of higher physical memory into a convenient mapped area, such as kseg2. The TLB flags for this translation should be set to ensure uncached access, but then the program can be written exactly as though the address was in the convenient place.

* Memory resources for an exception routine: 

    Suppose you’d like to run an exception handler without using the reserved k0/k1 registers to save context. If so, you’d have trouble, because a MIPS CPU normally has nowhere to save any registers without overwriting at least one of these. You can do loads or stores using the zero register as a base address, but with a positive offset these addresses are located in the first 32 KB of kuseg, and with a negative offset they are located in the last 32 KB of kseg2. Without the TLB, these go nowhere. With the TLB, you could map one or more pages in this region into read/write memory and then use zero-based stores to save context and rescue your exception handler.

* Extendable stacks and heaps in a non-VM system: 

    Even when you don’t have a disk and have no intention of supporting full demand paging, it can still be useful to grow an application’s stack and heap on demand while monitoring its growth. In this case you’ll need the TLB to map the stack/heap addresses, and you’ll use TLB miss events to decide whether to allocate more memory or whether the application is out of control.

* Emulating hardware: 

    If you have hardware that is sometimes present and sometimes not, then accessing registers through a mapped region can connect directly to the hardware in properly equipped systems and invoke a software handler on others.

The main idea is that the TLB, with all the ingenuity of a specification that fits so well into a big OS, is a useful, straightforward general resource for programmers.


# 简单操作系统中的内存管理

An OS designed for use off the desktop is generally called a real-time OS (RTOS), hijacking a term that once meant something about real time.6 The UNIX-like system outlined in the first part of this chapter has all the elements you’re likely to find in a smaller OS, but many RTOSs are much simpler. 

Some OS products you might meet up with are VxWorks from Wind River Systems, Thread/X from Express Logic, and Nucleus from Mentor (following their acquisition of Accelerated Technology). All provide multiple threads running in a single address space. There is no task-to-task protection—software running on these is assumed to be a single tightly integrated application. In many cases the OS run time is really quite small, and much of the supplier’s effort is devoted to providing developers with build, debug, and profiling tools.

The jury is still out on whether it’s worth using a more sophisticated OS such as Linux for many different kinds of embedded systems. You get a richer programming environment, task-to-task protection that can be very valuable when integrating a system, and probably cleaner interfaces. Is that worth devoting extra memory andCPUpower to, and losing a degree of control over timing, for the benefits of the cleverer system? Builders of TV set-top boxes, DVD players, and domestic network routers have found Linux worthwhile: Other systems (not necessarily of very different complexity) are still habitually using simpler systems.

And of course Linux is open source. Sometimes it’s just good that there are no license fees; perhaps, more importantly, if your system doesn’t work because of an OS bug, open source means you can fix it yourself or commission any of a number of experts to fix it for you—right away. It’s paradoxical, but the more successful a commercial OS becomes, the harder it is to find someone to fix it on a reasonable schedule.

But for now, as a developer, you may be faced with almost anything. When you’re trying to understand a new memory management system, the first thing is to figure out the memory maps, both the virtual map presented to application software and the physical map of the system. It’s the simple-minded virtual address map that makes UNIX memory management relatively straightforward to describe. But operating systems targeted at embedded applications do not usually have their roots in hardware with memory management, and the process memory map often has the fossils of unmapped memory maps hidden inside it. The use of a pencil, paper, and patience will sort it out.


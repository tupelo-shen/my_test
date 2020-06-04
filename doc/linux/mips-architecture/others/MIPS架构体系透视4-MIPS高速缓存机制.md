
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

While early MIPS CPUs use simple write-through data caches, later CPUs are
too fast for this approach—theywould swamp their memory systemswithwrites
and slow to a (relative) crawl.

The solution is to retain write data in the cache.Write data is stored only to
the cache, and the cache line is marked to make sure we don’t forget to write it
back to memory sometime later (a line that needs writing back is called dirty or
modified).3

There’s a subvariant here: If the addressed data is not currently in the cache,
we can either write it to main memory and ignore the cache, or we can bring
the data in specially just so we can write it—this is called write-allocate. Writeallocate
is not always the best approach: Data that is being written for some I/O
device to read (and that the CPU will not read or write again) is ideally kept out
of the cache. But that only works if the programmers can be relied upon to tell
the operating system.

All except the very lowest-end modern MIPS CPUs have on-chip caches that
are write-back and have line sizes of 16 or 32 bytes.

The R4000 and some later CPUs found uses in large computer servers from Silicon Graphics and others. Their cache design choices are influenced by the needs of multiprocessor systems. There’s a short description of typical multiprocessor systems in section 15.3.

# 5 高速缓存设计中的一些其它考虑

The 1980s and 1990s have seen much work and exploration of how to build caches. So there are yet more choices:

* Physically addressed/virtually addressed:

While the CPU is running a grown-up OS, data and instruction addresses in your program (the program address or virtual address) are translated before appearing as physical addresses in the system memory.

A cache that works purely on physical addresses is easier to manage (we’ll explain why below), but raw program (virtual) addresses are available to start the cache lookup earlier, letting the system run that little bit faster. So what’s wrong with program addresses? They’re not unique; many different programs running in different address spaces on a CPU may share the same program address for different data. We could reinitialize the entire cache every time we switch contexts between different address spaces; that used to be done some years ago andmay be a reasonable solution for very small caches. But for big caches it’s ridiculously inefficient, and we’ll need to include a field identifying the address space in the cache tag to make sure we don’t mix them up.

Many MIPS CPUs use the program (virtual) address to provide a fast index for their L1 caches. But rather than using the program address plus an address space identifier to tag the cache line, they use the physical address. The physical address is unique to the cache line and is efficient because the scheme allows the CPU to translate program addresses to physical addresses at the same time as it is looking up the cache.

There’s another, more subtle problem with program addresses, which the physical tag does not solve: The same physical location may be described by different addresses in different tasks. In turn, that might lead to the same memory location being cached in two different cache entries (because they were referred to by different virtual addresses
that selected different cache indexes). Many MIPS CPUs do not have
hardware to detect or avoid these cache aliases and leave them as
a problem to be worked around by the OS’s memory manager; see
section 4.12 for details.

* Choice of line size: The line size is the number of words of data stored
with each tag. Early MIPS caches had one word per tag, but it’s usually
advantageous to store multiple words per tag, particularly when your
memory system will support fast burst reads (most do). Modern MIPS
caches tend to use four- or eight-word line sizes, but large L2 and L3
caches may have bigger lines.
When a cache miss occurs, the whole line must be filled from memory.

* Split/unified: MIPS L1 caches are always separated into an I- and a
D-cache; the selection is done purely by function, in that instruction
fetches look in the I-cache and data loads/stores in the D-cache. (This
means, by the way, that if you try to execute code that the CPU just
copied into memory, you must both write back those instructions out
of the D-cache and invalidate the I-cache locations, to make sure you
really execute the new instructions.)
However, L2 caches are rarely divided up this way—it’s complex, more
costly, and generally performs poorly.

# 6 管理Cache

I hope you recall from section 2.8 that a MIPS CPU has two fixed 512-MB windows onto physical memory, one cached (“kseg0”) and one uncached (“kseg1”). Typically, OS code runs in kseg0 and uses kseg1 to build uncached references. Physical addresses higher than 512MB are not accessible here: A 64-bit CPU will have direct access through another window, or you can set up the TLB (memory management/translation hardware) to map an address. Each TLB entry can be marked to make accesses either cached or uncached.

The cache hardware,with the help of systemsoftware,must be able to ensure that any application gets the same data as it would have in an uncached system and that any direct memory access (DMA) I/O controller (getting data directly from memory) obtains the data that the program thinks it has written.

We’ve said before that in PCs and some other integrated systems the assistance of system software is often not required; it’s worth spending the money, silicon area, and extra cycles to get the hardware to make the cache genuinely transparent. Such a hardware-managed cache is said to be coherent.

The contents of the cache arrays of your CPU are typically random following power-up. Bootstrap software is responsible for initializing the caches; this can be quite an intricate process, and there’s some advice about it below. But once the system is up and running, there are only three circumstances in which the CPU must intervene:

* Before DMA out of memory:

    If a device is taking data out of memory, it’s vital that it gets the right data. If the data cache is write back and a program has recently written some data, some of the correct data may still be held in the D-cache but not yet be written back to main memory. The CPU can’t see this problem, of course; if it looks at the memory locations it will get the correct data back from its cache.

    So before the DMA device starts reading data from memory, any data for that range of locations that is currently held in the D-cache must be written back to memory if necessary.

* DMA into memory:

    If a device is loading data into memory, it’s important to invalidate any cache entries purporting to hold copies of the memory locations concerned; otherwise, the CPU reading these locations will obtain stale cached data. The cache entriesmust be invalidated before the CPU uses any data from the DMA input stream, but it’s quite common to invalidate them before starting the DMA.

* Writing instructions:

    When the CPU itself is storing instructions into memory for subsequent execution, you must first ensure that the instructions are written back to memory and then make sure that the corresponding I-cache locations are invalidated; the MIPS CPU has no connection between the D-cache and the I-cache.

    In the most modern MIPS CPUs the synci instruction does everything that is necessary to make the instructions you’ve just stored usable for execution—and it’s a user-privilege instruction.

If your software is going to fix these problems, it needs to be able to do two distinct operations on a cache entry.

The first operation is called write-back.When the data of interest is present in the cache and is dirty (marked as having been written by the CPU since it was last obtained from memory or written back to memory), then the CPU copies the data from the cache into main memory.

The second is invalidate.When the data of interest is in the cache, the CPU marks it invalid so that any subsequent access will fetch fresh data from memory.

# 为什么不是硬件管理Cache？

Caches managed with hardware are described as “coherent” (or, more informally, “snoopy.”) When another CPU or some DMA device accesses memory, the cache control logic is notified. With a CPU attached to a shared bus, this is pretty straightforward; the address bus contains most of the information you need. The cache control logic watches (snoops) the address bus even when the CPU is not using it and picks out relevant cycles. It does that by looking up its own cache to see whether it holds a copy of the location being accessed.

If someone is writing data that is inside the cache, the controller could pick up the data and update the cache line but is more likely to just invalidate its own, nowstale, copy. If someone is reading data for which updated information is held in the cache, the controller may be able to intervene on the bus, telling the memory controller that it has a more up-to-date version.

One major problem with doing this is that it works only within a system designed to operate that way. Not all systems have a single bus where all transactions appear, and bought-in I/O controllers are unlikely to conform to the right protocols.

Also, that’s a lot of snooping going on. Most of the locations that CPUs work with are the CPU’s private areas; they will never be read or written by any other CPU or device. We’d like not to build hardware ingenuity into the cache, loading every cache location andbuscycle withcomplexitythat willonlysometimes be used. It’s easy to suppose that a hardware cache control mechanism must be faster than software, but that’s not necessarily so. A snoopy cache controller must look at the cache tags on every external cycle, which could shut the CPU out of its cache and slow it down; snoopy cache controllers usually fix this by keeping two copies of the cache tags (a private-to-CPU and public version). Software management can operate on blocks of cache locations in a single fast loop;hardwaremanagementwill interleave invalidations or write-backs with CPU accesses at I/O speed, and that usually implies more arbitration overhead.

Historically, MIPS designers took the radical RISC position: MIPS CPUs either had no cache management hardware or, where designed for multiprocessors, they had everything.

From a 21st-century perspective, the trade-off looks different. For most classes ofCPU,it looks more worthwhile to accept some hardware complexity to avoid  the system software bugs that occur when programmers miss a place where the cache needs attention. At the time of writing (2006), there’s a shift in progress toward implementing “invisible” caches with some level of hardware cache management on all but the smallest MIPS CPUs. If your MIPS CPU is fully coherent, you may not need this chapter at all (but very few do the whole job). And many MIPS cores that will be in production for years to come still don’t have coherent cache controllers, so the shift will take a long time to complete.

It’s tempting to use the more colorful and evocative word “flush” in this
context, but it is ambiguously used to mean write-back, invalidate, or the combination
of the two—so we’ll avoid it.

There are some much more complicated issues involved when two or more
processors share memory. Most shared-memory systems are too complex for
one CPU to know which locations the other will read or write, so the software
can’t figure out exactly which memory regions need to be invalidated or written
back. Either any shared memory must always be accessed uncached (very slow
unless the amount of interaction is very limited), or the caches must have special
hardware that keeps the caches (and memory) coherent. Multi-CPU cache
coherency got less scary after it was systematized by a group of engineers working
on the ambitious FutureBus standard in the mid-1980s. There’s a discussion
of multiprocessor mechanisms in section 15.3.

# L2和L3两级Cache

In larger systems, there’s often a nested hierarchy of caches. A small and
fast L1 or primary cache is close to the CPU. Accesses that miss in the
L1 cache are looked up not directly in memory but in an L2 or secondary
cache—typically several times bigger and several times slower than the L1
(but still several times faster than the main memory). The number of levels
of hierarchy that might be useful depends on how slow main memory is
compared with the CPU’s fastest access; with CPU cycle times falling much
faster than memory access times, 2006 desktop systems commonly have L3
cache. 2006 embedded systems, trailing desktop speeds by several years (and
different constraints, particularly for power consumption and heat dissipation)
are only just getting around to using L2 caches outside a few specialist
high-end applications.

# MIPS架构CPU的Cache配置

We can now classify some landmark MIPS CPUs, ancient and modern, by
their cache implementations and see how the cache hierarchy has evolved
(Table 4.1).

As clock speeds get higher, we see more variety in cache configurations as
designers try to cope with a CPU that is running increasingly ahead of its memory
system. To earn its keep, a cache must improve performance by supplying
data significantly faster than the next outer memory and must usually succeed
in supplying the data (hitting).

CPUs that add a second level of cache reduce the miss penalty for the L1
cache—which may then be able to be smaller or simpler. CPUs with on-chip
L2 caches typically have smaller L1s,with dual 16-KB L1 caches a favored “sweet
spot.” Until recently, most MIPS CPUs fitted the L1 cache access into one clock
cycle. It seems reasonable that, as chip performance grows and clock rates
increase, the size of memory that can be accessed in one clock period should
be more or less constant. But in fact, on-chip memory speed is lagging behind
logic. Modern CPU designs often lengthen the pipeline mainly to allow for a
two-clock cache access.

Off-chip caches are often direct mapped because a set-associative cache
needs much wider interfaces to carry multiple tags, so they can be matched
in parallel.

Amidst all this evolution, there have been two main generations of the software
interface to a MIPS cache. There is one style founded by the R3000 and
followed by most early 32-bit MIPS CPUs; there is another starting with the
R4000 and used by all 64-bit CPUs to date. The R4000 style has now become a
part of the MIPS32 standard too and is now commonplace—and that’s all we
will describe.

Most modern MIPS CPUs have L1 caches that are write-back, virtually
indexed, physically tagged, and two- or four-way set-associative. Cache management
is done using a special cache instruction.

The first R4000 CPUs had on-chip L2 cache control, and QED’s RM7000
(around 1998) introduced on-chip L2 cache, which is now commonplace in
high-end designs. The cache instruction as defined by MIPS32/64 extends to
L2 and L3 caches.

> Some system implementations have L2 caches that are not controlled by
hardware inside the MIPS CPU but built onto the memory bus. The software
interface to caches like that is going to be system specific and may
be quite different from the CPU-implemented or CPU-controlled caches
described in this chapter.

# 对MIPS32/64高速缓存的编程

The MIPS32/64 specifications define a tidy way of managing caches (following
the lead of the R4000, which fixed the unseemly cache maintenance of the
earlier CPUs).

MIPS32/64 CPUs often have much more sophisticated caches than early
MIPS CPUs—write-back, andwith longer lines. Because it’s awrite-back cache,
each line needs a status bit that marks it as dirty when it gets written by the
CPU (and hence becomes different from the main memory copy of the data).
To manage the caches, there are a number of primitive operations we’d really
like. The actions we need to achieve are:

* Invalidate range of locations: Removes any data from this address range
from the cache, so that the next reference to it will acquire fresh data
from memory.
The instruction cache HitInvalidate has the form of a load/store
instruction, providing a virtual address. It invalidates any single cache
line containing the data referenced by that virtual address. Repeat the
instruction at line-size-separated addresses across the range.

* Write-back range of locations: Ensures that any CPU-written data in
this range currently held in dirty cache lines is sent back to main
memory.
The instruction cache HitWritebackInvalidate writes back any
single cache line containing the data referenced by that virtual address,
and then makes it invalid as a bonus.

* Invalidate entire cache: Discards all cached data. This is rarely required
except at initialization, but is sometimes the last resort of operating system
code that is not sure which cached data needs to be
invalidated.
The instruction cache IndexInvalidate is used here. Its address
argument is interpreted only as an index into the memory array underlying
the cache—successive lines are accessed at index values from 0
upward, in cache-line-size increments.

* Initialize caches:Whatever is required to get the caches usable from an
unknown state. Setting up the cache control fields (the “tag”) usually
involves zeroing the CP0 TagLo register and using a cache Index-
StoreTag operation on each line-sized chunk of the cache. Caches with
data check bits may also need to be prefilled with data—even though
the lines are invalid, they should not have bad check bits. Prefilling
an I-cache would be tricky, but you can usually use a cache Fill
instruction.

## Cache相关指令

The cache instruction has the general form of a MIPS load or store instruction
(with the usual register plus 16-bit signed displacement address)—but
where the data register would have been encoded there’s a 5-bit operation
field, which must encode the cache to be operated on, determine how to
find the line, and figure out what to do with the line when you find it.
You write a cache line in assembly as cache OP,addr, where OP is just
a numeric value for the operation field.

The best thing to do is to use the C preprocessor to define text “names”
representing the numeric values for the operations. There are no standard
names; I’ve arbitrarily used the names of C preprocessor definitions found in
header files from the MIPS Technologies toolkit package.

Of the 5-bit field, the upper 2 bits select which cache to work on:

    0 = L1 I-cache
    1 = L1 D-cache
    2 = L3 cache, if fitted
    3 = L2 cache, if fitted

Before we list the individual commands, note that they come in three flavors,
which differ in how they select the cache entry (the “cache line”) they will
work on:

* Hit-type cache operation: Presents an address (just like a load/store),
which is looked up in the cache. If this location is in the cache (if it
“hits”), the cache operation is carried out on the enclosing line. If this
location is not in the cache, nothing happens.

* Address-type cache operation: Presents an address of some memory data,
which is processed just like a cached access. That is, if the line you
addressed was not previously in cache, the data is fetched from memory
before the cache operation.

* Index-type cache operation: Uses as many low bits of the virtual address
as are needed to select the byte within the cache line, then the cache
line address inside one of the cache ways, and then the way.5 You have
to know the size of your cache (discoverable from Config1-2, see
section 3.3.7 for details) to know exactly where the field boundaries
are, but your address is used something like this:

Once you’ve picked your cache and cache line, you have a choice of operations
you can perform on it, as shown in Table 4.2. Three operations must be
supported by a CPU to claim MIPS32/64 compatibility: that’s index invalidate,
index store tag, and hit write-back invalidate. Other operations are optional—if
you use them, check your CPU manual carefully.

The synci instruction (new to the MIPS32 Release 2 update) provides a
clean mechanism for ensuring that instructions you’ve just written are correctly
presented for execution (it combines a D-cache write-back with an
I-cache invalidate). If your CPU supports it, you should use synci in preference
to the traditional alternative (a pair of cache instructions, to do D-cache
write-back followed by an I-cache invalidate).

## Cache初始化和Tag/Data寄存器

For diagnostic and maintenance purposes it’s good to be able to read and write
the cache tags; MIPS32/64 defines a pair of 32-bit registers TagLo and TagHi6
to stage data between the tag part of the cache and management software. TagHi
is often not necessary: Until your physical address space is more than 36 bits,
there is usually room for the whole tag in TagLo.

The fields in the two registers reflect the tag field in the cache and are CPU
dependent. Only one thing is guaranteed: An all-zero value in the tag register(s)
corresponds to a legal, properly formed tag for a line that contains no valid data.
The CPU implements a cache IndexStoreTag instruction, which copies the
contents of the tag registers into the cache line. So by setting the registers to zero,
and storing the tag value, you can start to initialize a cache from an unknown
starting state.

There are “store data” and “load data” cache operations defined by
MIPS32/64, but they are optional and should only be used by code that is
already known to be CPU-specific. You can always access the data by “hitting”
on it.

A cache tag must hold all the address bits that are not implicitly known
from the cache index (look back at Figure 4.1 if this does not seem
obvious).

So the L1 cache tag field length is the difference between the number of
bits of physical address supported and the number of bits used to index the L1
cache—12 bits for a 16-K four-way set-associative cache. TagLo(PTagLo) has
room for 24 bits, which would support up to a 36-bit physical address range for
such a cache.

The field TagLo(PState) contains the state bits. Your CPU manual will
tell you more about them; however, for all cache management and initialization,
it suffices to know that an all-zero tag is always a legitimate code representing
an invalid cache entry.

## CacheErr、ERR和ErrorEPC寄存器：Memory/Cache错误处理

The CPU’s caches forma vital part of the memory system, and high-availability
or trustworthy systems find it worthwhile to use some extra bits to monitor the
integrity of the data stored there.

Data integrity checks should ideally be implemented end to end; check bits
should be computed as soon as data is generated or introduced to the system,
stored with the data, and checked just before it’s used. That way the check
catches faults not just in the memory array but in the complex buses and gizmos
that data passes through on its way to the CPU and back.

For this reason MIPS CPUs that may be used to implement high-reliability
systems often choose to provide error checking in the caches. As in a main
memory system, you can use either simple parity or an error-correcting code
(ECC).

The components used to build memory systems tend to come in multiples
of 8-bit widths these days, and memory modules that allow for checking
provide 64 data and 8 check bits. So whatever else we use should follow
that lead.

Parity is simple to implement as an extra bit for each byte of memory.
A parity error tells the system that data is unreliable and allows a
somewhat-controlled shutdown instead of creeping random failure. A crucial
role of parity is that it can be an enormous help during system development,
because it unambiguously identifies problems as being due to memory data
integrity.

But a byte of complete garbage has a 50 percent chance of having correct
parity, and random rubbish on the 64-bit bus and its parity bits will still escape
detection 1 time in 256. Some systems want something better.

An error-correcting code (ECC) is more complex to calculate, because it
involves the whole 64-bit word with 8 check bits used together. It’s more
thorough: a 1-bit error can be uniquely identified and corrected, and no

2-bit error can escape detection. ECC is a powerful tool for weeding out
random errors in very large memory arrays.

Because the ECC bits check the whole 64-bit word at once, ECC
memories can’t perform a partial-word write by just selecting which part of
the word to operate on but must always merge the new data and recompute
the ECC. MIPS CPUs running uncached require their memory system to
implement partial-word writes, making things complicated. Memory system
hardware must transform partial-word writes into a read-merge-recalculatewrite
sequence.

For simpler systems, the choice is usually parity or nothing. It can be valuable
to make parity optional, to get the diagnostic benefits during design development
without paying the price in production.

Ideally, whichever check mechanism is implemented in the memory system
will be carried inside the caches. In different CPUs, you may be able to use
parity, a per-doubleword 8-bit ECC field, or no protection at all.

If possible, the data check bits are usually carried straight from the system
interface into the cache store: They need not be checked when the data
arrives from memory and is loaded into the cache. The data is checked
when it’s used, which ensures that any cache parity exception is delivered
to the instruction that causes it, not just to one that happens to share the
same cache line.

A data check error on the system bus for an uncached fetch is handled by the
same mechanism, which means it will be reported as a “cache parity error”—
which can confuse you.

Note that it’s possible for the system interface to mark incoming data as
having no valid check bits. In this case, the CPU will generate check bits for its
internal cache.

If an error occurs, the CPU takes the special error trap. This vectors
through a dedicated location in uncached space (if the cache contains bad
data, it would be foolish to execute code from it). If a system uses ECC,
the hardware will either correct the error or present enough information
for the software to fix the data.

The fields in the CacheErr register are implementation dependent, and
you’ll need to consult your CPU manual. You may be able to get sample cache
error management routines from your CPU supplier.

#### 4 设置Cache大小和配置

In MIPS32/64-compliant CPUs cache size, cache organization, and line size are
reliably reported to you as part of the CP0 Config1-2 registers, described in
section 3.3.7.

But for portability it makes sense to write or recycle initialization software,
which works robustly across a large range of MIPS CPUs. The next section has
some tried-and-tested solutions.

#### 5 初始化程序

Here’s one good way to do it:

1. Set up some memory you can fill the cache from—it doesn’t matter what
data you store in it, but it needs to have correct check bits if your system
uses parity or ECC. A good trick is to reserve the bottom 32 K of system
memory for this purpose, at least until after the cache has been initialized.
If you fill it with data using uncached writes, it will contain correct
parity.
A buffer that size is not going to be big enough to initialize a large L2
cache, and you may need to do something more complicated.

2. Set TagLo to zero, which makes sure that the valid bit is unset and the
tag parity is consistent (we’d set TagHi on a CPU that needed it).
The TagLo register will be used by the cache IndexStoreTag cache
instructions to forcibly invalidate a line and clear the tag parity.

3. Disable interrupts if they might otherwise happen.

4. Initialize the I-cache first, then the D-cache. Following is C code for
I-cache initialization. (You have to believe in the functions or macros
like Index Store Tag I(),which do low-level functions; they’re either
trivial assembly code subroutines that run the appropriate machine
instructions or—for the brave GNU C user—macros invoking a C asm
statement.)

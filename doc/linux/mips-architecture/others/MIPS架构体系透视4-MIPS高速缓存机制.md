
# 0 引言

AMIPS CPU without a cache isn’t really a RISC. Perhaps that’s not fair; for
special purposes you might be able to build a MIPS CPU with a small,
tightly coupled memory that can be accessed in a fixed number of pipeline
stages (preferably one). But MIPS CPUs have always had cache hardware built
tightly into the pipeline.

This chapter will describe the way MIPS caches work and what the software
has to do to make caches useful and reliable. From reset, almost everything
about the cache state is undefined, so bootstrap software must be careful
to initialize the caches correctly before relying on them. You might also benefit
from some hints and tips for use when sizing the caches (it would be bad
software practice to assume you know how big the cache is). For the diagnostics
programmer, we discuss how to test the cache memory and probe for particular
entries.

Some real-time applications writers may want to control exactly what will
get cached at run time. We discuss how to do that, even though I am skeptical
about the wisdom of using such tricks.

There’s also some evolution to contend with. In early 32-bit MIPS processors,
cache management functions relied upon putting the cache into a special
state and then using ordinary reads and writes whose side effects could initialize
or invalidate cache locations. But we won’t dwell on that here; even CPUs
that do not fully comply with MIPS32/64 generally use something close to the
mechanism described in the following text.

# 1 Caches and Cache Management

The cache’s job is to keep a copy of memory data that has been recently read or
written, so it can be returned to the CPU quickly. For L1 caches, the read must
complete in a fixed period of time to keep the pipeline running.

MIPS CPUs always have separate L1 caches for instructions and data
(I-cache and D-cache, respectively) so that an instruction can be read and a
load or store done simultaneously.

Cached CPUs in old-established families (such as the x86) have to be compatible
with code that was written for CPUs that didn’t have any caches. Modern
x86 chips contain ingeniously designed hardware to make sure that software
doesn’t have to know about the caches at all (if you’re building a machine to run
MS-DOS, this is essential to provide backward compatibility).

But because MIPS machines have always had caches, there’s no absolute
requirement for the cache to be so clever. The caches must be transparent to
application software, apart fromthe increased speed. But in a MIPS CPU,which
has always had cache hardware, there is no attempt to make the caches invisible
to system software or device drivers—cache hardware is installed to make the
CPU go fast, not to help the system programmer. A UNIX-like operating system
hides the cache from applications, of course, but while a more lightweight OS
might hide the details of cache manipulation from you, you will still probably
have to know when to invoke the appropriate subroutine.


# 2 How Caches Work

Conceptually, a cache is an associative memory, a chunk of storage where data
is deposited and can be found again using an arbitrary data pattern as a key. In
a cache, the key is the full memory address. Produce the same key back to an
associative memory and you’ll get the same data back again. A real associative
memory will accept a new item using an arbitrary key, at least until it’s full;
however, since a presented key has to be compared with every stored key simultaneously,
a genuine associative memory of any size is either hopelessly resource
hungry, slow, or both.

So howcanwe make a useful cache that is fast and efficient? Figure 4.1 shows
the basic layout of the simplest kind of cache, the direct-mapped cache used in
most MIPS CPUs up to the 1992 generation.

The direct-mapped arrangement uses a simple chunk of high-speed memory
(the cache store) indexed by enough low address bits to span its size. Each
line inside the cache store contains one or more words of data and a cache tag
field, which records the memory address where this data belongs.

On a read, the cache line is accessed, and the tag field is compared with the
higher addresses of the memory address; if the tag matches, we know we’ve got
the right data and have “hit” in the cache.Where there’s more than one word in
the line, the appropriate word will be selected based on the very lowest address
bits.

If the tag doesn’t match, we’ve missed and the data will be read from memory
and copied into the cache. The data that was previously held in the cache
is simply discarded and will need to be fetched from memory again if the CPU
references it.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_4_1.PNG">

A direct-mapped cache like this one has the property that, for any given memory address, there is only one line in the cache store where that data can be kept.1 That might be good or bad; it’s good because such a simple structure will be fast and will allow us to run the whole CPU faster. But simplicity has its bad side too: If your program makes repeated reference to two data items that happen to share the same cache location (presumably because the low bits of their addresses happen to be close together), then the two data items will keep pushing each other out of the cache and efficiency will fall drastically.

> In a fully associative memory, data associated with any given memory address (key) can be stored anywhere; a direct-mapped cache is as far from being content addressable as a cache store can be.

A real associative memory wouldn’t suffer from this kind of thrashing, but it is too slow and expensive.

A common compromise is to use a two-way set-associative cache—which is really just a matter of running two direct-mapped caches in parallel and looking up memory locations in both of them, as shown in Figure 4.2.

Now we’ve got two chances of getting a hit on any address. Four-way setassociative caches (where there are effectively four direct-mapped subcaches) are also common in on-chip caches.

In a multiway cache there’s more than one choice of the cache location to
be used in fixing up a cache miss, and thus more than one acceptable choice
of the cache line to be discarded. The ideal solution is probably to keep track
of accesses to cache lines and pick the “least recently used” (“LRU”) line to be
replaced, but maintaining strict LRU order means updating LRU bits in every
cache line every time the cache is read. Moreover, keeping strict LRU information
in a more-than-four-way set-associative cache becomes impractical. Real
caches often use compromise algorithms like “least recently filled” to pick the
line to discard.

There are penalties, however. Compared with a direct-mapped cache, a
set-associative cache requires many more bus connections between the cache
memory and controller. That means that caches too big to integrate onto a
single chip are much easier to build direct mapped. More subtly, because the
direct-mapped cache has only one possible candidate for the data you need, it’s
possible to keep the CPU running ahead of the tag check (so long as the CPU
does not do anything irrevocable based on the data). Simplicity and running
ahead can translate to a faster clock rate.

Once the cache has been running for awhile it will be full, so capturing new
memory data usually means discarding some previously cached data. If you
know that the data in the cache is already safely in memory, you can just discard
the cached copy; if the data in the cache is more up-to-date than memory, you
need to write it back first.

That brings us to how the cache handles writes.

# 3 Write-Through Caches in Early MIPS CPUs

CPUs don’t just read data (as the above discussion seems to be assuming)—
they write it too. Since a cache is intended to be a local copy of some data from
main memory, one obvious way of handling the CPU’s writes is the use of what
is called a write-through cache.

In a write-through cache, the CPU’s data is always written to main memory;
if a copy of that memory location is resident in the cache, the cached copy is
updated too. If we always do this, then any data in the cache is known to be in
memory too, so we can discard the contents of a cache line anytime without
losing any data.

We would slow the processor down drastically if we waited for the memory
write-through to complete, but we can fix that. Writes (address and data
together) destined for main memory can always be kept in a queue while the
memory controller gets itself ready and completes the write. The place where
writes are queued is organized as a first-in, first-out (FIFO) store and is called
a write buffer.

Early MIPS CPUs had a direct-mapped write-through cache and a write
buffer. So long as the memory system can happily absorb writes at the average
rate produced by a CPU, running a particular programthis way works very well.
But CPU speeds have grown much faster than memory speeds, and somewhere
around the time that the 32-bit MIPS generation was giving way to the
64-bit R4000, MIPS speeds passed the point where a memory system could
reasonably hope to absorb every write.

# 4 Write-Back Caches in MIPS CPUs

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

The R4000 and some later CPUs found uses in large computer servers from
Silicon Graphics and others. Their cache design choices are influenced by the
needs of multiprocessor systems. There’s a short description of typical multiprocessor
systems in section 15.3.

# 5 Other Choices in Cache Design

The 1980s and 1990s have seen much work and exploration of how to build
caches. So there are yet more choices:

* Physically addressed/virtually addressed:

While the CPU is running a
grown-up OS, data and instruction addresses in your program (the
program address or virtual address) are translated before appearing as
physical addresses in the system memory.

A cache that works purely on physical addresses is easier to manage (we’ll
explain why below), but raw program (virtual) addresses are available to
start the cache lookup earlier, letting the system run that little bit faster.
So what’s wrong with program addresses? They’re not unique; many
different programs running in different address spaces on a CPU may
share the same program address for different data. We could reinitialize
the entire cache every time we switch contexts between different address
spaces; that used to be done some years ago andmay be a reasonable solution
for very small caches. But for big caches it’s ridiculously inefficient, and we’ll need to include a field identifying the address space in the cache tag to make sure we don’t mix them up.

Many MIPS CPUs use the program (virtual) address to provide a fast
index for their L1 caches. But rather than using the program address
plus an address space identifier to tag the cache line, they use the
physical address. The physical address is unique to the cache line and
is efficient because the scheme allows the CPU to translate program
addresses to physical addresses at the same time as it is looking up
the cache.

There’s another, more subtle problem with program addresses, which
the physical tag does not solve: The same physical location may be
described by different addresses in different tasks. In turn, that might
lead to the same memory location being cached in two different cache
entries (because they were referred to by different virtual addresses
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

# 6 Managing Caches

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

# Why Not Manage Caches in Hardware?

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

# 4.7 L2 and L3 Caches

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

# 4.8 Cache Configurations for MIPS CPUs

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

# 4.9 Programming MIPS32/64 Caches

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

## 4.9.1 The Cache Instruction

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

## 4.9.2 Cache Initialization and Tag/Data Registers

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

## 4.9.3 CacheErr, ERR, and ErrorEPC Registers: Memory/Cache Error Handling

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

## 4.9.4 Cache Sizing and Figuring Out Configuration

In MIPS32/64-compliant CPUs cache size, cache organization, and line size are
reliably reported to you as part of the CP0 Config1-2 registers, described in
section 3.3.7.

But for portability it makes sense to write or recycle initialization software,
which works robustly across a large range of MIPS CPUs. The next section has
some tried-and-tested solutions.

## 4.9.5 Initialization Routines

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

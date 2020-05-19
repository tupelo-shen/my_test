We’ve tended to introduce most topics in this book from the bottom, which is perhaps natural in a book about low-level computer architecture. But—unless you’re already pretty familiar with a virtual memory system and how it’s used by an OS—you’re recommended to turn now to section 14.4, which describes how virtual memory management works in Linux. Once that makes some sense to you, come back here for hardware details and to see how the hardware can be made to work in other contexts. When we’re thinking top-down, the hardware is often called a memory management unit or MMU. When we’re looking bottom-up, we focus on the main hardware item, known as the TLB (for “translation lookaside buffer,” which is unlikely to help much).

# 1 The TLB/MMU Hardware andWhat It Does

The TLB is the hardware that translates an address used by your program (program addresses or virtual addresses) to the physical address, which accesses memory. The OS’s control over memory translation is the key to all software security features.

On MIPS CPUs (and all modern CPUs), translation operates on 4-Kbyte1
chunks called pages. The 12-bit addresswithin the page is simply passed on from
virtual address to physical address.

> Page sizes bigger than 4 Kbytes are supported by MIPS hardware and, with memory so cheap, a
bigger basic page size might be advantageous to software, but the 4-Kbyte habit is proving very
hard to kick. Much larger page sizes are occasionally used for special translations.

Each entry in the table has the virtual address of a page (the VPN
for virtual page number), and a physical page address (PFN for page frame
number). When a program presents a virtual address, it is compared with each VPN in the TLB, and if it matches an entry the corresponding PFN
is delivered. The TLB is what is called an associative or content-addressable
memory—an entry is selected not by an index but by its contents. It’s a
fairly complex piece of logic, where each entry has a built-in comparator,
and complexity and performance scale badly. So the TLB typically has
between 16 and 64 entries.

A set of flag bits is stored and returned with each PFN and allows the OS to
designate a page as read-only or to specify how data from that page might be
cached.

Most modern MIPS CPUs (and all MIPS32/64 CPUs) double up, with each TLB entry holding two independent physical addresses corresponding to an adjacent pair of virtual pages.

Figure 6.1 shows one TLB entry. The fields are labeled with the names of the CP0 registers used when loading and reading TLB entries in software, as described in the next section.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_6_1.PNG">

When you’re running a real, complicated OS, the software rapidly covers more ground than can be accommodated by the TLB’s modest set of translations. That’s fixed by maintaining the TLB as a software-managed cache of recently used translations. When a needed translation isn’t in the TLB, that generates an exception and the exception handler should figure out and install the correct translation. It’s hard to believe this can work efficiently, but it does.

# 2 TLB/MMU Registers Described

Like everything else in a MIPS CPU, MMU control is effected by a rather small number of extra instructions and a set of registers in the coprocessor 0 set. Table 6.1 lists the control registers, and we’ll get around to the instructions in section 6.3. All TLB entries beingwritten or read are staged through the registers `EntryHi`, `EntryLo0-1`, and `PageMask`.

与内存管理相关的控制寄存器

1. EntryHi

    在协处理器0中的编号为`10`。

2. EntryLo0-1

    在协处理器0中的编号为`2/3`。

3. PageMask

    在协处理器0中的编号为`5`。

4. Index

    在协处理器0中的编号为`0`。

5. Random

    在协处理器0中的编号为`1`。

    This pseudorandom value (actually a free-running counter) is used by a tlbwr to write a new TLB entry into a randomly selected location. Saves time when processing TLB refill traps for software that likes the idea of random replacement (there is probably no viable alternative).

6. Context

    在协处理器0中的编号为`4`。

7. XContext

    在协处理器0中的编号为`20`。

    These are convenience registers, provided to speed up the processing of 
TLB refill traps. The high-order bits are read/write; the low-order bits are
taken from the VPN of the address that couldn’t be translated.
The register fields are laid out so that, if you use the favored arrangement
of memory-held copies of memory translation records, then following a
TLB refill trap Context will contain a pointer to the page table record
needed to map the offending address. See section 6.2.4.
XContext does the same job for traps from processes using more than
32 bits of effective address space; a straightforward extension of the
Context layout to larger spaces would be unworkable because of the size
of the resulting data structures. Some 64-bit CPU software is happy with
32-bit virtual address spaces; but for when that’s not enough, 64-bit CPUs
are equipped with “mode bits” SR(UX), SR(KX), which can be set to
invoke an alternative TLB refill handler; in turn that handler can use
XContext to support a huge but manageable page table format.

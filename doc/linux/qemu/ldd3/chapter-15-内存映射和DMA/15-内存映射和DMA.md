
This chapter delves into the area of Linux memory management, with an emphasis on techniques that are useful to the device driver writer. Many types of driver programming require some understanding of how the virtual memory subsystem works; the material we cover in this chapter comes in handy more than once as we get into some of the more complex and performance-critical subsystems. The virtual memory subsystem is also a highly interesting part of the core Linux kernel and, therefore, it merits a look.

The material in this chapter is divided into three sections:

1. The first covers the implementation of the mmap system call, which allows the mapping of device memory directly into a user process’s address space. Not all devices require mmap support, but, for some, mapping device memory can yield significant performance improvements.

2. We then look at crossing the boundary from the other direction with a discussion of direct access to user-space pages. Relatively few drivers need this capability; in many cases, the kernel performs this sort of mapping without the driver even being aware of it. But an awareness of how to map user-space memory into the kernel (with get_user_pages) can be useful.

3. The final section covers direct memory access (DMA) I/O operations, which provide peripherals with direct access to system memory. Of course, all of these techniques require an understanding of how Linux memory management works, so we start with an overview of that subsystem.


<h2 id="15.1">15.1 Memory Management in Linux</h2>

Rather than describing the theory of memory management in operating systems, this section tries to pinpoint[查明] the main features of the Linux implementation. Although you do not need to be a Linux virtual memory guru[老师] to implement mmap, a basic overview of how things work is useful. What follows is a fairly lengthy description of the data structures used by the kernel to manage memory. Once the necessary background has been covered, we can get into working with these structures.

<h3 id="15.1.1">15.1.1 地址类型</h3>

Linux is, of course, a virtual memory system, meaning that the addresses seen by user programs do not directly correspond to the physical addresses used by the hardware. Virtual memory introduces a layer of indirection that allows a number of nice things. With virtual memory, programs running on the system can allocate far more memory than is physically available; indeed, even a single process can have a virtual address space larger than the system’s physical memory. Virtual memory also allows the program to play a number of tricks with the process’s address space, including mapping the program’s memory to device memory. 

Thus far[[迄今]](), we have talked about virtual and physical addresses, but a number of the details have been glossed over. The Linux system deals with several types of addresses, each with its own semantics. Unfortunately, the kernel code is not always very clear on exactly which type of address is being used in each situation, so the programmer must be careful.

The following is a list of address types used in Linux. Figure 15-1 shows how these address types relate to physical memory.

![Figure 15-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/15-1.PNG)

1. 用户虚拟地址
    
    These are the regular addresses seen by user-space programs. User addresses are either 32 or 64 bits in length, depending on the underlying hardware architecture, and each process has its own virtual address space. 

2. 物理地址

    用在处理器和系统内存之间。物理地址是32位或64位数量; 在某些情况下，甚至32位系统也可以使用更大的物理地址。

3. 总线地址 
    
    用在外设总线和内存之间。 Often, they are the same as the physical addresses used by the processor, but that is not necessarily the case. Some architectures can provide an I/O memory management unit (IOMMU) that remaps addresses between a bus and main memory. An IOMMU can make life easier in a number of ways (making a buffer scattered in memory appear contiguous to the device, for example), but programming the IOMMU is an extra step that must be performed when setting up DMA operations. Bus addresses are highly architecture dependent, of course. 

4. 内核逻辑地址
    
    这构成了内核的正常地址空间。这些地址映射主内存的部分（或许，所有），通常就被认为是物理地址。在大部分架构体系中，逻辑地址和其相关的物理地址仅仅就是地址偏移的差别。逻辑地址使用硬件的固有地址，因此，在32位系统上可能无法访问所有的物理地址。逻辑地址通常使用类型`unsigned long`或`void *`进行存储。使用函数`kmalloc`返回的地址就是内核逻辑地址。

5. 内核虚拟地址 
    
    内核虚拟地址和逻辑地址相似，它们是从内核空间地址到物理地址的映射。内核虚拟地址空间不必非得是线性的，一对一地到物理地址的映射。所有逻辑地址都是内核虚拟地址，但是许多内核虚拟地址不是逻辑地址。例如，由函数`vmalloc`分配的地址是虚拟地址（但不是直接物理映射）。`kmap`函数（后面还会再描述）也返回虚拟地址。虚拟地址通常存储在指针变量中。

If you have a logical address, the macro __pa() (defined in `<asm/page.h`>) returns its associated physical address. Physical addresses can be mapped back to logical addresses with __va(), but only for low-memory pages.

Different kernel functions require different types of addresses. It would be nice if there were different C types defined, so that the required address types were explicit, but we have no such luck. In this chapter, we try to be clear on which types of addresses are used where.

<h3 id="15.1.2">15.1.2 物理地址和页</h3>

Physical memory is divided into discrete units called pages. Much of the system’s internal handling of memory is done on a per-page basis. Page size varies from one architecture to the next, although most systems currently use 4096-byte pages. The constant PAGE_SIZE (defined in `<asm/page.h>`) gives the page size on any given architecture. 

If you look at a memory address—virtual or physical—it is divisible into a page number and an offset within the page. If 4096-byte pages are being used, for example, the 12 least-significant bits are the offset, and the remaining, higher bits indicate the page number. If you discard the offset and shift the rest of an offset to the right, the result is called a page frame number (PFN). Shifting bits to convert between page frame numbers and addresses is a fairly common operation; the macro PAGE_SHIFT tells how many bits must be shifted to make this conversion.

<h3 id="15.1.3">15.1.3 高内存和低内存</h3>

逻辑内核和内核虚拟地址之间的区别，主要是在配备大量内存的32位系统上。 32位系统，最大寻址空间4GB。 直到最近，Linux-32位系统的内存被限制远远小于此值，这是因为它设置虚拟地址空间的方式。

The kernel (on the x86 architecture, in the default configuration) splits the 4-GB virtual address space between user-space and the kernel; the same set of mappings is used in both contexts. A typical split dedicates 3 GB to user space, and 1 GB for kernel space.* The kernel’s code and data structures must fit into that space, but the biggest consumer of kernel address space is virtual mappings for physical memory. The kernel cannot directly manipulate memory that is not mapped into the kernel’s address space. The kernel, in other words, needs its own virtual address for any memory it must touch directly. Thus, for many years, the maximum amount of physical memory that could be handled by the kernel was the amount that could be mapped into the kernel’s portion of the virtual address space, minus the space needed for the kernel code itself. As a result, x86-based Linux systems could work with a maximum of a little under 1 GB of physical memory. 

In response to commercial pressure to support more memory while not breaking 32bit application and the system’s compatibility, the processor manufacturers have added “address extension” features to their products. The result is that, in many cases, even 32-bit processors can address more than 4 GB of physical memory. The limitation on how much memory can be directly mapped with logical addresses remains, however. Only the lowest portion of memory (up to 1 or 2 GB, depending on the hardware and the kernel configuration) has logical addresses;* the rest (high memory) does not. Before accessing a specific high-memory page, the kernel must set up an explicit virtual mapping to make that page available in the kernel’s address space. Thus, many kernel data structures must be placed in low memory; high memory tends to be reserved for user-space process pages. 

The term “high memory” can be confusing to some, especially since it has other meanings in the PC world. So, to make things clear, we’ll define the terms here: 

1. Low memory
    
    Memory for which logical addresses exist in kernel space. On almost every system you will likely encounter, all memory is low memory. 

2. 高内存
    
    Memory for which logical addresses do not exist, because it is beyond the address range set aside for kernel virtual addresses.

On i386 systems, the boundary between low and high memory is usually set at just under 1 GB, although that boundary can be changed at kernel configuration time. This boundary is not related in any way to the old 640 KB limit found on the original PC, and its placement is not dictated by the hardware. It is, instead, a limit set by the kernel itself as it splits the 32-bit address space between kernel and user space. We will point out limitations on the use of high memory as we come to them in this chapter.


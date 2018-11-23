<h2 id="0">0 目录</h2>

* [15.1 Linux内存管理](#15.1)
    - [15.1.1 地址类型](#15.1.1)
    - [15.1.2 物理地址和页](#15.1.2)
* [15.2 MMAP设备操作](#15.2)
* [15.3 进行直接I/O](#15.2)
* [15.4 直接内存存取](#15.2)

***

This chapter delves into the area of Linux memory management, with an emphasis on techniques that are useful to the device driver writer. Many types of driver programming require some understanding of how the virtual memory subsystem works; the material we cover in this chapter comes in handy more than once as we get into some of the more complex and performance-critical subsystems. The virtual memory subsystem is also a highly interesting part of the core Linux kernel and, therefore, it merits a look.

The material in this chapter is divided into three sections:

1. The first covers the implementation of the mmap system call, which allows the mapping of device memory directly into a user process’s address space. Not all devices require mmap support, but, for some, mapping device memory can yield significant performance improvements.

2. We then look at crossing the boundary from the other direction with a discussion of direct access to user-space pages. Relatively few drivers need this capability; in many cases, the kernel performs this sort of mapping without the driver even being aware of it. But an awareness of how to map user-space memory into the kernel (with get_user_pages) can be useful.

3. The final section covers direct memory access (DMA) I/O operations, which provide peripherals with direct access to system memory. Of course, all of these techniques require an understanding of how Linux memory management works, so we start with an overview of that subsystem.


<h2 id="15.1">15.1 Linux内存管理</h2>

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

内核为用户空间和内核空间分割这4GB的虚拟地址空间；在这两种情况下都使用相同的映射集。典型的分割方法就是用户空间-3GB，内核空间-1GB。内核的代码和数据结构必须适合其分配的空间，但是内核地址空间的最大用户是物理内存的映射。内核不会直接操作没有映射到内核地址空间的内存。换句话说，内核需要自己的虚拟地址空间用于它必须直接接触的任何内存。因而，多年来，被内核所处理的最大物理内存数量就是能够映射到内核的虚拟地址空间的部分的数量，减去内核代码本身所占用的空间。导致的结果就是，基于x86平台的Linux系统能够使用的最大地址空间小于1GB物理内存。

In response to commercial pressure to support more memory while not breaking 32bit application and the system’s compatibility, the processor manufacturers have added “address extension” features to their products. The result is that, in many cases, even 32-bit processors can address more than 4 GB of physical memory. The limitation on how much memory can be directly mapped with logical addresses remains, however. Only the lowest portion of memory (up to 1 or 2 GB, depending on the hardware and the kernel configuration) has logical addresses;* the rest (high memory) does not. Before accessing a specific high-memory page, the kernel must set up an explicit virtual mapping to make that page available in the kernel’s address space. Thus, many kernel data structures must be placed in low memory; high memory tends to be reserved for user-space process pages. 

The term “high memory” can be confusing to some, especially since it has other meanings in the PC world. So, to make things clear, we’ll define the terms here: 

1. 低内存
    
    内核空间中存在逻辑地址的内存。几乎在可能遇到的每个系统上，所有内存都是低内存。

2. 高内存
    
    不存在逻辑地址的内存，因为它超出了为内核虚拟地址预留的地址范围。

在i386平台上，高低内存之间的界限可能被设置低于1GB，当然了，这个界限可以在内核配置时改变。此界限值与早期PC上的旧640KB限制没有关系，且其布局不受硬件限制。相反，它是内核本身设置的限制，因为它在内核和用户空间之间分割了32位的地址空间。我们将在本章中指出对高内存使用的限制。

>  \* 2.6内核（带有补丁）可以在x86硬件上支持“4G / 4G”模式，从而以较低的性能成本实现更大的内核和用户虚拟地址空间。 

<h3 id="15.1.4">15.1.4 内存映射和struct Page</h3>

Historically, the kernel has used logical addresses to refer to pages of physical memory. The addition of high-memory support, however, has exposed an obvious problem with that approach—logical addresses are not available for high memory. Therefore, kernel functions that deal with memory are increasingly using pointers to struct page (defined in `<linux/mm.h>`) instead. This data structure is used to keep track of just about everything the kernel needs to know about physical memory;there is one struct page for each physical page on the system. Some of the fields of this structure include the following:

1. atomic_t count; 
    
    The number of references there are to this page. When the count drops to 0, the page is returned to the free list. 

2. void *virtual; 
    
    The kernel virtual address of the page, if it is mapped; NULL, otherwise. Lowmemory pages are always mapped; high-memory pages usually are not. This field does not appear on all architectures; it generally is compiled only where the kernel virtual address of a page cannot be easily calculated. If you want to look at this field, the proper method is to use the page_address macro, described below. 

3. unsigned long flags; 
    
    A set of bit flags describing the status of the page. These include PG_locked, which indicates that the page has been locked in memory, and PG_reserved, which prevents the memory management system from working with the page at all. 

There is much more information within struct page, but it is part of the deeper black magic of memory management and is not of concern to driver writers. 

The kernel maintains one or more arrays of struct page entries that track all of the physical memory on the system. On some systems, there is a single array called mem_map. On some systems, however, the situation is more complicated. Nonuniform memory access (NUMA) systems and those with widely discontiguous physical memory may have more than one memory map array, so code that is meant to be portable should avoid direct access to the array when ever possible. Fortunately,itis usually quite easyto just work with struct page pointers without worrying about where they come from. 

Some functions and macros are defined for translating between struct page pointers and virtual addresses:

1. struct page *virt_to_page(void *kaddr); 
    
    This macro, defined in <asm/page.h>, takes a kernel logical address and returns its associated struct page pointer. Since it requires a logical address, it does not work with memory from vmalloc or high memory. 

2. struct page *pfn_to_page(int pfn);
    
    Returns the struct page pointer for the given page frame number. If necessary, it checks a page frame number for validity with pfn_valid before passing it to pfn_to_page. 

3. void *page_address(struct page *page); 
    
    Returns the kernel virtual address of this page, if such an address exists. For high memory, that address exists only if the page has been mapped. This function is defined in `<linux/mm.h>`. In most situations, you want to use a version of kmap rather than page_address. 

    
`#include <linux/highmem.h>` 

4. void *kmap(struct page *page); 
5. void kunmap(struct page *page);
    
    kmap returns a kernel virtual address for any page in the system. For low-memory pages, it just returns the logical address of the page; for high-memory pages, kmap creates a special mapping in a dedicated part of the kernel address space. Mappings created with kmap should always be freed with kunmap; a limited number of such mappings is available, so it is better not to hold on to them for too long. kmap calls maintain a counter, so if two or more functions both call kmap on the same page, the right thing happens. Note also that kmap can sleep if no mappings are available. 

`#include <linux/highmem.h>` 

`#include <asm/kmap_types.h>`     

6. void *kmap_atomic(struct page *page, enum km_type type); 
7. void kunmap_atomic(void *addr, enum km_type type);     
    
    kmap_atomic is a high-performance form of kmap. Each architecture maintains a small list of slots (dedicated page table entries) for atomic kmaps; a caller of kmap_atomic must tell the system which of those slots to use in the type argument. The only slots that make sense for drivers are KM_USER0 and KM_USER1 (for code running directly from a call from user space), and KM_IRQ0 and KM_IRQ1 (for interrupt handlers). Note that atomic kmaps must be handled atomically; your code cannot sleep while holding one. Note also that nothing in the kernel keeps two functions from trying to use the same slot and interfering with each other (although there is a unique set of slots for each CPU). In practice, contention for atomic kmap slots seems to not be a problem.

We see some uses of these functions when we get into the example code, later in this chapter and in subsequent chapters.

<h3 id="15.1.5">15.1.5 页表</h3>

<h3 id="15.1.6">15.1.6 虚拟内存区</h3>

<h3 id="15.1.7">15.1.7 进程内存映射</h3>


<h2 id="15.2">15.2 mmap设备操作</h2>

内存映射是现代Unix系统最有趣的功能之一。就驱动程序而言，可以实现内存映射以向用户程序提供直接访问设备内存的功能。

通过查看X Window System服务器的虚拟内存区的子集，展示mmap使用：

    cat /proc/731/maps 
    000a0000-000c0000 rwxs 000a0000 03:01 282652      /dev/mem 
    000f0000-00100000 r-xs 000f0000 03:01 282652      /dev/mem 
    00400000-005c0000 r-xp 00000000 03:01 1366927     /usr/X11R6/bin/Xorg 
    006bf000-006f7000 rw-p 001bf000 03:01 1366927     /usr/X11R6/bin/Xorg 
    2a95828000-2a958a8000 rw-s fcc00000 03:01 282652  /dev/mem 
    2a958a8000-2a9d8a8000 rw-s e8000000 03:01 282652  /dev/mem ...

The full list of the X server’s VMAs is lengthy, but most of the entries are not of interest here. We do see, however, four separate mappings of /dev/mem, which give some insight into how the X server works with the video card. The first mapping is at a0000, which is the standard location for video RAM in the 640-KB ISA hole. Further down, we see a large mapping at e8000000, an address which is above the highest RAM address on the system. This is a direct mapping of the video memory on the adapter. 
X服务器的VMA的全部列表很长，但是大部分在这里也不需要。但是，我们可以看到`/dev/mem`有四个独立的映射，

可以在`/proc/iomem`查看这些区域:

    000a0000-000bffff : Video RAM area 
    000c0000-000ccfff : Video ROM 
    000d1000-000d1fff : Adapter ROM 
    000f0000-000fffff : System ROM 
    d7f00000-f7efffff : PCI Bus #01  
      e8000000-efffffff : 0000:01:00.0 
    fc700000-fccfffff : PCI Bus #01  
      fcc00000-fcc0ffff : 0000:01:00.0 

下面是`mini2440`上打印的结果：

    20000300-20000303 : dm9000
      20000300-20000303 : dm9000
    20000304-20000307 : dm9000
      20000304-20000307 : dm9000
    30000000-33ffffff : System RAM
      30008000-30437877 : Kernel code
      3045c000-304bd0df : Kernel data
    49000000-490000ff : s3c2410-ohci
      49000000-490000ff : ohci_hcd
    4d000000-4d0fffff : s3c2410-lcd
      4d000000-4d0fffff : s3c2410-lcd
    4e000000-4e0fffff : s3c2440-nand
      4e000000-4e0fffff : s3c2440-nand
    50000000-50003fff : s3c2440-uart.0
      50000000-500000ff : s3c2440-uart
    50004000-50007fff : s3c2440-uart.1
      50004000-500040ff : s3c2440-uart
    50008000-5000bfff : s3c2440-uart.2
      50008000-500080ff : s3c2440-uart
    52000000-520fffff : s3c2440-usbgadget
    53000000-530003ff : s3c2410-wdt
      53000000-530003ff : s3c2410-wdt
    54000000-54000fff : s3c2440-i2c
      54000000-54000fff : s3c2440-i2c
    55000000-550fffff : s3c24xx-iis
    57000000-570000ff : s3c2410-rtc
      57000000-570000ff : s3c2410-rtc
    5a000000-5a0fffff : s3c2440-sdi
      5a000000-5a0fffff : s3c2440-sdi

映射设备意味着把一段用户空间的地址和设备内存关联起来。只要程序在指定的地址范围内读写，就能直接访问设备。在X-server的例子中，使用mmap可以轻松快速地访问视频卡的内存区域。对于这种性能要求比较苛刻的引用程序，直接访问会产生很大的差异。

正如你所怀疑的那样，并不是每个设备适合于mmap抽象；例如，对于串行端口和其它的流控设备，这就没有意义。mmap的另一个限制就是映射的大小是以PAGE_SIZE为单位的。内核只能在页表级别管理虚拟地址；因此，映射的区域必须是PAGE_SIZE的倍数大小且对应于物理内存中的起始地址也必须是PAGE_SIZE倍数。如果该区域不是页大小的倍数，内核会扩展该区域使其成为PAGE_SIZE的倍数。

这些限制对于驱动来说不是很大的约束，因为程序访问设备毕竟还是依赖于设备的。因为程序必须知道设备的工作原理，程序员不需要因为要查看页对齐而困扰。一个更大的困扰是，当ISA设备应用在非x86平台上时，因为它们的ISA设备的视图不是连续的。例如，某些Alpha计算机把ISA内存看作是分散的8位，16位，32位的项，没有直接映射。在这些情况下，你不能直接使用mmap。不能直接将ISA地址直接映射到Alpha地址，是因为两个系统的数据传输规范的不兼容。因而，早期的Alpha处理器只能发起32位和64位内存访问，ISA只能进行8位和16位数据传输，它们之间没有直接的映射协议。

There are sound advantages to using mmap when it’s feasible to do so. For instance, we have already looked at the X server, which transfers a lot of data to and from video memory; mapping the graphic display to user space dramatically improves the throughput, as opposed to an lseek/write implementation. Another typical example is a program controlling a PCI device. Most PCI peripherals map their control registers to a memory address, and a high-performance application might prefer to have direct access to the registers instead of repeatedly having to call ioctl to get its work done. 

The mmap method is part of the file_operations structure and is invoked when the mmap system call is issued. With mmap, the kernel performs a good deal of work before the actual method is invoked, and, therefore, the prototype of the method is quite different from that of the system call. This is unlike calls such as ioctl and poll, where the kernel does not do much before calling the method. 

The system call is declared as follows (as described in the mmap(2) manual page):

    mmap(caddr_t addr, size_t len, int prot, int flags, int fd, off_t offset) 

其函数原型声明为：

    int (*mmap) (struct file *filp, struct vm_area_struct *vma); 

The filp argument in the method is the same as that introduced in Chapter 3, while vma contains the information about the virtual address range that is used to access the device. Therefore, much of the work has been done by the kernel; to implement mmap, the driver only has to build suitable page tables for the address range and, if necessary, replace vma->vm_ops with a new set of operations. 

There are two ways of building the page tables: doing it all at once with a function called remap_pfn_range or doing it a page at a time via the nopage VMA method. Each method has its advantages and limitations. We start with the “all at once” approach, which is simpler. From there, we add the complications needed for a realworld implementation.

<h3 id="15.2.1">15.2.1 使用remap_pfn_range</h3>

The job of building new page tables to map a range of physical addresses is handled by remap_pfn_range and io_remap_page_range, which have the following prototypes:

    int remap_pfn_range(struct vm_area_struct *vma,
                        unsigned long virt_addr, unsigned long pfn,
                        unsigned long size, pgprot_t prot);
    int io_remap_page_range(struct vm_area_struct *vma,
                        unsigned long virt_addr, unsigned long phys_addr,
                        unsigned long size, pgprot_t prot);

The value returned by the function is the usual 0 or a negative error code. Let’s look at the exact meaning of the function’s arguments:

1. vma
    
    The virtual memory area into which the page range is being mapped. 

2. virt_addr
    
    The user virtual address where remapping should begin. The function builds page tables for the virtual address range between virt_addr and virt_addr+size.

3. pfn
   
   The page frame number corresponding to the physical address to which the virtual address should be mapped. The page frame number is simply the physical address right-shifted by PAGE_SHIFT bits. For most uses, the vm_pgoff field of the VMA structure contains exactly the value you need. The function affects physical addresses from (pfn<<PAGE_SHIFT) to (pfn<<PAGE_SHIFT)+size. 

4. size
    
    The dimension, in bytes, of the area being remapped. 

5. prot
    
    The “protection” requested for the new VMA. The driver can (and should) use the value found in vma->vm_page_prot. 

The arguments to remap_pfn_range are fairly straightforward, and most of them are already provided to you in the VMA when your mmap method is called. You may be wondering why there are two functions, however. The first (remap_pfn_range) is intended for situations where pfn refers to actual system RAM, while io_remap_ page_range should be used when phys_addr points to I/O memory. In practice, the two functions are identical on every architecture except the SPARC, and you see remap_pfn_range used in most situations. In the interest of writing portable drivers, however, you should use the variant of remap_pfn_range that is suited to your particular situation. 

One other complication has to do with caching: usually, references to device memory should not be cached by the processor. Often the system BIOS sets things up properly, but it is also possible to disable caching of specific VMAs via the protection field. Unfortunately, disabling caching at this level is highly processor dependent. The curious reader may wish to look at the pgprot_noncached function from drivers/char/mem.c to see what’s involved. We won’t discuss the topic further here.

<h3 id="15.2.2">15.2.2 一个简单的实现</h3>

If your driver needs to do a simple, linear mapping of device memory into a user address space, remap_pfn_range is almost all you really need to do the job. The following code is derived from drivers/char/mem.c and shows how this task is performed in a typical module called simple (Simple Implementation Mapping Pages with Little Enthusiasm):

如果您的驱动程序需要将设备内存简单，线性映射到用户地址空间，则`remap_pfn_range`几乎就是您真正需要完成的任务。 以下代码派生自`drivers/char/mem.c`，并显示了如何在一个名为`simple`的简单模块中执行此任务：

    static int simple_remap_mmap(struct file *filp, struct vm_area_struct *vma)
    {
        if (remap_pfn_range(vma, vma->vm_start, vm->vm_pgoff,
                            vma->vm_end - vma->vm_start,
                            vma->vm_page_prot))
            return -EAGAIN;
        
        vma->vm_ops = &simple_remap_vm_ops;
        simple_vma_open(vma);
        return 0;
    }

As you can see, remapping memory just a matter of calling remap_pfn_range to create the necessary page tables.

<h3 id="15.2.3">15.2.3 添加VMA操作</h3>

As we have seen, the vm_area_struct structure contains a set of operations that may be applied to the VMA. Now we look at providing those operations in a simple way. In particular, we provide open and close operations for our VMA. These operations are called whenever a process opens or closes the VMA; in particular, the open method is invoked anytime a process forks and creates a new reference to the VMA. The open and close VMA methods are called in addition to the processing performed by the kernel, so they need not reimplement any of the work done there. They exist as a way for drivers to do any additional processing that they may require. 

As it turns out, a simple driver such as simple need not do any extra processing in particular. So we have created open and close methods, which print a message to the system log informing the world that they have been called. Not particularly useful, but it does allow us to show how these methods can be provided, and see when they are invoked. 

To this end, we override the default vma->vm_ops with operations that call printk:

    void simple_vma_open(struct vm_area_struct *vma) 
    {    
        printk(KERN_NOTICE "Simple VMA open, virt %lx, phys %lx\n",
                vma->vm_start, vma->vm_pgoff << PAGE_SHIFT); 
    }
    void simple_vma_close(struct vm_area_struct *vma)
    {
        printk(KERN_NOTICE "Simple VMA close.\n");
    }
    static struct vm_operations_struct simple_remap_vm_ops = {
        .open = simple_vma_open,
        .close = simple_vma_close,
    };

To make these operations active for a specific mapping, it is necessary to store a pointer to simple_remap_vm_ops in the vm_ops field of the relevant VMA. This is usually done in the mmap method. If you turn back to the simple_remap_mmap example, you see these lines of code:

    vma->vm_ops = &simple_remap_vm_ops;
    simple_vma_open(vma);

Note the explicit call to simple_vma_open. Since the open method is not invoked on the initial mmap, we must call it explicitly if we want it to run.

<h3 id="15.2.4">15.2.4 Mapping Memory with nopage</h3>

Although remap_pfn_range works well for many, if not most, driver mmap implementations, sometimes it is necessary to be a little more flexible. In such situations, an implementation using the nopage VMA method may be called for. 

One situation in which the nopage approach is useful can be brought about by the mremap system call, which is used by applications to change the bounding addresses of a mapped region. As it happens, the kernel does not notify drivers directly when a mapped VMA is changed by mremap. If the VMA is reduced in size, the kernel can quietly flush out the unwanted pages without telling the driver. If, instead, the VMA is expanded, the driver eventually finds out by way of calls to nopage when mappings must be set up for the new pages, so there is no need to perform a separate notification. The nopage method, therefore, must be implemented if you want to support the mremap system call. Here, we show a simple implementation of nopage for the simple device. 

The nopage method, remember, has the following prototype:

    struct page *(*nopage)(struct vm_area_struct *vma,
                            unsigned long address, int *type);

When a user process attempts to access a page in a VMA that is not present in memory, the associated nopage function is called. The address parameter contains the virtual address that caused the fault, rounded down to the beginning of the page. The nopage function must locate and return the struct page pointer that refers to the page the user wanted. This function must also take care to increment the usage count for the page it returns by calling the get_page macro:

    get_page(struct page *pageptr); 

This step is necessary to keep the reference counts correct on the mapped pages. The kernel maintains this count for every page; when the count goes to 0, the kernel knows that the page may be placed on the free list. When a VMA is unmapped, the kernel decrements the usage count for every page in the area. If your driver does not increment the count when adding a page to the area, the usage count becomes 0 prematurely, and the integrity of the system is compromised.

The nopage method should also store the type of fault in the location pointed to by the type argument—but only if that argument is not NULL. In device drivers, the proper value for type will invariably be VM_FAULT_MINOR. 

If you are using nopage, there is usually very little work to be done when mmap is called; our version looks like this:

    static int simple_nopage_mmap(struct file *filp, struct vm_area_struct *vma)
    {
        unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

        if (offset >= __pa(high_memory) || (filp->f_flags & O_SYNC))
            vma->vm_flags |= VM_IO;
        vma->vm_flags |= VM_RESERVED;
        
        vma->vm_ops = &simple_nopage_vm_ops;
        simple_vma_open(vma);
        return 0;
    }

The main thing mmap has to do is to replace the default (NULL) vm_ops pointer with our own operations. The nopage method then takes care of “remapping” one page at a time and returning the address of its struct page structure. Because we are just implementing a window onto physical memory here, the remapping step is simple: we only need to locate and return a pointer to the struct page for the desired address. Our nopage method looks like the following:

    struct page *simple_vma_nopage(struct vm_area_struct *vma,
                    unsigned long address, int *type)
    {
        struct page *pageptr;
        unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
        unsigned long physaddr = address - vma->vm_start + offset;
        unsigned long pageframe = physaddr >> PAGE_SHIFT;
        
        if (!pfn_valid(pageframe))
            return NOPAGE_SIGBUS;
        pageptr = pfn_to_page(pageframe);
        get_page(pageptr);
        if (type)
            *type = VM_FAULT_MINOR;
        return pageptr;
    }

Since, once again, we are simply mapping main memory here, the nopage function need only find the correct struct page for the faulting address and increment its reference count. Therefore, the required sequence of events is to calculate the desired physical address, and turn it into a page frame number by right-shifting it PAGE_SHIFT bits. Since user space can give us any address it likes, we must ensure that we have a valid page frame; the pfn_valid function does that for us. If the address is out of range, we return NOPAGE_SIGBUS, which causes a bus signal to be delivered to the calling process.

Otherwise, pfn_to_page gets the necessary struct page pointer; we can increment its reference count (with a call to get_page) and return it. 

The nopage method normally returns a pointer to a struct page. If, for some reason, a normal page cannot be returned (e.g., the requested address is beyond the device’s memory region), NOPAGE_SIGBUS can be returned to signal the error; that is what the simple code above does. nopage can also return NOPAGE_OOM to indicate failures caused by resource limitations. 

Note that this implementation works for ISA memory regions but not for those on the PCI bus. PCI memory is mapped above the highest system memory, and there are no entries in the system memory map for those addresses. Because there is no struct page to return a pointer to, nopage cannot be used in these situations; you must use remap_pfn_range instead. 

If the nopage method is left NULL, kernel code that handles page faults maps the zero page to the faulting virtual address. The zero page is a copy-on-write page that reads as 0 and that is used, for example, to map the BSS segment. Any process referencing the zero page sees exactly that: a page filled with zeroes. If the process writes to the page, it ends up modifying a private copy. Therefore, if a process extends a mapped region by calling mremap, and the driver hasn’t implemented nopage, the process ends up with zero-filled memory instead of a segmentation fault.


<h3 id="15.2.5">15.2.5 Remapping Speciﬁc I/O Regions </h3>

All the examples we’ve seen so far are reimplementations of /dev/mem; they remap physical addresses into user space. The typical driver, however, wants to map only the small address range that applies to its peripheral device, not all memory. In order to map to user space only a subset of the whole memory range, the driver needs only to play with the offsets. The following does the trick for a driver mapping a region of simple_region_size bytes, beginning at physical address simple_region_start (which should be page-aligned):

    unsigned long off = vma->vm_pgoff << PAGE_SHIFT;
    unsigned long physical = simple_region_start + off;
    unsigned long vsize = vma->vm_end - vma->vm_start;
    unsigned long psize = simple_region_size - off;

    if (vsize > psize)
        return -EINVAL; /* spans too high */
    remap_pfn_range(vma, vma_>vm_start, physical, vsize, vma->vm_page_prot);

In addition to calculating the offsets, this code introduces a check that reports an error when the program tries to map more memory than is available in the I/O region of the target device. In this code, psize is the physical I/O size that is left after the offset has been specified, and vsize is the requested size of virtual memory; the function refuses to map addresses that extend beyond the allowed memory range.

Note that the user process can always use mremap to extend its mapping, possibly past the end of the physical device area. If your driver fails to define a nopage method, it is never notified of this extension, and the additional area maps to the zero page. As a driver writer, you may well want to prevent this sort of behavior; mapping the zero page onto the end of your region is not an explicitly bad thing to do, but it is highly unlikely that the programmer wanted that to happen. 

The simplest way to prevent extension of the mapping is to implement a simple nopage method that always causes a bus signal to be sent to the faulting process. Such a method would look like this:

    struct page *simple_nopage(struct vm_area_struct *vma,                           
                                unsigned long address, int *type); 
    { 
        return NOPAGE_SIGBUS; /* send a SIGBUS */
    } 

As we have seen, the nopage method is called only when the process dereferences an address that is within a known VMA but for which there is currently no valid page table entry. If we have used remap_pfn_range to map the entire device region, the nopage method shown here is called only for references outside of that region. Thus, it can safely return NOPAGE_SIGBUS to signal an error. Of course, a more thorough implementation of nopage could check to see whether the faulting address is within the device area, and perform the remapping if that is the case. Once again, however, nopage does not work with PCI memory areas, so extension of PCI mappings is not possible.

<h3 id="15.2.6">15.2.6 Remapping RAM</h3>

An interesting limitation of remap_pfn_range is that it gives access only to reserved pages and physical addresses above the top of physical memory. In Linux, a page of physical addresses is marked as “reserved” in the memory map to indicate that it is not available for memory management. On the PC, for example, the range between 640 KB and 1 MB is marked as reserved, as are the pages that host the kernel code itself. Reserved pages are locked in memory and are the only ones that can be safely mapped to user space; this limitation is a basic requirement for system stability.

Therefore, remap_pfn_range won’t allow you to remap conventional addresses, which include the ones you obtain by calling get_free_page. Instead, it maps in the zero page. Everything appears to work, with the exception that the process sees private, zero-filled pages rather than the remapped RAM that it was hoping for. Nonetheless, the function does everything that most hardware drivers need it to do, because it can remap high PCI buffers and ISA memory.

The limitations of remap_pfn_range can be seen by running mapper, one of the sample programs in misc-progs in the files provided on O’Reilly’s FTP site. mapper is a simple tool that can be used to quickly test the mmap system call; it maps read-only parts of a file specified by command-line options and dumps the mapped region to standard output. The following session, for instance, shows that /dev/mem doesn’t map the physical page located at address 64 KB—instead, we see a page full of zeros (the host computer in this example is a PC, but the result would be the same on other platforms):

    morgana.root# ./mapper /dev/mem 0x10000 0x1000 | od -Ax -t x1
    mapped "/dev/mem" from 65536 to 69632
    000000 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    *
    001000

The inability of remap_pfn_range to deal with RAM suggests that memory-based devices like scull can’t easily implement mmap, because its device memory is conventional RAM, not I/O memory. Fortunately, a relatively easy workaround is available to any driver that needs to map RAM into user space; it uses the nopage method that we have seen earlier.

<h4 id="15.2.6.1">15.2.6.1 Remapping RAM with the nopage method</h4>

The way to map real RAM to user space is to use vm_ops->nopage to deal with page faults one at a time. A sample implementation is part of the scullp module, introduced in Chapter 8. 

scullp is a page-oriented char device. Because it is page oriented, it can implement mmap on its memory. The code implementing memory mapping uses some of the concepts introduced in the section “Memory Management in Linux.” 

Before examining the code, let’s look at the design choices that affect the mmap implementation in scullp: 

*  scullp doesn’t release device memory as long as the device is mapped. This is a matter of policy rather than a requirement, and it is different from the behavior of scull and similar devices, which are truncated to a length of 0 when opened for writing. Refusing to free a mapped scullp device allows a process to overwrite regions actively mapped by another process, so you can test and see how processes and device memory interact. To avoid releasing a mapped device, the driver must keep a count of active mappings; the vmas field in the device structure is used for this purpose. 

*  Memory mapping is performed only when the scullp order parameter (set at module load time) is 0. The parameter controls how __get_free_pages is invoked (see the section “get_free_page and Friends” in Chapter 8). The zero-order limitation (which forces pages to be allocated one at a time, rather than in larger groups) is dictated by the internals of __get_free_pages, the allocation function used by scullp. To maximize allocation performance, the Linux kernel maintains a list of free pages for each allocation order, and only the reference count of the first page in a cluster is incremented by get_free_pages and decremented by free_pages. The mmap method is disabled for a scullp device if the allocation order is greater than zero, because nopage deals with single pages rather than clusters of pages. scullp simply does not know how to properly manage reference counts for pages that are part of higher-order allocations. (Return to the section “A scull Using Whole Pages: scullp” in Chapter 8 if you need a refresher on scullp and the memory allocation order value.) 

The zero-order limitation is mostly intended to keep the code simple. It is possible to correctly implement mmap for multipage allocations by playing with the usage count of the pages, but it would only add to the complexity of the example without introducing any interesting information. 

Code that is intended to map RAM according to the rules just outlined needs to implement the open, close, and nopage VMA methods; it also needs to access the memory map to adjust the page usage counts. 

This implementation of scullp_mmap is very short, because it relies on the nopage function to do all the interesting work:

    int scullp_mmap(struct file *filp, struct vm_area_struct *vma)
    {
        struct inode *inode = filp->f_dentry->d_inode;
        
        /* refuse to map if order is not 0 */
        if (scullp_devices[iminor(inode)].order)
            return -ENODEV;
        
        /* don't do anything here: "nopage" will fill the holes */
        vma->vm_ops = &scullp_vm_ops;
        vma->vm_flags |= VM_RESERVED;
        vma->vm_private_data = filp->private_data;
        scullp_vma_open(vma);
        return 0;
    }

The purpose of the if statement is to avoid mapping devices whose allocation order is not 0. scullp’s operations are stored in the vm_ops field, and a pointer to the device structure is stashed in the vm_private_data field. At the end, vm_ops->open is called to update the count of active mappings for the device. 

open and close simply keep track of the mapping count and are defined as follows:

    void scullp_vma_open(struct vm_area_struct *vma)
    {
        struct scullp_dev *dev = vma->vm_private_data;
        dev->vmas++;
    }
    void scullp_vma_close(struct vm_area_struct *vma)
    {
        struct scullp_dev *dev = vma->vm_private_data;
        dev->vmas--;
    }

Most of the work is then performed by nopage. In the scullp implementation, the address parameter to nopage is used to calculate an offset into the device; the offset is then used to look up the correct page in the scullp memory tree:

    struct page *scullp_vma_nopage(struct vm_area_struct *vma,
                                    unsigned long address, int *type)
    {
        unsigned long offset;
        struct scullp_dev *ptr, *dev = vma->vm_private_data;
        struct page *page = NOPAGE_SIGBUS;
        void *pageptr = NULL; /* default to "missing" */
        
        down(&dev->sem);
        offset = (address - vma->vm_start) + (vma->vm_pgoff << PAGE_SHIFT);
        if (offset >= dev->size) goto out; /* out of range */
        
        /*
        * Now retrieve the scullp device from the list,then the page.
        * If the device has holes, the process receives a SIGBUS when
        * accessing the hole.
        */
        offset >>= PAGE_SHIFT; /* offset is a number of pages */
        for (ptr = dev; ptr && offset >= dev->qset;) {
            ptr = ptr->next;
            offset -= dev->qset;
        }
        if (ptr && ptr->data) pageptr = ptr->data[offset];
        if (!pageptr) goto out; /* hole or end-of-file */
        page = virt_to_page(pageptr);
        
        /* got it, now increment the count */
        get_page(page);
        if (type)
            *type = VM_FAULT_MINOR;
        out:
            up(&dev->sem);
            return page;
    }

scullp uses memory obtained with get_free_pages. That memory is addressed using logical addresses, so all scullp_nopage has to do to get a struct page pointer is to call virt_to_page. 

The scullp device now works as expected, as you can see in this sample output from the mapper utility. Here, we send a directory listing of /dev (which is long) to the scullp device and then use the mapper utility to look at pieces of that listing with mmap:

    morgana% ls -l /dev > /dev/scullp
    morgana% ./mapper /dev/scullp 0 140
    mapped "/dev/scullp" from 0 (0x00000000) to 140 (0x0000008c)
    total 232
    crw------- 1 root root 10, 10 Sep 15 07:40 adbmouse
    crw-r--r-- 1 root root 10, 175 Sep 15 07:40 agpgart
    morgana% ./mapper /dev/scullp 8192 200
    mapped "/dev/scullp" from 8192 (0x00002000) to 8392 (0x000020c8)
    d0h1494
    brw-rw---- 1 root floppy 2, 92 Sep 15 07:40 fd0h1660
    brw-rw---- 1 root floppy 2, 20 Sep 15 07:40 fd0h360
    brw-rw---- 1 root floppy 2, 12 Sep 15 07:40 fd0H360

<h3 id="15.2.7">15.2.7 Remapping Kernel Virtual Addresses</h3>

Although it’s rarely necessary, it’s interesting to see how a driver can map a kernel virtual address to user space using mmap. A true kernel virtual address, remember, is an address returned by a function such as vmalloc—that is, a virtual address mapped in the kernel page tables. The code in this section is taken from scullv, which is the module that works like scullp but allocates its storage through vmalloc. 

Most of the scullv implementation is like the one we’ve just seen for scullp, except that there is no need to check the order parameter that controls memory allocation. The reason for this is that vmalloc allocates its pages one at a time, because singlepage allocations are far more likely to succeed than multipage allocations. Therefore, the allocation order problem doesn’t apply to vmalloced space.

Beyond that, there is only one difference between the nopage implementations used by scullp and scullv. Remember that scullp, once it found the page of interest, would obtain the corresponding struct page pointer with virt_to_page. That function does not work with kernel virtual addresses, however. Instead, you must use vmalloc_to_page. So the final part of thescullv version of nopage looks like:

    /*
    * After scullv lookup, "page" is now the address of the page
    * needed by the current process. Since it's a vmalloc address,
    * turn it into a struct page.
    */
    page = vmalloc_to_page(pageptr);
    
    /* got it, now increment the count */
    get_page(page);
    if (type)
        *type = VM_FAULT_MINOR;
    out:
        up(&dev->sem);
        return page;

Based on this discussion, you might also want to map addresses returned by ioremap to user space. That would be a mistake, however; addresses from ioremap are special and cannot be treated like normal kernel virtual addresses. Instead, you should use remap_pfn_range to remap I/O memory areas into user space.


<h2 id="15.3">15.3 进行直接I/O</h2>

Most I/O operations are buffered through the kernel. The use of a kernel-space buffer allows a degree of separation between user space and the actual device; this separation can make programming easier and can also yield performance benefits in many situations. There are cases, however, where it can be beneficial to perform I/O directly to or from a user-space buffer. If the amount of data being transferred is large, transferring data directly without an extra copy through kernel space can speed things up. 

One example of direct I/O use in the 2.6 kernel is the SCSI tape driver. Streaming tapes can pass a lot of data through the system, and tape transfers are usually recordoriented, so there is little benefit to buffering data in the kernel. So, when the conditions are right (the user-space buffer is page-aligned, for example), the SCSI tape driver performs its I/O without copying the data. 

That said, it is important to recognize that direct I/O does not always provide the performance boost that one might expect. The overhead of setting up direct I/O (which involves faulting in and pinning down the relevant user pages) can be significant, and the benefits of buffered I/O are lost. For example, the use of direct I/O requires that the write system call operate synchronously; otherwise the application does not know when it can reuse its I/O buffer. Stopping the application until each write completes can slow things down, which is why applications that use direct I/O often use asynchronous I/O operations as well. 

The real moral of the story, in any case, is that implementing direct I/O in a char driver is usually unnecessary and can be hurtful. You should take that step only if you are sure that the overhead of buffered I/O is truly slowing things down. Note also that block and network drivers need not worry about implementing direct I/O at all; in both cases, higher-level code in the kernel sets up and makes use of direct I/O when it is indicated, and driver-level code need not even know that direct I/O is being performed. 

The key to implementing direct I/O in the 2.6 kernel is a function called get_user_pages, which is declared in<linux/mm.h> with the following prototype:

    int get_user_pages(struct task_struct *tsk,
                        struct mm_struct *mm,
                        unsigned long start,
                        int len,
                        int write,
                        int force,
                        struct page **pages,
                        struct vm_area_struct **vmas);

This function has several arguments:

1. tsk
    
    A pointer to the task performing the I/O; its main purpose is to tell the kernel who should be charged for any page faults incurred while setting up the buffer. This argument is almost always passed as current. 

2. mm
    
    A pointer to the memory management structure describing the address space to be mapped. The mm_struct structure is the piece that ties together all of the parts (VMAs) of a process’s virtual address space. For driver use, this argument should always be current->mm.

3. start 和 len
    
    start is the (page-aligned) address of the user-space buffer, and len is the length of the buffer in pages.

4. write 和 force 
    
    If write is nonzero, the pages are mapped for write access (implying, of course, that user space is performing a read operation). The force flag tells get_user_pages to override the protections on the given pages to provide the requested access; drivers should always pass 0 here.

5. pages 和 vmas
    
    Output parameters. Upon successful completion, pages contain a list of pointers to the struct page structures describing the user-space buffer, and vmas contains pointers to the associated VMAs. The parameters should, obviously, point to arrays capable of holding at least len pointers. Either parameter can be NULL, but you need, at least, the struct page pointers to actually operate on the buffer. 

get_user_pages is a low-level memory management function, with a suitably complex interface. It also requires that the mmap reader/writer semaphore for the address space be obtained in read mode before the call. As a result, calls to get_user_pages usually look something like:

    down_read(&current->mm->mmap_sem);
    result = get_user_pages(current, current->mm, ...);
    up_read(&current->mm->mmap_sem);

The return value is the number of pages actually mapped, which could be fewer than the number requested (but greater than zero).

Upon successful completion, the caller has a pages array pointing to the user-space buffer, which is locked into memory. To operate on the buffer directly, the kernelspace code must turn each struct page pointer into a kernel virtual address with kmap or kmap_atomic. Usually, however, devices for which direct I/O is justified are using DMA operations, so your driver will probably want to create a scatter/gather list from the array of struct page pointers. We discuss how to do this in the section, “Scatter/gather mappings.” 

Once your direct I/O operation is complete, you must release the user pages. Before doing so, however, you must inform the kernel if you changed the contents of those pages. Otherwise, the kernel may think that the pages are “clean,” meaning that they match a copy found on the swap device, and free them without writing them out to backing store. So, if you have changed the pages (in response to a user-space read request), you must mark each affected page dirty with a call to:

    void SetPageDirty(struct page *page); 

(This macro is defined in <linux/page-flags.h>). Most code that performs this operation checks first to ensure that the page is not in the reserved part of the memory map, which is never swapped out. Therefore, the code usually looks like:

    if (! PageReserved(page))    
        SetPageDirty(page);

Since user-space memory is not normally marked reserved, this check should not strictly be necessary, but when you are getting your hands dirty deep within the memory management subsystem, it is best to be thorough and careful.

Regardless of whether the pages have been changed, they must be freed from the page cache, or they stay there forever. The call to use is:

    void page_cache_release(struct page *page);

This call should, of course, be made after the page has been marked dirty, if need be.

<h3 id="15.3.1">15.3.1 异步I/O</h3>

One of the new features added to the 2.6 kernel was the asynchronous I/O capability. Asynchronous I/O allows user space to initiate operations without waiting for their completion; thus, an application can do other processing while its I/O is in flight. A complex, high-performance application can also use asynchronous I/O to have multiple operations going at the same time. 

The implementation of asynchronous I/O is optional, and very few driver authors bother; most devices do not benefit from this capability. As we will see in the coming chapters, block and network drivers are fully asynchronous at all times, so only char drivers are candidates for explicit asynchronous I/O support. A char device can benefit from this support if there are good reasons for having more than one I/O operation outstanding at any given time. One good example is streaming tape drives, where the drive can stall and slow down significantly if I/O operations do not arrive quickly enough. An application trying to get the best performance out of a streaming drive could use asynchronous I/O to have multiple operations ready to go at any given time.

For the rare driver author who needs to implement asynchronous I/O, we present a quick overview of how it works. We cover asynchronous I/O in this chapter, because its implementation almost always involves direct I/O operations as well (if you are buffering data in the kernel, you can usually implement asynchronous behavior without imposing the added complexity on user space).

Drivers supporting asynchronous I/O should include <linux/aio.h>. There are three file_operations methods for the implementation of asynchronous I/O:

    ssize_t (*aio_read) (struct kiocb *iocb, char *buffer,
                        size_t count, loff_t offset);
    ssize_t (*aio_write) (struct kiocb *iocb, const char *buffer,
                        size_t count, loff_t offset);
    int (*aio_fsync) (struct kiocb *iocb, int datasync);

The aio_fsync operation is only of interest to filesystem code, so we do not discuss it further here. The other two, aio_read and aio_write, look very much like the regular read and write methods but with a couple of exceptions. One is that the offset parameter is passed by value; asynchronous operations never change the file position, so there is no reason to pass a pointer to it. These methods also take the iocb (“I/O control block”) parameter, which we get to in a moment.

The purpose of the aio_read and aio_write methods is to initiate a read or write operation that may or may not be complete by the time they return. If it is possible to complete the operation immediately, the method should do so and return the usual status: the number of bytes transferred or a negative error code. Thus, if your driver has a read method called my_read, the following aio_read method is entirely correct (though rather pointless):

    static ssize_t my_aio_read(struct kiocb *iocb, char *buffer,
                                ssize_t count, loff_t offset)
    {
        return my_read(iocb->ki_filp, buffer, count, &offset);
    }

Note that the struct file pointer is found in the ki_filp field of the kiocb structure.

If you support asynchronous I/O, you must be aware of the fact that the kernel can, on occasion, create “synchronous IOCBs.” These are, essentially, asynchronous operations that must actually be executed synchronously. One may well wonder why things are done this way, but it’s best to just do what the kernel asks. Synchronous operations are marked in the IOCB; your driver should query that status with:

    int is_sync_kiocb(struct kiocb *iocb); 

If this function returns a nonzero value, your driver must execute the operation synchronously. 

In the end, however, the point of all this structure is to enable asynchronous operations. If your driver is able to initiate the operation (or, simply, to queue it until some future time when it can be executed), it must do two things: remember everything it needs to know about the operation, and return -EIOCBQUEUED to the caller. Remembering the operation information includes arranging access to the user-space buffer; once you return, you will not again have the opportunity to access that buffer while running in the context of the calling process. In general, that means you will likely have to set up a direct kernel mapping (with get_user_pages) or a DMA mapping. The -EIOCBQUEUED error code indicates that the operation is not yet complete, and its final status will be posted later.

When “later” comes, your driver must inform the kernel that the operation has completed. That is done with a call to aio_complete:

    int aio_complete(struct kiocb *iocb, long res, long res2); 

Here, iocb is the same IOCB that was initially passed to you, and res is the usual result status for the operation. res2 is a second result code that will be returned to user space; most asynchronous I/O implementations pass res2 as 0. Once you call aio_complete, you should not touch the IOCB or user buffer again.

<h3 id="15.3.2">15.3.2 异步I/O例子</h3>

The page-oriented scullp driver in the example source implements asynchronous I/O. The implementation is simple, but it is enough to show how asynchronous operations should be structured. 

The aio_read and aio_write methods don’t actually do much:

    static ssize_t scullp_aio_read(struct kiocb *iocb, char *buf, size_t count,
                                    loff_t pos)
    {
        return scullp_defer_op(0, iocb, buf, count, pos);
    }
    static ssize_t scullp_aio_write(struct kiocb *iocb, const char *buf,
                                    size_t count, loff_t pos)
    {
        return scullp_defer_op(1, iocb, (char *) buf, count, pos);
    }

These methods simply call a common function:

    struct async_work {
        struct kiocb *iocb;
        int result;
        struct work_struct work;
    };
    
    static int scullp_defer_op(int write, struct kiocb *iocb, char *buf,
                                size_t count, loff_t pos)
    {
        struct async_work *stuff;
        int result;
        
        /* Copy now while we can access the buffer */
        if (write)
            result = scullp_write(iocb->ki_filp, buf, count, &pos);
        else
         result = scullp_read(iocb->ki_filp, buf, count, &pos);
        
        /* If this is a synchronous IOCB, we return our status now. */
        if (is_sync_kiocb(iocb))
            return result;
        
        /* Otherwise defer the completion for a few milliseconds. */
        stuff = kmalloc (sizeof (*stuff), GFP_KERNEL);
        if (stuff = = NULL)
            return result; /* No memory, just complete now */
        stuff->iocb = iocb;
        stuff->result = result;
        INIT_WORK(&stuff->work, scullp_do_deferred_op, stuff);
        schedule_delayed_work(&stuff->work, HZ/100);
        return -EIOCBQUEUED;
    }

A more complete implementation would use get_user_pages to map the user buffer into kernel space. We chose to keep life simple by just copying over the data at the outset. Then a call is made to is_sync_kiocb to see if this operation must be completed synchronously; if so, the result status is returned, and we are done. Otherwise we remember the relevant information in a little structure, arrange for “completion” via a workqueue, and return -EIOCBQUEUED. At this point, control returns to user space.

Later on, the workqueue executes our completion function:

    static void scullp_do_deferred_op(void *p)
    {
        struct async_work *stuff = (struct async_work *) p;
        aio_complete(stuff->iocb, stuff->result, 0);
        kfree(stuff);
    }

Here, it is simply a matter of calling aio_complete with our saved information. A real driver’s asynchronous I/O implementation is somewhat more complicated, of course, but it follows this sort of structure.


<h2 id="15.4">15.4 直接内存存取</h2>

    
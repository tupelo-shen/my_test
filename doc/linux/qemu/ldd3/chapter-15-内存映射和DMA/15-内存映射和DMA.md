<h2 id="0">0 目录</h2>

* [15.1 Linux内存管理](#15.1)
    - [15.1.1 地址类型](#15.1.1)
    - [15.1.2 物理地址和页](#15.1.2)
    - [15.1.3 高内存和低内存](#15.1.3)
    - [15.1.4 内存映射和struct Page](#15.1.4)
    - [15.1.5 页表](#15.1.5)
    - [15.1.6 虚拟内存区](#15.1.6)
    - [15.1.7 进程内存映射](#15.1.7)
* [15.2 MMAP设备操作](#15.2)
    - [15.2.1 使用remap_pfn_range](#15.2.1)
    - [15.2.2 一个简单的实现](#15.2.2)
    - [15.2.3 添加VMA操作](#15.2.3)
    - [15.2.4 使用nopage方法映射内存](#15.2.4)
    - [15.2.5 重映射特定的I/O区域](#15.2.5)
    - [15.2.6 重映射RAM](#15.2.6)
        + [15.2.6.1 使用nopage方法重新映射RAM](#15.2.6.1)
    - [15.2.7 重映射内核虚拟地址](#15.2.7)
* [15.3 进行直接I/O](#15.3.1)
    - [15.3.1 异步I/O](#15.3.1)
    - [15.3.2 异步I/O例子](#15.3.2)
* [15.4 直接内存存取](#15.2)

***

本文深入探讨了Linux内存管理的部分，重点介绍了对设备驱动作者有用的技术。许多驱动程序编程要求理解虚拟内存子系统的工作原理；当我们进入更复杂和性能关键的子系统时，本章讨论的内容不止一次派上用场。虚拟内存子系统也是Linux内核非常有意思的一部分，因此，值得一看。

本章内容被分成3部分：

1. 第一部分介绍`mmap`系统调用的实现，在其中，允许设备内存区域直接映射到用户进程的地址空间。并不是所有的设备都要求`mmap`支持，但是，对于其中的一些设备，如果实现设备内存的映射，会产生显著的性能提升。

2. 然后，我们将讨论如何通过将用户空间的地址映射到内核中，而实现直接访问用户空间。相对地，较少的驱动编程者需要了解此项能力；大多数情况下，驱动编写者都没有意识到内核实现了这种映射。但是，了解用户空间的内存映射到内核中（使用`get_user_pages`）是非常有用的。

3. 最后一部分，我们讨论了对于I/O的直接内存访问（DMA），其提供了外设直接访问系统内存的能力。当然了，所有的这些技术都需要理解Linux内存管理的工作原理，所以，先让我们开始了解该子系统。


<h2 id="15.1">15.1 Linux内存管理</h2>

本节不是描述内存管理的理论，而是探究Linux实现的功能。尽管，你不需要成为一个实现`mmap`的Linux虚拟内存的专家，但是，对其如何工作的基本概述还是非常有用的。以下就是一段非常冗长的数据结构体的描述，内核正是用其管理内存。先让我们了解这些背景知识，然后再使用它们。

<h3 id="15.1.1">15.1.1 地址类型</h3>

当然了，Linux是一个虚拟内存系统，这意味着用户程序看到的地址并不直接对应于硬件使用的物理地址。虚拟内存，引入了一层中间层。使用虚拟地址，运行在系统上的程序能够分配比可用物理内存还大的内存空间；事实上，即使单个进程也可以拥有大于物理内存的虚拟地址空间。虚拟内存也可以允许程序，对于进程的地址空间使用一些技巧，包括映射程序的内存到设备的内存。

迄今为止，我们已经讨论了虚拟和物理地址，但是略过了许多细节。Linux系统处理多个类型的地址，每一个都有自己的语义。不幸的是，内核代码在那种情况下使用哪种类型的地址定义并不是很清晰，所以，编程者必须小心。

下面就是Linux使用的地址类型。 `Figure 15-1` 展示了这些类型的地址和实际的物理内存的关系。

![Figure 15-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/15-1.PNG)

1. 用户虚拟地址
    
    这是用户空间的程序里经常看见的常规地址。用户地址通常是32位或64位，这依赖于具体的硬件架构，每个进程都有其自己的虚拟地址空间。

2. 物理地址

    用在处理器和系统内存之间。物理地址是32位或64位数量; 在某些情况下，甚至32位系统也可以使用更大的物理地址。

3. 总线地址 
    
    用在外设总线和内存之间的地址。通常，它们和处理器使用的物理地址相同，但是，并不是完全如此。一些架构实现了I/O内存管理单元（IOMMU），其重映射了总线和主内存之间的地址。IOMMU可以使用多种方式使事情变得更轻松（例如，使分散在内存中的缓冲区，相对于设备，看起来是连续的），但是，使用IOMMU编程，设置DMA是一步必须的操作。当然了总线地址高度依赖架构。 

4. 内核逻辑地址
    
    这构成了内核的正常地址空间。这些地址映射主内存的部分（或许，所有），通常就被认为是物理地址。在大部分架构体系中，逻辑地址和其相关的物理地址仅仅就是地址偏移的差别。逻辑地址使用硬件的固有地址，因此，在32位系统上可能无法访问所有的物理地址。逻辑地址通常使用类型`unsigned long`或`void *`进行存储。使用函数`kmalloc`返回的地址就是内核逻辑地址。

5. 内核虚拟地址 
    
    内核虚拟地址和逻辑地址相似，它们是从内核空间地址到物理地址的映射。内核虚拟地址空间不必非得是线性的，一对一地对物理地址的映射。所有逻辑地址都是内核虚拟地址，但是许多内核虚拟地址不是逻辑地址。例如，由函数`vmalloc`分配的地址是虚拟地址（但不是直接物理映射）。`kmap`函数（后面还会再描述）也返回虚拟地址。虚拟地址通常存储在指针变量中。

使用宏定义`__pa()`（其定义位于`<asm/page.h>`）,将逻辑地址转换成相关的物理地址。也可以使用`__va()`将物理地址重新映射回逻辑地址， 但是，这只适用于低地址。

不同的内核函数要求不同类型的地址。在本章中，我们试图明确在哪里使用哪种类型。

<h3 id="15.1.2">15.1.2 物理地址和页</h3>

物理内存的构成单元是`page`，在系统的内存处理都是以`page`为基础进行的。尽管大多数系统使用页的大小为'4096'，但是其大小因架构而异。 常数`PAGE_SIZE`给出了每种架构的页面大小，其定义位于`<asm/page.h>`中。

如果你仔细观察内存地址-虚拟或物理内存-它可以分为页码和页面内的偏移量。如果使用大小为4096的页，低12位表示偏移量，其余的表示页数。如果去掉低位，然后将其右移，结果就称为页帧编号（PFN）。这种位移操作可以使用宏`PAGE_SHIFT`。

<h3 id="15.1.3">15.1.3 高内存和低内存</h3>

逻辑地址和内核虚拟地址之间的区别，主要是在配备大量内存的32位系统上。 32位系统，最大寻址空间4GB。 直到最近，由于它设置虚拟地址的方式 ，Linux 32位系统的内存被限制而远远小于此值。

内核为用户空间和内核空间分割这4GB的虚拟地址空间；在这两种情况下都使用相同的映射集。典型的分割方法就是用户空间-3GB，内核空间-1GB。内核的代码和数据结构必须适合其分配的空间，但是内核地址空间的最大用户是物理内存的映射。内核不会直接操作没有映射到内核地址空间的内存。换句话说，内核需要自己的虚拟地址空间用于它必须直接接触的任何内存。因而，多年来，被内核所处理的最大物理内存数量就是能够映射到内核的虚拟地址空间的部分的数量，减去内核代码本身所占用的空间。导致的结果就是，基于x86平台的Linux系统能够使用的最大地址空间小于1GB物理内存。

为了应对商业压力以支持更多内存而又不破坏32位应用程序和系统的兼容性，处理器制造商添加了“地址扩展”功能。这样一来，即使是32位的处理器也可以处理大于4GB的物理内存。但是，可以直接映射到逻辑地址的数量限制仍然存在。只有物理内存的最低地址部分具有逻辑地址（达到1或2GB，依赖于硬件和内核配置）；剩余的不行（高内存）。在访问特定的高内存页之前，内核必须建立显式虚拟映射，以便物理内存页在内核地址空间里可用。因而，许多内核数据结构被放在低内存；高内存往往保留给用户空间进程页。

术语“高内存”可能让人感到困惑，尤其是因为其在PC地址还有其它的意义。所以，为了清楚说明，在这里，我们定义如下：

1. 低内存
    
    内核空间中存在逻辑地址的内存。几乎在可能遇到的每个系统上，所有内存都是低内存。

2. 高内存
    
    不存在逻辑地址的内存，因为它超出了为内核虚拟地址预留的地址范围。

在i386平台上，高低内存之间的界限可能被设置低于1GB，当然了，这个界限可以在内核配置时改变。此界限值与早期PC上的旧640KB限制没有关系，且其布局不受硬件限制。相反，它是内核本身设置的限制，因为它在内核和用户空间之间分割了32位的地址空间。我们将在本章中指出对高内存使用的限制。

>  \* 2.6内核（带有补丁）可以在x86硬件上支持“4G / 4G”模式，从而以较低的性能成本实现更大的内核和用户虚拟地址空间。 

<h3 id="15.1.4">15.1.4 内存映射和struct Page</h3>

历史上，内核使用逻辑地址引用物理内存页。然而，增加高内存的支持后，这种方法暴露了明显的问题，对于高地址来说，逻辑地址不可用。因此，处理内存的内核函数越来越多地使用指向`struct page`的指针（其定义位于`<linux/mm.h>`），此数据结构用于追踪内核需要了解的有关物理内存的所有内容；系统上的每个物理页面都有一个`struct page`与之对应。 下面是其一些常用的结构成员：

1. atomic_t count; 
    
    页面的引用数量。当其为0时，页面将返回到空闲列表。 

2. void *virtual; 
    
    `page`的内核虚拟地址（如果已映射）；否则为`NULL`。低内存页总是被映射，高内存页通常不被映射。该字段并不是在所有的架构中都出现；它通常只在内核虚拟地址无法轻松计算出的地方被编译。如果你想查看该成员的内容，可以使用宏`page_address`，后面会有描述。

3. unsigned long flags; 
    
    
    一组描述页面状态的位标志。这包含`PG_locked`，它表明页面已经被锁定在内存中，`PG_reserved`，这会组织内存管理系统根本不使用页面 

`struct page`中还有更多信息，但它是内存管理更深层次的内容，并不是驱动程序编写者所关心的。

内核维护着一个或多个数组，其包含着`struct page`条目，用其追踪所有的物理内存。在一些系统上，有一个名为`mem_map`的单一数组。但是，在另一些系统上，情况就更为复杂了。非均匀内存访问系统（NUMA）和那些具有宽泛的不连续物理内存的系统可能具有多个内存映射数组，所以，可移植的代码应该避免在可能的情况下直接访问阵列。幸运的是，通常很容易使用`struct page`指针而不必担心它们来自何处。

下面是转换`struct page`指针和虚拟地址的一些函数和宏：

1. struct page *virt_to_page(void *kaddr); 
    
    宏定义位于`<asm/page.h>`中，需要一个逻辑地址，返回与它相关的`struct page`指针。因为它的参数是逻辑地址，所以它不能使用`vmalloc`函数返回的内存和高内存。 

2. struct page *pfn_to_page(int pfn);
    
    根据给定的页帧编号，返回相关的`struct page`指针。可以使用`pfn_valid`检查参数是否合法。

3. void *page_address(struct page *page); 
    
    返回该页的内核虚拟地址，如果这个地址存在。对于高内存，只有这个页面被映射后，地址才会存在。这个函数定义位于`<linux/mm.h>`文件中。在大多数情况下，需要使用`kmap`而不是`page_address`。
    
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

在现代操作系统中，处理器必须有将虚拟地址映射到物理地址的机制。这个机制就称为“页表”；其本质上就是一个树形结构数组，包含虚拟到物理内存的映射和一些相关的标志。即使不使用页表的架构上，Linux内核也维护着这样的一组页表。

通常情况下，有设备驱动程序执行的许多操作都可以操作页表。幸运的是，2.6版本的内核消除了直接操作页表的必要性。于是，我们就不在此详细描述其细节了；感兴趣的读者可以阅读`Daniel P. Bovet` 和 `Marco Cesati (O’Reilly)`它们对Linux内核的理解。

<h3 id="15.1.6">15.1.6 虚拟内存区</h3>

虚拟内存区（`VMA`）是用来管理进程的地址空间的目标区域的内核数据结构。`VMA`表示进程的虚拟内存中的一块同类的区域：就是一段连续的虚拟地址，它们拥有相同的权限标志并由同一对象进行备份（一个文件，或者说交换空间）。它大概与“段”的概念相同，尽管将其描述为`一个具有相同属性的内存对象`可能更为合适。进程的内存布局至少由下面这些区域组成：

* 程序可执行代码区（通常称为`text`） 

* 数据的多个区域，包括初始化数据（在程序开始执行之前显式指定数值的数据），未初始化数据（BSS），和堆栈（stack）

* 程序中使用的动态库之类的内存映射 

具体可以查看下图，系统内存分布图：

![Figure 15-1-6](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/15-1-6.png)

一个进程的内存区域可以在`/proc/<pid/maps>`文件中查看（在这里，`pid`代表的是进程的实际ID）。`/proc/self`是`/proc/pid`的一种特殊情况，总是引用当前的进程。下面我们来看几个内存映射（我们在每行后面做了简要说明）：

    # cat /proc/1/maps **look at init**
    08048000-0804e000 r-xp 00000000 03:01 64652 /sbin/init # text
    0804e000-0804f000 rw-p 00006000 03:01 64652 /sbin/init # data
    0804f000-08053000 rwxp 00000000 00:00 0 zero-mapped BSS
    40000000-40015000 r-xp 00000000 03:01 96278 /lib/ld-2.3.2.so text
    40015000-40016000 rw-p 00014000 03:01 96278 /lib/ld-2.3.2.so data
    40016000-40017000 rw-p 00000000 00:00 0 BSS for ld.so
    42000000-4212e000 r-xp 00000000 03:01 80290 /lib/tls/libc-2.3.2.so text
    4212e000-42131000 rw-p 0012e000 03:01 80290 /lib/tls/libc-2.3.2.so data
    42131000-42133000 rw-p 00000000 00:00 0 BSS for libc
    bffff000-c0000000 rwxp 00000000 00:00 0 Stack segment
    ffffe000-fffff000 ---p 00000000 00:00 0 vsyscall page

    # rsh wolf cat /proc/self/maps #### x86-64 (trimmed)
    00400000-00405000 r-xp 00000000 03:01 1596291 /bin/cat text
    00504000-00505000 rw-p 00004000 03:01 1596291 /bin/cat data
    00505000-00526000 rwxp 00505000 00:00 0 bss
    3252200000-3252214000 r-xp 00000000 03:01 1237890 /lib64/ld-2.3.3.so
    3252300000-3252301000 r--p 00100000 03:01 1237890 /lib64/ld-2.3.3.so
    3252301000-3252302000 rw-p 00101000 03:01 1237890 /lib64/ld-2.3.3.so
    7fbfffe000-7fc0000000 rw-p 7fbfffe000 00:00 0 stack
    ffffffffff600000-ffffffffffe00000 ---p 00000000 00:00 0 vsyscall

每个字段的意义说明如下：

    start-end perm offset major:minor inode image

`/proc/*/maps`中的每个字段（除了`image`名称）都对应于结构体`struct vm_area_struct`中的一个成员：

* end/start
    
    这段内存的起始/结束位置。

* perm
    
    内存区域的读、写和执行权限的位掩码。此字段描述了对这段区域内的页可进行的操作。该字段的最后一个字符`p`表示`private`或`shared`。 

* offset
    
    内存区域在它所映射的文件里的位置。偏移量为0表示内存区域开始处对应于文件的开始处。

* major/minor  
    
    保存拥有已映射文件的设备的主次设备号。令人困惑的是，对于设备映射，主次设备号引用的是由用户打开的设备文件的磁盘分区的主次设备号，而不是设备本身的设备号。

* inode  
    
    映射文件的节点数量。

* image   
    
    已映射文件的名称（通常是可执行镜像）。

测试程序simple_test的系统内存分布（`cat /proc/835/maps`，系统平台是`mini2440`，内核是`Linux3.3.7`）：

    00008000-00009000 r-xp 00000000 00:0e 2133150    /aaron/simple_test # text
    00010000-00011000 rwxp 00000000 00:0e 2133150    /aaron/simple_test # data
    b6f8a000-b6fcc000 r-xp 00000000 00:0e 1999623    /lib/libuClibc-0.9.33.2.so
    b6fcc000-b6fd3000 ---p 00000000 00:00 0         # bss
    b6fd3000-b6fd4000 r-xp 00041000 00:0e 1999623    /lib/libuClibc-0.9.33.2.so
    b6fd4000-b6fd5000 rwxp 00042000 00:0e 1999623    /lib/libuClibc-0.9.33.2.so
    b6fd5000-b6fd9000 rwxp 00000000 00:00 0 
    b6fd9000-b6fdf000 r-xp 00000000 00:0e 1999615    /lib/ld-uClibc-0.9.33.2.so
    b6fe2000-b6fe4000 r-xs 00000000 00:0e 1880931    /dev/simple
    b6fe4000-b6fe6000 rwxp 00000000 00:00 0 
    b6fe6000-b6fe7000 r-xp 00005000 00:0e 1999615    /lib/ld-uClibc-0.9.33.2.so
    b6fe7000-b6fe8000 rwxp 00006000 00:0e 1999615    /lib/ld-uClibc-0.9.33.2.so
    bec31000-bec52000 rw-p 00000000 00:00 0          [stack]
    ffff0000-ffff1000 r-xp 00000000 00:00 0          [vectors]

程序的打印结果是

    vma information
    vma->vm_start = b6fe2000;
    vma->vm_end = b6fe4000;
    vma->vm_pgoff = 0;
    Simple VMA open, virt b6fe2000, phys 0
    b6fe2000
    b6fe3000
    Simple VMA close.


<h4 id="15.1.6.1">15.1.6.1 vm_area_struct</h4>

当用户空间的进程调用`mmap`方法时，映射设备内存到它自己的地址空间，系统通过建立一个新的`VMA`表述这种映射。支持`mmap`的驱动程序需要该`VMA`的初始化。为此，驱动编写者至少应该理解VMA。

让我们看一下`struct vm_area_struct`中最重要的成员，其定义位于(`<linux/mm.h>`)。设备驱动程序在它们的`mmap`实现中都可以使用这些成员。请注意，内核维护VMA列表和树已优化内存查找，`struct vm_area_struct`中的几个成员用来维护这个组织。因此，VMA不是驱动想在哪里创建就创建的。VMA的主要成员如下：（注意这些成员和我们刚刚看到的/proc输出之间的相似性）

* unsigned long vm_start;
* unsigned long vm_end;   
    
    该VMA的起始/结束虚拟地址。这也就是`/proc/*/maps`中前2个成员。

* struct file *vm_file;
    
    一个指向`struct file`结构的指针，该文件结构与该虚拟地址区域相关（如果有的话）。

* unsigned long vm_pgoff;

    文件中对应的内存区域的偏移量，单位为`页`。当一个文件或设备被映射时，这是映射到该区域的第一页的文件位置。

* unsigned long vm_flags;

    A set of flags describing this area. The flags of the most interest to device driver writers are VM_IO and VM_RESERVED. VM_IO marks a VMA as being a memory mapped I/O region. Among other things, the VM_IO flag prevents the region from being included in process core dumps. VM_RESERVED tells the memory management system not to attempt to swap out this VMA; it should be set in most device mappings.

* struct vm_operations_struct *vm_ops; 
    
    A set of functions that the kernel may invoke to operate on this memory area. Its presence indicates that the memory area is a kernel “object,” like the struct file we have been using throughout the book.

* void *vm_private_data;
    
    A field that may be used by the driver to store its own information. 

Like `struct vm_area_struct`, the `vm_operations_struct` is defined in `<linux/mm.h>`; it includes the operations listed below. These operations are the only ones needed to handle the process’s memory needs, and they are listed in the order they are declared. Later in this chapter, some of these functions are implemented.

* void (*open)(struct vm_area_struct *vma);
    
    The open method is called by the kernel to allow the subsystem implementing the VMA to initialize the area. This method is invoked any time a new reference to the VMA is made (when a process forks, for example). The one exception happens when the VMA is first created by mmap; in this case, the driver’s mmap method is called instead. 

* void (*close)(struct vm_area_struct *vma)
    
    The open method is called by the kernel to allow the subsystem implementing the VMA to initialize the area. This method is invoked any time a new reference to the VMA is made (when a process forks, for example). The one exception happens when the VMA is first created by mmap; in this case, the driver’s mmap method is called instead.

* struct page \*(*nopage)(struct vm_area_struct *vma, unsigned long address, int *type); 
    
    When a process tries to access a page that belongs to a valid VMA, but that is currently not in memory, the nopage method is called (if it is defined) for the related area. The method returns the struct page pointer for the physical page after, perhaps, having read it in from secondary storage. If the nopage method isn’t defined for the area, an empty page is allocated by the kernel.

* int (*populate)(struct vm_area_struct *vm, unsigned long address, unsigned  long len, pgprot_t prot, unsigned long pgoff, int nonblock);
    
    This method allows the kernel to “prefault” pages into memory before they are accessed by user space. There is generally no need for drivers to implement the populate method.


<h3 id="15.1.7">15.1.7 进程内存映射</h3>

The final piece of the memory management puzzle is the process memory map structure, which holds all of the other data structures together. Each process in the system (with the exception of a few kernel-space helper threads) has a `struct mm_struct` (defined in `<linux/sched.h>`) that contains the process’s list of virtual memory areas, page tables, and various other bits of memory management housekeeping information, along with a semaphore (`mmap_sem`) and a spinlock (`page_table_lock`). The pointer to this structure is found in the `task` structure; in the rare cases where a driver needs to access it, the usual way is to use `current->mm`. Note that the memory management structure can be shared between processes; the Linux implementation of threads works in this way, for example. 

上面是我们对Linux内存管理的数据结构的概述。有了这个，我们现在就可以继续学习`mmap`系统调用了。

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

X服务器的VMA的全部列表很长，但是大部分在这里也不需要。但是，我们可以看到`/dev/mem`有四个独立的映射，它们可以深入理解X-Server如何和视频卡一起配合使用。第一个映射是在0xa000，这是640KB ISA孔中视频RAM的标准位置。再往下看，可以在0xe8000000处看到一个更大的映射，这个地址高于系统上最高的RAM地址。这是适配器上的视频内存的直接映射。

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

在可行的情况下使用`mmap`有很多好处。比如，我们已经看过了X-server，其与视频内存有大量的数据传输；将图像显示映射到用户空间可以显著提高吞吐量，起到使用`lseek/write`所达不到的效果。另外的一个典型例子就是，控制PCI设备的程序。大部分的PCI设备映射控制寄存器到内存中，高性能的应用程序选择直接访问寄存器，而不是反复调用ioctl来完成工作。

`mmap`方法也是结构体`file_operations`的成员之一，当发起`mmap`系统调用的时候就会被调用。使用`mmap`，内核在调用实际的方法之前执行大量的工作，因此，该方法的原型与系统调用的原型完全不同。这与ioctl和`poll`之类的调用不同，在调用这些方法之前内核没有做太多工作。

系统调用声明如下（`mmap(2)`帮助手册里描述）：

    mmap(caddr_t addr, size_t len, int prot, int flags, int fd, off_t offset) 

其函数原型声明为：

    int (*mmap) (struct file *filp, struct vm_area_struct *vma); 

参数`filp`如第三章里介绍的一样，而`vma`包含用于访问设备的虚拟地址范围的信息。因此，大部分工作都是由内核完成的；为了实现`mmap`，驱动程序只需要为地址范围构建合适的页表，并在必要时，用一组新的操作替换`vma->vm_pos`。

有两种构建页表的方法：使用`remap_pfn_range`一次完成所有内存映射，或者使用`nopage VMA`方法一次执行一页。每种方法都有其优点和局限性。我们从`一次性`方法开始，这更简单。然后，我们再根据真实设备逐渐增加难度。

<h3 id="15.2.1">15.2.1 使用remap_pfn_range</h3>

构建新页表，将其映射到一段物理地址上，这些工作可以由函数`remap_pfn_range`和`io_remap_page_range`完成，它们具有下面的原型：

    int remap_pfn_range(struct vm_area_struct *vma,
                        unsigned long virt_addr, unsigned long pfn,
                        unsigned long size, pgprot_t prot);
    int io_remap_page_range(struct vm_area_struct *vma,
                        unsigned long virt_addr, unsigned long phys_addr,
                        unsigned long size, pgprot_t prot);

函数返回值是通常的0或负错误代码。 让我们看一下函数参数的确切含义：

1. vma
    
    虚拟内存区域，某段页范围映射到其上。

2. virt_addr
    
    重映射开始的用户虚拟地址。这个函数在`virt_addr`和`virt_addr+size`之间的虚拟地址范围内构建页表。

3. pfn
   
   虚拟地址映射到的物理地址上对应的页帧编号。页帧编号就是通过宏`PAGE_SHIFT`右移获得的。对于大多数用途，`VMA`结构的`vm_pgoff`包含你所需要的值。函数影响的物理地址从`pfn<<PAGE_SHIFT`到`(pfn<<PAGE_SHIFT)+size`。

4. size
    
    重映射的维度（以字节为单位）。

5. prot
    
    为新`VMA`请求保护，驱动程序可以（应该）使用`vma->vm_page_prot`中的值。 

`remap_pfn_range`的参数十分简单，当你的`mmap`方法被调用时，大多数参数已经在`VMA`中提供给你了。但是，为什么会有2个函数呢？第一个，`remap_pfn_range`设计的目的是用于在`pfn`引用实际的系统RAM的情况下，而`io_remap_page_range`应该用于`phys_addr`指向I/O内存时。实际上，除了SPARC架构之外，这两个函数在每个体系架构上都是相同的，大多数情况下都会使用`remap_pfn_range`。

另一个复杂因素和缓存有关：通常，处理器不应缓存对设备内存的引用。正常情况下，系统BIOS会正确设置，但也可以通过保护字段禁用特定的VMA的缓存。不幸的是，禁用缓存高度依赖于处理器的。好奇的读者可以通过`drivers/char/mem.c`中的`pgprot_noncached`函数来查看所涉及的内容。

<h3 id="15.2.2">15.2.2 一个简单的实现</h3>

如果您的驱动程序需要将设备内存简单，线性映射到用户地址空间，则`remap_pfn_range`几乎就是您想要的函数。 以下代码派生自`drivers/char/mem.c`，最终编译为一个名为`simple`的模块，并执行：

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

如您所见，重映射内存只需调用`remap_pfn_range`来创建必要的页表即可。

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

要想使上面的操作对具体的映射有效，就必须把结构体`simple_remap_vm_ops`赋值给相关`VMA`的`vm_ops`成员。这通常在`mmap`方法中完成。如果你返回到`simple_remap_mmap`例子中，你会看见下面这些行代码：

    vma->vm_ops = &simple_remap_vm_ops;
    simple_vma_open(vma);

Note the explicit call to simple_vma_open. Since the open method is not invoked on the initial mmap, we must call it explicitly if we want it to run.

<h3 id="15.2.4">15.2.4 使用nopage方法映射内存</h3>

Although remap_pfn_range works well for many, if not most, driver mmap implementations, sometimes it is necessary to be a little more flexible. In such situations, an implementation using the nopage VMA method may be called for. 

One situation in which the nopage approach is useful can be brought about by the mremap system call, which is used by applications to change the bounding addresses of a mapped region. As it happens, the kernel does not notify drivers directly when a mapped VMA is changed by mremap. If the VMA is reduced in size, the kernel can quietly flush out the unwanted pages without telling the driver. If, instead, the VMA is expanded, the driver eventually finds out by way of calls to nopage when mappings must be set up for the new pages, so there is no need to perform a separate notification. The nopage method, therefore, must be implemented if you want to support the mremap system call. Here, we show a simple implementation of nopage for the simple device. 

`nopage`方法的原型如下：

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


<h3 id="15.2.5">15.2.5 重新映射特定的I/O区域</h3>

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

<h3 id="15.2.6">15.2.6 重映射RAM</h3>

An interesting limitation of remap_pfn_range is that it gives access only to reserved pages and physical addresses above the top of physical memory. In Linux, a page of physical addresses is marked as “reserved” in the memory map to indicate that it is not available for memory management. On the PC, for example, the range between 640 KB and 1 MB is marked as reserved, as are the pages that host the kernel code itself. Reserved pages are locked in memory and are the only ones that can be safely mapped to user space; this limitation is a basic requirement for system stability.

Therefore, remap_pfn_range won’t allow you to remap conventional addresses, which include the ones you obtain by calling get_free_page. Instead, it maps in the zero page. Everything appears to work, with the exception that the process sees private, zero-filled pages rather than the remapped RAM that it was hoping for. Nonetheless, the function does everything that most hardware drivers need it to do, because it can remap high PCI buffers and ISA memory.

The limitations of remap_pfn_range can be seen by running mapper, one of the sample programs in misc-progs in the files provided on O’Reilly’s FTP site. mapper is a simple tool that can be used to quickly test the mmap system call; it maps read-only parts of a file specified by command-line options and dumps the mapped region to standard output. The following session, for instance, shows that /dev/mem doesn’t map the physical page located at address 64 KB—instead, we see a page full of zeros (the host computer in this example is a PC, but the result would be the same on other platforms):

    morgana.root# ./mapper /dev/mem 0x10000 0x1000 | od -Ax -t x1
    mapped "/dev/mem" from 65536 to 69632
    000000 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    *
    001000

The inability of remap_pfn_range to deal with RAM suggests that memory-based devices like scull can’t easily implement mmap, because its device memory is conventional RAM, not I/O memory. Fortunately, a relatively easy workaround is available to any driver that needs to map RAM into user space; it uses the nopage method that we have seen earlier.

<h4 id="15.2.6.1">15.2.6.1 使用nopage方法重新映射RAM</h4>

The way to map real RAM to user space is to use vm_ops->nopage to deal with page faults one at a time. A sample implementation is part of the scullp module, introduced in Chapter 8. 

scullp is a page-oriented char device. Because it is page oriented, it can implement mmap on its memory. The code implementing memory mapping uses some of the concepts introduced in the section “Memory Management in Linux.” 

Before examining the code, let’s look at the design choices that affect the mmap implementation in scullp: 

*  scullp doesn’t release device memory as long as the device is mapped. This is a matter of policy rather than a requirement, and it is different from the behavior of scull and similar devices, which are truncated to a length of 0 when opened for writing. Refusing to free a mapped scullp device allows a process to overwrite regions actively mapped by another process, so you can test and see how processes and device memory interact. To avoid releasing a mapped device, the driver must keep a count of active mappings; the vmas field in the device structure is used for this purpose. 

*  Memory mapping is performed only when the scullp order parameter (set at module load time) is 0. The parameter controls how __get_free_pages is invoked (see the section “get_free_page and Friends” in Chapter 8). The zero-order limitation (which forces pages to be allocated one at a time, rather than in larger groups) is dictated by the internals of __get_free_pages, the allocation function used by scullp. To maximize allocation performance, the Linux kernel maintains a list of free pages for each allocation order, and only the reference count of the first page in a cluster is incremented by get_free_pages and decremented by free_pages. The mmap method is disabled for a scullp device if the allocation order is greater than zero, because nopage deals with single pages rather than clusters of pages. scullp simply does not know how to properly manage reference counts for pages that are part of higher-order allocations. (Return to the section “A scull Using Whole Pages: scullp” in Chapter 8 if you need a refresher on scullp and the memory allocation order value.) 

The zero-order limitation is mostly intended to keep the code simple. It is possible to correctly implement mmap for multipage allocations by playing with the usage count of the pages, but it would only add to the complexity of the example without introducing any interesting information. 

Code that is intended to map RAM according to the rules just outlined needs to implement the open, close, and nopage VMA methods; it also needs to access the memory map to adjust the page usage counts. 

`scullp_mmap`的实现非常短，因为它依赖于`nopage`函数去完成所有有趣的工作：

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

<h3 id="15.2.7">15.2.7 重映射内核虚拟地址</h3>

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

    
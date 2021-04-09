[TOC]

之前，我们已经研究过Linux如何利用x86机器的段和页管理电路实现逻辑地址到物理地址的转换。我们也已经知道，内核代码和其使用地静态内核数据结构永久地占据了一部分RAM空间。

剩余的RAM，我们称之为动态内存，留给内核本身和进程使用。动态内存的管理直接影响了系统的整体性能。因此，现代操作系统都在想法设法地优化动态内存的使用，比如，需要的时候申请，不需要的时候释放。

接下来的文章，我们会根据两种不同的情况讲述内核如何分配内存。第一种就是物理连续地址；第二种是物理非连续地址。


过程中，会涉及一些概念：`memory zone`、`kernel mapping`、`buddy`系统、`slab cache`、和`内存池`等。

## 1 页帧管理

<img id="Figure_8-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_8_1.PNG">

以x86奔腾系列处理器为例，可以支持两种不同的页帧结构大小:`4KB`和`4MB`(如果使能物理内存扩展功能-PAE，可以支持2MB)。Linux选择了常规的4KB大小作为基本的内存申请单元。这样做有两个目的：

1. `page fault`异常很容易解释。产生该错误的原因基本就是两个：申请的页存在，但是进程无权访问；申请的页不存在。第二种情况下，内存分配器可以找一个自由的4K大小的页，分配给进程。

2. 尽管4K和4M都是磁盘块大小的整数倍。但是，磁盘和内存之间的数据，采用小块的方式更有效率。

## 2 页描述符

> 关于`page`和`page frame`的概念其实我们已经阐述过。通俗讲，就像酒店的所有客房一样，都可以类比页帧（page frame）；而住了客人的客房可以类比页（page）。

所以，如果页帧中已经有了数据，这些数据可能是内核代码，内核使用的数据结构，用户态进程的数据，用户态使用的数据结构等，就称为页。内核想要管理内存，就必须知道页帧的状态。

内核中，页帧的状态信息保存在一个类型为`page`的页描述符中，其结构如下表所示。所有的页描述符存储在`mem_map`数组中。每个描述符32位，所以`mem_map`所占内存空间不到总RAM的`1%`。`virt_to_page(addr)`宏可以产生线性地址`addr`对应的页描述符。`pfn_to_page(pfn)`可以计算出页帧`pfn`对应的页描述符。

| 类型 | 名称 | 描述 |
| ---- | ---- | ---- |
| unsigned long | flags     | 描述页帧状态的标志 | 
| atomic_t      | _count    | 页帧引用计数器 |
| atomic_t      | _mapcount | 引用页帧的页表项数量 |
| unsigned long | private   | 如果page是自由的，被buddy系统使用 |
| struct address_space *| mapping | 与page cache对应关系 |
| unsigned long | index     | 内核不同组件使用意义不同。|
| struct list_head | lru    | 指向最近最少使用的页双向链表中 |

上表中，有几个字段不同情况下会有不同的意义，具体需要考虑页帧的使用情况，或者被内核哪部分内容使用。我们先来研究一下这几个字段：

1. **_count**

    页的引用计数器。如果被设为-1，相应的页帧是自由未分配的，内核本身和进程都可以使用。如果该值大于等于0，则说明已经被内核本身或者进程使用，用来存储一些内核的数据结构。`page_count()`函数可以获取`_count`字段的值。

2. **flags**

    支持32个flag标志，具体参考下表。对于每一种标志，内核都有相应的宏可以操作这些值。通常，`PageXyz`宏读取`Page_xyz`字段值；而`SetPageXyz`宏设置`Page_xyz`字段值；而`ClearPageXyz`宏清零`Page_xyz`字段。

`flags`可能的取值：

| 名称 | 意义 |
| ---- | ---- |
| PG_locked     | 页被锁住，比如磁盘I/O操作的时候 |
| PG_error      | 读写页时发生I/O错误 |
| PG_referenced | 页最近被访问过 |
| PG_uptodate   | 完成read操作，设置该标志 |
| PG_dirty      | 页被修改 |
| PG_lru        | 表示该页位于激活还是非激活页列表中 |
| PG_active     | 该页在有效页列表中 |
| PG_slab       | 页帧属于slab |
| PG_highmem    | 页帧属于ZONE_HIGHMEM区 |
| PG_checked    | 某些文件系统会使用，比如EXT2和EXT3|
| PG_arch_1     | x86架构未使用 |
| PG_reserved   | 为内核保留的页帧未使用 |
| PG_private    | 页描述符的私有字段 |
| PG_writeback  | 正在写入磁盘的页 |
| PG_nosave     | 系统挂起/恢复时使用 |
| PG_compound   | 通过扩展分页机制处理的页帧 |
| PG_swapcache  | 属于swap cache的页 |
| PG_mappedtodisk| 页帧上所有数据对应磁盘上分配的块 |
| PG_reclaim    | 回收内存，标志已经写入磁盘 |
| PG_nosave_free| 系统挂起/恢复时使用 |

## 3 NUMA-非一致内存访问

通常情况下，我们认为内存是一致的，可共享的资源。具体来说，忽略掉Cache，CPU访问内存的时间响应是一样的。不幸的是，这种假设在某些CPU架构上是不成立的，比如，Alpha和MIPS多核处理器。

于是，有人提出了NUMA模型（非一致性内存访问模型）。将计算机的内存分为几个节点（node），节点内CPU访问内存的时间是一致的。某个CPU核想要访问节点外的内存，一般会经历更长的时间。Linux 2.6版本开始支持NUMA模型，通过将CPU经常访问的内核数据结构尽量放在同节点内的内存上，降低CPU访存的时间成本。

每个节点内的物理内存可以被划分为几个区。节点对应一个描述符，内核使用数据结构是`pg_data_t`表示，所有的节点描述符存储在一个单向链表中，`pgdat_list`变量指向其第一个元素。

表-节点描述符的各个字段含义

| 类型 | 名称 | 描述 |
| ---- | ---- | ---- |
| struct zone[]         | node_zones        | zone描述符的数组 |
| struct zonelist []    | node_zonelists    | zonelist数组，页分配器使用 |
| int                   | nr_zones          | 节点内zone数量 |
| struct page *         | node_mem_map      | 节点内页描述符数组 |
| struct bootmem_data * | bdata             | 内核初始化使用 |
| unsigned long         | node_start_pfn    | 节点内第一个页帧的索引 |
| unsigned long         | node_present_pages| 节点内内存大小，<br>排除空洞内存，<br>单位是页帧 |
| unsigned long         | node_spanned_pages| 节点内内存大小，<br>包含空洞内存 |
| int                   | node_id           | 节点标识符 |
| pg_data_t *           | pgdat_next        | 指向下一个节点 |
| wait_queue_head_t     | kswapd_wait       | kswapd的等待队列 |
| struct task_struct *  | kswapd            | 指向kswapd守护进程 |
| int                   | kswapd_max_order  | 由kswapd创建的空闲块的对数大小 |

对于UMA模型，Linux内核只使用节点0，其数据结构存储在`contig_page_data`变量中，内核还是通过`pgdat_list`指向这个变量。

之所以这样设计，是因为Linux内核要兼容所有的架构。

## 4 内存分区(Memory Zone)

一个理想的计算机架构，页帧作为一个的存储单元，可以存储任何东西：内核和用户数据，缓存磁盘数据等，没有任何限制。

但是，理想很丰满，现实很骨感。实际的计算机架构往往会限制页帧的使用。对于x86架构来说，Linux内核需要处理两个硬件限制：

* 用于旧ISA总线的直接内存访问（DMA）处理器有一个很大的限制：它们只能寻址前16MB的RAM。

* 现代32位计算机中，因为线性物理地址空间有限，即使具有大量的物理内存，CPU也无法直接访问。

为了解决这两个限制，Linux 2.6版本内核将所有的内存节点的物理内存分为三个区，英文称为`zone`。在x86 UMA架构中，有下面三个区：

* **ZONE_DMA**

    包含低16M的页帧。

* **ZONE_NORMAL**

    包含16M到896M的页帧。

* **ZONE_HIGHMEM**

    包含大于896M的页帧。

`ZONE_DMA`包含的页帧，基于旧ISA总线的设备通过DMA方式访问。

`ZONE_DMA`和`ZONE_NORMAL`包含的正常页帧，内核可以通过4G线性地址空间的线性映射进行访问。`ZONE_HIGHMEM`区的页帧，内核不能通过4G线性地址空间的线性映射直接访问。64位架构上，`ZONE_HIGHMEM`总是空的。

每个区都有自己的描述符，使用数据结构`zone`表示。


## 5 保留内存池

内存分配请求可能会面临两种情况：如果有足够的空闲内存可用，直接分配成功；否则，必须进行内存回收，此时申请内存分配的代码将会被阻塞，直到释放额外的内存。

但是，有些时候，申请内存分配的代码不能被阻塞。比如中断处理程序，或者一段临界代码区。此时，申请内存的代码应该发出原子内存分配请求（使用GFP_ATOMIC标志）。原子请求不会被阻塞：如果没有足够的空闲内存，则申请失败。

尽管没有绝对的办法保证原子内存分配请求成功，内核还是努力降低失败发生的可能性。所以，内核为原子内存分配请求保留了一个页帧池，仅在内存不足的时候使用。

## 6 ZONE内存分配器

Linux内核处理连续物理内存分配请求的子系统，我们称之为`Zone页帧分配器`。其架构如下图所示：

<img id="Figure_8-2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_8_2.PNG">

从图中可以看出，`ZONE内存分配器`负责接收处理动态内存的申请和释放。它会根据内存分配请求的类型，选择合适的ZONE区，满足内存分配请求。每一个内存ZONE内，都有一个`buddy`系统处理页帧。为了更好的整体性能，每个CPU都有一个页帧Cache，用于单个页帧的内存分配请求。

## 7 高端内存的内核映射

> 关于高端内存的概念，作用等请参考[【Linux】Linux的内核空间（低端内存、高端内存）](https://blog.csdn.net/qq_38410730/article/details/81105132)

高端内存的开始地址，也就是直接映射的线性地址的结束位置。该线性地址存储在`high_memory`变量中，这个值等于`896M`。超过896M的物理内存通常不会被内核的线性地址直接映射（1G地址空间大小），所以，内核无法访问它们。

假设，内核调用`__get_free_pages(GFP_HIGHMEM,0)`在高端内存中申请一个页帧。如果分配器在高端内存中分配了一个页帧，该函数也只能返回NULL，因为这样的线性地址不存在。反过来，内核也无法使用该页帧。更糟糕的是，该页帧还无法被释放，因为内核失去了对它的控制。

高端内存的问题只存在于32位架构，64位架构不存在，其线性地址远远大于实际的RAM数量。也就是说，64位架构上，`ZONE_HIGHMEM`区都是空的。但是，在32位系统上，比如x86架构，内核设计人员就必须保证能够访问所有的内存空间（最大64G，使能PAE的情况下）。

实现的方法是：

* 使用`alloc_pages()`和`alloc_page()`函数，返回的不是申请的高端内存的第一个页帧的线性地址，而是返回该页帧的页描述符的线性地址。这些页描述符一般位于低端内存，内核初始化后，这些值是固定的。

* 高端内存的页帧如果没有映射到内核的线性地址空间的话，内核是无法访问的。所以，内核预留了128M的内核线性地址空间专门映射高端内存。当然了，这种映射肯定是临时的，要不然也只能访问高端内存的128M空间。通过重复的映射128M线性地址空间，可以访问整个高端内存区域。

内核映射高端内存的方式有三种：

1. 永久内核映射

2. 临时内核映射

3. 非连续物理地址映射


#### 7.1 永久内核映射

永久内核映射允许内核为高端内存建立长期的内核地址空间映射关系。内核中，主内核页表中拥有一个专用的页表，用来管理这些永久内核映射关系。这个专用页表的地址存储在变量`pkmap_page_table`中。使用`LAST_PKMAP`可以得到页表数量。通常情况下，包含512项或1024项，取决于是否开启PAE功能。因此，内核一次最多可以访问2M或4M的高端内存。

映射的线性地址起始位置是`PKMAP_BASE`。`pkmap_count`数组保存`LAST_PKMAP`计数器、以及`pkmap_page_table`页表的每一项的引用计数器。我们来考虑三种情况：

1. 引用计数器为0

    表明该页表项没有映射任何高端内存，是可用的。

2. 引用计数器为1

    表明该页表项没有映射任何高端内存，但是相应的TLB项还没有被刷新。

3. 引用计数器为n（大于1）

    表明该页表项被映射到一个高端内存的页帧上，同时被n-1个内核组件使用。

为了记录高端内存的页帧和永久内核映射之间的映射关系，内核使用了一个`page_address_htable`哈希表。该表包含一个`page_address_map`数据结构，记录每一个映射的高端内存的页帧。而且，这个数据结构还包含指向该页帧对应的页描述符的指针和指派给该页帧的线性地址。

`page_address()`函数可以返回某个页帧的对应的线性地址，如果该页帧在高端内存中，但是没有被映射，则返回NULL。这个函数接收页描述符指针`page`作为其参数，

1. 如果对应的页帧不属于高端内存（标志PG_highmem清空），则线性地址总是存在，通过计算页帧的索引，将其转换为物理地址，最终得到与物理地址对应的线性地址。下面是可能的实现代码：

```c
    __va((unsigned long)(page - mem_map) << 12)

```

2. 如果页帧属于高端内存（标志PG_highmem被设置），则该函数遍历`page_address_htable`哈希表。如果在哈希表中发现该页帧，则返回对应的线性地址，否则返回NULL。

`kmap()`函数负责建立永久内核映射。实现如下：

```c
void * kmap(struct page * page)
{
    if (!PageHighMem(page))
        return page_address(page);
    return kmap_high(page);
}
```

`kmap_high()`函数处理页帧属于高端内存的情况。基本等于如下代码：

```c
void * kmap_high(struct page * page)
{
    unsigned long vaddr;
    spin_lock(&kmap_lock);
    vaddr = (unsigned long) page_address(page);
    if (!vaddr)
        vaddr = map_new_virtual(page);
    pkmap_count[(vaddr-PKMAP_BASE) >> PAGE_SHIFT]++;
    spin_unlock(&kmap_lock);
    return (void *) vaddr;
}
```

使用自旋锁`kmap_lock`是防止多核系统的并发情况。注意，这儿无需禁止中断，因为中断处理函数和可延时函数不会调用`kmap()`函数。接下俩，调用函数`page_address`判断该页帧是否已经被映射。如果没有，则调用`map_new_virtual`函数将该页帧的物理地址插入到`pkmap_page_table`表中，同时更新`page_address_htable`哈希表增加一个新元素。然后，增加对该线性地址的引用计数器，表示引用该线性地址的内核组件又多了一个。最后，释放自旋锁并返回映射的线性地址。

`map_new_virtual()`函数实际上执行了两个嵌套循环：

```c
for (;;) {
    int count;
    DECLARE_WAITQUEUE(wait, current);
    for (count = LAST_PKMAP; count > 0; --count) {
        last_pkmap_nr = (last_pkmap_nr + 1) & (LAST_PKMAP - 1);
        if (!last_pkmap_nr) {
            flush_all_zero_pkmaps();
            count = LAST_PKMAP;
        }
        if (!pkmap_count[last_pkmap_nr]) {
            unsigned long vaddr = PKMAP_BASE +
                                (last_pkmap_nr << PAGE_SHIFT);
            set_pte(&(pkmap_page_table[last_pkmap_nr]),
                    mk_pte(page, __pgprot(0x63)));
            pkmap_count[last_pkmap_nr] = 1;
            set_page_address(page, (void *) vaddr);
            return vaddr;
        }
    }
    current->state = TASK_UNINTERRUPTIBLE;
    add_wait_queue(&pkmap_map_wait, &wait);
    spin_unlock(&kmap_lock);
    schedule();
    remove_wait_queue(&pkmap_map_wait, &wait);
    spin_lock(&kmap_lock);
    if (page_address(page))
        return (unsigned long) page_address(page);
}  
```

内层的for循环，扫描`pkmap_count`数组中所有的计数器，直到找到一个null值。找到后，根据计数器的索引可以得到线性地址，并在`永久内核映射`表`pkmap_page_table`中创建一项，同时设置引用计数器为1，表示该页表项已经被使用；调用`set_page_address()`函数在`page_address_htable`哈希表中创建一个新元素，并返回线性地址。

该函数通过`last_pkmap_nr`记录上一次遍历`pkmap_count`数组的位置，也就是`pkmap_page_table`页表中被使用的最后一项。也就是说，每一次的搜索都是从最后一次调用`map_new_virtual()`函数留下的位置开始。

当`pkmap_count`数组的最后一个计数器时，重新从索引为0的计数器开始。但是，在继续之前，`map_new_virtual()`函数会调用`flush_all_zero_pkmaps()`函数，遍历所有值为1的计数器。计数器的值为1，表示`pkmap_page_table`表中的项是空闲的，但是相应的TLB项还没有被刷新。`flush_all_zero_pkmaps()`函数会将所有的这类计数器复位为0，从`page_address_htable`哈希表中删除掉，并对`pkmap_page_table`表中所有项发出TLB刷新请求。

如果在`pkmap_count`数组中没有发现空计数器，`map_new_virtual()`函数会阻塞当前进程，直到其它进程释放`pkmap_page_table`页表中的某一项为止。实现方法就是，将当前进程插入到`pkmap_map_wait`等待队列中，并将当前进程的状态设置为`TASK_UNINTERRUPTIBLE`，然后调用`schedule()`放弃CPU的使用权。一旦该进程重新被唤醒，该函数调用`page_address()`检查该page是否被其它进程映射过；如果没有其它进程映射该page，那么再次启动一次遍历`pkmap_count`数组的循环。

`kunmap()`函数负责销毁由`kmap()`函数创建的永久内核映射。如果参数`page`确实指向高端内存区域，继续调用`kunmap_high()`函数，释放该内存，代码类似于下面的代码：

```c
void kunmap_high(struct page * page)
{
    spin_lock(&kmap_lock);
    if ((--pkmap_count[((unsigned long)page_address(page)-PKMAP_BASE)>>PAGE_SHIFT]) == 1)
        if (waitqueue_active(&pkmap_map_wait))
            wake_up(&pkmap_map_wait);
    spin_unlock(&kmap_lock);
}
```

该函数实现的功能是，根据page所指向的线性地址计算出指向`pkmap_count`数组的索引。由该索引所指向的数组元素，也就是对应的计数器减1，并与1进行比较。如果相等，则说明没有进程使用该`page`。则唤醒由`map_new_virtual()`函数加入等待队列的进程，如果有的话。

#### 7.2 临时内核映射

临时内核映射要比永久内核映射简单，更重要的是，它可以应用在中断处理程序或延时函数中，因为它不会阻塞当前进程。

Linux内核在内核地址空间中保留了非常小的一个窗口，专门用来映射高端内存的某个页帧。

每个CPU都有一组窗口（13个），使用`enum km_type`枚举类型表示。该枚举结构中的每个符号，比如`KM_BOUNCE_READ`、`KM_USER0`或`km_pte0`，都表示对应的窗口的线性地址。

Linux内核必须保证每个窗口不能被内核中的两段代码同时使用。所以，`km_type`枚举中的每个元素代表的窗口，都是某个内核组件专用的，其命名也是根据组件名称命名的。枚举中的最后一个元素`KM_TYPE_NR`，表示每个CPU可用的窗口数量。

`km_type`中每个元素，除了最后一个表示窗口数量之外，都是某个固定线性地址的索引。枚举类型的`fixed_addresses`数据结构，包含符号`FIX_KMAP_BEGIN`和`FIX_KMAP_END`，后者等于`FIX_KMAP_BEGIN + (KM_TYPE_NR * NR_CPUS)-1`的索引。系统中，每个CPU具有`KM_TYPE_NR`固定映射的线性地址。内核使用`fix_to_virt(FIX_KMAP_BEGIN)`对应的页表项的地址初始化`kmap_pte`变量。

为了建立临时内核映射，需要调用函数`kmap_atomic()`，其代码大概如下所示：

```c
void * kmap_atomic(struct page * page, enum km_type type)
{
    enum fixed_addresses idx;
    unsigned long vaddr;
    current_thread_info()->preempt_count++;
    if (!PageHighMem(page))
        return page_address(page);
    idx = type + KM_TYPE_NR * smp_processor_id();
    vaddr = fix_to_virt(FIX_KMAP_BEGIN + idx);
    set_pte(kmap_pte-idx, mk_pte(page, 0x063));
    __flush_tlb_single(vaddr);
    return (void *) vaddr;
}
```

参数`type`，结合CPU的标识符（可以通过`smp_processor_id()`函数获得），就能够指定，请求的`page`页应该映射到哪个固定的线性地址上。如果该`page`不属于高端内存，则返回该页帧的线性地址；否则，它将为该页帧物理地址对应的线性地址建立页表项，并设置`Present`、`Accessed`、`Read/Write`和`Dirty`内存标志位。最后，刷新正确的TLB项，并返回该线性地址。

对应的，销毁临时内核映射关系，使用函数`kunmap_atomic()`。在80x86架构中，该函数对当前进程的`preempt_count`成员进行减1操作。如果发起请求临时内核映射的内核代码之前是可抢占的，那么在销毁了临时内核映射之后，它将再次成为可抢占式的代码。更进一步，`kunmap_atomic()`函数会检查当前进程的`TIF_NEED_RESCHED`标志，如果被设置，则调用`schedule()`函数，进行重新调度。


## 8 Buddy系统算法

## 9 每个CPU的页帧Cache

## 10 Zone分配器




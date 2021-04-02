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

高端内存的开始地址，也就是直接映射的线性地址的结束位置。该线性地址存储在`high_memory`变量中，这个值等于`896M`。

## 8 Buddy系统算法

## 9 每个CPU的页帧Cache

## 10 Zone分配器





之前，我们已经研究过Linux如何利用x86机器的段和页管理电路实现逻辑地址到物理地址的转换。我们也已经知道，内核代码和其使用地静态内核数据结构永久地占据了一部分RAM空间。

剩余的RAM，我们称之为动态内存，留给内核本身和进程使用。动态内存的管理直接影响了系统的整体性能。因此，现代操作系统都在想法设法地优化动态内存的使用，比如，需要的时候申请，不需要的时候释放。

接下来的文章，我们会根据两种不同的情况讲述内核如何分配内存。第一种就是物理连续地址；第二种是物理非连续地址。


过程中，会涉及一些概念：`memory zone`、`kernel mapping`、`buddy`系统、`slab cache`、和`内存池`等。

## 页帧管理

<img id="Figure_8-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_8_1.PNG">

以x86奔腾系列处理器为例，可以支持两种不同的页帧结构大小:`4KB`和`4MB`(如果使能物理内存扩展功能-PAE，可以支持2MB)。Linux选择了常规的4KB大小作为基本的内存申请单元。这样做有两个目的：

1. `page fault`异常很容易解释。产生该错误的原因基本就是两个：申请的页存在，但是进程无权访问；申请的页不存在。第二种情况下，内存分配器可以找一个自由的4K大小的页，分配给进程。

2. 尽管4K和4M都是磁盘块大小的整数倍。但是，磁盘和内存之间的数据，采用小块的方式更有效率。

## 页描述符

> 关于`page`和`page frame`的概念其实我们已经阐述过。通俗讲，就像酒店的所有客房一样，都可以类比页帧（page frame）；而住了客人的客房可以类比页（page）。

所以，如果页帧中已经有了数据，这些数据可能是内核代码，内核使用的数据结构，用户态进程的数据，用户态使用的数据结构等，就称为页。内核想要管理内存，就必须知道页帧的状态。

内核中，页帧的状态信息保存在一个类型为`page`的页描述符中，其结构如下表所示：

| 类型 | 名称 | 描述 |
| ---- | ---- | ---- |
| unsigned long | flags     | | 
| atomic_t      | _count    | 页帧引用计数器 |
| atomic_t      | _mapcount | 引用页帧的页表项数量 |
| unsigned long | private   | 如果page是自由的，被buddy系统使用 |
| struct address_space *| mapping | 与page cache对应关系 |
| unsigned long | index     | 内核不同组件使用意义不同。|
| struct list_head | lru    | 指向最近最少使用的页双向链表中 |

`flags`可能的取值：

| 名称 | 意义 |
| ---- | ---- |
| PG_locked     | 页被锁住，比如磁盘I/O操作的时候 |
| PG_error      | 读写页时发生I/O错误 |
| PG_referenced | 页最近被访问过 |
| PG_uptodate   | 完成read操作，设置该标志 |
| PG_dirty      | 页被修改 |
| PG_lru        |
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



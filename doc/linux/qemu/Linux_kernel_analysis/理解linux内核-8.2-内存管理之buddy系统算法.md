[TOC]

众所周知，内存管理存在一个重要的难题-`内存碎片化`：频繁地请求和释放不同大小的连续物理内存，就会导致在已分配的物理内存块之间存在一些无法使用的小块物理内存。很多时候，明明还有许多空闲的内存，却总是申请不成功。

应对这种内存碎片化问题，有两种方向：

* Use the paging circuitry to map groups of noncontiguous free page frames into intervals of contiguous linear addresses.

* Develop a suitable technique to keep track of the existing blocks of free contiguous page frames, avoiding as much as possible the need to split up a large free block to satisfy a request for a smaller one.

The second approach is preferred by the kernel for three good reasons:

* In some cases, contiguous page frames are really necessary, because contiguous linear addresses are not sufficient to satisfy the request. A typical example is a memory request for buffers to be assigned to a DMA processor (see Chapter 13). Because most DMAs ignore the paging circuitry and access the address bus directly while transferring several disk sectors in a single I/O operation, the buffers requested must be located in contiguous page frames.

* Even if contiguous page frame allocation is not strictly necessary, it offers the big advantage of leaving the kernel paging tables unchanged. What’s wrong with modifying the Page Tables? As we know from Chapter 2, frequent Page Table modifications lead to higher average memory access times, because they make the CPU flush the contents of the translation lookaside buffers.

* Large chunks of contiguous physical memory can be accessed by the kernel through 4 MB pages. This reduces the translation lookaside buffers misses, thus significantly speeding up the average memory access time (see the section “Translation Lookaside Buffers (TLB)” in Chapter 2).

Linux内核解决`外部碎片化`的问题是采用著名的伙伴系统算法（buddy system）。具体的做法就是，将所有的空闲页帧分为11个链表，每个链表的元素大小分别为1、2、4、8、16、32、64、128、256、512和1024个连续的页帧。这样的话，伙伴系统最大可以请求的连续物理地址就是4M，对应1024个连续页帧。如下图所示。链表中每个内存块元素的第一个页帧的物理地址是内存块大小的整数倍。比如，16个页帧的内存块的初始地址就是16*2^12的整数倍（2^12=4K，常规页大小）。

<img id="Figure_8-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_8_3.PNG">

我们举一个例子，阐述伙伴系统的工作原理：

假设请求1MB大小的连续物理内存（256个连续页帧）。伙伴系统首先检查第8个链表（元素大小为256个连续页帧）中是否存在空闲块。如果没有这样的块，伙伴系统会遍历更大的块链表，第9个链表（元素大小为512个连续页帧）。如果这样的块存在，则将512个连续页帧块的256个页帧分配，剩余的256个页帧插入到第8个链表中。如果没有空闲的512页帧块，则继续寻找更大的块（1024页帧块，第10个链表）。如果这样的快存在，则分配1024个页帧中的256个页帧满足请求，将剩余的768个页帧中的头512个页帧插入第9个链表（元素大小为512个连续页帧）中，再将最后的256个页帧插入第8个链表中（元素大小为256个连续页帧）。如果连第10个链表（元素大小为1024个连续页帧）也为空，则算法终止，并发出错误信号。

释放过程则相反。内核尝试将大小相同，而物理地址又连续的内存块合并为一个内存块
The reverse operation, releasing blocks of page frames, gives rise to the name of this algorithm. The kernel attempts to merge pairs of free buddy blocks of size b together into a single block of size 2b. Two blocks are considered buddies if:

* Both blocks have the same size, say b.
* They are located in contiguous physical addresses.
* The physical address of the first page frame of the first block is a multiple of 2 × b × 2^12.

The algorithm is iterative; if it succeeds in merging released blocks, it doubles b and tries again so as to create even bigger blocks.


## 数据结构

Linux 2.6 uses a different buddy system for each zone. Thus, in the 80 × 86 architecture, there are 3 buddy systems: the first handles the page frames suitable for ISA DMA, the second handles the “normal” page frames, and the third handles the highmemory page frames. Each buddy system relies on the following main data structures:

* The mem_map array introduced previously. Actually, each zone is concerned with a
subset of the mem_map elements. The first element in the subset and its number of
elements are specified, respectively, by the zone_mem_map and size fields of the
zone descriptor.

* An array consisting of eleven elements of type free_area, one element for each group size. The array is stored in the free_area field of the zone descriptor.

Let us consider the kth element of the free_area array in the zone descriptor, which
identifies all the free blocks of size 2k. The free_list field of this element is the head
of a doubly linked circular list that collects the page descriptors associated with the
free blocks of 2k pages. More precisely, this list includes the page descriptors of the
starting page frame of every block of 2k free page frames; the pointers to the adjacent
elements in the list are stored in the lru field of the page descriptor.*

Besides the head of the list, the kth element of the free_area array includes also the
field nr_free, which specifies the number of free blocks of size 2k pages. Of course, if
there are no blocks of 2k free page frames, nr_free is equal to 0 and the free_list list
is empty (both pointers of free_list point to the free_list field itself).

Finally, the private field of the descriptor of the first page in a block of 2k free pages
stores the order of the block, that is, the number k. Thanks to this field, when a
block of pages is freed, the kernel can determine whether the buddy of the block is
also free and, if so, it can coalesce the two blocks in a single block of 2k+1 pages. It
should be noted that up to Linux 2.6.10, the kernel used 10 arrays of flags to encode
this information.

## 申请一个block

The __rmqueue() function is used to find a free block in a zone. The function takes
two arguments: the address of the zone descriptor, and order, which denotes the
logarithm of the size of the requested block of free pages (0 for a one-page block, 1
for a two-page block, and so forth). If the page frames are successfully allocated, the
__rmqueue() function returns the address of the page descriptor of the first allocated
page frame. Otherwise, the function returns NULL.

The _ _rmqueue() function assumes that the caller has already disabled local interrupts
and acquired the zone->lock spin lock, which protects the data structures of
the buddy system. It performs a cyclic search through each list for an available block
(denoted by an entry that doesn’t point to the entry itself), starting with the list for
the requested order and continuing if necessary to larger orders:

```c
struct free_area *area;
unsigned int current_order;
for (current_order=order; current_order<11; ++current_order) {
    area = zone->free_area + current_order;
    if (!list_empty(&area->free_list))
        goto block_found;
}
return NULL;
```

If the loop terminates, no suitable free block has been found, so _ _rmqueue( ) returns
a NULL value. Otherwise, a suitable free block has been found; in this case, the
descriptor of its first page frame is removed from the list and the value of free_pages
in the zone descriptor is decreased:

```c
block_found:
    page = list_entry(area->free_list.next, struct page, lru);
    list_del(&page->lru);
    ClearPagePrivate(page);
    page->private = 0;
    area->nr_free--;
    zone->free_pages -= 1UL << order;
```

If the block found comes from a list of size curr_order greater than the requested size
order, a while cycle is executed. The rationale behind these lines of codes is as follows:
when it becomes necessary to use a block of 2k page frames to satisfy a request
for 2h page frames (h < k), the program allocates the first 2h page frames and iteratively
reassigns the last 2k – 2h page frames to the free_area lists that have indexes
between h and k:

```c
size = 1 << curr_order;
while (curr_order > order) {
    area--;
    curr_order--;
    size >>= 1;
    buddy = page + size;
    /* insert buddy as first element in the list */
    list_add(&buddy->lru, &area->free_list);
    area->nr_free++;
    buddy->private = curr_order;
    SetPagePrivate(buddy);
}
return page;
```

Because the __rmqueue() function has found a suitable free block, it returns the address page of the page descriptor associated with the first allocated page frame.

## 释放一个block





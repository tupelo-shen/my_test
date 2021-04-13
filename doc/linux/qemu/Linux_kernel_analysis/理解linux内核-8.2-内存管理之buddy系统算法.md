[TOC]

## 1 伙伴系统介绍

众所周知，内存管理存在一个重要的难题-`内存碎片化`：频繁地请求和释放不同大小的连续物理内存，就会导致在已分配的物理内存块之间存在一些无法使用的小块物理内存。很多时候，明明还有许多空闲的内存，却总是申请不成功。

Linux内核解决`外部碎片化`的问题是采用著名的伙伴系统算法（buddy system）。具体的做法就是，将所有的空闲页帧分为11个链表，每个链表的元素大小分别为1、2、4、8、16、32、64、128、256、512和1024个连续的页帧。这样的话，伙伴系统最大可以请求的连续物理地址就是4M，对应1024个连续页帧。如下图所示。链表中每个内存块元素的第一个页帧的物理地址是内存块大小的整数倍。比如，16个页帧的内存块的初始地址就是16*2^12的整数倍（2^12=4K，常规页大小）。

<img id="Figure_8-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_8_3.PNG">

我们举一个例子，阐述伙伴系统的工作原理：

假设请求1MB大小的连续物理内存（256个连续页帧）。伙伴系统首先检查第8个链表（元素大小为256个连续页帧）中是否存在空闲块。如果没有这样的块，伙伴系统会遍历更大的块链表，第9个链表（元素大小为512个连续页帧）。如果这样的块存在，则将512个连续页帧块的256个页帧分配，剩余的256个页帧插入到第8个链表中。如果没有空闲的512页帧块，则继续寻找更大的块（1024页帧块，第10个链表）。如果这样的快存在，则分配1024个页帧中的256个页帧满足请求，将剩余的768个页帧中的头512个页帧插入第9个链表（元素大小为512个连续页帧）中，再将最后的256个页帧插入第8个链表中（元素大小为256个连续页帧）。如果连第10个链表（元素大小为1024个连续页帧）也为空，则算法终止，并发出错误信号。

释放过程则相反。内核尝试将大小相同，而物理地址又连续的内存块合并为一个内存块。假设有2个空闲内存块，它们的大小相同，都是`b`。内核将它们合并成为一个2b大小的内存块。

所以，如果两个内存块是伙伴关系，则应该具备如下条件：

* 具有相同大小；
* 位于连续的物理地址上；
* 第一个内存块的第一个页帧的物理地址是`2*b*2^12`的倍数。

这是一个不断迭代的过程。如果合并后的内存块，还存在着相同大小的伙伴内存块，则继续合并为更大的内存块。

## 2 数据结构

Linux 2.6内核为每一个ZONE区建立一个buddy伙伴系统。因而，80x86架构下，一共有3个buddy系统：分别对应`ISA DMA`页帧，正常的页帧，高端内存页帧。每个buddy系统依赖于以下主要数据结构：

* `mem_map`数组，记录所有的页描述符。事实上，每个ZONE区都与`mem_map`数组的一个子集有关。每个ZONE区的描述符中的成员`zone_mem_map`和`size`成员分别指定其所使用的第一个页描述符和描述符的总数。

* 一个数组，用于记录伙伴系统算法使用的11个链表和每个链表的元素个数。该数组存储在每个ZONE描述符的`free_area`成员中。

让我们考虑一种情况，假设ZONE描述符中的`free_area`数组的第k个链表，其链表中每个元素是大小为2^K的空闲内存块。链表的`free_list`成员指向一个双向链表的头，该双向链表记录着大小为2^K的空间内存块。更准确地说，该双向链表包含每个空闲内存块的起始页帧的页描述符；指向双向链表前后相邻元素的指针存储在页描述符的`lru`成员中。

`free_area`数组中，除了指向内存块的链表头之外，还包含该链表中包含的内存块（大小为2^K）个数，使用`nr_free`表示。如果某个链表中没有可用的空闲内存块，`nr_free`等于0，也就是说，`free_lish`链表头指向自身。

最后，解释一下每个内存块中第一个页描述符的`private`成员，存储着内存块，也就是链表的序号，K。通过`private`成员变量，释放某个内存块时，可以方便确定该块的伙伴关系内存块，以便于决定是否合并两个伙伴内存块为更大的内存块（大小为`2^(k+1)`）。

> <font>
> 另外，需要特殊说明的是：Linux 2.6.10之前，内核使用10个链表管理伙伴系统的信息。也就是最大可以申请2M的连续物理内存。
> </font>

## 3 申请一个内存块

在某个ZONE区中，申请一个内存块使用`__rmqueue()`函数。该函数需要两个参数：指定ZONE的描述符地址和指定内存块大小的对数，order（比如，order等于0，表示申请一页的内存块，1表示申请2页的内存块，以此类推）。申请成功，`__rmqueue()`函数返回申请的第一个页帧的页描述符地址；否则，该函数返回`NULL`。

使用`__rmqueue()`函数需要注意，它假定调用者已经关闭了局部中断并获取了`zone->lock`自旋锁，这将会保护伙伴系统的数据结构不受破坏。剩下的工作就很简单了，就是遍历11个链表，找到可用的空闲内存。如果在order指定的链表中，没有找到合适的内存块，则向更大块内存的链表中查找，代码如下所示：

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

从上面的代码可以看出：如果循环结束，没有合适的空闲块，`__rmqueue()`函数返回`NULL`值。如果找到合适的空闲块，则跳转到`block_found`分支，代码如下所示。此时，将第一个页帧的描述符从对应的链表中删除，zone描述符中记录空闲页帧总数的`free_pages`减少申请的页帧数量：

```c
block_found:
    page = list_entry(area->free_list.next, struct page, lru);
    list_del(&page->lru);
    ClearPagePrivate(page);
    page->private = 0;
    area->nr_free--;
    zone->free_pages -= 1UL << order;
```

如果是从更大块的链表中申请到的内存块，也就是说`curr_order`大于请求的`order`，执行一个while循环。该循环实现的内容是：如果必须使用更大块的内存块（大小为2^k）满足比它小的内存块（大小为2^h）请求（`h < k`），程序会分配第一个`2^h`大小的内存块，对于剩下的`2^k - 2^h`个页帧，则通过迭代插入到`free_area`数组中索引在`h-k`之间的链表中。

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

至此，整个申请过程完成。`__rmqueue()`函数返回申请的第一个页帧关联的页描述符的地址`page`。

## 4 释放一个内存块

从伙伴系统中释放内存则调用函数`__free_pages_bulk()`，下面是其3个基本的输入参数：

1. page

    要释放的内存块中包含的第一个页帧的描述符地址；

2. zone

    zone描述符的地址；

3. order

    块的对数大小；

同申请内存的`__rmqueue()`函数一样，假设调用者已经禁止局部中断并获取了`zone->lock`自旋锁。`__free_pages_bulk()`函数首先声明并初始化一些局部变量：

```c
struct page     *base = zone->zone_mem_map;
unsigned long   buddy_idx, page_idx = page - base;
struct page     *buddy, *coalesced;
int             order_size = 1 << order;
```

`page_idx`变量记录要释放的内存块的第一个页帧到zone中的第一个页帧的索引。`order_size`变量记录本次释放的页帧数量，最后该变量的值会被加到zone区中总的空闲页帧的计数器中。

```c
zone->free_pages += order_size;
```

该循环最多执行10次，检查是否该块有需要合并的伙伴关系块。该函数从指定的块大小开始，一直到最大的内存块大小结束，代码如下：

```c
while (order < 10) {
    buddy_idx = page_idx ^ (1 << order);
    buddy = base + buddy_idx;
    if (!page_is_buddy(buddy, order))
        break;
    list_del(&buddy->lru);
    zone->free_area[order].nr_free--;
    ClearPagePrivate(buddy);
    buddy->private = 0;
    page_idx &= buddy_idx;
    order++;
}
```

在循环内部，该函数会查找索引为`buddy_idx`的内存块，它和具有页描述符索引为`page_idx`的内存块为伙伴关系。计算方式为：

```c
buddy_idx = page_idx ^ (1 << order);
```

在这儿，通过和`1 << order`进行异或操作，`page_idx`的第`order`位会被改变。如果先前该位等于0，则`buddy_idx`等于`page_idx + order_size`；相反，如果先前该位等于1，则`buddy_idx`等于`page_idx - order_size`。通过这种运算，始终能找到`page_idx`内存块对应的伙伴关系块。

一旦伙伴关系块被找到，伙伴关系块的页描述符就很容易计算得到：

```c
buddy = base + buddy_idx;
```

现在，调用`page_is_buddy()`函数，检查buddy是否满足空闲伙伴关系块的要求，代码如下：

```c
int page_is_buddy(struct page *page, int order)
{
    if (PagePrivate(buddy) && page->private == order &&
            !PageReserved(buddy) && page_count(page) ==0)
        return 1;
    return 0;
}
```

如你所见，伙伴关系块的第一个页帧必须是空闲的（`__count`等于`-1`），必须属于动态内存（`PG_reserved`标志位清空），它的`private`属性必须被设置（`PG_private`标志位设置），最后，必须保证该块的`private`设置为将要释放的内存块所在的链表序号。

如果所有的条件满足，伙伴关系块被释放，并将其从序号为`order`的空闲块链表中移除。通过迭代过程，不断判断是否有2倍大的伙伴关系块。

如果`page_is_buddy()`函数中某个条件不满足，将会退出循环，因为将要释放的空闲内存块不会进一步与其它内存块进行合并。将获取的空闲内存块插入恰当的伙伴关系链表中，同时，更新该空闲内存块的第一页帧的`private`成员为该块对应的链表序号。

```c
coalesced = base + page_idx;
coalesced->private = order;
SetPagePrivate(coalesced);
list_add(&coalesced->lru, &zone->free_area[order].free_list);
zone->free_area[order].nr_free++;
```

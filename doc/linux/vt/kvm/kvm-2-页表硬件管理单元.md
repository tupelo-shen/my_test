原文地址：[https://wiki.osdev.org/Paging](https://wiki.osdev.org/Paging)

Paging is a system which allows each process to see a full virtual address space, without actually requiring the full amount of physical memory to be available or present. 32-bit x86 processors support 32-bit virtual addresses and 4-GiB virtual address spaces, and current 64-bit processors support 48-bit virtual addressing and 256-TiB virtual address spaces. Intel has released documentation for a extension to 57-bit virtual addressing and 128-PiB virtual address spaces. Currently, implementations of x86-64 have a limit of between 4 GiB and 256 TiB of physical address space (and an architectural limit of 4 PiB of physical address space).

In addition to this, paging introduces the benefit of page-level protection. In this system, user processes can only see and modify data which is paged in on their own address space, providing hardware-based isolation. System pages are also protected from user processes. On the x86-64 architecture, page-level protection now completely supersedes Segmentation as the memory protection mechanism. On the IA-32 architecture, both paging and segmentation exist, but segmentation is now considered 'legacy'.

Once an Operating System has paging, it can also make use of other benefits and workarounds, such as linear framebuffer simulation for memory-mapped IO and paging out to disk, where disk storage space is used to free up physical RAM.

[TOC]

## 1 MMU

Paging is achieved through the use of the Memory Management Unit (MMU). On the x86, the MMU maps memory through a series of tables, two to be exact. They are the paging directory (PD), and the paging table (PT).

Both tables contain 1024 4-byte entries, making them 4 KiB each. In the page directory, each entry points to a page table. In the page table, each entry points to a physical address that is then mapped to the virtual address found by calculating the offset within the directory and the offset within the table. This can be done as the entire table system represents a linear 4-GiB virtual memory map.

#### 1.1 页目录

The topmost paging structure is the page directory. It is essentially an array of page directory entries that take the following form.

The page table address field represents the physical address of the page table that manages the four megabytes at that point. Please note that it is very important that this address be 4-KiB aligned. This is needed, due to the fact that the last 12 bits of the 32-bit value are overwritten by access bits and such.

* S, or 'Page Size' stores the page size for that specific entry. If the bit is set, then pages are 4 MiB in size. Otherwise, they are 4 KiB. Please note that 4-MiB pages require PSE to be enabled.

* A, or 'Accessed' is used to discover whether a page has been read or written to. If it has, then the bit is set, otherwise, it is not. Note that, this bit will not be cleared by the CPU, so that burden falls on the OS (if it needs this bit at all).

* D, is the 'Cache Disable' bit. If the bit is set, the page will not be cached. Otherwise, it will be.

* W, the controls 'Write-Through' abilities of the page. If the bit is set, write-through caching is enabled. If not, then write-back is enabled instead.

* U, the 'User/Supervisor' bit, controls access to the page based on privilege level. If the bit is set, then the page may be accessed by all; if the bit is not set, however, only the supervisor can access it. For a page directory entry, the user bit controls access to all the pages referenced by the page directory entry. Therefore if you wish to make a page a user page, you must set the user bit in the relevant page directory entry as well as the page table entry.

* R, the 'Read/Write' permissions flag. If the bit is set, the page is read/write. Otherwise when it is not set, the page is read-only. The WP bit in CR0 determines if this is only applied to userland, always giving the kernel write access (the default) or both userland and the kernel (see Intel Manuals 3A 2-20).

* P, or 'Present'. If the bit is set, the page is actually in physical memory at the moment. For example, when a page is swapped out, it is not in physical memory and therefore not 'Present'. If a page is called, but not present, a page fault will occur, and the OS should handle it. (See below.)

The remaining bits 9 through 11 are not used by the processor, and are free for the OS to store some of its own accounting information. In addition, when P is not set, the processor ignores the rest of the entry and you can use all remaining 31 bits for extra information, like recording where the page has ended up in swap space.

Setting the S bit makes the page directory entry point directly to a 4-MiB page. There is no paging table involved in the address translation. Note: With 4-MiB pages, bits 21 through 12 are reserved! Thus, the physical address must also be 4-MiB-aligned.

#### 1.2 页表


## 2 使能

## 3 物理地址扩展

## 4 使用

#### 4.1 虚拟地址空间

#### 4.2 虚拟内存

## 5 manipulation

## 6 页错误

## 7 INVLPG

## 8 分页技巧


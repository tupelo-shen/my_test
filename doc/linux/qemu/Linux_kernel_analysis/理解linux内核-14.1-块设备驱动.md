[TOC]

## 1 简介

This chapter deals with I/O drivers for block devices, i.e., for disks of every kind. The key aspect of a block device is the disparity between the time taken by the CPU and buses to read or write data and the speed of the disk hardware. Block devices have very high average access times. Each operation requires several milliseconds to complete, mainly because the disk controller must move the heads on the disk surface to reach the exact position where the data is recorded. However, when the heads are correctly placed, data transfer can be sustained at rates of tens of megabytes per second.

The organization of Linux block device handlers is quite involved. We won’t be able to discuss in detail all the functions that are included in the block I/O subsystem of the kernel; however, we’ll outline the general software architecture. As in the previous chapter, our objective is to explain how Linux supports the implementation of block device drivers, rather than showing how to implement one of them.

We start in the first section “Block Devices Handling” to explain the general architecture of the Linux block I/O subsystem. In the sections “The Generic Block Layer,” “The I/O Scheduler,” and “Block Device Drivers,” we will describe the main components
of the block I/O subsystem. Finally, in the last section, “Opening a Block
Device File,” we will outline the steps performed by the kernel when opening a block
device file.

## 2 块设备处理

Each operation on a block device driver involves a large number of kernel components;
the most important ones are shown in Figure 14-1.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_14_1.PNG">

Let us suppose, for instance, that a process issued a read() system call on some disk
file—we’ll see that write requests are handled essentially in the same way. Here is
what the kernel typically does to service the process request:

1. The service routine of the read() system call activates a suitable VFS function,
passing to it a file descriptor and an offset inside the file. The Virtual Filesystem is the upper layer of the block device handling architecture, and it provides a
common file model adopted by all filesystems supported by Linux. We have
described at length the VFS layer in Chapter 12.

2. The VFS function determines if the requested data is already available and, if
necessary, how to perform the read operation. Sometimes there is no need to
access the data on disk, because the kernel keeps in RAM the data most recently
read from—or written to—a block device. The disk cache mechanism is
explained in Chapter 15, while details on how the VFS handles the disk operations
and how it interfaces with the disk cache and the filesystems are given in
Chapter 16.

3. Let’s assume that the kernel must read the data from the block device, thus it
must determine the physical location of that data. To do this, the kernel relies on
the mapping layer, which typically executes two steps:

    * It determines the block size of the filesystem including the file and computes
the extent of the requested data in terms of file block numbers. Essentially,
the file is seen as split in many blocks, and the kernel determines the
numbers (indices relative to the beginning of file) of the blocks containing
the requested data.

    * Next, the mapping layer invokes a filesystem-specific function that accesses
the file’s disk inode and determines the position of the requested data on
disk in terms of logical block numbers. Essentially, the disk is seen as split in blocks, and the kernel determines the numbers (indices relative to the beginning
of the disk or partition) corresponding to the blocks storing the
requested data. Because a file may be stored in nonadjacent blocks on disk,
a data structure stored in the disk inode maps each file block number to a
logical block number.*

    We will see the mapping layer in action in Chapter 16, while we will present some typical disk-based filesystems in Chapter 18.

4. The kernel can now issue the read operation on the block device. It makes use of
the generic block layer, which starts the I/O operations that transfer the
requested data. In general, each I/O operation involves a group of blocks that are
adjacent on disk. Because the requested data is not necessarily adjacent on disk,
the generic block layer might start several I/O operations. Each I/O operation is
represented by a “block I/O” (in short, “bio”) structure, which collects all information
needed by the lower components to satisfy the request.

    The generic block layer hides the peculiarities of each hardware block device,
thus offering an abstract view of the block devices. Because almost all block
devices are disks, the generic block layer also provides some general data structures
that describe “disks” and “disk partitions.” We will discuss the generic
block layer and the bio structure in the section “The Generic Block Layer” later
in this chapter.

5. Below the generic block layer, the “I/O scheduler” sorts the pending I/O data
transfer requests according to predefined kernel policies. The purpose of the
scheduler is to group requests of data that lie near each other on the physical
medium. We will describe this component in the section “The I/O Scheduler”
later in this chapter.

6. Finally, the block device drivers take care of the actual data transfer by sending
suitable commands to the hardware interfaces of the disk controllers. We will
explain the overall organization of a generic block device driver in the section
“Block Device Drivers” later in this chapter.

As you can see, there are many kernel components that are concerned with data
stored in block devices; each of them manages the disk data using chunks of different
length:

* The controllers of the hardware block devices transfer data in chunks of fixed
length called “sectors.” Therefore, the I/O scheduler and the block device drivers
must manage sectors of data.

* The Virtual Filesystem, the mapping layer, and the filesystems group the disk
data in logical units called “blocks.” A block corresponds to the minimal disk
storage unit inside a filesystem.

* As we will see shortly, block device drivers should be able to cope with “segments”
of data: each segment is a memory page—or a portion of a memory
page—including chunks of data that are physically adjacent on disk.

* The disk caches work on “pages” of disk data, each of which fits in a page frame.

* The generic block layer glues together all the upper and lower components, thus
it knows about sectors, blocks, segments, and pages of data.

Even if there are many different chunks of data, they usually share the same physical
RAM cells. For instance, Figure 14-2 shows the layout of a 4,096-byte page. The
upper kernel components see the page as composed of four block buffers of 1,024
bytes each. The last three blocks of the page are being transferred by the block device
driver, thus they are inserted in a segment covering the last 3,072 bytes of the page.
The hard disk controller considers the segment as composed of six 512-byte sectors.
In this chapter we describe the lower kernel components that handle the block
devices—generic block layer, I/O scheduler, and block device drivers—thus we focus
our attention on sectors, blocks, and segments.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_14_2.PNG">

## 3 扇区-sector

To achieve acceptable performance, hard disks and similar devices transfer several
adjacent bytes at once. Each data transfer operation for a block device acts on a
group of adjacent bytes called a sector. In the following discussion, we say that

groups of bytes are adjacent when they are recorded on the disk surface in such a
manner that a single seek operation can access them. Although the physical geometry
of a disk is usually very complicated, the hard disk controller accepts commands
that refer to the disk as a large array of sectors.

In most disk devices, the size of a sector is 512 bytes, although there are devices that
use larger sectors (1,024 and 2,048 bytes). Notice that the sector should be considered
as the basic unit of data transfer; it is never possible to transfer less than one
sector, although most disk devices are capable of transferring several adjacent sectors
at once.

In Linux, the size of a sector is conventionally set to 512 bytes; if a block device uses
larger sectors, the corresponding low-level block device driver will do the necessary
conversions. Thus, a group of data stored in a block device is identified on disk by its
position—the index of the first 512-byte sector—and its length as number of 512-
byte sectors. Sector indices are stored in 32- or 64-bit variables of type sector_t.

## 4 块-block

While the sector is the basic unit of data transfer for the hardware devices, the block
is the basic unit of data transfer for the VFS and, consequently, for the filesystems.
For example, when the kernel accesses the contents of a file, it must first read from
disk a block containing the disk inode of the file (see the section “Inode Objects” in
Chapter 12). This block on disk corresponds to one or more adjacent sectors, which
are looked at by the VFS as a single data unit.
In Linux, the block size must be a power of 2 and cannot be larger than a page frame.
Moreover, it must be a multiple of the sector size, because each block must include
an integral number of sectors. Therefore, on 80 × 86 architecture, the permitted
block sizes are 512, 1,024, 2,048, and 4,096 bytes.
The block size is not specific to a block device. When creating a disk-based filesystem,
the administrator may select the proper block size. Thus, several partitions on
the same disk might make use of different block sizes. Furthermore, each read or
write operation issued on a block device file is a “raw” access that bypasses the diskbased
filesystem; the kernel executes it by using blocks of largest size (4,096 bytes).
Each block requires its own block buffer, which is a RAM memory area used by the
kernel to store the block’s content. When the kernel reads a block from disk, it fills
the corresponding block buffer with the values obtained from the hardware device;
similarly, when the kernel writes a block on disk, it updates the corresponding group
of adjacent bytes on the hardware device with the actual values of the associated block
buffer. The size of a block buffer always matches the size of the corresponding block.
Each buffer has a “buffer head” descriptor of type buffer_head. This descriptor contains
all the information needed by the kernel to know how to handle the buffer; thus,
before operating on each buffer, the kernel checks its buffer head. We will give a
detailed explanation of all fields of the buffer head in Chapter 15; in the present chapter,
however, we will only consider a few fields: b_page, b_data, b_blocknr, and b_bdev.
The b_page field stores the page descriptor address of the page frame that includes the
block buffer. If the page frame is in high memory, the b_data field stores the offset of the
block buffer inside the page; otherwise, it stores the starting linear address of the block
buffer itself. The b_blocknr field stores the logical block number (i.e., the index of the
block inside the disk partition). Finally, the b_bdev field identifies the block device that
is using the buffer head (see the section “Block Devices” later in this chapter).

## 5 段-segment

We know that each disk I/O operation consists of transferring the contents of some
adjacent sectors from—or to—some RAM locations. In almost all cases, the data
transfer is directly performed by the disk controller with a DMA operation (see the
section “Direct Memory Access (DMA)” in Chapter 13). The block device driver simply
triggers the data transfer by sending suitable commands to the disk controller;
once the data transfer is finished, the controller raises an interrupt to notify the block
device driver.
The data transferred by a single DMA operation must belong to sectors that are adjacent
on disk. This is a physical constraint: a disk controller that allows DMA transfers
to non-adjacent sectors would have a poor transfer rate, because moving a read/write
head on the disk surface is quite a slow operation.
Older disk controllers support “simple” DMA operations only: in each such operation,
data is transferred from or to memory cells that are physically contiguous in
RAM. Recent disk controllers, however, may also support the so-called scatter-gather
DMA transfers: in each such operation, the data can be transferred from or to several
noncontiguous memory areas.
For each scatter-gather DMA transfer, the block device driver must send to the disk
controller:

* The initial disk sector number and the total number of sectors to be transferred

* A list of descriptors of memory areas, each of which consists of an address and a
length.

The disk controller takes care of the whole data transfer; for instance, in a read operation
the controller fetches the data from the adjacent disk sectors and scatters it into
the various memory areas.
To make use of scatter-gather DMA operations, block device drivers must handle the
data in units called segments. A segment is simply a memory page—or a portion of a
memory page—that includes the data of some adjacent disk sectors. Thus, a scattergather
DMA operation may involve several segments at once.

Notice that a block device driver does not need to know about blocks, block sizes,
and block buffers. Thus, even if a segment is seen by the higher levels as a page composed
of several block buffers, the block device driver does not care about it.
As we’ll see, the generic block layer can merge different segments if the corresponding
page frames happen to be contiguous in RAM and the corresponding chunks of
disk data are adjacent on disk. The larger memory area resulting from this merge
operation is called physical segment.
Yet another merge operation is allowed on architectures that handle the mapping
between bus addresses and physical addresses through a dedicated bus circuitry (the
IO-MMU; see the section “Direct Memory Access (DMA)” in Chapter 13). The
memory area resulting from this kind of merge operation is called hardware segment.
Because we will focus on the 80 × 86 architecture, which has no such dynamic mapping
between bus addresses and physical addresses, we will assume in the rest of this
chapter that hardware segments always coincide with physical segments.



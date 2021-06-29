[TOC]

One of Linux’s keys to success is its ability to coexist comfortably with other systems. You can transparently mount disks or partitions that host file formats used by Windows, other Unix systems, or even systems with tiny market shares like the Amiga. Linux manages to support multiple filesystem types in the same way other Unix variants do, through a concept called the Virtual Filesystem.

The idea behind the Virtual Filesystem is to put a wide range of information in the kernel to represent many different types of filesystems; there is a field or function to support each operation provided by all real filesystems supported by Linux. For each read, write, or other function called, the kernel substitutes the actual function that supports a native Linux filesystem, the NTFS filesystem, or whatever other filesystem the file is on.

This chapter discusses the aims, structure, and implementation of Linux’s Virtual Filesystem. It focuses on three of the five standard Unix file types—namely, regular files, directories, and symbolic links. Device files are covered in Chapter 13, while pipes are discussed in Chapter 19. To show how a real filesystem works, Chapter 18 covers the Second Extended Filesystem that appears on nearly all Linux systems.

## 1 虚拟文件系统的作用

The Virtual Filesystem (also known as Virtual Filesystem Switch or VFS) is a kernel software layer that handles all system calls related to a standard Unix filesystem. Its main strength is providing a common interface to several kinds of filesystems.

For instance, let’s assume that a user issues the shell command:

```shell
$ cp /floppy/TEST /tmp/test
```

where /floppy is the mount point of an MS-DOS diskette and /tmp is a normal Second Extended Filesystem (Ext2) directory. The VFS is an abstraction layer between the application program and the filesystem implementations (see Figure 12-1(a)). Therefore, the cp program is not required to know the filesystem types of /floppy/TEST and /tmp/test. Instead, cp interacts with the VFS by means of generic system calls known to anyone who has done Unix programming (see the section “File-Handling System Calls” in Chapter 1); the code executed by cp is shown in Figure 12-1(b).

<img id="Figure_12-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_12_1.PNG">

Figure 12-1. VFS role in a simple file copy operation

VFS支持的文件系统可以分为三类：

* 磁盘文件系统

    管理磁盘或模拟磁盘的设备，比如常见的硬盘或USB闪存。VFS支持的一些基于磁盘的著名文件系统有：

    - Linux支持的文件系统，比如`Ext2`、`Ext3`。

    - 类UNIX操作系统支持的文件系统，比如`sysv`文件系统（`System V`、`Coherent`、`Xenix`）、`UFS`文件系统（`BSD`、`Solaris`、`NEXTSTEP`）、`MINIX`文件系统和`VERITAS VxFS`（`SCO UnixWare`）。
    
    - 微软系文件系统，比如`MS-DOS`、`VFAT`（Win95及其后版本支持）和`NTFS`（Windows NT及其后版本支持）。
    
    - ISO9660光盘文件系统(以前的高塞拉文件系统)和通用磁盘格式(UDF) DVD文件系统。
    
    - 其它专有文件系统，如IBM的OS/2 (HPFS)、苹果的Macintosh (HFS)、Amiga的快速文件系统(AFFS)和Acorn磁盘文件系统(ADFS)。
    
    - 其它类型的日志文件系统，比如`IBM`的`JFS`、`SGI`的`XFS`。

* 网络文件系统

    可以方便访问其它网络计算机的文件系统中的文件。比如，`NFS`文件系统。

* 特殊文件系统

    `/proc`文件系统就是一个典型的例子。

鉴于篇幅，我们只详细讲解`ext2`和`ext3`文件系统。

我们知道，类Unix文件系统都是以`/`目录作为`root`节点

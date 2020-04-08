<h1 id="0">0 目录</h1>

* [12 虚拟文件系统](#12)
    - [12.1 虚拟文件系统的作用](#12.1)
        + [12.1.1 通用文件模型](#12.1.1)
        + [12.1.2 VFS实现的系统调用](#12.1.2)
    - [12.2 VFS数据结构](#12.2)
        + [12.2.1 Superblock对象](#12.2.1)
        + [12.2.2 Inode对象](#12.2.2)
        + [12.2.3 File对象](#12.2.3)
        + [12.2.4 dentry对象](#12.2.4)
        + [12.2.5 与进程关联的文件](#12.2.5)
    - [12.3 文件系统类型](#12.3)
        + [12.3.1 特殊的文件系统](#12.3.1)
        + [12.3.2 注册文件系统类型](#12.3.2)
    - [12.4 文件系统处理](#12.4)
        + [12.4.1 命名空间](#12.4.1)
        + [12.4.2 文件系统挂载](#12.4.2)
        + [12.4.3 挂载一个通用文件系统](#12.4.3)
        + [12.4.4 挂载根文件系统](#12.4.4)
        + [12.4.5 卸载文件系统](#12.4.5)
    - [12.5 路径查找](#12.5)
        + [12.5.1 标准路径查找](#12.5.1)
        + [12.5.2 父路径查找](#12.5.2)
        + [12.5.3 符号链接查找](#12.5.3)
    - [12.6 VFS系统调用的实现](#12.6)
        + [12.6.1 open()实现](#12.6.1)
        + [12.6.2 read()实现](#12.6.2)
        + [12.6.3 close()实现](#12.6.3)
    - [12.7 文件保护](#12.7)
        + [12.7.1 Linux文件锁保护](#12.7.1)
        + [12.7.2 文件锁数据结构](#12.7.2)
        + [12.7.3 FL_FLOCK锁](#12.7.3)
        + [12.7.4 FL_POSIX锁](#12.7.4)

---

One of Linux’s keys to success is its ability to coexist comfortably with other systems. You can transparently mount disks or partitions that host file formats used by Windows, other Unix systems, or even systems with tiny market shares like the Amiga. Linux manages to support multiple filesystem types in the same way other Unix variants do, through a concept called the Virtual Filesystem.

The idea behind the Virtual Filesystem is to put a wide range of information in the kernel to represent many different types of filesystems; there is a field or function to support each operation provided by all real filesystems supported by Linux. For each read, write, or other function called, the kernel substitutes the actual function that supports a native Linux filesystem, the NTFS filesystem, or whatever other filesystem the file is on.

This chapter discusses the aims, structure, and implementation of Linux’s Virtual Filesystem. It focuses on three of the five standard Unix file types—namely, regular files, directories, and symbolic links. Device files are covered in Chapter 13, while pipes are discussed in Chapter 19. To show how a real filesystem works, Chapter 18 covers the Second Extended Filesystem that appears on nearly all Linux systems.

<h2 id="12.1">12.1 虚拟文件系统的作用</h2>

The Virtual Filesystem (also known as Virtual Filesystem Switch or VFS) is a kernel software layer that handles all system calls related to a standard Unix filesystem. Its main strength is providing a common interface to several kinds of filesystems.

For instance, let’s assume that a user issues the shell command:

    $ cp /floppy/TEST /tmp/test

where /floppy is the mount point of an MS-DOS diskette and /tmp is a normal Second Extended Filesystem (Ext2) directory. The VFS is an abstraction layer between the application program and the filesystem implementations (see Figure 12-1(a)). Therefore, the cp program is not required to know the filesystem types of /floppy/TEST and /tmp/test. Instead, cp interacts with the VFS by means of generic system calls known to anyone who has done Unix programming (see the section “File-Handling System Calls” in Chapter 1); the code executed by cp is shown in Figure 12-1(b).

<img id="Figure_12-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_12_1.PNG">

Figure 12-1. VFS role in a simple file copy operation

Filesystems supported by the VFS may be grouped into three main classes:

* 磁盘文件系统

    These manage memory space available in a local disk or in some other device that emulates a disk (such as a USB flash drive). Some of the well-known disk-based filesystems supported by the VFS are:

    * Filesystems for Linux such as the widely used Second Extended Filesystem (Ext2), the recent Third Extended Filesystem (Ext3), and the Reiser Filesystems (ReiserFS)

    * Filesystems for Unix variants such as sysv filesystem (System V, Coherent, Xenix), UFS (BSD, Solaris, NEXTSTEP), MINIX filesystem, and VERITAS VxFS (SCO UnixWare)

    * Microsoft filesystems such as MS-DOS, VFAT (Windows 95 and later releases), and NTFS (Windows NT 4 and later releases)

    * ISO9660 CD-ROM filesystem (formerly High Sierra Filesystem) and Universal Disk Format (UDF) DVD filesystem

    * Other proprietary filesystems such as IBM’s OS/2 (HPFS), Apple’s Macintosh (HFS), Amiga’s Fast Filesystem (AFFS), and Acorn Disk Filing System(ADFS)

    * Additional journaling filesystems originating in systems other than Linux such as IBM’s JFS and SGI’s XFS

* 网络文件系统

    These allow easy access to files included in filesystems belonging to other networked computers. Some well-known network filesystems supported by the VFS are NFS, Coda, AFS (Andrew filesystem), CIFS (Common Internet File System, used in Microsoft Windows), and NCP (Novell’s NetWare Core Protocol).

* 特殊文件系统

    These do not manage disk space, either locally or remotely. The /proc filesystem is a typical example of a special filesystem (see the later section “Special Filesystems”).

In this book, we describe in detail the Ext2 and Ext3 filesystems only (see Chapter 18); the other filesystems are not covered for lack of space.

As mentioned in the section “An Overview of the Unix Filesystem” in Chapter 1, Unix directories build a tree whose root is the / directory. The root directory is contained in the root filesystem, which in Linux, is usually of type Ext2 or Ext3. All other filesystems can be “mounted” on subdirectories of the root filesystem.*

A disk-based filesystem is usually stored in a hardware block device such as a hard disk, a floppy, or a CD-ROM. A useful feature of Linux’s VFS allows it to handle virtual block devices such as /dev/loop0, which may be used to mount filesystems stored in regular files. As a possible application, a user may protect her own private filesystem by storing an encrypted version of it in a regular file.

The first Virtual Filesystem was included in Sun Microsystems’s SunOS in 1986. Since then, most Unix filesystems include a VFS. Linux’s VFS, however, supports the widest range of filesystems.

<h3 id="12.1.1">12.1.1 通用文件模型</h3>

The key idea behind the VFS consists of introducing a common file model capable of representing all supported filesystems. This model strictly mirrors the file model provided by the traditional Unix filesystem. This is not surprising, because Linux wants to run its native filesystem with minimum overhead. However, each specific filesystem implementation must translate its physical organization into the VFS’s common file model.

For instance, in the common file model, each directory is regarded as a file, which contains a list of files and other directories. However, several non-Unix disk-based filesystems use a File Allocation Table (FAT), which stores the position of each file in the directory tree. In these filesystems, directories are not files. To stick to the VFS’s common file model, the Linux implementations of such FAT-based filesystems must be able to construct on the fly, when needed, the files corresponding to the directories. Such files exist only as objects in kernel memory.

More essentially, the Linux kernel cannot hardcode a particular function to handle an operation such as read( ) or ioctl( ). Instead, it must use a pointer for each operation; the pointer is made to point to the proper function for the particular filesystem being accessed.

Let’s illustrate this concept by showing how the read( ) shown in Figure 12-1 would be translated by the kernel into a call specific to the MS-DOS filesystem. The application’s call to read( ) makes the kernel invoke the corresponding sys_read( ) service routine, like every other system call. The file is represented by a file data structure in kernel memory, as we’ll see later in this chapter. This data structure contains a field called f_op that contains pointers to functions specific to MS-DOS files, including a function that reads a file. sys_read( ) finds the pointer to this function and invokes it. Thus, the application’s read( ) is turned into the rather indirect call:

    file->f_op->read(...);

Similarly, the write( ) operation triggers the execution of a proper Ext2 write function associated with the output file. In short, the kernel is responsible for assigning the right set of pointers to the file variable associated with each open file, and then for invoking the call specific to each filesystem that the f_op field points to.

One can think of the common file model as object-oriented, where an object is a software construct that defines both a data structure and the methods that operate on it. For reasons of efficiency, Linux is not coded in an object-oriented language such as C++. Objects are therefore implemented as plain C data structures with some fields pointing to functions that correspond to the object’s methods.

通用文件模型由以下对象类型组成：

* superblock对象

    存储实际挂载的文件系统的信息。对于磁盘文件系统，这个对象通常是存储在磁盘上的文件系统控制块。

* inode对象

    存储具体文件的常规信息。对于磁盘文件系统，这个对象通常是存储在磁盘上的文件控制块。每个inode对象都有一个编号，唯一地标识文件系统中的文件。

* file对象

    保存进程与其关联文件的交互需要的信息。这个文件对象只有在进程打开文件期间存在于内核内存中。也就是说，这是一个动态分配的对象，当文件关闭后，file对象就会被销毁。

* dentry对象

    存储目录（一个特定的文件）和文件的链接关系。至于这些链接信息如何存储，依赖于具体的磁盘文件系统。这也是体现各个文件系统差异的关键。

Figure 12-2 illustrates with a simple example how processes interact with files. Three different processes have opened the same file, two of them using the same hard link. In this case, each of the three processes uses its own file object, while only two dentry objects are required—one for each hard link. Both dentry objects refer to the same inode object, which identifies the superblock object and, together with the latter, the common disk file.

<img id="Figure_12-2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_12_2.PNG">

Figure 12-2. Interaction between processes and VFS objects

Besides providing a common interface to all filesystem implementations, the VFS has another important role related to system performance. The most recently used dentry objects are contained in a disk cache named the dentry cache, which speeds up the translation from a file pathname to the inode of the last pathname component.

Generally speaking, a disk cache is a software mechanism that allows the kernel to keep in RAM some information that is normally stored on a disk, so that further accesses to that data can be quickly satisfied without a slow access to the disk itself.

Notice how a disk cache differs from a hardware cache or a memory cache, neither of which has anything to do with disks or other devices. A hardware cache is a fast static RAM that speeds up requests directed to the slower dynamic RAM (see the section “Hardware Cache” in Chapter 2). A memory cache is a software mechanism introduced to bypass the Kernel Memory Allocator (see the section “The Slab Allocator” in Chapter 8).

Beside the dentry cache and the inode cache, Linux uses other disk caches. The most important one, called the page cache, is described in detail in Chapter 15.

<h3 id="12.1.2">12.1.2 VFS实现的系统调用</h3>

Table 12-1 illustrates the VFS system calls that refer to filesystems, regular files, directories, and symbolic links. A few other system calls handled by the VFS, such as ioperm( ), ioctl( ), pipe( ), and mknod( ), refer to device files and pipes. These are discussed in later chapters. A last group of system calls handled by the VFS, such as socket( ), connect( ), and bind( ), refer to sockets and are used to implement networking. Some of the kernel service routines that correspond to the system calls listed in Table 12-1 are discussed either in this chapter or in Chapter 18.

Table 12-1. Some system calls handled by the VFS

| 系统调用 | 描述 |
| -------- | ----- |
| mount( ) <br> umount( ) <br> umount2() | 挂载、卸载文件系统 |
| sysfs( ) | 得到文件系统的信息 |
| statfs( ) <br>fstatfs( ) <br> statfs64() <br> fstatfs64() <br> ustat( ) | 获取文件系统状态信息 |
| chroot( ) <br> pivot_root() | 改变root目录 |
| chdir( ) <br> fchdir( ) <br> getcwd( ) | 操作当前目录 |
| mkdir( ) <br> rmdir( ) | 创建和销毁目录 |
| getdents( ) <br> getdents64() <br> readdir( ) <br> link( ) <br> unlink( ) <br> rename( ) <br> lookup_dcookie() | 操作目录项 |
| readlink( ) <br> symlink( ) | 操作软链接 |
| chown( ) <br> fchown( ) <br> lchown( ) <br>chown16()<br>fchown16()<br>lchown16() | 修改文件拥有者 |
| chmod( )<br>fchmod( )<br>utime( ) | 修改文件属性 |
| stat( )<br>fstat( )<br>lstat( )<br>access( )<br>oldstat()<br>oldfstat()<br>oldlstat()<br>stat64()<br>lstat64()<br>fstat64() | 读文件状态 |
| open( )<br>close( )<br>creat( )<br>umask( ) | 打开、关闭和创建文件 |
| dup( )<br>dup2( )<br>fcntl()<br>fcntl64() | 操作文件描述符 |
| select( )<br>poll( ) | 等待一组文件描述符发生的事件 |
| truncate( )<br>ftruncate( )<br>truncate64()<br>ftruncate64() | 更变文件大小 |
| lseek( )<br>_llseek( ) | 改变文件的位置 |
| read( )<br>write( )<br>readv( )<br>writev( )<br>sendfile( )<br>sendfile64()<br>readahead() | 执行文件I/O操作 |
| io_setup()<br>io_submit()<br>io_getevents()<br>io_cancel()<br>io_destroy() | 异步I/O |
| pread64()<br>pwrite64() | 锁定文件内当前位置并访问 |
| mmap( )<br>mmap2()<br>munmap( )<br>madvise()<br>mincore()<br>remap_file_pages() | 处理文件映射 |
| fdatasync( )<br>fsync( )<br>sync( )<br>msync( ) | 同步文件数据 |
| flock( ) | 操作文件锁 |
| setxattr()<br>lsetxattr()<br>fsetxattr()<br>getxattr()<br>lgetxattr()<br>fgetxattr()<br>listxattr()<br>llistxattr()<br>flistxattr()<br>removexattr()<br>lremovexattr()<br>fremovexattr() | 操作文件扩展属性 |

We said earlier that the VFS is a layer between application programs and specific filesystems.
However, in some cases, a file operation can be performed by the VFS itself,
without invoking a lower-level procedure. For instance, when a process closes an
open file, the file on disk doesn’t usually need to be touched, and hence the VFS simply
releases the corresponding file object. Similarly, when the lseek( ) system call
modifies a file pointer, which is an attribute related to the interaction between an
opened file and a process, the VFS needs to modify only the corresponding file object
without accessing the file on disk, and therefore it does not have to invoke a specific
filesystem procedure. In some sense, the VFS could be considered a “generic” filesystem
that relies, when necessary, on specific ones.

<h2 id="12.2">12.2 VFS数据结构</h2>

Each VFS object is stored in a suitable data structure, which includes both the object
attributes and a pointer to a table of object methods. The kernel may dynamically
modify the methods of the object and, hence, it may install specialized behavior for
the object. The following sections explain the VFS objects and their interrelationships
in detail.

<h3 id="12.2.1">12.2.1 Superblock对象</h3>

A superblock object consists of a super_block structure whose fields are described in
Table 12-2.

Table 12-2. The fields of the superblock object

All superblock objects are linked in a circular doubly linked list. The first element of
this list is represented by the super_blocks variable, while the s_list field of the
superblock object stores the pointers to the adjacent elements in the list. The sb_lock
spin lock protects the list against concurrent accesses in multiprocessor systems.

The s_fs_info field points to superblock information that belongs to a specific filesystem;
for instance, as we’ll see later in Chapter 18, if the superblock object refers to
an Ext2 filesystem, the field points to an ext2_sb_info structure, which includes the
disk allocation bit masks and other data of no concern to the VFS common file
model.

In general, data pointed to by the s_fs_info field is information from the disk duplicated
in memory for reasons of efficiency. Each disk-based filesystem needs to access
and update its allocation bitmaps in order to allocate or release disk blocks. The VFS
allows these filesystems to act directly on the s_fs_info field of the superblock in
memory without accessing the disk.

This approach leads to a new problem, however: the VFS superblock might end up
no longer synchronized with the corresponding superblock on disk. It is thus necessary
to introduce an s_dirt flag, which specifies whether the superblock is dirty—
that is, whether the data on the disk must be updated. The lack of synchronization
leads to the familiar problem of a corrupted filesystem when a site’s power goes
down without giving the user the chance to shut down a system cleanly. As we’ll see
in the section “Writing Dirty Pages to Disk” in Chapter 15, Linux minimizes this
problem by periodically copying all dirty superblocks to disk.

The methods associated with a superblock are called superblock operations. They are
described by the super_operations structure whose address is included in the s_op field.

Each specific filesystem can define its own superblock operations. When the VFS
needs to invoke one of them, say read_inode( ), it executes the following:

    sb->s_op->read_inode(inode);

where sb stores the address of the superblock object involved. The read_inode field
of the super_operations table contains the address of the suitable function, which is
therefore directly invoked.

Let’s briefly describe the superblock operations, which implement higher-level operations
like deleting files or mounting disks. They are listed in the order they appear
in the super_operations table:

* alloc_inode(sb)

    Allocates space for an inode object, including the space required for filesystemspecific data.

* destroy_inode(inode)

    Destroys an inode object, including the filesystem-specific data.

* read_inode(inode)

    Fills the fields of the inode object passed as the parameter with the data on disk; the i_ino field of the inode object identifies the specific filesystem inode on the disk to be read.

* dirty_inode(inode)

    Invoked when the inode is marked as modified (dirty). Used by filesystems such as ReiserFS and Ext3 to update the filesystem journal on disk.

* write_inode(inode, flag)

    Updates a filesystem inode with the contents of the inode object passed as the parameter; the i_ino field of the inode object identifies the filesystem inode on disk that is concerned. The flag parameter indicates whether the I/O operation should be synchronous.

* put_inode(inode)

    Invoked when the inode is released—its reference counter is decreased—to perform filesystem-specific operations.

* drop_inode(inode)

    Invoked when the inode is about to be destroyed—that is, when the last user releases the inode; filesystems that implement this method usually make use of generic_drop_inode(). This function removes every reference to the inode from the VFS data structures and, if the inode no longer appears in any directory, invokes the delete_inode superblock method to delete the inode from the filesystem.

* delete_inode(inode)

    Invoked when the inode must be destroyed. Deletes the VFS inode in memory and the file data and metadata on disk.

* put_super(super)

    Releases the superblock object passed as the parameter (because the corresponding filesystem is unmounted).

* write_super(super)

    Updates a filesystem superblock with the contents of the object indicated.

* sync_fs(sb, wait)

    Invoked when flushing the filesystem to update filesystem-specific data structures on disk (used by journaling filesystems).

* write_super_lockfs(super)

    Blocks changes to the filesystem and updates the superblock with the contents of the object indicated. This method is invoked when the filesystem is frozen, for instance by the Logical Volume Manager (LVM) driver.

* unlockfs(super)

    Undoes the block of filesystem updates achieved by the write_super_lockfs superblock method.

* statfs(super, buf)

    Returns statistics on a filesystem by filling the buf buffer.

* remount_fs(super, flags, data)

    Remounts the filesystem with new options (invoked when a mount option must be changed).

* clear_inode(inode)

    Invoked when a disk inode is being destroyed to perform filesystem-specific operations.

* umount_begin(super)

    Aborts a mount operation because the corresponding unmount operation has been started (used only by network filesystems).

* show_options(seq_file, vfsmount)

    Used to display the filesystem-specific options

* quota_read(super, type, data, size, offset)

    Used by the quota system to read data from the file that specifies the limits for this filesystem.*

* quota_write(super, type, data, size, offset)

    Used by the quota system to write data into the file that specifies the limits for this filesystem.

The preceding methods are available to all possible filesystem types. However, only a subset of them applies to each specific filesystem; the fields corresponding to unimplemented methods are set to NULL. Notice that no get_super method to read a superblock is defined—how could the kernel invoke a method of an object yet to be read from disk? We’ll find an equivalent get_sb method in another object describing the filesystem type (see the later section “Filesystem Type Registration”).

<h3 id="12.2.2">12.2.2 Inode对象</h3>

All information needed by the filesystem to handle a file is included in a data structure called an inode. A filename is a casually assigned label that can be changed, but the inode is unique to the file and remains the same as long as the file exists. An inode object in memory consists of an inode structure whose fields are described in Table 12-3.

Each inode object duplicates some of the data included in the disk inode—for instance, the number of blocks allocated to the file. When the value of the i_state field is equal to I_DIRTY_SYNC, I_DIRTY_DATASYNC, or I_DIRTY_PAGES, the inode is dirty—that is, the corresponding disk inode must be updated. The I_DIRTY macro can be used to check the value of these three flags at once (see later for details). Other values of the i_state field are I_LOCK (the inode object is involved in an I/O transfer), I_FREEING (the inode object is being freed), I_CLEAR (the inode object contents are no longer meaningful), and I_NEW (the inode object has been allocated but not yet filled with data read from the disk inode).

Each inode object always appears in one of the following circular doubly linked lists (in all cases, the pointers to the adjacent elements are stored in the i_list field):

* The list of valid unused inodes, typically those mirroring valid disk inodes and not
currently used by any process. These inodes are not dirty and their i_count field is
set to 0. The first and last elements of this list are referenced by the next and prev
fields, respectively, of the inode_unused variable. This list acts as a disk cache.

* The list of in-use inodes, that is, those mirroring valid disk inodes and used by
some process. These inodes are not dirty and their i_count field is positive. The
first and last elements are referenced by the inode_in_use variable.

* The list of dirty inodes. The first and last elements are referenced by the s_dirty
field of the corresponding superblock object.

Each of the lists just mentioned links the i_list fields of the proper inode objects.

Moreover, each inode object is also included in a per-filesystem doubly linked circular
list headed at the s_inodes field of the superblock object; the i_sb_list field of
the inode object stores the pointers for the adjacent elements in this list.

Finally, the inode objects are also included in a hash table named inode_hashtable.
The hash table speeds up the search of the inode object when the kernel knows both
the inode number and the address of the superblock object corresponding to the filesystem
that includes the file. Because hashing may induce collisions, the inode object
includes an i_hash field that contains a backward and a forward pointer to other
inodes that hash to the same position; this field creates a doubly linked list of those
inodes.

The methods associated with an inode object are also called inode operations. They
are described by an inode_operations structure, whose address is included in the i_op
field. Here are the inode operations in the order they appear in the inode_operations
table:

create(dir, dentry, mode, nameidata)

Creates a new disk inode for a regular file associated with a dentry object in some directory.

lookup(dir, dentry, nameidata)

Searches a directory for an inode corresponding to the filename included in a dentry object.

link(old_dentry, dir, new_dentry)

Creates a new hard link that refers to the file specified by old_dentry in the directory dir; the new hard link has the name specified by new_dentry.

unlink(dir, dentry)

Removes the hard link of the file specified by a dentry object from a directory.

symlink(dir, dentry, symname)

Creates a new inode for a symbolic link associated with a dentry object in some directory.

mkdir(dir, dentry, mode)

Creates a new inode for a directory associated with a dentry object in some directory.

rmdir(dir, dentry)

Removes from a directory the subdirectory whose name is included in a dentry object.

mknod(dir, dentry, mode, rdev)

Creates a new disk inode for a special file associated with a dentry object in some directory. The mode and rdev parameters specify, respectively, the file type and the device’s major and minor numbers.

rename(old_dir, old_dentry, new_dir, new_dentry)

Moves the file identified by old_entry from the old_dir directory to the new_dir one. The new filename is included in the dentry object that new_dentry points to.

readlink(dentry, buffer, buflen)

Copies into a User Mode memory area specified by buffer the file pathname corresponding to the symbolic link specified by the dentry.

follow_link(inode, nameidata)

Translates a symbolic link specified by an inode object; if the symbolic link is a relative pathname, the lookup operation starts from the directory specified in the second parameter.

put_link(dentry, nameidata)

Releases all temporary data structures allocated by the follow_link method to translate a symbolic link.

truncate(inode)

Modifies the size of the file associated with an inode. Before invoking this method, it is necessary to set the i_size field of the inode object to the required new size.

permission(inode, mask, nameidata)

Checks whether the specified access mode is allowed for the file associated with inode.

setattr(dentry, iattr)

Notifies a “change event” after touching the inode attributes.

getattr(mnt, dentry, kstat)

Used by some filesystems to read inode attributes.

setxattr(dentry, name, value, size, flags)

Sets an “extended attribute” of an inode (extended attributes are stored on disk blocks outside of any inode).

getxattr(dentry, name, buffer, size)

Gets an extended attribute of an inode.

listxattr(dentry, buffer, size)

Gets the whole list of extended attribute names.

removexattr(dentry, name)

Removes an extended attribute of an inode.

The methods just listed are available to all possible inodes and filesystem types. However, only a subset of them applies to a specific inode and filesystem; the fields corresponding to unimplemented methods are set to NULL.

<h3 id="12.2.3">12.2.3 File对象</h3>

A file object describes how a process interacts with a file it has opened. The object is
created when the file is opened and consists of a file structure, whose fields are
described in Table 12-4. Notice that file objects have no corresponding image on
disk, and hence no “dirty” field is included in the file structure to specify that the
file object has been modified.

The main information stored in a file object is the file pointer—the current position
in the file from which the next operation will take place. Because several processes
may access the same file concurrently, the file pointer must be kept in the file object
rather than the inode object.

File objects are allocated through a slab cache named filp, whose descriptor address
is stored in the filp_cachep variable. Because there is a limit on the number of file
objects that can be allocated, the files_stat variable specifies in the max_files field
the maximum number of allocatable file objects—i.e., the maximum number of files
that can be accessed at the same time in the system.*

“In use” file objects are collected in several lists rooted at the superblocks of the
owning filesystems. Each superblock object stores in the s_files field the head of a
list of file objects; thus, file objects of files belonging to different filesystems are
included in different lists. The pointers to the previous and next element in the list
are stored in the f_list field of the file object. The files_lock spin lock protects the
superblock s_files lists against concurrent accesses in multiprocessor systems.

The f_count field of the file object is a reference counter: it counts the number of
processes that are using the file object (remember that lightweight processes created
with the CLONE_FILES flag share the table that identifies the open files, thus they use
the same file objects). The counter is also increased when the file object is used by
the kernel itself—for instance, when the object is inserted in a list, or when a dup()
system call has been issued.

When the VFS must open a file on behalf of a process, it invokes the get_empty_filp(
) function to allocate a new file object. The function invokes kmem_cache_alloc() to
get a free file object from the filp cache, then it initializes the fields of the object as
follows:

memset(f, 0, sizeof(*f));
INIT_LIST_HEAD(&f->f_ep_links);
spin_lock_init(&f->f_ep_lock);
atomic_set(&f->f_count, 1);
f->f_uid = current->fsuid;
f->f_gid = current->fsgid;
f->f_owner.lock = RW_LOCK_UNLOCKED;
INIT_LIST_HEAD(&f->f_list);
f->f_maxcount = INT_MAX;

As we explained earlier in the section “The Common File Model,” each filesystem
includes its own set of file operations that perform such activities as reading and writing
a file. When the kernel loads an inode into memory from disk, it stores a pointer
to these file operations in a file_operations structure whose address is contained in
the i_fop field of the inode object. When a process opens the file, the VFS initializes
the f_op field of the new file object with the address stored in the inode so that further
calls to file operations can use these functions. If necessary, the VFS may later
modify the set of file operations by storing a new value in f_op.

The following list describes the file operations in the order in which they appear in
the file_operations table:

llseek(file, offset, origin)
Updates the file pointer.
read(file, buf, count, offset)
Reads count bytes from a file starting at position *offset; the value *offset
(which usually corresponds to the file pointer) is then increased.
aio_read(req, buf, len, pos)
Starts an asynchronous I/O operation to read len bytes into buf from file position
pos (introduced to support the io_submit() system call).
write(file, buf, count, offset)
Writes count bytes into a file starting at position *offset; the value *offset
(which usually corresponds to the file pointer) is then increased.
aio_write(req, buf, len, pos)
Starts an asynchronous I/O operation to write len bytes from buf to file position
pos.
readdir(dir, dirent, filldir)
Returns the next directory entry of a directory in dirent; the filldir parameter
contains the address of an auxiliary function that extracts the fields in a directory
entry.
poll(file, poll_table)
Checks whether there is activity on a file and goes to sleep until something happens
on it.
ioctl(inode, file, cmd, arg)
Sends a command to an underlying hardware device. This method applies only
to device files.
unlocked_ioctl(file, cmd, arg)
Similar to the ioctl method, but it does not take the big kernel lock (see the section
“The Big Kernel Lock” in Chapter 5). It is expected that all device drivers
and all filesystems will implement this new method instead of the ioctl method.
compat_ioctl(file, cmd, arg)
Method used to implement the ioctl() 32-bit system call by 64-bit kernels.
mmap(file, vma)
Performs a memory mapping of the file into a process address space (see the section
“Memory Mapping” in Chapter 16).
open(inode, file)
Opens a file by creating a new file object and linking it to the corresponding
inode object (see the section “The open( ) System Call” later in this chapter).
flush(file)
Called when a reference to an open file is closed. The actual purpose of this
method is filesystem-dependent.
release(inode, file)
Releases the file object. Called when the last reference to an open file is closed—
that is, when the f_count field of the file object becomes 0.
fsync(file, dentry, flag)
Flushes the file by writing all cached data to disk.
aio_fsync(req, flag)
Starts an asynchronous I/O flush operation.
fasync(fd, file, on)
Enables or disables I/O event notification by means of signals.
lock(file, cmd, file_lock)
Applies a lock to the file (see the section “File Locking” later in this chapter).
readv(file, vector, count, offset)
Reads bytes from a file and puts the results in the buffers described by vector;
the number of buffers is specified by count.
writev(file, vector, count, offset)
Writes bytes into a file from the buffers described by vector; the number of buffers
is specified by count.
sendfile(in_file, offset, count, file_send_actor, out_file)
Transfers data from in_file to out_file (introduced to support the sendfile()
system call).
sendpage(file, page, offset, size, pointer, fill)
Transfers data from file to the page cache’s page; this is a low-level method
used by sendfile() and by the networking code for sockets.
get_unmapped_area(file, addr, len, offset, flags)
Gets an unused address range to map the file.
check_flags(flags)
Method invoked by the service routine of the fcntl() system call to perform
additional checks when setting the status flags of a file (F_SETFL command). Currently
used only by the NFS network filesystem.
dir_notify(file, arg)
Method invoked by the service routine of the fcntl() system call when establishing
a directory change notification (F_NOTIFY command). Currently used only by
the Common Internet File System (CIFS) network filesystem.
flock(file, flag, lock)
Used to customize the behavior of the flock() system call. No official Linux filesystem
makes use of this method.

The methods just described are available to all possible file types. However, only a
subset of them apply to a specific file type; the fields corresponding to unimplemented
methods are set to NULL.

<h3 id="12.2.4">12.2.4 dentry对象</h3>

We mentioned in the section “The Common File Model” that the VFS considers each directory a file that contains a list of files and other directories. We will discuss in Chapter 18 how directories are implemented on a specific filesystem. Once a directory entry is read into memory, however, it is transformed by the VFS into a dentry object based on the dentry structure, whose fields are described in Table 12-5. The kernel creates a dentry object for every component of a pathname that a process looks up; the dentry object associates the component to its corresponding inode. For example, when looking up the /tmp/test pathname, the kernel creates a dentry object for the / root directory, a second dentry object for the tmp entry of the root directory, and a third dentry object for the test entry of the /tmp directory.

Notice that dentry objects have no corresponding image on disk, and hence no field is included in the dentry structure to specify that the object has been modified. Dentry objects are stored in a slab allocator cache whose descriptor is dentry_cache; dentry objects are thus created and destroyed by invoking kmem_cache_alloc( ) and kmem_cache_free( ).

Table 12-5. The fields of the dentry object

Each dentry object may be in one of four states:

Free

The dentry object contains no valid information and is not used by the VFS. The corresponding memory area is handled by the slab allocator.

Unused

The dentry object is not currently used by the kernel. The d_count usage counter of the object is 0, but the d_inode field still points to the associated inode. The dentry object contains valid information, but its contents may be discarded if necessary in order to reclaim memory.

In use

The dentry object is currently used by the kernel. The d_count usage counter is positive, and the d_inode field points to the associated inode object. The dentry object contains valid information and cannot be discarded.

Negative

The inode associated with the dentry does not exist, either because the corresponding disk inode has been deleted or because the dentry object was created by resolving a pathname of a nonexistent file. The d_inode field of the dentry object is set to NULL, but the object still remains in the dentry cache, so that further lookup operations to the same file pathname can be quickly resolved. The term “negative” is somewhat misleading, because no negative value is involved.

The methods associated with a dentry object are called dentry operations; they are described by the dentry_operations structure, whose address is stored in the d_op field. Although some filesystems define their own dentry methods, the fields are usually NULL and the VFS replaces them with default functions. Here are the methods, in the order they appear in the dentry_operations table:

<h3 id="12.2.5">12.2.5 与进程关联的文件</h3>

We mentioned in the section “An Overview of the Unix Filesystem” in Chapter 1 that each process has its own current working directory and its own root directory. These are only two examples of data that must be maintained by the kernel to represent the interactions between a process and a filesystem. A whole data structure of type fs_struct is used for that purpose (see Table 12-6), and each process descriptor has an fs field that points to the process fs_struct structure.


<h2 id="12.3">12.3 文件系统类型</h2>

The Linux kernel supports many different types of filesystems. In the following, we
introduce a few special types of filesystems that play an important role in the internal
design of the Linux kernel.

Next, we’ll discuss filesystem registration—that is, the basic operation that must be
performed, usually during system initialization, before using a filesystem type. Once
a filesystem is registered, its specific functions are available to the kernel, so that type
of filesystem can be mounted on the system’s directory tree.


<h3 id="12.3.1">12.3.1 特殊的文件系统</h3>

While network and disk-based filesystems enable the user to handle information
stored outside the kernel, special filesystems may provide an easy way for system
programs and administrators to manipulate the data structures of the kernel and to
implement special features of the operating system. Table 12-8 lists the most common
special filesystems used in Linux; for each of them, the table reports its suggested
mount point and a short description.

Notice that a few filesystems have no fixed mount point (keyword “any” in the
table). These filesystems can be freely mounted and used by the users. Moreover,
some other special filesystems do not have a mount point at all (keyword “none” in
the table). They are not for user interaction, but the kernel can use them to easily
reuse some of the VFS layer code; for instance, we’ll see in Chapter 19 that, thanks to
the pipefs special filesystem, pipes can be treated in the same way as FIFO files.

Special filesystems are not bound to physical block devices. However, the kernel
assigns to each mounted special filesystem a fictitious block device that has the value
0 as major number and an arbitrary value (different for each special filesystem) as a
minor number. The set_anon_super() function is used to initialize superblocks of
special filesystems; this function essentially gets an unused minor number dev and
sets the s_dev field of the new superblock with major number 0 and minor number
dev. Another function called kill_anon_super() removes the superblock of a special
filesystem. The unnamed_dev_idr variable includes pointers to auxiliary structures that
record the minor numbers currently in use. Although some kernel designers dislike
the fictitious block device identifiers, they help the kernel to handle special filesystems
and regular ones in a uniform way.

We’ll see a practical example of how the kernel defines and initializes a special filesystem
in the later section “Mounting a Generic Filesystem.”


<h3 id="12.3.2">12.3.2 注册文件系统类型</h3>

Often, the user configures Linux to recognize all the filesystems needed when compiling
the kernel for his system. But the code for a filesystem actually may either be
included in the kernel image or dynamically loaded as a module (see Appendix B).
The VFS must keep track of all filesystem types whose code is currently included in
the kernel. It does this by performing filesystem type registration.

Each registered filesystem is represented as a file_system_type object whose fields
are illustrated in Table 12-9.

All filesystem-type objects are inserted into a singly linked list. The file_systems
variable points to the first item, while the next field of the structure points to the
next item in the list. The file_systems_lock read/write spin lock protects the whole
list against concurrent accesses.

The fs_supers field represents the head (first dummy element) of a list of superblock
objects corresponding to mounted filesystems of the given type. The backward and
forward links of a list element are stored in the s_instances field of the superblock
object.

The get_sb field points to the filesystem-type-dependent function that allocates a
new superblock object and initializes it (if necessary, by reading a disk). The kill_sb
field points to the function that destroys a superblock.

The fs_flags field stores several flags, which are listed in Table 12-10.

During system initialization, the register_filesystem( ) function is invoked for every
filesystem specified at compile time; the function inserts the corresponding file_
system_type object into the filesystem-type list.

The register_filesystem( ) function is also invoked when a module implementing a
filesystem is loaded. In this case, the filesystem may also be unregistered (by invoking
the unregister_filesystem( ) function) when the module is unloaded.

The get_fs_type( ) function, which receives a filesystem name as its parameter, scans
the list of registered filesystems looking at the name field of their descriptors, and
returns a pointer to the corresponding file_system_type object, if it is present.


<h2 id="12.4">12.4 文件系统处理</h2>
<h3 id="12.4.1">12.4.1 命名空间</h3>
<h3 id="12.4.2">12.4.2 文件系统挂载</h3>
<h3 id="12.4.3">12.4.3 挂载一个通用文件系统</h3>
<h3 id="12.4.4">12.4.4 挂载根文件系统</h3>
<h3 id="12.4.5">12.4.5 卸载文件系统</h3>

<h2 id="12.5">12.5 路径查找</h2>
<h3 id="12.5.1">12.5.1 标准路径查找</h3>
<h3 id="12.5.2">12.5.2 父路径查找</h3>
<h3 id="12.5.1">12.5.1 符号链接查找</h3>

<h2 id="12.6">12.6 VFS系统调用的实现</h2>
<h3 id="12.6.1">12.6.1 open()实现</h3>
<h3 id="12.6.2">12.6.2 read()实现</h3>
<h3 id="12.6.3">12.6.3 close()实现</h3>

<h2 id="12.7">12.7 文件锁保护</h2>
<h3 id="12.7.1">12.7.1 Linux文件锁保护</h3>
<h3 id="12.7.2">12.7.2 文件锁数据结构</h3>
<h3 id="12.7.3">12.7.3 FL_FLOCK锁</h3>
<h3 id="12.7.4">12.7.4 FL_POSIX锁</h3>

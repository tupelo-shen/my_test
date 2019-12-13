<h1 id="0">0 目录</h1>

* [1 Linux文件系统目录结构](#1)
* [2 Linux文件系统与设备驱动之间的关系](#2)
* [3 Linux文件系统相关的结构体](#3)
    - [3.1 file_operations结构](#3.1)
    - [3.2 file结构](#3.2)

---

<h1 id="1">1 Linux文件系统目录结构</h1>

linux根目录下的内容：

1. /bin

    包含基本命令，如ls、cp、mkdir等，这个目录中的文件都是可执行的。相等于Windows的可执行文件`*.exe`。

2. /sbin

    包含系统命令，如modprobe、hwclock、ifconfig等，大多是涉及系统管理的命令，这个目录中的文件都是可执行的。

3. /dev

    存放设备文件，应用程序可以通过对这些文件的读写和控制以访问实际的设备

4. /etc

    存放系统配置文件。busybox的启动脚本也存放在该目录下。

5. /lib

    系统库文件存放目录

6. /mnt

    存放挂载存储设备的挂载目录，比如含有cdrom等目录。可以查看/etc/fstab的定义。有时，我们可以让系统开机自动挂载文件系统，并把挂载点放在这里。

7. /opt

    opt是可选的意思，这也就是说，用户安装的一些软件可以存放在这儿。

8. /proc

    操作系统运行时，进程以及内核信息（比如CPU、硬盘分区、内存信息等）存放在这里。/proc目录为伪文件系统proc的挂载目录，proc并不是真正的文件系统，它存在于内存之中。

9. /tmp

    存放临时文件

10. /usr

    这个是系统存放程序的目录，比如用户命令、用户库等

11. /var

    存放系统运行中产生的文件等

12. /sys

    Linux2.6以后的内核所支持的sysfs文件系统被映射到此处。Linux设备驱动模型中的总线、驱动和设备都可以在sysfs文件系统中找到对应的节点。当内核检测到在系统中出现了新设备后，内核会在sysfs文件系统中为该新设备生成一个目录。

<h1 id="2">2 Linux文件系统与设备驱动之间的关系</h1>

在linux的世界，一直存在着`一切皆文件`的传说。相传有一个文件系统在管理着linux的一切设备，我们将这个文件系统称为虚拟文件系统。至于虚拟文件系统的实现，我们后面再写文章具体分析。

![5-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/SongBaohua/images/5-1.PNG)

如上图所示，应用程序与虚拟文件系统VFS的接口是系统调用，而VFS与文件系统以及设备文件之间的接口是file_operation这个结构体中的成员函数。file_operation提供了对文件进行open，read，write，close，ctrl等方法。

对于设备文件，设备驱动直接提供了file_operation各个成员函数的实现，这正是字符驱动设备的核心。

但是对于块设备，其访问方法有2种。一种是通过文件系统访问，file_operation在文件系统中实现，这样的文件系统有ext2，ext3，ext4，fatfs等，设备驱动层无需关注file_operation。还有一种方法是，跳过文件系统，直接访问设备，就像字符设备的访问方式一样。针对这种方法，Linux内核实现了统一的file_operation，命名为def_blk_fops，源代码位于fs/block_dev.c文件中。

<h1 id="3">3 Linux文件系统相关的结构体</h1>

<h2 id="3.1">3.1 file_operations结构</h2>

通过上面的[第2章 Linux文件系统与设备驱动之间的关系](#2)，我们了解到file_operations结构是设备驱动程序的实现关键。那么就来看一下，它的实现细节。源代码位于/include/linux/fs.h文件中：

    struct file_operations {
        struct module *owner;
        loff_t (*llseek) (struct file *, loff_t, int);
        ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
        ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
        ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
        ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
        int (*iterate) (struct file *, struct dir_context *);
        unsigned int (*poll) (struct file *, struct poll_table_struct *);
        long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
        long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
        int (*mmap) (struct file *, struct vm_area_struct *);
        int (*open) (struct inode *, struct file *);
        int (*flush) (struct file *, fl_owner_t id);
        int (*release) (struct inode *, struct file *);
        int (*fsync) (struct file *, loff_t, loff_t, int datasync);
        int (*aio_fsync) (struct kiocb *, int datasync);
        int (*fasync) (int, struct file *, int);
        int (*lock) (struct file *, int, struct file_lock *);
        ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
        unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
        int (*check_flags)(int);
        int (*flock) (struct file *, int, struct file_lock *);
        ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
        ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
        int (*setlease)(struct file *, long, struct file_lock **, void **);
        long (*fallocate)(struct file *file, int mode, loff_t offset,
                  loff_t len);
        void (*show_fdinfo)(struct seq_file *m, struct file *f);
    #ifndef CONFIG_MMU
        unsigned (*mmap_capabilities)(struct file *);
    #endif
    };

该结构体定义了所有设备访问所需要的方法。但是，大部分情况下，我们只需要实现其中一些关键的，自己需要的方法就可以了。对于每一种方法的理解可以参考[Linux设备驱动程序]()一书。

现在，我们已经了解了一些实现访问设备的方法。但是，访问设备的话，我们必须有一个地方，存储这个设备的信息和数据等，这样前面的方法才能有操作的对象不是？那么，让我们来看下一节。

<h2 id="3.2">3.2 file结构</h2>

在`<linux/fs.h>`中定义的结构体file是设备驱动程序中使用的第二个最重要的数据结构。请注意，file结构体和用户空间程序的FILE结构体没有任何关联，不要混淆。系统中每个打开的文件在内核中都有一个关联的file结构与之对应。它由内核在open的时候创建，并传递给操作文件的任何函数，直到最后一次关闭。文件的所有引用关闭后，内核会释放数据结构。

    struct file {
        union {
            struct llist_node   fu_llist;
            struct rcu_head     fu_rcuhead;
        } f_u;
        struct path                     f_path;
        struct inode                    *f_inode;   /* cached value */
        const struct file_operations    *f_op;

        spinlock_t              f_lock;     # 保护 f_flags不会被IRQ改写。
        atomic_long_t           f_count;
        unsigned int            f_flags;
        fmode_t                 f_mode;
        struct mutex            f_pos_lock;
        loff_t                  f_pos;
        struct fown_struct      f_owner;
        const struct cred       *f_cred;
        struct file_ra_state    f_ra;

        u64                     f_version;
        struct address_space    *f_mapping;
    };

上面的代码，是去除了一些宏定义和不必要的信息后的精简版本，但是不影响我们对file结构的理解。我们对几个关键的成员变量进行说明：

1. f_mode

    可以通过设置FMODE_READ和FMODE_WRITE位将文件标识为可读或可写（或两者）。

2. f_pos

    文件访问的当前位置。

3. f_flags

    这些是文件标志，例如O_RDONLY，O_NONBLOCK和O_SYNC。

4. *f_op

    与文件关联的操作。

对于file结构，我们只需要理解即可。因为在实际的驱动开发中，不会直接创建file结构的，都是从其它结构引用过来的。

<h2 id="3.3">3.3 inode结构</h2>

内核使用inode结构来表示文件。因此，它与file结构不同，后者表示打开的文件描述符。对单个文件，可能会有许多个表示打开的文件描述符的file结构存在，但是它们都指向单个inode结构。源代码如下：

    struct inode {
        umode_t         i_mode;                     # inode的权限
        unsigned short  i_opflags;                  #
        kuid_t          i_uid;                      # inode拥有者的id
        kgid_t          i_gid;                      # inode所属群组id
        unsigned int    i_flags;

        const struct inode_operations   *i_op;
        struct super_block              *i_sb;
        struct address_space            *i_mapping;

        unsigned long       i_ino;
        dev_t               i_rdev;                 # 设备编号，主次设备号
        loff_t              i_size;                 # inode所代表的文件大小
        struct timespec     i_atime;                # 最近一次的访问时间
        struct timespec     i_mtime;                # inode的产生时间
        struct timespec     i_ctime;
        spinlock_t          i_lock;
        unsigned short      i_bytes;                # inode所使用的字节数
        unsigned int        i_blkbits;
        blkcnt_t            i_blocks;               # inode所使用的block数，通常一个block大小为512字节
        ......(省略)
        const struct file_operations    *i_fop;
        struct file_lock_context    *i_flctx;
        struct address_space    i_data;
        struct list_head    i_devices;
        union {
            struct pipe_inode_info  *i_pipe;
            struct block_device     *i_bdev;        # 若是块设备，为其对应的block_device结构体指针
            struct cdev             *i_cdev;        # 若是字符设备，为其对应的cdev结构体指针
            char                    *i_link;
        };

        __u32           i_generation;
        void            *i_private; /* fs or device private pointer */
    };


inode结构包含大量关于有关文件的信息。一般情况下，我们只需要关心下面几个成员就可以了：

* i_rdev

    对于表示设备文件的inode，此成员包含实际的设备编号。操作下列操作可以从inode中获取主设备和次设备号

        unsigned int iminor(struct inode *inode);
        unsigned int imajor(struct inode *inode);

    查看/proc/devices文件可以获知系统中注册的设备的主设备号和设备名称。我们查看前面运行的系统的/proc/devices文件，结果如下：

        Character devices:
          1 mem
          2 pty
        ......(太长，省略)
        Block devices:
        259 blkext
          8 sd
        ......(太长，省略)

    * 主设备号

        主设备号是与驱动对应的概念，同一类设备一般使用相同的主设备号，不同类的设备一般使用不同的主设备号。

    * 次设备号

        因为同一个驱动可以支持多个同类设备，所以需要次设备号表示具体的设备驱动，一般从0开始。

* *i_cdev

    若是字符设备，为其对应的cdev结构体指针

* *i_bdev

    若是块设备，为其对应的block_device结构体指针

<h1 id="4">4 devfs</h1>

特点如下：

1. 可以通过程序在设备初始化的时候，在/dev目录下创建设备文件，卸载时将它删除。
2. 允许驱动程序指定设备名、所有者和权限位，用户空间程序仍可以修改所有者和用户权限。
3. 不再需要为设备驱动程序分配主设备号以及处理次设备号。

因为devfs已经被废弃，所以不再深入研究。



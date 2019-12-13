<h1 id="0">0 目录</h1>

* [3.1 scull设计](#3.1)
* [3.2 主次设备号](#3.2)
    - [3.2.1 设备号的内部表示](#3.2.1)
    - [3.2.2 分配和释放设备号](#3.2.2)
    - [3.2.3 主设备号的动态分配](#3.2.3)
* [3.3 字符设备注册](#3.3)
    - [3.3.1 scull的设备注册](#3.3.1)
* [3.4 open和release](#3.4)
    - [3.4.1 open方法](#3.4.1)
    - [3.4.2 release方法](#3.4.2)
* [3.5 scull内存使用](#3.5)
* [3.6 write和read](#3.6)
* [3.7 实例](#3.7)
    - [3.7.1 测试环境](#3.7.1)
    - [3.7.2 源代码](#3.7.2)
    - [3.7.3 测试过程](#3.7.3)

---

参考LDD3的scull系列虚拟字符设备。通俗地讲，scull就是一个能够读写内存区域的字符设备。

<h2 id="3.1">3.1 scull设计</h2>

驱动程序编写的第一步是定义驱动程序为用户程序提供的功能（机制）。由于我们的“设备”是计算机内存的一部分，因此我们可以自由地按照自己的意愿去做。它可以是顺序或随机访问设备，也可以是一个或多个设备，等等。

为了让scull成为编写真实设备、真实驱动程序的模板，我们将向您展示如何在计算机内存上实现多个设备抽象，每个设备抽象具有不同的个性。

scull源代码实现以下几种设备。 模块实现的每一种设备归为一类。

* scull0~scull3

    4个设备，每个设备都包含一个全局、永久的内存区域。全局意味着如果设备被多次打开，设备中包含的数据将被打开它的所有文件描述符共享。持久意味着如果设备关闭并重新打开，数据不会丢失。使用这些设备非常有趣，因为可以使用常见命令，如 **cp**，**cat** 和 **shell I/O重定向** 进行访问和测试。

* scullpipe0~scullpipe3

    4个 *FIFO*（先进先出）设备，就像管道一样。一个进程读取，另一个进程写入。如果多个进程读取相同的设备，会产生数据竞争。*scullpipe* 的内部将展示，如何实现阻塞和非阻塞读写，而不必求助于中断。 虽然真正的驱动程序使用硬件中断与其设备同步， 但是阻塞和非阻塞操作的内容也非常重要，与中断处理是分开学习。

* scullsingle、scullpriv、sculluid、scullwuid

    这些设备与scull0类似，但在打开时有一些限制。第一个（scullsingle）每次只允许一个进程使用驱动程序，而scullpriv对于每个虚拟控制台（或X终端会话）是私有的，因为每个控制台/终端上的进程获得不同的内存区域。sculluid和scullwuid可以多次打开，但一次只能由一个用户打开;如果另一个用户正在锁定设备，则前者返回“设备忙”的错误，而后者实现阻止打开。这些变种的scull似乎是混淆策略和机制，但值得关注，因为一些现实生活中的设备需要这种管理。

每个scull设备都展示了驱动程序的不同特征，表现出了不同的难度。本章涵盖了scull0到scull3的内部结构；关于更高级的设备在第6章中介绍。有关scullpipe的信息，请参见“一个阻塞I/O的例子”一节，其它的在“设备文件的访问控制”中进行了介绍。

<h2 id="3.2">3.2 主次设备号</h2>

字符设备通过文件系统中的名称进行访问。这些名称被称为特殊文件或设备文件或简称为文件系统树的节点;它们通常位于 */dev*目录中。字符驱动程序的特殊文件由 *ls -l*输出的第一列中的“*c*”标识。块设备也出现在 */dev*中，但是它们由“*b*”标识。本章的焦点在字符设备上，但以下大多数信息也适用于块设备。
如果键入 *ls -l*命令，则会在上次修改日期之前的设备文件项中看到两个数字（用逗号分隔），其中文件长度通常出现在该处。这些数字是特定设备的主要和次要设备号。以下列表显示了一些典型系统上显示的设备。它们的主要设备号是1,4,7和10，而次要设备号是1,3,5,64,65和129。

    crw-rw-rw- 1 root root  1,  3   Apr 11 2002 null
    crw------- 1 root root  10, 1   Apr 11 2002 psaux
    crw------- 1 root root  4,  1   Oct 28 03:04 tty1
    crw-rw-rw- 1 root tty   4,  64  Apr 11 2002 ttys0
    crw-rw---- 1 root uucp  4,  65  Apr 11 2002 ttyS1
    crw--w---- 1 vcsa tty   7,  1   Apr 11 2002 vcs1
    crw--w---- 1 vcsa tty   7,  129 Apr 11 2002 vcsa1
    crw-rw-rw- 1 root root  1,  5   Apr 11 2002 zero

传统上，主设备号标识与设备关联的驱动程序。例如，/dev/null和/dev/zero都由驱动程序1管理，而虚拟控制台和串行终端由驱动程序4管理；类似地，vcs1和vcsa1设备都由驱动程序7管理。现代Linux内核允许多个驱动程序共享主设备号，但您将看到的大多数设备仍然按照 *一一对应*原则组织。

内核使用次设备号来确定究竟哪个设备被引用。根据驱动程序的写法（如下面所示），您可以从内核获取指向设备的直接指针，也可以使用次要编号作为本地设备阵列的索引。无论哪种方式，内核本身除了知道它们引用的是驱动程序实现的设备之外，对其它一无所知。

> <font color=red>读书笔记：</font>
>
> <font color=red>为了方便的标识文件，除了文件名称以外，内核提供了主设备号和次设备号对设备进行分类编号。一般情况下，主设备号对应于一类设备驱动程序；次设备号对应于某一具体的设备。</font>
>
> <font color=red>下面的内容就是对主次设备号的读写操作细节进行介绍</font>

<h3 id="3.2.1">3.2.1 设备号的内部表示</h3>

在内核中，*dev_t* （位于 *linux/types.h* 头文件中）用于保存设备号-主、次设备号。从内核版本2.6.0开始，*dev_t* 是一个32位数， 主设备号为12位，次设备号为20。我们应该使用 *linux/kdev_t.h* 中的一组宏来操作主、次设备号。如下：

    MAJOR(dev_t dev);
    MINOR(dev_t dev);

反过来，把主次设备号转换成 *dev_t* 型的数据，请使用：

    MKDEV(int major, int minor);

<h3 id="3.2.2">3.2.2 分配和释放设备号</h3>

在注册字符设备时，驱动程序需要做的第一件事就是获取设备号。要调用的函数就是 *register_chrdev_region*，其声明位于 *linux/fs.h* 文件中：

    int register_chrdev_region(dev_t first, unsigned int count, char *name);

函数参数说明：

* first

    是想要分配的设备号范围的起始位置；first的次要设备号通常是0，但没有要求。

* count

    是请求的连续设备号的总数。请注意，如果count很大，您请求的范围可能会溢出到下一个主要设备号；但只要请求的号码范围可用，所有内容仍将正常工作。

* name

    是应该与此号码范围关联的设备的名称；它会出现在 */proc/devices* 和 *sysfs* 中。

与大多数内核函数一样，如果分配成功执行，*register_chrdev_region* 的返回值将为0。 如果发生错误， 将返回一个负数的错误代码， 并且将无法访问请求的区域。

如果您事先知道所需的设备号， *register_chrdev_region* 可以很好地工作。但是，通常情况下，不会知道设备将使用哪些主设备号；Linux内核开发社区一直在努力转向使用动态分配的设备编号。内核会快速地为设备分配一个主设备号，但必须使用不同的函数来请求：

    int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);

使用此函数，*dev* 是一个只输出的参数，在成功完成时，将保留分配范围中的第一个数字。 *firstminor*应该是请求的第一个次设备号； 它通常是0。*count* 和 *name* 参数与 *request_chrdev_region* 的参数一样。

但是，无论如何分配设备号，不再使用时都应该释放它们。设备号释放函数：

    void unregister_chrdev_region(dev_t first, unsigned int count);

调用 *unregister_chrdev_region* 的通常位置是在模块的清理函数中。

上述函数为驱动程序分配设备号，但它们不会告诉内核使用这些设备号做什么。在用户空间程序访问其中一个设备号之前，驱动程序需要将它们连接到实现设备操作的内部函数。我们将尽快描述这种连接是如何完成的，但首先需要处理几个其它的问题。

<h3 id="3.2.3">3.2.3 主设备号的动态分配</h3>

一些主设备号被静态地分配给最常用的设备了。这些设备的列表可以在内核源码树中的 *Documentation/devices.txt* 中找到。作为一名驱动开发者，你可以简单地选择一个似乎未被使用的设备号，或者可以动态地分配主设备号。如果驱动程序的唯一用户是自己，选择一个号码就可以工作；一旦驱动程序被广泛地部署，随机挑选的主设备号将导致冲突和麻烦。

因此，对于新的驱动程序，我们强烈建议使用动态分配来获取主设备号，而不是随机选择一个号。换句话说，驱动程序尽量使用 *alloc_chrdev_region* 而不是 *register_chrdev_region*。动态分配的缺点是无法预先创建设备节点，因为分配给模块的主设备号会有所不同。对于驱动程序的正常使用，这不是一个问题，因为一旦分配了编号，就可以从 */proc/devices* 中读取它。

当然了，分配主设备号的最好办法就是，默认动态分配，但可以在编译或者加载模块的时候手动指定主设备号。*scull* 模块的代码使用一个全局 变量 *scull_major* 来保存设定的主设备号（使用 *scull_minor* 保存次设备号）。该变量在 *scull.h* 中定义，初始化为 *SCULL_MAJOR*。源代码中 *SCULL_MAJOR*的缺省值为 *0*，这意味着“使用动态分配”。除此之外，用户可以接受默认值或选择特定的主设备号， 也可以通过在编译前修改宏或在 **insmod**中指定 *scull_major*的值。还可以使用 *scull_load*脚本，通过命令行参数传递给 *insmod*。

在我们看来，分配主设备号的最好方法是默认动态分配，同时让你自己可以选择在加载时或编译时指定主设备号。scull实现以这种方式工作；它使用一个全局变量scull_major来保存选定的主设备号（对于次设备号也有一个scull_minor）。该变量在scull.h中定义，初始化为SCULL_MAJOR。源代码中SCULL_MAJOR的缺省值为0，这意味着“使用动态分配”。用户可以接受缺省值或选择特定的主设备号，可以通过在编译前修改宏或在insmod中指定scull_major的值。最后，通过使用scull_load脚本，用户可以在scull_load的命令行上将参数传递给insmod。
以下是我们在scull源代码中使用的代码，以获得一个主设备号：

    if (scull_major) {
        dev = MKDEV(scull_major, scull_minor);
        result = register_chrdev_region(dev, scull_nr_devs, "scull");
    } else {
        result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
        scull_major = MAJOR(dev);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }

本书中几乎所有使用的示例驱动程序都使用类似的代码来进行主设备号的分配。

> <font color=red>读书笔记：</font>
>
> <font color=red>对于主设备号的分配，尽量选择使用alloc_chrdev_region动态分配函数</font>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3.3">3.3 字符设备注册</h2>

正如我们所提到的，内核在内部使用struct cdev表示字符设备。在内核调用设备的操作之前，必须分配、注册一个或多个上述结构。为此，我们的代码应该包含`<linux/cdev.h>`，该文件定义了这个结构及其相关的辅助函数。

分配、初始化struct cdev结构有2种方法，一种是单纯的就是cdev结构，比如

    struct cdev *my_cdev = cdev_alloc( );
    my_cdev->ops = &my_fops;

还有一种就是，将struct cdev架构嵌入到自定义的特定设备结构中。scull设备就是这么做的。

    void cdev_init(struct cdev *cdev, struct file_operations *fops);

无论那种方式，struct cdev的owner成员都应该设置为THIS_MODULE。

在cdev结构设置好后，最后一步就是通过下面的调用告知内核该结构的信息：

    int cdev_add(struct cdev *dev, dev_t num, unsigned int count);

这里，dev是cdev结构，num是该设备对应的第一个设备编号，count是与设备关联的设备编号的数量。通常count取1，但在某些情况下，有多个设备编号对应于特定设备是有意义的。 例如，考虑SCSI磁带驱动程序，它允许用户空间通过为每个物理设备分配多个次设备编号来选择操作模式（如密度）。

使用cdev_add时需要牢记几件重要的事情。首先，这个调用可能会失败。如果它返回负的错误码，则说明设备尚未添加到系统中。然而，它几乎总是成功的，这就又提出了另一个问题：只要cdev_add返回，我们的设备就是“活着的”，它的操作可以被内核调用。所以，在驱动程序完全准备好处理设备上的操作之前，不应该调用cdev_add。

要从系统中移除一个字符设备，请调用：

    void cdev_del(struct cdev *dev);

显然，在将cdev结构传递给cdev_del之后，就不应该再访问cdev结构了。

> <font color=red>读书笔记：</font>
>
> <font color=red>我们应该首选cdev_init这种字符设备分配、初始化方式</font>

<h2 id="3.3.1">3.3.1 scull的设备注册</h2>

在scull内部，它通过struct scull_dev结构来表示每个设备。这个结构的定义如下：

    struct scull_dev {
        struct scull_qset *data;        /* 指向第一个量子集的指针 */
        int quantum;                    /* 当前量子的大小 */
        int qset;                       /* 当前数组的大小 */
        unsigned long size;             /* 保存在其中的数据总量 */
        unsigned int access_key;        /* sculluid 和scullpriv使用 */
        struct semaphore sem;           /* 互斥信号量 */
        struct cdev cdev;               /* 字符设备结构 */
    };

当我们遇到这个结构体中的成员的时候再展开讨论，但是现在，我们先将注意力放到cdev身上，结构体cdev是我们的设备和内核之间的接口。这个结构体必须如上所述地被初始化并被添加到系统中，scull中的处理这一部分的代码如下：

    static void scull_setup_cdev(struct scull_dev *dev, int index)
    {
        int err, devno = MKDEV(scull_major, scull_minor + index);
        cdev_init(&dev->cdev, &scull_fops);
        dev->cdev.owner = THIS_MODULE;
        dev->cdev.ops = &scull_fops;
        err = cdev_add (&dev->cdev, devno, 1);
        /* 异常处理 */
        if (err)
            printk(KERN_NOTICE "Error %d adding scull%d", err, index);
    }

由于cdev结构嵌入在struct scull_dev中，因此必须调用cdev_init才能执行该结构的初始化。

<h2 id="3.4">3.4 open和release</h2>

在[Linux设备驱动开发5-Linux文件系统]()一文中，我们已经介绍了要实现设备驱动程序，必须实现的方法。这儿就是介绍scull中实现的方法。

<h2 id="3.4.1">3.4.1 open方法</h2>

open方法提供给驱动程序进行初始化的能力，为以后的操作做准备。在大多数驱动程序中，open应该执行以下任务：

* 检查设备特定错误（像设备未就绪或类似的硬件问题）；
* 如果设备是首次打开，初始化设备；
* 如有必要，更新f_op指针；
* 分配并填充放置于filp->private_data中的任何数据结构；

然而，首先要做的就是确定要打开的设备。注意，open方法的原型如下：

    int (*open)(struct inode *inode, struct file *filp);

参数inode以i_cdev字段的形式提供我们需要的信息，其中包含我们之前设置的cdev结构。 唯一的问题是我们通常不需要cdev结构本身，我们希望包含该cdev结构的scull_dev结构。 C语言让程序员可以玩各种各样的技巧来进行这种转换；然而，使用这样的技巧是容易出错的，并且导致其他人难以阅读和理解代码。幸运的是，在这种情况下，内核hacker已经为我们做了棘手的事情，它通过定义在`<linux / kernel.h>`中的`container_of`宏的形式实现：

    container_of(pointer, container_type, container_field);

这个宏在类型为container_type的结构中接受一个类型为container_field的字段的指针，并返回一个指向包含该字段的结构的指针。在scull_open中，该宏用于查找适当的设备结构：

    struct scull_dev *dev;         /* 设备信息 */
    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev; /* for other methods */

一旦找到了scull_dev结构，scull就会在file结构的private_data字段中存储一个指向它的指针，以便将来访问。

另外一个确定要打开的设备的方法是查看inode结构中存储的次设备号。如果使用register_chrdev注册设备，则必须使用此技术。 请务必使用iminor宏从inode结构中获取次设备号，并确保它对应于驱动程序实际准备打开的设备。

经过简化的scull_open的代码如下：

    int scull_open(struct inode *inode, struct file *filp)
    {
        struct scull_dev *dev;                  /* 设备信息 */
        dev = container_of(inode->i_cdev, struct scull_dev, cdev);
        filp->private_data = dev;               /* 其它方法 */
        /* 如果以write-only方式打开设备，修改文件的长度为0 */
        if ( (filp->f_flags & O_ACCMODE) = = O_WRONLY) {
            scull_trim(dev);                    /* 忽略错误 */
        }
        return 0;                               /* 成功返回0 */
    }

这段代码看起来相当短小，因为在调用open时不执行任何特定的设备处理。因为scull设备被设计为全局且持久的，这段代码无需做什么工作。特别是，我们不需要维护scull设备的打开计数，而只维护模块的使用计数，因此也就没有“首次打开时初始化设备”这类动作。

对设备唯一的实际操作就是，当设备以写的方式打开时，它的长度被截为0.出现这种特性的原因在于，在设计上，当用更短的文件覆盖一个scull设备时，设备数据区应相应缩小。这与用写方式打开普通文件时将长度截短为0的方式很相似。如果设备以读取方式打开，则什么也不做。

<h2 id="3.4.2">3.4.2 release方法</h2>

release方法的作用正好和open相反。有时候，读者会发现这个方法的实现被称为device_close。无论是哪种形式，都应实现下面的功能：

* 释放由open分配的、保存在filp->private_data中的所有内容；
* 在最后一次关闭操作时关闭设备；

最基本的scull没有硬件可以关闭，因此所需的代码是最小的。

    int scull_release(struct inode *inode, struct file *filp)
    {
        return 0;
    }

当关闭一个设备文件的次数比打开它的次数多时，系统中会发生什么情况呢？毕竟，dup和fork都会在不调用open的情况下创建已打开文件的副本；但是在程序终止时每一个副本都会被关闭。例如，大多数程序从来不会打开它们的stdin文件（或设备），但是它们都会在终止时关闭。驱动程序如何才能知道已打开设备文件要被真正关闭呢？

答案很简单：并不是每个close系统调用都会导致release方法被调用。只有真正释放设备数据结构的close调用才会调用该方法–这就是它名称的由来。内核保存一个file结构使用次数的计数器。fork和dup都不会创建一个新的file结构（只有open方法才会创建新的file结构）。他们只是增加现有结构中的计数器。只有当file结构的计数器下降到0时，close系统调用才会执行release方法，这发生在file结构被销毁时。release方法和close系统调用之间的这种关系可以保证对于每次open调用，驱动程序只会看到一个release调用。

请注意，flush方法在应用程序每次调用close时都会调用。然而，很少有驱动程序实现flush，因为除非release被调用，否则通常没有什么可以在close时去做。

正如你想象的那样，即使应用程序终止时，没有显式地关闭它打开的文件，前面的讨论也是适用的：内核会在进程退出时自动调用close系统调用关闭所有相关的文件。

<h2 id="3.5">3.5 scull内存使用</h2>

在介绍读写操作之前，我们最好先看看scull如何并为何执行内存分配。“如何分配”有助于彻底理解代码，“为何分配”则表明了驱动程序作者需要做出的选择，尽管scull作为一种设备来说，肯定还不具备代表性。

本节仅涉及scull中的内存分配策略，并未涉及编写实际驱动程序所需的硬件管理技能。这些技巧在第9章和第10章中介绍。因此，如果读者对面向内存操作的scull驱动程序的内部工作原理不感兴趣的话，可以跳过本节。

scull使用的内存区域也称为设备，长度可变。写的越多，它就越长；通过用较短的文件重写设备来执行修剪。

scull驱动程序引入了在Linux内核中管理内存的两个主要函数。定义在`<linux/slab.h>`中：

    void *kmalloc(size_t size, int flags);
    void kfree(void *ptr);

对kmalloc的调用会尝试分配size个大小的内存；返回值是指向该内存的指针，如果分配失败，则返回NULL。flags参数用于描述如何分配内存；我们在第8章详细讨论这些标志。现在，我们一直使用GFP_KERNEL。分配的内存应该用kfree释放。我们不应该把非kmalloc返回的指针传递给kfree。但是，将NULL指针传递给kfree是合法的。

kmalloc并不是分配大块内存的最有效方式（参见第8章），所以为scull选择的实现并不是特别巧妙。更巧妙的实现的源代码将更难以阅读，但本节的目标是演示读写，而不是内存管理。这就是为什么虽然按整页分配更高效，而代码只使用了kmalloc和kfree而没有采取分配整个页面的操作的原因。

另一方面，从理论和实际的角度，我们不想限制“设备”区域的大小。从理论角度来看，对被管理的数据项任意施加限制总是一个坏主意。从实际角度来看，为了在较小内存的情况下进行测试，scull可以用来暂时吞噬您的系统内存。运行这些测试可能会帮助你了解系统的内部。可以使用命令`cp /dev/zero /dev/scull0`用光所有系统RAM，也可以使用dd工具选择将多少数据复制到scull设备中。

在scull中，每个设备都是一个指针链表，每个指针指向一个scull_dev结构。默认情况下，每个这样的结构可以通过一组中间指针数组引用至多400万个字节。我们发布的源代码使用了一个有1000个指针的数组，每个指针指向一个4000个字节的区域。我们称每个存储区为一个量子，而指针数组（或其长度）称为量子集。图3-1显示了scull设备及其存储区。

![6-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/SongBaohua/images/6-1.PNG)

这样，选择的参数使得向sucll写入一个字节，就会消耗8000或12000个字节的内存：每个量子占用4000个字节，而一个量子集占用4000到8000个字节（取决于目标平台的指针本身占用32位还是64位）。然而，如果向scull写入大量的数据，链表的开销不会太大。每4M字节对应一个链表元素，而设备的最大尺寸受计算机内存大小的限制。

为量子和量子集合选择适当的值是一个策略问题，而不是机制，最佳大小取决于设备的使用方式。因此，scull驱动程序不应该强制使用任何特定的量子和量子集尺寸数值。在scull中，用户可以通过以下几种方式更改该数值：通过在编译时更改scull.h中的宏SCULL_QUANTUM和SCULL_QSET；在模块加载时设置整数值scull_quantum和scull_qset；或者在运行时，使用ioctl更改当前值和默认值。


使用宏和整数值来允许编译时和加载时进行配置，这种方法和前面选择主设备号的方法类似。 对于驱动程序中任何不确定的或与策略相关的数值我们都可以使用这种技巧。

剩下的唯一问题是如何选择默认数值。在这种特殊情况下，量子和量子集未充分填满会造成内存浪费，而如果量子和量子集过小则会在进行内存分配、释放和指针链接等操作时增加系统开销，默认数值的选择就是在这两者之间找到最佳平衡点。 此外，还应考虑kmalloc的内部设计。（不过我们现在不会去追求这一点；第8章将探讨kmalloc的内部特征）。现在，默认数值的选择基于在测试时可能会写入大量的数据的假设，尽管正常情况下，可能只传输几K字节的数据。

我们已经看到了代表设备内部的scull_dev结构。该结构的quantum和qset字段分别保存设备的量子和量子集大小。但是，实际的数据由另外一个结构处理，对于该结构，我们称之为struct scull_qset：

    struct scull_qset {
        void **data;
        struct scull_qset *next;
    };

下面的代码段说明了如何利用struct scull_dev和struct scull_qset来保存数据。函数scull_trim负责释放整个数据区域，并且在文件以写入方式打开时由scull_open调用。它简单地遍历链表，并释放所有找到的量子和量子集。

    int scull_trim(struct scull_dev *dev)
    {
        struct scull_qset *next, *dptr;
        int qset = dev->qset; /* "dev" is not-null */
        int i;
        for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
            if (dptr->data) {
                for (i = 0; i < qset; i++)
                    kfree(dptr->data[i]);
                kfree(dptr->data);
                dptr->data = NULL;
            }
            next = dptr->next;
            kfree(dptr);
        }
        dev->size = 0;
        dev->quantum = scull_quantum;
        dev->qset = scull_qset;
        dev->data = NULL;
        return 0;
    }

scull_trim也在模块清理函数中用于将scull使用的内存返回给系统。

<h2 id="3.6">3.6 write和read</h2>

read和write方法完成的任务相似，亦即拷贝数据到应用程序空间，或反过来从应用程序空间拷贝数据。因此，它们的原型十分相似，不妨同时介绍它们：

    ssize_t read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
    ssize_t write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);

对于这两个方法，参数flip是文件指针，参数count是请求传输的数据长度。参数buff是指向用户空间的缓冲区，这个缓冲区或者保存要写入的数据，或者是一个存放新读入数据的空缓冲区。最后的offp是一个指向“long offset type”对象的指针，这个对象指明用户在文件中存取操作的位置。返回值是“signed size”类型，后面会谈到它的用法。

需要再次指出的是，buff参数是用户空间的指针。因此，内核代码不能直接应用其中的内容。出现这种限制的原因有如下几个：

* 随着驱动程序所运行的架构不同或者内核配置不同，在内核模式中运行时，用户空间的指针可能是无效的。该地址可能根本无法映射到内核空间，或者可能指向某些随机的数据。

* 即使该指针在内核空间代表相同的东西，但用户空间的内存是分页的，而在调用系统调用时，涉及的内存可能根本就不在RAM中。对用户空间内存的直接引用将导致页错误，而这对内核代码来说是不允许发生的事情。其结果可能就是一个“oop”，它将导致调用该系统调用的进程死亡。

* 我们讨论的指针可能由用户程序提供，而该程序可能存在缺陷或者是恶意程序。如果我们的驱动程序盲目引用用户提供的指针，将导致系统出现打开的后门，从而允许用户空间程序随意访问或覆盖系统中的内存。如果读者不打算因为自己的驱动程序而危及用户系统的安全性，则永远不应直接应用用户空间的指针。

很显然，驱动程序必须访问用户空间的缓冲区以便完成自己的工作。为了确保安全，这种访问应该始终通过内核的专有函数来完成。这里，我们将介绍几个类似的函数（定义在`<asm/uaccess.h>`），其它的函数将在第6章介绍。这些函数使用了一些特殊地与架构相关的方法来确保在内核和用户空间之间安全正确地交换数据。


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3.7">3.7 实例</h2>

一旦准备好了上面的四个方法就可以编译和测试驱动程序了。它保留写入的数据，直到用新数据覆盖。这个设备有点像长度只受物理RAM限制的数据缓冲区。

<h3 id="3.7.1">3.7.1 测试环境</h3>

请参考[Linux设备驱动开发0-环境搭建]()

<h3 id="3.7.2">3.7.2 源代码</h3>

<h3 id="3.7.3">3.7.3 测试过程</h3>


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
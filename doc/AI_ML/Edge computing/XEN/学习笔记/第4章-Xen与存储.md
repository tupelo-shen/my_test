[TOC]

Throughout this book, so far, we’ve talked about Xen mostly as an integrated whole, a complete virtualization solution, to use marketing’s word. The reality is a bit more complex than that. Xen itself is only one component of a platform that aims to free users from having to work with real hardware. The Xen hypervisor virtualizes a processor (along with several other basic components, as outlined in Chapter 2), but it relies on several underlying technologies to provide seamless abstractions of the resources a computer needs. This distinction is clearest in the realm of storage, where Xen has to work closely with a virtualized storage layer to provide the capabilities we expect of a virtual machine.

By that we mean that Xen, combined with appropriate storage mechanisms, provides near total hardware independence. The user can run the Xen machine anywhere, move the instance about almost at will, add storage freely, save the filesystem state cleanly, and remove it easily after it’s done.

Sounds good? Let’s get started.

# 1 Xen存储初认识

在深入Dom0的配置之前，首先应该了解存储，也就是Dom域来说，是怎样感知存储存在的。其实，DomU通过在DomU配置文件中添加一行`disk=`内容查找它的存储设备。看起来像是下面这样：

    disk = [
           'phy:/dev/cleopatra/menas,sda,w',
           'phy:/dev/cleopatra/menas_swap,sdb,w'
    ]

上面定义了两个设备，在DomU中分别表示为`sda`和`sdb`两个设备。使用`phy`前缀表示为物理设备[^1]。当然，还有基于其它存储格式的前缀，比如`file`和`tap`，都是基于文件的存储设备。可以混合使用，我们常常提供一对这样的物理设备：卷和基于文件的只读备份文件镜像。

在本例中，我们使用LVM，管理一个名为`cleopatra`的卷组和一对称为`menas`和`menas_swap`的逻辑卷。

> 注意：习惯上，我们倾向于dom域、它的设备、以及配置文件使用相同的名称。所以，在这儿，逻辑卷`menas`和`menas_swap`都属于dom域`menas`，它的配置文件名称为`/etc/xen/menas`，网络接口也使用相似的名称。

可以使用命令`xl block-list`查看某个域下的存储设备：

    # xl block-list menas
    Vdev BE handle state evt-ch ring-ref BE-path
    2049  0     0     4      6      8      /local/domain/0/backend/vbd/1/2049
    2050  0     0     4      7      9      /local/domain/0/backend/vbd/1/2050

# 2 存储类型

Xen诞生于开源世界，支持多种存储选择，这一点都不稀奇。每种存储都有自己的有缺点和设计哲学。最后，这些存储可以分为两大类：基于文件和基于设备。

Xen可以使用文件作为块设备。这样做的优点是简单、易于移动、可以从主机OS上轻松挂载，并且易于管理。曾经，这种方式也非常缓慢，但随着blktap驱动程序的出现，这个问题几乎消失了。基于文件的块设备在Xen访问它们的方式(基本环回与blktap)和内部格式(AIO、QCOW等)方面有所不同。

Xen也能够对物理设备执行I/O操作。物理设备可以是任何硬件设备，比如硬件RAID[^2]、光纤通道技术[^3]、MD[^4]、网络块设备或LVM，只要内核具有相应的驱动程序即可。因为Xen使用DMA技术，将I/O操作直接映射到客户机操作系统的内存区域内，完成设备驱动和Xen虚拟机之间进行搬运，所以，DomU几乎可以以接近本机的速度访问物理设备。

但是，无论是什么存储设备，对于Xen虚拟域来说，都是一样的。管理程序hypervisor将Xen虚拟块设备（VBD）导出给DomU域，继而传递给客户机OS，客户机OS就可以像传统Unix设备节点那样对其进行映射访问。通常，设备的形式为`hdx`或`sdx`，但是，现在许多发行版使用`xvdx`作为Xen虚拟硬盘。

我们推荐使用`blktap`（一种基于文件的块设备虚拟技术）和LVM作为存储支撑。这两种技术都提供了良好的管理性，可以自由地调整大小和移动，并且还对未来的文件系统提供了某些机制上的支持。blktap易于设置，适合测试；而LVM可扩展，适合产品化。

LVM并不是Xen特有的，在其它的Linux发行版上，比如Red Hat，使用它抽象存储设备，从而更易于管理存储设备。blktap是一种特定于Xen的机制，它使用文件作为块设备，就像传统的块loop设备驱动程序那样。但是，它又比传统的loop机制更好，因为它提高了性能，提供了更通用的文件系统格式，比如`QCOW`。但是，从管理员的角度看，没有什么不同，因为它根本无需关心存储是采用的什么技术，性能如何。

OK，现在让我们深入每种存储技术，一探究竟吧。

# 3 Basic Setup: Files

如果不想使用LVM，Xen通过使用blktap驱动和lib库支持快速有效的基于文件的块设备。

blktap包含一个内核驱动和一个用户空间的daemon。内核驱动直接映射文件包含的块，通过环回方式挂载文件时涉及的许多间接操作。它可以处理用于虚拟块设备的许多文件格式，包括通过`dd`命令获取的raw镜像。

使用`dd`命令创建的镜像文件：

    # dd if=/dev/zero of=/opt/xen/anthony.img bs=1M count=1024

运行结果，如下所示：

    1024+0 records in
    1024+0 records out
    1073741824 bytes (1.1 GB) copied, 15.1442 seconds, 70.9 MB/s

Thus armed with a filesystem image, you can attach it using the tap driver, make a filesystem on it, and mount it as usual with the mount command.

    # xm block-attach 0 tap:aio:/opt/xen/anthony.img /dev/xvda1 w 0
    # mkfs /dev/xvda1
    # mount /dev/xvda1 /mnt/



First, we use the xm(8) command to attach the block device to domain 0. In this case the xm command is followed by the block-attach subcommand, with the arguments <domain id to attach the device to\> <backend device\> <frontend device\> <mode\> and optionally [backend domain id]. To decompose our example, we are attaching anthony.img read/write using the tap:aio driver to /dev/xvda1 in domain 0 using domain 0 to mediate access (because we tend to avoid using non-dom0 driver domains). When the file is attached as /dev/xvda1, we can create a filesystem on it and mount it as with any block device.

    # cp -a /opt/xen/images/centos-4.4/* /mnt/

在DomU配置文件中添加`disk=`行，增加硬盘：

    disk = ['tap:aio:/opt/xen/anthony.img']

Now you should be able to start the domain with its new root device:

    # xm create -c anthony

Watch the console and bask in its soothing glow.

## 3.1 MOUNTING PARTITIONS WITHIN A FILE-BACKED VBD

## 3.2 LVM: Device-Independent Physical Devices

### 3.2.1 QCOW

### 3.2.2 Basic Setup: LVM

# 4 Enlarge Your Disk

## 4.1 File-Backed Images

## 4.2 LVM

# 5 Copy-on-Write and Snapshots

# 6 LVM and Snapshots

## 6.1 THE XEN LIVECD REVISITED: COPY-ON-WRITE IN ACTION

# 7 Storage and Migration

## 7.1  Network Storage

### 7.1.1 NFS

# 8 Closing Suggestions

# 9 脚注

[^1]: 这儿的物理设备并不是严格意义上的独立物理存储设备，而是只要符合块设备的访问要求即可。这个前缀指示Xen将设备看作一个基本的块设备进行处理，而不是像基于文件支持的镜像提供翻译那样处理。
[^2]: RAID代表磁盘冗余阵列。这是一种将多个独立硬盘驱动器虚拟化为一个或多个阵列的方法，在用户租用服务器之后，通常都会做RAID阵列，以此来提高硬盘性能，容量和可靠性。通常RAID可以使用特殊控制器（硬件RAID）或操作系统驱动程序（软件RAID）实现。
[^3]: `Fibre Channel (FC)`是一种高速网络互联技术（通常的运行速率有2Gbps、4Gbps、8Gbps和16Gbps），主要用于连接计算机存储设备。过去，光纤通道大多用于超级计算机，但它也成为企业级存储SAN中的一种常见连接类型。尽管被称为光纤通道，但其信号也能在光纤之外的双绞线上运行。
[^4]: MiniDisc（迷你磁光盘Mini ），它是由日本索尼公司(Sony)于1992年正式批量生产的一种音乐存储介质，一般笼统地称便携式MD机为MD。MD机所采用的音频法有ATRAC/ATRAC 3 (与CD－DA的压缩比分别是1:5和1:10）两种格式，归根结底就是为了把数据塞到小小的MD盘片里去。

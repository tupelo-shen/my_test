这是一篇向初学者介绍Xen相关的基本概念，以及如何在没有经验的情况下启动Xen项目的指导手册。但是，一些Linux的经验还是必要的，比如网络、lvm和grub，这对于Xen的理解和使用大有裨益。另外，还应该熟悉在root权限下的操作方法。很多时候，我们可能需要在root权限下进行操作。

完成本手册，你就拥有了一个全功能的Xen监控程序，并且能够启动第一个客户操作系统。在此基础上，你可以将其接入网络，学习一些其它的基本知识，比如虚拟机存储和虚拟网络。

为了简化本文的过程，直接使用了一个Linux的发行版本，Debian。本文档最初编写的时候，使用了Debian6/7（分别称之为`Wheezy`和`Wheezy`），并且在最新的Debian10（称为`Buster`）上进行了测试。在较新版本上应该也可以运行。Debian对`Xen 4.x`支持较好。

[TOC]

# 1 Xen是什么?

Xen项目是一个虚拟机监控软件，也称为`hypervisor`：是一个允许多个虚拟客户机操作系统运行在同一个物理机上的软件系统。它创建一个类型1的hypervisor监控程序，这意味着它直接运行在物理机上，而不是操作系统之上。

运行在Xen之上的虚拟客户机称为`domain`。domain0或者dom0是一个特殊的控制域，用来管理Xen程序并启动其它客户机操作系统。其它的虚拟机称为domU，它们运行的非特权模式下，不能控制Xen程序或启停其它虚拟机。

Xen监控程序支持两种虚拟化：半虚拟化（PV）和全虚拟化（硬件虚拟化）。半虚拟化需要修改客户操作系统，这些操作系统知道它们正在被虚拟化，因此不需要虚拟硬件设备。相反，它们调用hypervisor的特殊接口，访问CPU，存储设备和网络资源。

相反，全虚拟化（HVM）不需要修改客户机操作系统，因为它会完全模拟一个物理计算机设备。所以，相比半虚拟化，其允许不修改客户机操作系统，但是牺牲了更多的性能。HVM需要CPU支持虚拟化扩展功能，熟知的有英特尔的V-T和AMD公司的AMD-V。

Xen虚拟化实现范围介于半虚拟化和硬件虚拟化之间，在其中还有许多变体，比如带有半虚拟化驱动的硬件虚拟机，或者在硬件虚拟机上实现半虚拟化等。之所以，存在这些变体，都是为了提高它们的性能。为了简化本手册，我们创建一个通用半虚拟化VM和一个可选的硬件虚拟化的客户机。对于其它的变体，可以查看下面这篇文章：

* [Understanding the Virtualization Spectrum](https://wiki.xenproject.org/wiki/Understanding_the_Virtualization_Spectrum)


# 2 Xen架构简介

为了理解存储、网络和其它资源如何被派发到客户系统中，我们需要深入研究一下软件的不同部分是如何交互的。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/Xen_beginner_guide_XenArch1_1.png">

这是Xen的基本架构，可以看出，监控程序直接运行在裸机之上。虚拟机运行在监控程序之上，比如Dom0（控制域）。Dom0有两个不同于其它虚拟机的功能：

1. Dom0有能力调用监控程序，从而启停其它虚拟机。

2. 默认情况下，Dom0包含访问硬件设备的驱动程序。也就是说，Xen监控程序通过使用Dom0中的驱动程序来访问不同的硬件设备。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/Xen_beginner_guide_XenArch2_2.png">

Dom0重要的功能之一就是提供监控程序的接口。通过特殊的指令，Dom0和Xen监控程序进行通信，并配置它。这包含实例化一个新的域和相关任务。

Dom0，另外一个重要的角色就是作为硬件设备的接口。监控程序并不包含设备驱动。相反，设备都依附于Dom0，使用标准的Linux驱动。Dom0与其它虚拟机共享这些资源。

为了实现半虚拟化，每个设备的模拟分为两部分：

1. 存在于Dom0中的`后端`，提供虚拟设备；
2. 存在于其它DomU中的`前端驱动`，允许客户机操作系统访问虚拟设备。

后端和前端驱动通过基于贡献内存的高速软件接口在客户机DomU和控制域Dom0之间传递数据。

典型的半虚拟化设备如网络和存储系统，还有半虚拟化中断、定时器和页表等等。

如果想要了解更多关于Xen项目系统的架构、半虚拟化以及这样做的好处，可以参考下面的文章：

* Details of [Paravirtualization (PV)](https://wiki.xenproject.org/wiki/Paravirtualization_(PV)) and how it is used on Xen Project

对于HVM虚拟机，Dom0使用CPU提供的硬件虚拟化扩展功能。首先，实现CPU自身的虚拟化。然后，是页表管理（MMU）和I/O虚拟化（IOMMU）。另外，Dom0还可以借助QEMU的组件虚拟化一些硬件设备。但是，使用软件进行设备模拟会降低系统性能。

# 3 准备工作

需要满足一下条件：

* 至少1G内存的64位X86计算机

* 支持V-T或AMD-V（对于半虚拟化可选，对于全虚拟化和某些半虚拟化优化来说是必须的）

* 为Dom0和其它客户机提供足够的存储空间

* CD/DVD刻录机和一张CD/DVD（也可以使用USB，可以参考其它资料）

* 下载Debian，并刻录到CD/DVD中

* 具有license的Window或Windows server 2008的ISO镜像

* VNC客户端（PV可选，HVM必须）

> 关于VT和AMD-V：
> 
> 确认主机是否支持虚拟化扩展是很重要的。如果主板不支持，即使CPU使能硬件扩展也没有用。如果想使用HVM实例，那么底层硬件至少支持VT-d和VT-i或AMD-V和AMD-Vi中的一种。话虽如此，最简单的检查方法还是通过BIOS进行查看。

## 3.1 BIOS中使能虚拟化支持

> 对于半虚拟化客户机来说，这不是必须的。但是，开启虚拟化支持后，半虚拟化和全虚拟化你都可以使用了。

如果你的系统不支持硬件虚拟化扩展支持，那么就无法使用Xen监控程序虚拟化未经修改的操作系统。但是，半虚拟化的客户机可以工作。
不同的计算机，BIOS程序可能不一样。但是，一般情况下，基于Intel芯片的计算机会有`Enable Intel VT`字样，ADM芯片会有`Enable AMD-V`字样等。这些选项一般是在BIOS程序的`Advanced Chipset Features`菜单下。有时候，默认是开启的。

## 3.2 下载并刻录Debian安装光盘

你可以在下面的地址找到最新的Debian的ISO镜像:

[http://cdimage.debian.org/debian-cd/current/amd64/iso-cd/](http://cdimage.debian.org/debian-cd/current/amd64/iso-cd/)

对于我们来说，`netinst`版本镜像就足够了。

使用工具将ISO刻录到光盘上。Linux有wodim，windows有自带的ISO刻录功能。

## 3.3 Debian简要介绍

关于Debian的介绍可以参考其它资料。这儿只提一句：从`Debian3.1`版本开始，以及包含对Xen的支持。

# 4 安装Debian

插入Debian光盘，在BIOS中，配置CDROM驱动器作为你默认的引导设备。

你应该会看到一个默认的`Install`选项，点击开始安装。系统的安装比较简单，特别需要注意的是，硬盘分区部分。

选择自定义选项，我们自己配置几个分区：一个用于boot过程，另一个用于swap分区；最后一个分区设置为客户机的LVM逻辑分区。

1. 创建`/boot`分区

    选择磁盘，点击Enter，设置分区大小为300MB，并格式化为ext2，选择boot作为挂载点。

2. 创建`/`分区

    选择`/`作为挂载点，大小至少为15GB以上，格式化为ext3。

3. 创建`/swap`分区

    设置大小为RAM的1.5倍，选择它作为交换区。

4. 创建保留分区

    该分区是为客户机的LVM逻辑分区保留的。大小根据后面可能的客户机预留。但是不要格式化，也不要指定挂载点。

分区创建完成后，大概的布局如下：

    sda1 - /boot 300MB
    sda2 - / 15GB
    sda3 - swap
    sda4 - reserved for LVM

只安装基本的系统，不要GUI或者其它软件。（如果想要在Dom0中设置图形化桌面环境，这不是问题。但是本手册为了简单起见，尽量缩减了功能）。

您可以从Debian文档中找到Debian安装过程的详细信息。

继续执行安装程序，然后重新启动，并在提示符下以root用户身份登录。

# 5 安装Xen

Debian下的Xen项目包含：支持Xen的Linux内核，监控程序本身，支持监控程序HVM模式的修改版QEMU，和一组用户工具。所有这些都包含在一个称为`xen-linux-system`的元软件包，使用APT软件包管理方式进行安装。

安装`xen-linux-system`元软件包：

    apt-get install xen-system-amd64

现在，我们已经有了Xen监控程序，内核和用户工具。当你再次启动系统的时候，启动菜单中会有启动带有Xen监控程序的Debian选项（默认）。再次以root身份重启。

接下来，检查BIOS中是否将虚拟化功能打开。有多种方式：

最简单的方式就是查看启动过程的dmesg信息中Xen相关内容。至于`xl`命令后面会再讲：

    xl dmesg

输出中会包含对BIOS中设置的启动虚拟化的相关标志：`vmx`之于Intel，`svm`之于AMD。还会显示一些其它关于虚拟化扩展的内容。

另外一种方法，查看cpu相关信息：

    egrep '(vmx|svm|hypervisor)' /proc/cpuinfo

或者全部打印出来，自己查看相关信息：

    cat /proc/cpuinfo

至于`egrep`命令，请自行查阅相关shell书籍。

# 6 为虚拟机建立LVM存储

## 6.1 LVM概念

LVM是Linux逻辑卷管理器，允许Linux以更为抽象的方法管理块设备。 LVM引入了`逻辑卷`的概念，有效地允许一个虚拟的块设备（具有多个块）可以写入到一个或者多个物理设备中。与真正的硬盘分区不同，这些块不需要是连续的。这种抽象的逻辑卷可以在不影响其它逻辑卷的情况下被创建、删除、调整大小、甚至是快照功能。

LVM在所谓的卷组中创建逻辑卷，卷组只是共享相同物理存储的一组逻辑卷，称为物理卷。创建LVM的过程概括起来就是，申请分配一个物理卷，在此之上创建卷组，然后创建逻辑卷存储数据。

> <font color="blue"> 
> 总结：
> 
> LVM其实就是屏蔽硬件块设备的差异，对于上层的应用来说，就是一个可以自由使用的虚拟硬件设备。
> 
> </font>

由于其特性和由于文件支持的虚拟机，如果想要存储虚拟机数据，我们推荐使用LVM。

## 6.2 安装LVM

安装LVM：

    apt-get install lvm2

配置LVM使用`/dev/sda4`作为物理卷：

    pvcreate /dev/sda4

OK，现在让我们使用这个物理卷创建一个卷组`vg0`：

    vgcreate vg0 /dev/sda4

现在，LVM已经建立并被初始化。后面我们可以使用它，为我们的虚拟机创建逻辑卷。

> 下面是一些使用LVM的有用命令和小技巧：
>
> 1. 创建一个新的逻辑卷：
>
>       lvcreate -n [逻辑卷名称] -L [大小(可以使用G和M表示大小)]  [卷组]
>
> 比如，在卷组`vg0`上创建一个称为`database-data`的100G大小的卷：
>
>       lvcreate -n database-data -L 100G vg0
>
> 2. 移除逻辑卷：
>
>       lvremove /dev/vg0/database-data     # 注意必须使用正确路径
>
> 更多关于Debian上使用LVM的方法，请参考 [More on LVM on Debian here.](https://wiki.debian.org/LVM#List_of_VG_commands)
>
> 如果你已经有一个逻辑卷了，想要对其进行拷贝，LVM有一个很酷的功能，允许`写时拷贝-COW`克隆，称为`快照`功能。这意味你可以创建一个即时副本，它将只存储与原始卷相比作出的更改部分。但是，有一些注意事项，我们将在其它文章中讨论。还有一个值得注意的事情就是，`快照`的大小只是存储变化内容的空间大小，所以，快照的大小比原始卷的要小很多。
>
> 创建快照，使用下面的命令：
>
>       lvcreate -s /dev/vg0/database-data -ndatabase-backup -L5G
>
> 注意，使用全路径。

# 7 为虚拟机设置Linux网桥

现在需要为虚拟机配置网络。这可以在Dom0中创建一个虚拟交换机实现。该交换机从虚拟机接收数据包，转发给物理网络设备。

这可以使用Linux网桥设备实现，它的核心组件存在于Linux内核中。这时候，网桥的角色就是我们的虚拟交换机。Debian内核支持Linux网桥模块，所以，我们只需要安装控制管理工具即可：

    apt-get install bridge-utils

网桥的管理可以使用`brctl`命令。但是，我们还是使用下面的方法进行网络的配置。

打开文件。（如果没有nano，可以安装；也可以使用其它编辑器）

    nano /etc/network/interfaces

你可以看到类似下面的内容：

    auto lo
    iface lo inet loopback

    auto eth0
    iface eth0 inet dhcp

`auto eth0`的意思是，如果运行`ifup -a`，就会配置eth0（这发生在系统启动阶段）。也就是说，系统自动启停这个网络接口，不需要咱们干预。eth0是一个常见名称，也可能是`ens1`、`enp0s3`之类的。

`iface eth0`就是描述接口本身的属性。在本例中，配置使用DHCP方式。也就是使用动态IP的方式。

修改如下：

    auto lo
    iface lo inet loopback

    auto eth0
    iface eth0 inet manual
    
    auto xenbr0
    iface xenbr0 inet dhcp
         bridge_ports eth0

上面的设置是，将IP指定给网桥进行处理，而不是网络接口。网络接口只提供物理和数据链路层的功能。

重启网络。

    service networking restart

检查是否工作：

    brctl show

如果可以，网桥会被列出，网络接口也会出现：

    bridge name     bridge id               STP enabled     interfaces
    xenbr0          8000.4ccc6ad1847d       no              enp2s0

至此，桥接网络在启动时会自动启动。

# 8 配置GRUB启动Xen 

GRUB，是一个引导程序。为了使用监控程序Xen，必须在操作系统之前启动Xen。根据你的使用情况，可以选择是否默认加载Xen。

GRUB2的配置位于`/boot/grub/grub.cfg`文件中。

我们不会直接修改这个文件，因为每当我们更新内核的时候，它都会发生变化。Debian为了方便，提供了许多自动更新脚本，位于`/etc/grub.d/*`目录下，可以通过下面的文件进行配置：

    /etc/default/grub

为了改变默认操作系统，将下面的值改为0（如果没有改行，请在文件中添加）：

    GRUB_DEFAULT=0

上面的代码是将列表中的第一个操作系统作为默认值，以此类推。如果Xen是第三种操作系统，则将其改为：

    GRUB_DEFAULT=2

默认情况下就会加载Xen。

运行下面的命令，重新产生`/boot/grub/grub.cfg`文件：

    update-grub

下一次重启的时候，看是否选择了正确的操作系统。

# 9 基本的Xen命令

俗话说，"磨刀不误砍柴工"。下面我们先来熟悉几个`xl`命令行工具（旧版本的Xen软件使用`xm`命令行工具）。其实，`xl`和`xm`命令行格式差不多（可能输出格式会有轻微的不同）。如果在阅读旧文档的时候，碰见`xm`，替换为`xl`即可。

让我们操练起来吧！😊

    xl info

无非就是返回Xen监控程序、Dom0以及可以使用的内存等信息。

    xl list

列出运行的DomX，它们的ID，内存，状态以及占用的CPU运行时间。

    xl top

实时显示正在运行的Dom域，与Linux下的`top`命令有点类似。这可以用来查看CPU，内存使用率以及块设备的访问情况。后面创建虚拟机的过程中，还会继续熟悉其它命令。

想要查看更多的命令内容，请参考 [Xen Project 4.x Man Pages](https://wiki.xenproject.org/wiki/Xen_4.x_Manuals)。

# 10 创建一个半虚拟化的Debian虚拟机

半虚拟化的客户机安装肯定不同。幸运的是，有许多工具帮助我们准备可以运行在客户域中运行的操作系统镜像。

Debian下有许多创建Xen客户机的辅助工具，最简单的就是`xen-tools`。这个工具管理客户操作系统的下载和安装，包括基于Debian和RHEL的DomU。在本指南中，我们将使用`xen-tools`准备一个Debian半虚拟化的DomU。

`xen-tools`可以使用LVM存储客户操作系统。在前面，我们已经创建卷组`vg0`。

客户操作系统半虚拟化的意思就是，没有类似于`BIOS`或`bootloader`存在于客户机的文件系统中。但是，这不利于可维护性（如果不能访问Dom0，客户机无法更新它们的内核），而且在引导方面，也不如使用配置文件传递那样灵活方便。

Xen社区写了一个工具，称为`pygrub`（使用python编写），辅助半虚拟化客户机使能Dom0，去解析DomU的grub配置，并提取其内核、initrd和引导参数。这允许在我们的客户机内部实现使用GRUB菜单更新内核。使用`pygrub`或称为`PV -grub`的`stub-dom`实现是启动PV客户机的最佳实践。在某些情况下，`pv-grub`可能更安全，但由于Debian中没有包含它，所以我们在这里不使用它，但建议在不信任客户机的生产环境中使用它。

除此之外，PV客户机与对应的HVM和物理OS非常相似。

## 10.1 配置`xen-tools`并构建客户机

首先，安装`xen-tools`软件包：

    apt-get install xen-tools

现在，我们可以创建自己的客户操作系统了。下面是从头建立一个PV客户机，到创建配置文件并启动客户机的过程：

1. 为`rootfs`创建逻辑卷

2. 为`swap`创建逻辑卷

3. 为`rootfs`创建文件系统

4. 挂载`rootfs`

5. 使用`debootstrap`安装操作系统（或者`rinse`）

6. 运行一系列脚本，产生客户机配置文件，比如`fstab/inittab/menu.lst`

7. 为客户机创建VM配置文件

8. 为客户机系统产生一个root密码

9. 卸载客户机文件系统

这9步可以手动执行，但是本指南不讲手动实现。我们将使用下面的命令自动创建（对于`--dist`选项，可以使用其它，比如`Squeeze`、甚至是Ubuntu的`Precise`或`Quantal`进行替代）。

    xen-create-image --hostname=tutorial-pv-guest \
    --memory=512mb \
    --vcpus=2 \
    --lvm=vg0 \
    --dhcp \
    --pygrub \
    --dist=jessie
    --mirror=https://mirrors.tuna.tsinghua.edu.cn/debian/

> 因为我在安装的时候选择了清华的镜像服务器，所以需要在上面修改mirror路径。默认使用`http://deb.debian.org/debian/`。

这个命令指示`xen-create-image`（`xen-tools`工具链中的主要二进制文件）创建一个具有512M内存、2个vcpu、使用`vg0`逻辑卷、网络使用DHCP、在启动过程中使用`pygrub`从镜像中抽取内核、最后指定我们想要部署一个Debian的`Wheezy`操作系统。

这个过程会花费几分钟。一旦完成，会显示一些安装的概要信息。请记住客户机的root密码<root_password: MQ2jKJqxBMfZvJ7ZLzBXgQe>。

更多内容请参考下面这两篇文章：

1. [xen-create-image(8) man page](http://manpages.debian.org/cgi-bin/man.cgi?query=xen-create-image&apropos=0&sektion=0&manpath=Debian+6.0+squeeze&format=html&locale=en)
2. [Further articles on xen-tools](https://wiki.xenproject.org/wiki/Xen-tools)

# 11 手动创建一个PV客户机

并不是每个发行版都提供用于自动创建和配置PV客户机的`xen-tools`软件包，并且有时候，我们可能需要对客户机的建立过程有更多的控制。

`Alpine Linux`就是这样的一个发行版本。具体参考[on installing and starting a PV domU manually](http://wiki.alpinelinux.org/wiki/Create_Alpine_Linux_PV_DomU)。其中，它使用的是`PVGRUB2`而不是`pygrub`。

# 12 启动客户机（控制台）

运行下面的命令启动客户机：

    xl create -c /etc/xen/tutorial-pv-guest.cfg

`-c`选项是告诉`xl`，我们想要连接客户机虚拟控制台，是客户域中的一个半虚拟化串口端口，`xen-create-image`配置为使用getty侦听。与下面的命令效果一样。

    xl create /etc/xen/tutorial-pv-guest.cfg && xl console tutorial-pv-guest

离开客户机虚拟控制台的命令是`ctrl+]`，重新进入的命令是`xl console <domain>`。

关闭客户机的命令，如下（从DomU或者Dom0都可以）：

    xl shutdown tutorial-pv-guest

至此，我们建立了第一个半虚拟化的Dom域。如果你对建立一个全虚拟化Dom域不感兴趣，请跳过下面的一章，直接阅读[14. 启动一个GUI客户机](#14)

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/Xen_beginner_guide_3.png">

# 13 创建一个Windows全虚拟化客户机

HVM guests are quite a bit different to their PV counterparts. Because they require the emulation of hardware there are more moving pieces that need to be configured etc.

The main point worth mentioning here is that HVM requires the emulation of ATA, Ethernet and other devices, while virtualized CPU and Memory access is performed in hardware to achieve good performance. Because of this the default emulated devices are very slow and we generally try to use PV drivers within HVM domains. We will be installing a set of Windows PV drivers that greatly increase performance once we have our Windows guest running.

This extra emulation is provided by QEMU which will have been installed along with the Xen software.

To set up the HVM domU, we need to create a logical volume to store our Windows VM hard disk, create a config file that tells the hypervisor to start the domain in HVM mode and boot from the DVD in order to install Windows.

First, create the new logical volume - name the volume "windows", set the size to 20GB and use the volume group vg0 we created earlier.

    lvcreate -nwindows -L20G vg0

Next open a new file with your text editor of choice:

    nano windows.cfg

Paste the config below into the file and save it, NOTE this assumes your Windows iso is located in /root/ with the filename windows.iso. In the kernel = line below, be sure the xen version number matches your installation (e.g. xen-4.11, not 4.0):

    kernel = "/usr/lib/xen-4.0/boot/hvmloader"
    type='hvm'
    memory = 4096
    vcpus=4
    name = "ovm-1734"
    vif = ['bridge=xenbr0']
    disk = ['phy:/dev/vg0/windows,hda,w','file:/root/windows.iso,hdc:cdrom,r']
    acpi = 1
    device_model_version = 'qemu-xen'
    boot="d"
    sdl=0
    serial='pty'
    vnc=1
    vnclisten=""
    vncpasswd=""

The vnclisten= line specifies valid VNC connection addresses. vnclisten="127.0.0.1" will limit connections to the local machine. vnclisten="0.0.0.0" will accept unauthenticated remote connections from anywhere so is not suitable except in a secure network.

Start the guest as described below in Starting a GUI guest and proceed with Windows' installation.

Once you have installed Windows by formatting the disk and by following the prompts the domain will restart - however this time we want to prevent it booting from DVD so destroy the domain with

    xl destroy windows

Then change the boot line in the config file to read boot="c"' restart the domain with

    xl create windows.cfg

Reconnect with VNC and finish the installation. When this process is complete you should then proceed to download the GPLPV drivers for Windows by James Harper.

## 13.1 Installing PV drivers for HVM guests

Signed drivers can be obtained from [Univention's website](http://wiki.univention.de/index.php?title=Installing-signed-GPLPV-drivers).

Many thanks for Univention for making signed drivers available to the Xen Project community and of course a massive thanks to James for all his work on making Windows in guest VMs such a smooth experience.

On finalizing the installation and rebooting you should notice much improved disk and network performance and the hypervisor will now be able to gracefully shutdown your Windows domains.

Another slightly different version of James Harper's drivers can be found [here](https://github.com/spurious/win-pvdrivers-mirror).

# 14 启动GUI客户机（使用VNC服务器） 

<div id="14"></div>

Here is the command to start the domain and connect to it via VNC from your graphical machine.

    xl create windows.cfg

The VNC display should be available on port 5900 of your dom0 IP, for instance using gvncviewer:

    gvncviewer <dom0-ip-address>:5900

If this does not work try it without the port number and if you are trying from a GUI on dom0, try specifying localhost instead of the dom0 ip:

    gvncviewer localhost

# 15 Conclusion

That concludes our introduction to the Xen Project software, by now you can setup both PV and HVM domains on a bare dom0 hypervisor!

You can now move onto building your own guest images or try out some prebuilt [Guest VM Images](https://wiki.xenproject.org/wiki/Guest_VM_Images).
[TOC]

# 1 文档范围

本文档主要讲述搭建一个基本的CentOS/x86_64系统，在其上运行完整的Xen4软件。然后，使用Xen虚拟技术启动第一个虚拟机。通过本文，对Xen的使用有一个基本的认知，为后续虚拟化的研究打下基础。

# 2 背景知识

> 关于`Xen-4.4`和`libxl`
> 
> `Xen-4.4`之前的版本，默认使用`xm`和`xend`工具。`xen-4.4.1`（或者更新的版本）rpm包中使用`xl`代替，不再使用`xend`。详情可以参考[/MigratingToXl](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/MigratingToXl)。

本文假设你已经具备了相当的虚拟化知识，知道半虚拟化（PV）和全虚拟化（hvm）之间的差异，对Linux如何使用网络有一个基本的认识，包括bridge工具等。

* Read the Release Notes at http://wiki.centos.org/Manuals/ReleaseNotes/Xen4-01 ; they cover important notes, known issues, workarounds and details on where to get help, should you need it.
* The machine hosting the Xen hypervisor should be a CentOS-6/x86_64 minimal install; before going any further, you should ensure that only the CentOS repos are enabled for yum, and that you have the latest updates applied. The Xen4 stack for CentOS has only been tested and verified on CentOS-6.4 and newer.

* 假设机器具有至少`1.5GB`的RAM，超过10GB的硬盘空间，留给挂载到root目录`/`节点下的文件系统。

# 3 安装Xen4软件包

Xen软件包存储在CentOS镜像网站对应的数据库中。确保`CentOS-Extras repo`使能，键入如下命令：

    yum install centos-release-xen

Xen需要一个运行在Dom0的内核来支持工作。Xen4已经提供了一个更新后的内核，所以需要运行更新，pull最新的内核：

    yum update

然后，安装Xen：

    yum install xen

Xen和内核安装期间，一个名为`grub-bootxen.sh`的脚本会自动运行，保证grub更新到对应的文件中（CentOS 6：`/boot/grub/grub.conf`；CentOS 7：`/boot/grub2/grub.cfg`）。对于CentOS 6，`/boot/grub/grub.conf`应该是下面这个样子：

    title CentOS (3.4.46-8.el6.centos.alt.x86_64)
            root (hd0,0)
            kernel /xen.gz dom0_mem=1024M,max:1024M loglvl=all guest_loglvl=all
            module /vmlinuz-3.4.46-8.el6.centos.alt.x86_64 ro root=/dev/mapper/vg_xen01-lv_root rd_LVM_LV=vg_xen01/lv_swap rd_NO_LUKS  KEYBOARDTYPE=pc KEYTABLE=uk rd_NO_MD LANG=en_GB rd_LVM_LV=vg_xen01/lv_root SYSFONT=latarcyrheb-sun16 crashkernel=auto rd_NO_DM rhgb quiet
            module /initramfs-3.4.46-8.el6.centos.alt.x86_64.img

> 注意：CentOS 6的grub配置脚本中存在一个bug，缺少最后一行（initramfs行）。如果没有，自己手动添加上。也就是第2行的Module，保证信息的正确性。

运行`reboot`重启，使用下面的命令查看是否运行的对应版本内核：

    uname -r

验证Xen是否运行：

    xl info

> 注意：这里可能有一个bug，参考[Bug 6893](http://bugs.centos.org/view.php?id=6893)。设置`dom0_mem=1024M,max:1536M`会消除这个错误信息。

# 4 建立桥接网络

The standard method of giving your guests networking is to set up bridging; the default bridge that the toolstack expects is called xenbr0.

Basic how-to can be found in [/Xen4Networking6](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Networking6) or [/Xen4Networking7](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Networking7).

Full documentation for setting up and configuring bridges can be found in the [RHEL 6 documentation](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Deployment_Guide/s2-networkscripts-interfaces_network-bridge.html) or [RHEL 7 documentation](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/ch-Configure_Network_Bridging.html).

# 5 启动第一个虚拟机

At this point you are now ready to bring up your first VM, and there are multiple ways of achieving this. For new users, who are looking for the easiest install path, the Libvirt process is recommended. Advanced users looking to hand setup the network, the backing filestore and the xen environment will most likely want to use the xen command line tools.

* With Xen CLI tools, including xl: [/Xen4Cli](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Cli)
* Using !LibVirt/ Virt-install and Virt-Manager with xen4centos: [/Xen4Libvirt](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Libvirt)



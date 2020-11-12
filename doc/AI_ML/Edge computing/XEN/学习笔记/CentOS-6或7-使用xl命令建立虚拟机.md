[TOC]

本文假设已经安装完成Xen，且建立好了桥接网络，默认的桥接网卡名称为`xenbr0`。[TODO]

本文提供了一种基于xl命令行工具创建、管理虚拟机的方法（xm不再支持）。另外一种基于libvirt建立虚拟机的方法请参考[TODO]。

下面介绍使用xl工具，在一个运行CentOS 6的Dom0上，分别创建一个运行`CentOS 6`和`CentOS 7`的DomU域。

# 1 获取Xen信息-xl命令

使用xl命令获取一些关于Xen地安装信息，如下所示：

    [root@c6-xen-dom0 ~]# xl info

    host                   : c6-xen-dom0
    release                : 4.9.13-22.el6.x86_64
    ....(省略)
    cc_compiler            : gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-17)
    cc_compile_by          : mockbuild
    cc_compile_domain      : centos.org
    cc_compile_date        : Tue Feb 28 14:18:26 UTC 2017
    xend_config_format     : 4

在本示例中，我们假定网桥名为`xenbr0`。所有的虚拟机都是用这个网桥。下面是Dom0下设置的网卡配置文件：

    [root@c6-xen-dom0 ~]# cat /etc/sysconfig/network-scripts/ifcfg-eth0 
    DEVICE="eth0"
    NM_CONTROLLED="no"
    ONBOOT="yes"
    TYPE="Ethernet"
    BRIDGE="xenbr0"
    IPV6INIT=no


    [root@c6-xen-dom0 ~]# cat /etc/sysconfig/network-scripts/ifcfg-xenbr0 
    DEVICE="xenbr0"
    BOOTPROTO=none
    NM_CONTROLLED="no"
    ONBOOT="yes"
    TYPE="Bridge"
    IPADDR=192.168.0.9
    PREFIX=24
    GATEWAY=192.168.0.1
    DNS1=8.8.8.8
    DNS2=8.8.4.4
    DEFROUTE=yes
    IPV6INIT=no

使用`ifconfig -a`查看网卡及IP相关信息（IPv4）：

    [root@c6-xen-dom0 ~]# ifconfig -a
    eth0      Link encap:Ethernet  HWaddr D0:50:99:62:6C:43  
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:2206 errors:0 dropped:0 overruns:0 frame:0
              TX packets:714 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:1000 
              RX bytes:208511 (203.6 KiB)  TX bytes:558027 (544.9 KiB)

    lo        Link encap:Local Loopback  
              inet addr:127.0.0.1  Mask:255.0.0.0
              UP LOOPBACK RUNNING  MTU:65536  Metric:1
              RX packets:0 errors:0 dropped:0 overruns:0 frame:0
              TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:1 
              RX bytes:0 (0.0 b)  TX bytes:0 (0.0 b)

    xenbr0    Link encap:Ethernet  HWaddr D0:50:99:62:6C:43  
              inet addr:192.168.0.9  Bcast:192.168.0.255  Mask:255.255.255.0
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:1515 errors:0 dropped:0 overruns:0 frame:0
              TX packets:716 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:1000 
              RX bytes:102780 (100.3 KiB)  TX bytes:558321 (545.2 KiB)

# 2 为虚拟机建立存储设备

我们必须为我们的DomU虚拟机建立存储设备。可以创建镜像文件，也可以基于LVM创建逻辑卷（LV-本文采用这个方法）。

我个人喜欢使用LV，是因为很容易从Dom0上挂载并查看它们；也很容易进行扩展。

默认情况下，CentOS 6和7将为/home目录创建一个很大的LV卷。如果没有很大的硬盘空间，可以挂载一个硬盘，为lv创建一个新的物理卷和一个新的卷组。具体方法可以参考（[LVM管理](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Logical_Volume_Manager_Administration/index.html)）。

本文中，使用安装CentOS 6的过程中，默认安装的卷组（VG）。使用命令`vgdisplay`可以查看相关信息：

    [root@c6-xen-dom0 ~]# vgdisplay
      --- Volume group ---
      VG Name               vg_c6xendom0
      System ID             
      Format                lvm2
      Metadata Areas        1
      Metadata Sequence No  4
      VG Access             read/write
      VG Status             resizable
      MAX LV                0
      Cur LV                3
      Open LV               3
      Max PV                0
      Cur PV                1
      Act PV                1
      VG Size               1.82 TiB
      PE Size               4.00 MiB
      Total PE              476806
      Alloc PE / Size       102006 / 398.46 GiB
      Free  PE / Size       374800 / 1.43 TiB
      VG UUID               vqWRX0-SZOy-pxRx-R3oN-O61F-vt1B-xLdgjT

> CentOS 7会提示`-bash: pvdisplay: command not found`错误，运行命令`yum install lvm2`安装相关工具命令。

通过上面的信息，可以看出，默认创建了一个1.43T大小的VG，名称为`vg_c6xendom0`。

使用`lvcreate`命令创建两个`20GB`的逻辑卷LV：

    [root@c6-xen-dom0 ~]# lvcreate -L 20G -n c6-x8664-hvm vg_c6xendom0
      Logical volume "c6-x8664-hvm" created.

    [root@c6-xen-dom0 ~]# lvcreate -L 20G -n c7-x8664-hvm vg_c6xendom0
      Logical volume "c7-x8664-hvm" created

使用`lvdisplay`命令查看逻辑卷创建结果：

    [root@c6-xen-dom0 ~]# lvdisplay

    <other non xen stuff>

      --- Logical volume ---
      LV Path                /dev/vg_c6xendom0/c6-x8664-hvm
      LV Name                c6-x8664-hvm
      VG Name                vg_c6xendom0
      LV UUID                ojMKYF-UvDC-lq7i-C0HO-G1I2-qo5o-QocEHU
      LV Write Access        read/write
      LV Creation host, time c6-xen-dom0, 2017-03-07 23:50:31 -0600
      LV Status              available
      # open                 0
      LV Size                20.00 GiB
      Current LE             5120
      Segments               1
      Allocation             inherit
      Read ahead sectors     auto
      - currently set to     4096
      Block device           253:3
       
      --- Logical volume ---
      LV Path                /dev/vg_c6xendom0/c7-x8664-hvm
      LV Name                c7-x8664-hvm
      VG Name                vg_c6xendom0
      LV UUID                YJEQVD-t1K9-mM2a-XyKW-2M5G-EreW-1dgIUL
      LV Write Access        read/write
      LV Creation host, time c6-xen-dom0, 2017-03-07 23:54:38 -0600
      LV Status              available
      # open                 0
      LV Size                20.00 GiB
      Current LE             5120
      Segments               1
      Allocation             inherit
      Read ahead sectors     auto
      - currently set to     1024
      Block device           253:4

我们打算从ISO镜像中加载CentOS。所以，在Dom0上创建一个目录`/opt/isos/`，用来保存下载的CentOS 6和7的镜像。下载地址：[`CentOS 6`](http://isoredirect.centos.org/centos/6/isos/x86_64/)和[`CentOS 7`](http://isoredirect.centos.org/centos/7/isos/x86_64/)镜像。

    wget http://mirrors.aliyun.com/centos/6.10/isos/x86_64/CentOS-6.10-x86_64-minimal.iso
    wget https://mirrors.aliyun.com/centos/7.8.2003/isos/x86_64/CentOS-7-x86_64-Minimal-2003.iso

# 3 Xen虚拟机类型

主要有两种虚拟机类型：半虚拟化（PV）和全虚拟化（HVM）。也可能混合使用。在本示例中，我们将创建一个HVM虚拟机，因为它是最简单的类型。

# 4 使用xl工具

`xl`的官方文档请参考：[xl(1)](http://xenbits.xen.org/docs/4.6-testing/man/xl.1.html).

我们将分别创建运行CentOS-6和CentOS-7系统的HVM虚拟机。

首先，我们需要为每个虚拟机建立配置文件。可以参考xen自带的配置文件示例`/etc/xen/xlexample.hvm`。你也可以参考[xl.cfg(5)](http://xenbits.xen.org/docs/4.6-testing/man/xl.cfg.5.html#Fully-virtualised-HVM-Guest-Specific-Options)。

在`/etc/xen/`目录下创建名为`config.d`的目录，在这儿，保存自定义的配置文件。

* 首先，创建`/etc/xen/config.d/c6-x8664.hvm.cfg`

        builder = "hvm"
        name = "c6-x8664.hvm"
        memory = 4096
        vcpus = 2
        # serial='pty'
        vif = [ 'mac=00:16:3E:29:00:00,bridge=xenbr0' ]
        disk = [ 'phy:/dev/vg_c6xendom0/c68-x8664-hvm,xvda,rw', 'file:/opt/isos/CentOS-6.8-x86_64-minimal.iso,xvdb:cdrom,r' ]
        boot = "dc"
        sdl = 0
        vnc = 1
        vnclisten  = "192.168.0.9"
        vncdisplay = 0
        vncpasswd  = "supersecret"
        stdvga=1
        videoram = 64

* 其次，创建`/etc/xen/config.d/c7-x8664.hvm.cfg`

        builder = "hvm"
        name = "c7-x8664.hvm"
        memory = 4096
        vcpus = 2
        serial='pty'
        vif = [ 'mac=00:16:3E:29:00:01,bridge=xenbr0' ]
        disk = [ 'phy:/dev/vg_c6xendom0/c73-x8664-hvm,xvda,rw', 'file:/opt/isos/CentOS-7-x86_64-Minimal-1611.iso,xvdb:cdrom,r' ]
        boot = "dc"
        sdl = 0
        vnc = 1
        vnclisten  = "192.168.0.9"
        vncdisplay = 1
        vncpasswd  = "supersecret"
        stdvga=1
        videoram = 64

每一项的意义可以参考[xl.cfg(5)](http://xenbits.xen.org/docs/4.6-testing/man/xl.cfg.5.html#Fully-virtualised-HVM-Guest-Specific-Options)，这儿，只对重要的项进行说明：

* vnclisten的IP地址指的是网桥的地址，比如`192.168.0.9`；

* boot选项，分别对应`floppy(a)`、`hard disk (c)`、`Network (n)`或`CD-ROM (d)`。所以，`dc`代表先从CD，其次从硬盘。一旦完成后，我们修改它，`c`，让其只从CD启动。

* 我们将使用`vnc`客户端远程连接到`192.168.0.9:5900`(centos-6)和`192.168.0.9:5901 (centos-7)`。

We will just do a normal CentOS install of each version from the ISO.


# 5 xl create

The command to start the CentOS-6 VM is:

    xl create /etc/xen/config.d/c6-x8664.hvm.cfg

Connect to the 5900 port on 192.168.0.9 and do a normal install. I use this command to connect from a CentOS-7 workstation to do the install:

    remote-viewer vnc://192.168.0.9:5900

(There are many other VNC clients out there, you can connect from a Windows or Mac machine, etc.)

You can start the CentOS-7 install with this command:

    xl create /etc/xen/config.d/c7-x8664.hvm.cfg

You would connect to that install via:

    remote-viewer vnc://192.168.0.9:5901

Once the installs are complete, you would edit the /etc/xen/config.d/c6-x8664.hvm.cfg (and c7-x8664.hvm.cfg) and change boot to boot="c", to boot from the disk and not the CD.

# 6 半虚拟化

## 6.1 PV-on-HVM (PVHVM)

PV-on-HVM is discussed here in detail ([LINK](https://wiki.xen.org/wiki/PV_on_HVM)), and actual examples here ([LINK](https://wiki.xen.org/wiki/Xen_Linux_PV_on_HVM_drivers)). Basically, all you need to do is to add xen_platform_pci=1 to your HVM config files above to use PVHVM.

## 6.2 半虚拟化(PV)

默认情况下，CentOS-6和CentOS-7内核不再支持半虚拟化。您可以在DomU中使用Dom0内核，并在安装后通过修改配置文件来打开PV。

如果想要尝试半虚拟化，下面是一些注意事项：

1. You can NOT use the default install as the boot partition in Xen PV machines can not be the XFS file system .. the default used by CentOS. Manually setup a /boot partition as ext4 instead in the CentOS installer.

2. Do not use an LVM partition in the CentOS installer for that Boot Partition .. make it a normal partition instead.

3. If you have done those things, you can create a PV configuration file and boot the installed partition in PV Mode. Here is an example for the c7-x8664.hvm DomU above, named /etc/xen/config.d/c7-x8664.pv.cfg

        bootloader = "/usr/lib/xen/bin/pygrub"
        name = "c7-x8664.pv"
        memory = 4096
        vcpus = 2
        vif = [ 'mac=00:16:3E:29:00:01,bridge=xenbr0' ]
        disk = [ 'phy:/dev/vg_c6xendom0/c73-x8664-hvm,xvda,rw' ]
        vfb = [ 'type=vnc,vncdisplay=1,vncpasswd=supersecret' ]

If you have followed all the rules (the xen dom0 kernel replacing the CentOS-7 installed kernel, no xfs file system on /boot, no LVM on /boot, etc.) .. then that machine will boot in PV mode. Obviously, since the DISK in this case is shared, you can run the HVM and PV instances at the SAME time. You can run either of them individually though.

To start your PV DomU, use this command:

    xl create /etc/xen/config.d/c7-x8664.pv.cfg




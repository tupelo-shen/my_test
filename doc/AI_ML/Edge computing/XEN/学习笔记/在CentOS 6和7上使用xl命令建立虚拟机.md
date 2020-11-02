[TOC]

# 1 Xen基本知识点

Once you have set up a basic install Xen4QuickStart and have bridging ( CentOS6 or CentOS7) (the default bridge name is xenbr0 .. but you can do anything you want), you are ready to do some things with xen. One way is to use the normal VM tools in CentOS via libvirt (see Xen4Libvirt). But the built in libvirt is not extremely xen friendly and it limits what you can do to only the things virsh, virt-manager and virt-install understand. This is not very robust with respect to Xen.

另一种方法是使用`xl`工具创建、管理虚拟机（`xm`不再支持）。

This page is a basic example for creating a CentOS-6 and CentOS-7 DomU on a CentOS-6 Dom0 machine using xl.

1. 获取信息-xl命令

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

In this example, we will assume a bridge name of xenbr0. All the VMs will use this bridge. Here are the config files being used in this setup on CentOS-6:

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

Here is what it looks like on the example machine with IPv4 only:

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

You will also need some storage for your DomU VMs. You can also create image files if you prefer for your disks, but this example will use Logical Volumes (LVs) on LVM.

I personally like to use LVs for my machine drives because you can easily mount them and look at them from the Dom0 machine if there is a problem and it is easy to expand them later if need to. It is also used by default on almost all Linux distributions for the installed file system.

CentOS-6 and CentOS-7 will create a huge LV for /home by default. If you have a large drive it is easy to recover some space there to use LVs for xen drives. You can also use a second disk drive and create a new Physical Volume and a new Volume Group for your LVs. Here is a really good reference for creating some space in your Volume Group for creating blank Logical Volumes（关于LVM，请参考[LVM管理](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Logical_Volume_Manager_Administration/index.html)）

For this example, I am using the default VG that was installed by the CentOS installer for CentOS-6, I have some free space to create new logical volumes as shown here:

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

So I have a VG (named vg_c6xendom0) that has 1.43 TB of free space.

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

我们打算从ISO镜像中加载CentOS，所以，在Dom0上创建一个目录`/opt/isos/`，用来保存下载的CentOS 6和7的镜像。下载地址：[`CentOS 6`](http://isoredirect.centos.org/centos/6/isos/x86_64/)和[`CentOS 7`](http://isoredirect.centos.org/centos/7/isos/x86_64/)镜像。

# Xen虚拟机类型

There are 2 major VM types, PV (paravirtualized) and HVM (fully virtualized). There are also hybrid options. Here is am overview of Xen that discusses these: Xen Overview. In this simple example, we will create an HVM (fully virtualized) install, as that is the easiest type.

# 使用xl工具

Here is the documentation for xl from the Xen man pages :[xl(1)](http://xenbits.xen.org/docs/4.6-testing/man/xl.1.html).

We will be creating a CentOS-6 and a CentOS-7 HVM.

First we need config files for the VMs .. one for each. There is an example HVM config file in the xen package called `/etc/xen/xlexample.hvm`. You can look at the file and at this HVM doc: [xl.cfg(5)](http://xenbits.xen.org/docs/4.6-testing/man/xl.cfg.5.html#Fully-virtualised-HVM-Guest-Specific-Options)。

Here are our config files .. I created a directory in `/etc/xen/` called `config.d` where I keep config files.

* 首先，创建`/etc/xen/config.d/c6-x8664.hvm.cfg`

        builder = "hvm"
        name = "c6-x8664.hvm"
        memory = 4096
        vcpus = 2
        serial='pty'
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

See the above link in xl.cfg(5) for what each option means, but some important points are:

* the IP address for vnc listen is the IP of the bridge, in this example, 192.168.0.9.
* boot on floppy (a), hard disk (c), Network (n) or CD-ROM (d) .. so 'dc' means boot on CD then Disk We will edit it to only boot="c" once we are done.
* we would use a vnc client to connect to 192.168.0.9:5900 (centos-6) and 192.168.0.9:5901 (centos-7).

We will just do a normal CentOS install of each version from the ISO.


# xl create

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

# 半虚拟化

# Using Para-Virtualization (PV) or PV-on-HVM (PVHVM) instead

# PV-on-HVM (PVHVM)

PV-on-HVM is discussed here in detail ([LINK](https://wiki.xen.org/wiki/PV_on_HVM)), and actual examples here ([LINK](https://wiki.xen.org/wiki/Xen_Linux_PV_on_HVM_drivers)). Basically, all you need to do is to add xen_platform_pci=1 to your HVM config files above to use PVHVM.

# Para-Virtualization (PV)

Para-Virtualization no longer works by default on the CentOS-6 or CentOS-7 kernels. You could use the Dom0 kernels in your DomU and turn on PV after the install by modifying your config file.

If you would like to try to do it, here are some notes:

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


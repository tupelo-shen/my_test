[TOC]

# 1 关于Libvirt和Xen

在Xen中安装的主机OS称为Dom0。通过Xen运行的虚拟机称为DomU。

默认情况下，`libvirt`创建NAT网络连接方式（192.168.122.0/24），默认网卡通常是`eth0`。

通过这种NAT网络连接方式，DomU上的虚拟机可以互相连接，也可通过Dom0与外界进行通信（但是，只能通过Dom0上的libvirt（或Xen）。当然，如果你喜欢，可以将`NAT`连接方式改为`bridge`桥接方式。至于桥接方式怎么修改，本文的第7段文字会进行阐述。

# 2 先决条件

本文的前提条件是已经按照前面讲过的[CentOS-快速搭建Xen虚拟环境](TODO)指导手册，已经安装好了Xen。比如，调用`xl info`命令能有如下输出：

    [root@xentest1 ~]# xl info
    host                   : immortal
    release                : 3.10.56-11.el6.centos.alt.x86_64
    version                : #1 SMP Thu Oct 9 14:57:01 CDT 2014
    machine                : x86_64
    nr_cpus                : 4
    max_cpu_id             : 31
    nr_nodes               : 1
    cores_per_socket       : 4
    threads_per_core       : 1
    cpu_mhz                : 2533
    hw_caps                : bfebfbff:2c100800:00000000:00003f00:029ee3ff:00000000:00000001:00000000
    virt_caps              : hvm hvm_directio
    total_memory           : 6141
    free_memory            : 5030
    sharing_freed_memory   : 0
    sharing_used_memory    : 0
    outstanding_claims     : 0
    free_cpus              : 0
    xen_major              : 4
    xen_minor              : 4
    xen_extra              : .1-4.el6
    xen_version            : 4.4.1-4.el6
    xen_caps               : xen-3.0-x86_64 xen-3.0-x86_32p hvm-3.0-x86_32 hvm-3.0-x86_32p hvm-3.0-x86_64 
    xen_scheduler          : credit
    xen_pagesize           : 4096
    platform_params        : virt_start=0xffff800000000000
    xen_changeset          : Wed Oct 15 15:36:23 2014 +0100 git:9f49483-dirty
    xen_commandline        : dom0_mem=1024M,max:1024M loglvl=all guest_loglvl=all com1=115200,8n1 console=com1,vga
    cc_compiler            : gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-11)
    cc_compile_by          : mockbuild
    cc_compile_domain      : centos.org
    cc_compile_date        : Mon Dec 15 17:54:14 UTC 2014
    xend_config_format     : 4

> 除非特别注明：所有的指令都是在Dom0的root用户下完成的。

因为本文的安装都是在CentOS的mini版上进行的，所以，可能需要安装下面这些小工具，方便使用。

    yum install rsync wget vim-enhanced openssh-clients

# 3 安装libvirt（在Dom0上）

首先，我们需要安装libvirt所需的基本包:

如果是`CentOS-6 hypervisor`：

    yum install libvirt python-virtinst libvirt-daemon-xen

如果是`CentOS-7 hypervisor`：

    yum install libvirt libvirt-daemon-xen

完成安装后，重启Dom0。

> 如果在执行`# virt-install `命令时，报`bash: virt-install: 未找到命令...`的错误：
> 
> 请执行下面两条指令：
> 
>       yum install libguestfs-tools
>       yum install virt-install.noarch


# 4 远程LibVirt访问

Most of the time, we do not recommend installing a full GUI system on Dom0 servers, so most people will be accessing/controlling the Dom0 libvirt remotely.

There are 2 normal ways to do this ... one is from a graphical machine (like a CentOS-6 workstation) and another is to use ssh and interact with the virsh console application.

## 4.1 Controlling libvirt Access via PolicyKit

This would be used if you want to connect a CentOS-6 workstation that has virt-manager installed to a Dom0 install and control DomU VMs on that machine. It would also be used to connect a virsh terminal running on one machine directly to the Dom0 and control DomU VMs.

PolicyKit allows for very flexible, fine grained access control that greatly exceeds just granting access to libvirt via a unix group as is explained in this simple example.

For more complex access controls, see [polkit documentation](http://www.freedesktop.org/software/polkit/docs/0.105/)

### 4.1.1 Configuration of libvirt for Group Access

To give management access to members of a unix group, we only need to create a PolicyKit Local Authority file.

This is a plain text file, generally placed in this directory:

    /etc/polkit-1/localauthority/50-local.d/

The name of the file is up to you, but needs to start with a two digit number and end with .pkla. For example:

    /etc/polkit-1/localauthority/50-local.d/50-libvirt-remote-access.pkla

It's contents should be:

    [Remote libvirt SSH access]
    Identity=unix-group:group_name
    Action=org.libvirt.unix.manage
    ResultAny=yes
    ResultInactive=yes
    ResultActive=yes

You would replace **group_name** with the applicable group.

So, for example, we have a unix user named **johnny** and we want to create a group called **remote-libvirt** to control remote access to the libvirt and add johnny to that group.

First we would login to the Dom0 machine via ssh and gain root access. Then we would issue this command to create the new group:

    groupadd remote-libvirt 

Then we would add our user to the remote-libvirt group with this command:

    usermod -G remote-libvirt johnny

Now the unix user johnny can control libvirt remotely.

## 4.2 Connection to a Remote Dom0

### 4.2.1 Connecting to the Dom0 Machine via virt-manager from a Remote Machine

Login to the desktop of your GUI machine (this can be a CentOS-6 workstation or one GUI server that you can use to control several the other non GUI Dom0 machines) and make sure virt-manager is installed by opening a terminal widow and issuing this command as root:

    yum install virt-manager

As a normal user, open virt-manager. This uses the **Applications => System Tools => Virtual Machine Manager** launcher ... or type **virt-manager** from a terminal window.

Inside the virt-manager app, select the **File => Add Connection** menu item and pick **Xen** for the hypervisor, check the **connect to a remote host** box, pick **SSH** as the method, use the user you added to the remote-libvirt group (in our example, **johnny**) username, and lastly for hostname use either a hostname or IP address for the DomU (in my example, we will use **192.168.0.5**) ... then click connect.

It will prompt you for johnny's ssh username for the DomU ... give it and you should see a connection to the host and a **Domain-0** indicator in virt-manager.

The URI in this case is:

    xen+ssh://johnny@192.168.0.5





### 4.2.2 Connecting to the Dom0 Machine via virsh from a Remote Machine

You can use virsh two ways on the Dom0 instance to control DomU VMs.

The first way is to just ssh to the Dom0 machine and become the root user and just run **virsh** from the command line.

The second way is to use our access group from the above example and do this command:

    virsh -c xen+ssh://johnny@192.168.0.5

The good thing about this is that we do not need to give user **johnny** full root access to Dom0 instance, just his normal login. He can still do all the functions in virsh.

For more information on virsh, see this [guide](https://access.redhat.com/site/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Virtualization_Administration_Guide/chap-Virtualization_Administration_Guide-Managing_guests_with_virsh.html)


# 5 Creating DomU Virtual Machines

>   
if you're installing CentOS 7 DomU as a paravirt guest, you have to remember that /boot in your VM can't be xfs - see https://wiki.xen.org/wiki/PyGrub. So either you use a kickstart or you have to install it with vnc so that you can specificy something else than the default. You can add those options in 'extra-args'


## 5.1 使用virt-manager安装DomU

Now you can highlight the new hostname/IP in virt-manager and either **right-click** and pick **New** or click the **Create a new Virtual Machine** icon and then follow the wizard to add a new machine.

This process is the same for Remote or Local connections ... the only difference is the Connection box on the **Step 1 of 5** page of the wizard.

The new machine install will be very similar to [this guide](https://access.redhat.com/site/documentation//en-US/Red_Hat_Enterprise_Linux/6/html/Virtualization_Host_Configuration_and_Guest_Installation_Guide/chap-Virtualization_Host_Configuration_and_Guest_Installation_Guide-Guest_Installation_Virt_Manager-Creating_guests_with_virt_manager.html), except the connection on the **Step 1 of 5** page of the wizard you will have your **remote hostname** (in our example 192.168.0.5) and on the **step 5 of 5** page under advanced options you will have **xen(paravirt)** or **xen(fullvirt)** instead of KVM for **Virt Type**.



## 5.2 virt-install安装DomU（通过SSH） 

使用ssh连接Dom0的客户机，切换到root用户，然后使用命令`virt-install`和命令行进行安装。这个命令允许root用户远程使用文本命令进行安装。


    virt-install -d -n TestVM1 -r 2048 --vcpus=1 --disk /var/lib/libvirt/images/TestVM1.img,size=8 \
    --nographics -p -l "http://192.168.0.10/centos/6.4/os/x86_64" \
    --extra-args="text console=com1 utf8 console=hvc0"

下面的命令已经成功：

    virt-install -d -n TestVM1 -r 2048 --vcpus=1 --disk /dev/centos/c6-x8664-hvm,size=30 \
    --nographics -p -l "http://mirror.centos.org/centos/6/os/x86_64/" \
    --extra-args="text console=com1 utf8 console=hvc0"

各个选项的意义：

* -d - 调试模式，打印大量的文本信息和配置文件内容，方便调试

* -n TestVM1 - 指定VM的名称

* -r 2048 - 指定RAM大小（2GB）

* --vcpus=1 - 指定虚拟CPU的数量

* --disk=/var/lib/libvirt/images/TestVM1.img,size=8 - 指定硬盘的位置，size指定大小，单位是GB。当然了，这个也可以指向LVM

* --nographics - 由于我们没有在Dom0上安装GUI，不要使用VNC等

* -p - 指定半虚拟化。如果是全虚拟化，需要使用`-v`选项

* -l "http://192.168.0.10/centos/6.4/os/x86_64" - 指定安装镜像的位置，也可以使用 http://mirror.centos.org/centos/6/os/x86_64/ 或者其它镜像网站

* --extra-args="text console=com1 utf8 console=hvc0" - 这告诉linux使用com1和xen来使用hvc0，这样您就可以通过控制台进行文本安装

> 这种方式的安装只适用于文本方式。virt-manager可以允许图形化方式安装。

更多关于`virt-install`的使用方式，请参考该[指导手册](https://access.redhat.com/site/documentation//en-US/Red_Hat_Enterprise_Linux/6/html/Virtualization_Host_Configuration_and_Guest_Installation_Guide/sect-Virtualization_Host_Configuration_and_Guest_Installation_Guide-Guest_Installation-Creating_guests_with_virt_install.html).



# 6 Controlling DomU Virtual Machines


DomU VMs can be controlled using libvirt either by a graphical application (virt-manager) or a console application (virsh).

These applications can be either running on the Dom0 where the DomU VMs reside (local control) or the applications can reside on a different machine and connect to the DomU (remote control).

The actual use of either tool is the same whether connected remotely or locally.

Please see these instructions for remote virt-manager or remote virsh connections. Remote access for both tools require that you configure libvirt for remote access.

Please see this guide for using virsh and this guide for using virt-manager.


# 7 桥接网络设置

要想配置`bridge`，必须安装`bridge-utils`工具。CentOS的安装方式：

    yum install bridge-utils

修改`/etc/sysconfig/network-scripts/`目录下的文件。下面是建立一个名为`br1`的桥接网卡的最小设置

1. 修改`ifcfg-eth0`文件：

        DEVICE=eth0
        NM_CONTROLLED=no
        ONBOOT=yes
        TYPE=Ethernet
        BRIDGE="br1"
        USERCTL=no

2. 添加`ifcfg-br1`文件，填入如下内容：

        DEVICE=br1
        BOOTPROTO=none
        NM_CONTROLLED=no
        ONBOOT=yes
        TYPE=Bridge
        IPADDR=192.168.0.5
        PREFIX=24
        GATEWAY=192.168.0.1
        DNS1=8.8.8.8
        DNS2=8.8.4.4
        USERCTL=no

更多关于桥接的设置，请参考[这儿](http://wiki.xen.org/wiki/Network_Configuration_Examples_%28Xen_4.1%2B%29#Red_Hat-style_bridge_configuration_.28e.g._RHEL.2C_Fedora.2C_CentOS.29)还有[这儿](https://sites.google.com/site/ghidit/how-to-2/configure-bridging-on-centos)。当然，也可以baidu 

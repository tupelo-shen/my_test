[TOC]

# 1 Libvirt and Xen Basics

The HostOS install in Xen is known as Dom0. Virtual Machines (VMs) running via Xen are known as DomU's.

By default, libvirt creates a Network Address Translation (NATed) 192.168.122.0/24 network behind the default network card (normally eth0).

The DomU VMs running on this NATed network can connect to each other and connect outbound from the Dom0 network, but your only connection to them is via libvirt (or xen) on the Dom0. You can modify the default network to become a Bridged network instead of a NATed network, which will allow you to connect to the DomU VMs as if they where on the same physical network as the Dom0. Bridging is discussed below (Section 7).

# 2 Prerequisites

This page assumes you have followed the Xen4QuickStart guide and have a Xen kernel installed and the command xl info looks something like this:

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

Unless otherwise noted, all instructions should be done as the root user on the Dom0 machine.

I normally like to add a couple of packages to every minimal install, and some things later may need these packages. Install them via this command:

    yum install rsync wget vim-enhanced openssh-clients

# 3 Installing libvirt (on the Dom0 machine)

First we need to install the basic packages required for libvirt:

on a CentOS 6 hypervisor :

    yum install libvirt python-virtinst libvirt-daemon-xen

on a CentOS 7 hypervisor :

    yum install libvirt libvirt-daemon-xen

After the install, restart your Dom0 machine.

# 4 Remote LibVirt Access

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


## 5.1 Using virt-manager to install a DomU

Now you can highlight the new hostname/IP in virt-manager and either **right-click** and pick **New** or click the **Create a new Virtual Machine** icon and then follow the wizard to add a new machine.

This process is the same for Remote or Local connections ... the only difference is the Connection box on the **Step 1 of 5** page of the wizard.

The new machine install will be very similar to [this guide](https://access.redhat.com/site/documentation//en-US/Red_Hat_Enterprise_Linux/6/html/Virtualization_Host_Configuration_and_Guest_Installation_Guide/chap-Virtualization_Host_Configuration_and_Guest_Installation_Guide-Guest_Installation_Virt_Manager-Creating_guests_with_virt_manager.html), except the connection on the **Step 1 of 5** page of the wizard you will have your **remote hostname** (in our example 192.168.0.5) and on the **step 5 of 5** page under advanced options you will have **xen(paravirt)** or **xen(fullvirt)** instead of KVM for **Virt Type**.



## 5.2 Using virt-install to install a DomU via SSH

Another method for a remote install is to connect to the Dom0 machine via ssh and become the root user, and then use virt-install and the console to do an install. This command (as root, from the command line) will allow a text install remotely:


    virt-install -d -n TestVM1 -r 2048 --vcpus=1 --disk /var/lib/libvirt/images/TestVM1.img,size=8 --nographics -p -l "http://192.168.0.10/centos/6.4/os/x86_64" --extra-args="text console=com1 utf8 console=hvc0"

In the above example, the meanings are:

-d - Debug mode, lots of text and full config files printed for debuging

-n TestVM1 - The name of the VM

-r 2048 - Ram size (2048 MB or 2 GB)

--vcpus=1 - Number of Virtual CPUS

--disk=/var/lib/libvirt/images/TestVM1.img,size=8 - disk image location and size in GB ... this can also point to LVM, etc

--nographics - since we do not have GUI installed on our Dom0, do not use VNC, etc.

-p - use para-virtualization

-l "http://192.168.0.10/centos/6.4/os/x86_64" - Location of a centos tree, can also use http://mirror.centos.org/centos/6/os/x86_64/ or other mirrors.

--extra-args="text console=com1 utf8 console=hvc0" - this tells linux to use com1 and xen to use hvc0 so that you can do a text install via the console

Note: This type of install is only console based, so only a text install can be done this way. The virt-manager install (previous section) above will allow fully graphical installs as well.

For more information on doing installs via virt-install, see this [guide](https://access.redhat.com/site/documentation//en-US/Red_Hat_Enterprise_Linux/6/html/Virtualization_Host_Configuration_and_Guest_Installation_Guide/sect-Virtualization_Host_Configuration_and_Guest_Installation_Guide-Guest_Installation-Creating_guests_with_virt_install.html).



# 6 Controlling DomU Virtual Machines


DomU VMs can be controlled using libvirt either by a graphical application (virt-manager) or a console application (virsh).

These applications can be either running on the Dom0 where the DomU VMs reside (local control) or the applications can reside on a different machine and connect to the DomU (remote control).

The actual use of either tool is the same whether connected remotely or locally.

Please see these instructions for remote virt-manager or remote virsh connections. Remote access for both tools require that you configure libvirt for remote access.

Please see this guide for using virsh and this guide for using virt-manager.


# 7 Example Bridge Setup

Here is another example bridge setup:

You must have bridge-utils installed to configure bridges. You can install it on CentOS with this command:

    yum install bridge-utils

The below files are in /etc/sysconfig/network-scripts/ and must be edited.

Minimal Example with a br1

ifcfg-eth0

    DEVICE=eth0
    NM_CONTROLLED=no
    ONBOOT=yes
    TYPE=Ethernet
    BRIDGE="br1"
    USERCTL=no

ifcfg-br1

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

Other examples of bridge setups are discussed [here](http://wiki.xen.org/wiki/Network_Configuration_Examples_%28Xen_4.1%2B%29#Red_Hat-style_bridge_configuration_.28e.g._RHEL.2C_Fedora.2C_CentOS.29) and [here](https://sites.google.com/site/ghidit/how-to-2/configure-bridging-on-centos). Also, Google is your friend 

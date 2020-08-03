这是一篇向初学者介绍Xen相关的基本概念，以及如何在没有经验的情况下启动Xen项目的指导手册。但是，一些Linux的经验还是必要的，比如网络、lvm和grub，这对于Xen的理解和使用大有裨益。另外，还应该熟悉在root权限下的操作方法。很多时候，我们可能需要在root权限下进行操作。

完成本手册，你就拥有了一个全功能的Xen管理程序，并且能够启动第一个客户操作系统。在此基础上，你可以将其接入网络，学习一些其它的基本知识，比如虚拟机存储和虚拟网络。

为了简化本文的过程，直接使用了一个Linux的发行版本，Debian。本文档最初编写的时候，使用了Debian6/7（分别称之为`Wheezy`和`Wheezy`），并且在最新的Debian10（称为`Buster`）上进行了测试。在较新版本上应该也可以运行。Debian对`Xen 4.x`支持较好。

[TOC]

# 1 Xen是什么?

Xen项目是一个虚拟机监控软件，也称为`hypervisor`：是一个允许多个虚拟客户机操作系统运行在同一个物理机上的软件系统。它创建一个类型1的hypervisor管理程序，这意味着它直接运行在物理机上，而不是操作系统之上。

运行在Xen之上的虚拟客户机称为`domain`。domain0或者dom0是一个特殊的控制域，用来管理Xen程序并启动其它客户机操作系统。其它的虚拟机称为domU，它们运行的非特权模式下，不能控制Xen程序或启停其它虚拟机。

Xen管理程序支持两种虚拟化：半虚拟化（PV）和全虚拟化（硬件虚拟化）。半虚拟化需要修改客户操作系统，这些操作系统知道它们正在被虚拟化，因此不需要虚拟硬件设备。相反，它们调用hypervisor的特殊接口，访问CPU，存储设备和网络资源。

相反，全虚拟化（HVM）不需要修改客户机操作系统，因为它会完全模拟一个物理计算机设备。所以，相比半虚拟化，其允许不修改客户机操作系统，但是牺牲了更多的性能。HVM需要CPU支持虚拟化扩展功能，熟知的有英特尔的V-T和AMD公司的AMD-V。

Xen虚拟化实现范围介于半虚拟化和硬件虚拟化之间，在其中还有许多变体，比如带有半虚拟化驱动的硬件虚拟机，或者在硬件虚拟机上实现半虚拟化等。之所以，存在这些变体，都是为了提高它们的性能。为了简化本手册，我们创建一个通用半虚拟化VM和一个可选的硬件虚拟化的客户机。对于其它的变体，可以查看下面这篇文章：

* [Understanding the Virtualization Spectrum](https://wiki.xenproject.org/wiki/Understanding_the_Virtualization_Spectrum)


# 2 Xen架构简介

为了理解存储、网络和其它资源如何被派发到客户系统中，我们需要深入研究一下软件的不同部分是如何交互的。

<img src=" ">

This is the basic architecture of the Xen Project Hypervisor. We see that the hypervisor sits on the bare metal (the actual computer hardware). The guest VMs all sit on the hypervisor layer, as does dom0, the "Control Domain". The Control Domain is a VM like the guest VMs, except that it has two basic functional differences:

1. The Control Domain has the ability to talk to the hypervisor to instruct it to start and stop guest VMs.

2. The Control Domain by default contains the device drivers needed to address the hardware. This stops the problem that often plagued Linux users in the 1990s: You install your software on a new piece of hardware, only to find that you lack the drivers to use it. Since those early days, Linux and the BSDs have become quite good at supporting more pieces of hardware fairly quickly after they are birthed. Xen Project leverages that support by using the drivers in the Control Domain's operating system to access many types of hardware.

<img src=" ">

Dom0 forms the interface to the hypervisor. Through special instructions dom0 communicates to the Xen Project software and changes the configuration of the hypervisor. This includes instantiating new domains and related tasks.

Another crucial part of dom0’s role is as the primary interface to the hardware. The hypervisor doesn’t contain device drivers. Instead the devices are attached to dom0 and use standard Linux drivers. Dom0 then shares these resources with guest operating systems.

To implement paravirtualization, each paravirtualized datapath consists of two parts: 1) a “backend” that lives in dom0, which provides the virtual device and 2) a “frontend” driver within the guest domain, which allows the guest OS to access the virtual device. The backend and frontend use a high-speed software interface based on shared memory to transfer data between the guest and dom0.

The two important paravirtualized datapaths are: net-back/net-front, and blk-back/blk-front - which are the paravirtualized networking and storage systems, respectively. There are also paravirtualized interrupts, timers, page-tables and more.

You can read more about how the Xen Project system is architected, paravirtualization and the benefits of such here:

* Details of [Paravirtualization (PV)](https://wiki.xenproject.org/wiki/Paravirtualization_(PV)) and how it is used on Xen Project

In the case of HVM guests, dom0 uses hardware virtualization extensions provided by the CPU. The most basic of these is virtualization of the CPU itself. Support was later added for page-table management (MMU) and I/O virtualization (IOMMU). Dom0 also emulates some hardware using components of qemu (the Quick Emulator). Emulation in software requires the most overhead, however, so performance is reduced.


# 3 Preparation

This guide requires a number of items, this checklist is what you will need:

* 64bit x86 computer with at least 1GB of RAM (this can be a server, desktop or laptop)

* Intel VT or AMD-V support (optional for PV, required for HVM and some PV optimizations)

* Sufficient storage space for your dom0 and whatever guests you want to install

* A CD/DVD burner + blank CD/DVD (you can use USB but that's not covered here - see the Debian site for details)

* Internet access and some way to download Debian and burn it to the CD/DVD (e.g. another computer)

* installation ISO for a licensed copy of Windows or a trial copy of Windows Server 2008R2 (only if you want a Windows HVM)

* VNC client (optional for PV, required for HVM)

A word about VT/AMD-V: If you want to be sure you can use the hardware extensions, it is important to check that both the CPU chipset and the motherboard support virtualization. It is quite possible to have virtualization features in the chipset that cannot be enabled because the mobo isn't designed for it. Also, if you plan to use an HVM instance for more that demonstration purposes, the underlying hardware should support at least VT-d and VT-i or AMD-V and AMD-Vi. Having said all of that, sometimes the easiest (or only way) to see what is supported is to check the BIOS.


## 3.1 Enable virtualization support in BIOS

NOTE: This is not strictly required for PV guests. However, it is highly recommended so that you have the widest number of options for virtualization modes once you get underway.

In order to support HVM guests we need to ensure that virtualization extensions are enabled in the BIOS. If you find your system doesn’t support these extensions you cannot use the hypervisor to virtualize unmodified operating systems. Paravirtualization will work fine though.

The virtualization options appear differently in different BIOS builds but often are referred to as “Enable Intel VT” for Intel chipsets, "Enable AMD-V" for AMD or simply “Enable Virtualization Technology”. Oftentimes this option can be found under the “Advanced Chipset Features” menu in the BIOS or by using Search if the BIOS supports that. It is worthwhile digging around on this a bit. The options may be specified individually, for example: VT-x and VT-d or AMD-V and AMD-IOMMU (aka AMD-Vi or AMD-RVI). You may even find one is enabled by default but the other is not!

Consult your motherboard documentation for more assistance in enabling virtualization extensions on your system.


## 3.2 Download and Burn the Debian Installer CD

You can find the most recent Debian ISO images at this URL:

[http://cdimage.debian.org/debian-cd/current/amd64/iso-cd/](http://cdimage.debian.org/debian-cd/current/amd64/iso-cd/)

The netinst image is sufficient for our purposes.

Burn the ISO to disk using your computer's standard utilities. Linux has wodim (among others) or use the built in ISO burning feature in Windows.

## 3.3 Quick intro to Debian

Debian is a simple, stable and well supported Linux distribution. It has included Xen Project Hypervisor support since Debian 3.1 “Sarge” released in 2005.

Debian uses the simple Apt package management system which is both powerful and simple to use. Installing a package is as simple as the following example:

    apt-get install htop

where htop is the application desired to install.

Simple tasks such as configuring startup scripts, setting up the network etc are covered by this tutorial so don’t worry if you haven’t used Debian before!

Many popular distributions are based off of Debian and also use the Apt package manager, if you have used Ubuntu, Linux Mint or Damn Small Linux you will feel right at home.


# 4 Installing Debian

Boot the Debian Installer CD/DVD

Insert the Debian CD/DVD and configure the CDROM drive as your default boot device in the BIOS or use the system boot menu if your BIOS supports it (usually F12).

You should see a menu, choose the default “Install” option to begin the installation process. Install the system The Debian installer is very straight forward. Follow the prompts until you reach the disk partitioning section.

Choose advanced/custom, we are going to configure a few partitions here, one for /boot another for /, one more for swap and a final partition to setup as an LVM volume group for our guest machines.

First create the /boot partition by choosing the disk and hitting enter, make the partition 300MB and format it as ext2, choose /boot as the mountpoint.

Repeat the process for / but of course changing the mountpoint to / and making it 15GB or so large. Format it as ext3.

Create another partition approximately 1.5x the amount of RAM you have in size and elect to have it used as a swap volume.

Finally create a partition that consumes the rest of the diskspace but don’t format it or assign a mount point.

We should now have a layout that looks like this assuming your disk device is /dev/sda :

    sda1 - /boot 200MB
    sda2 - / 15GB
    sda3 - swap
    sda4 - reserved for LVM

When you reach the package selection stage only install the base system. We won’t require any GUI or other packages for this guide. (If you want to set up a graphical desktop environment in dom0, that's not a problem, but you may want to wait until after you've completed this guide to avoid complicating things.)

You can find out details of the Debian installation process from the Debian documentation

Continue through the installer then reboot and login at the prompt as root.

If you've got any hardware you're not sure open source drivers are available for, you may want to install non-free firmware files via:

    apt-get install firmware-linux-nonfree

If this does not work straight away make sure your /etc/apt/sources.list has entries including non-free and perhaps contrib while you're at it, e.g. like this:

    deb http://some.debian.server.org/debian stretch main contrib non-free

Add the same to deb-src and the stretch/updates lines (changing 'stretch' to the name of the current Debian release if you're using some newer release).

# 5 Installing the Xen Project Software

We've still got a few more steps to complete before we're ready to launch a domU, but let's install the Xen Project software now and use it to check the BIOS settings.

The Debian Xen Project packages consist primarily of a Xen Project-enabled Linux kernel, the hypervisor itself, a modified version of QEMU that support the hypervisor’s HVM mode and a set of userland tools.

All of this can be installed via an Apt meta-package called xen-linux-system. A meta-package is basically a way of installing a group of packages automatically. Apt will of course resolve all dependencies and bring in all the extra libraries we need.

Let's install the xen-linux-system meta-package:

    apt-get install xen-system-amd64

Now we have a Xen Project hypervisor, a Xen Project kernel and the userland tools installed. When you next boot the system, the boot menu should include entries for starting Debian with the Xen hypervisor. One of them should be highlighted, to start Xen by default. Do that now, logging in as root again.

Next, let's check to see if virtualization is enabled in the BIOS. There are a few ways to do that.

The most comprehensive is to review the Xen section of dmesg created during the boot process. This will be your first use of xl, the very versatile Xen tool, which we will come back to shortly to create and manage domUs:

    xl dmesg

Included in the output will be references to the CPU flags set in the BIOS to enable virtualization: 'vmx' for Intel, 'svm' for AMD. It will also detail other hardware virtualization extensions: VT-d features, Hardware Assisted Paging (HAP), I/O Virtualization and so on.

Another way is to check the flags set in the CPU on boot:

    egrep '(vmx|svm|hypervisor)' /proc/cpuinfo


egrep will return any line containing one or more of those same text fragments (vmx/svm or more recently, just 'hypervisor'). If nothing comes back and you think it should, you may wish to look through the flags yourself:

    cat /proc/cpuinfo

If the virtualization extensions don't appear, take a closer look at the BIOS settings. A few round-trips through the BIOS are often required to get all the bits working right.





# 6 Setup LVM storage for guests

LVM is the Linux Logical Volume Manager. It is a technology that allows Linux to manage block devices in a more abstract manner.

LVM introduces the concept of a “logical volume”, effectively a virtualized block device composed of blocks written to one or more physical devices. Unlike proper disk partitions, these blocks don’t need to be contiguous.

Because of this abstraction logical volumes can be created, deleted, resized and even snapshotted without affecting other logical volumes.

LVM creates logical volumes within what is called a volume group, which is simply a set of logical volumes that share the same physical storage, known as physical volumes.

The process of setting up LVM can be summarized as allocating a physical volume, creating a volume group on top of this, then creating logical volumes to store data.

Because of these features and superior performance over file backed virtual machines we recommend the use of LVM if you are going to store VM data locally.

Now lets install LVM and get started!

Install LVM:

    apt-get install lvm2

Now that we have LVM installed let's configure it to use /dev/sda4 as its physical volume

    pvcreate /dev/sda4

Ok, now LVM has somewhere to store its blocks (known as extents for future reference). Let's create a volume group called ‘vg0’ using this physical volume:

    vgcreate vg0 /dev/sda4

Now LVM is setup and initialized so that we can later create logical volumes for our virtual machines.

For the interested below is a number of useful commands and tricks when using LVM.

Create a new logical volume:

    lvcreate -n <name of the volume> -L <size, you can use G and M here> <volume group>

For example, creating a 100 gigabyte volume called database-data on a volume group called vg0.

    lvcreate -n database-data -L 100G vg0

You can then remove this volume with the following:

    lvremove /dev/vg0/database-data

Note that you have to provide the path to the volume here.

[More on LVM on Debian here.](https://wiki.debian.org/LVM#List_of_VG_commands)

If you already have a volume setup that you would like to copy, LVM has a cool feature that allows you to create a CoW (copy on write) clone called a snapshot. This means that you can make an "instant" copy that will only store the changes compared to the original. There are a number of caveats to this that will be discussed in a yet unwritten article. The most important thing to note is that the "size" of the snapshot is only the amount of space allocated to store changes. So you can make the snapshot "size" a lot smaller than the source volume.

To create a snapshot use the following command:

    lvcreate -s /dev/vg0/database-data -ndatabase-backup -L5G

Once again note the use of the full path.

# 7 Setup Linux Bridge for guest networking

Next we need to set up our system so that we can attach virtual machines to the external network. This is done by creating a virtual switch within dom0. The switch will take packets from the virtual machines and forward them on to the physical network so they can see the internet and other machines on your network.

The piece of software we use to do this is called the Linux bridge and its core components reside inside the Linux kernel. In this case, the bridge acts as our virtual switch. The Debian kernel is compiled with the Linux bridging module so all we need to do is install the control utilities:

    apt-get install bridge-utils

Management of the bridge is usually done using the brctl command. The initial setup for our Xen bridge, though, is a "set it once and forget it" kind of thing, so we are instead going to configure our bridge through Debian’s networking infrastructure. It can be configured via /etc/network/interfaces.

Open this file with the editor of your choice. If you selected a minimal installation, the nano text editor should already be installed. Open the file:

    nano /etc/network/interfaces

(If you get nano: command not found, install it with apt-get install nano.)

Depending on your hardware you probably see a file pretty similar to this:

    auto lo
    iface lo inet loopback

    auto eth0
    iface eth0 inet dhcp

This file is very simple. Each stanza represents a single interface.

Breaking it down, “auto eth0” means that eth0 will be configured when ifup -a is run (which happens at boot time). This means that the interface will automatically be started/stopped for you. ("eth0 is its traditional name - you'll probably see something more current like "ens1", "en0sp2" or even "enx78e7d1ea46da")

“iface eth0” then describes the interface itself. In this case, it specifies that it should be configured by DHCP - we are going to assume that you have DHCP running on your network for this guide. If you are using static addressing you probably know how to set that up.

We are going to edit this file so it resembles such:


    auto lo
    iface lo inet loopback

    auto eth0
    iface eth0 inet manual
    
    auto xenbr0
    iface xenbr0 inet dhcp
         bridge_ports eth0

As well as adding the bridge stanza, be sure to change dhcp to manual in the iface eth0 inet manual line, so that IP (Layer 3) is assigned to the bridge, not the interface. The interface will provide the physical and data-link layers (Layers 1 & 2) only.

Now restart networking (for a remote machine, make sure you have a backup way to access the host if this fails):

    service networking restart

and check to make sure that it worked:

    brctl show

If all is well, the bridge will be listed and your interface will appear in the interfaces column:

    bridge name     bridge id               STP enabled     interfaces
    xenbr0          8000.4ccc6ad1847d       no              enp2s0

Bridged networking will now start automatically every boot.

If the bridge isn't operating correctly, go back and check the edits to the interfaces file very carefully.

Reboot before continuing. During the reboot, note the list of OS choices and check to see what the default start-up choice is. "Debian GNU/Linux, with Xen hypervisor" (or equivalent) means Xen is loading by default. If both the start-up default is fine, skip the next section and go directly to Basic Xen Project Commands.



# 8 Configure GRUB to start Xen Project

GRUB, the bootloader installed during installation, tells the computer which operating system to start and how. To use the hypervisor, Xen must be started before the operating system. Depending on your use-case for Xen, you may or may not want GRUB to default to loading Xen.

GRUB2's configuration is stored in the file /boot/grub/grub.cfg

We aren’t going to edit this file directly, as it changes every time we update our kernel. Debian configures GRUB for us using a number of automated scripts that handle upgrades etc, these scripts are stored in /etc/grub.d/* and can be configured via

    /etc/default/grub

To change the default operating system change the GRUB_DEFAULT line in that file (or add it if it's not already there).

    GRUB_DEFAULT=0

causes the first OS in the boot list to be the default, and so on. So, if Xen was, say, the third OS choice, change the line to

    GRUB_DEFAULT=2

to have Xen load by default.

Then regenerate the /boot/grub/grub.cfg file by running:

    update-grub

At the next reboot confirm the correct default boot option is selected.



# 9 Basic Xen Project Commands

Before we dive into creating some guest domains we will quickly cover some basic commands. In the examples below, we use xl command line tool. Older versions of the Xen Project software used the xm command line tool. xl and xm are command line compatible (the format of the output may be slightly different). If, for example, you come across "xm" while reading old documentation, say, just substitute "xl".

Lets start with simple stuff!

    xl info

returns the information about the hypervisor and dom0 including version, free memory etc.

    xl list

lists running domains, their IDs, memory, state and CPU time consumed

    xl top

shows running domains in real time and is similar to the “top” command under Linux. This can be used to visualize CPU, memory usage and block device access.

We will cover some more commands during the creation of our guest domains.

See also: [Xen Project 4.x Man Pages](https://wiki.xenproject.org/wiki/Xen_4.x_Manuals)


# 10 Creating a Debian PV (Paravirtualized) Guest

PV guests are notoriously “different” to install. Fortunately, though, there are tools that help us prepare “images” or snapshots of the operating systems to run inside guest domains.

Debian contains a number of tools for creating Xen Project guests, the easiest of which is known as xen-tools. This software suite manages the downloading and installing of guest operating systems including both Debian and RHEL based DomUs. In this guide we are going to use xen-tools to prepare a Debian paravirtualized domU.

xen-tools can use LVM storage for storing the guest operating systems. In this guide we created the volume group “vg0” in the Setting up LVM Storage section.

When guests are paravirtualized there is no “BIOS” or bootloader resident within the guest filesystem and for a long time guests were provided with kernels external to the guest image. This however is bad for maintainability (guests cannot upgrade their kernels without access to the dom0) and is not as flexible in terms of boot options as they must be passed via the config file.

The Xen Project community wrote a utility known as pygrub which is a python application for PV guests that enables the dom0 to parse the GRUB configuration of the domU and extract its kernel, initrd and boot parameters. This allows for kernel upgrades etc inside of our guest machines along with a GRUB menu. Using pygrub or the stub-dom implementation known as pv-grub is best practice for starting PV guests. In some cases pv-grub is arguably more secure but as it is not included with Debian we won’t use it here though it is recommended in production environments where guests cannot be trusted.

Apart from this PV guests are very similar to their HVM and physical OS counterparts.

## 10.1 Configuring xen-tools and building our guest

First lets install the xen-tools package:

    apt-get install xen-tools

We can now create a guest operating system with this tool. It effectively automates the process of setting up a PV guest from scratch right to the point of creating config files and starting the guest. The process can be summarized as follows:

* Create logical volume for rootfs
* Create logical volume for swap
* Create filesystem for rootfs
* Mount rootfs
* Install operating system using debootstrap (or rinse etc, only debootstrap covered here)
* Run a series of scripts to generate guest config files like fstab/inittab/menu.lst
* Create a VM config file for the guest
* Generate a root password for the guest system
* Unmount the guest filesystem

These 9 steps can be carried out manually but the manual process is outside the scope of this guide. We instead will execute the below command (for --dist you could in place of Wheezy e.g. use Squeeze, or even Precise or Quantal for a Ubuntu install):

    xen-create-image --hostname=tutorial-pv-guest \
    --memory=512mb \
    --vcpus=2 \
    --lvm=vg0 \
    --dhcp \
    --pygrub \
    --dist=wheezy

This command instructs xen-create-image (the primary binary of the xen-tools toolkit) to create a guest domain with 512MB of memory, 2 vcpus, using storage from the vg0 volume group we created, use DHCP for networking, pygrub to extract the kernel from the image when booted and lastly we specify that we want to deploy a Debian Wheezy operating system.

This process will take a few minutes. Once it is complete, it will provide a summary of the installation. Take note of the root password for the guest.

Also see

* [xen-create-image(8) man page](http://manpages.debian.org/cgi-bin/man.cgi?query=xen-create-image&apropos=0&sektion=0&manpath=Debian+6.0+squeeze&format=html&locale=en)
* [Further articles on xen-tools](https://wiki.xenproject.org/wiki/Xen-tools)


# 11 Creating a PV (Paravirtualized) Guest manually

Not every distribution provides the xen-tools package for an automated PV creation and configuration, and some Xen users prefer more control of the setup process.

Alpine Linux is such a distro. It provides detailed instructions [on installing and starting a PV domU manually](http://wiki.alpinelinux.org/wiki/Create_Alpine_Linux_PV_DomU), using PVGRUB2 rather than pygrub.

It also provides valuable information regarding the startup options available.

# 12 Starting a console guest

To start the guest, run (as root or using sudo):

    xl create -c /etc/xen/tutorial-pv-guest.cfg

The -c in this command tells xl that we wish to connect to the guest virtual console, a paravirtualized serial port within the domain that xen-create-image configured to listen with a getty. This is analogous to running:

    xl create /etc/xen/tutorial-pv-guest.cfg && xl console tutorial-pv-guest

You can leave the guest virtual console by pressing ctrl+] and re-enter it by running the “xl console <domain>” command.

You can later shutdown this guest either from within the domain or from dom0 with the following:

    xl shutdown tutorial-pv-guest

That completes our section on setting up your first paravirtualized domain! If you don’t have any interest in setting up a HVM domain, skip ahead to Starting a GUI guest, below.

# 13 Creating a Windows HVM (Hardware Virtualized) Guest

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

# 14 Starting a GUI guest (with VNC server)

Here is the command to start the domain and connect to it via VNC from your graphical machine.

    xl create windows.cfg

The VNC display should be available on port 5900 of your dom0 IP, for instance using gvncviewer:

    gvncviewer <dom0-ip-address>:5900

If this does not work try it without the port number and if you are trying from a GUI on dom0, try specifying localhost instead of the dom0 ip:

    gvncviewer localhost

# 15 Conclusion

That concludes our introduction to the Xen Project software, by now you can setup both PV and HVM domains on a bare dom0 hypervisor!

You can now move onto building your own guest images or try out some prebuilt [Guest VM Images](https://wiki.xenproject.org/wiki/Guest_VM_Images).
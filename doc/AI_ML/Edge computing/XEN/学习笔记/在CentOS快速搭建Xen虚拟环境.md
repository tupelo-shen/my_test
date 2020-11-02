[TOC]

# 1 Scope

This guide will help you get from a baseline CentOS/x86_64 install to running the complete Xen-4 stack in just under 10 minutes, this includes getting your first VM up.


# 2 Pre-requisites

> Xen-4.4 and libxl
> 
> Note: All versions of Xen before version 4.4 had xm and xend enabled by default. The xen-4.4.1 (and newer) rpms instead enable xl support and no longer use xend. Please see [/MigratingToXl](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/MigratingToXl) for details on how to migrate from em rpms older than 4.4.1 to the new version

We assume you already have a fair idea of virtualisation, the difference between para-virt (pv) and fullvirt (hvm) and have a basic understanding of how networking on linux works, including bridge utils.

* Read the Release Notes at http://wiki.centos.org/Manuals/ReleaseNotes/Xen4-01 ; they cover important notes, known issues, workarounds and details on where to get help, should you need it.
* The machine hosting the Xen hypervisor should be a CentOS-6/x86_64 minimal install; before going any further, you should ensure that only the CentOS repos are enabled for yum, and that you have the latest updates applied. The Xen4 stack for CentOS has only been tested and verified on CentOS-6.4 and newer.
* We assume that the machine has 1.5GB or more RAM and more than 10GB of disk space for the root filesystem under /

# 3 Install the Xen4CentOS Stack

The software released for this stack is hosted in its own self contained repository on the CentOS mirror network. In order to enable this repository, ensure that the CentOS-Extras repo is enabled and type:

    yum install centos-release-xen

Xen needs a kernel built with "domain 0" support to operate. Xen4 provides an updated kernel; so the first thing to do is to run an update to pull the new kernel in:

    yum update

Once that is done, we can now install xen itself:

    yum install xen

During Xen and the kernel installation, a script called `grub-bootxen.sh` should have been run which makes sure grub is updated (`/boot/grub/grub.conf` for CentOS 6, `/boot/grub2/grub.cfg` for CentOS 7). For CentOS 6, if you look at your `/boot/grub/grub.conf` it should have something like this:

    title CentOS (3.4.46-8.el6.centos.alt.x86_64)
            root (hd0,0)
            kernel /xen.gz dom0_mem=1024M,max:1024M loglvl=all guest_loglvl=all
            module /vmlinuz-3.4.46-8.el6.centos.alt.x86_64 ro root=/dev/mapper/vg_xen01-lv_root rd_LVM_LV=vg_xen01/lv_swap rd_NO_LUKS  KEYBOARDTYPE=pc KEYTABLE=uk rd_NO_MD LANG=en_GB rd_LVM_LV=vg_xen01/lv_root SYSFONT=latarcyrheb-sun16 crashkernel=auto rd_NO_DM rhgb quiet
            module /initramfs-3.4.46-8.el6.centos.alt.x86_64.img

NOTE: There is now a bug in the CentOS 6 grub configuration script such that many people find the grub config is missing the last line (the initramfs line). You may have to manually add a line like the one above (a second 'module' line), with the appropriate initramfs.

If this is not set to the default kernel, edit the file and set it now. Once you **reboot**, verify that the new kernel is running with:

    uname -r

and verify that xen is running with:

    xl info

NOTE: There is a ballooning bug that causes a error log entry as described in [Bug 6893](http://bugs.centos.org/view.php?id=6893), so dom0_mem=1024M,max:1536M would get rid of that error message.

# 4 Setting up bridging

The standard method of giving your guests networking is to set up bridging; the default bridge that the toolstack expects is called xenbr0.

Basic how-to can be found in [/Xen4Networking6](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Networking6) or [/Xen4Networking7](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Networking7).

Full documentation for setting up and configuring bridges can be found in the [RHEL 6 documentation](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Deployment_Guide/s2-networkscripts-interfaces_network-bridge.html) or [RHEL 7 documentation](https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/ch-Configure_Network_Bridging.html).

# 5 启动第一个虚拟机

At this point you are now ready to bring up your first VM, and there are multiple ways of achieving this. For new users, who are looking for the easiest install path, the Libvirt process is recommended. Advanced users looking to hand setup the network, the backing filestore and the xen environment will most likely want to use the xen command line tools.

* With Xen CLI tools, including xl: [/Xen4Cli](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Cli)
* Using !LibVirt/ Virt-install and Virt-Manager with xen4centos: [/Xen4Libvirt](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Libvirt)



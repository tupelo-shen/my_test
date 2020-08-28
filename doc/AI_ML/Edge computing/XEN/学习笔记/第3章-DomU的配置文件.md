前面我们已经了解了如何管理Dom0，但是对于DomU的创建交给了virt-install工具完成。但是，有时候，我们需要从头创建DomU镜像。至于理由，管它呢😀。

我们知道，创建OS镜像的方法有许多种，我们将给出一些经常使用的方法的详细说明，对于其它的，会简要提及，但不会提供一个详尽的列表（而且非常无聊）。本章的目的就是，让您了解配置DomU文件系统时所拥有的选项范围，理解原理，以及步骤熟悉整个过程。

[TOC]

# 1 DomU 基本配置

我们后面提及的所有示例都是基于一个基本的DomU配置文件，如下所示：

    kernel = /boot/vmlinuz-2.6-xen.gz
    vif = ['']
    disk = ['phy:/dev/targetvg/lv,sda,w']

This specifies a kernel, a network interface, and a disk, and lets Xen use defaults for everything else. Tailor the variables, such as volume group and kernel name, to your site. As we mention elsewhere, we recommend including other variables, such as a MAC and IP address, but we’ll omit them during this chapter for clarity so we can focus on creating domU images.

> NOTE: This doesn’t include a ramdisk. Either add a ramdisk= line or include xenblk (and xennet if you plan on accessing the network before modules are available) in your kernel. When we compile our own kernels, we usually include the xenblk and xennet drivers directly in the kernel. We only use a ramdisk to satisfy the requirements of the distro kernels.

If you’re using a modular kernel, which is very likely, you’ll also need to ensure that the kernel has a matching set of modules that it can load from the domU filesystem. If you’re booting the domU using the same kernel as the dom0, you can copy over the modules like this (if the domU image is mounted on /mnt):

    # mkdir -p /mnt/lib/modules
    # cp -a /lib/modules/`uname -r` /mnt

Note that this command only works if the domU kernel is the same as the dom0 kernel! Some install procedures will install the correct modules automatically; others won’t. No matter how you create the domU, remember that modules need to be accessible from the domU, even if the kernel lives in the dom0. If you have trouble, make sure that the kernel and module versions match, either by booting from a different kernel or copying in different modules.

# 2 选择内核

Traditionally, one boots a domU image using a kernel stored in the dom0 filesystem, as in the sample config file in the last section. In this case, it’s common to use the same kernel for domUs and the dom0. However, this can lead to trouble—one distro’s kernels may be too specialized to work properly with another distro. We recommend either using the proper distro kernel, copying it into the dom0 filesystem so the domain builder can find it, or compiling your own generic kernel.

Another possible choice is to download Xen’s binary distribution, which includes precompiled domU kernels, and extracting an appropriate domU kernel from that.

Alternatively (and this is the option that we usually use when dealing with distros that ship Xen-aware kernels), you can bypass the entire problem of kernel selection and use PyGRUB to boot the distro’s own kernel from within the domU filesystem. For more details on PyGRUB, see Chapter 7. PyGRUB also makes it more intuitive to match modules to kernels by keeping both the domU kernel and its corresponding modules in the domU.

# 3 通过tar快速安装

Let’s start by considering the most basic install method possible, just to get an idea of the principles involved. We’ll generate a root filesystem by copying files out of the dom0 (or an entirely separate physical machine) and into the domU. This approach copies out a filesystem known to work, requires no special tools, and is easy to debug. However, it’s also likely to pollute the domU with a lot of unnecessary stuff from the source system and is kind of a lot of work.

A good set of commands for this “cowboy” approach might be:

    # xm block-attach 0 duncan.img /dev/xvda1 w 0
    # mke2fs -j /dev/xvda1
    # mount /dev/xvda1 /mnt
    # cd /
    # tar -c -f - --exclude /home --exclude /mnt --exclude /tmp --exclude \
      /proc --exclude /sys --exclude /var | ( cd /mnt/ ; tar xf - )
    # mkdir /mnt/sys
    # mkdir /mnt/proc

> NOTE: Do all this as root.

These commands, in order, map the backing file to a virtual device in the dom0, create a filesystem on that device, mount the filesystem, and tar up the dom0 root directory while omitting `/home`, `/mnt`, `/tmp`, `/proc`, `/sys`, and `/var`. The output from this tar command then goes to a complementary tar used to extract the file in `/mnt`. Finally, we make some directories that the domU will need after it boots. At the end of this process, we have a self-contained domU in `duncan.img`.

## 3.1 Why This Is Not the Best Idea

The biggest problem with the cowboy approach, apart from its basic inelegance, is that it copies a lot of unnecessary stuff with no easy way to clear it out. When the domU is booted, you could use the package manager to remove things or just delete files by hand. But that’s work, and we are all about avoiding work.

## 3.2 Stuff to Watch Out For

There are some things to note:

* You must mkdir /sys and /proc or else things will not work properly.

    The issue here is that the Linux startup process uses /sys and /proc to discover and configure hardware—if, say, /proc/mounts doesn’t exist, the boot scripts will become extremely annoyed.

* You may need to mknod /dev/xvda b 220 0.

    `/dev/xvd` is the standard name for Xen virtual disks, by analogy with the hd and sd device nodes. The first virtual disk is `/dev/xvda`, which can be partitioned into `/dev/xvda1`, and so on. The command

        # /mknod /dev/xvda b 220 0

    creates the node `/dev/xvda` as a block device (b) with major number 220 (the number reserved for Xen VBDs) and minor number 0 (because it’s `xvda`—the first such device in the system).

    > NOTE: On most modern Linux systems, udev makes this unnecessary.
    
* You may need to edit `/etc/inittab` and `/etc/securettys` so that `/dev/xvc0` works as the console and has a proper getty.    

    We’ve noticed this problem only with Red Hat’s kernels: for regular XenSource kernels (at least through 3.1) the default getty on tty0 should work without further action on your part. If it doesn’t, read on!

    The term console is something of a holdover from the days of giant time-sharing machines, when the system operator sat at a dedicated terminal called the system console. Nowadays, the console is a device that receives system administration messages—usually a graphics device, sometimes a serial console.

    In the Xen case, all output goes to the Xen virtual console, xvc0. The xm console command attaches to this device with help from xenconsoled. To log in to it, Xen’s virtual console must be added to /etc/inittab so that init knows to attach a getty.1 Do this by adding a line like the following:

    xvc:2345:respawn:/sbin/agetty -L xvc0

    (As with all examples in books, don’t take this construction too literally! If you have a differently named getty binary, for example, you will definitely want to use that instead.)

    You might also, depending on your policy regarding root logins, want to add /dev/xvc0 to /etc/securetty so that root will be able to log in on it. Simply append a line containing the device name, xvc0, to the file.

# 4   Using the Package Management System with an Alternate Root

Another way to obtain a domU image would be to just run the setup program for your distro of choice and instruct it to install to the mounted domU root. The disadvantage here is that most setup programs expect to be installed on a real machine, and they become surly and uncooperative when forced to deal with paravirtualization.

Nonetheless, this is a viable process for most installers, including both RPM and Debian-based distros. We’ll describe installation using both Red Hat’s and Debian’s tools.

## 4.1 Red Hat, CentOS, and Other RPM-Based Distros

On Red Hat–derived systems, we treat this as a package installation, rather than a system installation. Thus, rather than using anaconda, the system installer, we use yum, which has an installation mode suitable for this sort of thing. First, it’s easiest to make sure that SELinux is disabled or nonenforcing because its extended permissions and policies don’t work well with the installer.2 The quickest way to do this is to issue echo 0 >/selinux/enforce. A more permanent solution would be to boot with selinux=0 on the kernel command line.

> NOTE: Specify kernel parameters as a space-separated list on the “module” line that loads the Linux kernel—either in /boot/grub/menu.lst or by pushing e at the GRUB menu.

When that’s done, mount your target domU image somewhere appropriate. Here we create the logical volume malcom in the volume group scotland and mount it on `/mnt`:

    # lvcreate -L 4096 -n malcom scotland
    # mount /dev/scotland/malcom /mnt/

Create some vital directories, just as in the tar example:

    # cd /mnt
    # mkdir proc sys etc

Make a basic fstab (you can just copy the one from dom0 and edit the root device as appropriate—with the sample config file mentioned earlier, you would use /dev/sda):

    # cp /etc/fstab /mnt/etc
    # vi /mnt/etc/fstab

Fix modprobe.conf, so that the kernel knows where to find its device drivers. (This step isn’t technically necessary, but it enables yum upgrade to properly build a new initrd when the kernel changes—handy if you’re using PyGRUB.)

    # echo "alias scsi_hostadapter xenblk\nalias eth0 xennet" > /mnt/etc/modprobe.conf

At this point you’ll need an RPM that describes the software release version and creates the yum configuration files—we installed CentOS 5, so we used centos-release-5-2.el5.centos.i386.rpm.

    # wget http://mirrors.prgmr.com/os/centos/5/os/i386/CentOS/centos-release-5.el5.centos.i386.rpm
    # rpm -ivh --nodeps --root /mnt centos-release-5-2.el5.centos.i386.rpm

In this case we just used the first mirror that we could find. You may want to look at a list of CentOS mirrors and pick a more suitable one. Next we install yum under the new install tree. If we don’t do this before installing other packages, yum will complain about transaction errors:

    # yum --installroot=/mnt -y install yum

Now that the directory has been appropriately populated, we can use yum to finish the install.

    # yum --installroot=/mnt -y groupinstall Base

And that’s really all there is to it. Create a domU config file as normal.

## 4.2 Debootstrap with Debian and Ubuntu

Debootstrap is quite a bit easier. Create a target for the install (using LVM or a flat file), mount it, and then use debootstrap to install a base system into that directory. For example, to install Debian Etch on an x68_64 machine:

    # mount /dev/scotland/banquo /mnt
    # debootstrap --include=ssh,udev,linux-image-xen-amd64 etch /mnt http://mirrors.easynews.com/
    linux/debian

Note the --include= option. Because Xen’s networking requires the hotplug system, the domU must include a working install of udev with its support scripts. (We’ve also included SSH, just for convenience and to demonstrate the syntax for multiple items.) If you are on an i386 platform, add libc6-xen to the include list. Finally, to ensure that we have a compatible kernel and module set, we add a suitable kernel to the include= list. We use linux-imagexen- amd64. Pick an appropriate kernel for your hardware.

If you want to use PyGRUB, create /mnt/etc/modules before you run debootstrap, and put in that file:

    xennet
    xenblk

Also, create a /mnt/boot/grub/menu.lst file as for a physical machine. If you’re not planning to use PyGRUB, make sure that an appropriate Debian kernel and ramdisk are accessible from the dom0, or make sure that modules matching your planned kernel are available within the domU. In this case, we’ll copy the sdom0 kernel modules into the domU.

    # cp -a /lib/modules/<domU kernel version> /mnt/lib/modules

When that’s done, copy over /etc/fstab to the new system, editing it if necessary:

    # cp /etc/fstab /mnt/etc

## 4.3 Renaming Network Devices

Debian, like many systems, uses udev to tie eth0 and eth1 to consistent physical devices. It does this by assigning the device name (ethX) based on the MAC address of the Ethernet device. It will do this during debootstrap—this means that it ties eth0 to the MAC of the box you are running debootstrap on. In turn, the domU’s Ethernet interface, which presumably has a different MAC address, will become eth1.3 You can avoid this by removing /mnt/etc/udev/rules.d/z25_persistent-net.rules, which contains the stored mappings between MAC addresses and device names. That file will be recreated next time you reboot. If you only have one interface, it might make sense to remove the file that generates it, /mnt/etc/udev/rules.d/z45_persistent-net-generator.rules.

    # rm /mnt/etc/udev/rules.d/z25_persistent-net.rules

Finally, unmount the install root. Your system should then essentially work. You may want to change the hostname and edit /etc/inittab within the domU’s filesystem, but these are purely optional steps.

    # umount /mnt

Test the new install by creating a config file as previously described (say, /etc/xen/banquo) and issuing:

    # xm create -c /etc/xen/banquo

# 5 QEMU Install

## 5.1 QEMU’S RELATION TO XEN

# 6 virt-install—Red Hat’s One-Step DomU Installer

# 7 Converting VMware Disk Images

## 7.1 PYGRUB, PYPXEBOOT, AND FRIENDS

## 7.2 RPATH’S RBUILDER: A NEW APPROACH

# 8 Mass Deployment

## 8.1 Manual Deployment

## 8.2 QEMU and Your Existing Infrastructure

### 8.2.1 Setting Up SystemImager

## 8.3 Installing pypxeboot

## 8.4 Automated Installs the Red Hat Way

# 9 And Then . . .

# 10 Footnotes

# 11 Navigation
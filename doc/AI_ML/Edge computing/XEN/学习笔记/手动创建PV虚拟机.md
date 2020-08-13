[TOC]

# 1 获取镜像文件

下载最新的alpine-virt镜像文件，下载地址：[https://alpinelinux.org/downloads/](https://alpinelinux.org/downloads/)。

> 注意下载的时候选择合适的版本，在这儿应该选择给虚拟系统使用的镜像

在本文中，我们使用目录`/data/`作为下载和存储的位置。

# 2 挂载镜像文件

挂载镜像文件，方便读取`kernel`和`initramfs`:

    mount -t iso9660 -o loop /data/alpine-virt-3.8.0-x86_64.iso  /media/cdrom

现在，我们可以在目录`/media/cdrom/boot/vmlinuz-virt`得到`kernel`，在目录`/media/cdrom/boot/initramfs-virt`下得到`initramfs`。

或者，您可以使用`uniso`或`p7zip`将内容解压缩到一个临时区域。

# 3 创建硬盘镜像

现在，我们创建一个空文件，作为DomU的硬盘驱动器（本示例中，将创建一个3GB的硬盘）：

    dd if=/dev/zero of=/data/a1.img bs=1M count=3000

或者，Dom0上已经有一个LVM卷组（假设叫`vg0`），还有空间，可以为`alpine`创建一个逻辑卷：

    sudo lvcreate -n alpine -L 10g vg0

# 4 创建引导ISO镜像的DomU配置文件

创建一个基本的DomU配置文件，让我们可以启动PV虚拟机iso镜像（通常保存在`/etc/xen/`，可以自行选择）：

    # Alpine Linux PV DomU
    # 要安装的内核路径
    kernel = "/media/cdrom/boot/vmlinuz-virt"
    ramdisk = "/media/cdrom/boot/initramfs-virt"
    extra="modules=loop,squashfs console=hvc0"

    # HDD和iso镜像文件的路径
    disk = [
            'format=raw, vdev=xvda, access=w, target=/data/a1.img',
            'format=raw, vdev=xvdc, access=r, devtype=cdrom, target=/data/alpine-virt-3.8.0-x86_64.iso'
           ]

    # 网络配置
    vif = ['bridge=br0']

    # DomU相关信息设置
    memory = 512
    name = "alpine-a1"
    vcpus = 1
    maxvcpus = 1

如果使用`LVM`，在硬盘指定的时候，使用`/dev/vg0/alpine`取代`/data/a1.img`。

# 5 安装虚拟机

现在我们已经有了所有必要文件，我们启动DomU，进行安装：

    xl create -f /etc/xen/a1.cfg  -c

使用`root`账号登陆虚拟机，没有密码。然后执行`setup-alpine`命令完成基本配置。具体的可以参考[Installation_Handbook](https://wiki.alpinelinux.org/wiki/Installation#Installation_Handbook)的第3部分。

完成基本的配置之后，会询问你安装`Alpine`的位置，选择`xvda`和`sys`。这将会为你创建3个分区：`xvda1`作为`/boot`分区，`xvda2`作为`swap`分区，`xvda3`作为`/`分区。过程如下所示：

    Available disks are:
      xvda  (3.1 GB  )
    Which disk(s) would you like to use? (or '?' for help or 'none') [none] xvda
    The following disk is selected:
      xvda  (3.1 GB  )
    How would you like to use it? ('sys', 'data' or '?' for help) [?] sys
    WARNING: The following disk(s) will be erased:
      xvda  (3.1 GB  )
    WARNING: Erase the above disk(s) and continue? [y/N]: y
    Initializing partitions on /dev/xvda...
    Creating file systems...
    Installing system on /dev/xvda3:

    Installation is complete. Please reboot.

    #

当你重启时，pv的引导程序，`pvgrub`将会把`/boot/grub/grub.cfg`作为它的菜单。所以，首先应该创建这个文件。

挂载`boot`分区，并且为`pvgrub`创建`grub/grub.cfg`配置文件。（注意，从2013年开始，`grub.cfg`支持`pvgrub2`，其取代了`pvgrub1`和它的`menu.lst`文件）。

    mount /dev/xvda1 /mnt
    mkdir /mnt/grub

安装一个文本编辑器，比如`nano`或`vim`：

    apk add nano

如果使用`nano`，

    nano /mnt/grub/grub.cfg

然后，将下面的内容添加到文件中

    menuentry 'alpine-xen' {
        set root=(xen/xvda,msdos1)
        linux /boot/vmlinuz-virt root=/dev/xvda3 modules=ext4
        initrd /boot/initramfs-virt 
     }

最后，`Ctrl-S`保存，`Ctrl-X`退出。

取消挂载并关机。

    umount /mnt
    poweroff

# 6 调整DomU配置文件，从新安装位置启动

文件内容如下，

    # Alpine Linux PV DomU

    kernel = "/usr/lib/xen/boot/pv-grub-x86_64.gz"
    # 配置硬盘和iso镜像文件路径
    disk = [
            'format=raw, vdev=xvda, access=w, target=/data/a1.img'
           ]
    # 网络配置
    vif = ['bridge=br0'] # 需要注意，与你的主机的网络名称相同
    # DomU设置
    memory = 512
    name = "alpine-a1"
    vcpus = 1
    maxvcpus = 1

Dom0中的`pvgrub`的名称和位置是特定于某个版本的，因此，可能需要修改上面的`kernel=`行。 比如，在`Debian 10`中，该值应该为`/usr/lib/grub-xen/grub-x86_64-xen.bin`。

记得下载掉CDROM中的镜像文件：

    umount /media/cdrom

下一次启动时，将会出现`grub`引导菜单，可以引导你创建的虚拟机。
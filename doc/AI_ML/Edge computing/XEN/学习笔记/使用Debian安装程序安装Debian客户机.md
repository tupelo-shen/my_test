[TOC]

# 1 Debian Installer

[Debian Installer](http://www.debian.org/devel/debian-installer/) 是一个标准的Debian安装程序。从`Lenny(Debian 5.0)`开始，它支持Xen作为PV客户机的网络引导安装（包括`i386`和`amd64`）。从`Squeeze(Debian 6.0)`开始，它支持`CD-ROM`、`DVD`和`蓝光DVD`的安装。

使用`Debian Installer`安装Xen客户机的好处是，你能够使用在裸机系统上安装系统时所有的技术（比如`preseeding`）。安装机制与安装裸机Debian系统时相似。安装一个Xen客户机的`Debian Installer`，可以生成一个配置完全的Debian系统，而不需要手动配置诸如`/etc/fstab`之类的东西。

# 2 网络引导安装

网络引导安装是安装Debian的一种快速、简单的方式。首先，下载一个小的安装程序（大约20M），然后引导kernel和initrd为客户机。之后，`Debian Installer`将会接管工作，可以直接从网络上安装客户机。它的优点是，如果你只安装一个客户机程序，只下载所需的东西；如果你批量安装客户机，它会创建一个本地镜像（可能会需要大量的带宽）或者安装一个缓存代理，比如[`apt-cacher`](http://packages.debian.org/search?keywords=apt-cacher)，来减少外部下载。

## 2.1 选择镜像

如果你不是使用自己的镜像网站或者类似于`apt-cacher`的工具，需要选择一个[Debian的镜像网站](http://www.debian.org/mirror/list)。通常，需要选择离自己最近的镜像网站。另外，你还可以使用[http://cdn.debian.net](http://cdn.debian.net)服务（非官方），自动选择一个就近镜像网站。

下面的操作都是假定使用了[http://cdn.debian.net](http://cdn.debian.net)服务，替换掉你选择的镜像网站或者你自己的`apt-cacher`或镜像名称。

## 2.2 下载`Debian Installer`镜像

接下来就可以下载`Debian Installer`的内核和initrd。只需下载一次，因为可以重复使用。

可以从[amd64](http://cdn.debian.net/debian/dists/squeeze/main/installer-amd64/current/images/netboot/xen/)或者[i386](http://cdn.debian.net/debian/dists/squeeze/main/installer-i386/current/images/netboot/xen/)下载`vmlinuz`和`initrd.img`，保存到你的Xen系统中。本文假设保存位置分别为`/scratch/debian/squeeze/amd64`和`/scratch/debian/squeeze/i386`。

另外，假设安装`Squeeze`的Debian系统。如果想要其它版本的系统，自行下载。

## 2.3 创建初始客户机配置

> 某些版本的Debian附带了一个`xm-debian.cfg`示例配置文件，其包含Python代码，来自动执行部分安装程序。使用了不可靠的xend工具，而且也不能使用XL工具。强烈建议不要使用这个文件作为新安装系统的参考配置文件。新版本的Debian附带了一个`debian.cfg`文件，没有包含Python代码，可以使用它作为修改配置文件的起点。

第一次引导，需要配置客户机使用上面下载的`vmlinux`和`initrd.gz`，且添加一行命令指定安装程序，如下所示：

    kernel = "/scratch/debian/squeeze/amd64/vmlinuz"
    ramdisk = "/scratch/debian/squeeze/amd64/initrd.gz"
    extra = "debian-installer/exit/always_halt=true -- console=hvc0"

配置文件的其余部分取决于你对客户机配置的要求。通常，需要配置硬盘和网络设备。在安装过程中，需要一个具有外部连接（或连接到本地镜像/缓存）的网络设备。

为了使用`Debian Installer`，应该指定整个硬盘，而不是某个分区。换言之，应该使用xvda设备，而不是xvda1、xvda2等。参见下面的示例：

    name = "debian-installer"

    memory = 256

    disk = ['phy:/dev/VG/debian-installer,xvda,w']
    vif = [' ']

    kernel = "/scratch/debian/squeeze/amd64/vmlinuz"
    ramdisk = "/scratch/debian/squeeze/amd64/initrd.gz"
    extra = "debian-installer/exit/always_halt=true -- console=hvc0"

需要创建`debian-installer`的逻辑卷：

    # lvcreate -L 4G -n debian-installer /dev/VG

关于客户机配置语法，请参考[Xen Man Pages](https://wiki.xen.org/wiki/Xen_Man_Pages)，尤其是[xl.cfg(5)](http://xenbits.xen.org/docs/unstable/man/xl.cfg.5.html)帮助手册。

## 2.4 安装Debian

Now that you have created the guest configuration you can start the guest and connect to its console. e.g. assuming your guest configuration file is /etc/xen/debian-installer.cfg:

    # xl create -c /etc/xen/debian-installer.cfg

At this point the guest will boot and Debian Installer will start. Now you can install Debian like you normally would, perhaps following the [Debian Installation Guide](http://www.debian.org/releases/stable/install)

## 2.5 完成客户机配置

Once installation has completed the guest will shutdown and exit. At this point you should remove the kernel, initrd and extra lines from the guest configuration and replace them with simply:

    bootloader = "pygrub"

Keep the rest of the configuration file the same.

Now you can start the guest again to boot your newly installed Debian guest!

    # xl create -c /etc/xen/debian-installer.cfg

# 3 CD-ROM安装

## 3.1 选择ISO镜像

Full support for installing a Xen PV guest (32 or 64 bit) from an ISO image was added to Debian in the Squeeze (6.0.0) release.

Downloads of Debian CD images are available from the [Debian CD Image](http://www.debian.org/CD/) site. Several options are available for downloading, it is preferred to use [jigdo](http://atterer.org/jigdo/) (a tool which can reconstitute an ISO image from a Debian mirror) or [BitTorrent](http://bitconjurer.org/BitTorrent/) rather than direct download of ISO (which may be throttled or otherwise slower). More info on downloading using jigdo is available on the Debian CD Jigdo page. Selection of a Debian mirror, which is required for a jigdo download, is discussed above.

Debian provides several different flavours of the first ISO image, only some of which support installation as a Xen PV guest. The main two flavours which support Xen PV are:

* multi-arch (i386 & amd64) netinst CD Images of Debian Squeeze 6.0.4 (jigdo,torrent)
* multi-arch (i386, amd64 & source) DVD Images of [Debian Squeeze 6.0.4](http://www.debian.org/releases/squeeze/) (jigdo,torrent)

In both cases the image supports installation of both 32 bit (i386) and 64 bit (amd64) Debian guests.

Information on verifying the authenticity of these images is available here.

If the machine on which you want to install guests has a network connection then the netinst option is preferable since it reduces the initial download and will then only download precisely what you need for your configuration. Note that if you have a network connection then booting a netboot image (as described above) is an even better option.

Having selected the appropriate first ISO you can then combine this with the images from the standard set in order to make up a complete set of Debian packages installable from ISO.

In addition to the above stable release images there are daily builds of multi-arch netinst CD images (jigdo,torrent) and weekly builds of the multi-arch DVD images (jigdo, torrent) for the testing release. Note that the testing release of Debian is by definition a work in progress and may contain transient issues etc.

## 3.2 创建初始客户机配置

## 3.3 安装Debian

## 3.4 完成客户机配置

# 4 高级选项

## 4.1 Preseeding

# 5 其它安装方式

# 6 常用链接

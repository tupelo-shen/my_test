在Ubuntu12.04上安装Xen

[TOC]

为了测试，我们需要建立几个虚拟服务器。CentOS从RHEL6开始放弃了对Xen的支持。所以我们选择`Ubuntu16.04`作为Dom0的内核。

本文档详细记录了安装和配置Xen的每一步，访问方式使用SSH（需要安装网络）。

# 1 安装Ubuntu

安装Ubuntu 16.04。当询问分区时，选择`Guided`，然后建立LVM，大小设为25G以上（根据你的机器自行设定），为以后的应用留下足够的空间。

选择安装基本的Ubuntu服务器和虚拟机主机。后者将会安装KVM，而且会添加`libvirt`工具（本文不打算使用，但是，在将来简化创建是很方便的）。我们可以在安装完成后，删除KVM，然后再安装Xen。

# 1.1 更新系统源

Ubuntu更新源
安装Ubuntu16.04后，使用国外源太慢了，修改为国内源会快很多。

修改阿里源为Ubuntu 18.04默认的源

备份/etc/apt/sources.list

    #备份
    cp /etc/apt/sources.list /etc/apt/sources.list.bak

在/etc/apt/sources.list文件前面添加如下条目
    
    #添加阿里源
    deb http://mirrors.aliyun.com/ubuntu/ bionic main restricted universe multiverse
    deb http://mirrors.aliyun.com/ubuntu/ bionic-security main restricted universe multiverse
    deb http://mirrors.aliyun.com/ubuntu/ bionic-updates main restricted universe multiverse
    deb http://mirrors.aliyun.com/ubuntu/ bionic-proposed main restricted universe multiverse
    deb http://mirrors.aliyun.com/ubuntu/ bionic-backports main restricted universe multiverse
    deb-src http://mirrors.aliyun.com/ubuntu/ bionic main restricted universe multiverse
    deb-src http://mirrors.aliyun.com/ubuntu/ bionic-security main restricted universe multiverse
    deb-src http://mirrors.aliyun.com/ubuntu/ bionic-updates main restricted universe multiverse
    deb-src http://mirrors.aliyun.com/ubuntu/ bionic-proposed main restricted universe multiverse
    deb-src http://mirrors.aliyun.com/ubuntu/ bionic-backports main restricted universe multiverse

最后执行如下命令更新源
    
    ##更新
    sudo apt-get update
    sudo apt-get upgrade

另外其他几个国内源如下：

163源

    ##163源
    deb http://mirrors.163.com/ubuntu/ bionic main restricted universe multiverse
    deb http://mirrors.163.com/ubuntu/ bionic-security main restricted universe multiverse
    deb http://mirrors.163.com/ubuntu/ bionic-updates main restricted universe multiverse
    deb http://mirrors.163.com/ubuntu/ bionic-proposed main restricted universe multiverse
    deb http://mirrors.163.com/ubuntu/ bionic-backports main restricted universe multiverse
    deb-src http://mirrors.163.com/ubuntu/ bionic main restricted universe multiverse
    deb-src http://mirrors.163.com/ubuntu/ bionic-security main restricted universe multiverse
    deb-src http://mirrors.163.com/ubuntu/ bionic-updates main restricted universe multiverse
    deb-src http://mirrors.163.com/ubuntu/ bionic-proposed main restricted universe multiverse
    deb-src http://mirrors.163.com/ubuntu/ bionic-backports main restricted universe multiverse


如果在执行命令`apt-get update`时，报错：`Hash Sum mismatch`。通过搜索资料发现使用apt命令的时候附加参数-o Acquire-by-hash=yes。但是这个命令是在`apt 1.2.10`以及更高版本进行支持的（Ubuntu 16.04支持这个命令）。

# 2 系统安装

系统安装完成后，删除KVM，再安装Xen：

    sudo -s
    # apt-get remove kvm
    # 使用64位CPU
    apt-cache search xen-hypervisor     # 搜索适合的xen版本
    apt-get install xen-hypervisor-amd64
    sed -i 's/GRUB_DEFAULT=.*\+/GRUB_DEFAULT="Xen 4.9-amd64"/' /etc/default/grub

    # 使用32位CPU
    apt-get install xen-hypervisor-i386
    sed -i 's/GRUB_DEFAULT=.*\+/GRUB_DEFAULT="Xen 4.9-i386"/' /etc/default/grub

    sed -i 's/TOOLSTACK=.*\+/TOOLSTACK="xl"/' /etc/default/xen
    mv /etc/grub.d/10_linux /etc/grub.d/50_linux

> 注意：如果不使用`/etc/grub.d/10_linux`替换`/etc/grub.d/50_linux`，当你重新启动尝试运行`xm list`时，可能会报错："Can't find hypervisor information in sysfs!"。但是，这是在12.04版本上发生的，而我使用的Ubuntu是16.04版本，而且Xen版本是4.9，管理工具也已经替换成了`xl`,而不再是`xm`。所以，最后两行代码可以不用。

## 2.1 限制Dom0的RAM

你可能想要限制Dom0所使用的内存数量，否则你可能会发现无法启动任何虚拟机。为此，使用root权限运行下面的命令：

    nano /etc/default/grub
    # 添加下面这行代码，限制Dom0的内存。如果你的Ubuntu不是一个基本的系统，
    # 而是复杂的一个系统，这个限制值需要重新考虑，要不然无法启动Xen。
    GRUB_CMDLINE_XEN="dom0_mem=2048M"
    # 保存并关闭

    update-grub
    reboot

系统重新启动后，可以使用命令`xl list`查看，结果如下：

    Name            ID   Mem VCPUs      State   Time(s)
    Domain-0        0    512     4     r-----     33.0

现在Xen已经运行起来了，接下俩设置网络。

# 3 网络

编辑文件`/etc/network/interfaces`，创建接口，重新启动网络即可。

    sudo nano /etc/network/interfaces
    # 添加下面内容（假设你想要虚拟机使用eth0-根据自身情况进行更改）
    auto enp0s3
    iface enp0s3 inet manual

    auto xenbr0
    iface xenbr0 inet dhcp
    bridge_ports eth0

重启网络：

    sudo /etc/init.d/networking restart

现在我们可以开始创建第一个DomU客户机域了！😀😀😀！

# 4 手动创建一个Ubuntu的DomU域

让我们先来创建一个Ubuntu的DomU域。为此，我们需要获取LVM虚拟卷组的名称，以便向其添加一个卷组。命令如下：

    sudo pvs

下面是输出：

      PV         VG        Fmt  Attr PSize  PFree
      /dev/sda5  ubuntu-vg lvm2 a--  99.28g 4.00m

创建一个逻辑卷，这个逻辑卷将会作为DomU的硬盘。我们称它为ubuntu，基于逻辑卷组-`ubuntu-vg`。

    sudo lvcreate -L 4G -n ubuntu ubuntu-vg

接下来，我们将获取网络引导镜像文件，通过网络进行安装：

    sudo -s
    mkdir -p /var/lib/xen/images/ubuntu-netboot
    cd /var/lib/xen/images/ubuntu-netboot

    #64 bit users
    wget http://mirror.as29550.net/archive.ubuntu.com/ubuntu/dists/precise/main/installer-amd64/current/images/netboot/xen/initrd.gz
    wget http://mirror.as29550.net/archive.ubuntu.com/ubuntu/dists/precise/main/installer-amd64/current/images/netboot/xen/vmlinuz

    #32 bit users
    wget http://mirror.as29550.net/archive.ubuntu.com/ubuntu/dists/precise/main/installer-i386/current/images/netboot/xen/initrd.gz
    wget http://mirror.as29550.net/archive.ubuntu.com/ubuntu/dists/precise/main/installer-i386/current/images/netboot/xen/vmlinuz

    wget http://mirrors.ustc.edu.cn/debian/dists/stable/main/installer-amd64/current/images/netboot/xen/initrd.gz
    wget http://mirrors.ustc.edu.cn/debian/dists/stable/main/installer-amd64/current/images/netboot/xen/vmlinuz
    # 中国大陆用户可以选择下面的镜像
    wget http://mirrors.ustc.edu.cn/debian/dists/jessie/main/installer-amd64/current/images/netboot/xen/initrd.gz
    wget http://mirrors.ustc.edu.cn/debian/dists/jessie/main/installer-amd64/current/images/netboot/xen/vmlinuz
    wget http://mirrors.ustc.edu.cn/debian/dists/jessie/main/installer-amd64/current/images/netboot/xen/debian.cfg

    wget http://mirrors.aliyun.com/ubuntu/dists/xenial/main/installer-amd64/current/images/netboot/xen/xm-debian.cfg

> 注意：我的wget不知道为什么下载不下来？？？我是手动下载到相应目录下的。

接下来，我们创建虚拟机的配置文件，限制其内存为2048M，将文件命名为`ubuntu.cfg`。不要忘记替换我们之前查找的卷组名称！

    nano /etc/xen/ubuntu.cfg
    # 插入下面的内容
    name = "ubuntu"
    memory = 256 
    disk = ['phy:/dev/<VG NAME>/ubuntu,xvda,w']
    vif = [' ']
    kernel = "/var/lib/xen/images/ubuntu-netboot/vmlinuz"

    ramdisk = "/var/lib/xen/images/ubuntu-netboot/initrd.gz"
    extra = "debian-installer/exit/always_halt=true -- console=hvc0"

启动DomU并运行安装程序，无需赘言：

    xm create /etc/xen/ubuntu.cfg -c

> xc: error: panic: xc_dom_core.c:702: xc_dom_find_loader: no loader found: Invalid kernel
> 
> 1. 原因常见为下载的内核版本不对。

You should see a Linux system start booting in your console, follow the installer through. Once complete, the DomU will shutdown. Now we're going to want to set-up a bootloader and configuring our DomU to use it

    sudo -s
    ln -s /usr/lib/xen-4.1/bin/pygrub /usr/bin/pygrub
    nano /etc/xen/ubuntu.cfg

    #Add this line
    bootloader = "pygrub"

    #Remove or comment out the kernel, ramdisk and extra lines
    #Save and close

We can start the virtual machine with the same command we used before

    sudo xm create /etc/xen/ubuntu.cfg -c

See the [troubleshooting section](https://www.bentasker.co.uk/documentation/linux/188-setting-up-xen-on-ubuntu-12-04#troubleshooting) if you receive the following error

    Error: (2, 'Invalid kernel', 'elf_xen_note_check: ERROR: Will only load images built for the generic loader or Linux images')

# 5 使用安装盘安装

有时候，我们可能无法使用网络安装。幸运的是，我们还可以选择ISO镜像文件进行安装。除了配置文件以外，几乎与网络引导的过程差不多。假设，ISO镜像存储在`/root/`目录下。按照下面的内容修改`/etc/xen/ubuntu.cfg`：

    name = "ubuntu"
    memory = 256 
    disk = ['phy:/dev/{VG NAME}/ubuntu,xvda,w','file:/root/MYINSTALLDISK.iso,hdc:cdrom,r']
    vif = [' ']
    extra = "debian-installer/exit/always_halt=true -- console=hvc0"
    vnc = 1

    boot="dc"

What we've changed is the Disk definition (to include the CDROM) and we've added a definition for the boot order. The inclusion of the VNC line allows us to connect to the server via VNC (which is useful if your installer only has a graphical mode).

We've also removed the paths to the kernel and ramdisk because they're on the CD/DVD and it's own bootloader will take care of that.

Once the install is complete, we'll need to either remove the CDROM definition, or at least swap the boot order round (to cd)

# 6 自动启动DomU

We've gone to the trouble of creating our VM's, but as it stands whenever the host system is restarted, we'll have to manually fire the things up! Setting DomU's to start automatically is simply a case of creating a symbolic link though

    sudo ln -s /etc/xen/ubuntu.cfg /etc/xen/auto 

Obviously use the relevant configuration filename and path

# 7 创建一个Clone版本

Important: Don't do this when the DomU is running - there'll be things changing on disk all the time!

There may be a myriad of reasons why you want to create a clone of the image we just created - as I'm primarily using the VM's as testing environments I want to be able to restore the system back to it's original state quickly, or to launch several identical systems so that I can play around with other bits.

We'll still need to create the Xen config files, but the first step is to clone the Logical Volume

    # This command is much quicker, but can be buggy sometimes
    lvcreate -L 25G -s -n ubuntu_clone /dev/<VGNAME>/ubuntu

    #These have a better chance of success but can take quite a while
    lvcreate -L 25G -n ubuntu_clone <VGNAME>
    dd if=/dev/<VGNAME>/ubuntu of=/dev/<VGNAME>/ubuntu_clone

Obviously if your original logical volume wasn't called ubuntu you'll need to change the name

We can now create a new Xen config file referencing our new Logical volume. Hint: You can use more or less any name you want instead of ubuntu_clone, so long as it's unique to the LVM Virtual Group

# Troubleshooting an installed VM

If when trying to start the Ubuntu VM we created, you receive an error about an invalid kernel, it's because the Xen compatible kernel wasn't installed (if there's an option in the installer, I must have missed it!). There are two packages we need to install.

In theory it's possible to mount the Logical volumes contained within our VM's logical volume, but in all honesty it's just as easy to get Xen to do that for us. Reconfigure the Xen config file to that state it was for install (i.e. remove bootloader and re-insert Kernel, ramdisk and extra).

Fire up the VM, and as soon as you hit the first installation question, press Esc on your keyboard. Select Execute a Shell from the resulting menu and then follow the steps below

    mount /dev/xvda1 /mnt

    echo "91.189.91.13      security.ubuntu.com" >> /mnt/etc/hosts
    #This next line may be incorrect for you.. we'll fix in a minute if so
    echo "91.189.92.200      gb.archive.ubuntu.com" >> /mnt/etc/hosts

    ifconfig eth0 up
    dhclient eth0 #Ignore errors about resolv.conf
    mount -o bind /proc /mnt/proc
    mount -o bind /dev /mnt/dev
    chroot /mnt
    apt-get install linux-virtual grub-legacy-ec2

If apt fails because it couldn't look up an address, the quickest way is to ping that address from another machine and then add a line to /etc/hosts, then re-run. Once the packages are installed

    exit
    umount /mnt/dev
    umount /mnt/proc
    umount /mnt
    exit

Finally, choose Abort Installation and wait whilst your VM shuts down. Re-enable the bootloader declaration in the config file and disable kernel, ramdisk and extra. The VM should run successfully when you exectute

    xm create /etc/xen/ubuntu.cfg -c

# Non-Work Uses

We've covered why you might want to set this up for work, but there are just as many non-work reasons, including Creating a DOS Games Server!
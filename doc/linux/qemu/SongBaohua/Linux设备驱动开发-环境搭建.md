<h1 id="0">0 目录</h1>
* [1 环境介绍](#1)
* [2 下载Linux内核](#2)
* [3 安装交叉编译工具](#3)
* [4 编译Linux内核](#4)
* [5 编译安装QEMU](#5)
* [6 测试QEMU和内核](#6)
* [7 制作根文件系统](#7)
    - [7.1 根文件系统存放位置](#7.1)
    - [7.2 下载、编译和安装busybox](#7.2)
    - [7.3 rootfs目录结构](#7.3)
    - [7.4 制作rootfs文件系统镜像](#7.4)
* [8 系统启动运行](#8)


---

<h1 id="1">1 环境介绍</h1>

主机环境：

    Win10 64 + VirtualBox 6.0 + ubuntu16.04（32位）-4.10.0-28-generic

u-boot 版本：

    u-boot-2015-04

Linux kernel版本：

    linux-4.4.203

busybox版本：

    busybox-1.31.1.tar.bz2

交叉编译工具链：

    arm-linux-gnueabi-

qemu版本：

    QEMU emulator version 4.1.93 (v4.2.0-rc3-dirty)


<h1 id="2">2 下载Linux内核</h1>

[linux kernel下载地址](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/) :

    https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/

方法1: 直接下载某一个版本的内核

    wgets https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.203.tar.xz

方法2： 获取最新的内核版本

    git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

<h1 id="3">3 安装交叉编译工具</h1>

ubuntu安装方法：

    sudo apt-get install gcc-arm-linux-gnueabi


<h1 id="4">4 编译Linux内核</h1>

解压下载的内核tarball：

    tar -xvf linux-4.4.203.tar.xz

清理编译环境的命令：

    make mrproper

生成`vexpress`开发板的`.config`文件：

    make CROSS_COMPILE=arm-linux-gnueabi- ARCH=arm vexpress_defconfig

编译内核：

    make CROSS_COMPILE=arm-linux-gnueabi- ARCH=arm

<h1 id="5">5 编译安装QEMU</h1>

具体的QEMU安装方法可以参考另一篇文章-[Ubuntu-16.04编译Qemu](https://blog.csdn.net/shenwanjiang111/article/details/82898119)

<h1 id="6">6 测试QEMU和内核</h1>

运行下面的命令：

    qemu-system-arm -M vexpress-a9 -m 512M -kernel /home/qemu4/qemu/linux/linux-4.4.203/arch/arm/boot/zImage -dtb  /home/qemu4/qemu/linux/linux-4.4.203/arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic -append "console=ttyAMA0"

    # 请将/home/qemu4/qemu/linux/linux-4.4.203/修改为自己的目录

运行结果：

    #0: ARM AC'97 Interface PL041 rev0 at 0x10004000, irq 33
    input: ImExPS/2 Generic Explorer Mouse as /devices/platform/smb/smb:motherboard/smb:motherboard:iofpga@7,00000000/10007000.kmi/serio1/input/input2
    VFS: Cannot open root device "(null)" or unknown-block(0,0): error -6
    Please append a correct "root=" boot option; here are the available partitions:
    1f00          131072 mtdblock0  (driver?)
    1f01           32768 mtdblock1  (driver?)
    Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)
    CPU: 0 PID: 1 Comm: swapper/0 Not tainted 4.4.203 #1
    Hardware name: ARM-Versatile Express
    [<80015d70>] (unwind_backtrace) from [<800127a8>] (show_stack+0x10/0x14)
    [<800127a8>] (show_stack) from [<80248be4>] (dump_stack+0x90/0xa4)
    [<80248be4>] (dump_stack) from [<800a834c>] (panic+0x9c/0x1f4)
    [<800a834c>] (panic) from [<806362c8>] (mount_block_root+0x1c8/0x268)
    [<806362c8>] (mount_block_root) from [<8063648c>] (mount_root+0x124/0x12c)
    [<8063648c>] (mount_root) from [<806365e4>] (prepare_namespace+0x150/0x198)
    [<806365e4>] (prepare_namespace) from [<80635ed0>] (kernel_init_freeable+0x250/0x260)
    [<80635ed0>] (kernel_init_freeable) from [<804afafc>] (kernel_init+0x8/0xe8)
    [<804afafc>] (kernel_init) from [<8000f2d0>] (ret_from_fork+0x14/0x24)
    ---[ end Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)

在这儿之所以报`Kernel panic`，是因为我们现在还没有安装根文件系统，无法启动init进程所致。那么接下来我们就来研究根文件系统。

<h1 id="7">7 制作根文件系统</h1>

本文要制作的根文件系统rootfs=busybox（包含基本的linux命令）+运行库+几个字符设备。

<h2 id="7.1">7.1 根文件系统存放位置</h2>

其实依赖于每个开发板支持的存储设备，可以放到Nor Flash上，也可以放到SD卡，甚至外部磁盘上。最关键的一点是你要清楚知道开发板有什么存储设备。

本文直接使用SD卡做为存储空间，文件格式为ext3格式

<h2 id="7.2">7.2 下载、编译和安装busybox</h2>

下载：

    wget https://busybox.net/downloads/busybox-1.31.1.tar.bz2

编译安装：

    make defconfig
    make CROSS_COMPILE=arm-linux-gnueabi-
    make install CROSS_COMPILE=arm-linux-gnueabi-

安装完成后，会在busybox目录下生成_install目录，该目录下的程序就是单板运行所需要的命令。

<h2 id="7.3">7.3 rootfs目录结构</h2>

1. 创建rootfs目录，根文件系统内的文件全部放到其中。

        mkdir -p rootfs/{dev,etc/init.d,lib}

2. 拷贝busybox到根目录下

        sudo cp busybox-1.20.2/_install/* -r rootfs/

3. 从工具链中拷贝运行库到lib目录下

        sudo cp -P /usr/arm-linux-gnueabi/lib/* rootfs/lib/

4. 创建4个tty端终设备

        sudo mknod rootfs/dev/tty1 c 4 1
        sudo mknod rootfs/dev/tty2 c 4 2
        sudo mknod rootfs/dev/tty3 c 4 3
        sudo mknod rootfs/dev/tty4 c 4 4

<h2 id="7.4">7.4 制作rootfs文件系统镜像</h2>

1. 生成32M大小的镜像

        dd if=/dev/zero of=a9rootfs.ext3 bs=1M count=32

2. 格式化成ext3文件系统

        mkfs.ext3 a9rootfs.ext3

3. 将文件拷贝到镜像中

        sudo mkdir tmpfs
        sudo mount -t ext3 a9rootfs.ext3 tmpfs/ -o loop
        sudo cp -r rootfs/*  tmpfs/
        sudo umount tmpfs

<h1 id="8">8 系统启动运行</h1>

现在是时候运行QEMU，模拟vexpress-A9开发板了，启动命令如下：

    qemu-system-arm -M vexpress-a9 -m 512M -kernel /home/qemu4/qemu/linux/linux-4.4.203/arch/arm/boot/zImage -dtb  /home/qemu4/qemu/linux/linux-4.4.203/arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic -append "root=/dev/mmcblk0  console=ttyAMA0" -sd a9rootfs.ext3

激动人心的时刻来了：

    aaci-pl041 10004000.aaci: ARM AC'97 Interface PL041 rev0 at 0x10004000, irq 33
    aaci-pl041 10004000.aaci: FIFO 512 entries
    oprofile: using arm/armv7-ca9
    NET: Registered protocol family 17
    9pnet: Installing 9P2000 support
    Registering SWP/SWPB emulation handler
    rtc-pl031 10017000.rtc: setting system clock to 2019-11-29 05:53:09 UTC (1575006789)
    ALSA device list:
      #0: ARM AC'97 Interface PL041 rev0 at 0x10004000, irq 33
    input: ImExPS/2 Generic Explorer Mouse as /devices/platform/smb/smb:motherboard/smb:motherboard:iofpga@7,00000000/10007000.kmi/serio1/input/input2
    EXT4-fs (mmcblk0): mounting ext3 file system using the ext4 subsystem
    EXT4-fs (mmcblk0): mounted filesystem with ordered data mode. Opts: (null)
    VFS: Mounted root (ext3 filesystem) readonly on device 179:0.
    Freeing unused kernel memory: 276K
    random: nonblocking pool is initialized
    can't run '/etc/init.d/rcS': No such file or directory

    Please press Enter to activate this console.
    / # ls
    bin         etc         linuxrc     sbin
    dev         lib         lost+found  usr
    / #

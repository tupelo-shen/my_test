<h1 id="0">0 目录</h1>

* [1 配置内核](#1)
* [2 构建系统](#2)
* [3 实例](#3)

---

<h1 id="1">1 配置内核</h1>

1. 配置内核。在编译内核之前，必须根据实际情况对内核的各个模块作出选择。

        #make config（基于文本的配置界面，不推荐使用）
        #make menuconfig（基于文本菜单的配置界面）
        #make xconfig（要求QT被安装）

2. 配置内核的工具是基于Kconfig的机制完成的，也就是在每一级的目录添加Kconfig文件，对源代码的选择作出描述或者配置。相当于，在每一级目录下添加了条件编译。

<h1 id="2">2 构建系统</h1>

基于上一小节的理解，为了将程序添加到Linux内核中，需要作下面工作：

1. 编写源代码，拷贝到Linux内核源代码的相应目录中。
2. 在该目录的Kconfig文件中增加关于新源代码对应项目的编译配置选项。
3. 在该目录的 Makefile 文件中增加对新源代码的编译条目。

关于构建系统的具体语法可以参考[Linux设备驱动开发3-Kconfig构建系统]()。

<h1 id="3">3 实例</h1>

我们还是以hello.c文件作为示例，看看详细的添加一个模块程序的步骤。

在drivers/目录下添加test目录，在test/下添加下面三个文件

1. hello.c

        #include <linux/init.h>
        #include <linux/module.h>

        MODULE_LICENSE("Dual BSD/GPL");
        static int hello_init(void)
        {
            printk(KERN_ALERT "Hello, world\n");
            return 0;
        }
        static void hello_exit(void)
        {
            printk(KERN_ALERT "Goodbye, cruel world\n");
        }
        module_init(hello_init);
        module_exit(hello_exit);

2. Makefile

        # drivers/test/Makefile
        #
        # Makefile for the TEST.
        #
        obj-$(CONFIG_TEST_HELLO)        += hello.o

3. Kconfig

        #
        # TEST driver configuration
        #

        menu "TEST Driver"
        comment "TEST Driver"

        config TEST
            bool "TEST support "
            default y

        config TEST_USER
            tristate "TEST user-space interface"
            depends on TEST

        config TEST_HELLO
            tristate "TEST hello world"
            depends on TEST

        endmenu

4. 修改drivers/目录下的Makefile

    在文件的最后，添加如下一行代码：

        obj-$(CONFIG_TEST)      += test/

5. 修改drivers/目录下的Kconfig

    在文件的最后，添加如下一行代码：

        source "drivers/test/Kconfig"

6. 然后执行,

        make CROSS_COMPILE=arm-linux-gnueabi- ARCH=arm menuconfig

    进入如下所示的界面中选择我们想要的模块。

    ![2-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/SongBaohua/images/2-1.PNG)

7. 保存配置结果，进行编译

        make CROSS_COMPILE=arm-linux-gnueabi- ARCH=arm

    编译结果：

        OBJCOPY arch/arm/boot/Image
        Kernel: arch/arm/boot/Image is ready
        Kernel: arch/arm/boot/Image is ready
        GZIP    arch/arm/boot/compressed/piggy.gzip
        AS      arch/arm/boot/compressed/piggy.gzip.o
        LD      arch/arm/boot/compressed/vmlinux
        OBJCOPY arch/arm/boot/zImage
        Kernel: arch/arm/boot/zImage is ready
        Building modules, stage 2.
        MODPOST 6 modules
        CC      drivers/test/hello.mod.o
        LD [M]  drivers/test/hello.ko

从上面的结果可以看出，hello.ko内核模块已经生成。将编译的内核和模块与rootfs根文件系统一起加载到开发板上运行，具体的步骤可以参考[Linux设备驱动开发0-环境搭建]()。

制作完成后，加载到开发板，运行。代码执行结果：

    / # insmod /lib/modules/4.4.203/kernel/drivers/test/hello.ko
    Hello, world
    / # rmmod hello.ko
    Goodbye, cruel world
    / #

# rcS文件

    PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/bin:

    runlevel=S
    prevlevel=N
    umask 022
    export PATH runlevel prevlevel

    # Trap CTRL-C &c only in this shell so we can interrupt subprocesses.
    trap ":" INT QUIT TSTP
    /bin/hostname qemu-a9-test

    /bin/mount -n -t proc none /proc
    /bin/mount -n -t sysfs none /sys
    # /bin/mount -n -t usbfs none /proc/bus/usb
    /bin/mount -t ramfs none /dev

    echo /sbin/mdev > /proc/sys/kernel/hotplug
    /sbin/mdev -s
    # /bin/hotplug

    # mounting file system specified in /etc/fstab
    mkdir -p /dev/pts
    mkdir -p /dev/shm
    /bin/mount -n -t devpts none /dev/pts -o mode=0622
    /bin/mount -n -t tmpfs tmpfs /dev/shm
    /bin/mount -n -t ramfs none /tmp
    /bin/mount -n -t ramfs none /var

    mkdir -p /var/empty
    mkdir -p /var/log
    mkdir -p /var/lock
    mkdir -p /var/run
    mkdir -p /var/tmp

    # /sbin/hwclock -s -f /dev/rtc
    syslogd

    # /etc/rc.d/init.d/netd start
    # echo " " > /dev/tty1
    # echo "Starting networking..." > /dev/tty1
    # mkdir /mnt/disk
    # mount -t yaffs2 /dev/mtdblock3 /mnt/disk
    # echo V > /dev/watchdog
    # /sbin/ifconfig lo 127.0.0.1
    # insmod /lib/modules/s3c2416_gpio.ko
    # /bin/sleep 2 dmesg -n 1
    # exec /usr/etc/rc.local


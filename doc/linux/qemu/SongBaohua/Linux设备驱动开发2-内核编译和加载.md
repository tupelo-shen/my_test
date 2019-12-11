<h1 id="0">0 目录</h1>

* [1 内核编译](#1)
    - [1.1 配置内核](#1.1)
    - [1.2 构建系统](#1.2)
* [2 主机编译和加载](#2)
* [3 目标板编译和加载](#3)

---

<h1 id="1">1 内核编译</h1>

<h2 id="1.1">1.1 配置内核</h2>

1. 配置内核。在编译内核之前，必须根据实际情况对内核的各个模块作出选择。

        #make config（基于文本的配置界面，不推荐使用）
        #make menuconfig（基于文本菜单的配置界面）
        #make xconfig（要求QT被安装）

2. 配置内核的工具是基于Kconfig的机制完成的，也就是在每一级的目录添加Kconfig文件，对源代码的选择作出描述或者配置。相当于，在每一级目录下添加了条件编译。

<h2 id="1.2">1.2 构建系统</h2>

基于上一小节的理解，为了将程序添加到Linux内核中，需要作下面工作：

1. 编写源代码，拷贝到Linux内核源代码的相应目录中。
2. 在该目录的Kconfig文件中增加关于新源代码对应项目的编译配置选项。
3. 在该目录的 Makefile 文件中增加对新源代码的编译条目。

关于构建系统的具体语法可以参考[Linux设备驱动开发3-Kconfig构建系统]()。

<h2 id="1.3">1.3 实例</h2>

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



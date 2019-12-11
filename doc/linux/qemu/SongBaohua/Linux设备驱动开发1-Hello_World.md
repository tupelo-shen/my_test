<h1 id="0">0 目录</h1>

* [1 Hello World](#1)
* [2 主机编译和加载](#2)
* [3 目标板编译和加载](#3)

---

<h1 id="1">1 Hello World</h1>

学习任何一门语言，几乎都是从Hello World开始的！真是无处不在啊！

代码内容如下：

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

这个模块定义了2个函数，一个是模块加载时调用的函数（*hello_init*）；还有一个是卸载模块时调用的函数(*hello_exit*)。*module_init* 和 *module_exit* 使用特定的内核宏表明这两个函数的职责。另外，*MODULE_LICENSE* 告知内核，该模块使用的许可证。

至于具体的每个概念，我们下一章再阐述。

*printk* 函数类似于C标准库中的 *printf* 函数。为什么模块能够调用内核的 *printk* 函数？那是因为模块被使用 *insmod* 命令加载进入内核后，模块就会被连接到内核，可以访问内核的公共符号（函数和变量）。*KERN_ALERT* 是消息的优先级。
你可以使用 **insmod** 和 **rmmod** 命令加载、卸载模块程序。

> 值得注意的是，只有root用户具有加载、卸载的权限。

<h1 id="2">2 主机编译和加载</h1>

可以在主机系统上编译运行，其Makefile内容如下：

    # 如果定义了KERNELRELEASE，从主机内核系统中调用相应的资源进行编译。
    ifneq ($(KERNELRELEASE),)
        obj-m := hello.o
    # 否则直接从命令行调用内核构建系统
    else
        KERNELDIR ?= /lib/modules/$(shell uname -r)/build   # 指定内核目录
        PWD := $(shell pwd)                                 # 获取当前路径
    default:
        $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
    endif

编译的结果如下：

    make -C /lib/modules/4.15.0-72-generic/build     M=/home/qemu4/qemu/exercise/drivers/hello                                   modules
    make[1]: Entering directory '/usr/src/linux-headers-4.15.0-72-generic'
      CC [M]  /home/qemu4/qemu/exercise/drivers/hello/hello.o
      Building modules, stage 2.
      MODPOST 1 modules
      CC      /home/qemu4/qemu/exercise/drivers/hello/hello.mod.o
      LD [M]  /home/qemu4/qemu/exercise/drivers/hello/hello.ko
    make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-72-generic'

在主机环境上运行，查看效果。

1. 加载模块

        sudo insmod hello.ko

2. 卸载模块

        sudo rmmod hello

如果您正在窗口系统下运行的终端仿真程序运行insmod和rmmod，则屏幕上不会显示任何内容。消息发送到其中一个系统日志文件，例如/var/log/kern.log（这是我的主机环境下的日志文件，实际文件的名称因Linux发行版而异）。其内容如下：

    Dec 10 11:27:25 qemu4-VirtualBox kernel: [ 1439.701029] Hello, world
    Dec 10 11:27:56 qemu4-VirtualBox kernel: [ 1470.614722] Goodbye, cruel world

<h1 id="3">3 目标板编译和加载</h1>

参考另一篇文章[Linux设备驱动开发2-内核编译和加载]()
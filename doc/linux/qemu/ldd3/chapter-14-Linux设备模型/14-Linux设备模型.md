* [14.1 Kobjects，Ksets和Subsystems](#14.1)
    - [14.1.1 Kobject基本知识](#14.1.1)
    - [14.1.2 Kobject Hierarchies, Ksets, and Subsystems](#14.1.2)
* [14.2 底层sysfs操作](#14.2)
    - [14.2.1 缺省属性](#14.2.1)
    - [14.2.2 非缺省属性](#14.2.2)
    - [14.2.3 二进制属性](#14.2.3)
    - [14.2.4 符号链接](#14.2.4)
* [14.3 热插拔事件产生](#14.3)
    - [14.3.1 热插拔操作](#14.3.1)
* [14.4 总线，设备和驱动](#14.4)
    - [14.4.1 总线](#14.4.1)
    - [14.4.2 设备](#14.4.2)
        + [14.4.2.1 设备注册](#14.4.2.1)
        + [14.4.2.2 设备属性](#14.4.2.2)
        + [14.4.2.3 设备结构嵌入](#14.4.2.3)
    - [14.4.3 设备驱动](#14.4.3)
        + [14.4.3.1 驱动结构嵌入](#14.4.3.1)
* [14.5 类](#14.5)
    - [14.5.1 class_simple接口](#14.5.1)
    - [14.5.2 完整类接口](#14.5.2)
        + [14.5.2.1 管理类](#14.5.2.1)
        + [14.5.2.2 类设备](#14.5.2.2)
        + [14.5.2.3 类接口](#14.5.2.3)
* [14.6 集成起来](#14.6)
    - [14.6.1 添加一个设备](#14.6.1)
    - [14.6.2 移除一个设备](#14.6.2)
    - [14.6.3 添加一个驱动](#14.6.3)
    - [14.6.4 移除一个驱动](#14.6.4)
* [14.7 热插拔](#14.7)
    - [14.7.1 动态设备](#14.7.1)
    - [14.7.2 /sbin/hotplug](#14.7.2)
        + [14.7.2.1 IEEE1394（火线）](#14.7.2.1)
        + [14.7.2.2 网络](#14.7.2.2)
        + [14.7.2.3 PCI总线](#14.7.2.3)
        + [14.7.2.4 输入](#14.7.2.4)
        + [14.7.2.5 USB总线](#14.7.2.5)
        + [14.7.2.6 SCSI总线](#14.7.2.6)
        + [14.7.2.7 笔记本坞站](#14.7.2.7)
        + [14.7.2.8 S/390和zSeries](#14.7.2.8)
    - [14.7.3 使用hotplug](#14.7.3)
        + [14.7.3.1 Linux热插拔脚本](#14.7.3.1)
        + [14.7.3.2 udev](#14.7.3.1)
* [14.8 处理固件](#14.8)
    - [14.8.1 内核固件接口](#14.8.1)
    - [14.8.2 如何工作？](#14.8.1)

***

2.5内核开发的既定目标之一就是为内核创建统一的设备模型。随着新系统的拓扑结构越来越复杂，以及对电源管理等功能的需求都表明，需要一个描述系统结构的通用抽象模型。

在2.6版本内核里设备模型提供了这种抽象概念。直到现在，还在内核中支撑着多种任务的实现，包括：

* 电源管理和系统关机

    这需要了解系统结构。例如，在处理完所有连接到USB适配器的设备之前，该适配器不能关闭。设备模型可以按照正确的顺序遍历系统的硬件。

* 与用户空间通信

    `sysfs`虚拟文件系统的实现与设备模型紧密相关，并把它表示的结构暴露给用户空间。 提供系统的信息给用户空间和修改操作参数， 越来越多地通过`sysfs`虚拟文件系统和设备模型来实现。

* 热插拔设备

    动态添加/移除外设，内核就需要处理设备的插拔，并和用户空间通信，这里需要的机制就是“热插拔机制”，也是通过设备模型管理的。

* 设备类

    系统的许多部分对设备的连接方式并不关心，它们最想知道哪类设备可用。设备模型也包括把设备分类，在更高功能层面描述这些设备，并允许用户空间发现它们。

* 对象生命周期

    上述功能（包括`hotplug`和`sysfs`）使内核中对象的创建和操作变得复杂。设备模型的实现就需要创建一组处理对象生命周期， 彼此之间的关系以及在用户空间的表示等内容的机制。


The Linux device model is a complex data structure. For example,consider Figure 14-1,which shows (in simplified form) a tiny piece of the device model structure associated with a USB mouse. Down the center of the diagram, we see the part of the core “devices” tree that shows how the mouse is connected to the system. The “bus” tree tracks what is connected to each bus, while the subtree under “classes” concerns itself with the functions provided by the devices, regardless of how they are connected. The device model tree on even a simple system contains hundreds of nodes like those shown in the diagram; it is a difficult data structure to visualize as a whole.

![Figure 14-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/14-1.PNG)

<h2 id="13.1">13.1 Kobjects，Ksets和Subsystems</h2>

<h2 id="13.2">13.2 底层sysfs操作</h2>

<h2 id="13.3">13.3 热插拔事件产生</h2>

<h2 id="13.4">13.4 总线，设备和驱动</h2>

<h2 id="13.5">13.5 类</h2>

<h2 id="13.6">13.6 集成起来</h2>

<h2 id="13.7">13.7 热插拔</h2>



<h4 id="13.7.2.7">13.7.2.7 笔记本坞站</h4>

如果一个“即插即用”的笔记本坞站从运行中的Linux系统上被添加或移除，就会产生“热插拔”时间。`/sbin/hotplug`将参数名称和`SUBSYSTEM`环境变量设置为`dock`。其它环境变量不用设置。


<h2 id="13.8">13.8 处理固件</h2>
* [14.1 kobject，kset和子系统](#14.1)
    - [14.1.1 Kobject基本知识](#14.1.1)
        + [14.1.1.1 嵌入的内核对象](#14.1.1.1)
        + [14.1.1.2 内核对象的初始化](#14.1.1.2)
        + [14.1.1.3 引用计数操作](#14.1.1.3)
        + [14.1.1.4 释放函数和kobject类型](#14.1.1.4)
    - [14.1.2 kobject层次结构，kset和子系统](#14.1.2)
        + [14.1.2.1 kset](#14.1.2.1)
        + [14.1.2.2 操作kset](#14.1.2.2)
        + [14.1.2.3 子系统](#14.1.2.3)
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

Linux设备模型是一个复杂的数据结构。例如，考虑下面这个USB鼠标的设备模型简易图（14-1）。图的中间，可以看到其设备树，从中可以看出鼠标是如何连接到系统的。`bus`总线树追踪连接到每个总线的是什么，而`classes`关心设备提供的功能，不关心是怎么连接的。即使一个简单的系统的设备树模型也有如图所示的成百上千的节点，所以想把设备模型完全作为一个整体可视化是很困难的。

![Figure 14-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/14-1.PNG)

大多数情况下，Linux设备模型会考虑这些事情，而不会留给驱动开发者。与设备模型的直接交互由总线级逻辑和各种其它内核子系统。结果就是，驱动开发者能够忽略设备模型，相信它能处理好。

但是，有时候了解设备模型是件好事。设备模型也有可能被其它层使用；例如，通用DMA代码（将在第15章遇到）就使用`struct device`工作。你也许想要使用设备模型提供的功能，诸如由`kobject`提供的引用计数和其相关功能、通过`sysfs`与用户空间通信等。 本章我们就会介绍通信的方式。

我们对设备模型将采用“自下而上”的表述方式。设备模型的复杂性使得很难从更高的视角理解它。我们的希望是，通过展示底层的设备组件工作方式，继而让你掌握由这些组件构成更大的结构时的工作方式。

对于很多读者，可以把本章作为高级内容，无需在第一次的时候阅读。鼓励那些对Linux设备模型工作方式感兴趣的人随我们一起探讨底层的细节。

<h2 id="14.1">14.1 kobject，kset和子系统</h2>

结构体`kobject`是设备模型保持在一起的基本结构。最初构想只是作为一个引用计数器，但是，随着时间的推移， 它的职责和字段都在不断增加。它能处理的任务主要是：

* 对象的引用计数

    通常，内核对象被创建后，没有办法知晓它能存在多长时间。追踪对象生命周期的方法之一就是引用计数器。如果内核代码中没有对给定对象的引用了，那么对象的生命周期就结束了，可以被删除。

* Sysfs 表示

    出现在`sysfs`中的每一个对象，都有一个与内核交互的`kobject`用以创建对象的可视化表示。

* 数据结构的粘合剂

    设备模型是一个非常复杂的数据结构，由多个层次结构组成，它们之间有许多连接。`kobject`就是实现这种结构并把它联系在一起的纽带。

* 热插拔事件处理

    `kobject`子系统处理热插拔事件的产生，这些事件通知用户空间设备的添加、移除。


从上面可以看出，`kobject`是一个复杂的结构。但是，我们将其分割成块，一次一块，慢慢地就会理解这种结构和其工作原理了。

<h3 id="14.1.1">14.1.1 Kobject的基本知识</h3>

内核对象用结构体`kobject`来表示，定义在`<linux/kobject.h>`文件中。这个文件里还包含一些其它关于内核对象的结构体， 以及操作这些对象结构体的函数。

下面是Linux 3.3.7内核中`kobject`结构体的定义：

    struct kobject {
        const char          *name;      /* 该对象在sysfs中的名称指针 */
        struct list_head    entry;      /* kobject 之间的双向链表，与所属的kset形成环形链表*/
        struct kobject      *parent;    /* 在sysfs分层结构中定位对象，指向上一级kset中的struct kobject kobj */
        struct kset         *kset;      /* 指向所属的kset */
        struct kobj_type    *ktype;     /* 负责对该kobject类型进行跟踪的struct kobj_type的指针 */
        struct sysfs_dirent *sd;        /* sysfs文件系统中与该对象对应的文件节点路径指针*/
        struct kref         kref;       /* kobject 的引用计数*/
        unsigned int state_initialized:1;
        unsigned int state_in_sysfs:1;
        unsigned int state_add_uevent_sent:1;
        unsigned int state_remove_uevent_sent:1;
        unsigned int uevent_suppress:1;
    };

<h4 id="14.1.1.1">14.1.1.1 嵌入的kobject</h4>

在我们深入了解`kobject`细节之前，有必要花费一点时间理解内核对象是如何工作的。单独一个内核对象`kobject`，本身没有太多意义； 它的存在仅仅是为了把更高级的对象绑定到设备模型中。

因此，内核代码很少创建独立的`kobject`对象；取而代之的是，把`kobject`嵌入到其它结构中，用以访问更大，特定作用域的对象。 如果习惯于用面向对象的编程思维考虑事情，那么`kobject`对象就可以看作是其它类的派生对象，一个高级的抽象类。`kobject`对象实现的功能对自己没什么特殊的作用，但是对其它对象却很用。C语言不允许继承，所以需要在另一个结构里嵌入`kobject`结构实现。

让我们在回顾一下第3章中的结构体 `struct cdev`：

    struct cdev {
        struct kobject kobj;
        struct module *owner;
        struct file_operations *ops;
        struct list_head list;
        dev_t dev;
        unsigned int count;
    };

可以看到，`cdev`结构中嵌入了一个`kobject`。但是，使用`kobject`对象经常遇到一个反推的问题： 给定一个`struct kobject`指针， 指向包含它的结构的指针是什么？ 你应该避免假设（例如，假设`kobject`位于结构的开头）， 而是使用`container_of`宏（在第3章的“open”方法一节中介绍过）。 因此根据类型为`struct kobject`的指针`kp`获取包含它的结构体`cdev`的指针的方法如下：

    struct cdev *device = container_of(kp, struct cdev, kobj);

通常都会定义一个简单的宏实现这个操作。

<h4 id="14.1.1.2">14.1.1.2 内核对象的初始化</h4>

本书介绍了在编译或运行时对一些类型进行初始化的简单机理。而`kobject`的初始化更复杂，尤其是当所有功能都被使用时。但是，无论一个`kobject`对象怎么被使用，有一些步骤是必须执行的。

首先，就是将整个`kobject`对象初始化为0，这通常作为包含该对象的结构体归零的一部分。如果`kobject`对象归零化失败， 则会导致奇怪的崩溃；所以， 这一步不能跳过。

下面一步是调用 `kobject_init()`函数，设置其内部的成员：

    void kobject_init(struct kobject *kobj);

`kobject_init()`将`kobject`的引用计数器设置为 `1`。但是，光调用`kobject_init()`还不够。必须最小限度的设置`kobject`对象的名称；这个名称就会在`sysfs`项中使用。如果翻遍内核源代码，会发现直接将一个字符串拷贝到`kobject`对象的`name`字段的代码。但是这种方法应该使用下面的代替：

    int kobject_set_name(struct kobject *kobj, const char *format, ...);

该函数具有和printk风格类似的可变参数列表。无论相信与否，这个函数实际上可能会失败（它可能会尝试分配内存）；所以，应该检查其返回值，并作出相应的处理。

其它的对象成员初始化在后面介绍。

<h4 id="14.1.1.3">14.1.1.3 引用计数操作</h4>

一个`kobject`对象主要的功能之一就是作为一个引用计数器存在。只要该对象还被引用，那么该对象就应该继续存在。 操作引用计数器的底层函数如下所示：

    struct kobject *kobject_get(struct kobject *kobj);
    void kobject_put(struct kobject *kobj);

成功调用`kobject_get`会增加引用计数器，返回一个指向`kobject`对象的指针。但是，如果`kobject`对象已经被进程销毁， 操作会失败，且`kobject_get`返回`NULL`。这个返回值必须被检查，以防止这种竞争条件的发生。

当引用被释放，调用`kobject_put`会使引用计数器减`1`，如果到`0`，就会释放掉这个对象。请记住，调用`kobject_init()`将引用计数器初始化为`1`，所以这个计数器不使用的时候，应该调用`kobject_put`，把计数器初始化为`0`。

请注意，在许多情况下，`kobject`本身的引用计数可能不足以防止竞争条件。 例如，`kobject`（及其包含它的结构）的存在， 就要求创建该`kobject`的驱动程序持续存在。 但是，当`kobject`还在被传递时，就不应该卸载该驱动模块。 这就是为什么我们会在上面的`cdev`结构中看到包含一个`struct module`类型指针的原因。 `struct cdev`的引用计数实现如下：

    struct kobject *cdev_get(struct cdev *p)
    {
        struct module *owner = p->owner;
        struct kobject *kobj;

        if (owner && !try_module_get(owner))
            return NULL;
        kobj = kobject_get(&p->kobj);
        if (!kobj)
            module_put(owner);
        return kobj;
    }

创建一个对`cdev`的引用时，还需要创建一个对包含它的驱动模块的引用。所以，`cdev_get`使用`try_module_get`尝试让驱动模块使用计数加`1`。如果操作成功，才会调用`kobject_get`，使`kobject`引用计数加`1`。当然了，`kobject_get` 可能失败, 因此, 检查其返回值，如果调用失败，则释放它的对驱动模块的引用计数。

<h4 id="14.1.1.4">14.1.1.4 释放函数和kobject类型</h4>

还有一个重要的事情就是，引用计数减少到`0`时，会发生什么？通常，创建`kobject`对象的代码不知道这件事什么时候发生； 因为其无法得知对象的生命周期，`sysfs`的引入使得对象的生命周期变得极其复杂， 而且，用户空间的程序可以以任意时间引用这个`kobject`对象（比如，打开与其相关的`sysfs`文件）。所以，当引用计数到`0`时，必须异步通知。通过`kobject`对象的`release`函数实现。通常示例如下：

    void my_object_release(struct kobject *kobj)
    {
        struct my_object *mine = container_of(kobj, struct my_object, kobj);
        /* 对这个对象进行额外的清理工作，然后再释放掉... */
        kfree(mine);
    }

一个重要的观点不容小觑：每个`kobject`对象必须有一个`release`方法，在该方法调用之前，该对象必须保持不变（稳定状态）。 如果不能满足这个条件，代码就会存在风险。比如，该对象还在被引用就有可能被释放，或者，当引用计数为`0`时，没有释放掉该对象。

有趣的是，`release`方法并不存在于`kobject`结构本身内； 而是与包含`kobject`的结构类型相关，这种类型结构就是`kobj_type`。

    struct kobj_type {
        void (*release)(struct kobject *);
        struct sysfs_ops *sysfs_ops;
        struct attribute **default_attrs;
    };

`struct kobj_type`的`release`函数指针成员指向`kobject`类型的`release`方法。后面介绍其它2个成员。

每个`kobject`都需要有一个与之相关联的`kobj_type`结构。令人困惑的是，可以在两个不同的地方找到该结构指针。`kobject`本身包含一个指针成员，称为`ktype`。但是，如果该`kobject`是`kset`的成员，则由`kset`提供的指针起作用。同时， 使用下面的方法可以得到一个给定`kobject`对象的`kobj_type`结构指针。

    struct kobj_type *get_ktype(struct kobject *kobj);

<h3 id="14.1.2">14.1.2 kobject层次结构，kset和子系统</h3>

内核通常使用`kobject`结构将各个对象连接起来组成一个分层的结构体系，与模型化的子系统相匹配。这种连接有2个独立的机制：`parent`指针和`kset`。

`parent`是指向另一个`kobject`结构（分层结构中上一层的节点）的指针。例如，一个表示USB设备的`kobject`，它的`parent`指针指向USB插入的那个`hub`集线器的`kobject`对象。

`parent`的主要作用就是在`sysfs`结构体系中定位对象。

<h4 id="14.1.2.1">14.1.2.1 kset</h4>

在许多方面，`kset`都像是`kobj_type`结构的扩展；`kset`是内嵌有相同的类型结构体成员的对象的集合。但是，`struct kobj_type`关注的是对象的类型，而`struct kset`关心的是对象的集合和聚合。把这两个概念分开，就是为了相同类型的对象能够出现在不同的集合中。

`kset`数据结构为（Linux 3.3.7内核）：

    struct kset {
        struct list_head        list;           /*用于连接该kset中所有kobject以形成环形链表的链表头*/
        spinlock_t              list_lock;      /*用于避免竞态的自旋锁*/
        struct kobject          kobj;           /*嵌入的kobject*/
        struct kset_uevent_ops  *uevent_ops;
    };

因此，`kset`的主要功能就是`包含`；可以被认为是顶层容器类。事实上，每一个`kset`内部都包含自己的`kobject`，在许多情况下， 都可以像处理`kobject`那样处理它。值得注意的是，`kset`一旦被建立，就一定会出现在`sysfs`中，目录的形式表示。而`kobject`不是必须出现在`sysfs`中，但是`kset`中的每一个`kobject`成员都会在`sysfs`中表示。

将`kobject`对象添加到`kset`中，通常在对象被建立时完成；这分为2步。首先，`kobject`对象的`kset`指针指向与其有关的`kset`。然后，将`kobject`对象传递给下面的函数即可：

    int kobject_add(struct kobject *kobj);

注意，函数可能会失败-返回负值，必须检查并作相关处理。内核提供了一个方便的组合函数，是`kobject_init`和`kobject_add`的组合。

    extern int kobject_register(struct kobject *kobj);

`kobject`对象传递给`kobject_add`时，其引用计数器会增加。毕竟，`kset`中的包含就是一种对对象的引用。 当把一个`kobject`从`kset`中删除以清除引用时使用：

    void kobject_del(struct kobject *kobj);

也有一个`kobject_del`和`kobject_put`的组合函数`kobject_unregister`。

`kset`使用标准内核链表结构管理它包含的子对象。几乎在所有的情况下，被包含的`kobject`对象使用它们的`parent`指针指向`kset`（严格意义上说， 是'kset'内嵌的`kobject'）。如图14-2所示。请记住：

* 下面框图中包含的所有`kobject`，在实际情况中，都会被嵌入到其它类型，甚至是其它`kset`中。
* 一个`kobject`的父节点不一定是包含它的`kset`(尽管这很少见)。

![Figure 14-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/14-2.PNG)

<h4 id="14.1.2.2">14.1.2.2 操作kset</h4>

`kset`相关的操作函数如下，与操作`kobject`的函数类似：

    void kset_init(struct kset *kset);
    int kset_add(struct kset *kset);
    int kset_register(struct kset *kset);
    void kset_unregister(struct kset *kset);

管理`kset`的引用计数器的方法也相似：

    struct kset *kset_get(struct kset *kset);
    void kset_put(struct kset *kset);

一个`kset`也有名称，存储在其内嵌的`kobject`里。所以，如果有一个叫`my_set`的`kset`，应该通过下面的函数设置其名称：

    kobject_set_name(&my_set->kobj, "The name");

ksets 还有一个指针指向 kobj_type 结构来描述它包含的 kobject，这个类型优先于 kobject 自身中的 ktype 。因此在典型的应用中, 在 struct kobject 中的 ktype 成员被设为 NULL, 而 kset 中的ktype是实际被使用的。
在新的内核里， kset 不再包含一个子系统指针struct subsystem * subsys， 而且subsystem已经被kset取代。

`kset`还有一个指向`kobj_type`结构的指针成员，用来描述它本身内嵌的`kobject`结构。结果就是，在通常的使用中，`struct kobject`中的`ktype`成员被设置为`NULL`，因为在`kset`中，已经有一个相同的表示了。（**注意，在较新的内核中，`kset`中的`kobj_type`结构的指针成员已经被取消，因为没有什么实际意义**）

最后，`kset`还包含一个子系统指针（`subsys`）。所以下面讨论一下子系统。（**注意，在较新的内核版本里，这个成员也已经被取消`kset`取代**)

<h4 id="14.1.2.3">14.1.2.3 子系统</h4>

A subsystem is a representation for a high-level portion of the kernel as a whole. Subsystems usually (but not always) show up at the top of the sysfs hierarchy. Some example subsystems in the kernel include block_subsys (/sys/block,for block devices), devices_subsys (/sys/devices,the core device hierarchy),and a specific subsystem for every bus type known to the kernel. A driver author almost never needs to create a new subsystem; if you feel tempted to do so,think again. What you probably want, in the end, is to add a new class, as discussed in the section “Classes.”

子系统是整个内核的高级部分的表示。 子系统通常（但不总是）显示在`sysfs`层次结构的顶部。内核中的一些示例子系统包括`block_subsys`（`/sys/block`，用于块设备），`devices_subsys`（`/sys/devices`，核心设备层次结构）， 以及针对内核已知的每种总线类型的特定子系统。 驱动程序作者几乎从不需要创建新的子系统。 最后， 您可能想要添加一个新类， 如“类”一节中所述。

子系统的结构定义如下：

    struct subsystem {
        struct kset kset;
        struct rw_semaphore rwsem;
    };

一个子系统就是`kset`和一个互斥量的封装。

Every kset must belong to a subsystem. The subsystem membership helps establish the kset’s position in the hierarchy,but,more importantly,the subsystem’s rwsem semaphore is used to serialize access to a kset’s internal-linked list. This membership is represented by the subsys pointer in struct kset. Thus,one can find each kset’s containing subsystem from the kset’s structure,but one cannot find the multiple ksets contained in a subsystem directly from the subsystem structure.

Subsystems are often declared with a special macro:

    decl_subsys(name, struct kobj_type *type, struct kset_hotplug_ops *hotplug_ops);

This macro creates a struct subsystem with a name formed by taking the name given to the macro and appending _subsys to it. The macro also initializes the internal kset with the given type and hotplug_ops. (We discuss hotplug operations later in this chapter.)

Subsystems have the usual list of setup and teardown functions:

    void subsystem_init(struct subsystem *subsys);
    int subsystem_register(struct subsystem *subsys);
    void subsystem_unregister(struct subsystem *subsys);
    struct subsystem *subsys_get(struct subsystem *subsys)
    void subsys_put(struct subsystem *subsys);

Most of these operations just act upon the subsystem’s kset.


<h2 id="14.2">14.2 底层sysfs操作</h2>

<h2 id="14.3">14.3 热插拔事件产生</h2>

<h2 id="14.4">14.4 总线，设备和驱动</h2>

<h2 id="14.5">14.5 类</h2>

<h2 id="14.6">14.6 集成起来</h2>

<h2 id="14.7">14.7 热插拔</h2>



<h4 id="14.7.2.7">14.7.2.7 笔记本坞站</h4>

如果一个“即插即用”的笔记本坞站从运行中的Linux系统上被添加或移除，就会产生“热插拔”时间。`/sbin/hotplug`将参数名称和`SUBSYSTEM`环境变量设置为`dock`。其它环境变量不用设置。


<h2 id="14.8">14.8 处理固件</h2>
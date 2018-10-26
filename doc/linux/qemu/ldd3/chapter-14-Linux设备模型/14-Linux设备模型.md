<h2 id="0">0 目录</h2>

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
        + [14.4.1.1 总线注册](#14.4.1.1)
        + [14.4.1.2 总线方法](#14.4.1.2)
        + [14.4.1.3 设备和驱动的迭代](#14.4.1.3)
        + [14.4.1.4 总线属性](#14.4.1.4)
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
* [14.6 各个环节整合起来](#14.6)
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

子系统是整个内核的高级部分的表示。 子系统通常（但不总是）显示在`sysfs`层次结构的顶部。内核中的一些示例子系统包括`block_subsys`（`/sys/block`，用于块设备），`devices_subsys`（`/sys/devices`，核心设备层次结构）， 以及针对内核已知的每种总线类型的特定子系统。 驱动开发者几乎从不需要创建新的子系统，如果你尝试这样做，请三思。 可能到最后， 只想要添加一个新class就行， 如“class”一节中所述。

子系统的结构定义如下：

    struct subsystem {
        struct kset kset;
        struct rw_semaphore rwsem;
    };

一个子系统就是`kset`和一个互斥量的封装。

每一个`kset`必须属于一个子系统。`subsystem`的`kset`成员帮助在体系结构中建立`kset`的位置，更重要的是， `subsystem` 的 `rwsem` 互斥量被用来序列化访问 `kset`的内部链表。This membership is represented by the subsys pointer in struct kset. Thus,one can find each kset’s containing subsystem from the kset’s structure,but one cannot find the multiple ksets contained in a subsystem directly from the subsystem structure.

Subsystems are often declared with a special macro:

    decl_subsys(name, struct kobj_type *type, struct kset_hotplug_ops *hotplug_ops);

This macro creates a struct subsystem with a name formed by taking the name given to the macro and appending _subsys to it. The macro also initializes the internal kset with the given type and hotplug_ops. (We discuss hotplug operations later in this chapter.)

子系统常用的建立和销毁的函数：

    void subsystem_init(struct subsystem *subsys);
    int subsystem_register(struct subsystem *subsys);
    void subsystem_unregister(struct subsystem *subsys);
    struct subsystem *subsys_get(struct subsystem *subsys)
    void subsys_put(struct subsystem *subsys);

所有这些操作仅仅对子系统的`kset`起作用。


<h2 id="14.2">14.2 底层sysfs操作</h2>

`kobject`是`sysfs`虚拟文件系统背后的机制。 对于在`sysfs`中找到的每个目录， 内核中都有一个`kobject`与之对应。 每个感兴趣的`kobject`还会导出一个或多个属性， 这些属性以文件的形式出现在`kobject`的`sysfs`目录中， 包含着内核生成的信息。 本节将探讨`kobject`和`sysfs`在底层是如何交互的。

与`sysfs`部分相关的代码，位于文件`<linux/sysfs.h>`中。

让一个`kobject`对象出现在`sysfs`中，只需简单地调用`kobject_add`这个函数即可。在介绍如何添加`kobject`对象到`kset`中时， 已经看见过；在`sysfs`创建条目也是其工作的一部分。关于如何创建`sysfs`条目，需要了解下面的事情：

* 为`kobject`对象所创建的`sysfs`条目总是目录，所以调用`kobject_add`在会`sysfs`中创建一个目录。通常， 这个目录包含一个或多个属性，稍后，我们会介绍属性是如何指定的。

* `kobject`设置的名称就是`sysfs`目录中的名称。因而，出现在`sysfs`体系结构中的相同部分的`kobject`应该具有唯一的名称。 分配给`kobject`的名称也必须遵循文件命名方式：不能包含"/"，也不推荐使用"空格"。

* `sysfs`条目位于该`kobject`对象的`parent`指针指向的父目录中。如果`parent`为`NULL`， 调用`kobject_add`时，被设置为新的对象集`kset`中内嵌的`kobject`; 因而，`sysfs`体系结构通常还是匹配使用`kset`创建的其内部体系。如果`parent`和`kset`都为`NULL`，则在顶层目录创建一个`sysfs`目录， 这往往不是你想要的结果。

到目前为止，使用我们介绍的机制，已经可以在`sysfs`创建空的目录了。但是，我们感兴趣的不会就这些，所以，是时候介绍属性了。

<h3 id="14.2.1">14.2.1 缺省属性</h3>

当创建时，每一个`kobject`都会被设定一组默认属性。这些属性使用结构体`kobj_type`保存。这个结构体看起来如下所示：

    struct kobj_type {
        void (*release)(struct kobject *);
        struct sysfs_ops *sysfs_ops;
        struct attribute **default_attrs;
        /* 省略 */
    };

`default_attrs`成员列出了为该类型的每一个`kobject`对象所创建的属性，`sysfs_ops`提供实施这些属性的方法。`default_attrs`指向一个指针数组，该数组成员是指向`attribute`结构的指针。

    struct attribute {
        char *name;
        struct module *owner;
        mode_t mode;
    };

> <font color=red>在内核3.3.7中，已经变更为</font>
>
>       struct attribute {
>           const char *name;
>           umode_t mode;
>       #ifdef CONFIG_DEBUG_LOCK_ALLOC
>           struct lock_class_key *key;
>           struct lock_class_key skey;
>       #endif
>       };

`name`是属性的名称（因为它会出现在该对象的`sysfs`目录下）；`owner`指向实现该属性的模块（如果有的话）的指针；（<font color=red>根据上面的代码，较新的内核代码已经没有`owner`这个成员了</font>）；`mode`是保护位。通常，只读属性是`S_IRUGO`;只写属性是`S_IWUSR`；具体的宏定义位于文件`<linux/stat.h>`中。上面的`default_attrs`列表的最后一项必须用0填充，表示数组结束。

`default_attrs`数组告诉了`sysfs`属性是什么，但是没有告知怎样实现这些属性。这些任务就落到了`kobj_type->sysfs_ops`成员身上，它指向下面的结构体：

    struct sysfs_ops {
        ssize_t (*show)(struct kobject *kobj, struct attribute *attr, char *buffer);
        ssize_t (*store)(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size);
    };

> <font color=red>在内核3.3.7中，又添加了下面这个成员</font>
>
>       const void *(*namespace)(struct kobject *, const struct attribute *);
>

从用户空间读取属性时，`show`方法就会被调用，参数是指向`kobject`类型的对象和正确的`attribute`结构。该方法把给定属性的值存到`buffer`中，保证不要越界（最大值`PAGE_SIZE`字节）， 返回值是返回数据的真实长度。`sysfs`规定，每个属性都应该是一个简单， 符合人们读写习惯的值； 如果要返回很多信息，你可能会考虑把它拆分成多个属性。

`attr`指针决定请求哪个属性。有一些`show`方法，会对属性的名称作测试。 还有一些实现， 把包含其它信息的结构体内嵌到`attribute`结构体中， 而这些信息还需要返回给属性值；在这种情况下，可以在`show`方法中使用`container_of`，获取那个内嵌结构体的指针。

`store`与其类似；它负责解码存储在`buffer`中的数据（`size`是所包含数据的长度，不要超过`PAGE_SIZE`）。`store`方法只有在属性拥有写权限的时候才会被调用。当调用`write`方法的时候，要注意，接收到的数据可以是任意数据，所以要验证， 然后做出正确的响应。如果接受的数据，不能与预期匹配，则返回负值，不要随便处理。如果你的设备导出一个`self_destruct`属性，应该要求写一个特定的字符串来调用该功能；随机的写入将会产生错误。

<h3 id="14.2.2">14.2.2 非缺省属性</h3>

在许多情况下，`kobject`对象类型的`default_attrs`成员描述了该对象将永远拥有的属性。但是，在设计上，这不是一个严格限制； 我们可以根据医院，随意对`kobject`的属性进行添加和删除。如果你想添加一个新的`attribute`到`kobject`的`sysfs`目录上，只需简单的填充一个`attribute`，把它传递给下面的函数即可：

    int sysfs_create_file(struct kobject *kobj, struct attribute *attr);

如果调用成功，使用`attribute`结构中给定的名称创建一个文件，函数返回0；否则，返回负值。

对新属性的操作还是调用`show()`和`store()`函数。在向`kobject`添加新属性之前， 应该采取必要的步骤， 确保这些函数知道如何实现该属性。

移除属性，调用：

    int sysfs_remove_file(struct kobject *kobj, struct attribute *attr);

移除之后，`attribute`不再出现在`kobject`的`sysfs`目录里。但是，必须意识到， 属性移除之后， 用户空间的进程可能还使用着该属性的文件描述符， `show`和`store`方法仍然被调用着。


<h3 id="14.2.3">14.2.3 二进制属性</h3>

`sysfs`规范提倡所有的属性包含一个可读的文本格式存储的值。那也就是说，还是有极少的需求，创建能够处理大数据量的二进制数据的属性。
这种属性只有在用户空间和设备之间传递数据时才会发生。例如，将固件下载到设备上就需要这种功能。当在系统中遇到这种设备时，一个用户空间的程序就会被启动（通过`hotplug`机制；该程序通过`sysfs`的二进制属性传递固件代码到内核，如[内核固件接口](#14.8.1)所展示的。 二进制属性使用`bin_attribute`结构描述：

    struct bin_attribute {
        struct attribute attr;
        size_t size;
        ssize_t (*read)(struct kobject *kobj, char *buffer, loff_t pos, size_t size);
        ssize_t (*write)(struct kobject *kobj, char *buffer, loff_t pos, size_t size);
    };

在这儿，`attr`是一个`attribute`结构对象成员，提供属性的名称，所有者（<font color=red>新内核中`owner`已被删除</font>）和二进制属性的权限等；`size`是二进制属性的最大值（如果等于0，没有最大值）。`read`和`write`方法与常规的字符驱动相似； 对于单次下载，最大可以传递一页的数据，多次调用即可，因此实现二进制属性的代码必须能够以其它方式确定数据的结尾。

二进制属性必须显式创建，不能设为默认属性。为了创建二进制属性，调用：

    int sysfs_create_bin_file(struct kobject *kobj, struct bin_attribute *attr);

移除属性：

    int sysfs_remove_bin_file(struct kobject *kobj, struct bin_attribute *attr);

<h3 id="14.2.4">14.2.4 符号链接</h3>

`sysfs`文件系统具有普通的树结构， 反映了它所表示的`kobjects`的层次结构。 然而， 内核中对象之间的关系通常比这更复杂。 例如， 一个`sysfs`子树（`/sys/devices`）表示系统已知的所有设备， 而其它子树（`/sys/bus`）表示设备驱动程序。 但是， 这些树没有表达出驱动程序与它们管理的设备之间的关系。 显示这些附加关系需要额外的指针，这些指针在`sysfs`中通过符号链接实现。

在`sysfs`中创建一个符号链接非常简单：

    int sysfs_create_link(struct kobject *kobj, struct kobject *target, char *name);

这个函数闯将一个指向目标`sysfs`项的链接， 作为`kobj`的属性， `name`是其名称。它是一个相对链接，因此， 无论`sysfs`装在什么系统上， 它都可以工作。

即使目标被移除，链接也会继续存在。如果你将要为其它对象创建符号链接，你应该很有可能有知道那些对象的变化、或保证目标对象不会消失的方法。结果（`sysfs`中有僵死的符号链接）并不是特别严重，但是，不是最佳编程风格的表达且容易导致用户空间混乱。

移除符号链接：

    void sysfs_remove_link(struct kobject *kobj, char *name);

<h2 id="14.3">14.3 热插拔事件产生</h2>

一个`hotplug`事件，是内核用来通知用户空间系统配置发生了变化的方式。无论什么时候， `kobject`被创建或销毁， 都会生成事件。 比如， USB接口的数字摄像机插入系统、或者用户切换控制台模式，再或者磁盘被重新分区等， 都会产生事件。“热插拔事件”会调用`/sbin/hotplug`， 能通过载入驱动，创建设备节点，安装分区或其它合适的操作来响应每个事件。

我们看到的关于`kobject`的最后的一个主要函数负责这些事件的产生。 当调用`kobject_add`或`kobject_del`，并传递一个`kobject`作为参数， 一个真实的事件就会发生。在事件传递给用户空间之前， 和`kobject`（或者，更具体地， 它所属的`kset`） 有关的代码有机会为用户空间添加信息或完全禁用事件的生成。

<h3 id="14.3.1">14.3.1 热插拔操作</h3>

使用下面的结构体里的方法处理“热插拔事件”：

    struct kset_hotplug_ops {
        int (*filter)(struct kset *kset, struct kobject *kobj);
        char *(*name)(struct kset *kset, struct kobject *kobj);
        int (*hotplug)(struct kset *kset, struct kobject *kobj,
                        char **envp, int num_envp, char *buffer,
                        int buffer_size);
    };

可以在`kset`结构体的`hotplug_ops`成员用来指向这个结构体。 如果给定的`kobject`没有包含在一个`kset`里， 内核会顺着体系结构向上寻找， 直到发现一个拥有`kset`的`kobject`;然后使用那个`kset`的`hotplug`操作。

只要内核考虑为给定的`kobject`生成事件，就会调用`filter`方法。 如果返回 0， 事件不会被创建。 因此，这个方法给与了`kset`代码一个机会，能够决定哪个事件可以被传递给用户空间，哪个不能。

考虑`block`子系统，作为示例， 展示这个方法的使用方法。这里，至少有3种类型的`kobject`被使用，表示磁盘、分区、请求队列。 用户空间想要对磁盘或分区的添加做出反应，但是通常不会关心请求队列。 所以，`filter`方法允许只为表示磁盘和分区的`kobject`产生事件。 如下所示：

    static int block_hotplug_filter(struct kset *kset, struct kobject *kobj)
    {
        struct kobj_type *ktype = get_ktype(kobj);
        return ((ktype == &ktype_block) || (ktype == &ktype_part));
    }

在这里，快速地检查了`kobject`的类型，决定哪个事件应该产生，哪个不能。

当用户空间`hotplug`事件被调用时，传递相关子系统的名称作为它的唯一参数。`name`方法提供该名称。 应该返回一个适合传递给用户空间的简单字符串。

`hotplug`脚本可能想要知道的所有其它内容都在环境中传递。 最终的热插拔方法（hotplug）提供了在调用该脚本之前添加有用的环境变量的机会。 这个方法的原型是：

    int (*hotplug)(struct kset *kset, struct kobject *kobj,
                    char **envp, int num_envp, char *buffer,
                    int buffer_size);

与往常一样，`kset`和`kobject`描述了将要产生事件的对象。`envp`是存储其它环境变量定义的地方（通常使用`NAME=value`的格式）； 它具有`num_envp`项可用。变量本身应该被放入到`buffer`中，大小是`buffer_size`字节。如果添加了任何变量到`envp`中，最后一项应该被设置为`NULL`，让内核知道`envp`的结束位置。正常情况下， 返回值是0； 任何非零值都会放弃这次热插拔事件的产生。

通设备模型中的许多工作一样， “热插拔事件”的产生都是在总线驱动层面的逻辑处理。

<h2 id="14.4">14.4 总线，设备和驱动</h2>

到目前为止，我们已经看了许多底层的结构实现和相对简短的示例。接下来，我们把它们组合起来，讨论更为高深的Linux设备模型。为此，我们引入一个新的虚拟总线，称之为`lddbus`，修改`scullp`驱动，让其挂载到该总线上。

再一次，这儿所覆盖的一些概念和实现，驱动程序开发者不需要。在这个层面上的细节通常由总线处理，很少有开发者需要添加一条新的总线。但是，这些信息对于那些想要知道PCI，USB内部究竟是怎样实现的读者来说非常有用。

<h3 id="14.4.1">14.4.1 总线</h3>

总线是处理器和一个或多个设备之间的通道。设备模型的目的，所有的设备都通过总线连接，即使它是内部虚拟的`platform`总线。 总线可以互插-例如，USB控制器通常是一个PCI设备。设备模型表示总线与其控制的设备之间的实际连接。

在Linux设备模型中， 总线使用结构体`bus_type`表示，定义位于`<linux/device.h>`中，如下所示：

    struct bus_type {
        char *name;
        struct subsystem subsys;
        struct kset drivers;
        struct kset devices;
        int (*match)(struct device *dev, struct device_driver *drv);
        struct device *(*add)(struct device * parent, char * bus_id);
        int (*hotplug) (struct device *dev, char **envp,
        int num_envp, char *buffer, int buffer_size);
        /* 省略 */
    };

`name`表示总线的名称，比如说“PCI”。从这个结构体中，可以看出每个总线都有它自己的子系统`subsys`；但是， 这些子系统并不位于`sysfs`的顶层。 而是在该总线的下面。一个总线包含两个`kset`对象，分别表示这条总线已知的驱动和插入到该总线上所有的设备。然后， 就是一组函数。

<h4 id="14.4.1.1">14.4.1.1 总线注册</h4>

示例代码实现一条虚拟的总线， 称为`lddbus`。它的`bus_type`结构如下所示：

    struct bus_type ldd_bus_type = {
        .name = "ldd",
        .match = ldd_match,
        .hotplug = ldd_hotplug,
    };

请注意，`bus_type`结构成员只需要初始化一小部分就可以了；大部分工作由设备模型的核心部分完成。 但是， 我们必须指定总线的名称和其使用的方法。

调用函数`bus_register`注册一条新总线。

    ret = bus_register(&ldd_bus_type);
    if (ret)
        return ret;

检查返回值，如果成功，说明子系统被成功地添加到系统中；在`sysfs`下的`/sys/bus`中可见，并且可以开始添加设备。

从系统中移除总线，使用下面的函数：

    void bus_unregister(struct bus_type *bus);

<h4 id="14.4.1.2">14.4.1.2 总线方法</h4>

`bus_type`结构中常用的几种方法；它们作为设备核和各自的驱动之间的中间媒介存在。下面看`2.6.10`内核中定义的方法：

* int (*match)(struct device *device, struct device_driver *driver);

    每当添加新设备或驱动到总线上，调用该方法，也许会多次。如果给定的设备可以由给定的驱动处理，则返回非零值。（后面我们会讨论`device`和`device_driver`结构体）。这个函数必须在总线层处理，主要内核不知道如何为每种总线类型匹配设备和驱动。

* int (\*hotplug) (struct device *device, char **envp, int num_envp, char *buffer, int buffer_size);

    该方法允许总线在用户空间中生成`hotplug`事件之前，添加变量到环境中。参数和`kset`的`hotplug`方法相同（描述见于[热插拔事件的产生](#14.3)）。

> **hotplug**方法已经被**uevent**替代（内核3.3.7）。其原型为
>
>       int (*uevent)(struct device *dev, struct kobj_uevent_env *env);

`lddbus`驱动有一个简单的`match`函数，只是比较设备和驱动的名称：

    static int ldd_match(struct device *dev, struct device_driver *driver)
    {
        return !strncmp(dev->bus_id, driver->name, strlen(driver->name));
    }

> <font color=red>*`bud_id`这个成员已经被删除，应使用`shdev_name(dev)`代替`dev->bus_id`*</font>

当涉及真实的硬件时，`match`函数通常进行设备本身提供的硬件ID和驱动提供的ID之间作比较。

`lddbus`的热插拔方法，其原型为：

    static int ldd_hotplug(struct device *dev, char **envp, int num_envp,
            char *buffer, int buffer_size)
    {
        envp[0] = buffer;
        if (snprintf(buffer, buffer_size, "LDDBUS_VERSION=%s",
                    Version) >= buffer_size)
            return -ENOMEM;
        envp[1] = NULL;
        return 0;
    }

<font color=red>使用下面的方法代替(内核3.3.7)</font>

    static int ldd_uevent(struct device *dev, struct kobj_uevent_env *env)
    {
        env->envp[0] = env->buf;
        if (snprintf(env->buf, env->buflen, "LDDBUS_VERSION=%s",  Version) >= env->buflen)
            return -ENOMEM;
        env->envp[1] = NULL;
        return 0;
    }

其中，`kobj_uevent_env`的结构体定义如下：

    struct kobj_uevent_env {
        char *envp[UEVENT_NUM_ENVP];
        int envp_idx;
        char buf[UEVENT_BUFFER_SIZE];
        int buflen;
    };

在这里，我们添加`lddbus`的版本信息作为事件信息。

<h4 id="14.4.1.3">14.4.1.3 设备和驱动的迭代</h4>

编写总线级代码时，有时候， 必须对已使用总线注册的设备和驱动进行某些操作。当然，可以直接操作`bus_type`结构中的成员， 但是， 最好使用提供的辅助函数。 使用下面的函数操作总线上已知的每个设备：

    int bus_for_each_dev(struct bus_type *bus, struct device *start,
                        void *data, int (*fn)(struct device *, void *));

该函数迭代总线上的每一个设备，传递相关的`device`结构和作为数据传递的数值，给函数`fn`。如果`start`是`NULL`，从总线上的第一个设备开始进行迭代； 否则，从`start`之后的第一个设备开始迭代。如果`fn`返回非0值，则迭代结束，该值作为函数`bus_for_each_dev`的返回值。

迭代驱动使用相似的驱动：

    int bus_for_each_drv(struct bus_type *bus, struct device_driver *start,
                        void *data, int (*fn)(struct device_driver *, void *));

功能和 `bus_for_each_dev`类似，例外就是， 该函数用于操作驱动。

应该注意的是，这两个函数在工作期间都会占有总线子系统的读写互斥量。如果试图一起使用它们，可能会死锁-每一个函数都想获得同一个互斥量。修改总线的操作（比如，卸载设备）也会锁定。所以，请谨慎使用`bus_for_each`函数。

<h4 id="14.4.1.4">14.4.1.4 总线属性</h4>

Linux设备模型中，几乎每一层都提供了一个添加属性的接口，总线层也不例外。`bus_attribute`总线属性定义在文件`<linux/device.h>`中：

    struct bus_attribute {
        struct attribute attr;
        ssize_t (*show)(struct bus_type *bus, char *buf);
        ssize_t (*store)(struct bus_type *bus, const char *buf, size_t count);
    };

在[默认属性](#12.2.1)段中，我们已经讨论过`attribute`结构了。`bus_attribute`也包含两个方法，显示和设置属性的值。在`kobject`层之上的大部分设备模型都按这种方式工作。

一个方便初始化`bus_attribute`结构的宏，如下所示（编译时创建）：

    BUS_ATTR(name, mode, show, store);

这个宏声明了一个结构体，名称使用`bus_attr_`前缀加上给定的名称`name`。

创建和删除总线属性文件的函数如下：

    int bus_create_file(struct bus_type *bus, struct bus_attribute *attr);
    void bus_remove_file(struct bus_type *bus, struct bus_attribute *attr);

`lddbus`驱动创建了一个简单的属性文件，包含源代码版本号。`show`方法和`bus_attribute`结构的建立如下：

    static ssize_t show_bus_version(struct bus_type *bus, char *buf)
    {
        return snprintf(buf, PAGE_SIZE, "%s\n", Version);
    }
    static BUS_ATTR(version, S_IRUGO, show_bus_version, NULL);

在模块加载的时候创建属性文件：

    if (bus_create_file(&ldd_bus_type, &bus_attr_version))
        printk(KERN_NOTICE "Unable to create version attribute\n");

该调用创建一个属性文件(`/sys/bus/ldd/version`)，包含`lddbus`代码的版本号。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h3 id="14.4.2">14.4.2 设备</h3>

在Linux系统的最底层， 使用`struct device`的实例来表示每一个设备。

    struct device {
        struct device *parent;
        struct kobject kobj;
        char bus_id[BUS_ID_SIZE];
        struct bus_type *bus;
        struct device_driver *driver;
        void *driver_data;
        void (*release)(struct device *dev);
        /* 省略 */
    };

还有一些其它的成员，只有设备核心代码感兴趣。下面看一下其中主要的成员：

* struct device *parent

    设备的父设备—它所附加的设备。在大部分的情况下，父设备是一类总线或主控制器。如果`parent`等于`NULL`，这代表是一个顶层设备，这往往不会使用。

* struct kobject kobj;

    表示这个设备的`kobject`，把它链接到层次结构体中。 注意，作为通用规则，`device->kobj->parent`就等于`&device->parent->kobj`。

* char bus_id[BUS_ID_SIZE];

    在总线上唯一标识此设备的字符串。例如，PCI设备使用标准的PCI ID格式，里面包含域，总线，设备，和功能码等。

* struct bus_type *bus;

    识别设备所在的总线类型。

* struct device_driver *driver;

    管理此设备的驱动，我们将在下一节研究[`struct device_driver`](#14.4.3)。

* void *driver_data;

    设备驱动使用的私有数据

* void (*release)(struct device *dev);

    没有对该设备的引用时调用此方法； 它被内嵌`kobject`对象的release`方法调用。所有的使用核心代码注册的`device`结构都必须有一个`release`方法，否则，内核打印“可怕的投诉”。

注册`device`结构之前，至少设置`parent`，`bus_id`，`bus`和`release`成员。

<h4 id="14.4.2.1">14.4.2.1 设备注册</h4>

常用的注册和注销函数是：

    int device_register(struct device *dev);
    void device_unregister(struct device *dev);

我们已经看了`lddbus`是如何注册它的总线类型的。但是，实际的总线是设备，必须单独注册。为简单起见，`lddbus`模块仅支持一个简单的虚拟总线，所以驱动在编译时建立它的设备。

    static void ldd_bus_release(struct device *dev)
    {
        printk(KERN_DEBUG "lddbus release\n");
    }
    struct device ldd_bus = {
        .bus_id = "ldd0",
        .release = ldd_bus_release
    };

> <font color=red>*`bud_id`这个成员已经被删除，应使用`init_name`代替`dev->bus_id`*</font>

这是一个顶层总线，所以，`parent`和`bus`成员被设置为`NULL`。我们有一个没有操作的简单`release`方法，且作为第一条总线，名称设为`ldd0`。总线设备使用下面的代码进行注册：

    ret = device_register(&ldd_bus);
    if (ret)
        printk(KERN_NOTICE "Unable to register ldd0\n");

一旦调用完成，就能在`sysfs`中的`/sys/devices`下看见。添加到该总线上的设备都会在`/sys/devices/ldd0/`目录下看到。

<h4 id="14.4.2.2">14.4.2.2 设备属性</h4>

`sysfs`中的设备拥有属性，相关的结构是：

    struct device_attribute {
        struct attribute attr;
        ssize_t (*show)(struct device *dev, char *buf);
        ssize_t (*store)(struct device *dev, const char *buf, size_t count);
    };

> <font color=red>在内核3.3.7中，已经变更为</font>
>
>       struct device_attribute {
>           struct attribute attr;
>           ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
>           ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
>       };

可以使用此宏在编译时设置这些属性结构：

    DEVICE_ATTR(name, mode, show, store);

> <font color=red>在内核3.3.7中，已经变更为</font>
>
>       DEVICE_ATTR(_name, _mode, _show, _store);
>

在给定名称的前面添加`dev_attr_`字符串，作为生成的属性结构名称。属性文件的管理使用下面这2个函数：

    int device_create_file(struct device *device, struct device_attribute *entry);
    void device_remove_file(struct device *dev, struct device_attribute *attr);

> <font color=red>在内核3.3.7中，已经变更为</font>
>
>       int device_create_file(struct device *device, const struct device_attribute *entry);
>       void device_remove_file(struct device *dev, const struct device_attribute *attr);
>

`struct bus_type`的`dev_attrs`成员指向为每个添加到总线上的设备创建的默认属性列表。

<h4 id="14.4.2.3">14.4.2.3 设备结构嵌入</h4>

`device`结构已经包含设备模型核心为系统建模所需的信息。但是，大多数子系统都会跟踪有关其托管设备的其它信息。 因此， 设备很少用单纯的`device`结构来表示；相反，该结构，如`kobject`结构一样，通常嵌入在设备的更高级别表示中。 如果你看一下`struct pci_dev`或`struct usb_device`的定义，你会发现里面有一个`struct device`。通常，底层驱动程序甚至不知道“struct device”，但不是绝对的。

`lddbus`驱动创建自己的设备类型`struct ldd_device`且期望每一个设备驱动使用这个类型注册它们的设备。下面是这个简单的结构体：

    struct ldd_device {
        char *name;
        struct ldd_driver *driver;
        struct device dev;
    };

    #define to_ldd_device(dev) container_of(dev, struct ldd_device, dev);

该结构允许驱动程序提供设备的实际名称（可以与存储在`device structure`中的总线ID不同）和指向驱动信息的指针`driver`。通常， 真实的设备包含厂商、设备模型、设备配置，使用的资源等等信息。可以参考`struct pci_dev`（定义在`<linux/pci.h>`中）或`struct usb_device`（定义在`<linux/usb.h>`中）。一个方便的宏-`to_ldd_device`-被定义，方便地把指向内嵌的设备结构的指针转化成指向`ldd_device`指针。

`lddbus`导出的注册接口如下所示：

    int register_ldd_device(struct ldd_device *ldddev)
    {
        ldddev->dev.bus = &ldd_bus_type;
        ldddev->dev.parent = &ldd_bus;
        ldddev->dev.release = ldd_dev_release;
        strncpy(ldddev->dev.bus_id, ldddev->name, BUS_ID_SIZE);
        return device_register(&ldddev->dev);
    }
    EXPORT_SYMBOL(register_ldd_device);

> <font color=red>*`bud_id`这个成员已经被删除，所以*</font>
>
>       strncpy(ldddev->dev.bus_id, ldddev->name, BUS_ID_SIZE);
>
>       替换为
>
>       dev_set_name(&(ldddev->dev), "%s", ldddev->name);

在这里，我们填充内嵌的`device structure`的成员（各个驱动程序不需要了解），然后使用驱动核心注册设备。 如果我们想要将特定于总线的属性添加到设备，应该在此完成操作。

为了展示这个接口是如何工作的，我们再引入一个简单的驱动-`sculld`。这是第8章里引入的`scullp`驱动的变体。 它是一个普通的内存读写设备，但是`sculld`也可以使用设备模型实现，使用`lddbus`接口。

`sculld`驱动添加它自己的一个属性到其设备项中；这个属性，称为`dev`，只包含相关的设备号。加载脚本的模块或`hotplug`子系统可以使用此属性，在设备添加到系统内时，自动创建设备节点。此属性的设备通常使用下面的模式：

    static ssize_t sculld_show_dev(struct device *ddev, char *buf)
    {
        struct sculld_dev *dev = ddev->driver_data;
        return print_dev_t(buf, dev->cdev.dev);
    }
    static DEVICE_ATTR(dev, S_IRUGO, sculld_show_dev, NULL);

然后，在初始化时，设备被注册，且通过下面的函数创建`dev attribute`。

    static void sculld_register_dev(struct sculld_dev *dev, int index)
    {
        sprintf(dev->devname, "sculld%d", index);
        dev->ldev.name = dev->devname;
        dev->ldev.driver = &sculld_driver;
        dev->ldev.dev.driver_data = dev;
        register_ldd_device(&dev->ldev);
        device_create_file(&dev->ldev.dev, &dev_attr_dev);
    }

请注意，使用`driver_data`成员来存储指向我们自己内部的`device`结构。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h3 id="14.4.3">14.4.3 设备驱动</h3>

设备模型追踪系统已知的所有驱动。追踪这些驱动程序的主要原因是，使驱动核心代码能够为新设备匹配驱动。但是，一旦驱动成为系统中已知的对象，一切皆有可能。例如，设备驱动程序可以导出独立于任何特定设备的信息和配置变量。

驱动使用下面的结构定义：

    struct device_driver {
        char *name;
        struct bus_type *bus;
        struct kobject kobj;
        struct list_head devices;
        int (*probe)(struct device *dev);
        int (*remove)(struct device *dev);
        void (*shutdown) (struct device *dev);
    };

这里，再一次省略了一些成员，该结构体的定义位于`<linux/device.h>`。在这里，`name`是驱动的名称，会在`sysfs`中显示； `bus`是该驱动所占据的总线类型； `kobj`是必不可少的`kobject`对象； `devices`是绑定到该驱动上的所有设备； `probe`是一个查询特定设备是否存在的函数（以及驱动程序是否可以使用它）； `remove`是当从系统移除设备时调用的函数；`shutdown`是当系统关闭时调用的函数；

驱动的注册函数：

    int driver_register(struct device_driver *drv);
    void driver_unregister(struct device_driver *drv);

驱动的属性结构体如下所示：

    struct driver_attribute {
        struct attribute attr;
        ssize_t (*show)(struct device_driver *drv, char *buf);
        ssize_t (*store)(struct device_driver *drv, const char *buf, size_t count);
    };
    DRIVER_ATTR(name, mode, show, store);

创建属性文件的函数：

    int driver_create_file(struct device_driver *drv, struct driver_attribute *attr);
    void driver_remove_file(struct device_driver *drv, struct driver_attribute *attr);

`bus_type`包含一个指向一组默认属性的成员（`drv_attrs`），这是为该总线上所有设备创建的。

<h4 id="14.4.3.1">14.4.3.1 驱动结构嵌入</h4>

和大多数驱动核心结构一样，`device_driver`通常可以嵌入到更高层级的特定总线结构中。`lddbus`子系统也不例外，也定义了自己的`ldd_driver`结构：

    struct ldd_driver {
        char *version;
        struct module *module;
        struct device_driver driver;
        struct driver_attribute version_attr;
    };
    #define to_ldd_driver(drv) container_of(drv, struct ldd_driver, driver);

在这里，我们要求每个驱动程序提供其当前的软件版本，`lddbus`导出每个驱动的版本。特定于总线的驱动注册函数是：

    int register_ldd_driver(struct ldd_driver *driver)
    {
        int ret;

        driver->driver.bus = &ldd_bus_type;
        ret = driver_register(&driver->driver);
        if (ret)
            return ret;
        driver->version_attr.attr.name = "version";
        driver->version_attr.attr.owner = driver->module;
        driver->version_attr.attr.mode = S_IRUGO;
        driver->version_attr.show = show_version;
        driver->version_attr.store = NULL;
        return driver_create_file(&driver->driver, &driver->version_attr);
    }

函数的前半部分使用驱动核心代码注册一个底层的`device_driver`结构； 其余的代码建立了驱动代码的版本属性。 因为这个属性是在运行时建立， 所以， 不能再使用`DRIVER_ATTR`宏；因此，`driver_attribute`结构体必须手动填充。值得注意的是，设置属性的`owner`为驱动模块所有，而不是为`lddbus`模块所有；原因在该属性的`show`函数的实现中解释：

    static ssize_t show_version(struct device_driver *driver, char *buf)
    {
        struct ldd_driver *ldriver = to_ldd_driver(driver);
        sprintf(buf, "%s\n", ldriver->version);
        return strlen(buf);
    }

有人可能认为属性`version_attr`的所有者是`lddbus`模块，因为`show_version`函数是在`lddbus`模块的实现文件里定义的。但是， 该函数和结构体`ldd_driver`一起工作，而结构体是由驱动本身所创建并拥有。如果当用户空间的进程试图访问版本号时，`ldd_driver`结构体消失了，则事情将会混乱。将驱动模块作为属性的拥有者，将会保证，在用户空间打开属性文件的同时，驱动模块不能被卸载。因为， 每一个驱动模块都是创建一个`lddbus`模块的引用，所以可以确定`lddbus`不会在不合适的时间被卸载。

`sculld`创建的`ldd_driver`结构，如下所示：

static struct ldd_driver sculld_driver = {
    .version = "$Revision: 1.1 $",
    .module = THIS_MODULE,
    .driver = {
        .name = "sculld",
    },
};

调用`register_ldd_driver`将驱动添加到系统中，一旦初始化完成，可以在`sysfs`看到驱动信息：

    $ tree /sys/bus/ldd/drivers
    /sys/bus/ldd/drivers
    `-- sculld
     |-- sculld0 -> ../../../../devices/ldd0/sculld0
     |-- sculld1 -> ../../../../devices/ldd0/sculld1
     |-- sculld2 -> ../../../../devices/ldd0/sculld2
     |-- sculld3 -> ../../../../devices/ldd0/sculld3
     `-- version

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="14.5">14.5 类</h2>

The final device model concept we examine in this chapter is the `class`. A class is a higher-level view of a device that abstracts out low-level implementation details. Drivers may see a SCSI disk or an ATA disk, but, at the class level, they are all simply disks. Classes allow user space to work with devices based on what they do, rather than how they are connected or how they work.

Almost all classes show up in `sysfs` under `/sys/class`. Thus, for example, all network interfaces can be found under `/sys/class/net`, regardless of the type of interface. Input devices can be found in `/sys/class/input`, and serial devices are in `/sys/class/tty`. The one exception is block devices, which can be found under `/sys/block` for historical reasons.

Class membership is usually handled by high-level code without the need for explicit support from drivers. When the `sbull` driver (see Chapter 16) creates a virtual disk device, it automatically appears in `/sys/block`. The `snull` network driver (see Chapter 17) does not have to do anything special for its interfaces to be represented in `/sys/class/net`. There will be times, however, when drivers end up dealing with classes directly.

In many cases, the `class` subsystem is the best way of exporting information to user space. When a subsystem creates a class, it owns the class entirely, so there is no need to worry about which module owns the attributes found there. It also takes very little time wandering around in the more hardware-oriented parts of `sysfs` to realize that it can be an unfriendly place for direct browsing. Users more happily find information in `/sys/class/some-widget` than under, say, `/sys/devices/pci0000:00/0000:00:10.0/usb2/2-0:1.0`.

The driver core exports two distinct interfaces for managing classes. The `class_simple` routines are designed to make it as easy as possible to add new classes to the system; their main purpose, usually, is to expose attributes containing device numbers to enable the automatic creation of device nodes. The regular class interface is more complex but offers more features as well. We start with the simple version.

<h3 id="14.5.1">14.5.1 class_simple接口</h3>

The class_simple interface was intended to be so easy to use that nobody would have any excuse for not exporting, at a minimum, an attribute containing a device’s assigned number. Using this interface is simply a matter of a couple of function calls, with little of the usual boilerplate associated with the Linux device model.

The first step is to create the class itself. That is accomplished with a call to class_simple_create:

    struct class_simple *class_simple_create(struct module *owner, char *name);

This function creates a class with the given `name`. The operation can fail, of course, so the return value should always be checked (using IS_ERR, described in the section “Pointers and Error Values” in Chapter 1) before continuing.

A simple class can be destroyed with:

    void class_simple_destroy(struct class_simple *cs);

The real purpose of creating a simple class is to add devices to it; that task is achieved with:

    struct class_device *class_simple_device_add(struct class_simple *cs,
                                                    dev_t devnum,
                                                    struct device *device,
                                                    const char *fmt, ...);

Here, `cs` is the previously created simple class, `devnum` is the assigned device number, `device` is the `struct device` representing this device, and the remaining parameters are a printk-style format string and arguments to create the device name. This call adds an entry to the class containing one attribute, dev, which holds the device number. If the `device` parameter is not `NULL`, a symbolic link (called `device`) points to the device’s entry under /sys/devices.

It is possible to add other attributes to a `device` entry. It is just a matter of using `class_device_create_file`, which we discuss in the next section with the rest of the full class subsystem.

Classes generate hotplug events when devices come and go. If your driver needs to add variables to the environment for the user-space event handler, it can set up a hotplug callback with:

    int class_simple_set_hotplug(struct class_simple *cs,
                                int (*hotplug)(struct class_device *dev,
                                                char **envp, int num_envp,
                                                char *buffer, int buffer_size));

When your device goes away, the class entry should be removed with:

    void class_simple_device_remove(dev_t dev);

Note that the `class_device` structure returned by `class_simple_device_add` is not needed here; the device number (which should certainly be unique) is sufficient.

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h3 id="14.5.2">14.5.2 完整类接口</h3>

The `class_simple` interface suffices for many needs, but sometimes more flexibility is required. The following discussion describes how to use the full class mechanism, upon which `class_simple` is based. It is brief: the class functions and structures follow the same patterns as the rest of the device model, so there is little that is truly new here.

<h4 id="14.5.2.1">14.5.2.1 管理类</h4>

A class is defined by an instance of struct class:

    struct class {
        char *name;
        struct class_attribute *class_attrs;
        struct class_device_attribute *class_dev_attrs;
        int (*hotplug)(struct class_device *dev, char **envp,
        int num_envp, char *buffer, int buffer_size);
        void (*release)(struct class_device *dev);
        void (*class_release)(struct class *class);
        /* Some fields omitted */
    };

Each class needs a unique name, which is how this class appears under `/sys/class`. When the class is registered, all of the attributes listed in the (NULL-terminated) array pointed to by `class_attrs` is created. There is also a set of default attributes for every device added to the class; `class_dev_attrs` points to those. There is the usual `hotplug` function for adding variables to the environment when events are generated. There are also two `release` methods: `release` is called whenever a device is removed from the class, while `class_release` is called when the class itself is released.

The registration functions are:

    int class_register(struct class *cls);
    void class_unregister(struct class *cls);

The interface for working with attributes should not surprise anybody at this point:

    struct class_attribute {
        struct attribute attr;
        ssize_t (*show)(struct class *cls, char *buf);
        ssize_t (*store)(struct class *cls, const char *buf, size_t count);
    };
    CLASS_ATTR(name, mode, show, store);

    int class_create_file(struct class *cls, const struct class_attribute *attr);
    void class_remove_file(struct class *cls, const struct class_attribute *attr);

<h4 id="14.5.2.2">14.5.2.2 类设备</h4>

The real purpose of a class is to serve as a container for the devices that are members of that class. A member is represented by struct `class_device`:

    struct class_device {
        struct kobject kobj;
        struct class *class;
        struct device *dev;
        void *class_data;
        char class_id[BUS_ID_SIZE];
    };

The class_id field holds the name of this device as it appears in sysfs. The class pointer should point to the class holding this device,and dev should point to the associated device structure. Setting dev is optional; if it is non-NULL,it is used to create a symbolic link from the class entry to the corresponding entry under `/sys/devices`, making it easy to find the device entry in user space. The class can use `class_data` to hold a private pointer.

The usual registration functions have been provided:

    int class_device_register(struct class_device *cd);
    void class_device_unregister(struct class_device *cd);

The class device interface also allows the renaming of an already registered entry:

    int class_device_rename(struct class_device *cd, char *new_name);

Class device entries have attributes:

    struct class_device_attribute {
        struct attribute attr;
        ssize_t (*show)(struct class_device *cls, char *buf);
        ssize_t (*store)(struct class_device *cls, const char *buf, size_t count);
    };
    CLASS_DEVICE_ATTR(name, mode, show, store);

    int class_device_create_file(struct class_device *cls, const struct class_device_attribute *attr);
    void class_device_remove_file(struct class_device *cls, const struct class_device_attribute *attr);

A default set of attributes,in the class’s class_dev_attrs field,is created when the class device is registered; class_device_create_file may be used to create additional attributes. Attributes may also be added to class devices created with the class_simple interface.


<h4 id="14.5.2.3">14.5.2.3 类接口</h4>

The class subsystem has an additional concept not found in other parts of the Linux device model. This mechanism is called an interface,but it is,perhaps,best thought of as a sort of trigger mechanism that can be used to get notification when devices enter or leave the class.

An interface is represented by:

    struct class_interface {
        struct class *class;
        int (*add) (struct class_device *cd);
        void (*remove) (struct class_device *cd);
    };

Interfaces can be registered and unregistered with:

    int class_interface_register(struct class_interface *intf);
    void class_interface_unregister(struct class_interface *intf);

The functioning of an interface is straightforward. Whenever a class device is added to the class specified in the class_interface structure,the interface’s add function is called. That function can perform any additional setup required for that device; this setup often takes the form of adding more attributes,but other applications are possible. When the device is removed from the class,the remove method is called to perform any required cleanup.

可以为一个类注册多个接口。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="14.6">14.6 各个环节整合起来</h2>

为了更好地理解设备驱动模型的作用，让我们逐步介绍内核中设备的生命周期。 我们描述了PCI子系统如何与驱动模型交互， 如何添加和删除驱动，以及如何在系统中添加和删除设备的基本概念。 这些描述PCI内核代码的细节， 同样适用于那些使用驱动核心程序管理它们自己的驱动和设备的其它子系统。

PCI核心层，驱动核心层，各个PCI驱动之间的交互非常复杂，如图14-3所示：

![Figure 14-3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/14-3.PNG)

<h3 id="14.6.1">14.6.1 添加一个设备</h3>

PCI子系统声明了一个`struct bus_type`类型的结构，称为`pci_bus_type`，使用下面的值进行初始化：

    struct bus_type pci_bus_type = {
        .name = "pci",
        .match = pci_bus_match,
        .hotplug = pci_hotplug,
        .suspend = pci_device_suspend,
        .resume = pci_device_resume,
        .dev_attrs = pci_dev_attrs,
    };

当PCI子系统被加载到内核时，调用`bus_register`函数，把`pci_bus_type`变量向驱动核心层注册。这之后，驱动核心层就会在目录`/sys/bus/pci`下创建2个`sysfs`目录： `devices`和`drivers`。

所有的PCI驱动必须定义一个`struct pci_driver`类型变量， 它定义了该驱动所能实现的不同功能（关于PCI子系统和如何编写PCI驱动的更多信息， 可以参考第12章）。`struct pci_driver`结构包含了一个`struct device_driver`结构成员，当PCI驱动被注册时，由PCI核心层完成其初始化。

    /* 初始化常用的driver结构体成员 */
    drv->driver.name = drv->name;
    drv->driver.bus = &pci_bus_type;
    drv->driver.probe = pci_device_probe;
    drv->driver.remove = pci_device_remove;
    drv->driver.kobj.ktype = &pci_driver_kobj_type;

这段代码将驱动程序指向`pci_bus_type`结构的总线，`probe` 和 `remove`成员函数指向`PCI`核心代码层里的函数。 为了`PCI`驱动的属性文件能够正常工作， 驱动的`kobject`的`ktype`被设置为`pci_driver_kobj_type`。然后PCI核心层代码把`PCI`驱动注册到驱动核心层：

    /* 使用驱动核心代码注册 */
    error = driver_register(&drv->driver);

现在，驱动程序已经准备好驱动任何它所支持的PCI设备了。

在能和PCI总线通信的特定于体系架构的代码的帮助下，PCI核心层开始探测PCI地址空间，查找PCI设备。当PCI设备被发现，PCI核心层在内存中创建一个新的变量，类型为`struct pci_dev`。`struct pci_dev`结构的部分代码如下所示：

    struct pci_dev {
        /* ... */
        unsigned int devfn;/* 设备功能号，或PCI逻辑设备号(0-255)。其中，bit[7:3]是物理设备号，bit[2:0]是功能号 */
        unsigned short vendor; /* PCI设备的厂商ID */
        unsigned short device; /* PCI设备的设备ID */
        unsigned short subsystem_vendor; /* PCI设备的子系统厂商ID */
        unsigned short subsystem_device; /* PCI设备的子系统设备ID */
        unsigned int class;
        /* ... */
        struct pci_driver *driver;
        /* ... */
        struct device dev;
        /* ... */
    };

这个PCI设备与总线相关的成员由PCI核心层完成初始化（`devfn`，`vendor`，`device`，其它成员)，类型为`struct device`的成员`dev`的`parent`指针指向该PCI设备所依存的PCI总线设备。`bus`变量指向`pci_bus_type`结构，然后设置`name`和`bus_id`变量， 基于从PCI设备读到的名称和ID。

在上面这个PCI设备结构被初始化后，调用下面的函数向驱动核心层注册该设备。

    device_register(&dev->dev);

在函数`device_register`里，驱动核心代码初始化大部分`device`的成员，向`kobject`核心代码注册该设备的`kobject`对象（这会产生一个`hotplug`事件，我们稍后讨论），然后，通过`device`结构中的`parent`成员把该设备添加到设备列表中。这样做是为了所有设备按照正确顺序排列， 总是能够知道每个设备的层次结构。

The device is then added to the bus-specific list of all devices, in this example, the `pci_bus_type` list. Then the list of all drivers that are registered with the bus is walked, and the `match` function of the bus is called for every driver, specifying this device. For the `pci_bus_type` bus,the `match` function was set to point to the `pci_bus_match` function by the PCI core before the device was submitted to the driver core.

然后设备被添加到与总线相关的所有设备的列表中，在本例中，就是`pci_bus_type`列表。然后遍历这个列表中注册的所有驱动，对其每一个调用`match`函数，
The `pci_bus_match` function casts the `struct device` that was passed to it by the driver core,back into a `struct pci_dev`. It also casts the `struct device_driver` back into a `struct pci_driver` and then looks at the PCI device-specific information of the device and driver to see if the driver states that it can support this kind of device. If the `match` is not successful, the function returns 0 back to the driver core,and the driver core moves on to the next driver in its list.

If the match is successful, the function returns 1 back to the driver core. This causes the driver core to set the driver pointer in the `struct device` to point to this driver, and then it calls the `probe` function that is specified in the `struct device_driver`. Earlier, before the PCI driver was registered with the driver core, the `probe` variable was set to point at the `pci_device_probe` function. This function casts (yet again) the `struct device` back into a struct `pci_dev` and the `struct driver` that is set in the device back into a struct pci_driver. It again verifies that this driver states that it can support this device (which seems to be a redundant extra check for some unknown reason),increments the reference count of the device, and then calls the PCI driver’s probe function with a pointer to the struct pci_dev structure it should bind to.

If the PCI driver’s probe function determines that it can not handle this device for some reason,it returns a negative error value,which is propagated back to the driver core and causes it to continue looking through the list of drivers to match one up with this device. If the probe function can claim the device,it does all the initialization that it needs to do to handle the device properly,and then it returns 0 back up to the driver core. This causes the driver core to add the device to the list of all devices currently bound by this specific driver and creates a symlink within the driver’s directory in sysfs to the device that it is now controlling. This symlink allows users to see exactly which devices are bound to which devices. This can be seen as:

    $ tree /sys/bus/pci
    /sys/bus/pci/
    |-- devices
    | |-- 0000:00:00.0 -> ../../../devices/pci0000:00/0000:00:00.0
    | |-- 0000:00:00.1 -> ../../../devices/pci0000:00/0000:00:00.1
    | |-- 0000:00:00.2 -> ../../../devices/pci0000:00/0000:00:00.2
    | |-- 0000:00:02.0 -> ../../../devices/pci0000:00/0000:00:02.0
    | |-- 0000:00:04.0 -> ../../../devices/pci0000:00/0000:00:04.0
    | |-- 0000:00:06.0 -> ../../../devices/pci0000:00/0000:00:06.0
    | |-- 0000:00:07.0 -> ../../../devices/pci0000:00/0000:00:07.0
    | |-- 0000:00:09.0 -> ../../../devices/pci0000:00/0000:00:09.0
    | |-- 0000:00:09.1 -> ../../../devices/pci0000:00/0000:00:09.1
    | |-- 0000:00:09.2 -> ../../../devices/pci0000:00/0000:00:09.2
    | |-- 0000:00:0c.0 -> ../../../devices/pci0000:00/0000:00:0c.0
    | |-- 0000:00:0f.0 -> ../../../devices/pci0000:00/0000:00:0f.0
    | |-- 0000:00:10.0 -> ../../../devices/pci0000:00/0000:00:10.0
    | |-- 0000:00:12.0 -> ../../../devices/pci0000:00/0000:00:12.0
    | |-- 0000:00:13.0 -> ../../../devices/pci0000:00/0000:00:13.0
    | `-- 0000:00:14.0 -> ../../../devices/pci0000:00/0000:00:14.0
    `-- drivers
     |-- ALI15x3_IDE
     | `-- 0000:00:0f.0 -> ../../../../devices/pci0000:00/0000:00:0f.0
     |-- ehci_hcd
     | `-- 0000:00:09.2 -> ../../../../devices/pci0000:00/0000:00:09.2
     |-- ohci_hcd
     | |-- 0000:00:02.0 -> ../../../../devices/pci0000:00/0000:00:02.0
     | |-- 0000:00:09.0 -> ../../../../devices/pci0000:00/0000:00:09.0
     | `-- 0000:00:09.1 -> ../../../../devices/pci0000:00/0000:00:09.1
     |-- orinoco_pci
     | `-- 0000:00:12.0 -> ../../../../devices/pci0000:00/0000:00:12.0
     |-- radeonfb
     | `-- 0000:00:14.0 -> ../../../../devices/pci0000:00/0000:00:14.0
     |-- serial
     `-- trident
     `-- 0000:00:04.0 -> ../../../../devices/pci0000:00/0000:00:04.0

<h3 id="14.6.2">14.6.2 添加一个设备</h3>

A PCI device can be removed from a system in a number of different ways. All CardBus devices are really PCI devices in a different physical form factor,and the kernel PCI core does not differenciate between them. Systems that allow the removal or addition of PCI devices while the machine is still running are becoming more popular,and Linux supports them. There is also a fake PCI Hotplug driver that allows developers to test to see if their PCI driver properly handles the removal of a device while the system is running. This module is called fakephp and causes the kernel to think the PCI device is gone,but it does not allow users to physically remove a PCI device from a system that does not have the proper hardware to do so. See the documentation with this driver for more information on how to use it to test your PCI drivers.

The PCI core exerts a lot less effort to remove a device than it does to add it. When a PCI device is to be removed,the pci_remove_bus_device function is called. This function does some PCI-specific cleanups and housekeeping,and then calls the device_unregister function with a pointer to the struct pci_dev’s struct device member.

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="14.7">14.7 热插拔</h2>



<h4 id="14.7.2.7">14.7.2.7 笔记本坞站</h4>

如果一个“即插即用”的笔记本坞站从运行中的Linux系统上被添加或移除，就会产生“热插拔”时间。`/sbin/hotplug`将参数名称和`SUBSYSTEM`环境变量设置为`dock`。其它环境变量不用设置。


<h2 id="14.8">14.8 处理固件</h2>
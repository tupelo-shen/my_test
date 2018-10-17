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

Kobjects are the mechanism behind the sysfs virtual filesystem. For every directory found in `sysfs`,there is a kobject lurking somewhere within the kernel. Every kobject of interest also exports one or more attributes, which appear in that kobject’s `sysfs` directory as files containing kernel-generated information. This section examines how kobjects and sysfs interact at a low level.

Code that works with sysfs should include `<linux/sysfs.h>`.

Getting a kobject to show up in `sysfs` is simply a matter of calling `kobject_add`. We have already seen that function as the way to add a kobject to a `kset`; creating entries in sysfs is also part of its job. There are a couple of things worth knowing about how the `sysfs` entry is created:

* Sysfs entries for kobjects are always directories, so a call to `kobject_add` results in the creation of a directory in sysfs. Usually that directory contains one or more attributes; we see how attributes are specified shortly.
* The name assigned to the kobject (with kobject_set_name) is the name used for the sysfs directory. Thus, kobjects that appear in the same part of the sysfs hierarchy must have unique names. Names assigned to kobjects should also be reasonable file names: they cannot contain the slash character,and the use of white space is strongly discouraged.
* The sysfs entry is located in the directory corresponding to the kobject’s parent pointer. If `parent` is NULL when `kobject_add` is called, it is set to the kobject embedded in the new kobject’s kset; thus, the sysfs hierarchy usually matches the internal hierarchy created with ksets. If both parent and kset are NULL,the sysfs directory is created at the top level,which is almost certainly not what you want.

Using the mechanisms we have described so far,we can use a kobject to create an empty directory in sysfs. Usually,you want to do something a little more interesting than that, so it is time to look at the implementation of attributes.

<h3 id="14.2.1">14.2.1 缺省属性</h3>

当创建时，每一个`kobject`都会被设定一组默认属性。这些属性使用结构体`kobj_type`保存。这个结构体看起来如下所示：

    struct kobj_type {
        void (*release)(struct kobject *);
        struct sysfs_ops *sysfs_ops;
        struct attribute **default_attrs;
    };

The `default_attrs` field lists the attributes to be created for every kobject of this type, and `sysfs_ops` provides the methods to implement those attributes. We start with `default_attrs`, which points to an array of pointers to attribute structures:

    struct attribute {
        char *name;
        struct module *owner;
        mode_t mode;
    };

In this structure, `name` is the name of the attribute (as it appears within the kobject’s sysfs directory), `owner` is a pointer to the module (if any) that is responsible for the implementation of this attribute,and `mode` is the protection bits that are to be applied to this attribute. The mode is usually S_IRUGO for read-only attributes; if the attribute is writable,you can toss in S_IWUSR to give write access to root only (the macros for modes are defined in `<linux/stat.h>`). The last entry in the default_attrs list must be zero-filled.

The `default_attrs` array says what the attributes are but does not tell sysfs how to actually implement those attributes. That task falls to the `kobj_type->sysfs_ops` field, which points to a structure defined as:

    struct sysfs_ops {
        ssize_t (*show)(struct kobject *kobj, struct attribute *attr, char *buffer);
        ssize_t (*store)(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size);
    };

从用户空间读取属性时，`show`方法就会被调用，参数是指向`kobject`类型的对象和合适的`attribute`结构。该方法把给定属性的值存到`buffer`中，保证不要越界（最大值`PAGE_SIZE`字节）， 返回值是返回数据的真实长度。`sysfs`规定，每个属性都是一个简单， 符合人们读写习惯的值； 如果要返回很多信息，你可能会考虑把它拆分成多个属性。

`attr`指针决定请求哪个属性。有一些`show`方法，会对属性的名称作测试。 还有一些实现， 把包含其它信息的结构体内嵌到`attribute`结构体中， 而这些信息还需要返回给属性值；在这种情况下，可以在`show`方法中使用`container_of`，获取那个内嵌结构体的指针。

The `store` method is similar; it should decode the data stored in buffer (size contains the length of that data, which does not exceed `PAGE_SIZE`), store and respond to the new value in whatever way makes sense, and return the number of bytes actually decoded. The `store` method can be called only if the attribute’s permissions allow writes. When writing a `store` method, never forget that you are receiving arbitrary information from user space; you should validate it very carefully before taking any action in response. If the incoming data does not match expectations, return a negative error value rather than possibly doing something unwanted and unrecoverable. If your device exports a `self_destruct` attribute, you should require that a specific string be written there to invoke that functionality; an accidental, random write should yield only an error.

<h3 id="14.2.2">14.2.2 非缺省属性</h3>

In many cases,the kobject type’s default_attrs field describes all the attributes that kobject will ever have. But that’s not a restriction in the design; attributes can be added and removed to kobjects at will. If you wish to add a new attribute to a kobject’s sysfs directory, simply fill in an attribute structure and pass it to:

    int sysfs_create_file(struct kobject *kobj, struct attribute *attr);

If all goes well, the file is created with the name given in the attribute structure, and the return value is 0; otherwise, the usual negative error code is returned.

Note that the same `show()` and `store()` functions are called to implement operations on the new attribute. Before you add a new, nondefault attribute to a kobject, you should take whatever steps are necessary to ensure that those functions know how to implement that attribute.

To remove an attribute, call:

    int sysfs_remove_file(struct kobject *kobj, struct attribute *attr);

After the call, the attribute no longer appears in the kobject’s sysfs entry. Do be aware, however, that a user-space process could have an open file descriptor for that attribute and that show and store calls are still possible after the attribute has been removed.


<h3 id="14.2.3">14.2.3 二进制属性</h3>

The sysfs conventions call for all attributes to contain a single value in a human-readable text format. That said, there is an occasional, rare need for the creation of attributes that can handle larger chunks of binary data. That need really only comes about when data must be passed, untouched, between user space and the device. For example,uploading firmware to devices requires this feature. When such a device is encountered in the system, a user-space program can be started (via the hotplug mechanism); that program then passes the firmware code to the kernel via a binary sysfs attribute, as is shown in the section “The Kernel Firmware Interface.” Binary attributes are described with a `bin_attribute` structure:

    struct bin_attribute {
        struct attribute attr;
        size_t size;
        ssize_t (*read)(struct kobject *kobj, char *buffer,
                        loff_t pos, size_t size);
        ssize_t (*write)(struct kobject *kobj, char *buffer,
                        loff_t pos, size_t size);
    };

Here, `attr` is an `attribute` structure giving the name, owner, and permissions for the binary attribute, and `size` is the maximum size of the binary attribute (or 0 if there is no maximum). The `read` and `write` methods work similarly to the normal char driver equivalents; they can be called multiple times for a single load with a maximum of one page worth of data in each call. There is no way for sysfs to signal the last of a set of write operations, so code implementing a binary attribute must be able to determine the end of the data some other way.

Binary attributes must be created explicitly; they cannot be set up as default attributes. To create a binary attribute, call:

    int sysfs_create_bin_file(struct kobject *kobj,
                                struct bin_attribute *attr);

Binary attributes can be removed with:

    int sysfs_remove_bin_file(struct kobject *kobj,
                                struct bin_attribute *attr);

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

So far, we have seen a great deal of low-level infrastructures and a relative shortage of examples. We try to make up for that in the rest of this chapter as we get into the higher levels of the Linux device model. To that end, we introduce a new virtual bus, which we call lddbus, * and modify the `scullp` driver to “connect” to that bus.

Once again, much of the material covered here will never be needed by many driver authors. Details at this level are generally handled at the bus level, and few authors need to add a new bus type. This information is useful, however, for anybody wondering what is happening inside the PCI, USB, etc. layers or who needs to make changes at that level.

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

使用下面的方法代替(内核3.3.7)

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

Almost every layer in the Linux device model provides an interface for the addition of attributes, and the bus layer is no exception. The `bus_attribute` type is defined in `<linux/device.h>` as follows:

    struct bus_attribute {
        struct attribute attr;
        ssize_t (*show)(struct bus_type *bus, char *buf);
        ssize_t (*store)(struct bus_type *bus, const char *buf,
                        size_t count);
    };

We have already seen struct `attribute` in the section “Default Attributes.” The `bus_attribute` type also includes two methods for displaying and setting the value of the attribute. Most device model layers above the kobject level work this way.

A convenience macro has been provided for the compile-time creation and initialization of `bus_attribute` structures:

    BUS_ATTR(name, mode, show, store);

这个宏声明了一个结构体，名称使用`bus_attr_`前缀加上给定的名称`name`。

创建和删除总线属性文件的函数如下：

    int bus_create_file(struct bus_type *bus, struct bus_attribute *attr);
    void bus_remove_file(struct bus_type *bus, struct bus_attribute *attr);

The `lddbus` driver creates a simple attribute file containing, once again, the source version number. The show `method` and `bus_attribute` structure are set up as follows:

    static ssize_t show_bus_version(struct bus_type *bus, char *buf)
    {
        return snprintf(buf, PAGE_SIZE, "%s\n", Version);
    }
    static BUS_ATTR(version, S_IRUGO, show_bus_version, NULL);

Creating the attribute file is done at module load time:

    if (bus_create_file(&ldd_bus_type, &bus_attr_version))
        printk(KERN_NOTICE "Unable to create version attribute\n");

This call creates an attribute file (`/sys/bus/ldd/version`) containing the revision number for the `lddbus` code.

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
        ssize_t (*store)(struct device *dev, const char *buf,
                        size_t count);
    };

在内核3.3.7中，已经变更为

    struct device_attribute {
        struct attribute attr;
        ssize_t (*show)(struct device *dev, struct device_attribute *attr,
            char *buf);
        ssize_t (*store)(struct device *dev, struct device_attribute *attr,
            const char *buf, size_t count);
    };

可以使用此宏在编译时设置这些属性结构：

    DEVICE_ATTR(name, mode, show, store);

在给定名称的前面添加`dev_attr_`字符串，作为生成的属性结构名称。属性文件的管理使用下面这2个函数：

    int device_create_file(struct device *device,
                            struct device_attribute *entry);
    void device_remove_file(struct device *dev,
                            struct device_attribute *attr);

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
        ssize_t (*store)(struct device_driver *drv, const char *buf,
        size_t count);
    };
    DRIVER_ATTR(name, mode, show, store);

创建属性文件的函数：

    int driver_create_file(struct device_driver *drv,
                            struct driver_attribute *attr);
    void driver_remove_file(struct device_driver *drv,
                            struct driver_attribute *attr);

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

函数的前半部分使用驱动核心代码注册一个底层的`device_driver`结构； 其余的代码建立了版本属性。因为这个属性是在运行时建立， 所以， 不能再使用`DRIVER_ATTR`宏；因此，`driver_attribute`结构体必须手动填充。值得注意的是，设置属性的`owner`为驱动模块所有，而不是为`lddbus`模块所有；原因在该属性的`show`函数的实现中解释：

    static ssize_t show_version(struct device_driver *driver, char *buf)
    {
        struct ldd_driver *ldriver = to_ldd_driver(driver);
        sprintf(buf, "%s\n", ldriver->version);
        return strlen(buf);
    }

One might think that the attribute owner should be the lddbus module, since the function that implements the attribute is defined there. This function, however, is working with the `ldd_driver` structure created (and owned) by the driver itself. If that structure were to go away while a user-space process tried to read the version number, things could get messy. Designating the driver module as the owner of the attribute prevents the module from being unloaded, while user-space holds the attribute file open. Since each driver module creates a reference to the `lddbus` module, we can be sure that `lddbus` will not be unloaded at an inopportune time.

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

<h2 id="14.5">14.5 类</h2>

<h2 id="14.6">14.6 集成起来</h2>

<h2 id="14.7">14.7 热插拔</h2>



<h4 id="14.7.2.7">14.7.2.7 笔记本坞站</h4>

如果一个“即插即用”的笔记本坞站从运行中的Linux系统上被添加或移除，就会产生“热插拔”时间。`/sbin/hotplug`将参数名称和`SUBSYSTEM`环境变量设置为`dock`。其它环境变量不用设置。


<h2 id="14.8">14.8 处理固件</h2>
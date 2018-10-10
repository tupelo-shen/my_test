* [14.1 Kobjects，Ksets和Subsystems](#14.1)
    - [14.1.1 Kobject基本知识](#14.1.1)
        + [14.1.1.1 嵌入的内核对象](#14.1.1.1)
        + [14.1.1.2 内核对象的初始化](#14.1.1.2)
        + [14.1.1.3 引用计数操作](#14.1.1.3)
        + [14.1.1.4 Release functions and kobject types](#14.1.1.4)
    - [14.1.2 Kobject Hierarchies, Ksets, and Subsystems](#14.1.2)
        + [14.1.2.1 Ksets](#14.1.2.1)
        + [14.1.2.2 Operations on ksets](#14.1.2.2)
        + [14.1.2.3 Subsystems](#14.1.2.3)
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

<h2 id="14.1">14.1 Kobjects，Ksets和Subsystems</h2>

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

Note that,in many cases,the reference count in the kobject itself may not be sufficient to prevent race conditions. The existence of a kobject (and its containing structure) may well,for example,require the continued existence of the module that created that kobject. It would not do to unload that module while the kobject is still being passed around. That is why the cdev structure we saw above contains a struct module pointer. Reference counting for struct cdev is implemented as follows:

请注意，在许多情况下，`kobject`本身的引用计数可能不足以防止竞争条件。 例如，`kobject`（及其包含它的结构）的存在可能需要继续存在创建该kobject的模块。 当kobject仍然被传递时，它不会卸载该模块。 这就是我们上面看到的cdev结构包含结构模块指针的原因。 struct cdev的引用计数实现如下：

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

Creating a reference to a cdev structure requires creating a reference also to the module that owns it. So cdev_get uses try_module_get to attempt to increment that module’s usage count. If that operation succeeds, kobject_get is used to increment the kobject’s reference count as well. That operation could fail,of course,so the code checks the return value from kobject_get and releases its reference to the module if things don’t work out.

<h4 id="14.1.1.4">14.1.1.4 Release functions and kobject types</h4>

One important thing still missing from the discussion is what happens to a kobject
when its reference count reaches 0. The code that created the kobject generally does
not know when that will happen; if it did,there would be little point in using a reference count in the first place. Even predictable object life cycles become more complicated when sysfs is brought in; user-space programs can keep a reference to a kobject
(by keeping one of its associated sysfs files open) for an arbitrary period of time.

The end result is that a structure protected by a kobject cannot be freed at any single,predictable point in the driver’s lifecycle,but in code that must be prepared to
run at whatever moment the kobject’s reference count goes to 0. The reference count
is not under the direct control of the code that created the kobject. So that code must
be notified asynchronously whenever the last reference to one of its kobjects goes
away.

This notification is done through a kobject’s release method. Usually,this method has a form such as:

    void my_object_release(struct kobject *kobj)
    {
        struct my_object *mine = container_of(kobj, struct my_object, kobj);
        /* Perform any additional cleanup on this object, then... */
        kfree(mine);
    }

One important point cannot be overstated: every kobject must have a release
method,and the kobject must persist (in a consistent state) until that method is
called. If these constraints are not met,the code is flawed. It risks freeing the object
when it is still in use,or it fails to release the object after the last reference is
returned.

Interestingly,the release method is not stored in the kobject itself; instead,it is associated with the type of the structure that contains the kobject. This type is tracked
with a structure of type struct kobj_type,often simply called a “ktype.” This structure looks like the following:

    struct kobj_type {
        void (*release)(struct kobject *);
        struct sysfs_ops *sysfs_ops;
        struct attribute **default_attrs;
    };

The release field in struct kobj_type is,of course,a pointer to the release method
for this type of kobject. We will come back to the other two fields (sysfs_ops and
default_attrs) later in this chapter.

Every kobject needs to have an associated kobj_type structure. Confusingly,the
pointer to this structure can be found in two different places. The kobject structure
itself contains a field (called ktype) that can contain this pointer. If,however,this
kobject is a member of a kset,the kobj_type pointer is provided by that kset instead.
(We will look at ksets in the next section.) Meanwhile, the macro:

    struct kobj_type *get_ktype(struct kobject *kobj);

finds the kobj_type pointer for a given kobject.

<h3 id="14.1.2">14.1.2 Kobject Hierarchies, Ksets, and Subsystems</h3>

The kobject structure is often used to link together objects into a hierarchical structure that matches the structure of the subsystem being modeled. There are two separate mechanisms for this linking: the parent pointer and ksets.

The parent field in struct kobject is a pointer to another kobject—the one representing the next level up in the hierarchy. If,for example,a kobject represents a USB
device,its parent pointer may indicate the object representing the hub into which the
device is plugged.

The main use for the parent pointer is to position the object in the sysfs hierarchy.
We’ll see how this works in the section “Low-Level Sysfs Operations.”

<h4 id="14.1.2.1">14.1.2.1 Ksets</h4>

In many ways,a kset looks like an extension of the kobj_type structure; a kset is a
collection of kobjects embedded within structures of the same type. However,while
struct kobj_type concerns itself with the type of an object, struct kset is concerned
with aggregation and collection. The two concepts have been separated so that
objects of identical type can appear in distinct sets.

Therefore,the main function of a kset is containment; it can be thought of as the
top-level container class for kobjects. In fact,each kset contains its own kobject
internally,and it can,in many ways,be treated the same way as a kobject. It is worth
noting that ksets are always represented in sysfs; once a kset has been set up and
added to the system,there will be a sysfs directory for it. Kobjects do not necessarily
show up in sysfs, but every kobject that is a member of a kset is represented there.

Adding a kobject to a kset is usually done when the object is created; it is a two-step
process. The kobject’s kset field must be pointed at the kset of interest; then the
kobject should be passed to:

    int kobject_add(struct kobject *kobj);

As always,programmers should be aware that this function can fail (in which case it
returns a negative error code) and respond accordingly. There is a convenience function provided by the kernel:

    extern int kobject_register(struct kobject *kobj);

This function is simply a combination of kobject_init and kobject_add.

When a kobject is passed to kobject_add,its reference count is incremented. Containment within the kset is,after all,a reference to the object. At some point,the
kobject will probably have to be removed from the kset to clear that reference; that is
done with:

    void kobject_del(struct kobject *kobj);

There is also a kobject_unregister function,which is a combination of kobject_del and
kobject_put.

A kset keeps its children in a standard kernel linked list. In almost all cases,the contained kobjects also have pointers to the kset (or,strictly,its embedded kobject) in
their parent’s fields. So,typically,a kset and its kobjects look something like what
you see in Figure 14-2. Bear in mind that:

* All of the contained kobjects in the diagram are actually embedded within some other type, possibly even other ksets.
* It is not required that a kobject’s parent be the containing kset (although any other organization would be strange and rare).

![Figure 14-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/14-2.PNG)

<h4 id="14.1.2.2">14.1.2.2 Operations on ksets</h4>

For initialization and setup,ksets have an interface very similar to that of kobjects. The following functions exist:

    void kset_init(struct kset *kset);
    int kset_add(struct kset *kset);
    int kset_register(struct kset *kset);
    void kset_unregister(struct kset *kset);

For the most part,these functions just call the analogous kobject_ function on the kset’s embedded kobject.

To manage the reference counts of ksets, the situation is about the same:

    struct kset *kset_get(struct kset *kset);
    void kset_put(struct kset *kset);

A kset also has a name,which is stored in the embedded kobject. So,if you have a kset called my_set, you would set its name with:

    kobject_set_name(&my_set->kobj, "The name");

Ksets also have a pointer (in the ktype field) to the kobj_type structure describing the
kobjects it contains. This type is used in preference to the ktype field in a kobject
itself. As a result,in typical usage,the ktype field in struct kobject is left NULL,
because the same field within the kset is the one actually used.

Finally,a kset contains a subsystem pointer (called subsys). So it’s time to talk about subsystems.

<h4 id="14.1.2.3">14.1.2.3 Subsystems</h4>

A subsystem is a representation for a high-level portion of the kernel as a whole. Subsystems usually (but not always) show up at the top of the sysfs hierarchy. Some example subsystems in the kernel include block_subsys (/sys/block,for block devices), devices_subsys (/sys/devices,the core device hierarchy),and a specific subsystem for every bus type known to the kernel. A driver author almost never needs to create a new subsystem; if you feel tempted to do so,think again. What you probably want, in the end, is to add a new class, as discussed in the section “Classes.”

A subsystem is represented by a simple structure:

    struct subsystem {
        struct kset kset;
        struct rw_semaphore rwsem;
    };

A subsystem, thus, is really just a wrapper around a kset, with a semaphore thrown in.

Every kset must belong to a subsystem. The subsystem membership helps establish the kset’s position in the hierarchy,but,more importantly,the subsystem’s rwsem semaphore is used to serialize access to a kset’s internal-linked list. This membership is represented by the subsys pointer in struct kset. Thus,one can find each kset’s containing subsystem from the kset’s structure,but one cannot find the multiple ksets contained in a subsystem directly from the subsystem structure.

Subsystems are often declared with a special macro:

    decl_subsys(name, struct kobj_type *type, struct kset_hotplug_ops *hotplug_ops);

This macro creates a struct subsystem with a name formed by taking the name given
to the macro and appending _subsys to it. The macro also initializes the internal kset
with the given type and hotplug_ops. (We discuss hotplug operations later in this
chapter.)

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
[TOC]

## 1 设备驱动模型

Earlier versions of the Linux kernel offered few basic functionalities to the device driver developers: allocating dynamic memory, reserving a range of I/O addresses or an IRQ line, activating an interrupt service routine in response to a device’s interrupt. Older hardware devices, in fact, were cumbersome and difficult to program, and two different hardware devices had little in common even if they were hosted on the same bus. Thus, there was no point in trying to offer a unifying model to the device driver developers.

Things are different now. Bus types such as PCI put strong demands on the internal design of the hardware devices; as a consequence, recent hardware devices, even of different classes, support similar functionalities. Drivers for such devices should typically take care of:

* 电源管理(handling of different voltage levels on the device’s power line)

* 即插即用(transparent allocation of resources when configuring the device)

* 热插拔(support for insertion and removal of the device while the system is running)

Power management is performed globally by the kernel on every hardware device in the system. For instance, when a battery-powered computer enters the “standby” state, the kernel must force every hardware device (hard disks, graphics card, sound card, network card, bus controllers, and so on) in a low-power state. Thus, each driver of a device that can be put in the “standby” state must include a callback function that puts the hardware device in the low-power state. Moreover, the hardware devices must be put in the “standby” state in a precise order, otherwise some devices could be left in the wrong power state. For instance, the kernel must put in “standby” first the hard disks and then their disk controller, because in the opposite case it would be impossible to send commands to the hard disks.

To implement these kinds of operations, Linux 2.6 provides some data structures and helper functions that offer a unifying view of all buses, devices, and device drivers in the system; this framework is called the device driver model.

## 2 sysfs文件系统

The sysfs filesystem is a special filesystem similar to /proc that is usually mounted on
the /sys directory. The /proc filesystem was the first special filesystem designed to
allow User Mode applications to access kernel internal data structures. The /sysfs
filesystem has essentially the same objective, but it provides additional information
on kernel data structures; furthermore, /sysfs is organized in a more structured way
than /proc. Likely, both /proc and /sysfs will continue to coexist in the near future.

A goal of the sysfs filesystem is to expose the hierarchical relationships among the
components of the device driver model. The related top-level directories of this filesystem
are:

block
The block devices, independently from the bus to which they are connected.
devices
All hardware devices recognized by the kernel, organized according to the bus in
which they are connected.
bus
The buses in the system, which host the devices.
drivers
The device drivers registered in the kernel.
class
The types of devices in the system (audio cards, network cards, graphics cards,
and so on); the same class may include devices hosted by different buses and
driven by different drivers.
power
Files to handle the power states of some hardware devices.
firmware
Files to handle the firmware of some hardware devices.

Relationships between components of the device driver models are expressed in the
sysfs filesystem as symbolic links between directories and files. For example, the /sys/
block/sda/device file can be a symbolic link to a subdirectory nested in /sys/devices/
pci0000:00 representing the SCSI controller connected to the PCI bus. Moreover, the
/sys/block/sda/device/block file is a symbolic link to /sys/block/sda, stating that this
PCI device is the controller of the SCSI disk.

The main role of regular files in the sysfs filesystem is to represent attributes of drivers
and devices. For instance, the dev file in the /sys/block/hda directory contains the
major and minor numbers of the master disk in the first IDE chain.

## 3 Kobjects

The core data structure of the device driver model is a generic data structure named
kobject, which is inherently tied to the sysfs filesystem: each kobject corresponds to a
directory in that filesystem.

Kobjects are embedded inside larger objects—the so-called “containers”—that
describe the components of the device driver model.* The descriptors of buses,
devices, and drivers are typical examples of containers; for instance, the descriptor of
the first partition in the first IDE disk corresponds to the /sys/block/hda/hda1 directory.

Embedding a kobject inside a container allows the kernel to:

* Keep a reference counter for the container;
* Maintain hierarchical lists or sets of containers (for instance, a sysfs directory associated with a block device includes a different subdirectory for each disk partition)
* Provide a User Mode view for the attributes of the container

#### 3.1 Kobjects, ksets, and subsystems

A kobject is represented by a kobject data structure, whose fields are listed in
Table 13-2.

Table 13-2. The fields of the kobject data structure

Type Field Description
char * k_name Pointer to a string holding the name of the container
char [] name String holding the name of the container, if it fits in 20 bytes
struct k_ref kref The reference counter for the container
struct list_head entry Pointers for the list in which the kobject is inserted
struct kobject * parent Pointer to the parent kobject, if any
struct kset * kset Pointer to the containing kset
struct kobj_type * ktype Pointer to the kobject type descriptor
struct dentry * dentry Pointer to the dentry of the sysfs file associated with the kobject

The ktype field points to a kobj_type object representing the “type” of the kobject—
essentially, the type of the container that includes the kobject. The kobj_type data
structure includes three fields: a release method (executed when the kobject is being
freed), a sysfs_ops pointer to a table of sysfs operations, and a list of default
attributes for the sysfs filesystem.

> Kobjects are mainly used to implement the device driver model; however, there is an ongoing effort to change some other kernel components—such as the module subsystem—so as to use them.

The kref field is a structure of type k_ref consisting of a single refcount field. As the
name implies, this field is the reference counter for the kobject, but it may act also as
the reference counter for the container of the kobject. The kobject_get() and
kobject_put() functions increase and decrease, respectively, the reference counter; if
the counter reaches the value zero, the resources used by the kobject are released and
the release method of the kobj_type object of the kobject is executed. This method,
which is usually defined only if the container of the kobject was allocated dynamically,
frees the container itself.

The kobjects can be organized in a hierarchical tree by means of ksets. A kset is a collection
of kobjects of the same type—that is, included in the same type of container.
The fields of the kset data structure are listed in Table 13-3.

Table 13-3. The fields of the kset data structure
Type Field Description
struct subsystem * subsys Pointer to the subsystem descriptor
struct kobj_type * ktype Pointer to the kobject type descriptor of the kset
struct list_head list Head of the list of kobjects included in the kset
struct kobject kobj Embedded kobject (see text)
struct kset_hotplug_ops * hotplug_ops Pointer to a table of callback functions for kobject filtering and
hot-plugging

The list field is the head of the doubly linked circular list of kobjects included in the
kset; the ktype field points to the same kobj_type descriptor shared by all kobjects in
the kset.
The kobj field is a kobject embedded in the kset data structure; the parent field of
the kobjects contained in the kset points to this embedded kobject. Thus, a kset is a
collection of kobjects, but it relies on a kobject of higher level for reference counting
and linking in the hierarchical tree. This design choice is code-efficient and allows
the greatest flexibility. For instance, the kset_get() and kset_put() functions, which
increase and decrease respectively the reference counter of the kset, simply invoke
kobject_get() and kobject_put() on the embedded kobject; because the reference
counter of a kset is merely the reference counter of the kobj kobject embedded in the
kset. Moreover, thanks to the embedded kobject, the kset data structure can be
embedded in a “container” object, exactly as for the kobject data structure. Finally, a
kset can be made a member of another kset: it suffices to insert the embedded kobject
in the higher-level kset.

Collections of ksets called subsystems also exist. A subsystem may include ksets of
different types, and it is represented by a subsystem data structure having just two
fields:

kset
An embedded kset that stores the ksets included in the subsystem

rwsem
A read-write semaphore that protects all ksets and kobjects recursively included
in the subsystem
Even the subsystem data structure can be embedded in a larger “container” object;
the reference counter of the container is thus the reference counter of the embedded
subsystem—that is, the reference counter of the kobject embedded in the kset
embedded in the subsystem. The subsys_get() and subsys_put() functions respectively
increase and decrease this reference counter.
Figure 13-3 illustrates an example of the device driver model hierarchy. The bus subsystem
includes a pci subsystem, which, in turn, includes a drivers kset. This kset
contains a serial kobject—corresponding to the device driver for the serial port—
having a single new-id attribute.

<img id="Figure_3-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_13_3.PNG">

#### 3.2 Registering kobjects, ksets, and subsystems

As a general rule, if you want a kobject, kset, or subsystem to appear in the sysfs subtree,
you must first register it. The directory associated with a kobject always appears
in the directory of the parent kobject. For instance, the directories of kobjects
included in the same kset appear in the directory of the kset itself. Therefore, the
structure of the sysfs subtree represents the hierarchical relationships between the
various registered kobjects and, consequently, between the various container objects.

Usually, the top-level directories of the sysfs filesystem are associated with the registered
subsystems.
The kobject_register() function initializes a kobject and adds the corresponding
directory to the sysfs filesystem. Before invoking it, the caller should set the kset field
in the kobject so that it points to the parent kset, if any. The kobject_unregister()
function removes a kobject’s directory from the sysfs filesystem. To make life easier
for kernel developers, Linux also offers the kset_register() and kset_unregister()
functions, and the subsystem_register() and subsystem_unregister() functions, but
they are essentially wrapper functions around kobject_register() and kobject_
unregister().
As stated before, many kobject directories include regular files called attributes. The
sysfs_create_file() function receives as its parameters the addresses of a kobject
and an attribute descriptor, and creates the special file in the proper directory. Other
relationships between the objects represented in the sysfs filesystem are established
by means of symbolic links: the sysfs_create_link() function creates a symbolic link
for a given kobject in a directory associated with another kobject.

## 4 Components of the Device Driver Model

The device driver model is built upon a handful of basic data structures, which represent
buses, devices, device drivers, etc. Let us examine them.

#### 4.1 Devices

Each device in the device driver model is represented by a device object, whose fields
are shown in Table 13-4.
Table 13-4. The fields of the device object
Type Field Description
struct list_head node Pointers for the list of sibling devices
struct list_head bus_list Pointers for the list of devices on the same bus
type
struct list_head driver_list Pointers for the driver’s list of devices
struct list_head children Head of the list of children devices
struct device * parent Pointer to the parent device
struct kobject kobj Embedded kobject
char [] bus_id Device position on the hosting bus
struct bus_type * bus Pointer to the hosting bus
struct device_driver
*
driver Pointer to the controlling device driver
void * driver_data Pointer to private data for the driver
void * platform_data Pointer to private data for legacy device drivers
struct dev_pm_info power Power management information
unsigned long detach_state Power state to be entered when unloading the
device driver
unsigned long long * dma_mask Pointer to the DMA mask of the device (see the
later section “Direct Memory Access (DMA)”)
unsigned long long coherent_dma_mask Mask for coherent DMA of the device
struct list_head dma_pools Head of a list of aggregate DMA buffers
struct
dma_coherent_mem *
dma_mem Pointer to a descriptor of the coherent DMA
memory used by the device (see the later section
“Direct Memory Access (DMA)”)
void (*)(struct
device *)
release Callback function for releasing the device
descriptor

The device objects are globally collected in the devices_subsys subsystem, which is
associated with the /sys/devices directory (see the earlier section “Kobjects”). The
devices are organized hierarchically: a device is the “parent” of some “children”
devices if the children devices cannot work properly without the parent device. For
instance, in a PCI-based computer, a bridge between the PCI bus and the USB bus is
the parent device of every device hosted on the USB bus. The parent field of the
device object points to the descriptor of the parent device, the children field is the
head of the list of children devices, and the node field stores the pointers to the adjacent
elements in the children list. The parenthood relationships between the kobjects
embedded in the device objects reflect also the device hierarchy; thus, the
structure of the directories below /sys/devices matches the physical organization of
the hardware devices.

Each driver keeps a list of device objects including all managed devices; the driver_
list field of the device object stores the pointers to the adjacent elements, while the
driver field points to the descriptor of the device driver. For each bus type, moreover,
there is a list including all devices that are hosted on the buses of the given
type; the bus_list field of the device object stores the pointers to the adjacent elements,
while the bus field points to the bus type descriptor.

A reference counter keeps track of the usage of the device object; it is included in the
kobj kobject embedded in the descriptor. The counter is increased by invoking get_
device(), and it is decreased by invoking put_device().
The device_register() function inserts a new device object in the device driver
model, and automatically creates a new directory for it under /sys/devices. Conversely,
the device_unregister() function removes a device from the device driver model.
Usually, the device object is statically embedded in a larger descriptor. For instance,
PCI devices are described by pci_dev data structures; the dev field of this structure is
a device object, while the other fields are specific to the PCI bus. The device_
register() and device_unregister() functions are executed when the device is being
registered or de-registered in the PCI kernel layer.

#### 4.2 Drivers

Each driver in the device driver model is described by a device_driver object, whose
fields are listed in Table 13-5.

Table 13-5. The fields of the device_driver object
Type Field Description
char * name Name of the device driver
struct bus_type * bus Pointer to descriptor of the bus that hosts the supported
devices
struct semaphore unload_sem Semaphore to forbid device driver unloading; it is
released when the reference counter reaches zero
struct kobject kobj Embedded kobject
struct list_head devices Head of the list including all devices supported by
the driver
struct module * owner Identifies the module that implements the device
driver, if any (see Appendix B)
int (*)(struct device *) probe Method for probing a device (checking that it can be
handled by the device driver)
int (*)(struct device *) remove Method invoked on a device when it is removed
void (*)(struct device *) shutdown Method invoked on a device when it is powered off
(shut down)
int (*)(struct device *,
unsigned long, unsigned long)
suspend Method invoked on a device when it is put in lowpower
state
int (*)(struct device *,
unsigned long)
resume Method invoked on a device when it is put back in
the normal state (full power)

The device_driver object includes four methods for handling hot-plugging, plug and
play, and power management. The probe method is invoked whenever a bus device
driver discovers a device that could possibly be handled by the driver; the corresponding
function should probe the hardware to perform further checks on the
device. The remove method is invoked on a hot-pluggable device whenever it is
removed; it is also invoked on every device handled by the driver when the driver
itself is unloaded. The shutdown, suspend, and resume methods are invoked on a
device when the kernel must change its power state.

The reference counter included in the kobj kobject embedded in the descriptor keeps
track of the usage of the device_driver object. The counter is increased by invoking
get_driver(), and it is decreased by invoking put_driver().

The driver_register() function inserts a new device_driver object in the device
driver model, and automatically creates a new directory for it in the sysfs filesystem.
Conversely, the driver_unregister() function removes a driver from the device
driver model.

Usually, the device_driver object is statically embedded in a larger descriptor. For
instance, PCI device drivers are described by pci_driver data structures; the driver
field of this structure is a device_driver object, while the other fields are specific to
the PCI bus.

#### 4.3 Buses

Each bus type supported by the kernel is described by a bus_type object, whose fields are listed in Table 13-6. Each bus_type object includes an embedded subsystem; the subsystem stored in the bus_subsys variable collects all subsystems embedded in the bus_type objects. The bus_subsys subsystem is associated with the /sys/bus directory; thus, for example, there exists a /sys/bus/pci directory associated with the PCI bus type. The per-bus subsystem typically includes only two ksets named drivers and devices (corresponding to the drivers and devices fields of the bus_type object, respectively).

Table 13-6. The fields of the bus_type object

| Type | Field | Description |
| ---- | ----- | ----------- |
| char * | name | Name of the bus type |
| struct subsystem | subsys | Kobject subsystem associated with this bus type
struct kset drivers The set of kobjects of the drivers
struct kset devices The set of kobjects of the devices
struct bus_attribute * bus_attrs Pointer to the object including the bus attributes
and the methods for exporting them to the sysfs
filesystem
struct device_attribute * dev_attrs Pointer to the object including the device attributes
and the methods for exporting them to the sysfs
filesystem
struct driver_attribute * drv_attrs Pointer to the object including the device driver
attributes and the methods for exporting them to
the sysfs filesystem
int (*)(struct device *, struct
device_driver *)
match Method for checking whether a given driver supports
a given device
int (*)(struct device *, char **,
int, char *, int)
hotplug Method invoked when a device is being registered
int (*)(struct device *,
unsigned long)
suspend Method for saving the hardware context state and
changing the power level of a device
int (*)(struct device *) resume Method for changing the power level and restoring
the hardware context of a device

Each bus_type object includes an embedded subsystem; the subsystem stored in the
bus_subsys variable collects all subsystems embedded in the bus_type objects. The
bus_subsys subsystem is associated with the /sys/bus directory; thus, for example,
there exists a /sys/bus/pci directory associated with the PCI bus type. The per-bus
subsystem typically includes only two ksets named drivers and devices (corresponding
to the drivers and devices fields of the bus_type object, respectively).

The drivers kset contains the device_driver descriptors of all device drivers pertaining
to the bus type, while the devices kset contains the device descriptors of all
devices of the given bus type. Because the directories of the devices’ kobjects already
appear in the sysfs filesystem under /sys/devices, the devices directory of the per-bus
subsystem stores symbolic links pointing to directories under /sys/devices. The bus_
for_each_drv() and bus_for_each_dev() functions iterate over the elements of the
lists of drivers and devices, respectively.
The match method is executed when the kernel must check whether a given device
can be handled by a given driver. Even if each device’s identifier has a format specific
to the bus that hosts the device, the function that implements the method is
usually simple, because it searches the device’s identifier in the driver’s table of supported
identifiers. The hotplug method is executed when a device is being registered
in the device driver model; the implementing function should add bus-specific information
to be passed as environment variables to a User Mode program that is notified
about the new available device (see the later section “Device Driver
Registration”). Finally, the suspend and resume methods are executed when a device
on a bus of the given type must change its power state.

#### 4.4 Classes

Each class is described by a class object. All class objects belong to the class_subsys subsystem associated with the `/sys/class` directory. Each class object, moreover, includes an embedded subsystem; thus, for example, there exists a `/sys/class/input` directory associated with the input class of the device driver model. Each class object includes a list of class_device descriptors, each of which represents a single logical device belonging to the class. The class_device structure includes a dev field that points to a device descriptor, thus a logical device always refers to a given device in the device driver model. However, there can be several class_device descriptors that refer to the same device. In fact, a hardware device might include several different sub-devices, each of which requires a different User Mode interface. For example, the sound card is a hardware device that usually includes a DSP, a mixer, a game port interface, and so on; each sub-device requires its own User Mode interface, thus it is associated with its own directory in the sysfs filesystem.

Device drivers in the same class are expected to offer the same functionalities to the User Mode applications; for instance, all device drivers of sound cards should offer a way to write sound samples to the DSP.

The classes of the device driver model are essentially aimed to provide a standard method for exporting to User Mode applications the interfaces of the logical devices. Each class_device descriptor embeds a kobject having an attribute (special file) named dev. Such attribute stores the major and minor numbers of the device file that is needed to access to the corresponding logical device (see the next section).



* [13.1 USB设备基础知识](#13.1)
    - [13.1.1 端点](#13.1.1)
    - [13.1.2 接口](#13.1.2)
    - [13.1.3 配置](#13.1.3)
* [13.2 USB和sysfs](#13.2)
* [13.3 USB的Urbs](#13.3)
    - [13.3.1 结构struct urb](#13.3.1)
    - [13.3.2 创建和销毁urb](#13.3.2)
        + [13.3.2.1 中断urb](#13.3.2.1)
        + [13.3.2.2 块urb](#13.3.2.2)
        + [13.3.2.3 控制urb](#13.3.2.3)
        + [13.3.2.4 同步urb](#13.3.2.4)
    - [13.3.3 提交urb](#13.3.3)
    - [13.3.4 完成urb：完成回调处理函数](#13.3.4)
    - [13.3.5 取消urb](#13.3.5)
* [13.4 编写一个USB驱动](#13.4)
    * [13.4.1 驱动支持什么设备](#13.4.1)
    * [13.4.2 注册一个USB设备](#13.4.2)
        - [13.4.2.1 探测和去连接的细节](#13.4.2.1)
    * [13.4.3 提交和控制一个urb](#13.4.3)
* [13.5 无urb的USB传送](#13.5)
    - [13.5.1 usb_bulk_msg接口](#13.5.1)
    - [13.5.2 usb_control_msg接口](#13.5.2)
    - [13.5.3 使用USB数据函数](#13.5.3)

***

The universal serial bus (USB) is a connection between a host computer and a number of peripheral devices. It was originally created to replace a wide range of slow and different buses—the parallel, serial, and keyboard connections—with a single bus type that all devices could connect to.* USB has grown beyond these slow connections and now supports almost every type of device that can be connected to a PC. The latest revision of the USB specification added high-speed connections with a theoretical speed limit of 480 MBps.

Topologically, a USB subsystem is not laid out as a bus; it is rather a tree built out of several point-to-point links. The links are four-wire cables (ground, power, and two signal wires) that connect a device and a hub, just like twisted-pair Ethernet. The USB host controller is in charge of asking every USB device if it has any data to send. Because of this topology, a USB device can never start sending data without first being asked to by the host controller. This configuration allows for a very easy plug-and-play type of system, whereby devices can be automatically configured by the host computer.

The bus is very simple at the technological level, as it’s a single-master implementation in which the host computer polls the various peripheral devices. Despite this intrinsic limitation, the bus has some interesting features, such as the ability for a device to request a fixed bandwidth for its data transfers in order to reliably support video and audio I/O. Another important feature of USB is that it acts merely as a communication channel between the device and the host, without requiring specific meaning or structure to the data it delivers.

>Actually, some structure is there, but it mostly reduces to a requirement for the communication to fit into one of a few predefined classes: a keyboard won’t allocate bandwidth, for example, while some video cameras will.

The USB protocol specifications define a set of standards that any device of a specific type can follow. If a device follows that standard, then a special driver for that device is not necessary. These different types are called classes and consist of things like storage devices, keyboards, mice, joysticks, network devices, and modems. Other types of devices that do not fit into these classes require a special vendor-specific driver to be written for that specific device. Video devices and USB-to-serial devices are a good example where there is no defined standard, and a driver is needed for every different device from different manufacturers.

These features, together with the inherent hotplug capability of the design, make USB a handy, low-cost mechanism to connect (and disconnect) several devices to the computer without the need to shut the system down, open the cover, and swear over screws and wires.

The Linux kernel supports two main types of USB drivers: drivers on a host system and drivers on a device. The USB drivers for a host system control the USB devices that are plugged into it, from the host’s point of view (a common USB host is a desktop computer.) The USB drivers in a device, control how that single device looks to the host computer as a USB device. As the term “USB device drivers” is very confusing, the USB developers have created the term “USB gadget drivers” to describe the drivers that control a USB device that connects to a computer (remember that Linux also runs in those tiny embedded devices, too.) This chapter details how the USB system that runs on a desktop computer works. USB gadget drivers are outside the realm of this book at this point in time.

As Figure 13-1 shows, USB drivers live between the different kernel subsytems (block, net, char, etc.) and the USB hardware controllers. The USB core provides an interface for USB drivers to use to access and control the USB hardware, without having to worry about the different types of USB hardware controllers that are present on the system.

<h2 id="13.1">13.1 USB设备基础知识</h2>

USB设备是一个复杂的事物，详细描述可以参考[官方USB文档](http://www.usb.org)。幸运的是，Linux内核提供了一个子系统-USB核-负责处理这些复杂的事情。本章主要描述驱动和USB核之间的交互。图13-2展示了USB设备的组成，配置，接口和端点以及USB驱动是如何绑定到USB接口，而不是整个USB设备上。

![Figure 13-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/13-1.PNG)
![Figure 13-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/13-2.PNG)

<h3 id="13.1.1">13.1.1 端点</h3>

USB通信最基本的形式就是端点。USB端点只能在一个方向上传输数据，比如，从主机到设备（称为OUT端点）或从设备到主机（称为IN端点）。端点可以被认为是单向管道。

USB端点有4种类型：

* CONTROL

    CONTROL 端点被用于允许访问USB设备的不同部分。被用来配置设备，检索设备信息，向设备发送命令，或检索设备的状态报告。这些端点通常很小。每个USB设备都有一个称为“端点0”的端点，USB核用来在设备插入时，配置设备使用。这些数据的传输由USB协议保证，保留足够的带宽，以使数据穿过。

* INTERRUPT

    每次USB主机向设备请求数据时，INTERRUPT端点以固定速率传输少量数据。这些端点是USB键盘和鼠标的主要传输方式。常常也会被用来向USB设备发送数据，以便控制设备，但是通常不会传输大量数据。传输由USB协议保证足够的带宽。

* BULK

    BULK端点传输大量数据。传输数据量大于INTERRUPT端点（每次可以携带更多字符）。常常用于保证数据传输没有损失的设备中。USB协议无法保证在特定的时间段内完成。如果总线上没有足够的空间发送整个BULK数据包，它会被分成多个发送到总线上。常常用于打印机，存储和网络设备。

* ISOCHRONOUS

    ISOCHRONOUS端点也可以传输大量数据，但是不保证数据完成传输。这些端点用于能够处理数据丢失，依赖于保持恒定的数据流的设备。实时数据收集（例如音频和视频设备）总是使用这种端点。

CONTROL 和 BULK 端点用于异步数据传输。INTERRUPT 和 ISOCHRONOUS是周期性的。其带宽是由USB核保留。

内核中使用 `struct usb_host_endpoint`描述USB端点。该结构体还包含另一个结构体`struct usb_endpoint_descriptor`，其保存着真实的端点信息。后一个结构体包含USB具体的数据，数据格式是由设备本身指定的。该结构体的主要成员描述如下：

* bEndpointAddress

    这是此端点的USB地址。此8位数值中还包括端点的方向。使用位掩码 `USB_DIR_OUT` 和 `USB_DIR_IN`决定该端点是指向设备的还是主机的。

* bmAttributes

    端点类型。 使用位掩码`USB_ENDPOINT_XFERTYPE_MASK` 决定端点的类型是 `USB_ENDPOINT_XFER_ISOC`, `USB_ENDPOINT_XFER_BULK`, 或 `USB_ENDPOINT_XFER_INT`。

* wMaxPacketSize

    该端点一次可以处理的最大数据，以字节为单位。需要注意的是，驱动程序有可能发送大于该值的数据，但是实际发送给设备时，数据将会被按`wMaxPacketSize`为大小分成块。对于高速设备，使用该成员的高位支持端点的高带宽模式。具体内容可以查看USB规约，是如何实现的。

* bInterval

    端点类型为INTERRUPT类型时，为间隔设置。单位为mS。

该结构体的成员变量命名并不是遵循Linux内核的常用命名方法。这是因为这些变量都与USB规约有直接关系。所以，USB内核编程者觉得使用指定的名称更为重要，而不是Linux开发者更为熟悉的变量名称。

<h3 id="13.1.2">13.1.2 接口</h3>

把USB端点捆绑起来就称之为`接口`. USB接口只能处理一种类型的逻辑连接，比如鼠标，键盘或音频流。有一些USB设备有多个接口，比如USB扬声器就有2个接口：用于按键的USB键盘类型接口和USB音频流类型接口。因为USB接口代表一种基本的功能，所以每一个USB驱动控制一个接口；那么，扬声器示例中，Linux就需要两个不同的驱动。

USB接口可能具有备用设置，这些设置是接口参数的不同选择。一个接口的初始状态就是第一个设置，编号为0。备用设置可以被用于以不同方式控制各个端点，比如为设备保留不同数量的USB带宽。使用isochronous端点的设备使用相同接口的备用设置。

在内核中使用结构体 `struct usb_interface`描述USB接口。这个结构体是USB核传递给USB驱动，然后USB驱动负责控制。其中重要的成员有：

* struct usb_host_interface *altsetting

    接口结构体数组，包含可供该接口选择的所有备选设置。每一个`struct usb_host_interface`包含由`struct usb_host_endpoint`结构体定义的端点集合。注意的是，这些接口结构没有特定的顺序。

* unsigned num_altsetting

    备选设置的数量。

* struct usb_host_interface *cur_altsetting

    指向数组`altsetting`的指针, 表示当前有效的设置。

* int minor

    USB驱动使用major主设备号绑定此接口，使用该变量指定minor设备号，其由USB核指定给接口。 只有成功调用函数 `usb_register_dev` (在本章的后面会描述)后才会有效。

结构体`usb_interface`还有一些其它成员，但是USB驱动不需要关注。

<h3 id="13.1.3">13.1.3 配置</h3>

将USB接口捆绑起来就是配置。USB设备能够拥有多个配置，可以在它们之间切换以改变设备的状态。例如，那些允许固件被下载的设备就包含多个配置来实现。在某一个时间点上只能有一个配置被使能。Linux不处理多种配置的USB设备，但是，幸运的是，它们极少。

Linux使用结构体`struct usb_host_config`描述USB配置，使用`struct usb_device`描述整个USB设备。USB设备通常不需要读写这些结构体中的数值，所以，在这里没有详细讨论。好奇的读者可以通过源代码树中的文件`include/linux/usb.h`进行查阅。

USB设备驱动通常需要将给定的结构类型为`struct usb_interface`的数据结构数据转化为结构为`struct usb_device`的数据，以满足多种函数调用的需求。Linux内核提供了这个函数接口，`interface_to_usbdev`。但愿在未来，这种接口函数不再被需要。

总而言之，USB设备相当复杂，由不同的逻辑单元组成。这些逻辑单元的关系简单描述如下：

* 设备可以拥有一个或多个配置

* 配置通常可以拥有一个或多个接口

* 接口通常拥有一个或多个设置

* 接口拥有`0`个或更多端点

<h2 id="13.2">13.2 USB和sysfs</h2>

Due to the complexity of a single USB physical device, the representation of that device in `sysfs` is also quite complex. Both the physical USB device (as represented by a struct usb_device) and the individual USB interfaces (as represented by a struct usb_interface) are shown in `sysfs` as individual devices. (This is because both of those structures contain a struct `device` structure.) As an example, for a simple USB mouse that contains only one USB interface, the following would be the `sysfs` directory tree for that device:

The struct `usb_device` is represented in the tree at:

    /sys/devices/pci0000:00/0000:00:09.0/usb2/2-1

while the USB interface for the mouse—the interface that the USB mouse driver is bound to—is located at the directory:

    /sys/devices/pci0000:00/0000:00:09.0/usb2/2-1/2-1:1.0

To help understand what this long device path means, we describe how the kernel labels the USB devices.

The first USB device is a root hub. This is the USB controller, usually contained in a PCI device. The controller is so named because it controls the whole USB bus connected to it. The controller is a bridge between the PCI bus and the USB bus, as well as being the first USB device on that bus.

All root hubs are assigned a unique number by the USB core. In our example, the root hub is called usb2, as it is the second root hub that was registered with the USB core. There is no limit on the number of root hubs that can be contained in a single system at any time.

Every device that is on a USB bus takes the number of the root hub as the first number in its name. That is followed by a `-` character and then the number of the port that the device is plugged into. As the device in our example is plugged into the first port, a `1` is added to the name. So the device name for the main USB mouse device is 2-1. Because this USB device contains one interface, that causes another device in the tree to be added to the sysfs path. The naming scheme for USB interfaces is the device name up to this point: in our example, it’s 2-1 followed by a colon and the USB configuration number, then a period and the interface number. So for this example, the device name is 2-1:1.0 because it is the first configuration and has interface number zero.

So to summarize, the USB sysfs device naming scheme is:

    root_hub-hub_port: config. interface

As the devices go further down in the USB tree, and as more and more USB hubs are used, the hub port number is added to the string following the previous hub port number in the chain. For a two-deep tree, the device name looks like:

    root_hub- hub_port- hub_port: config. interface

As can be seen in the previous directory listing of the USB device and interface, all of the USB specific information is available directly through sysfs (for example, the idVendor, idProduct, and bMaxPower information). One of these files, bConfigurationValue, can be written to in order to change the active USB configuration that is being used. This is useful for devices that have multiple configurations, when the kernel is unable to determine what configuration to select in order to properly operate the device. A number of USB modems need to have the proper configuration value written to this file in order to have the correct USB driver bind to the device.

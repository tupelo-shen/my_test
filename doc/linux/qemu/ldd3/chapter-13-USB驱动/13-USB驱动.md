* [13.1 USB设备基础知识](#13.1)
    - [13.1.1 端点](#13.1.1)
    - [13.1.2 接口](#13.1.2)
    - [13.1.3 配置](#13.1.3)
* [13.2 USB和sysfs](#13.2)
* [13.3 USB的Urbs](#13.3)
    - [13.3.1 结构体struct urb](#13.3.1)
    - [13.3.2 创建和销毁urb](#13.3.2)
        + [13.3.2.1 中断urb](#13.3.2.1)
        + [13.3.2.2 块urb](#13.3.2.2)
        + [13.3.2.3 控制urb](#13.3.2.3)
        + [13.3.2.4 等时urb](#13.3.2.4)
    - [13.3.3 提交urb](#13.3.3)
    - [13.3.4 完成urb时调用的回调函数](#13.3.4)
    - [13.3.5 取消urb](#13.3.5)
* [13.4 编写一个USB驱动](#13.4)
    * [13.4.1 驱动支持什么设备](#13.4.1)
    * [13.4.2 注册一个USB设备](#13.4.2)
        - [13.4.2.1 probe和disconnect的细节](#13.4.2.1)
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

USB设备在`sysfs`里的表示也比较复杂。物理USB设备（由结构体'struct usb_device'表示）和各个USB接口（由结构体 `struct usb_interface`表示）分别表示为单独的设备。这是因为2中结构体中都包含结构体`device`。例如，只包含一个USB接口的简单的USB鼠标，一下是该设备的'sysfs'目录树：

结构体`usb_device`在目录树中位置：

    /sys/devices/pci0000:00/0000:00:09.0/usb2/2-1

鼠标的USB接口-USB鼠标驱动绑定的接口-位于目录中的位置：

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

`Sysfs`只显示到接口层级。设备的备选配置和该接口有关的端点详细信息不显示，这些信息可以在`usbfs`文件系统里找到，其安装在'/proc/bus/usb/'目录下。`/proc/bus/usb/devices`显示系统中存在的所有USB设备的信息，包含备选配置和端点信息。`usbfs`允许用户空间的程序直接访问USB设备，这样许多内核驱动程序就可以被移动到用户空间，方便维护和调试。USB扫描仪驱动程序就是一个很好的例子，它的功能包含在用户空间的SANE库中，所以不再存在于内核中。

<h2 id="13.3">13.3 USB的Urbs</h2>

The USB code in the Linux kernel communicates with all USB devices using something called a urb (USB request block). This request block is described with the struct `urb` structure and can be found in the `include/linux/usb.h` file.

A urb is used to send or receive data to or from a specific USB endpoint on a specific USB device in an asynchronous manner. It is used much like a `kiocb` structure is used in the filesystem async I/O code or as a struct `skbuff` is used in the networking code. A USB device driver may allocate many urbs for a single endpoint or may reuse a single urb for many different endpoints, depending on the need of the driver. Every endpoint in a device can handle a queue of urbs, so that multiple urbs can be sent to the same endpoint before the queue is empty. The typical lifecycle of a urb is as follows:

1. 由USB设备驱动创建；
2. 为具体的USB设备指定一个具体的端点；
3. 由USB设备驱动提交给USB核；
4. 由USB核提交给具体设备对应的USB主控制器的驱动程序；
5. 由USB主控制器驱动程序处理，并发送给设备；
6. 当urb完成时，由USB主控制器驱动程序通知USB设备驱动程序。

Urbs can also be canceled any time by the driver that submitted the urb, or by the USB core if the device is removed from the system. urbs are dynamically created and contain an internal reference count that enables them to be automatically freed when the last user of the urb releases it.

The procedure described in this chapter for handling urbs is useful, because it permits streaming and other complex, overlapping communications that allow drivers to achieve the highest possible data transfer speeds. But less cumbersome procedures are available if you just want to send individual bulk or control messages and do not care about data throughput rates. (查看 [无urb的USB传送](#13.5))

<h3 id="13.3.1">13.3.1 结构体struct urb</h3>

对USB设备驱动很重要的`struct urb`的成员变量：

* struct usb_device *dev
* unsigned int pipe
* unsigned int usb_sndctrlpipe(struct usb_device *dev, unsigned int endpoint)
* unsigned int transfer_flags
* void *transfer_buffer
* dma_addr_t transfer_dma
* int transfer_buffer_length
* unsigned char *setup_packet
* dma_addr_t setup_dma
* usb_complete_t complete
* void *context
* int actual_length
* int status

    When the urb is finished, or being processed by the USB core, this variable is set to the current status of the urb. The only time a USB driver can safely access this variable is in the urb completion handler function (described in the section “[完成urb时调用的回调函数](#13.3.4)”). This restriction is to prevent race conditions that occur while the urb is being processed by the USB core. For isochronous urbs, a successful value (0) in this variable merely indicates whether the urb has been unlinked. To obtain a detailed status on isochronous urbs, the `iso_frame_desc` variables should be checked.

    合法数值包括：

    * 0

        `urb`发送成功

    * -ENOENT

        The urb was stopped by a call to usb_kill_urb.

    * -ECONNRESET

        The urb was unlinked by a call to usb_unlink_urb, and the transfer_flags variable of the urb was set to URB_ASYNC_UNLINK.

    * -EINPROGRESS

        The urb is still beingprocessed by the USB host controllers. If your driver ever sees this value, it is a bug in your driver.

    * -EPROTO

        One of the following errors occurred with this urb:

        * A bitstuff error happened during the transfer.
        * No response packet was received in time by the hardware.



    * -EILSEQ

        There was a CRC mismatch in the urb transfer.

    * -EPIPE

        The endpoint is now stalled. If the endpoint involved is not a control endpoint, this error can be cleared through a call to the function `usb_clear_halt`.

    * -ECOMM

        Data was received faster duringthe transfer than it could be written to system memory. This error value happens only for an IN urb.

    * -ENOSR

        Data could not be retrieved from the system memory duringthe transfer fast enough to keep up with the requested USB data rate. This error value happens only for an OUT urb.

    * -EOVERFLOW

        A “babble” error happened to the urb. A “babble” error occurs when the endpoint receives more data than the endpoint’s specified maximum packet size.

    * -EREMOTEIO

        Occurs only if the URB_SHORT_NOT_OK flagis set in the urb’s `transfer_flags` variable and means that the full amount of data requested by the urb was not received.

    * -ENODEV

        The USB device is now gone from the system.

    * -EXDEV

        Occurs only for a isochronous urb and means that the transfer was only partially completed. In order to determine what was transferred, the driver must look at the individual frame status.

    * -EINVAL

        Something very bad happened with the urb. The USB kernel documentation describes what this value means:

            ISO madness, if this happens: Log off and go home

        It also can happen if a parameter is incorrectly set in the urb stucture or if an incorrect function parameter in the usb_submit_urb call submitted the urb to the USB core.

    * -ESHUTDOWN

        There was a severe error with the USB host controller driver; it has now been disabled, or the device was disconnected from the system, and the urb was submitted after the device was removed. It can also occur if the configuration was changed for the device, while the urb was submitted to the device.

    Generally, the error values -EPROTO, -EILSEQ, and -EOVERFLOW indicate hardware problems with the device, the device firmware, or the cable connectingthe device to the computer.

* int start_frame
* int interval
* int number_of_packets
* int error_count
* struct usb_iso_packet_descriptor iso_frame_desc[0]

<h3 id="13.3.2">13.3.2 创建和销毁urb</h3>

结构体`urb`只能动态创建，因为静态创建会破坏USB核为urb使用的引用计数方法。使用下面的函数创建，原型如下：

    struct urb *usb_alloc_urb(int iso_packets, int mem_flags);

The first parameter, iso_packets, is the number of isochronous packets this urb should contain. If you do not want to create an isochronous urb, this variable should be set to 0. The second parameter, mem_flags, is the same type of flag that is passed to the kmalloc function call to allocate memory from the kernel (see the section “The Flags Argument” in Chapter 8 for the details on these flags). If the function is successful in allocating enough space for the urb, a pointer to the urb is returned to the caller. If the return value is NULL, some error occurred within the USB core, and the driver needs to clean up properly.

After a urb has been created, it must be properly initialized before it can be used by the USB core. See the next sections for how to initialize different types of urbs.

In order to tell the USB core that the driver is finished with the urb, the driver must call the usb_free_urb function. This function only has one argument:

    void usb_free_urb(struct urb *urb);

The argument is a pointer to the struct `urb` you want to release. After this function is called, the urb structure is gone, and the driver cannot access it any more.

<h3 id="13.3.2.1">13.3.2.1 中断urb</h3>

<h3 id="13.3.2.2">13.3.2.2 块urb</h3>

<h3 id="13.3.2.3">13.3.2.3 控制urb</h3>

<h3 id="13.3.2.4">13.3.2.4 等时urb</h3>

Isochronous urbs unfortunately do not have an initializer function like the interrupt, control, and bulk urbs do. So they must be initialized “by hand” in the driver before they can be submitted to the USB core. The following is an example of how to properly initialize this type of urb. It was taken from the `konicawc.c` kernel driver located in the `drivers/usb/media` directory in the main kernel source tree.

    urb->dev = dev;
    urb->context = uvd;
    urb->pipe = usb_rcvisocpipe(dev, uvd->video_endp-1);
    urb->interval = 1;
    urb->transfer_flags = URB_ISO_ASAP;
    urb->transfer_buffer = cam->sts_buf[i];
    urb->complete = konicawc_isoc_irq;
    urb->number_of_packets = FRAMES_PER_DESC;
    urb->transfer_buffer_length = FRAMES_PER_DESC;
    for (j=0; j < FRAMES_PER_DESC; j++)
    {
        urb->iso_frame_desc[j].offset = j;
        urb->iso_frame_desc[j].length = 1;
    }


<h3 id="13.3.3">13.3.3 提交urb</h3>

Once the urb has been properly created and initialized by the USB driver, it is ready to be submitted to the USB core to be sent out to the USB device. This is done with a call to the function `usb_submit_urb`:

    int usb_submit_urb(struct urb *urb, int mem_flags);

The `urb` parameter is a pointer to the urb that is to be sent to the device. The `mem_flags` parameter is equivalent to the same parameter that is passed to the `kmalloc` call and is used to tell the USB core how to allocate any memory buffers at this moment in time.

After a urb has been submitted to the USB core successfully, it should never try to access any fields of the urb structure until the `complete` function is called.

Because the function `usb_submit_urb` can be called at any time (including from within an interrupt context), the specification of the `mem_flags` variable must be correct. There are really only three valid values that should be used, depending on when `usb_submit_urb` is being called:

* GFP_ATOMIC

    This value should be used whenever the following are true:

    * The caller is within a urb completion handler, an interrupt, a bottom half, a tasklet, or a timer callback.
    * The caller is holdinga spinlock or rwlock. Note that if a semaphore is being held, this value is not necessary.
    * The `current->state` is not TASK_RUNNING. The state is always TASK_RUNNING unless the driver has changed the current state itself.

* GFP_NOIO

    This value should be used if the driver is in the block I/O patch. It should also be used in the error handling path of all storage-type devices.

* GFP_KERNEL

    This should be used for all other situations that do not fall into one of the previously mentioned categories.

<h3 id="13.3.4">13.3.4 完成urb时调用的回调函数</h3>

If the call to `usb_submit_urb` was successful, transferring control of the urb to the USB core, the function returns 0; otherwise, a negative error number is returned. If the function succeeds, the completion handler of the urb (as specified by the `complete` function pointer) is called exactly once when the urb is completed. When this function is called, the USB core is finished with the URB, and control of it is now returned to the device driver.

urb只有三种方式可以完成，然后调用`complete`函数：

* The urb is successfully sent to the device, and the device returns the proper acknowledgment. For an OUT urb, the data was successfully sent, and for an IN urb, the requested data was successfully received. If this has happened, the status variable in the urb is set to 0.

* Some kind of error happened when sendingor receivingdata from the device. This is noted by the error value in the status variable in the urb structure.

* The urb was “unlinked” from the USB core. This happens either when the driver tells the USB core to cancel a submitted urb with a call to `usb_unlink_urb` or `usb_kill_urb`, or when a device is removed from the system and a urb had been submitted to it.

An example of how to test for the different return values within a urb completion call is shown later in this chapter.

<h3 id="13.3.5">13.3.5 取消urb</h3>

To stop a urb that has been submitted to the USB core, the functions `usb_kill_urb` or `usb_unlink_urb` should be called:

    int usb_kill_urb(struct urb *urb);
    int usb_unlink_urb(struct urb *urb);

参数urb指向要取消的urb。

When the function is `usb_kill_urb`, the urb lifecycle is stopped. This function is usually used when the device is disconnected from the system, in the disconnect callback.

For some drivers, the usb_unlink_urb function should be used to tell the USB core to stop an urb. This function does not wait for the urb to be fully stopped before returning to the caller. This is useful for stopping the urb while in an interrupt handler or when a spinlock is held, as waitingfor a urb to fully stop requires the ability for the USB core to put the calling process to sleep. This function requires that the URB_ASYNC_UNLINK flagvalue be set in the urb that is being asked to be stopped in order to work properly.

<h2 id="13.4">13.4 编写USB驱动</h2>

The approach to writinga USB device driver is similar to a pci_driver: the driver registers its driver object with the USB subsystem and later uses vendor and device identifiers to tell if its hardware has been installed.

<h3 id="13.4.1">13.4.1 驱动支持设备</h3>

结构体`usb_device_id`提供了该驱动支持的不同USB设备类型的列表。这个列表决定了当一个具体的设备插入了系统中时，USB核把设备给予哪种驱动，通过`hotplug`自动载入哪种驱动。

结构体`usb_device_id`的成员变量定义如下：

* __u16 match_flags

    Determines which of the followingfields in the structure the device should be matched against. This is a bit field defined by the different USB_DEVICE_ID_MATCH_* values specified in the include/linux/mod_devicetable.h file. This field is usually never set directly but is initialized by the USB_DEVICE type macros described later.

* __u16 idVendor

    The USB vendor ID for the device. This number is assigned by the USB forum to its members and cannot be made up by anyone else.

* __u16 idProduct

    The USB product ID for the device. All vendors that have a vendor ID assigned to them can manage their product IDs however they choose to.

* __u16 bcdDevice_lo
* __u16 bcdDevice_hi

    Define the low and high ends of the range of the vendor-assigned product version number. The bcdDevice_hi value is inclusive; its value is the number of the highest-numbered device. Both of these values are expressed in binary-coded decimal (BCD) form. These variables, combined with the idVendor and idProduct, are used to define a specific version of a device.

* __u8 bDeviceClass
* __u8 bDeviceSubClass
* __u8 bDeviceProtocol

    Define the class, subclass, and protocol of the device, respectively. These numbers are assigned by the USB forum and are defined in the USB specification. These values specify the behavior for the whole device, includingall interfaces on this device.

* __u8 bInterfaceClass
* __u8 bInterfaceSubClass
* __u8 bInterfaceProtocol

    Much like the device-specific values above, these define the class, subclass, and protocol of the individual interface, respectively. These numbers are assigned by the USB forum and are defined in the USB specification.

* kernel_ulong_t driver_info

    This value is not used to match against, but it holds information that the driver can use to differentiate the different devices from each other in the probe callback function to the USB driver.

As with PCI devices, there are a number of macros that are used to initialize this structure:

* USB_DEVICE(vendor, product)

    Creates a struct usb_device_id that can be used to match only the specified vendor and product ID values. This is very commonly used for USB devices that need a specific driver.

* USB_DEVICE_VER(vendor, product, lo, hi)

    Creates a struct usb_device_id that can be used to match only the specified vendor and product ID values within a version range.

* USB_DEVICE_INFO(class, subclass, protocol)

    Creates a struct usb_device_id that can be used to match a specific class of USB devices.

* USB_INTERFACE_INFO(class, subclass, protocol)

    Creates a struct usb_device_id that can be used to match a specific class of USB interfaces.

对于仅控制一个来自单个厂商的USB设备的驱动程序来说，`struct usb_device_id`表 应该定义如下：

    /* 驱动支持的设备表 */
    static struct usb_device_id skel_table [ ] = {
        { USB_DEVICE(USB_SKEL_VENDOR_ID, USB_SKEL_PRODUCT_ID) },
        { } /* 结束项 */
    };
    MODULE_DEVICE_TABLE (usb, skel_table);

同PCI驱动一样，`MODULE_DEVICE_TABLE`宏是必须的，因为要允许用户空间的程序能够确定该驱动可以控制的设备。对于USB驱动，第一个参数必须是`usb`字符串。

<h3 id="13.4.2">13.4.2 注册一个USB设备</h3>

The main structure that all USB drivers must create is a struct usb_driver. This structure must be filled out by the USB driver and consists of a number of function callbacks and variables that describe the USB driver to the USB core code:

所有的USB驱动都必须注册的主要结构体是 `struct usb_driver`。 包含许多回调函数和变量，由USB驱动程序完成实现。

* struct module *owner

    指定驱动模块的所有者。USB核使用它正确地对该驱动引用计数，以便不会在不恰当的时刻卸载掉驱动程序。使用宏`THIS_MODULE`进行设置。

* const char *name

    驱动的名称。内核中所有的USB驱动名称必须是唯一的，常常设置为和模块名称相同。在`sysfs`和`/sys/bus/usb/drivers/`中能够看到。

* const struct usb_device_id *id_table

    设备支持列表。如果没设，USB驱动程序中的`probe`回调函数不会被调用。如果你想你的驱动总是被系统中的每一个USB设备调用，创建一个只设置`driver_info`字段的项:

        static struct usb_device_id usb_ids[ ] = {
            {.driver_info = 42},
            { }
        };

* int (*probe) (struct usb_interface *intf, const struct usb_device_id *id)

    'probe'函数。详细描述可以参考[probe and disconnect in Detail](#13.4.2.1)。当USB核认为它有该驱动程序处理的结构体`struct usb_interface`时，就会调用该函数。USB核作出决定而使用的结构体`struct usb_device_id`也会被传递给该函数，在这里，就是`id`。`intf`是传递给驱动程序的接口，类型是`struct usb_interface`，如果USB驱动也声明了这个接口，该函数就会正确初始化设备，并且返回0.如果驱动程序不想声明设备，或发生错误，则返回负值作为错误码。

* void (*disconnect) (struct usb_interface *intf)

    设备驱动中的`disconnect`函数。当结构体`struct usb_interface`被从系统中移除或当驱动被从USB核中卸载的时候，USB核调用该函数。详细描述可以参考[probe and disconnect in Detail](#13.4.2.1)。

所以，只需初始化5个成员就可以创建一个最简单的类型为`struct usb_driver`的值：

        static struct usb_driver skel_driver = {
            .owner = THIS_MODULE,
            .name = "skeleton",
            .id_table = skel_table,
            .probe = skel_probe,
            .disconnect = skel_disconnect,
        };

结构体`struct usb_driver`还包含一些回调函数，它们并不常用，为了USB驱动能够正常工作也不要求添加：

* int (*ioctl) (struct usb_interface *intf, unsigned int code, void *buf)

    USB驱动中的控制函数。 事实上只有`USB hub`需要使用它，其它USB驱动没有这个需求。

* int (*suspend) (struct usb_interface *intf, u32 state)

    挂起函数。USB核挂起该设备时调用该函数。

* int (*resume) (struct usb_interface *intf)

    恢复函数。当USB核恢复设备时调用它。

使用USB核注册结构体`struct usb_driver`，调用函数`usb_register_driver`,参数就是类型为`struct usb_driver`的结构体指针。 下面是USB设备典型的初始化代码：

    static int __init usb_skel_init(void)
    {
        int result;

        /* 使用USB核注册驱动 */
        result = usb_register(&skel_driver);
        if (result)
            err("usb_register failed. Error number %d", result);

        return result;
    }

卸载USB驱动时，解除结构体`struct usb_driver`的注册。方法就是调用`usb_deregister_driver`函数，任何绑定到该驱动的USB接口都会被失连，并且`disconnect`函数会被调用。

    static void __exit usb_skel_exit(void)
    {
        /* 解除USB驱动的注册 */
        usb_deregister(&skel_driver);
    }

<h4 id="13.4.2.1">13.4.2.1 probe和disconnect的细节</h4>

当设备被安装，USB核认为驱动和设备对应时，就会调用`probe`函数；`probe`函数检查传递给它的设备信息，判断驱动程序是否和设备真正匹配。在驱动卸载的时候，调用`disconnect`函数，负责清除工作。


`probe`和`disconnect`函数在USB hub内核线程的上下文中调用，所以，休眠是合法的。但是，
 function callbacks are called in the context of the USB hub kernel thread, so it is legal to sleep within them. However, it is recommended that the majority of work be done when the device is opened by a user if possible, in order to keep the USB probing time to a minimum. This is because the USB core handles the addition and removal of USB devices within a single thread, so any slow device driver can cause the USB device detection time to slow down and become noticeable by the user.

In the probe function callback, the USB driver should initialize any local structures that it might use to manage the USB device. It should also save any information that it needs about the device to the local structure, as it is usually easier to do so at this time. As an example, USB drivers usually want to detect what the endpoint address and buffer sizes are for the device, as they are needed in order to communicate with the device. Here is some example code that detects both IN and OUT endpoints of BULK type and saves some information about them in a local device structure:

    /* 设置端点信息 */
    /* use only the first bulk-in and bulk-out endpoints */
    iface_desc = interface->cur_altsetting;
    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
    {
        endpoint = &iface_desc->endpoint[i].desc;

        if (!dev->bulk_in_endpointAddr &&
            (endpoint->bEndpointAddress & USB_DIR_IN) &&
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK))
        {
            /* we found a bulk in endpoint */
            buffer_size = endpoint->wMaxPacketSize;
            dev->bulk_in_size = buffer_size;
            dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
            dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
            if (!dev->bulk_in_buffer)
            {
                err("Could not allocate bulk_in_buffer");
                goto error;
            }
        }

        if (!dev->bulk_out_endpointAddr &&
            !(endpoint->bEndpointAddress & USB_DIR_IN) &&
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK))
        {
            /* we found a bulk out endpoint */
            dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
        }
    }

    if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr))
    {
        err("Could not find both bulk-in and bulk-out endpoints");
        goto error;
    }

This block of code first loops over every endpoint that is present in this interface and assigns a local pointer to the endpoint structure to make it easier to access later:

    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;

Then, after we have an endpoint, and we have not found a bulk IN type endpoint already, we look to see if this endpoint’s direction is IN. That can be tested by seeing whether the bitmask USB_DIR_IN is contained in the bEndpointAddress endpoint variable. If this is true, we determine whether the endpoint type is bulk or not, by first masking off the `bmAttributes` variable with the `USB_ENDPOINT_XFERTYPE_MASK` bitmask, and then checking if it matches the value `USB_ENDPOINT_XFER_BULK`:

    if (!dev->bulk_in_endpointAddr &&
        (endpoint->bEndpointAddress & USB_DIR_IN) &&
        ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK))

If all of these tests are true, the driver knows it found the proper type of endpoint and can save the information about the endpoint that it will later need to communicate over it in a local structure:

    /* we found a bulk in endpoint */
    buffer_size = endpoint->wMaxPacketSize;
    dev->bulk_in_size = buffer_size;
    dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
    dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
    if (!dev->bulk_in_buffer)
    {
        err("Could not allocate bulk_in_buffer");
        goto error;
    }

Because the USB driver needs to retrieve the local data structure that is associated with this struct `usb_interface` later in the lifecycle of the device, the function `usb_set_intfdata` can be called:

    /* save our data pointer in this interface device *
    usb_set_intfdata(interface, dev);

This function accepts a pointer to any data type and saves it in the struct `usb_interface` structure for later access. To retrieve the data, the function `usb_get_intfdata` should be called:

    struct usb_skel *dev;
    struct usb_interface *interface;
    int subminor;
    int retval = 0;

    subminor = iminor(inode);

    interface = usb_find_interface(&skel_driver, subminor);
    if (!interface)
    {
        err ("%s - error, can't find device for minor %d", __FUNCTION__, subminor);
        retval = -ENODEV;
        goto exit;
    }

    dev = usb_get_intfdata(interface);
    if (!dev)
    {
        retval = -ENODEV;
        goto exit;
    }

usb_get_intfdata is usually called in the open function of the USB driver and again in the disconnect function. Thanks to these two functions, USB drivers do not need to keep a static array of pointers that store the individual device structures for all current devices in the system. The indirect reference to device information allows an unlimited number of devices to be supported by any USB driver.

If the USB driver is not associated with another type of subsystem that handles the user interaction with the device (such as input, tty, video, etc.), the driver can use the USB major number in order to use the traditional char driver interface with user space. To do this, the USB driver must call the usb_register_dev function in the probe function when it wants to register a device with the USB core. Make sure that the device and driver are in a proper state to handle a user wantingto access the device as soon as this function is called.

    /* we can register the device now, as it is ready */
    retval = usb_register_dev(interface, &skel_class);
    if (retval) {
        /* something prevented us from registering this driver */
        err("Not able to get a minor for this device.");
        usb_set_intfdata(interface, NULL);
        goto error;
    }

The usb_register_dev function requires a pointer to a struct usb_interface and a pointer to a struct usb_class_driver. This struct usb_class_driver is used to define a number of different parameters that the USB driver wants the USB core to know when registering for a minor number. This structure consists of the following variables:

* char *name

    The name that sysfs uses to describe the device. A leadingpathname, if present, is used only in devfs and is not covered in this book. If the number of the device needs to be in the name, the characters %d should be in the name string. For example, to create the devfs name usb/foo1 and the sysfs class name foo1, the name string should be set to usb/foo%d.

* struct file_operations *fops;

    Pointer to the struct file_operations that this driver has defined to use to register as the character device. See Chapter 3 for more information about this structure.

* mode_t mode;

    The mode for the devfs file to be created for this driver; unused otherwise. A typical settingfor this variable would be the value S_IRUSR combined with the value S_IWUSR, which would provide only read and write access by the owner of the device file.

* int minor_base;

    This is the start of the assigned minor range for this driver. All devices associated with this driver are created with unique, increasingminor numbers beginningwith this value. Only 16 devices are allowed to be associated with this driver at any one time unless the CONFIG_USB_DYNAMIC_MINORS configuration option has been enabled for the kernel. If so, this variable is ignored, and all minor numbers for the device are allocated on a first-come, first-served manner. It is recommended that systems that have enabled this option use a program such as udev to manage the device nodes in the system, as a static /dev tree will not work properly.

When the USB device is disconnected, all resources associated with the device should be cleaned up, if possible. At this time, if `usb_register_dev` has been called to allocate a minor number for this USB device duringthe probe function, the function `usb_deregister_dev` must be called to give the minor number back to the USB core.

In the `disconnect` function, it is also important to retrieve from the interface any data that was previously set with a call to `usb_set_intfdata`. Then set the data pointer in the `struct usb_interface` structure to NULL to prevent any further mistakes in accessing the data improperly:

    static void skel_disconnect(struct usb_interface *interface)
    {
        struct usb_skel *dev;
        int minor = interface->minor;

        /* prevent skel_open( ) from racing skel_disconnect( ) */
        lock_kernel( );

        dev = usb_get_intfdata(interface);
        usb_set_intfdata(interface, NULL);

        /* give back our minor */
        usb_deregister_dev(interface, &skel_class);

        unlock_kernel( );

        /* decrement our usage count */
        kref_put(&dev->kref, skel_delete);
        info("USB Skeleton #%d now disconnected", minor);
    }

Note the call to lock_kernel in the previous code snippet. This takes the big kernel lock, so that the disconnect callback does not encounter a race condition with the open call when tryingto get a pointer to the correct interface data structure. Because the open is called with the bigkernel lock taken, if the disconnect also takes that same lock, only one portion of the driver can access and then set the interface data pointer.

Just before the disconnect function is called for a USB device, all urbs that are currently in transmission for the device are canceled by the USB core, so the driver does not have to explicitly call usb_kill_urb for these urbs. If a driver tries to submit a urb to a USB device after it has been disconnected with a call to usb_submit_urb, the submission will fail with an error value of -EPIPE.

<h3 id="13.4.3">13.4.3 提交和控制一个urb</h3>

When the driver has data to send to the USB device (as typically happens in a driver’s write function), a urb must be allocated for transmitting the data to the device:

    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb) {
        retval = -ENOMEM;
        goto error;
    }

After the urb is allocated successfully, a DMA buffer should also be created to send the data to the device in the most efficient manner, and the data that is passed to the driver should be copied into that buffer:

    buf = usb_buffer_alloc(dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
    if (!buf) {
        retval = -ENOMEM;
        goto error;
    }
    if (copy_from_user(buf, user_buffer, count)) {
        retval = -EFAULT;
        goto error;
    }

Once the data is properly copied from the user space into the local buffer, the urb must be initialized correctly before it can be submitted to the USB core:

    /* initialize the urb properly */
    usb_fill_bulk_urb(urb, dev->udev,
    usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                    buf, count, skel_write_bulk_callback, dev);
    urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

Now that the urb is properly allocated, the data is properly copied, and the urb is properly initialized, it can be submitted to the USB core to be transmitted to the device:

    /* send the data out the bulk port */
    retval = usb_submit_urb(urb, GFP_KERNEL);
    if (retval) {
        err("%s - failed submitting write urb, error %d", __FUNCTION__, retval);
        goto error;
    }

After the urb is successfully transmitted to the USB device (or something happens in transmission), the urb callback is called by the USB core. In our example, we initialized the urb to point to the function skel_write_bulk_callback, and that is the function that is called:

    static void skel_write_bulk_callback(struct urb *urb, struct pt_regs *regs)
    {
        /* sync/async unlink faults aren't errors */
        if (urb->status &&
            !(urb->status = = -ENOENT ||
            urb->status = = -ECONNRESET ||
            urb->status = = -ESHUTDOWN))
        {
            dbg("%s - nonzero write bulk status received: %d", __FUNCTION__, urb->status);
        }

        /* free up our allocated buffer */
        usb_buffer_free(urb->dev, urb->transfer_buffer_length,
        urb->transfer_buffer, urb->transfer_dma);
    }

The first thingthe callback function does is check the status of the urb to determine if this urb completed successfully or not. The error values, -ENOENT, -ECONNRESET, and -ESHUTDOWN are not real transmission errors, just reports about conditions accompanyinga successful transmission. (See the list of possible errors for urbs detailed in the section “struct urb.”) Then the callback frees up the allocated buffer that was assigned to this urb to transmit.

It’s common for another urb to be submitted to the device while the urb callback function is running. This is useful when streaming data to a device. Remember that the urb callback is runningin interrupt context, so it should do any memory allocation, hold any semaphores, or do anythingelse that could cause the process to sleep. When submittinga urb from within a callback, use the GFP_ATOMIC flag to tell the USB core to not sleep if it needs to allocate new memory chunks duringthe submission process.


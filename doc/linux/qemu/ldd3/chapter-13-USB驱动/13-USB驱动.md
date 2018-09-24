* [13.1 USB设备基础知识](#13.1)
    - [13.1.1 端点](#13.1.1)
    - [13.1.2 接口](#13.1.2)
    - [13.1.3 配置](#13.1.3)
* [13.2 USB和sysfs](#13.2)
* [13.3 USB的Urbs](#13.3)
    - 13.3.1 结构struct urb
    - 13.3.2 创建和销毁urb
        + 13.3.2.1 中断urb
        + 13.3.2.2 块urb
        + 13.3.2.3 控制urb
        + 13.3.2.4 同步urb
    - 13.3.3 提交urb
    - 13.3.4 完成urb：完成回调处理函数
    - 13.3.5 取消urb
* [13.4 编写一个USB驱动](#13.4 )
    * [13.4.1 驱动支持什么设备](#13.4.1 )
    * [13.4.2 注册一个USB设备](#13.4.2 )
        - 13.4.2.1 探测和去连接的细节
    * [13.4.3 提交和控制一个urb](#13.4.3 )
* [13.5 无urb的USB传送](#13.5)
    - 13.5.1 usb_bulk_msg接口
    - 13.5.2 usb_control_msg接口
    - 13.5.3 使用USB数据函数

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

A USB device is a very complex thing, as described in the official USB documentation (available at http://www.usb.org). Fortunately, the Linux kernel provides a subsystem called the USB core to handle most of the complexity. This chapter describes the interaction between a driver and the USB core. Figure 13-2 shows how USB devices consist of configurations, interfaces, and endpoints and how USB drivers bind to USB interfaces, not the entire USB device.

![Figure 13-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/13-1.PNG)
![Figure 13-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/13-2.PNG)

<h3 id="13.1.1">13.1.1 端点</h3>

The most basic form of USB communication is through something called an endpoint. A USB endpoint can carry data in only one direction, either from the host computer to the device (called an OUT endpoint) or from the device to the host computer (called an IN endpoint). Endpoints can be thought of as unidirectional pipes.
<h1 id="0">0 目录</h1>

* [12.1 PCI接口](#12.1 )
    * [12.1.1 PCI寻址](#12.1.1 )
    * [12.1.2 启动时间](#12.1.2 )
    * [12.1.3 配置寄存器和初始化](#12.1.3 )
    * [12.1.4 MODULEDEVICETABLE宏](#12.1.4 )
    * [12.1.5 注册PCI驱动](#12.1.5 )
    * [12.1.6 老式PCI探测](#12.1.6 )
    * [12.1.7 存取配置空间](#12.1.7 )
    * [12.1.8 存取I/O和内存空间](#12.1.8 )
    * [12.1.9 存取I/O和内存空间](#12.1.9 )
    * [12.1.10 PCI中断](#12.1.10 )
    * [12.1.11 硬件抽象](#12.1.11 )
* [12.2 回归：ISA](#12.2 )
    * [12.2.1 硬件资源](#12.2.1 )
    * [12.2.2 ISA编程](#12.2.2 )
    * [12.2.3 即插即用规范](#12.2.3 )
* [12.3 PC/104和PC/104+](#12.3 )
* [12.4 其它PC总线](#12.4 )
    * [12.4.1 MCA总线](#12.4.1 )
    * [12.4.2 EISA总线](#12.4.2 )
    * [12.4.3 VLB总线](#12.4.3 )
* [12.5 链表](#12.5 )

***

While Chapter 9 introduced the lowest levels of hardware control, this chapter provides an overview of the higher-level bus architectures. A bus is made up of both an electrical interface and a programming interface. In this chapter, we deal with the programming interface.

This chapter covers a number of bus architectures. However, the primary focus is on the kernel functions that access Peripheral Component Interconnect (PCI) peripherals, because these days the PCI bus is the most commonly used peripheral bus on desktops and bigger computers. The bus is the one that is best supported by the kernel. ISA is still common for electronic hobbyists and is described later, although it is pretty much a bare-metal kind of bus, and there isn’t much to say in addition to what is covered in Chapters 9 and 10.

<h2 id="12.1">12.1 PCI接口</h2>

Although many computer users think of PCI as a way of laying out electrical wires, it is actually a complete set of specifications defining how different parts of a computer should interact.

The PCI specification covers most issues related to computer interfaces. We are not going to cover it all here; in this section, we are mainly concerned with how a PCI driver can find its hardware and gain access to it. The probing techniques discussed in the sections “Module Parameters” in Chapter 2 and “Autodetecting the IRQ Number” in Chapter 10 can be used with PCI devices, but the specification offers an alternative that is preferable to probing.

The PCI architecture was designed as a replacement for the ISA standard, with three main goals: to get better performance when transferring data between the computer and its peripherals, to be as platform independent as possible, and to simplify adding and removing peripherals to the system.

The PCI bus achieves better performance by using a higher clock rate than ISA; its clock runs at 25 or 33 MHz (its actual rate being a factor of the system clock), and 66-MHz and even 133-MHz implementations have recently been deployed as well. Moreover, it is equipped with a 32-bit data bus, and a 64-bit extension has been included in the specification. Platform independence is often a goal in the design of a computer bus, and it’s an especially important feature of PCI, because the PC world has always been dominated by processor-specific interface standards. PCI is currently used extensively on IA-32, Alpha, PowerPC, SPARC64, and IA-64 systems, and some other platforms as well.

What is most relevant to the driver writer, however, is PCI’s support for autodetection of interface boards. PCI devices are jumperless (unlike most older peripherals) and are automatically configured at boot time. Then, the device driver must be able to access configuration information in the device in order to complete initialization. This happens without the need to perform any probing.

<h3 id="12.1.1">12.1.1 PCI寻址</h3>

Each PCI peripheral is identified by a bus number, a device number, and a function number. The PCI specification permits a single system to host up to 256 buses, but because 256 buses are not sufficient for many large systems, Linux now supports PCI domains. Each PCI domain can host up to 256 buses. Each bus hosts up to 32 devices, and each device can be a multifunction board (such as an audio device with an accompanying CD-ROM drive) with a maximum of eight functions. Therefore, each function can be identified at hardware level by a 16-bit address, or key. Device drivers written for Linux, though, don’t need to deal with those binary addresses, because they use a specific data structure, called pci_dev, to act on the devices.

Most recent workstations feature at least two PCI buses. Plugging more than one bus in a single system is accomplished by means of bridges, special-purpose PCI peripherals whose task is joining two buses. The overall layout of a PCI system is a tree where each bus is connected to an upper-layer bus, up to bus 0 at the root of the tree. The CardBus PC-card system is also connected to the PCI system via bridges. A typical PCI system is represented in Figure 12-1, where the various bridges are highlighted.

The 16-bit hardware addresses associated with PCI peripherals, although mostly hidden in the struct pci_dev object, are still visible occasionally, especially when lists of devices are being used. One such situation is the output of `lspci` (part of the `pciutils` package, available with most distributions) and the layout of information in `/proc/pci` and `/proc/bus/pci`. The sysfs representation of PCI devices also shows this addressing scheme, with the addition of the PCI domain information.* When the hardware address is displayed, it can be shown as two values (an 8-bit bus number and an 8-bit device and function number), as three values (bus, device, and function), or as four values (domain, bus, device, and function); all the values are usually displayed in hexadecimal.

![图12-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/12-1.PNG)

For example, `/proc/bus/pci/devices` uses a single 16-bit field (to ease parsing and sorting), while `/proc/bus/busnumber` splits the address into three fields. The following shows how those addresses appear, showing only the beginning of the output lines:

    $ lspci | cut -d: -f1-3
    0000:00:00.0 Host bridge
    0000:00:00.1 RAM memory
    0000:00:00.2 RAM memory
    0000:00:02.0 USB Controller
    0000:00:04.0 Multimedia audio controller
    0000:00:06.0 Bridge
    0000:00:07.0 ISA bridge
    0000:00:09.0 USB Controller
    0000:00:09.1 USB Controller
    0000:00:09.2 USB Controller
    0000:00:0c.0 CardBus bridge
    0000:00:0f.0 IDE interface
    0000:00:10.0 Ethernet controller
    0000:00:12.0 Network controller
    0000:00:13.0 FireWire (IEEE 1394)
    0000:00:14.0 VGA compatible controller
    $ cat /proc/bus/pci/devices | cut -f1
    0000
    0001
    0002
    0010
    0020
    0030
    0038
    0048
    0049
    004a
    0060
    0078
    0080
    0090
    0098
    00a0
    $ tree /sys/bus/pci/devices/
    /sys/bus/pci/devices/
    |-- 0000:00:00.0 -> ../../../devices/pci0000:00/0000:00:00.0
    |-- 0000:00:00.1 -> ../../../devices/pci0000:00/0000:00:00.1
    |-- 0000:00:00.2 -> ../../../devices/pci0000:00/0000:00:00.2
    |-- 0000:00:02.0 -> ../../../devices/pci0000:00/0000:00:02.0
    |-- 0000:00:04.0 -> ../../../devices/pci0000:00/0000:00:04.0
    |-- 0000:00:06.0 -> ../../../devices/pci0000:00/0000:00:06.0
    |-- 0000:00:07.0 -> ../../../devices/pci0000:00/0000:00:07.0
    |-- 0000:00:09.0 -> ../../../devices/pci0000:00/0000:00:09.0
    |-- 0000:00:09.1 -> ../../../devices/pci0000:00/0000:00:09.1
    |-- 0000:00:09.2 -> ../../../devices/pci0000:00/0000:00:09.2
    |-- 0000:00:0c.0 -> ../../../devices/pci0000:00/0000:00:0c.0
    |-- 0000:00:0f.0 -> ../../../devices/pci0000:00/0000:00:0f.0
    |-- 0000:00:10.0 -> ../../../devices/pci0000:00/0000:00:10.0
    |-- 0000:00:12.0 -> ../../../devices/pci0000:00/0000:00:12.0
    |-- 0000:00:13.0 -> ../../../devices/pci0000:00/0000:00:13.0
    `-- 0000:00:14.0 -> ../../../devices/pci0000:00/0000:00:14.0

上面的3个设备列表，按照相同的顺序进行排列，因为`lspci`使用`/proc`文件作为它的信息源。以VGA视频控制器为例，`0x00a0`与`0000:00:14.0`是等价的，总线控制域占有16位（0x0000），总线占用8位（0x00），设备占用5位（0x14），功能占用3位（0x0）。

The hardware circuitry of each peripheral board answers queries pertaining to three address spaces: memory locations, I/O ports, and configuration registers. The first two address spaces are shared by all the devices on the same PCI bus (i.e., when you access a memory location, all the devices on that PCI bus see the bus cycle at the same time). The configuration space, on the other hand, exploits geographical addressing. Configuration queries address only one slot at a time, so they never collide.

As far as the driver is concerned, memory and I/O regions are accessed in the usual ways via inb, readb, and so forth. Configuration transactions, on the other hand, are performed by calling specific kernel functions to access configuration registers. With regard to interrupts, every PCI slot has four interrupt pins, and each device function can use one of them without being concerned about how those pins are routed to the CPU. Such routing is the responsibility of the computer platform and is implemented outside of the PCI bus. Since the PCI specification requires interrupt lines to be shareable, even a processor with a limited number of IRQ lines, such as the x86, can host many PCI interface boards (each with four interrupt pins).

The I/O space in a PCI bus uses a 32-bit address bus (leading to 4 GB of I/O ports), while the memory space can be accessed with either 32-bit or 64-bit addresses. 64-bit addresses are available on more recent platforms. Addresses are supposed to be unique to one device, but software may erroneously configure two devices to the same address, making it impossible to access either one. But this problem never occurs unless a driver is willingly playing with registers it shouldn’t touch. The good news is that every memory and I/O address region offered by the interface board can be remapped by means of configuration transactions. That is, the firmware initializes PCI hardware at system boot, mapping each region to a different address to avoid collisions.* The addresses to which these regions are currently mapped can be read from the configuration space, so the Linux driver can access its devices without probing. After reading the configuration registers, the driver can safely access its hardware.

The PCI configuration space consists of 256 bytes for each device function (except for PCI Express devices, which have 4 KB of configuration space for each function), and the layout of the configuration registers is standardized. Four bytes of the configuration space hold a unique function ID, so the driver can identify its device by looking for the specific ID for that peripheral. In summary, each device board is geographically addressed to retrieve its configuration registers; the information in those registers can then be used to perform normal I/O access, without the need for further geographic addressing.

It should be clear from this description that the main innovation of the PCI interface standard over ISA is the configuration address space. Therefore, in addition to the usual driver code, a PCI driver needs the ability to access the configuration space, in order to save itself from risky probing tasks.

For the remainder of this chapter, we use the word `device` to refer to a device function, because each function in a multifunction board acts as an independent entity. When we refer to a device, we mean the tuple “domain number, bus number, device number, and function number.”

在本章的其余部分，使用单词`设备-device`指代设备功能，因为多功能主板上的每一个功能都是一个独立的实体。当我们提及`设备-device`时，代表着是一个元组信息`控制域编号，总线编号，设备编号，和功能编号`。

> <font color="blue">总结:</font>
>
> <font color="blue">每个PCI设备由一个控制域编号，一个总线编号，一个设备编号及一个功能编号来表示。</font>


<h3 id="12.1.2">12.1.2 引导阶段</h3>

To see how PCI works, we start from system boot, since that’s when the devices are configured.

When power is applied to a PCI device, the hardware remains inactive. In other words, the device responds only to configuration transactions. At power on, the device has no memory and no I/O ports mapped in the computer’s address space; every other device-specific feature, such as interrupt reporting, is disabled as well.

Fortunately, every PCI motherboard is equipped with PCI-aware firmware, called the BIOS, NVRAM, or PROM, depending on the platform. The firmware offers access to the device configuration address space by reading and writing registers in the PCI controller.

At system boot, the firmware (or the Linux kernel, if so configured) performs configuration transactions with every PCI peripheral in order to allocate a safe place for each address region it offers. By the time a device driver accesses the device, its memory and I/O regions have already been mapped into the processor’s address space. The driver can change this default assignment, but it never needs to do that.

As suggested, the user can look at the PCI device list and the devices’ configuration registers by reading `/proc/bus/pci/devices` and `/proc/bus/pci/*/*`. The former is a text file with (hexadecimal) device information, and the latter are binary files that report a snapshot of the configuration registers of each device, one file per device. The individual PCI device directories in the sysfs tree can be found in `/sys/bus/pci/devices`. A PCI device directory contains a number of different files:

    $ tree /sys/bus/pci/devices/0000:00:10.0
    /sys/bus/pci/devices/0000:00:10.0
    |-- class
    |-- config
    |-- detach_state
    |-- device
    |-- irq
    |-- power
    | `-- state
    |-- resource
    |-- subsystem_device
    |-- subsystem_vendor
    `-- vendor

The file `config` is a binary file that allows the raw PCI config information to be read from the device (just like the `/proc/bus/pci/*/*` provides.) The files `vendor`, `device`,`subsystem_device`, `subsystem_vendor`, and `class` all refer to the specific values of this PCI device (all PCI devices provide this information.) The file `irq` shows the current IRQ assigned to this PCI device, and the file `resource` shows the current memory resources allocated by this device.

> <font color="blue">总结:</font>
>
> <font color="blue">在系统引导阶段，配置好PCI总线上的各个外设。</font>

<h3 id="12.1.3">12.1.3 配置寄存器和初始化</h3>

In this section, we look at the configuration registers that PCI devices contain. All PCI devices feature at least a 256-byte address space. The first 64 bytes are standardized, while the rest are device dependent. Figure 12-2 shows the layout of the device independent configuration space.

![图12-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/12-2.PNG)
图12-2 标准化的PCI配置寄存器



<h3 id="12.1.4">12.1.4 MODULE_DEVICE_TABLE</h3>
<h3 id="12.1.5">12.1.5 注册PCI驱动</h3>
<h3 id="12.1.6">12.1.6 老式PCI探测</h3>
<h3 id="12.1.7">12.1.7 存取配置空间</h3>
<h3 id="12.1.8">12.1.8 存取I/O和内存空间</h3>
<h3 id="12.1.9">12.1.9 存取I/O和内存空间</h3>
<h3 id="12.1.10">12.1.10 PCI中断</h3>

As far as interrupts are concerned, PCI is easy to handle. By the time Linux boots, the computer’s firmware has already assigned a unique interrupt number to the device, and the driver just needs to use it. The interrupt number is stored in configuration register 60 (`PCI_INTERRUPT_LINE`), which is one byte wide. This allows for as many as 256 interrupt lines, but the actual limit depends on the CPU being used. The driver doesn’t need to bother checking the interrupt number, because the value found in PCI_INTERRUPT_LINE is guaranteed to be the right one.




<h3 id="12.1.11">12.1.11 硬件抽象</h3>
<h2 id="12.2">12.2 回归：ISA</h2>

<h3 id="12.2.1">12.2.1 硬件资源</h3>
<h3 id="12.2.2">12.2.2 EISA总线</h3>
<h3 id="12.2.3">12.2.3 VLB总线</h3>

<h2 id="12.3">12.3 PC/104和PC/104+</h2>

<h2 id="12.4">12.4 其它PC总线</h2>

<h3 id="12.4.1">12.4.1 MCA总线</h3>
<h3 id="12.4.2">14.4.2 ISA编程</h3>
<h3 id="12.4.3">12.4.3 即插即用规范</h3>

<h2 id="12.5">12.5 链表</h2>
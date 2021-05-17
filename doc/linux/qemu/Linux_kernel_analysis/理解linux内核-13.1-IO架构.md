[TOC]

The Virtual File System in the last chapter depends on lower-level functions to carry out each read, write, or other operation in a manner suited to each device. The previous chapter included a brief discussion of how operations are handled by different filesystems. In this chapter, we look at how the kernel invokes the operations on actual devices.

In the section “I/O Architecture,” we give a brief survey of the 80 × 86 I/O architecture. In the section “The Device Driver Model,” we introduce the Linux device driver model. Next, in the section “Device Files,” we show how the VFS associates a special file called “device file” with each different hardware device, so that application programs can use all kinds of devices in the same way. We then introduce in the section “Device Drivers” some common characteristics of device drivers. Finally, in the section “Character Device Drivers,” we illustrate the overall organization of character device drivers in Linux. We’ll defer the discussion of block device drivers to the next chapters.

Readers interested in developing device drivers on their own may want to refer to Jonathan Corbet, Alessandro Rubini, and Greg Kroah-Hartman’s Linux Device Drivers, Third Edition (O’Reilly).

## 1 I/O架构

To make a computer work properly, data paths must be provided that let information flow between CPU(s), RAM, and the score of I/O devices that can be connected to a personal computer. These data paths, which are denoted as the buses, act as the primary communication channels inside the computer.

Any computer has a system bus that connects most of the internal hardware devices. A typical system bus is the PCI (Peripheral Component Interconnect) bus. Several other types of buses, such as ISA, EISA, MCA, SCSI, and USB, are currently in use. Typically, the same computer includes several buses of different types, linked together by hardware devices called bridges. Two high-speed buses are dedicated to the data transfers to and from the memory chips: the frontside bus connects the CPUs to the RAM controller, while the backside bus connects the CPUs directly to the external hardware cache. The host bridge links together the system bus and the frontside bus.

Any I/O device is hosted by one, and only one, bus. The bus type affects the internal design of the I/O device, as well as how the device has to be handled by the kernel. In this section, we discuss the functional characteristics common to all PC architectures, without giving details about a specific bus type.

The data path that connects a CPU to an I/O device is generically called an I/O bus. The 80 × 86 microprocessors use 16 of their address pins to address I/O devices and 8, 16, or 32 of their data pins to transfer data. The I/O bus, in turn, is connected to each I/O device by means of a hierarchy of hardware components including up to three elements: I/O ports, interfaces, and device controllers. Figure 13-1 shows the components of the I/O architecture.

<img id="Figure_3-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_13_1.PNG">

## 2 I/O端口

Each device connected to the I/O bus has its own set of I/O addresses, which are usually called I/O ports. In the IBM PC architecture, the I/O address space provides up to 65,536 8-bit I/O ports. Two consecutive 8-bit ports may be regarded as a single 16-bit port, which must start on an even address. Similarly, two consecutive 16-bit ports may be regarded as a single 32-bit port, which must start on an address that is a multiple of 4. Four special assembly language instructions called `in`, `ins`, `out`, and `outs` allow the CPU to read from and write into an I/O port. While executing one of these instructions, the CPU selects the required I/O port and transfers the data between a CPU register and the port.

I/O ports may also be mapped into addresses of the physical address space. The processor is then able to communicate with an I/O device by issuing assembly language instructions that operate directly on memory (for instance, `mov`, and, `or`, and so on). Modern hardware devices are more suited to mapped I/O, because it is faster and can be combined with DMA.

An important objective for system designers is to offer a unified approach to I/O programming without sacrificing performance. Toward that end, the I/O ports of each device are structured into a set of specialized registers, as shown in Figure 13-2. The CPU writes the commands to be sent to the device into the device control register and reads a value that represents the internal state of the device from the device status register. The CPU also fetches data from the device by reading bytes from the device input register and pushes data to the device by writing bytes into the device output register.

<img id="Figure_3-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_13_2.PNG">

To lower costs, the same I/O port is often used for different purposes. For instance, some bits describe the device state, while others specify the command to be issued to the device. Similarly, the same I/O port may be used as an input register or an output register.

#### 2.1 访问I/O端口

80x86架构为访问外设I/O端口，专门提供了汇编指令`in`和`out`。Linux内核对其进行了封装，方便使用：

1. inb()、inw()、inl()

    分别从I/O端口读取1、2、4个字节。

2. inb_p()、inw_p()、inl_p()

    同上，仅仅是加了一个`dummy`指令，引入一个暂停，为了等待数据真正读取

3. outb()、outw()、outl()

    分别写入1、2、4个字节到I/O端口中。

4. outb_p()、outw_p()、outl_p()

    功能同3。引入`dummy`指令。

5. insb()、insw()、insl()

    读取一组连续的字节，具体长度由函数的参数指定。

6. outsb()、outsw()、outsl()

    同上相反。

While accessing I/O ports is simple, detecting which I/O ports have been assigned to I/O devices may not be easy, in particular, for systems based on an ISA bus. Often a device driver must blindly write into some I/O port to probe the hardware device; if, however, this I/O port is already used by some other hardware device, a system crash could occur. To prevent such situations, the kernel keeps track of I/O ports assigned to each hardware device by means of “resources.”

A resource represents a portion of some entity that can be exclusively assigned to a device driver. In our case, a resource represents a range of I/O port addresses. The information relative to each resource is stored in a resource data structure, whose fields are shown in Table 13-1. All resources of the same kind are inserted in a tree-like data structure; for instance, all resources representing I/O port address ranges are included in a tree rooted at the node ioport_resource.

Table 13-1. The fields of the resource data structure

| Type | Field | Description |
| ---- | ----- | ----------- |
| const char *  | name  | 资源所有者的名称 |
| unsigned long | start | 资源的起始位置 |
| unsigned long | end   | 资源的结束位置 |
| unsigned long | flags | 标志 |
| struct resource * | parent  | 指向资源树中的父节点指针 |
| struct resource * | sibling | 指向资源树中的兄弟节点指针 |
| struct resource * | child   | 指向资源树中的第一个子节点 |

The children of a node are collected in a list whose first element is pointed to by the `child` field. The sibling field points to the next node in the list.

Why use a tree? Well, consider, for instance, the I/O port addresses used by an IDE hard disk interface—let’s say from 0xf000 to 0xf00f. A resource with the `start` field set to 0xf000 and the `end` field set to 0xf00f is then included in the tree, and the conventional name of the controller is stored in the name field. However, the IDE device driver needs to remember another bit of information, namely that the subrange from 0xf000 to 0xf007 is used for the master disk of the IDE chain, while the subrange from 0xf008 to 0xf00f is used for the slave disk. To do this, the device driver inserts two children below the resource corresponding to the whole range from 0xf000 to 0xf00f, one child for each subrange of I/O ports. As a general rule, each node of the tree must correspond to a subrange of the range associated with the parent. The root of the I/O port resource tree (ioport_resource) spans the whole I/O address space (from port number 0 to 65535).

Each device driver may use the following three functions, passing to them the root node of the resource tree and the address of a resource data structure of interest:

1. request_resource()

    Assigns a given range to an I/O device.

2. allocate_resource()

    Finds an available range having a given size and alignment in the resource tree; if it exists, assigns the range to an I/O device (mainly used by drivers of PCI devices, which can be configured to use arbitrary port numbers and on-board memory addresses).

3. release_resource()

    Releases a given range previously assigned to an I/O device.

The kernel also defines some shortcuts to the above functions that apply to I/O ports: request_region() assigns a given interval of I/O ports and release_region() releases a previously assigned interval of I/O ports. The tree of all I/O addresses currently assigned to I/O devices can be obtained from the /proc/ioports file.

## 3 I/O接口

An I/O interface is a hardware circuit inserted between a group of I/O ports and the corresponding device controller. It acts as an interpreter that translates the values in the I/O ports into commands and data for the device. In the opposite direction, it detects changes in the device state and correspondingly updates the I/O port that plays the role of status register. This circuit can also be connected through an IRQ line to a Programmable Interrupt Controller, so that it issues interrupt requests on behalf of the device.

There are two types of interfaces:

1. Custom I/O interfaces

    Devoted to one specific hardware device. In some cases, the device controller is located in the same card * that contains the I/O interface. The devices attached to a custom I/O interface can be either internal devices (devices located inside the PC’s cabinet) or external devices (devices located outside the PC’s cabinet).

2. General-purpose I/O interfaces

    Used to connect several different hardware devices. Devices attached to a general-purpose I/O interface are usually external devices.

#### 3.1 专用I/O接口

Just to give an idea of how much variety is encompassed by custom I/O interfaces—thus by the devices currently installed in a PC—we’ll list some of the most commonly found:

1. Keyboard interface

    Connected to a keyboard controller that includes a dedicated microprocessor. This microprocessor decodes the combination of pressed keys, generates an interrupt, and puts the corresponding scan code in an input register.

2. Graphic interface

    Packed together with the corresponding controller in a graphic card that has its own frame buffer, as well as a specialized processor and some code stored in a Read-Only Memory chip (ROM). The frame buffer is an on-board memory containing a description of the current screen contents.

3. Disk interface

    Connected by a cable to the disk controller, which is usually integrated with the disk. For instance, the IDE interface is connected by a 40-wire flat conductor cable to an intelligent disk controller that can be found on the disk itself.

4. Bus mouse interface

    Connected by a cable to the corresponding controller, which is included in the mouse.

5. Network interface

    Packed together with the corresponding controller in a network card used to receive or transmit network packets. Although there are several widely adopted network standards, Ethernet (IEEE 802.3) is the most common.

#### 3.2 通用I/O接口

Modern PCs include several general-purpose I/O interfaces, which connect a wide range of external devices. The most common interfaces are:

1. Parallel port

    Traditionally used to connect printers, it can also be used to connect removable disks, scanners, backup units, and other computers. The data is transferred 1 byte (8 bits) at a time.

2. Serial port

    Like the parallel port, but the data is transferred 1 bit at a time. It includes a Universal Asynchronous Receiver and Transmitter (UART) chip to string out the bytes to be sent into a sequence of bits and to reassemble the received bits into bytes. Because it is intrinsically slower than the parallel port, this interface is mainly used to connect external devices that do not operate at a high speed, such as modems, mouses, and printers.

3. PCMCIA interface

    Included mostly on portable computers. The external device, which has the shape of a credit card, can be inserted into and removed from a slot without rebooting the system. The most common PCMCIA devices are hard disks, modems, network cards, and RAM expansions.

4. SCSI (Small Computer System Interface) interface

    A circuit that connects the main PC bus to a secondary bus called the SCSI bus. The SCSI-2 bus allows up to eight PCs and external devices—hard disks, scanners, CD-ROM writers, and so on—to be connected. Wide SCSI-2 and the SCSI-3 interfaces allow you to connect 16 devices or more if additional interfaces are present. The SCSI standard is the communication protocol used to connect devices via the SCSI bus.

5. Universal serial bus (USB)

    A general-purpose I/O interface that operates at a high speed and may be used for the external devices traditionally connected to the parallel port, the serial port, and the SCSI interface.

## 4 设备控制器

A complex device may require a device controller to drive it. Essentially, the controller plays two important roles:

* It interprets the high-level commands received from the I/O interface and forces the device to execute specific actions by sending proper sequences of electrical signals to it.

* It converts and properly interprets the electrical signals received from the device and modifies (through the I/O interface) the value of the status register.

A typical device controller is the disk controller, which receives high-level commands such as a "write this block of data" from the microprocessor (through the I/O interface) and converts them into low-level disk operations such as "position the disk head on the right track" and "write the data inside the track". Modern disk controllers are very sophisticated, because they can keep the disk data in on-board fast disk caches and can reorder the CPU high-level requests optimized for the actual disk geometry.

Simpler devices do not have a device controller; examples include the Programmable Interrupt Controller (see the section “Interrupts and Exceptions” in Chapter 4) and the Programmable Interval Timer (see the section “Programmable Interval Timer (PIT)” in Chapter 6).

Several hardware devices include their own memory, which is often called I/O shared memory. For instance, all recent graphic cards include tens of megabytes of RAM in the frame buffer, which is used to store the screen image to be displayed on the monitor. We will discuss I/O shared memory in the section “Accessing the I/O Shared Memory” later in this chapter.

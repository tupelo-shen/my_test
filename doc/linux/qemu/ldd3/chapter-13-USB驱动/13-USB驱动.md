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
    - [13.3.4 完成urb时调用的服务函数](#13.3.4)
    - [13.3.5 取消urb](#13.3.5)
* [13.4 编写一个USB驱动](#13.4)
    * [13.4.1 驱动支持什么设备](#13.4.1)
    * [13.4.2 注册一个USB设备](#13.4.2)
        - [13.4.2.1 probe和disconnect的细节](#13.4.2.1)
    * [13.4.3 提交和控制一个urb](#13.4.3)
* [13.5 无urb的USB传送](#13.5)
    - [13.5.1 usb_bulk_msg接口](#13.5.1)
    - [13.5.2 usb_control_msg接口](#13.5.2)
    - [13.5.3 其它的USB数据函数](#13.5.3)

***

通用串行总线（USB）是主机连接外设的方式。创建它的最初目的就是，用简单的总线，取代杂乱无章的低速总线-并口，串口，和键盘连接等。现在，USB几乎支持所有连接到PC上的设备类型。理论上，最新的高速USB接口速度达到480 Mbps。

在拓扑结构上，USB子系统并不是一个总线类型，而是由几个点对点链接构建的树。类似于双绞线的以太网，USB连接需要四根线（地线，电源，2个信号线），连接hub和设备。USB主控制器负责询问每一个USB设备是否有数据需要发送。基于这种拓扑结构，如果主控制器没有主动询问，USB设备绝对不会主动发送数据。这种配置允许“即插即用”，从而主机可以自动配置设备。

USB总线在技术上非常简单，就是一个单主机，主机负责轮询各种外围设备。尽管有这种固有限制，但是USB总线还是有一些有趣的特点，比如可以为音视频的数据传输提供固定的带宽；另一个重要的特点就是，仅仅是作为主控制器和设备的通信手段，没有特殊的意义或对传输的数据结构有要求。

对于某种特定类型的设备，USB协议规范定义了一组标准，如果一设备遵循这个标准，那么就不需要特定的驱动。这些不同的类型称之为类，包括存储设备，键盘，鼠标，游戏杆，网络设备和调制解调器等。对于不符合这些类型的其它设备，就要求特定于厂商的特定驱动了。视频设备和USB转串口就是很好的例子，没有预定义的标准，不同厂商的驱动都不同。

这些功能，配合设计上固有的“热插拔”功能，使得USB成为一种便捷，低成本的连接设备和主机的方案。

Linux支持两种主要的USB驱动程序：主控制器端的驱动程序和设备上的驱动程序。

如图13-1所示，USB驱动位于不同的内核子系统（块，网络，字符，等等）和USB硬件控制器之间。USB核为USB驱动提供了一个访问和控制USB硬件的一个接口，而无需担心USB硬件控制器的类型不同。

<h2 id="13.1">13.1 USB设备基础知识</h2>

USB设备是一个复杂的事物，详细描述可以参考[官方USB文档](http://www.usb.org)。幸运的是，Linux内核提供了一个子系统-USB核-负责处理这些复杂的事情。本章主要描述驱动和USB核之间的交互。图13-2展示了USB设备的组成，配置，接口和端点以及USB驱动是如何绑定到USB接口上，而不是整个USB设备上。

![Figure 13-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/13-1.PNG)
![Figure 13-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/13-2.PNG)

<h3 id="13.1.1">13.1.1 端点</h3>

USB通信最基本的单元就是端点。USB端点只能在一个方向上传输数据，比如，从主机到设备（称为OUT端点）或从设备到主机（称为IN端点）。端点可以被认为是单向管道。

USB端点有4种类型：

* CONTROL

    `CONTROL`端点被用于允许访问USB设备的不同部分。被用来配置设备，检索设备信息，向设备发送命令，或检索设备的状态报告。这些端点通常很小。每个USB设备都有一个称为“端点0”的端点，USB核用来在设备插入时，配置设备使用。这些数据的传输由USB协议保证，保留足够的带宽，以使数据穿过。

* INTERRUPT

    每次USB主控制器向设备请求数据时，`INTERRUPT`端点以固定速率传输少量数据。这些端点是USB键盘和鼠标的主要传输方式。往往，也会被用来向USB设备发送数据，以便控制设备，但是通常不会传输大量数据。传输由USB协议保证足够的带宽。

* BULK

    `BULK`端点传输大量数据。传输数据量大于`INTERRUPT`端点（每次可以携带更多字符）。常常用于保证数据传输没有损失的设备中。USB协议无法保证在特定的时间段内完成。如果总线上没有足够的空间发送整个BULK数据包，它会被分成多个发送到总线上。常常用于打印机，存储和网络设备。

* ISOCHRONOUS

    `ISOCHRONOUS`端点也可以传输大量数据，但是不保证数据完成传输。这些端点用于能够处理数据丢失，依赖于保持恒定的数据流的设备。实时数据收集（例如音频和视频设备）总是使用这种端点。

`CONTROL`和`BULK`端点用于异步数据传输。`INTERRUPT`和`ISOCHRONOUS`是周期性的。其带宽是由USB核保留。

内核中使用 `struct usb_host_endpoint`描述USB端点。该结构体还包含另一个结构体`struct usb_endpoint_descriptor`，其保存着真实的端点信息。后一个结构体包含USB具体的数据，数据格式是由设备本身指定的。该结构体的主要成员描述如下：

* bEndpointAddress

    这是此端点的USB地址。此8位数值中还包括端点的方向。使用位掩码 `USB_DIR_OUT` 和 `USB_DIR_IN`决定该端点是指向设备的还是主机的。

* bmAttributes

    端点类型。 使用位掩码`USB_ENDPOINT_XFERTYPE_MASK` 决定端点的类型是 `USB_ENDPOINT_XFER_ISOC`, `USB_ENDPOINT_XFER_BULK`, 或 `USB_ENDPOINT_XFER_INT`。

* wMaxPacketSize

    该端点一次可以处理的最大数据，以字节为单位。需要注意的是，驱动程序有可能发送大于该值的数据，但是实际发送给设备时，数据将会被按`wMaxPacketSize`为大小分成块。对于高速设备，使用该成员的高位支持端点的高带宽模式。具体内容可以查看USB规约，是如何实现的。

* bInterval

    端点类型为`INTERRUPT`类型时，为间隔设置。单位为mS。

该结构体的成员变量命名并不是遵循Linux内核的常用命名方法。这是因为这些变量都与USB规约有直接关系。所以，USB内核编程者觉得使用指定的名称更为重要，而不是Linux开发者更为熟悉的变量名称。

<h3 id="13.1.2">13.1.2 接口</h3>

把USB端点捆绑起来就称之为`接口`。 USB接口只能处理一种类型的逻辑连接，比如鼠标，键盘或音频流。有一些USB设备有多个接口，比如USB扬声器就有2个接口：用于按键的USB键盘类型接口和USB音频流类型接口。因为USB接口代表一种基本的功能，所以每一个USB驱动控制一个接口；那么，扬声器示例中，Linux就需要两个不同的驱动。

USB接口可能具有备用设置，这些设置是接口参数的不同选择。一个接口的初始状态就是第一个设置，编号为0。备用设置可以被用于以不同方式控制各个端点，比如为设备保留不同数量的USB带宽。使用`isochronous`端点的设备使用相同接口的备用设置。

在内核中使用结构体 `struct usb_interface`描述USB接口。这个结构体是USB核传递给USB驱动，然后USB驱动负责控制。其中重要的成员有：

* struct usb_host_interface *altsetting

    接口结构体数组，包含可供该接口选择的所有备选设置。每一个`struct usb_host_interface`包含由`struct usb_host_endpoint`结构体定义的端点集合。注意的是，这些接口结构没有特定的顺序。

* unsigned num_altsetting

    备选设置的数量。

* struct usb_host_interface *cur_altsetting

    指向数组`altsetting`的指针, 表示当前有效的设置。

* int minor

    USB驱动使用`major`主设备号绑定此接口，使用该变量指定`minor`设备号，其由USB核负责分配给`接口`。 只有成功调用函数 `usb_register_dev` (在本章的后面会描述)后才会有效。

结构体`usb_interface`还有一些其它成员，但是USB驱动不需要关注。

<h3 id="13.1.3">13.1.3 配置</h3>

将USB接口捆绑起来就是`配置`。USB设备能够拥有多个配置，可以在它们之间切换以改变设备的状态。例如，那些允许固件被下载的设备就包含多个配置来实现。在某一个时间点上只能有一个配置被使能。Linux不处理多种配置的USB设备，但是，幸运的是，它们极少。

Linux使用结构体`struct usb_host_config`描述USB配置，使用`struct usb_device`描述整个USB设备。USB设备通常不需要读写这些结构体中的数值，所以，在这里没有详细讨论。好奇的读者可以通过源代码树中的文件`include/linux/usb.h`进行查阅。

USB设备驱动通常需要将给定的结构类型为`struct usb_interface`的数据转化为结构为`struct usb_device`的数据，以满足多种函数调用的需求。Linux内核提供了这个函数接口，`interface_to_usbdev`。但愿在未来，这种接口函数不再被需要。

总而言之，USB设备相当复杂，由不同的逻辑单元组成。这些逻辑单元的关系简单描述如下：

* 设备可以拥有一个或多个配置

* 配置通常可以拥有一个或多个接口

* 接口通常拥有一个或多个设置

* 接口拥有`0`个或更多端点

<h2 id="13.2">13.2 USB和sysfs</h2>

USB设备在`sysfs`里的表示也比较复杂。物理USB设备（由结构体'struct usb_device'表示）和各个USB接口（由结构体 `struct usb_interface`表示）分别表示为单独的设备。这是因为2个结构体中都包含结构体`device`。例如，只包含一个USB接口的简单的USB鼠标，以下是该设备的'sysfs'目录树：

结构体`usb_device`在目录树中位置：

    /sys/devices/pci0000:00/0000:00:09.0/usb2/2-1

鼠标的USB接口-USB鼠标驱动绑定的接口-位于目录中的位置：

    /sys/devices/pci0000:00/0000:00:09.0/usb2/2-1/2-1:1.0

要想理解上面长长的设备路径名的意思，必须先来看一下内核是怎样标记USB设备的。

第一个USB设备是根集线器（`root hub`）。这是USB控制器，通常包含在PCI设备中。之所以如此命名，是因为它控制了连接到它的整个USB总线。这个控制器是PCI总线和USB总线之间的桥梁，也是该总线上的第一个USB设备。

USB核为所有的根集线器分配了唯一的编号。在我们的例子中，根集线器被称为usb2， 因为它是向USB核注册的第2个根集线器。系统中包含的根集线器的数量没有限制。

USB总线上的每一个设备都会选择根集线器（root hub）的编号作为它名称中的第一个数字，后面紧跟`-`，然后就是设备插入的端口号。例子中，端口号是1，所以USB鼠标的名称是2-1。由于此USB设备包含一个接口，所以在sysfs中还有另一个设备。USB接口的命名规则是，设备名称后面紧跟“：”符号，然后就是USB配置编号和接口编号，中间使用符号“.”隔开。例如，在上面的例子中，设备名称是`2-1:1.0`，因为这是第一个配置，接口编号为0。

所以总结起来，USB sysfs设备命名规则就是：

    根集线器-集线器端口号: 配置.接口

随之在USB设备树中，加入越来越多的设备，USB集线器也越来越多，集线器的端口号就添加到前一个集线器端口号的后面。对于一个两级深的树，设备名称看起来应该是：

    根集线器-集线器端口号-集线器端口号: 配置.接口

通过`sysfs`可以获得所有USB特定的信息（例如，设备厂商号-idVendor，产品id-idProduct，和 bMaxPower信息）。比如，可以修改文件`bConfigurationValue`，改变正在使用的USB配置。对于具有多个配置的USB设备，且内核不能决定选择哪个配置去正确操作设备的时候，这是非常有用的。许多USB调制解调器就需要将正确的配置写入该文件，以便将正确的驱动程序和设备进行绑定。

`Sysfs`只显示到接口层级。设备的备选配置和该接口有关的端点详细信息不显示，这些信息可以在`usbfs`文件系统里找到，其安装在'/proc/bus/usb/'目录下。`/proc/bus/usb/devices`显示系统中存在的所有USB设备的信息，包含备选配置和端点信息。`usbfs`允许用户空间的程序直接访问USB设备，这样许多内核驱动程序就可以被移动到用户空间，方便维护和调试。USB扫描仪驱动程序就是一个很好的例子，它的功能包含在用户空间的SANE库中，所以不再存在于内核中。

<h2 id="13.3">13.3 USB的Urbs</h2>

Linux内核中的USB代码使用USB请求块（urb-USB request block）与所有的USB设备进行通信。使用`struct urb`的结构体描述该请求块，其定义位于`include/linux/usb.h`文件中。

urb用于以异步方式向特定USB设备上的特定USB端点发送数据或从其接收数据。它的使用很像在文件系统异步I/O代码中使用的`kiocb`结构或在网络代码中使用的`struct skbuff`。 USB设备驱动程序可以为单个端点分配许多urb，或者可以根据驱动程序的需要为许多不同的端点重用某个urb。 设备中的每个端点都可以处理urb队列，以便在队列为空之前可以将多个urb发送到同一端点。 urb的典型生命周期如下：

1. 由USB设备驱动创建；
2. 分配给特定USB设备的特定端点；
3. 由USB设备驱动提交给USB核；
4. 由USB核提交给特定设备对应的USB主控制器的驱动程序；
5. 由USB主控制器驱动程序处理，并发送给设备；
6. 当urb完成时，由USB主控制器驱动程序通知USB设备驱动程序。

`urb`可以随时由提交它的驱动程序取消，或者当设备被移除是，由USB核取消。`urb`是动态创建的，包含一个内部引用计数器，保证当`urb`最后一个使用者释放它时，能够自动地被释放掉。

本章中描述的用于处理urb的过程很有用，因为它允许数据流传输和其他复杂的重叠通信，允许驱动程序实现尽可能高的数据传输速度。 但是，如果您只想发送单个bulk或控制消息而不关心数据吞吐率，则可以使用较为简单的过程。(查看 [无urb的USB传送](#13.5))

<h3 id="13.3.1">13.3.1 结构体struct urb</h3>

对USB设备驱动很重要的`struct urb`的成员变量：

* struct usb_device *dev

    指向`struct usb_device`的指针。指向`urb`要发送的目的地。这个变量必须在发送给USB核之前由USB驱动完成初始化。

* unsigned int pipe

    'urb'要发送的具体的`struct usb_device`的端点信息。这个变量必须在发送给USB核之前由USB驱动完成初始化。

    为了设置该字段，使用下面的函数。注意，每个端点只能是一种类型。

        1. unsigned int usb_sndctrlpipe(struct usb_device *dev, unsigned int endpoint)
        2. unsigned int usb_rcvctrlpipe(struct usb_device *dev, unsigned int endpoint)
        3. unsigned int usb_sndbulkpipe(struct usb_device *dev, unsigned int endpoint)
        4. unsigned int usb_rcvbulkpipe(struct usb_device *dev, unsigned int endpoint)
        5. unsigned int usb_sndintpipe(struct usb_device *dev, unsigned int endpoint)
        6. unsigned int usb_rcvintpipe(struct usb_device *dev, unsigned int endpoint)
        7. unsigned int usb_sndisocpipe(struct usb_device *dev, unsigned int endpoint)
        8. unsigned int usb_rcvisocpipe(struct usb_device *dev, unsigned int endpoint)

* unsigned int transfer_flags

    可以设置不同的位值，依赖于USB驱动想要`urb`做什么。取值可能是：

    * URB_SHORT_NOT_OK
    * URB_ISO_ASAP
    * URB_NO_TRANSFER_DMA_MAP
    * URB_NO_SETUP_DMA_MAP
    * URB_ASYNC_UNLINK
    * URB_NO_FSBR
    * URB_ZERO_PACKET
    * URB_NO_INTERRUPT

* void *transfer_buffer

    指向收发数据的缓冲区。为了主机控制器能够正确访问该缓冲区，必须调用`kmalloc`分配该空间，而不是在堆栈或静态分配区。对于`CONTROL`端点，该缓冲区用于传输的数据阶段。

* dma_addr_t transfer_dma

    使用DMA发送数据给USB设备的缓冲区。

* int transfer_buffer_length

    `transfer_buffer`和`transfer_dma`的数据长度。（对于一个urb，只能使用一个缓冲区）。如果这是0，USB核没有发送缓冲区。

* unsigned char *setup_packet

    在数据存入到发送缓冲区之前为一个控制`urb`发送的setup包。这个变量只对控制urb有效。

* dma_addr_t setup_dma

    同上，区别只是通过DMA方式而已。

* usb_complete_t complete

    由USB核在`urb`完全发送或当错误发生时调用的回调函数。在这个函数里，USB驱动可以检查urb，释放它，或者再次提交一次发送。（查阅：[完成urb时调用的服务函数](#13.3.4)获取更多详细信息）。

    `usb_complete_t`的类型定义为：

        typedef void (*usb_complete_t)(struct urb *, struct pt_regs *);

* void *context

    指向可由USB驱动程序设置的数据blob的指针。 当urb返回给驱动程序时，它可以在回调函数中使用。

* int actual_length

    当urb完成时，此变量设置为urb发送的数据的实际长度（对于OUT urbs）或由urb接收（对于IN urbs。）对于IN urbs，必须使用此变量而不是transfer_buffer_length 变量，因为接收的数据可能小于整个缓冲区大小。

* int status

    记录当前的`urb`的状态。USB驱动能够安全访问该变量的时间就是在urb完成时调用的回调函数里（[完成urb时调用的服务函数](#13.3.4)）。这种处理避免了和USB核正在处理urb时的数据竞争问题。对于isochronous urb，该值为0仅仅表明urb是否已经被取消链接。要想获得isochronous urb的详细状态，请检查`iso_frame_desc`变量。

    合法数值包括：

    * 0

        `urb`发送成功

    * -ENOENT

        通过调用`usb_kill_urb`来停止urb。

    * -ECONNRESET

        `urb`在调用函数`usb_unlink_urb`后取消了链接。前提是，`transfer_flags`必须被设置为 `URB_ASYNC_UNLINK`。

    * -EINPROGRESS

        The urb is still being processed by the USB host controllers. If your driver ever sees this value, it is a bug in your driver.

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

    通常，错误值`-EPROTO`，`-EILSEQ`和`-OVERFLOW`表示设备，设备固件或将设备连接到计算机的电缆存在硬件问题。

* int start_frame

    设置或返回要使用的等时传输的初始帧编号。

* int interval

    轮询`urb`的时间间隔。这仅适用于中断或等时`urb`。值的单位根据设备的速度而不同。对于低速和全速设备，单位是帧，相当于毫秒。对于设备，单位是微帧，相当于1/8毫秒。必须在发送给USB核之前由USB驱动设置好。

* int number_of_packets

    指定由该urb处理的等时发送缓存区的数量。只对isochronous urb有效。

* int error_count

    只对isochronous urb有效。

* struct usb_iso_packet_descriptor iso_frame_desc[0]

    只对isochronous urb有效。

<h3 id="13.3.2">13.3.2 创建和销毁urb</h3>

结构体`urb`只能动态创建，因为静态创建会破坏USB核为`urb`使用的引用计数方法。使用下面的函数创建，原型如下：

    struct urb *usb_alloc_urb(int iso_packets, int mem_flags);

第一个参数，`iso_packets`，是该`urb`应该包含的等时数据包的数量。如果不想创建一个等时`urb`，这个变量应该被设置为0。 第二个参数，`mem_flags`和传递给'kmalloc'函数的`flag`参数具有相同的类型。如果分配空间成功， 则函数返回指向`urb`结构的指针。 如果返回值为`NULL`， 在说明在USB核里发生错误， 驱动程序需要正确地清理处理。

在USB核使用`urb`之前，必须进行恰当的初始化。关于初始化看后面的内容。

为了告知USB核，驱动程序已经完成`urb`，驱动程序必须调用下面的函数：

    void usb_free_urb(struct urb *urb);

参数就是指向要释放的`urb`的指针。

<h3 id="13.3.2.1">13.3.2.1 中断urb</h3>

辅助函数`usb_fill_int_urb`用来初始化一个要发送给USB设备的中断类型端点的`urb`：

    void usb_fill_int_urb(struct urb *urb,          /* 要初始化的 urb */
                        struct usb_device *dev,     /* 要发送的目的USB设备 */
                        unsigned int pipe,          /* 要发送的USB设备的端点 */
                        void *transfer_buffer,      /* 存储发送和接收数据的地方 */
                        int buffer_length,          /* 数据长度 */
                        usb_complete_t complete,    /* urb完成时调用的函数 */
                        void *context,              /* 指向blob数据的指针，供完成时调用函数检索使用 */
                        int interval);              /* 该urb被调度的间隔 */

<h3 id="13.3.2.2">13.3.2.2 块urb</h3>

初始化块`urb`时的函数为：

    void usb_fill_bulk_urb(struct urb *urb,
                        struct usb_device *dev,
                        unsigned int pipe,
                        void *transfer_buffer,
                        int buffer_length,
                        usb_complete_t complete,
                        void *context);

与初始化中断`urb`的函数极为相似，只是没有间隔参数-interval。`pipe`参数的初始化函数也不同。

<h3 id="13.3.2.3">13.3.2.3 控制urb</h3>

初始化函数：

    void usb_fill_control_urb(struct urb *urb,
                            struct usb_device *dev,
                            unsigned int pipe,
                            unsigned char *setup_packet,
                            void *transfer_buffer,
                            int buffer_length,
                            usb_complete_t complete,
                            void *context);

它的参数和`usb_fill_bulk_urb`函数极为相似，只是新添加了一个参数`setup_packet`,它指向发送给端点的setup包数据。`pipe`使用函数`usb_sndctrlpipe`和`usb_rcvictrlpipe`进行初始化。

大部分驱动都不使用这个函数，因为正如[13.5 无urb的USB传送](#13.5)所描述的那样，使用synchronous API更为简单。

<h3 id="13.3.2.4">13.3.2.4 等时urb</h3>

不像`interrupt`, `control`, 和 `bulk`,等时`urb`没有初始化函数。所以，必须自己手动初始化。下面是一个初始化的例程。取自`drivers/usb/media`目录下`konicawc.c`文件的内容。

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

一旦`urb`被USB驱动程序正确创建并初始化，就可以将其提交给USB核以发送到USB设备。 这是通过调用函数`usb_submit_urb`来完成的：

    int usb_submit_urb(struct urb *urb, int mem_flags);

参数的意义与`usb_alloc_urb`函数相同。`mem_flags`参数告知USB核如何分配内核缓冲区。

在`urb`成功提交到USB核之后，它应该永远不会尝试访问`urb`结构的任何字段，直到调用的`complete`函数完成。

因为函数`usb_submit_urb`可以随时调用（包括在中断上下文中），所以`mem_flags`变量的规范必须是正确的。 实际上只应使用三个有效值，具体取决于调用`usb_submit_urb`的时间：

* GFP_ATOMIC

    下列情况下使用该标志：

    * 调用者在`urb`完成时调用函数，中断上下文，底半部，tasklet，或定时器回调函数里。
    * 调用者持有`spinlock` 或 `rwlock`。 注意：如果持有一个`semaphore`，该值不需要。
    * `current->state`不是`TASK_RUNNING`。该状态一直是`TASK_RUNNING` 除非驱动本身改变了当前的状态。

* GFP_NOIO

    如果驱动程序位于块I/O补丁中，使用该值。所有的存储类型的设备的错误处理路径上也应该使用该值。

* GFP_KERNEL

    除了上面两种情况之外使用。

<h3 id="13.3.4">13.3.4 完成urb时调用的服务函数</h3>

如果调用`usb_submit_urb`成功，就将`urb`的控制权交给了USB核，并返回0；否则，返回一个负值作为错误码。如果调用函数成功，一旦`urb`完成，就会调用先前指定的服务函数（通过`complete`函数指针）。一旦该函数被调用，`urb`的控制权就返回给设备驱动程序。

`urb`只有三种方式可以完成，然后调用`complete`函数：

* `urb`被成功地发送给设备，且设备给出了正确的应答。对于`OUT urb`是指数据成功发送，对于`IN urb`是指请求的数据被成功接收。如果都正确，`urb`中的`status`变量被设置为0。

* 从设备中收发数据发生错误时。具体的错误情况在结构体`urb`中的`status`变量的说明中已经指出。

* USB核取消`urb`链接。 这发生在驱动告知USB核取消`urb`的提交或者设备已经从系统中移除，再提交`urb`时。

本章稍后将介绍如何在`urb`完成时调用的服务函数中测试不同返回值。

<h3 id="13.3.5">13.3.5 取消urb</h3>

驱动告知USB核取消`urb`提交的函数为：

    int usb_kill_urb(struct urb *urb);
    int usb_unlink_urb(struct urb *urb);

参数urb指向要取消的urb。

调用函数 `usb_kill_urb`后，`urb`的声明周期就会停止。 这个函数通常在设备从系统中断开时使用，比如`disconnect`回调函数里。

对于某些驱动，应该使用`usb_unlink_urb`函数告知USB核停止`urb`。这个函数不会等到`urb`完全停止就会返回调用者。因为等待`urb`完全停止就要求USB核能够把调用者进程休眠，所以，这在中断上下文或持有自旋锁的情况时非常有用。必须设置`urb`结构中的`transfer_flags`为`URB_ASYNC_UNLINK`标志，才能正确工作。

<h2 id="13.4">13.4 编写USB驱动</h2>

编写USB设备驱动程序的方法类似于pci驱动：驱动程序使用USB子系统注册其驱动程序对象，稍后使用供应商和设备ID来判断其硬件是否已安装。

<h3 id="13.4.1">13.4.1 驱动支持什么设备</h3>

结构体`usb_device_id`提供了该驱动支持的不同USB设备类型的列表。 这个列表决定了当一个具体的设备插入了系统中时， USB核该给予设备哪种驱动， 然后通过`hotplug`自动载入那种驱动。

结构体`usb_device_id`的成员变量定义如下：

* __u16 match_flags

    确定设备应与之匹配的结构中的以下哪个字段。 这是由`include/linux/mod_devicetable.h`文件中指定的不同`USB_DEVICE_ID_MATCH_*`值定义的位字段。 该字段通常不会直接设置，而是由稍后描述的USB_DEVICE类型宏初始化。

* __u16 idVendor

    设备的USB供应商ID。 此号由USB协会分配给其成员，不能由其它任何人编写。

* __u16 idProduct

    设备的USB产品ID。 所有分配了供应商ID的供应商都可以管理他们选择的产品ID。

* __u16 bcdDevice_lo
* __u16 bcdDevice_hi

    定义供应商指定的产品版本号范围的起始范围。 范围包含`bcdDevice_hi`的值，它的值是供应商提供产品的最高版本号。 这两个值都是十进制表示。 这两个变量结合`idVendor`和`idProduct`被用来定义特定的版本号。

* __u8 bDeviceClass
* __u8 bDeviceSubClass
* __u8 bDeviceProtocol

    分别定义设备的类，子类和协议。 这些数字由USB协会分配，并在USB规范中定义。 这些值指定整个设备的行为， 包括此设备上的所有接口。

* __u8 bInterfaceClass
* __u8 bInterfaceSubClass
* __u8 bInterfaceProtocol

    与上面特定于设备的值非常相似，它们分别定义了各个接口的类，子类和协议。 这些数字由USB协会分配，并在USB规范中定义。

* kernel_ulong_t driver_info

    此值不用于匹配，但它包含驱动程序可用于在探针回调函数中将不同设备彼此区分开的信息。

与PCI设备一样，有许多宏用于初始化此结构：

* USB_DEVICE(vendor, product)

    创建一个`struct usb_device_id`，仅用于匹配指定的供应商和产品ID。 这通常用于需要特定驱动程序的USB设备。

* USB_DEVICE_VER(vendor, product, lo, hi)

    创建一个`struct usb_device_id`，用于在一个版本范围内，匹配指定的供应商和产品ID。

* USB_DEVICE_INFO(class, subclass, protocol)

    创建一个结构usb_device_id，可用于匹配特定类的USB设备。

* USB_INTERFACE_INFO(class, subclass, protocol)

    创建一个结构usb_device_id，可用于匹配特定接口类的USB设备。

对于仅控制一个来自单个厂商的USB设备的驱动程序来说，`struct usb_device_id`表 应该定义如下：

    /* 驱动支持的设备表 */
    static struct usb_device_id skel_table[ ] = {
        { USB_DEVICE(USB_SKEL_VENDOR_ID, USB_SKEL_PRODUCT_ID) },
        { } /* 结束项 */
    };
    MODULE_DEVICE_TABLE (usb, skel_table);

同PCI驱动一样，`MODULE_DEVICE_TABLE`宏是必须的， 因为要允许用户空间的程序能够确定该驱动可以控制什么设备。 对于USB驱动， 第一个参数必须是`usb`字符串。

<h3 id="13.4.2">13.4.2 注册一个USB设备</h3>

所有的USB驱动都必须注册的主要结构体是 `struct usb_driver`。 包含许多回调函数和变量， 由USB驱动程序完成实现。

* struct module *owner

    指定驱动模块的所有者。USB核使用它正确地对该驱动引用计数，以便不会在不恰当的时刻卸载掉驱动程序。使用宏`THIS_MODULE`进行设置。

* const char *name

    驱动的名称。内核中所有的USB驱动名称必须是唯一的，常常设置为和模块名称相同。在`sysfs`和`/sys/bus/usb/drivers/`中能够看到。

* const struct usb_device_id *id_table

    设备支持列表。如果没设，USB驱动程序中的`probe`回调函数不会被调用。 如果你想你的驱动总是被系统中的每一个USB设备调用，创建一个只设置`driver_info`字段的项:

        static struct usb_device_id usb_ids[ ] = {
            {.driver_info = 42},
            { }
        };

* int (*probe) (struct usb_interface *intf, const struct usb_device_id *id)

    'probe'函数。详细描述可以参考[probe和disconnect的细节](#13.4.2.1)。当USB核认为它有该驱动程序处理的结构体`struct usb_interface`时，就会调用该函数。USB核作出决定而使用的结构体`struct usb_device_id`也会被传递给该函数。`intf`是传递给驱动程序的接口，类型是`struct usb_interface`，如果USB驱动也声明了这个接口，该函数就会正确初始化设备，并且返回0.如果驱动程序不想声明设备，或发生错误，则返回负值作为错误码。

* void (*disconnect) (struct usb_interface *intf)

    设备驱动中的`disconnect`函数。当结构体`struct usb_interface`被从系统中移除或当驱动被从USB核中卸载的时候， USB核调用该函数。 详细描述可以参考[probe和disconnect的细节](#13.4.2.1)。

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

为了使用USB核注册结构体`struct usb_driver`，调用函数`usb_register_driver`,参数就是类型为`struct usb_driver`的结构体指针。 下面是USB设备典型的初始化代码：

    static int __init usb_skel_init(void)
    {
        int result;

        /* 使用USB核注册驱动 */
        result = usb_register(&skel_driver);
        if (result)
            err("usb_register failed. Error number %d", result);

        return result;
    }

卸载USB驱动时，解除结构体`struct usb_driver`的注册。方法就是调用`usb_deregister_driver`函数，任何绑定到该驱动的USB接口都会被断开，并且`disconnect`函数会被调用。

    static void __exit usb_skel_exit(void)
    {
        /* 解除USB驱动的注册 */
        usb_deregister(&skel_driver);
    }

<h4 id="13.4.2.1">13.4.2.1 probe和disconnect的细节</h4>

当设备被安装，USB核认为驱动和设备对应时，就会调用`probe`函数；`probe`函数检查传递给它的设备信息， 判断驱动程序是否和设备真正匹配。 在驱动卸载的时候，调用`disconnect`函数，负责清除工作。


`probe`和`disconnect`函数在USB集线器（hub）内核线程的上下文中调用，所以是允许休眠的。但是，推荐尽可能地在设备打开时由用户完成大部分工作，这是为了保证USB匹配时间尽可能的短。这是因为USB核在一个单线程中处理设备的添加和移除，任何用时较长的驱动程序都能造成USB设备的检测时间变长，在用户看来就非常显著了。

在`probe`回调函数里，USB驱动应该初始化用于管理USB设备的局部结构体变量。还应该保存需要的设备信息到局部变量中。比如，为了和设备进行通信需要直到端点地址和缓冲区大小。下面就是一段示例代码，它检测BULK类型的IN和OUT端点，并将这些信息保存到本地。

    /* 设置端点信息 */
    /* 只使用第一个bulk-in 和 bulk-out 端点 */
    iface_desc = interface->cur_altsetting;
    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
    {
        endpoint = &iface_desc->endpoint[i].desc;

        if (!dev->bulk_in_endpointAddr &&
            (endpoint->bEndpointAddress & USB_DIR_IN) &&
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK))
        {
            /* 发现一个bulk-in端点*/
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
            /* 发现一个bulk-out端点 */
            dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
        }
    }

    if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr))
    {
        err("Could not find both bulk-in and bulk-out endpoints");
        goto error;
    }

首先，这段代码遍历此接口中的每个端点，并赋予一个局部指针，便于后面更好的访问：

    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;

然后，查看方向是不是IN；端点类型是不是BULK类型：

    if (!dev->bulk_in_endpointAddr &&
        (endpoint->bEndpointAddress & USB_DIR_IN) &&
        ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK))

如果上面的条件成立，就认为找到了一个合适的端点，然后保存这些信息：

    /* 发现一个bulk-in端点 */
    buffer_size = endpoint->wMaxPacketSize;
    dev->bulk_in_size = buffer_size;
    dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
    dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
    if (!dev->bulk_in_buffer)
    {
        err("Could not allocate bulk_in_buffer");
        goto error;
    }

因为，USB驱动程序需要在设备的生命周期中还要取回与这个结构`usb_interface`相关联的局部数据结构。 所以， 可以调用函数`usb_set_intfdata`先将其保存起来：

    /* 保存这个接口设备中的数据指针 *
    usb_set_intfdata(interface, dev);

此函数接受一个指向任何数据类型的指针，并将其保存在`struct usb_interface` 结构中以供以后的访问。 要取回数据，应调用函数 `usb_get_intfdata` ：

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

`usb_get_intfdata`通常会在USB驱动中的`open`函数和`disconnect`函数中调用。有了`usb_get_intfdata`和`usb_set_intfdata`这2个函数， USB驱动无需使用静态数组，保存所有当前设备各自的设备信息。 对设备信息的间接引用允许任何USB驱动程序支持无限数量的设备。

如果USB驱动程序与处理用户与设备交互的其它类型的子系统（例如输入，tty，视频等）没有关联，则驱动程序可以使用USB主设备号以便使用传统的字符驱动程序与用户空间交互。 为此，当USB驱动程序想要使用USB核注册设备时，必须在`probe`函数中调用`usb_register_dev`函数注册设备。 请确保设备和驱动程序处于正确状态，以便在调用此函数后，立即处理想要访问设备的用户。

    /* 现在我们就可以注册这个设备了 */
    retval = usb_register_dev(interface, &skel_class);
    if (retval) {
        /* 注册设备失败 */
        err("Not able to get a minor for this device.");
        usb_set_intfdata(interface, NULL);
        goto error;
    }

`usb_register_dev`函数需要一个指向`struct usb_interface`的指针和一个指向`struct usb_class_driver`的指针。 这个`struct usb_class_driver`用于定义USB驱动程序在注册次要设备号时希望USB核知道的许多不同参数。 该结构由以下变量组成：

* char **name*

    `sysfs`用来描述设备的名称。 如果`name`字符串里存在路径名称，其路径名称只在`devfs`中使用，（`devfs`暂时不介绍）。如果设备号想要出现在`name`字符串中，使用`%d`即可。例如，想要在`devfs`下创建名称`usb/foo1`，在`sysfs`下创建`foo1`，那么，`name`字符串应该被设置为`usb/foo%d`。

* struct file_operations **fops*;

    指向`struct file_operations`的指针,驱动程序定义好用于注册为字符设备所使用的结构体。 有关此结构的更多信息， 请参见第3章。

* mode_t *mode*;

    为此驱动程序创建`devfs`文件的模式; 不创建该文件的话不用。 此变量的典型设置是`S_IRUSR`与`S_IWUSR`结合使用，该值仅提供设备文件所有者的读写访问权限。

* int *minor_base*;

    为该驱动指定的次要设备号范围的开始。所有与该驱动相关的设备，都会使用该值的递增，创建一个唯一的设备号。除非`CONFIG_USB_DYNAMIC_MINORS`配置选项被使能，否则一次只允许`16`个设备。如果`CONFIG_USB_DYNAMIC_MINORS`配置选项被使能，这个变量被忽略，所有设备的次要设备号以“先到先得”的方式进行分配。 建议启用此选项的系统， 使用`udev`等程序来管理系统中的设备节点， 因为静态`/dev`树将无法正常工作。

当USB设备断开时，所有与其相关的资源都应该被释放。与`usb_register_dev`函数配套使用的`usb_deregister_dev`函数会被调用，回收次要设备号给USB核。

在`disconnect`函数中，从接口检索先前通过调用`usb_set_intfdata`设置的数据也很重要。 然后将`struct usb_interface`结构中的数据指针设置为`NULL`，以防止在不正确地访问数据时出现任何进一步的错误：

    static void skel_disconnect(struct usb_interface *interface)
    {
        struct usb_skel *dev;
        int minor = interface->minor;

        /* 防止 skel_open() 和 skel_disconnect() 发生竞争 */
        lock_kernel();

        dev = usb_get_intfdata(interface);
        usb_set_intfdata(interface, NULL);

        /* 归还次要设备号-minor */
        usb_deregister_dev(interface, &skel_class);

        unlock_kernel();

        /* 减少引用计数 */
        kref_put(&dev->kref, skel_delete);
        info("USB Skeleton #%d now disconnected", minor);
    }

请注意该代码段中对`lock_kernel`的调用。 这将获得大内核锁，因此当尝试获取指向正确接口数据结构的指针时，`disconnect`回调函数不会与`open`调用发生竞争，此时，`open`调用可能也正在尝试获得正确的接口数据指针。 因为使用大内核锁调用`open`时，如果`disconnect`也采用相同的锁，只有驱动程序的一部分可以访问，设置接口数据指针。

就在为USB设备调用`disconnect`函数之前， USB核会取消当前正在为设备传输的所有`urb`， 因此驱动程序不必为这些`urb`显式调用`usb_kill_urb`。 如果驱动程序在断开连接后, 尝试调用`usb_submit_urb`向USB设备提交`urb`，则提交将失败，错误值为`-EPIPE`。

<h3 id="13.4.3">13.4.3 提交和控制一个urb</h3>

当驱动程序有数据要发送到USB设备时（通常发生在驱动程序的`write`函数中），必须分配一个`urb`来将数据传输到设备：

    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb) {
        retval = -ENOMEM;
        goto error;
    }

成功分配`urb`后，还应创建DMA缓冲区以最有效的方式将数据发送到设备，并将传递给驱动程序的数据复制到该缓冲区中：

    buf = usb_buffer_alloc(dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
    if (!buf) {
        retval = -ENOMEM;
        goto error;
    }
    if (copy_from_user(buf, user_buffer, count)) {
        retval = -EFAULT;
        goto error;
    }

将数据从用户空间正确拷贝到局部变量`buf`中后，`urb`必须在提交给USB核之前被正确地初始化：

    /* 初始化 urb */
    usb_fill_bulk_urb(urb, dev->udev,
                    usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                    buf, count, skel_write_bulk_callback, dev);
    urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

现在，已经完成了准备工作，就可以提交给USB核，由其转发给USB设备了。

    /* 发送数据到 bulk 端口 */
    retval = usb_submit_urb(urb, GFP_KERNEL);
    if (retval) {
        err("%s - failed submitting write urb, error %d", __FUNCTION__, retval);
        goto error;
    }

在`urb`成功传输到USB设备（或传输中发生的事情）之后，USB核调用`urb`回调函数。 在我们的示例中， 我们使用函数`skel_write_bulk_callback`初始化该回调函数指针：

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

        /* 释放分配的缓存 */
        usb_buffer_free(urb->dev, urb->transfer_buffer_length,
                        urb->transfer_buffer, urb->transfer_dma);
    }

这个函数做的第一件事情就是检查`urb`的状态，判断传输是否成功。错误值，`-ENOENT`，`-ECONNRESET`，和`-ESHUTDOWN`不是真正意义的传输错误，只是报告了传输成功过程中伴随着的一些条件。具体详细的意义可以参考[结构体struct urb](#13.3.1)。

然后就释放了分配给`urb`的缓冲区。

当回调函数运行时，提交另一个`urb`给设备是很常见的事情。尤其是对流设备很有用。切记，`urb`回调函数运行在中断上下文里时， 不应该申请内存分配、使用互斥量、或做任何导致进程休眠的事情。当在一个回调函数里提交`urb`时，如果需要申请分配新的内存块， 要使用标志`GFP_ATOMIC`告诉USB核不要休眠。

<h2 id="13.5">13.5 无urb的USB传送</h2>

有时USB驱动程序不想经历创建结构`urb`，初始化它，然后等待`urb`完成时调用服务函数的执行等所有麻烦， 只是为了发送或接收一些简单的USB数据。 有2个函数可以提供这样简单的接口。

<h3 id="13.5.1">13.5.1 usb_bulk_msg接口</h3>

`usb_bulk_msg`接口创建一个USB bulk型`urb`且发送给指定设备，等待，直到完成。定义如下：

    int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe,
                    void *data, int len, int *actual_length,
                    int timeout);

参数说明：

* struct usb_device **usb_dev*

    指向要发送批量消息的目标USB设备的指针。

* unsigned int *pipe*

    目标USB设备的具体端点。使用函数`usb_sndbulkpipe` 或 `usb_rcvbulkpipe`创建。

* void **data*

    收发数据存储的缓冲区指针。

* int *len*

    `data`指向缓冲区的大小。

* int **actual_length*

    指向收发数据的真实字节数的指针。

* int *timeout*

    等待时间，单位是jiffies。如果设为0，则永远等待，直到消息发送完成。

如果函数调用成功，则返回0；否则，返回负数。这里的错误码与[结构体struct urb](#13.3.1)中描述的错误码一致。如果成功，`actual_length`包含实际收发的消息的自己数。

下面是这个函数调用的示例：

    /* 执行阻塞批量读取以从设备获取数据 */
    retval = usb_bulk_msg(dev->udev,
                        usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
                        dev->bulk_in_buffer,
                        min(dev->bulk_in_size, count),
                        &count, HZ*10);

    /* 如果读取成功，复制到用户空间 */
    if (!retval) {
        if (copy_to_user(buffer, dev->bulk_in_buffer, count))
            retval = -EFAULT;
        else
            retval = count;
    }

这个示例展示了从IN端点批量读取数据的处理。如果读取成功，数据就会被拷贝到用户空间。这通常是一个USB驱动在`read`函数的实现。

不能在中断上下文或自旋锁中调用这个函数`usb_bulk_msg`。另外， 这个函数也不能被其它函数要取消， 所以使用时要小心； 确保在允许驱动被卸载前， `disconnect`函数能够等待足够长时间直到`usb_bulk_msg`的调用完成。

<h3 id="13.5.2">13.5.2 usb_control_msg接口</h3>

函数 `usb_control_msg`工作方式与`usb_bulk_msg`一样，除了消息类型是CONTROL类型以外。

    int usb_control_msg(struct usb_device *dev, unsigned int pipe,
                        __u8 request, __u8 requesttype,
                        __u16 value, __u16 index,
                        void *data, __u16 size, int timeout);

参数说明：

* struct usb_device **dev*

    指向目标USB设备。

* unsigned int *pipe*

    目标USB设备的具体端点。使用函数`usb_sndbulkpipe` 或 `usb_rcvbulkpipe`创建。

* __u8 *request*

    控制消息的请求值。

* __u8 *requesttype*

    请求类型。

* __u16 *value*

    USB消息值。

* __u16 *index*

    USB消息索引值。

* void **data*

    指向收发的数据。

* __u16 *size*

    `data`指针指向的缓冲区的大小。

* int *timeout*

    等待时间，单位是jiffies。如果设为0，则永远等待，直到消息发送完成。

如果函数成功，则返回收发的字节数。如果失败，返回负值作为错误码。

参数`request`，`requesttype`，`value`，和`index`直接映射到USB规范中控制消息的定义上。

函数 `usb_control_msg`与`usb_bulk_msg`一样，也不能从中断上下文和自旋锁中调用。也不能被其它函数取消， 所以在使用时要格外小心。确保在允许驱动被卸载前， `disconnect`函数能够等待足够长时间直到`usb_control_msg`的调用完成。


<h3 id="13.5.3">13.5.3 其它的USB数据函数</h3>

USB核中的许多辅助功能可用于从USB设备检索标准信息。 无法从中断上下文或自旋锁中调用这些函数。

函数`usb_get_descriptor`从指定的设备上检索指定的USB描述符。函数定义如下：

    int usb_get_descriptor(struct usb_device *dev, unsigned char type,
                            unsigned char index, void *buf, int size);

USB驱动程序可以使用此函数从`struct usb_device`结构中检索现有`struct usb_device`和`struct usb_interface`结构中尚未存在的任何设备描述符，例如音频描述符或其他类特定信息。 该函数的参数是：

* struct usb_device **usb_dev*

    指向目标USB设备。

* unsigned char *type*

    描述符类型。在USB规约中定义。

    * USB_DT_DEVICE
    * USB_DT_CONFIG
    * USB_DT_STRING
    * USB_DT_INTERFACE
    * USB_DT_ENDPOINT
    * USB_DT_DEVICE_QUALIFIER
    * USB_DT_OTHER_SPEED_CONFIG
    * USB_DT_INTERFACE_POWER
    * USB_DT_OTG
    * USB_DT_DEBUG
    * USB_DT_INTERFACE_ASSOCIATION
    * USB_DT_CS_DEVICE
    * USB_DT_CS_CONFIG
    * USB_DT_CS_STRING
    * USB_DT_CS_INTERFACE
    * USB_DT_CS_ENDPOINT

* unsigned char *index*

    从设备中检索的描述符的编号。

* void **buf*

    指向要将描述符存储的地方。

* int *size*

    `buf`变量执行的缓冲区的大小。

如果这个函数成功，返回从设备上读取的字节数。否则，返回负数。内部调用`usb_control_msg`函数。

`usb_get_descriptor`函数的一个常见用途是从USB设备检索字符串。 因为这很常见，所以特地封装了`usb_get_string`的辅助函数：

    int usb_get_string(struct usb_device *dev, unsigned short langid,
                        unsigned char index, void *buf, int size);

作用跟`usb_get_descriptor`一样。

如果这个函数成功，则返回`UTF-16LE`格式编码的字符串，存储在`buf`指向的缓冲区内。但是，这种编码格式并不常用，所以，推荐使用函数`usb_string`，它返回的字符串，编码格式是`ISO 8859-1`,8位编码，这更符合英语或者其他西语系的编码格式。
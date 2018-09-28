* [1 简介](#1)
    - [1.1 特点](#1.1)
* [2 QEMU-PC系统仿真器](#1)
    - [2.1 介绍](#2.1)
    - [2.2 启动](#2.2)
        + [2.2.1 标准选项](#2.2.1)
        + [2.2.2 块设备选项](#2.2.2)
        + [2.2.3 USB选项](#2.2.3)
        + [2.2.4 显示选项](#2.2.4)
    - [2.7 磁盘镜像]（#2.7）
    -
    - [2.11 USB仿真]（#2.11）

<h1 id="1">1 简介</h1>

<h2 id="1.1">1.1 特点</h2>

QEMU是一个快速的处理器的模拟器，采用动态转换，以达到更好的仿真速度。

QEMU两个工作模式：

* 全系统模拟。

    在此工作模式，QEMU模拟一个完整的系统（例如，PC），包括一个处理器或者多个处理器，或者多个外设。它可以被用来运行不同的操作系统，而不用重启PC或者调试系统代码。

* 用户模拟。

    QEMU可以启动在一个CPU上编译另一个CPU。可以被用来启动Wine Windows API模拟器或者简化交叉编译或交叉调试。

QEMU有下面的特点：

* QEMU可以在没有host内核驱动的情况下运行，仍然能够获得可以接受的性能。它对原生代码使用动态转译以提供合理的速度。
* 它适用于多种操作系统（GNU/Linux，*BSD，Mac OS X， Windows）和体系结构。
* 执行FPU的精确软件仿真。

QEMU用户模式仿真具有如下特点：

* 通用Linux系统调用转换器，包括大部分ioctls。
* 通过使用原生CPU的clone()，克隆模拟器，为线程使用linux调度器。
* 通过将主机信号重新映射到目标信号来实现精确信号处理。

QEMU完整系统仿真具有如下特点：

* QEMU 使用一个全功能的软件MMU，以最大化可移植性。
* QEMU 可选用内核加速器，像`kvm`。加速器原生态般执行客户端代码，仿真器的其余部分模拟执行。
* 可以模拟各种硬件设备，并且在一些情况下，客户端操作系统可以透明地使用主机端设备（例如，，串行和并行端口，USB，磁盘驱动器等）。主机设备透传功能可以用来直接访问外部设备（例如网络摄像头，调制解调器或磁带驱动器）。
* 对称多处理器支持(SMP)。目前，内核加速器需要使用多个主机CPU进行仿真。

<h1 id="2">2 QEMU-PC系统仿真器</h1>

<h2 id="2.1">2.1 简介</h2>

PC仿真器可以模拟下面这些外设：

* i440FX host PCI bridge and PIIX3 PCI to ISA bridge
* Cirrus CLGD 5446 PCI VGA card or dummy VGA card with Bochs VESA extensions (硬件层，包含所有非标准模式)
* PS/2 鼠标和键盘
* 2个PCI IDE接口的硬盘和CD-ROM
* 软磁盘
* PCI 和 ISA网络适配器
* 串口
* IPMI BMC, either and internal or external one
* Creative SoundBlaster 16 sound card
* ENSONIQ AudioPCI ES1370 sound card
* Intel 82801AA AC97 Audio compatible sound card
* Intel HD Audio Controller and HDA codec
* Adlib (OPL2) - Yamaha YM3812 compatible chip
* Gravis Ultrasound GF1 sound card
* CS4231A兼容声卡
* PCI UHCI, OHCI, EHCI 或 XHCI USB 控制器和一个虚拟USB-1.1 hub

SMP支持255个CPU。

QEMU uses the PC BIOS from the Seabios project and the Plex86/Bochs LGPL VGA BIOS.

QEMU uses YM3812 emulation by Tatsuyuki Satoh.

QEMU uses GUS emulation ([GUSEMU32](http://www.deinmeister.de/gusemu/)) by Tibor "TS" Schütz.

Note that, by default, GUS shares IRQ(7) with parallel ports and so QEMU must be told to not have parallel ports to have working GUS.

    qemu-system-i386 dos.img -soundhw gus -parallel none

Alternatively:

    qemu-system-i386 dos.img -device gus,irq=5

Or some other unclaimed IRQ.

CS4231A is the chip used in Windows Sound System and GUSMAX products

<h2 id="2.2">2.2 启动</h2>

启动命令如下：

    qemu-system-i386 [options] [disk_image]

`disk_image`是IDE硬盘0的原始硬盘镜像文件。某些目标不需要磁盘镜像文件。

<h3 id="2.2.1">2.2.1 标准选项</h3>

* -h
* -version
* machine [type=]name[,prop=value[,...]]
* -cpu model
* -accel name[,prop=value[,...]]
* -smp [cpus=]n[,cores=cores][,threads=threads][,sockets=sockets][,maxcpus=maxcpus]
* -numa node[,mem=size][,cpus=firstcpu[-lastcpu]][,nodeid=node]
* -numa node[,memdev=id][,cpus=firstcpu[-lastcpu]][,nodeid=node]
* -numa dist,src=source,dst=destination,val=distance
* -numa cpu,node-id=node[,socket-id=x][,core-id=y][,thread-id=z]
* -add-fd fd=fd,set=set[,opaque=opaque]
* -set group.id.arg=value
* -global driver.prop=value
* -global driver=driver,property=property,value=value
* -boot [order=drives][,once=drives][,menu=on|off][,splash=sp_name][,splash-time=sp_time][,reboot-timeout=rb_timeout][,strict=on|off]
* -m [size=]megs[,slots=n,maxmem=size]
* -mem-path path
* -mem-prealloc
* -k language
* -audio-help
* -soundhw card1[,card2,...] or -soundhw all
* -balloon virtio[,addr=addr]
* -device driver[,prop[=value][,...]]
* -device ipmi-bmc-sim,id=id[,slave_addr=val][,sdrfile=file][,furareasize=val][,furdatafile=file]
* -device ipmi-bmc-extern,id=id,chardev=id[,slave_addr=val]
* -name name

<h3 id="2.2.2">2.2.2 块设备选项</h3>

* -fda *file*
* -fdb *file*

    使用软盘0/1镜像文件（查看[硬盘镜像]）

* -hda *file*
* -hdb *file*
* -hdc *file*
* -hdd *file*

    使用硬盘0,1,2,3镜像文件（查看[硬盘镜像]）

* -mtdblock *file*

    使用板载Flash内存镜像文件

* -sd `file`

    使用SD卡镜像文件

<h3 id="2.2.3">2.2.3 USB选项</h3>

* -usb

    使能USB驱动(如果尚未默认使用)。

* -usbdevice `devname`

    添加USB设备。**注意**：该选项已经被废弃，请使用`-device usb-`查看[usb_devices]。

* mouse

    虚拟鼠标。 这将在激活时覆盖PS/2鼠标仿真。

* tablet

    使用绝对坐标的指针设备（如触摸屏）。 这意味着QEMU能够报告鼠标位置而无需抓住鼠标。 激活时也会覆盖PS/2鼠标仿真。

* braille

    盲文设备。 这将使用BrlAPI在真实或虚拟设备上显示盲文输出。

<h3 id="2.2.4">2.2.4 显示选项</h3>

* -display `type`

    选择显示类型。支持sdl，curses，none， gtk，vnc

* -nographic

    Normally, if QEMU is compiled with graphical window support, it displays output such as guest graphics, guest console, and the QEMU monitor in a window. With this option, you can totally disable graphical output so that QEMU is a simple command line application. The emulated serial port is redirected on the console and muxed with the monitor (unless redirected elsewhere explicitly). Therefore, you can still use QEMU to debug a Linux kernel with a serial console. Use C-a h for help on switching between the console and monitor.

<h2 id="2.7">2.7 磁盘镜像</h2>

QEMU支持多磁盘镜像文件格式，包括可增长的磁盘镜像（它们的大小随着非空扇区的写入而增加），压缩和加密的磁盘镜像。

<h3 id="2.7.1">2.7.1 快速创建磁盘镜像</h3>

创建磁盘镜像命令：

    qemu-img create myimage.img mysize

在这儿，`myimage.img`是镜像文件名称，`mysize`是镜像大小，单位kB。也可以在后面添加`M`后缀，表示单位是MB；还可以添加`G`后缀，表示单位是GB。

<h2 id="2.11">2.11 USB仿真</h2>

QEMU可以模拟PCI UHCI，OHCI，EHCI或XHCI USB控制器。可以插入虚拟USB设备或真正的主机USB设备（仅适用于某些主机操作系统）。 QEMU将根据需要自动创建和连接虚拟USB集线器，以连接多个USB设备。

<h3 id="2.11.1">2.11.1 连接USB设备</h3>

使用命令行参数： `-device usb-...`。或者，使用monitor命令： `device_add`。可用的设备有：

* usb-mouse

    虚拟鼠标。会覆盖掉PS/2鼠标仿真。

* usb-tablet

    触摸屏时使用的选项。会覆盖掉PS/2鼠标仿真。

* usb-storage,drive=`drive_id`

    大容量存储设备

* usb-uas

    USB连接的SCSI设备， see usb-storage.txt for details

* usb-bot

    Bulk-only transport storage device, see usb-storage.txt for details here, too

* usb-mtp,x-root=dir
* usb-host,hostbus=bus,hostaddr=addr
* usb-host,vendorid=vendor,productid=product
* usb-wacom-tablet
* usb-kbd
* usb-serial,chardev=id
* usb-braille,chardev=id
* usb-net[,netdev=id]
* usb-ccid
* usb-audio
* usb-bt-dongle



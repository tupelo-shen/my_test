* [1 简介](#1)
    - [1.1 特点](#1.1)
* [2 QEMU-PC系统模拟器](#1)
    - [2.1 介绍](#2.1)
    - [2.2 启动](#2.2)
        + [2.2.1 标准选项](#2.2.1)
        + [2.2.2 块设备选项](#2.2.2)
        + [2.2.3 USB选项](#2.2.3)
        + [2.2.4 显示选项](#2.2.4)
    - [2.6 QEMU Monitor](#2.6)
        + [2.6.1 常用命令](#2.6.1)
    - [2.7 磁盘镜像](#2.7)
    - [2.11 USB模拟](#2.11)
        + [2.11.1 连接USB设备](#2.11.1)
        + [2.11.2 在Linux主机上使用主USB设备](#2.11.2)
* [3 非PC系统模拟器](#3)
    - [3.5 ARM系统模拟器](#3.5)


<h1 id="1">1 简介</h1>

<h2 id="1.1">1.1 特点</h2>

QEMU是一个快速的处理器的模拟器，采用动态转换，以达到更好的模拟速度。

QEMU两个工作模式：

* 全系统模拟。

    在此工作模式，QEMU模拟一个完整的系统（例如，PC），包括一个处理器或者多个处理器，或者多个外设。它可以被用来运行不同的操作系统，而不用重启PC或者调试系统代码。

* 用户模拟。

    QEMU可以启动在一个CPU上编译另一个CPU。可以被用来启动Wine Windows API模拟器或者简化交叉编译或交叉调试。

QEMU有下面的特点：

* QEMU可以在没有host内核驱动的情况下运行，仍然能够获得可以接受的性能。它对原生代码使用动态转译以提供合理的速度。
* 它适用于多种操作系统（GNU/Linux，*BSD，Mac OS X， Windows）和体系结构。
* 执行FPU的精确软件模拟。

QEMU用户模式模拟具有如下特点：

* 通用Linux系统调用转换器，包括大部分ioctls。
* 通过使用原生CPU的clone()，克隆模拟器，为线程使用linux调度器。
* 通过将主机信号重新映射到目标信号来实现精确信号处理。

QEMU完整系统模拟具有如下特点：

* QEMU 使用一个全功能的软件MMU，以最大化可移植性。
* QEMU 可选用内核加速器，像`kvm`。加速器原生态般执行客户端代码，模拟器的其余部分模拟执行。
* 可以模拟各种硬件设备，并且在一些情况下，客户端操作系统可以透明地使用主机端设备（例如，，串行和并行端口，USB，磁盘驱动器等）。主机设备透传功能可以用来直接访问外部设备（例如网络摄像头，调制解调器或磁带驱动器）。
* 对称多处理器支持(SMP)。目前，内核加速器需要使用多个主机CPU进行模拟。

<h1 id="2">2 QEMU-PC系统模拟器</h1>

<h2 id="2.1">2.1 简介</h2>

PC模拟器可以模拟下面这些外设：

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

    虚拟鼠标。 这将在激活时覆盖PS/2鼠标模拟。

* tablet

    使用绝对坐标的指针设备（如触摸屏）。 这意味着QEMU能够报告鼠标位置而无需抓住鼠标。 激活时也会覆盖PS/2鼠标模拟。

* braille

    盲文设备。 这将使用BrlAPI在真实或虚拟设备上显示盲文输出。

<h3 id="2.2.4">2.2.4 显示选项</h3>

* -display `type`

    选择显示类型。支持sdl，curses，none， gtk，vnc

* -nographic

    Normally, if QEMU is compiled with graphical window support, it displays output such as guest graphics, guest console, and the QEMU monitor in a window. With this option, you can totally disable graphical output so that QEMU is a simple command line application. The emulated serial port is redirected on the console and muxed with the monitor (unless redirected elsewhere explicitly). Therefore, you can still use QEMU to debug a Linux kernel with a serial console. Use C-a h for help on switching between the console and monitor.

<h2 id="2.6">2.6 QEMU Monitor</h2>

`monitor` 是用来给QEMU模拟器发送复杂命令的。你可以用它:

* 移除或添加可移动媒介（CD-ROM或软盘）
* 冻结/解冻虚拟机（VM）并从磁盘文件保存或恢复其状态
* 在没有外部调试器的情况下检查VM状态

<h3 id="2.6.1">2.6.1 常用命令</h3>

* help 或 ? [cmd]

    显示所有命令的帮助信息或命令*cmd*的帮助信息。

* commit

* q 或 quit

    退出模拟器

* gdbserver [port]

    启动gdbserver（默认端口是1234）。

* system_reset

    重启系统。

* info subcommand

    显示系统状态的各种信息，比如info usb，显示所有的USB设备信息。

<h2 id="2.7">2.7 磁盘镜像</h2>

QEMU支持多磁盘镜像文件格式，包括可增长的磁盘镜像（它们的大小随着非空扇区的写入而增加），压缩和加密的磁盘镜像。

<h3 id="2.7.1">2.7.1 快速创建磁盘镜像</h3>

创建磁盘镜像命令：

    qemu-img create myimage.img mysize

在这儿，`myimage.img`是镜像文件名称，`mysize`是镜像大小，单位kB。也可以在后面添加`M`后缀，表示单位是MB；还可以添加`G`后缀，表示单位是GB。

<h2 id="2.11">2.11 USB模拟</h2>

QEMU可以模拟PCI UHCI，OHCI，EHCI或XHCI USB控制器。可以插入虚拟USB设备或真正的主机USB设备（仅适用于某些主机操作系统）。 QEMU将根据需要自动创建和连接虚拟USB集线器，以连接多个USB设备。

<h3 id="2.11.1">2.11.1 连接USB设备</h3>

使用命令行参数： `-device usb-...`。或者，使用monitor命令： `device_add`。可用的设备有：

* usb-mouse

    虚拟鼠标。会覆盖掉PS/2鼠标模拟。

* usb-tablet

    触摸屏时使用的选项。会覆盖掉PS/2鼠标模拟。

* usb-storage,drive=`drive_id`

    大容量存储设备

* usb-uas

    USB SCSI设备。详细查看[usb-storage.md](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/QEMU%E6%96%87%E6%A1%A3/usb-storage.md)

* usb-bot

    批量传输协议USB设备。详细查看[usb-storage.md](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/QEMU%E6%96%87%E6%A1%A3/usb-storage.md)

* usb-mtp,x-root=dir


* usb-host,hostbus=bus,hostaddr=addr

    通过总线和地址直接识别的主机设备

* usb-host,vendorid=vendor,productid=product

    通过设备厂商和产品ID直接识别的主机设备

* usb-wacom-tablet

* usb-kbd

    标准USB键盘。将会覆盖PS/2键盘模拟器（如果存在的话）。

* usb-serial,chardev=id

    USB转串口设备。模拟了FTDI FT232BM 芯片。

* usb-braille,chardev=id
* usb-net[,netdev=id]
* usb-ccid
* usb-audio
* usb-bt-dongle

<h3 id="2.11.2">2.11.2 在Linux主机上使用主USB设备</h3>

**警告**：

这是一项实验性功能。 QEMU在使用时会变慢。 尚不支持需要实时流式传输的USB设备（即USB视频摄像机）。

1. 如果使用早期的Linux 2.4内核，请确认没有Linux驱动正在使用USB设备。简单的方法就是禁掉相应的内核模块，重命名`mydriver.o` 为 `mydriver.o.disabled`。

2. Verify that `/proc/bus/usb` is working (most Linux distributions should enable it by default). You should see something like that:
3. 确认`/proc/bus/usb`是否正在工作（大部分Linux发布版本默认是使能的）。你会看到如下内容：

        ls /proc/bus/usb
        001  devices  drivers

3. 因为只有root才能直接访问USB设备，所以你可以使用root账户启动QEMU或者改变你要使用的USB设备的权限。

        chown -R myuid /proc/bus/usb

4. Launch QEMU and do in the monitor:

        info usbhost
            Device 1.2, speed 480 Mb/s
                Class 00: USB device 1234:5678, USB DISK

    You should see the list of the devices you can use (Never try to use hubs, it won’t work).

5. 在QEMU中添加设备，使用下面的方法：

    device_add usb-host,vendorid=0x1234,productid=0x5678

    正常情况下，客户端OS会报告有新的USB设备插入。也可以使用选项`-device usb-host,...`做相同的工作。

6. 现在就可以在QEMU中使用主USB设备了。

重新启动QEMU时，可能需要先拔下再插入USB设备，才能再次运行（这是一个bug）。

<h2 id="3">3 非PC系统模拟器</h2>

<h2 id="3.5">3.5 ARM系统模拟器</h2>

执行可执行文件`qemu-system-arm `运行ARM模拟器。

1. ARM Integrator/CP板使用以下设备进行模拟：

    * ARM926E, ARM1026E, ARM946E, ARM1136 或 Cortex-A8 CPU
    * 2个 PL011 UARTs
    * SMC 91c111 以太网适配器
    * PL110 LCD 控制器
    * PL050 KMI with PS/2 keyboard and mouse.
    * PL181 MultiMedia Card Interface with SD card.

2. ARM Versatile baseboard模拟的设备：

    * ARM926E, ARM1136 or Cortex-A8 CPU
    * PL190 Vectored Interrupt Controller
    * 4个PL011 UARTs
    + SMC 91c111 以太网适配器
    + PL110 LCD 控制器
    + PL050 KMI with PS/2 keyboard and mouse.
    + PCI host bridge. Note the emulated PCI bridge only provides access to PCI memory space. It does not provide access to PCI IO space. This means some devices (eg. ne2k_pci NIC) are not usable, and others (eg. rtl8139 NIC) are only usable when the guest drivers use the memory mapped control registers.
    + PCI OHCI USB 控制器。
    + LSI53C895A PCI SCSI Host Bus Adapter with hard disk and CD-ROM devices.
    + PL181 MultiMedia Card Interface with SD card.

Several variants of the ARM RealView baseboard are emulated, including the EB, PB-A8 and PBX-A9. Due to interactions with the bootloader, only certain Linux kernel configurations work out of the box on these boards.

Kernels for the PB-A8 board should have CONFIG_REALVIEW_HIGH_PHYS_OFFSET enabled in the kernel, and expect 512M RAM. Kernels for The PBX-A9 board should have CONFIG_SPARSEMEM enabled, CONFIG_REALVIEW_HIGH_PHYS_OFFSET disabled and expect 1024M RAM.

The following devices are emulated:

- ARM926E, ARM1136, ARM11MPCore, Cortex-A8 or Cortex-A9 MPCore CPU
- ARM AMBA Generic/Distributed Interrupt Controller
- Four PL011 UARTs
- SMC 91c111 or SMSC LAN9118 Ethernet adapter
- PL110 LCD controller
- PL050 KMI with PS/2 keyboard and mouse
- PCI host bridge
- PCI OHCI USB controller
- LSI53C895A PCI SCSI Host Bus Adapter with hard disk and CD-ROM devices
- PL181 MultiMedia Card Interface with SD card.

The XScale-based clamshell PDA models ("Spitz", "Akita", "Borzoi" and "Terrier") emulation includes the following peripherals:

- Intel PXA270 System-on-chip (ARM V5TE core)
- NAND Flash memory
- IBM/Hitachi DSCM microdrive in a PXA PCMCIA slot - not in "Akita"
- On-chip OHCI USB controller
- On-chip LCD controller
- On-chip Real Time Clock
- TI ADS7846 touchscreen controller on SSP bus
- Maxim MAX1111 analog-digital converter on I^2C bus
- GPIO-connected keyboard controller and LEDs
- Secure Digital card connected to PXA MMC/SD host
- Three on-chip UARTs
- WM8750 audio CODEC on I^2C and I^2S busses

The Palm Tungsten|E PDA (codename "Cheetah") emulation includes the following elements:

- Texas Instruments OMAP310 System-on-chip (ARM 925T core)
- ROM and RAM memories (ROM firmware image can be loaded with -option-rom)
- On-chip LCD controller
- On-chip Real Time Clock
- TI TSC2102i touchscreen controller / analog-digital converter / Audio CODEC, connected through MicroWire and I^2S busses
- GPIO-connected matrix keypad
- Secure Digital card connected to OMAP MMC/SD host
- Three on-chip UARTs

Nokia N800 and N810 internet tablets (known also as RX-34 and RX-44 / 48) emulation supports the following elements:

- Texas Instruments OMAP2420 System-on-chip (ARM 1136 core)
- RAM and non-volatile OneNAND Flash memories
- Display connected to EPSON remote framebuffer chip and OMAP on-chip display controller and a LS041y3 MIPI DBI-C controller
- TI TSC2301 (in N800) and TI TSC2005 (in N810) touchscreen controllers driven through SPI bus
- National Semiconductor LM8323-controlled qwerty keyboard driven through I^2C bus
- Secure Digital card connected to OMAP MMC/SD host
- Three OMAP on-chip UARTs and on-chip STI debugging console
- A Bluetooth(R) transceiver and HCI connected to an UART
- Mentor Graphics "Inventra" dual-role USB controller embedded in a TI TUSB6010 chip - only USB host mode is supported
- TI TMP105 temperature sensor driven through I^2C bus
- TI TWL92230C power management companion with an RTC on I^2C bus
- Nokia RETU and TAHVO multi-purpose chips with an RTC, connected through CBUS

The Luminary Micro Stellaris LM3S811EVB emulation includes the following devices:

- Cortex-M3 CPU core.
- 64k Flash and 8k SRAM.
- Timers, UARTs, ADC and I^2C interface.
- OSRAM Pictiva 96x16 OLED with SSD0303 controller on I^2C bus.

The Luminary Micro Stellaris LM3S6965EVB emulation includes the following devices:

- Cortex-M3 CPU core.
- 256k Flash and 64k SRAM.
- Timers, UARTs, ADC, I^2C and SSI interfaces.
- OSRAM Pictiva 128x64 OLED with SSD0323 controller connected via SSI.

The Freecom MusicPal internet radio emulation includes the following elements:

- Marvell MV88W8618 ARM core.
- 32 MB RAM, 256 KB SRAM, 8 MB flash.
- Up to 2 16550 UARTs
- MV88W8xx8 Ethernet controller
- MV88W8618 audio controller, WM8750 CODEC and mixer
- 128×64 display with brightness control
- 2 buttons, 2 navigation wheels with button function

The Siemens SX1 models v1 and v2 (default) basic emulation. The emulation includes the following elements:

- Texas Instruments OMAP310 System-on-chip (ARM 925T core)
- ROM and RAM memories (ROM firmware image can be loaded with -pflash) V1 1 Flash of 16MB and 1 Flash of 8MB V2 1 Flash of 32MB
- On-chip LCD controller
- On-chip Real Time Clock
- Secure Digital card connected to OMAP MMC/SD host
- Three on-chip UARTs



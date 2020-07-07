# 1 介绍

## 1.1 Virtuosity介绍

Virtuosity是运行在赛灵思公司Zynq+UltraScale+多处理器硬件平台上的Xen开源管理程序。XZD所有组件，除了裸机容器（BMC）之外，都是基于GPLv2协议发布的；BMC是基于FreeBSD许可协议发布的。具体的要求请参考各个协议。但是，要知道一点的是，这里所说的不包含运行在Xen创建的虚拟环境里的操作系统或者裸机程序。

### 1.1.1 Xen以及虚拟化介绍

Xen是type-1类型的管理程序。可以运行在X86或者ARM平台上，因为需要hypervisor硬件扩展的支持，目前ARM平台只有ARMv8架构的CPU全部支持，ARMv7架构只有部分支持。Xen运行在Intel处理器上已经有着广泛的历史了，为云服务提供支持。比如，Amazon的web服务就运行着50万左右的Xen实例。但是，对于ARM平台的支持开始于2008年，第一次发布是在2013年。对于64位ARMv8架构的多核系统的支持版本于2015年发布。所以，2016年被认为是嵌入式系统虚拟化的一个拐点，和20世纪90年代服务器市场上的发展趋势类似，系统工程师们想尽一切办法，充分发挥芯片的计算能力，将虚拟化引入到嵌入式系统中。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/Xen-Zynq/images/DornerWorks_1_1.PNG">

Hypervisor，又称为虚机监控器（VMM），是硬件之上的一个软件抽象层，用来创建和管理虚拟机的一个管理程序。这样的虚拟机可以运行不同的操作系统或者应用程序。

`type-1`类型的虚机管理程序，是直接运行在硬件系统之上的软件层，可以完整地控制硬件；而`type-2`类型的虚拟管理程序，是运行在一个主机操作系统之上的软件程序，比如说，VMWare或者VirtualBox，只能控制主机操作系统允许它操控的硬件。

Xen提供了在相同的计算平台上运行不同操作系统的能力。Xen为每一个客户机操作系统提供一个虚拟机，使得其好像直接操控硬件一样。Xen创建内存分区，使得每个虚机都有自己独立的RAM，而其它虚机无法访问，除非使用特殊权限。Xen还可以控制一个虚机可以什么时候使用CPU，使用多长时间以及使用几个CPU核，这样可以避免客户机浪费资源。Xen甚至可以控制中断，决定何时由哪个客户机处理它。

Xen，作为系统软件的最底层，需要类似于U-Boot这类的引导程序进行引导。一旦完成初始化，Xen立即启动第一个虚拟机，称为`dom0`。`dom0`作为Xen一个特殊的代理人的角色存在，负责管理和启动。通过`dom0`，系统开发者可以控制虚机，启动、暂停或终止其它虚机。默认情况下，`dom0`拥有访问所有系统资源的能力。其它的虚机通常被称为`domN`，N=1、2、3...。如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/Xen-Zynq/images/DornerWorks_1_2.PNG">

对于IO设备的交互，Xen也提供了多种不同的方法。后面我们再细讨论。

### 1.1.2 使用虚拟化的好处

在嵌入式系统上，使用虚拟化技术，有3个好处：

1. 减少

The primary benefit of using virtualization is that it can reduce the production cost of your product, both in nonrecurring engineering cost and unit cost, while also helping to reduce schedule. The main way virtualization lets you accomplish that is by allowing you to combine and consolidate different software components while still maintaining isolation between them. This feature of virtualization enables many use cases.
One such use case is reducing of size, weight, power, and cost (SWaP-C) by reducing part count. Thanks to Moore’s law, modern multi-core processors like the ZUS+ are processing powerhouses, often providing more computation power than needed for a single function. The ability to consolidate while maintaining isolation allows you to combine software that otherwise might have been deployed on multiple hardware systems or processors onto a single MPSoC chip. A single hardware platform is also easier to manage than a multi-platform system.

Consolidation with isolation can also be used to enforce greater decoupling of the software components. Coupling between software components leads to all kinds of problems with development, integration, maintenance, and future migrations. This is because coupling leads to dependencies, sometimes implicit or unknown, between the components such that a change or addition to one software unit often has a wide-reaching and unexpected ripple effect. Running different software functions in their own VMs leads to very strong decoupling where any dependencies between software functions are made explicit through configuration or I/O calls, making it easier to understand and eliminate unintended or unexpected interactions. Strong decoupling also allows greater freedom to develop the software functions in parallel with a higher confidence that the different pieces won’t interfere with one another during integration.

As an aside, this level of decoupling is critical in applications needing security or safety certification, as it is a requirement to show certification authorities that there are no unintended interactions. By restricting and reducing the amount of intended interaction with strict design decoupling and VM isolation, you can also reduce re-certification costs by being able to show how changes and additions are bounded to the context of a particular VM.

Even outside the realm of safety and security considerations, the ability to replace a software function with a compatible one without having to worry about side effects can result in significant savings. Likewise, you can rest easy knowing that adding software in a new VM won’t causing existing software functions to fail. It is also easier to re-use software components developed for one project on another, simply take the VM in which it runs and deploy it to run as a guest in a different system, allowing a mix’n’match approach with your existing software IP.

2. Enable New and Improved Features

The capabilities provided by Xen virtualization can also be used to enable new features and improve old ones. The isolation capability allows for enhanced security and safety, as it becomes possible to run functions in isolation, i.e. sandbox them, so that a breach or failure in one VM is limited to that VM alone. Not even security vulnerabilities in the VM’s OS would result in compromise of functions in another VM, providing defense in depth.

The capability to consolidate disparate software functions enables the implementation of a centralized monitoring and management function that operates externally to the software functions being monitored. This MM function could be used to detect and dynamically respond to breaches and faults, for example, restarting faulted VMs or terminating compromised VMs before the hacker could exploit it. A centralized monitoring function could also prove useful in embedded applications which have a greater emphasis on up-time. The monitoring function could detect or predict when a VM is faulting, or about to fault, and ready a backup VM to take over with minimal loss of service.

There are other use cases that are common in the server world, where VMs are managed algorithmically by other programs, being created, copied, migrated, or destroyed in response to predefined stimulus. Virtualization enables guest migration, where the entire software stack, or part of it, could be moved from one VM to another, potentially on another platform entirely. This could be an important enabler for self-healing systems. Migration can help with live system upgrades, where the system operator could patch the OS or service critical library in a backup copy of the VM then test the patched VM to validate correct operation before migrating the actively running application to the patched VM, again with a minimal loss of service. Another use case seen in the server market is the ability to perform load balancing, either by dynamically controlling the number of VMs running to meet the current demands, or by migrating VMs to a computing resource closer to where the processing is actually needed, reducing traffic on the network.

3. Reduce Program and Product Risk

Virtualization can be used to reduce program risk by providing means to reconcile contradictory requirements. The most obvious example being the case where two pre-existing applications are needed for a product, but where each were developed to run on a different RTOS. In this case the contradictory requirements are regarding the OS to use. Other examples including different safety or security levels, where isolation allows you to avoid having to develop all of your software to the highest level, or using software functions with different license agreements.

Long lived programs can also benefit from the ability to add new VMs to the system at a later date, creating a path for future upgrades. Likewise, in a system using VMs, it becomes easier to migrate to newer hardware, especially if the hardware supports backward compatibility, like the ARMv8 does for the ARMv7. Even if it isn’t, thanks to Moore’s law, newer processors will have even greater processing capabilities, and emulation can be used in an VM to provide the environment necessary to run legacy software.

Virtualization can also be used to reduce risk of system failure during runtime. Previously mentioned was dynamic load balancing, which can also be considered one way to reduce the risk of failure, but virtualization can also be used to easily provide redundancy to key functionality by running a second copy of the same VM. With the centralized monitoring also previously mentioned, the redundant VM can even be kept in a standby state, and only brought to an active state if data indicates a critical function is experiencing issues or otherwise about to fail.

## 1.2 Xilinx公司Zynq UltraScale+多处理器系统平台

对于赛思灵公司的硬件平台请参考[《https://www.xilinx.com/products/silicon-devices/soc/zynq-ultrascale-mpsoc.html》](https://www.xilinx.com/products/silicon-devices/soc/zynq-ultrascale-mpsoc.html)这个网址。

# 2 主机配置

## 2.1 镜像文件

镜像文件是一个压缩的tar文件。包含一个预先打包好的镜像，其允许工程师使用一组domain运行Xen管理程序。压缩镜像解压缩之后，将会是下面的目录结构：

    Virtuosity_2019
    ├── dist
    │   ├── images
    │   │   └── linux
    |   └── project-spec
    ├── docs
    ├── dts
    ├── fs
    ├── misc
    │   ├── examples
    │   │   └── baremetal
    │   └── xzd_bmc 
    |── sdk

## 2.2 配置主机OS

系统要求：x86_64版本的Ubuntu14.04或者16.04。另外，还需要保证主机具有至少40G的硬盘空间。

### 2.2.1 必要工具

需要git，Ubuntu的build-essential包中的几个工具，还有一些其它工具来完成这个构建过程。使用下面的命令进行安装：

    $ sudo apt-get install -y build-essential git mercurial dos2unix gawk tftpd-hpa flex bison unzip screen

如果需要对FPGA或其它板卡设备进行修改，需要安装Vivado。Vivado的安装不在本手册的范围之内。

如果计划从JTAG引导主板，则需要安装Xilinx的XSDK。本手册中假设XSDK的安装位置在`/opt/Xilinx`的默认位置。正确安装XSDK之后，请确保正确安装Xilinx设备驱动程序：

    $ cd /opt/Xilinx/SDK/2017.3/data/xicom/cable_drivers/lin64/install_script/install_drivers
    $ sudo ./install_drivers

### 2.2.2 依赖库

在安装PetaLinux和XSDK之前，还需要安装下面的这些依赖库：

| 库 | Ubuntu下安装包名称 |
| -- | ------------------ |
| ncurses终端库 | ncurses-dev |
| 64-bit Openssl library | libssl-dev |
| 64-bit zlib compression library | zlib1g-dev |
| 32-bit zlib compression library | lib32z1 |
| 32-bit GCC support library | lib32gcc1 |
| 32-bit ncurses | lib32ncurses5 |
| 32-bit Standard C++ library | lib32stdc++6 |
| 32-bit selinux library | libselinux1:i386 |
| virtual framebuffer | xvfb |
| M4 macros for configuration shell scripts | autoconf |
| multipurpose relay for data dransfer | socat |
| generic library support script | libtool |
| compression library | zlib1g:i386 |
| Library containing repo | phablet-tools |
| Edit rpath in ELF binaries | chrpath |
| Direct media layer development files | Libsdl1.2-dev |
| Documentation system | texinfo |
| GNU Compiler multilib files | gcc-multilib |
| Development files for glib library | Libglib2.0-dev |

安装命令：

    $ sudo apt-get install -y ncurses-dev lib32z1 lib32gcc1 lib32ncurses5 lib32stdc++6 libselinux1:i386 zlib1g-dev libssl-dev xvfb autoconf socat libtool zlib1g:i386 phablet-tools chrpath libsdl1.2-dev texinfo gcc-multilib libglib2.0-dev

## 2.3 安装镜像

这里，要求至少10G以上的硬盘空间，用来解压缩文件，运行其中的脚本。下面的步骤已经在基于x84_64的`Ubuntu 14.04或16.04`上进行过验证。

下面的步骤是直接使用已经发布的镜像：

1. 创建目录`/opt/Xilinx`，也可以是其它目录。但是要保证对这个目录具有完全的读写权限。然后，拷贝压缩文件到该目录下：

        $ sudo chown $USER:$USER /opt
        $ mkdir -p /opt/Xilinx

2. `PetaLinux`构建环境要求建立链接文件`/bin/sh`到`/bin/bash`。打开一个终端窗口，执行下面的命令

        $ cd /bin
        $ sudo rm sh
        $ sudo ln -s bash sh

4. 关闭，重新打开一个终端，切换到压缩文件所在的目录下，使用下面的命令进行解压：
    
    $ tar -xvzf Virtuosity_2019.tgz

    正确的目录结构应该是`/opt/Xilinx/Virtuosity_2019/`这个样子

6. 设置环境变量 `$RELEASE_DIR`和`$BOARD`:

    $ export RELEASE_DIR=`pwd`/Virtuosity_2019
    $ export BOARD=zcu102

7. 从Xilinx工具网站上下载`petalinux-v2017.3-final-installer.run`到$RELEASE_DIR目录下；

8. 下载完成，修改PetaLinux安装文件为可执行权限；

    $ cd $RELEASE_DIR
    $ chmod +x petalinux-v2017.3-final-installer.run

9. 运行下面的命令，安装`PetaLinux`(这里要求接受许可协议)：

    $ ./petalinux-v2017.3-final-installer.run /opt/Xilinx/petalinux-v2017.3-final（安装目录）

10. Xilinx工具网站上，下载[`SDK 2017.3 Web Installer for Linux 64`](https://www.xilinx.com/member/forms/download/xef.html?filename=Xilinx_SDK_2017.3_1005_1_Lin64.bin) 到`$RELEASE_DIR`目录下。

11. 安装XSDK：

    $ chmod +x Xilinx_SDK_2017.3_1215_1_Lin64.bin
    $ ./Xilinx_SDK_2017.3_1215_1_Lin64.bin

12. 如果使用`UltraZed`板子，还需要手动添加主板定义文件到XSDK中。使用下面的命令获取文件，解压zip文件到正确的位置。

        $ wget http://zedboard.org/sites/default/files/documentations/UltraZed_Board_Definition_Files_v2017_2_Release_All_CC_3.zip
        $ unzip UltraZed_Board_Definition_Files_v2017_2_Release_All_CC_3.zip
        $ unzip ultrazed_board_definition_files_v2017_2.zip -d /opt/Xilinx/SDK/2017.3/data/boards/board_files/

13. 一旦`PetaLinux`被安装，使用下面的命令进行配置：

    $ source petalinux-v2017.3-final/settings.sh

    注意：现在Petalinux的安装目录已经被设置到环境变量$PETALINUX中了。

14. 创建`tftpboot`目录并使用下面的命令安装预构建的二进制文件。

    $ sudo mkdir -p /tftpboot
    $ sudo chmod 777 /tftpboot
    $ cp $RELEASE_DIR/dist/images/linux/$BOARD/* /tftpboot/


## 2.4 配置TFTP

A TFTP server is needed to load images to any target boards.

1. Configure the TFTP server by changing the value of TFTP_DIRECTORY to “/tftpboot” in /etc/default/tftpd-hpa

2. Restart the TFTP server

    $ sudo service tftpd-hpa restart

3. Take note of your IP configuration. Save the values for inet addr, Bcast, and Mask. These will be used in U-Boot for serverip, gatewayip, and netmask respectively.


# 3 目标板配置

## 3.1 目标板

### 3.1.1 配置双分区SD卡

# 4 引导和运行XZD

## 4.1 引导目标板

### 4.1.1 通过SD卡引导

#### 4.1.1.1 配置ZCU102引导模式开关

#### 4.1.1.2 配置UltraZed引导模式开关

#### 4.1.1.3 连接UART并引导

### 4.1.2 通过JTAG引导

## 4.2 运行XZD

### 4.2.1 引导客户机

### 4.2.2 拷贝客户机

### 4.2.3 通过其它文件系统引导客户机

# 5 使用源码构建

## 5.1 环境配置和构建过程

如果你还没有执行2.3节中的步骤，请在终端命令行中执行下面的命令，因为这个环境变量后面会使用：

    $ export RELEASE_DIR=`pwd`/Virtuosity_2019

## 5.2 构建Dom0，包括Linux内核、Xen、U-Boot和FSBL

> 如果还没有建立主机环境，请参考第2章进行设置。

1. 克隆`Yocto`源代码并且为dom0的FS文件系统创建默认配置文件。

        $ cd $RELEASE_DIR
        $ mkdir Xocto
        $ cd Xocto
        $ repo init -u git://github.com/dornerworks/xzd-yocto-manifests.git -m xzd.xml -b XZD_20190808
        $ repo sync

2. 配置`Yocto`环境

        $ source setupsdk

3. 配置`Yocto`使用正确的工具链。打开`conf/local.conf`文件，然后更新内容，指向正确的工具链位置。另外，需要使用`${BOARD}`代替{BOARD}。

        MACHINE = “{BOARD}-zynqmp”
        XILINX_VER_MAIN = “2017.3” 
        EXTERNAL_TOOLCHAIN_zynq = “/opt/Xilinx/SDK/2017.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi”
        EXTERNAL_TOOLCHAIN_microblaze =“/opt/Xilinx/SDK/2017.3/gnu/microblaze/linux_toolchain/
        lin64_le”EXTERNAL_TOOLCHAIN_aarch64 = “/opt/Xilinx/SDK/2017.3/gnu/aarch64/lin/aarch64-linux” 
        XILINX_SDK_TOOLCHAIN = “/opt/Xilinx/SDK/2017.3”

### 5.2.1 定制镜像

Refer to the Yocto Documentation for image customization.

[http://www.yoctoproject.org/docs/latest/mega-manual/mega-manual.html](http://www.yoctoproject.org/docs/latest/mega-manual/mega-manual.html)

## 5.3 安装并使用构建的镜像

1. Setup a two partition SD according to the instructions in section 3.1.1 steps 1 through 10.

2. Copy the files ‘BOOT.bin’, ‘xen.ub’, ‘xen.dtb’, ‘uboot.env’, and ‘Image’ to the fat partition of the SD card.

        $ sudo cp $RELEASE_DIR/Xocto/build/tmp/deploy/images/${BOARD}-zynqmp/boot.bin/media/$USER/BOOT/
        $ sudo cp $RELEASE_DIR/Xocto/build/tmp/deploy/images/${BOARD}-zynqmp/xen.ub/media/$USER/BOOT/
        $ sudo cp $RELEASE_DIR/Xocto/build/tmp/deploy/images/${BOARD}-zynqmp/xen.dtb/media/$USER/BOOT/
        $ sudo cp $RELEASE_DIR/Xocto/build/tmp/deploy/images/${BOARD}-zynqmp/Image/media/$USER/BOOT/
        $ sudo cp $RELEASE_DIR/dist/images/linux/${BOARD}/uboot.env /media/$USER/BOOT/

3. Create the overlay directory and add any additional files into the locations you need them on the EXT4partition. The example below will place a copy of the dom0 Linux kernel generated by Yocto into to theroot directory which can be used as a basis for any number of Linux domUs.

        $ mkdir /media/$USER/overlayfs/overlay
        $ mkdir /media/$USER/overlayfs/overlay/root
        $ cp $RELEASE_DIR/Xocto/build/tmp/deploy/images/${BOARD}-zynqmp/Image/media/$USER/rootfs/overlay/root/Dom1-Kernel

4. Unmount your SD card. Follow the steps in section 4.1.1 to boot your built system from the SD card

5. Once at the dom0 prompt, you can use the following commands to start a domU.
Use a text editor to create a configuration file with the following contents for your domain in ‘/etc/xen/`:

        # =====================================================================
        # Example PV Linux guest configuration

## 5.4 创建更多的客户机

# 6 基于Zynq的Xen

## 6.1 Xen引导过程

## 6.2 xl-Xen的接口

### 6.2.1 列出Domains

### 6.2.2 创建客户机Domain

### 6.2.3 关闭或销毁一个客户机Domain

### 6.2.4 切换Domain

## 6.3 xentop-分析Domain资源利用率

## 6.4 共享内存

### 6.4.1 libvchan

### 6.4.2 示例：利用libvchan在domain之间进行通信

# 7 裸机客户机

## 7.1 介绍

### 7.1.1 裸机客户机的启动

### 7.1.2 负载应用程序

## 7.2 构建裸机客户机

### 7.2.1 创建负载应用


### 7.2.2 构建客户机镜像

## 7.3 在XZD上安装和运行客户机镜像

### 7.3.1 客户机镜像

### 7.3.2 客户机配置

### 7.3.3 运行客户机

## 7.4 XSDK示例

# 8 其它客户机

# 9 IO设备的交互

## 9.1 半虚拟化

## 9.2 passthrough

### 9.2.1 UART passthrough

### 9.2.2 Ethernet Passthrough

#### 9.2.2.1 修改Xen设备树

#### 9.2.2.2 修改Domain配置文件

#### 9.2.2.3 创建部分设备树

#### 9.2.2.4 和Domain的通信

# 10 其它解决方案

## 10.1 目前支持的选项


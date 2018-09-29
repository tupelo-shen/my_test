

## Ubuntu 16.04 编译 QEMU

正常情况下，多数`Linux`版本已经提供了可供安装的`QEMU（KVM）`二进制安装包。但是，有时候因为需要，还需要自己编译`QEMU`。在编译之前，有一些依赖软件包需要安装。

默认情况下， 编译器和编译工具（`gcc`，`make`...）已经有了。

###依赖条件：

1. 必要安装包：
    * `git`->用于版本管理(在这里可以不要)
    * `glib2.0-dev`，自动包含 `zlib1g-dev`
    * `libfdt-devel`

对于Ubuntu LTS 长期稳定版本，可以使用下面的命令安装必要的安装包：

    sudo apt-get install git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev

2. 推荐安装包：

    * `git-email`->发送补丁使用(在这里可以不要)
    * `libsdl1.2-dev`，使用基于 SDL 图形用户接口时需要
    * `gtk2-devel`, 代替 VNC 的一个简单UI
    * `vte-devel`, 为了使用GTK接口访问QEMU监控器（monitor）和串口/控制台时需要

要想获得尽可能多的QEMU功能，上面列出的这些安装包还不够。所以在运行`./configure`命令时，可能很多行显示'Yes',只有很少行显示'No'。

对于Ubuntu LTS 长期稳定版本，可以使用下面的命令安装推荐的安装包：

    sudo apt-get install git-email
    sudo apt-get install libaio-dev libbluetooth-dev libbrlapi-dev libbz2-dev
    sudo apt-get install libcap-dev libcap-ng-dev libcurl4-gnutls-dev libgtk-3-dev
    sudo apt-get install libibverbs-dev libjpeg8-dev libncurses5-dev libnuma-dev
    sudo apt-get install librbd-dev librdmacm-dev
    sudo apt-get install libsasl2-dev libsdl1.2-dev libseccomp-dev libsnappy-dev libssh2-1-dev
    sudo apt-get install libvde-dev libvdeplug-dev libvte-2.90-dev libxen-dev liblzo2-dev
    sudo apt-get install valgrind xfslibs-dev

比较新的Ubuntu版本也可以尝试安装这些额外的安装包：

    sudo apt-get install libnfs-dev libiscsi-dev

这些软件包在Ubuntu LTS 长期稳定版本中已经存在，但是对于QEMU来说，有点旧了。

如果想要USB passthough功能的话，需要安装[libusb](https://libusb.info/)库。

下载源代码[libusb-1.0.22](https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.tar.bz2)。

运行下面的命令：

    tar -xjvf libusb-1.0.22.tar.bz2
    cd libusb-1.0.22
    ./configure
    make

运行 `./configure` 时会报错，`configure: error: udev support requested but libudev header not installed`。

再安装udev就好了，安装命令如下：

    sudo apt-get install libudev-dev

然后再执行 `sudo make install`就可以了。

### 下载

`QEMU`源码下载地址： [https://www.qemu.org/download/](https://www.qemu.org/download/)

下载代码方式一：

    git clone git://git.qemu.org/qemu.git

下载代码方式二：

    wget https://download.qemu.org/qemu-3.0.0.tar.xz

Note that when building QEMU from GIT, 'make' will attempt to checkout various [GIT submodules](https://wiki.qemu.org/Documentation/GitSubmodules).

## 编译安装

1. 下载编译 `QEMU 3.0.0`:

        wget https://download.qemu.org/qemu-3.0.0.tar.xz
        tar xvJf qemu-3.0.0.tar.xz
        cd qemu-3.0.0
        ./configure
        make

2. 使用`git`,下载编译 `QEMU 3.0.0`:

        git clone git://git.qemu.org/qemu.git
        cd qemu
        git submodule init
        git submodule update --recursive
        ./configure
        make

3. 可以在qemu这个root根目录下编译（但是不推荐），也可以在其它目录编译（推荐）

        # 切换到qemu根目录下
        cd qemu
        # 准备一个本机debug版本
        mkdir -p bin/debug/native
        cd bin/debug/native
        # 配置 QEMU 并启动构建
        ../../../configure --enable-debug
        make
        # 返回到QEMU根目录
        cd ../../..

4. 安装

        sudo make install #可以不安装

**注意：**

    运行`./configure --help`，可以获得帮助信息。

## 测试

1. 先启动一个简单的测试，是否编译正常

        bin/debug/native/x86_64-softmmu/qemu-system-x86_64 -L pc-bios

    结果就是启动了QEMU系统模拟器，通过PC BIOS引导。

    问题： 在启动时，可能会报错`qemu-system-x86_64: symbol lookup error: qemu-system-x86_64: undefined symbol: libusb_set_option`？

        这是因为我们安装的libusb在/usr/local/lib/目录下，但是，在此运行过程中是去根目录的`/lib`目录下找对应的库文件，所以会报找不到符号。

        解决办法就是：

            sudo ln -sf /usr/local/lib/libusb-1.0.so /lib/**x86_64-linux-gnu**/libusb-1.0.so.0

        加粗部分（`x86_64-linux-gnu`）对应你自己的系统，也可以是`i386-linux-gnu`。



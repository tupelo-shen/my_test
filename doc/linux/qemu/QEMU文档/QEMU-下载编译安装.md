

## 下载

1. 下载地址：

[https://www.qemu.org/download/](https://www.qemu.org/download/)

2. 依赖条件：

        sudo apt-get install git-email
        sudo apt-get install libaio-dev libbluetooth-dev libbrlapi-dev libbz2-dev
        sudo apt-get install libcap-dev libcap-ng-dev libcurl4-gnutls-dev libgtk-3-dev
        sudo apt-get install libibverbs-dev libjpeg8-dev libncurses5-dev libnuma-dev
        sudo apt-get install librbd-dev librdmacm-dev
        sudo apt-get install libsasl2-dev libsdl1.2-dev libseccomp-dev libsnappy-dev libssh2-1-dev
        sudo apt-get install libvde-dev libvdeplug-dev libvte-2.90-dev libxen-dev liblzo2-dev
        sudo apt-get install valgrind xfslibs-dev
        sudo apt-get install libnfs-dev libiscsi-dev




## 下载编译安装

1. To download and build QEMU 3.0.0:

        wget https://download.qemu.org/qemu-3.0.0.tar.xz
        tar xvJf qemu-3.0.0.tar.xz
        cd qemu-3.0.0
        ./configure
        make

2. To download and build QEMU from git:

        git clone git://git.qemu.org/qemu.git
        cd qemu
        git submodule init
        git submodule update --recursive
        ./configure
        make


## 测试
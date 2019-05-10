# 1. 必要条件

需要的条件如下：

* qemu-kvm-release.tar.gz
* kvm-kmod-release.tar.bz2（如果你想自己编译内核模块的话）
* 支持VT的Intel处理器或支持SVM的AMD处理器
* qemu必要条件：
    - zlib库和头文件
    - SDL库和头文件
    - alsa库和头文件（可选：默认是禁止的，可通过添加命令行参数--enable-alsa使能）
    - gnutls库和头文件（可选的VNC TLS支持：默认是使能的，可通过添加命令行参数--disable-vnc-tls禁止）
    - kernel头文件（对于Fedora来说，就是kernel-devel包）

对于debian系统，可以使用下面的命令进行安装：

    apt-get install gcc \
                    libsdl1.2-dev \
                    zlib1g-dev \
                    libasound2-dev \
                    linux-kernel-headers \
                    pkg-config \
                    libgnutls-dev \
                    libpci-dev

> * 如果需要从git上构建，还需要gawk。

# 2. 安装kvm

You may wish to take a look at the ["Kernel-optimizations"] page. There exists a [attachment:kvm-26-alt-grab.diff.gz patch] which will change the SDL keygrab combination from ctrl-alt to ctrl-alt-shift. It was written primarily to deal with the heavy use of ctrl-alt-delete in NT-based VMs.

If you are using a recent kernel (2.6.25+) with kvm modules included, boot into it, and:

    tar xzf qemu-kvm-release.tar.gz
    cd qemu-kvm-release
    ./configure --prefix=/usr/local/kvm
    make
    sudo make install
    sudo /sbin/modprobe kvm-intel
    # or: sudo /sbin/modprobe kvm-amd

If you're using an older kernel, or a kernel from your distribution without the kvm modules, you'll have to compile the modules yourself:

    tar xjf kvm-kmod-release.tar.bz2
    cd kvm-kmod-release 
    ./configure
    make 
    sudo make install 
    sudo /sbin/modprobe kvm-intel 
    # or: sudo /sbin/modprobe kvm-amd

Note: if sound doesn't play in the guest vm you can add --audio-drv-list="alsa oss" to ./configure as explained in http://www.linux-kvm.com/content/sound-problem-solved

# 3. 为guest虚拟机创建磁盘映像

/usr/local/kvm/bin/qemu-img create -f qcow2 vdisk.img 10G

# 4. 安装客户机操作系统

    sudo /usr/local/kvm/bin/qemu-system-x86_64 -hda vdisk.img -cdrom /path/to/boot-media.iso \ 
       -boot d  -m 384

(kvm doesn't make a distinction between i386 and x86_64 so even in i386 you should use `qemu-system-x86_64`)BR

If you have less than 1GB of memory don't use the -m 384 flag (which allocates 384 MB of RAM for the guest). For computers with 512MB of RAM it's safe to use -m 192, or even -m 128 (the default)

# 5. 运行新安装的客户机操作系统

    sudo /usr/local/kvm/bin/qemu-system-x86_64 vdisk.img -m 384

or a slightly more complicated example, where it is assumed that bridged networking is available on tap0; see ["Kernel-optimizations"] for some setup hints:

    /usr/local/kvm/bin/qemu-system-x86_64 -hda xp-curr.img \
                            -m 512 \
                            -soundhw es1370 \ 
                            -no-acpi \
                            -snapshot \
                            -localtime \
                            -boot c \
                            -usb -usbdevice tablet \ 
                            -net nic,vlan=0,macaddr=00:00:10:52:37:48 \
                            -net tap,vlan=0,ifname=tap0,script=no

(kvm doesn't make a distinction between i386 and x86_64 so even in i386 you should use `qemu-system-x86_64`)

If you're on Debian Etch, substitute `kvm` for `qemu-system-x86_64` (thanks to fromport, soren and mael_). See also the entries under the label "Ubuntu" on the HOWTO page. qemu-system-x86_64`

If you're on Fedora/RHEL/CentOS (and installed a kvm package and not built kvm yourself from source) then substitute qemu-kvm for qemu-system-x86_64

---
以上内容的参考网址是： [Getting KVM to run on your machine](https://www.linux-kvm.org/page/RunningKVM)
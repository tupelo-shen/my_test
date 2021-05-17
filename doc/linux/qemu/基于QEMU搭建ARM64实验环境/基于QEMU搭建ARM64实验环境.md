[TOC]

## 1. 开场白

环境：

1. 处理器架构：`arm64`

2. 内核源码：`linux-5.12.0`

3. ubuntu版本：`20.04.2`

4. 代码阅读工具：`sublime text 3`

对于Linux爱好者，你是否也有这样的困扰，为了学习Ｌinux而去购买昂贵的开发版，这大可不必，QEMU模拟器几乎可以满足你的需求，足够你去学习Linux，它能够模拟x86, arm, riscv等各种处理器架构，本文将向你呈现的不是QEMU/虚拟化的原理解读，而是如何搭建一个用于学习linux的QEMU环境，当然对于Ｌinux内核的学习这已经足够了。



## 2. 交叉编译工具链的安装

工欲善其事必先利其器，搭建QEMU的模拟环境首先需要下载安装对应架构的交叉编译工具链(这里是arm64架构): 

> 注：有时候需要安装一些依赖，根据提示安装即可！

```shell
$ sudo apt-get install gcc-aarch64-linux-gnu
```

查看版本说明：

```shell
$ aarch64-linux-gnu-gcc -v
Using built-in specs.
COLLECT_GCC=aarch64-linux-gnu-gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc-cross/aarch64-linux-gnu/9/lto-wrapper
Target: aarch64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.3.0-17ubuntu1~20.04' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs --enable-languages=c,ada,c++,go,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-libquadmath --disable-libquadmath-support --enable-plugin --enable-default-pie --with-system-zlib --without-target-system-zlib --enable-libpth-m2 --enable-multiarch --enable-fix-cortex-a53-843419 --disable-werror --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=aarch64-linux-gnu --program-prefix=aarch64-linux-gnu- --includedir=/usr/aarch64-linux-gnu/include
Thread model: posix
gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
```

## 3. QEMU的安装

下面安装我们的QEMU，当然我们可以下载QEMU的源码通过编译的方式安装，但这里我们是直接apt-get的这种方式安装：

```shell
$ sudo apt-get install qemu-system-arm
```

安装完成之后同样查看版本号:

```shell
$ qemu-system-aarch64 --version
QEMU emulator version 4.2.1 (Debian 1:4.2-3ubuntu6.16)
Copyright (c) 2003-2019 Fabrice Bellard and the QEMU Project developers
```

说明安装成功！

## 4. 根文件系统制作

这一步我们将要完成的是最小的根文件系统，虽然是最小的根文件系统，但是我们还是希望能够运行动态编译的应用程序，具体的制作过程如下：

1. 首先准备下载一份Busybox源码：Busybox的官方源码下载路径为[http://busybox.net/downloads/](http://busybox.net/downloads/)：

    > 这儿我们使用的是busybox-1.33.0。

2. 解压源码：

```shell
tar xvf busybox-1.33.0.tar.bz2
```

3. 进入源码目录:

```shell
cd busybox-1.33.0
```

4. 指定工具链:

```
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
```

5. 配置

```
Settings --->
    [*] Build static binary (no shared libs)
```

6. 编译安装

```
make
make install
```

完善其他目录结构。上面我们编译安装完成之后会在busybox源码目录的_install目录下生成必要的一些文件：

```
$ ls
bin  linuxrc  sbin  usr
```

可以看到都是一些命令相关的文件和作为init进程的linuxrc，缺少其它的一些配置文件等目录，所以需要进一步完善。

我们来创建其他需要的目录：

```
mkdir dev etc lib sys proc tmp var home root mnt 
```

我们主要需要更新etc、dev和lib目录：

8. 对于etc目录的更新：

    * 添加profile文件：

    ```
    #!/bin/sh
    export HOSTNAME=aaaa
    export USER=root
    export HOME=/home
    export PS1="[$USER@$HOSTNAME \W]\# "
    PATH=/bin:/sbin:/usr/bin:/usr/sbin
    LD_LIBRARY_PATH=/lib:/usr/lib:$LD_LIBRARY_PATH
    export PATH LD_LIBRARY_PATH
    ```

    可以看到我们自定义了命令提示符，cd进入了我们制定的家目录homes,导出了环境变量。

    * 添加inittab文件：

    ```
    ::sysinit:/etc/init.d/rcS
    ::respawn:-/bin/sh
    ::askfirst:-/bin/sh
    ::ctrlaltdel:/bin/umount -a -r
    ```

    这个是init进程解析的配置文件，通过这个配置文件决定执行哪个进程，何时执行。

    * 添加fstab文件：

    ```
    #device  mount-point    type     options   dump   fsck order
    proc /proc proc defaults 0 0
    tmpfs /tmp tmpfs defaults 0 0
    sysfs /sys sysfs defaults 0 0
    tmpfs /dev tmpfs defaults 0 0
    debugfs /sys/kernel/debug debugfs defaults 0 0
    kmod_mount /mnt 9p trans=virtio 0 0
    ```

    指定挂载的文件系统。

    * 创建init.d目录:

    ```
    mkdir  init.d
    ```

    * init.d下添加rcS文件：

    ```
    mkdir -p /sys
    mkdir -p /tmp
    mkdir -p /proc
    mkdir -p /mnt
    /bin/mount -a
    mkdir -p /dev/pts
    mount -t devpts devpts /dev/pts
    echo /sbin/mdev > /proc/sys/kernel/hotplug
    mdev -s
    ```

    到此我们etc下的文件都制作好了,目录结构如下：

    ```shell
    $ tree
    .
    ├── fstab
    ├── init.d
    │   └── rcS
    ├── inittab
    └── profile

    1 directory, 4 files
    ```

9. 制作`/dev`目录下的必要文件：

```
cd dev
sudo mknod 660 console c 5 1
sudo mknod 660 null c 1 3
```

10. 制作lib下必要文件：为了支持动态编译的应用程序的执行，根文件系统需要支持动态库，所以我们添加arm64相关的动态库文件到lib下：

```
cd lib
cp /usr/aarch64-linux-gnu/lib/*.so* -a .
```

对库文件进行瘦身（去除符号表和调试信息）,使得库文件变小：

```
aarch64-linux-gnu-strip *
```

至此，我们的最小的根文件系统已经全部制作完成！

## 5. 内核源码的编译

* 下载最新的Ｌiunx内核源码：官网下载最新的`Linux-5.12.0`内核：[http://www.kernel.org/pub/linux/kernel/](http://www.kernel.org/pub/linux/kernel/)

* 解压：

```
tar xvf linux-5.12.0.tar.xz
```

* 放置根文件系统到源码目录：

```
cd linux-5.12.0
sudo cp  ../../busybox-1.33.0/_install  _install_arm64 -a
```

* 配置

添加hotplug支持：

```
make defconfig
make menuconfig
```

    - 在弹出的界面中选择平台类型为`ARMv8 softwre model(Versatile Express)`；
    - 选择生成根文件系统（initramfs）的源文件（此处为`_install_arm64`）
    - 选择hotplug工具（`/sbin/hotplug`）。

* 编译

现在进行漫长的编译过程，编译的快慢取决于电脑的性能：

```
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
make all -j4
```

## 6. 开始体验

1. 创建共享目录`kmodules`:

```
$ mkdir kmodules
```

> 用于主机和qemu运行的系统进行共享文件。


2. 为了方便，创建启动脚本`run_qemu.sh`

```
qemu-system-aarch64 -machine virt -cpu cortex-a57 -machine type=virt \
     -m 1024 -smp 4 -kernel arch/arm64/boot/Image --append "rdinit=/linuxrc root=/dev/vda rw console=ttyAMA0 loglevel=8" -nographic \
     --fsdev local,id=kmod_dev,path=$PWD/kmodules,security_model=none \
     -device virtio-9p-device,fsdev=kmod_dev,mount_tag=kmod_mount
```

使用命令`chmod 755 run_qemu.sh`，修改文件可执行权限。

> 说明：
> `-cpu`：指定了模拟的cpu为 cortex-a57，
> `-m 1024`：指定内存大小为１G，
> `-smp 4`：指定模拟的系统为4核处理器，
> `-kernel`：指定启动的内核镜像，
> `--append`：指定传递的命令行参数，
> 后面的选项指定共享目录已经使用的9p协议。

运行起来之后就进入了跟文件系统：

```
Please press Enter to activate this console. 
[root@aaaa ]# 
[root@aaaa ]# 
[root@aaaa ]# ls
bin      etc      lib      mnt      root     sys      usr
dev      home     linuxrc  proc     sbin     tmp      var
[root@aaaa ]# uname -a
Linux (none)aaaa 5.12.0 #8 SMP PREEMPT Mon May 17 11:47:41 CST 2021 aarch64 GNU/Linux
```

可以看到内核版本是我们编译的新的Linux-5.12.0内核，到处我们成功搭建了QEMU环境来运行我们的新内核。

3. 使用模拟磁盘

上面我们使用initramfs的方式将我们的根文件系统的目录直接打包到内核源码，成为了内核的一部分，当然这个时候可以操作文件，但是文件都是在内存中，系统重启就会丢失，所以下面我们使用模拟磁盘的方式来挂载根文件系统。

制作磁盘文件：

```
dd if=/dev/zero of=rootfs_ext4.img bs=1M count=8192
mkfs.ext4 rootfs_ext4.img
mkdir -p tmpfs
mount -t ext4 rootfs_ext4.img tmpfs/ -o loop
cp -af _install_arm64/* tmpfs/
umount tmpfs
rm -rf tmpfs
chmod 777 rootfs_ext4.img
```

执行qemu命令：

```
qemu-system-aarch64 -machine virt -cpu cortex-a57 -machine type=virt \
          -m 1024 -smp 4 -kernel arch/arm64/boot/Image --append "noinitrd root=/dev/vda rw console=ttyAMA0 loglevel=8"  -nographic \
-driver if=none,file=rootfs_ext4.img,id=hd0 \
          -device virtio-blk-device,drive=hd0 \
          --fsdev local,id=kmod_dev,path=$PWD/kmodules,security_model=none \
          -device virtio-9p-device,fsdev=kmod_dev,mount_tag=kmod_mount
```

我们可以发现，传递给内核的命令行参数变化了，添加了noinitrd选项，这样就会挂载我们自己的模拟磁盘。

同样能进入到最小文件系统，但是这次我们查看：

```
[root@liebao ]# df -mh
Filesystem                Size      Used Available Use% Mounted on
/dev/root                 7.8G    148.5M      7.3G   2% /
devtmpfs                468.4M         0    468.4M   0% /dev
tmpfs                   489.8M         0    489.8M   0% /tmp
kmod_mount              901.1G    672.5G    182.8G  79% /mnt
```

可以看到大小为8G的磁盘可以我们使用，而且文件的操作重启之后不会丢失！！！

4. 共享文件

前面已经支持了主机和QEMU上的系统共享目录，这个目录就是kmodules目录：通过mount可以查看被挂载到了QEMU上的系统的/mnt目录下
在主机的内核源码目录的kmodules目录中echo一个文件：

```
$ echo "Hello QEMU" > test.txt
```

然后进入到我们启动QEMU的内核根文件系统的/mnt目录：

```
[root@aaaa mnt]# ls
test.txt
[root@aaaa mnt]# cat test.txt 
hello QEMU
[root@aaaa mnt]# 
```

可以看到我们之前写的文件，共享目录OK!

5. 应用测试

我们来写一个简单的hello world应用程序，体验一下在QEMU中的系统执行：源码如下：

```c
#include <stdio.h>
  
int main(int argc, char **argv)
{
  
    printf("Hello World, QEMU!!!\n");
              
    return 0;
}
```

然后使用交叉编译工具链动态编译：

```
$ aarch64-linux-gnu-gcc test.c -o test
```

拷贝到共享目录下：

```
cp test ../../kmodules/
```

在QEMU系统中，进入/mnt目录下执行：

```
[root@aaaa mnt]# ./test
Hello World, QEMU!!!
```

可以发现被成功执行了，说明模拟出来的系统可以运行应用程序，而且可以使用动态链接库！！！

6. 内核模块测试

下面写一个简单的内核模块：

Makefile文件：

```
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

KERNEL_DIR ?=  /home/hanch/study/kernel/linux-5.12.0
obj-m := module_test.o

modules:
    $(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
    $(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

install:
    cp *.ko $(KERNEL_DIR)/kmodules
```

内核模块文件module_test.c：

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init module_test_init(void)
{
    printk("module_test_init\n");
    return 0;
}    

static void __exit module_test_exit(void)
{
    printk("module_test_exit\n");
}

module_init(module_test_init);
module_exit(module_test_exit);
MODULE_LICENSE("GPL");
```

编译拷贝：

```
$ make modules 
make -C /home/hanch/study/kernel/linux-5.12.0 M=/home/hanch/study/kernel/linux-5.12.0/mydriver/module_eg modules
make[1]: 进入目录“/home/hanch/study/kernel/linux-5.12.0”
  CC [M]  /home/hanch/study/kernel/linux-5.12.0/mydriver/module_eg/module_test.o
  MODPOST /home/hanch/study/kernel/linux-5.12.0/mydriver/module_eg/Module.symvers
  CC [M]  /home/hanch/study/kernel/linux-5.12.0/mydriver/module_eg/module_test.mod.o
  LD [M]  /home/hanch/study/kernel/linux-5.12.0/mydriver/module_eg/module_test.ko
make[1]: 离开目录“/home/hanch/study/kernel/linux-5.12.0”

$ make install 
cp *.ko /home/hanch/study/kernel/linux-5.12.0/kmodules
```

到QEMU的内核系统中，进入/mnt目录下：执行模块的插入/移除:

```
[root@liebao mnt]# insmod module_test.ko 
[ 1406.614976] module_test_init
[root@liebao mnt]# lsmod 
module_test 16384 0 - Live 0xffff800008d40000 (O)
[root@liebao mnt]# rmmod module_test
[ 1424.748114] module_test_exit
```

内核模块也正常运行！！！

文章到这里关于QEMU体验最新的Ｌinux内核已经完成了，当然QEMU有多功能在此不在描述，目前配置的QEMU环境已经足够内核的学习和实际，只有大量的内核源代码＋在一种体系架构的处理器上实际内核才能真正的去理解内核的实现机理，才能更好去使用内核。

## 7. 安装依赖

上述过程中需要的依赖条件：

```
sudo apt-get install libssl-dev
sudo apt-get install libelf-dev
```
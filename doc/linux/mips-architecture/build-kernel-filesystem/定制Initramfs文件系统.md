[TOC]

initramfs is a root filesystem that is embedded into the kernel and loaded at an early stage of the boot process. It is the successor of initrd. It provides early userspace which can do things the kernel can't easily do by itself during the boot process.

Using initramfs is optional. By default, the kernel initializes hardware using built-in drivers, mounts the specified root partition, loads the init system of the installed Linux distribution. The init system then loads additional modules and starts services until it eventually presents a log in dialog. This is a good default behavior and sufficient for many users. initramfs is for users with advanced requirements; for users who need to do things as early as possible, even before the root partition is mounted.

Typically, an initramfs is not needed, but may be necessary for:

* Mounting an encrypted, logical, or otherwise special root partition
* Providing a minimalistic rescue shell (if something goes wrong)
* Customize the boot process (e.g., print a welcome message)
* Load modules necessary to boot (e.g., third party storage drivers)
* Anything the kernel can't do that's usually handled in user space

## 1 简述

有无数种方法可以创建`initramfs`文件系统。你可以选择不创建`initramfs`，也可以使用现成工具，比如`Genkernel`或`Dracut`等工具完成这个工作。但是，有些时候，我们需要自己手动创建一个`initramfs`文件系统。

`initramfs`至少包含一个`/init`文件。内核将这个文件作为主init线程（`PID=1`）。它完成所有的工作。另外，同其它的root根文件系统一样，`/init`还需要一些额外的文件和目录，比如`/dev`（记录设备节点信息）、`/proc`（记录内核信息）、`/bin`（保存可执行二进制文件）等等。`initramfs`的结构可简单可复杂，依赖于你具体的需求。

当内核挂载`initramfs`文件系统时，目标root分区还没有挂载，因此还不能访问任何文件。所以，你想要做的工作，必须都包含在`initramfs`文件系统中。比如，如果想使用shell，就必须将其包含在`initramfs`文件系统中；如果想挂载设备，就必须包含`mount`命令；如果想要加载某个模块，`initramfs`文件系统就必须提供加载工具和要加载的模块；如果这个工具还需要依赖某个库才能运行，则必须包含那个库。这看起来很复杂，事实也确实如此，因为`initramfs`必须能够独立运行。

## 2 基本知识

In this section you will learn the easy and straightforward way to initramfs creation. You will make a functional - albeit minimalistic - initramfs which you then can extend according to your own requirements.

### 2.1 目录结构

Create a basic `initramfs` directory structure that will later become your `initramfs` root. For consistency, we'll work in `/usr/src/initramfs`, but any location would do. If you choose another location, please adapt accordingly.

```
root #mkdir --parents /usr/src/initramfs/{bin,dev,etc,lib,lib64,mnt/root,proc,root,sbin,sys}
```

#### 2.1.1 设备节点

Most things the initramfs does will require a couple of device nodes to be present, especially the device for the root partition. Throughout this document, `/dev/sda1` will be used as example device. Copy basic device nodes from the root filesystem to the initramfs example location:

```
root #cp --archive /dev/{null,console,tty,sda1} /usr/src/initramfs/dev/
```

Which devices you need exactly depends entirely on what you are going to use initramfs for. Please adapt to your own needs.

> More advanced approaches to device nodes are covered in the [Dynamic devices](https://wiki.gentoo.org/wiki/Custom_Initramfs#Dynamic_devices) section.

### 2.2 应用

Any binary you want to execute at boot needs to be copied into your initramfs layout. You also need to copy any libraries that your binaries require. To see what libraries any particular binary requires, use the `ldd` tool. For example, the [dev-util/strace](https://packages.gentoo.org/packages/dev-util/strace) binary requires:

```
user $ldd /usr/bin/strace
    linux-vdso.so.1 (0x00007fff271ff000)
    libc.so.6 => /lib64/libc.so.6 (0x00007f5b954fe000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f5b958a9000)
```

Here you see that for `/usr/bin/strace` to work in your initramfs, you not only need to copy `/usr/bin/strace` to your `/usr/src/initramfs/bin`, but also `/lib64/libc.so.6` and `/lib64/ld-linux-x86-64.so.2` to your `/usr/src/initramfs/lib64`. The exception is `linux-vdso.so.1` which is provided by the kernel.

Some applications might depend on other files and libraries to work. For example, `app-editor/nano` needs a terminfo file `/usr/share/terminfo/l/linux` from `sys-libs/ncurses`, so you have to copy it to your `initramfs` as well. To find these dependencies, tools like `equery` and `strace` prove to be most helpful.

### 2.2.1 Busybox

Instead of collecting countless utilities and libraries (and never seeing the end of it), you can just use [sys-apps/busybox](https://packages.gentoo.org/packages/sys-apps/busybox). It's a set of utilities for rescue and embedded systems, it contains a shell, utilities like `ls`, `mkdir`, `cp`, `mount`, `insmod`, and many more - all in a single binary called `/bin/busybox`. For busybox to work properly in a initramfs, you need to emerge it with the `static` USE flag enabled, then copy the `/bin/busybox` binary into your initramfs layout as `/usr/src/initramfs/bin/busybox`:

> Use `ldd` to verify that the binary is static.

### 2.3 init

The file structure of your initramfs is almost complete. The only thing that is missing is `/init` itself, the executable in the root of the initramfs that is executed by the kernel. Because [sys-apps/busybox](https://packages.gentoo.org/packages/sys-apps/busybox) includes a fully functional shell, this means you can write your `/init` binary as a simple shell script (instead of making it a complicated application written in Assembler or C that you'd have to compile).

The following example shows a minimalistic shell script, based on the busybox shell(/usr/src/initramfs/init):

```
#!/bin/busybox sh

# Mount the /proc and /sys filesystems.
mount -t proc none /proc
mount -t sysfs none /sys

# Do your stuff here.
echo "This script just mounts and boots the rootfs, nothing else!"

# Mount the root filesystem.
mount -o ro /dev/sda1 /mnt/root

# Clean up.
umount /proc
umount /sys

# Boot the real thing.
exec switch_root /mnt/root /sbin/init
```

This example needs some device nodes to work, mainly the root block device. Change the script and copy the corresponding /dev/ node to fit your needs.

Don't forget to make the `/init` file executable:

```
root #chmod +x /usr/src/initramfs/init
```

### 2.4 打包

Your initramfs now has to be made available to the kernel at boot time. This is done by packaging it as a compressed cpio archive. This archive is then either embedded directly into the kernel image, or stored as a separate file which can be loaded by the bootloader during the boot process. Both methods perform equally well, simply choose the method you prefer.

#### 2.4.1 内核配置

With either method, you need to enable Initial RAM filesystem and RAM disk (initramfs/initrd) support.

```
CONFIG_BLK_DEV_INITRD=y

General setup  --->
    [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
```

> Also enable all drivers, filesystems, compression methods and other settings that are required for booting and accessing your root partition. If you select such drivers as modules, you'll have to collect and integrate the module files into your initramfs and load them in your /init. Generally this means a lot of unnecessary extra work, so just use built-in drivers for now.

```
genkernel --install --ramdisk-modules --kerneldir=/home/dslab/loongson-linux/ --kernel-config=/home/dslab/loongson-linux/.config initramfs
```
-kerneldir是指定内核源码的位置，-kernel-config是指定内核配置文件.config的位置，该initramfs生成后也会被默认放在/boot文件夹下。


# added ENVIRONMENT VARS for loongson
# CC_PREFIX=/opt/cross-gcc-4.9.3-n64-loongson-rc6.1
CC_PREFIX=/opt/gcc-4.9.3-64-gnu

export PATH=$CC_PREFIX/usr/bin:$PATH
# export LD_LIBRARY_PATH=$CC_PREFIX/usr/lib:$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=$CC_PREFIX/usr/x86_64-unknown-linux-gnu/mips64el-loongson-linux/lib/:$LD_LIBRARY_PATH

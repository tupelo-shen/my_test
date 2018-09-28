
# 虚拟机VirtuabBox支持USB设备

    在设置里面添加指定的USB设备就可以了。

## Ubuntu中查看USB设备的方法

    使用命令`lsusb`

结果如下：

    Bus 001 Device 003: ID 0bda:0158 Realtek Semiconductor Corp. USB 2.0 multicard reader
    Bus 001 Device 002: ID 80ee:0021 VirtualBox USB Tablet
    Bus 001 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub

#Qemu monitor中添加USB存储设备

## 创建磁盘映像文件

    dd if=/dev/zero of=usbdisk.img bs=512 count=102400
    mkfs.ext2 usbdisk.img

## 通过QEMU Monitor插入优盘

    (qemu) drive_add 0 id=my_usb_disk,if=none,file=usbdisk.img
    (qemu) device_add usb-storage,id=my_usb_disk,drive=my_usb_disk

drive_add创建一个虚拟的磁盘驱动器，usbdisk.img为磁盘映像文件
device_add添加优盘设备到虚拟机，使用上一步创建的磁盘驱动器

## 通过QEMU Monitor移除优盘

    (qemu) device_del my_usb_disk

# 遇到的问题

1. lsusb命令不识别

    解决：安装 usbutils-007-4.el7.x86_64.rpm --nodeps

2. 有多个usb的时候，怎么判断虚拟机要哪个usb设备？
3. attach的时候，提示 'usb-host' is not a valid device model name

    解决：编译qemu的时候，带参数 --enable-libusb，并且编译好之后，要安装rbd包

    From qemu-1.7 release version, the old usb-host(host-linux.c) had been removed, re-implemented by libusbx. So you should build qemu with --enable-libusb, then you can use usb pass-through capacity.

4. 如果usb分区成两个分区，不清楚对挂载有没有影响，需要进行测试验证，没有影响。
5. u盘被一个虚拟机挂载之后，其他虚拟机不可以再挂载这个u盘

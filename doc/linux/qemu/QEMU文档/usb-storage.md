
QEMU USB存储设备的仿真
--------------------------

QEMU 有三个用于USB存储设备仿真的设备。

第一种仿真了市场上99%的使用批量传输协议的USB设备，称为"usb-storage"。使用方法如下（连接到xhci控制器，其它主机控制器也可以工作）：

    qemu ${other_vm_args}                                \
        -drive if=none,id=stick,file=/path/to/file.img  \
        -device nec-usb-xhci,id=xhci                    \
        -device usb-storage,bus=xhci.0,drive=stick

第二种是支持UASP协议（usb attached scsi transport）的USB设备。这个不会自动创建scsi磁盘，因此必须手动添加一个。 支持多个逻辑单元。 以下是树型逻辑单元的示例：

    qemu ${other_vm_args}                                                \
        -drive if=none,id=uas-disk1,file=/path/to/file1.img             \
        -drive if=none,id=uas-disk2,file=/path/to/file2.img             \
        -drive if=none,id=uas-cdrom,media=cdrom,file=/path/to/image.iso \
        -device nec-usb-xhci,id=xhci                                    \
        -device usb-uas,id=uas,bus=xhci.0                               \
        -device scsi-hd,bus=uas.0,scsi-id=0,lun=0,drive=uas-disk1       \
        -device scsi-hd,bus=uas.0,scsi-id=0,lun=1,drive=uas-disk2       \
        -device scsi-cd,bus=uas.0,scsi-id=0,lun=5,drive=uas-cdrom

第三种也是模拟了经典的使用批量传输协议的USB设备，称为"usb-bot"。它和"usb-storage"共享大部分代码，在客户端看来也没有什么差别。QEMU命令行与"usb-uas"类似，例如，也不会自动添加scsi磁盘。也支持多达16个逻辑单元。逻辑单元号必须是连续的，如果有三个设备你必须使用0-1-2。如果使用"usb-uas"的示例中的0-1-5这种逻辑单元顺序是不工作的。

从QEMU 2.7版本开始，usb-bot和usb-uas开始支持`即插即用`。在`即插即用`这种情况下，默认添加"attached = false"，所以需要设置"attached = true"客户端才能看到USB设备。在客户端看见设备之前，可以添加一个或多个设备，工作流程看起来像下面的步骤：

* (1) device-add usb-bot,id=foo
* (2) device-add scsi-{hd,cd},bus=foo.0,lun=0
    * (2b) optionally add more devices (luns 1 ... 15).
* (3) scripts/qmp/qom-set foo.attached = true

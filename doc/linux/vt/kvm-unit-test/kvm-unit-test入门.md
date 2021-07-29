[TOC]

## 1 简介

`kvm-unit-tests`是`kvm`的单元测试。`kvm-unit-tests`是小型的`guest`操作系统，通常仅执行数十行C和汇编程序测试代码就能获得测试结果。

`kvm-unit-tests`可以用来测试开发，同时，对于初学者，例如根据`vmx`的测试，去`kvm`源码中仔细研究这部分功能的实现，有助于找到入口点。

## 2 编译

```
git clone https://gitlab.com/kvm-unit-tests/kvm-unit-tests.git
./configure
make
```

编译成功后，会发现`x86`目录中有很多`* .elf`文件，每个文件都是在特定环境下进行测试的测试用例。假设我们用`x86/apic.elf`作为测试用例。

## 3 运行kvm-unit-tests

软件安装：

    sudo apt-get install grub-pc-bin xorriso

构建image：

    mkdir -p iso/boot/grub/bin
    cp x86/apic.elf iso/boot/grub/bin/  
    echo >  iso/boot/grub/grub.cfg

编辑`iso/boot/grub/grub.cfg`：

    set default=0
    set timeout=3
    GRUB_TERMINAL="console serial"
    GRUB_SERIAL_COMMAND="serial --unit=0 --speed=115200 --word=8 --parity=no --stop=1"
    menuentry ACRN hypervisor {
    echo Load apic ...
            multiboot --quirk-modules-after-kernel /boot/grub/bin/apic.elf
    }

制作镜像：

    grub-mkrescue -o test.img iso

## 4 结果

    qemu-system-x86_64 -machine q35,kernel_irqchip=split,accel=kvm \
    -m 4G -smp cpus=4 -enable-kvm \
    -device isa-debug-exit -device intel-iommu,intremap=on,caching-mode=on,device-iotlb=on \
    -debugcon file:/dev/stdout -serial mon:stdio -display none \
    -cdrom test.img

启动过程：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/vt/kvm-unit-test/images/kvm_unit_test_1_1.png">

* 当需要`enable vnc`时，qemu无需`-display none`项。
* 当需要在qemu中运行`hypervisor`时，qemu需要添加`-cpu host`。
* 如果出现问题，则`cat /sys/module/kvm_intel/parameters/nested`。如果显示`N`，则需要`modprobe kvm_intel nested=1`。

## 5 新方法

运行`kvm-unit-tests`的另一种方法是：

    sudo qemu-system-x86_64 -machine q35,kernel_irqchip=split,accel=kvm \
    -m 4G -smp cpus=4 -enable-kvm \
    -serial stdio \
    -device isa-debug-exit -device intel-iommu,intremap=on,caching-mode=on,device-iotlb=on \
    -cpu host \
    -kernel ./x86/vmx.flat


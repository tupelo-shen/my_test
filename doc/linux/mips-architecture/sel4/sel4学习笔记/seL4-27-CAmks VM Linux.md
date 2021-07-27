[TOC]

[TOC]

本教程将会讲解基于`seL4`微内核，使用CAmkES工具构建VM虚拟机和应用程序。

## 1 前提条件

1. [建立虚拟开发环境](https://docs.sel4.systems/HostDependencies)

## 2 学习目标

1. 在CAmkES系统中创建、配置和构建Linux虚拟机组件；
2. 构建和安装自己的Linux虚拟机用户态程序和内核模块；

## 3 背景知识

This tutorial is set up with a basic CAmkES VM configuration for you to build upon. The starting application should boot a single, very basic Linux guest.

To build the tutorial, run:

```
# In build directory
ninja
```

You can boot the tutorial on an x86 hardware platform with a multiboot boot loader, or use the QEMU simulator. Note if you are using QEMU it is important to ensure that your host machine has VT-x support and KVM installed. You also need to ensure you have enabled nested virtulisation with KVM guests as described here.

./simulate: qemu-system-x86_64  -cpu Nehalem,+vme,+pdpe1gb,-xsave,-xsaveopt,-xsavec,-fsgsbase,-invpcid,enforce -nographic -serial mon:stdio -m size=512M  -kernel images/kernel-x86_64-pc99 -initrd images/capdl-loader-image-x86_64-pc99 qemu-system-x86_64: warning: TCG doesn't support requested feature: CPUID.01H:EDX.vme [bit 1]
qemu-system-x86_64: TCG doesn't support requested features

To simulate the image you can run the provided simulation script with some additional parameters:

```
# 在build目录下，
# 首先需要创建一个tap设备：
# ip tuntap add mode tap tap0 
# ip addr add 10.0.120.100/24 dev tap0
# ip link set dev tap0 up
sudo ./simulate --machine q35,accel=kvm,kernel-irqchip=split --mem-size 2G --extra-cpu-opts "+vmx" --extra-qemu-args="-enable-kvm -device intel-iommu,intremap=off -net nic,model=e1000 -net tap,script=no,ifname=tap0"
```

./simulate: qemu-system-x86_64 -machine q35,accel=kvm,kernel-irqchip=split -cpu Nehalem,+vme,+pdpe1gb,-xsave,-xsaveopt,-xsavec,-fsgsbase,-invpcid,enforce,+vmx -nographic -serial mon:stdio -m size=2G -enable-kvm -device intel-iommu,intremap=off -net nic,model=e1000 -net tap,script=no,ifname=tap0 -kernel images/kernel-x86_64-pc99 -initrd images/capdl-loader-image-x86_64-pc99 Could not access KVM kernel module: No such file or directory
qemu-system-x86_64: failed to initialize KVM: No such file or directory

./simulate: qemu-system-x86_64 -machine q35,accel=kvm,kernel-irqchip=split -cpu Nehalem,+vme,+pdpe1gb,-xsave,-xsaveopt,-xsavec,-fsgsbase,-invpcid,enforce,+vmx -nographic -serial mon:stdio -m size=2G -enable-kvm -device intel-iommu,intremap=off -net nic,model=e1000 -net tap,script=no,ifname=tap0 -kernel images/kernel-x86_64-pc99 -initrd images/capdl-loader-image-x86_64-pc99 qemu-system-x86_64: warning: host doesn't support requested feature: CPUID.80000001H:EDX.pdpe1gb [bit 26]
qemu-system-x86_64: Host doesn't support requested features

./simulate: QEMU failed; resetting terminal in 5 seconds--interrupt to abort

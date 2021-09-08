[TOC]

原文地址：[https://www.jianshu.com/p/5ec4507e9be0](https://www.jianshu.com/p/5ec4507e9be0)

## 1 背景知识

* **介绍**

    `KVM` 全称是 `基于内核的虚拟机`（`Kernel-based Virtual Machine`），它是Linux 的一个内核模块，该内核模块使得 Linux 变成了一个 Hypervisor。

* **KVM架构**

    KVM 是基于虚拟化扩展（Intel VT 或者 AMD-V）的 X86 硬件的开源的 Linux 原生的全虚拟化解决方案。KVM 本身不执行任何硬件模拟，需要用户空间程序（QEMU）通过 `/dev/kvm` 接口设置一个客户机虚拟服务器的地址空间，向它提供模拟 I/O，并将它的视频显示映射回宿主的显示屏。

## 2 KVM实现

现在很多文章都是在讲怎样用`libvirt`或者`QEMU`来实现`KVM`，本文则是直接从底层实现`KVM`。从底层实现KVM可参考[Using the KVM API](https://links.jianshu.com/go?to=https%3A%2F%2Flwn.net%2FArticles%2F658511%2F)，github上还有两个项目[kvm-hello-world](https://links.jianshu.com/go?to=https%3A%2F%2Fgithub.com%2Fdpw%2Fkvm-hello-world)和[kvmtool](https://links.jianshu.com/go?to=https%3A%2F%2Fgithub.com%2Fkvmtool%2Fkvmtool)也很不错，[OSDev.org](https://links.jianshu.com/go?to=http%3A%2F%2Fwww.osdev.org%2F)上有很多关于操作系统的文章。

## 3 开始




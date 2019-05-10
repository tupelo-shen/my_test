# KVM-内核虚拟机

KVM的全称就是 **Kernel-based Virtual Machine**，是一个完整的虚拟化解决方案，适用于运行在x86硬件上的Linux操作系统， 该硬件平台 必须支持虚拟化扩展（Intel VT或AMD-V）。它包含一个可加载的内核模块，**kvm.ko**，它提供了内核虚拟化的基础架构和CPU特定模块，**kvm-intel.ko** 或 **kvm-amd.ko** 。

使用KVM，可以运行多个虚拟机，每个虚拟机都运行着不用修改的Linux或Windows镜像。每个虚拟机都有自己独立的虚拟硬件：网卡，硬盘，图形适配器等。

KVM是开源软件。KVM的内核部分包含在Linux内核中，从2.6.20版本开始。其用户空间部分包含在QEMU中，从1.3版本开始。

在网站 [http://planet.virt-tools.org/](http://planet.virt-tools.org/)上可以找到活跃在KVM相关开发领域的开发人员发布的博客。

---
以上内容的参考网址是： [KVM官网首页](https://www.linux-kvm.org/page/Main_Page)


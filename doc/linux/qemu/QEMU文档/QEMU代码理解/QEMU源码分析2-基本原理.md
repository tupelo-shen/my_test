*QEMU*作为系统模拟器时，会模拟出一台能够独立运行操作系统的虚拟机。如下图所示，每个虚拟机对应Host主机中的一个 *QEMU*进程，而虚拟机的 *vCPU* 对应 *QEMU* 进程中的一个线程。

![2-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/QEMU%E6%96%87%E6%A1%A3/QEMU%E4%BB%A3%E7%A0%81%E7%90%86%E8%A7%A3/images/2-1.png)

系统虚拟化最主要是虚拟出CPU、内存和I/O设备。虚拟出的CPU称之为 *vCPU*, QEMU为了提升效率，借用KVM、XEN等虚拟化技术，直接利用硬件对虚拟化的支持，在主机上安全地运行虚拟机代码（需要硬件支持）。虚拟机 *vCPU* 调用 *KVM* 的接口来执行任务的流程如下（代码源自QEMU开发者Stefan的[技术博客](http://blog.vmsplice.net/2011/03/qemu-internals-overall-architecture-and.html)）：

    open("/dev/kvm")
    ioctl(KVM_CREATE_VM)
    ioctl(KVM_CREATE_VCPU)
    for (;;) {
        ioctl(KVM_RUN)
        switch (exit_reason) {
        case KVM_EXIT_IO:  /* ... */
        case KVM_EXIT_HLT: /* ... */
        }
    }

QEMU发起 *ioctl* 调用KVM接口，KVM则利用硬件扩展直接将虚拟机代码运行与主机之上，一旦vCPU需要操作设备寄存器，vCPU将会停止并退回到QEMU，QEMU去模拟出操作结果。

虚拟机内存会被映射到QEMU的进程地址空间，在启动时分配。在虚拟机看来，QEMU所分配的主机上的虚拟地址空间为虚拟机的物理地址空间。

QEMU在主机用户态模拟虚拟机的硬件设备，vCPU对硬件的操作结果会在用户态进行模拟，如虚拟机需要将数据写入硬盘，实际结果是将数据写入到了主机中的一个镜像文件中。
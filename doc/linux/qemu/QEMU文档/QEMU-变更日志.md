<h1 id="0">0 目录</h1>

主要关注基于`ARM`、`RISC-V`架构的QEMU的版本变更历史

* [1 版本4.0](#1)
    - [1.1 ARM](#1.1)
    - [1.2 RISC-V](#1.2)
* [2 版本4.1](#2)
    - [2.1 ARM](#2.1)
    - [2.2 RISC-V](#2.2)
* [3 版本4.2](#3)
    - [3.1 ARM](#3.1)
    - [3.2 RISC-V](#3.2)

<h1 id="1">1 版本4.0</h1>

<h2 id="1.1">1.1 ARM</h2>

1. 实现对`ARMv8.x`架构各种扩展的支持。
2. 对新处理器的支持，`musca-a`和`musca-b1`。它们模拟了Arm推出的`Musca`开发板，一款物联网芯片的开发板。
3. 对新处理器的支持，`mps2-an521`。这是MPS2开发板的AN521 FPGA镜像的模型。
4. `linux-user`模式下正确支持`TBI`技术。
5. `micro:bit`开发板现在能够引导简单的`MicroPython`程序，因为添加了timer、GPIO、NVMC和RNG的设备模拟。
6. `cubieboard`模型实现了`'A' SRAM`。
7. `AArch64`64位处理器可以从大于4GB的存储位置引导内核到RAM中。
8. `stellaris`系列开发板("lm3s6965evb", "lm3s811evb")实现了看门狗定时器的设备模拟。
9. `TZ MPC`设备中的`BLK_MAX`寄存器能够上报正确的值。
10. `u-boot`的`noload`镜像类型，在Arm 虚拟开发板上得到支持。
11. `Arm虚拟开发板`现在支持超过255GB的RAM。
12. `stm32f2xx_usart`:当设备被禁止时，不会更新数据寄存器。
13. 虚拟开发板`ACPI`表:在IORT SMMUv3节点上能够正确设置`COHACC`重写标志。
14. 修正了`AArch32`异常返回，允许从`Mon->Hyp`模式切换。
15. `ftgmac100`：在Aspeed SoC上实现新的MDIO接口。
16. 改进了对`ARM PMU`单元的模拟。
17. 修复因未定义（UNDEF）无效A64指令而失败的各个地方。
18. 不要在32位输入上定义一个有效的FCMLA。
19. 使FPSCR/FPCR捕获异常位RAZ/WI。
20. 修复了处理`FPSCR/FPSR`异常状态位清除的错误。
21. `armv7m_nvic`：允许字节访问`SHPR1`寄存器。
22. `linux-user`：支持HWCAP_CPUID，它向用户代码公开ID寄存器。
23. 修正了一个`v8M MPU`错误:只要其使能，总是使用background region，如果它是启用的，而不是只有在没有击中一个更具体的区域时。

<h2 id="1.2">1.2 RISC-V</h2>

1. 虚拟开发板现在支持PCI和USB。
2. `mstatus`的`FS`域现在支持三种状态(dirty、clean和off)。
3. 现在已经实现了mstatus的TSR、TW和TVM域。
4. `misa CSR`现在可写。
5. 内置的gdbserver通过XML文件支持寄存器列表。
6. `sifive u`机器支持SMP。
7. `SiFive UART`支持TX中断。
8. `sifive_u`机器有正确数量的PLIC中断。

<h1 id="2">2 版本4.1</h1>

<h2 id="2.1">2.1 ARM</h2>

1. QEMU现在支持为Cortex-M cpu模拟了FPU单元, Cortex-M4和Cortex-M33现在支持FPU。
2. Cortex-R5F现在只正确地提供了寄存器为D0-D15的FPU，D16-D31还不能正确提供。
3. 构建配置文件已经切换到新的Kconfig系统，因此可以更容易地构建某个开发板的QEMU。
4. 支持ARMv8.5-RNG扩展，用于cpu生成的随机数。
5. Exynos4210 SoC模型现在支持PL330 DMA控制器。
6. 处理GICv3 ICC_CTLR_EL3和ICH_VMCR_EL2寄存器写的一些错误已经修复。
7. 内置的bootloader引导程序可以引导大于128M的AArch64内核镜像。
8. GICv3现在正确地报告GICD_TYPER。如果`GICD_CTLR.SecurityExtn`为0，如果`GICD_CTLR.DS`是1的话。
9. 支持新的开发板：swift-bmc。
10. 支持新的开发板：sbsa-ref（用于开发服务器固件和内核软件的AArch64系统）。
11. Aspeed SoC/开发板，现在支持RTC设备和xdma设备。
12. `i.mx7`的PCI控制器仿真已经得到了改进，因此它可以引导当前的Linux内核。
13. pl031 RTC设备现在正确地使来宾RTC跟踪在VM状态保存和稍后重新加载之间的主机RTC时间上向前推进。

<h2 id="2.2">2.2 RISC-V</h2>

1. SIP中的任意位不能再被设置。
2. 定义了一个新的“spike”机器，旧的机器被弃用。
3. sfence.vma不能再从用户空间执行。
4. 单步跳过分支和跳转现在可以工作了。
5. 少量的非法指令(全部压缩)现在被正确处理，而不是被当作nop处理。
6. 增加了对特权规范1.11.0版本的支持。
7. QEMU在支持设备树的目标上提供一个cpu拓扑设备树节点。

总结，对RISC-V架构的支持还在完善中。

<h1 id="3">3 版本4.2</h1>

<h2 id="3.1">3.1 ARM</h2>

1. 添加对`Aspeed AST2600`的支持。
2. QEMU的Arm半托管实现现在支持规范的v2.0，包括“STDOUT_STDERR”和“EXIT_EXTENDED”扩展。
3. KVM功能现在可支持超过256个cpu。
4. 虚拟开发板现在可以支持"内存的热插拔-hotplug"，当与UEFI客户BIOS和ACPI一起使用时。
5. 树莓派模型现在实现了bcm2835系统定时器装置和一个虚拟热传感器。
6. 改进了ARM核的TCG仿真性能。
7. KVM客户机支持SVE(需要SVE硬件和内核支持)。

<h2 id="3.2">3.2 RISC-V</h2>

1. 现在支持'-initrd'参数。
2. 调试器现在可以查看所有架构状态，而不管当前在每个hart上执行的特权模式如何。
3. sifive_u板现在包含更多的内存区域，这些内存区域与开发板更加匹配。

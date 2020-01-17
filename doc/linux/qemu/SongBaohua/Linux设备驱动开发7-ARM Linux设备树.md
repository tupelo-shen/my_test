<h1 id="0">0 目录</h1>

* [1 什么是ARM Linux设备树](#1)
* [2 对Linux驱动开发的影响](#2)
* [3 实际应用](#3)

---

<h1 id="1">1 什么是ARM Linux设备树</h1>

Linux内核从3.x开始引入设备树的概念，用于实现`驱动代码与设备信息相分离`。在设备树出现以前，所有关于设备的具体信息都要写在驱动里，一旦外围设备变化，驱动代码就要重写。引入了设备树之后，驱动代码只负责处理驱动的逻辑，而关于设备的具体信息存放到设备树文件中，这样，如果只是硬件接口信息的变化而没有驱动逻辑的变化，驱动开发者只需要修改设备树文件信息，不需要改写驱动代码。比如在ARM Linux内，一个`.dts(device tree source)`文件对应一个ARM的machine，一般放置在内核的"arch/arm/boot/dts/"目录内，比如exynos4412参考板的板级设备树文件就是"arch/arm/boot/dts/exynos4412-origen.dts"。这个文件可以通过`$make dtbs`命令编译成二进制的.dtb文件供内核驱动使用。

基于同样的软件分层设计的思想，由于一个SoC可能对应多个machine，如果每个machine的设备树都写成一个完全独立的.dts文件，那么势必相当一些.dts文件有重复的部分，为了解决这个问题，Linux设备树目录把一个SoC公用的部分或者多个machine共同的部分提炼为相应的.dtsi文件。这样每个.dts就只有自己差异的部分，公有的部分只需要"include"相应的.dtsi文件, 这样就是整个设备树的管理更加有序。我这里用`Linux4.8.5`源码自带的dm9000网卡为例来分析设备树的使用和移植。这个网卡的设备树节点信息在`"Documentation/devicetree/bindings/net/davicom-dm9000.txt"`有详细说明，其网卡驱动源码是`"drivers/net/ethernet/davicom/dm9000.c"`。
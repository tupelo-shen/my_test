[TOC]

`elfloader`负责在`ARM`和`RISC`平台上为`seL4`的运行准备好硬件。使用CPIO归档文件加载内核和用户镜像，初始化其它CPU核（如果`SMP`被使能，并为内核建立初始化页表。

## 1 ARM平台的elfloader

在ARM平台上，`elfloader`支持4种方式引导：二进制镜像，`u-boot`使用的`uImage`镜像，`ELF`文件，`EFI`可执行文件。每种方法都有些许不同。它还可以为`seL4`提供一个`DTB`文件，从引导程序中传递，或者包含在CPIO归档文件中都可以。

1. (EFI only) `Elfloader`的入口点是`_gnuefi_start`；

2. (EFI only) `Elfloader`重定向自己；

3. `Elfloader`自身的入口点`_start`。位于`arch-arm/<arch_bitness>/crt0.S`；

4. `Elfloader`初始化[driver framework](https://docs.sel4.systems/projects/elfloader/#driver-framework)，使能`UART/printf`；

5. `Elfloader`加载内核，用户镜像和DTB，确定内核需要被映射到内存的位置；

6. 如果内核窗口与`elfloader`的代码重叠：
    
    * (AArch32 EFI only) `elfloader`自己重新定位。有关重定位逻辑的详细解释，请参阅`relocate_below_kernel`；
    
    * (Other platforms) `elfloader`中止；

7. `elfloader`继续引导。如果它重新定位自己，它将重新初始化驱动程序模型；

8. 如果`elfloader`处于`HYP`模式，但`seL4`没有配置为支持`HYP`，它将离开`HYP`模式；

9. The `elfloader`为内核建立初始页表（参考`init_hyp_boot_vspace`或`init_boot_vspace`）；

10. 如果`SMP`使能，`elfloader`将启动所有的其它CPU核；

11. `elfloader`使能`MMU`内存管理单元；

12. `elfloader`启动`seL4`，传递用户镜像和DTB文件。

#### 1.1 二进制镜像

`elfloader`期望从某个基地址开始执行，`shoehorn`小工具可以产生这个基地址。可以在`build`目录下运行下面的命令获取正确的地址：

```
$ arm-linux-gnueabi-objdump -t elfloader | grep _text
0080f000 g       .start 00000000 _text
```

在构建目录下， `0x0080f000`就是基地址。

在`aarch64`架构上，`elfloader`尝试移动自己到正确的地址。但是，如果加载地址和正确地址太接近，这将失败，因为重定位代码将被覆盖。（TODO: 目前不太明白）

也可以通过在`CMake`中设置`IMAGE_START_ADDR`变量来覆盖`shoehorn`，从而硬编码一个加载地址。

#### 1.2 U-Boot

`elfloader`可以根据Linux内核对`ARM/ARM64`的引导约定来引导。如果提供了`DTB`，则将其传递给`seL4`，然后，`seL4`将其传递给root任务。

#### 1.3 ELF

`elfloader`支持作为`ELF`映像执行(通过`U-Boot`中的`bootelf`或类似的方式)。

#### 1.4 EFI

`elfloader`支持EFI，基于`gnu-efi`。它将根据需要重新定位自己，并支持从`EFI实现`中加载`DTB`。

## 2 RISC-V平台的elfloader

在`RISC-V`平台上，`elfloader`基本上与`ARM`平台一样。但是，由于缺乏可用的平台，目前只支持两种方法：支持`ELF`文件和二进制镜像运行。两种情况下，都应该提供[SBI](https://github.com/riscv/riscv-sbi-doc)实现，`elfloader`用其作为日志输出通道和多核引导。`seL4`构建系统允许使用`elfloader`作为有效载荷构建[OpenSBI](https://github.com/riscv/opensbi)。[bbl](https://github.com/riscv/riscv-pk)已经被`OpenSBI`取代，所以对其的支持也被放弃。

## 3 驱动框架

elfloader提供了一个驱动框架，以减少平台之间的代码重复。目前该框架只支持`UART`输出，但是，在设计上保持了可扩展性。实际上，目前也只在ARM架构上使用，因为`RISC-V`使用了`SBI`作为`UART`的载体，并且`SBI`也没有设备树入口。希望在未来，`RISC-V`平台上也支持该驱动框架。

该驱动框架使用了一个头文件，包含设备的列表，该列表是由`hardware_gen.py`小工具产生的。目前，该头文件仅包含`DTB`中的`stdout-path`属性指定的`UART`。列表中，每个设备包含一个兼容字符串`compat`，一个地址列表`region_bases[]`，该地址列表对应`DTB`文件中`reg`属性指定的区域。

elfloader中的每个驱动程序中，有一组字符串列表，跟设备树中指定的设备名称进行匹配。比如，在`Tegra`和`TI`平台上，`8250 UART`驱动，有如下的字符串列表：

```c
static const struct dtb_match_table uart_8250_matches[] = {
    { .compatible = "nvidia,tegra20-uart" },
    { .compatible = "ti,omap3-uart" },
    { .compatible = "snps,dw-apb-uart" },
    { .compatible = NULL /* sentinel */ },
};
```

每个驱动中都有一个字段`type`，目前只支持`DRIVER_UART`。该类型表明驱动的类型，并提供相应的函数。例如，`UART`驱动程序，有一个`elfloader_uart_ops`数据结构，该结构提供了一个写字符的函数`putc()`。另外，驱动还提供一个初始化函数，该函数用于驱动和设备匹配的时候，执行设备相关的初始化工作（比如，设置UART设备为输出等）。

最后，每个驱动程序包含一个`struct elfloader_driver`数据结构，和相应的`ELFLOADER_DRIVER`声明语句。还是以`8250 UART`驱动为例，代码如下：

```
static const struct elfloader_driver uart_8250 = {
    .match_table = uart_8250_matches,
    .type = DRIVER_UART,
    .init = &uart_8250_init,
    .ops = &uart_8250_ops,
};

ELFLOADER_DRIVER(uart_8250);
```

#### 3.1 UART

驱动框架提供了一个`plat_console_putchar`的默认实现（符号限制为`__attribute__((weak))`），elfloader设备的真实`putc`函数会被赋值给`uart_set_out`函数，在`uart_set_out`调用之前，所有的字符都会被丢弃。如果不想用驱动框架，这可以被重写。

## 4 移植elfloader

#### 4.1 ARM平台

Once a kernel port has been started (and a DTB provided), porting the elfloader to a platform is reasonably straightforward.

Most platform-specific information is extracted from a DTB, including available physical memory ranges. If the platform uses a UART compatible with another platform, even the UART will work out of the box. In other cases, it might be necessary to add a new `dtb_match_table` entry to an existing driver, or add a new driver (which is fairly trivial - only the `match_table` and `putchar` functions from an existing driver would need to be changed).

An appropriate image type needs to be selected. By default `ElfloaderImage` is set to `elf`, however, various platform-specific overrides exist and can be found in `ApplyData61ElfLoaderSettings` in this repo, at `cmake-tool/helpers/application_settings.cmake`.

#### 4.2 RISC-V平台

TODO:待完善项
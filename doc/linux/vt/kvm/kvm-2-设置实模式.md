Real Mode is a simplistic 16-bit mode that is present on all x86 processors. Real Mode was the first x86 mode design and was used by many early operating systems before the birth of Protected Mode. For compatibility purposes, all x86 processors [begin execution](https://wiki.osdev.org/Boot_Sequence) in Real Mode.

[TOC]

## 1 information

All modern operating systems (Windows, Linux, ...) run in Protected Mode, due to the many limitations and problems that Real Mode presents (see below, and the [Real Mode OS Warning](https://wiki.osdev.org/Real_Mode_OS_Warning) page). Older operating systems (such as DOS) and programs ran in Real Mode because it was the only mode available at the time. For information on how to switch from Real Mode to Protected Mode, see the corresponding article.

Note: There is a mode called [Virtual 8086 Mode](https://wiki.osdev.org/Virtual_8086_Mode) which allows operating systems running in Protected mode to emulate the Real Mode segmented model for individual applications. This can be used to allow a Protected Mode operating system to still have access to e.g. BIOS functions, whenever needed.

Below you'll find a list of cons and pros. These are mostly 'in comparison to Protected Mode'.

#### 1.1 缺点

1. 内存小于1M；
2. 没有基于硬件的内存保护（GDT），没有虚拟地址；
3. 没有内置的安全机制来防止错误或者恶意的应用程序；
4. 默认的CPU操作数长度只有16位；
5. 提供的内存寻址模式比其它工作模式有更多的限制；
6. 访问超过64k需要使用难以处理的段寄存器。

#### 1.2 优点

1. BIOS安装设备驱动程序来控制设备和处理中断；
2. BIOS函数为操作系统提供了底层API函数的高级集合；
3. 不适用描述符表和更小的寄存器，访存速度快。

#### 1.3 常见误解

Programmers often think that since Real Mode defaults to 16 bits, that the 32 bit registers are not accessible. This is not true.

All of the 32-bit registers (EAX, ...) are still usable, by simply adding the "Operand Size Override Prefix" (0x66) to the beginning of any instruction. Your assembler is likely to do this for you, if you simply try to use a 32-bit register.

## 2 内存寻址

In Real Mode, there is a little over 1 MB of "addressable" memory (including the [High Memory Area](https://wiki.osdev.org/Real_Mode#High_Memory_Area)). See [Detecting Memory (x86)](https://wiki.osdev.org/Detecting_Memory_(x86)) and [Memory Map (x86)](https://wiki.osdev.org/Memory_Map_(x86)) to determine how much is actually usable. The usable amount will be much less than 1 MB. Memory access is done using [Segmentation](https://wiki.osdev.org/Segmentation) via a segment:offset system.

There are six 16-bit segment registers: CS, DS, ES, FS, GS, and SS. When using segment registers, addresses are given with the following notation (where 'Segment' is a value in a segment register and 'Offset' is a value in an address register):

```
 12F3  :  4B27
   ^       ^
Segment   Offset
```

Segments and Offsets are related to physical addresses by the equation:

```
PhysicalAddress = Segment * 16 + Offset
```

Thus, `12F3:4B27` corresponds to the physical address` 0x17A57`. Any physical address can be represented in multiple ways, with different segments and offsets. For example, physical address 0x210 can be `0020:0010`, `0000:0210`, or `0021:0000`.


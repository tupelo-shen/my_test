[TOC]

**Protected mode** is the main operating mode of modern Intel processors (and clones) since the 80286 (16 bit). On 80386s and later, the 32 bit Protected Mode allows working with several virtual address spaces, each of which has a maximum of 4GB of addressable memory; and enables the system to enforce strict memory and hardware I/O protection as well as restricting the available instruction set via Rings.

A CPU that is initialized by the [BIOS](https://wiki.osdev.org/BIOS) starts in [Real Mode](https://wiki.osdev.org/Real_Mode). Enabling Protected Mode unleashes the real power of your CPU. However, it will prevent you from using most of the BIOS interrupts, since these work in Real Mode (unless you have also written a [V86](https://wiki.osdev.org/Virtual_8086_Mode) monitor).

## 1 进入保护模式

在切换到保护模式之前：


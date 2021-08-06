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

## 2.1 The Stack

SS and SP are 16-bit segment:offset registers that specify a 20-bit physical address (described above), which is the current "top" of the stack. The stack stores 16-bit words, grows downwards, and must be aligned on a word (16-bit) boundary. It is used every time a program does a PUSH, POP, CALL, INT, or RET opcode and also when the BIOS handles any hardware interrupt.

## 2.2 High Memory Area

If you set DS (or any segment register) to a value of 0xFFFF, it points to an address that is 16 bytes below 1 MB. If you then use that segment register as a base, with an offset of 0x10 to 0xFFFF, you can access physical memory addresses from 0x100000 to 0x10FFEF. This (almost 64 kB) area above 1 MB is called the "High Memory Area" in Real Mode. Note that you have to have the A20 address line activated for this to work.

## 2.3 Addressing Modes

Real Mode uses 16-bit addressing mode by default. Assembly programmers are typically familiar with the more common 32-bit addressing modes, and may want to make adjustments -- because the registers that are available in 16-bit addressing mode for use as "pointers" are much more limited. The typical programs that run in Real Mode are often limited in the number of bytes available, and it takes one extra byte in each opcode to use 32-bit addressing instead.

Note that you can still use 32-bit addressing modes in Real Mode, simply by adding the "Address Size Override Prefix" (0x67) to the beginning of any instruction. Your assembler is likely to do this for you, if you simply try to use a 32-bit addressing mode. But you are still constrained by the current "limit" for the segment that you use in each memory access (always 64K in Real Mode -- [Unreal Mode](https://wiki.osdev.org/Unreal_Mode) can be bigger).

* [BX + val]
* [SI + val]
* [DI + val]
* [BP + val]
* [BX + SI + val]
* [BX + DI + val]
* [BP + SI + val]
* [BP + DI + val]
* [address]

## 3 从保护模式切换到实模式

As noted above, it is possible for a Protected mode operating system to use Virtual 8086 Mode mode to access BIOS functions. However, VM86 mode has its own complications and difficulties. Some OS designers think that it is simpler and cleaner to temporarily return to Real Mode on those occasions when it is necessary to access a BIOS function. This requires creating a special Ring 0 program, and placing it in a physical memory address that can be accessed in Real Mode.

The OS usually needs to pass an information packet about which BIOS function to execute.

The program needs to go through the following steps:

1. 禁止中断

    * 使用`CLI`指令关闭可屏蔽中断；
    * 禁止`NMI`中断（可选）；

2. 关闭页表硬件管理单元

    * 保证`1:1`的页转换；
    * 保证GDT和IDT
    * 清除第0个控制寄存器的PG标志位；
    * 设置第3个控制寄存器为0；

3. 将GDT设置为16位的表（如果已经可用，跳过这一步）

    * Create a new GDT with a 16-bit data and code segment:
        - Limit: 0xFFFFF
        - Base: 0x0
        - 16-bit
        - Privilege level: 0
        - Granularity: 0
        - Read and Write: 1

    * Load new GDT ensuring that the currently used selectors will remain the same (index in cs/ds/ss will be copy of original segment in new GDT)

4. Far jump to 16-bit protected mode:

    * Far jump to 16-bit protected mode with a 16-bit segment index.

5. Load data segment selectors with 16-bit indexes:

    * Load ds, es, fs, gs, ss with a 16-bit data segment.

6. Load real mode IDT:

    * Limit: 0x3FF
    * Base 0x0
    * Use lidt

7. Disable protected mode:

    * Set PE bit in CR0 to false.

8. Far jump to real mode:

    * Far jump to real mode with real mode segment selector (usually 0).

9. Reload data segment registers with real mode values:

    * Load ds, es, fs, gs, ss with appropriate real mode values (usually 0).

10. Set stack pointer to appropriate value:

    Set sp to stack value that will not interfere with real mode program.

11. Enable interrupts:

    * Enable maskable interrupts with STI.

12. Continue on in real mode with all bios interrupts.

## 4 x86汇编示例

```
[bits 16]
 
idt_real:
    dw 0x3ff        ; 256 entries, 4b each = 1K
    dd 0            ; Real Mode IVT @ 0x0000
 
savcr0:
    dd 0            ; Storage location for pmode CR0.
 
Entry16:
        ; We are already in 16-bit mode here!
 
    cli         ; Disable interrupts.
 
    ; Need 16-bit Protected Mode GDT entries!
    mov eax, DATASEL16  ; 16-bit Protected Mode data selector.
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
 
 
    ; Disable paging (we need everything to be 1:1 mapped).
    mov eax, cr0
    mov [savcr0], eax   ; save pmode CR0
    and eax, 0x7FFFFFFe ; Disable paging bit & disable 16-bit pmode.
    mov cr0, eax
 
    jmp 0:GoRMode       ; Perform Far jump to set CS.
 
GoRMode:
    mov sp, 0x8000      ; pick a stack pointer.
    mov ax, 0       ; Reset segment registers to 0.
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    lidt [idt_real]
    sti         ; Restore interrupts -- be careful, unhandled int's will kill it.
```

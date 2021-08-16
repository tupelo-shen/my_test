[TOC]

## 1 ELF基本格式

ELF 分四块，其中三块是描述信息(也叫头信息)；另一块是内容，放的是所有段/区的内容.

* ELF头定义全局性信息；

* `Segment`(段)头，内容描述段的名字，开始位置，类型，偏移，大小及每段由哪些区组成；

* 内容区，ELF有两个重要概念 `Segment`(段) 和 `Section`(区)，段比区大，二者之间关系如下:

    - 每个Segment可以包含多个Section；
    - 每个Section可以属于多个Segment；
    - `Segment`之间可以有重合的部分；
    - 拿大家熟知的.text，.data，.bss举例，它们都叫区，但它们又属于LOAD段；

* `Section`(区)头，内容描述区的名字，开始位置，类型，偏移，大小等信息

ELF一体两面，面对不同的场景扮演不同的角色，这是理解ELF的关键，链接器只关注1，3(区)，4 的内容，加载器只关注1，2，3(段)的内容；

鸿蒙对`EFL`的定义在` kernel\extended\dynload\include\los_ld_elf_pri.h`文件中：

## 2 ELF关键词（中英文对照）

以下是关于ELF的所有中英名词对照.建议先仔细看一篇再看系列篇部分.

| 中文 | 英文 |
| ---- | ---- |
| 可执行可连接格式  | ELF(Executable and Linking Format) |
| ELF文件头         | ELF header |
| 基地址            | base address |
| 动态连接器        | dynamic linker |
| 动态连接          | dynamic linking |
| 全局偏移量表      | got(global offset table) |
| 进程链接表        | plt(Procedure Linkage Table) | 
| 哈希表            | hash table |
| 初始化函数        | initialization function |
| 连接编辑器        | link editor |
| 目标文件          | object file |
| 函数连接表        | procedure linkage table |
| 程序头            | program header |
| 程序头表          | program header table |
| 程序解析器        | program interpreter |
| 重定位            | relocation |
| 共享目标          | shared object |
| 区(节)            | section |
| 区(节)头          | section header |
| 区(节)表          | section header table |
| 段                | segment |
| 字符串表          | string table |
| 符号表            | symbol table |
| 终止函数          | termination function |

## 3 ELF整体布局

ELF规范中把ELF文件宽泛地称为"目标文件 (object file)"，这与我们平时的理解不同.一般地，我们把经过编译但没有连接的文件(比如Unix/Linux上的`.o`文件)称为目标文件，而ELF文件仅指连接好的可执行文件；在ELF规范中，所有符合ELF格式规范的都称为ELF文件，也称为目标文件，这两个名字是相同的，而经过编译但没有连接的文件则称为"可重定位文件 (relocatable file)"或"待重定位文件 (relocatable file)".本文采用与此规范相同的命名方式，所以当提到可重定位文件时，一般可以理解为惯常所说的目标文件；而提到目标文件时，即指各种类型的ELF文件。

ELF格式可以表达四种类型的二进制对象文件(object files):

* **可重定位文件**(`relocatable file`)，用于与其它目标文件进行连接以构建可执行文件或动态链接库。`可重定位文件`就是常说的目标文件，由源文件编译而成，但还没有连接成可执行文件。在`UNIX`系统下，一般有扩展名"`.o`"。之所以称其为"可重定位"，是因为在这些文件中，如果引用到其它目标文件或库文件中定义的符号（变量或者函数）的话，只是给出一个名字，这里还并不知道这个符号在哪里，其具体的地址是什么。需要在连接的过程中，把对这些外部符号的引用重新定位到其真正定义的位置上，所以称目标文件为"可重定位"或者"待重定位"的。

* **可执行文件**(`executable file`)包含代码和数据，是可以直接运行的程序。其代码和数据都有固定的地址 （或相对于基地址的偏移 ），系统可根据这些地址信息把程序加载到内存执行。

* **共享目标文件**(`shared object file`)，即**动态连接库文件**。它在以下两种情况下被使用：第一，在连接过程中与其它动态链接库或可重定位文件一起构建新的目标文件；第二，在可执行文件被加载的过程中，被动态链接到新的进程中，成为运行代码的一部分。包含了代码和数据，这些数据是在链接时被链接器（ld）和运行时动态链接器（`ld.so.l`、`libc.so.l`、`ld-linux.so.l`）使用的。

* **核心转储文件**(`core dump file`，就是`core dump`文件)。

> <font color="blue">
> 1. 可重定位文件用在编译和链接阶段；
> 2. 可执行文件用在程序运行阶段；
> 3. 共享库则同时用在编译链接和运行阶段；
> </font>

在不同阶段，我们可以用不同视角来理解`ELF`文件，整体布局如下图所示:

<img src="">

从上图可见，`ELF`格式文件整体可分为四大部分:

* `ELF Header`: 在文件的开始，描述整个文件的组织。即`readelf -h app`看到的内容；

* `Program Header Table`： 告诉系统如何创建进程映像。用来构造进程映像的目标文件必须具有程序头部表，可重定位文件可以不需要这个表。表描述所有段(`Segment`)信息，即`readelf -l app`看到的前半部分内容；

* `Segments`：段(`Segment`)由若干区(`Section`)组成。是从加载器角度来描述`ELF`文件。加载器只关心 `ELF header`， `Program header table` 和`Segment`这三部分内容。 在加载阶段可以忽略 `section header table` 来处理程序（所以很多加固手段删除了section header table）；

* `Sections`：是从链接器角度来描述`ELF`文件。链接器只关心 `ELF header`，`Sections` 以及 `Section header table` 这三部分内容。在链接阶段，可以忽略`program header table`来处理文件；

* `Section Header Table`：描述区(`Section`)信息的数组，每个元素对应一个区，通常包含在可重定位文件中，可执行文件中为可选(通常包含) 即`readelf -S app`看到的内容；

* 从图中可以看出 `Segment:Section`(M:N)是多对多的包含关系。`Segment`是由多个`Section`组成，`Section`也能属于多个段。

## 4 ELF头信息

`ELF`头部信息对应`seL4`源码结构体为`Elf32_Ehdr`，文件位于`~/projects/musllibc/include/elf.h`。

```c
#define EI_NIDENT (16)
typedef struct {
  unsigned char e_ident[EI_NIDENT];     /* Magic number and other info */
  Elf32_Half    e_type;                 /* Object file type */
  Elf32_Half    e_machine;              /* Architecture */
  Elf32_Word    e_version;              /* Object file version */
  Elf32_Addr    e_entry;                /* Entry point virtual address */
  Elf32_Off     e_phoff;                /* Program header table file offset */
  Elf32_Off     e_shoff;                /* Section header table file offset */
  Elf32_Word    e_flags;                /* Processor-specific flags */
  Elf32_Half    e_ehsize;               /* ELF header size in bytes */
  Elf32_Half    e_phentsize;            /* Program header table entry size */
  Elf32_Half    e_phnum;                /* Program header table entry count */
  Elf32_Half    e_shentsize;            /* Section header table entry size */
  Elf32_Half    e_shnum;                /* Section header table entry count */
  Elf32_Half    e_shstrndx;             /* Section header string table index */
} Elf32_Ehdr;
```

读取编译后的镜像文件，查看其`ELF`文件头信息：

```
sel4@sel4-VirtualBox:~/sel4test1/cbuild/images$ arm-linux-gnueabi-readelf -h sel4test-driver-image-arm-zynq7000 
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           ARM
  Version:                           0x1
  Entry point address:               0x80f000
  Start of program headers:          52 (bytes into file)
  Start of section headers:          4568468 (bytes into file)
  Flags:                             0x5000200, Version5 EABI, soft-float ABI
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         3
  Size of section headers:           40 (bytes)
  Number of section headers:         21
  Section header string table index: 20
```

> 上面显示的就是`ELF header`中描述的所有内容了。与结构体`Elf32_Ehdr`是一一对应的。`Size of this header: 64 (bytes)`，也就是说：`ELF header` 部分的内容，一共是 `52` 个字节。`52`个字节码长啥样可以用命令
>   
>       od -Ax -t x1 -N 52 sel4test-driver-image-arm-zynq7000
>   
> 看，并对照结构体`Elf32_Ehdr`来理解。
> 
> 其中，命令中的参数为：
> 
> * `-Ax`: 显示地址的时候，用十六进制来表示。如果使用 `-Ad`，意思就是用十进制来显示地址;
> * `-t -x1`: 显示字节码内容的时候，使用十六进制(`x`)，每次显示一个字节(`1`);
> * `-N 52`：只需要读取`52`个字节;

```
& od -Ax -t x1 -N 52 sel4test-driver-image-arm-zynq7000

000000 7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
000010 02 00 28 00 01 00 00 00 00 f0 80 00 34 00 00 00
000020 94 b5 45 00 00 02 00 05 34 00 20 00 03 00 28 00
000030 15 00 14 00
000034
```

这里留意这几个内容，下面会说明，先记住：

```
Entry point address:               0x80f000     /* 代码区，.text起始位置 */
Size of program headers:           32 (bytes)   /* 每个段头大小 */
Number of program headers:         3            /* 段数量 */
Size of section headers:           40 (bytes)   /* 每个区头大小 */
Number of section headers:         21           /* 区数量 */
Section header string table index: 20           /* 字符串数组索引，该区记录所有区名称 */
```


## 5 段头信息（segment）

段(`Segment`)信息对应`seL4`源码结构体为`Elf32_Phdr`：

```c
typedef struct {
  Elf32_Word    p_type;     /* segment type */
  Elf32_Off     p_offset;   /* Segment file offset */
  Elf32_Addr    p_vaddr;    /* Segment virtual address */
  Elf32_Addr    p_paddr;    /* Segment physical address */
  Elf32_Word    p_filesz;   /* Segment size in file */
  Elf32_Word    p_memsz;    /* Segment size in memory */
  Elf32_Word    p_flags;    /* Segment flags */
  Elf32_Word    p_align;    /* Segment alignment */
} Elf32_Phdr;
```
用`readelf -l`查看`sel4test-driver-image-arm-zynq7000`段头部表内容，先看命令返回的前半部分:

```
$ arm-linux-gnueabi-readelf -l sel4test-driver-image-arm-zynq7000 

Elf file type is EXEC (Executable file)
Entry point 0x80f000
There are 3 program headers, starting at offset 52

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  EXIDX          0x01b4a0 0x0081b4a0 0x0081b4a0 0x00008 0x00008 R   0x4
  LOAD           0x000000 0x00800000 0x00800000 0x444cbc 0x444cbc RWE 0x10000
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x10

 Section to Segment mapping:
  Segment Sections...
   00     .ARM.exidx 
   01     .start .text .rodata .ARM.exidx .bss ._archive_cpio .data _driver_list 
   02
```

一共有3个段，与`ELF`头信息一致：

    Number of program headers: 3 /* 段数量 */


## 6 区头信息（section）

区(`section`)信息对应`seL4`源码结构体为`Elf32_Shdr`：

```c
typedef struct {
  Elf32_Word    sh_name;        /* Section name (string tbl index) */
  Elf32_Word    sh_type;        /* Section type */
  Elf32_Word    sh_flags;       /* Section flags */
  Elf32_Addr    sh_addr;        /* Section virtual addr at execution */
  Elf32_Off     sh_offset;      /* Section file offset */
  Elf32_Word    sh_size;        /* Section size in bytes */
  Elf32_Word    sh_link;        /* Link to another section */
  Elf32_Word    sh_info;        /* Additional section information */
  Elf32_Word    sh_addralign;   /* Section alignment */
  Elf32_Word    sh_entsize;     /* Entry size if section holds table */
} Elf32_Shdr;
```

通过`readelf -S`命令读取程序中 `section header table`的内容，如下所示：

```
$ arm-linux-gnueabi-readelf -S sel4test-driver-image-arm-zynq7000 
There are 21 section headers, starting at offset 0x45b594:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .start            PROGBITS        0080f000 00f000 000034 00  AX  0   0  4
  [ 2] .text             PROGBITS        00810000 010000 00a780 00  AX  0   0 4096
  [ 3] .rodata           PROGBITS        0081a780 01a780 000d1f 00   A  0   0  4
  [ 4] .ARM.exidx        ARM_EXIDX       0081b4a0 01b4a0 000008 00  AL  2   0  4
  [ 5] .bss              NOBITS          0081c000 01b4a8 00d048 00  WA  0   0 16384
  [ 6] ._archive_cpio    PROGBITS        00829048 029048 40bc00 00  WA  0   0  1
  [ 7] .data             PROGBITS        00c44c48 444c48 000048 00  WA  0   0  4
  [ 8] _driver_list      PROGBITS        00c44c90 444c90 00002c 00  WA  0   0  4
  [ 9] .comment          PROGBITS        00000000 444cbc 000030 01  MS  0   0  1
  [10] .ARM.attributes   ARM_ATTRIBUTES  00000000 444cec 00002f 00      0   0  1
  [11] .debug_aranges    PROGBITS        00000000 444d20 000518 00      0   0  8
  [12] .debug_info       PROGBITS        00000000 445238 008bbf 00      0   0  1
  [13] .debug_abbrev     PROGBITS        00000000 44ddf7 002afc 00      0   0  1
  [14] .debug_line       PROGBITS        00000000 4508f3 003790 00      0   0  1
  [15] .debug_frame      PROGBITS        00000000 454084 001ab4 00      0   0  4
  [16] .debug_str        PROGBITS        00000000 455b38 0026a6 01  MS  0   0  1
  [17] .debug_ranges     PROGBITS        00000000 4581de 000078 00      0   0  1
  [18] .symtab           SYMTAB          00000000 458258 001d80 10     19 296  4
  [19] .strtab           STRTAB          00000000 459fd8 0014e5 00      0   0  1
  [20] .shstrtab         STRTAB          00000000 45b4bd 0000d6 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  y (purecode), p (processor specific)
```

## 7 字符串表

在 `ELF header` 的最后 `2` 个字节是 `0x14 0x00`，即`20`。它对应结构体中的成员 `e_shstrndx`，意思是这个 `ELF` 文件中，字符串表是一个普通的 `Section`，在这个 `Section` 中，存储了 `ELF` 文件中使用到的所有的字符串。 我们使用`readelf -x`读出下标`20`区的数据:

```
$ arm-linux-gnueabi-readelf -x 20 sel4test-driver-image-arm-zynq7000 

Hex dump of section '.shstrtab':
  0x00000000 002e7379 6d746162 002e7374 72746162 ..symtab..strtab
  0x00000010 002e7368 73747274 6162002e 73746172 ..shstrtab..star
  0x00000020 74002e74 65787400 2e726f64 61746100 t..text..rodata.
  0x00000030 2e41524d 2e657869 6478002e 62737300 .ARM.exidx..bss.
  0x00000040 2e5f6172 63686976 655f6370 696f002e ._archive_cpio..
  0x00000050 64617461 005f6472 69766572 5f6c6973 data._driver_lis
  0x00000060 74002e63 6f6d6d65 6e74002e 41524d2e t..comment..ARM.
  0x00000070 61747472 69627574 6573002e 64656275 attributes..debu
  0x00000080 675f6172 616e6765 73002e64 65627567 g_aranges..debug
  0x00000090 5f696e66 6f002e64 65627567 5f616262 _info..debug_abb
  0x000000a0 72657600 2e646562 75675f6c 696e6500 rev..debug_line.
  0x000000b0 2e646562 75675f66 72616d65 002e6465 .debug_frame..de
  0x000000c0 6275675f 73747200 2e646562 75675f72 bug_str..debug_r
  0x000000d0 616e6765 7300                       anges.
```

可以发现，这里其实是一堆字符串，这些字符串对应的就是各个区的名字。因此`section header table`中每个元素的`Name`字段其实是这个`string table`的索引。为节省空间而做的设计，再回头看看`ELF header`中的 `e_shstrndx`，

    Section header string table index: 20 /* 字符串数组索引，该区记录所有区名称 */

它的值正好就是20，指向了当前的`string table`。

## 8 符号表

`Section Header Table`中，还有一类`SYMTAB(DYNSYM)`区，该区叫符号表。符号表中的每个元素对应一个符号，记录了每个符号对应的实际数值信息，通常用在重定位过程中或问题定位过程中，进程执行阶段并不加载符号表。符号表对应鸿蒙源码结构体为 LDElf32Sym. //kernel\extended\dynload\include\los_ld_elf_pri.h
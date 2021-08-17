[TOC]

## 1 ELF基本格式

`ELF`分四块，其中，三块是描述信息(也叫头信息)；另一块是内容，放的是所有段/区的内容。

* `ELF`头定义全局性信息；

* `Segment`(段)头，内容描述段的名字，开始位置，类型，偏移，大小及每段由哪些区组成；

* 内容区，`ELF`有两个重要概念 `Segment`(段) 和 `Section`(区)，段比区大，二者之间关系如下:

    - 每个Segment可以包含多个Section；
    - 每个Section可以属于多个Segment；
    - `Segment`之间可以有重合的部分；
    - 拿大家熟知的`.text`，`.data`，`.bss`举例，它们都叫区，但它们又属于`LOAD`段；

* `Section`(区)头，内容描述区的名字，开始位置，类型，偏移，大小等信息

`ELF`一体两面，面对不同的场景扮演不同的角色，这是理解ELF的关键。链接器只关注1，3(区)，4 的内容；加载器只关注1，2，3(段)的内容；

鸿蒙对`ELF`的定义在` kernel\extended\dynload\include\los_ld_elf_pri.h`文件中：

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

`ELF`规范中把`ELF`文件宽泛地称为目标文件 (`object file`)，这与我们平时的理解不同。一般地，我们把经过编译但没有连接的文件(比如`Unix/Linux`上的`.o`文件)称为目标文件，而`ELF`文件仅指连接好的可执行文件；在`ELF`规范中，所有符合`ELF`格式规范的都称为`ELF`文件，也称为目标文件，这两个名字是相同的，而经过编译但没有连接的文件则称为"可重定位文件 (`relocatable file`)"或"待重定位文件 (`relocatable file`)"。本文采用与此规范相同的命名方式，所以当提到可重定位文件时，一般可以理解为惯常所说的目标文件；而提到目标文件时，即指各种类型的`ELF`文件。

`ELF`格式可以表达四种类型的二进制对象文件(`object files`):

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
$ arm-linux-gnueabi-readelf -h sel4test-tests
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
  Entry point address:               0x10164
  Start of program headers:          52 (bytes into file)
  Start of section headers:          2211876 (bytes into file)
  Flags:                             0x5000200, Version5 EABI, soft-float ABI
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         7
  Size of section headers:           40 (bytes)
  Number of section headers:         31
  Section header string table index: 30
```

> 上面显示的就是`ELF header`中描述的所有内容了。与结构体`Elf32_Ehdr`是一一对应的。`Size of this header: 52 (bytes)`，也就是说，`ELF header`部分的内容，一共是`52`个字节。`52`个字节码长啥样可以用命令
>   
>       od -Ax -t x1 -N 52 sel4test-tests
>   
> 看，并对照结构体`Elf32_Ehdr`来理解。
> 
> 其中，命令中的参数为：
> 
> * `-Ax`: 显示地址的时候，用十六进制来表示。如果使用 `-Ad`，意思就是用十进制来显示地址;
> * `-t -x1`: 显示字节码内容的时候，使用十六进制(`x`)，每次显示一个字节(`1`);
> * `-N 52`：只需要读取`52`个字节;

```
$ od -Ax -t x1 -N 52 sel4test-tests
000000 7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
000010 02 00 28 00 01 00 00 00 64 01 01 00 34 00 00 00
000020 24 c0 21 00 00 02 00 05 34 00 20 00 07 00 28 00
000030 1f 00 1e 00
000034
```

这里留意这几个内容，下面会说明，先记住：

```
Entry point address:               0x10164      /* 代码区，.text起始位置 */
Size of program headers:           32 (bytes)   /* 每个段头大小 */
Number of program headers:         7            /* 段数量 */
Size of section headers:           40 (bytes)   /* 每个区头大小 */
Number of section headers:         31           /* 区数量 */
Section header string table index: 30           /* 字符串数组索引，该区记录所有区名称 */
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
用`readelf -l`查看`sel4test-tests`段头部表内容，先看命令返回的前半部分:

```
$ arm-linux-gnueabi-readelf -l sel4test-tests

Elf file type is EXEC (Executable file)
Entry point 0x10164
There are 7 program headers, starting at offset 52

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  EXIDX          0x0cb184 0x000db184 0x000db184 0x00120 0x00120 R   0x4
  LOAD           0x000000 0x00010000 0x00010000 0xcb2a8 0xcb2a8 R E 0x1000
  LOAD           0x0cbfd0 0x000dcfd0 0x000dcfd0 0x02fa8 0x13e078 RW  0x1000
  NOTE           0x000114 0x00010114 0x00010114 0x00024 0x00024 R   0x4
  TLS            0x0cbfd0 0x000dcfd0 0x000dcfd0 0x0001c 0x0003c R   0x4
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x10
  GNU_RELRO      0x0cbfd0 0x000dcfd0 0x000dcfd0 0x00030 0x00030 R   0x1

 Section to Segment mapping:
  Segment Sections...
   00     .ARM.exidx 
   01     .note.gnu.build-id .init .text .fini .rodata .ARM.extab .ARM.exidx .eh_frame 
   02     .tdata .init_array .fini_array .got .data _test_case __vsyscall _ps_irqchips .bss 
   03     .note.gnu.build-id 
   04     .tdata .tbss 
   05     
   06     .tdata .init_array .fini_array 
```

一共有7个段，与`ELF`头信息一致：

    Number of program headers: 7 /* 段数量 */

仔细看下这些段的开始地址和大小，发现有些段是重叠的。那是因为一个区可以被多个段所拥有。例如:`0x000dcfd0`对应的 `.tdata`区就被`LOAD`、`TLS`和`GNU_RELRO`三个段所共有。

* `LOAD`
    
    该类型的段描述了可加载到进程空间的代码区或数据区；

再看命令返回内容的后半部分-段区映射关系：

```
 Section to Segment mapping:
  Segment Sections...
   00     .ARM.exidx 
   01     .note.gnu.build-id .init .text .fini .rodata .ARM.extab .ARM.exidx .eh_frame 
   02     .tdata .init_array .fini_array .got .data _test_case __vsyscall _ps_irqchips .bss 
   03     .note.gnu.build-id 
   04     .tdata .tbss 
   05     
   06     .tdata .init_array .fini_array 
```



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
$ arm-linux-gnueabi-readelf -S sel4test-tests
There are 31 section headers, starting at offset 0x21c024:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .note.gnu.build-i NOTE            00010114 000114 000024 00   A  0   0  4
  [ 2] .init             PROGBITS        00010138 000138 000010 00  AX  0   0  4
  [ 3] .text             PROGBITS        00010148 000148 0af904 00  AX  0   0  8
  [ 4] .fini             PROGBITS        000bfa4c 0afa4c 000010 00  AX  0   0  4
  [ 5] .rodata           PROGBITS        000bfa5c 0afa5c 01b68c 00   A  0   0  4
  [ 6] .ARM.extab        PROGBITS        000db0e8 0cb0e8 00009c 00   A  0   0  4
  [ 7] .ARM.exidx        ARM_EXIDX       000db184 0cb184 000120 00  AL  3   0  4
  [ 8] .eh_frame         PROGBITS        000db2a4 0cb2a4 000004 00   A  0   0  4
  [ 9] .tdata            PROGBITS        000dcfd0 0cbfd0 00001c 00 WAT  0   0  4
  [10] .tbss             NOBITS          000dcfec 0cbfec 000020 00 WAT  0   0  4
  [11] .init_array       INIT_ARRAY      000dcfec 0cbfec 000010 04  WA  0   0  4
  [12] .fini_array       FINI_ARRAY      000dcffc 0cbffc 000004 04  WA  0   0  4
  [13] .got              PROGBITS        000dd000 0cc000 000024 04  WA  0   0  4
  [14] .data             PROGBITS        000dd028 0cc028 000978 00  WA  0   0  8
  [15] _test_case        PROGBITS        000dd9a0 0cc9a0 0025c0 00  WA  0   0  4
  [16] __vsyscall        PROGBITS        000dff60 0cef60 000004 00  WA  0   0  4
  [17] _ps_irqchips      PROGBITS        000dff64 0cef64 000014 00  WA  0   0  4
  [18] .bss              NOBITS          000e0000 0cef78 13b048 00  WA  0   0 4096
  [19] .comment          PROGBITS        00000000 0cef78 000030 01  MS  0   0  1
  [20] .ARM.attributes   ARM_ATTRIBUTES  00000000 0cefa8 00002d 00      0   0  1
  [21] .debug_aranges    PROGBITS        00000000 0cefd8 000d18 00      0   0  8
  [22] .debug_info       PROGBITS        00000000 0cfcf0 0bf594 00      0   0  1
  [23] .debug_abbrev     PROGBITS        00000000 18f284 00feca 00      0   0  1
  [24] .debug_line       PROGBITS        00000000 19f14e 0356d8 00      0   0  1
  [25] .debug_frame      PROGBITS        00000000 1d4828 016348 00      0   0  4
  [26] .debug_str        PROGBITS        00000000 1eab70 010750 01  MS  0   0  1
  [27] .debug_ranges     PROGBITS        00000000 1fb2c0 0001d8 00      0   0  8
  [28] .symtab           SYMTAB          00000000 1fb498 0150c0 10     29 4441  4
  [29] .strtab           STRTAB          00000000 210558 00b996 00      0   0  1
  [30] .shstrtab         STRTAB          00000000 21beee 000134 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  y (purecode), p (processor specific)
```

## 7 字符串表

在 `ELF header` 的最后 `2` 个字节是 `0x1E 0x00`，即`30`。它对应结构体中的成员 `e_shstrndx`，意思是这个 `ELF` 文件中，字符串表是一个普通的 `Section`，在这个 `Section` 中，存储了 `ELF` 文件中使用到的所有的字符串。 我们使用`readelf -x`读出下标`20`区的数据:

```
$ arm-linux-gnueabi-readelf -x 30 sel4test-tests

Hex dump of section '.shstrtab':
  0x00000000 002e7379 6d746162 002e7374 72746162 ..symtab..strtab
  0x00000010 002e7368 73747274 6162002e 6e6f7465 ..shstrtab..note
  0x00000020 2e676e75 2e627569 6c642d69 64002e69 .gnu.build-id..i
  0x00000030 6e697400 2e746578 74002e66 696e6900 nit..text..fini.
  0x00000040 2e726f64 61746100 2e41524d 2e657874 .rodata..ARM.ext
  0x00000050 6162002e 41524d2e 65786964 78002e65 ab..ARM.exidx..e
  0x00000060 685f6672 616d6500 2e746461 7461002e h_frame..tdata..
  0x00000070 74627373 002e696e 69745f61 72726179 tbss..init_array
  0x00000080 002e6669 6e695f61 72726179 002e676f ..fini_array..go
  0x00000090 74002e64 61746100 5f746573 745f6361 t..data._test_ca
  0x000000a0 7365005f 5f767379 7363616c 6c005f70 se.__vsyscall._p
  0x000000b0 735f6972 71636869 7073002e 62737300 s_irqchips..bss.
  0x000000c0 2e636f6d 6d656e74 002e4152 4d2e6174 .comment..ARM.at
  0x000000d0 74726962 75746573 002e6465 6275675f tributes..debug_
  0x000000e0 6172616e 67657300 2e646562 75675f69 aranges..debug_i
  0x000000f0 6e666f00 2e646562 75675f61 62627265 nfo..debug_abbre
  0x00000100 76002e64 65627567 5f6c696e 65002e64 v..debug_line..d
  0x00000110 65627567 5f667261 6d65002e 64656275 ebug_frame..debu
  0x00000120 675f7374 72002e64 65627567 5f72616e g_str..debug_ran
  0x00000130 67657300                            ges.
```

可以发现，这里其实是一堆字符串，这些字符串对应的就是各个区的名字。因此`section header table`中每个元素的`Name`字段其实是这个`string table`的索引。为节省空间而做的设计，再回头看看`ELF header`中的 `e_shstrndx`，

    Section header string table index: 30 /* 字符串数组索引，该区记录所有区名称 */

它的值正好就是30，指向了当前的`string table`。

## 8 符号表

`Section Header Table`中，还有一类`SYMTAB(DYNSYM)`区，该区叫符号表。符号表中的每个元素对应一个符号，记录了每个符号对应的实际数值信息，通常用在重定位过程中或问题定位过程中，进程执行阶段并不加载符号表。符号表对应`seL4`源码结构体为`Elf32_Sym`，文件位于`~/projects/musllibc/include/elf.h`。

```c
typedef struct {
  Elf32_Word    st_name;    /* Symbol table name (string tbl index) */
  Elf32_Addr    st_value;   /* Symbol table value */
  Elf32_Word    st_size;    /* Symbol table size */
  unsigned char st_info;    /* Symbol table type and binding */
  unsigned char st_other;   /* Symbol table visibility */
  Elf32_Section st_shndx;   /* Section table index */
} Elf32_Sym;
```
用`readelf -s`读出示例程序中的符号表，如下所示:

```
$ arm-linux-gnueabi-readelf -s sel4test-driver-image-arm-zynq7000 

Symbol table '.symtab' contains 5388 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
   ...省略
  5024: 000169e4   788 FUNC    GLOBAL DEFAULT    3 main
```

`main`函数符号对应的数值为`0x000169e4`，其类型为`FUNC`，大小为`788`字节，对应的代码区索引为`3`。`Section Header Table`：

    [ 3] .text             PROGBITS        00010148 000148 0af904 00  AX  0   0  8

## 9 反汇编代码区

在理解了`String Table`和`Symbol Table`的作用后，通过`objdump`反汇编来理解一下`.text`代码区:

```
sel4test-tests:     file format elf32-littlearm


Disassembly of section .text:

00010148 <exit>:
   10148:   e92d4010    push    {r4, lr}
   1014c:   e1a04000    mov r4, r0
   10150:   eb02a07d    bl  b834c <__funcs_on_exit>
   10154:   eb02a07d    bl  b8350 <__libc_exit_fini>
   10158:   eb02bd33    bl  bf62c <__stdio_exit>
   1015c:   e1a00004    mov r0, r4
   10160:   eb02b041    bl  bc26c <_Exit>

   ......省略

000169e4 <main>:

int main(int argc, char **argv)
{
   169e4:   e92d4800    push    {fp, lr}
   169e8:   e28db004    add fp, sp, #4
   169ec:   e24ddd07    sub sp, sp, #448    ; 0x1c0
   169f0:   e50b01b0    str r0, [fp, #-432] ; 0xfffffe50
   169f4:   e50b11b4    str r1, [fp, #-436] ; 0xfffffe4c
    sel4muslcsys_register_stdio_write_fn(write_buf);
   169f8:   e306038c    movw    r0, #25484  ; 0x638c
   169fc:   e3400001    movt    r0, #1
   16a00:   eb0201f0    bl  971c8 <sel4muslcsys_register_stdio_write_fn>

    test_init_data_t *init_data;
    struct env env;

......(省略)
```

> 反汇编命令中的参数解析：
>
> * `-j .text` ：显示指定`section`为`.text`的区的内容；
> * `-l` ：显示在源代码中的行号和文件名称；
> * `-C` ：符号重组；
> * `-S` ：显示源码；

`0x000169e4`正是`main`函数的入口地址。并看到了激动人心的指令。

但是，还有一个问题，`main`的地址(`0x169e4`)和整个文件显示的入口地址并不一致，这是为什么呢？

```
Entry point address:               0x10164   //代码区 .text 起始位置，即程序运行开始位置
```

我们再次查看符号表，看看`0x10164`地址处到底是何方神圣？

```
4901: 00010164     0 NOTYPE  GLOBAL DEFAULT    3 _start
```

从反汇编中找到`_start`:

```
00010164 <_start>:
.syntax unified

.section .text
.global _start
_start:
    mov fp, 0
   10164:   e3a0b000    mov fp, #0
    mov lr, 0
   10168:   e3a0e000    mov lr, #0

    mov a1, sp
   1016c:   e1a0000d    mov r0, sp
    bl __sel4_start_c
   10170:   eb016ce7    bl  6b514 <__sel4_start_c>

    /* should not return */
1:
    b 1b
   10174:   eafffffe    b   10174 <_start+0x10>
```

`0x10164`地址对应着`_start`，而它又调用了`__sel4_start_c`，继而加载`main`函数。

总结：

* 从内核动态加载的视角看，程序运行首个函数并不是`main`，而是`_start`。
* 但从应用程序开发者视角看，`main`就是启动函数。
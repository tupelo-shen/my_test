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

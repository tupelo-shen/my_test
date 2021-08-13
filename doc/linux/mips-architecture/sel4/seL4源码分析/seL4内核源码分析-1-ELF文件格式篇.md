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
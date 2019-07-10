* [1 查找](#1)
    - [1.1 顺序查找](#1.1)
    - [1.2 二分法查找](#1.2)

* [A 参考资料](#A)
    - [A.1 arm公司官方提供的ARMv7-A体系结构文档](#A.1)
    - [A.2 Uboot中start.S源码的指令级的详尽解析_v1.6.pdf](#A.2)
    - [A.3 ARM指令集快速查询手册.pdf](#A.3)
    - [A.4 ARM指令详解[ARM标准].pdf](#A.4)
    - [A.5 程序员的自我修养—链接、装载与库.pdf](#A.5)

---



<h1 id="A">A 参考资料</h1>

<h2 id="A.1">A.1 arm公司官方提供的ARMv7-A体系结构文档</h2>

[《ARM® Architecture Reference Manual ARM®v7-A and ARM®v7-R》](https://developer.arm.com/docs/ddi0406/c/arm-architecture-reference-manual-armv7-a-and-armv7-r-edition)

这是ARM官方文档，具有相当的权威性。但是如果实在不愿意看英文文档，也可以借鉴杜春雷的《ARM体系结构与编程》。

<h2 id="A.2">A.2 Uboot中start.S源码的指令级的详尽解析_v1.6.pdf</h2>

[Uboot中start.S源码的指令级的详尽解析_v1.6.pdf](https://www.crifan.com/files/doc/docbook/uboot_starts_analysis/release/html/uboot_starts_analysis.html)

文章对Uboot中的Start.S源码进行了逐行解读，而且还建立了相关的论坛，是一个不错的资料。

<h2 id="A.3">A.3 ARM指令集快速查询手册.pdf</h2>

（工具手册，快速查找各种arm指令用法）

<h2 id="A.4">A.4 ARM指令详解[ARM标准].pdf</h2>

（arm汇编的一些规范和常用形式，很多例子非常有用：子程序调用、散转、数据块复制等。对写裸机程序很有帮助，同样对理解uboot和内核启动代码有很大帮助）

<h2 id="A.5">A.5 程序员的自我修养—链接、装载与库.pdf</h2>

（里面会介绍ELF文件  程序链接的过程  最重要的部分就是帮助理解uboot的核心重定位！这个真的很重要！！）
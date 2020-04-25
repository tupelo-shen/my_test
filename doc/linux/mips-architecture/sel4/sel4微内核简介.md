# 1 微内核与宏内核有何异同

微内核——内核只提供任务调度和进程间通信（IPC），当然还包括为任务调度提供服务的系统时钟、中断以及内存管理等最基本的硬件管理能能力。至于用户态需要的系统服务，则由用户态完成。代表OS:L4系列微内核。

宏内核——也称为单内核，所有的系统服务，包括任务调度，系统资源访问等等，都由内核态通过系统调用向用户态提供系统服务。

一张图说明它们的区别：

<img src="https://github.com/tupelo-shen/my_test/blob/master/doc/linux/mips-architecture/sel4/images/whitepaper_1_1.PNG">

总结seL4微内核的优缺点：

1. 优势:

    安全性好、可移植性好、灵活性高、容易debug

2. 劣势:

    性能低

# 2 微内核的发展史

1. 第一代微内核:

    目标是用于解决Unix的可维护性等问题。代表是CMU的Mach——以失败告终，主要原因是性能不足。

2. 第二代微内核:

    目标是解决第一代微内核的性能问题。代表是Liedtke的L3和L4。主要贡献是通过改进IPC机制,大幅提高了性能，并支持运行Linux，即L4 Linux(虚拟化)。

3. 第三代微内核:

    目标是解决第二代微内核的安全性问题，引用了形式化验证，号称世界上最安全的内核OS。代表是OKL4、`seL4`(我们后面的研究重点)。第三代微内核的主要贡献是提出enndpoint的IPC机制、基于capability的权限管理机制，提高了安全性.

# 3 参考资料

如果想要深入了解微内核以及sel4，请参考下面的文章：

1. [sel4白皮书](https://sel4.systems/About/seL4-whitepaper.pdf)
2. [sel4官方资料](https://docs.sel4.systems/Tutorials/)
3. [网友laokz翻译的seL4内核参考手册](https://gitee.com/laokz/sel4_reference_manual/repository/archive/master.zip)
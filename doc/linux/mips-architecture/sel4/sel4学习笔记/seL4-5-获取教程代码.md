[TOC]

# 1 Python依赖

想要获取seL4教程代码，需要安装额外的依赖关系：

    pip install --user aenum
    pip install --user pyelftools

# 2 获取代码

    mkdir sel4-tutorials-manifest
    cd sel4-tutorials-manifest
    repo init -u https://github.com/SEL4PROJ/sel4-tutorials-manifest
    repo sync

# 3 制作教程

查看文件树（`ls`命令）。可以发现，顶层目录包含kernel本身，教程可以在子目录`projects/sel4-tutorials`发现。本教程由一些预先编写的示例应用程序组成，它们故意被编写了一半。通过引导你来填写缺失的部分，从而熟悉seL4代码。但是，对于每一个示例应用程序都有一个完整的解决方案，其中显示了正确的答案，作为参考。完成教程后，将能够使用CMake初始化和构建你自己的解决方案。

创建教程示例的流程一般如下所示：

    # 创建一个教程目录
    mkdir tutorial
    cd tutorial
    # 为一个练习初始化build目录
    ../init --plat <platform> --tut <tutorial exercise>
    # 编译练习示例
    ninja

初始化之后，建立一个等待你填充缺失部分的示例代码。

这部分的练习作业就是根据教程的指导，完成tutorial目录下的应用程序。

<font color="blue">`完整的教程示例`</font>可以通过在`init`命令后面添加`--solution`参数来获取。比如：`../init --plat <platform> --tut <tutorial exercise> --solution`。

# 4 课程列表

## 4.1 课程介绍

在学习课程之前，需要保证前面的几步都已经完成。另外，在完成其它课程之前，先要完成`hello world`课程。`Hello World`课程是seL4工程和课程的指导。完成这一步，可以保证你的配置都是正确的。

## 4.2 seL4机制相关课程

这部分课程主要是介绍seL4微内核所使用的基本机制。通过练习，你可以学习内核API的使用。

1. 能力 - 介绍微内核API中使用的能力
2. Untyped - 用户态内存管理
3. 映射 - seL4微内核中的虚拟内存管理
4. 线程 - 如何使用seL4微内核API启动一个线程
5. IPC - 学习线程间通信机制
6. 通知 - 学习使用通知对象和信号
7. 中断 - 中断处理
8. Fault - 错误处理（如虚拟内存错误）和错误端点

## 4.3 库函数教程

学习由[`seL4_libs`](https://docs.sel4.systems/Tutorials/TODO%20LINK)提供的动态库。这些库没有经过严格的验证，可能存在缺陷。

1. [动态库1](https://docs.sel4.systems/Tutorials/dynamic-1.html) - 用于系统初始化和线程化相关的函数
2. [动态库2](https://docs.sel4.systems/Tutorials/dynamic-2.html) - IPC通信机制相关的函数
3. [动态库3](https://docs.sel4.systems/Tutorials/dynamic-3.html) - 进程管理相关的函数
4. [动态库4](https://docs.sel4.systems/Tutorials/dynamic-4.html) - 定时器和超时相关的函数

指导完成这些课程的幻灯片，保存在下面两个文件中：

* projects/sel4-tutorials/docs/seL4-Overview.pdf

    主要是seL4的设计理念。强烈推荐在开始课程之前完成阅读。

* projects/sel4-tutorials/docs/seL4-APILib-details.pdf

    这是真正的课程指导。

# 5 CAmkES课程

这些课程介绍组件系统`CAmkES`，它允许使用组件配置一个静态系统。`CAmkES`生成用于与seL4交互的粘合代码，专门设计用于构建高保证的静态系统。

这些课程和seL4的课程类似，包括半完成的示例程序，还有一些幻灯片指导说明，也有一些挑战任务。完成的示例程序请参考上面的获取方式获得。

1. [Camkes](https://docs.sel4.systems/Tutorials/hello-camkes-0.html) - 介绍Camkes概念。
2. [Camkes 1](https://docs.sel4.systems/Tutorials/hello-camkes-1.html) - 介绍Camkes概念。
3. [Camkes 2](https://docs.sel4.systems/Tutorials/hello-camkes-2.html) - 介绍Camkes概念。
4. [Camkes 3](https://docs.sel4.systems/Tutorials/hello-camkes-timer.html) - 介绍Camkes硬件组件。

这部分的幻灯片教程位于：`projects/sel4-tutorials/docs/CAmkESTutorial.pdf`

# 6 虚拟机课程

这些教程展示了如何在Camkes上运行Linux guest以及如何在它们之间进行通信。目前这些教程仅针对x86。

1. [Camkes VM Linux](https://docs.sel4.systems/Tutorials/camkes-vm-linux.html) 在Camkes虚拟机中使用Linux作为客户机。

2. [Camkes Cross-VM communication](https://docs.sel4.systems/Tutorials/camkes-vm-crossvm.html) 在不同的虚拟机之间进行通信的演练。

# 7 MCS扩展

MCS扩展是即将对seL4进行的API更改。

1. [MCS](https://docs.sel4.systems/Tutorials/mcs.html) seL4 MCS扩展的介绍。

# 8 后续内容

完成这些课程之后，可以尝试构建和运行[seL4test](https://docs.sel4.systems/seL4Test)。

接下来，可以学习[suggested projects](https://docs.sel4.systems/SuggestedProjects.html) 或者帮助我们扩展构建基于seL4所用的库和组件，请参考[libraries and components](https://docs.sel4.systems/projects/available-user-components.html)。


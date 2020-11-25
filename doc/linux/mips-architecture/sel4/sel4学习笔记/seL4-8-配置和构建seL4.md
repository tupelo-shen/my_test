[TOC]

本文描述了如何使用构建系统搭建一个seL4项目。对于新项目开发，请参考[下一章-合并到构建系统中](TODO)

# 1 基本编译步骤

在seL4工程的根目录下，首先创建一个目录，用于输出编译的二进制文件，然后配置CMake编译参数：

    mkdir build
    cd build
    cmake -DCROSS_COMPILER_PREFIX=arm-linux-gnueabi- -DCMAKE_TOOLCHAIN_FILE=../kernel/gcc.cmake <其它命令行参数> -G Ninja ..

说明：

* `-D`

    定义一个变量，格式很容易理解`X=Y`。

* `CROSS_COMPILER_PREFIX`

    指定交叉编译工具链，完成初始化后不能改变。

* `CMAKE_TOOLCHAIN_FILE`

    指定CMake所使用的工具链文件。该文件应该指定了C编译器，链接器等等。

* `-G Ninja`

    告诉CMake，产生Ninja编译脚本，而不是GNU的Makefile。内核某些部分只支持Ninja脚本。

* `..`目录

    是顶层CMakeLists.txt文件的路径，本示例中也就是放在工程的root目录下。

工程目录下，还提供了一个便利的工具脚本`init-build`，使用方式如下：

    ../init-build -DCROSS_COMPILER_PREFIX=arm-linux-gnueabi- <其它命令行参数>

编译：

    ninja

编译完成后，生成的二进制文件位于`images/`子目录下。

# 2 配置


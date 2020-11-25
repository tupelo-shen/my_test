[TOC]

# 1 要求

搭建好开发环境，参考[seL4-1-搭建环境](TODO)。

# 2 初始化

获取教程源代码和教程使用说明，请参考[`https://docs.sel4.systems/Tutorials/#get-the-code`](https://docs.sel4.systems/Tutorials/#get-the-code):

    # 按照教程使用说明，完成教程的初始化
    # 为练习构建初始化目录
    ./init --tut hello-world
    # 进入编译目录，编译
    cd hello-world_build
    ninja

# 3 学习目标

完成本课程，应该学会：

* 熟悉术语`root`任务；
* 能够编译和模拟seL4工程；
* 对于`CMakeLists.txt`文件的作用有一个基本的认识。

# 4 编译第一个程序

seL4是微内核，并不是操作系统，所以它只提供最小的服务。内核启动后，首先启动一个初始化线程，称为root任务，它负责建立用户系统。当root任务启动后，还没有可用的驱动，但是提供了一个最小的C库供调用。

本教程的示例程序将会打印`Hello world`，所以编译并运行本教程示例代码：

    # 在编译目录下调用ninja
    ninja

编译成功，会有下面的结果输出：

    [180/180] objcopy kernel into bootable elf

运行最终的镜像：

    # 在编译目录下
    ./simulate

这一步会调用QEMU的模拟器，并在生成的模拟器实例上运行编译的可执行文件。如果一切顺利，将会看到：

    Booting all finished, dropped to user space

    Hello, World!

# 5 分析源文件

在hello-world_build目录下，能看到下面的文件：

* CMakeLists.txt

    告诉CMake在生成编译脚本的时候应该包含哪些内容。

* src/main.c

    源代码文件。

* hello-world.md

    就是本文的内容。

## 5.1 CMakeLists.txt

seL4工程中的每个应用和库都要求一个`CMakeLists.txt`文件，将其加入到整个工程的编译构建系统中。我们来分析一下其文件内容：

    # @TAG(DATA61_BSD)
    include(${SEL4_TUTORIALS_DIR}/settings.cmake)
    sel4_tutorials_regenerate_tutorial(${CMAKE_CURRENT_SOURCE_DIR})

    cmake_minimum_required(VERSION 3.7.2)
    # 声明hello-world的CMake工程和它所使用的语言（其实仅使用了C语言）
    project(hello-world C ASM)

    # 在未来的教程中，这些步骤将会被替换为
    # sel4_tutorials_setup_roottask_tutorial_environment()
    find_package(seL4 REQUIRED)
    find_package(elfloader-tool REQUIRED)
    find_package(musllibc REQUIRED)
    find_package(util_libs REQUIRED)
    find_package(seL4_libs REQUIRED)

    sel4_import_kernel()
    elfloader_import_project()

    # 设置环境构建标志并导入musllibc库和运行时库
    musllibc_setup_build_environment_with_sel4runtime()
    sel4_import_libsel4()
    util_libs_import_libraries()
    sel4_libs_import_libraries()
    sel4_tutorials_import_libsel4tutorials()

    # 指定可执行文件的名称和需要的源文件
    add_executable(hello-world src/main.c)

    # 指定应用需要使用的库
    target_link_libraries(hello-world
        sel4runtime sel4
        muslc utils sel4tutorials
        sel4muslcsys sel4platsupport sel4utils sel4debug)

    # 告诉编译构建系统这是一个root任务
    include(rootserver)
    DeclareRootserver(hello-world)

## 5.2 main.c

这就是一个特别典型的main函数。

    #include <stdio.h>
    int main(int argc, char *argv[]) {
        printf("Hello, World!\n");

        return 0;
    }

# 6 尝试改变

尝试向main.c文件添加另一个printf语句，看看效果：

    int main(int argc, char *argv[]) {
        printf("Hello, World!\n");
        printf("Second hello\n");
        return 0;
    }

重新运行`ninja`，编译代码：

    ninja

运行模拟器：

    ./simulate

一旦成功，你能看见下面的内容：

    Second hello


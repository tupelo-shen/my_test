[TOC]

[sel4test](https://github.com/seL4/sel4test-manifest)是seL4的测试工程。

开发环境配置请参考文章[seL4-1-搭建环境](TODO)。

# 1 获取代码

    mkdir sel4test
    cd sel4test
    repo init -u https://github.com/seL4/sel4test-manifest.git
    repo sync
    ls
    # apps/ CMakeLists.txt init-build.sh kernel libs/ projects/ tools/

克隆seL4微内核、测试工程、测试使用的一些库文件和构建过程中使用的一些工具。默认克隆master分支。如果想要克隆特定的版本，需要在`repo init`那行的后面，添加一些参数，比如：

    repo init -u https://github.com/seL4/sel4test-manifest.git -b refs/tags/

更多的版本号信息，请参考[ReleaseProcess](https://docs.sel4.systems/ReleaseProcess#version-numbers)。

# 2 构建

## 2.1 配置

该工程使用CMake编译工具。对于支持的硬件平台和它们相关的CMake配置选项，请参考[Supported Platforms](https://docs.sel4.systems/Hardware)。已经测试过和回归测试的硬件平台的列表由Data61维护。

为了构建特定于硬件平台的配置，我们在工程的根目录下创建一个子目录，使用CMake初始化，如下所示：

    # create build directory
    mkdir build
    cd build
    # configure build directory
    ../init-build.sh -DPLATFORM=<platform-name> -DRELEASE=[TRUE|FALSE] -DSIMULATION=[TRUE|FALSE]

这会为你选择的硬件平台创建必要的CMake配置选项。你可以使用下面的命令，

    ccmake .

查看并编辑CMake编译选项。

需要注意`PLATFORM `和`SIMULATION`选项，会覆盖掉之前的设置。可以通过使能`Sel4testAllowSettingsOverride`，禁止这个功能。

## 2.2 有用的配置选项

有关CMake配置选项，包括配置交叉编译（目标ARM），请参考[seL4’s CMake Build System.](https://docs.sel4.systems/Developing/Building/Using)。

# 3 编译

配置完成后，运行下面的命令进行编译：

    ninja

# 4 运行工程

下面是一个ia32示例，在QEMU模拟器上运行。

配置：

    mkdir ia32_build
    cd ia32_build
    ../init-build.sh -DPLATFORM=ia32 -DRELEASE=TRUE -DSIMULATION=TRUE

编译：

    ninja

运行：

    ./simulate

这一步将会启动QEMU模拟器，并在其上运行我们刚刚编译的镜像。运行结果如下：

    ...
    ELF-loading userland images from boot modules:
    size=0x1dd000 v_entry=0x806716f v_start=0x8048000 v_end=0x8225000 p_start=0x21f000 p_end=0x3fc000
    Moving loaded userland images to final location: from=0x21f000 to=0x12e000 size=0x1dd000
    Starting node #0 with APIC ID 0
    Booting all finished, dropped to user space
    ...
    Starting test 18: BIND0001
    Running test BIND0001 (Test that a bound tcb waiting on a sync endpoint receives normal sync ipc and notification notifications.)
    Test BIND0001 passed
    Starting test 19: BIND0002
    Running test BIND0002 (Test that a bound tcb waiting on its bound notification recieves notifications)
    Test BIND0002 passed
    Starting test 20: BIND0003
    Running test BIND0003 (Test IPC ordering 1) bound tcb waits on bound notification 2, true) another tcb sends a message)
    Test BIND0003 passed
    ...

退出QEMU，使用`Ctrl+a+x`组合键。出现字符串`All is well in the universe`的字样，就证明整个过程成功了。

进一步，我们还可以配置测试工程，以打印JUnit风格的XML文件。然后，就可以运用相关的分析工具进行解析了。为此，使用ccmake编辑构建设置，并使能`Sel4testAllowSettingsOverride`和`LibSel4TestPrintXML`变量。（这一步，留待以后再分析）

# 5 测试用户自定义的内核

假设已经获取seL4微内核的代码到`~/projects/seL4`目录下，而seL4的测试工程位于`~/tests/sel4test`目录下，并且你一直在seL4的一个名为`awesame -new-feature`分支上进行更改。如果想要验证修改后的内核是否还满足seL4test的测试。

    cd ~/tests/sel4tests/kernel
    git remote add feature ~/projects/seL4
    git fetch feature
    git checkout feature/awesome-new-feature
    cd ..

现在，sel4test中的内核已经被改为你自己的内核了。重新编译运行上面的测试过程即可。

# 6 运行测试工程的一个子集

你可以使用正则表达式选择测试的一个子集。这可以通过设置CMake的变量`LibSel4TestPrinterRegex`进行配置。在我们之前的构建目录下，运行`ccmake .`修改这个变量。默认情况下，测试工程运行所有的测试。

[TOC]

本文主要内容：引导一个基本的静态CAmkES应用，描述其组件并将它们链接到一起。

# 1 要求

1. 建立主机开发环境，参考[seL4-1-搭建环境](TODO)

2. 完成[`Camkes hello world`示例](TODO)

# 2 初始化教程

    # For instructions about obtaining the tutorial sources see https://docs.sel4.systems/Tutorials/#get-the-code
    #
    # Follow these instructions to initialise the tutorial
    # initialising the build directory with a tutorial exercise
    ./init --tut hello-camkes-1
    # building the tutorial exercise
    cd hello-camkes-1_build
    ninja

# 3 学习目标

1. 理解一个可描述的，定义好的静态CAmkES应用的结构

2. 理解CAmkES ADL工程的文件布局

3. 掌握创建一个实际的CAmkES应用的知识点

# 4 背景知识

CAmkES的基础是`组件`、`接口`和`连接`。

## 4.1 组件

`组件`是代码和资源的逻辑分组。它们通过接口进行通信，这些接口基于通信通道定义。本教程构建具有两个组件的CAmkES应用：Echo服务器和一个客户端。在上面初始化编译目录的过程中，这些组件已经定义好了。可以在下面的cmakes文件中找到：

* `hello-camkes-1/hello-1.camkes`

关于`组件`的手册可以参考[component](https://github.com/seL4/camkes-tool/blob/master/docs/index.md#component)。

## 4.2 连接

`连接`是CAmkES中的两个软件组件之间通信方法的表达。底层实现可以是共享内存、同步IPC、通知或其它实现的方法。本教程中，我们使用同步IPC。在实现方面，就是调用seL4的`sel4_Call`系统调用。

关于`连接`的参考手册可以看[Connection](https://github.com/seL4/camkes-tool/blob/master/docs/index.md#connection)。

## 4.3 接口

必须很好地定义基于CAmkES的`连接`的所有通信：也就是说，在构建的时候，能够对静态系统的通信进行推理。所有基于这些通信通道的函数调用也必须是定义完好的，并且将其进行逻辑分组，以便能够提供对于所有传输的清晰的方向性理解。虽然，在CAmkES中，组件被连接一起，但是，暴露出来供其它组件调用的接口也需要描述。

接口的类型有：数据端口，过程接口和通知。

本教程主要是学习一下过程接口，这类接口是根据预定义好的API进行函数调用的接口。在CAmkES中，这种接口的关键字是`procedure`。这个过程接口的定义可以在文件`hello-camkes-1/interfaces/HelloSimple.idl4`中找到。

对于`Procedure`可以参考[Procedure](https://github.com/seL4/camkes-tool/blob/master/docs/index.md#procedure)。

## 4.4 组件源文件

基于ADL，CAmkES生成符合系统架构的样板文件，你可以在其中填充自己的逻辑。本文的两个组件的文件是：

* `hello-camkes-1/components/Echo/src/echo.c`
* `hello-camkes-1/components/Client/src/client.c`

现在，当调用在接口规范(`hello- camkes1 /interfaces/HelloSimple.idl4`)中定义的函数时，必须在API函数名前面加上通过特定连接公开的接口实例的名称。

这样做的原因是，一个组件可以多次提供一个接口，而该接口的每个实例都完全引用一个不同的函数。举个例子，如果一个复合设备，比如一个网卡与串口集成到一起，都会暴露两个称为`send()`的过程接口，那么调用者如何知道该调用NIC网卡连接的send还是串口的send函数呢？

因此，CAmkES在函数实例的前面前缀接口实例的名称。比如在上面复合设备的示例中，如果调用者想要调用NIC的发送函数，它就可以`nic_send()`；如果想要调用串口的发送函数，它就可以调用`serial_send()`函数。

所以，假设使用`Echo`声明了一个实例`a`，而`Echo`提供接口`Hello`。那么可以通过调用`a_hello()`来调用`Echo Hello`的`a`实例。但是，如果`Echo`提供了`Hello`接口的两个实例，第二个被命名为`a2`。那么为了调用第二个`Hello`接口实例，需要调用`a2_hello()`。

# 5 练习

首先，修改`hello-1.camkes`文件。在`composition`中定义`Echo`和`Client`两个实例。

    component EmptyComponent {
    }

    component Client {
        control;
        uses HelloSimple hello;
    }

    component Echo {
        provides HelloSimple hello;
    }
    assembly {
        composition {
             component EmptyComponent empty;
             component Client client;
             component Echo echo;

现在，添加一个从`client.hello`到`echo.hello`的连接。

    省略...... 
    assembly {
        composition {
            component EmptyComponent empty;
            component Client client;
            component Echo echo;
            /* 提示1: 使用seL4RPCCall作为连接器（喜欢的话，也可以使用seL4RPC）
             * 提示2: 参考https://github.com/seL4/camkes-tool/blob/master/docs/index.md#creating-an-application
             */
            connection seL4RPCCall hello_con(from client.hello, to echo.hello);

在`interfaces/HelloSimple.idl4`文件中为hello定义接口：

    /* 简单的RPC接口 */
    procedure HelloSimple {
        /* TODO 定义RPC函数 */
        /* 提示1: 定义一个函数，使用一个字符串作为输入，命名为say_hello，没有返回值
         * 提示2: 参考https://github.com/seL4/camkes-tool/blob/master/docs/index.md#creating-an-application
         */
    };

实现RPC say_hello函数，位于echo.c文件中：

    /* @TAG(DATA61_BSD) */
    /*
     * CAmkES教程 2: 带有RPC的组件。服务器部分。
     */
    #include <stdio.h>

    /* 为我们的组件产生的头文件 */
    #include <camkes.h>
    /* TASK 5: 实现 RPC 函数 */
    /* 提示 1: 函数名称是一个接口名称和函数名称的组合，比如: <接口>_<函数>
     * 提示 2: 可用的接口由组件定义，比如，hello-1.camkes文件中
     * 提示 3: 函数名称定义位于接口定义中，比如，interfaces/HelloSimple.idl4文件中定义的HelloSimple接口
     * 提示 4: 所以，函数的名称是hello_say_hello()
     * 提示 5: CAmkES的'string'类型对应C语言的'const char *'
     * 提示 6: 使用printf使函数打印出一个消息
     * 提示 7: 参考https://github.com/seL4/camkes-tool/blob/master/docs/index.md#creating-an-application
     */
    void hello_say_hello(const char *str) {
        printf("Component %s saying: %s\n", get_instance_name(), str);
    }

在`components/Client/src/client.c`文件中调用RPC函数：

    /* @TAG(DATA61_BSD) */
    /*
     * CAmkES tutorial part 1: components with RPC. Client part.
     */

    #include <stdio.h>

    /* 为我们的组件产生的头文件 */
    #include <camkes.h>

    /* run the control thread */
    int run(void) {
        printf("Starting the client\n");
        printf("-------------------\n");
        /* TODO: 调用 RPC 函数 */
        /* 提示 1: 调用的函数名称是一个接口名称和函数名称的组合，比如: <接口>_<函数>
         * 提示 2: 可用的接口由组件定义，比如，hello-1.camkes文件中
         * 提示 3: 函数名称定义位于接口定义中，比如，interfaces/HelloSimple.idl4文件中定义的HelloSimple接口
         * 提示 4: 所以，函数的名称是hello_say_hello()
         * 提示 5: 参考https://github.com/seL4/camkes-tool/blob/master/docs/index.md#creating-an-application
         */
        char *shello = "hello world";
        hello_say_hello(shello);

        printf("After the client\n");
        return 0;
    }

# 6 编译运行

编译，运行：

    # ninja
    # ./simulate

结果：

    Starting the client
    -------------------
    Component echo saying: hello world
    After the client


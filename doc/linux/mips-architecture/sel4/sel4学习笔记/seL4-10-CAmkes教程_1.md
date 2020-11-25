[TOC]

本文主要介绍CAmkES。了解CAmkES语法，引导一个基本的静态CAmkES应用程序并描述其组件。

# 1 要求

1. 建立主机开发环境，参考[seL4-1-搭建环境](TODO)

2. [`Hello world`示例](TODO)

3. 翻阅过 [CAmkES 手册](https://github.com/seL4/camkes-tool/blob/master/docs/index.md)。请注意，即使没有阅读手册，也可以成功地完成CAmkES教程，但还是强烈推荐阅读。

# 2 学习目标

1. 理解一个可描述的，定义好的静态CAmkES应用的结构

2. 理解CAmkES ADL工程的文件布局

3. 掌握创建一个实际的CAmkES应用的知识点

# 3 背景知识

CAmkES的基础就是：组件、接口和连接。组件就是代码和资源的逻辑分组；它们通过定义好的接口与其它组件进行通信，这些接口必须通过通信通道静态定义。

## 3.1 组件

下面是组件的一个具体示例：

    component foo {
        control;
        uses MyInterface a;
        attribute Int b;
    }

这儿，我们创建了一个组件，其类型是foo。后面，我们就可以使用foo类型定义一个组件实例。比如，

    component foo bar

这条语句就是定义了一个组件实例bar，其类型是foo。

## 3.2 描述一个静态系统：Assembly、Composition和Configuration

在CAmkES中，我们通常看到三个层次的元素：即`Assembly`、`Composition`和`Configuration`。我们使用这几个概念去描述一个定义完备的静态系统。首先，我们使用术语`Assembly`对我们的静态系统作一个完整描述。在CAmkES架构描述语言（ADL）中，使用`Assembly`作为系统定义的最顶层的元素。每个CAmkES工程必须至少一个`assembly`定义。示例如下所示：

    assembly {
        composition {
            component foo bar;
        }

        configuration {
            bar.b = 0;
        }
    }

在上面的示例中，我们还看到了`Composition`和`Configuration`的身影。`Composition`就好比是一个容器，可以封装我们之前提到的组件和连接器实例。在上面，我们声明了一个foo组件的实例，将其命名为bar。`Configuration`元素用于描述对给定的系统属性进行设置。

# 4 创建第一个CAmkES应用

我们还是使用'Hello world'示例中的代码。我们创建一个CAmkES组件，当启动的时候，这个组件会打印`Hello CAmkES World`。

## 4.1 源代码

源文件列表：

* `CMakeLists.txt` - 定义如何构建CAmkES应用程序的文件
* `client.c` - 我们的“Hello World”客户端组件的单个源文件
* `hello.camkes` - 描述静态系统的CAmkES文件

### 4.1.1 client.c

代码比较简单：

    #include <stdio.h>

    /* 产生的组件头文件 */
    #include <camkes.h>

    /* 运行控制线程 */
    int run(void) {
        printf("Hello CAmkES World\n");
        return 0;
    }

注意，上面的client.c文件代替了之前的main.c文件。我们将代码放在了`run()`函数中，它是CAmkES组件的入口。

### 4.1.2 hello.camkes

描述静态CAmkES系统的文件。这类文件都必须遵循CAmkES语法。这儿比较简单，内容如下：

    component Client {
        control;
    }

    assembly {
        composition {
            component Client client;
        }
    }

在上面的代码中，我们创建了只有一个简单组件的静态系统。定义了组件`Client`，并声明了一个组件实例`client`。

### 4.1.3 CMakeLists.txt

该文件的作用就不再累述。本示例中的`CMakeLists.txt`应该具有如下内容：

    # @TAG(DATA61_BSD)
    include(${SEL4_TUTORIALS_DIR}/settings.cmake)
    sel4_tutorials_regenerate_tutorial(${CMAKE_CURRENT_SOURCE_DIR})

    cmake_minimum_required(VERSION 3.7.2)

    project(hello-camkes-0 C ASM)

    find_package(camkes-tool REQUIRED)
    camkes_tool_setup_camkes_build_environment()

    DeclareCAmkESComponent(Client SOURCES client.c)

    DeclareCAmkESRootserver(hello.camkes)

    GenerateCAmkESRootserver()

该文件声明了Client组件，将其与文件`client.c`源文件链接起来。另外，还使用`hello.camkes`声明了`CAmkES Root Server `。

## 4.2 编译运行

编译：

    ninja

模拟运行：

    ./simulate

能够看到下面的结果：

    Hello CAmkES World

至此本课程结束。




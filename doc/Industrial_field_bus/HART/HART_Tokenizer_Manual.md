<h1 id="0">0 目录</h1>

* [1 关于本手册](#1)
    - [1.1 范围](#1.1)
    - [1.2 必要知识](#1.2)
    - [1.3 组织结构](#1.3)
    - [1.5 获取帮助](#1.5)
* [3 关于本手册](#3)
    - [3.1 DD开发环境](#3.1)
        + [3.1.1 DD源文件](#3.1.1)
        + [3.1.2 标准DD](#3.1.2)
        + [3.1.3 用户自定义DD](#3.1.3)
    - [3.2 编写DD](#3.2)
    - [3.3 DEVICES.CFG](#3.3)
* [4 调用Tokenizer](#4)
    - [4.1 使用IDE调用Tokenizer](#4.1)
    - [4.2 使用命令行调用Tokenizer](#4.2)
        + [4.2.1 使用命令行的规则](#4.2.1)
        + [4.2.2 命令行选项](#4.2.2)
    - [4.3 编译输出](#4.3)
    - [4.4 编译后的发布目录](#4.4)

---

<h1 id="1">1 关于本手册</h1>

<h2 id="1.1">1.1 范围</h2>

1. 本手册涵盖的Tokenizer版本为5、6、8、10。
2. 本手册不包含EDDL语法、EDDL源代码练习、生成的编码后的DD文件格式。

<h2 id="1.2">1.2 必要知识</h2>

在阅读本手册之前，假定读者已经理解：

1. 一个抽象设备的标准HART模型
2. EDDL的语法和结构
3. 怎样使用EDDL语言编写一个设备描述源文件
4. 标准设备描述和现场设备描述的区别

<h2 id="1.3">1.3 组织结构</h2>


* 第2章 引言
    
    综述部分，Tokenizer的关键组件以及使用它们的相关概念。

* 第3章 开发DD
    
    描述Tokenizer的工作目录，所使用的文件以及怎样编译你的DD。

* 第4章 Tokenizer引用
    
    描述怎样调用Tokenizer和命令行参数。

* 附录A 版本历史
    
    描述了从版本5开始的变化。

* 附录B 符号表
    
    描述了符号文件的创建和结构。

* 附录C 增强设备描述语言注解
    
    提供开发DD的额外指导。

* 附录D release目录
    
    理解输出目录的结构。

* 附录E Tokenizer消息
    
    描述Tokenizer产生的所有消息。

<h2 id="1.5">1.5 获取帮助</h2>

可以从官网获取帮助：[https://support.fieldcommgroup.org](https://support.fieldcommgroup.org)。

<h1 id="3">3 开发DD</h1>

<h2 id="3.1">3.1 DD开发环境</h2>

开发DD所需要的环境：

1. FDI包-IDE，包含支持所有FCG协议的Tokenizer以及相关文件。
2. HART-IDE，只支持HART的开发环境，包含HART的Tokenizer以及相关文件。
3. 两个IDE都提供的构建一个DD所提供的相关源文件和库文件。

这些IDE都是完整的DD开发环境，包含编辑器、浏览器、编译器和支持的库。它们会安装相关的支持文件到指定的目录结构中。关于目录结构请参考原文。

每一个IDE包含编译HART-DD的所有文件，以及不同Tokenizer版本所对应的那些变化文件。这些文件被分为两类：

* 源文件
* 编译后的DD库文件

<h3 id="3.1.1">3.1.1 DD源文件</h3>

DEV目录包含源文件和其它文件：

* MACROS.H 
    
    包含一些有用的宏：访问Common Table，执行METHOD操作以及PV、SV、TV、FV的定义。

* METHODS.H
    
    包含EDDL内嵌函数返回的代码。（内嵌函数指的是被METHOD项调用的函数）

* 示例代码
    
    阐释如何构建DD。

<h3 id="3.1.2">3.1.2 标准DD</h3>

RELEASE目录包含编译后的标准DD文件和其它一些相关文件：

* 标准DD（MANUFACTURER，0x000000）
* SYMBOLS.TXT
    
    包含已经注册过的符号名称，符号类型和每种设备类型的编码（这类文件夹按照设备类型编码进行布局）。

* STANDARD.DCT

    常用工业术语的词典

* STANDARD.DC8
    
    常用工业术语的词典，专门针对Tokenizer8及其以后版本。

<h3 id="3.1.3">3.1.3 用户自定义DD</h3>

用户需要自己编写的DD文件：

* DEVICES.CFG
    
    允许Tokenizer把DD声明的MANUFACTURER翻译成编号。

* YOURDEVICE.DDL
    
    用户自定义设备的主文件，包含设备的标识符，可以导入其它DD语句，也可以包含#include其它文件。

* FILENAME.DD 
    
    自定义的DD源文件

* .h、.gif和其它需要构建的源文件

<h2 id="3.2">3.2 编写DD</h2>

写DD的过程：

    了解你的设备 -> 编写DD -> 编译DD（包含标准DD等文件） -> 二进制DD文件

1. 理解设备
    
    因为DD是对设备信息的模型化，所以必须充分理解式样。

2. 文件拆分

    一般的工程都非常庞大，不可能一个文件写完，所以需要把其细分。使用#include把所有的文件关联起来。文件的命名要与内容对应起来。

3. 专用预处理宏
    
        __TOKVER__

    有时候，我们定义的内容可能根据Tokenizer的版本不同而有细微的差别，需要区分，比如

        VARIABLE wp_code
        {
            HELP "Write protect disable/change password";
            LABEL "Write Protect Status"
            CLASS LOCAL;
            HANDLING READ&WRITE;
            TYPE ENUMERATED
            #if __TOKVER__ >= 602
                DEFAULT_VALUE WP_READ;
            #endif
        }

    因为，Tokenizer从6.2版本开始接受DEFAULT_VALUE这个属性。条件编译#if指令允许EDD可以使用早期版本的Tokenizer编译。

<h2 id="3.3">3.3 DEVICES.CFG</h2>

DEVICES.CFG定义制造商和设备类型编码。这些编码应该是在协会注册过的。你必须添加你的设备类型在该文件的设备列表中。

它都包含：

* 制造商定义-MANUFACTURER
* 设备类型-DEVICE_TYPE，一个制造商可以用多个设备类型

示例：

    MANUFACTURER 0x000037 = YOKOGAWA
    DEVICE_TYPE 0x3760 = _ROTAMASS_TI
    DEVICE_TYPE 0x371a = _AXG4A
    DEVICE_TYPE 0x371b = _AXW4A
    DEVICE_TYPE 0x371c = _AXG1A

在上面的例子中，0x000037是我司YOKOGAWA的编码，16进制形式。`_AXG4A`和编码0x371a是一对，名称前面有一下划线。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="4">4 调用Tokenizer</h1>

本章主要描述：

1. 使用Tokenizer编译DD
2. Tokenizer的命令行选项
3. Tokenizer输出-错误信息请参考附录E

<h2 id="4.1">4.1 使用IDE调用Tokenizer</h2>

具体参加各个IDE的使用说明。

<h2 id="4.2">4.2 使用命令行调用Tokenizer</h2>

语法：
    
    TOK [选项] 文件名称

在这儿，TOK可以是

| HART Tokenizer 版本 | TOK |
| ------------------- | ------------ |
| 5                   | HCFTOK32.EXE |
| 6                   | DDT.EXE |
| 8                   | DDT8.EXE |
| 10                  | DDTA.EXE |

文件的后缀一般是.DDL。

<h3 id="4.2.1">4.2.1 使用命令行的规则</h3>

1. 选项和文件名称之间使用空格隔开
2. 命令选型一个个指定，不能一起指定，前缀连字符-

比如，

1. 正确
    
        TOK -u -r usr_dev.ddl

2. 错误
    
        TOK -ur usr_dev.ddl
    
<h3 id="4.2.2">4.2.2 命令行选项</h3>

| 命令行选项 | 语法 | 意义 |
| ---------- | ---- | ---- |
| @<optfile> | DDT @myoptions.opt univ6.ddl | 从文本文件中读取参数选项。 |
| -c         | -c | 把输出文件拷贝到当前工作目录 |
| -e         | -e error-filename | 把标准错误信息输出到指定文件中 |
| -h         | -h | 获取帮助信息 |
| -i         | -i | 输出预编译后的文件 |
| -k         | -k message-number | 屏蔽掉你不想看到的信息，使用信息编号 |
| -l         | -l | 输出不合法的UTF-8字符串 |
| -t         | -t directory-name | 指定一个临时目录，保存Tokenizer的临时文件 |
| -v         | -v | 使Tokenizer向错误文件发出扩展的诊断信息 |
| -C         | -C | 保留注释信息 |
| -R         | -R directory-name | 改变release目录，如果没有指定，默认.\Library |
| -U         | -U macro-name | 禁止某个宏 |
| -W         | -W integer | 指定显示消息类型，默认是1，Warning、Error和Fatal |

<h2 id="4.3">4.3 编译输出</h2>

| Tokenizer版本 | 编译后缀 |
| ------------- | -------- |
| 5 | .out |
| 6 | .fms |
| 8 | .fm8 |
| 10| .fma |

<h2 id="4.4">4.4 编译后的发布目录</h2>

位于DEV\LIBRARY\目录下，按照制造商和设备类型编码的分类进行目录分类。


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

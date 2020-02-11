<h1 id="0">0 目录</h1>

* [<s>1 范围</s>](#1)
    - [<s>1.1 一致性要求</s>](#1.1)
    - [<s>1.2 本规范中对DDL语法的符号表示</s>](#1.2)
    - [<s>1.3 版本控制</s>](#1.3)
* [<s>2 引用</s>](#2)
    - [<s>2.1 HART DDL规范</s>](#2.1)
    - [<s>2.2 HART现场通信协议规范</s>](#2.2)
    - [<s>2.3 相关文档</s>](#2.3)
* [<s>3 定义</s>](#3)
* [4 符号/缩略语](#4)
* [5 综述](#5)
* [6 DD基本元素](#6)
    - [6.1 识别符](#6.1)
    - [6.2 头文件](#6.2)
    - [6.3 导入-使用DD模板](#6.3)
    - [6.4 声明DD项](#6.4)
* [7 词法约定](#7)
    - [7.1 转译阶段](#7.1)
    - [7.2 DDL源字符集](#7.2)
    - [7.3 符号-Token](#7.3)
    - [7.4 标识符](#7.4)
    - [7.5 常量和文字](#7.5)
    - [7.6 字符串](#7.6)
    - [7.7 预处理机](#7.7)
    - [7.8 条件](#7.8)
    - [7.9 表达式](#7.9)
* [8 数据模型化](#8)
    - [8.1 变量](#8.1)
    - [8.2 数组](#8.2)
    - [8.3 列表](#8.3)
    - [8.4 集合](#8.4)
    - [8.5 文件](#8.5)
    - [8.6 二进制大对象](#8.6)
    - [8.7 关系](#8.7)
* [9 COMMAND](#9)
* [10 用户接口](#10)
* [11 METHOD](#11)
* [12 模板](#12)
* [13 环境](#13)


---

<h1 id="1">1 范围</h1>

<h2 id="1.1">1.1 一致性要求</h2>

<h2 id="1.2">1.2 本规范中对DDL语法的符号表示</h2>

定义本文中使用的DDL语法，以方便阅读。比如说，关键词、分号和花括号使用粗体，定义的变量名称等使用斜体等。

<h2 id="1.3">1.3 版本控制</h2>
<h1 id="2">2 引用</h1>
<h2 id="2.1">2.1 HART DDL规范</h2>
<h2 id="2.2">2.2 HART现场通信协议规范</h2>
<h2 id="2.3">2.3 相关文档</h2>
<h1 id="3">3 定义</h1>

1. DD Host Application
    
    能够使用DD并理解DDL语言的主机应用程序。


<h1 id="4">4 符号/缩略语</h1>

1. DD - Device Description
2. DDL - Device Description Language
3. HCF - HART Communication Foundation
4. SOP - Standard Operating Procedure
5. GUI - Graphical User Interface
6. HMI - Human Machine Interface

<h1 id="5">5 综述</h1>

1. DDL结构
    
    从逻辑上划分为`data`、`HMI`、`通信模型`。另外，和维护、校正、试运行和配置设备有关的标准操作过程（SOP）使用`METHOD`结构实现。

2. 数据模型
    
    使用`VARIABLE`、`ARRAY`、`COLLECTION`、`FILE`、`LIST`组织数据。

    关系（比如`UNIT`和`REFRESH`）用来表示数据之间的依赖。

    `VARIABLE`、`value-ARRAY`和`LIST`用来描述设备中数据空间（例如，设备使用的内存）。

    `reference-ARRAY`和`COLLECTION`主要用于组织数据，并允许在具有多种操作模式的设备中进行间接引用。

    `FILE`允许DD指定一些数据存储在主机上，这些数据可以表示一个现场设备。

3. 通信模型-COMMAND
    
    包含：HART命令号，transaction，response code。

    * 事务 - 使用数据引用定义。这些引用可以是直接引用（`VARIABLE`）或间接引用（`ARRAY`、`COLLECTION`和`LIST`。
    * 主机应用程序使用命令访问设备数据，保持数据更新。由DD描述的设备模型为主机应用程序提供了提供了足够的信息，由其决定何时发送哪个命令。

4. HMI支持
    
    就是提供信息，明确指出设备应该如何呈现给终端用户。

    * `MENU`是HMI的基本结构。每个MENU包含一组逻辑相关的项以及指导如何呈现给用户的指令。可以通过为大部分的DD项（菜单、变量、图片等）提供内容帮助信息，从而为用户提供指导和支持。
    * `EDIT_DISPLAY`是用来补充`MENU`结构的，提供类似弹出式对话框显示，用来显示消息数据和需要编辑的数据。
    * `GRAPH`和`CHART`用来显示数值和文本数据。
        - `GRAPH`用来刻画`WAVEFORM`数据。
        - `CHART`用来刻画一项数据随时间的变化曲线。
    * HMI相关结构本身是可移植的、技术上是独立的，可以运行在任何主机平台上，不管是手持设备还是计算机。
 
5. SOP-标准操作过程
    
    使用`METHOD`表示。
    
第6章描述了DD的基本元素。其余章节描述了DDL语言的语法和语义。第8到11章描述了语言的基本结构：变量、数组、列表、集合、文件、关系、命令、菜单、编辑显示对话框、图形数据表示和方法。第7章描述了DDL语言的条件、表达式、字符串和词法约定。

<h1 id="6">6 DD基本元素</h1>

DD描述的基本结构如下：

    device-description:
        identification  definition-list

    identification:
        menufacturer, device-type, device-revision, DD-revision

    menufacturer:
        MENUFACTURER uppercase-identifier

    device-type:
        DEVICE_TYPE _uppercase-identifier

    device-revision:
        DEVICE_REVISION integer

    DD-revision:
        DD_REVISION integer

    definition-list:
        definition
        definition-list definition
    definition:
        imported-description
    item
    like

***

<h2 id="6.1">6.1 识别符</h2>

用来获取menufacturer、device-type、device-revision、DD-revision的信息，其中
    
1. **menufacturer**: HART通信基金会获取；
2. **device-type**: 制造商指定，但是需要遵循HART协议要求；

    `menufacturer`和`device-type`的值都是大写，且`device-type`的值前面前缀一个下划线。定义位于`Devices.cfg`文件中，具体的格式如下：

        menufacturer-block:
            menufacturer-definition device-type-list 

        menufacturer-definition:
            MENUFACTURER hexadecimal-integer = uppercase-identifier

        device-type-list:
            device-type-definition
            device-type-list        device-type-definition

        device-type-definition:
            DEVICE_TYPE hexadecimal-integer = _uppercase-identifier

3. **device-revision**: 如果设备增加数据或者命令，就要修改该值；
4. **DD-revision**: DD和现场设备是一一对应的关系。每次修改DD都要修改该值。

DD主机通过`Identify`命令识别该标识符（具体参见 *Network Management Specification*）。该命令包含`Menufacturer ID`、`device-type`、和`device-revision`。


<h2 id="6.2">6.2 头文件</h2>

具有预处理程序。

使用`#include`包含头文件，标准头文件比如`macros.h`、`methods.h`等，DD特定的头文件需要自定义。

使用`#define`指令定义宏。

<h2 id="6.3">6.3 导入-使用DD模板</h2>

提供标准DD，用来模型化HART应用层。

IMPORT-导入已经存在的DD作为模板。通常用于常用的表、通用和常用的规范之类的。

<h2 id="6.4">6.4 声明DD项</h2>

DD项的基本结构：

    ddl-keyword identifier { attribute, attribute, ...}

1. ddl-keyword，比如VARIABLE之类的DDL关键词；
2. identifier，比如low_flow_cutoff之类的项名称，用户定义；
3. attribute，比如LABEL，HELP，TYPE之类的属性；

示例：

    VARIABLE low_flow_cutoff
    {
        LABEL "Low flow cutoff";
        HELP "对此项有帮助的说明性文字";
        TYPE FLOAT
        {
            DISPLAY_FORMAT "6.4f";   /* ##.#### */
        }
        CONSTANT_UNIT "%";
        HANDLING READ & WRITE;
    }

<h1 id="7">7 词法约定</h1>

<h2 id="7.1">7.1 转译阶段</h2>

表1. 转译阶段

| 步骤 | 描述 |
| ---- | ---- |
| 1    | 处理DD源文件并将文件结束符替换为换行符。|
| 2    | 删除换行符和它前面的一个反斜杠，将强制换行的行拼接成逻辑DD行。|
| 3    | 将源文件分解成符号和空白字符组成的序列（包括注释）。换行符保留。|
| 4    | 执行预处理指令和宏展开。#include指令会将包含进来的文件按照1-4步递归展开。|
| 5    | 相邻的字符串拼接。|
| 6    | 所有的IMPORT用LIKE结构解析。|
| 7    | 从语法和语义上解析由空白字符分割的符号。这时候，会引用标准字典和IMAGE文件。|
| 8    | 把所有结果输出到一个文件。|

<h2 id="7.2">7.2 DDL字符集</h2>

支持 *空白字符*、*水平制表符*、*垂直制表符*、*换页符*、*！*、*换行*、*回车*、*null(0x00)*再加上91个可见字符。

这些字符的约束不包括字符串，DDL中的字符串可以支持多国语。

<h2 id="7.3">7.3 符号-Token</h2>

Token是DD转译阶段使用的最小词语。像 *空白字符*、*水平制表符*、*垂直制表符*、*换页符*和 *注释*会被忽略。

Token包括，

1. 标识符
2. 预处理指令
3. 表达式
4. 条件语句
5. 整数
6. 浮点常量
7. 字符串常量

<h2 id="7.4">7.4 标识符</h2>

标识符可以使DDL关键字，为DD项定义或引用的符号，标准词典字符串句柄，或者成员定义或引用。

<h3 id="7.4.1">7.4.1 关键词</h3>

请参考英文规范中的内容。

<h3 id="7.4.2">7.4.2 符号和符号范围</h3>

所有的DD项具有一个或多个标识符（非关键字）。这些标识符被称为符号。比如DD项的名称和成员的名称。符号只对定义它的DD（包含修订版本）可见，不能被其它DD引用。但是，标准DD（查看[第12章](#12)）可以通过IMPORT导入任何DD，因此，它们中的符号也可以被使用。

<h2 id="7.5">7.5 常量和文字</h2>

DDL支持整数和浮点数常量以及字符串。

<h2 id="7.5.1">7.5.1 整数</h2>

整数可以使用后缀表示类型，比如有无符号、以及它是几进制的之类。

1. 10进制整数
2. 16进制整数
3. 8进制整数
4. 2进制整数

默认情况下，整数以最小的类型（比如，int、long int）表示值，l或L表示整数被存储为long型。

<h3 id="7.5.2">7.5.2 浮点数</h3>

浮点数有4部分组成：整数部分、小数点部分、小数部分、指数部分。

满足科学计数法。

<h3 id="7.5.3">7.5.3 字符串</h3>

字符串使用`ISO Latin-1(ISO 8859-1)`或`UTF-8(RFC 3629)`编码。其它要求跟我们理解的编程语言中的C习惯差不多。特殊字符需要转义。具体的转义字符可以查阅英文文档。比如，`"abc"`。

1. 语言编码（Language Coding）

    比如，`| en | English   |it| Italian`

    还有特殊的语言编码，具体查看英文文档。应该不常用。

<h2 id="7.6">7.6 字符串</h2>

<h3 id="7.6.1">7.6.1 字符串引用</h3>

指定字符串的方法：

1. 字符串常量
2. 枚举标识符
    
    语法：`enumeration-identifier (integer)`

3. 字典引用，用于从字典中获取的显示字符，比如之前的多国语开发。
    
    语法：`[ identifier ]`

4. 字符串变量标识符
    
    语法：`string-variable-identifier`

5. 属性字符串引用
    
    语法：`attribute-string-reference`

<h3 id="7.6.2">7.6.2 标准字典</h3>

符合标准字符串的要求，如果需要支持特殊语言，开发者可以把对应的语言的字符串添加到标准字典中。

<h2 id="7.7">7.7 预处理机</h2>

预处理机符合ANSI C++的标准。

<h3 id="7.7.1">7.7.1 注释</h3>
<h3 id="7.7.2">7.7.2 #define</h3>
<h3 id="7.7.3">7.7.3 #if-#ifdef-#ifndef</h3>
<h3 id="7.7.4">7.7.4 #line</h3>
<h3 id="7.7.5">7.7.5 #error</h3>
<h3 id="7.7.6">7.7.6 #pragma</h3>
<h3 id="7.7.7">7.7.7 #include</h3>
<h3 id="7.7.8">7.7.8 macros.h头文件</h3>

<h2 id="7.8">7.8 条件语句</h2>

<h3 id="7.8.1">7.8.1 if条件语句</h3>

语法：

    IF(expression) {
        then-clause
    }
    ELSE {
        subclause
    }

<h3 id="7.8.2">7.8.2 选择语句</h3>

    SELECT (expression) {
    CASE constant-expression :
        clause
    CASE constant-expression :
        clause
    DEFAULT :
        clause
    }

<h2 id="7.9">7.9 表达式</h2>

<h3 id="7.9.1">7.9.1 基本表达式</h3>

主要包括：

1. 整数
2. 浮点数
3. 引用
4. 对变量的引用，比如，`upper_range_value.MAX_VALUE`。
5. (expression)

<h3 id="7.9.2">7.9.2 一目表达式</h3>

主要有，

1. `-`表示算术上的负值意义
2. `~`按位取反
3. `!`逻辑负

<h3 id="7.9.3">7.9.3 二目表达式</h3>

主要有

1. 乘除法
2. 加减法
3. 移位操作
4. 比较
5. 位操作
6. 逻辑操作


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="8">8 数据模型化</h1>

<h2 id="8.1">8.1 变量</h2>

`VARIABLE`结构是模型化设备数据的基本构件。变量name必须有，方便引用。共支持10个属性，TYPE必须，其余可选，见下面的代码段。关于属性的具体细节可以参考附录A。

比如，

    VARIABLE name {
        LABEL           constant-string;
        HELP            constant-string;
        CLASS           class-name & class-name & ...;
        VALIDITY        boolean-specifier;
        HANDLING        handling-specifier;
        CONSTANT_UNIT   constant-string;
        DEFAULT_VALUE   constant-expression;
        TYPE            variable-type;
        pre/post action
        REFRESH_ACTION { method, method, ...}
    }

<h2 id="8.1.1">8.1.1 类型</h2>

类型有：

1. integer、unsigned integer - 整形
2. float 、double - 浮点型
3. enumerated、bit enumerated - 枚举类型
4. index
5. ASCII、Packed-ASCII、password - 字符串类型
6. date

其中，浮点型和整形统称为算术型。

1. 算术型示例
    
        TYPE FLOAT
        {
            DISPLAY_FORMAT "6.4f";   /* ##.#### */
        }

    可以指定可选项，可选项有，

    1. DISPLAY_FORMAT
    2. EDIT_FORMAT
    3. MIN_VALUE
    4. MAX_VALUE
    5. SCALING_FACTOR

2. 枚举型
    
    通用语法：

        TYPE ENUMERATED (size)_opt {
            { value, description, help_opt},
            ...
        }

    1. size 是大于0的整数。指定变量的大小，单位字节。size是可选的，默认值1。
    2. value 是整数值，和设备中的合法值匹配
    3. description 显示的字符串
    4. help 提示信息。
    
    举例：

        TYPE ENUMERATED
        {
            { 6, [ykgw_com_enum_sts_bit_not_supt] }, //This is the default value
            { 0, [ykgw_com_enum_no_effect] },
            { 1, [ykgw_com_enum_mtac_required] },
            { 3, [ykgw_enum_failure] },
            { 4, [ykgw_com_enum_out_of_spec] },
            { 5, [ykgw_com_enum_fuc_chk] }
        }

3. 位枚举型

    通用语法：

        TYPE BIT_ENUMERATED (size)_opt {
            { value, description, help_opt, class_opt, status-class_opt, actions_opt },
            ...
        }    

    1. size =枚举型。
    2. value 指定第几位
    3. description 显示的字符串
    4. help 提示信息。
    5. class 指定该位的功能分类。默认与变量的class相同。

        当BIT_ENUMERATED型的变量是 CLASS DIAGNOSTIC的时候，所有的位就是状态指示标志。置位表示红色指示，复位状态显示为绿色。比如，在一个STYLE WINDOW类型的MENU上，这些位就可以使用红绿LED样式的指示器表示了。

    6. status-class 指定了该位的意义。一个状态位可以是多种状态类的一种：cause，duration，correctability，scope，output等。比如severity(严重程度)的取值。

        1. INFO
        2. WARNING
        3. ERROR
        4. IGNORE_IN_HOST
        5. IGNORE_IN_HANDHELD

    7. actions 指定了置位后执行的方法。

    举例：

        TYPE    BIT_ENUMERATED(4) 
        {
            { 0x00000008,   [ykgw_enum_13_mbrd_eep_default_on] }
            { 0x00010000,   [ykgw_enum_21_pwm1_stop_on] }
            { 0x00020000,   [ykgw_enum_22_pwm2_stop_on] }
            { 0x00040000,   [ykgw_enum_23_obrd_mismat_on] }
            { 0x00080000,   [ykgw_enum_24_opt_eep_fail] }
            { 0x00100000,   [ykgw_enum_25_obrd_ad_fail_on] }
            { 0x00200000,   [ykgw_enum_26_obrd_spi_fail_on] }
            { 0x02000000,   [ykgw_enum_28_ibrd_fail_on] }
            { 0x04000000,   [ykgw_enum_29_ind_eep_fail] }
            { 0x08000000,   [ykgw_enum_30_lcd_drv_fail_on] }
            { 0x10000000,   [ykgw_enum_31_ibrd_mismat_on] }
            { 0x20000000,   [ykgw_enum_32_ind_comm_err_on] }
            { 0x40000000,   [ykgw_enum_33_msd_fail_on] }
        }

4. INDEX类型
    
    index类型标志在array或者list中的索引（查看[8.2](#8.2)和[8.3](#8.3)）。它的值只能是数据或者列表中的值。如果超出边界自动会有一个FALSE的VALIDITY。

    语法：

        TYPE INDEX (size)_opt array-or-list;

    举例：

        TYPE INDEX (2) ASCII_32_array;

5. 字符串类型
    
    ASCII、Packed-ASCII、password有这3种。

    语法：

        TYPE PACKED-ASCII length;
        TYPE ASCII length;
        TYPE PASSWORD length;

6. DATE类型
    
    语法：

        TYPE DATE;

    DATE类型的VARIABLE被标识为3个字节的无符号整数。第1个字节->day；第2个字节->month；第3个字节->year，其中year=真实年-1900。

7. TIME类型
    
    语法：

        TYPE TIME_VALUE {
            DISPLAY_FORMAT_opt string;
            EDIT_FORMAT_opt string;
            TIME_FORMAT_opt string;
            TIME_SCALE_opt time-scale-qualifier;
        }

<h2 id="8.1.2">8.1.2 默认值</h2>

请参考英文文档查找即可。

<h2 id="8.2">8.2 数组</h2>

可以是变量、集合或menu的表。其中每一个元素应该是相同类型、大小和形状。

对数组引用的语法：

    array-name [ index ]

<h2 id="8.3">8.3 列表</h2>

<h2 id="8.4">8.4 集合</h2>

<h2 id="8.5">8.5 文件</h2>

<h2 id="8.6">8.6 二进制大对象</h2>

<h2 id="8.7">8.7 关系</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="9">9 COMMAND</h1>

<h2 id="9.1">9.1 事务</h2>

<h3 id="9.1.1">9.1.1 数据项掩码</h3>

<h3 id="9.1.2">9.1.2 数据项限定符</h3>

<h2 id="9.2">9.2 响应码</h2>

<h3 id="9.2.1">9.2.1 响应码类型</h3>

<h1 id="10">10 用户接口</h1>
<h1 id="11">11 METHOD</h1>
<h1 id="12">12 模板</h1>
<h1 id="13">13 环境</h1>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

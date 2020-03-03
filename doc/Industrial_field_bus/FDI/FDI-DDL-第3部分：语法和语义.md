<h1 id="0">0 目录</h1>

* [5 EDD和EDDL模型](#5)
    - [5.1 EDD和EDDL综述](#5.1)
    - [5.2 EDD架构](#5.2)
    - [5.3 EDD概念](#5.3)
    - [5.4 EDD开发过程](#5.4)
    - [5.5 词汇结构与形式定义之间的相互关系](#5.5)
    - [5.6 内嵌](#5.6)
    - [5.7 配置](#5.7)
* [7 概述](#7)
    - [7.1 EDDL的特点](#7.1)
    - [7.2 语法表达](#7.2)
    - [7.3 EDDL组件](#7.3)
    - [7.4 基本组件](#7.4)
    - [7.5 通用属性](#7.5)
    - [7.6 特殊组件](#7.6)
    - [7.7 实例规则](#7.7)
    - [7.8 变量列表规则](#7.8)
* [10 概述](#10)
    - [<font color="green">10.1 COMMAND</font>](#10.1)
    - [10.2 RECORD](#10.2)
    - [10.3 RESPONSE_CODES](#10.3)
    - [<font color="green">10.4 VARIABLE_LIST</font>](#10.4)
* [13 概述](#13)
    - [13.1 BLOCK_A](#13.1)
    - [13.2 BLOCK_B](#13.2)
    - [<font color="green">13.3 LIKE</font>](#13.3)
    - [13.4 IMPORT](#13.4)
    - [13.5 TEMPLATE](#13.5)
    - [13.6 COMPONENT](#13.6)
    - [13.7 COMPONENT_FOLDER](#13.7)
    - [13.8 COMPONENT_REFERENCE](#13.8)
    - [13.9 COMPONENT_RELATION](#13.9)
    - [13.10 INTERFACE](#13.10)
***

<h1 id="5">5 EDD和EDDL模型</h1>

<h2 id="5.1">5.1 EDD和EDDL综述</h2>

EDD包含自动系统中一个组件的所有参数。描述EDD的技术称为电子设备描述语言（EDDL）。EDDL提供了一组可扩展的基本语言元素处理简单的、复杂的或模块化的设备。EDDL是基于ASCII格式的描述语言，能够清晰区分数据和程序。

使用国家编码（比如Japanese）进行标记的文本中的数据，可能使用多个字节表示。

<h2 id="5.2">5.2 EDD架构</h2>

从ISO/OSI模型（标准ISO/IEC 7498-1）来看，EDD位于第7层。但是EDD应用程序使用通信系统传输它的信息。EDD包含支持映射到通信层的架构。

设备制造商定义的对象，是由EDD应用程序中的对象逻辑表示反应出来的。基于这个原因，EDDL提供了语言基本元素，可以将EDD数据映射到通信层的数据表示，以至于EDD的用户无需知道设备对象的物理地址。

EDD描述了将要显示给用户的信息的管理。但是，这类可视化的特殊表示不是EDD或EDDL定义的一部分。

<h2 id="5.3">5.3 EDD概念</h2>

设备制造商使用EDDL语言描述设备的属性。产生的EDD应该包含下面的信息：

1. 设备参数的描述；
2. 参数依赖的描述；
3. 设备参数的逻辑分组；
4. 支持功能的选择和执行；
5. 业务逻辑；
6. 数据集的描述。

依据使用场景，EDD可能位于：

1. 设备中；
2. 外部存储介质，比如硬盘，软盘或服务器；
3. 分开存储在设备和外部存储介质中。

EDD支持文本字符串（常用术语、短语等），支持多国语（英语、德语、法语等）。文本字符串可以存储在独立的字典中，一个EDD可以有多个字典。

EDD实现中包含足够的信息识别设备，比如，制造商、设备类型和版本都是被用来匹配设备的信息。

<h2 id="5.4">5.4 EDD开发过程</h2>

创建EDD分为3步：EDD源文件作成、EDD预处理和EDD编译。


1. EDD源文件作成

    设备制造商为设备编写正确的EDD并交付，这样的文件可以通过EDD应用程序进行浏览。

2. EDD预处理

    为编译阶段做准备。

3. EDD编译
    
    从预处理的EDD编译成可以在EDD应用程序中使用的内部表示。


<h2 id="5.5">5.5 词汇结构与形式定义之间的相互关系</h2>

The lexical structure of EDDL and its elements are described in Clauses 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20. Formal definitions and syntax for each EDDL element are given in Annex A. The lexical structure and its formal description use the same name.

<h2 id="5.6">5.6 内嵌</h2>

Builtins are predefined subroutines which are executed in the EDD application.

The library of Builtins is defined in FCG TS61804-5.

<h2 id="5.7">5.7 配置文件</h2>

EDDL is a unified specification of existing legacy EDD concepts. Concrete EDD applications use a subset of the EDDL specification. The elements of the specification applicable to various profiles are described in tables throughout the document, along with the description of the element.

EDDL是现有
In addition to EDD profiles, implementing consortia also publish “Device Profiles”, which are used to support the interchangeability of compliant devices. These Device Profiles may be described using the EDDL specified in this document.

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="6">6 约定</h1>

<h2 id="6.1">6.1 通用</h2>

In FCG TS61804 series all keywords are written in uppercase letters. An EDDL element is written in lowercase letters to address the semantic of the whole element.

The EDDL is generally described using lexical structures in which the elements and the presence of fields are specified.

<h2 id="6.2">6.2 基本结构描述的约定</h2>

The following conventions apply for the basic construct definition.

* Table 1 defines the format for the Builtin overview table.
* Table 2 defines the contents of the columns of Table 1.
* Table 3 defines the usage of attributes of basic constructs or their subattributes.
* EDDL basic constructs and attributes referenced in this document are written in upper case letters.


<h2 id="6.3">6.3 词法结构的约定</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="7">7 概述</h1>

EDDL是结构化、可解释性的语言，用来描述设备属性。设备和EDD实时运行环境的交互也被合并到EDDL中。为了达成这些目的，EDDL提供了一组语言元素。

For a specific EDD implementation it is not necessary to use all of the elements provided by the language.
对于某一个特定的EDD实现，不必实现所有的元素。

Compatible subsets of EDDL are permitted and may be specified using profiles (for example, choice of constructs, number of recursions, and selection of options). EDD developers are required to identify within each EDD details as to which profile has been used.



<h2 id="7.1">7.1 EDDL的特点</h2>

<h2 id="7.2">7.2 语法表达</h2>

<h2 id="7.3">7.3 EDDL组件</h2>

<h2 id="7.4">7.4 基本组件</h2>

<h3 id="7.4.1">7.4.1 通用</h3>

这些基本组件用来支持工业控制应用中使用的设备的描述、属性和功能。

为了和已有的几个描述语言兼容，增加了一些基本组件，虽然名称和功能类似，但是与各自的详细规范不同。使用配置文件指定具体使用哪个组件。

每一个基本组件都有与其相关的一组属性。属性又可以有子属性，加强了组件的定义。

属性的定义可以是static或者dynamic。静态属性定义永远不会改变，而动态属性定义可能随着参数值变化而变化。

<h3 id="7.4.2">7.4.2 AXIS</h3>

AXIS描述CHART或GRAPH的坐标。（[第11章](#11)）

<h3 id="7.4.3">7.4.3 BLOB</h3>

描述了二进制大对象，用来与设备交互二进制数据的。（见[第9.8节](#9.8)）

<h3 id="7.4.4">7.4.4 BLOCK_A</h3>

BLOCK_A is a logical grouping of CHARACTERISTICS, PARAMETERS, PARAMETER_LISTS, and ITEM_LISTS, see Figure 2. To access one item of BLOCK_A, the instance of the block should be used (see 13.1 and Figure 2).

<h3 id="7.4.5">7.4.5 BLOCK_B</h3>
<h3 id="7.4.6">7.4.6 CHART</h3>
<h3 id="7.4.7">7.4.7 COLLECTION</h3>
<h3 id="7.4.8">7.4.8 COMMAND</h3>
<h3 id="7.4.9">7.4.9 COMPONENT</h3>
<h3 id="7.4.10">7.4.10 COMPONENT_FOLDER</h3>
<h3 id="7.4.11">7.4.11 COMPONENT_REFERENCE</h3>
<h3 id="7.4.12">7.4.12 COMPONENT_RELATION</h3>
<h3 id="7.4.13">7.4.13 EDIT_DISPLAY</h3>
<h3 id="7.4.14">7.4.14 FILE</h3>
<h3 id="7.4.15">7.4.15 GRAPH</h3>
<h3 id="7.4.16">7.4.16 GRID</h3>
<h3 id="7.4.17">7.4.17 IMAGE</h3>
<h3 id="7.4.18">7.4.18 IMPORT</h3>
<h3 id="7.4.19">7.4.19 INTERFACE</h3>
<h3 id="7.4.20">7.4.20 LIKE</h3>
<h3 id="7.4.21">7.4.21 LIST</h3>
<h3 id="7.4.22">7.4.22 MENU</h3>
<h3 id="7.4.23">7.4.23 MWTHOD</h3>
<h3 id="7.4.24">7.4.24 PLUGIN</h3>
<h3 id="7.4.25">7.4.25 RECORD</h3>



<h3 id="7.4.26">7.4.26 REFERENCE_ARRAY</h3>
<h3 id="7.4.27">7.4.27 Relations</h3>
<h3 id="7.4.28">7.4.28 RESPONSE_CODES</h3>
<h3 id="7.4.29">7.4.29 SOURCE</h3>
<h3 id="7.4.30">7.4.30 TEMPLATE</h3>
<h3 id="7.4.31">7.4.31 VALUE_ARRAY</h3>
<h3 id="7.4.32">7.4.32 VARIABLE</h3>
<h3 id="7.4.33">7.4.33 VARIABLE_LIST</h3>
<h3 id="7.4.34">7.4.34 WAVEFORM</h3>

<h2 id="7.5">7.5 通用属性</h2>

<h2 id="7.6">7.6 特殊组件</h2>

<h2 id="7.7">7.7 实例规则</h2>

<h2 id="7.8">7.8 变量列表规则</h2>

<h1 id="10">10 通信结构</h1>

<h2 id="10.1">10.1 COMMAND</h2>

<font color="red"> CS的一个完整概念，FF没有这个概念。其余协议原先部分支持</font>

COMMAND就是把数据映射到通信结构中。COMMAND指定与设备交互的数据。可以写到设备的数据在REQUEST属性中实现。从设备读回来的数据在REPLY属性中。它指定了构建一个通信帧所需要的的各种元素。

> 注意1:通信帧的地址字段使用可选的结构BLOCK、SLOT、SUB_SLOT、NUMBER和INDEX等指定（没有的就算了）。通信帧的类型或者控制字段使用OPERATION结构指定。数据内容在TRANSACTION结构中指定。

数据结构

    COMMAND identifier
    {
        API         [integer-constant | reference | integer-expression]<cond> ;
        BLOCK_B     reference<cond> ;
        HEADER      string-value<cond>
        INDEX       [integer-constant | reference | integer-expression]<cond> ;
        NUMBER      integer-constant ;
        OPERATION   [COMMAND | READ | WRITE] ;
        RESPONSE_CODES  { [referenced-response-codes-specifier | response-codes-specifier] }
        SLOT            [integer-constant | reference | integer-expression]<cond> ;
        SUB_SLOT        [integer-constant | reference | integer-expression]<cond> ;
        TRANSACTION [integer]?
        {
            REQUEST         { request-item-specifier [, request-item-specifier]* }
            REPLY           { reply-item-specifier [, reply-item-specifier]* }
            RESPONSE_CODES  { [referenced-response-codes-specifier | response-codes-specifier] }
            POST_RQSTRECEIVE_ACTIONS { actions-specifier }
        }
    }

<h2 id="10.2">10.2 RECORD</h2>

<font color="red"> 仅FF和ISA100有这个概念。</font>

是用来描述一个通信对象的，包含一组逻辑变量。RECORD中的每一个成员都是一个变量的引用，变量可以是不同的类型。通过RECORD标识符和成员标识符，在EDD的任何地方都可以引用。

数据结构：

    RECORD identifier
    {
        LABEL           string-value ;
        HELP            string-value ;
        MEMBERS         { members-specifier }
        PRIVATE         boolean-specifier ;
        RESPONSE_CODES  referenced-response-codes-specifier ;
        VALIDITY        boolean-specifier ;
        VISIBILITY      boolean-specifier ;
        WRITE_MODE      write-mode-specifier ;
    }

<h2 id="10.3">10.3 RESPONSE_CODES</h2>

<font color="red"> HART没有这个概念。在HART中，就是COMMAND结构的一个属性，不是一个结构单元，没有标识符，不能被其它结构引用</font>

RESPONSE_CODES指定设备返回的错误信息值，每一个VARIABLE、RECORD、VALUE_ARRAY、VARIABLE_LIST或COMMAND都有自己的一组RESPONSE_CODES.

数据结构：

    RESPONSE_CODES identifier
    {
        [[integer , response-code-type , description , [help]?]<cond> ;]+
    }
    response-code-type := [DATA_ENTRY_ERROR | DATA_ENTRY_WARNING | MISC_ERROR | MISC_WARNING | MODE_ERROR | PROCESS_ERROR | SUCCESS]

<h2 id="10.4">10.4 VARIABLE_LIST</h2>

<font color="red"> FF和ISA100没有这个概念。</font>

VARIABLE_LIST是一组EDD通信对象（VARIABLE，VALUE_ARRAY或RECORDS）。方便应用程序组织对象而设计的一个结构。VARIABLE_LIST是通过将多个项组成更少的通信对象而优化通信。

数据结构

    VARIABLE_LIST identifier
    {
        LABEL string-value ;
        HELP string-value ;
        MEMBERS { members-specifier }
        RESPONSE_CODES referenced-response-codes-specifier ;
    }

<h1 id="13">13 设备模型</h1>

<h2 id="13.1">13.1 BLOCK_A</h2>


<h2 id="13.2">13.2 BLOCK_B</h2>

<h2 id="13.3" ><font color="green">13.3 LIKE</font></h2>
<h2 id="13.4">13.4 IMPORT</h2>
<h2 id="13.5">13.5 TEMPLATE</h2>
<h2 id="13.6">13.6 COMPONENT</h2>
<h2 id="13.7">13.7 COMPONENT_FOLDER</h2>
<h2 id="13.8">13.8 COMPONENT_REFERENCE</h2>
<h2 id="13.9">13.9 COMPONENT_RELATION</h2>
<h2 id="13.10">13.10 INTERFACE</h2>

<h1 id="15">15 重定义</h1>

<h2 id="15.1">15.1 概述</h2>


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
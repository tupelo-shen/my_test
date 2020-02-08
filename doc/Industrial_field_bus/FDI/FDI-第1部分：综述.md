<h1 id="0">0 目录</h1>

* [1 范围](#1)
* [2 参考规范](#2)
* [3 术语、定义、缩略语](#3)
    - [3.1 术语和定义](#3.1)
    - [3.2 IEC TR 62541-1 术语(OPC UA)](#3.2)
    - [3.3 IEC 62541-3 (OPC UA) terms](#3.3)
    - [3.4 IEC 62541-4 (OPC UA) terms](#3.4)
    - [3.5 IEC 62541-5 (OPC UA) terms](#3.5)
    - [3.6 IEC 62541-100 (OPC UA for Devices) terms](#3.6)
    - [3.7 缩略语](#3.7)
    - [3.8 习惯](#3.8)
* [4 背景](#4)
    - [4.1 动机](#4.1)
    - [4.2 EDDL](#4.2)
    - [4.3 FDT](#4.3)
    - [4.4 OPC UA](#4.4)
* [5 架构](#5)
    - [5.1 基于.NET实时运行库](#5.1)
        + [5.1.1 基于.NET的FDI类型库](#5.1.1)
    - [5.2 基于HTML5的实时运行库](#5.2)
        + [5.2.1 基于HTML5的FDI类型库](#5.2.1)
* [6 部署](#6)
    - [6.1 工作站](#6.1)
    - [6.2 移动终端](#6.2)
* [7 FDI Host](#7)
* [8 生命周期模型](#8)
* [9 附录A](#9)

*** 

<h1 id="1">1 范围</h1>

<h1 id="2">2 参考规范</h1>

<h1 id="3">3 术语、定义、缩略语</h1>

<h2 id="3.1">3.1 术语和定义</h2>

<h1 id="4">4 背景</h1>

<h2 id="4.1">4.1 动机</h2>

在当今的自动化系统领域，来自许多不同供应商的设备必须集成到系统中，这导致了对这些设备的安装、版本管理和操作的额外工作。这一挑战最好通过开放和标准化的设备集成解决方案来解决。

目前，2种不同的设备集成技术共存：遵循IEC-61804的EDDL和遵循IEC-62453的FDT。这些技术采用不同的方案解决问题，但它们之间有许多重合的地方。这导致了两种技术相互竞争而不是互相补充。结果，系统提供商提供了这些方案，设备供应商不得不付出两倍的努力去支持EDDL和FDT，而终端用户想要使用这两种技术而感到沮丧。

对于所有相关方来说，理想的解决方案是不同的。系统提供商希望实现健壮性，同时确保高水平的技术和平台独立性。设备供应商希望支持一种技术，而不是两种，以减少成本和投入，提供最佳的操作设备的方法。用户想要避免错误的投资，因此想要一个面向未来的，具有所有竞争技术优势的方案。

<h2 id="4.2">4.2 EDDL</h2>

EDDL是一种描述现场设备的语言。它使系统能够配置、校准、故障诊断和操作设备，而无需事先了解设备。

使用EDDL写的设备描述，描述了现场设备的能力；由系统决定如何利用这些功能。这些设备描述使系统能够访问所有设备的数据和属性，这样，简化了设备的维护、支持和操作。同时支持手持设备应用和大型集成自动化系统。另外，对于嵌入式系统和运行在商业操作系统上的系统都能很好的支持。

使用EDDL，设备提供商可以组织设备的数据、属性和用户访问的过程。这为动态创建用户界面提供了系统指导。对于不同类型的设备，这个用户界面功能可能很不同，可简单，也可复杂。

在20世纪90年代早期，EDDL的第一个版本被创建出来，并被用来描述HART field设备。在1996年，EDDL被用来描述基金会现场总线设备。然后在2000年，它被用来描述PROFIBUS设备。所有三个版本的EDDL都可以追溯到原始的HART版本。因此，这三个版本在很大程度上是相同的，但是由于底层通信协议的不同而存在一些差异。2004年3月，EDDL作为IEC 61804-3和IEC 61804-4的一部分首次标准化。

<h2 id="4.3">4.3 FDT</h2>

FDT®是一个接口规范，规范了设备软件和系统之间的接口。它独立于通信协议，并在设备供应商提供的软件和系统供应商提供的软件之间建立明确的界限。

在FDT规范中，现场设备由设备特定的软件组件(称为设备类型管理器-DTM)提供，该组件仅在与基于FDT的框架应用程序（比如，YOKOGAWA公司的FieldMate）结合使用时才有用。框架应用程序通过一组标准FDT接口与DTM交互。

设备供应商可以为每一个设备开发一个DTM，也可以为一组设备开发一个DTM。使用DTM可以访问设备参数、配置和操作设备和诊断设备。DTM可以是用于设置设备参数的简单图形用户界面(GUI)，也可以是用于执行复杂计算进行诊断的非常复杂的应用程序。

为了支持模块化设备，可以嵌套DTM。DTM的嵌套还允许多级通信层次结构。通过不同总线协议路由的设备可以通过标准接口连接。一个设备DTM仅必须支持它自己的通信协议。连接到设备DTM的网关DTM处理协议转换。

FDT®规范支持多种总线协议，例如:PROFIBUS、HART、基金会现场总线、Interbus、AS-interface、IO- link、DeviceNet和PROFINET IO。

<h2 id="4.4">4.4 OPC UA</h2>

OPC Unified Architecture (OPC UA) is a platform-independent standard through which various kinds of systems and devices can communicate by sending messages between clients and servers over various types of networks. It supports robust, secure communication that assures the identity of clients and servers and resists attacks.

OPC UA defines standard sets of services that servers can provide, and individual servers specify to clients what service sets they support. The services act on an object model which is managed by the server and discoverable by a client. Information is conveyed using standard and vendor-defined data types, and servers define object models that clients can dynamically discover. Servers can provide access to both current and historical data, as well as alarms and events to notify clients of important changes.

OPC UA can be mapped onto a variety of communication protocols and data can be encoded in various ways to trade off portability and efficiency. Transports and encodings for XML based Web Services as well as a high performance binary are defined for OPC UA. The abstraction of the OPC UA standard from any particular technology provides future-proofing allowing OPC UA to be mapped onto future technologies.

The integration of system components includes a “how” factor and a “what” factor. The comprehensive set of services provided by OPC UA enables the “how” of system integration. OPC UA also provides the basic building blocks of the “what” of system integration by defining an extensible object model. Other standards bodies, vendors, and end users can extend this object model to achieve a tight integration between system components.

OPC UA is standardized in IEC 62541.

<h1 id="5">5 架构</h1>

<h2 id="5.1">5.1 综述</h2>

FDI架构由3部分组成：FDI包、FDI客户端、FDI服务器，如下图所示：

<img id="Pic_1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_PART1_1_1.PNG">

图1-1 FDI架构框图

<h2 id="5.2">5.2 FDI包</h2>
<h2 id="5.3">5.3 FDI客户端</h2>
<h2 id="5.4">5.4 FDI服务器</h2>
<h2 id="5.5">5.5 FDI通信服务器</h2>
<h2 id="5.6">5.6 用户接口分层</h2>
<h2 id="5.7">5.7 FDI安全考虑</h2>
<h2 id="5.8">5.8 冗余</h2>
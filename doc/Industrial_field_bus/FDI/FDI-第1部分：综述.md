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

FDT® is an interface specification that standardizes the interface between the device software and the systems. It provides independence from the communication protocol and establishes a clear boundary between the software provided by the device supplier and the software provided by the system supplier.

In FDT®, field devices are delivered with a device-specific software component called a Device Type Manager (DTM), which is only functional when used in conjunction with an FDT®-specific environment called a "frame application". A frame application interacts with a DTM through a set of standard FDT® interfaces.

A device supplier can develop a DTM for each of its devices, or it can develop a DTM for a group of devices. A DTM can be used to access Device Parameters, configure and operate the device, and diagnose problems. A DTM can range from a simple Graphical User Interface (GUI) for setting Device Parameters to a highly sophisticated application for performing complex calculations for diagnosis.

DTMs can be nested in order to support Modular Devices. The nesting of DTMs also allows multi-level communication hierarchies to be supported. Devices routed through different bus protocols can be connected through standard interfaces. A device DTM just has to support its own communication protocol. Gateway DTMs that connect to the device DTM handle protocol transformation.

The FDT® specification supports a variety of bus protocols, for example: PROFIBUS, HART, FOUNDATION Fieldbus, Interbus, AS-interface, IO-Link, DeviceNet, and PROFINET IO.

In 1998, the specification phase started in the context of the Zentralverband Elektrotechnik und Elektronikindustrie e. V. (ZVEI). In 1999, completion of the technology was accelerated when the specification was adopted by PROFIBUS Nutzerorganisation e. V. (PNO), which later transferred the rights to the FDT Group AISBL. FDT® was standardardized as IEC 62453-1 in May 2009.

<h2 id="4.4">4.4 OPC UA</h2>
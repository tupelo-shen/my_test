<h1 id="0">0 目录</h1>

* [1 引言](#1)
* [2 FDI技术](#2)
    - [2.1 FDI包](#2.1)
    - [2.2 EDDL](#2.2)
* [3 FDI主机](#3)
    - [3.1 FDI主机-C/S架构](#3.1)
    - [3.2 单机版FDI主机](#3.2)
    - [3.3 基于FDT的FDI主机](#3.3)
    - [3.4 分层网络-嵌套通信](#3.4)
* [4 已有方案的迁移和重用](#4)
    - [4.1 EDD](#4.1)
    - [4.2 DTM](#4.2)
* [5 增加互操作性的工具和标准化组件](#5)
    - [5.1 集成开发环境-IDE](#5.1)
    - [5.2 标准组件](#5.2)
* [6 优点](#6)

***

FDI，一种现场设备集成技术。

<h1 id="1">1 引言</h1>

通过设备集成，可以在更上一级的系统中访问设备的功能和信息。不论简单设备还是复杂设备，都需要集成！！！

高效且经济可行的设备集成，要求多协议、标准化技术，以便可以跨厂商访问设备信息。在过去，这样的统一技术的开发受到了各种组织和自动化制造商的各种利益的限制，它们开发了不同的技术方案。目前的解决方案-不同格式的EDDL和FDT-有其优点和缺点，但是在很多程度上重叠，从而导致用户、制造商和标准化的额外费用。使用FDI，一种将FDT和EDDL的优点结合在一起的单一且可扩展的方案。FDI考虑了简单和复杂设备，在整个生命周期中的各种任务，包括配置、调试、诊断和校准。

全球主要的控制系统和设备制造商，诸如`ABB`、`Emerson`、`Endress+Hauser`、`Honeywell`、`Siemens`、`Yokogawa`，和主要的协会组织，如FDT组织、Fieldbus基金会、HART通信基金会、OPC基金会、PROFIBUS & PROFINET国际，共同支持和推动FDI的发展技术。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="2">2 FDI技术</h1>

<h2 id="2.1">2.1 FDI包</h2>

FDI的核心技术是可扩展FDI包。FDI包就是一堆文件的集合：电子设备描述（EDD），包含设备定义（Def），业务逻辑（BL）和用户接口描述（UID）。它基于电子设备描述语言（EDDL，IEC 61804-3）。

可选的用户接口插件（UIP）提供了自由编程的用户接口，这些接口与FDT相似，基于WPF。设备制造商通过FDI设备包定义数据、功能和用户接口，并存储在FDI服务器上。

* 设备定义-Def-描述了现场设备的数据和内部结构（比如，`block`）。
* 业务逻辑-BL-主要保证数据的一致性，比如，当单位发生变化时，刷新数据。尤其是存在动态依赖时，比如，选项和范围可能依赖于之前的设置，比如只有选择了温度才会选择温度单位，再或者选择了自定义才会显示自定义设置。
* 用户接口描述（UID）和用户接口插件（UIP）定义了现场设备UI。
* 附件，如产品说明文档，协议文件，诸如GSD或CFF等添加到FDI包中。

FDI已经为FDI包的EDD部分定义了一个与协议无关的编码文件格式。

![图-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture1.PNG)

> <font color="blue">Bonus</font>
>
> <font color="blue">1. GSD-General Station Description：PROFIBUS-DP产品的驱动文件，是不同生产商之间为了互相集成所建立的标准通讯接口。当从站模块的生产商与主站PLC生产商不同时，需要在主站组态时安装从站模块的GSD文件。</font>
>

***

> <font color="red">Todo-list</font>
>
> <font color="red">1. block的概念？</font>

<h2 id="2.2">2.2 EDDL</h2>

国际标准组织IEC 61804-3规定了作为FDI基础的EDDL语言。此外，该标准还定义了哪些结构可以使用整个语言构建，哪些库函数允许被用于HART、基金会总线和PROFIBUS协议。因此，这些协议都是使用了语言标准的一个子集。

在某种程度上，这归因于协议或底层设备模型的差异。但是，差异最重要的部分不是协议上的要求，而是语言历史的结果，这种历史直到2006年一直存在于现场总线组织的分散的规范中。对于设备制造商，这意味一个设备需要分别创建HART、PROFIBUS和FF3种不同的EDD，而不是一个核心EDD，其通信描述是独立的。

在FDI技术层面，EDDL在很大程度上是跨协议协调和标准化的结果。EDDL是多协议FDI包开发工具（FDI IDE）、主机组件如EDD引擎（解释器）和客户端组件UID渲染器和UIP主机的基础。成为可持续性地加强互操作和质量的关键因素。同时，可以为设备和系统制造商、现场总线组织和终端用户节省成本。

![图-3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture2.PNG)

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="3">3 FDI主机</h1>

FDI主机可以是作为过程控制或资产管理系统的一部分的设备管理工具，还可以是PC或手持设备上的配置工具。FDI架构允许不同类型的主机实现，其包括基于FDT的主机系统，以保证使用FDT规范的设备的互操作性。

FDI主机总是支持FDI设备包的所有功能。

![图-3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture3.PNG)

图-3：不同应用中的FDI主机

<h2 id="3.1">3.1 FDI主机-C/S架构</h2>

在C/S架构中，服务器提供由各种客户机（通常是分布式的）访问的服务。FDI架构基于OPC UA中使用的信息模型，具有平台独立性等优点。FDI服务器把FDI包倒入其内部设备目录中。这样使得FDI包的版本管理更加容易，因为它们是在FDI服务器里集中管理的。由于FDI包在软件安装方面不需要注册，因此没有令人不爽的其它问题。

FDI服务器中的设备实例在信息模型中表示。信息模型通过将整个通信基础设施和现场设备描述为对象来映射自动化系统的通信拓扑。设备的数据、功能和用户接口也存储在这里。如果一个FDI客户端想要使用设备，它可以访问信息模型，比如载入设备的用户接口以便在客户端侧显示它，这与浏览器的工作方式相似。解析EDD引擎里的EDD，FDI服务器总是能够保证设备数据的一致性。如果客户端与服务器端使用OPC UA通信，OPC UA会进行授权和加密，以阻止未授权的访问。其它的OPC UA客户端（非FDI），比如MES系统，也可以在没有设备接口的情况下访问信息模型中的设备参数。

<img id="Pic_4" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture4.PNG">

图-4：FDI主机-C/S架构

> <font color="blue">Bonus</font>
>
> <font color="blue">1. `OPC UA`-OPC统一架构（OPC Unified Architecture）是OPC基金会创建的新技术，更加安全、可靠、中性（与供应商无关），为制造现场到生产计划或企业资源计划（ERP）系统传输原始数据和预处理信息。使用`OPC UA`技术，所有需要的信息可随时随地到达每个授权应用和每个授权人员。</font>

<h2 id="3.2">3.2 单机版FDI主机</h2>

FDI主机也可以不使用C/S架构，允许实现为单机应用程序。

![图-5](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture5.PNG)

图-5：单机应用工具

<h2 id="3.3">3.3 基于FDT的FDI主机</h2>

FDI为FDI/UIP而重用了FDT2.0接口。这就造成了2种系统架构的实现：纯FDI主机-[图-4](#Pic_4)和基于FDT的FDI主机-[图-6](#Pic_6)。

在基于FDT的架构中，从FDI包的观点来看，FDI-DTM表现的就像是一个FDI主机；对于FDT框架应用程序，它充当DTM的角色。对于许多提供FDT框架应用程序的制造商来说，这开辟了一条往FDI迁移的经济适用方法。这是因为保留了FDT2.0框架，没有发生更改，只是简单地补充了FDI-DTM。

预计FDI-DTM将以现有的EDD-DTM一样的方式推向市场，这意味着FDT框架应用程序的设备制造商自己不用开发组件。

FDT的互操作性允许所有的系统和工具制造商去支持FDI，也就是能够访问FDI包。这对提供DTM和设备EDD的设备制造商非常有利，因为它们只需开发一个FDI包。长期来看，这会减少设备的驱动数量，因而节省产品开发和维护成本。最后，也会为终端用户提供互操作性和版本减少的便利性。

<img id="Pic_6" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture6.PNG">

图-6：基于FDT的FDI主机

<h2 id="3.4">3.4 分层嵌套通信</h2>

FDI中使用的另一个FDT概念是嵌套通信，即网关的开放集成，及通过通信服务器集成的通信驱动。

在FDI中，每个设备都通过FDI包进行映射并集成到FDI主机里。这同样也适用于通信设备。通信所需要的所有操作和服务也都以标准化的格式，通过EDDL代码实现，然后打包到FDI通信包中。FDI服务器负责所有任务的执行和管理。负责网络访问的设备请求硬件资源，这是由FDI通信服务器实现的。

FDI通信概念允许与异构层次网络中的设备通信，并使用任何通信硬件。

<img id="Pic_7" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture7.PNG">

图-7：嵌套通信

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="4">4 已有方案的迁移和重用</h1>

FDI是一种新的集成技术，从长远来看，它将取代已有的EDDL和FDT技术。当然，已经安装的软件还要有一个过渡期。在过程控制行业，这意味着往往超过10年。工具和系统制造商负责提供维护系统硬件和软件。因为EDD，DTM和新的FDI包只存在PC端，而不在设备上，所以在系统软件升级的时候，是可以在不改变任何设备的情况下，从DTM/EDD升级到FDI。

此外，不论新旧设备，设备制造商应该都能高效且经济地创建FDI包。具体地说，就是已有的EDD资源和DTM可以继续使用。

FDI技术支持这2种方法。

<h2 id="4.1">4.1 EDD</h2>

已安装的EDD资源可以通过下面2种方法支持：

1. FDI包开发工具(IDE，集成开发环境)允许将现有的EDD资源转换成调整后的EDD，并与UIP一起用于设备包中。

2. EDD引擎（EDD解释器）向后兼容已有的EDD格式。因此，可以直接在FDI主机里处理已有的EDD资源。

<img id="Pic_8" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture8.PNG">

图-8：EDD迁移

<h2 id="4.2">4.2 DTM</h2>

与EDD的迁移概念类似，DTM的迁移将会使下面成为可能：

1. FDI包开发工具(IDE)允许现有的EDD来源转换成调整后的EDD，并在设备包中与UIP结合使用。

2. 现有的DTM软件可以用于创建UIP，然后作为FDI包的一个组件使用。

3. FDT框架应用程序可以在FDI-DTM的帮助下处理设备包。因为EDD引擎可以向后兼容已有的EDD格式，也可以使用FDI-DTM处理已有的EDD资源。

4. 现有的DTMs仍然可以在FDT框架应用程序中进行处理。

<img id="Pic_9" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture9.PNG">

图-9：DTM迁移

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="5">5 增加互操作性的工具和标准化组件</h1>

为了支持设备和系统的开发，以及提高FDI产品的互操作性，现场总线组织提供了多协议软件工具和标准主机组件。

<h2 id="5.1">5.1 集成开发环境-IDE</h2>

集成开发环境(IDE)可以开发FF、HART、PROFIBUS和PROFINET协议的设备包。实际上，它具有以下4个组件：

* 使用编辑器编辑、创建EDD，然后通过工具`tokenizing`将其转换成二进制格式。

* 编码的EDD，使用Visual Studio开发的UIP，和附件结合起来就形成FDI包。

* 运行时环境(参考主机)运行FDI包以进行测试和调试。

* 测试引擎用于自动运行FDI包一致性测试。

设备制造商以这种方式创建的FDI包，以及相应的设备硬件由现场总线基金会、HART通信基金会和PROFIBUS & PROFINET国际认证和注册。

<img id="Pic_10" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture10.PNG">

图-10：FDI集成开发环境

<h2 id="5.2">5.2 标准组件</h2>

目前针对特定协议的可用解释器组件，在功能、质量和对主机系统的行为上有着显著的差异。它们也仅仅是支持某一种现场总线的EDD版本。解决这一困境的方法就是开发统一的，多协议标准的FDI主机组件。EDD引擎（解释器），UID渲染器和UIP托管组件保证在各种系统中行为相同。EDD引擎使用多协议的方法支持EDD整个语言范围，符合IEC 61804-3，且向后兼容已有的EDD格式。这意味，以后，系统制造商不再需要集成3个解释器，只需一个即可。这节省了时间和精力，有助于提供质量和互操作性。

<img id="Pic_11" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_White_Paper_2012_Picture11.PNG">

图-11：FDI标准主机组件

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h1 id="6">6 优点</h1>

FDI结合了EDDL和FDT的久经考验的概念，从而为控制系统制造商、设备制造商和用户提供了好处。

对于控制系统制造商，C/S架构简化了在强大的分布式控制系统中设备数据和功能的使用。此外，对设备数据和功能的透明访问有助于其他应用程序的集成(例如MES的连接)。其他好处很明显:数据的集中管理防止了不一致性，客户端自动加载用户界面意味着不再需要客户端安装。

对于设备制造商来说，FDI减少了工作并节省了成本，因为在未来，只需要为一个设备创建一个FDI设备包，而不是当前的各种EDD变体和DTM。另一个优点是FDI设备包的可扩展性。简单的设备与简单的设备包共存，复杂的设备需要更复杂的设备包。

集成的开发环境和标准的主机组件确保了FDI设备包和主机系统的互操作性和低成本高效开发。

对于客户而言，FDI的主要好处在于通过一种未来证明标准对现场设备进行标准化集成，从而确保来自各种设备制造商的设备包与来自各种控制系统制造商的FDI系统(FDI主机)之间不受限制的互操作性。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
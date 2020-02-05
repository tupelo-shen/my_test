OPC-UA的全名是OPC Unified Architecture（OPC统一架构）。是OPC基金会（英语：OPC Foundation）应用在自动化技术的机器对机器网络传输协议。有以下的特点：

1. 着重在资料收集以及控制为目的的通讯，用在工业设备以及系统中
2. 开源标准：标准可以免费取得，实作设备不需授权费，也没有其他限制
3. 跨平台：不限制操作系统或是编程语言
4. 面向服务的架构（SOA）
5. 强健的信息安全特性
6. 整合的信息模型，是信息整合中，基础设施的基础，制造商以及组织可以将其复杂的数据在OPC-UA命名空间上建模，利用OPC-UA面向服务的架构的优点。

# 历史

OPC-UA和其前身开放平台通信（OPC）是由同一个组织所开发，但两者有显著不同。基金会开发OPC-UA的目的是发展比原来OPC通信架构（只使用微软的进程交换`COM/DCOM`）更理想的架构，也更符合正在发展中的工业自动化。

OPC-UA规范开发了三年，之后花了一年实现通讯协定，OPC-UA的第一个版本在2006年问世。

到2015年10月10日为止，OPC-UA最近的版本是1.03。除了client/server通讯协定外，新版的OPC-UA也加入了publish/subscribe的机制。

# 创新

开放平台通信（OPC）和COM/DCOM的结合虽让开放平台通信可以顺利推展，但有以下的缺点：

1. 频繁配置DCOM的问题
2. 没有可以规划的逾时机能
3. 只适用于Microsoft Windows
4. 资源安全性较低
5. 没有针对DCOM的控管（COM/DCOM类似黑盒子，开发者无法取得源代码，因此需要处理一些相关的问题或是未充份实现的问题）

因为这些缺点以及许多其他的考虑因素，使得OPC基金会决定开发一个针对全新而且独立的OPC-UA通讯协定栈来取代COM/DCOM。其优点有：

1. 多平台实现，包括可移植的ANSI C、Java及.NET框架。
2. 可扩展性，从智能传感器、智能传动器一直到大型计算机。
3. 支持多线程，也有单线程/单任务的模式，以便将此通讯协定栈放在嵌入式系统中。
4. 基于新标准的资料安全性
5. 每个设备都有可以规划的逾时机能
6. 大容量资料报文的组块

通讯协定栈反映了架构创新的基础，OPC-UA架构是面向服务的体系结构（SOA），以许多不同逻辑层级为其基础。

OPC基础服务是抽象形别的叙述，和通讯协定无关，是OPC-UA机能的基础。传输层将方法转换为通讯协定，将资料序列化（或反序列化），再传送到网络上。 为了上述目的，定义了两种通讯协定，其中一个是以效率进行过最佳化的二进制TCP讯定，另一个则是Web服务导向的协定。

OPC资讯模型是所谓的全网状网络（Full Mesh Network），以节点为基础。节点类似面向对象程式设计（OOP）中的物件，可以包括各种的元资料。节点可以有属性让其他设备读取（DA、HDA），有方法可以呼叫（Commands），也有可以启动传输的触发事件（AE、DataAccess、DataChange）。节点包括过程资料，也包括其他种类的元资料。OPC命名空间中包括了形态模型。

客户端软件可以确认服务器支持哪些行规（profile），有需要获得这些资讯，知道服务器是否只支持DA，或是还支持AE和HDA。而且可以得到服务器是否支持特定行规的讯息。OPC-UA还有以下重要的新机能：

1. 支持冗余（Redundancy）
2. 双向连结的心跳报文（Heartbeat）（确定另外一端是否“活着”），这表示客户端和服务器都可以识别是否中断。
3. 传输信息及应答的缓冲，连结丢失不会造成信息丢失，可以重新获取之前丢失的信息报文。

OPC-UA 是在2006年10月在慕尼黑举行的 OPC-UA DevCon 中首次公开其协定，在Beckhoff（英语：Beckhoff） 的 PLC 上已有许多的 UA 服务器，在 Euros 嵌入式测试电路板中也有UA服务器。Beckhoff PLC 的底层是 Windows XP嵌入式系统，而嵌入式控制器执行的操作系统是实时操作系统 Euros。Embedded Labs Ltd 公司在他们执行在单芯片ARM控制器（64kb RAM） C++ UA Stack 上展示了 OPC-UA。2012年10月时德国的 Fraunhofer-Application Center IOSB-INA 以及工业信息技术研究所（Institute for industrial Information Technologies, inIT）证实了OPC-UA服务器可以只使用15 kB RAM以及10 kB ROM，因此是芯片等级可以使用的通讯架构[2]。

# 通讯协定

OPC-UA支持两种通讯协定[3]，这两种通讯协定的差异只有URL的不同，二进制通讯协定是opc.tcp://Server，而Web服务的通讯协定是http://Server，其他情形下，OPC-UA对应用程序接口的作业完全透明，其他作业不受OPC-UA的影响。

二进制通讯协定的效率最高，其overhead也最少，让需要的资源最小化（不需要XML解析器、SOAP及HTTP，对嵌入式系统格外重要），提供最佳的互操控性（在实现时，二进制通讯协定提供较少的自由度），使用任意选取的TCP通道，可以较容易的进行隧道协议，也可以从透过防火墙开启。

Web服务（SOAP）通讯协定可以支持许多不同的工具（包括Java环境或是.NET环境的工具），使用标准HTTP(S)埠，可以和防火墙共同使用。

所有的实现方式都支持二进制通讯协定，但只有用.NET实现的设备才支持SOAP。

# 规范

OPC-UA规范属于多部分的规范，包括了以下部分：

1. Concepts
2. Security Model
3. Address Space Model
4. Services
5. Information Model
6. Mappings
7. Profiles
8. Data Access
9. Alarms and Conditions
10. Programs
11. Historical Access
12. Discovery
13. Aggregates
14. PubSub

OPC-UA规范和其他以COM为基础的规范不同，OPC-UA规范不是单纯的应用规范。其中会描述典型的UA内部机制，这些会在通讯协定栈中处理，因此一般只有要将通讯协定栈导入特定硬件的人，或是要开发通讯协定栈的人才会对这些内容有兴趣。

OPC-UA应用程序的开发者需要撰写程式和OPC-UA API沟通，因此主要只需要用到API的说明文件。不过应用程序开发者也可能会对其中的第3、4、5部分感兴趣

# UA通讯协定栈

UA应用程序在服务器端或是客户端，其架构上都有分层的结构。

有些部分和以往的COM Proxy/Stubs相等，由OPC协会提供。可移植性级别（portability level）是新的，简化了引入UA ANSI C通讯协定栈的程序，也简化了移植到其他平台的的难度。OPC协会也提供了针对Window及Linux的port layer。

# UA安全性

UA安全性包括了认证、授权、加密以及透过签名实现的资料整合性。针对Web服务会使用WS-SecureConversation（英语：WS-SecureConversation），可以和.NET和其他以SOAP实现的软件相容。若是二进制的版本，也会依循WS-SecureConversation的算法，转换为二进制的版本，称为 UA Secure Conversation。

有另外一个混合的版本，程式是二进制的，而其传输层是用SOAP。这是在二进制的效率以及对防火墙友善的传输之间的妥协。因为是二进制的程式，会需要使用UA Secure Conversation。 其认证只使用X.509认证，是靠程式开发者决定UA应用程序要使用哪一个证书存储区。例如，也可能使用Active Directory中的公开金钥基础建设（PKI）。

# OPC-UA API

在许多编程语言中都有UA的API。在C、C++、Java及.NET中有商业的SDK。而至少在C、C++、Java、Javascript(node)及Python中有其开源的通讯协定栈。

# C++下的OPC-UA

* open62541
    
    是开源的OPC-UA实现，基于C语言（C99标准）编写，遵循Mozilla公共许可协议。

    具有以下特性：

    1. 开源
    
        1. 完全地基于[IEC 62541](https://webstore.iec.ch/webstore/webstore.nsf/mysearchajax?Openform&key=62541)标准实现；
        2. 开源许可协议（Mozilla Public License v2.0）；
        3. 免版税，可在[GitHub](https://github.com/open62541/open62541/)上下载。
    
    2. 可扩展
        
        1. 基于事件驱动框架
        2. 支持单线程或多线程

    3. 可移植性

        1. 使用C99编写，带有各个架构体系的插件（POSIX等）
        2. 可以运行在`Windows`、`Linux`、`VxWorks`、`QNX`、`Android`和各种嵌入式系统上
        3. 编译的服务器可以小于100KB

    4. 灵活性
        
        1. 信息模型可以在运行时修改
        2. 可以使用[XML规范文件](https://github.com/open62541/open62541/tree/master/tools/schema)产生数据类型和消息模型
    
    最新消息：

    1. 2019-4:实时OPC UA PubSub Publisher发布

        在2019年汉诺威展会上，我们在OPC基金会的展台上展示了open62541实时`OPC UA PubSub Publisher`。代码现在也发布在Github上。

        可以在RTOS和Linux（带有RT-Preempt补丁）上重现测试结果。[示例代码](https://github.com/open62541/open62541/tree/master/examples/pubsub_realtime)可以在这儿获取。

    2. 编译安装等内容请参考[官网](https://open62541.org/)。
    
* ASNeG计划是C++开源（Apache License 2.0）OPC-UA应用层及OPC-UA 网页服务器（β版，目前只有基本机能）。
* FreeOpcUa计划提供开源（LGPL）的C++服务器端及客户端实现。
* UAF计划提供开源（LGPL）) 的C++/Python实现。

# .NET下的OPC UA

.NET版本的实现在底层用ANSI C，其余部分使用.NET。因此只有网络端点的处理以及消息分块的取得是从ANSI C的通讯栈处理的。反序列化直接在.NET中进行，会直接转换为.NET的结构及物件。因此其效能会比先反序列化成C言言结构，再复制到.NET结构的效率要好。

# Java下的OPC UA

有许多Java语言的通讯栈，类似.NET，Java语言的通讯栈分为三种：

* 用Java本地接口封装完整的ANSI C通讯栈，这样不利于可移植性。虽然通讯栈可以移殖到不同的操作系统，但需要个别的编辑这些程式。而且资料也需要复制到Java本地接口的边界，不过在反序列化时会有C语言的效能。
* 直接撰写网络层的代码（类似目前.Net的实现），用Java进行反序列化，省去了一次的资料复制，但仍会受到C堆叠的影响。
* 撰写原生的Java OPC UA通讯栈，这个是可移植性最好的，但是工程师需花费的心力也是最多的。Eclipse Milo专案提供一个纯Java开源OPC UA实现，是依照UA 1.03客户及服务器规范

也有一些简单的变体只支援WebService协定，此情形下，需要有可以支援WS-Security的SOAP Toolkit。

# Python下的OPC UA

FreeOpcUa专案提供了以Python编程语言实现的OPC UA（和Python 2, 3 及pypy相容），也提供了OPC-UA客户及服务器的高层次抽象化，可以用在客户的应用中，或是渐渐延伸到客户的应用。

# IEC 62541

IEC 62541是OPC UA的标准

| ID | 发布日期 | 标题 |
| -- | -------- | ---- |
| IEC/TR 62541-1 | 02/2010 | OPC Unified Architecture - Part 1: Overview and Concepts |
| IEC/TR 62541-2 | 02/2010 | OPC Unified Architecture - Part 2: Security Model | 
| IEC 62541-3    | 07/2010 | OPC Unified Architecture - Part 3: Address Space Model |
| IEC 62541-4    | 10/2011 | OPC Unified Architecture - Part 4: Services |
| IEC 62541-5    | 10/2011 | OPC Unified Architecture - Part 5: Information Model |
| IEC 62541-6    | 10/2011 | OPC Unified Architecture - Part 6: Mappings |
| IEC 62541-7    | 07/2012 | OPC Unified Architecture - Part 7: Profiles |
| IEC 62541-8    | 10/2011 | OPC Unified Architecture - Part 8: Data Access |
| IEC 62541-9    | 07/2012 | OPC Unified Architecture - Part 9: Alarms and Conditions |
| IEC 62541-10   | 07/2012 | OPC Unified Architecture - Part 10: Programs |
# 0 概述

**EtherNet/IP** 是一种工业互联网协议，它将通用工业协议适用于标准以太网。在美国，**EtherNet/IP** 是主要的工业协议之一，被广泛地应用在大量行业之中，包括工厂，混合动力汽车，过程控制处理等。**EtherNet/IP** 和 **CIP** 技术由 *ODVA* 管理，一个全球贸易和标准开发组织，成立于1995年，拥有超过300个企业成员。

**EtherNet/IP** 使用了互联网协议组和 *IEEE 802.3* ，它们定义了传输层、网络层、数据链路层和物理层。而 **EtherNet/IP** 在会话层、表示层、应用层（OSI-开放式系统互联参考模型的5/6/7层）这3层执行。**CIP**使用面向对象设计，为实时控制应用提供所需的 **EtherNet/IP** 服务和设备的配置文件，并促进多样化的产品生态系统的自动化功能的同一实施。此外，**EtherNet/IP** 把互联网标准功能和服务的主要元素转变为CIP对象模型框架，比如用户数据报协议（UDP），**EtherNet/IP**用它传输I/O消息。

# 1 历史

**EtherNet/IP** 的开发始于20世纪90年代，在另一个贸易和标准开发组织 *ControlNet International，Ltd* （CI）的技术工作组内。 2000年，**ODVA**和 **CI**为 *EtherNet*的开发形成了联合技术协议（*JTA*）。 2009年，*JTA*终止，**EtherNet/IP**成为 *ODVA*的成员。 今天，**EtherNet/IP**是四个网络之一，与 **DeviceNet**，**ControlNet**和 **CompoNet**一起将 **CIP**适配到工业网络。 所有这些网络都由 **ODVA**管理。

# 2 技术细节

**EtherNet/IP** 将以太网节点分类为具有特定行为的预定义设备类型。 除此之外，这还可以：

1. 通过基于用户数据报协议（*UDP*）的隐式消息传输基本I/O数据
2. 通过 *TCP*上传和下载参数，设定点，程序和配方（即显式消息传递）
3. 通过 *UDP*，进行轮询，监控周期性或即时状态改变
4. 通过 *IP*，进行点对点，点对多，广播通信
5. *EtherNet/IP*使用 *TCP*端口号 *44818* 发送显式消息，使用 *UDP*端口号 *2222* 发送隐式消息。

# 3 开源实现

可移植的开源实现，称为 *OpENer*（修改后的BSD协议）， 与2009年启动。 源代码可在[GitHub](https://github.com/EIPStackGroup/OpENer)上获得。

# 4 其它内容

* [通用工业协议](https://en.wikipedia.org/wiki/Common_Industrial_Protocol)

* [工业互联网](https://en.wikipedia.org/wiki/Industrial_Ethernet)

# 5 参考

* [ODVA](http://www.odva.org/)
* [ODVA EtherNet/IP page](http://www.odva.org/default.aspx?tabid=67)
* [EtherNet/IP developers guide from ODVA](http://www.odva.org/Portals/0/Library/Publications_Numbered/PUB00213R0_EtherNetIP_Developers_Guide.pdf)
* ["EtherNet/IP™ – CIP on Ethernet Technology" (PDF)](https://www.odva.org/Portals/0/Library/Publications_Numbered/PUB00138R6_Tech-Series-EtherNetIP.pdf). ODVA. Retrieved 6 July 2016.
* ["OpENer EtherNet/IP stack"](https://github.com/EIPStackGroup/OpENer). Retrieved 5 May 2017.
* ["The instruction to OpENer EtherNet/IP stack"](http://eipstackgroup.github.io/OpENer/index.html).
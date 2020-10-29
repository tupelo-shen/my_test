[TOC]

XenServer是一个由Citrix发起和管理的开源项目和社区。该项目开发的开源软件实现了虚拟化的多种功能，允许在硬件设备上安全地运行多个操作系统和应用程序，完成硬件整合和自动化；将静态、复杂的IT环境转变为更加动态、易于管理的虚拟数据中心，有效地降低IT资源成本，提供的先进管理功能，实现虚拟数据中心的集成和自动化，简化服务器和应用程序的管理。

# 1 XenServer的发展

Xen最开始起源于剑桥大学的一个研究项目“XenoServer 范围的计算项目”，由剑桥大学高级讲师Ian Pratt领导，后来他与剑桥大学的Simon Crosby共同创建了Xensource。

* Xen的首次公开发行是在2003年。

* 2004年，Xen2.0发布后不久，Ian Pratt和其他几位技术领先者成立了Xensource，将Xen hypervisor从一个研究工具升级为一个具有竞争力的企业计算产品。作为公司战略的一部分，Xen hypervisor仍然是一个开源解决方案。

* 2005年，xen hypervisor得到了广泛的采用，Red Hat、Novell和Sun都将Xen hypervisor添加进他们的虚拟化解决方案。同年，xen hypervisor得到了广泛的采用，Red Hat、Novell和Sun都将Xen hypervisor添加进他们的虚拟化解决方案。开发社区还通过Xen 3.0版本加速了Xen的功能。Microsoft和VMware也采用了由Xen社区首次引入的准虚拟化概念。

* 2006年，XenServer 3.1的发布，打破了Xen虚拟机只对linux的限制，通过Intel VT和AMD-V的硬件特性，实现了windows虚拟机的支持。

* 2007年10月22日，Citrix Systems完成了对Xensource的收购，并公开了Xen项目咨询委员会（Xen AB）的存在，该委员会的成员来自Citrix、IBM、Intel、Hewlett-Packard、Novell、Red Hat、Sun Microsystems和Oracle等。在被Citrix收购前不久，XenSource发布了XenEnterprise v4，引入了新的集群管理组件XAPI工具集和全新的基于.NET的界面化管理工具XenCenter，这是一款将XenSource引入Enterprise版本的商用产品。Citrix收购之后将其产品整合，统一更名为XenServer。

* 2009年，XenServer 5.5.0发布，功能包括合并备份、增强搜索工具、与Active Directory整合、以及对Windows、Linux等操作系统更大的支持。奠定了Citrix在服务器虚拟化市场的地位。之后的多个版本继续丰富了虚拟化技术的各个方面，如在线实时迁移，自动恢复，使用ovs作为默认网络管理工具，增强了在计算、存储、网络功能和对guest操作系统的支持。

* 2013年4月15日，Xen项目被宣布作为一个合作项目移至Linux基金会下。Linux基金会推出了一个新的“Xen项目”以区别于旧的“Xen”。2013年6月24日，Citrix宣布了开源XenServer项目，其目标是将其以前的专有产品XenServer作为一种开源产品交付给开源社区，以获得更多的投入和参与。

* 2015年1月，XenServer 6.5版本发布，基于64-bit的CentOS 5.10实现的control domain，使用Xen hypervisor v4.4，提升了网络、存储及vGPU的功能和性能。这是目前使用较为广泛的一个版本。

目前，XenServer版本已发布到7.6版本。

# 2 XenServer架构

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/three_virtual_technologies_1.png">

上图是Xen的架构。Xen hypervisor直接运行在物理硬件上，负责处理CPU、内存、定时器和中断等相关任务。系统在完成引导加载程序后，Xen hypervisor首先启动。

在Xen hypervisor之上，运行着多个虚拟机，这些运行着实例的虚拟机在Xen中被称为域（domain）或来宾（guest）。其中最特殊的domain被称为Domain 0或VM 0，这是一个控制Domain，简称为Dom0，如上图左侧。Domain 0除去包含了对应系统设备的所有驱动程序外，还有用来管理基于Xen系统的一个控制栈及多种系统服务。通过Domain 0分解，可以将某些Domain 0中的服务及设备驱动拆出来，放在某专有虚拟机中运行，这需要一些特殊配置。

* Xen hypervisor：

    `Xen hypervisor`是一个非常瘦的软件层（ARM上<65ksloc，x86上<300ksloc），直接在硬件上运行，负责管理CPU、内存和中断。它是引导装载程序退出后运行的第一个程序。管理程序本身不处理I/O功能，如网络和存储。

* Guest Domain：

    别名DomU，这是实际业务的虚拟化环境，每个都运行自己的操作系统和应用程序。管理程序支持几种不同的虚拟化模式，Guest Domain与硬件完全隔离：换句话说，它们没有访问硬件或I/O功能的特权。因此，它们也被称为无特权域（DomU）。

* Control Domain（Domain 0）：

    别名Dom0，这是一个特殊的虚拟机，具有特殊的特权，例如直接访问硬件、处理对系统I/O功能的所有访问以及与其他虚拟机交互的能力。没有Dom0（系统启动的第一个虚拟机），Xen hypervisor就不能使用。在标准设置中，Dom0包含以下功能：

    - 系统服务：

        如用来管理参数的xenstore/xenbus（XS），向虚机暴露用户接口的Toolstack工具集（TS），基于Qemu的设备仿真（DE）服务。

    - 原生设备驱动：
    
        Dom0纳管了物理设备驱动，因此对Xen系统支持原生的硬件支持。
    
    - 虚拟设备驱动：
    
        Dom0除去有物理设备驱动，还支持虚拟设备驱动，也被称为backends。

    - 工具集：
    
        允许用户管理虚拟机的创建、删除和修改配置。工具集提供的功能接口，既可以由命令行、图形界面调用，也可以通过第三方的云管平台调度，如openstack和cloudstack。多种不同的工具集可以与Xen一起使用。

    - Xen Project-enabled operating systems：
    
        Dom0需要一个有效的内核。基于比Linux 3.0更新的内核的Linux发行版支持Xen项目，通常包括包含hypervisor和工具（默认工具集和控制台）的包。

# 3 XenServer基本概念

在xen提供的虚拟化服务中，对计算、网络、存储中的多个概念抽象出不同对象，以便在逻辑上有效管理，隔离出具体实现和操作，使得用户可以更专注于业务逻辑和使用实例。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/three_virtual_technologies_2.png">

上图罗列了Xen中主要的数据对象和关系，可通过命令行、界面化XenCenter和XAPI来操作实现计算、存储、网络的各种相关功能。映射于功能的主要对象如下图，虚拟机和Xen服务器间从网络和存储两条功能线上联系起来。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/three_virtual_technologies_3.png">

* VM

    此处泛指用户创建的`Guest Domain`，即`DomU`。实际业务的虚拟化环境，每个都运行自己的操作系统和应用程序，向外提供虚拟化服务。

* Host

    代表一个XenServer。

* Pool

    一个或多个同构XenServer主机的聚合，组成一个可承载虚拟机的实体向外提供虚拟化服务。当绑定共享存储时，可在Pool中任意一个内存满足虚拟机条件的XenServer上启动虚拟机。一个XenServer只允许属于一个Pool，不同版本的XenServer的Pool中最大容纳Server数略有不同。同构的条件如XenServer的cpu型号基本一致，虚拟化选项一致，版本、license相同等。

## 3.1 网络对象

* PIF

    是Dom0中使用的对象，代表主机上的物理网卡。PIF对象有名称和描述、UUID、它们所代表的物理网卡的参数以及它们所连接的网络和XenServer。

* Network

    实质是一个虚拟以太网交换机，实现网络流量的路由功能。网络对象有名称和描述、UUID，以及连接到它们的VIF和PIF的集合。

* VIF

    是Dom0中使用的对象，表示虚拟机上的虚拟网卡。VIF对象有名称和描述、UUID，以及它们所连接的网络和VM。

## 3.2 存储对象

* SR

    存储库，英文称为`Storage Repository`。是存储虚拟机虚拟磁盘镜像（VDI）的特定存储目标。可分为共享存储和份共享存储，Xen Host和Pool可同时挂载多个相同或不同种类的SR。

* VDI

    虚拟磁盘镜像，英文称为`Virtual Disk Image`表示虚拟硬盘驱动器（HDD）的存储抽象概念。VDI是XenServer中虚拟化存储的基本单元。虚拟机可挂载多个VDI对象。

* VBD

    虚拟块设备，英文称为`Virtual Block Device`。表示VDI和VM之间的连接关系。除了表示VDI连接到虚拟机外，还存储给定VDI有关qos和统计信息，如该VDI是否可启动。

* PBD

    物理块设备，英文称为`Physical Block Device`。表示物理XenServer和SR之间的连接关系，代表给定SR映射到XenServer的连接器对象。PBD存储设备配置项，用于连接和与给定存储目标交互。

# 4 XenServer资源管理工具

可通过命令行、界面化XenCenter和XAPI来操作实现计算、存储、网络的各种相关功能。Xen提供了一簇`xe`命令来管理资源，ssh登录对应的XenServer后，可通过`xe help –all`查看命令及具体参数。如下图的XenServer 6.5 xe命令部分截图：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/three_virtual_technologies_4.png">

Xen还提供了图形化管理工具XenCenter来管理资源。通过XenCenter 可以完成所有的配置操作，在实施中大部分配置操作都会通过该管理工具实现。如下图：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/three_virtual_technologies_5.png">

XAPI由一系列的toolstack组成，可通过此 API 以编程方式访问一组数量庞大的XenServer 来管理。详细信息可参考Xen的SDK文档。

# 5 总结

XenServer是一套在实际的云生产环境中经过检验的企业级服务器虚拟化解决方案，且以开源的姿态面向开发者和用户。本文以点带面地介绍了XenServer一些内容，XenServer也继续着在云领域的耕耘和演进。
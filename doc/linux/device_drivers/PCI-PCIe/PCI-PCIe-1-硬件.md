
> PCI总线和设备树是X86硬件体系内很重要的组成部分，几乎所有的外围硬件都以这样或那样的形式连接到PCI设备树上。虽然Intel为了方便各种IP的接入而提出IOSF总线，但是其主体接口还依然是PCIe形式。我们下面分成两部分介绍PCI和他的继承者PCIe（PCI express）：第一部分是历史沿革和硬件架构；第二部分是软件界面和UEFI中的PCI/PCIe。

自PC在1981年被IBM发明以来，主板上都有扩展槽用于扩充计算机功能。现在最常见的扩展槽是PCIe插槽，实际上在你看不见的计算机主板芯片内部，各种硬件控制模块大部分也是以PCIe设备的形式挂载到了一颗或者几颗PCI/PCIe设备树上。固件和操作系统正是通过枚举设备树们才能发现绝大多数即插即用（PNP）设备的。那究竟什么是PCI呢？

## 1 PCI/PCIe的历史

在我们看PCIe是什么之前，我们应该要了解一下PCIe的祖先们，这样我们才能对PCIe的一些设计有了更深刻的理解，并感叹计算机技术的飞速发展和工程师们的不懈努力。

1. ISA (Industry Standard Architecture)
2. MCA (Micro Channel Architecture)
3. EISA (Extended Industry Standard Architecture)
4. VLB (VESA Local Bus)
5. PCI (Peripheral Component Interconnect)
6. PCI-X (Peripheral Component Interconnect eXtended)
7. AGP (Accelerated Graphics Port)
8. PCI Express (Peripheral Component Interconnect Express)

科技的每一步前进都是为了解决前一代中出现的问题，这里的问题就是速度。作为扩展接口，它主要用于外围设备的连接和扩展，而外围设备吞吐速度的提高，往往会倒推接口速度的提升。第一代ISA插槽出现在第一代IBM PC XT机型上（1981），作为现代PC的盘古之作，8位的ISA提供了4.77MB/s的带宽（或传输率）。到了1984年，IBM就在PC AT上将带宽提高了几乎一倍，16位ISA第二代提供了8MB/s的传输率。但其对传输像图像这种数据来说还是杯水车薪。

IBM自作聪明在PS/2产品线上引入了MCA总线，迫使其他几家PC兼容机厂商联合起来捣鼓出来EISA。因为两者都期待兼容ISA，导致速度没有多大提升。真正的高速总线始于VLB，它绑定自己的频率到了当时486 CPU内部总线频率：33MHz。而到了奔腾时代，内部总线提高到了66MHz，给VLB带来了严重的兼容问题，造成致命一击。

Intel在1992年提出PCI（Peripheral Component Interconnect）总线协议，并召集其它的小伙伴组成了名为 PCI-SIG (PCI Special Interest Group)（PCI 特殊兴趣组J）的企业联盟。从那以后这个组织就负责PCI和其继承者们（PCI-X和PCIe的标准制定和推广。

不得不点赞下这种开放的行为，相对IBM当时的封闭，合作共赢的心态使得PCI标准得以广泛推广和使用。有似天雷勾动地火，统一的标准撩拨起了外围设备制造商的创新，从那以后各种各样的PCI设备应运而生，丰富了PC的整个生态环境。

PCI总线标准初试啼声就提供了133MB/s的带宽(33MHz时钟，每时钟传送32bit)。这对当时一般的台式机已经是超高速了，但对于服务器或者视频来说还是不够。于是AGP被发明出来专门连接北桥与显卡，而为服务器则提出PCI-X来连接高速设备。

2004年，Intel再一次带领小伙伴革了PCI的命。PCI express（PCIe，注意官方写法是这样，而不是PCIE或者PCI-E）诞生了，其后又经历了两代，现在是第三代(gen3，3.0)，gen4有望在2017年公布，而gen5已经开始起草中。

下面这个大表列出所有的速度比较。其中一些x8,x16的概念后面细节部分有介绍。

<img src="speed_comparison_of_various_buses">

从下面的主频变化图中，大家可能注意到更新速度越来越快。

<img src="speed_comparison_trend_of_various_buses">

## 2 PCI和PCIe架构

### 2.1 PCI架构

一个典型的桌面系统PCI架构如下图：

<img src="standard_pci_structure">

如图，桌面系统一般只有一个Host Bridge用于隔离处理器系统的存储器域与PCI总线域，并完成处理器与PCI设备间的数据交换。每个Host Bridge单独管理独立的总线空间，包括PCI Bus, PCI I/O, PCI Memory和PCI Prefetchable Memory Space。桌面系统也一般只有一个Root Bridge，每个Root Bridge管理一个Local Bus空间，它下面挂载了一颗PCI总线树，在同一颗PCI总线树上的所有PCI设备属于同一个PCI总线域。一颗典型的PCI总线树如图：

<img src="tree_of_standard_pci_structure">

从图中我们可以看出 PCI 总线主要被分成三部分：

1. PCI 设备。符合 PCI 总线标准的设备就被称为 PCI 设备，PCI 总线架构中可以包含多个 PCI 设备。图中的 Audio、LAN 都是一个 PCI 设备。PCI 设备同时也分为主设备和目标设备两种，主设备是一次访问操作的发起者，而目标设备则是被访问者。

2. PCI 总线。PCI 总线在系统中可以有多条，类似于树状结构进行扩展，每条 PCI 总线都可以连接多个 PCI 设备/桥。上图中有两条 PCI 总线。

3. PCI 桥。当一条 PCI 总线的承载量不够时，可以用新的 PCI 总线进行扩展，而 PCI 桥则是连接 PCI 总线之间的纽带。

服务器的情况要复杂一点，举个例子，如Intel志强第三代四路服务器，共四颗CPU，每个CPU都被划分了共享但区隔的Bus, PCI I/O, PCI Memory范围，其构成可以表示成如下图：

<img src="tree_of_special_pci_structure">



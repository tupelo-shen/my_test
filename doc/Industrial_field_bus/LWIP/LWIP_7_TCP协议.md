# 1 协议简介

#### 1.1 TCP的必要性

在很多情 况下，应用程序期望传输层能提供可靠的服务，在上层应用想把大量的顺序数据（例如一个文件）发送到另一台主机时，使用 UDP 会显得非常不方便。 在接收端，所有数据将按照编号被顺序组织起来，当所有数据接收成功后，TCP 才把数据递交给应用层。应用层不必担心报文的乱序、重复、丢失等题，TCP 采用正面确认以及重传等机制保证数据流能全部正确到达。

#### 1.2 TCP特性

TCP 是一个很复杂的机制，它规定了两台主机间传送的报文的格式，以及为保证报文能正确到达目的地而采取的一系列措施，同时也包括了必要的差错与控制机制。

#### 2.3 连接的定义

TCP 中引入了端点的概念，端点定义为一对整数，标识为（IP 地址，端口号），例如端口（192.168.1.1，80）就代表了主机 192.168.1.1 上的端口 80。 同一台主机上的两个应用可以使用两个不同的端点建立连接并进行数据交互，利用 TCP/IP 进行数据交互是主机上实现进程间通信、数据共享的常用方法。

连接 1：（192.168.1.37，80）和（192.168.1.1，7675）
连接 2：（192.168.1.37，80）和（192.168.1.37，9546）

TCP 中也使用端口号的概念，端口号是 0 到 65535 之间的整数，端口号可以分为熟知端口号和短暂端口号，常见的 TCP 熟知端口定义如表 13­1 所示。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_1.png">

#### 1.4 数据流编号

TCP 把一个连接中的所有数据字节都进行了编号，当然两个方向上的编号是彼此独立的，编号的初始值由发送数据的一方随机选取，编号的取值范围在0~（2的32方-­1），比如发送方选择的起始编号为 20，且将要发送的数据长度为 800 字节，那么字节编号将覆盖 20 到 819 的范围。

例如，将上述的 800 字节放在三个报文段中来发送，前两个报文段各装载了300 字节的数据，最后一个报文装载了 200 字节的数据，则三个报文段携带的数据情况如下：

1. 报文段 1：：起始序号：20，数据长度：300，序号范围：20~319
2. 报文段 2：：起始序号：320，数据长度：300，序号范围：320~619
3. 报文段 3：：起始序号：620，数据长度：200，序号范围：620~819

接收方通过确认的机制来告诉发送方数据的接收状况，这是通过向发送方返回一个确认号来完成的，确认号标识出自己期望接收到的下一个字节的编号 ，例如在上面的例子中，如果接收方接收到了报文段 1，则它返回给发送方的确认号为 320，表示自己期望收到数据编号为 320 的数据 。还有一种情况，如果接收方只收到报文段 1 和 3，那么它返回的确认号仍然是 320，确认号始终表示接收方期望的下一字节数据，尽管可能已有更高编号的数据被收到。

#### 1.5 滑动窗口

在数据发送端，所有数据流按照顺序被组织在发送缓存中，什么时候将发送数据以及发送的多少是由滑动窗口决定的，使用窗口滑动的概念可以达到很好的流量控制效果和拥塞控制效果。 接收方为了接收数据，也必须在接收缓存上维护一个接收窗口，接收方需要将数据填入缓冲区，对数据进行顺序组织等操作，并向发送方通告自己的接收窗口大小 。

最后需要指出的是，TCP 是全双工的通信，两个方向上的数据传送是独立的，任何一方既可以作为发送端同时也会成为接收端，因此，任何一端都将为每个 TCP 连接维护两个窗口，一个用于数据接收，一个用于数据发送。这样，在一条完整的 TCP 连接上，应该同时存在四个窗口 。

如图所示，窗口内的数据又可以分为两部分，第一是编号 5~10 的 已发送但是未被接收方确认的数据，第二是编号为 11~13 的可发送但未发送的数据（这些数据可能是因为不能组成一个合适长度的报文，因此未被发送）。若此时发送方接收到值为 6 的确认号，则窗口会向右滑动，5 和 6 被排除在窗口之外，而 14~15 将被包括在窗口内，成为可发送的数据。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_2.png">

# 2 TCP报文

TCP 报文段由 TCP 首部和 TCP 数据区组成，如图 13­2 所示，首部区域包含了连接建立与断开、数据确认、窗口大小通告、数据发送相关的所有标志和控制信息。首部的大小为 20~60 字节，在没有任何选项的情况下，首部大小为 20 字节，与不含选项字段的 IP 报首部大小相同。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_3.png">

32 位序号字段标识了从 TCP 发送端到 TCP 接收端的数据字节编号，它的值为当前报文段中的第一个数据的字节序号。在接收方，先计算出数据区数据的长度，然后使用首部中的序号字段，就能计算出报文最后一字节数据的序号。

4位首部长度指出了TCP首部的长度，以4字节为单位。 TCP 最多有 60 字节的首部，如果没有任何选项字段，首部长度应该为 5（20 字节）。

接下来的 6bit 保留字段暂时未用，为将来保留。

6 个标志位的意义如表 13­2 所示，后续会对各个字段进行详细的介绍。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_4.png">

窗口大小字段是 16bit 的，所以通告窗口的最大值为 65535 字节。窗口字段是实现流量控制的关键字段，当接收方向发送方通知一个大小为 0 的窗口时，将完全阻止发送方的数据发送 。

16 位的紧急指针只有当紧急标志位 URG 置位时才有效，此时报文中包含了紧急数据，紧急数据始终放在报文段数据开始的地方，而紧急指针定义出了紧急数据在数据区中的结束处，用这个值加上序号字段值就得到了最后一个紧急数据的序号。

#### 2.2 TCP选项

每条 TCP 选项由 3 部分组成：1 字节的选项类型+1 字节的选项总长度+选项数据。如图 13­3所示，它展示出了常见的两种 TCP 选项的格式定义，它们分别是最大报文段长度选项和窗口扩大因子选项。（最大为40字节）

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_5.png">

最大报文段长度（MSS） ：用于向对方指明自己所能接受的最大报文段 ，每一方都不应该发送超过对方指定 MSS 大小的报文段，由于选项中最大报文段长度是 16 位的，所以 MSS 的值在 0 到 65535 之间，如果一方没有向另一方指明自己的 MSS，则表明它将使用默认最大报文段长度 536。

窗口扩大因子：通告的窗口大小=首部中窗口大小字段值×2的A次方

#### 2.3 紧急数据

应用程序在向接收方发送了大量的数据后，发现由于自身的问题，使得这些数据中存在错误，此时它期望向接收方发送一个命令，告诉接收方：这些数据是错误的，你不要处理，否则会给连接带来致命的危害。这个命令不能通过数据流的形式被发送，假若这样，接收端只有处理完了所有数据才会去读取这个命令。

使用 URG 位置 1 的报文段，这种类型的报文段将告诉接收方：这里面的数据是紧急的，你可以直接读取，不必把它们放在接收缓冲里面。在包含紧急数据的报文段中，紧急数据总是放在数据区域的起始处，且报文首部中的紧急指针指明了紧急数据的最后一个字节。

#### 2.4 强迫数据交互

发送方应用程序向 TCP传递数据时，请求推送（push）操作，这时，TCP 协议不会等待发送缓冲区被填满，而是直接将报文发送出去。同时，被推送出去的报文首部中推送位（PSH）将被置 1，接收端在收到这类的报文时，会 尽快将数据递交给应用程序，而不必缓冲更多的数据再递交。

#### 2.5 报文首部数据结构

协议栈中 TCP 部分的代码量很大，源代码中的 tcp.h、tcp.c、tcp_in.c、tcp_out.c 四个文件包含了 TCP 协议实现的全部数据结构和函数。其中 tcp.c 文件包含了与 TCP 编程、TCP 定时器相关的函数，tcp_in.c 包含了 TCP 报文段输入处理相关的函数、tcp_out.c 包含了 TCP 报文段输出处理相关的函数，而 tcp.h 包含了所有的宏、结构体的定义。在 LwIP 中是怎样来描述一个 TCP 首部的，数据结构名字叫做 tcp_hdr。

    //定义 TCP 首部结构体
    PACK_STRUCT_BEGIN
    struct tcp_hdr {
    　　PACK_STRUCT_FIELD(u16_t src); //源端口
    　　PACK_STRUCT_FIELD(u16_t dest); //目的端口
    　　PACK_STRUCT_FIELD(u32_t seqno); //序号
    　　PACK_STRUCT_FIELD(u32_t ackno); //确认序号
    　　PACK_STRUCT_FIELD(u16_t _hdrlen_rsvd_flags); //首部长度+保留位+标志位
    　　PACK_STRUCT_FIELD(u16_t wnd); //窗口大小
    　　PACK_STRUCT_FIELD(u16_t chksum); //校验和
    　　PACK_STRUCT_FIELD(u16_t urgp); //紧急指针
    } PACK_STRUCT_STRUCT;
    PACK_STRUCT_END

# 3 TCP连接

#### 3.1 建立连接

握手过程由客户端程序首先发起，整个过程要经历下面三个步骤：

* （1）客户端发送一个 SYN 标志置 1 的 TCP 报文段，报文段首部指明自己的端口号及期望连接的服务器端口号。同时在报文段中，客户端需要通告自己的初始序号 ISN（这里假设为 A）。除此之外，这个报文中还可以携带一些选项字段。

* （2）当服务器接收到该报文并解析后，返回一个 SYN 和 ACK 标志置 1 的报文段作为应答。ACK 为 1 表示该报文段包含了有效的确认号，这个值应该是客户端初始序列号加 1（即 A+1）。另一方面，SYN 标志表明服务器响应连接，并在回应报文中包含服务器自身选定的初始序号 ISN（这里假设为 B）。

* （3）当客户端接收到服务器的 SYN 应答报文后，会再次产生 ACK 置位的报文段，该报文段包含了对服务器 SYN 报文段的有效确认号，该值为服务器发送的 ISN 加 1（即 B+1），同时，该报文段中还包含了有效的窗口大小，用来向服务器指明客户端的接收窗口大小。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_6.png">

当服务器接收到（3）中的这个 ACK 报文后，服务器和客户端之间的连接就建立起来了，它们双方都获得彼此的窗口大小、初始序列号、最大报文段等信息。

注意：当建立一个新连接时，握手报文首部中的 SYN 标志置 1，此时，序号字段包含由发送方随机选择的初始序号 ISN（Initial Sequence Number）。建立连接的报文（SYN）将占用一个数据编号，因此发送方随后将要发送数据的第一个字节序号为 ISN+1。

#### 3.2 断开连接

断开连接的四次握手过程如下：

* （1）首先，当客户端应用程序主动执行关闭操作时，客户端会向服务器发送一个 FIN 标志置1 的报文段，用来关闭从客户端到服务器的数据传送，假设该报文段序号字段值为 C。

* （2）当服务器收到这个 FIN 报文段后，它返回一个 ACK 报文，确认序号为收到的序号加 1（即 C+1）。和 SYN 一样，一个 FIN 将占用一个序号。当客户端接收到这个 ACK 后，从客户端到服务器方向的连接就断开了。

* （3）服务器 TCP 向其上层应用程序通告客户端的断开操作，这会导致服务器程序关闭它的连接，同样，此时一个 FIN 标志置 1 的报文段将被发送到客户端，假设序号为 D。

* （4）客户端也会返回一个 ACK 报文段（D+1）作为响应，以完成该方向连接的断开操作。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_7.png">

#### 3.3 复位连接

复位连接时，发送方将发送一个 RST 标志被置 1 的报文段，接收方对复位报文段的处理是直接终止该连接上的数据传送，释放发送、接收缓冲，并向应用程序通知连接复位。 

#### 3.4 TCP状态转换图

TCP 为每个连接定义了 11 种状态，在 LwIP 中，这 11 种状态的名称定义以及各自的含义如下代码所示：

    //枚举类型，每个成员定义了 TCP 连接的一个状态
    enum tcp_state {
    　　CLOSED = 0, //没有连接
    　　LISTEN = 1, //服务器进入侦听态，等待客户端的连接请求
    　　SYN_SENT = 2, //连接请求已发送，等待确认
    　　SYN_RCVD = 3, //已收到对方的连接请求
    　　ESTABLISHED = 4, //连接已建立
    　　FIN_WAIT_1 = 5, //程序已关闭该连接
    　　FIN_WAIT_2 = 6, //另一端已接受关闭该连接
    　　CLOSE_WAIT = 7, //等待程序关闭连接
    　　CLOSING = 8, //两端同时收到对方的关闭请求
    　　LAST_ACK = 9, //服务器等待对方接受关闭操作
    　　TIME_WAIT = 10 //关闭成功，等待网络中可能出现的剩余数据
    };

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_8.png">

第一条路径描述了客户端申请建立连接与断开连接的整个过程，如图中虚线所示。与前面描述的一致，在客户端通过发送一个 SYN 报文段，主动向服务器申请一个连接，在报文发出之后客户端进入 SYN_SENT 状态等待服务器的 ACK 和 SYN 报文返回，当收到这个返回后，客户端对服务器的 SYN 进行确认，然后自身进入 ESTABLISHED 状态，与前面描述的三次握手过程完全一致。

当客户端申请断开连接时，它发送 FIN 报文给服务器申请断开连接，当 FIN 发送后，客户端进入 FIN_WAIT_1 状态等待服务器返回确认；当收到确认后，表明客户端到服务器方向的连接断开成功，此时客户端进入 FIN_WAIT_2 状态等待服务器到客户端方向的连接断开，等待等待……；当客户端收到服务器的 FIN 报文时，表明服务器向客户端发送断开连接请求，此时，客户端向服务器返回一个 ACK，并进入 TIME_WAIT 状态，在该状态下等待 2MSL（MSL，报文最大生存时间，在很多实现中取为 30 秒，在 LwIP 中取为 60 秒）时间后，客户端进入初始的 CLOSED 状态。 在连接处于 2MSL 等待时，任何迟到的报文段将被丢弃。上述过程与断开连接的四次握手过程完全相符。

另一条典型的状态转换路径是描述服务器的，如图 13­-6 中粗实线所示。服务器建立连接一般属于被动过程，它首先打开某个熟知端口，进入 LISTEN 状态以侦听客户端的连接请求。当服务器收到客户端的 SYN 连接请求，则进入 SYN_RCVD 状态，并向客户端返回一个 ACK 及自身的 SYN报文；此后，服务器等待客户端返回一个确认，收到该 ACK 后，服务器进入 ESTABLISHED 状态，在该状态下客户端可以和服务器进行稳定的数据交换。可见，连接建立的过程和前面描述的三次握手过程完全一致。

当服务器收到客户端发送的一个断开连接报文 FIN 时，则进入 CLOSE_WAIT 状态，并向上层应用程序通告这个消息，同时向客户端返回一个 ACK，此时客户端到服务器方向的连接断开成功；此后，当服务器上层应用处理完毕相关信息后会向客户端发送一个 FIN 报文，并进入 LAST_ACK状态，等待客户端返回最后 ACK，当收到返回的 ACK 后，此时服务器到客户端方向的连接断开成功，服务器端至此进入初始的 CLOSED 状态。此过程与断开连接的四次握手过程完全相符。

#### 3.5 特殊的状态转换

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_9.png">

两台主机同时执行连接请求时，握手报文的交互如图 13­7 所示，两台主机在同时发送自身的SYN 请求报文后各自进入 SYN_SENT 状态，等待对方的 ACK 返回，但此后，每个主机都收到对方的 SYN（其中的 ACK 标志没有置位），此时两端都可以判定已经遇到了同时打开的状况，两端都进入 SYN_RCVD 状态，同时对对方的 SYN 进行确认并再次发送自己的 SYN；当接收到对方的ACK+SYN 后，双方都进入 ESTABLISHED 状态。从这一点看，状态转换图中从 SYN_RCVD 到ESTABLISHED 转换的条件应该有两个，而图中只标出了一个。一个同时打开的连接需要交换 4 个报文段，比正常的三次握手多一个。要注意的是在这样的连接过程中，没有将任何一端称为客户端或服务器，因为每一端既是客户端又是服务器。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_10.png">

两台主机同时发起主动关闭时，报文交互如图 13­8 所示，当两个主机的用户程序同时执行关闭操作时，两主机都向对方发送一个 FIN 报文，并进入 FIN_WAIT_1 状态等待对方的 ACK 返回，但一段时间后，双方各自都收到对方的 FIN，而不是 ACK，此时两主机都判定遇到了双方同时主动关闭的状况。此时，两个主机就没有必要进入 FIN_WAIT_2 状态等待对方的 FIN 报文了，而是直接进入 CLOSING 状态，同时向对方发送一个 FIN 报文的确认，等到双方都收到对方的 ACK 后，两边都各自进入 TIME_WAIT 状态。

# 4 TCP控制块

#### 4.1 控制块数据结构

TCP 协议实现的本质就是对 TCP 控制块中各个字段的操作：在接收到 TCP报文段时，在所有控制块中查找，以得到和报文目的地相匹配的控制块，并调用控制块上注册的各个函数对报文进行处理；TCP 内核维护了一些周期性的定时事件，在定时处理函数中会对所有控制块进行处理 。

    //先定义一个宏，事实上在 LwIP 中定义了两种类型的 TCP 控制块，一种专门用于描述
    //处于 LISTEN 状态的连接，另一种用于描述处于其他状态的连接，这个宏定义出了两种
    //类型控制块共有的一些字段
    #define TCP_PCB_COMMON(type) \ //type 为宏参数
    　　type *next; \ //用于将控制块组成链表
    　　enum tcp_state state; \ //连接的状态，如 13.3.4 节所示
    　　u8_t prio; \ //优先级，可用于回收低优先级控制块
    　　void *callback_arg; \ //指向用户自定义数据，在函数回调时使用
    　　u16_t local_port; \ //连接绑定的本地端口
    　　err_t (* accept)(void *arg, struct tcp_pcb *newpcb, err_t err) //回调函数
    //当处于 LISTEN 状态的控制块侦听到连接，该函数会被调用
    //下面定义 TCP 控制块数据结构 tcp_pcb
    struct tcp_pcb {
    　　IP_PCB; //该宏在 ip.h 中被定义，包含源 IP 地址、目的 IP 地址两个重要字段
    　　TCP_PCB_COMMON(struct tcp_pcb); //两种控制块都具有的字段
    　　u16_t remote_port; //远端端口号
    　　u8_t flags; //控制块状态、标志字段
    　　//下面这些宏定义了 flags 字段中各位的含义
    　　#define TF_ACK_DELAY ((u8_t)0x01U) //延迟发送 ACK（推迟确认）
    　　#define TF_ACK_NOW ((u8_t)0x02U) //立即发送 ACK
    　　#define TF_INFR ((u8_t)0x04U) //连接处于快重传状态
    　　#define TF_TIMESTAMP ((u8_t)0x08U) //连接的时间戳选项已使能
    　　#define TF_FIN ((u8_t)0x20U) //应用程序已关闭该连接
    　　#define TF_NODELAY ((u8_t)0x40U) //禁止 Nagle 算法
    　　#define TF_NAGLEMEMERR ((u8_t)0x80U) //本地缓冲区溢出
    　　//下面所有字段记录了通信双方的数据发送状态，以及本连接上的各种定时器信息
    　　//接收窗口相关的字段
    　　u32_t rcv_nxt; //下一个期望接收的字节序号
    　　u16_t rcv_wnd; //当前接收窗口的大小，会随着数据的接收与递交动态变化
    　　u16_t rcv_ann_wnd; //将向对方通告的窗口大小，随着数据的接收与递交动态变化
    　　u32_t rcv_ann_right_edge; //上一次窗口通告时窗口的右边界值
    　　//下面这个字段记录了控制块上一次活动时的系统时间，也可以表示连接处于
    　　//某个状态的时间，控制块其他各计数器都基于 tmr 的值来实现
    　　u32_t tmr;
    　　//下面两个字段用于周期性的调用一个函数，polltmr 会周期性增加，
    　　u8_t polltmr, pollinterval; //当其值超过 pollinterval 时，poll 函数会被回调
    　　s16_t rtime; //重传定时器，该值随时间递增，当大于 rto 的值时重传报文
    　　u16_t mss; //对方可接收的最大报文段大小
    　　//RTT 估计相关的字段
    　　u32_t rttest; //RTT 估计时，以 500ms 为周期递增
    　　u32_t rtseq; //用于测试 RTT 的报文段序号
    　　s16_t sa, sv; //RTT 估计出的平均值及其时间差
    　　u16_t rto; //重发超时时间，使用上面的几个值计算出来
    　　u8_t nrtx; //重发次数，多次重发时，将使用该字段设置 rto 的值
    　　//快速重传与恢复相关的字段
    　　u32_t lastack; //接收到的最大确认号
    　　u8_t dupacks; //上述最大确认号被重复收到的次数
    　　//阻塞控制相关字段
    　　u16_t cwnd; //连接当前的阻塞窗口大小
    　　u16_t ssthresh; //拥塞避免算法启动阈值
    　　//发送窗口相关的字段
    　　u32_t snd_nxt; //下一个将要发送的数据的序号
    　　u16_t snd_wnd; //发送窗口的大小
    　　u32_t snd_wl1, snd_wl2; //上次窗口更新时收到的数据序号和确认序号
    　　u32_t snd_lbb; //下一个被缓冲的应用程序数据的编号
    　　u16_t acked; //上一次成功发送的字节数
    　　u16_t snd_buf; //可使用的发送缓冲区大小
    　　#define TCP_SNDQUEUELEN_OVERFLOW (0xffff­3) //该宏用于缓冲区上限溢出检查
    　　u16_t snd_queuelen; //缓冲数据已占用的 pbuf 个数
    　　//下面三个字段是用户数据缓冲的队列指针
    　　struct tcp_seg *unsent; //未发送的报文段队列
    　　struct tcp_seg *unacked; //发送了但未收到确认的报文段队列
    　　struct tcp_seg *ooseq; //接收到的无序报文段队列
    　　struct pbuf *refused_data; //指向上一次成功接收但未被应用层取用的数据 pbuf
    　　//下面是几个函数指针，用于回调，用户可以在初始化时注册这些函数
    　　err_t (* sent)(void *arg, struct tcp_pcb *pcb, u16_t space); //当数据被成功发送后被调用
    　　err_t (* recv)(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err); //接收到数据后被调用
    　　err_t (* connected)(void *arg, struct tcp_pcb *pcb, err_t err); //连接建立后被调用
    　　err_t (* poll)(void *arg, struct tcp_pcb *pcb); //该函数被内核周期性调用
    　　void (* errf)(void *arg, err_t err); //连接发生错误时调用
    　　u32_t keep_idle; //保活计时器的上限值
    　　u32_t persist_cnt; //坚持定时器计数值
    　　u8_t persist_backoff; //坚持定时器探查报文发送的数目
    　　u8_t keep_cnt_sent; //保活报文发送的次数
    }
    //下面定义用于 LISTEN 状态 TCP 连接的控制块结构 tcp_pcb_listen
    struct tcp_pcb_listen {
    　　IP_PCB; //该宏包含源 IP 地址、目的 IP 地址两个重要字段
    　　TCP_PCB_COMMON(struct tcp_pcb_listen); //两种控制块都具有的字段
    };

除了定义结构体 tcp_pcb，上面的代码还定义了结构体 tcp_pcb_listen，后者主要是用来描述处于 LISTEN 状态的连接。在后面会看到，处于 LISTEN 状态的连接只记录本地端口信息，不记录任何远程端口信息，一般只用在服务器端打开某个端口为客户端服务。处于 LISTEN 状态的控制块不会对应于任何一条有效连接，它也不会进行数据发送、连接握手之类的工作，因此描述 LISTEN 状态的控制块的结构体与 tcp_pcb 相比更小，使用它可以节省内存空间。

flags 字段，它描述了当前控制块的特性，例如是否允许立即发送 ACK、是否使能 Nagle 算法等，这些标志位是提高 TCP 传输性能的关键；而 TCP 状态字段叫做 state，它表示一个连接在整个通信过程中的状态变迁。

#### 4.2 控制块链表

定义四条链表的代码如下，

* tcp_bound_pcbs 链表用来连接新创建的且绑定了本地端口的控制块，可以认为此时的控制块处于 closed 状态；

* tcp_listen_pcbs 链表用来连接处于 LISTEN 状态的控制块，该状态下是用结构体 tcp_pcb_listen 来描述一个本地连接的；

* tcp_tw_pcbs 链表用来连接处于 TIME_WAIT 状态的控制块；

* tcp_active_pcbs 链表用来连接处于 TCP 状态转换图中其他所有状态的控制块。

代码定义如下：

    struct tcp_pcb *tcp_bound_pcbs; //连接所有进行了端口号绑定，但是还没有发起连接（主动连接）或进入侦听状态（被动连接）的控制块
    union tcp_listen_pcbs_t tcp_listen_pcbs; //连接所有进入侦听状态（被动连接）的控制块
    struct tcp_pcb *tcp_active_pcbs; //连接所有处于其他状态的控制块
    struct tcp_pcb *tcp_tw_pcbs; //连接所有处于 TIME­WAIT 状态的控制块

#### 4.3 接收窗口

* rcv_nxt 是自己期望收到的下一个数据字节编号；

* rcv_wnd 表示接收窗口的大小；

* rcv_ann_wnd 表示将向对方通告的窗口大小值，这个值在报文发送时会被填在首部中的窗口大小字段；

* rcv_ann_right_edge 记录了上一次窗口通告时窗口右边界取值，该字段在窗口滑动过程中经常被用到。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_11.png">

#### 4.4 发送窗口

* ast 字段记录了被接收方确认的最高序列号；

* snd_nxt 表示自己将要发送的下一个数据的起始编号；

* snd_wnd 记录了当前的发送窗口大小，它常被设置为接收方通告的接收窗口值；

* snd_lbb 记录了下一个将被应用程序缓存的数据的起始编号。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_12.png">

snd_wl1、snd_wl2 两个字段与发送窗口的更新密切相关，它们分别记录上一次窗口更新时收到的数据序号和确认序号。

# 5 TCP编程函数

TCP 编程的本质上也是对 TCP 控制块的操作。这一节将看到 TCP 的几个基础编程函数，这些函数无论是在客户机程序还是在服务器程序的编写中都具有很重要的作用。另一方面，它们也是实现上层 API 函数的基础。通过对这些函数的学习，可以加深读者对 TCP 状态转换图和 TCP 控制块的理解。

#### 5.1 控制块新建

实现该功能的编程函数是 tcp_new，它通过调用 tcp_alloc 函数为连接分配一个 TCP 控制块结构 tcp_pcb。tcp_alloc 函数代码如下，它首先为新的 tcp_pcb 分配内存池空间，若空间不够，则函数会释放处于 TIME­WAIT 状态的控制块或其他优先级更低的控制块（使用控制块的 prio 字段）为新控制块寻找空间。当内存空间成功分配后，函数会初始化 tcp_pcb 中各个字段的内容。

    struct tcp_pcb *tcp_alloc(u8_t prio)
    {
    　　struct tcp_pcb *pcb;
    　　u32_t iss;
    　　pcb = memp_malloc(MEMP_TCP_PCB); //申请一个内存池空间
    　　if (pcb == NULL) { //申请失败
    　　　　tcp_kill_timewait(); //回收最老的 TIME­WAIT 状态控制块
    　　　　pcb = memp_malloc(MEMP_TCP_PCB); //再次申请分配
    　　　　if (pcb == NULL) { //申请失败
    　　　　　　tcp_kill_prio(prio); //回收优先级较低的控制块
    　　　　　　pcb = memp_malloc(MEMP_TCP_PCB); //再次申请分配
    　　　　}
    　　}
    　　if (pcb != NULL) { //若分配成功，则初始化相关各个字段
    　　　　memset(pcb, 0, sizeof(struct tcp_pcb)); //清 0 所有字段
    　　　　pcb­>prio = TCP_PRIO_NORMAL; //设置控制块优先级
    　　　　pcb­>snd_buf = TCP_SND_BUF; //可使用的发送缓冲区大小
    　　　　pcb­>snd_queuelen = 0; //缓冲区已占用的 pbuf 个数
    　　　　pcb­>rcv_wnd = TCP_WND; //接收窗口
    　　　　pcb­>rcv_ann_wnd = TCP_WND; //通告接收窗口
    　　　　pcb­>tos = 0; //服务类型
    　　　　pcb­>ttl = TCP_TTL; //TTL 字段
    　　　　pcb­>mss = (TCP_MSS > 536) ? 536 : TCP_MSS; //初始化最大报文段大小
    　　　　pcb­>rto = 3000 / TCP_SLOW_INTERVAL; //初始化超时时间
    　　　　pcb­>sa = 0; //初始化与 RTT 估计相关的字段
    　　　　pcb­>sv = 3000 / TCP_SLOW_INTERVAL;
    　　　　pcb­>rtime = ­1;
    　　　　pcb­>cwnd = 1; //初始化阻塞窗口
    　　　　iss = tcp_next_iss(); //获得初始序列号
    　　　　pcb­>snd_wl2 = iss; //初始化发送窗口的各个字段
    　　　　pcb­>snd_nxt = iss;
    　　　　pcb­>lastack = iss;
    　　　　pcb­>snd_lbb = iss;
    　　　　pcb­>tmr = tcp_ticks; //记录控制块创建时的系统时刻
    　　　　pcb­>polltmr = 0; //清空周期性事件的定时器
    　　　　pcb­>recv = tcp_recv_null; //注册接收数据的默认上层函数
    　　　　pcb­>keep_idle = TCP_KEEPIDLE_DEFAULT; //保活计数器计数上限值
    　　　　pcb­>keep_cnt_sent = 0; //保活报文发送的次数
    　　}
    　　return pcb;
    }

#### 5.2 控制块绑定

tcp_bind 函数的工作很简单，与前面讲的 udp_bind 函数相似，就是将两个参数的值赋值给 TCP 控制块中 local_ip和 local_port 的字段。但这里有个前提，就是这个端点（IP 地址、端口号）没有被其他控制块使用。为了达到这个目的，函数需要先遍历各个控制块链表，以保证这个端点（IP 地址、端口）没有被其他控制块使用 。如果遍历完这些链表后，都没有找到相应的端点（IP 地址、端口），则说明该端点可用，此时tcp_bind 将 进 行 上述的赋值操作 。 最后 ， tcp_bind 还 要将绑 定完毕的控制块插入到链表tcp_bound_pcbs首部。

#### 5.3 控制块监听

从 TCP 状态转换图上可知，服务器程序需进入 LISTEN 状态等待客户端的连接。此时，服务器需要调用函数 tcp_listen 使相应的控制块进入 LISTEN 状态。 tcp_listen 本质上是一个宏，它定义为函数 tcp_listen_with_backlog，后者代码如下：

    //函数功能：将某个绑定的控制块置为侦听状态
    //参数 pcb：操作的控制块对象；参数 backlog：用户参数，这里未使用到
    //返回值：指向处于侦听状态的控制块
    struct tcp_pcb *tcp_listen_with_backlog(struct tcp_pcb *pcb, u8_t backlog)
    {
    　　struct tcp_pcb_listen *lpcb;
    　　if (pcb­>state == LISTEN) { //如果控制块是 LISTEN 状态，直接返回
    　　　　return pcb;
    　　}
    　　lpcb = memp_malloc(MEMP_TCP_PCB_LISTEN); //分配一个内存池空间
    　　if (lpcb == NULL) { //申请失败，直接返回
    　　　　return NULL;
    　　}
    　　lpcb­>callback_arg = pcb­>callback_arg; //拷贝控制块的各个字段
    　　lpcb­>local_port = pcb­>local_port;
    　　lpcb­>state = LISTEN;
    　　lpcb­>so_options = pcb­>so_options;
    　　lpcb­>so_options |= SOF_ACCEPTCONN;
    　　lpcb­>ttl = pcb­>ttl;
    　　lpcb­>tos = pcb­>tos;
    　　ip_addr_set(&lpcb­>local_ip, &pcb­>local_ip);
    　　TCP_RMV(&tcp_bound_pcbs, pcb); //从 tcp_bound_pcbs 链表中删除控制块
    　　memp_free(MEMP_TCP_PCB, pcb); //释放控制块内存空间
    　　lpcb­>accept = tcp_accept_null; //接受客户端连接的默认回调函数
    　　TCP_REG(&tcp_listen_pcbs.listen_pcbs, lpcb); //控制块加入 tcp_listen_pcbs 链表首部
    　　return (struct tcp_pcb *)lpcb;
    }

进入 LISTEN 状态后，服务器就等待客户端发送来的 SYN 报文进行连接，当内核收到一个 SYN握手报文后，会遍历 tcp_listen_pcbs 链表，以找到和报文中目的 IP 地址、目的端口号匹配的控制块。若找到匹配的 LISTEN 状态控制块，则内核会新建一个 tcp_pcb 结构，并将 tcp_pcb_listen 结构中各个字段拷贝至其中，同时填写结构中的源端口号、源 IP 地址等字段，最后在 tcp_active_pcbs 链表中添加这个新 tcp_pcb 结构。这样，新 TCP 控制块就处在 tcp_active_pcbs 中了，此时这个 tcp_pcb 结构的 state 字段应该设置为 SYN_RCVD。注意 tcp_listen_pcbs 链表中的匹配 tcp_pcb_listen 结构将一直存在，它并不会被删除，以等待其他客户端的连接，这正是服务器需要实现的功能。

#### 5.4 控制块连接

对于客户端程序来说，它需要执行主动打开操作，主动打开就是向服务器发送一个 SYN 握手报文段，这一点是通过调用函数 tcp_connect 来实现的。函数将控制块从链表 tcp_bound_pcbs 搬到tcp_active_pcbs，同时控制块状态设置为 SYN_SENT 状态，并初始化发送、接收相关的字段，最后调用函数 tcp_enqueue 为控制块组装一个握手报文，并调用函数 tcp_output 将报文发送出去。

    //函数功能：向服务器发送一个 SYN 握手报文
    //参数 ipaddr：服务器 IP 地址，参数 port：服务器端口号
    //参数 connected：注册给控制块的回调函数，当 SYN 报文得到服务器的正确响应
    //（SYN+ACK）后，该函数将被内核回调
    err_t tcp_connect(struct tcp_pcb *pcb, struct ip_addr *ipaddr, u16_t port,err_t (* connected)(void *arg, struct tcp_pcb *tpcb, err_t err))
    {
    　　err_t ret;
    　　u32_t iss;
    　　if (ipaddr != NULL) { //服务器 IP 地址有效，则在连接中记录该 IP 地址
    　　　　pcb­>remote_ip = *ipaddr;
    　　} else { //服务器 IP 地址无效，返回错误
    　　　　return ERR_VAL;
    　　}
    　　pcb­>remote_port = port; //记录服务器端口（目的端口）
    　　if (pcb­>local_port == 0) { //如果本地端口未绑定，则绑定一个短暂端口
    　　　　pcb­>local_port = tcp_new_port();
    　　}
    　　iss = tcp_next_iss(); //初始化序号
    　　pcb­>rcv_nxt = 0; //设置发送窗口的各个字段
    　　pcb­>snd_nxt = iss;
    　　pcb­>lastack = iss ­ 1;
    　　pcb­>snd_lbb = iss ­ 1;
    　　pcb­>rcv_wnd = TCP_WND; //设置默认接收窗口各个字段值
    　　pcb­>rcv_ann_wnd = TCP_WND;
    　　pcb­>rcv_ann_right_edge = pcb­>rcv_nxt;
    　　pcb­>snd_wnd = TCP_WND;
    　　pcb­>mss = (TCP_MSS > 536) ? 536 : TCP_MSS; //初始化最大报文段大小
    　　pcb­>cwnd = 1; //初始化阻塞窗口
    　　pcb­>ssthresh = pcb­>mss * 10;
    　　pcb­>state = SYN_SENT; //控制块设置为 SYN_SENT 状态
    　　pcb­>connected = connected; //注册 connected 回调函数
    　　TCP_RMV(&tcp_bound_pcbs, pcb); //控制块从 tcp_bound_pcbs 链表中删除
    　　TCP_REG(&tcp_active_pcbs, pcb); //控制块加入到 tcp_active_pcbs 链表
    　　//下面调用函数 tcp_enqueue 为连接构造一个报文段，首部 SYN 置 1，数据为空，
    　　//且报文中包含最大报文段选项
    　　ret = tcp_enqueue(pcb, NULL, 0, TCP_SYN, 0, TF_SEG_OPTS_MSS);
    　　if (ret == ERR_OK) { //将控制块上连接的报文段发送出去
    　　　　tcp_output(pcb);
    　　}
    　　return ret;
    }

#### 5.5 发送数据

当连接建立起来后，双方都可以调用函数 tcp_write 向对方发送数据，该函数的功能实现如下所示：

    //函数功能：连接向另一方发送数据，该函数构造一个报文段并放在控制块缓冲队列中
    //参数 pcb：指向相应连接的控制块
    //参数 data：待发送数据的起始地址
    //参数 len：待发送数据的长度
    //参数 apiflags：数据是否进行拷贝，以及报文段首部是否设置 PSH 标志
    err_t tcp_write(struct tcp_pcb *pcb, const void *data, u16_t len, u8_t apiflags)
    {
    　　if (pcb­>state == ESTABLISHED ||pcb­>state == CLOSE_WAIT || //控制块状态允许
    　　　　pcb­>state == SYN_SENT ||pcb­>state == SYN_RCVD) {
    　　　　if (len > 0) {//发送数据的长度不为 0
    　　　　　　return tcp_enqueue(pcb, (void *)data, len, 0, apiflags, 0); //调用函数构造报文段
    　　　　}
    　　　　return ERR_OK;
    　　　　} else { //控制块状态不允许发送数据，则返回错误
    　　　　　　return ERR_CONN;
    　　　　}
    }

在数据的接收方，若用户想处理接收到的数据，则必须向控制块的recv字段注册自定义处理函数。当内核收到关于某连接的数据后，对应控制块中注册的 recv 函数会被内核回调执行，这点和 UDP 中的回调执行完全相同

#### 5.6 关闭连接

在任意时刻，应用程序都可以调用函数 tcp_close 关闭当前的连接。 

　　当控 制块 处于 CLOSED 状态 时， 控制 块将 在tcp_bound_pcbs 链表中被删除，相应的空间被释放；

　　当控制块处于 LISTEN 状态时，控制块会在tcp_listen_pcbs 链表中被删除，相应的空间被释放，控制块再也不能侦听任何客户端的连接请求；　　

　　当控制块处于 SYN_SENT 状态时，控制块会在 tcp_active_pcbs 链表中被删除，相应的空间被释放；

当控制处于其他状态时，一个 FIN 握手报文将被发送给连接的另一方，同时，连接的状态会按照TCP 状态图做相应的转换。 

    //函数功能：关闭一个 TCP 连接
    err_t tcp_close(struct tcp_pcb *pcb)
    {
    　　err_t err;
    　　switch (pcb­>state) { //根据控制块的不同状态做出不同的处理
    　　case CLOSED: // CLOSED 状态
    　　　　err = ERR_OK;
    　　　　TCP_RMV(&tcp_bound_pcbs, pcb); //从链表上删除
    　　　　memp_free(MEMP_TCP_PCB, pcb); //释放内存空间
    　　　　pcb = NULL;
    　　　　break;
    　　case LISTEN: // LISTEN 状态
    　　　　err = ERR_OK;
    　　　　tcp_pcb_remove((struct tcp_pcb **)&tcp_listen_pcbs.pcbs, pcb); //从链表上删除
    　　　　memp_free(MEMP_TCP_PCB_LISTEN, pcb); //释放内存空间
    　　　　pcb = NULL;
    　　　　break;
    　　case SYN_SENT: //SYN_SENT 状态
    　　　　err = ERR_OK;
    　　　　tcp_pcb_remove(&tcp_active_pcbs, pcb); //从链表上删除
    　　　　memp_free(MEMP_TCP_PCB, pcb); //释放内存空间
    　　　　pcb = NULL;
    　　　　break;
    　　case SYN_RCVD: //SYN_RCVD 状态
    　　　　err = tcp_send_ctrl(pcb, TCP_FIN); //构造主动关闭 FIN 握手报文
    　　　　if (err == ERR_OK) { //转到 FIN_WAIT_1 状态
    　　　　　　pcb­>state = FIN_WAIT_1;
    　　　　}
    　　　　break;
    　　case ESTABLISHED: //ESTABLISHED 状态
    　　　　err = tcp_send_ctrl(pcb, TCP_FIN); //构造主动关闭 FIN 握手报文
    　　　　if (err == ERR_OK) {
    　　　　　　pcb­>state = FIN_WAIT_1; //转到 FIN_WAIT_1 状态
    　　　　}
    　　　　break;
    　　case CLOSE_WAIT: //CLOSE_WAIT 状态，即已收到对方的 FIN
    　　　　err = tcp_send_ctrl(pcb, TCP_FIN); //构造 FIN 握手报文
    　　　　if (err == ERR_OK) {
    　　　　　　pcb­>state = LAST_ACK; //进入 LAST_ACK 等待 ACK 或超时
    　　　　}
    　　　　break;
    　　default: //对于其他状态，由 TCP 定时函数来处理
    　　　　err = ERR_OK;
    　　　　pcb = NULL;
    　　　　break;
    　　}
    　　if (pcb != NULL && err == ERR_OK) {//调用函数，发送控制块队列中的剩余报文段
    　　　　tcp_output(pcb); //包括 FIN 握手报文段
    　　}
    　　return err;
    }

#### 5.7 其它函数

还有几个函数也与 TCP 编程密切相关，如表 13­3 所示，它们的实现代码也很简单，表中简要介绍了它们的功能 

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_13.png">

# 6 TCP输出处理

#### 6.1 报文段缓冲

在内核中，所有待发送数据、已接收数据都是以报文段的形式被保存，报文段存放于 pbuf 中，为了实现对所有这些报文段 pbuf 的管理，内核引入了一个名为 tcp_seg 的结构。

    //定义组织 TCP 报文段的结构
    struct tcp_seg {
    　　struct tcp_seg *next; //该指针用于将报文段组织为队列的形式
    　　struct pbuf *p; //指向装载报文段的 pbuf
    　　void *dataptr; //指向报文段中的 TCP 数据区域
    　　u16_t len; //报文段中的数据长度
    　　u8_t flags; //表示所描述报文段的选项属性
    　　#define TF_SEG_OPTS_MSS (u8_t)0x01U //包含了最大报文段大小选项
    　　#define TF_SEG_OPTS_TS (u8_t)0x02U //包含了时间戳选项
    　　struct tcp_hdr *tcphdr; //指向报文段中的 TCP 首部
    };

每个控制块中都维护了三个缓冲队列，unsent、unacked、ooseq 三个字段分别为队列的首指针。unsent 用于连接还未被发送出去的报文段、unacked 用于连接已经发送出去但是还未被确认的报文段、ooseq 用于连接接收到的无序报文段。 如图7-14：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_14.png">

#### 6.2 报文段构建

    err_t tcp_enqueue(struct tcp_pcb *pcb, void *arg, u16_t len,u8_t flags, u8_t apiflags, u8_t optflags)

* pcb 是相应连接的 TCP 控制块；

* arg 是待发送的数据指针；

* len是待发送数据的长度，以字节为单位；

* flags 是 TCP 报文段首部中的标志字段，主要用在连接建立或断开时的握手过程中；

* apiflags 表示应用程序期望 TCP 内核对该报文段进行的额外操作，包括是否拷贝数据、是否设置首部中的 PSH 标志；

* optflags 表示该报文首部中应该包含的 TCP 选项，目前只支持最大报文段选项和时间戳选项。

如图 13­12 展示了该函数的流程图，它的主要工作流程如下：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_15.png">


* （1）pcb­>snd_buf，若缓冲区不够，则不会对该数据进行任何处理 ；

* （2）pcb­>snd_queuelen 是否超过了控制块上允许挂接的 pbuf 个数上限值 TCP_SND_QUEUELEN 

* （3）接下来 tcp_enqueue 函数会将数据组装成为 TCP 报文段，每个报文段 pbuf 还会用一个tcp_seg 类型的结构来描述 ；

* （4）接下来，函数 tcp_enqueue 需要将临时队列 queue 上的报文段放入控制块的 unsent 队列上 ，

* （5）最后，函数需要调整 TCP 控制块中的相关字段值，这也是最重要的地方，代码如下所示：

代码如下：

    if ((flags & TCP_SYN) || (flags & TCP_FIN)) { // SYN 或 FIN 占用 1 个数据长度
    　　++len; //计算所有报文段占用的长度
    }
    if (flags & TCP_FIN) { // 若为 FIN，则设置控制块 flags 字段为相应状态
    　　pcb­>flags |= TF_FIN;
    }
    pcb­>snd_lbb += len; //下一个要被缓冲数据的序号
    pcb­>snd_buf ­= len; //减小空闲的发送缓冲空间
    pcb­>snd_queuelen = queuelen; //控制块已占用的 pbuf 个数


#### 6.3 报文段发送

发送报文段的函数叫做 tcp_output，该函数的功能描述为“Find out what we can send and send it” 。控制块的 flags 字段设置了 TF_ACK_NOW 标志，则函数必须马上发送一个带有 ACK 的报文 。若此时 unsent 队列中无数据发送或者发送窗口此时不允许发送数据，则函数需要发送一个不含任何数据的 ACK 报文段。 接下来，tcp_output 需将发送出去的报文段放入控制块 unacked 缓冲队列中，以便后续的重发操作。 当unsent 队列上的第一个报文段处理完毕后，tcp_output函数还会按照上述方法依次处理unsent队列上的剩余报文段，直到数据被全部发送出去或者发送窗口被填满。

    //发送控制块缓冲队列中的报文段
    err_t tcp_output(struct tcp_pcb *pcb)
    {
    　　struct tcp_seg *seg, *useg;
    　　u32_t wnd, snd_nxt;
    　　//如果控制块当前正有数据被处理，这里不做任何输出，直接返回
    　　if (tcp_input_pcb == pcb) { //在控制块的数据处理完成后，内核会再次
    　　　　return ERR_OK; //调用 tcp_output 发送数据，见函数 tcp_input
    　　}
    　　//从发送窗口和阻塞窗口取小者得到有效发送窗口，拥塞避免会讲解到这个原理
    　　wnd = LWIP_MIN(pcb­>snd_wnd, pcb­>cwnd);
    　　seg = pcb­>unsent; //未发送队列
    　　//若要求立即确认，但该 ACK 又不能被捎带出去，则只发送一个纯 ACK 的报文段
    　　if (pcb­>flags & TF_ACK_NOW &&
    　　　　(seg == NULL ||ntohl(seg­>tcphdr­>seqno) ­ pcb­>lastack + seg­>len > wnd)) {
    　　　　return tcp_send_empty_ack(pcb); //发送只带 ACK 的报文段
    　　}
    　　//将 useg 变量指向未确认队列的尾部 tcp_seg 结构
    　　useg = pcb­>unacked;
    　　if (useg != NULL) {
    　　　　for (; useg­>next != NULL; useg = useg­>next); //得到尾部
    　　}
    　　//若当前有效发送窗口允许报文的发送，则循环发送报文，直至填满窗口
    　　while (seg != NULL &&ntohl(seg­>tcphdr­>seqno) ­ pcb­>lastack + seg­>len <= wnd) {
    　　　　//如果 Nagle 算法有效，或者当前控制块内存有错误，则不发送报文
    　　　　if((tcp_do_output_nagle(pcb) == 0) &&
    　　　　　　((pcb­>flags & (TF_NAGLEMEMERR | TF_FIN)) == 0)){
    　　　　　　break; //结束发送循环
    　　　　}
    　　　　//否则，当前报文段可以发送
    　　　　pcb­>unsent = seg­>next; //从缓冲队列中删除报文段
    　　　　if (pcb­>state != SYN_SENT) { //当前不为 SYN_SENT 状态
    　　　　　　TCPH_SET_FLAG(seg­>tcphdr, TCP_ACK); //填写首部中的 ACK 标志
    　　　　　　pcb­>flags &= ~(TF_ACK_DELAY | TF_ACK_NOW); //清除标志位
    　　　　}
    　　　　tcp_output_segment(seg, pcb); //调用函数发送报文段
    　　　　snd_nxt = ntohl(seg­>tcphdr­>seqno) + TCP_TCPLEN(seg); //计算 snd_nxt 值
    　　　　if (TCP_SEQ_LT(pcb­>snd_nxt, snd_nxt)) { //更新下一个要发送的数据编号
    　　　　　　pcb­>snd_nxt = snd_nxt;
    　　　　}
    　　　　//如果发送出去的报文段数据长度不为 0，或者带有 SYN、FIN 标志，则将该报
    　　　　//文段加入到未确认队列中以便超时后重传
    　　　　if (TCP_TCPLEN(seg) > 0) {
    　　　　　　seg­>next = NULL; //清空报文段 next 字段
    　　　　　　if (pcb­>unacked == NULL) { //若未确认队列为空，则直接挂接
    　　　　　　　　pcb­>unacked = seg;
    　　　　　　　　useg = seg; //变量 useg 指向未确认队列尾部
    　　　　　　} else {
    　　　　　　　　//如果未确认队列不为空，则需要把当前报文按照顺序组织在队列中
    　　　　　　　　if (TCP_SEQ_LT(ntohl(seg­>tcphdr­>seqno), ntohl(useg­>tcphdr­>seqno))){
    　　　　　　　　　　//如果当前报文的序列号比队列尾部报文的序列号低，则从队列首部开始
    　　　　　　　　　　//查找合适的位置，插入报文段
    　　　　　　　　　　struct tcp_seg **cur_seg = &(pcb­>unacked);
    　　　　　　　　　　while (*cur_seg &&
    　　　　　　　　　　　　TCP_SEQ_LT(ntohl((*cur_seg)­>tcphdr­>seqno), ntohl(seg­>tcphdr­>seqno))) {
    　　　　　　　　　　　　cur_seg = &((*cur_seg)­>next );
    　　　　　　　　　　} //找到插入位置，将报文段插入到队列中
    　　　　　　　　　　seg­>next = (*cur_seg);
    　　　　　　　　　　(*cur_seg) = seg;
    　　　　　　　　} else { //报文段序号最高，则放在未确认队列尾部
    　　　　　　　　　　useg­>next = seg;
    　　　　　　　　　　useg = useg­>next;
    　　　　　　　　}
    　　　　　　}
    　　　　} else { //报文段长度为 0，不需要重传，直接删除
    　　　　　　　tcp_seg_free(seg);
    　　　　}
    　　　　seg = pcb­>unsent; //发送下一个报文段
    　　}//while
    　　//如果发送窗口被填满导致报文段不能发送，则启动零窗口探测，以保证能
    　　//准确地收到接收方的窗口通告，关于窗口探测，将在后续讲解
    　　if (seg != NULL && pcb­>persist_backoff == 0 &&
    　　ntohl(seg­>tcphdr­>seqno) ­ pcb­>lastack + seg­>len > pcb­>snd_wnd) {
    　　　　pcb­>persist_cnt = 0; //清 0 已发送的窗口探测包数目
    　　　　pcb­>persist_backoff = 1; //启动探测定时器
    　　}
    　　pcb­>flags &= ~TF_NAGLEMEMERR; //报文段成功发送，清除内存错误标志
    　　return ERR_OK;
    }

# 7 TCP输入处理

#### 7.1 TCP层函数调用流程

TCP 报文被 IP 层递交给 tcp_input 函数，这个函数可以说是 TCP 层的总输入函数，它会为报文段寻找一个匹配的 TCP 控制块，根据控制块状态的不同，调用 tcp_timewait_input、tcp_listen_input或 tcp_process 处理报文段；这里的重点是函数 tcp_process，它实现了图 13­6 中所示的 TCP 状态机，函数根据报文信息完成连接状态的变迁，同时，若报文中有数据，则函数 tcp_receive 会被调用；整个过程中的难点在于函数 tcp_receive，它完成了 TCP 中的数据接收、数据重组等工作，同时 TCP中各种性能算法的实现也在该函数中完成。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_16.png">

#### 7.2 报文输入

对于每一个待处理报文，tcp_input 都将它们的信息记录在一些全局变量中，其他各函数可以直接操作这些全局变量来得到想要的信息，这些全局变量的定义如下：

    //定义一些全局静态变量，这些变量可以在该文件的所有函数中直接引用
    static struct tcp_seg inseg; //tcp_seg 结构，用来描述输入的报文段
    static struct tcp_hdr *tcphdr; //指向报文段中的 TCP 首部
    static struct ip_hdr *iphdr; //指向 IP 数据报首部
    static u32_t seqno, ackno; //报文段 TCP 首部中的序号字段与确认号字段值
    static u8_t flags; //首部中的标志字段值
    static u16_t tcplen; //TCP 报文段长度
    static u8_t recv_flags; //该变量记录了所有函数对当前报文段的处理结果
    static struct pbuf *recv_data; //指向报文段中的数据 pbuf
    struct tcp_pcb *tcp_input_pcb; //处理当前报文段的控制块

这里，画出 tcp_input 函数的流程图如图 13­14 所示，tcp_input 函数开始会对 IP 层递交进来的报文段进行一些基本操作，如丢弃广播或多播数据报、数据校验和验证，同时提取 TCP 报文首部各个字段填写到上述全局变量中。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_7_17.png">

接下来，函数根据 TCP 报文段中（源 IP 地址、目的 IP 地址、源端口号、目的端口号）四个字段的值来查找与该报文段匹配的控制块。首先遍历 tcp_active_pcbs 链表，若找到，则调用tcp_process 函数对该报文进行处理。若找不到，则再分别到 tcp_tw_pcbs 和 tcp_listen_pcbs 链表中寻找，找到则调用各自的报文处理函数 tcp_timewait_input 和 tcp_listen_input 对报文进行处理，若到这里都还未找到匹配的 TCP 控制块，则 tcp_input 会调用函数 tcp_rst 向源主机发送一个 TCP 复位报文。

tcp_timewait_input 和 tcp_listen_input 分别是处于 TIME_WAIT 和 LISTEN 状态的控制块处理输入报文的函数，稍后两节将讲解这两个函数。对于其他状态的控制块，它们的处理函数叫做tcp_process，这个函数是整个 TCP 部分最重要的函数，它完成了 TCP 状态机的实现，同时它还调用函数 tcp_receive 处理报文中的数据。

最重要的，tcp_input 函数会等待 tcp_process 执行结束，且相关的执行结果（控制块的状态变迁）已经被保存在了全局变量 recv_flags 中，同时数据接收结果也被保存在了 recv_data 中，被当前报文确认的已发送数据的长度保存在了控制块的 acked 字段中。tcp_input 函数会根据这些值分别回调用户注册的 recv、sent 等函数，如下代码所示：

    void tcp_input(struct pbuf *p, struct netif *inp)
    {
    　　……
    　　tcp_input_pcb = pcb; //记录处理当前报文的控制块
    　　err = tcp_process(pcb); //处理报文段
    　　//若返回值为 ERR_ABRT，说明控制块已经被完全删除，这里什么也不需要做
    　　if (err != ERR_ABRT) { //不为 ERR_ABRT
    　　　　if (recv_flags & TF_RESET) { //若收到对方的复位报文，连接出错，回调用户的
    　　　　　　TCP_EVENT_ERR(pcb­>errf, pcb­>callback_arg, ERR_RST); //errf 函数
    　　　　　　tcp_pcb_remove(&tcp_active_pcbs, pcb); //删除控制块
    　　　　　　memp_free(MEMP_TCP_PCB, pcb); //释放控制块空间
    　　　　} else if (recv_flags & TF_CLOSED) { //双方连接成功断开
    　　　　　　tcp_pcb_remove(&tcp_active_pcbs, pcb); //删除控制块
    　　　　　　memp_free(MEMP_TCP_PCB, pcb); //释放控制块空间
    　　　　} else {
    　　　　　　err = ERR_OK; //若有数据被确认，则回调用户的 sent 函数
    　　　　　　if (pcb­>acked > 0) {
    　　　　　　　　TCP_EVENT_SENT(pcb, pcb­>acked, err);
    　　　　　　}
    　　　　　　if (recv_data != NULL) { //如果有数据被接收到
    　　　　　　　　if(flags & TCP_PSH) { //若有 PSH 标志，则设置 pbuf 首部的 flags 字段
    　　　　　　　　　　recv_data­>flags |= PBUF_FLAG_PUSH;
    　　　　　　　　}
    　　　　　　　　TCP_EVENT_RECV(pcb, recv_data, ERR_OK, err); //回调用户的 recv 函数
    　　　　　　　　if (err != ERR_OK) { //如果用户处理数据失败
    　　　　　　　　　　pcb­>refused_data = recv_data; //将数据暂存在 refused_data 中
    　　　　　　　　}
    　　　　　　}
    　　　　　　//如果收到对方的 FIN 请求，则用一个 NULL 的数据指针回调 recv，
    　　　　　　/通过这种方式用户程序可以知道对方的关闭请求
    　　　　　　if (recv_flags & TF_GOT_FIN) {
    　　　　　　　　TCP_EVENT_RECV(pcb, NULL, ERR_OK, err);
    　　　　　　}
    　　　　　　tcp_input_pcb = NULL; //清空全局变量
    　　　　　　tcp_output(pcb); //尝试输出报文
    　　　　}
    　　}// if (err != ERR_ABRT)
    ……
    }

#### 7.3 TIME_WAIT状态的报文处理

处于 TIME_WAIT 状态的控制块调用 tcp_timewait_input 处理收到的报文段，在该状态下，关闭连接的握手过程已经结束，正在等待 2MSL 超时，在该状态下的报文段应该是连接中的旧数据，直接删除即可，来看看源代码中是怎样处理的。

    //函数功能：TIME_WAIT 状态的控制块处理报文段
    //函数说明：所有报文相关的信息已被保存在上节所述的全局变量中，在该状态下，应该
    //忽略掉所有 RST、ACK、FIN 置位的报文，且任何数据都会被删除，但需要向发送方
    //返回 ACK 报文
    static err_t tcp_timewait_input(struct tcp_pcb *pcb)
    {
    　　if (flags & TCP_RST) { //若报文的 RST 置位，直接返回
    　　　　return ERR_OK;
    　　}
    　　if (flags & TCP_SYN) { //若报文段包含了 SYN 握手信息
    　　　　//且握手数据编号在接收窗口内，则向对方发送 RST 报文
    　　　　if (TCP_SEQ_BETWEEN(seqno, pcb­>rcv_nxt, pcb­>rcv_nxt+pcb­>rcv_wnd)) {
    　　　　　　tcp_rst(ackno, seqno + tcplen, &(iphdr­>dest), &(iphdr­>src),
    　　　　　　tcphdr­>dest, tcphdr­>src);
    　　　　　　return ERR_OK;
    　　　　}
    　　} else if (flags & TCP_FIN) { //若报文段包含了 FIN 握手信息
    　　　　pcb­>tmr = tcp_ticks; //复位 2MSL 等待时间，控制块重新等待 2MSL
    　　}
    　　if ((tcplen > 0)) {//对于有数据的报文，或者在接收窗口外的 SYN 报文
    　　　　pcb­>flags |= TF_ACK_NOW; //发送一个 ACK 报文段
    　　　　return tcp_output(pcb);
    　　}
    　　return ERR_OK;
    }

#### 7.4 LISTEN状态的报文处理

从状态转换图上可以看出，处于 LISTEN 状态的控制块只能响应 SYN 握手包，所以，tcp_listen_input 函数对非 SYN 握手包返回一个 TCP 复位报文。客户端的连接请求 SYN 包中，报文首部的 ACK 标志必为 0，所以 tcp_listen_input 函数通过检验该位来判断一个报文是否合法。

接下来，函数通过验证 SYN 位来确认该报文是否为 SYN 报文。若是，则需要新建一个 tcp_pcb结构，因为处于 tcp_listen_pcbs 上的控制块结构是 tcp_pcb_listen 结构的，而其他链表上的控制块结构是 tcp_pcb 结构的，所以这里新建一个 tcp_pcb 结构，并将 tcp_pcb_listen 结构中的相应字段拷贝至其中，同时在 tcp_active_pcbs 链表中添加这个新的 tcp_pcb 结构，这样新的 TCP 控制块就处在tcp_active_pcbs 中了。注意 tcp_listen_pcbs 链表中的这个 tcp_pcb_listen 结构还一直存在，它并不会被删除，以等待其他客户端的连接。


到这里，函数 tcp_listen_input 还没完，它应该从收到的 SYN 报文中提取 TCP 选项字段，并设置对应控制块的属性。这里被调用的函数叫 tcp_parseopt，它目前仅能够处理最大报文段大小选项和时间戳选项（可选）。

最后，函数需要向源端返回一个带 SYN 和 ACK 标志的握手报文，同时可以向客户端通告自己的 MSS 大小。

    //函数功能：LISTEN 状态的控制块处理报文段
    //函数说明：所有报文相关的信息已经被保存在前面所述的全局变量中
    static err_t tcp_listen_input(struct tcp_pcb_listen *pcb)
    {
    　　struct tcp_pcb *npcb;
    　　err_t rc;
    　　if (flags & TCP_ACK) {
    　　　　//侦听到任何包含 ACK 的报文段，直接返回 RST 报文，SYN 握手包
    　　　　//不应该包含 ACK
    　　　　tcp_rst(ackno + 1, seqno + tcplen,&(iphdr­>dest), &(iphdr­>src),tcphdr­>dest, tcphdr­>src);
    　　} else if (flags & TCP_SYN) { //收到 SYN 握手报文
    　　　　npcb = tcp_alloc(pcb­>prio); //为新连接创建一个控制块
    　　　　if (npcb == NULL) { //若创建失败，直接返回内存错误，不进行任何回复，
    　　　　　　return ERR_MEM; //对方超时后会重发 SYN，那时可能就有内存空间了
    　　　　}
    　　　　//设置新控制块中与连接相关的四个字段
    　　　　ip_addr_set(&(npcb­>local_ip), &(iphdr­>dest));
    　　　　npcb­>local_port = pcb­>local_port;
    　　　　ip_addr_set(&(npcb­>remote_ip), &(iphdr­>src));
    　　　　npcb­>remote_port = tcphdr­>src;
    　　　　//设置控制块中其余字段
    　　　　npcb­>state = SYN_RCVD; //设置连接状态
    　　　　npcb­>rcv_nxt = seqno + 1; //设置下一个接收数据序号
    　　　　npcb­>rcv_ann_right_edge = npcb­>rcv_nxt;
    　　　　npcb­>snd_wnd = tcphdr­>wnd; //设置发送窗口
    　　　　npcb­>ssthresh = npcb­>snd_wnd;
    　　　　//snd_wl1 设置为 seqno­1，在收到对方 ACK（含窗口通告）后会迫使发送窗口更新
    　　　　npcb­>snd_wl1 = seqno ­ 1;
    　　　　npcb­>callback_arg = pcb­>callback_arg;
    　　　　npcb­>accept = pcb­>accept;
    　　　　npcb­>so_options = pcb­>so_options &
    　　　　(SOF_DEBUG|SOF_DONTROUTE|SOF_KEEPALIVE|SOF_OOBINLINE|SOF_LINGER);
    　　　　TCP_REG(&tcp_active_pcbs, npcb); //将新控制块加入 tcp_active_pcbs 链表
    　　　　//处理 SYN 包中的选项字段，并设置控制块 flags 字段中与相应选项对应的位，
    　　　　//目前只支持最大报文段选项和时间戳选项
    　　　　tcp_parseopt(npcb);
    　　　　npcb­>mss = tcp_eff_send_mss(npcb­>mss, &(npcb­>remote_ip)); //设置最大报文段长度
    　　　　//构造 SYN+ACK 握手报文，且包含 MSS 选项，通告自己的最大报文段大小
    　　　　rc = tcp_enqueue(npcb, NULL, 0, TCP_SYN | TCP_ACK, 0, TF_SEG_OPTS_MSS);
    　　　　if (rc != ERR_OK) { //如果发生错误则释放新控制块
    　　　　　　tcp_abandon(npcb, 0);
    　　　　　　return rc;
    　　　　}
    　　　　return tcp_output(npcb); //发送报文段
    　　}
    　　return ERR_OK;
    }

#### 7.5 SYN攻击

SYN 洪水攻击是目前被广泛使用的一种基于 TCP 的 DDos 攻击技术，通常受攻击的机器是网络中提供固定功能的 TCP 服务器，由于它们的端口号和 IP 地址都很容易得到，所以它们很容易成为黑客攻击的对象。这种攻击过程可以用上节中函数 tcp_listen_input 的原理来解释。当服务器接收到一个连接请求后，它无法判断客户端的合法性（协议本身的缺陷），另一方面，服务器需要为新连接申请一个控制块内存空间，然后向对方返回 ACK+SYN，并等待对方的握手 ACK 返回。通常，如果这个连接请求是恶意者发起的，那么服务器永远等不到这个 ACK 返回（SYN 握手报文中的源IP 地址是伪造的）。服务器必须将这个连接维持足够长的时间，直到相当长的时间后，服务器才能清除它认为“无效”的连接。

假如网络黑客控制了大量的计算机，并同时向服务器发送 SYN 请求，则此时服务器将占用大量的内存空间和时间在等待对方的 ACK 返回上，而显然这种等待都是徒劳的。如果这样的连接达到了很大的数目，系统没有更多的资源来响应新连接，那么正常用户的 TCP 连接也就无法建立，换而言之，服务器无法提供正常的访问服务。TCP 协议连接建立握手过程存在的缺陷，注定了网 络中的 TCP 服务器很容易受到 SYN 攻击。

#### 7.6 TCP状态机函数

其他所有状态控制块的报文处理是由函数 tcp_process 来完成的，该函数主要实现了图 13­6 中的 TCP 状态转换功能，同时它还调用函数接收报文段中的数据。

在讲解 tcp_process 函数之前，先来回顾下这个函数要使用到的一些重要全局变量，这些变量是 tcp_input 函数根据接收报文段中的各个字段值进行设置的。全局变量 tcphdr 指向收到的报文段的 TCP 首部；全局变量 seqno 记录了 TCP 首部中的数据序号；全局量 ackno 记录确认序号；全局变量 flags 表示 TCP 首部中的各个标志字段；全局变量 tcplen 表示报文的数据长度，对于 SYN或 FIN 报文，该长度需加 1；全局变量 inseg 用于组织收到的报文段 pbuf；全局变量 recv_flags 用来记录 tcp_process 函数对报文的处理结果，初始化为 0。

tcp_process 函数首先判断该报文是不是一个复位报文，若是则进行相应的处理，如下代码所示。再接下来就是 TCP 状态机相关的内容，它事实上就是对 TCP 状态转换图的简单代码诠释。

    //函数功能：实现 TCP 状态机的函数
    //函数说明：所有报文相关的信息已经被保存在如前所述的全局变量中
    static err_t tcp_process(struct tcp_pcb *pcb)
    {
        struct tcp_seg *rseg;
        u8_t acceptable = 0;
        err_t err;
        err = ERR_OK;
        //如果收到的报文段 RST 标志置位，则判断该报文段是否合法，若合法，
        //则复位当前的连接
        if (flags & TCP_RST) {
            if (pcb->state == SYN_SENT) { //第一种情况，连接处于 SYN_SENT 状态
                if (ackno == pcb->snd_nxt) { //且报文中确认号与 snd_nxt 相等
                    acceptable = 1;
                }
            } else { //第二种情况，其他状态下报文中的序列号在接收窗口内
                if (TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt, pcb->rcv_nxt+pcb->rcv_wnd)) {
                    acceptable = 1;
                }
            }
            //如果复位报文合法，则需要复位当前连接的控制块
            if (acceptable) {
                recv_flags |= TF_RESET; //设置 recv_flags 为复位，tcp_input 将删除控制块
                pcb->flags &= ~TF_ACK_DELAY;
                return ERR_RST; //返回
            } else { //复位报文不合法，直接返回，不对报文做处理
                return ERR_OK;
            }
        }//if(flags& TCP_RST)
        //处理握手报文 SYN，连接已经建立，但还收到对方的握手包，说明这
        //可能是一个超时重发的握手包，直接向对方返回 ACK 即可
        if ((flags & TCP_SYN) && (pcb->state != SYN_SENT && pcb->state != SYN_RCVD)) {
            tcp_ack_now(pcb); //返回 ACK
            return ERR_OK;
        }
        pcb->tmr = tcp_ticks; //复位控制块的活动计数器
        pcb->keep_cnt_sent = 0; //保活报文计数器清 0，后续讲解保活机制
        tcp_parseopt(pcb); //处理报文首部中的选项字段，设置控制块中的 flags 标志
        //下面是实现 TCP 状态机的具体部分
        switch (pcb->state) {
            case SYN_SENT://客户端发送 SYN 后，处于该状态等待服务器返回 AYN+ACK
                if ((flags & TCP_ACK) && (flags & TCP_SYN) //是 AYN+ACK，且序列号正确
                && ackno == ntohl(pcb->unacked->tcphdr->seqno) + 1) {
                    pcb->snd_buf++; //SYN 被确认，该报文占用一个字节，可用空间加上 1 字节
                    pcb->rcv_nxt = seqno + 1; //设置接收序号
                    pcb->rcv_ann_right_edge = pcb->rcv_nxt; //初始化通告窗口的右边界值
                    pcb->lastack = ackno; //被确认的最高序号
                    pcb->snd_wnd = tcphdr->wnd; //发送窗口设置为接收窗口大小
                    //snd_wl1 设置为 seqno-1，在收到对方
                    pcb->snd_wl1 = seqno - 1; //有效窗口通告后会迫使发送窗口更新
                    pcb->state = ESTABLISHED; //进入 ESTABLISHED 状态
                    pcb->mss = tcp_eff_send_mss(pcb->mss, &(pcb->remote_ip)); //设置最大报文段
                    pcb->ssthresh = pcb->mss * 10; //重新设置了 mss 值，所以要重设 ssthresh 值
                    pcb->cwnd = ((pcb->cwnd == 1) ; (pcb->mss * 2) : pcb->mss); //初始化阻塞窗口
                    --pcb->snd_queuelen; // SYN 报文被确认，占用 pbuf 个数减 1
                    rseg = pcb->unacked; //在缓冲队列中删除 SYN 报文
                    pcb->unacked = rseg->next;
                    if(pcb->unacked == NULL) //未确认队列为空，则停止重传计时器
                        pcb->rtime = -1;
                    else { //队列上有报文，则复位重传计时器、重传次数
                        pcb->rtime = 0;
                        pcb->nrtx = 0;
                    }
                    tcp_seg_free(rseg); //释放取下的 SYN 报文段空间
                    TCP_EVENT_CONNECTED(pcb, ERR_OK, err); //回调用户的 connected 函数
                    tcp_ack_now(pcb); //向服务器返回 ACK，三次握手结束
                }
                else if (flags & TCP_ACK) {//只收到对方的 ACK 却没有 SYN，则向对方返回 RST
                    tcp_rst(ackno, seqno + tcplen, &(iphdr->dest), &(iphdr->src), //不支持半打开状态
                    tcphdr->dest, tcphdr->src);
                }
                break;
            case SYN_RCVD: //服务器在发送完 SYN+ACK 后，将处于该状态
                if (flags & TCP_ACK) { //收到 ACK 报文，三次握手的最后一个报文
                    if (TCP_SEQ_BETWEEN(ackno, pcb->lastack+1, pcb->snd_nxt)) { //确认号合法
                        u16_t old_cwnd;
                        pcb->state = ESTABLISHED; //进入 ESTABLISHED 状态
                        TCP_EVENT_ACCEPT(pcb, ERR_OK, err); //回调用户的 accept 函数
                        if (err != ERR_OK) { //如果 accept 回调函数执行错误，则关闭当前连接
                            tcp_abort(pcb);
                            return ERR_ABRT; //返回错误
                        }
                        old_cwnd = pcb->cwnd; //保存旧的阻塞窗口
                        tcp_receive(pcb); //调用函数处理报文中的数据
                        if (pcb->acked != 0) { //如果本地有未确认数据被报文中的 ACK 确认
                            pcb->acked--; //调整确认的字节数，因为 SYN 报文占了一个字节
                        }
                        pcb->cwnd = ((old_cwnd == 1) ; (pcb->mss * 2) : pcb->mss);//重新设置阻塞窗口
                        if (recv_flags & TF_GOT_FIN) { //tcp_receive 处理时设置了关闭连接标志
                            tcp_ack_now(pcb); //响应对方的 FIN 握手标志
                            pcb->state = CLOSE_WAIT; //进入 CLOSE_WAIT 状态
                        }
                    }
                    else { //不合法 ACK 序号则返回一个复位报文
                        tcp_rst(ackno, seqno + tcplen, &(iphdr->dest), &(iphdr->src),
                        tcphdr->dest, tcphdr->src);
                    }
                } else if ((flags & TCP_SYN) && (seqno == pcb->rcv_nxt - 1)) { //收到对方重复的
                    tcp_rexmit(pcb); //SYN 握手，说明 SYN+ACK 丢失，这里重传 SYN+ACK
                }
                break;
            case CLOSE_WAIT: //服务器处于半关闭状态，不可能再接收到来自客户端的报文，服
                //务器在此状态下会一直等待上层应用执行关闭命令 tcp_close，
                //并将状态变为 LAST_ACK
            case ESTABLISHED: //连接双方都处于稳定状态
                tcp_receive(pcb); //调用函数处理报文中的数据
                if (recv_flags & TF_GOT_FIN) { //tcp_receive 处理时设置了关闭连接标志
                    tcp_ack_now(pcb); //响应对方的 FIN 握手标志
                    pcb->state = CLOSE_WAIT; //进入 CLOSE_WAIT 状态
                }
                break;
            case FIN_WAIT_1: //上层应用调用 tcp_close 关闭连接，发送 FIN 后处于该状态
                tcp_receive(pcb); //调用函数处理报文中的数据
                if (recv_flags & TF_GOT_FIN) { //收到对方的 FIN 握手
                    if ((flags & TCP_ACK) && (ackno == pcb->snd_nxt)) { //且包含有效 ACK
                        tcp_ack_now(pcb); //发送 ACK
                        tcp_pcb_purge(pcb); //清除该连接中的所有现存数据
                        TCP_RMV(&tcp_active_pcbs, pcb); //从 tcp_active_pcbs 链表中删除
                        pcb->state = TIME_WAIT; //置为 TIME_WAIT 状态
                        TCP_REG(&tcp_tw_pcbs, pcb); //加入 tcp_tw_pcbs 链表
                    } else { //双方同时执行关闭操作
                        tcp_ack_now(pcb); //返回 ACK
                        pcb->state = CLOSING; //进入 CLOSING 状态
                    }
                } else if ((flags & TCP_ACK) && (ackno == pcb->snd_nxt)) { //只收到有效 ACK
                    pcb->state = FIN_WAIT_2; //进入 FIN_WAIT_2 状态
                }
                break;
            case FIN_WAIT_2: //主动关闭，发送 FIN 握手且接收到 ACK 后处于该状态
                tcp_receive(pcb); //调用函数处理报文中的数据
                if (recv_flags & TF_GOT_FIN) { //收到对方的 FIN 握手
                    tcp_ack_now(pcb); //发送 ACK
                    tcp_pcb_purge(pcb); //清除该连接中的所有现存数据
                    TCP_RMV(&tcp_active_pcbs, pcb); //从 tcp_active_pcbs 链表中删除
                    pcb->state = TIME_WAIT; //置为 TIME_WAIT 状态
                    TCP_REG(&tcp_tw_pcbs, pcb); //加入 tcp_tw_pcbs 链表
                }
                break;
            case CLOSING: //双方同时执行主动关闭，处于该状态
                tcp_receive(pcb); //调用函数处理报文中的数据
                if (flags & TCP_ACK && ackno == pcb->snd_nxt) { //收到有效 ACK
                    tcp_pcb_purge(pcb); //清除该连接中的所有现存数据
                    TCP_RMV(&tcp_active_pcbs, pcb); //从 tcp_active_pcbs 链表中删除
                    pcb->state = TIME_WAIT; //置为 TIME_WAIT 状态
                    TCP_REG(&tcp_tw_pcbs, pcb); //加入 tcp_tw_pcbs 链表
                }
                break;
            case LAST_ACK: //服务器在执行被动关闭时，发送完 FIN，等待 ACK 返回
                tcp_receive(pcb); //调用函数处理报文中的数据
                if (flags & TCP_ACK && ackno == pcb->snd_nxt) { //收到有效 ACK
                recv_flags |= TF_CLOSED; //设置 recv_flags 为 TF_CLOSED，由 tcp_input 函数
                } //对该控制块进行释放和清除工作
                break;
            default:
                break;
        }
        return ERR_OK;
    }

这就是 TCP 状态机的所有转换代码，如果对照着 TCP 状态转换图来看，它原来如此的简单！注意 TCP 状态转换图中存在双方同时打开的情况，即从 LISTEN 状态到 SYN_SENT 状态，以及SYN_SENT 状态到 SYN_RCVD 状态，在 LwIP 中都没有实现。对于许多其他 TCP/IP 的实现而言，包括 BSD，都没有实现这样的功能，因为在实际应用 TCP 的编程模型都是客户机—服务器模型，这种情况不可能出现。 

#### 7.7 TCP数据输入

# 8 可靠的传输服务

#### 8.1 超时重传与RTT估计

#### 8.2 慢启动与拥塞避免

#### 8.3 快速重传与快速恢复

#### 8.4 糊涂窗口与避免

#### 8.5 零窗口探查

#### 8.6 保活机制

#### 8.7 TCP定时器

# 9 测试程序
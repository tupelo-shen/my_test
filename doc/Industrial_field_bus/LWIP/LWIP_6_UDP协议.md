IP协议提供了在各台主机之间传送数据报的功能，但是各个主机并不是数据报中数据的最终目的地，数据的最终目的地应该是主机上的某个特定应用程序。那么IP层怎么样将数据报递交给各个应用程序呢？这就是传输层协议的功能了，TCP/IP协议分层中，典型的传输层协议有 UDP和TCP两种。UDP 为两个应用程序提供了简单的数据交互方式，有着很高的数据递交效率，在局域网环境或在视频播放领域有着广泛的应用。另一方面，UDP 也是实现多种著名上层应用协议的基础，例如 DNS、DHCP、IGMP、SNMP 等协议都使用UDP传送协议数据。Anyway，本章中将涉及的知识点如下：

* UDP协议与端口号；
* UDP报文格式、校验和、报文交付方式；
* LwIP中的UDP报文数据结构和UDP控制块数据结构；
* UDP控制块操作函数；
* UDP报文的发送处理、接收处理；
* 基于UDP的回显测试程序。

# 1 背景知识

#### 1.1 传输层协议

UDP 和TCP 都属于传输层协议，前面讲解的IP协议只能完成数据报在互联网中各主机之间的递交，IP协议中，数据报的目的地是某一台主机，而不是主机上的某个应用程序。

要实现进程到进程间的通信，传输层协议需要完成几个重要任务：


* 第一，为两个通信的进程提供连接机制，即传输层将怎样去识别两个正在通信的进程，当主机的传输层从IP层得到一个数据报时，它将使用何种方式把数据递交给最终的应用程序？在传输层中，这是通过端口号来完成的；

* 第二，传输层应该提供数据传送服务，在数据发送端，传输层将用户数据进行组装、编号，将数据分割成可运输的单元，然后依次递交给IP层发送出去。接收端传输层等待属于同一应用程序的所有数据单元到达，对它们进行差错校验，最后将整个数据交付给应用程序；

* 第三，为了提供更可靠的传输服务，传输层还应该提供流量控制机制，例如数据的确认、重传等，以保证数据在两个应用程序之间递交的有效性。

#### 1.2 UDP协议

UDP 称为用户数据报协议，是一种无连接的、不可靠的传输协议，它只在低级程度上实现了上述的传输层功能。UDP 只是简单地完成数据从一个进程到另一个进程的交付，它没有提供任何流量控制机制，收到的报文也没有确认；在差错控制上，UDP 只提供了一种简单的差错控制方法，即校验和计算，当UDP收到的报文校验和计算不成功时，它将丢弃掉这个报文。

UDP 协议的可靠性如此差，那为何还要使用它呢？

首先，这里的不可靠定义还是要根据具体使用环境来的，在现在的高可靠性、低时延的局域网环境下，使用UDP协议出现传输错误的可能性很小，但使用UDP却可以带来数据递交效率和处理速度的提升，因为它省去了连接建立、数据确认、流量控制等一系列过程。

从代码的实现角度讲，UDP 协议的代码量非常小，对于小型嵌入式设备来说，在局域网中使用UDP来实现通信还是很合适的。

除此之外，UDP 也经常在那些对轻微数据差错不敏感的应用中被使用到，例如实时视频传输、网络电话等。

#### 1.3 端口

每台主机都包含了一组称为协议端口的抽象目的点，每个协议端口用一个正整数来标志，在TCP/IP协议簇中，端口号范围为 0～65535，进程可以绑定到某一个端口号上，UDP 报文需要在其内部指出该报文应该递交的目的端口号，这样，绑定到相关端口号的进程将最终得到数据报文。两个进程要进行互相间的通信，它们都必须知道对方的IP地址和绑定的端口号。

UDP 协议的端口分配方法可以分为两大类，第一种是一些中央授权机构已经明确规定功能的端口号，称之为熟知端口号（well­known port assignment），这些端口号与实现某些上层协议的功能密切相关；第二种端口分配方法称为动态绑定（dynamic binding），这种类型的端口号称为短暂端口号。

服务器进程必须绑定到一个熟知的端口号上，这个端口号是通信双方事先都知道的，客户端进程可以直接往该端口号上发送数据，这样数据就能正确到达目的主机上的服务器程序了。UDP 中常见的熟知端口号及其作用如表 12­1 所示，提供相关功能的服务器进程必须绑定到对应的端口号上 。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_6_1.png">

另一方面，客户端程序也必须使用一个端口号来标识自己，这个端口号可以在允许范围内随机的选取（短暂端口号），但最好不要使用熟知端口号，客户端可以在报文中携带这个端口号，服务器进程通过这个端口号就能向客户端进程返回数据 。

#### 1.4 UDP报文的交付

用户进程使用UDP来传送数据时，UDP 协议会在数据前加上首部组成UDP报文，并交给 IP协议来发送，而IP层将报文封装在IP数据报中并交给底层发送，在底层，IP数据报会被封装在物理数据帧中。因此，一份用户数据在被发送时，经历了三次封装过程，如图6-2所示。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_6_2.png">

在接收端，物理网络先接收到数据帧，然后逐层地将数据递交给上层协议，每一层都在向上层递交前去除掉一个首部。在UDP层，它将从IP层得到UDP报文，UDP 协议会根据该报文首部的目的端口字段将报文递交给用户进程，绑定到这个目的端口的进程将得到报文中的数据。

#### 1.5 UDP报文格式

如图 12­2 展示了一份UDP报文的具体结构，UDP 首部很简单，它由四 个 16 位字段组成，分别指出了该用户数据报从哪个端口来、要到哪个端口去、总长度和校验和。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_6_3.png">

在用户数据报的发起端（通常作为客户机），通常会将目的端口号填写为服务器上某个熟知的端口，对源端口号字段的填写则是可选的，如果客户端期望服务器为自己返回数据，则必须填写源端口号字段，服务器会在收到的报文中提取到这个源端口号，并在返回数据时使用到。

16 位的总长度字段定义了用户数据报的总长度，包括首部长度和数据区长度，以字节为单位。 UDP数据区的数据最多只能有 65507 字节（65535-­8-­20），因为我们在讲解IP数据报首部时，IP首部中的总长度字段也为 16 位，UDP要使用IP层来传送数据报，所以其数据长度也必须满足IP首部中的长度要求。

如果不使用校验和，可以直接将该字段填入 0，之所以可以不使用校验和，是因为在某些特殊场合，例如在高可靠性的局域网中使用UDP时，减少校验和的计算能增加UDP的处理速度。在以太网的底层物理帧接收过程中，通常会对整个数据帧进行 CRC 校验，因此，数据报出错的可能性已被降到最低。

# 2 UDP数据结构

源文件`udp.h`和`udp.c`中包含了与UDP协议实现相关的所有数据结构和函数，这节来看看UDP有哪些重要数据结构。

#### 2.1 报文首部结构

源代码用结构体 udp_hdr 定义了UDP报文首部中的各个字段，首部结构如图6-3所示。

    ————udp.h——————————————————————
    #define UDP_HLEN 8 //定义UDP数据报首部长度
    PACK_STRUCT_BEGIN
    struct udp_hdr {
    　　PACK_STRUCT_FIELD(u16_t src); //源端口号
    　　PACK_STRUCT_FIELD(u16_t dest); //目的端口号
    　　PACK_STRUCT_FIELD(u16_t len); //总长度
    　　PACK_STRUCT_FIELD(u16_t chksum); //校验和
    } PACK_STRUCT_STRUCT;
    PACK_STRUCT_END
    ——————————————————————————————————

#### 2.2 控制块

系统为每一个连接分配一个UDP控制块，并把它们组织在一个全局的链表上，当UDP层收到IP层递交的报文时，会去遍历这个链表，找出与报文首部信息匹配的控制块，并调用控制块中注册的函数最终完成报文的处理。定义UDP控制块时，会用到了IP层中定义的一个宏，先看看这个宏的结构。 

    ————ip.h————————————
    /*
     * 下面定义宏IP_PCB，它是与IP层相关的字段
     */
    #define IP_PCB \
    　　struct ip_addr local_ip; \        // 本地IP地址
    　　struct ip_addr remote_ip; \       // 远端IP地址
    　　u16_t so_options; \               // socket选项
    　　u8_t tos; \                       // 服务类型
    　　u8_t ttl                          // 生存时间（TTL）
    /*
     * 定义IP控制块
     */
    struct ip_pcb {
    　　　IP_PCB;                          // 宏IP_PCB相关的字段
    };
    ————————————————————————————

在ip.h文件中定义了宏IP_PCB，这个宏在本章的UDP控制块以及下一章的TCP控制块中都会被用到，它定义了在这些控制块中都会使用到的与IP协议相关的字段，这也是为什么会把这个宏放在ip.h文件中的原因。

需要指出的是，虽然宏IP_PCB中定义了很多字段，但是被使用最多的只有前两个字段，即本地IP地址（源IP地址）和远端IP地址（目的IP地址）。最后，上面的代码还定义了一个结构体ip_pcb，这里称它为IP控制块，它由宏IP_PCB包含的各 个字段组成。

下面来看正题，即UDP控制块，源代码如下：

    ————udp.h————————————————
    //定义两个宏，用于控制块的 flags 字段，标识控制块的状态信息
    #define UDP_FLAGS_NOCHKSUM 0x01U //不进行校验和的计算
    #define UDP_FLAGS_CONNECTED 0x04U //控制块已和远端建立连接
    //定义UDP控制块结构体
    struct udp_pcb {
    　　IP_PCB; //宏IP_PCB中的各个字段
    　　struct udp_pcb *next; //用于将控制块组织成链表的指针
    　　u8_t flags; //控制块状态字段
    　　u16_t local_port, remote_port; //保存本地端口号和远端端口号，使用主机字节序
    　　void (* recv)(void *arg, struct udp_pcb *pcb, struct pbuf *p, //处理数据时的回调函数
    　　　　　　struct ip_addr *addr, u16_t port);
    　　void *recv_arg; //当调用回调函数时，将传递给函数的用户定义数据信息
    };
    ——————————————————————————————————

UDP 控制块包含了宏IP_PCB中定义的各个字段，会被多次使用到的是其中的本地IP地址和远端IP地址。

链表头指针为 udp_pcbs，next 字段就是用来构成链表的。UDP协议实现的本质是对链表udp_pcbs 上各个UDP控制块的操作。

第一个标志该控制块是否进行校验和的计算，当 flags 的无校验位（位 0）为 1 时，表示在发送报文时不计算首部中的校验和字段，这样的话在报文发送时，校验和字段直接置 0 就可以了。第二个标志该控制块是否处于连接状态（位 2），当某个控制块处于连接状态时，表示它内部已经完整地记录了关于通信双方的IP地址和端口号信息。

当UDP接收到一个报文时，会遍历链表 udp_pcbs 上的所有控制块，检查其中的本地端口号与报文首部中的目的端口号是否匹配，并将报文递交给匹配成功的控制块处理。

用户程序在初始化一个控制块时，需要在该字段注册自定义的报文处理函数，在内核接收到报文并匹配到某个控制块后，通过函数指针 recv 来回调用户自定义的处理函数，这样就最终完成了报文向用户程序的递交。

关于函数指针 recv，来看看它的具体定义：

    void (* recv)(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

1. 参数 arg 表示将传递给函数的用户自定义数据；
2. 参数 pcb 指向接收到报文的 UDP控制块结构；
3. 参数 p 指向接收到的报文 pbuf；
4. 参数 addr 表示发送该报文的源主机IP地址；
5. 参数 port 表示发送该报文的源主机上的端口号，用户应用程序应该合理地使用这些参数传进来的值完成对报文中数据的处理。 
6. 最后一个字段 recv_arg 的作用就很明显了，它可以用来指向一个用户自定义的数据信息，在回调 recv 指向的函数时，recv_arg 会作为函数的第一参数。 　　

最后需要重点说明的是，控制块中的最后两个字段具有很重要的作用，它们是用户程序与协议栈内核进行通信的纽带，在后面讲解其他两种类型的 API 函数可以看到，API 函数的实现机制都需基于这两个字段 

从图中可以看出，由于前两个控制块中记录了连接双方的地址和端口号信息，所以它们处于连接状态；而最后一个控制块只记录了本地IP地址和端口号，它处于非连接状态。若此时UDP接收到一个目的端口号为 4321 的数据报，则内核函数会从链表起始处 udp_pcbs 开始查找整个链表，以找到具有本地端口为 4321 的控制块，这里为第二个控制块。当找到该控制块后，控制块的 recv 字段指向的函数 proc2 会被调用以处理报文数据，传递进 proc2 的参数包含了足够的信息，用户程序编写的关键就在于如何处理这些信息。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_6_4.png">

上述这种应用程序编写方法就叫做 raw/callback API 方法，在这种方式下的应用程序与协议栈内核处于用一个进程中，用户程序通过回调的方式被协议栈调用，以取得协议栈中的数据，基于回调机制的应用程序会使得整个代码的灵活性加大。另一方面，使用这种方式编程需要直接与内核交互，所以编程难度较大，对程序设计人员的要求较高。在后面读者会看到，基于回调机制的程序编写方式也存在着一些缺陷，使其不适合在大型应用程序的开发中使用。

# 3 控制块操作函数

#### 3.1 使用UDP编程

根据接收到的报文查找 UDP控制块，然后调用注册的用户函数处理报文数据，如果用户注册的函数为空，则相应的报文会被直接删除，这种情况下，没有任何错误会被报告给源主机；当查找不到对应的控制块时，UDP 会向源主机返回一个 ICMP 端口不可达差错报告报文。

#### 3.2 新建控制块

任何想使用UDP服务的应用程序都必须拥有一个控制块，并把控制块绑定到相应的端口号上，在接收报文时，端口号将作为报文终点选择的唯一依据。 在内存池中为UDP控制块申请一个 MEMP_UDP_PCB 类型的内存池空间，并初始化相关字段。

    ————udp.c——————————————
    //新建UDP控制块
    struct udp_pcb *udp_new(void)
    {
    　　struct udp_pcb *pcb;
    　　pcb = memp_malloc(MEMP_UDP_PCB); //为控制块申请一个内存池空间
    　　if (pcb != NULL) { //申请成功后，初始化各个字段
    　　　　memset(pcb, 0, sizeof(struct udp_pcb)); //将所有字段全部清 0
    　　　　pcb­>ttl = UDP_TTL; //设置控制块中的 TTL 字段
    　　}
    　　return pcb;
    }
    ——————————————————————

#### 3.3 绑定控制块

当作为服务器程序时，必须手动为控制块绑定一个熟知端口号，当作为客户端程序时，手绑定端口号并不是必须的，此时，在与服务器通信前，UDP 会自动为控制块绑定一个短暂端口号。端口号绑定的本质就是设置控制块中的local_port 和 local_ip 字段，它还涉及对链表 udp_pcbs 的操作。

    ————udp.c——————————————————
    //函数功能：为UDP控制块绑定一个本地IP地址和端口号
    //参数 pcb：指向要操作的控制块指针
    //参数 ipaddr：本地IP地址，若为 IP_ADDR_ANY（0），表示任意网络接口结构的IP地址
    //参数 port：本地端口号，若为 0，则函数将自动为控制块分配一个有效的短暂端口号
    err_t udp_bind(struct udp_pcb *pcb, struct ip_addr *ipaddr, u16_t port)
    {
    　　struct udp_pcb *ipcb;
    　　u8_t rebind; //全局变量，表示控制块是否已在链表 udp_pcbs 中置 0
    　　rebind = 0;
    　　//遍历整个链表，查找控制块 pcb，若控制块已在链表中，则后续不再进行链表插入操作
    　　for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb­>next) {
    　　　　if (pcb == ipcb) { //找到控制块
    　　　　　　rebind = 1; //设置全局变量有效
    　　　　}
    　　}
    　　ip_addr_set(&pcb­>local_ip, ipaddr); //设置控制块的本地IP地址字段
    　　//如果 port 为 0，则要自动为控制块寻找一个有效短暂端口
    　　if (port == 0) { //自动寻找一个短暂端口号
    　　　　#ifndef UDP_LOCAL_PORT_RANGE_START
    　　　　#define UDP_LOCAL_PORT_RANGE_START 4096 //定义起始短暂端口号
    　　　　#define UDP_LOCAL_PORT_RANGE_END 0x7fff //定义结束短暂端口号
    　　　　#endif
    　　　　//下面从第一个短暂端口开始，依次判断该端口号是否已被其他控制块占用，若未被
    　　　　//占用，这就是我们要寻找的有效端口号
    　　　　port = UDP_LOCAL_PORT_RANGE_START;
    　　　　ipcb = udp_pcbs; //从链表第一个控制块开始
    　　　　while ((ipcb != NULL) && (port != UDP_LOCAL_PORT_RANGE_END)) {
    　　　　　　if (ipcb­>local_port == port) { //如果端口号被当前控制块占用
    　　　　　　　　port++; //检查下一个端口号
    　　　　　　　　ipcb = udp_pcbs; //ipcb 复位，指向链表首部
    　　　　　　} else
    　　　　　　　　ipcb = ipcb­>next; //端口未被当前控制块占用，检查下一个控制块
    　　　　}//while
    　　　　if (ipcb != NULL) { //查找结束后，若 ipcb 不为空，则说明未找到有效可用端口
    　　　　　　return ERR_USE; //返回端口被占用错误
    　　　　　}
    　　}
    　　pcb­>local_port = port; //到这里，有了有效的端口号，直接设置 local_port 字段
    　　if (rebind == 0) 　　{ //如果控制块没有在链表中，则将它加入链表的首部
    　　　　pcb­>next = udp_pcbs;
    　　　　udp_pcbs = pcb;
    　　}
    　　return ERR_OK; //返回处理结果
    }
    ——————————————————————————————

该函数本质是设置控制块的 local_port 和 local_ip 字段，并把控制块加入到链表 udp_pcbs 中，这里涉及一个重绑定的问题，即如果控制块已经在链表汇中，说明已经对其进行过绑定工作，这里就只是重新修改 local_port 和 local_ip 字段，并不需要再进行插入链表操作。

#### 3.4 连接控制块

与绑定控制块函数相对应，连接控制块函数完成控制块中 remote_ip 和 remote_port 的设置。只有绑定了本地IP地址和端口号，以及远端IP地址和端口号的控制块才会处于连接状态。

    ————udp.c——————————————————
    //函数功能：为UDP控制块绑定一个远端IP地址和端口号
    //参数 pcb：指向要操作的控制块
    //参数 ipaddr：远端IP地址
    //参数 port：远端端口号
    err_t udp_connect(struct udp_pcb *pcb, struct ip_addr *ipaddr, u16_t port)
    {
    　　struct udp_pcb *ipcb;
    　　if (pcb­>local_port == 0) { //如果本地端口号未绑定，调用函数绑定本地端口
    　　　　err_t err = udp_bind(pcb, &pcb­>local_ip, pcb­>local_port);
    　　　　if (err != ERR_OK) //如果绑定失败，则返回错误
    　　　　　　return err;
    　　　　}
    　　　　ip_addr_set(&pcb­>remote_ip, ipaddr); //设置 remote_ip 字段
    　　　　pcb­>remote_port = port; //设置 remote_port 字段
    　　　　pcb­>flags |= UDP_FLAGS_CONNECTED; //控制块状态设置为连接状态
    　　　　for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb­>next) {//遍历链表，查找是否控制块
    　　　　　　if (pcb == ipcb) { //已经处在链表中
    　　　　　　　　return ERR_OK; //若是，则返回成功
    　　　　　　}
    　　　　}
    　　//若控制块没有在链表中，则将控制块插入到链表首部
    　　pcb­>next = udp_pcbs;
    　　udp_pcbs = pcb;
    　　return ERR_OK;
    }
    ——————————————————————————————————

#### 3.5 其他控制块操作函数

还有几个控制块操作函数，在应用程序的编写过程中也经常用到，它们的代码和功能都很简单，如下所示：

    ————udp.c——————————————————
    //函数功能：清除 remote_ip 和 remote_port 字段，将控制块置为非连接状态
    void udp_disconnect(struct udp_pcb *pcb)
    {
    　　ip_addr_set(&pcb­>remote_ip, IP_ADDR_ANY); //清空 remote_ip
    　　pcb­>remote_port = 0; //清空 remote_port 字段
    　　pcb­>flags &= ~UDP_FLAGS_CONNECTED; //置为非连接状态
    }
    //函数功能：为控制块注册回调函数
    //参数 pcb：指向要操作的控制块
    //参数 recv：用户自定义的数据报处理函数
    //参数 recv_arg：用户自定义数据
    void udp_recv(struct udp_pcb *pcb, void (* recv)(void *arg, struct udp_pcb *upcb, struct pbuf *p,struct ip_addr *addr, u16_t port),void *recv_arg)
    {
    　　pcb­>recv = recv; //填写控制块的 recv 字段
    　　pcb­>recv_arg = recv_arg; //填写控制块的 recv_arg 字段
    }
    //函数功能：将一个控制块结构从链表中删除，并释放其占用的内存空间
    void udp_remove(struct udp_pcb *pcb)
    {
    　　struct udp_pcb *pcb2;
    　　if (udp_pcbs == pcb) { //如果控制块在链表的首部
    　　　　udp_pcbs = udp_pcbs­>next; //从链表上删除
    　　} else //否则，依次查找链表
    　　for (pcb2 = udp_pcbs; pcb2 != NULL; pcb2 = pcb2­>next) {
    　　　　if (pcb2­>next != NULL && pcb2­>next == pcb) { //找到控制块
    　　　　　　pcb2­>next = pcb­>next; //从链表上删除
    　　　　}
    　　}
    　　memp_free(MEMP_UDP_PCB, pcb); //释放内存池空间
    }
    ————————————————————————————————

# 4 报文处理函数

#### 4.1 报文的发送

数据第一次封装（UDP）：

    err_t udp_send(struct udp_pcb *pcb, struct pbuf *p)  // udp_pcb控制块 + pbuf数据块

数据第二次封装（IP）：

    err_t udp_sendto(struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *dst_ip, u16_t dst_port)
    // ip控制块 + udp_pcb控制块 + pbuf数据块

数据第三次封装（netif）：

    err_t udp_sendto_if(struct udp_pcb *pcb, struct pbuf *p,struct ip_addr *dst_ip, u16_t dst_port, struct netif *netif) 
    // netif控制块 + ip控制块 + udp_pcb控制块 + pbuf数据块

#### 4.2 报文的接收

    struct udp_pcb *udp_pcbs; //全局变量，指向UDP控制块链表
    //函数功能：UDP 报文处理函数
    //参数 pbuf：IP层接收到的包含UDP报文的数据报 pbuf，payload 指针指向IP首部
    //参数 inp：接收到IP数据报的网络接口结构
    void udp_input(struct pbuf *p, struct netif *inp)
    {
        struct udp_hdr *udphdr; //UDP 报文首部结构
        struct udp_pcb *pcb, *prev; //UDP 控制块指针，用于查找过程
        struct udp_pcb *uncon_pcb; //指向第一个匹配的处于非连接状态的控制块
        struct ip_hdr *iphdr; //IP数据报首部结构
        u16_t src, dest; //保存报文中的源端口与目的端口
        u8_t local_match; //志控制块是否匹配
        u8_t broadcast; //记录该IP数据报是否为广播数据报
        iphdr = p->payload; //指向IP数据报首部
        //进行长度校验：整个数据报的长度不小于IP首部+UDP 首部的大小
        //移动数据报 payload 指针，使其指向UDP首部
        if (p->tot_len < (IPH_HL(iphdr) * 4 + UDP_HLEN) || pbuf_header(p, -(s16_t)(IPH_HL(iphdr) * 4))) {
            pbuf_free(p); //如果检验不通过或操作不成功，则释放掉 pbuf
            goto end; //跳到 end 处，执行返回操作
        }
        udphdr = (struct udp_hdr *)p->payload; //指向UDP报文首部
        broadcast = ip_addr_isbroadcast(&(iphdr->dest), inp); //判断IP数据报是否为广播
        src = ntohs(udphdr->src); //取得UDP首部中的源端口号
        dest = ntohs(udphdr->dest); //取得UDP首部中的目的端口号
        //下面开始查找匹配的UDP控制块，第一查找目标为与目的端口号和目的IP地址匹配
        //且处于连接状态的控制块，找不到，则查找与目的端口号和目的IP地址匹配的第一个
        //处于非连接状态的控制块
        {
            prev = NULL; //指针清空
            local_match = 0; //当前控制块的匹配状况
            uncon_pcb = NULL; //第一个匹配的非连接状态控制块
            for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next) { //遍历整个控制块链表
                local_match = 0; //当前控制块不匹配
                //先判断控制块中记录的本地端口号、IP地址与数据报中记录的
                //目的端口号、目的IP地址是否匹配
                if ((pcb->local_port == dest) && //若端口号和IP地址都匹配
                    ( (!broadcast && ip_addr_isany(&pcb->local_ip)) ||
                        ip_addr_cmp(&(pcb->local_ip), &(iphdr->dest)) ||
                            (broadcast)) ) {
                    local_match = 1; //当前控制块匹配
                    if ((uncon_pcb == NULL) && //若当前控制块为未连接态且 uncon_pcb 为空
                    ((pcb->flags & UDP_FLAGS_CONNECTED) == 0)) {
                        uncon_pcb = pcb; //记录下第一个匹配的非连接态控制块
                    }
                }
                //前阶段匹配成功，则继续匹配控制块中记录的源端口号、源IP地址与数据报
                //中记录的源端口号、源IP地址是否匹配，以找到一个处于连接状态的控制块
                if ((local_match != 0) && //前阶段匹配成功
                (pcb->remote_port == src) && //且源端口号匹配
                (ip_addr_isany(&pcb->remote_ip) || //且源IP地址匹配
                    ip_addr_cmp(&(pcb->remote_ip), &(iphdr->src)))) {
                    //到这里我们得到一个完全匹配的控制块，若控制块不在链表首部，则需
                    //要把该控制块移到链表的首部，这样能提高下次报文处理时的查找效率
                    if (prev != NULL) { //当前控制块不为链表的首部
                        prev->next = pcb->next;
                        pcb->next = udp_pcbs;
                        udp_pcbs = pcb;
                    }
                    break; //跳出 for 循环，结束查找过程
                }
                prev = pcb; //进行下一个控制块的比较
            }//for
            if (pcb == NULL) { //遍历完链表上的所有控制块，没有找到完全匹配的连接态控制块
                pcb = uncon_pcb; //则将第一个匹配的非连接状态控制块作为匹配结果
            }
        }
        //到这里，如果找到了匹配的控制块，或者找不到控制块，但是数据报确实是给本地的
        //对于前者调用用户注册的回调函数处理数据，对于后者，为源主机返回一个端口不可
        //达差错报文，Anyway，先得进行校验和的验证
        if (pcb != NULL || ip_addr_cmp(&inp->ip_addr, &iphdr->dest)) {
            if (udphdr->chksum != 0) { //数据报中已经填写了校验和字段，则必须验证
                if (inet_chksum_pseudo(p, (struct ip_addr *)&(iphdr->src),
                (struct ip_addr *)&(iphdr->dest),
                IP_PROTO_UDP, p->tot_len) != 0) {
                    pbuf_free(p); //校验和失败，删除数据报，并返回
                    goto end;
                }
            }
            //校验和通过，则将报文中的数据递交给用户程序处理，先调整 payload 指针
            if(pbuf_header(p, -UDP_HLEN)) { //指向报文中的数据区
                pbuf_free(p); //调整失败，则删除整个数据报
                goto end;
            }
            //如果有匹配的控制块，则调用控制块中注册的用户函数处理数据
            if (pcb != NULL) {
                if (pcb->recv != NULL) { //如果注册了用户处理程序
                    //调用用户函数，用户函数要负责报文 pbuf 的释放
                    pcb->recv(pcb->recv_arg, pcb, p, &iphdr->src, src);
                } else { //若没有注册用户处理函数
                    pbuf_free(p); //释放数据报 pbuf 并返回
                    goto end;
                }
            } else { // 没有匹配的控制块，说明找不到匹配的端口号，返回端口不可达报文
                if (!broadcast &&!ip_addr_ismulticast(&iphdr->dest)) { //不是广播和多播包
                    pbuf_header(p, (IPH_HL(iphdr) * 4) + UDP_HLEN); //payload 指向IP首部
                    icmp_dest_unreach(p, ICMP_DUR_PORT); //发送端口不可达报文
                }
                pbuf_free(p); //释放数据包
            }
        } else { //不是给本地的数据报
            pbuf_free(p); //直接删除数据报
        }
        end:
    }

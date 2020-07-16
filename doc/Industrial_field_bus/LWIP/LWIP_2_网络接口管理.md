# 1 引言

LWIP分为四个层次：`链路层`、`网络层`、`传输层`和`应用层`。运行LWIP的嵌入式设备可以有多个网络接口：以太网接口、串行链路接口、环回接口等。为了实现对所有网络接口的有效管理，协议栈内部使用了一个名为`netif`的网络接口结构来描述各种网络设备。本章讨论的内容包括：

1. 网络接口管理的作用；
2. 网络接口结构`netif`；

# 2 数据结构

源文件中 netif.c 和 netif.h 文件实现了与网络接口结构管理相关的所有数据结构和函数。来看看结构 netif是怎样被定义的，如下代码所示。

    ————netif.h—————————————————————
    #define NETIF_MAX_HWADDR_LEN    6U      // 最大MAC长度6
    // 下面几个宏为网络接口属性、状态相关的宏，主要用于描述 netif中 flags 字段的各位
    #define NETIF_FLAG_UP           0x01U   // 网络接口是否已被上层使能
    #define NETIF_FLAG_BROADCAST    0x02U   // 网络接口是否支持广播
    #define NETIF_FLAG_POINTTOPOINT 0x04U   // 网络接口是否属于点到点连接
    #define NETIF_FLAG_DHCP         0x08U   // 网络接口是否支持 DHCP 功能
    #define NETIF_FLAG_LINK_UP      0x10U   // 网络接口的底层链路是否已经使能
    #define NETIF_FLAG_ETHARP       0x20U   // 网络接口是否支持ARP功能
    #define NETIF_FLAG_IGMP         0x40U   // 网络接口是否支持 IGMP 功能
    //下面是结构 netif的定义
    struct netif{
    　　struct netif     *next;          // 指向下一个 netif结构，在构成链表 netif_list时使用
    　　struct ip_addr    ip_addr;        // 网络接口的IP地址
    　　struct ip_addr    netmask;        // 子网掩码
    　　struct ip_addr    gw;             // 网关地址
    　　//下面为三个函数指针，调用它们指向的函数就可以完成数据包的发送或接收
    　　err_t (* input)(structpbuf *p, struct netif*inp); 
                                          // 该函数向IP层输入数据包
    　　err_t (*output)(struct netif*netif, structpbuf *p,struct ip_addr *ipaddr); 
                                          // 该函数发送IP包
    　　err_t (* linkoutput)(struct netif*netif, structpbuf *p); 
                                          // 该函数实现底层数据包发送
    　　void *state;                      // 该字段用户可以自由设置，例如用于指向一些底层设备相关的信息
    　　u16_t mtu;                        // 该接口允许的最大数据包长度
    　　u8_t hwaddr_len;                  // 该接口物理地址长度
    　　u8_t hwaddr[NETIF_MAX_HWADDR_LEN]; // 该接口的物理地址
    　　u8_t flags;                       // 该接口的状态、属性字段
    　　char name[2];                     // 该接口的名字
    　　u8_t num;                         // 接口的编号
    　　
        // 在接口自输入使能或者有环回接口的情况下，下面的字段
    　　// 用于描述接口发送给自己的数据包
    　　structpbuf *loop_first;          // 指向发送给自己的数据包的第一个pbuf
    　　structpbuf *loop_last;           // 指向发送给自己的数据包的最后一个pbuf
    };
    ————————————————————————————————

* next

    是指向下一个 netif结构的指针，前面也说过，一个设备可能会有多个网络接口（例如路由器），LwIP会把所有网络接口的 netif结构组成一个链表进行管理，有一个名为netif_list的全局变量指向该链表的首部。next 字段就是用于组成链表时用。

* ip_addr/netmask/gw

    三个字段用于描述该网络接口的网络地址属性，依次称它们为`网络接口IP地址`、`子网掩码`和`网关地址`。IP地址和网络接口必须一一对应，即设备有几个硬件网络接口，它就得有几个IP地址；子网掩码可以用来判断某个目的IP地址与当前网络接口IP地址是否处于同一子网中，IP层会选择与目的IP处于同一子网的网络接口来发送数据包。gw字段在数据包的发送、转发过程中也有重要作用，如果目的IP地址与所有网络接口都不属于同一子网，LwIP将会把数据包发送到网关处，因为它认为网关设备会对该IP包进行正确的转发，此外网关也为设备提供了许多高级服务，如 DHCP、DNS等。 　　

* input

    指向一个函数，这个函数将网络设备接收到的数据包提交给IP层（在以太网中，通常这个函数需要解析以太网数据帧，然后将从中得到的IP数据包递交）。被指向的函数具有两个参数：一个是pbuf类型，代表将要递交的数据包；另一个为netif类型，代表递交数据包的网络设备，函数返回值是err_t类型。网络设备初始化时应该向input字段注册相应的输入函数，后面将详细讨论这个问题。 　　　　

*output

    指向一个函数，这个函数和具体网络接口设备驱动密切相关，它用于将IP层数据包发送到目的地址处。在IP层看来，每个网络接口都会提供一个这样的函数供它调用，当IP层发送数据包时，它会遍历netif_list链表，找出最合适的网络接口，并调用其注册的output函数发送 数据包。不同的网络接口需要根据实际接口特性来编写相关的发送函数，并在初始化时将output字段指向该函数。就以太网网卡的数据包发送而言，LwIP在源代码 etharp.c 文件中实现了一个名为etharp_output的函数，该函数就可以直接完成IP数据包在以太网中的发送，所以在初始化以太网卡结构时，可以直接将output字段指向函数 etharp_output。output指向函数的三个参数分别是pbuf类型、netif类型和ip_addr类型，返回值是err_t类型。其中pbuf代表要发送的IP数据包，ipaddr代表数据包的目的IP地址

*linkoutput

    和上面的output基本上是相似的功能，但是更底层一些，这个函数主要在以太网卡通信中被ARP模块调用，用来完成以太网数据帧的发送，在其他类型的网络接口中，这个字段的值就没啥用处了。上面说的函数 etharp_output，它一方面接收IP层数据包，另一方面将该数据包封装为以太网数据帧（填写物理地址等字段），最后便调用linkoutput字段注册的函数将以太网数据帧发送出去 。 

* loop_first和loop_last

    为了实现对这些数据包的管理，在网络接口结构 netif中定义了两个指针loop_first和loop_last，它们分别用于指向数据包pbuf链表的第一个pbuf和最后一个pbuf。这里的pbuf链表比较特殊，因为所有数据包的pbuf都组织在同一条链表上，这就说明一条pbuf链表上可能存在多个数据包，这时pbuf结构的next字段是否为空并不能成为判断一个数据包结束与否的标志，那在这样的一条pbuf链表中，怎样去将各个数据包的pbuf划分开呢？答案在于pbuf结构中的tot_len字段和len字段值，当两个字段的值相等时，就代表一个数据包的结束。

# 3 函数实现

向系统注册一个网络接口设备的函数netif_add：

    ————netif.c————————————————
    // 定义两个全局型的 netif 指针
    struct netif *netif_list;       // 系统的全局型 netif 链表
    struct netif *netif_default;    // 记录系统缺省（默认）网络接口
    
    //函数功能：     向 LwIP 内核注册一个网络接口结构
    //参数 netif：   指向一个已分配好的 netif 结构体
    //参数 ipaddr：  网络接口的 IP 地址
    //参数 netmask： 网络接口子网掩码
    //参数 gw：      网关地址
    //参数 state：   用户自定义的一些数据信息
    //参数 init：    网络接口的初始化函数
    //参数 input：   网络接口向 IP 层提交数据包的函数
    //返回值：成功注册的网络接口结构指针
    struct netif *netif_add(struct netif    *netif,
                            struct ip_addr  *ipaddr,
                            struct ip_addr  *netmask,
                            struct ip_addr  *gw,
                            void            *state,
                            err_t           (* init)(struct netif *netif),
                            err_t           (* input)(struct pbuf *p, struct netif *netif))
    {
        static u8_t netifnum = 0;         // 定义静态变量 netifnum，它记录网络接口的编号
    　　
        /*
         * 清空 netif 结构的各个字段值
         */
    　　netif­>ip_addr.addr   = 0;
    　　netif­>netmask.addr   = 0;
    　　netif­>gw.addr        = 0;
    　　netif­>flags          = 0;
    　　netif­>loop_first     = NULL;
    　　netif­>loop_last      = NULL;
    　　
        /*
         * 填写结构体各个字段值
         */
    　　netif­>state  = state;
    　　netif­>num    = netifnum++;       // 网络接口编号
    　　netif­>input  = input;            // 注册 input 函数
    　　// 调用函数 netif_set_addr 设置网络接口的三个地址字段值
    　　netif_set_addr(netif, ipaddr, netmask, gw);
    　　
        /*
         * 调用网络接口的初始化函数，初始化网络接口
         */
    　　if (init(netif) != ERR_OK) {
    　　　　return NULL;                    // 初始化失败，则返回空
    　　}

    　　/*
         * 将初始化成功的 netif 结构加入 netif_list 链表
         */
    　　netif­>next   = netif_list;
        netif_list    = netif;
    　　
        /*
         * 返回 netif 结构指针
         */
    　　return netif;
    }
    ————————————————————————————————

netif_add函数只是简单的初始化了netif结构的几个字段，然后回调网络接口定义的初始化函数init来完成网络接口的初始化工作。

    /*
     * 初始化三个地址
     */
    IP4_ADDR(&gw, 192,168,1,1); 
    IP4_ADDR(&ipaddr, 192,168,1,37);
    IP4_ADDR(&netmask, 255,255,255,0);
    
    /*
     * 调用 netif_add 函数
     */
    netif_add(&rtl8019_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input);
    netif_set_default(&rtl8019_netif);      // 设置系统的默认网络接口为 rtl8019_netif
    netif_set_up(&rtl8019_netif);           // 使能网络接口 rtl8019_netif

在上面这段代码中，调用netif_add函数时，传递给它的两个函数参数是ethernetif_init和ethernet_input，其中前者就是网卡初始化函数ethernetif_init，这是源码提供者为以太网网卡驱动程序编写的默认初始化函数，这个函数在第5章中也有所提及，这里来看看它的具体实现，源代码如下所示。ethernet_input是ARP层的一个函数，它的功能是提取以太网帧中的ARP地址数据，并将帧中的IP数据递交给IP层，关于这个函数在下一章中会讲解。

至于ethernetif_init函数内容：

    #define IFNAME0 'e'         // 定义以太网网卡的名字字符
    #define IFNAME1 'n'
    
    // 定义描述网卡用户信息的结构，该结构无实际用处，源码作者旨在用该结构来示意
    // netif 结构中的 state 字段的用法，该字段可以指向任何用户关心的信息
    struct ethernetif {         // 该结构只包含一个简单的指针
    　　struct eth_addr *ethaddr;
    };

    /*
     * 函数参数 netif：网络接口结构的指针
     */
    err_t ethernetif_init(struct netif *netif)
    {
    　　struct ethernetif *ethernetif;
    　　ethernetif = mem_malloc(sizeof(struct ethernetif));   // 为用户信息结构申请一个内存堆空间
    　　if (ethernetif == NULL) {                             // 申请失败，返回内存错误
    　　　　return ERR_MEM;
    　　　}
    　　netif­>state = ethernetif;                            // 将 state 字段指向 ethernetif
    　　netif­>name[0] = IFNAME0;                             // 设置名字字段
    　　netif­>name[1] = IFNAME1;
    　　netif­>output = etharp_output;                        // 注册 IP 数据包输出函数，这里使用ARP的相关函数
    　　netif­>linkoutput = low_level_output;                 // 注册以太网数据帧输出函数
    　　
        // 将 ethernetif 结构赋一个用户关心的值，这里无具体意义
        ethernetif­>ethaddr = (struct eth_addr *)&(netif­>hwaddr[0]); // 指向网卡的地址信息
        low_level_init(netif);                                // 调用网卡底层初始化函数，这个函数已经讲过了
    　　return ERR_OK;                                        // 返回成功
    }


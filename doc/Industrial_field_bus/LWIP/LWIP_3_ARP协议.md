# 1 ARP协议

ARP，全称`Address Resolution Protocol`，译作`地址解析协议`，ARP协议与底层网络接口密切相关。TCP/IP 标准分层结构中，把 ARP划分为了网络层的重要组成部分。当一个主机上的应用程序要向目标主机发送数据时，它只知道目标主机的IP地址，而在协议栈底层接口发送数据包时，需要将该IP地址转换为目标主机对应的MAC地址，这样才能在数据链路上选择正确的通道将数据包传送出去，在整个转换过程中发挥关键作用的就是ARP协议了。 在本章中将看到：

1. ARP 协议的原理；
2. ARP 缓存表及其创建、维护、查询；
3. ARP 报文结构；
4. ARP 层数据包的接收处理；
5. ARP 层数据包的发送；

ARP层是将底层链路与协议上层连接起来的纽带，是以太网通信中不可或缺的协议。

#### 1.1 物理地址和网络地址

网卡的48位MAC地址都保存在网卡的内部存储器中，另一方面，TCP/IP协议有自己的地址：32位的IP地址（网络地址）。网络层发送数据包时只知道目的主机的IP地址，而底层接口（如以太网驱动程序）必须知道对方的硬件地址才能将数据发送出去。 

为了解决地址映射的问题，ARP协议提供了一种地址动态解析的机制，ARP的功能是在32位的IP地址和采用不同网络技术的硬件地址之间提供动态映射，为上层将底层的物理地址差异屏蔽起来，这样上层的因特网协议便可以灵活的使用IP地址进行通信。

#### 1.2 ARP协议的本质

ARP协议的基本功能是，使用目标主机的IP地址，查询其对应的MAC地址，以保证底层链路上数据包通信的进行。

举一个简单的例子来看看ARP的功能。假如我们的主机（192.168.1.78）需要向开发板（192.168.1.37）发送一个IP数据包。当发送数据时，主机会在自己的ARP缓存表中寻找是否有目标IP地址。如果找到了，也就知道了目标MAC地址为（00­80­48­12­34­56），此时主机直接把目标MAC地址写入以太网帧首部发送就可以了；如果在ARP缓存表中没有找到相对应的IP地址，此时比较不幸，我们的数据需要被延迟发送，随后主机会先在网络上发送一个广播（ARP请求，以太网目的地址为FF­FF­FF­FF­FF­FF）。

广播的ARP请求表示同一网段内的所有主机将会收到这样一条信息：“192.168.1.37的MAC地址是什么？请回答”。

网络IP地址为192.168.1.37（开发板）的主机接收到这个帧后，它有义务做出这样的回答（ARP应答）：“192.168.1.37的MAC地址是（00­80­48­12­34­56）”。

这样，主机就知道了开发板的MAC地址，先前被延迟的数据包就可以发送了，此外，主机会将这个地址对保存在缓存表中以便后续数据包发送时使用。ARP的实质就是对缓存表的建立、更新、查询等操作。

# 2 数据结构

源文档中的etharp.c和etharp.h文件实现了以太网中ARP协议的全部数据结构和函数定义，ARP协议实现过程中有两个重要的数据结构，即ARP缓存表和ARP报文。

#### 2.1 ARP表

ARP协议的核心在于ARP缓存表，ARP的实质就是对缓存表的建立、更新、查询等操作。

ARP缓存表由缓存表项（entry）组成，每个表项记录了一组IP地址和MAC地址绑定信息。当然，除了这两个基本信息外，还包含了与数据包发送控制、缓存表项管理相关的状态、控制信息。LwIP中描述缓存表项的数据结构叫etharp_entry，这个结构比较简单，如下所示： 

    ————etharp.c—————————————————————————
    struct etharp_entry {
        struct etharp_q_entry *q;         // 数据包缓冲队列指针
        struct ip_addr        ipaddr;     // 目标IP地址
        struct eth_addr       ethaddr;    // MAC地址
        enum etharp_state     state;      // 描述该entry的状态
        u8_t                  ctime;      // 描述该entry的时间信息
        struct netif          *netif;     // 对应网络接口信息
    };
    ——————————————————————————————————

描述缓冲队列的数据结构也很简单，叫做etharp_q_entry，该结构的定义如下：

    ————etharp.h——————————————————
    struct etharp_q_entry {
        struct etharp_q_entry *next;          // 指向下一个缓冲数据包
        struct pbuf *p;                       // 指向数据包 pbuf
    };
    —————————————————————————————————

用一个图来看看etharp_q_entry结构在缓存表数据队列中的作用，如下图所示。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_3_1.png">

state是个枚举类型，它描述该缓存表项的状态，LwIP中定义一个缓存表项可能有三种不同的状态，用枚举型etharp_state进行描述。

    ————etharp.c—————————————————————————
    enum etharp_state {
        ETHARP_STATE_EMPTY = 0,           // empty 状态
        ETHARP_STATE_PENDING,             //pending状态
        ETHARP_STATE_STABLE               // stable 状态
    };
    ————————————————————————————————

编译器为ARP表预先定义了ARP_TABLE_SIZE（通常为 10）个表项空间，因此ARP缓存表内部最多只能存放ARP_TABLE_SIZE条IP地址与MAC 地址配对信息。

    ————etharp.c——————————————————————
    static struct etharp_entry arp_table[ARP_TABLE_SIZE];   // 定义ARP缓存表
    ——————————————————————————————————

1. `ETHARP_STATE_EMPTY`状态（empty）

    初始化的时候为empty状态。        

2. `ETHARP_STATE_PENDING`状态（pending）

    表示该表项处于不稳定状态，此时该表项只记录到了IP地址，但是还未记录到对应的MAC地址。很可能的情况是，LwIP内核已经发出一个关于该IP地址的ARP请求到数据链路上，但是还未收到ARP应答。 

3. `ETHARP_STATE_STABLE`状态（stable）

    当ARP表项被更新后，它就记录了一对完整的IP地址和MAC地址 。

在ETHARP_STATE_PENDING状态下会设定超时时间（10秒）。当计数超时后，对应的表项将被删除；在ETHARP_STATE_STABLE 状态下也会设定超时时间（20分钟），当计数超时后，对应的表项将被删除。

最后一个字段，网络接口结构指针`netif`，在ARP表项中维护这样一个指针还是很有用的，因为该结构中包含了网络接口的MAC地址和IP地址等信息，在发送数据包的时候，这些信息都起着至关重要的作用。     

ctime为每个表项的计数器，周期性的去调用一个etharp_tmr函数，这个函数以5秒为周期被调用，在这个函数中，它会将每个ARP缓存表项的ctime字段值加1，当相应表项的生存时间计数值ctime大于系统规定的某个值时，系统将删除对应的表项。 

    ————etharp.c————————————————————
    
    #define ARP_MAXAGE      240     // 稳定状态表项的最大生存时间计数值：240*5s=20min    
    
    #define ARP_MAXPENDING  2       // PENDING状态表项的最大生存时间计数值：2*5s=10s
    void etharp_tmr(void)
    {
        u8_t i;
        for (i = 0; i < ARP_TABLE_SIZE; ++i)    {       // 对每个表项操作，包括空闲状态的表项
            arp_table[i].ctime++;                       // 先将表项 ctime 值加 1
            
            // 如果表项是 stable 状态，且生存值大于 ARP_MAXAGE，
            // 或者是pending状态且其生存值大于 ARP_MAXPENDING，则删除表项
            if ( ((arp_table[i].state == ETHARP_STATE_STABLE) && //stable 状态
                    (arp_table[i].ctime >= ARP_MAXAGE))
                        ||        // 或者
                        ((arp_table[i].state == ETHARP_STATE_PENDING) && //pending 状态
                            (arp_table[i].ctime >= ARP_MAXPENDING)) )         {
                if (arp_table[i].q != NULL)             {       // 如果表项上的数据队列中有数据，
                    free_etharp_q(arp_table[i].q);              // 则释放队列中的所有数据
                    arp_table[i].q = NULL;                      // 队列设置为空
                }
                arp_table[i].state = ETHARP_STATE_EMPTY;        // 将表项状态改为未用，即删除
            }
        }
    }
    ——————————————————————————————————

#### 2.2 ARP报文

源主机如何告诉目的主机：我需要你的MAC地址；而目的主机如何回复：这就是我的MAC地址。ARP报文（或者称之为ARP数据包），这就派上用场了。ARP请求和ARP应答，它们都是被组装在一个ARP数据包中发送的，这里先来看看一个典型的ARP包的组成结构。如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_3_2.png">

1. 以太网目的地址和以太网源地址

    分别表示以太网目的MAC地址和源MAC地址，目的地址全1时是特殊地址-以太网广播地址。在ARP表项建立前，源主机只知道目的主机的IP地址，并不知道其MAC地址，所以在数据链路上，源主机只有通过广播的方式将ARP请求数据包发送出去，同一网段上的所有以太网接口都会接收到广播的数据包。 

2. 桢类型
    
    ARP-0x0806、IP-0x0800、PPPoE-0x8864

3. 硬件类型

    表示发送方想要知道的硬件类型。1-以太网MAC地址

4. 协议类型

    表示要映射的协议地址类型，0x0800-表示要映射为IP地址    

5. 硬件地址长度和协议地址长度

    以太网ARP请求和应答来说分别为6和4，代表MAC地址长度和IP地址长度。在ARP协议包中留出硬件地址长度字段和协议地址长度字段可以使得ARP协议在任何网络中被使用，而不仅仅只在以太网中。 

6. op

    指出ARP数据包的类型，ARP请求（1），ARP应答（2）

在以太网的数据帧头部中和ARP数据包中都有发送端的以太网MAC地址。对于一个ARP请求包来说，除接收方以太网地址外的所有字段都应该被填充相应的值。当接收方主机收到一份给自己的ARP请求报文后，它就把自己的硬件地址填进去，然后将该请求数据包的源主机信息和目的主机信息交换位置，并把操作字段op置为2，最后把该新构建的数据包发送回去，这就是ARP应答。 

关于上图中的这个结构，在ARP中用了一大堆的数据结构和宏来描述它们。

    ————etharp.h————————————————
    #ifndef ETHARP_HWADDR_LEN
    #define ETHARP_HWADDR_LEN   6 //以太网物理地址长度
    #endif

    // 我们移植时实现的结构体封装宏
    PACK_STRUCT_BEGIN 
    struct eth_addr {   // 定义以太网MAC地址结构体eth_addr，禁止编译器自对齐
        PACK_STRUCT_FIELD(u8_t addr[ETHARP_HWADDR_LEN]);
    } PACK_STRUCT_STRUCT;
    PACK_STRUCT_END

    //定义以太网数据帧首部结构体eth_hdr，禁止编译器自对齐
    PACK_STRUCT_BEGIN 
    struct eth_hdr {
        PACK_STRUCT_FIELD(struct eth_addr dest);  // 以太网目的地址（6 字节）
        PACK_STRUCT_FIELD(struct eth_addr src);   // 以太网源地址（6 字节）
        PACK_STRUCT_FIELD(u16_t type);            // 帧类型（2 字节）
    } PACK_STRUCT_STRUCT;
    PACK_STRUCT_END

    // 定义以太网帧头部长度宏，其中ETH_PAD_SIZE已定义为 0
    #define SIZEOF_ETH_HDR      (14 + ETH_PAD_SIZE)

    // 定义ARP数据包结构体 etharp_hdr，禁止编译器自对齐
    PACK_STRUCT_BEGIN
    struct etharp_hdr {
        PACK_STRUCT_FIELD(u16_t hwtype);              // 硬件类型（2 字节）
        PACK_STRUCT_FIELD(u16_t proto);               // 协议类型（2 字节）
        PACK_STRUCT_FIELD(u16_t _hwlen_protolen);     // 硬件+协议地址长度（2 字节）
        PACK_STRUCT_FIELD(u16_t opcode);              // 操作字段 op（2 字节）
        PACK_STRUCT_FIELD(struct eth_addr shwaddr);   // 发送方MAC地址（6 字节）
        PACK_STRUCT_FIELD(struct ip_addr2 sipaddr);   // 发送方IP地址（4 字节）
        PACK_STRUCT_FIELD(struct eth_addr dhwaddr);   // 接收方MAC地址（6 字节）
        PACK_STRUCT_FIELD(struct ip_addr2 dipaddr);   // 接收方IP地址（4 字节）
    } PACK_STRUCT_STRUCT;
    PACK_STRUCT_END

    #define SIZEOF_ETHARP_HDR 28 //宏，ARP 数据包长度
    // 宏，包含ARP数据包的以太网帧长度
    #define SIZEOF_ETHARP_PACKET (SIZEOF_ETH_HDR + SIZEOF_ETHARP_HDR)
    #define ARP_TMR_INTERVAL    5000       // 定义ARP定时器周期为5秒，不同帧类型的宏定义
    #define ETHTYPE_ARP         0x0806
    #define ETHTYPE_IP          0x0800
    
    //ARP数据包中 OP 字段取值宏定义
    #define ARP_REQUEST         1           //ARP请求
    #define ARP_REPLY           2           //ARP应答
    ————————————————————————————

发送ARP请求数据包的函数叫etharp_request，看名字就晓得了。这个函数很简单，它是通过调用etharp_raw函数来实现的，调用后者时，需要为它提供ARP数据包中各个字段的值，后者直接将各个字段的值填写到在一个ARP包中发送（该函数并不知道发送的是ARP请求还是ARP响应，它只管组装并发送，所以称之为raw）

    ————etharp.c——————————————————
    //函数功能：根据各个参数字段组织一个ARP数据包并发送
    //注：ARP 数据包中其他字段使用预定义值，例如硬件地址长度为 6，协议地址长度为 4
    err_t etharp_raw(struct netif           *netif,         /* 发送ARP包的网络接口结构 */
                    const struct eth_addr   *ethsrc_addr,   /* 以太网帧首部中的以太网源地址值 */ 
                    const struct eth_addr   *ethdst_addr,   /* 以太网帧首部中的以太网目的地址值 */
                    const struct eth_addr   *hwsrc_addr,    /* ARP数据包中的发送方MAC地址 */
                    const struct ip_addr    *ipsrc_addr,    /* ARP数据包中的发送方IP地址 */
                    const struct eth_addr   *hwdst_addr,    /* ARP数据包中的接收方MAC地址 */
                    const struct ip_addr    *ipdst_addr,    /* ARP数据包中的接收方IP地址 */
                    const u16_t             opcode          /* ARP数据包中的OP字段值 */
                    )
    {
        struct pbuf     *p;                 // 数据包指针
        err_t           result = ERR_OK;    // 返回结果
        u8_t            k;
        struct eth_hdr  *ethhdr;            // 以太网数据帧首部结构体指针
        struct etharp_hdr *hdr;             // ARP数据包结构体指针
        
        // 先在内存堆中为ARP包分配空间，大小为包含ARP数据包的以太网帧总大小
        p = pbuf_alloc(PBUF_RAW, SIZEOF_ETHARP_PACKET, PBUF_RAM);
        if (p == NULL)     { //若分配失败则返回内存错误
            return ERR_MEM;
        }
        // 到这里，内存分配成功
        ethhdr = p­>payload;                // ethhdr指向以太网帧首部区域
        hdr = (struct etharp_hdr *)((u8_t*)ethhdr + SIZEOF_ETH_HDR);// hdr指向ARP首部
        hdr­>opcode = htons(opcode);        // 填写ARP包的OP字段，注意大小端转换
        k = ETHARP_HWADDR_LEN;              // 循环填写数据包中各个MAC地址字段
        while(k > 0)     {
            k--­­;
            hdr­>shwaddr.addr[k] = hwsrc_addr­>addr[k];     //ARP头部的发送方MAC地址
            hdr­>dhwaddr.addr[k] = hwdst_addr­>addr[k];     //ARP头部的接收方MAC地址
            ethhdr­>dest.addr[k] = ethdst_addr­>addr[k];    // 以太网帧首部中的以太网目的地址
            ethhdr­>src.addr[k] = ethsrc_addr­>addr[k];     // 以太网帧首部中的以太网源地址
        }
        hdr­>sipaddr = *(struct ip_addr2 *)ipsrc_addr;      // 填写ARP头部的发送方IP地址
        hdr­>dipaddr = *(struct ip_addr2 *)ipdst_addr;      // 填写ARP头部的接收方IP地址
        
        // 下面填充一些固定字段的值
        hdr­>hwtype = htons(HWTYPE_ETHERNET);               //ARP头部的硬件类型为 1，即以太网
        hdr­>proto = htons(ETHTYPE_IP);                     //ARP头部的协议类型为 0x0800
        
        // 设置两个长度字段
        hdr­>_hwlen_protolen = htons((ETHARP_HWADDR_LEN << 8) | sizeof(struct ip_addr));
        ethhdr­>type = htons(ETHTYPE_ARP);                  // 以太网帧首部中的帧类型字段，ARP 包
        result = netif­>linkoutput(netif, p);               // 调用底层数据包发送函数
        pbuf_free(p);                                       // 释放数据包
        p = NULL;
        return result;                                      // 返回发送结果
    }
    
    // 特殊MAC地址的定义，以太网广播地址
    const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
    // 该值用于填充ARP请求包的接收方MAC字段，无实际意义
    const struct eth_addr ethzero = {{0,0,0,0,0,0}};
    
    /*
     * 函数功能：发送ARP请求
     * 参数 netif：发送ARP请求包的接口结构
     * 参数 ipaddr：请求具有该IP地址主机的MAC
     */
    err_t etharp_request(struct netif *netif, struct ip_addr *ipaddr)
    {
        //该函数只是简单的调用函数 etharp_raw，为函数提供所有相关参数
        return etharp_raw(netif, 
                        (struct eth_addr *)netif­>hwaddr, 
                        &ethbroadcast,
                        (struct eth_addr *)netif­>hwaddr, 
                        &netif­>ip_addr, 
                        &ethzero,
                        ipaddr, 
                        ARP_REQUEST
                        );
    }
    ——————————————————————————————————

# 3 ARP层数据包输入

#### 3.1 以太网数据包向上递交

在我们说网卡驱动的时候讲到了数据包接收函数ethernetif_input，这个函数是源码作者提供的一个以太网数据包接收和递交函数，它的功能是调用底层数据包接收函数low_level_input读取网卡中的数据包，然后在将该数据包递交给相应的上层处理。

    ————ethernetif.c————————————————
    static void ethernetif_input(struct netif *netif)
    {
        struct ethernetif     *ethernetif;    // 用户自定义的网络接口信息结构，这里无用处
        struct eth_hdr        *ethhdr;        // 以太网帧头部结构指针
        struct pbuf           *p;

        ethernetif = netif­>state;            // 获得自定义的网络信息结构，无实际意义
        p = low_level_input(netif);           // 调用底层函数读取一个数据包
        if (p == NULL) return;                // 如果数据包为空，则直接返回
        
        // 到这里数据包不为空
        ethhdr = p­>payload;                    // 将ethhdr指向数据包中的以太网头部
        switch (htons(ethhdr­>type)) {          // 判断帧类型，注意大小端转换
            case ETHTYPE_IP:                    // 对于IP包和ARP包，都调用注册的netif­>input函数
            case ETHTYPE_ARP:                   // 进行处理
                etharp_arp_input(netif, (struct eth_addr*)(netif­>hwaddr), p); 
                if (netif­>input(p, netif)!=ERR_OK) {  // 未完成正常的处理，则释放数据包
                    pbuf_free(p);
                    p = NULL;
                }
                break;
            default: // 对于其他类型的数据包，直接释放掉，不做处理
                pbuf_free(p);
                p = NULL;
                break;
        }//switch
    }
    ——————————————————————————————————————

#### 3.2 ARP数据包处理

首先，若这个请求的IP地址与本机地址不匹配，那么就不需要返回ARP应答，但由于该ARP请求包中包含了发送请求的主机的IP地址和MAC地址，可以将这个地址对加入到ARP缓存表中，以便后续使用；其次，如果ARP请求与本机IP地址匹配。此时，除了进行上述的记录源主机的IP地址和MAC地址外，还需要给源主机返回一个ARP应答。整个过程清楚后，就可以来看具体的代码实现了。 

    ————etharp.c————————————————————
    // 函数功能：处理ARP数据包，更新ARP缓存表，对ARP请求进行应答
    // 参数ethaddr：网络接口的MAC地址
    void etharp_arp_input(struct netif *netif, struct eth_addr *ethaddr, struct pbuf *p)
    {
        struct etharp_hdr   *hdr;               // 指向ARP数据包头部的变量
        struct eth_hdr      *ethhdr;            // 指向以太网帧头部的变量
        struct ip_addr      sipaddr, dipaddr;   //暂存ARP包中的源IP地址和目的IP地址
        u8_t                i;
        u8_t                for_us;             // 用于指示该ARP包是否是发给本机的
        
        /*
         * 接下来判断ARP包是否是放在一个pbuf中的，由于整个ARP包都使用结构etharp_hdr
         * 进行操作，如果ARP包是分装在两个pbuf中的，那么对于结构体etharp_hdr的操作将
         * 无意义，我们直接丢弃掉这种类型的ARP包
         */
        if (p­>len < SIZEOF_ETHARP_PACKET) {    // ARP包不能分装在两个pbuf中
            pbuf_free(p);                       // 否则直接删除，函数返回
            return;
        }
        
        ethhdr = p­>payload;                    // ethhdr指向以太网帧首部
        hdr = (struct etharp_hdr *)((u8_t*)ethhdr + SIZEOF_ETH_HDR); 
                                                // hdr指向ARP包首部

        /*
         * 这里需要判断ARP包的合法性，丢弃掉那些类型、长度不合法的ARP包
         */
        if ((hdr­>hwtype != htons(HWTYPE_ETHERNET)) ||  // 是否为以太网硬件类型
                (hdr­>_hwlen_protolen != htons((ETHARP_HWADDR_LEN << 8) | sizeof(struct ip_addr))) ||
                (hdr­>proto != htons(ETHTYPE_IP)) ||    // 协议类型为 IP
                (ethhdr­>type != htons(ETHTYPE_ARP))    // 是否为ARP数据包
                ) { 
            pbuf_free(p);                               // 若不符合，则删除数据包，函数返回
            return;
        }

        /*
         * 这里需要将ARP包中的两个IP地址数据拷贝到变量sipaddr和dipaddr中，因为后面
         * 会使用这两个IP地址，但ARP数据包中的IP地址字段并不是字对齐的，不能直接使用
         */
        SMEMCPY(&sipaddr, &hdr­>sipaddr, sizeof(sipaddr)); // 拷贝发送方IP地址到sipaddr中
        SMEMCPY(&dipaddr, &hdr­>dipaddr, sizeof(dipaddr)); // 拷贝接收方IP地址到dipaddr中
        
        /*
         * 下面判断这个ARP包是否是发送给我们的
         */
        if (netif­>ip_addr.addr == 0) {     // 如果网卡IP地址未配置
            for_us = 0;                     // 那么肯定不是给我们的，设置标志for_us为 0
        } else {                            // 如果网卡IP地址已经设置，则将目的IP地址与网卡IP地址比较
            for_us = ip_addr_cmp(&dipaddr, &(netif­>ip_addr)); 
                                            // 若相等，for_us被置为 1
        }

        /*
         * 下面我们开始更新ARP缓存表
         */
        if (for_us) {   // 如果这个ARP包（请求或响应）是给我们的，则更新ARP表
            update_arp_entry(netif, &sipaddr, &(hdr­>shwaddr), ETHARP_TRY_HARD);
        } else {        // 若不是给我们的，也更新ARP表，但是不设置ETHARP_TRY_HARD标志
            update_arp_entry(netif, &sipaddr, &(hdr­>shwaddr), 0);
        }

        /*
         * 到这里，ARP更新完毕，需要对ARP请求做出处理(应答)
         */
        switch (htons(hdr­>opcode)) {       // 判断ARP数据包的op字段
            case ARP_REQUEST:
                if (for_us) {
                    /*
                     * 如果是ARP请求且请求中的IP地址与本机的匹配，则需要返回ARP应答
                     * ARP应答的返回很简单，不需要再重新申请一个ARP数据包空间，
                　   * 而是直接将该ARP请求包中的相应字段进行改写，构造一个应答包
                     */
                    hdr­>opcode = htons(ARP_REPLY); // 将op字段改为ARP响应类型
                    hdr­>dipaddr = hdr­>sipaddr;    // 设置接收端IP地址
                    SMEMCPY(&hdr­>sipaddr, &netif­>ip_addr, sizeof(hdr­>sipaddr));
                                                    // 设置发送端IP地址为网络接口中的IP地址
                    /*
                     * 接下来，设置四个MAC地址字段
                     * 目标MAC地址可以直接从原来的ARP包中得到
                     * 源MAC地址我们已经通过参数ethaddr传入
                     */
                    i = ETHARP_HWADDR_LEN;
                    while(i > 0) {
                        i--;
                        hdr­>dhwaddr.addr[i] = hdr­>shwaddr.addr[i];    // 设置ARP包的接收端MAC地址
                        ethhdr­>dest.addr[i] = hdr­>shwaddr.addr[i];    // 以太网帧中的目标MAC地址
                        hdr­>shwaddr.addr[i] = ethaddr­>addr[i];        // ARP头部的发送端MAC地址
                        ethhdr­>src.addr[i]  = ethaddr­>addr[i];        // 以太网帧头部的源MAC地址
                    }
                    //对于ARP包中的其他字段的值（硬件类型、协议类型、长度字段等）
                    //保持它们的值不变，因为在前面已经测试过了它们的有效性
                    netif­>linkoutput(netif, p);                        // 直接发送ARP应答包
                } else if (netif­>ip_addr.addr == 0) {                  // ARP请求数据包不是给我们的，不做任何处理
                    } //这里只打印一些调试信息，笔者已将它们去掉
                else                 {
                    }
                break;
        　case ARP_REPLY: //如果是ARP应答，我们已经在最开始更新了ARP表
            break; //这里神马都不用做了
        　default:
            break;
        }// switch
        pbuf_free(p); //删除数据包 p
    }
    ————————————————————————————————————

#### 3.3 ARP攻击

#### 3.4 ARP缓存表更新

# 4 ARP层数据包输出

#### 4.1 ARP数据处理流程图

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_3_3.png">

#### 4.2 广播包与多播包的发送

etharp_output 函数被IP层的数据包发送函数ip_output调用，它首先根据目的 IP地址的类型为数据包选择不同的处理方式：当目的IP地址为广播或者多播地址时，etharp_output可以直接根据这个目的地址构造出相应的特殊MAC地址，同时把MAC地址作为参数，和数据包一起交给etharp_send_ip发送；当目的IP地址为单播地址时，需要调用etharp_query函数在 ARP表中查找与目的IP地址对应的MAC地址，若找到，则函数etharp_send_ip被调用，以发送数据包；若找不到，则函数etharp_request被调用它会发送一个关于目的IP地址的ARP请求包，出现这种情况时，我们还需要将IP包挂接的相应表项的缓冲队列中，直到对应的ARP应答返回时，该数据包才会被发送出去。 

    ————etharp.c——————————————
    //函数功能：发送一个IP数据包 pbuf 到目的地址 ipaddr 处，该函数被IP层调用
    //参数 netif：指向发送数据包的网络接口结构
    //参数 q：指向IP数据包 pbuf 的指针
    //参数 ipaddr：指向目的IP地址
    err_t etharp_output(struct netif *netif, struct pbuf *q, struct ip_addr *ipaddr)
    {
    　　struct eth_addr *dest, mcastaddr;
    　　if (pbuf_header(q, sizeof(struct eth_hdr)) != 0) {//调整 pbuf 的 payload 指针，使其指向
    　　return ERR_BUF; //以太网帧头部，失败则返回
    　　}
    　　dest = NULL;
    　　if (ip_addr_isbroadcast(ipaddr, netif)) 　　{//如果是广播IP地址
    　　　　dest = (struct eth_addr *)&ethbroadcast; //dest 指向广播 MAC 地址
    　　} else if (ip_addr_ismulticast(ipaddr)) 　　{//如果是多播IP地址
    　　　　mcastaddr.addr[0] = 0x01; //则构造多播 MAC 地址
    　　　　mcastaddr.addr[1] = 0x00;
    　　　　mcastaddr.addr[2] = 0x5e;
    　　　　mcastaddr.addr[3] = ip4_addr2(ipaddr) & 0x7f;
    　　　　mcastaddr.addr[4] = ip4_addr3(ipaddr);
    　　　　mcastaddr.addr[5] = ip4_addr4(ipaddr);
    　　　　dest = &mcastaddr; // dest 指向多播 MAC 地址
    　　} else { //如果为单播IP地址
    　　　　//判断目的IP地址是否为本地的子网上，若不在，则修改 ipaddr
    　　　　if (!ip_addr_netcmp(ipaddr, &(netif­>ip_addr), &(netif­>netmask))) 　　　　{
    　　　　　　if (netif­>gw.addr != 0) 　　　　　　{ //需要将数据包发送到网关处，由网关转发
    　　　　　　　　ipaddr = &(netif­>gw); //更改变量 ipaddr，数据包发往网关处
    　　　　　　} else 　　　　　　{ //如果网关未配置，返回错误
    　　　　　　　　return ERR_RTE;
    　　　　　　}
    　　　　}
    　　　　//对于单播包，调用 etharp_query 查询其 MAC 地址并发送数据包
    　　　　return etharp_query(netif, ipaddr, q);
    　　}
    　　//对于多播和广播包，由于得到了它们的目的 MAC 地址，所以可以直接发送
    　　return etharp_send_ip(netif, q, (struct eth_addr*)(netif­>hwaddr), dest);
    }
    ————————————————————————————————

广播包：调用函数ip_addr_isbroadcast判断目的IP地址是否为广播地址，如果是广播包，则目的MAC地址不需要查询arp表，由于广播MAC地址的48位均为 1，即目的MAC六个字节值为ff­ff­ff­ff­ff­ff。

多播包：判断目的IP地址是不是D类IP地址，如果是，则MAC地址可以直接计算得出，即将MAC地址01­00­5E­00­00­00的低23位设置为IP 地址的低23位。对于以上的两种数据包，etharp_output直接调用函数etharp_send_ip将数据包发送出去。

单播包：要比较目的IP和本地IP地址，看是否是局域网内的，若不是局域网内的，则将目的IP地址设置为默认网关的地址，然后再统一调用etharp_query函数查找目的MAC地址，最后将数据包发送出去。

    ————etharp_send_ip————————————————————
    //函数功能：填写以太网帧头部，发送以太网帧
    //参数 p：指向以太网帧的 pbuf
    //参数 src：指向源 MAC 地址
    //参数 dst：指向目的 MAC 地址
    static err_t
    etharp_send_ip(struct netif *netif, struct pbuf *p, struct eth_addr *src, struct eth_addr *dst)
    {
    　　struct eth_hdr *ethhdr = p­>payload; //指向以太网帧头部
    　　u8_t k;
    　　k = ETHARP_HWADDR_LEN;
    　　while(k > 0)　　{
    　　　　k--­­;
    　　　　ethhdr­>dest.addr[k] = dst­>addr[k]; //填写目的 MAC 字段
    　　　　ethhdr­>src.addr[k] = src­>addr[k]; //填写源 MAC 字段
    　　}
    　　ethhdr­>type = htons(ETHTYPE_IP); //填写帧类型字段
    　　return netif­>linkoutput(netif, p); //调用网卡数据包发送函数
    }
    ————————————————————————————————————

这个函数尤其简单，直接根据传入的参数填写以太网帧首部的三个字段，然后调用注册的底层数据包发送函数将数据包发送出去。

#### 4.3 单播包的发送

如果给定的IP地址不在ARP表中，则一个新的ARP表项会被创建，此时该表项处于pending状态，同时一个关于该IP地址的ARP请求会被广播出去，再同时要发送的IP数据包会被挂接在该表项的数据缓冲指针上；如果IP地址在ARP表中有相应的表项存在，但该表项处于pending 状态，则操作与前者相同，即发送一个ARP请求和挂接数据包；如果IP地址在ARP表中有相应的表项存在，且表项处于stable状态，此时再来判断给定的数据包是否为空，不为空则直接将该数据包发送出去，为空则向该IP地址发送一个ARP请求。 

    //函数功能：查找单播IP地址对应的 MAC 地址，并发送数据包
    //参数 ipaddr：指向目的IP地址
    //参数 q：指向以太网数据帧的 pbuf
    err_t etharp_query(struct netif *netif, struct ip_addr *ipaddr, struct pbuf *q)
    {
    　　struct eth_addr * srcaddr = (struct eth_addr *)netif->hwaddr;
    　　err_t result = ERR_MEM;
    　　s8_t i;
    　　//调用函数 find_entry 查找或创建一个ARP表项
    　　i = find_entry(ipaddr, ETHARP_TRY_HARD);
    　　if (i < 0) 
    　　{ //若查找失败，则 i 小于 0，直接返回
    　　　　return (err_t)i;
    　　}
    　　//如果表项的状态为 empty，说明表项是刚创建的，且其中已经记录了IP地址
    　　if (arp_table[i].state == ETHARP_STATE_EMPTY) 
    　　{ //将表项的状态改为 pending
    　　　　arp_table[i].state = ETHARP_STATE_PENDING;
    　　}
    　　if ((arp_table[i].state == ETHARP_STATE_PENDING) || (q == NULL)) 
    　　{//数据包为空，或
    　　　　result = etharp_request(netif, ipaddr); //表项为pending态，则发送ARP请求包
    　　}
    　　if (q != NULL) 
    　　{//数据包不为空，则进行数据包的发送或者将数据包挂接在缓冲队列上
    　　　　if (arp_table[i].state == ETHARP_STATE_STABLE) {//ARP 表稳定，则直接发送数据包
    　　　　　　result = etharp_send_ip(netif, q, srcaddr, &(arp_table[i].ethaddr));
    　　　　} else if (arp_table[i].state == ETHARP_STATE_PENDING) {//否则，挂接数据包
        　　　　struct pbuf *p;
        　　　　int copy_needed = 0;//是否需要重新拷贝整个数据包，数据包全由 PBUF_ROM
        　　　　p = q; //类型的 pbuf 组成时，才不需要拷贝
        　　　　while (p) { //判断是否需要拷贝整个数据包
        　　　　　　if(p->type != PBUF_ROM) {
        　　　　　　　　copy_needed = 1;
        　　　　　　　　break;
        　　　　　　}
        　　　　　　p = p->next;
        　　　　}
        　　　　if(copy_needed) { //如果需要拷贝，则申请内存堆空间
        　　　　　　p = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);//申请一个 pbuf 空间
        　　　　　　if(p != NULL) {申请成功，则执行拷贝操作
        　　　　　　if (pbuf_copy(p, q) != ERR_OK) {//拷贝失败，则释放申请的空间
        　　　　　　　　pbuf_free(p);
        　　　　　　　　p = NULL;
        　　　　　　}
        　　　　}
        　　　　} else { //如果不需要拷贝
        　　　　　　p = q; //设置 p
        　　　　　　pbuf_ref(p); //增加 pbuf 的 ref 值
        　　　　}
        　　　　//到这里，p 指向了我们需要挂接的数据包，下面执行挂接操作
        　　　　if (p != NULL) {
        　　　　　　struct etharp_q_entry *new_entry; //为数据包申请一个 etharp_q_entry 结构
        　　　　　　new_entry = memp_malloc(MEMP_ARP_QUEUE); //在内存池 POOL 中
        　　　　　　if (new_entry != NULL) 
        　　　　　　{ //申请成功，则进行挂接操作
        　　　　　　　　new_entry->next = 0; //设置 etharp_q_entry 结构的指针
        　　　　　　　　new_entry->p = p;
        　　　　　　　　if(arp_table[i].q != NULL) { //若缓冲队列不为空
        　　　　　　　　　　struct etharp_q_entry *r;
        　　　　　　　　　　r = arp_table[i].q;
            　　　　　　　　while (r->next != NULL) {//则找到最后一个缓冲包结构
            　　　　　　　　　　r = r->next;
            　　　　　　　　}
            　　　　　　　　r->next = new_entry; //将新的数据包挂接在队列尾部
            　　　　　　} else { //缓冲队列为空
            　　　　　　　　arp_table[i].q = new_entry; //直接挂接在缓冲队列首部
            　　　　　　}
            　　　　　　result = ERR_OK;
        　　　　　　} else { //etharp_q_entry 结构申请失败，则
        　　　　　　　　pbuf_free(p); //释放数据包空间
        　　　　　　}
        　　　　 // if (p != NULL)
    　　　　　}//else if
    　　}// if (q != NULL)
    　　return result; //返回函数操作结果
    }
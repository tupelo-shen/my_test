# 1 相关知识

#### 1.1 概述

#### 1.2 IP地址

A 类地址中只能有125个网络号被分配使用，即全世界只有125个机构能使用A类网络号，这样的机构通常是很大的机构，因为每个A类网络号内包含了上亿个主机地址，通常没有任何一个机构能够使用这么多的主机，因此数以百万计的A类网络地址将被浪费掉。

B 类网络地址（128～191）占据了整个 32bit 地址空间的四分之一，它用前两个字节的后 14bit表示网络号，后两个字节表示该网络内的主机号。B 类网络的网络号有 16384 个，但其中的16 个网络号（172.16 到 172.31）被保留为 B 类网络专用地址，所以可用的 B 类网络号有 16368 个，每个网络能容纳 65534 个主机（216­2）。使用 B 类地址的机构也应该是足够大的机构。

C 类网络地址（192～223）占据了整个 32bit 地址空间的八分之一，该类地址用前三个字节中的后21bit 表示网络号，最后一个字节表示该网络内的主机号。C 类地址网络号可达 2097152 个，但其中的 256 个网络号（192.168.0 至 192.168.255）被保留为 C 类网络专用地址，因此可用的 C 类网络号有 2096896 个，每个网络能容纳 254 个主机。C 类网络地址是为具有较少主机和路由器的小型机构设计的，通常该类网络中的主机号往往不够用。

D 类网络地址（224～239）占据了整个 32bit 地址空间的十六分之一，是一组专门为组播保留的地址。该类地址并不指向任何特定的网络，每一个 D 类地址定义了在网络中的一组主机，这组机器可以基于这个地址进行多播通信。

E 类网络地址（224～239）占据了整个 32bit 地址空间的十六分之一，为将来使用而保留的。其中全 1 的 IP 地址（255.255.255.255）用作特殊用途，表示当前子网的广播地址。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_4_1.png">

A类网络地址：0.xxx.xxx.xxx、127.xxx.xxx.xxx、10.xxx.xxx.xxx（3）
B类网络地址：172.16.0.0~172.31.255.255（16）
C类网络地址：192.168.0.0~192.168.255.255（256）

#### 4.3 特殊IP地址

下面说说几个具有特殊用途的 IP 地址，它们不能分配给任何主机。

环回地址 ：127.xxx.xxx.xxx（通常使用127.0.0.1），PS：环回地址使得 A 类地址少了一个网络号

网络地址 ：对任意一个 IP 地址来说，将其地址结构中的主机号全部取 0，就得到了主机所处的网络地址。 例如，某主机 IP 地址为 134.89.32.33，它属于 B 类地址，将其后两个主机号字节全部取 0，则得到这个主机所处的网络地址为 134.89.0.0。 PS：网络地址占用了 A、B、C 三类地址中的每个网络号下的一个主机号。

直接广播地址 ：在一个网络内，直接广播地址是指对应主机号全部取 1 而得到的 IP 地址，广播地址代表本网络内的所有网络设备，使用该地址可以向属于同一个网段内的所有网络设备传送数据。例如：一个标准 C 类地址 202.197.15.44，由于它的网络号由前面 3 个字节组成，主机号仅是最后一个字节，将主机号位全部取 1 得到的地址是 202.197.15.255，这个地址即是这个网络的广播地址。 A、B、C 三类网络的网络地址和广播地址结构如下所示：

A 类主机号：网络地址为：x.0.0.0，广播地址为：x.255.255.255；　　

B 类主机号：网络地址为：x.x.0.0，广播地址为：x.x.255.255；　　

C 类主机号：网络地址为：x.x.x.0，广播地址为：x.x.x.255。

这也是为什么在计算表 10­1 中的网络最大主机数时，需要减 2 的原因 

受限广播地址 ：IP 地址 32 位全部为 1，即“255.255.255.255”，代表本地受限广播。该地址用于向本地网络中的所有主机发送广播消息 本网络上的特定主机 ，PS：广播地址本质上是个 E 类地址。 

本网络上的特定主机 ：当用户想与本网络内部特定主机通信时，可以将网络号对应字节全部设置为 0 进行简化。如当具有 B 类地址的某个主机发送数据包时，数据包中的目标 IP 地址为 0.0.11.32，则表示数据包要发送到网络中主机号为 11.32 的主机处。PS：该地址本质上是个 A 类地址。 

本网络本主机 ：IP 地址 32 位全部为 0，即“0.0.0.0”，表示本网络上的本主机。这个地址通常用某个主机启动时，需要通信，但暂时不知道自己的 IP 地址，此时主机为了获得一个有效 IP 地址，将发送一个数 据包给有限广播地址，并用全 0 地址来标志自己。接收方知道发送方还没有 IP 地址，就会采用一种特殊的方式来发送回答，这地址不应该作为目的地址使用。PS：该地址本质上是个 A 类地址。 

下面讲的这几个专用地址却可以被分配给多个主机，当然这些主机之间应该互不相关，即处于互相独立的专用网内，例如目前很流行的以太网局域网。在每一类地址中部分地址为专用地址，如表 10­2 所示。 

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_4_2.png">

#### 4.4 子网划分与子网掩码

现在流行一种扩展的分组编址方案来节省网络号的使用，这种方法称为子网编址，它是一种将网络进一步划分为子网的设计，允许多个物理网络共享一个网络前缀，但每个子网都有自己的子网地址。 

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_4_3.png">

划分子网有诸多的好处：第一，减少标准 IP 编址中地址浪费现象。 第二，节省通信流量。 第三，由于每个网段的主机数减少，使得主机的管理更加方便。

子网掩码，标准的 IP 地址，一眼就能够看出 IP 地址的种类（第一个字节就行）并获取其中的网络号，但对于子网编址，要想得到其网络号和子网号，就必须使用子网掩码。子网掩码的长度也是 32 位，左边是网络位（包括子网位），右边是主机位，所有网络位都用 1 表示，所有主机位都用 0 表示，这就形成了子网掩码。

#### 4.5 网络地址转换（NAT）

企业内部使用的局域网路由器都是具有NAT功能的，具有NAT功能的路由器至少要有一个内部端口和一个外部端口，内部端口是路由器为了与局域网内的用户通信而使用的，它使用一个内部专用IP地址，例如常见的路由器内部IP地址可以为`192.168.1.1`；外部端口是路由器用来与外部网络通信用的，它通常具有一个有效的IP地址，假设为一个有效的C类地址`222.197.179.21`。NAT功能可简单描述为：当内部网络用户连接互联网时，NAT将用户的内部IP地址转换成一个外部公共IP地址，反之，数据从外部返回时，NAT将目的地址替换成用户的内部IP地址。

假如我们的局域网用户（其使用的专用地址为 192.168.1.78）需要使用 TCP 协议与 Internet 中的一个 HTTP 服务器进行通信（IP 地址为 130.21.45.20，服务端口号为 80），则它将发送一个包含源 IP 地址、源端口号、目的 IP 地址、目的端口号的 IP 分组到路由器处，这里假设这四个值分别为：（192.168.1.78，1234，130.21.45.20，80）

NAT转换：（222.178.197.21，5678，130.21.45.20，80）
服务器返回：（130.21.45.20，80，222.178.197.21，5678）
NAT转换：（130.21.45.20，80，192.168.1.78，1234）

这样，经过 NAT 两次简单的转换，局域网用户就实现了与外部网络的数据包交互，上述整个过程对所有用户来说是透明的，但所有局域网用户能够通过同一个 IP 地址与外部进行通信。

#### 4.6 单播、多播与广播

多播：多播地址是 D 类地址，在 LwIP 中提供了一个宏来判断某个地址是否为多播地址， 通过判断它的最高四位是否为 1110（e）就能知道该地址是不是 D 类多播地址。

    ————ip_addr.h————————————————
    #define ip_addr_ismulticast(addr1)
    (((addr1)­>addr & ntohl(0xf0000000UL)) == ntohl(0xe0000000UL))
    ————————————————————————————

广播：在前面讲到广播分为直接广播和有限广播，在以太网中将这两者看作相同的方式，在 LwIP 中，判断一个数据报中的目的地址是否为广播地址的函数叫做ip_addr_isbroadcast

    ————ip_addr.c——————
    //两个特殊 IP 地址的定义
    #define IP_ADDR_ANY_VALUE 0x00000000UL //某些使用规范中，全 0 也代表所有主机
    #define IP_ADDR_BROADCAST_VALUE 0xffffffffUL //全 1，受限广播地址
    //函数功能：判断一个目的 IP 地址是否是广播地址
    //参数 netif：本地网络接口结构
    //返回值：是广播地址则返回非 0 值
    u8_t ip_addr_isbroadcast(struct ip_addr *addr, struct netif *netif)
    {
    　　u32_t addr2test;
    　　addr2test = addr­>addr; //取得 IP 地址结构中的 32 位整数
    　　if ((~addr2test == IP_ADDR_ANY_VALUE) || //如果 32 位为全 0 或者全 1
    　　　　(addr2test == IP_ADDR_ANY_VALUE)) //都是广播地址
    　　　　return 1; //返回非 0 值
    　　//判断是否为受限广播
    　　else if ((netif­>flags & NETIF_FLAG_BROADCAST) == 0) //如果网络接口不支持广播
    　　　　return 0; //则上层应用不应该出现广播地址，直接返回 0
    　　else if (addr2test == netif­>ip_addr.addr) //如果目的 IP 地址和本接口的 IP 地址一样
    　　　　return 0; //也不是广播地址
    　　//如果目的 IP 地址与本网络接口处于同一子网中，且其主机位全部为 0，则是受限广播
    　　else if (ip_addr_netcmp(addr, &(netif­>ip_addr), &(netif­>netmask)) //同一网段
    　　　　&& ((addr2test & ~netif­>netmask.addr) == //且主机位全部为 0
    　　　　(IP_ADDR_BROADCAST_VALUE & ~netif­>netmask.addr)))
    　　　　return 1; //返回非 0 值
    　　else
    　　return 0;
    }
    ————————————————————

单播：除了组播和广播，剩下的就是单播

# 2 数据报

#### 2.1 数据报组成结构

IP 数据报有自己的组织格式，如图 10­4 所示，它通常由两部分组成，即 IP 首部和数据。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_4_4.png">

第一个字段是 4bit 的版本号（VER），包含了创建数据报所使用的 IP 协议版本信息，例如对于 IPv4，该值为 4，对于 IPv6，该值为 6。

接下来的 4bit 字段用于记录首部长度，这个长度以字为单位。 （最大为15）

再下来是一个 8bit 的服务类型字段（Type Of Service，TOS），该字段主要用于描述当前 IP 数据报急需的服务类型，如最小延时、最大吞吐量、最高可靠性、最小费用等。路由器在转发数据报时，可以根据这个字段的值来为数据报选择最合理的路由路径。 

16 位的总长度字段描述了整个 IP 数据报（IP 首部和数据区）的总字节数（一般1500，成为MTU）。当一个很大的 IP 数据报需要发送时，IP 层首先要检查底层接口设备的 MTU，然 后将大的数据报划分为几个分片包，最后分别递交给底层发送；

接下来的三个字段：16 位标识字段、3 位标志和 13 位片偏移字段常在 IP 数据报分片时使用。

生存时间（TTL）字段描述该 IP 数据报最多能被转发的次数，每经过一次转发，该值会减 1，当该值为 0 时，路由器会丢弃掉分组，同时一个 ICMP 差错报文会被返回至源主机。

8 位协议字段和以太网数据帧中的协议类型字段功能相似，不过这里它用来描述该 IP 数据报中的数据是来自于哪个上层协议，例如，该值为 1 表示 ICMP 协议，为 2 表示 IGMP 协议，为 6 表示 TCP协议，为 17 表 UDP 协议。事实上，该字段的值也间接的指出了 IP 数据报的数据区域中数据的格式，因为每一种上层协议都使用了一种独立的数据格式。 

#### 2.2 数据结构

为了方便对 IP 数据报首部字段进行读取或写入操作，在 LwIP 中定义了一个名为 ip_hdr 的结构体来描述数据报首部，如下代码所示：

    ————ip.h——————————————
    PACK_STRUCT_BEGIN //禁止编译器自对齐
    struct ip_hdr {
    　　PACK_STRUCT_FIELD(u16_t _v_hl_tos); //前三个字段：版本号+首部长度+服务类型
    　　PACK_STRUCT_FIELD(u16_t _len); //总长度
    　　PACK_STRUCT_FIELD(u16_t _id); //标识字段
    　　PACK_STRUCT_FIELD(u16_t _offset); //3 位标志位和 13 位片偏移字段
    　　#define IP_RF 0x8000 //标志位第一位（保留位）掩码
    　　#define IP_DF 0x4000 //标志位第二位（不分片标志）掩码
    　　#define IP_MF 0x2000 //标志位第三位（更多分片位）掩码
    　　#define IP_OFFMASK 0x1fff //13 位片偏移字段的掩码
    　　PACK_STRUCT_FIELD(u16_t _ttl_proto); //TTL 字段+协议字段
    　　PACK_STRUCT_FIELD(u16_t _chksum); //首部校验和字段
    　　PACK_STRUCT_FIELD(struct ip_addr src); //源 IP 地址
    　　PACK_STRUCT_FIELD(struct ip_addr dest); //目的 IP 地址
    } PACK_STRUCT_STRUCT;
    PACK_STRUCT_END
    //下面定义了几个宏，用于对 IP 首部中各个字段值的读取，宏变量 hdr 为指向
    //IP 首部结构 ip_hdr 型变量的指针
    #define IPH_V(hdr) (ntohs((hdr)­>_v_hl_tos) >> 12) //获取版本号
    #define IPH_HL(hdr) ((ntohs((hdr)­>_v_hl_tos) >> 8) & 0x0f) //获取首部长度
    #define IPH_TOS(hdr) (ntohs((hdr)­>_v_hl_tos) & 0xff) //获取服务类型
    #define IPH_LEN(hdr) ((hdr)­>_len) //获取数据报总长度（网络字节序）
    #define IPH_ID(hdr) ((hdr)­>_id) //获取数据报标识字段（网络字节序）
    #define IPH_OFFSET(hdr) ((hdr)­>_offset) //获取标志位+片偏移字段（网络字节序）
    #define IPH_TTL(hdr) (ntohs((hdr)­>_ttl_proto) >> 8) //获取 TTL 字段
    #define IPH_PROTO(hdr) (ntohs((hdr)­>_ttl_proto) & 0xff) //获取协议字段
    #define IPH_CHKSUM(hdr) ((hdr)­>_chksum) //获取首部校验和字段（网络字节序）
    //下面定义几个宏，用于填写 IP 首部各个字段的值，宏变量 hdr 为指向
    //IP 首部结构 ip_hdr 型变量的指针
    #define IPH_VHLTOS_SET(hdr, v, hl, tos) (hdr)­>_v_hl_tos = //版本号+首部长度+服务类型
    (htons(((v) << 12) | ((hl) << 8) | (tos)))
    #define IPH_LEN_SET(hdr, len) (hdr)­>_len = (len) //数据报总长度（len 应为网络字节序）
    #define IPH_ID_SET(hdr, id) (hdr)­>_id = (id) //数据报标识字段（id 应为网络字节序）
    #define IPH_OFFSET_SET(hdr, off) (hdr)­>_offset = (off) //标志位+片偏移字段
    #define IPH_TTL_SET(hdr, ttl) (hdr)­>_ttl_proto = //TTL 字段
    (htons(IPH_PROTO(hdr) | ((u16_t)(ttl) << 8)))
    #define IPH_PROTO_SET(hdr, proto) (hdr)­>_ttl_proto = //协议字段
    (htons((proto) | (IPH_TTL(hdr) << 8)))
    #define IPH_CHKSUM_SET(hdr, chksum) (hdr)­>_chksum = (chksum) //首部校验和
    ————————————————————————————————————

# 3 IP层输出

#### 3.1 发送数据报

当传输层协议（TCP 或 UDP）要发送数据时，它们会将数据按照自己的格式组装在一个 pbuf中，并将 payload 指针指向协议首部，然后调用 IP 层的数据报发送函数 ip_output 发送数据。 然后调用函数 ip_output_if 将数据报发送出去。

    ————ip.c——————————————
    //函数功能：被传输层协议调用以发送数据报，该函数查找网络接口并调用函数
    // ip_output_if 完成最终的发送工作
    //参数 p：传输层协议需要发送的数据包 pbuf，payload 指针已指向协议首部
    //参数 src：源 IP 地址，若为 NULL，则使用网络接口结构中保存的 IP 地址
    //参数 dest：目的 IP 地址，若为 IP_HDRINCL，则表示 p 中已经有了填写好的 IP
    //数据报首部，且 payload 指针也已经指向了 IP 首部
    //参数 ttl、tos、proto：IP 首部中的 TTL 字段、服务类型和协议类型值
    err_t ip_output(struct pbuf *p, struct ip_addr *src, struct ip_addr *dest,u8_t ttl, u8_t tos, u8_t proto)
    {
    　　struct netif *netif;
    　　//根据目的 IP 地址为数据报寻找一个合适的网络接口
    　　if ((netif = ip_route(dest)) == NULL) 　　{ //若找到，变量 netif 指向对应的接口结构
    　　　　return ERR_RTE; //找不到，返回接口错误
    　　}
    　　//否则，增加 netif 为参数调用函数 ip_output_if
    　　return ip_output_if(p, src, dest, ttl, tos, proto, netif);
    }
    //函数功能：填写 IP 首部中的各个字段值（不处理选项字段），并发送数据报
    //参数说明：netif 为发送数据报的网络接口结构，其他参数与 ip_output 的相同
    err_t ip_output_if(struct pbuf *p, struct ip_addr *src, struct ip_addr *dest,u8_t ttl, u8_t tos, u8_t proto, struct netif *netif)
    {
    　　struct ip_hdr *iphdr;
    　　static u16_t ip_id = 0; //静态变量，记录 IP 数据报的编号（标识字段）
    　　if (dest != IP_HDRINCL) 　　{ //dest 不为 IP_HDRINCL，说明 pbuf 中未填写 IP 首部
    　　　　u16_t ip_hlen = IP_HLEN; //宏 IP_HLEN 为默认的 IP 首部长度，20
    　　　　if (pbuf_header(p, IP_HLEN)) 　　　　{ //移动 payload 指针，指向 pbuf 中的 IP 首部
    　　　　　　return ERR_BUF; //失败，返回 pbuf 空间错误
    　　　　}
    　　　　iphdr = p­>payload; //iphdr 指向数据报首部
    　　　　IPH_TTL_SET(iphdr, ttl); //填写 TTL 字段
    　　　　IPH_PROTO_SET(iphdr, proto); //填写协议字段
    　　　　ip_addr_set(&(iphdr­>dest), dest); //填写目的 IP 地址
    　　　　IPH_VHLTOS_SET(iphdr, 4, ip_hlen / 4, tos); //填写版本号+首部长度+服务类型
    　　　　IPH_LEN_SET(iphdr, htons(p­>tot_len)); //填写数据报总长度
    　　　　IPH_OFFSET_SET(iphdr, 0); //填写标志位和片偏移字段，都为 0
    　　　　IPH_ID_SET(iphdr, htons(ip_id)); //填写标识字段
    　　　　++ip_id; //数据报编号值加 1
    　　　　if (ip_addr_isany(src)) 　　　　{ //若 src 为空，则将源 IP 地址填写为网络接口的 IP 地址
    　　　　　　ip_addr_set(&(iphdr­>src), &(netif­>ip_addr));
    　　　　} else 　　　　{ //若 src 不为空，则直接填写源 IP 地址
    　　　　　　ip_addr_set(&(iphdr­>src), src);
    　　　　}
    　　　　IPH_CHKSUM_SET(iphdr, 0); //清 0 校验和字段
    　　　　IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, ip_hlen)); //计算并填写首部校验和
    　　} else 　　{ //如果 pbuf 中已经填写 IP 首部，这里不需要填写任何字段
    　　　　iphdr = p­>payload;
    　　　　dest = &(iphdr­>dest); //只是用变量 dest 记录下数据报中的目的 IP 地址
    　　}
    　　//下面开始发送 IP 数据报，分为三种情况来处理
    　　if (ip_addr_cmp(dest, &netif­>ip_addr)) 　　{ //如果目的 IP 地址是本网卡的地址，
    　　　　return netif_loop_output(netif, p, dest); //则调用环回输入，这个函数见第 8 章
    　　}
    　　if (netif­>mtu && (p­>tot_len > netif­>mtu)) 　　{ //如果数据报太大（大于接口的 mtu）
    　　　　return ip_frag(p,netif,dest); //则调用函数 ip_frag 对数据报分片发送
    　　}
    　　//剩下的情况，直接调用接口注册的 output 函数发送数据报
    　　return netif­>output(netif, p, dest);
    }
    —————————————————————————————————

函数 ip_output 中还出现了一个 ip_route 函数，它比较简单，功能是根据目的 IP 地址在系统的所有网络接口结构中选择一个最佳的网络接口返回，该网络接口将用于发送最终的数据报。

    ————ip.c——————————————————
    //函数功能：根据目的 IP 地址选择一个最合适的网络接口结构
    //参数 dest：目的 IP 地址
    //返回值：指向相应网络接口结构的指针
    struct netif * ip_route(struct ip_addr *dest)
    {
    　　struct netif *netif;
    　　for(netif = netif_list; netif != NULL; netif = netif­>next) 　　{//依次遍历接口结构链表
    　　　　if (netif_is_up(netif)) 　　　　{ //接口已经使能
    　　　　　　if (ip_addr_netcmp(dest, &(netif­>ip_addr), &(netif­>netmask))) 　　　　　　{//且与目的 IP 地址
    　　　　　　　　return netif; //主机处于同一子网内，则返回这个接口结构
    　　　　　　}
    　　　　}
    　　}//for
    　　//到这里，说明没找到合适的接口，我们把系统的默认网络接口作为结果返回
    　　if ((netif_default == NULL) || (!netif_is_up(netif_default))) 　　{//如果默认网络接口未配置或
    　　　　return NULL; //未使能，则返回空指针
    　　}
    　　return netif_default; //返回默认网络接口
    }
    ——————————————————————————————————

从上面的代码看出，所谓最佳的网络接口，就是与目的 IP 地址处于统一子网的网络接口，这一点的判断需要使用到子网掩码的概念。

#### 3.2 数据报分片

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_4_5.png">

| ------------ | 总长度（16位）| 标识（16位）| 标志（3位）| 分片偏移量（13位）|
| ------------ | ------------- | ----------  | ---------- | ----------------- |
| 原始数据报   |  3020         |    12345    | 0　        | 　　　0           |
| 分片1　　　　|  1420         |    12345    | 1          | 　　　0           |
| 分片2        |  1420         |    12345　　| 1          |　　　175（1400/8）|
| 分片3　　　　|  220　　　　　|    12345　　| 0　　　　　|　　　350（2800/8）|

* 16位标识字段：用于标识IP层发送出去的每一份IP数据报，每发送一份报文，则该值加1，数据包被分片时，该字段会被复制到每一个分片中。在接收端，会使用这个字段值来组装所有分片为一个完整的数据报。
* 3位标志字段：第一位保留；第二位是不分片位；第三位表示更多分片位，当该位被置1表示该分片不是最后一分片，当该为被置0表示该分片为最后一分片
* 13位分片偏移量：相对于原始数据报以8个字节为单位的偏移量

具体代码参考如下：

    ————ip_frag.c————————————————————
    //定义一个全局型的数组，数组大小为 IP 层允许的最大分片大小，每个分片会被先后
    //拷贝到这个数组中，然后发送
    #if IP_FRAG_USES_STATIC_BUF
    static u8_t buf[LWIP_MEM_ALIGN_SIZE(IP_FRAG_MAX_MTU + MEM_ALIGNMENT ­ 1)];
    #endif
    //函数功能：将数据报 p 进行分片发送，该函数在 ip_output_if 中被调用
    //参数 p：需要分片发送的数据报
    //参数 netif：发送数据报的网络接口结构指针
    //参数 dest：目的 IP 地址
    err_t ip_frag(struct pbuf *p, struct netif *netif, struct ip_addr *dest)
    {
    　　struct pbuf *rambuf;      //分片的 pbuf 结构
    　　struct pbuf *header; //以太网帧 pbuf
    　　struct ip_hdr *iphdr; //IP 首部指针
    　　u16_t nfb; //分片中允许的最大数据量
    　　u16_t left, cop; //待发送数据长度和当前发送的数据长度
    　　u16_t mtu = netif­>mtu; //网络接口 MTU
    　　u16_t ofo, omf; //分片偏移量和更多分片位
    　　u16_t last; //是否为最后一个分片
    　　u16_t poff = IP_HLEN; //发送的数据在原始数据报 pbuf 中的偏移量
    　　u16_t tmp;
    　　rambuf = pbuf_alloc(PBUF_LINK, 0, PBUF_REF); //为数据分片申请一个 pbuf 结构
    　　if (rambuf == NULL) { //申请失败，则返回
    　　　　return ERR_MEM;
    　　}
    　　rambuf­>tot_len = rambuf­>len = mtu; //设置 pbuf 的 len 和 tot_len 字段为接口的 MTU 值
    　　rambuf­>payload = LWIP_MEM_ALIGN((void *)buf); //payload 指向全局数据区域
    　　iphdr = rambuf­>payload; //得到分片包存储区域
    　　SMEMCPY(iphdr, p­>payload, IP_HLEN); //把原始数据报首部拷贝到分片的首部
    　　tmp = ntohs(IPH_OFFSET(iphdr)); //暂存分片的相关字段
    　　ofo = tmp & IP_OFFMASK; //得到分片偏移量（对原始数据报来说应该为 0）
    　　omf = tmp & IP_MF; //得到更多分片标志值
    　　left = p­>tot_len ­ IP_HLEN; //待发送数据长度（总长度­IP 首部长度）
    　　nfb = (mtu ­ IP_HLEN) / 8; //一个分片中可以存放的最大数据量（8 字节为单位）
    　　while (left) { //待发送数据长度大于 0
    　　　　last = (left <= mtu ­ IP_HLEN); //若待发送长度小于分片最大长度，last 为 1，否则为 0
    　　　　tmp = omf | (IP_OFFMASK & (ofo)); //计算分片相关字段
    　　　　if (!last) //如果不是最后一个分片
    　　　　　　tmp = tmp | IP_MF; //则更多分片位置 1
    　　　　//计算分片中的数据长度，当 last 为 1 时，说明分片能装下所有剩余数据
    　　　　cop = last ? left : nfb * 8;
    　　　　//从原始数据报中拷贝 cop 字节的数据到分片中，poff 记录了拷贝起始位置
    　　　　poff += pbuf_copy_partial(p, (u8_t*)iphdr + IP_HLEN, cop, poff);
    　　　　//填写分片首部中的其他字段
    　　　　IPH_OFFSET_SET(iphdr, htons(tmp)); //填写分片相关字段
    　　　　IPH_LEN_SET(iphdr, htons(cop + IP_HLEN)); //填写总长度
    　　　　IPH_CHKSUM_SET(iphdr, 0); //清 0 校验和
    　　　　IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN)); //计算校验和
    　　　　if (last) //若为最后一个分片，则更改 pbuf 的 len 和 tot_len 字段
    　　　　　　pbuf_realloc(rambuf, left + IP_HLEN);
    　　　　//到这里，我们组装好了一个完整的分片，需要将该分片发送出去，这里重新在
    　　　　//内存堆中开辟一个 pbuf 空间，用来保存以太网帧首部
    　　　　header = pbuf_alloc(PBUF_LINK, 0, PBUF_RAM);
    　　　　if (header != NULL) { //申请成功，则做发送工作
    　　　　　　pbuf_chain(header, rambuf); //将两个 pbuf 连接成一个 pbuf 链表
    　　　　　　netif­>output(netif, header, dest); //调用函数发送
    　　　　　　pbuf_free(header); //发送完成后，释放 pbuf 链表
    　　　　} else { //若以太网首部空间申请失败
    　　　　　　pbuf_free(rambuf); //释放分片空间
    　　　　　　return ERR_MEM; //返回内存错误
    　　　　}
    　　　　left ­= cop; //待发送的总长度减少
    　　　　ofo += nfb; //分片偏移量增加
    　　}//while
    　　pbuf_free(rambuf); //释放结构 rambuf
    　　return ERR_OK;
    }
    ——————————————————————————————————

# 4 IP层输入函数

#### 4.1 数据报接收

#### 4.2 分片重装数据结构

#### 4.3 分片重装函数

#### 4.4 分片插入与检查
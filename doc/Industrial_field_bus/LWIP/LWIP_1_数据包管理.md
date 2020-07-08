# 1 数据包管理

TCP/IP 是一种数据通信机制，因此，协议栈的实现本质上就是对数据包的传输进行管理。合理的数据包管理能够提供一种有效的机制，使使协议栈各层能对数据包进行灵活的处理，同时减少数据在各层间传递时的时间与空间开销，这是提高协议栈工作效率的关键点。在 LwIP 中，也有个类似的结构，称之为`pbuf`，本章所有讨论将围绕`pbuf`而展开。 

#### 1.1 数据包结构

在 LwIP 中，文件`pbuf.h`和`pbuf.c`实现了协议栈数据包管理相关的所有数据结构和函数。结构`pbuf`的定义如下：

    ————pbuf.h————————————————————
    struct pbuf{
        struct pbuf     *next;      // 构成 pbuf 链表时指向下一个 pbuf 结构
        void            *payload;   // 数据指针，指向该 pbuf 所记录的数据区域
        u16_t           tot_len;    // 当前 pbuf 及其后续所有 pbuf 中包含的数据总长度
        u16_t           len;        // 当前 pbuf 的数据长度 
        u8_t            type;       // 当前 pbuf 的类型
        u8_t            flags;      // 状态位，未用到
        u16_t           ref;        // 指向该 pbuf 的指针数，即该 pbuf 被引用的次数
    };
    ————————————————————————————————

`tot_len`表示当前`pbuf`和其后所有`pbuf`的有效数据的总长度。`pbuf`链表中第一个`pbuf`的`tot_len`字段表示整个数据包的长度，而最后一个`pbuf`的`tot_len`字段必同`len`字段相等 。

#### 1.2 pbuf类型

`pbuf`有4类：`PBUF_RAM`、`PBUF_ROM`、`PBUF_REF`、`PBUF_POOL`，使用了一个专门的枚举类型`pbuf_type`来描述它们：

    ————pbuf.h—————————————————————
    typedef enum {
       PBUF_RAM,        // pbuf 描述的数据在 pbuf 结构之后的连续内存堆中
       PBUF_ROM,        // pbuf 描述的数据在 ROM 中
       PBUF_REF,        // pbuf 描述的数据在 RAM 中，但位置与 pbuf 结构所处位置无关
       PBUF_POOL        // pbuf 结构与其描述的数据处于同一内存池中
    } pbuf_type;
    ————————————————————————————————

1. `PBUF_RAM`类型`pbuf`

    这种类型的`pbuf`是通过内存堆分配得到的。这种类型的`pbuf`在协议栈中是使用得最多的，协议栈的待发送数据和应用程序的待发送数据一般都采用这个形式。下面来看看源代码是怎样申请`PBUF_RAM`型的，在后续讲解函数源代码时也会详细说到。

        p = (struct pbuf*)mem_malloc(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF + offset) + LWIP_MEM_ALIGN_SIZE(length));

    分配成功的`PBUF_RAM`类型`pbuf`如图1­1所示。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_1_1.png">

    从图中可看出 pbuf 结构和相应数据在一片连续的内存区域中，注意`payload`并没有指向整个数据区的起始处，而是间隔了一定区域。这段区域就是上面的`offset`，它通常用来存储数据包的各种首部字段，如 `TCP报文首部`、`IP首部`、`以太网帧首部`等。 

2. `PBUF_POOL`类型`pbuf`

    `PBUF_POOL`类型和`PBUF_RAM`类型的`pbuf`有很大的相似之处，但它的空间是通过内存池分配得到的。这种类型的`pbuf`可以在极短的时间内得到分配（得益于内存池的优点），在网卡接收数据包时，我们就使用了这种方式包装数据。在申请`PBUF_POOL` 类型`pbuf`时，协议栈会在内存池`MEMP_PBUF_POOL`中选择一个或多个`POOL`，以满足用户空间大小的申请。源代码是通过下面一条语句来完成`POOL`申请的，其中`p`是`pbuf`型指针。

        p = memp_malloc(MEMP_PBUF_POOL);

    通常，用户发送的数据可能很长，所以系统会多次调用上面的语句，为用户分配多个`POOL`，并把它们按照`pbuf` 链表的形式组织在一起，以保证用户的空间请求要求。分配成功的`PBUF_POOL`类型`pbuf`示意如图1­2所示。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/LWIP_1_2.png">

3. `PBUF_ROM`和`PBUF_REF`类型的`pbuf`

    `PBUF_ROM`和`PBUF_REF`类型的`pbuf`基本相同，它们的申请都是在内存池中分配一个相应的`pbuf`结构（即`MEMP_PBUF`类型的 POOL），而不申请数据区的空间。在发送某些静态数据时，可以采用这两种类型的`pbuf`，这将大大节省协议栈的内存空间。下面来看看源代码是怎样申请`PBUF_ROM`和`PBUF_REF`类型的，其中，`p`是`pbuf`型指针。

#### 1.3 数据包申请函数

数据包申请函数有两个重要的参数，一是想申请的数据包 pbuf 类型，这个刚说过了，不啰嗦；另一个重要参数是该数据包是在协议栈中哪一层被申请的，分配函数会根据这个层次的不同，在 pbuf 数据区域前为相应的协议预留出首部空间，这就是前面所说的 offset 值了。总的来说，LwIP 定义了四个层次，当数据包申请时，所处的层次不同，会导致预留空间的 offset值不同。层次的定义是通过一个枚举类型 pbuf_layer 来实现的，如下代码所示：

    ————pbuf.h——————————————————————
    #define PBUF_TRANSPORT_HLEN     20      //TCP 报文首部长度
    #define PBUF_IP_HLEN            20      //IP 数据报首部长度
    typedef enum {
    　　PBUF_TRANSPORT,                       //传输层
    　　PBUF_IP,                              //网络层
    　　PBUF_LINK,                            //链路层
    　　PBUF_RAW                              //原始层，不预留任何空间
    } pbuf_layer;
    ————————————————————————————————

`PBUF_TRANSPORT_HLEN`和`PBUF_IP_HLEN`，前者是典型的TCP报文首部长度，而后者是典型的不带任何选项字段的IP首部长度。

请求pbuf的函数原型声明，如下所示：

    struct pbuf *pbuf_alloc(pbuf_layer  layer,  /* 该数据pbuf所处的协议栈层 */
                            u16_t       length, /* 表示需要申请的数据区长度 */
                            pbuf_type   type)   /* 要申请的pbuf类型 */

使用方式，如下所示：

    p = pbuf_alloc(PBUF_RAW, packetLength, PBUF_POOL);

这个调用语句申请了一个`PBUF_POOL`类型的`pbuf`，且其申请的协议层为`PBUF_RAW`，所以`pbuf_alloc`函数不会在数据区前预留出任何首部空间; 通过使用 `p­>payload`，就可以实现对 `pbuf` 中数据区的读取或写入操作了。

在 TCP 层要申请一个数据包时，常常调用下面的语句：

    p = pbuf_alloc(PBUF_TRANSPORT, optlen, PBUF_RAM)

它告诉数据包分配函数，使用`PBUF_RAM`类型的`pbuf`，且数据区前应该预留一部分的首部空间，由于这里是 `PBUF_TRANSPORT` 层，所以预留空间将有54字节，即TCP首部长度`PBUF_TRANSPORT_HLEN`（20 字节）、IP数据包首部长度 `PBUF_IP_HLEN`（20 字节）以及以太网帧首部长度（14 字节）。当数据包往下层递交，各层协议就可以直接操作这些预留空间中的数据，以实现数据包首部的填写，这样就避免了数据的拷贝。

#### 1.4 数据包释放函数

假如现在我们的 pbuf 链表由 A，B，C 三个`pbuf`结构连接起来，结构为 `A->B->C`，利用` pbuf_free(A)`函数来删除 pbuf 结构，下面用 ABC 的几组不同ref值来看看删除结果：

1. 1­>2­>3 函数执行后变为 0->1­>3，节点 BC 仍在；
2. 3­>3­>3 函数执行后变为 2­>3­>3，节点 ABC仍在；
3. 1­>1­>2 函数执行后变为......1，节点 C 仍在；
4. 2­>1­>1 函数执行后变为 1­>1­>1，节点 ABC 仍在；
5. 1­>1­>1 函数执行后变为.......，节点全部被删除。

> 数字代表该结构被引用的次数，也就是ref的值

假如在上面的第4种情况下，错误的调用数据包释放函数，如 pbuf_free(B)，这会导致严重的错误。

函数原型为：

    u8_t pbuf_free(struct pbuf *p)

当可以删除某个 `pbuf`结构时，函数 `pbuf_free` 首先检查这个 `pbuf` 是属于四个类型中的哪种，根据类型的不同，调用不同的 内存释放函数进行删除。

1. 内存池

        memp_free->MEMP_PBUF_POLL->PBUF_POOL
                                        ->PBUF_ROM
                                        ->PBUF_REF

2. 内存堆

        mem_free->PBUF_RAM

#### 1.5 其它数据包操作函数

`pbuf_realloc`函数在相应 pbuf（链表）尾部释放一定的空间，将数据包 pbuf 中的数据长度减少为某个长度值。对于 PBUF_RAM 类型的 pbuf，函数将调用内存堆管理中介绍到的 mem_realloc 函数，释放这些多余的空间；对于其他三种类型的 pbuf，该函数只是修改 pbuf 中的长度字段值，并不释放对应的内存池空间。

`pbuf_header` 函数用于调整 pbuf 的 payload 指针（向前或向后移动一定的字节数），在前面也说到过了，在 pbuf 的数据区前可能会预留一些协议首部空间，而 pbuf 被创建时，payload 指针是指向数据区的，为了实现对这些预留空间的操作，可以调用函数 `pbuf_header` 使 payload 指针指向数据区前的首部字段，这就为各层对数据包首部的操作提供了方便。当然，进行这个操作的时候，len和 tot_len 字段值也会随之更新。

`pbuf_take` 函数用于向 pbuf 的数据区域拷贝数据；

`pbuf_copy` 函数用于将一个任何类型的 pbuf中的数据拷贝到一个 PBUF_RAM 类型的 pbuf 中。

`pbuf_chain` 函数用于连接两个 pbuf（链表）为一个 pbuf 链表；

`pbuf_ref` 函数用于将 pbuf 中的 ref 值加 1。
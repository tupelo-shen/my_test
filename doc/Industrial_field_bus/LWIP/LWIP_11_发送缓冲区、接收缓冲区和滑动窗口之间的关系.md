大家知道，TCP采用的是全双工的工作模式，对每一个TCP的SOCKET来说，都有一个发送缓冲区和接收缓冲区与之对应，TCP的流量(拥塞)控制便是依赖于这两个独立的buffer滑动窗口协议之间。我们可以通过一个简单实验体会三者之间关系。

# 1 实验方法

## 1.1 recv端

在在监听套接字上准备accept，在accept结束以后不做什么操作，直接sleep很久，也就是在recv端并不做接收数据的操作，在sleep结束之后再recv数据。

## 1.2 send端

将套接字设置为阻塞，一次发送的buffer大于发送缓冲区所能容纳的数据量。通过查看系统内核默认的支持的发送缓冲区大小，`cat /proc/sys/net/ipv4/tcp_wmem`，最后一个参数为发送缓冲区的最大值。接收缓冲区的配置文件在tcp_rmen中。

# 2 TCP/IP知识点

## 2.1 TCP窗口扩大选项-Window Scale

窗口扩大选项使TCP的窗口定义从16bit增加到32bit。这并不是通过修改TCP首部来实现的，TCP首部仍然使用16bit，而是通过定义一个选项实现对16bit的扩大操作来完成的。于是TCP在内部将实际的窗口大小维持为32bit的值；

窗口扩大选项的取值范围为0<=shif.cnt<=14；最大值为14的原因如下：

【这段内容为本人理解，非书中原文】TCP需要保留一半的序号用于判断是否是以前的旧数据段，(2^32-1)/2<2^31，也就是现在用于处理新数据段小于2^31个；假如窗口大小为wnd，发送方第一次发送[0，wnd-1]一个窗口的数据，这时接收方收到数据，并统一发送一个ack确认这个窗口的数据，确认完之后，窗口移动准备接收[wnd, 2wnd-1]段的数据；若发送方如果没有收到这个ack，需要重传[0, wnd-1]的数据，若收到了ack，则发送[wnd, 2wnd-1]的数据，这两种情况下，接收方都需要满足序号，也就是序号需要满足2*wnd；前面说了序号的一半用于新数据，也就是2*wnd<2^31，wnd<2^30，即(2^16-1)*2^shif < 2^30，得出shift<=14，即除了TCP首部的16bit，这里最多也就是30-16=14bit；

【这段是找到的RFC原文】大概意思是发送和接收窗口，一个窗口的左边沿到另外一个窗口的右边沿最大不能超过2^31，如果超过这个值就与保留的旧序号重叠了，也就是2*wnd < 2^31，wnd  < 2^30，得出shift<=14；

    TCP determines if a data segment is "old" or "new" by testing whether its sequence number is within 2**31 bytes of
    the left edge of the window, and if it is not, discarding the data as "old".  To insure that new data is never
    mistakenly considered old and vice-versa, the left edge of the sender's window has to be at most 2**31 away from
    the right edge of the receiver's window. Similarly with the sender's right edge and receiver's left edge. Since
    the right and left edges of either the sender's or receiver's window differ by the window size, and since the
    sender and receiver windows can be out of phase by at most the window size, the above constraints imply that 2 *
    the max window size must be less than 2**31, or

        max window < 2**30
       
    Since the max window is 2**S (where S is the scaling shift count) times at most 2**16 - 1 (the maximum unscaled
    window), the maximum window is guaranteed to be < 2*30 if S <= 14.  Thus, the shift count must be limited to 14 (
    which allows windows of 2**30 = 1 Gbyte).  If a Window Scale option is received with a shift.cnt value exceeding
    14, the TCP should log the error but use 14 instead of the specified value.

这个选项只能出现在SYN报文段中，因此当连接建立起来后，每个方向上的扩大因子是固定的。为了使用窗口扩大，两端必须在他们的SYN报文段中发送该选项。主动建立连接的一方在其SYN中发送这个选项，但是被动建立连接的一方只能够在接收到带这个选项的SYN之后才可以发送这个选项。每个方向上的扩大因此可以不同；

如果主动连接的一方发送一个非零的扩大因子，但是没有从另一端收到一个扩大选项，它就将发送和接收的移位计数器设置为0。这就允许较新的系统能够与较旧的系统进行互相操作；

假定我们正在使用窗口扩大选项，发送移位计数为S，而接收到的移位计数为R。于是我们从另一端接收到的16bit的通告窗口将被左移R位以获得实际的通告窗口大小。每次当我们向对方发送一个窗口通告的时候，我们将实际的32bit窗口大小右移S位，然后用它来替换TCP首部中的16bit窗口值；

TCP根据接收缓存的大小自动选择位移技术。这个大小是由系统设置的，但是通常向应用程序提供了修改途径；

# 3 相关配置

## 3.1 TCP_MSS

TCP最大的segment大小。默认是536，这是为了节省空间，可以增加该值，扩大发送速率。当建立连接时（也就是TCP三次握手协议），MSS大小会被告知远端的接收主机。其实，就是给远端主机设置了一个MSS的上限值。

## 3.2 TCP_SND_BUF

TCP发送缓存大小（用来保存单个数据包），单位是字节。为了达到比较好的性能，这个值应该至少是`2*TCP_MSS`大小。

## 3.3 TCP_SND_QUEUELEN

TCP发送缓存队列个数，也就是可以缓存的pbuf个数。为了达到较好性能，这个值至少是`2*TCP_SND_BUF/TCP_MSS`。

## 3.4 TCP_SNDLOWAT

TCP可写的内存空间大小（字节）。这个值必须小于`TCP_SND_BUF`。这个值定义了TCP发送缓存中还可以使用的空间。一般和`TCP_SNDQUEUELOWAT`组合使用。

## 3.5 TCP_SNDQUEUELOWAT

TCP可写的发送队列个数（pbuf数量）。必须小于`TCP_SND_QUEUELEN`。如果某个tcp上缓存的pbuf数量小于该值，那么返回可写。

## 3.6 TCP_OVERSIZE

The maximum number of bytes that tcp_write may allocate ahead of time in an attempt to create shorter pbuf chains for transmission. The meaningful range is 0 to TCP_MSS. Some suggested values are:

1. 0 - Disable oversized allocation. Each tcp_write() allocates a new pbuf (old behaviour).
2. 1 - Allocate size-aligned pbufs with minimal excess. Use this if your scatter-gather DMA requires aligned fragments.
3. 128 - Limit the pbuf/memory overhead to 20%.
4. TCP_MSS - Try to create unfragmented TCP packets.
5. TCP_MSS/4 - Try to create 4 fragments or less per TCP packet.

## 3.7 MEMP_NUM_PBUF

保存要发送的数据。如果用户想要一次发送特别大的数据包时，必须扩大这个值。

## 3.8 MEMP_NUM_TCP_SEG

如果你要发送超过`TCP_MSS`大小的数据包，LwIP协议栈会自动将其拆成segment。而在TCP发送缓冲区中保存这些segments，所以为了发送大数据包，`MEMP_NUM_TCP_SEG`的大小应该大于等于`TCP_SND_QUEUELEN`。

# 4 需要修改的数据结构

# 4.1 struct lwip_sock

`struct lwip_sock`结构具体定义如下所示：

    /** 记录一个socket使用的数据和状态 */
    struct lwip_sock {
        struct netconn *conn;   // 每个socket对应一个netconn
        void *lastdata;         // 指向上次读完后剩余的数据
        #if 0 /* 原有代码 */
        u16_t lastoffset;       // 上次读完之后剩余数据的偏移量结合上面的
        #else /* 修改代码 */    // 数据指针，就可以记录所有的接收数据。
        u32_t lastoffset;       // 如果想要缓存大于65535的数据，必须修改
        #endif /* 修改代码 */   // 这个变量的类型，以支持更大的计数

        s16_t rcvevent;         // 记录接收数据的次数，由event_callback()
                                // 函数进行设置，receive和select之类的函数判断
        u16_t sendevent;        // 数据被应答的数量，由event_callback()设置
                                // select判断
        u16_t errevent;         // 记录这个socket发生的错误次数，由event_callback()设置
                                // select判断
        int err;                // 这个socket发生的最新错误号
        int select_waiting;     // 判断等待这个socket的线程数量，select调用
    };

# 4.2 struct netconn

`struct netconn`结构具体定义如下所示：

    struct netconn {
        // ...省略
        union {
            struct ip_pcb  *ip;
            struct tcp_pcb *tcp;
            struct udp_pcb *udp;
            struct raw_pcb *raw;
        } pcb;
        
        // ...省略

    #if LWIP_SO_RCVBUF
        #if 0 /* 原有代码 */
        int recv_bufsize;       // 在接收mbox中等待的最大字节数，TCP不用；
                                // 如果调整的话，按照TCP_WND倍数调整
        s16_t recv_avail;       // 在recvmbox中收到的字节数，用于判断
                                // recv_bufsize中还有多少个可用的字节数
                                // 用于UDP和RAW接收数据时，非阻塞读取
        #else /* 修改代码 */
        u32_t recv_bufsize;     // 为了缓存大于64K的数据，修改数据类型
        u32_t recv_avail;       // 为了缓存大于64K的数据，修改数据类型
        #endif /* 修改代码 */
    #endif /* LWIP_SO_RCVBUF */
        
        // ...省略
    };

# 4.3 struct tcp_pcb

`struct tcp_pcb`结构具体定义如下所示：

    struct tcp_pcb {
        // ... 省略
        #if 0 /* 原有代码 */
        u16_t snd_buf;          // 可用的发送缓存，单位字节
    #define TCP_SNDQUEUELEN_OVERFLOW (0xffffU-3)
        u16_t snd_queuelen;     // 在可用的发送缓存能够缓存的队列个数，单位tcp_seg
        #else /* 修改代码 */
        u32_t snd_buf;          // 为了可以发送大的数据包，比如64K，1M等，需要扩展
    #define TCP_SNDQUEUELEN_OVERFLOW (0xffffU-3)
        u16_t snd_queuelen;     // 在可用的发送缓存能够缓存的队列个数，单位tcp_seg
      #endif /* 修改代码 */

        // ... 省略
    };

上面的两个结构体，都可以由`lwip_getsockopt`函数进行设置。当然，如果`lwip_getsockopt`函数中没有实现的话，请自行实现。下面提供一段参考代码：

    // ... 省略
    #if LWIP_SO_RCVBUF
        case SO_RCVBUF:
          netconn_set_recvbufsize(sock->conn, *(int*)optval);
          break;
    #endif /* LWIP_SO_RCVBUF */
    #if LWIP_SO_SNDBUF        /* shenwanjiang modified for bug718(2020-08-10) */
        case SO_SNDBUF:
          { 
            if (sock->conn->pcb.tcp == NULL) {
              printf("netconn->pcb.tcp is NULL in the function %s\n", __FUNCTION__);
            }
            if ( *(int*)optval <= TCP_SND_BUF_MAX )
            {
              int temp_optval=*(int*)optval;
              if ( (temp_optval*2)< 2*TCP_MSS ) temp_optval=2*TCP_MSS;                  // the minsize
              else if ((temp_optval*2)>= TCP_SND_BUF_MAX ) temp_optval=TCP_SND_BUF_MAX; // the maxsize
              else temp_optval=temp_optval*2;                                           // set snd_buffer

              sock->conn->pcb.tcp->snd_buf=temp_optval;

              /* set snd_queuelen size */
              if (((4*(sock->conn->pcb.tcp->snd_buf)+(TCP_MSS - 1))/(TCP_MSS)) > TCP_SNDQUEUELEN_OVERFLOW)
              {
                sock->conn->pcb.tcp->snd_queuelen = TCP_SNDQUEUELEN_OVERFLOW;
              }
              else if (((4*(sock->conn->pcb.tcp->snd_buf)+(TCP_MSS - 1))/(TCP_MSS)) < 8)
              {
                sock->conn->pcb.tcp->snd_queuelen = 8;
              }
              else 
              {
                sock->conn->pcb.tcp->snd_queuelen = ((4 * (sock->conn->pcb.tcp->snd_buf) + (TCP_MSS - 1))/(TCP_MSS));
              }
            }
          }
        break;
    #endif /* LWIP_SO_SNDBUF */
    // ... 省略

# 5 相关APIs

## 5.1 发送TCP数据

TCP数据的发送，主要由`tcp_write()`函数完成，所以如果想要发送比64K大的数据包的话，必须修改这个函数。这个函数主要完成的工作就是把要发送的数据包拆成tcp_seg，然后加入队列中。具体的发送工作交给tcpip_thread完成。当数据成功发送到目的主机后，会调用一个特定的回调函数通知应用程序。

函数原型如下：

    err_t tcp_write(struct tcp_pcb *pcb, const void *dataptr, u16_t len, u8_t apiflags)

参数说明：

1. pcb

    tcp控制块。

2. dataptr

    待加入发送队列数据的指针。

3. len

    待加入发送队列数据的长度。

4. apiflags - 标志位

    * TCP_WRITE_FLAG_COPY - indicates whether the new memory should be allocated for the data to be copied into. If this flag is not given, no new memory should be allocated and the data should only be referenced by pointer. This also means that the memory behind dataptr must not change until the data is ACKed by the remote host.
    * TCP_WRITE_FLAG_MORE: indicates that more data follows. If this is given, the PSH flag is set in the last segment created by this call to tcp_write. If this flag is not given, the PSH flag is not set.

The tcp_write() function will fail and return ERR_MEM if the length of the data exceeds the current send buffer size or if the length of the queue of outgoing segment is larger than the upper limit defined in lwipopts.h. The number of bytes available in the output queue can be retrieved with the tcp_sndbuf() function.

The proper way to use this function is to call the function with at most tcp_sndbuf() bytes of data. If the function returns ERR_MEM, the application should wait until some of the currently enqueued data has been successfully received by the other host and try again.

## 5.2 发送UDP数据

函数原型如下：

    int lwip_sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen)

因为项目没有需要，暂时可以不修改。

## 5.3 接收UDP数据

对于想要接收UDP大数据包时需要修改的函数，如下所示：

    int lwip_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen)
    {
        // 此处省略 ...
        #if 0 /* 原有代码 */
        u16_t            buflen, copylen;
        #else /* 修改代码 */
        u32_t            buflen, copylen; 
        #endif /* 修改代码 */

        // 此处省略 ...
    }

    static void recv_udp(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port)
    {
        struct netbuf *buf;
        struct netconn *conn;
        u16_t len;
    #if LWIP_SO_RCVBUF
        #if 0   /* old code */
        int recv_avail;
        #else   /* new code */
        u32_t recv_avail; /* modified by shenwanjiang for bug699(1M of buffers) on 2020-09-01 */
        #endif  /* new code */
    #endif /* LWIP_SO_RCVBUF */
        // 此处省略 ...
    }

上面两个函数体内，需要修改与修改的变量相关的内容。
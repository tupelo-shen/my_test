[TOC]

## 1 CPU访问外设的方式

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/net_hardware_cpu_access_io_dma.png">

## 2 CPU访问网卡的方式

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/net_hardware_cpu_access_netcard_dma.png">

## 3 网络收包原理

网络收包大致分为三种情况：

* `poll`方式

    常见于小型嵌入式设备的裸机程序。通常是一个while之类的大循环，定时查询数据包的接收情况实现。应用场景是对于网络通信的吞吐量的需求不那么迫切的情况下。

* `中断`方式

    而中断的方式是：MAC每收到一个以太网数据包就会产生一个接收中断给CPU，继而交给中断处理程序进行数据包的处理。

* `NAPI`方式

    `NAPI`采用`中断+轮询`的方式。MAC收到一个数据包后产生中断，进入中断处理程序。但是，在处理数据包之前，会先关闭接收中断。然后，一直接收数据，直到收够了N个包（Linux使用netdev_max_backlog参数表示N，默认是300）或者收完MAC上所有包之后，再打开接收中断，退出中断处理程序。

> 轮询的方式，不会增加CPU的负荷，但是实时性太差。如果网络数据量一旦增大，很容易丢失数据。而中断的方式极大地提高了数据的响应实时性。但是，也引入了新的问题。如果网络流量很大的时候，频繁地切换中断上下文，尤其是带有操作系统的情况下，极大地增加了CPU的额外负荷。而NAPI则是博众家之所长，将网络性能最大化的最佳方案。

> `NAPI`是不是很难理解。其实，没有什么特殊的意义，就是`New API`的意思。也许是当时的作者词穷了吧。

## 4 NAPI相关数据结构

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/net_hardware_NAPI_data_struct.png">

> 每个网络设备（上图中，有两个网络设备：MAC控制器和PCI网卡）都有自己描述设备的数据结构`net_device`，这个结构中有个成员是`napi_struct`，专门用来保存NAPI相关的数据。每当收到数据包时，网络设备驱动就会把自己的`napi_struct`挂到CPU私有变量上。当进入软中断时，`net_rx_action()`会遍历CPU私有变量的`poll_list`列表，找到并执行对应的`napi_struct`结构中的`poll`回调函数，将数据包从驱动传递给网络协议栈的接口。

## 5 内核启动时的网络初始化

内核启动时，需要完成网络相关的全局数据结构，并绑定处理网络相关的软中断的回调函数。

网络初始化的调用过程为

```c
start_kernel() 
    --> rest_init() 
        --> do_basic_setup()
            --> do_initcall()
                --> net_dev_init()
```

那么，我们重点关注的是`net_dev_init`函数

```c
__init net_dev_init()
{
    // 每个CPU都有一个CPU私有变量
    // _get_cpu_var(softnet_data)
    // poll_list很重要，软中断中需要遍历它
    for_each_possible_cpu(i) {
        struct softnet_data *queue;
        queue = &per_cpu(softnet_data, i);
        skb_queue_head_init(&queue->input_pkt_queue);
        queue->completion_queue = NULL;
        // 初始化poll_list列表的链表结构
        INIT_LIST_HEAD(&queue->poll_list);
        queue->backlog.poll = process_backlog;
        queue->backlog.weight = weight_p;
    }
    // 绑定软中断与网络发送处理函数
    open_softirq(NET_TX_SOFTIRQ, net_tx_action, NULL);
    // 绑定软中断与网络接收处理函数
    open_softirq(NET_RX_SOFTIRQ, net_rx_action, NULL);
}
```

## 6 加载网络设备驱动程序

加载网络设备驱动的过程，其实就是为每个网络设备创建对应的数据结构`net_device`，并让内核知道其存在的过程。我们知道，不管是CPU继承的MAC控制器还是外挂的PCI网卡一般都通过PCI总线与CPU相连。
所以，我们需要按照PCI驱动的数据结构定义一个数据结构体，在此，我们命名为`xxx_driver`。但是，现在新版本内核一般都使用`platform`虚拟的总线设备驱动完成硬件设备驱动的处理。

为了描述简单，我们简化掉对于我们理解网络驱动加载不太重要的内容：

```c
static struct platform_driver xxx_driver = {
    ...
    .probe    = xxx_probe,
    .remove   = xxx_remove,
    // 下面3个成员一般用于电源管理
    .driver = {
        .name = "设备名称字符串",
    },
    ...
};
```

PCI总线设备驱动一旦探测到设备节点存在网络设备，会调用这个probe函数，完成对应网络设备的驱动程序注册。我们来看一下这个函数的实现：

```c
int xxx_probe(struct platform_device *pdev)
{
    /* 创建net_device数据结构 */
    dev = alloc_etherdev(sizeof (*priv)); 

    dev->open = xxx_enet_open;
    dev->hard_start_xmit = xxx_start_xmit;
    dev->tx_timeout = xxx_timeout;
    dev->watchdog_timeo = TX_TIMEOUT;
#ifdef CONFIG_XXX_NAPI
    /* 软中断中会调用poll回调函数*/
    netif_napi_add(dev, &priv->napi,xxx_poll,XXX_DEV_WEIGHT); 
#endif
#ifdef CONFIG_NET_POLL_CONTROLLER
    dev->poll_controller = xxx_netpoll;
#endif
    dev->stop = xxx_close;
    dev->change_mtu = xxx_change_mtu;
    dev->mtu = 1500;
    dev->set_multicast_list = xxx_set_multi;
    dev->set_mac_address = xxx_set_mac_address;
    dev->ethtool_ops = &xxx_ethtool_ops;
}
```

## 7 启动网络设备

用户态程序调用ifconfig等工具命令时，发生的调用过程如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/net_hardware_net_system_call_process.png">

> 此处的`dev_open`回调函数就是前面注册的`xxx_enet_open`函数。

## 8 网络设备的open操作

那么，网络设备的open回调函数到底都做了什么呢？

分配收发缓冲区描述符，配置中断ISR（包括rx、tx、err），对于MAC控制器来说，

## 9 数据链路层接收数据包的过程

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/LWIP/images/net_hardware_data_link_layer_process.png">

> 系统启动时，网卡控制器（NIC）进行初始化。向内核的内存地址空间申请内存给`Ring Buffer`。`Ring Buffer`中的每个元素（数据包描述符）都指向一个`sk_buff`。准备就绪，等待数据的传送。
> 
> 上图描述了数据链路层接收数据包，到传递给网络协议栈的过程：
> 
> ①. 网卡控制器收到数据，DMA搬运到`Ring Buffer`对应的一个`sk_buff`中，产生Rx中断。
> 
> ②. 产生Rx中断后，IRQ传递给CPU，触发相应的中断处理程序。
> 
> ③. 中断处理程序中：如果选择`NAPI`方式，则关闭中断，连续接收数据包。满足条件后，触发软中断SoftIRQ，并将相应的网络设备数据结构挂载到`NAPI`的poll列表中，等待软中断处理；如果选择非`NAPI`方式，


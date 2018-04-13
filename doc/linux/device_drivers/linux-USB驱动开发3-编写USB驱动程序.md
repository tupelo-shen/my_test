前面学习了USB驱动的一些基础概念与重要的数据结构，那么究竟如何编写一个USB 驱动程序呢？编写与一个USB设备驱动程序的方法和其他总线驱动方式类似，驱动程序把驱动程序对象注册到USB子系统中，稍后再使用制造商和设备标识来判断是否安装了硬件。当然，这些制造商和设备标识需要我们编写进USB 驱动程序中。

USB 驱动程序依然遵循设备模型 —— 总线、设备、驱动。和I2C总线设备驱动编写一样，所有的USB驱动程序都必须创建的主要结构体是 struct usb_driver，它们向USB 核心代码描述了USB 驱动程序。但这是个外壳，只是实现设备和总线的挂接，具体的USB 设备是什么样的，如何实现的，比如一个字符设备，我们还需填写相应的文件操作接口 ，下面我们从外到里进行剖析，学习如何搭建这样的一个USB驱动外壳框架：

一、注册USB驱动程序
================
Linux的设备驱动，特别是这种hotplug的USB设备驱动，会被编译成模块，然后在需要时挂在到内核。所以USB驱动和注册与正常的模块注册、卸载是一样的，下面是USB驱动的注册与卸载：
```
static int __init usb_skel_init(void)     
{     
     int result;     
     /* register this driver with the USB subsystem */     
     result = usb_register(&skel_driver);     
     if (result)     
         err("usb_register failed. Error number %d", result);     
    
     return result;     
}     
    
static void __exit usb_skel_exit(void)     
{     
     /* deregister this driver with the USB subsystem */     
     usb_deregister(&skel_driver);     
}     
    
module_init (usb_skel_init);     
module_exit (usb_skel_exit);     
MODULE_LICENSE("GPL");  
```
USB设备驱动的模块加载函数通用的方法是在I2C设备驱动的模块加载函数中使用usb_register（struct *usb_driver）函数添加usb_driver的工作,而在模块卸载函数中利用usb_deregister（struct *usb_driver）做相反的工作。 对比I2C设备驱动中的 i2c_add_driver(&i2c_driver)与i2c_del_driver(&i2c_driver)。

struct usb_driver是USB设备驱动,我们需要实现其成员函数：
```
static struct usb_driver skel_driver = {     
     .owner = THIS_MODULE,      
     .name = "skeleton",    
     .id_table = skel_table,         
     .probe = skel_probe,       
     .disconnect = skel_disconnect,       
};  
```
从代码看来，usb_driver需要初始化五个字段：
- 模块的所有者 THIS_MODULE
- 模块的名字  skeleton
- probe函数   skel_probe
- disconnect函数skel_disconnect
- id_table

最重要的当然是probe函数与disconnect函数,这个在后面详细介绍，先谈一下id_table：
id_table 是struct usb_device_id 类型，包含了一列该驱动程序可以支持的所有不同类型的USB设备。如果没有设置该变量，USB驱动程序中的探测回调该函数将不会被调用。对比I2C中struct i2c_device_id *id_table，一个驱动程序可以对应多个设备，i2c 示例：
```
static const struct i2c_device_id mpu6050_id[] = {      
    { "mpu6050", 0},      
    {}      
}; 
```
usb子系统通过设备的production ID和vendor ID的组合或者设备的class、subclass跟protocol的组合来识别设备，并调用相关的驱动程序作处理。我们可以看看这个id_table到底是什么东西：
```
static struct usb_device_id skel_table [] = {       
     { USB_DEVICE(USB_SKEL_VENDOR_ID, USB_SKEL_PRODUCT_ID) },       
     { }                    /* Terminating entry */       
};       
      
MODULE_DEVICE_TABLE (usb, skel_table); 
```
*MODULE_DEVICE_TABLE*的第一个参数是设备的类型，如果是USB设备，那自然是usb。后面一个参数是设备表，这个设备表的最后一个元素是空的，用于标识结束。代码定义了USB_SKEL_VENDOR_ID是0xfff0，USB_SKEL_PRODUCT_ID是0xfff0，也就是说，当有一个设备接到集线器时，usb子系统就会检查这个设备的vendor ID和product ID，如果它们的值是0xfff0时，那么子系统就会调用这个skeleton模块作为设备的驱动。
当USB设备接到USB控制器接口时，usb_core就检测该设备的一些信息，例如生产厂商ID和产品的ID，或者是设备所属的class、subclass跟protocol，以便确定应该调用哪一个驱动处理该设备。
我们下面所要做的就是对probe函数与disconnect函数的填充了，但是在对probe函数与disconnect函数填充之前，有必要先学习三个重要的数据结构，这在我们后面probe函数与disconnect函数中有很大的作用：

二、USB驱动程序中重要数据结构
========================
1 usb-skeleton
--------------
usb-skeleton 是一个局部结构体，用于与端点进行通信。下面先看一下Linux内核源码中的一个usb-skeleton（就是usb驱动的骨架咯），其定义的设备结构体就叫做usb-skel：
```
struct usb_skel {     
     struct usb_device *udev;                 /* the usb device for this device */     
     struct usb_interface  *interface;            /* the interface for this device */     
     struct semaphore limit_sem;         /* limiting the number of writes in progress */     
     unsigned char *bulk_in_buffer;     /* the buffer to receive data */     
     size_t         bulk_in_size;                  /* the size of the receive buffer */     
     __u8          bulk_in_endpointAddr;        /* the address of the bulk in endpoint */     
     __u8          bulk_out_endpointAddr;      /* the address of the bulk out endpoint */     
     struct kref   kref;     
}; 
```
它拥有：

- 描述usb设备的结构体udev
- 一个接口interface
- 用于并发访问控制的semaphore(信号量) limit_sem
- 用于接收数据的缓冲bulk_in_buffer
- 用于接收数据的缓冲尺寸bulk_in_size
- 批量输入端口地址bulk_in_endpointAddr
- 批量输出端口地址bulk_out_endpointAddr
- 内核使用的引用计数器

从开发人员的角度看，每一个usb设备有若干个配置(configuration)组成，每个配置又可以有多个接口(interface)（我理解就是USB设备的一项功能），每个接口又有多个设置，而接口本身可能没有端点或者多个端点（end point）

2 USB接口数据结构struct usb_interface
-----------------------------------
```
struct usb_interface    
{             
         struct usb_host_interface *altsetting;     
         struct usb_host_interface *cur_altsetting;          
         unsigned num_altsetting;             
         int minor;                          
         enum usb_interface_condition condition;             
         unsigned is_active:1;               
         unsigned needs_remote_wakeup:1;      
         struct device dev;                  
         struct device *usb_dev;             
         int pm_usage_cnt;                   
}; 
```
在逻辑上，一个USB设备的功能划分是通过接口来完成的。比如说一个USB扬声器，可能会包括有两个接口：一个用于键盘控制，另外一个用于音频流传输。而事实上，这种设备需要用到不同的两个驱动程序来操作，一个控制键盘，一个控制音频流。但也有例外，比如蓝牙设备，要求有两个接口，第一用于ACL跟EVENT的传输，另外一个用于SCO链路，但两者通过一个驱动控制。在Linux上，接口使用struct usb_interface来描述，以下是该结构体中比较重要的字段：

- struct usb_host_interface *altsetting（注意不是usb_interface）

其实据我理解，他应该是每个接口的设置，虽然名字上有点奇怪。该字段是一个设置的数组（一个接口可以有多个设置），每个usb_host_interface都包含一套由struct usb_host_endpoint定义的端点配置。但这些配置次序是不定的。

- struct usb_host_interface *cur_altsetting

当前活动的设置，指向altsetting数组中的一个
struct usb_host_interface数据结构：
```
struct usb_host_interface     
{    
         struct usb_interface_descriptor desc;//usb描述符，主要有四种usb描述符，设备描述符，配置描述符，接口描述符和端点描述符，协议里规定一个usb设备是必须支持这四大描述符的。    
                                 //usb描述符放在usb设备的eeprom里边    
         /* array of desc.bNumEndpoint endpoints associated with this  
          * interface setting. these will be in no particular order.  
          */    
         struct usb_host_endpoint *endpoint;//这个设置所使用的端点    
    
         char *string;           /* iInterface string, if present */    
         unsigned char *extra;   /* Extra descriptors */关于额外描述符    
         int extralen;    
};  
```

- unsigned num_altstting

可选设置的数量，即altsetting所指数组的元素个数

- int minor

当捆绑到该接口的USB驱动程序使用USB主设备号时，USB core分配的次设备号。仅在成功调用usb_register_dev之后才有效。

3 USB 端点 struct usb_host_endpoint
----------------------------------
Linux中用struct usb_host_endpoint 来描述USB端点
```
struct usb_host_endpoint     
{    
         struct usb_endpoint_descriptor desc;    
         struct list_head                urb_list;//端点要处理的urb队列.urb是usb通信的主角，设备中的每个端点都可以处理一个urb队列.要想和你的usb通信，就得创建一个urb，并且为它赋好值，    
                                   //交给咱们的usb core，它会找到合适的host controller，从而进行具体的数据传输    
         void                            *hcpriv;//这是提供给HCD（host controller driver）用的    
         struct ep_device                *ep_dev;        /* For sysfs info */    
    
         unsigned char *extra;   /* Extra descriptors */    
         int extralen;    
}; 
```
每个usb_host_endpoint中包含一个struct usb_endpoint_descriptor结构体，当中包含该端点的信息以及设备自定义的各种信息，这些信息包括：

- bEndpointAddress（b for byte）

8位端点地址，其地址还隐藏了端点方向的信息（之前说过，端点是单向的），可以用掩码USB_DIR_OUT和USB_DIR_IN来确定。

- bmAttributes

端点的类型，结合USB_ENDPOINT_XFERTYPE_MASK可以确定端点是USB_ENDPOINT_XFER_ISOC（等时）、USB_ENDPOINT_XFER_BULK（批量）还是USB_ENDPOINT_XFER_INT（中断）。

- wMaxPacketSize

端点一次处理的最大字节数。发送的BULK包可以大于这个数值，但会被分割传送。

- bInterval

如果端点是中断类型，该值是端点的间隔设置，以毫秒为单位

三、探测和断开函数分析
==================
USB驱动程序指定了两个USB核心在适当时间调用的函数。

1 探测函数
---------
当一个设备被安装而USB核心认为该驱动程序应该处理时，探测函数被调用；

探测函数应该检查传递给他的设备信息，确定驱动程序是否真的适合该设备。当驱动程序因为某种原因不应控制设备时，断开函数被调用，它可以做一些清洁的工作。

系统会传递给探测函数的信息是什么呢？一个usb_interface * 跟一个struct usb_device_id *作为参数。他们分别是该USB设备的接口描述（一般会是该设备的第0号接口，该接口的默认设置也是第0号设置）跟它的设备ID描述（包括Vendor ID、Production ID等）。

USB驱动程序应该初始化任何可能用于控制USB设备的局部结构体，它还应该把所需的任何设备相关信息保存到局部结构体中。例如，USB驱动程序通常需要探测设备对的端点地址和缓冲区大小，因为需要他们才能和端点通信。

下面具体分析探测函数做了哪些事情：

- 探测设备的端点地址、缓冲区大小，初始化任何可能用于控制USB设备的数据结构

下面是一个实例代码，他们探测批量类型的IN和OUT端点，把相关信息保存到一个局部设备结构体中：
```
/* set up the endpoint information */     
     /* use only the first bulk-in and bulk-out endpoints */     
     iface_desc = interface->cur_altsetting;     
     for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {     
         endpoint = &iface_desc->endpoint[i].desc;   
    
         if ( !dev->bulk_in_endpointAddr &&     
                ((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK) = = USB_DIR_IN) &&     
             ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) = = USB_ENDPOINT_XFER_BULK)) {     
             /* we found a bulk in endpoint */     
              buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);     
              dev->bulk_in_size = buffer_size;     
              dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;     
              dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);     
              if (!dev->bulk_in_buffer) {     
                  err("Could not allocate bulk_in_buffer");     
                   goto error;                   
              }  
         }     
    
         if (!dev->bulk_out_endpointAddr &&     
            ((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK)= =USB_DIR_OUT) &&     
               ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)= = USB_ENDPOINT_XFER_BULK)) {     
              /* we found a bulk out endpoint */     
              dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;     
         }     
     }     
    
     if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {     
         err("Could not find both bulk-in and bulk-out endpoints");     
         goto error;     
     } 
```
具体流程如下：   

该代码块首先循环访问该接口中存在的每一个端点，赋予该端点结构体的局部指针以使稍后的访问更加容易
```
for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {     
         endpoint = &iface_desc->endpoint[i].desc;  
```
然后，我们有了一个端点，而还没有发现批量IN类型的端点时，查看该端点的方向是否为IN。这可以通过检查位掩码 USB_DIR_IN 是否包含在bEndpointAddress 端点变量中来确定。如果是的话，我们测定该端点类型是否批量，这首先通过USB_ENDPOINT_XFERTYPE_MASK 位掩码来取bmAttributes变量的值，然后检查它是否和USB_ENDPOINT_XFER_BULK 的值匹配来完成
```
if ( !dev->bulk_in_endpointAddr &&     
               ((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK) = = USB_DIR_IN) &&     
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) = = USB_ENDPOINT_XFER_BULK)) {
```
如果这些都通过了，驱动程序就知道它已经发现了正确的端点类型，可以把该端点相关的信息保存到一个局部结构体中，就是我们前面的usb_skel ,以便稍后使用它和端点进行通信：
```
/* we found a bulk in endpoint */     
              buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);     
              dev->bulk_in_size = buffer_size;     
              dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;     
              dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);     
              if (!dev->bulk_in_buffer) {     
                  err("Could not allocate bulk_in_buffer");     
                   goto error;                 
          } 
```

- 把已经初始化数据结构的指针保存到接口设备中

接下来的工作是向系统注册一些以后会用的的信息。首先我们来说明一下usb_set_intfdata()，他向内核注册一个data，这个data的结构可以是任意的，这段程序向内核注册了一个usb_skel结构，就是我们刚刚看到的被初始化的那个，这个data可以在以后用usb_get_intfdata来得到
```
usb_set_intfdata(interface, dev);
```

- 注册USB设备

如果USB驱动程序没有和处理设备与用户交互（例如输入、tty、视频等）的另一种类型的子系统相关联，驱动程序可以使用USB主设备号，以便在用户空间使用传统的字符驱动程序接口。如果要这样做，USB驱动程序必须在探测函数中调用 usb_resgister_dev 函数来把设备注册到USB核心。只要该函数被调用，就要确保设备和驱动陈旭都处于可以处理用户访问设备的要求的恰当状态
```
retval = usb_register_dev(interface, &skel_class);
```
skel_class结构。这个结构又是什么？我们就来看看这到底是个什么东西：
```
static struct usb_class_driver skel_class = {     
     .name =       "skel%d",     
     .fops =       &skel_fops,     
     .minor_base = USB_SKEL_MINOR_BASE,     
}; 
```
它其实是一个系统定义的结构，里面包含了一名字、一个文件操作结构体还有一个次设备号的基准值。事实上它才是定义真正完成对设备IO操作的函数。所以他的核心内容应该是skel_fops。

因为usb设备可以有多个interface，每个interface所定义的IO操作可能不一样，所以向系统注册的usb_class_driver要求注册到某一个interface，而不是device，因此，usb_register_dev的第一个参数才是interface，而第二个参数就是某一个usb_class_driver。

通常情况下，linux系统用主设备号来识别某类设备的驱动程序，用次设备号管理识别具体的设备，驱动程序可以依照次设备号来区分不同的设备，所以，这里的次设备好其实是用来管理不同的interface的，但由于这个范例只有一个interface，在代码上无法求证这个猜想。
```
static struct file_operations skel_fops = {     
     .owner = THIS_MODULE,     
     .read =       skel_read,     
     .write =   skel_write,     
     .open =       skel_open,     
     .release =    skel_release,     
}; 
```

2 断开函数
---------
当设备被拔出集线器时，usb子系统会自动地调用disconnect，他做的事情不多，最重要的是注销class_driver（交还次设备号）和interface的data:
```
dev = usb_get_intfdata(interface);    
usb_set_intfdata(interface, NULL);    
    
/* give back our minor */    
usb_deregister_dev(interface, &skel_class); 
```

四、USB请求块
===========
USB 设备驱动代码通过urb和所有的 USB 设备通讯。urb用 struct urb 结构描述（include/linux/usb.h ）。

urb 以一种异步的方式同一个特定USB设备的特定端点发送或接受数据。一个 USB 设备驱动可根据驱动的需要，分配多个 urb 给一个端点或重用单个 urb 给多个不同的端点。设备中的每个端点都处理一个 urb 队列, 所以多个 urb 可在队列清空之前被发送到相同的端点。

一个 urb 的典型生命循环如下:
a. 被创建；
b. 被分配给一个特定 USB 设备的特定端点；
c. 被提交给 USB 核心；
d. 被 USB 核心提交给特定设备的特定 USB 主机控制器驱动；
e. 被 USB 主机控制器驱动处理, 并传送到设备；
f. 以上操作完成后，USB主机控制器驱动通知 USB 设备驱动。

urb 也可被提交它的驱动在任何时间取消；如果设备被移除，urb 可以被USB核心取消。urb 被动态创建并包含一个内部引用计数，使它们可以在最后一个用户释放它们时被自动释放。

```
struct urb  
{  
    /* 私有的：只能由usb核心和主机控制器访问的字段 */  
    struct kref kref; /*urb引用计数 */  
    spinlock_t lock; /* urb锁 */  
    void *hcpriv; /* 主机控制器私有数据 */  
    int bandwidth; /* int/iso请求的带宽 */  
    atomic_t use_count; /* 并发传输计数 */  
    u8 reject; /* 传输将失败*/  
      
    /* 公共的： 可以被驱动使用的字段 */  
    struct list_head urb_list; /* 链表头*/  
    struct usb_device *dev; /* 关联的usb设备 */  
    unsigned int pipe; /* 管道信息 */  
    int status; /* urb的当前状态 */  
    unsigned int transfer_flags; /* urb_short_not_ok | ...*/  
    void *transfer_buffer; /* 发送数据到设备或从设备接收数据的缓冲区 */  
    dma_addr_t transfer_dma; /*用来以dma方式向设备传输数据的缓冲区 */  
    int transfer_buffer_length;/*transfer_buffer或transfer_dma 指向缓冲区的大小 */  
                           
    int actual_length; /* urb结束后，发送或接收数据的实际长度 */  
    unsigned char *setup_packet; /* 指向控制urb的设置数据包的指针*/  
    dma_addr_t setup_dma; /*控制urb的设置数据包的dma缓冲区*/  
    int start_frame; /*等时传输中用于设置或返回初始帧*/  
    int number_of_packets; /*等时传输中等时缓冲区数据 */  
    int interval; /* urb被轮询到的时间间隔（对中断和等时urb有效） */  
    int error_count;  /* 等时传输错误数量 */  
    void *context; /* completion函数上下文 */  
    usb_complete_t complete; /* 当urb被完全传输或发生错误时，被调用 */  
    struct usb_iso_packet_descriptor iso_frame_desc[0];  
    /*单个urb一次可定义多个等时传输时，描述各个等时传输 */  
};  
```

1 创建和注销 urb
--------------
struct urb 结构不能静态创建,必须使用 usb_alloc_urb 函数创建. 函数原型:
```
struct urb *usb_alloc_urb(int iso_packets, gfp_t mem_flags);
//int iso_packets : urb 包含等时数据包的数目。如果不使用等时urb，则为0
//gfp_t mem_flags : 与传递给 kmalloc 函数调用来从内核分配内存的标志类型相同
//返回值          : 如果成功分配足够内存给 urb , 返回值为指向 urb 的指针. 如果返回值是 NULL, 则在 USB 核心中发生了错误, 且驱动需要进行适当清理
```
如果驱动已经对 urb 使用完毕, 必须调用 usb_free_urb 函数，释放urb。函数原型:
```
void usb_free_urb(struct urb *urb);
//struct urb *urb : 要释放的 struct urb 指针
```

2 初始化 urb
-----------
```
static inline void usb_fill_int_urb(struct urb *urb,                                                                                                         
                 struct usb_device *dev,  
                 unsigned int pipe,  
                 void *transfer_buffer,  
                 int buffer_length,  
                 usb_complete_t complete_fn,  
                 void *context,  
                 int interval);  
  
static inline void usb_fill_bulk_urb(struct urb *urb,  
                 struct usb_device *dev,  
                 unsigned int pipe,  
                 void *transfer_buffer,  
                 int buffer_length,  
                 usb_complete_t complete_fn,  
                 void *context);  
  
static inline void usb_fill_control_urb(struct urb *urb,  
                    struct usb_device *dev,  
                    unsigned int pipe,  
                    unsigned char *setup_packet,  
                    void *transfer_buffer,  
                    int buffer_length,  
                    usb_complete_t complete_fn,  
                    void *context);  
  
  
//struct urb *urb :指向要被初始化的 urb 的指针  
//struct usb_device *dev :指向 urb 要发送到的 USB 设备.  
//unsigned int pipe : urb 要被发送到的 USB 设备的特定端点. 必须使用前面提过的 usb_******pipe 函数创建  
//void *transfer_buffer :指向外发数据或接收数据的缓冲区的指针.注意:不能是静态缓冲,必须使用 kmalloc 来创建.  
//int buffer_length :transfer_buffer 指针指向的缓冲区的大小  
//usb_complete_t complete :指向 urb 结束处理例程函数指针  
//void *context :指向一个小数据块的指针, 被添加到 urb 结构中，以便被结束处理例程函数获取使用.  
//int interval :中断 urb 被调度的间隔.  
//函数不设置 urb 中的 transfer_flags 变量, 因此对这个成员的修改必须由驱动手动完成  
  
/*等时 urb 没有初始化函数，必须手动初始化，以下为一个例子*/  
urb->dev = dev;  
urb->context = uvd;  
urb->pipe = usb_rcvisocpipe(dev, uvd->video_endp-1);  
urb->interval = 1;  
urb->transfer_flags = URB_ISO_ASAP;  
urb->transfer_buffer = cam->sts_buf[i];  
urb->complete = konicawc_isoc_irq;  
urb->number_of_packets = FRAMES_PER_DESC;  
urb->transfer_buffer_length = FRAMES_PER_DESC;  
for (j=0; j < FRAMES_PER_DESC; j++) {  
        urb->iso_frame_desc[j].offset = j;  
        urb->iso_frame_desc[j].length = 1;  
}  
```

3 提交 urb
---------
一旦 urb 被正确地创建并初始化, 它就可以提交给 USB 核心以发送出到 USB 设备. 这通过调用函数 usb_submit_urb 实现:
```
int usb_submit_urb(struct urb *urb, gfp_t mem_flags);
//struct urb *urb :指向被提交的 urb 的指针 
//gfp_t mem_flags :使用传递给 kmalloc 调用同样的参数, 用来告诉 USB 核心如何及时分配内存缓冲

/*因为函数 usb_submit_urb 可被在任何时候被调用(包括从一个中断上下文), mem_flags 变量必须正确设置. 根据 usb_submit_urb 被调用的时间,只有 3 个有效值可用:
GFP_ATOMIC 
只要满足以下条件,就应当使用此值:
1.调用者处于一个 urb 结束处理例程,中断处理例程,底半部,tasklet或者一个定时器回调函数.
2.调用者持有自旋锁或者读写锁. 注意如果正持有一个信号量, 这个值不必要.
3.current->state 不是 TASK_RUNNING. 除非驱动已自己改变 current 状态,否则状态应该一直是 TASK_RUNNING .

GFP_NOIO 
驱动处于块 I/O 处理过程中. 它还应当用在所有的存储类型的错误处理过程中.

GFP_KERNEL 
所有不属于之前提到的其他情况
*/
```
在 urb 被成功提交给 USB 核心之后, 直到结束处理例程函数被调用前,都不能访问 urb 结构的任何成员.

4 urb结束处理例程

如果 usb_submit_urb 被成功调用, 并把对 urb 的控制权传递给 USB 核心, 函数返回 0; 否则返回一个负的错误代码. 如果函数调用成功, 当 urb 被结束的时候结束处理例程会被调用一次.当这个函数被调用时, USB 核心就完成了这个urb, 并将它的控制权返回给设备驱动.

只有 3 种结束urb并调用结束处理例程的情况:

(1). urb 被成功发送给设备, 且设备返回正确的确认.如果这样, urb 中的status变量被设置为 0.
(2). 发生错误, 错误值记录在 urb 结构中的 status 变量.
(3). urb 从 USB 核心unlink. 这发生在要么当驱动通过调用 usb_unlink_urb 或者 usb_kill_urb告知 USB 核心取消一个已提交的 urb,或者在一个 urb 已经被提交给它时设备从系统中去除.

5 取消 urb
---------
使用以下函数停止一个已经提交给 USB 核心的 urb:
```
void usb_kill_urb(struct urb *urb)
int usb_unlink_urb(struct urb *urb);
```
如果调用usb_kill_urb函数,则 urb 的生命周期将被终止. 这通常在设备从系统移除时,在断开回调函数(disconnect callback)中调用.

对一些驱动, 应当调用 usb_unlink_urb 函数来使 USB 核心停止 urb. 这个函数不会等待 urb 完全停止才返回. 这对于在中断处理例程中或者持有一个自旋锁时去停止 urb 是很有用的, 因为等待一个 urb 完全停止需要 USB 核心有使调用进程休眠的能力(wait_event()函数).


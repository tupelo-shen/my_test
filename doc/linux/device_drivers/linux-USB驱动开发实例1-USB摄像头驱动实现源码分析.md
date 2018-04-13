Spac5xx的实现是按照标准的USB VIDEO设备的驱动框架编写（其具体的驱动框架可参照/usr/src/linux/drivers/usb/usbvideo.c文件），整个源程序由四个主体部分组成：
- 设备模块的初始化模块
- 卸载模块
- 上层软件接口模块
- 数据传输模块。

具体的模块分析如下：

一 初始化设备模块
==============
该驱动采用了显式的模块初始化和消除函数，即调用module_init来初始化一个模块，并在卸载时调用moduel-exit函数,其具体实现如下：

1 模块初始化
----------
```
module_init (usb_spca5xx_init);
static int __init  usb_spca5xx_init (void)
{
    #ifdef CONFIG_PROC_FS
    proc_spca50x_create ();                 //建立PROC设备文件
    #endif /* CONFIG_PROC_FS */
    if (usb_register (&spca5xx_driver) < 0) //注册USB设备驱动
        return -1;
    info ("spca5xx driver %s registered", version);

    return 0;
}
```

2 模块卸载
---------
```
module_exit (usb_spca5xx_exit);   
  
static void __exit  usb_spca5xx_exit (void)   
{      
    usb_deregister(&spca5xx_driver);       //注销USB设备驱动     
    info ("driver spca5xx deregistered");   
#ifdef CONFIG_PROC_FS      
    proc_spca50x_destroy ();               //撤消PROC设备文件   
#endif /* CONFIG_PROC_FS */   
}   
```

3 关键数据结构 *USB驱动结构，即插即用*功能的实现
------------------------------------------
```
static struct usb_driver spca5xx_driver = {          
    "spca5xx",           
    spca5xx_probe, //注册设备自我侦测功能          
    spca5xx_disconnect,//注册设备自我断开功能          
    {NULL,NULL}   
};
```

用两个函数调用spca5xx_probe 和spca5xx_disconnect来支持USB设备的即插即用功能：

4 spca5xx_probe
---------------
具体实现如下：
```
static void * spca5xx_probe (struct usb_device *dev, unsigned int ifnum, const struct usb_device_id *id)   
{      
    struct usb_interface_descriptor *interface;     // USB设备接口描述符   
    struct usb_spca50x *spca50x;                    // 物理设备数据结构     
    int err_probe;     
    int i;

    if (dev->descriptor.bNumConfigurations != 1)    // 探测设备是不是可配置       
        goto nodevice;     
    if (ifnum > 0)       
        goto nodevice;      
    interface = &dev->actconfig->interface[ifnum].altsetting[0];    
    MOD_INC_USE_COUNT;      
    interface = &intf->altsetting[0].desc;     
    if (interface->bInterfaceNumber > 0)       
        goto nodevice;
    // 分配物理地址空间      
    if ((spca50x=kmalloc(sizeof(struct usb_spca50x),GFP_KERNEL))==NULL)        
    {          
        err ("couldn't kmalloc spca50x struct");         
        goto error;       
    }      
  
    memset (spca50x, 0, sizeof(struct usb_spca50x));     
    spca50x->dev = dev;      
    spca50x->iface = interface->bInterfaceNumber;
    // 具体物理设备查找，匹配厂商号，设备号（在子程序中）      
    if ((err_probe = spcaDetectCamera(spca50x)) < 0)          
    {          
        err (" Devices not found !! ");         
        goto error;       
    }      
    PDEBUG (0, "Camera type %s ", Plist[spca50x->cameratype].name)  
    for (i = 0; i < SPCA50X_NUMFRAMES; i++)        
        init_waitqueue_head (&spca50x->frame[i].wq);    //初始化帧等待队列       
        init_waitqueue_head (&spca50x->wq);             //初始化驱动等待队列     
  
    if (!spca50x_configure (spca50x))                   //物理设备配置（主要完成传感器侦测和图形参数配置），主要思想是给控制寄存器写值，读回其返回值，以此判断具体的传感器型号       
    {          
        spca50x->user = 0;          
        init_MUTEX (&spca50x->lock);                  //信号量初始化         
        init_MUTEX (&spca50x->buf_lock);          
        spca50x->v4l_lock = SPIN_LOCK_UNLOCKED;         
        spca50x->buf_state = BUF_NOT_ALLOCATED;       
    }                                      
    else       
    {  
        err ("Failed to configure camera");         
        goto error;       
    }   
    
  
    /* Init video stuff */   
    
    spca50x->vdev = video_device_alloc ();           //设备控制块内存分配     
    if (!spca50x->vdev)       
        goto error;  
    memcpy (spca50x->vdev, &spca50x_template, sizeof (spca50x_template));    
    //系统调用的挂接，在此将驱动实现的系统调用，挂到内核中     
    video_set_drvdata (spca50x->vdev, spca50x);   
    
    //video设备注册
    if(video_register_device(spca50x->vdev,VFL_TYPE_GRABBER,video_nr)<0)       
    {                                                       
        err ("video_register_device failed");         
        goto error;       
    }   
    
    spca50x->present = 1;     
    if (spca50x->force_rgb)   
        info ("data format set to RGB");     
    spca50x->task.sync = 0;   
    
    spca50x->task.routine = auto_bh;     
    spca50x->task.data = spca50x;     
    spca50x->bh_requested = 0;   
              
    MOD_DEC_USE_COUNT;                              //增加模块使用数     
    return spca50x;                                 //返回数剧结构   
error:  //错误处理     
    if (spca50x->vdev)       
    {   
        
        if (spca50x->vdev->minor == -1)   
         
            video_device_release (spca50x->vdev);         
        else   
         
            video_unregister_device (spca50x->vdev);         
        spca50x->vdev = NULL;       
    }   
    
    if (spca50x)       
    {   
        
        kfree (spca50x);         
        spca50x = NULL;       
    }   
    
    MOD_DEC_USE_COUNT;     
    return NULL; 

nodevice:     
    return NULL;   
}  
```

5 Spca5xx_disconnect 
--------------------
具体实现如下：
```
static void  spca5xx_disconnect (struct usb_device *dev, void *ptr)   
{      
    struct usb_spca50x *spca50x = (struct usb_spca50x *) ptr;     
    int n;      
    MOD_INC_USE_COUNT; //增加模块使用数     
    if (!spca50x)       
        return;      
    down (&spca50x->lock); //减少信号量     
    spca50x->present = 0;  //驱动卸载置0      
    for (n = 0; n < SPCA50X_NUMFRAMES; n++)       //标示所有帧ABORTING状态       
    {  
        spca50x->frame[n].grabstate = FRAME_ABORTING;       
        spca50x->curframe = -1;     
      
    }  
     for (n = 0; n < SPCA50X_NUMFRAMES; n++)       //唤醒所有等待进程       
    {  
        if (waitqueue_active (&spca50x->frame[n].wq))         
            wake_up_interruptible (&spca50x->frame[n].wq);     
          
        if (waitqueue_active (&spca50x->wq))          
            wake_up_interruptible (&spca50x->wq);      
    }  
    spca5xx_kill_isoc(spca50x);  //子函数终止URB包的传输     
    PDEBUG (3,"Disconnect Kill isoc done");      
    up (&spca50x->lock);  //增加信号量      
    while(spca50x->user) /如果还有进程在使用，进程切换         
        schedule();        
    down (&spca50x->lock);       
    if (spca50x->vdev)    
    {         
        video_unregister_device (spca50x->vdev);   //注销video设备       
        usb_driver_release_interface (&spca5xx_driver,&spca50x->dev->actconfig->interface[spca50x->iface]); //端口释放         
        spca50x->dev = NULL;         
    }  
    up (&spca50x->lock);   
#ifdef CONFIG_PROC_FS          
    destroy_proc_spca50x_cam (spca50x); //注销PROC文件   
#endif /* CONFIG_PROC_FS */   
         
    if (spca50x && !spca50x->user)                      //释放内存空间          
    {             
        spca5xx_dealloc (spca50x);            
        kfree (spca50x);            
        spca50x = NULL;          
    }      
    MOD_DEC_USE_COUNT;                              //减少模块记数     
    PDEBUG (3, "Disconnect complete");   
}  
```

二 上层软件接口
=============
该模块通过file_operations数据结构，依据V4L协议规范，实现设备的关键系统调用，实现设备文件化的UNIX系统设计特点。作为摄相头驱动，其功能在于数据采集，而没有向摄相头输出的功能，因此在源码中没有实现write系统调用。

其关键的数据结构如下：
```
static struct video_device spca50x_template = {     
    .owner = THIS_MODULE,    
    .name = "SPCA5XX USB Camera",     
    .type = VID_TYPE_CAPTURE,    
    .hardware = VID_HARDWARE_SPCA5XX,     
    .fops = &spca5xx_fops,           
}；   
   
static struct file_operations spca5xx_fops = {     
    .owner = THIS_MODULE,    
    .open = spca5xx_open, //open功能     
    .release = spca5xx_close,//close功能     
    .read = spca5xx_read, //read功能     
    .mmap = spca5xx_mmap, //内存映射功能     
    .ioctl = spca5xx_ioctl, //文件信息获取    
    .llseek = no_llseek,//文件定位功能未实现   
}; 
```

1 Open功能
---------
完成设备的打开和初始化，并初始化解码器模块。其具体实现如下：
```
static int  spca5xx_open(struct video_device *vdev, int flags)   
{      
    struct usb_spca50x *spca50x = video_get_drvdata (vdev);     
    int err;      
    MOD_INC_USE_COUNT;                         //增加模块记数     
    down (&spca50x->lock);                                
    err = -ENODEV;      
  
    if (!spca50x->present)                 //检查设备是不是存在，有不有驱动，是不是忙       
        goto out;    
    err = -EBUSY;     
    if (spca50x->user)       
        goto out;      
    err = -ENOMEM;      
    if (spca50x_alloc (spca50x))    
        goto out;                      
      
    err = spca50x_init_source (spca50x);           //初始化传感器和解码模块,在此函数的实现中，对每一款DSP芯片的初始化都不一样，对中星微301P的DSP芯片的初始化在子函数zc3xx_init，其实现方法为寄存器填值。     
    if (err != 0)  
    {          
        PDEBUG (0, "DEALLOC error on spca50x_init_source\n");         
        up (&spca50x->lock);          
        spca5xx_dealloc (spca50x);         
        goto out2;       
    }   
    
    // 解码模块初始化，其模块的具体实现采用的是huffman算法
    spca5xx_initDecoder(spca50x);                        spca5xx_setFrameDecoder(spca50x);     
    spca50x->user++;      
      
    err = spca50x_init_isoc (spca50x);              //初始化URB(usb request block) 包，启动摄相头，采用同步传输的方式传送数据     
    if (err)       
    {          
        PDEBUG (0, " DEALLOC error on init_Isoc\n");         
        spca50x->user--;          
        spca5xx_kill_isoc (spca50x);         
        up (&spca50x->lock);          
        spca5xx_dealloc (spca50x);         
        goto out2;  
    }      
  
    spca50x->brightness = spca50x_get_brghtness (spca50x) << 8;     
    spca50x->whiteness = 0;   
  
out:      
    up (&spca50x->lock);   
out2:     
    if (err)       
        MOD_DEC_USE_COUNT;     
    if (err)          
    {          
        PDEBUG (2, "Open failed");       
    }     
    else       
    {          
        PDEBUG (2, "Open done");      
    }      
    return err;   
}  
```

2 Close功能
----------
完成设备的关闭，其具体过程是：
```
static void spca5xx_close(struct video_device *vdev)   
{     
    struct usb_spca50x *spca50x =vdev->priv;     
    int i;     
    PDEBUG (2, "spca50x_close");     
    down (&spca50x->lock);  //参数设置     
    spca50x->user--;     
    spca50x->curframe = -1;   
    
    if(spca50x->present)//present：是或有驱动加载       
    {        
        spca50x_stop_isoc (spca50x);//停止摄相头工作和数据包送         
        spcaCameraShutDown(spca50x);//关闭摄相头，由子函数spca50x_stop_iso完成     
        for (i = 0; i < SPCA50X_NUMFRAMES; i++)    //唤醒所有等待进程       
        {        
            if(waitqueue_active(&spca50x->frame[i].wq))   
                wake_up_interruptible (&spca50x->frame[i].wq);       
        }   
        if(waitqueue_active (&spca50x->wq))          
            wake_up_interruptible (&spca50x->wq);     
    }    
      
    up(&spca50x->lock);   
    spca5xx_dealloc (spca50x);//回收内存空间     
      
    PDEBUG(2,"Release ressources done");     
    MOD_DEC_USE_COUNT;   
}   
```

3 Read功能
---------
完成数据的读取，其主要的工作就是将数据由内核空间传送到进程用户空间
```
static long spca5xx_read(struct video_device *dev, char * buf, unsigned long count,int noblock)   
{     
    struct usb_spca50x *spca50x = video_get_drvdata (dev);     
    int i;     
    int frmx = -1;     
    int rc;     
    volatile struct spca50x_frame *frame;   
      
        if(down_interruptible(&spca50x->lock))  //获取信号量         
        return -EINTR;   
      
        if(!dev || !buf){//判断设备情况       
        up(&spca50x->lock);       
        return -EFAULT;   
    }    
    
    if(!spca50x->dev){       
        up(&spca50x->lock);       
        return -EIO;   
    }   
      
    if (!spca50x->streaming){       
        up(&spca50x->lock);       
        return -EIO;   
    }   
      
        if((rc = wait_event_interruptible(spca50x->wq,     //在指定的队列上睡眠，直到参数2的条件为真   
              spca50x->frame[0].grabstate == FRAME_DONE ||   
              spca50x->frame[1].grabstate == FRAME_DONE ||                 
          spca50x->frame[2].grabstate == FRAME_DONE ||                 
              spca50x->frame[3].grabstate == FRAME_DONE )))    
        {                 
        up(&spca50x->lock);                 
        return rc;          
    }    
    
  
    for (i = 0; i < SPCA50X_NUMFRAMES; i++)         //当数据到来       
        if (spca50x->frame[i].grabstate == FRAME_DONE)   //标识数已到         
            frmx = i;     
    if (frmx < 0)       
    {   
        
        PDEBUG(2, "Couldnt find a frame ready to be read.");         
        up(&spca50x->lock);         
        return -EFAULT;       
    }   
    
    frame = &spca50x->frame[frmx];   
    PDEBUG (2, "count asked: %d available: %d", (int) count,(int) frame->scanlength);     
    if (count > frame->scanlength)       
        count = frame->scanlength;   
    
    if ((i = copy_to_user (buf, frame->data, count)))   //实现用户空间和内核空间的数据贝       
    {   
        
        PDEBUG (2, "Copy failed! %d bytes not copied", i);             
        up(&spca50x->lock);         
        return -EFAULT;       
    }   
    
    /* Release the frame */    
    frame->grabstate = FRAME_READY; //标识数据已空   
    up(&spca50x->lock);                             
    return count;//返回拷贝的数据数   
}
```

4 Mmap功能
---------
实现将设备内存映射到用户进程的地址空间的功能，其关键函数是remap_page_range，其具体实现如下：
```
static int spca5xx_mmap(struct video_device *dev,const char *adr, unsigned long size)   
{   
    
    unsigned long start = (unsigned long) adr;     
    struct usb_spca50x *spca50x = dev->priv;     
    unsigned long page, pos;     
    if (spca50x->dev == NULL)       
        return -EIO;   
      
    PDEBUG (4, "mmap: %ld (%lX) bytes", size, size);     
    if(size > (((SPCA50X_NUMFRAMES * MAX_DATA_SIZE) + PAGE_SIZE - 1) & ~(PAGE_SIZE -1)))       
        return -EINVAL;   
         
    if (down_interruptible(&spca50x->lock))  //获取信号量                 
        return -EINTR;   
    
    pos = (unsigned long) spca50x->fbuf;  
    while (size > 0)  //循环实现内存映射       
    {                   
        page = kvirt_to_pa (pos);   
        if (remap_page_range (start, page, PAGE_SIZE, PAGE_SHARED)){  //实现内存映射   
            up(&spca50x->lock);   
            return -EAGAIN;      
        }         
      
        start += PAGE_SIZE;         
        pos += PAGE_SIZE;         
        if (size > PAGE_SIZE)          
            size -= PAGE_SIZE;         
        else       
            size = 0;       
    }   
  
    up(&spca50x->lock); //释放信号量     
    return 0;   
}  
```

5 Ioctl功能
----------
实现文件信息的获取功能
```
static int spca5xx_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)   
{         
    struct video_device *vdev = file->private_data;          
    struct usb_spca50x *spca50x = vdev->priv;          
    int rc;   
         
    if(down_interruptible(&spca50x->lock))       //获取信号量                 
        return -EINTR;   
        
    rc = video_usercopy (inode, file, cmd, arg, spca5xx_do_ioctl);  //将信息传送到用户进程，其关键函数实现spca5xx_do_ioctl   
    up(&spca50x->lock);     
    return rc;  
}  
```
spca5xx_do_ioctl函数的实现依赖于不同的硬件，本驱动为了支持多种芯片，实现程序过于烦琐，其主要思想是通过copy_to_user(arg,b,sizeof(struct video_capability)函数将设备信息传递给用户进程。

三、数据传输模块
=============
源程序采用tasklet来实现同步快速传递数据，并通过spcadecode.c上的软件解码模块实现图形信息的解码。此模块的入口点挂节在spca_open函数中，其具体的函数为spca50x_init_isoc。当设备被打开时，同步传输数据也已经开始，并通过spca50x_move_data函数将数据传递给驱动程序，驱动程序通过轮询的办法实现对数据的访问。
```
Void outpict_do_tasklet (unsigned long ptr)   
{     
    int err;     
    struct spca50x_frame *taskletframe = (struct spca50x_frame *) ptr;     
    taskletframe->scanlength = taskletframe->highwater - taskletframe->data;   
      PDEBUG (2, "Tasklet ask spcadecoder hdrwidth %d hdrheight %d method %d",           
        taskletframe->hdrwidth, taskletframe->hdrheight,            
        taskletframe->method);   
    
    err = spca50x_outpicture (taskletframe);  //输出处理过的图片数据     
    if (err != 0)       
    {        
        PDEBUG (0, "frame decoder failed (%d)", err);         
        taskletframe->grabstate = FRAME_ERROR;      
    }     
    else       
    {        
        taskletframe->grabstate = FRAME_DONE;       
    }   
    
    if (waitqueue_active (&taskletframe->wq))//如果有进程等待，唤醒等待进程       
        wake_up_interruptible (&taskletframe->wq);   
}  
```
值得一提的是spcadecode.c上解码模块将原始压缩图形数据流yyuyv，yuvy， jpeg411，jpeg422解码为RGB图形，但此部分解压缩算法的实现也依赖于压缩的格式，归根结底依赖于DSP（数字处理芯片）中的硬件压缩算法。

四 USB CORE的支持
================
LINUX下的USB设备对下层硬件的操作依靠系统实现的USB CORE层，USB CORE对上层驱动提供了众多函数接口如：usb_control_msg，usb_sndctrlpipe等，其中最典型的使用为源码中对USB端点寄存器的读写函数spca50x_reg_write和spca50x_reg_read等，具体实现如下：（举spca50x_reg_write的实现，其他类似）

```
static int spca50x_reg_write(struct usb_device *dev,__u16 reg，__u16 index,__u16 value)   
{         
    int rc;          
    rc = usb_control_msg(dev,          //通过USB CORE提供的接口函数设置寄存器的值   
                usb_sndctrlpipe(dev, 0),   
                        reg,   
                        USB_TYPE_VENDOR | USB_RECIP_DEVICE,   
                        value, index, NULL, 0, TimeOut);   
         
    PDEBUG(5, "reg write: 0x%02X,0x%02X:0x%02X, 0x%x", reg, index, value, rc);          
    if (rc < 0)                
        err("reg write: error %d", rc);          
    return rc;   
} 
```

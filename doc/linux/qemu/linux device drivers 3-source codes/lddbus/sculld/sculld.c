/* 
 * sculld.c -- the bare sculld char module
 */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    /* O_ACCMODE */
#include <linux/aio.h>
#include <asm/uaccess.h>
#include "sculld.h"     /* local definitions */

int sculld_major =   SCULLD_MAJOR;
int sculld_devs =    SCULLD_DEVS;   /* sculld设备的数量 */
int sculld_qset =    SCULLD_QSET;
int sculld_order =   SCULLD_ORDER;

module_param(sculld_major, int, 0);
module_param(sculld_devs, int, 0);
module_param(sculld_qset, int, 0);
module_param(sculld_order, int, 0);

MODULE_AUTHOR("Tupelo Shen");
MODULE_LICENSE("Dual BSD/GPL");

struct sculld_dev *sculld_devices; /* allocated in sculld_init */
int sculld_trim(struct sculld_dev *dev);
void sculld_cleanup(void);

/*
 * `sculld`创建的`ldd_driver`结构
 */
static struct ldd_driver sculld_driver = {
    .version = "$Revision: 1.21 $",
    .module = THIS_MODULE,
    .driver = {
        .name = "sculld",
    },
};
#ifdef SCULLD_USE_PROC /* don't waste space if unused */
/*
 * The proc filesystem: function to read and entry
 */

void sculld_proc_offset(char *buf, char **start, off_t *offset, int *len)
{
    if (*offset == 0)
        return;
    if (*offset >= *len) {
        /* Not there yet */
        *offset -= *len;
        *len = 0;
    } else {
        /* We're into the interesting stuff now */
        *start = buf + *offset;
        *offset = 0;
    }
}
/* FIXME: Do we need this here??  It be ugly  */
int sculld_read_procmem(char *buf, char **start, off_t offset,
                   int count, int *eof, void *data)
{
    int i, j, order, qset, len = 0;
    int limit = count - 80; /* Don't print more than this */
    struct sculld_dev *d;

    *start = buf;
    for(i = 0; i < sculld_devs; i++) {
        d = &sculld_devices[i];
        if (down_interruptible (&d->sem))
            return -ERESTARTSYS;
        qset = d->qset;  /* retrieve the features of each device */
        order = d->order;
        len += sprintf(buf+len,"\nDevice %i: qset %i, order %i, sz %li\n",
                i, qset, order, (long)(d->size));
        for (; d; d = d->next) { /* scan the list */
            len += sprintf(buf+len,"  item at %p, qset at %p\n",d,d->data);
            sculld_proc_offset (buf, start, &offset, &len);
            if (len > limit)
                goto out;
            if (d->data && !d->next) /* dump only the last item - save space */
                for (j = 0; j < qset; j++) {
                    if (d->data[j])
                        len += sprintf(buf+len,"    % 4i:%8p\n",j,d->data[j]);
                    sculld_proc_offset (buf, start, &offset, &len);
                    if (len > limit)
                        goto out;
                }
        }
      out:
        up (&sculld_devices[i].sem);
        if (len > limit)
            break;
    }
    *eof = 1;
    return len;
}
#endif /* SCULLD_USE_PROC */
/*
 * 设备的打开关闭方法
 */
int sculld_open (struct inode *inode, struct file *filp)
{
    struct sculld_dev *dev; /* device information */

    /*  Find the device */
    dev = container_of(inode->i_cdev, struct sculld_dev, cdev);

        /* now trim to 0 the length of the device if open was write-only */
    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
        if (down_interruptible (&dev->sem))
            return -ERESTARTSYS;
        sculld_trim(dev); /* ignore errors */
        up (&dev->sem);
    }

    /* and use filp->private_data to point to the device data */
    filp->private_data = dev;

    return 0;          /* success */
}

/*
 * 设备的release方法 
 */
int sculld_release (struct inode *inode, struct file *filp)
{
    return 0;
}

/*
 * 遍历设备列表 
 */
struct sculld_dev *sculld_follow(struct sculld_dev *dev, int n)
{
    while (n--) {
        if (!dev->next) {
            dev->next = kmalloc(sizeof(struct sculld_dev), GFP_KERNEL);
            memset(dev->next, 0, sizeof(struct sculld_dev));
        }
        dev = dev->next;
        continue;
    }
    return dev;
}

/*
 * 设备文件的读写方法
 */
ssize_t sculld_read (struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    struct sculld_dev *dev = filp->private_data; /* the first listitem */
    struct sculld_dev *dptr;
    int quantum = PAGE_SIZE << dev->order;
    int qset = dev->qset;
    int itemsize = quantum * qset; /* how many bytes in the listitem */
    int item, s_pos, q_pos, rest;
    ssize_t retval = 0;

    if (down_interruptible (&dev->sem))
        return -ERESTARTSYS;
    if (*f_pos > dev->size) 
        goto nothing;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;
    /* find listitem, qset index, and offset in the quantum */
    item = ((long) *f_pos) / itemsize;
    rest = ((long) *f_pos) % itemsize;
    s_pos = rest / quantum; q_pos = rest % quantum;

        /* follow the list up to the right position (defined elsewhere) */
    dptr = sculld_follow(dev, item);

    if (!dptr->data)
        goto nothing; /* don't fill holes */
    if (!dptr->data[s_pos])
        goto nothing;
    if (count > quantum - q_pos)
        count = quantum - q_pos; /* read only up to the end of this quantum */

    if (copy_to_user (buf, dptr->data[s_pos]+q_pos, count)) {
        retval = -EFAULT;
        goto nothing;
    }
    up (&dev->sem);

    *f_pos += count;
    return count;

  nothing:
    up (&dev->sem);
    return retval;
}

ssize_t sculld_write (struct file *filp, const char __user *buf, size_t count,
        loff_t *f_pos)
{
    struct sculld_dev *dev = filp->private_data;
    struct sculld_dev *dptr;
    int quantum = PAGE_SIZE << dev->order;
    int qset = dev->qset;
    int itemsize = quantum * qset;
    int item, s_pos, q_pos, rest;
    ssize_t retval = -ENOMEM; /* our most likely error */

    if (down_interruptible (&dev->sem))
        return -ERESTARTSYS;

    /* find listitem, qset index and offset in the quantum */
    item = ((long) *f_pos) / itemsize;
    rest = ((long) *f_pos) % itemsize;
    s_pos = rest / quantum; q_pos = rest % quantum;

    /* follow the list up to the right position */
    dptr = sculld_follow(dev, item);
    if (!dptr->data) {
        dptr->data = kmalloc(qset * sizeof(void *), GFP_KERNEL);
        if (!dptr->data)
            goto nomem;
        memset(dptr->data, 0, qset * sizeof(char *));
    }
    /* Here's the allocation of a single quantum */
    if (!dptr->data[s_pos]) {
        dptr->data[s_pos] =
            (void *)__get_free_pages(GFP_KERNEL, dptr->order);
        if (!dptr->data[s_pos])
            goto nomem;
        memset(dptr->data[s_pos], 0, PAGE_SIZE << dptr->order);
    }
    if (count > quantum - q_pos)
        count = quantum - q_pos; /* write only up to the end of this quantum */
    if (copy_from_user (dptr->data[s_pos]+q_pos, buf, count)) {
        retval = -EFAULT;
        goto nomem;
    }
    *f_pos += count;
 
        /* update the size */
    if (dev->size < *f_pos)
        dev->size = *f_pos;
    up (&dev->sem);
    return count;

  nomem:
    up (&dev->sem);
    return retval;
}
/*
 * "Complete" an asynchronous operation.
 */
// static void sculld_do_deferred_op(void *p)
// {
//     struct async_work *stuff = (struct async_work *) p;
//     aio_complete(stuff->iocb, stuff->result, 0);
//     kfree(stuff);
// }

// static int sculld_defer_op(int write, struct kiocb *iocb, char __user *buf,
//         size_t count, loff_t pos)
// {
//     struct async_work *stuff;
//     int result;

//     /* Copy now while we can access the buffer */
//     if (write)
//         result = sculld_write(iocb->ki_filp, buf, count, &pos);
//     else
//         result = sculld_read(iocb->ki_filp, buf, count, &pos);

//     /* If this is a synchronous IOCB, we return our status now. */
//     if (is_sync_kiocb(iocb))
//         return result;

//     /* Otherwise defer the completion for a few milliseconds. */
//     stuff = kmalloc (sizeof (*stuff), GFP_KERNEL);
//     if (stuff == NULL)
//         return result; /* No memory, just complete now */
//     stuff->iocb = iocb;
//     stuff->result = result;
//     INIT_WORK(&stuff->work, sculld_do_deferred_op, stuff);
//     schedule_delayed_work(&stuff->work, HZ/100);
//     return -EIOCBQUEUED;
// }

// static ssize_t sculld_aio_read(struct kiocb *iocb, char __user *buf, size_t count,
//         loff_t pos)
// {
//     return sculld_defer_op(0, iocb, buf, count, pos);
// }
// static ssize_t sculld_aio_write(struct kiocb *iocb, const char __user *buf,
//         size_t count, loff_t pos)
// {
//     return sculld_defer_op(1, iocb, (char __user *) buf, count, pos);
// }

/*
 * 文件操作
 */
struct file_operations sculld_fops = {
    .owner =     THIS_MODULE,
    .read =      sculld_read,
    .write =     sculld_write,
    .open =      sculld_open,
    .release =   sculld_release,
};

/*
 * 添加sculld为字符设备
 */
static void sculld_setup_cdev(struct sculld_dev *dev, int index)
{
    int err, devno = MKDEV(sculld_major, index);
    
    cdev_init(&dev->cdev, &sculld_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &sculld_fops;
    err = cdev_add(&dev->cdev, devno, 1);
    /* 如果失败，退出 */
    if (err)
        printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

/*
 * 设置sculld设备的属性的show方法
 */
static ssize_t sculld_show_dev(struct device *ddev, char *buf)
{
    struct sculld_dev *dev = ddev->driver_data;

    return print_dev_t(buf, dev->cdev.dev);
}
/*
 * 设置sculld设备的属性
 */
static DEVICE_ATTR(dev, S_IRUGO, sculld_show_dev, NULL);
/*
 * 注册sculld设备
 */
static void sculld_register_dev(struct sculld_dev *dev, int index)
{
    sprintf(dev->devname, "sculld%d", index);
    dev->ldev.name = dev->devname;
    dev->ldev.driver = &sculld_driver;
    dev->ldev.dev.driver_data = dev;
    register_ldd_device(&dev->ldev);
    device_create_file(&dev->ldev.dev, &dev_attr_dev);
}

/*
 * sculld设备驱动初始化
 */
int sculld_init(void)
{
    int result, i;
    dev_t dev = MKDEV(sculld_major, 0);
    
    /*
     * 注册主设备号。如果主设备号给定，则使用设定值；否则使用动态设备号进行注册
     */
    if (sculld_major)
        result = register_chrdev_region(dev, sculld_devs, "sculld");
    else {
        result = alloc_chrdev_region(&dev, 0, sculld_devs, "sculld");
        sculld_major = MAJOR(dev);
    }
    if (result < 0)
        return result;

    /*
     * 注册ldd驱动
     */
    register_ldd_driver(&sculld_driver);
    
    /* 
     * 分配设备 -- 在这里没有使用固定的设备个数，是为了在加载设备的时候可以改变设备个数
     */
    sculld_devices = kmalloc(sculld_devs*sizeof (struct sculld_dev), GFP_KERNEL);
    if (!sculld_devices) {
        result = -ENOMEM;
        goto fail_malloc;
    }
    memset(sculld_devices, 0, sculld_devs*sizeof (struct sculld_dev));
    for (i = 0; i < sculld_devs; i++) {
        sculld_devices[i].order = sculld_order;
        sculld_devices[i].qset = sculld_qset;
        sema_init (&sculld_devices[i].sem, 1);
        sculld_setup_cdev(sculld_devices + i, i);
        sculld_register_dev(sculld_devices + i, i);
    }


#ifdef SCULLD_USE_PROC /* only when available */
    create_proc_read_entry("sculldmem", 0, NULL, sculld_read_procmem, NULL);
#endif
    return 0; /* succeed */

  fail_malloc:
    unregister_chrdev_region(dev, sculld_devs);
    return result;
}


void sculld_cleanup(void)
{
    int i;

#ifdef SCULLD_USE_PROC
    remove_proc_entry("sculldmem", NULL);
#endif

    for (i = 0; i < sculld_devs; i++) {
        unregister_ldd_device(&sculld_devices[i].ldev);
        cdev_del(&sculld_devices[i].cdev);
        sculld_trim(sculld_devices + i);
    }
    kfree(sculld_devices);
    unregister_ldd_driver(&sculld_driver);
    unregister_chrdev_region(MKDEV (sculld_major, 0), sculld_devs);
}


module_init(sculld_init);
module_exit(sculld_cleanup);

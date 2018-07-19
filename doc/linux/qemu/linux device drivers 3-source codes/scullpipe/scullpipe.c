#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/kernel.h>   /* printk(), min() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#include "scull.h"          /* local definitions */

struct scull_pipe {
    wait_queue_head_t       inq, outq;          /* read 和 write队列 */
    char                    *buffer, *end;      /* 缓冲区的头尾 */
    int                     buffersize;         /* 缓冲区大小 */
    char                    *rp, *wp;           /* 读写位置指针 */
    int                     nreaders, nwriters; /* r/w操作的数量 */
    struct fasync_struct    *async_queue;       /* 异步读取者 */
    struct semaphore        sem;                /* 互斥信号量 */
    struct cdev             cdev;               /* 字符设备结构 */
};

/* 参数 */
static int scull_p_nr_devs = SCULL_P_NR_DEVS;   /* scullpipe设备数量 */
int scull_p_buffer =  SCULL_P_BUFFER;           /* 缓冲区大小 */
dev_t scull_p_devno;                            /* 第一个设备号 */

module_param(scull_p_nr_devs, int, 0);
module_param(scull_p_buffer, int, 0);

static struct scull_pipe *scull_p_devices;

static int scull_p_fasync(int fd, struct file *filp, int mode);
static int space_free(struct scull_pipe *dev);

/*
 * 实现阻塞和非阻塞
 */

static ssize_t scull_p_read(struct file *filp, char __user *buf, size_t count, 
    loff_t *f_pos)
{
    struct scull_pipe *dev = filp->private_data;

    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    while (dev->rp == dev->wp) {   /* 什么也没读到 */
        up(&dev->sem);              /* 释放锁 */
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        PDEBUG("\"%s\" reading: going to sleep\n", current->comm);
        if (wait_event_interruptible(dev->inq, (dev->rp != dev->wp)))
            return -ERESTARTSYS;    /* 信号: 通知fs层进行处理 */
        
        /* 否则循环，但是首先获取锁 */
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    /* 数据已就绪，返回 */
    if (dev->wp > dev->rp)
        count = min(count, (size_t)(dev->wp - dev->rp));
    else /* 写入指针回卷，返回数据直到dev->end */
        count = min(count, (size_t)(dev->end - dev->rp));
    if (copy_to_user(buf, dev->rp, count)) {
        up(&dev->sem);
        return -EFAULT;
    }
 
    dev->rp += count;
    if (dev->rp == dev->end)
        dev->rp = dev->buffer; /* 回卷 */
    up(&dev->sem);

    /* 最后，唤醒所有写入者并返回 */
    wake_up_interruptible(&dev->outq);
    PDEBUG("\"%s\" did read %li bytes\n",current->comm, (long)count);
    return count;
}
/*
 * 判断缓冲区剩余
 */
static int space_free(struct scull_pipe *dev)
{
    if (dev->rp == dev->wp)
    {
        return dev->buffersize - 1;
    }
    return ((dev->rp + dev->buffersize - dev->wp) % dev->buffersize) - 1;
}
/*
 * 等待有可写入空间；调用者必须拥有设备信号量；
 * 发生错误时，信号量必须在返回之前被释放。
 */
static int scull_get_write_space(struct scull_pipe *dev, struct file *filp)
{
    while (space_free(dev) == 0) /* 满 */
    {
        DEFINE_WAIT(wait);
        
        up(&dev->sem);
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        PDEBUG("\"%s\" writing: going to sleep\n", current->comm);
        prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
        if (space_free(dev) == 0)
            schedule();
        finish_wait(&dev->outq, &wait);
        if (signal_pending(current))
            return -ERESTARTSYS; /* 发送信号：通知fs层处理 */
        if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    }
    return 0;
}
/*
 * scullpipe设备的write方法
 */
static ssize_t scull_p_write(struct file *filp, const char __user *buf, 
    size_t count, loff_t *f_pos)
{
    struct scull_pipe   *dev = filp->private_data;
    int                 result;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
        
    /* 是否有足够的写空间 */
    result = scull_get_write_space(dev, filp);
    if(result)
    {
        return result;  /* scull_get_write_space 调用 up(&dev->sem) */
    }

    /* 空间足够，写入 */
    count = min(count, (size_t) space_free(dev));
    if(dev->wp >= dev->rp)
    {
        count = min(count, (size_t)(dev->end - dev->wp)); /* 到缓冲区尾 */
    }else /* 写指针回卷，最大可以写到读指针-1 */
    {
        count = min(count, (size_t)(dev->rp - dev->wp - 1));
    }
    PDEBUG("Going to accept %li bytes to %p from %p\n", 
        (long)count, dev->wp, buf);
    if (copy_from_user(dev->wp, buf, count))
    {
        up(&dev->sem);
        return -EFAULT;    
    }
    dev->wp += count;
    if (dev->wp == dev->end)
    {
        dev->wp = dev->buffer; /* 回卷 */
    }
    up(&dev->sem);
    
    /* 最后，唤醒所有的读进程 */
    wake_up_interruptible(&dev->inq); /* blocked in read( ) and select( ) */
    /* and signal asynchronous readers, explained late in chapter 5 */
    if (dev->async_queue)
    {
        kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
    }
    PDEBUG("\"%s\" did write %li bytes\n",current->comm, (long)count);
    return count;
}

/*
 * FIXME this should use seq_file
 */
#ifdef SCULL_DEBUG
static void scullp_proc_offset(char *buf, char **start, off_t *offset, int *len)
{
    if (*offset == 0)
        return;
    if (*offset >= *len) {/* Not there yet */
        *offset -= *len;
        *len = 0;
    }
    else {          /* We're into the interesting stuff now */
        *start = buf + *offset;
        *offset = 0;
    }
}
static int scull_read_p_mem(char *buf, char **start, off_t offset, int count, 
    int *eof, void *data)
{
    int i, len;
    struct scull_pipe *p;

    #define LIMIT (PAGE_SIZE-200)   /* 打印的内容不超过这个大小 */
    *start = buf;
    len = sprintf(buf, "Default buffersize is %i\n", scull_p_buffer);
    for(i = 0; i < scull_p_nr_devs && len <= LIMIT; i++)
    {
        p = &scull_p_devices[i];
        if(down_interruptible(&p->sem))
        {
            return -ERESTARTSYS;
        }
        len += sprintf(buf+len, "\nDevice %i: %p\n", i, p);
        /* len += sprintf(buf+len, "   Queues: %p %p\n", p->inq, p->outq);*/
        len += sprintf(buf+len, "   Buffer: %p to %p (%i bytes)\n", 
            p->buffer, p->end, p->buffersize);
        len += sprintf(buf+len, "   rp %p   wp %p\n", p->rp, p->wp);
        len += sprintf(buf+len, "   readers %i   writers %i\n", 
            p->nreaders, p->nwriters);
        up(&p->sem);
        scullp_proc_offset(buf, start, &offset, &len);
    }
    *eof = (len <= LIMIT);
    return len;
}
#endif /* SCULL_DEBUG */

#if 0
/*
 * Data management: read
 */
static ssize_t scull_p_read(struct file *filp, char __user *buf, size_t count, 
                loff_t *f_pos)
{
    struct scull_pipe *dev = filp->private_data;

    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    while (dev->rp == dev->wp) {   /* 什么也没读到 */
        up(&dev->sem);              /* 释放锁 */
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        PDEBUG("\"%s\" reading: going to sleep\n", current->comm);
        if (wait_event_interruptible(dev->inq, (dev->rp != dev->wp)))
            return -ERESTARTSYS;    /* 信号: 通知fs层进行处理 */
        
        /* 否则循环，但是首先获取锁 */
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    /* 数据已就绪，返回 */
    if (dev->wp > dev->rp)
        count = min(count, (size_t)(dev->wp - dev->rp));
    else /* 写入指针回卷，返回数据直到dev->end */
        count = min(count, (size_t)(dev->end - dev->rp));
    if (copy_to_user(buf, dev->rp, count)) {
        up(&dev->sem);
        return -EFAULT;
    }
 
    dev->rp += count;
    if (dev->rp == dev->end)
        dev->rp = dev->buffer; /* 回卷 */
    up(&dev->sem);

    /* 最后，唤醒所有写入者并返回 */
    wake_up_interruptible(&dev->outq);
    PDEBUG("\"%s\" did read %li bytes\n",current->comm, (long)count);
    return count;
}

/*
 * 判断缓冲区剩余
 */
static int space_free(struct scull_pipe *dev)
{
    if (dev->rp == dev->wp)
    {
        return dev->buffersize - 1;
    }
    return ((dev->rp + dev->buffersize - dev->wp) % dev->buffersize) - 1;
}
/*
 * 等待有可写入空间；调用者必须拥有设备信号量；
 * 发生错误时，信号量必须在返回之前被释放。
 */
static int scull_get_write_space(struct scull_pipe *dev, struct file *filp)
{
    while (space_free(dev) == 0) /* 满 */
    {
        DEFINE_WAIT(wait);
        
        up(&dev->sem);
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        PDEBUG("\"%s\" writing: going to sleep\n", current->comm);
        prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
        if (space_free(dev) = = 0)
            schedule();
        finish_wait(&dev->outq, &wait);
        if (signal_pending(current))
            return -ERESTARTSYS; /* 发送信号：通知fs层处理 */
        if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    }
    return 0;
}
/*
 * scullpipe设备的write方法
 */
static ssize_t scull_p_write(struct file *filp, const char __user *buf, 
    size_t count, loff_t *f_pos)
{
    struct scull_pipe   *dev = filp->private_data;
    int                 result;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
        
    /* 是否有足够的写空间 */
    result = scull_get_write_space(dev, filp)
    if(result)
    {
        return result;  /* scull_get_write_space 调用 up(&dev->sem) */
    }

    /* 空间足够，写入 */
    count = min(count, (size_t) space_free(dev));
    if(dev->wp >= dev->rp)
    {
        count = min(count, (size_t)(dev->end - dev->wp)); /* 到缓冲区尾 */
    }else /* 写指针回卷，最大可以写到读指针-1 */
    {
        count = min(count, (size_t)(dev->rp - dev->wp - 1));
    }
    PDEBUG("Going to accept %li bytes to %p from %p\n", 
        (long)count, dev->wp, buf);
    if (copy_from_user(dev->wp, buf, count))
    {
        up(&dev->sem);
        return -EFAULT;    
    }
    dev->wp += count;
    if (dev->wp == dev->end)
    {
        dev->wp = dev->buffer; /* 回卷 */
    }
    up(&dev->sem);
    
    /* 最后，唤醒所有的读进程 */
    wake_up_interruptible(&dev->inq); /* blocked in read( ) and select( ) */
    /* and signal asynchronous readers, explained late in chapter 5 */
    if (dev->async_queue)
    {
        kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
    }
    PDEBUG("\"%s\" did write %li bytes\n",current->comm, (long)count);
    return count;
}
#endif 
static unsigned int scull_p_poll(struct file *filp, poll_table *wait)
{
    struct scull_pipe *dev = filp->private_data;
    unsigned int mask = 0;

    /*
     * The buffer is circular; it is considered full
     * if "wp" is right behind "rp" and empty if the
     * two are equal.
     */
    down(&dev->sem);
    poll_wait(filp, &dev->inq,  wait);
    poll_wait(filp, &dev->outq, wait);
    if (dev->rp != dev->wp)
        mask |= POLLIN | POLLRDNORM;    /* readable */
    if (space_free(dev))
        mask |= POLLOUT | POLLWRNORM;   /* writable */
    up(&dev->sem);
    return mask;
}
/*
 * Open 
 */
static int scull_p_open(struct inode *inode, struct file *filp)
{
    struct scull_pipe *dev;

    dev = container_of(inode->i_cdev, struct scull_pipe, cdev);
    filp->private_data = dev;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    if (!dev->buffer) {
        /* allocate the buffer */
        dev->buffer = kmalloc(scull_p_buffer, GFP_KERNEL);
        if (!dev->buffer) {
            up(&dev->sem);
            return -ENOMEM;
        }
    }
    dev->buffersize = scull_p_buffer;
    dev->end = dev->buffer + dev->buffersize;
    dev->rp = dev->wp = dev->buffer; /* rd and wr from the beginning */

    /* use f_mode,not  f_flags: it's cleaner (fs/open.c tells why) */
    if (filp->f_mode & FMODE_READ)
        dev->nreaders++;
    if (filp->f_mode & FMODE_WRITE)
        dev->nwriters++;
    up(&dev->sem);

    return nonseekable_open(inode, filp);
}
/*
 * Close 
 */
static int scull_p_release(struct inode *inode, struct file *filp)
{
    struct scull_pipe *dev = filp->private_data;

    /* remove this filp from the asynchronously notified filp's */
    scull_p_fasync(-1, filp, 0);
    down(&dev->sem);
    if (filp->f_mode & FMODE_READ)
        dev->nreaders--;
    if (filp->f_mode & FMODE_WRITE)
        dev->nwriters--;
    if (dev->nreaders + dev->nwriters == 0) {
        kfree(dev->buffer);
        dev->buffer = NULL; /* the other fields are not checked on open */
    }
    up(&dev->sem);
    return 0;
}

static int scull_p_fasync(int fd, struct file *filp, int mode)
{
    struct scull_pipe *dev = filp->private_data;

    return fasync_helper(fd, filp, mode, &dev->async_queue);
}

/*
 * The file operations for the pipe device (some are overlayed with bare scull)
 */
struct file_operations scull_pipe_fops = {
    .owner          = THIS_MODULE,
    .llseek         = no_llseek,
    .read           = scull_p_read,
    .write          = scull_p_write,
    .poll           = scull_p_poll,
    .unlocked_ioctl = scull_ioctl,
    .open           = scull_p_open,
    .release        = scull_p_release,
    .fasync         = scull_p_fasync,
};

/*
 * Set up a cdev entry.
 */
static void scull_p_setup_cdev(struct scull_pipe *dev, int index)
{
    int err, devno = scull_p_devno + index;
    
    cdev_init(&dev->cdev, &scull_pipe_fops);
    dev->cdev.owner = THIS_MODULE;
    err = cdev_add (&dev->cdev, devno, 1);
    /* Fail gracefully if need be */
    if (err)
        printk(KERN_NOTICE "Error %d adding scullpipe%d", err, index);
}

/*
 * 初始化scullpipe设备；返回创建的个数
 */
int scull_p_init(dev_t firstdev)
{
    int i, result;

    result = register_chrdev_region(firstdev, scull_p_nr_devs, "scullp");
    if (result < 0) {
        printk(KERN_NOTICE "Unable to get scullp region, error %d\n", result);
        return 0;
    }
    scull_p_devno = firstdev;
    scull_p_devices = kmalloc(scull_p_nr_devs * sizeof(struct scull_pipe), GFP_KERNEL);
    if (scull_p_devices == NULL) {
        unregister_chrdev_region(firstdev, scull_p_nr_devs);
        return 0;
    }
    memset(scull_p_devices, 0, scull_p_nr_devs * sizeof(struct scull_pipe));
    for (i = 0; i < scull_p_nr_devs; i++) {
        init_waitqueue_head(&(scull_p_devices[i].inq));
        init_waitqueue_head(&(scull_p_devices[i].outq));
        init_MUTEX(&scull_p_devices[i].sem);
        scull_p_setup_cdev(scull_p_devices + i, i);
    }
#ifdef SCULL_DEBUG
    create_proc_read_entry("scullpipe", 0, NULL, scull_read_p_mem, NULL);
#endif
    return scull_p_nr_devs;
}
/*
 * This is called by cleanup_module or on failure.
 * It is required to never fail, even if nothing was initialized first
 */
void scull_p_cleanup(void)
{
    int i;

#ifdef SCULL_DEBUG
    remove_proc_entry("scullpipe", NULL);
#endif

    if (!scull_p_devices)
        return; /* nothing else to release */

    for (i = 0; i < scull_p_nr_devs; i++) {
        cdev_del(&scull_p_devices[i].cdev);
        kfree(scull_p_devices[i].buffer);
    }
    kfree(scull_p_devices);
    unregister_chrdev_region(scull_p_devno, scull_p_nr_devs);
    scull_p_devices = NULL; /* pedantic */
}

#if 0
int main(int argc, char **argv)
{
    int delay = 1, n, m = 0;
 
    if (argc > 1)
        delay = atoi(argv[1]);
    
    fcntl(0, F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK); /* stdin */
    fcntl(1, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK); /* stdout */
    
    while (1) 
    {
        n = read(0, buffer, 4096);
        if (n >= 0)
            m = write(1, buffer, n);
        if ((n < 0 || m < 0) && (errno != EAGAIN))
            break;
        sleep(delay);
    }
    perror(n < 0 ? "stdin" : "stdout");
    exit(1);
}
#endif
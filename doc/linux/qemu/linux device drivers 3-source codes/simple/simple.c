// #include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/mm.h>
#include <linux/kdev_t.h>
#include <asm/page.h>
#include <linux/cdev.h>

#include <linux/device.h>

static int simple_major = 0;
module_param(simple_major, int, 0);

MODULE_AUTHOR("Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");

/*
 * simple设备文件的open方法实现-什么也没做
 */
static int simple_open (struct inode *inode, struct file *filp)
{
    return 0;
}


/*
 * simple设备文件的close方法的实现-什么也没做
 */
static int simple_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*
 * 打印虚拟内存开始的区域，以及物理内存的页帧编号
 */
void simple_vma_open(struct vm_area_struct *vma) 
{    
    printk(KERN_NOTICE "Simple VMA open, virt %lx, phys %lx\n",
            vma->vm_start, vma->vm_pgoff << PAGE_SHIFT); 
}

/*
 * close方法实现
 */
void simple_vma_close(struct vm_area_struct *vma)
{
    printk(KERN_NOTICE "Simple VMA close.\n");
}

/*
 * vm_operations_struct结构体的实现，类似于file_operations
 */
static struct vm_operations_struct simple_remap_vm_ops = {
    .open = simple_vma_open,
    .close = simple_vma_close,
};

/*
 * 新建一个VMA，然后把对其操作的方法添加到结构体中。
 */
static int simple_remap_mmap(struct file *filp, struct vm_area_struct *vma)
{
    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                vma->vm_end - vma->vm_start,
                vma->vm_page_prot))
        return -EAGAIN;

    printk (KERN_NOTICE "vma information\n");
    printk (KERN_NOTICE "vma->vm_start = %lx;", vma->vm_start);
    printk (KERN_NOTICE "vma->vm_end = %lx;", vma->vm_end);
    printk (KERN_NOTICE "vma->vm_pgoff = %lx;", vma->vm_pgoff);

    vma->vm_ops = &simple_remap_vm_ops;
    simple_vma_open(vma);
    return 0;
}

/*
 * 使用nopage版本
 */
struct page *simple_vma_nopage(struct vm_area_struct *vma,
    unsigned long address, int *type)
{
    struct page *pageptr;
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
    unsigned long physaddr = address - vma->vm_start + offset;
    unsigned long pageframe = physaddr >> PAGE_SHIFT;

    // 打印一些调试信息
    printk (KERN_NOTICE "vma information\n");
    printk (KERN_NOTICE "vma->vm_start = %lx;", vma->vm_start);
    printk (KERN_NOTICE "vma->vm_end = %lx;", vma->vm_end);
    printk (KERN_NOTICE "vma->vm_pgoff = %lx;", vma->vm_pgoff);
    printk (KERN_NOTICE "---- Nopage, off %lx phys %lx\n", offset, physaddr);
    printk (KERN_NOTICE "VA is %p\n", __va (physaddr));
    printk (KERN_NOTICE "Page at %p\n", virt_to_page (__va (physaddr)));
    if (!pfn_valid(pageframe))
        return NULL;
    pageptr = pfn_to_page(pageframe);
    printk (KERN_NOTICE "page->index = %ld mapping %p\n", pageptr->index, pageptr->mapping);
    printk (KERN_NOTICE "Page frame %ld\n", pageframe);
    get_page(pageptr);
    if (type)
        *type = VM_FAULT_MINOR;
    return pageptr;
}

/*
 * vm_operations_struct结构体的实现，类似于file_operations
 */
static struct vm_operations_struct simple_nopage_vm_ops = {
    .open =   simple_vma_open,
    .close =  simple_vma_close,
    // .nopage = simple_vma_nopage, /* 2.6.10及以后版本没有这个成员了 */
};

/*
 * 使用nopage实现mmap方法，按页操作
 */
static int simple_nopage_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

    if (offset >= __pa(high_memory) || (filp->f_flags & O_SYNC))
        vma->vm_flags |= VM_IO;
    vma->vm_flags |= VM_RESERVED;

    vma->vm_ops = &simple_nopage_vm_ops;
    simple_vma_open(vma);
    return 0;
}

/*
 * 为每个设备设置cdev结构
 */
static void simple_setup_cdev(struct cdev *dev, int minor,
        struct file_operations *fops)
{
    int err, devno = MKDEV(simple_major, minor);
    
    cdev_init(dev, fops);
    dev->owner = THIS_MODULE;
    dev->ops = fops;
    err = cdev_add(dev, devno, 1);
    /* 失败处理 */
    if (err)
        printk (KERN_NOTICE "Error %d adding simple%d", err, minor);
}

/* 设备0使用remap_pfn_range */
static struct file_operations simple_remap_ops = {
    .owner   = THIS_MODULE,
    .open    = simple_open,
    .release = simple_release,
    .mmap    = simple_remap_mmap,
};

/* 设备1使用nopage */
static struct file_operations simple_nopage_ops = {
    .owner   = THIS_MODULE,
    .open    = simple_open,
    .release = simple_release,
    .mmap    = simple_nopage_mmap,
};

// 用来记录两个字符设备
#define MAX_SIMPLE_DEV 2
static struct cdev SimpleDevs[MAX_SIMPLE_DEV];

/*
 * 模块-初始化函数
 */
static int simple_init(void)
{
    int result;
    dev_t dev = MKDEV(simple_major, 0);

    /* 计算设备-主设备号、次设备号 */
    if (simple_major)
        result = register_chrdev_region(dev, 2, "simple");
    else {
        result = alloc_chrdev_region(&dev, 0, 2, "simple");
        simple_major = MAJOR(dev);
    }
    if (result < 0) {
        printk(KERN_WARNING "simple: unable to get major %d\n", simple_major);
        return result;
    }
    if (simple_major == 0)
        simple_major = result;

    /* 现在注册两个字符设备 */
    simple_setup_cdev(SimpleDevs, 0, &simple_remap_ops);
    simple_setup_cdev(SimpleDevs + 1, 1, &simple_nopage_ops);
    return 0;
}
/*
 * 模块-模块退出函数
 */
static void simple_cleanup(void)
{
    cdev_del(SimpleDevs);
    cdev_del(SimpleDevs + 1);
    unregister_chrdev_region(MKDEV(simple_major, 0), 2);
}

module_init(simple_init);
module_exit(simple_cleanup);

#include <linux/device.h>       /* 包含内核对象模型的数据结构的定义及辅助函数 */
#include <linux/module.h>       
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>         /* 包含 kcalloc(),kzalloc()函数等 */
 
MODULE_AUTHOR("Tupelo Shen");
MODULE_LICENSE("Dual BSD/GPL");
 
/*
 * 定义container_of的简便方法
 */
#define to_my_kobj(data)        container_of(data, struct my_kobj, kobj)
 
/*
 * 自定义内核对象结构
 */
struct my_kobj {
    int val;
    struct kobject kobj;
};

// 自定义属性
struct my_attribute {
    struct attribute attr;
 
    ssize_t (*show)(struct my_kobj *obj, struct my_attribute *attr, char *buf);
    ssize_t (*store)(struct my_kobj *obj, struct my_attribute *attr,
        const char *buf, size_t count);
};

/* 
 * 分别为name和val两个属性定义自己的show/store函数。
 * name这个属性是只读的，只要为它定义show即可
 */
ssize_t name_show(struct my_kobj *obj, struct my_attribute *attr, char *buffer)
{
    return sprintf(buffer, "%s\n", kobject_name(&obj->kobj));
}
ssize_t val_show(struct my_kobj *obj, struct my_attribute *attr, char *buffer)
{
    return sprintf(buffer, "%d\n", obj->val);
}
ssize_t val_store(struct my_kobj *obj, struct my_attribute *attr,
    const char *buffer, size_t size)
{
    sscanf(buffer, "%d", &obj->val);
 
    return size;
}

/*
 * 定义name和val属性
 */
struct my_attribute name_attribute = __ATTR(name, 0444, name_show, NULL);
struct my_attribute val_attribute = __ATTR(val, 0666, val_show, val_store);

struct attribute *my_attrs[] = {
    &name_attribute.attr,
    &val_attribute.attr,
    NULL,
};

/*
 * 用户空间调用读写函数时的属性显示和存储函数
 */
static ssize_t my_attr_show(struct kobject *kobj, struct attribute *attr, 
    char *buffer)
{
    struct my_attribute *my_attr;
    ssize_t ret = -EIO;

    my_attr = container_of(attr, struct my_attribute, attr);
    if(my_attr->show)
        ret = my_attr->show(container_of(kobj,struct my_kobj, kobj), my_attr, 
                            buffer);
    return ret;
}
 
static ssize_t my_attr_store(struct kobject *kobj, struct attribute *attr, 
    const char *buffer, size_t size)
{
    struct my_attribute *my_attr;
    ssize_t ret = -EIO;

    my_attr = container_of(attr, struct my_attribute, attr);
    if(my_attr->store)
        ret = my_attr->store(container_of(kobj, struct my_kobj, kobj), my_attr, 
            buffer, size);
    return ret;
}
 
struct sysfs_ops my_sysfsops = {
    .show = my_attr_show,
    .store = my_attr_store,
};

struct attribute_group my_group = {
    .name     = "mygroup",
    .attrs    = my_attrs,
}; 

/*
 * 定义两个内核对象，它们的层次结构如下： 
 *
 *   mykobj/
 *   |-- mygroup
 *   | |-- name
 *   | `-- val
 *   |-- name
 *   `-- val
 */ 
struct my_kobj *obj;
struct kobj_type my_type;
 
void obj_release(struct kobject *kobj)
{
    struct my_kobj *my_kobj_free = to_my_kobj(kobj);
    printk(KERN_INFO "my_kobj_release\n");
    kfree(my_kobj_free);
}

/*
 * 初始化内核对象，完成创建并添加
 */ 
static int __init my_kobj_init(void)
{
    int retval = 0;

    printk(KERN_INFO "mykobj_init\n");
 
    obj = (struct my_kobj *)kzalloc(sizeof(struct my_kobj), GFP_KERNEL);
    if (!obj) {
        return -ENOMEM;
    }
    obj->val = 1;    
 
    my_type.release = obj_release;
    // my_type.default_attrs = my_attrs;
    my_type.sysfs_ops = &my_sysfsops;

    // 父节点-在这儿为/sys/kernel/
    kobject_init_and_add(&obj->kobj, &my_type, kernel_kobj, "mykobj");

    // 创建多个属性文件name和val
    retval = sysfs_create_files(&obj->kobj,(const struct attribute **)my_attrs);
    if (retval)
    {
        printk(KERN_INFO "create obj attribute failed!\n");
        kobject_put(&obj->kobj);
        return retval;
    }

    // 对于一个给定的对象，创建属性组
    retval = sysfs_create_group(&obj->kobj, &my_group);
    if (retval) {
        printk(KERN_INFO "create obj attribute group failed!\n");
    }

    return retval;
}
 
static void __exit my_kobj_exit(void)
{
    printk(KERN_INFO "mykobj_exit\n");
     
    kobject_del(&obj->kobj);
    kobject_put(&obj->kobj);
 
    return;
}
 
module_init(my_kobj_init);
module_exit(my_kobj_exit);

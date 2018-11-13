#include <linux/device.h>       /* include the structure of the device and class */
#include <linux/module.h>       
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>         /* include kcalloc(),kzalloc() functions,and so on */
 
MODULE_AUTHOR("Tupelo Shen");
MODULE_LICENSE("Dual BSD/GPL");
 
/*
 * the macro container_of is included in <include/linux/kernel.h>
 * prototype:
 * #define container_of(ptr,type,member) ({             \
 * const typeof(((type *)0)->member) *__mptr = (ptr);   \
 * (type *)((char *)__mptr - offsetof(type,member));})
 * 
 * cast a member of a structure out to the containing structure.
 */
#define to_my_kobj(data)        container_of(data, struct my_kobj, kobj)
 
/*
 * define an object for my_kobj which type is struct kobject
 */
struct my_kobj {
    int val;
    struct kobject kobj;
};

/*
 * the tree structure in /sys/ directory.
 *
 *   mykobj1/
 *   |-- mykobj2
 *   | |-- name
 *   | `-- val
 *   |-- name
 *   `-- val
 */
struct attribute name_attr = {
    .name = "name",
    .mode = 0444,
};
 
struct attribute val_attr = {
    .name = "val",
    .mode = 0666,
};
 
struct attribute *my_attrs[] = {
    &name_attr,
    &val_attr,
    NULL,
};

/*
 * 属性的显示和存储函数
 */
ssize_t my_show(struct kobject *kobj, struct attribute *attr, char *buffer)
{
    struct my_kobj *obj = container_of(kobj, struct my_kobj, kobj);
    ssize_t count = 0;
 
    if (strcmp(attr->name, "name") == 0) {
        count = sprintf(buffer, "%s\n", kobject_name(kobj));
    } else if (strcmp(attr->name, "val") == 0) {
        count = sprintf(buffer, "%d\n", obj->val);
    }
 
    return count;
}
 
ssize_t my_store(struct kobject *kobj, struct attribute *attr, 
    const char *buffer, size_t size)
{
    struct my_kobj *obj = container_of(kobj, struct my_kobj, kobj);
 
    if (strcmp(attr->name, "val") == 0) {
        sscanf(buffer, "%d", &obj->val);
    }
 
    return size;
}
 
struct sysfs_ops my_sysfsops = {
    .show = my_show,
    .store = my_store,
};

/*
 * define two kernel objects, which hierarchy is as follows: 
 *
 *   mykobj1/
 *   |-- mykobj2
 *   | |-- name
 *   | `-- val
 *   |-- name
 *   `-- val
 */ 
struct my_kobj *obj1;
struct my_kobj *obj2;
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
    printk(KERN_INFO "mykobj_init\n");
 
    obj1 = (struct my_kobj *)kzalloc(sizeof(struct my_kobj), GFP_KERNEL);
    if (!obj1) {
        return -ENOMEM;
    }
    obj1->val = 1;
 
    obj2 = (struct my_kobj *)kzalloc(sizeof(struct my_kobj), GFP_KERNEL);
    if (!obj2) {
        kfree(obj1);
        return -ENOMEM;
    }
    obj2->val = 2;
 
    my_type.release = obj_release;
    my_type.default_attrs = my_attrs;
    my_type.sysfs_ops = &my_sysfsops;
 
    kobject_init_and_add(&obj1->kobj, &my_type, NULL, "mykobj1");
    kobject_init_and_add(&obj2->kobj,&my_type,&obj1->kobj, "mykobj2");
    
    return 0;
}
 
static void __exit my_kobj_exit(void)
{
    printk(KERN_INFO "mykobj_exit\n");
 
    kobject_del(&obj2->kobj);
    kobject_put(&obj2->kobj);
     
    kobject_del(&obj1->kobj);
    kobject_put(&obj1->kobj);
 
    return;
}
 
module_init(my_kobj_init);
module_exit(my_kobj_exit);



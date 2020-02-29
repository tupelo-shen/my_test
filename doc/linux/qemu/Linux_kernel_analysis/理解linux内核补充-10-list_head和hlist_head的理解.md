# 1 概述

在Linux内核中，对于数据的管理，提供了2种类型的双向链表：一种是使用`list_head`结构体构成的环形双向链表；另一种是使用`hlist_head`和`hlist_node`2个结构体构成的具有表头的链型双向链表。

`list_head`的结构体如下所示：

    struct list_head {
        struct list_head *next, *prev;
    };

`hlist_head`和`hlist_node`的结构体如下所示：

    struct hlist_head {
        struct hlist_node *first;
    };
    struct hlist_node {
        struct hlist_node *next, **pprev;
    };

它们组成的链表结构如图所示：

<img id="Figure" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/linux_doubly_linked_table.PNG">

> 注意图上的箭头指向，在（a）图中，next指向下一个list_head结构的地址，prev指向前一个list_head结构的地址。而在（b）图中，next类似，指向下一个hlist_node结构的地址，而pprev指向前一个hlist_node的next指针。这是为什么呢？我们将在[第3章](#3)中讨论。

内核为什么设计这么2个双向链表呢？使用它们有什么好处？它们的使用场景分别是什么呢？让我们一一阐述：

1. 设计目的
    
    内核中大量使用数据表，为了更好的管理这些表，必须满足：

    * **所有操作都是原子的**，不能受到并发的影响。
    * **必须是通用模型**，这样可以节省空间，使用灵活，而且也避免浪费编程人员的精力。
    * **访问时间尽可能短**，提高内核查找匹配速度。

2. 适用场景
    
    `list_head`结构组成的列表适用于对遍历时间要求比较严格的地方，比如进程的管理。我们在看`task_struct`结构体中就有多个`list_head`类型的变量，用来管理task。

    `hlist_head`和`hlist_node`主要用于散列表中，因为内核中存在大量的hash表，使用这种方式实现的散列表因为少一个指针，可以节省一半的空间。

# 2 list_head链表

图（a）是由`list_head`组成的双向链表。它和普通的双向链表非常相似，只是仅包含2个成员next和prev指针，分别指向下一个和前一个list_head结构体。

假设我们的内核中需要定义一个kernel_list的结构体，它除了2个list_head结构体类型成员list1和list2之外，还有1个`void *`类型成员item，那么它的结构体可以是：

    struct kernel_list {
        struct list_head list1;
        struct list_head list2;
        void *item;
    };

所以，`list_head`结构体组成的双向链表，具有一下特性：

* list在你需要链接数据结构的里面；
* 可以把`struct list_head`放在该数据结构的任何地方；
* 可以吧`struct list_head`变量命名为任何名字。
* 可以有多个list在一个数据结构中。

## 2.1 初始化

链表初始化分为静态初始化和动态初始化：

1. 动态
    
        #define LIST_HEAD_INIT(name) { &(name), &(name) }
        /* 或 */
        static inline void INIT_LIST_HEAD(struct list_head *list)
        {
                list->next = list;
                list->prev = list;
        }
        /* 例如 */
        struct list_head my_list = LIST_HEAD_INIT(my_list);
        struct list_head your_list;
        INIT_LIST_HEAD(&your_list);

2. 静态 - 用于声明时
    
        #define LIST_HEAD(name) \
                struct list_head name = LIST_HEAD_INIT(name)

        /* 例如 */
        LIST_HEAD(rds_sock_list);  
        /* ==> */
        struct rds_sock_list = { &(rds_sock_list), &(rds_sock_list) };

## 2.2 插入

    static inline void __list_add(struct list_head *new,
                                  struct list_head *prev,
                                  struct list_head *next)
    {
        next->prev = new;
        new->next = next;
        new->prev = prev;
        prev->next = new;
    }
    /* new 插入到head之后 */
    static inline void list_add(struct list_head *new, struct list_head *head)
    {
        __list_add(new, head, head->next);
    }
    /* new 插入到head之前 */
    static inline void list_add_tail(struct list_head *new, struct list_head *head)
    {
        __list_add(new, head->prev, head);
    }

## 2.3 删除

    static inline void __list_del(struct list_head * prev, struct list_head * next)
    {
        next->prev = prev;
        prev->next = next;
    }
    static inline void list_del(struct list_head *entry)
    {
        __list_del(entry->prev, entry->next); 
    }

## 2.4 替换

    static inline void list_replace(struct list_head *old, struct list_head *new)
    {
        new->next = old->next;
        new->next->prev = new;
        new->prev = old->prev;
        new->prev->next = new;
    }

    static inline void list_replace_init(struct list_head *old, 
                                        struct list_head *new)
    {
        list_replace(old, new);
        INIT_LIST_HEAD(old);
    }

## 2.5 拼接

    static inline void __list_splice(const struct list_head *list,
                                    struct list_head *prev,
                                    struct list_head *next)
    {
        struct list_head *first = list->next;
        struct list_head *last = list->prev;
        first->prev = prev;
        prev->next = first;
        last->next = next;
        next->prev = last;
    }
    /* list 加到 head 前面 */
    static inline void list_splice(const struct list_head *list,
                                    struct list_head *head)
    {
        if (!list_empty(list))
            __list_splice(list, head, head->next);
    }
    /* list 加到 head 后面 */
    static inline void list_splice_tail(struct list_head *list,
                                        struct list_head *head)
    {
        if (!list_empty(list))
            __list_splice(list, head->prev, head);
    }

## 2.6 列表其它操作

    static inline int list_is_last(const struct list_head *list,
                                    const struct list_head *head)
    {
        return list->next == head;
    }
    static inline int list_empty(const struct list_head *head)
    {
        return head->next == head;
    }
    /* list_empty_careful - tests whether a list is empty and not being modified */
    static inline int list_empty_careful(const struct list_head *head)
    {
        struct list_head *next = head->next;
        return (next == head) && (next == head->prev);
    }
    static inline int list_is_singular(const struct list_head *head)
    {
        return !list_empty(head) && (head->next == head->prev);
    }

## 2.7 链表入口点获取

一般通过`struct list_head`维护链表，那么有一个指向`struct list_head*`的指针`ptr` ,如何获取它所在结构体的指针，然后访问它的成员呢？

    /*
     * @ptr:        &struct list_head 指针。
     * @type:       ptr所在结构体的类型。
     * @member:     list_head在type结构体中的名字。
     */
    #define list_entry(ptr, type, member) \
            container_of(ptr, type, member)
    #define container_of(ptr, type, member) ({                      \
            const typeof(((type *)0)->member) *__mptr = (ptr);    \
            (type *)((char *)__mptr - offsetof(type, member));})
    #define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

    /* list_first_entry中的ptr是指表头的指针。不是第一个节点的指针，如上图所示 */ 
    #define list_first_entry(ptr, type, member) \
            list_entry((ptr)->next, type, member)

这段代码的核心就是`container_of`宏，它通过巧妙使用0指针，它的成员地址就是就是该成员在实际结构中的偏移，所以通过这种巧妙的转变取得了`type`类型的结构体的指针。有了这个指针，我们就可以访问其任何成员了。

# 2.8 遍历链表

1. list_for_each遍历一个链表。
    
        // @pos：struct list_head类型的指针，用于指向我们遍历的链表节点；
        // @head：我们要遍历链表的头节点；
        #define list_for_each(pos, head) \      
                for (pos = (head)->next; pos != (head); pos = pos->next)

2. __list_for_each与list_for_each完全一样，遍历一个链表，同时也不做预取。

        #define __list_for_each(pos, head) \
                for (pos = (head)->next; pos != (head); pos = pos->next)

3. __list_for_each_prev用于从后向前反向遍历。
    
        #define list_for_each_prev(pos, head) \
                for (pos = (head)->prev; pos != (head); pos = pos->prev)

4. list_for_each_safe安全的遍历一个链表，其机制是我们多传入一个struct list_head的指针n，用于指向pos的下一个节点，以保证我们在删除pos指向的节点时，仍能继续遍历链表的剩余节点。

        #define list_for_each_safe(pos, n, head) \
                for (pos = (head)->next, n = pos->next; pos != (head); \
                        pos = n, n = pos->next)

5. list_for_each_prev_safe反向遍历，安全查找。

        #define list_for_each_prev_safe(pos, n, head) \
                for (pos = (head)->prev, n = pos->prev; \
                     pos != (head); \
                     pos = n, n = pos->prev)

6. 前面5项在遍历链表时返回的是struct list_head指针的地址。当我们使用struct list_head型变量将一个节点挂到一个链表时，我们不是为了仅仅操纵这个光凸凸的节点，而是将struct list_head变量放到一个结构体内，根据对链表上struct list_head的遍历来得出strcut list_head所在结构体的首地址，list_for_each_entry正是为了完成这一功能而实现。

        #define list_for_each_entry(pos, head, member)                          \
                for (pos = list_entry((head)->next, typeof(*pos), member);      \
                     &pos->member != (head);    \
                     pos = list_entry(pos->member.next, typeof(*pos), member))

我们将所求结构体类型的指针变量pos、链表的头head和所求结构体内struct list_head的变量名member传到list_for_each_entry之后， list_entry的第一个参数用head->next指向下一个节点，此节点的地址也就是在所属结构体内的struct list_head成员变量的地址，第二个参数用typeof(*pos)求得pos的结构体类型，第三个参数为所求结构体内struct list_head的变量名。

# 2.9 list_head示例代码

从当前module节点,遍历系统所有module.

    #include <linux/module.h>
    #include <linux/kernel.h>
    #include <linux/init.h>
    #include <linux/version.h>
    #include <linux/list.h>

    MODULE_LICENSE("GPL");
    struct module *m = &__this_module;
    static void list_module_test(void)
    {
        struct module *mod;
        list_for_each_entry(mod, m->list.prev, list)
            printk ("%s\n", mod->name);
    }
    static int list_module_init (void)
    {
        list_module_test();
        return 0;
    }

    static void list_module_exit (void)
    {
        printk ("unload listmodule.ko\n");
    }

    module_init(list_module_init);
    module_exit(list_module_exit);

Makefile文件如下，可直接在linux主机上编译：

    ifeq ($(KERNELRELEASE),)
        KERNELDIR ?= /lib/modules/$(shell uname -r)/build
        PWD := $(shell pwd)
    modules:
            $(MAKE) -C $(KERNELDIR) M=$(PWD) modules

    modules_install:
            $(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

    clean:
            rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

    .PHONY: modules modules_install clean
    else
        obj-m := listmodule.o
    endif

# 3 hlist_head和hlist_node链表

拥有一个指针链表头的双向链表.它被分为`struct hlist_head`头节点和`struct hlist_node` 元素节点。`pprev`指向前一个节点的next指针.

现在疑问来了:为什么pprev不是prev也就是一个指针,用于简单的指向list的前一个指针呢?这样即使对于first而言,它可以将prev指针指向list的尾结点.

主要是基于以下几个考虑:

1. hash列表中的list一般元素不多(如果太多了一般是设计出现了问题),即使遍历也不需要太大的代价,同时需要得到尾结点的需求也不多.
2. 如果对于一般节点而言,prev指向的是前一个指针,而对于first也就是hash的第一个元素而言prev指向的是list的尾结点,那么在删除一个元素的时候还需要判断该节点是不是first节点进行处理.而在hlist提供的删除节点的API中,并没有带上hlist_head这个参数,因此做这个判断存在难度.
3. 以上两点说明了为什么不使用prev,现在来说明为什么需要的是pprev。这样的话只需要修改pprev指针的指向就可以删除成员。

## 3.1 初始化

    #define HLIST_HEAD_INIT { .first = NULL }
    #define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
    #define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
    static inline void INIT_HLIST_NODE(struct hlist_node *h)
    {
            h->next = NULL;
            h->pprev = NULL;
    } 

## 3.2 插入

    static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
    {
        struct hlist_node *first = h->first;
        n->next = first;
        if (first)
            first->pprev = &n->next;
        h->first = n;
        n->pprev = &h->first;
    }
    /* 节点n插入到节点next之前 */
    /* next must be != NULL */
    static inline void hlist_add_before(struct hlist_node *n,
                                            struct hlist_node *next)
    {
        n->pprev = next->pprev;
        n->next = next;
        next->pprev = &n->next;
        *(n->pprev) = n;
    }
    /* 节点n插入到节点next之后 */
    static inline void hlist_add_after(struct hlist_node *n,
                                            struct hlist_node *next)
    {
        next->next = n->next;
        n->next = next;
        next->pprev = &n->next;

        if(next->next)
            next->next->pprev  = &next->next;
    }

## 3.3 删除

    static inline void __hlist_del(struct hlist_node *n)
    {
            struct hlist_node *next = n->next;
            struct hlist_node **pprev = n->pprev;
            *pprev = next;
            if (next)
                    next->pprev = pprev;
    }

    static inline void hlist_del(struct hlist_node *n)
    {
            __hlist_del(n);
            n->next = LIST_POISON1;
            n->pprev = LIST_POISON2;
    }

    static inline void hlist_del_init(struct hlist_node *n)
    {
            if (!hlist_unhashed(n)) {
                    __hlist_del(n);
                    INIT_HLIST_NODE(n);
            }
    }

## 3.4 遍历

    #define hlist_for_each(pos, head) \
            for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; }); \
                 pos = pos->next)
    #define hlist_for_each_safe(pos, n, head) \
            for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
                 pos = n)
    #define hlist_entry(ptr, type, member) container_of(ptr,type,member)
    #define hlist_for_each_entry(tpos, pos, head, member)                    \
            for (pos = (head)->first;                                        \
                 pos && ({ prefetch(pos->next); 1;}) &&                      \
                    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
                 pos = pos->next)
    #define hlist_for_each_entry_continue(tpos, pos, member)                 \
            for (pos = (pos)->next;                                          \
                 pos && ({ prefetch(pos->next); 1;}) &&                      \
                    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
                 pos = pos->next)
    #define hlist_for_each_entry_from(tpos, pos, member)                     \
            for (; pos && ({ prefetch(pos->next); 1;}) &&                    \
                    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
                 pos = pos->next)
    #define hlist_for_each_entry_safe(tpos, pos, n, head, member)            \
            for (pos = (head)->first;                                        \
                 pos && ({ n = pos->next; 1; }) &&                           \
                    ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
                 pos = n)

## 3.5 其它

    static inline int hlist_unhashed(const struct hlist_node *h)
    {
        return !h->pprev;
    }

    static inline int hlist_empty(const struct hlist_head *h)
    {
        return !h->first;
    }

## hlish_head示例

    #include <linux/module.h>
    #include <linux/kernel.h>
    #include <linux/init.h>
    #include <linux/version.h>
    #include <linux/list.h>
    #include <linux/slab.h>

    MODULE_LICENSE("GPL");

    #define init_name_hash()                0
    #define IFNAMSIZ 16

    #define NETDEV_HASHBITS 8
    #define NETDEV_HASHENTRIES (1 << NETDEV_HASHBITS)

    /* 
     * partial_hash_update函数。假设每个字符占用4位 
     */
    static inline unsigned long
    partial_name_hash(unsigned long c, unsigned long prevhash)
    {
      return (prevhash + (c << 4) + (c >> 4)) * 11;
    }

    /*
     * long型->int型
     */
    static inline unsigned long end_name_hash(unsigned long hash)
    {
      return (unsigned int) hash;
    }

    /* 
     * 为一个字符串计算哈希值 
     */
    static inline unsigned int
    full_name_hash(const unsigned char *name, unsigned int len)
    {
        unsigned long hash = init_name_hash();
        while (len--)
            hash = partial_name_hash(*name++, hash);
        return end_name_hash(hash);
    }

    /*
     * 定义hash表头
     */
    struct net {
        struct hlist_head       *dev_name_head;
    };

    static inline struct hlist_head *dev_name_hash(struct net *net,
                                                   const char *name)
    {
        unsigned hash = full_name_hash(name, strnlen(name, IFNAMSIZ));
        return &net->dev_name_head[hash & ((1 << NETDEV_HASHBITS) - 1)];
    }  

    static struct hlist_head *netdev_create_hash(void)
    {
        int i;
        struct hlist_head *hash;

        hash = kmalloc(sizeof(*hash) * NETDEV_HASHENTRIES, GFP_KERNEL);
        if (hash != NULL)
            for (i = 0; i < NETDEV_HASHENTRIES; i++)
                INIT_HLIST_HEAD(&hash[i]);

        return hash;
    }

    struct net_device {
        char    name[IFNAMSIZ];
        struct  hlist_node name_hlist;
        int     net_num;
    };

    struct net_device *dev_get_by_name(struct net *net, const char *name)
    {
        struct hlist_node *p;
        hlist_for_each(p, dev_name_hash(net, name)) {
        struct net_device *dev = hlist_entry(p, struct net_device, name_hlist);
        if (!strncmp(dev->name, name, IFNAMSIZ))
            return dev;
        }
        return NULL;
    }

    static struct net net_space;
    static struct net_device *devices;
    static int hlist_module_init(void) 
    {
        const   int dev_num = 10;
                int i;
        
        net_space.dev_name_head = netdev_create_hash();
        if (net_space.dev_name_head == NULL) {
            goto err_name;
        }
        devices = kmalloc(sizeof(struct net_device) * dev_num, GFP_KERNEL);
        if (devices == NULL) {
            goto err_dev;
        }
        for (i = 0; i < dev_num; ++i) {
            snprintf(devices[i].name, IFNAMSIZ, "eth%d", i); 
            devices[i].net_num = i;
            hlist_add_head(&devices[i].name_hlist,
                    dev_name_hash(&net_space, devices[i].name));
        }
        struct net_device *dev;
        dev = dev_get_by_name(&net_space, "eth1");
        if (dev) {
            printk("%s, %d\n", dev->name, dev->net_num);
        }
        return 0;
    err_dev:
        kfree(net_space.dev_name_head);
    err_name:
        return -ENOMEM;
    }

    static void hlist_module_exit(void) {
        kfree(devices);
        kfree(net_space.dev_name_head);
    }

    module_init(hlist_module_init);
    module_exit(hlist_module_exit);
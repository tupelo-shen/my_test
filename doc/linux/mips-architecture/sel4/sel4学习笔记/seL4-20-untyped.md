[TOC]

本教程将会讲解`seL4`微内核中的物理内存管理。

## 1 前提条件

1. [建立虚拟开发环境](https://docs.sel4.systems/HostDependencies).
2. [能力教程](https://docs.sel4.systems/Tutorials/capabilities)

## 2 学习目标

1. 理解术语`untyped`和`device untyped`；
2. 知道如何在`seL4`中从未分配内存中创建对象；
3. 知道如何回收对象。

## 3 背景知识

`通知`允许进程之间发送异步信号，主要用于中断处理和共享内存之间的同步。

#### 3.1 物理内存

`seL4`系统中，除了内核占用的少量内存，其它所有的物理内存都在用户态进行管理。系统启动阶段，`seL4`会创建指向这些物理内存对象的能力，连同`seL4`管理的其它物理资源，一起传递给`root`任务。

#### 3.2 非类型化对象（untyped）

> 名词解释：
> 
> untyped - 根据对seL4内存管理的理解，可以翻译为非类型化内存。跟我们常规理解的自由内存（或空闲内存）的概念是不一样的。自由内存就是一段地址连续的物理内存，大小可以任意；而untyped内存对象是一段具有特定大小的连续物理内存，或者是一个可读写的设备。

除了创建`root`任务的内存对象之外，所有指向可用物理内存的能力都会被作为指向untyped内存的能力传递给`root`任务。untyped对象还有一个布尔型属性`device`，用来表示内核是否可写该内存；如果内存对象是`device`内存，它要访问的就不是RAM，而是其它设备。设备非类型化对象（`device untyped`）只能被类型为帧对象（`frame object`，物理页帧，可以被映射为虚拟内存），且内核不能访问它们。

#### 3.3 初始状态

`seL4_BootInfo`数据结构描述了所有的非类型化能力，包括它们的大小，如果是设备非类型化对象，

下面的代码展示了`seL4_BootInfo`提供的初始非类型化能力：

```c
printf("    CSlot   \tPaddr           \tSize\tType\n");
for (seL4_CPtr slot = info->untyped.start; slot != info->untyped.end; slot++) {
    seL4_UntypedDesc *desc = &info->untypedList[slot - info->untyped.start];
    printf("%8p\t%16p\t2^%d\t%s\n", (void *) slot, (void *) desc->paddr, desc->sizeBits, desc->isDevice ? "device untyped" : "untyped");
}
```

我们将上面的代码添加到能力教程提供的代码中，编译运行，可以得到下面的结果：

```
    CSlot       Paddr               Size    Type
   0x136                   0        2^20    device untyped
   0x137          0x1ffe0000        2^17    device untyped
   0x138          0x20000000        2^29    device untyped
   ......(省略)
   0x174      0x7c8000000000        2^41    device untyped
   0x175      0x7e8000000000        2^40    device untyped
   0x176      0x7f8000000000        2^39    device untyped
   0x177            0x100000        2^20    untyped
   0x178            0x200000        2^21    untyped
   0x179            0x400000        2^22    untyped
   ......(省略)
   0x190          0x1ff00000        2^19    untyped
   0x191          0x1ff80000        2^18    untyped
   0x192          0x1ffc0000        2^17    untyped
```

#### 3.4 分配类型化对象（retyping）

从未类型化能力（`untyped`）中创建一个新能力的系统调用是 [seL4_Untyped_Retype](https://docs.sel4.systems/ApiDoc.html#retype)。新创建的能力是源未类型化对象的子能力，能够访问的也只是未类型化对象所包含地址范围的一个子集。

碎片化问题是任何一个操作系统绕不开的话题，`seL4`也一样。从`未类型化对象`中申请分配`类型化对象`时，是一种贪婪模式。理解这一点，对提高`seL4`的内存使用率至关重要。每一个未类型化对象都维护着一个水印，水印之前的地址不可用（已经类型化），水印之后的地址是空闲可分配的（未类型化）。在申请内存的过程中，水印首先移动到创建对象的对齐处，然后再移动到创建对象实际大小的结束处。比如，要在未类型化对象中创建一个4K大小的对象时，由于对齐的原因，首先创建一个16K大小的对象，那么4K结束处到16K开始地址之间的12K地址空间就被浪费了。在子对象被撤销之前，内存无法被回收利用。

非类型化对象应该按照大小的顺序分配，最大的优先，以避免浪费内存。

```c
error = seL4_Untyped_Retype(parent_untyped,         // 要申请的非类型化对象
                            seL4_UntypedObject,     // 类型
                            untyped_size_bits,      // 大小
                            seL4_CapInitThreadCNode,// root
                            0,                      // node_index
                            0,                      // node_depth
                            child_untyped,          // node_offset
                            1                       // num_caps
                            );
```

参数说明：

1. type - 类型

    `seL4`中每个对象都有一个特定的类型，所有的类型定义都可以在`libsel4`中找到。有些类型特定于架构，其它的则是跨架构平台的。在上面的示例中，我们在一个未类型化的对象上又创建了一个未类型化的对象。创建对象的类型决定了何种调用可以作用于其上。比如，如果指定对象的类型为`seL4_TCBObject`，一个TCB任务控制块对象，我们就可以对该对象执行TCB相关的系统调用，比如，`seL4_TCB_SetPriority`。

2. size - 对象大小

    指定创建对象的大小，该参数是指数，对象的实际大小就是`2^size_bits`。

    所有seL4对象的大小必须是2的幂，但是有些对象的大小是固定的，而其他对象的大小是可变的。如果是固定大小的对象，参数`size`被忽略。可变大小的对象包含未类型化对象（`seL4_UntypedObject`）、能力节点（`seL4_CapTableObject`） 和帧对象，后者依赖于架构。

3. root、node_index和node_depth - 指定新能力存放的位置

    这3个参数指定要存放新能力的能力节点CNode。`depth`参数决定了寻址方式是`调用`还是`直接寻址`。

    在上面的示例中，`node_depth = 0`，这意味着`root`参数隐含地使用当前线程的`root`能力空间，深度是`seL4_WordBits`。所以，示例代码相当于指定了`root`任务的能力节点CNode（`seL4_CapInitThreadCNode`）。这种情况下，`node_index`参数被忽略。

    如果`node_depth`不设为0，则将当前线程的`root`能力空间作为root参数的入参，采用直接寻址方式。参数`node_index`和`node_depth`用来确定存放新能力的CNode能力。这是为多级能力空间准备的，本文不涉及。

4. node_offset - 创建的新能力的槽位置

    前面参数定义的能力节点中，新能力存放的槽位置。本示例中，就是初始能力节点的第一个空能力槽。

5. num_caps - 创建的新能力数量

    指定要创建的能力数量。该参数有两个限制条件：

    - 未类型化的对象必须足够大，能够满足`num_caps * (1u << size_bits)`;

    - CNode中必须有足够的空闲能力槽（CSlot）满足所有的新能力。

## 4 练习

#### 4.1 创建一个untyped内存

当第一次运行本教程的源代码时，会看到如下所示的内容。会打印启动阶段提供给`root`任务所有的未类型化的能力：

```
Booting all finished, dropped to user space
    CSlot       Paddr               Size    Type
   0x136                   0    2^20    device untyped
   0x137          0x1ffe0000    2^17    device untyped
   0x138          0x20000000    2^29    device untyped
   0x139          0x40000000    2^30    device untyped
   0x13a          0x80000000    2^30    device untyped
   0x13b          0xc0000000    2^29    device untyped
   0x13c          0xe0000000    2^28    device untyped
   0x13d          0xf0000000    2^27    device untyped
   0x13e          0xf8000000    2^26    device untyped
   ......
   main@main.c:49 [Cond failed: error != seL4_NoError]
    Failed to retype
```

这个错误是因为我们正在尝试创建一个大小为0的未类型化对象。

* 练习内容：计算所需的`子未类型化对象`的大小，以便子非类型化对象可以创建对象数组中所有的对象。

    ```c
    // 列出所有通用的seL4对象
    seL4_Word objects[] = {seL4_TCBObject, seL4_EndpointObject, seL4_NotificationObject};
    // 列出所有通用的seL4对象的位数大小
    seL4_Word sizes[] = {seL4_TCBBits, seL4_EndpointBits, seL4_NotificationBits};

    /* TODO: 计算应该创建一个多大的untyped对象，满足创建上面列出的所有对象
     * 前面列出的3个待创建的对象，最大的是TCB对象（12位）；
     * 而端点对象（4位）和通知对象（6位）加起来不如TCB对象。
     * 所以，将大小定为2个TCB对象的大小。
     */
    // seL4_Word untyped_size_bits = 0;
    seL4_Word untyped_size_bits = seL4_TCBBits + 1;
    seL4_CPtr parent_untyped = 0;
    seL4_CPtr child_untyped = info->empty.start;

    // 首先，寻找一个足够大的untyped对象，满足创建所有的对象
    for (int i = 0; i < (info->untyped.end - info->untyped.start); i++) {
        if (info->untypedList[i].sizeBits >= untyped_size_bits && !info->untypedList[i].isDevice) {
            parent_untyped = info->untyped.start + i;
            break;
        }
    }
    ```
    一旦成功，就会继续往下执行，打印`Failed to set priority`。

#### 4.2 创建一个TCB对象

因为子TCB对象（`child_tcb`）还没有创建，所以尝试设置优先级失败。

* 练习： 创建一个TCB对象，

    ```c
    /* 初始能力节点的第1个CSlot存放了parent_untyped能力。
     * 所以，child_tcb能力，存放在第2个CSlot位置。
     */
    seL4_CPtr child_tcb = child_untyped + 1;
    /* TODO: 在child_tcb槽中创建一个TCB对象 */
    error = seL4_Untyped_Retype(child_untyped,          /* the untyped capability to retype */
                                objects[0],             /* type */
                                sizes[0],               /* size */
                                seL4_CapInitThreadCNode,/* root */
                                0,                      /* node_index */
                                0,                      /* node_depth */
                                child_tcb,              /* node_offset */
                                1                       /* num_caps */
                                );
    ZF_LOGF_IF(error != seL4_NoError, "Failed to create child_tcb");

    /* 尝试设置TCB优先级 */
    error = seL4_TCB_SetPriority(child_tcb, seL4_CapInitThreadTCB, 10);
    ZF_LOGF_IF(error != seL4_NoError, "Failed to set priority");
    ```

    一旦成功，程序会继续往下执行，并打印`Endpoint cap is null cap`。

#### 4.3 创建一个端点对象

* 练习：在`child_untyped`中创建一个端点对象，存储在`child_ep`的CSlot中。

    ```c
    /* child_ep 存放在第3个CSlot位置 */
    seL4_CPtr child_ep = child_tcb + 1;
    /* TODO：在child_ep槽中创建端点对象 */
    error = seL4_Untyped_Retype(child_untyped,          /* the untyped capability to retype */
                                objects[1],             /* type */
                                sizes[1],               /* size */
                                seL4_CapInitThreadCNode,/* root */
                                0,                      /* node_index */
                                0,                      /* node_depth */
                                child_ep,               /* node_offset */
                                1                       /* num_caps */
                                );
    ZF_LOGF_IF(error != seL4_NoError, "Failed to create child_ep");

    // 判断child_ep的ID
    uint32_t cap_id = seL4_DebugCapIdentify(child_ep);
    ZF_LOGF_IF(cap_id == 0, "Endpoint cap is null cap");
    ```
    编译运行，输出`Failed to bind notification`。我们需要创建通知对象了。

#### 4.4 创建一个通知对象

* 练习：在`child_untyped`中创建一个通知对象，存储在`child_ntfn`的CSlot中。

    ```c
    /* child_ep 存放在第4个CSlot位置 */
    seL4_CPtr child_ntfn = child_ep + 1;
    /* TODO：在child_ntfn槽中创建通知对象 */
    error = seL4_Untyped_Retype(child_untyped,          /* the untyped capability to retype */
                                objects[2],             /* type */
                                sizes[2],               /* size */
                                seL4_CapInitThreadCNode,/* root */
                                0,                      /* node_index */
                                0,                      /* node_depth */
                                child_ntfn,             /* node_offset */
                                1                       /* num_caps */
                                );
    ZF_LOGF_IF(error != seL4_NoError, "Failed to create child_ntfn");

    // 尝试使用child_ntfn
    error = seL4_TCB_BindNotification(child_tcb, child_ntfn);
    ZF_LOGF_IF(error != seL4_NoError, "Failed to bind notification.");
    ```

#### 4.5 删除对象

上面的代码编译运行，会报错`Failed to create endpoints.`。这是因为试图在`child_untyped`上创建足够多的端点对象，直到`child_untyped`消耗完。

但是，这失败了。因为我们前面创建的3个对象已经把`child_untyped`消耗完了。

* 练习：回收`child_untyped`，重新创建端点对象，直到消耗完。

```c
    // TODO: 回收`child_untyped`
    error = seL4_CNode_Revoke(seL4_CapInitThreadCNode,  /* seL4_CNode _service */
                              child_untyped,            /* seL4_Word index */
                              seL4_WordBits             /* seL4_Uint8 depth */
                              );
    ZF_LOGF_IF(error != seL4_NoError, "Failed to delete all objects.");

    // 将整个child_untyped分配为端点对象
    seL4_Word num_eps = BIT(untyped_size_bits - seL4_EndpointBits);
    error = seL4_Untyped_Retype(child_untyped,          /* the untyped capability to retype */
                                objects[1],             /* type */
                                sizes[1],               /* fixed size, so may be any value */
                                seL4_CapInitThreadCNode,/* root */
                                0,                      /* node_index */
                                0,                      /* node_depth */
                                child_tcb,              /* node_offset */
                                1                       /* num_caps */
                                );
    ZF_LOGF_IF(error != seL4_NoError, "Failed to create endpoints.");

    printf("Success\n");
```

编译运行，可以打印出`Success`。

## 5 高级练习

为了进一步熟悉`seL4`微内核关于`untyped`对象和内存分配，可以尝试下面的练习：

* 按照指定的物理地址分配对象；
* 创建一个简单的对象分配器；
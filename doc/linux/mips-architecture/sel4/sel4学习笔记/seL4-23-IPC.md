[TOC]

## 1 背景知识

1. 建立虚拟环境
2. 已经学习完能力课程

## 2 初始化课程环境

```shell
# 获取课程源代码的方法，请参考https://docs.sel4.systems/Tutorials/#get-the-code
#
# 获取课程的初始化构建目录
./init --tut ipc
# 进入练习的目录，进行编译
cd ipc_build
ninja
```

## 3 学习目标

1. 能够使用IPC机制，在进程之间发送数据和能力
2. 学习术语`cap transfer`
3. 能够通过标记能力，区分不同的请求
4. 了解如何设计使用IPC快速通道的协议

## 4 背景知识

`IPC`是微内核的重要机制，用来在进程之间同步传递少量数据和能力。`seL4`中，`IPC`机制构建于端点之上，端点是通用通信端口的概念。发送和接收`IPC`消息就是借助于对这些端点对象的调用。

端点由一组等待发送或接收消息的线程组成。什么意思呢？假设有N个线程正在等待某个端点上的消息，则一旦端点上有消息，则所有N个线程都会接收消息并被唤醒。此时如果有N+1个发送线程，则发送方会被存入缓存队列中。

#### 4.1 相关系统调用

线程基于端点发送消息的系统调用是`seL4_Send`，该系统调用发送完消息后会阻塞，直到消费线程处理完该消息。`seL4_NBSend`支持poll发送，只有接收方正在阻塞接收消息时，该发送才会成功，否则发送失败。为了简化处理，`seL4_NBSend`不会返回消息发送成功还是失败的结果。

对应的，`seL4_Recv`用来阻塞接收消息；`seL4_NBRecv`用来poll接收消息。

`seL4_Call`系统调用，本质上是`seL4_Send`和`seL4_Recv`的组合：发送方发送完消息后，阻塞等待`reply`能力，而不是端点本身。通俗的讲，`seL4_Send`系统调用相当于广播消息，不需要应答；而`seL4_Call`则相当于一对一确定性请求，发送一个消息，等待接收方回复。比如说，典型的CS架构下，客户端调用`seL4_Call`发出请求，服务器可以显式地回复正确的客户端。

`reply`能力存在接收线程的TCB块中。`seL4_Reply`调用该能力，发送IPC消息，唤醒客户端。`seL4_ReplyRecv`，其实是reply和recv的组合调用，做的工作与`seL4_Reply`相同，但是会阻塞在提供的端点上。

因为TCB只有一个简单的空间存储reply能力，假设服务器需要服务多个请求怎么办？（比如，保存某个需要reply的请求，等待硬件操作完成后再响应）这时候，可以调用`seL4_CNode_SaveCaller`可以将reply能力保存到接收线程能力空间的一个空槽中。

#### 4.2 IPC缓存

进程之间不可能只传递几个寄存器值，有时候可能需要传递较多的数据和能力。所以，微内核提供了IPC缓存，用于传递大于4个寄存器值的消息。发送线程需要指定消息长度，内核负责在发送和接收线程之间拷贝消息。

#### 4.3 数据传递

`IPC`缓存的最小单位是WORD字大小的寄存器，通常称之为`MR`。最大消息长度由宏`seL4_MsgMaxLength`定义，位于`libsel4`中。

加载消息到IPC缓存中，可以使用`seL4_SetMR`方法，提取的话使用`seL4_GetMR`方法。小消息（`<=4`）只使用寄存器传递，无需内存拷贝。如果想要改变可以使用寄存器传递的消息大小，修改宏`seL4_FastMessageRegisters`定义即可。

根据需要传递的消息寄存器数量，必须设置`seL4_MessageInfo_t`数据结构的`length`成员。

#### 4.4 能力传递

同数据传递一样，`IPC`可以在进程之间的消息传递中携带能力信息，这称之为`能力传递`。可以传递的能力数量存储在`seL4_MessageInfo_t`数据结构的`extraCaps`成员中。下面是一个通过`IPC`发送能力的示例：

```c
seL4_MessageInfo info = seL4_MessageInfo_new(0, 0, 1, 0);
seL4_SetCap(0, free_slot);
seL4_Call(endpoint, info);
```

为了接收能力，接收方必须指定一个能力空间地址存储能力。下面是一个代码示例：

```c
seL4_SetCapReceivePath(cnode, badged_endpoint, seL4_WordBits);
seL4_Recv(endpoint, &sender);
```
接收方对于接收的能力的访问权限必须和对应端点的访问权限相同。注意：如果发送方可以同时发送多个能力的时候，接收方一次只能接收一个。

#### 4.5 打开能力

（TODO: 该部分不太理解使用目的）

seL4 can also unwrap capabilities on IPC. If the n-th capability in the message refers to the endpoint through which the message is sent, the capability is unwrapped: its badge is placed into the n-th position of the receiver’s IPC buffer (in the field `caps_or_badges`), and the kernel sets the n-th bit (counting from the least significant) in the `capsUnwrapped` field of `seL4_MessageInfo_t`.


#### 4.6 消息描述结构体

`seL4_MessageInfo_t`数据结构用来描述一条消息，长度是WORD。其构成如下所示：

1. `length` 消息寄存器的数量，最大值为`seL4_MsgMaxLength`；
2. `extraCaps` 消息中的能力数量，最大值为`seL4_MsgMaxExtraCaps`；
3. `capsUnwrapped` 标记内核打开的能力；
4. `label` 标签数据，内核不会修改，直接从发送方传递到接收方。

#### 4.7 Badge

> Badge: 我们将该单词翻译为标记。大概的意思就是，某个线程给端点打上标记后，如果接收到某个消息，比较标记不一样，则认为不是自己应该接收的消息。

标记端点可以使用`seL4_CNode_Mint`或`seL4_CNode_Mutate`。

```c
seL4_Word badge;
seL4_Recv(endpoint, &badge);
// 一旦接收到消息，seL4就会将badge值设置为发送方发送消息时使用的能力标记。
```

#### 4.8 Fastpath

快速`IPC`对于微内核系统来说，是一个很重要的概念。为了进程隔离，服务之间往往是独立的，它们之间采用IPC通信机制进行通信。因此，IPC拥有一个快速通道，内核中一个高度优化的通道 - 为了使用快速通道，IPC必须具有如下条件：

1. 必须使用`seL4_Call`或`seL4_ReplyRecv`；
2. 消息中的数据必须小于`seL4_FastMessageRegisters`寄存器的数量；
3. 进程必须具有合法的地址空间；
4. 不传输能力；
5. 调度器中没有比`IPC`唤醒的线程更高优先级的线程正在运行。

## 5 练习

本教程使用`capDL`加载器建立几个进程：2个客户端和1个服务器端。所有进程都访问同一个端点能力。

server端回显client端发送的消息。You will also alter the ordering of replies from the clients to get the right message.

运行示例程序，得到如下所示的输出：

```shell
Client 2: waiting for badged endpoint
Badged 2
Assertion failed: seL4_MessageInfo_get_extraCaps(info) == 1 (../ipcCkQ6Ub/client_2.c: main: 22)
Client 1: waiting for badged endpoint
Badged 1
Assertion failed: seL4_MessageInfo_get_extraCaps(info) == 1 (../ipcCkQ6Ub/client_1.c: main: 22)
```


#### 5.1 练习任务1：标记端点并发送给客户端

建立能力传输，让客户端成功接收到标记的端点。

```c
// 能力槽，包含着IPC端点能力
extern seL4_CPtr endpoint;
// 能力槽，包含着指向服务器能力节点的能力
extern seL4_CPtr cnode;
// 空能力槽
extern seL4_CPtr free_slot;

int main(int c, char *argv[]) {

    seL4_Word sender;
    seL4_MessageInfo_t info = seL4_Recv(endpoint, &sender);
    while (1) {
        seL4_Error error;
        if (sender == 0) {

            /* 还未标记!回复给发送方一个标记的端点备份 */
            seL4_Word badge = seL4_GetMR(0);
            seL4_Error error = seL4_CNode_Mint( cnode,          /* _service */
                                                free_slot,      /* dest_index */
                                                seL4_WordBits,  /* destDepth */
                                                cnode,          /* src_root */
                                                endpoint,       /* src_index */
                                                seL4_WordBits,  /* src_depth */
                                                seL4_AllRights, /* rights */
                                                badge           /* badge */
                                                );
            printf("Badged %lu\n", badge);

            // TODO: 在reply中发送标记的端点能力备份
            seL4_SetCap(0, free_slot);
            info = seL4_MessageInfo_new(0, 0, 1, 0);

            /* 回复发送方，等待下一条MSG */
            seL4_Reply(info);

            /* 删除已经传输过的能力 */
            error = seL4_CNode_Delete(cnode, free_slot, seL4_WordBits);
            assert(error == seL4_NoError);

            /* 等待下一条消息 */
            info = seL4_Recv(endpoint, &sender);
        } else {

            // TODO use printf to print out the message sent by the client
            // followed by a new line

            // TODO reply to the client and wait for the next message
        }
    }

    return 0;
}
```
修改代码后，输出应该如下所示：

```
Booting all finished, dropped to user space
Client 2: waiting for badged endpoint
Badged 2
Client 1: waiting for badged endpoint
Badged 1
Client 2: received badged endpoint
Client 1: received badged endpoint
```

我们可以看到上面的打印中，最终系统挂起。这是因为客户端被设置了标记，服务器端再次接收到消息后，进入了else分支，但是没有响应导致的。

#### 5.2 练习任务2：打印客户端发送的消息

实现服务器的`echo`功能。

```c
// TODO: 使用printf打印客户端发送的消息，完成一次后，添加一个新行符。
for (int i = 0; i < seL4_MessageInfo_get_length(info); ++i)
{
    printf("%c", (char)seL4_GetMR(i));
}
printf("\n");
```

完成这一步，编译运行程序，可以看到循环输出单词`the`。

之所以一直打印`the`字符串是因为服务器端并没有响应客户端，而是一直循环打印客户端发送的消息；此时，客户端一直被阻塞。

#### 5.3 练习任务3：响应客户端

```c
// TODO: 使用printf打印客户端发送的消息，完成一次后，添加一个新行符。
for (int i = 0; i < seL4_MessageInfo_get_length(info); ++i)
{
    printf("%c", (char)seL4_GetMR(i));
}
printf("\n");
// TODO: 回复客户端，等待下一条消息
info = seL4_ReplyRecv(endpoint, info, &sender);
```
完成代码修改后，编译执行。可以看到如下所示的输出结果：

```
Client 2: received badged endpoint
the
brown
jumps
the
dog
Client 1: received badged endpoint
quick
fox
over
lazy
```

我们看到，上面的打印是一个客户端的消息发送完成后，才会打印另一个客户端的消息。如果想要两个客户端的消息交替打印，我们该如何处理呢？参考下面的代码：

```c
// TODO: 使用printf打印客户端发送的消息，完成一次后，添加一个新行符。
for (int i = 0; i < seL4_MessageInfo_get_length(info); ++i)
{
    printf("%c", (char)seL4_GetMR(i));
}
printf("\n");
// TODO: 回复客户端，等待下一条消息
/* 保存接收端reply能力到空槽中 */
error = seL4_CNode_SaveCaller(
            cnode,          /* _service (src->root) */
            free_slot,      /* index (src->capPtr) */
            seL4_WordBits   /* depth (src->capDepth) */
            );
assert(error == 0);
/* 接收客户端发送的消息（基于端点endpoint） */
info = seL4_Recv(endpoint, &sender);
for (int i = 0; i < seL4_MessageInfo_get_length(info); ++i)
{
    printf("%c", (char)seL4_GetMR(i));
}
printf("\n");
seL4_Send(free_slot, seL4_MessageInfo_new(0, 0, 0, 0));

info = seL4_ReplyRecv(endpoint, info, &sender);
```

上面的实现，会得到如下类似的结果：

```c
Client 2: received badged endpoint
the
Client 1: received badged endpoint
quick
fox
brown
jumps
over
lazy
the
dog
```

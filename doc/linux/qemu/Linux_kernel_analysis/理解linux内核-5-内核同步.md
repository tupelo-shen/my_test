<h1 id="0">0 目录</h1>

* [5 内核同步](#5)
    - [5.1 如何请求内核服务](#5.1)
        + [5.1.1 内核抢占](#5.1.1)
        + [5.1.2 什么时候需要同步？](#5.1.2)
    - [5.2 同步原语](#5.2)
        + [5.2.1 Per-CPU变量](#5.2.1)
        + [5.2.2 原子操作](#5.2.2)
        + [5.2.3 优化和内存屏障](#5.2.3)
        + [5.2.4 自旋锁](#5.2.4)
        + [5.2.5 读写自旋锁](#5.2.5)
        + [5.2.6 信号量](#5.2.6)
        + [5.2.7 Seqlock](#5.2.7)
        + [5.2.8 中断禁止](#5.2.8)
        + [5.2.9 软中断禁止](#5.2.9)
        + [5.2.10 读-拷贝-更新（RCU）](#5.2.10)
    - [5.3 内核数据结构的同步访问](#5.3)
    - [5.4 防止竞态条件的示例](#5.4)


<h1 id="5">5 内核同步</h1>

我们可以把内核想象成一个服务器，专门响应各种请求。这些请求可以是CPU上正在运行的进程发起的请求，也可以是外部的设备发起的中断请求。所以说，内核并不是串行运行，而是交错执行。既然是交错执行，就会产生`竞态条件`，我们可以采用同步技术消除这种竞态条件。

我们首先了解一下如何向内核请求服务。然后，看一下这些请求如何实现同步。Linux内核又是采用了哪些同步技术。

<h2 id="5.1">5.1 如何请求内核服务</h2>

为了更好地理解内核是如何工作的，我们把内核比喻成一个酒吧服务员，他响应两种请求服务：一种是来自顾客，另外一种来自多个老板。这个服务员采用的策略是：

1. 如果老板呼叫服务员，而服务员恰巧空闲，则立即服务老板；

2. 如果老板呼叫服务员，而服务员恰巧正在服务一名顾客。则服务员停止为顾客服务，而是去服务老板。

3. 如果老板呼叫服务员，而服务员恰巧在服务另一个老板，则服务员停止服务第一个老板，转而服务第二个。当他服务完第二个老板，再回去服务第一个老板。

4. 老板让服务员停止为顾客服务转而为自己服务。在处理完老板的最后一个请求后，服务员也可能会决定是临时性地放弃之前的顾客，而迎接新顾客。

上面的服务员就非常类似于处于内核态的代码执行。如果CPU被用户态程序占用，服务员被认为是空闲的。老板的请求就类似于中断请求，而顾客请求就对应于用户进程发出的系统调用或异常。后面描述中，异常处理程序指的是系统调用和常规异常的处理程序。

仔细研究，就会发现，前3条规则其实与内核中的异常和中断嵌套执行的规则是一样的。第4条规则就对应于内核抢占。

<h3 id="5.1.1">5.1.1 内核抢占</h3>

给内核抢占下一个完美定义很难。在这儿，我们只是尝试着给其下一个定义：如果一个进程正运行在内核态，此时，发生了进程切换我们就称其为抢占式内核。当然了，Linux内核不可能这么简单：

* 不论是抢占式内核还是非抢占式内核，进程都有可能放弃CPU的使用权而休眠等待某些资源。我们称这类进程切换是有计划的进程切换。但是抢占式内核和非抢占式的区别就在于对于异步事件的响应方式不同-比如，抢占式内核的中断处理程序可以唤醒更高优先级的进程，而非抢占式内核不会。我们称这类进程切换为强迫性的进程切换。

* 我们已经知道所有的进程切换动作都由`switch_to`宏完成。不论是抢占式还是非抢占式，当进程完成内核活动的某个线程并调用调度器时就会发生进程切换。但是，在非抢占式内核中，除非即将切换到用户态时，否则不会发生进程替换。

因此，抢占式内核主要的特性就是运行在内核态的进程可以被其它进程打断而发生替换。让我们举例说明抢占式内核和非抢占式内核的区别：

假设进程A正在执行异常处理程序（内核态），这时候中断请求IRQ发生，相应的处理程序唤醒高优先级的进程B。如果内核是可抢占式的，就会发生进程A到进程B的替换。异常处理程序还没有执行完，只有当调度器再一次选择进程A执行的时候才会继续。相反，如果内核是非抢占式的，除非进程A完成异常处理或者自愿放弃CPU的使用权，否则不会发生进程切换。

再比如，考虑正在执行异常处理程序的进程，它的CPU使用时间已经超时。如果内核是抢占式的，进程被立即切换；但是，如果内核是非抢占式的，进程会继续执行，知道进程完成异常处理或自动放弃CPU的使用权。

实施内核抢占的动机就是减少用户态进程的调度延时，也就是减少`可运行状态`到`真正运行时`的延时。需要实时调度的任务（比如外部的硬件控制器等）需要内核具有抢占性，因为减少了被其它进程延时的风险。

Linux内核是从2.6版本开始的，相比那些旧版本的非抢占性内核而言，没有什么显著的变化。当`thread_info`描述符中的`preempt_count`成员的值大于0，内核抢占就被禁止。这个值分为3部分，也就是说可能有3种情况导致该值大于0：

1. 内核正在执行中断服务例程（ISR）；
2. 延时函数被禁止（当内核执行软中断或tasklet时总是使能状态）；
3. 内核抢占被禁止。

通过上面的规则可以看出，内核只有在执行异常处理程序（尤其是系统调用）的时候才能够被抢占，而且内核抢占也没有被禁止。所以，CPU必须使能中断，内核抢占才能被执行。

下表是操作`prempt_count`数据成员的一些宏：

| Macro | 描述 |
| ----- | ----------- |
| preempt_count() | 选择`preempt_count` |
| preempt_disable() | 抢占计数加1 |
| preempt_enable_no_resched() | 抢占计数减1 |
| preempt_enable() | 抢占计数减1，如果需要调度，调用`preempt_schedule()` |
| get_cpu() | 与`preempt_disable()`相似，但是返回CPU的数量 |
| put_cpu() | 与`preempt_enable()`相似 |
| put_cpu_no_resched() | 与`preempt_enable_no_resched()`相似 |

`preempt_enable()`使能抢占，还会检查`TIF_NEED_RESCHED`标志是否设置。如果设置，说明需要进行进程切换，就会调用函数`preempt_schedule()`，其代码片段如下所示：

    if (!current_thread_info->preempt_count && !irqs_disabled()) {
        current_thread_info->preempt_count = PREEMPT_ACTIVE;
        schedule();
        current_thread_info->preempt_count = 0;
    }

可以看出，这个函数首先检查中断是否使能，以及抢占计数是否为0。如果条件为真，调用`schedule()`切换到其它进程运行。因此，内核抢占既可以发生在中断处理程序结束时，也可以发生在异常处理程序重新使能内核抢占时（调用`preempt_enable()`。<font color="red">也就是说，对于抢占式内核来说，进程切换发生的时机有，中断、系统调用、异常处理，还有一种特殊情况就是内核线程，它们直接调用schedule()进行主动进程切换。</font>

内核抢占不可避免地引入了更多的开销。基于这个原因，Linux2.6内核允许用户在编译内核代码的时候，通过配置，可以使能和禁止内核抢占。

<h3 id="5.1.2">5.1.2 什么时候需要同步技术？</h3>

我们先了解一下内核进程的竞态条件和临界区的概念。当计算结果依赖于两个嵌套的内核控制路径时就会发生竞态条件。而临界区就是每次只能一个内核控制路径可以进入的代码段。

内核控制路径的交错执行给内核开发者带来很大的麻烦：必须小心地在异常处理程序、中断处理程序、可延时处理函数和内核线程中确定临界区。一旦确定了哪些代码是临界区，就需要为这个临界区代码提供合适的保护，确保至多有一个内核控制路径可以访问它。

假设两个不同的中断处理程序需要访问相同的数据结构。所有影响数据结构的语句都必须放到一个临界区中。如果是单核处理系统，临界区的保护只需要关闭中断即可，因为内核控制路径的嵌套只有在中断使能的情况下会发生。

另一方面，如果不同的系统调用服务程序访问相同的数据，系统也是单核处理系统，临界区的保护只需要禁止内核抢占即可。

但是，在多核系统中事情就比较复杂了。**因为除了内核抢占，中断、异常或软中断之外，多个CPU也可能会同时访问某个相同的数据**。

后面我们会看一下内核提供了哪些内核同步手段？每种同步手段最合适的使用场景是什么？通过这些问题，我们掌握内核同步技术，为自己的内核程序设计最好的同步方法。

<h2 id="5.2">5.2 同步原语</h2>

表5-2，列举了Linux内核使用的一些同步技术。范围一栏表明同步技术应用到所有的CPU还是单个CPU。比如局部中断禁止就是针对一个CPU（系统中的其它CPU不受影响）；相反，原子操作影响所有的CPU。

表5-2 Linux内核使用的一些同步技术

| 技术 | 描述 | 范围 |
| --------- | ----------- | ----- |
| Per-CPU变量 | 用于在CPU之间拷贝数据 | 所有CPU |
| 原子操作 | 针对计数器的原子RMW指令 | 所有CPU |
| 内存屏障 | 避免指令乱序 | 本地CPU或所有CPU |
| 自旋锁 | 忙等待 | 所有CPU |
| 信号量 | 阻塞等待（休眠） | 所有CPU |
| Seqlock |   | 所有CPU |
| 中断禁止 | 禁止响应中断 | 本地CPU |
| 软中断禁止 | 禁止处理可延时函数 | 本地CPU |
| 读-拷贝-更新（RCU） | 通过指针实现无锁访问共享资源 | 所有CPU |

接下来，我们介绍各个同步技术。

<h3 id="5.2.1">5.2.1 Per-CPU变量</h3>

其实，最好的同步手段在于设计阶段就要尽量避免同步的需求。因为，毕竟同步的实现都是需要牺牲系统性能的。

既然多核系统中，CPU之间访问共享数据需要同步，那么最简单和有效的同步技术就是为每个CPU声明自己的变量，这样就减少了它们的耦合性，降低了同步的可能性。

**使用场景：**

一个CPU访问自己专属的变量，而无需担心其它CPU访问而导致的竞态条件。这意味着，`per-CPU`变量只能在特定情况下使用，比如把数据进行逻辑划分，然后分派给各个CPU的时候。

因为这些`per-CPU`变量全部元素都存储在内存上，所有的数据结构都会落在Cache的不同行上。所以，访问这些`per-CPU`变量不会导致对Cache行进行窥视（snoop）和失效（invalidate）操作，它们都对系统性能有很大的牺牲。

**缺点：**

1. 尽管，`per-CPU`变量保护了来自多个CPU的并发访问，但是无法阻止异步访问（比如，中断处理程序和可延时函数）。这时候，就需要其它同步技术了。

2. 此外，不管是单核系统还是多核系统，`per-CPU`变量都易于受到内核抢占所导致的竞态条件的影响。一般来说，内核控制路径访问每个CPU变量的时候，应该禁用内核抢占。假设，内核控制路径获得一个`per-CPU`变量的拷贝的地址，然后被转移到其它CPU上运行，这个值就可能会被其它CPU修改。

表5-3 列出了操作`per-CPU`变量的函数和宏

| 函数 | 描述 |
| ---- | ---- |
| DEFINE_PER_CPU(type, name)| 静态分配一个`per-CPU`数组 |
| per_cpu(name, cpu)        | 选择元素 |
| __get_cpu_var(name)       | 选择元素 |
| get_cpu_var(name)         | 禁止内核抢占，选择元素 |
| put_cpu_var(name)         | 使能内核抢占 |
| alloc_percpu(type)        | 动态分配一个`per-CPU`数组 |
| free_percpu(pointer)      | 释放动态分配的数组 |
| per_cpu_ptr(pointer, cpu) | 返回某个元素的地址 |

<h3 id="5.2.2">5.2.2 原子操作</h3>

汇编指令读写内存变量的过程我们称为`read-modify-write`，简称为RMW操作。也就是说，它们读写一个内存区域两次，第一次读取旧值，第二次写入新值。

假设有两个不同的内核控制路径运行在两个CPU上，同时尝试RMW操作相同的内存区域且执行的是非原子操作。起初，两个CPU尝试读取相同位置，但是内存仲裁器（促使串行访问RAM的电路）确定一个可以访问，让另一个等待。但是，当第一个读操作完成，延时的CPU也会读取相同的旧值。但是等到两个CPU都往这个内存区域写入新值的时候，还是由内存仲裁器决定谁先访问，然后写操作都会成功。但是，最终的结果却是最后写入的值，先写入的值会被覆盖掉。

防止RMW操作造成的竞态条件最简单的方式就是保证这样的指令操作是原子的，也就是这个指令的执行过程不能被打断。这就是原子操作的由来。

让我们看一下X86的汇编指令有哪些是原子的：

* 进行零或一对齐内存访问的汇编指令是原子的。

* RMW操作汇编指令（比如`inc`或`dec`），如果在read之后，write之前内存总线没有被其它CPU抢占，那么这些指令就是原子的。

* 所以，基于上一点，RMW操作汇编指令前缀`lock（0xf0）`就称为原子操作指令。当控制单元检测到这个前缀，它会锁住内存总线，直到指令完成。

* 带有前缀`rep`（0xf2、0xf3，强迫控制单元重复指令多次）的汇编指令就不是原子的。

但是，我们在编写完C代码后，编译器不能保证给你使用原子指令进行替代。因此，Linux内核提供了`atomic_t`类型变量并提供了相关的操作函数和宏（如表5-4所示）。多核系统中，会在每个指令的前面前缀`lock`。

表5-4 Linux中的原子操作

| 函数 | 描述 |
| ---- | ---- |
| atomic_read(v)            | 返回`*v` |
| atomic_set(v,i)           | *v=i |
| atomic_add(i,v)           | *v+i |
| atomic_sub(i,v)           | *v-i |
| atomic_sub_and_test(i, v) | 如果`*v-i = 0`，返回1；否则0 |
| atomic_inc(v)             | *v+1 |
| atomic_dec(v)             | *v-1 |
| atomic_dec_and_test(v)    | 如果`*v-1 = 0`，返回1；否则0 |
| atomic_inc_and_test(v)    | 如果`*v+1 = 0`，返回1；否则0 |
| atomic_add_negative(i, v) | 如果`*v+i < 0`，返回1；否则0  |
| atomic_inc_return(v)      | 返回`*v-1` |
| atomic_dec_return(v)      | 返回`*v+i` |
| atomic_add_return(i, v)   | 返回`*v-i` |


<h3 id="5.2.3">5.2.3 优化和内存屏障</h3>

带有优化的编译器，会尝试重新排序汇编指令，以提高程序的执行速度。但是，当在处理同步问题的时候，重新排序的指令应该被避免。因为重新排序可能会打乱我们之前想要的同步效果。因此，所有的同步原语都充当优化和内存屏障。

优化屏障保证屏障原语前后的C语言转换成汇编语言之后，指令序列不会发生变化。比如说，对于Linux内核的`barrier()`宏，展开后就是`asm volatile("":::"memory")`，就是一个优化屏障。`asm`告知编译器插入一条汇编指令，`volatile`关键字禁止编译器用程序的其它指令重新洗牌`asm`指令。`memory`关键字强迫编译器假设RAM中所有的位置都被汇编指令更改了；因此，编译器不会使用CPU寄存器中的值优化`asm`指令之前的代码。我们需要注意的是优化屏障不能保证汇编指令的执行不会乱序，这是由内存屏障保障的。

内存屏障确保屏障原语前的指令完成后，才会启动原语之后的指令操作。

X86系统中，下面这些汇编指令都是串行的，可以充当内存屏障：

* 所有操作I/O端口的指令；
* 前缀`lock`的指令；
* 所有写控制寄存器，系统寄存器或debug寄存器的指令（比如，`cli`和`sti`指令，可以改变`eflags`寄存器的IF标志）；
* `lfence`、`sfence`和`mfence`汇编指令，分别用来实现读内存屏障、写内存屏障和读/写内存屏障；
* 特殊的汇编指令，比如`iret`指令，可以终止中断或异常处理程序。

> ARM系统中，使用`ldrex`和`strex`汇编指令实现内存屏障。

Linux内核中使用的内存屏障原语如下，如表5-6所示。当然了，这些原语完全可以作为优化屏障，阻止编译器优化该屏障前后的汇编指令。读内存屏障只对内存的读操作指令有效；写内存屏障只对内存的写操作指令有效。`smp_xxx()`之类的内存屏障只对发生在多核系统里的竞态条件有效，单核系统中，什么也没有做。其它的内存屏障对多核系统和单核系统都有效。

表5-6 Linux内存屏障

| macro | 描述 |
| ----- | ---- |
| mb()      | MP和UP的内存屏障 |
| rmb()     | MP和UP的读内存屏障 |
| wmb()     | MP和UP的写内存屏障 |
| smp_mb()  | MP内存屏障 |
| smp_rmb() | MP读内存屏障 |
| smp_wmb() | MP写内存屏障 |

内存屏障的实现跟系统架构息息相关。在X86系统上，如果支持`lfence`汇编指令，则`rmb()`实现为：

    asm volatile("lfence":::"memory")

如不支持`lfence`汇编指令，则`rmb()`实现为：

    asm volatile("lock;addl $0,0(%%esp)":::"memory")

`asm volatile`的作用之前的文章已经介绍过，不再赘述。`lock;addl $0,0(%%esp)":::"memory"`的意思是，对栈顶保存的内存地址内的内容加上0，所以这条命令本身没有意义，主要还是`lock`前缀，对数据总线加锁，从而使该条指令对CPU而言，称为内存屏障。

而`wmb()`的实现事实上非常简单，就是`barrier()`的宏声明。这是因为，现有的Intel处理器不会对写内存访问进行重新排序，所以无法插入特定的内存屏障指令。但是，该宏还是会禁止编译器打乱指令。

值得注意的是多核处理器中，所有的原子操作指令都会前缀`lock`，所以都可以充当内存屏障。

总结：

内存屏障主要解决的还是硬件数据总线上对于指令的读取可能会发生乱序问题。

<h3 id="5.2.4">5.2.4 自旋锁</h3>

使用最广泛的同步技术就是`加锁`。对于锁概念，我相信大家已经不陌生了，不论是实时嵌入式系统还是服务器上的操作系统，都使用了这个概念。所以对于锁的理解就不再赘述了。

自旋锁是设计用来在多核系统中工作的一种特殊锁。如果内核控制路径发现自旋锁空闲，则申请加锁然后执行。相反，如果发现锁已经被其它CPU上的内核控制路径占用，它就会一直`自旋`，就是在循环查看锁是否已经释放，直到该锁被释放。

自旋锁的自旋过程就是一个忙等待的过程。也就是说，正在等待的内核控制路径正在浪费时间，因为什么也不干。但是，大部分的内核资源加锁的时间可能仅为毫秒的几分之一，因此，释放CPU使用权再获取可能比一直等待更消耗时间。所以，自旋锁使用的场合就是，**内核资源的占用时间一般比较短，且是多核系统的时候**。

<h3 id="5.2.4.1">5.2.4.1 自旋锁结构实现</h3>

Linux内核系统中，自旋锁`spinlock_t`的实现主要使用了`raw_spinlock_t`结构，这个结构的实现，参考下面的代码：

    typedef struct raw_spinlock {
        arch_spinlock_t raw_lock;
    #ifdef CONFIG_GENERIC_LOCKBREAK
        unsigned int break_lock;
    #endif
        ...
    } raw_spinlock_t;
    typedef struct spinlock {
        union {
            struct raw_spinlock rlock;
            ...
        };
    } spinlock_t;

上面的代码中，核心的数据成员是`raw_lock`和`break_lock`。对于`raw_lock`来说，其类型为`arch_spinlock_t`，从名称上也能看出，这个结构是跟体系结构相关的。

* raw_lock

    表示自旋锁的状态，依赖于具体的架构实现。

* break_lock

    标志着进程正在忙等待锁（仅当内核同时支持SMP和内核抢占时才会出现）。

接下来，我们分析加锁的流程。

<h4 id="5.2.4.1">5.2.4.1 spin_lock()函数</h4>

本章我们直接看源代码，用函数出现的顺序表示函数调用的顺序。首先，看加锁的函数为:

    static __always_inline void spin_lock(spinlock_t *lock)
    {
        raw_spin_lock(&lock->rlock);
    }

`raw_spin_lock`函数的代码如下：

    #define raw_spin_lock(lock) _raw_spin_lock(lock)

`_raw_spin_lock`函数分为2个版本：SMP版本和UP版本。

1. UP版本的代码如下：

    `_raw_spin_lock`函数：

        #define _raw_spin_lock(lock)            __LOCK(lock)

    `__LOCK`函数代码如下：

        #define __LOCK(lock) \
                do { preempt_disable(); ___LOCK(lock); } while (0)

    可以看出，首先禁止内核抢占。然后调用

        #define ___LOCK(lock) \
                do { __acquire(lock); (void)(lock); } while (0)

    从上面的代码可以看出，单核系统没有处于debug状态时，没有真正的锁在运行。因此，就是禁止了内核抢占。至于`void`是避免编译器对未使用的锁变量发出警告。`__acquire(lock)`就是给检查器（CHECKER）添加适当的注释。真正的定义就是`# define __acquire(x) (void)0`。

2. SMP版本的代码如下：

    `_raw_spin_lock`函数：

        static inline void __raw_spin_lock(raw_spinlock_t *lock)
        {
            // 禁止内核抢占
            preempt_disable();
            // debug用
            spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
            // 真正申请锁的地方
            LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
        }

    `LOCK_CONTENDED`是一个通用的加锁流程。`do_raw_spin_trylock`和`do_raw_spin_lock`的实现依赖于具体的体系结构，以`x86`为例，`do_raw_spin_trylock`最终调用的是：

        static inline int do_raw_spin_trylock(raw_spinlock_t *lock)
        {
            return arch_spin_trylock(&(lock)->raw_lock);
        }

    `arch_spin_trylock`函数的实现依赖于具体的体系架构，以X86为例，代码如下：

        typedef struct arch_spinlock {
            union {
                __ticketpair_t head_tail;
                struct __raw_tickets {
                    __ticket_t head, tail; // 注意，x86使用的是小端模式，存在高地址空间的是tail
                } tickets;
            };
        } arch_spinlock_t;

        static __always_inline int arch_spin_trylock(arch_spinlock_t *lock)
        {
            arch_spinlock_t old, new;

            // 获取旧的ticket信息
            old.tickets = READ_ONCE(lock->tickets);
            // head和tail不一致，说明锁正在被占用，加锁不成功
            if (!__tickets_equal(old.tickets.head, old.tickets.tail))
                return 0;

            // 将tail + 1
            new.head_tail = old.head_tail + (TICKET_LOCK_INC << TICKET_SHIFT);
            new.head_tail &= ~TICKET_SLOWPATH_FLAG;

            /* cmpxchg是一个完整的内存屏障 */
            return cmpxchg(&lock->head_tail, old.head_tail, new.head_tail) == old.head_tail;
        }

    从上述代码中可知，`arch_spin_trylock`的核心功能，就是判断自旋锁是否被占用，如果没被占用，尝试原子性地更新lock中的`head_tail`的值，将tail+1，返回是否加锁成功。

    不考虑CONFIG_DEBUG_SPINLOCK宏的话，` do_raw_spin_lock`的源代码如下：

        static inline void do_raw_spin_lock(raw_spinlock_t *lock) __acquires(lock)
        {
            __acquire(lock);
            arch_spin_lock(&lock->raw_lock);
        }

    `arch_spin_lock`函数的源代码：

        static __always_inline void arch_spin_lock(arch_spinlock_t *lock)
        {
            register struct __raw_tickets inc = { .tail = TICKET_LOCK_INC };

            // 原子性地把ticket中的tail+1，返回的inc是+1之前的原始值
            inc = xadd(&lock->tickets, inc);
            if (likely(inc.head == inc.tail))
                goto out;

            for (;;) {
                unsigned count = SPIN_THRESHOLD;

                do {
                    // 读取新的head值
                    inc.head = READ_ONCE(lock->tickets.head);
                    if (__tickets_equal(inc.head, inc.tail))
                        goto clear_slowpath;
                    cpu_relax();
                } while (--count);
                __ticket_lock_spinning(lock, inc.tail);
            }
        // 循环直到head和tail相等
        clear_slowpath:
            __ticket_check_and_clear_slowpath(lock, inc.head);
        out:
            barrier();  /* make sure nothing creeps before the lock is taken */
        }

    `__ticket_check_and_clear_slowpath`函数执行的操作是把tail加1，并把之前的值记录下来，完成加锁操作。

        static inline void __ticket_check_and_clear_slowpath(arch_spinlock_t *lock,
                                    __ticket_t head)
        {
            if (head & TICKET_SLOWPATH_FLAG) {
                arch_spinlock_t old, new;

                old.tickets.head = head;
                new.tickets.head = head & ~TICKET_SLOWPATH_FLAG;
                old.tickets.tail = new.tickets.head + TICKET_LOCK_INC;
                new.tickets.tail = old.tickets.tail;

                /* try to clear slowpath flag when there are no contenders */
                cmpxchg(&lock->head_tail, old.head_tail, new.head_tail);
            }
        }

    至此，就完成了申请锁的操作。接下来我们再来研究一下，解锁流程。

<h4 id="5.2.4.3">5.2.4.3 spin_unlock函数</h4>

对于SMP架构来说，`spin_unlock`最终调用的是`__raw_spin_unlock`，其源代码如下：

    static inline void __raw_spin_unlock(raw_spinlock_t *lock)
    {
        spin_release(&lock->dep_map, 1, _RET_IP_);
        do_raw_spin_unlock(lock);   // 完成主要的解锁工作
        preempt_enable();           // 启动抢占
    }

    static inline void do_raw_spin_unlock(raw_spinlock_t *lock) __releases(lock)
    {
        arch_spin_unlock(&lock->raw_lock);
        __release(lock);
    }

`arch_spin_unlock`函数的代码如下：

    static __always_inline void arch_spin_unlock(arch_spinlock_t *lock)
    {
        if (TICKET_SLOWPATH_FLAG &&
            static_key_false(&paravirt_ticketlocks_enabled)) {
            __ticket_t head;

            BUILD_BUG_ON(((__ticket_t)NR_CPUS) != NR_CPUS);

            // 主要内容：将head+1；所以现在head>tail表示锁又空闲了。
            head = xadd(&lock->tickets.head, TICKET_LOCK_INC);

            if (unlikely(head & TICKET_SLOWPATH_FLAG)) {
                head &= ~TICKET_SLOWPATH_FLAG;
                __ticket_unlock_kick(lock, (head + TICKET_LOCK_INC));
            }
        } else
            __add(&lock->tickets.head, TICKET_LOCK_INC, UNLOCK_LOCK_PREFIX);
    }

所以，解锁的过程就是将head和tail不相等，且重新使能内核抢占的过程。

<h3 id="5.2.5">5.2.5 `读/写自旋锁`</h3>

自旋锁解决了多核系统在内核抢占模式下的数据共享问题。但是，这样的自旋锁一次只能一个内核控制路径使用，这严重影响了系统的并发性能。根据我们以往的开发经验，大部分的程序都是读取共享的数据，并不更改；只有少数时候会修改数据。为此，Linux内核提出了`读/写自旋锁`的概念。也就是说，没有内核控制路径修改共享数据的时候，多个内核控制路径可以同时读取它。如果有内核控制路径想要修改这个数据结构，它就申请`读/写自旋锁`的写自旋锁，独占访问这个资源。这大大提高了系统的并发性能。

`读/写自旋锁`的数据结构是`rwlock_t`，其定义如下：

    typedef struct {
        arch_rwlock_t raw_lock;
    #ifdef CONFIG_GENERIC_LOCKBREAK
        unsigned int break_lock;
    #endif
        ......
    } rwlock_t;

从上面的代码可以看出，`读/写自旋锁`的实现还是依赖于具体的架构体系。下面我们先以ARM体系解析一遍：

1. `arch_rwlock_t`的定义：

        typedef struct { 
            u32 lock; 
        } arch_rwlock_t;

2. 申请写自旋锁`arch_write_lock`的实现：

        static inline void arch_write_lock(arch_rwlock_t *rw)
        {
            unsigned long tmp;

            prefetchw(&rw->lock);   // ----------（0）
            __asm__ __volatile__(
        "1: ldrex   %0, [%1]\n"     // ----------（1）
        "   teq %0, #0\n"           // ----------（2）
            WFE("ne")               // ----------（3）
        "   strexeq %0, %2, [%1]\n" // ----------（4）
        "   teq %0, #0\n"           // ----------（5）
        "   bne 1b"                 // ----------（6）
            : "=&r" (tmp)
            : "r" (&rw->lock), "r" (0x80000000)
            : "cc");

            smp_mb();               // ----------（7）
        }

    * （0）通知硬件提前将`rw->lock`的值加载到cache中，缩短等待预取指令的时延。
    * （1）使用独占指令`ldrex`标记相应的内存位置已经被独占，并将其值存储到tmp变量中。
    * （2）判断tmp是否等于0。
    * （3）如果tmp不等于0，则说明rw->lock正在被占用，所以进入低功耗待机模式。
    * （4）如果tmp等于0，则向rw->lock的内存地址处写入`0x80000000`，然后清除独占标记。
    * （5）测试tmp是否等于0，相当于验证第4步是否成功。
    * （6）如果加锁失败，则重新（0）->（5）的过程。
    * （7）现在只是把指令写入到数据总线上，还没有完全成功。所以`smp_mb()`内存屏障保证加锁成功。

3. 写自旋锁的是否过程，`arch_write_unlock`函数实现，代码如下：

        static inline void arch_write_unlock(arch_rwlock_t *rw)
        {
            smp_mb();           // ----------（0）

            __asm__ __volatile__(
            "str    %1, [%0]\n" // ----------（1）
            :
            : "r" (&rw->lock), "r" (0)
            : "cc");

            dsb_sev();          // ----------（2）
        }

    * （0）保证释放锁之前的操作都完成。
    * （1）将`rw->lock`的值赋值为0。
    * （2）调用sev指令，唤醒正在执行WFE指令的内核控制路径。

4. 读自旋锁的申请过程（低功耗版）由`arch_read_lock`函数实现，代码如下：

        static inline void arch_read_lock(arch_rwlock_t *rw)
        {
            unsigned long tmp, tmp2;

            prefetchw(&rw->lock);
            __asm__ __volatile__(
        "1: ldrex   %0, [%2]\n"     // ----------（0）
        "   adds    %0, %0, #1\n"   // ----------（1）
        "   strexpl %1, %0, [%2]\n" // ----------（2）
            WFE("mi")               // ----------（3）
        "   rsbpls  %0, %1, #0\n"   // ----------（4）
        "   bmi 1b"                 // ----------（5）
            : "=&r" (tmp), "=&r" (tmp2)
            : "r" (&rw->lock)
            : "cc");

            smp_mb();
        }

    * （0）读取`rw->lock`地址处的内容，然后标记为独占。
    * （1）tmp=tmp+1。
    * （2）将这条指令的执行结果写入到tmp2变量中，将tmp的值写入到`rw->lock`地址处。
    * （3）如果tmp是负值，说明锁已经被占有，则执行wfe指令，进入低功耗待机模式。
    * （4）执行0减去tmp2，将结果写入tmp。因为tmp2的值有2个：0-更新成功；1-更新失败。所以正常情况，此时tmp的结果应该为0，也就是申请加锁成功。
    * （5）如果加锁失败，则重新进行（0）->（4）的操作。失败的可能就是，独占标记被其它加锁操作破会。

5. 读自旋锁的申请过程（不断尝试版）由`arch_read_trylock`函数实现，代码如下：

        static inline int arch_read_trylock(arch_rwlock_t *rw)
        {
            unsigned long contended, res;

            prefetchw(&rw->lock);
            do {
                __asm__ __volatile__(
                "   ldrex   %0, [%2]\n"     // ----------（0）
                "   mov %1, #0\n"           // ----------（1）
                "   adds    %0, %0, #1\n"   // ----------（2）
                "   strexpl %1, %0, [%2]"   // ----------（3）
                : "=&r" (contended), "=&r" (res)
                : "r" (&rw->lock)
                : "cc");
            } while (res);                  // ----------（4）

            /* 如果lock为负，则已经处于write状态 */
            if (contended < 0x80000000) {   // ----------（5）
                smp_mb();
                return 1;
            } else {
                return 0;
            }
        }

    * （0）读取`rw->lock`地址处的内容，然后标记为独占。
    * （1）res = 0。
    * （2）contended = contended + 1。
    * （3）将contended的值写入`rw->lock`地址处，操作结果写入res。
    * （4）如果res等于0，操作成功；否则重新前面的操作。
    * （5）如果此时处于write状态，则申请锁失败，返回1；否则，成功返回0。

    根据4和5两个申请锁的过程分析，可以看出除了是否根据需要进入低功耗状态之外，其它没有区别。

6. 读自旋锁的释放过程由`arch_read_unlock`函数实现，代码如下：

        static inline void arch_read_unlock(arch_rwlock_t *rw)
        {
            unsigned long tmp, tmp2;

            smp_mb();

            prefetchw(&rw->lock);
            __asm__ __volatile__(
        "1: ldrex   %0, [%2]\n"     // ----------（0）
        "   sub %0, %0, #1\n"       // ----------（1）
        "   strex   %1, %0, [%2]\n" // ----------（2）
        "   teq %1, #0\n"           // ----------（3）
        "   bne 1b"                 // ----------（4）
            : "=&r" (tmp), "=&r" (tmp2)
            : "r" (&rw->lock)
            : "cc");

            if (tmp == 0)
                dsb_sev();
        }

    * （0）读取`rw->lock`地址处的内容，然后标记为独占。
    * （1）要退出临界区，所以，tmp = tmp - 1。
    * （2）tmp写入到`rw->lock`地址处，操作结果写入tmp2。
    * （3）判断tmp2是否等于0。
    * （4）等于0成功，不等于0，则跳转到标签1处继续执行。

通过上面的分析可以看出，读写自旋锁使用bit31表示写自旋锁，bit30-0表示读自旋锁，对于读自旋锁而言，绰绰有余了。

对于另一个成员`break_lock`来说，同自旋锁数据结构中的成员一样，标志锁的状态。

`rwlock_init`宏初始化读写锁的lock成员。

对于X86系统来说，处理的流程差不多。但是，因为与ARM架构体系不同，所以具体的加锁和释放锁的实现是不一样的。在此，就不一一细分析了。

<h3 id="5.2.6">5.2.6 Seqlock</h3>

1. 什么是seqlock锁？

上一篇文章中，我们已经学习了读/写自旋锁的工作原理和实现方式（基于ARM架构体系）。但是，有一个问题我们不得不考虑，那就是read锁和write锁的优先级问题：它们具有相同的优先级，所以，读操作必须等到写操作完成后才能执行，同样，写操作必须等到读操作完成后才能执行。

Linux2.6内核版本引入了`Seqlock`锁，与读写自旋锁基本一样，只是对于写操作来说，具有更高的优先级；也就是说，即使现在读操作正在执行，写操作也会被立即执行。这个策略的优点就是，写操作绝不会等待（除非是有其它写操作在占用锁）；缺点就是，读操作可能需要读取多次，才能获取正确的备份。

2. seqlock锁实现

`seqlock`锁的数据结构如下所示，包含两个数据成员`lock`和`seqcount`。查看代码可知，`seqlock`锁就是一个自旋锁加上一个序列计数器。

        typedef struct {
            struct seqcount seqcount;       // 序列计数器
            spinlock_t lock;
        } seqlock_t;

`seqlock`锁的工作原理是，对于读操作而言，每次读取数据前后，都要读取序列计数器2次，检查这前后两次的值是否一致，一致则认为可以使用锁。相反，如果一个新的写操作开始工作，增加序列计数器的值，隐含地告知读操作刚刚读到的数据不合法，需要重新读取数据。

`seqlock_t`类型变量初始化的方法有两种：一种是直接赋值`SEQLOCK_UNLOCKED`，另外一种是调用`seqlock_init`宏。写操作分别申请锁和释放锁，分别调用`write_seqlock()`和`write_sequnlock()`。申请锁的过程是，申请`seqlock_t`数据结构中的自旋锁，并对序列计数器进行加一操作。释放锁的过程是，再一次对序列计数器进行加一操作，并释放掉自旋锁。这样操作的结果就是，写操作过程中，计数器的计数是奇数；没有写操作的时候，计数器是偶数。

3. seqlock锁使用范例

对于读操作来说，大概的代码实现如下所示：

        unsigned int seq;
        do {
            seq = read_seqbegin(&seqlock);
            /* ... 临界代码段 ... */
        } while (read_seqretry(&seqlock, seq));

`read_seqbegin()`获取锁的当前序列号。`read_seqretry()`判断序列号是否一致，如果seq的值是奇数，则会返回1，也就是条件为真（也就是说，`read_seqbegin()`函数被调用之后，有写操作更新了数据）。因此，需要重新读取数据。如果seq的值是偶数，则读取数据成功。

值得注意的是，当读操作进入临界代码段时，无需禁止内核抢占。因为，我们允许写操作打断读操作的执行，这也是Seqlock锁写操作优先级高的设计初衷。但是，写操作进入临界代码段时，会自动禁止内核抢占。

4. seqlock锁使用场合

并不是所有的数据结构都能使用seqlock锁保护。因为seqlock锁有自身的缺点：因为写操作的优先级高于读操作，所以，对于写操作负荷比较的重的场合来说就不合适。如果写操作过于频繁，那么对读操作来说极为不公平，可能需要多次读取数据才能成功。所以，使用seqlock锁的场合应该满足下面的条件：

* 要保护的数据结构不能包含指针，而且这些指针写操作修改，读操作进行引用。因为可能写操作修改了指针，而读操作还会引用之前的指针。

* 要保护的数据结构必须是特别短小的代码，而且读操作比较频繁，写操作很少且非常快。（这也是读写自旋锁的使用原则）

* 读操作的临界代码段中的代码不能有副作用（否则，多次读操作可能与单次读取有不同的效果）。

典型应用可以参考linux内核关于系统时间处理的部分。

<h3 id="5.2.7">5.2.7 读-拷贝-更新（RCU）</h3>

每一种技术的出现必然是因为某种需求。正因为人的本性是贪婪的，所以科技的创新才能日新月异。

1. 引言

seqlock锁只能允许一个写操作，但是有些时候我们可能需要多个写操作可以并发执行。所以，Linux内核引入了读-拷贝-更新技术（英文是`Read-copy update`，简称RCU），它是另外一种同步技术，主要用来保护被多个CPU读取的数据结构。RCU允许多个读操作和多个写操作并发执行。更重要的是，RCU是一种免锁算法，也就是说，它没有使用共享的锁或计数器保护数据结构（但是，这儿还是主要指的读操作是无锁算法。而对于多个写操作来说，需要使用lock保护避免多个CPU的并发访问。所以，其使用场合也是比较严格的，多个写操作中的锁开销不能大于读操作采用无锁算法省下的开销）。这相对于读写自旋锁和seqlock来说，具有很大的优势，毕竟锁的申请和释放对Cache行的"窥视"和失效也是一个很大的负担。

> 1. Cache行的"窥视"，指的是因为每个CPU具有局部Cache，所以硬件snoop单元必须时时刻刻在"窥视"所有的Cache行，并对其不合法的数据进行失效处理，重新从内存获取数据替换到相应的Cache行中。而在这里，如果使用了共享的lock或者计数器，那么每次对其进行写操作，必然导致相应Cache行的失效。然后重新把使用这个lock的CPU的局部Cache进行更新。

2. RCU实现

既然RCU没有使用共享数据结构，那么它是如何神奇地实现同步技术的呢？其核心思想就是限制RCU的使用范围：

* 1. 只有动态分配的、通过指针进行访问的数据结构。
* 2. 进入RCU保护的临界代码段的内核控制路径不能休眠。

3. 基本操作

* 对于reader，RCU的基本操作为：
    
    * （1）调用`rcu_read_lock()`，进入RCU保护的临界代码段。等价于调用`preempt_disable()`。
    * （2）调用`rcu_dereference`，获取RCU保护的数据指针。然后通过该指针读取数据。当然了，在此期间读操作不能发生休眠。
    * （3）调用`rcu_read_unlock()`，离开RCU保护的临界代码段。等价于调用` preempt_enable()`。 

* 对于writer，RCU的基本操作为：

    * （1）拷贝一份旧数据到新数据，修改新数据。

    * （2）调用`rcu_assign_pointer()`，将RCU保护的指针修改为新数据的指针。
    
        因为指针的修改是一个原子操作，所以不会发生读写不一致的问题。但是，需要插入一个内存屏障保证只有在数据被修改完成后，其它CPU才能看见更新的指针。尤其是当使用了自旋锁保护RCU禁止多个写操作的并发访问的时候。

    * （3）调用`synchronize_rcu`，等待所有的读操作都离开临界代码段，完成同步。

        RCU技术的真正问题是当写操作更新了指针后，旧数据的存储空间不能立马释放。因为，这时候读操作可能还在读取旧数据，所以，必须等到所有的可能的读操作执行`rcu_read_unlock()`离开临界代码段后，旧数据的存储空间才能被释放。

    * （4）调用`call_rcu()`，完成旧数据存储空间的回收工作。

        该函数的参数是类型为`rcu_head`的描述符的地址。该描述符嵌入在要回收的数据结构的内部。该函数还有一个参数就是一个回调函数，当所有的CPU处于空闲状态的时候执行这个回调函数。这个函数通常是负责旧数据存储空间的释放工作。

        有一个问题需要注意的是，这个回调函数的执行是在另一个内核线程中执行。`call_rcu()`函数吧回调函数的地址和其参数存储在`rcu_head`描述符中，然后将这个描述符插入到每个CPU的回调函数列表中（这儿又体现了`per-CPU变量`的重要性）。每个系统时间滴答，内核都会检查局部CPU是否处于空闲状态。当所有的CPU处于空闲状态的时候，一个特殊的tasklet就会执行所有的回调函数，这个tasklet描述符存储在每个CPU的rcu_tasklet变量中。

4. 使用场合

    RCU是从Linux2.6版本引入的，主要使用在网络层和虚拟文件系统层。

<h3 id="5.2.8">5.2.8 信号量</h3>

对于信号量我们并不陌生。本质上，就是提供了一个锁，允许锁的等待着进入休眠，直到想要的资源被释放。事实上，Linux提供了两类信号量：

* 内核使用的信号量
* 用户态使用的信号量（遵循System V IPC信号量要求）

在本文中，我们集中研究内核信号量，至于进程间通信使用的信号量以后再分析。所以，后面再提及的信号量指的是内核信号量。

信号量与自旋锁及其类型，不同之处是使用自旋锁的话，获取锁失败的时候，进入忙等待状态，也就是一直在自旋。而使用信号量的话，如果获取信号量失败，则相应的进程会被挂起，知道资源被释放，相应的进程就会继续运行。因此，信号量只能由那些允许休眠的程序可以使用，像中断处理程序和可延时函数等不能使用。

A kernel semaphore is an object of type struct semaphore, containing the fields shown in the following list.
信号量的结构体是`semaphore`，包含下面的成员：

* count

Stores an atomic_t value. If it is greater than 0, the resource is free—that is, it is currently available. If count is equal to 0, the semaphore is busy but no other process is waiting for the protected resource. Finally, if count is negative, the resource is unavailable and at least one process is waiting for it.

* wait

Stores the address of a wait queue list that includes all sleeping processes that are currently waiting for the resource. Of course, if count is greater than or equal to 0, the wait queue is empty.

* sleepers

Stores a flag that indicates whether some processes are sleeping on the semaphore. We’ll see this field in operation soon.

The init_MUTEX() and init_MUTEX_LOCKED() functions may be used to initialize a semaphore for exclusive access: they set the count field to 1 (free resource with exclusive access) and 0 (busy resource with exclusive access currently granted to the process that initializes the semaphore), respectively. The DECLARE_MUTEX and DECLARE_MUTEX_LOCKED macros do the same, but they also statically allocate the struct semaphore variable. Note that a semaphore could also be initialized with an arbitrary positive value n for count. In this case, at most n processes are allowed to concurrently access the resource.

<h4 id="5.2.8.1">5.2.8.1 获取和释放信号量</h4>

Let’s start by discussing how to release a semaphore, which is much simpler than getting one. When a process wishes to release a kernel semaphore lock, it invokes the up() function. This function is essentially equivalent to the following assembly language fragment:

        movl $sem->count,%ecx
        lock; incl (%ecx)
        jg 1f
        lea %ecx,%eax
        pushl %edx
        pushl %ecx
        call __up
        popl %ecx
        popl %edx
    1:

where __up() is the following C function:
    
    __attribute__((regparm(3))) void __up(struct semaphore *sem)
    {
        wake_up(&sem->wait);
    }

The up() function increases the count field of the *sem semaphore, and then it checks
whether its value is greater than 0. The increment of count and the setting of the flag
tested by the following jump instruction must be atomically executed, or else
another kernel control path could concurrently access the field value, with disastrous
results. If count is greater than 0, there was no process sleeping in the wait
queue, so nothing has to be done. Otherwise, the _ _up() function is invoked so that
one sleeping process is woken up. Notice that _ _up() receives its parameter from the
eax register (see the description of the _ _switch_to() function in the section “Performing
the Process Switch” in Chapter 3).

Conversely, when a process wishes to acquire a kernel semaphore lock, it invokes the
down( ) function. The implementation of down( ) is quite involved, but it is essentially
equivalent to the following:

        down:
        movl $sem->count,%ecx
        lock; decl (%ecx);
        jns 1f
        lea %ecx, %eax
        pushl %edx
        pushl %ecx
        call __down
        popl %ecx
        popl %edx
    1:

where __down() is the following C function:

    __attribute__((regparm(3))) void __down(struct semaphore * sem)
    {
        DECLARE_WAITQUEUE(wait, current);
        unsigned long flags;
        current->state = TASK_UNINTERRUPTIBLE;
        spin_lock_irqsave(&sem->wait.lock, flags);
        add_wait_queue_exclusive_locked(&sem->wait, &wait);
        sem->sleepers++;
        for (;;) {
            if (!atomic_add_negative(sem->sleepers-1, &sem->count)) {
                sem->sleepers = 0;
                break;
            }
            sem->sleepers = 1;
            spin_unlock_irqrestore(&sem->wait.lock, flags);
            schedule();
            spin_lock_irqsave(&sem->wait.lock, flags);
            current->state = TASK_UNINTERRUPTIBLE;
        }
        remove_wait_queue_locked(&sem->wait, &wait);
        wake_up_locked(&sem->wait);
        spin_unlock_irqrestore(&sem->wait.lock, flags);
        current->state = TASK_RUNNING;
    }

The down() function decreases the count field of the *sem semaphore, and then checks whether its value is negative. Again, the decrement and the test must be atomically executed. If count is greater than or equal to 0, the current process acquires the resource and the execution continues normally. Otherwise, count is negative, and the current process must be suspended. The contents of some registers are saved on the stack, and then _ _down() is invoked.

Essentially, the _ _down() function changes the state of the current process from TASK_RUNNING to TASK_UNINTERRUPTIBLE, and it puts the process in the semaphore wait queue. Before accessing the fields of the semaphore structure, the function also gets the sem->wait.lock spin lock that protects the semaphore wait queue (see “How Processes Are Organized” in Chapter 3) and disables local interrupts. Usually, wait queue functions get and release the wait queue spin lock as necessary when inserting and deleting an element. The _ _down() function, however, uses the wait queue spin lock also to protect the other fields of the semaphore data structure, so that no process running on another CPU is able to read or modify them. To that end, _ _down() uses the “_locked” versions of the wait queue functions, which assume that the spin lock has been already acquired before their invocations.

The main task of the _ _down() function is to suspend the current process until the
semaphore is released. However, the way in which this is done is quite involved. To
easily understand the code, keep in mind that the sleepers field of the semaphore is
usually set to 0 if no process is sleeping in the wait queue of the semaphore, and it is
set to 1 otherwise. Let’s try to explain the code by considering a few typical cases.

MUTEX semaphore open (count equal to 1, sleepers equal to 0)
The down macro just sets the count field to 0 and jumps to the next instruction of
the main program; therefore, the _ _down() function is not executed at all.

MUTEX semaphore closed, no sleeping processes (count equal to 0, sleepers equal to 0)
The down macro decreases count and invokes the _ _down() function with the
count field set to –1 and the sleepers field set to 0. In each iteration of the loop,
the function checks whether the count field is negative. (Observe that the count
field is not changed by atomic_add_negative() because sleepers is equal to 0
when the function is invoked.)

* If the count field is negative, the function invokes schedule() to suspend the
current process. The count field is still set to –1, and the sleepers field to 1.
The process picks up its run subsequently inside this loop and issues the test
again.

* If the count field is not negative, the function sets sleepers to 0 and exits from
the loop. It tries to wake up another process in the semaphore wait queue
(but in our scenario, the queue is now empty) and terminates holding the
semaphore. On exit, both the count field and the sleepers field are set to 0, as
required when the semaphore is closed but no process is waiting for it.

MUTEX semaphore closed, other sleeping processes (count equal to –1, sleepers equal
to 1)
The down macro decreases count and invokes the _ _down() function with count
set to –2 and sleepers set to 1. The function temporarily sets sleepers to 2, and
then undoes the decrement performed by the down macro by adding the value
sleepers–1 to count. At the same time, the function checks whether count is still
negative (the semaphore could have been released by the holding process right
before _ _down() entered the critical region).
• If the count field is negative, the function resets sleepers to 1 and invokes
schedule() to suspend the current process. The count field is still set to –1,
and the sleepers field to 1.
• If the count field is not negative, the function sets sleepers to 0, tries to
wake up another process in the semaphore wait queue, and exits holding the
semaphore. On exit, the count field is set to 0 and the sleepers field to 0.
The values of both fields look wrong, because there are other sleeping processes.
However, consider that another process in the wait queue has been
woken up. This process does another iteration of the loop; the atomic_add_
negative() function subtracts 1 from count, restoring it to –1; moreover,
before returning to sleep, the woken-up process resets sleepers to 1.

So, the code properly works in all cases. Consider that the wake_up() function in __
down() wakes up at most one process, because the sleeping processes in the wait
queue are exclusive (see the section “How Processes Are Organized” in Chapter 3).

Only exception handlers, and particularly system call service routines, can use the
down() function. Interrupt handlers or deferrable functions must not invoke down( ),
because this function suspends the process when the semaphore is busy. For this reason,
Linux provides the down_trylock( ) function, which may be safely used by one of the previously mentioned asynchronous functions. It is identical to down( ) except
when the resource is busy. In this case, the function returns immediately instead of
putting the process to sleep.

A slightly different function called down_interruptible( ) is also defined. It is widely
used by device drivers, because it allows processes that receive a signal while being
blocked on a semaphore to give up the “down” operation. If the sleeping process is
woken up by a signal before getting the needed resource, the function increases the
count field of the semaphore and returns the value –EINTR. On the other hand, if down_
interruptible( ) runs to normal completion and gets the resource, it returns 0. The
device driver may thus abort the I/O operation when the return value is –EINTR.

Finally, because processes usually find semaphores in an open state, the semaphore
functions are optimized for this case. In particular, the up() function does not execute
jump instructions if the semaphore wait queue is empty; similarly, the down()
function does not execute jump instructions if the semaphore is open. Much of the
complexity of the semaphore implementation is precisely due to the effort of avoiding
costly instructions in the main branch of the execution flow.


<h3 id="5.2.9">5.2.9 读写信号量</h3>

<h3 id="5.2.10">5.2.10 Completion机制</h3>

<h3 id="5.2.11">5.2.11 中断禁止</h3>

<h3 id="5.2.12">5.2.12 软中断禁止</h3>

<h2 id="5.3">5.3 内核数据结构的同步访问</h2>

<h2 id="5.4">5.4 防止竞态条件的示例</h2>
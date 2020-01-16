<h1 id="0">Linux内核同步机制之三：memory barrier</h1>

* [1 前言](#1)

<h2 id="1">1 前言</h2>

<h3 id="1.1">1.1 编译器 </h3>

<h3 id="1.2">1.2 CPU指令的执行 </h3>

编译器将C代码语句翻译成了符合CPU运算规则的汇编指令。编译器了解底层CPU的思维模式，因此，它可以在将c翻译成汇编的时候进行优化（例如内存访问指令的重新排序），让产出的汇编指令在CPU上运行的时候更快。然而，这种优化产出的结果未必符合程序员原始的逻辑。因此，作为程序员，尤其是作为c程序员，必须有能力了解编译器的行为，并在通过内嵌在c代码中的memory barrier来指导编译器的优化行为（这种memory barrier又叫做优化屏障，Optimization barrier），让编译器产出即高效，又逻辑正确的代码。

CPU的核心思想就是取指执行，对于有序执行的单核CPU，并且没有cache（这种CPU在现实世界中还存在吗？），汇编指令的取指和执行是严格按照顺序进行的，也就是说，汇编指令就是所见即所得的，汇编指令的逻辑顺序被严格执行。然而，随着计算机系统越来越复杂（多核、cache、superscalar、out-of-order），使用汇编指令这样贴近处理器的语言也无法保证其被CPU执行的结果的一致性，从而需要程序员（看，人还是最不可以替代的）告知CPU如何保证逻辑正确。

综上所述，memory barrier是一种保证内存访问顺序的一种方法，让系统中的HW block（各个cpu、DMA控制器、设备等）对内存有一致性的视角。

<h2 id="2">2 不使用memory barrier会导致问题的场景</h2>

<h3 id="2.1">2.1 编译器的优化 </h3>

我们先看下面的一个例子：

    preempt_disable（）
    临界区
    preempt_enable

有些共享资源可以通过禁止任务抢占来进行保护，因此临界区代码被preempt_disable和preempt_enable给保护起来。其实，我们知道所谓的preempt enable和disable其实就是对当前进程的struct thread_info中的preempt_count进行加一和减一的操作。具体的代码如下：

    #define preempt_disable() \
    do { \
        preempt_count_inc(); \
        barrier(); \
    } while (0)

linux 内核中的定义和我们的想像一样，除了`barrier`这个优化屏障。`barrier`就象是c代码中的一个栅栏，将代码逻辑分成两段，`barrier`之前的代码和`barrier`之后的代码在经过编译器编译后顺序不能乱掉。也就是说，`barrier`之后的c代码对应的汇编，不能跑到`barrier`之前去，反之亦然。之所以这么做是因为在我们这个场景中，如果编译器为了提高CPU的性能，而对汇编指令重新排序，那么临界区的代码就有可能位于`preempt_count_inc`之外，从而起不到保护作用。

现在，我们知道了`barrier`的作用。那么，问题来了，仅有`barrier`是否够呢？对于多核系统，只有当该任务被调度到该CPU上执行的时候，该CPU才会访问该任务的`preempt count`，因此对于`preempt enable`和`disable`而言，不存在多个CPU同时访问的场景。但是，即便这样，如果CPU是乱序执行（out-of-order excution）的呢？其实，我们也不用担心，正如前面叙述的，preempt count这个memory实际上是不存在多个cpu同时访问的情况，因此，它实际上会本cpu的进程上下文和中断上下文访问。能终止当前thread执行preempt_disable的只有中断。为了方便描述，我们给代码编址，如下：

原文地址：[Linux内核同步机制之（三）：memory barrier](http://www.wowotech.net/kernel_synchronization/memory-barrier.html)

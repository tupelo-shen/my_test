[TOC]

# 1 Prerequisites

1. [Set up your machine.](https://docs.sel4.systems/HostDependencies)

2. [Notification tutorial](https://docs.sel4.systems/Tutorials/notifications)

# 2 初始化工程

    # For instructions about obtaining the tutorial sources see https://docs.sel4.systems/Tutorials/#get-the-code
    #
    # Follow these instructions to initialise the tutorial
    # initialising the build directory with a tutorial exercise
    ./init --tut interrupts
    # building the tutorial exercise
    cd interrupts_build
    ninja

# 3 Outcomes

1. Understand the purpose of the IRQControl capability.

2. Be able to obtain capabilities for specific interrupts.

3. Learn how to handle interrupts and their relation with notification objects.

# 4 背景知识

## 4.1 IRQControl

The root task is given a single capability from which capabilities to all irq numbers in the system can be derived, seL4_CapIRQControl. This capability can be moved between CSpaces and CSlots but cannot be duplicated. Revoking this capability results in all access to all irq capabilities being removed.

## 4.2 IRQHandlers

IRQHandler capabilities give access to a single irq and are standard seL4 capabilities: they can be moved and duplicated according to system policy. IRQHandlers are obtained by invoking the IRQControl capability, with architecture specific parameters. Below is an example of obtaining an IRQHandler.

    // Get a capability for irq number 7 and place it in cslot 10 in a single-level cspace.
    error = seL4_IRQControl_Get(seL4_IRQControl, 7, cspace_root, 10, seL4_WordBits);

There are a variety of different invocations to obtain irq capabilities which are hardware dependent, including:

* seL4_IRQControl_GetIOAPIC (x86)
* seL4_IRQControl_GetMSI (x86)
* seL4_IRQControl_GetTrigger (ARM)

## 4.3 接收中断

将IRQHandler能力注册到通知对象中：

    seL4_IRQHandler_setNotification(irq_handler, notification);

如果设置成功，当发生中断请求的时候，IRQ信号就会被传送给`通知`对象。为了在一个通知对象中，处理多个中断，将绑定到每个中断信号上的通知能力打上徽章，以示区分。当中断信号到达时，将徽章的值`按位或`到通知对象的数据字上。

可以使用`seL4_Poll`轮询中断信号，也可以使用`seL4_Wait`等待中断信号。两种系统调用都会传递通知对象的数据字和消息的徽章，然后，数据字就会被清除。

`seL4_IRQHandler_Clear`可以用来解除IRQHandler和通知的绑定。

## 4.4 处理中断

一旦中断被接收，如果没有调用`seL4_IRQHandler_Ack`通知内核解除对某个中断信号的屏蔽，seL4微内核不会进一步传递中断信号。

# 5 Exercises

In this tutorial you will set up interrupt handling for a provided timer driver on the zynq7000 ARM platform. This timer driver can be located inside the `projects/sel4-tutorials/zynq_timer_driver` folder from the root of the projects directory, i.e. where the `.repo` folder can be found and where the initial `repo init` command was executed. The tutorial has been set up with two processes: `timer.c`, the timer driver and RPC server, and `client.c`, which makes a single request.

On successful initialisation of the tutorial, you will see the following:

    timer client: hey hey hey
    timer: got a message from 61 to sleep 2 seconds
    <<seL4(CPU 0) [decodeInvocation/530 T0xe8265600 "tcb_timer" @84e4]: Attempted to invoke a null cap #9.>>
    main@timer.c:78 [Cond failed: error]
        Failed to ack irq

The timer driver we are using emits an interrupt in the `TTC0_TIMER1_IRQ` number.

Exercise Invoke `irq_control`, which contains the `seL4_IRQControl` capability, the place the `IRQHandler` capability for `TTC0_TIMER1_IRQ` into the `irq_handler` CSlot.

    /* TODO invoke irq_control to put the interrupt for TTC0_TIMER1_IRQ in
       cslot irq_handler (depth is seL4_WordBits) */

On success, you should see the following output, without the error message that occurred earlier, as the irq_handle capability is now valid:

    Undelivered IRQ: 42

This is a warning message from the kernel that an IRQ was recieved for irq number 42, but no notification capability is set to sent a signal to.

Exercise Now set the notification capability (ntfn) by invoking the irq handler.

    /* TODO set ntfn as the notification for irq_handler */

Now the output will be:

    Tick

Only one interrupt is delivered, as the interrupt has not been acknowledged. The timer driver is programmed to emit an interrupt every millisecond, so we need to count 2000 interrupts before replying to the client.

Exercise Acknowledge the interrupt after handling it in the timer driver.

        /* TODO ack the interrupt */

Now the timer interrupts continue to come in, and the reply is delivered to the client.

    timer client wakes up

That’s it for this tutorial.
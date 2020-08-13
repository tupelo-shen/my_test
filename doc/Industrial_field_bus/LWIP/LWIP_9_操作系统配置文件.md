[TOC]

# 1 操作系统抽象层说明

特定于操作系统的代码应该在 arch/ sys_arch.h和sys_arch.c中实现。

操作系统仿真层在lwIP代码和底层操作系统内核之间提供通用接口。 一般的想法是将lwIP移植到新架构只需要对少量头文件和新的sys_arch实现进行小的更改。 也可以执行不依赖于任何底层操作系统的sys_arch实现。

sys_arch为lwIP提供信号量，邮箱和互斥锁。 对于完整的lwIP功能，可以在sys_arch中实现多线程支持，但基本的lwIP功能不需要这样做。

除了提供sys_arch功能的源文件之外，OS仿真层还必须提供几个头文件，用于定义整个lwip中使用的宏。 所需的文件和它们必须定义的宏列在sys_arch描述下面。

从lwIP 1.4.0开始，信号量，互斥量和邮箱功能的原型都允许使用指针或实际的OS结构。 这样，这些类型所需的内存可以就地分配（全局或堆栈）或堆（在“* _new()”函数内部分配）。

注意：

在sys_arch中使用mem_malloc()时要小心。 当malloc()引用mem_malloc()时，您可能遇到循环函数调用问题。在mem.c中，mem_init()尝试使用mem_malloc分配信号量，当sys_arch使用mem_malloc时，当然无法执行该信号量。

# 1.1 信号量

信号量可以是计数或二进制 - lwIP可用于两种类型。 信号量由“sys_sem_t”类型表示，该类型在sys_arch.h文件中是typedef。邮箱等效地由“sys_mbox_t”类型表示。 互斥体由“sys_mutex_t”类型表示。 lwIP对内部表示这些类型的方式没有任何限制。

    /* 
        创建一个新的信号量。 
        信号量被分配给'sem'指向的内存（可以是指针或实际的OS结构）。
        “count”参数指定信号量的初始状态（为0或1）。
        如果已创建信号量，则应返回ERR_OK。 返回任何其他错误将提示出现了什么问题，但除了断言之外，没有实现真正的错误处理。
        参数
            sem指向要创建的信号量的指针
            计算信号量的初始计数
        返回
            如果成功则为ERR_OK，否则为另一个错误
     */
    err_t   sys_sem_new (sys_sem_t *sem, u8_t count)
     
    /*
        发送信号量
        参数
            sem 信号量发出信号
    */ 
    void    sys_sem_signal (sys_sem_t *sem)
     
    /*
        在等待信号量发出信号的同时阻塞线程。 
        如果“timeout”参数不为零，则只应在指定时间内阻塞线程（以毫秒为单位）。 
        如果“timeout”参数为零，则应阻塞线程，直到发出信号量信号。
        如果信号量未在指定时间内发出信号，则返回值为SYS_ARCH_TIMEOUT;
        如果已发出信号（有或没有等待），则返回值为任何其他值。 
        请注意，lwIP实现了一个名为sys_sem_wait()()的函数，该函数使用sys_arch_sem_wait()函数。
        参数
            sem信号量等待
            超时超时（以毫秒为单位）等待（0 =永远等待）
        返回
            超时时SYS_ARCH_TIMEOUT，成功时的任何其他值
    */ 
    u32_t   sys_arch_sem_wait (sys_sem_t *sem, u32_t timeout)
     
    /*
        释放信号量。
        参数
            sem信号量删除
    */ 
    void    sys_sem_free (sys_sem_t *sem)
     
    /*
        如果信号量有效则返回1，如果信号量无效则返回0。 
        使用指针时，一种简单的方法是检查 指针！= NULL。
    */ 
    int     sys_sem_valid (sys_sem_t *sem)
     
    /*
        使信号量无效，以便sys_sem_valid()返回0.
        注意：这并不意味着信号量应该被释放：在调用此函数之前总是调用sys_sem_free()
    */ 
    void    sys_sem_set_invalid (sys_sem_t *sem)

# 1.2 互斥锁

建议使用互斥锁正确处理优先级倒置，尤其是在使用LWIP_CORE_LOCKING时。

互斥体由“sys_mutex_t”类型表示。 lwIP对内部表示这些类型的方式没有任何限制。

    /*
        创建一个新的互斥锁。 
        请注意，lwIP代码不会递归地使用互斥锁，因此两种实现类型（递归或非递归）都应该起作用。 
        互斥锁被分配给'mutex'指向的内存（可以是指针或实际的OS结构）。 
        如果已创建互斥锁，则应返回ERR_OK。 
        返回任何其他错误将提示出现了什么问题，但除了断言之外，没有实现真正的错误处理。
        参数
            mutex 指向要创建的互斥锁
        返回
            如果成功则为ERR_OK，否则为另一个错误
    */
    err_t   sys_mutex_new (sys_mutex_t *mutex)
     
    /*
        阻止线程，直到可以抓住互斥锁。
        参数
            mutex 锁定的互斥锁
    */ 
    void    sys_mutex_lock (sys_mutex_t *mutex)
     
    /*
        释放先前通过'sys_mutex_lock()'锁定的互斥锁。
        参数
            mutex 解锁互斥锁
    */ 
    void    sys_mutex_unlock (sys_mutex_t *mutex)
     
    /*
        取消分配互斥锁。
        参数
            要删除的互斥锁
    */ 
    void    sys_mutex_free (sys_mutex_t *mutex)
     
    /* 
        有效性判断：
        返回；1有效 0 无效
    */ 
    int     sys_mutex_valid (sys_mutex_t *mutex)
     
    /*
        使互斥锁无效，以便sys_mutex_valid()返回0。
        注意：这并不意味着应该释放互斥锁：在调用此函数之前总是调用sys_mutex_free()！ 
    */ 
    void    sys_mutex_set_invalid (sys_mutex_t *mutex)

# 1.3 邮箱

邮箱应该实现为允许发布多个邮件的队列。邮箱中的邮件只是一个指针，仅此而已。

邮箱等效地由“sys_mbox_t”类型表示。 lwIP对内部表示这些类型的方式没有任何限制。

    /*
        创建一个大小为size的空邮箱
        存储在邮箱中的元素是指针。 您必须在lwipopts.h中定义宏“_MBOX_SIZE”，或在实现中忽略此参数并使用默认大小。 如果已创建邮箱，则应返回ERR_OK。 返回任何其他错误将提示出现了什么问题，但除了断言之外，没有实现真正的错误处理。
        参数
            mbox指向要创建的mbox的指针
            此mbox中的大小（最小）消息数
        返回
            如果成功则为ERR_OK，否则为其他错误
    */
    err_t   sys_mbox_new (sys_mbox_t *mbox, int size)
     
    /*
        将消息发布到mbox  （如果已满，不会返回失败）
        仅用于来自非ISR的任务
        参数
            mbox发布消息
            msg 要发布的消息（注意：可以为NULL）
    */ 
    void    sys_mbox_post (sys_mbox_t *mbox, void *msg)
     
    /*
        尝试将邮件发布到mbox  - (如果已满，可能会失败。)
        可以在ISR中使用（如果sys arch层允许这样做）。一般情况不可以在ISR使用,仅任务使用
        如果已满，则返回ERR_MEM，否则，如果发布“msg”，则返回ERR_OK。
        参数
            邮箱发布消息
            要发布的消息（注意：可以为NULL）
    */ 
    err_t   sys_mbox_trypost (sys_mbox_t *mbox, void *msg)
     
    /*
        尝试将邮件发布到mbox  -( 如果已满，可能会失败。)
        ISR使用。 
        如果已满，则返回ERR_MEM，否则，如果发布“msg”，则返回ERR_OK。
        参数
            mbox mbox发布消息
            要发布的消息（注意：可以为NULL）
    */ 
    err_t   sys_mbox_trypost_fromisr (sys_mbox_t *mbox, void *msg)
     
    /*
        阻塞线程，直到消息到达邮箱，但不会阻塞线程超过“timeout”毫秒（类似于sys_arch_sem_wait()函数）。 
        如果“timeout”为0，则应阻止该线程，直到消息到达。 
        “msg”参数是由函数设置的结果参数（即，通过执行“* msg = ptr”）。
        “msg”参数可能为NULL，表示应删除该消息。 
        返回值：超时返回SYS_ARCH_TIMEOUT如果有超时，如果收到消息，则返回任何其他值。
        请注意，具有类似名称sys_mbox_fetch()的函数由lwIP实现。
        参数
            mbox从中获取消息
            指针存储消息的位置
            超时等待消息的最长时间（以毫秒为单位）（0 =永远等待）
        返回
            超时时SYS_ARCH_TIMEOUT，如果收到消息则为任何其他值
    */ 
    u32_t   sys_arch_mbox_fetch (sys_mbox_t *mbox, void **msg, u32_t timeout)
     
    /*
        类似于sys_arch_mbox_fetch，但是如果邮箱中没有邮件，它会立即返回代码SYS_MBOX_EMPTY。 成功时返回0。
        为了实现高效实现，可以将其定义为sys_arch.h中的类似函数的宏，而不是普通函数。
        例如，一个简单的实现可能是：#define sys_arch_mbox_tryfetch（mbox，msg）sys_arch_mbox_fetch（mbox，msg，1）虽然这会引入不必要的延迟。
        返回
            如果已收到消息，则为0（毫秒）;如果邮箱为空，则为SYS_MBOX_EMPTY
    */ 
    u32_t   sys_arch_mbox_tryfetch (sys_mbox_t *mbox, void **msg)
     
    /*
        取消分配邮箱。
        如果在取消分配邮箱时邮箱中仍然存在邮件，则表明lwIP中存在编程错误，应通知开发人员。
        参数
            mbox要删除
    */ 
    void    sys_mbox_free (sys_mbox_t *mbox)
     
    /* Returns 1 if the mailbox is valid, 0 if it is not valid. 参考sem */ 
    int     sys_mbox_valid (sys_mbox_t *mbox)
     
    /*
        使邮箱无效，以便sys_mbox_valid()返回0。
        注意：这并不意味着应该释放邮箱：
        在调用此函数之前总是调用sys_mbox_free()！
    */ 
    void    sys_mbox_set_invalid (sys_mbox_t *mbox)

# 1.4 杂项

    /*
        睡眠指定的ms数
        睡一段时间。 睡觉时不处理超时。
        参数
            ms睡眠的毫秒数
    */
    void    sys_msleep (u32_t ms)
     
    /*
        创建线程函数
        唯一的线程函数：启动一个名为“name”的新线程，优先级为“prio”，它将在函数“thread()”中开始执行。 “arg”参数将作为参数传递给thread()函数。 用于此线程的堆栈大小是“stacksize”参数。 返回新线程的id。 id和优先级都取决于系统。 
        注意：虽然这个函数返回一个值，但它绝不能失败（端口必须断言！）
        参数
            name       线程名称（用于调试目的）
            thread     线程的线程函数
            arg        参数传递给'thread'
            stacksize  新线程的堆栈大小（以字节为单位）（端口可能会被忽略）
            prio       新线程的优先级（可能被端口忽略）
    */ 
    sys_thread_t    sys_thread_new (const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
     
    /*
        初始化sys_arch层。
        必须在其他任何事情之前调用sys_init()。 
     */
    void    sys_init (void)

# 2 时间

相关声明见 sys.h

    /*
        返回当前时间（以毫秒为单位），可以与sys_jiffies相同或至少基于它。 这仅用于时间差异。
        不实现此功能意味着您不能使用某些模块（例如，TCP时间戳，NO_SYS == 1的内部超时）。
        无论有无操作系统，为了使用完整的协议栈，最好都实现
    */
    u32_t   sys_now (void)

# 3 关键代码段/临界区

用于保护短代码区域免受并发访问。

相关声明见 sys.h

系统是裸机系统（可能带有RTOS），并且可以控制中断：将其实现为LockInterrupts()/ UnlockInterrupts()
系统使用具有来自工作线程的延迟中断处理的RTOS：实现为全局互斥锁或锁定/解锁调度程序
系统使用高级操作系统，例如 POSIX信号：实现为全局互斥体

    /* 
        根据opt.h 中可知，若希望在缓冲区分配，释放和内存分配以及释放期间对某些关键区域进行任务间保护，则    在lwipopts.h中定义SYS_LIGHTWEIGHT_PROT==1
    */
     
    /*
        SYS_ARCH_DECL_PROTECT声明一个保护变量。
        此宏将默认定义sys_prot_t类型的变量。
        如果特定端口需要不同的实现，则可以在sys_arch.h中定义此宏。
    */
    #define     SYS_ARCH_DECL_PROTECT(lev)   sys_prot_t lev
     
     
    /*
        SYS_ARCH_PROTECT执行“快速”保护。 
        这可以通过禁用嵌入式系统的中断或使用信号量或互斥量来实现。 
        该实现应允许在已受保护时调用SYS_ARCH_PROTECT。 
        旧保护级别在变量“lev”中返回。 
        此宏将默认调用sys_arch_protect()函数，该函数应在sys_arch.c中实现。 
        如果特定端口需要不同的实现，则可以在sys_arch.h中定义此宏
    */ 
    #define     SYS_ARCH_PROTECT(lev)   lev = sys_arch_protect()
     
    /* 
        SYS_ARCH_UNPROTECT执行“快速”保护级别设置为“lev”。
        这可以通过在MACRO中将中断级别设置为“lev”或使用信号量或互斥量来实现。 
        该宏将默认调用sys_arch_unprotect()函数，该函数应在sys_arch.c中实现。 
        如果特定端口需要不同的实现，则可以在sys_arch.h中定义此宏
    */ 
    #define     SYS_ARCH_UNPROTECT(lev)   sys_arch_unprotect(lev)


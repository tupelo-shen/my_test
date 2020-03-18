<h1 id="0">0 目录</h1>

* [4 中断和异常](#4)
    - [4.1 中断信号的角色](#4.1)
    - [4.2 中断和异常](#4.2)
        + [4.2.1 中断请求线-IRQ](#4.2.1)
            * [4.2.1.1 高级可编程中断控制器-APIC](#4.2.1.1)
        + [4.2.2 异常](#4.2.2)
        + [4.2.3 中断描述符表](#4.2.3)
        + [4.2.4 中断和异常的硬件处理](#4.2.4)
    - [4.3 嵌套中断和异常](#4.3)
    - [4.4 初始化中断描述符表](#4.4)
        + [4.4.1 中断门、陷阱门和系统门](#4.4.1)
        + [4.4.2 IDT初始化](#4.4.2)
    - [4.5 异常处理](#4.5)
    - [4.6 中断处理](#4.6)
    - [4.7 软件中断和Tasklet](#4.7)
    - [4.8 工作队列](#4.8)
    - [4.9 中断和异常的返回](#4.9)

---

<h1 id="4">4 中断和异常</h1>

中断定义：通常被定义为改变CPU指令执行序列的事件。

中断可以分为异步和同步中断：

* **同步中断**，是由CPU在执行指令时由CPU控制单元产生的中断。这意味着，CPU必须停止指令的执行，转而响应中断。通常情况下，同步中断就是指 **异常**。

* **异步中断**，是由外部设备随机产生的，信号采样按照CPU时钟信号。异步中断就是我们通常情况下所指的中断。一般是定时器中断和I/O设备中断。

异常通常分为2类：一类是编程错误，另外一类就是需要内核处理的异常情况。编程错误，比如程序异常终止，处理这种异常，内核只需要给当前进程发送一个信号即可。而需要内核处理的异常，比如页错误、通过汇编语言指令比如int或sysenter等请求内核服务等，需要内核作出相应的处理。

<h2 id="4.1">4.1 中断信号的角色</h2>

**顾名思义，中断信号提供了一种使CPU跳出当前正在执行的代码流的方法**。这是对于轮询机制的一种有效补充，中断机制提高了系统效率。当中断信号来临时，CPU停止当前正在执行的指令，保存当前进程内核态栈中的PC寄存器值（例如，eip和cs寄存器），取而代之的是，将中断类型相关的地址写入到PC寄存器中，从而切换到新的中断上下文。

但是，需要注意的是中断处理和进程切换有着很大不同：中断或者异常处理程序不是进程。它的处理完全在内核态，而且处理的内容非常少，要求响应时间必须非常短。

中断处理对时间高度敏感，有着严格要求：

* 因为中断随时发生，打断内核的调度。因此，内核希望尽快处理完中断，然后回到正常的调度执行中。比如，假设从网络上接收一个数据包，硬件中断内核，标记数据已经接收，然后就把CPU的使用权交还给之前正在运行的任务。稍后，由负责数据接收的进程来搬运数据到缓冲区，并作进一步处理。由此可见，响应中断的任务就被分成了两部分：紧急部分，由内核立即处理；可延时处理部分留给其它任务处理。

* 因为中断会随时发生，有时候，内核正在处理一个中断的时候，另一个中断可能会发生。中断处理程序必须能够允许中断嵌套处理。

* 虽然内核允许中断嵌套处理，但是内核代码中，必须提供临界段代码，在其中，中断必须被禁止。因为有些时候，我们的代码是不允许被中断的，这也是内核同步的一种手段。

<h2 id="4.2">4.2 中断和异常</h2>

Intel官方文档将中断和异常分类为：

* 中断：

    - 可屏蔽中断

        所有I/O设备发出的IRQ都能产生可屏蔽中断。屏蔽掉的中断，中断控制器忽略其存在。

    - 非可屏蔽中断

        只有很少的重要事件会产生非屏蔽中断。比如，硬件错误。非屏蔽中断总是能够被硬件识别。

* 异常：

    - 处理器检测异常

        当CPU在执行指令时，检测出的异常。依赖于异常发生时，内核态栈中的eip寄存器指令，又可以分为三类：

        + Fault

            这类异常可以纠正。因为这类错误就是eip指令造成的，所以，一旦异常处理程序正确处理异常后，就可以继续执行eip寄存器中的指令了。

        + Trap

            陷阱指令造成的异常。陷阱同Fault一样，因为没有破坏内核态栈中的任何东西，异常处理程序终止后，可以继续执行eip寄存器中的指令。它的设计目的主要是为了调试，告知调试器正在执行一个特殊的指令（比如，在程序里打一个断点）。一旦用户查看完断点处信息后，他就可以让程序继续执行了。

        + Abort

            发生严重错误时的异常。此时，CPU控制单元发生异常，但是无法确定发生错误的指令的准确位置，也就是说，在eip寄存器中的指令并不是造成错误的指令。这类错误一般是硬件错误或系统页表中非法或者不一致的地址等。控制单元发出信号，让CPU跳转到异常处理程序。Abort异常处理程序一般都是终止程序的执行。

    - 编程异常

        这类异常一般是由程序员故意造成的。可以使用int或int3指令触发，也可以使用into-溢出中断指令和bound-地址限制异常中断指令检查相应的条件，如果条件为假，也会产生异常。可编程错误一般被当作陷阱-trap处理，通常被称为软件中断。这类异常一般有两种作用：系统调用和告知调试器某个事件。

中断或异常使用一张中断向量表进行管理，编号为0-255。非可屏蔽中断和异常编号是固定的；而可屏蔽中断是不固定的，可以通过对中断控制器进行编程进行修改。

<h3 id="4.2.1">4.2.1 中断请求线-IRQ</h3>

硬件设备和可编程中断控制器之间使用中断请求线（IRQ）进行连接。中断请求线用来传输电信号。可编程中断控制器接收这些电信号，然后将其转换成中断号。具体如下：

1. 监听IRQ线，检查上升沿信号。如果同时检测到多个信号，选择数字小的IRQ线。

2. 如果IRQ中断请求线上产生一个上升沿信号：

    1. 将电信号转换成对应的中断向量，说白了，就是转换成一个对应的数字；
    2. 将该向量存储到中断控制器的I/O端口，然后允许CPU通过数据总线读取它；
    3. 发送一个中断信号到INTR管脚；
    4. 等待CPU应答该中断信号，然后，清除INTR中断线。

3. 回到第1步。

IRQ线一般从0开始编号，也就是说，第一条IRQ线标记为IRQ0。Intel默认的中断向量表是从IRQ32开始的，也就是说，前32个中断号已经被不可屏蔽中断和保留中断所占用。IRQ线和向量表之间的对应关系可以通过给中断控制器发送合适的I/O指令进行修改。

可以通过对可编程中断控制器（PIC）进行编程，控制是否为某个指定的IRQ线发送中断。被禁止的中断不会丢失，一旦重新使能，PIC就会把它们发送给CPU。大部分的中断处理程序都使用这个特性，可以连续地处理相同类型的IRQ请求。

需要注意的是，这里的使能、禁止IRQ和中断的屏蔽不是一个概念。当eflag寄存器中的IF标志被清除，PIC发送的可屏蔽中断会被CPU忽略掉。cli和sti汇编指令分别用于清除和设置该标志。

传统的PIC控制器使用2个8259A外部芯片，通过"级联"的方式组成的。每个芯片可以接收8个不同的IRQ，但是从PIC的INT输出管脚连接到主PIC的IRQ2管脚上，所以总的可以使用的IRQ线就是15个。

<h4 id="4.2.1.1">4.2.1.1 高级可编程中断控制器-APIC</h4>

上面的讨论我们主要考虑的单处理器系统，如果是多处理器系统，主PIC控制器的INTR管脚是如何接到CPU上的？我们接下来讨论这个话题。

我们知道，多核处理系统的价值在于 **并行处理**。所以，如何把中断分配到每一个CPU上就至关重要了。基于这个原因，Intel从奔腾III开始，引入一个新的高级可编程中断控制器（`I/O-APIC`）。这个控制器是8259A中断控制器的加强版。为了兼容旧版本的操作系统，有些主板包含这两种芯片。x86架构中，每个处理器包含自己的APIC，每个APIC具有32位的寄存器，内部时钟，内部定时器以及2个额外的IRQ线，LINT0和LINT1，用作APIC的中断。所有私有的APIC都连接到`I/O-APIC`，组成一个多APIC系统。

图4-1展示了一个多APIC系统的原理图。`I/O-APIC`通过APIC总线和各个APIC连接在一起。`I/O-APIC`相等于一个中继的角色。

<img id="Figure_4-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_1.PNG">

图4-1 多APIC系统

`I/O-APIC`由24条中断线，中断重定向表，可编程寄存器和一个通过APIC总线收发数据的消息单元组成。与8259A中断控制器不同，管脚编号不再具有优先级：重定向表中的每一项都可以被独立设置中断向量和优先级，目的处理器以及处理器如何处理该中断。也就是说，中断重定向表就是外部IRQ到私有APIC的映射关系。

中断请求被分配到CPU上的方式有两种：

1. 静态分配

    按照重定向表中的定义把IRQ请求分配到相应的私有APIC高级可编程中断控制器上。中断可以指定给单个CPU，或者一组CPU，或者所有的CPU（相当于广播）。

2. 动态分配

    IRQ请求被发送给正在运行低优先级进程的处理器的私有APIC中断控制器上。通俗地说，就是哪个处理器正在运行低优先级任务，IRQ请求就发送给谁。

    每个私有APIC都有一个可编程任务优先级寄存器，用来保存当前运行任务的优先级。Intel期望每次进程切换的时候，操作系统内核修改这个寄存器。

    如果有多个CPU拥有相同的最低任务优先级，则使用仲裁技术分配中断请求。根据仲裁，每个CPU被分配一个不同的优先级（0-15，数字越小，优先级越大），这个优先级存储在私有APIC的任务优先级寄存器中。

    **分配策略**是，每当分配一个中断请求给一个CPU，则它对应的仲裁优先级被自动设为0，而其它CPU的仲裁优先级则被增加。当优先级寄存器中的值大于15时，则设为1。因为具有相同任务优先级的CPU的中断分配使用循环方式进行。

    > 动态分配的策略就是负载均衡的一种手段。关于负载均衡的算法以后再研究。

除了CPU与外设之间的中断，多APIC系统还允许CPU产生CPU之间的中断。当一个CPU想给另一个CPU发送中断时，它就会把目标CPU的私有APIC的标识符和中断号存储到自己APIC的中断命令寄存器（ICR）中。然后通过APIC总线发送给目标APIC，该APIC就会给自己的CPU发送一个相应的中断。

CPU间的中断（简称IPI）是多核系统一个重要组成部分。Linux有效地利用它们，在CPU之间传递消息。

目前，大部分的单核系统也都包含一个I/O-APIC芯片，可以使用两种不同的方式配置它：

1. 当一个标准的8259A类型的外部PIC使用。私有APIC被禁止，LINT0和LINT1这两个IRQ请求线被分别配置为INTR和NMI管脚。

2. 作为标准的I/O-APIC使用，只不过只有一个CPU而已。

<h3 id="4.2.2">4.2.2 异常</h3>

x86架构大约有20种不同的异常。内核必须为每种异常提供专用的处理函数。对于某些异常，CPU控制单元也会产生硬件错误码，并将其压入内核态栈，然后再启动异常处理函数。

下表是异常列表，列出了异常号，名称，类型等等。更多信息请参考Intel技术手册。

|#| 异常          | 类型    | 异常处理函数 | 信号 |
|-| ------------- | ------ | --------- | ------ |
|0| 除法错误        | fault| divide_error() | SIGFPE |
|1| Debug         | trap/fault| debug( ) | SIGTRAP |
|2| NMI           | -     | nmi( ) | - |
|3| 断点           | trap | int3( ) | SIGTRAP |
|4| 溢出           | trap | overflow( ) | SIGSEGV |
|5| 边界检查        | fault | bounds( ) | SIGSEGV |
|6| 非法操作码      | fault | invalid_op( ) | SIGILL |
|7| 设备不可用      | fault | device_not_available( ) | - |
|8| 串行处理异常错误 | abort | doublefault_fn() | - |
|9| 协处理器错误    | abort | coprocessor_segment_overrun( ) | SIGFPE |
|10| 非法TSS       | fault | invalid_TSS( ) | SIGSEGV |
|11| 段引用错误     | fault | segment_not_present( ) | SIGBUS |
|12| 栈段错误       | fault | stack_segment( ) | SIGBUS |
|13| 通用保护       | fault | general_protection( ) | SIGSEGV |
|14| 页错误         | fault | page_fault( ) | SIGSEGV |
|15| Intel保留     | -     | - | - |
|16| 浮点错误       | fault | coprocessor_error( ) | SIGFPE |
|17| 对齐检查       | fault | alignment_check( ) | SIGBUS |
|18| 机器检查       | abort | machine_check() | - |
|19| SIMD浮点异常   | fault | simd_coprocessor_error() | SIGFPE |

Intel保留20-31未来使用。如上表所示，每个异常都有一个专门的处理函数处理，并给造成异常的进程发送一个信号。

<h3 id="4.2.3">4.2.3 中断描述符表</h3>

现在，我们已经知道了中断信号是如何从设备发出，然后经过高级可编程中断控制器的分配，到达各个指定的CPU中。那么，剩下的工作就是内核的了，内核使用一个中断描述符表（IDT），记录每个中断或者异常编号以及相应的处理函数。那么，收到中断信号后，将相应的处理函数的地址加载到eip寄存器中执行即可。

IDT表中，每一项对应一个中断或者异常，大小8个字节。因而，IDT需要256x8=2048个字节大小的存储空间。

IDT表的物理地址存储在CPU寄存器`idtr`中：包括IDT的基地址和最大长度。在使能中断之前，必须使用lidt汇编指令初始化IDT表。

IDT表包含三种类型的描述符，使用Type位域表示（40-43位）。下图分别解释了这三种描述符各个位的意义。

<img id="Figure_4-2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_2.PNG">

三种描述符分别为：

1. 任务门

    包含中断发生时要替换当前进程的新进程的TSS选择器。

2. 中断门

    包含段选择器和在段中的偏移量。设置了正确的段后，处理器清除IF标志，禁止可屏蔽中断。

3. 陷阱门

    同中断门类似，只是不会修改IF标志。

<h3 id="4.2.4">4.2.4 中断和异常的硬件处理</h3>

现在，我们来探究以下CPU控制单元是如何处理中断和异常的。我们假设内核已经完成初始化，CPU工作在保护模式下。

CPU控制单元，在取指令之前，检查控制单元在执行前一条指令的时候是否有中断或异常发生。如果发生中断，控制单元就会做如下处理：

1. 确定中断或异常的编号N；

2. 读取IDT表中的第N项；（在后面的描述中，假设包含的是中断门或陷阱门）

3. 获取GDT的基地址，遍历GDT找到IDT表第N项中的段选择器标识的段描述符。这个描述符指定了包含中断或异常处理程序的段的基地址。

4. 确保中断合法性。

    首先比较cs寄存器中的CPL（当前特权等级）和包含在GDT中的段描述符的DPL（描述符特权等级），如果CPL小于DPL，产生 *通用保护* 异常，因为中断处理程序的特权等级不能比造成中断的程序的低。对于可编程异常，还会做进一步的安全检查：比较当前特权等级（CPL）和IDT表中包含的描述符的DPL，如果DPL小于CPL，则产生通用保护的异常。后一项检查，可以阻止用户应用程序访问特定的trap或中断门。

5. 检查特权等级是否发生变化。如果CPL与描述符中的DPL不同，控制单元应该使用新特权等级下的堆栈。

    1. 读取tr寄存器，访问运行中的进程的TSS段；
    2. 使用新特权等级对应的堆栈段和堆栈指针加载ss和esp寄存器；（这些值存储在TSS中）
    3. 在新的堆栈中，保存旧任务的ss和esp寄存器值。（处理完中断或异常后，还要恢复到旧任务执行）

    > 其实对于Linux来说，只使用了supervisor和user两种特权等级。所以中断应该都是在supervisor特权等级下运行。

6. 根据造成异常的指令的逻辑地址，加载cs和eip寄存器（异常解决后，程序可以继续从这儿执行）；

7. 保存eflags、cs和eip到堆栈中；

8. 如果异常携带异常错误码，将其保存在堆栈中；

9. 根据IDT表中的第N项内容，加载cs和eip寄存器。

至此，CPU控制单元跳转到中断或异常处理程序处开始执行。等到中断或异常处理完成后，把CPU的使用权让给之前被中断的进程，使用iret指令，该指令强迫控制单元执行下面步骤：

1. 加载被中断进程的cs，eip和eflags寄存器。（如果压栈过异常错误码，应该在执行iret指令之前弹出）
2. 检查CPL是否等于cs寄存器中的CPL，如果相等，则iret指令结束执行；否则，继续。
3. 加载旧特权等级的ss和esp寄存器值。
4. 检查ds、es、fs和gs寄存器中的值。如果它们之中任何一个的描述符中的DPL小于CPL，则清除相应的段寄存器。这么做，可以禁止用户态程序使用先前内核态的段寄存器。如果这些寄存器没有被清除，恶意用户态程序就可以利用它们访问内核地址空间。

<h2 id="4.3">4.3 嵌套中断和异常</h2>

Every interrupt or exception gives rise to a kernel control path or separate sequence of instructions that execute in Kernel Mode on behalf of the current process. For instance, when an I/O device raises an interrupt, the first instructions of the corresponding kernel control path are those that save the contents of the CPU registers in the Kernel Mode stack, while the last are those that restore the contents of the registers.

Kernel control paths may be arbitrarily nested; an interrupt handler may be interrupted by another interrupt handler, thus giving rise to a nested execution of kernel control paths, as shown in Figure 4-3. As a result, the last instructions of a kernel control path that is taking care of an interrupt do not always put the current process back into User Mode: if the level of nesting is greater than 1, these instructions will put into execution the kernel control path that was interrupted last, and the CPU will continue to run in Kernel Mode.

<img id="Figure_4-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_3.PNG">

Figure 4-3. An example of nested execution of kernel control paths

The price to pay for allowing nested kernel control paths is that an interrupt handler must never block, that is, no process switch can take place until an interrupt handler is running. In fact, all the data needed to resume a nested kernel control path is stored in the Kernel Mode stack, which is tightly bound to the current process.

Assuming that the kernel is bug free, most exceptions can occur only while the CPU  is in User Mode. Indeed, they are either caused by programming errors or triggered by debuggers. However, the “Page Fault” exception may occur in Kernel Mode. This happens when the process attempts to address a page that belongs to its address space but is not currently in RAM. While handling such an exception, the kernel may suspend the current process and replace it with another one until the requested page is available. The kernel control path that handles the “Page Fault” exception resumes execution as soon as the process gets the processor again.

Because the “Page Fault” exception handler never gives rise to further exceptions, at most two kernel control paths associated with exceptions (the first one caused by a system call invocation, the second one caused by a Page Fault) may be stacked, one on top of the other.

In contrast to exceptions, interrupts issued by I/O devices do not refer to data structures specific to the current process, although the kernel control paths that handle them run on behalf of that process. As a matter of fact, it is impossible to predict which process will be running when a given interrupt occurs.

An interrupt handler may preempt both other interrupt handlers and exception handlers. Conversely, an exception handler never preempts an interrupt handler. The only exception that can be triggered in Kernel Mode is “Page Fault,” which we just described. But interrupt handlers never perform operations that can induce page faults, and thus, potentially, a process switch.

Linux interleaves kernel control paths for two major reasons:

* To improve the throughput of programmable interrupt controllers and device controllers. Assume that a device controller issues a signal on an IRQ line: the PIC transforms it into an external interrupt, and then both the PIC and the device controller remain blocked until the PIC receives an acknowledgment from the CPU. Thanks to kernel control path interleaving, the kernel is able to send the acknowledgment even when it is handling a previous interrupt.
* To implement an interrupt model without priority levels. Because each interrupt handler may be deferred by another one, there is no need to establish predefined priorities among hardware devices. This simplifies the kernel code and improves its portability.

On multiprocessor systems, several kernel control paths may execute concurrently. Moreover, a kernel control path associated with an exception may start executing on a CPU and, due to a process switch, migrate to another CPU.

<h2 id="4.4">4.4 初始化中断描述符表</h2>

Now that we understand what the 80×86 microprocessors do with interrupts and exceptions at the hardware level, we can move on to describe how the Interrupt Descriptor Table is initialized.

Remember that before the kernel enables the interrupts, it must load the initial address of the IDT table into the idtr register and initialize all the entries of that table. This activity is done while initializing the system (see Appendix A).

The int instruction allows a User Mode process to issue an interrupt signal that has an arbitrary vector ranging from 0 to 255. Therefore, initialization of the IDT must be done carefully, to block illegal interrupts and exceptions simulated by User Mode processes via int instructions. This can be achieved by setting the DPL field of the particular Interrupt or Trap Gate Descriptor to 0. If the process attempts to issue one of these interrupt signals, the control unit checks the CPL value against the DPL field and issues a “General protection” exception.

In a few cases, however, a User Mode process must be able to issue a programmed exception. To allow this, it is sufficient to set the DPL field of the corresponding Interrupt or Trap Gate Descriptors to 3—that is, as high as possible.

Let’s now see how Linux implements this strategy.

<h3 id="4.4.1">4.4.1 中断、陷阱和系统门</h3>

<h3 id="4.4.2">4.4.2 IDT初始化</h3>

<h2 id="4.5">4.5 异常处理</h2>

<h2 id="4.6">4.6 中断处理</h2>

As we explained earlier, most exceptions are handled simply by sending a Unix signal to the process that caused the exception. The action to be taken is thus deferred until the process receives the signal; as a result, the kernel is able to process the exception quickly.

This approach does not hold for interrupts, because they frequently arrive long after the process to which they are related (for instance, a process that requested a data transfer) has been suspended and a completely unrelated process is running. So it would make no sense to send a Unix signal to the current process.

Interrupt handling depends on the type of interrupt. For our purposes, we’ll distinguish three main classes of interrupts:

1. I/O中断
2. 定时器中断
3. CPU之间的中断

<h3 id="4.6.1">4.6.1 I/O中断处理</h3>

In general, an I/O interrupt handler must be flexible enough to service several devices at the same time. In the PCI bus architecture, for instance, several devices may share the same IRQ line. This means that the interrupt vector alone does not tell the whole story. In the example shown in Table 4-3, the same vector 43 is assigned to the USB port and to the sound card. However, some hardware devices found in older PC architectures (such as ISA) do not reliably operate if their IRQ line is shared with other devices.

Interrupt handler flexibility is achieved in two distinct ways, as discussed in the following list.

* IRQ sharing

    The interrupt handler executes several interrupt service routines (ISRs). Each ISR is a function related to a single device sharing the IRQ line. Because it is not possible to know in advance which particular device issued the IRQ, each ISR is executed to verify whether its device needs attention; if so, the ISR performs all the operations that need to be executed when the device raises an interrupt.

* IRQ dynamic allocation

    An IRQ line is associated with a device driver at the last possible moment; for instance, the IRQ line of the floppy device is allocated only when a user accesses the floppy disk device. In this way, the same IRQ vector may be used by several hardware devices even if they cannot share the IRQ line; of course, the hardware devices cannot be used at the same time. (See the discussion at the end of this section.)

Not all actions to be performed when an interrupt occurs have the same urgency. In fact, the interrupt handler itself is not a suitable place for all kind of actions. Long noncritical operations should be deferred, because while an interrupt handler is running, the signals on the corresponding IRQ line are temporarily ignored. Most important, the process on behalf of which an interrupt handler is executed must always stay in the TASK_RUNNING state, or a system freeze can occur. Therefore, interrupt handlers cannot perform any blocking procedure such as an I/O disk operation.

Linux divides the actions to be performed following an interrupt into three classes:

* Critical

    Actions such as acknowledging an interrupt to the PIC, reprogramming the PIC or the device controller, or updating data structures accessed by both the device and the processor. These can be executed quickly and are critical, because they must be performed as soon as possible. Critical actions are executed within the interrupt handler immediately, with maskable interrupts disabled.

* Noncritical

    Actions such as updating data structures that are accessed only by the processor (for instance, reading the scan code after a keyboard key has been pushed). These actions can also finish quickly, so they are executed by the interrupt handler immediately, with the interrupts enabled.

* Noncritical deferrable

    Actions such as copying a buffer’s contents into the address space of a process (for instance, sending the keyboard line buffer to the terminal handler process). These may be delayed for a long time interval without affecting the kernel operations; the interested process will just keep waiting for the data. Noncritical deferrable actions are performed by means of separate functions that are discussed in the later section “Softirqs and Tasklets.”

Regardless of the kind of circuit that caused the interrupt, all I/O interrupt handlers perform the same four basic actions:

1. Save the IRQ value and the register’s contents on the Kernel Mode stack.
2. Send an acknowledgment to the PIC that is servicing the IRQ line, thus allowing it to issue further interrupts.
3. Execute the interrupt service routines (ISRs) associated with all the devices that share the IRQ.
4. Terminate by jumping to the ret_from_intr( ) address.

Several descriptors are needed to represent both the state of the IRQ lines and the functions to be executed when an interrupt occurs. Figure 4-4 represents in a schematic way the hardware circuits and the software functions used to handle an interrupt. These functions are discussed in the following sections.

<img id="Figure_4-4" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_4.PNG">

<h4 id="4.6.1.1">4.6.1.1 中断向量表</h4>

As illustrated in Table 4-2, physical IRQs may be assigned any vector in the range 32–238. However, Linux uses vector 128 to implement system calls.

The IBM-compatible PC architecture requires that some devices be statically connected to specific IRQ lines. In particular:

1. The interval timer device must be connected to the IRQ0 line (see Chapter 6).
2. The slave 8259A PIC must be connected to the IRQ2 line (although more advanced PICs are now being used, Linux still supports 8259A-style PICs).
3. The external mathematical coprocessor must be connected to the IRQ13 line (although recent 80 × 86 processors no longer use such a device, Linux continues to support the hardy 80386 model).
4. In general, an I/O device can be connected to a limited number of IRQ lines. (As a matter of fact, when playing with an old PC where IRQ sharing is not possible, you might not succeed in installing a new card because of IRQ conflicts with other already present hardware devices.)

表4-2 Linux中断向量表

| 中断线号 | 使用范围 |
| -------- | -------- |
| 0–19      | 不可屏蔽中断和异常 |
| 20–31     | 为Intel保留 |
| 32–127    | 外部中断 |
| 128       | 系统调用专用 |
| 129–238   | 外部中断 |
| 239       | APIC定时器中断 |
| 240       | APIC温度中断 |
| 241–250   | 保留 |
| 251–253   | CPU之间的中断 |
| 254       | APIC错误中断 |
| 255       | APIC伪中断 |

There are three ways to select a line for an IRQ-configurable device:

* By setting hardware jumpers (only on very old device cards).

* By a utility program shipped with the device and executed when installing it. Such a program may either ask the user to select an available IRQ number or probe the system to determine an available number by itself.

* By a hardware protocol executed at system startup. Peripheral devices declare which interrupt lines they are ready to use; the final values are then negotiated to reduce conflicts as much as possible. Once this is done, each interrupt handler can read the assigned IRQ by using a function that accesses some I/O ports of the device. For instance, drivers for devices that comply with the Peripheral Component Interconnect (PCI) standard use a group of functions such as pci_read_config_byte( ) to access the device configuration space.

Table 4-3 shows a fairly arbitrary arrangement of devices and IRQs, such as those that might be found on one particular PC.

Table 4-3. An example of IRQ assignment to I/O devices

| IRQ | INT | Hardware device |
| --- | --- | --------------- |
| 0 | 32 | Timer |
| 1 | 33 | Keyboard |
| 2 | 34 | PIC cascading |
| 3 | 35 | Second serial port |
| 4 | 36 | First serial port |
| 6 | 38 | Floppy disk |
| 8 | 40 | System clock |
| 10| 42 | Network interface |
| 11| 43 | USB port, sound card |
| 12| 44 | PS/2 mouse |
| 13| 45 | Mathematical coprocessor |
| 14| 46 | EIDE disk controller’s first chain |
| 15| 47 | EIDE disk controller’s second chain |

The kernel must discover which I/O device corresponds to the IRQ number before enabling interrupts. Otherwise, for example, how could the kernel handle a signal from a SCSI disk without knowing which vector corresponds to the device? The correspondence is established while initializing each device driver (see Chapter 13).

<h4 id="4.6.1.2">4.6.1.2 IRQ数据结构</h4>

As always, when discussing complicated operations involving state transitions, it helps to understand first where key data is stored. Thus, this section explains the data structures that support interrupt handling and how they are laid out in various descriptors. Figure 4-5 illustrates schematically the relationships between the main descriptors that represent the state of the IRQ lines. (The figure does not illustrate the data structures needed to handle softirqs and tasklets; they are discussed later in this chapter.)

<img id="Figure_4-5" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_5.PNG">

Every interrupt vector has its own irq_desc_t descriptor, whose fields are listed in Table 4-4. All such descriptors are grouped together in the irq_desc array.

Table 4-4. The irq_desc_t descriptor

| 成员 | 描述 |
| ---- | ---- |
| handler | ---- |
| handler_data | ---- |
| action | ---- |
| status | ---- |
| depth | ---- |
| irq_count | ---- |
| irqs_unhandled | ---- |
| lock | ---- |

An interrupt is unexpected if it is not handled by the kernel, that is, either if there is no ISR associated with the IRQ line, or if no ISR associated with the line recognizes the interrupt as raised by its own hardware device. Usually the kernel checks the number of unexpected interrupts received on an IRQ line, so as to disable the line in case a faulty hardware device keeps raising an interrupt over and over. Because the IRQ line can be shared among several devices, the kernel does not disable the line as soon as it detects a single unhandled interrupt. Rather, the kernel stores in the irq_count and irqs_unhandled fields of the irq_desc_t descriptor the total number of interrupts and the number of unexpected interrupts, respectively; when the 100,000th interrupt is raised, the kernel disables the line if the number of unhandled interrupts is above 99,900 (that is, if less than 101 interrupts over the last 100,000 received are expected interrupts from hardware devices sharing the line).

The status of an IRQ line is described by the flags listed in Table 4-5.

Table 4-5. Flags describing the IRQ line status

| 标志 | 描述 |
| ---- | ---- |
| IRQ_INPROGRESS | IRQ的服务程序正在被执行 |
| IRQ_DISABLED   | IRQ线被禁止 |
| IRQ_PENDING    | IRQ被挂起 |
| IRQ_REPLAY     | IRQ的服务程序正在被执行 |
| IRQ_AUTODETECT | IRQ的服务程序正在被执行 |
| IRQ_WAITING    | IRQ的服务程序正在被执行 |
| IRQ_LEVEL      | IRQ的服务程序正在被执行 |
| IRQ_MASKED     | IRQ的服务程序正在被执行 |
| IRQ_PER_CPU    | IRQ的服务程序正在被执行 |

The depth field and the IRQ_DISABLED flag of the irq_desc_t descriptor specify whether the IRQ line is enabled or disabled. Every time the disable_irq() or disable_irq_nosync() function is invoked, the depth field is increased; if depth is equal to 0, the function disables the IRQ line and sets its IRQ_DISABLED flag.* Conversely, each invocation of the enable_irq() function decreases the field; if depth becomes 0, the function enables the IRQ line and clears its IRQ_DISABLED flag.

During system initialization, the init_IRQ( ) function sets the status field of each IRQ main descriptor to IRQ_DISABLED. Moreover, init_IRQ( ) updates the IDT by replacing the interrupt gates set up by setup_idt() (see the section “Preliminary Initialization of the IDT,” earlier in this chapter) with new ones. This is accomplished through the following statements:

    for (i = 0; i < NR_IRQS; i++)
        if (i+32 != 128)
            set_intr_gate(i+32,interrupt[i]);

This code looks in the interrupt array to find the interrupt handler addresses that it uses to set up the interrupt gates. Each entry n of the interrupt array stores the address of the interrupt handler for IRQn (see the later section “Saving the registers for the interrupt handler”). Notice that the interrupt gate corresponding to vector 128 is left untouched, because it is used for the system call’s programmed exception.

In addition to the 8259A chip that was mentioned near the beginning of this chapter, Linux supports several other PIC circuits such as the SMP IO-APIC, Intel PIIX4’s internal 8259 PIC, and SGI’s Visual Workstation Cobalt (IO-)APIC. To handle all such devices in a uniform way, Linux uses a PIC object, consisting of the PIC name and seven PIC standard methods. The advantage of this object-oriented approach is that drivers need not to be aware of the kind of PIC installed in the system. Each driver-visible interrupt source is transparently wired to the appropriate controller. The data structure that defines a PIC object is called hw_interrupt_type (also called hw_irq_controller).

For the sake of concreteness, let’s assume that our computer is a uniprocessor with two 8259A PICs, which provide 16 standard IRQs. In this case, the handler field in each of the 16 irq_desc_t descriptors points to the i8259A_irq_type variable, which describes the 8259A PIC. This variable is initialized as follows:

    struct hw_interrupt_type i8259A_irq_type = {
        .typename = "XT-PIC",
        .startup = startup_8259A_irq,
        .shutdown = shutdown_8259A_irq,
        .enable = enable_8259A_irq,
        .disable = disable_8259A_irq,
        .ack = mask_and_ack_8259A,
        .end = end_8259A_irq,
        .set_affinity = NULL
    };

The first field in this structure, "XT-PIC", is the PIC name. Next come the pointers to six different functions used to program the PIC. The first two functions start up and shut down an IRQ line of the chip, respectively. But in the case of the 8259A chip, these functions coincide with the third and fourth functions, which enable and disable the line. The mask_and_ack_8259A( ) function acknowledges the IRQ received by sending the proper bytes to the 8259A I/O ports. The end_8259A_irq() function is invoked when the interrupt handler for the IRQ line terminates. The last set_affinity method is set to NULL: it is used in multiprocessor systems to declare the “affinity” of CPUs for specified IRQs—that is, which CPUs are enabled to handle specific IRQs.

As described earlier, multiple devices can share a single IRQ. Therefore, the kernel maintains irqaction descriptors (see Figure 4-5 earlier in this chapter), each of which refers to a specific hardware device and a specific interrupt. The fields included in such descriptor are shown in Table 4-6, and the flags are shown in Table 4-7.

Table 4-6. Fields of the irqaction descriptor

<h4 id="4.6.1.3">4.6.1.3 多核系统中的IRQ分配</h4>

Linux sticks to the Symmetric Multiprocessing model (SMP); this means, essentially, that the kernel should not have any bias toward one CPU with respect to the others. As a consequence, the kernel tries to distribute the IRQ signals coming from the hardware devices in a round-robin fashion among all the CPUs. Therefore, all the CPUs should spend approximately the same fraction of their execution time servicing I/O interrupts.

In the earlier section “The Advanced Programmable Interrupt Controller (APIC),” we said that the multi-APIC system has sophisticated mechanisms to dynamically distribute the IRQ signals among the CPUs.

During system bootstrap, the booting CPU executes the setup_IO_APIC_irqs() function
to initialize the I/O APIC chip. The 24 entries of the Interrupt Redirection Table
of the chip are filled, so that all IRQ signals from the I/O hardware devices can be
routed to each CPU in the system according to the “lowest priority” scheme (see the
earlier section “IRQs and Interrupts”). During system bootstrap, moreover, all CPUs
execute the setup_local_APIC() function, which takes care of initializing the local
APICs. In particular, the task priority register (TPR) of each chip is initialized to a fixed
value, meaning that the CPU is willing to handle every kind of IRQ signal, regardless
of its priority. The Linux kernel never modifies this value after its initialization.

All task priority registers contain the same value, thus all CPUs always have the same
priority. To break a tie, the multi-APIC system uses the values in the arbitration priority
registers of local APICs, as explained earlier. Because such values are automatically
changed after every interrupt, the IRQ signals are, in most cases, fairly
distributed among all CPUs.*

In short, when a hardware device raises an IRQ signal, the multi-APIC system selects
one of the CPUs and delivers the signal to the corresponding local APIC, which in
turn interrupts its CPU. No other CPUs are notified of the event.

All this is magically done by the hardware, so it should be of no concern for the kernel
after multi-APIC system initialization. Unfortunately, in some cases the hardware
fails to distribute the interrupts among the microprocessors in a fair way (for
instance, some Pentium 4–based SMP motherboards have this problem). Therefore,
Linux 2.6 makes use of a special kernel thread called kirqd to correct, if necessary,
the automatic assignment of IRQs to CPUs.

The kernel thread exploits a nice feature of multi-APIC systems, called the IRQ affinity
of a CPU: by modifying the Interrupt Redirection Table entries of the I/O APIC, it
is possible to route an interrupt signal to a specific CPU. This can be done by invoking
the set_ioapic_affinity_irq() function, which acts on two parameters: the IRQ
vector to be rerouted and a 32-bit mask denoting the CPUs that can receive the IRQ.
The IRQ affinity of a given interrupt also can be changed by the system administrator by writing a new CPU bitmap mask into the /proc/irq/n/smp_affinity file (n being
the interrupt vector).

The kirqd kernel thread periodically executes the do_irq_balance() function, which
keeps track of the number of interrupt occurrences received by every CPU in the
most recent time interval. If the function discovers that the IRQ load imbalance
between the heaviest loaded CPU and the least loaded CPU is significantly high, then
it either selects an IRQ to be “moved” from a CPU to another, or rotates all IRQs
among all existing CPUs.

<h4 id="4.6.1.4">4.6.1.4 多核系统中的IRQ分配</h4>

As mentioned in the section “Identifying a Process” in Chapter 3, the thread_info descriptor of each process is coupled with a Kernel Mode stack in a thread_union data structure composed by one or two page frames, according to an option selected when the kernel has been compiled. If the size of the thread_union structure is 8 KB, the Kernel Mode stack of the current process is used for every type of kernel control path: exceptions, interrupts, and deferrable functions (see the later section “Softirqs and Tasklets”). Conversely, if the size of the thread_union structure is 4 KB, the kernel makes use of three types of Kernel Mode stacks:

* The exception stack is used when handling exceptions (including system calls). This is the stack contained in the per-process thread_union data structure, thus the kernel makes use of a different exception stack for each process in the system.
* The hard IRQ stack is used when handling interrupts. There is one hard IRQ stack for each CPU in the system, and each stack is contained in a single page frame.
* The soft IRQ stack is used when handling deferrable functions (softirqs or tasklets; see the later section “Softirqs and Tasklets”). There is one soft IRQ stack for each CPU in the system, and each stack is contained in a single page frame.

All hard IRQ stacks are contained in the hardirq_stack array, while all soft IRQ
stacks are contained in the softirq_stack array. Each array element is a union of type
irq_ctx that span a single page. At the bottom of this page is stored a thread_info
structure, while the spare memory locations are used for the stack; remember that
each stack grows towards lower addresses. Thus, hard IRQ stacks and soft IRQ
stacks are very similar to the exception stacks described in the section “Identifying a
Process” in Chapter 3; the only difference is that the thread_info structure coupled
with each stack is associated with a CPU rather than a process.

The hardirq_ctx and softirq_ctx arrays allow the kernel to quickly determine the
hard IRQ stack and soft IRQ stack of a given CPU, respectively: they contain pointers
to the corresponding irq_ctx elements.

<h4 id="4.6.1.5">4.6.1.5 为中断服务程序保存寄存器</h4>

When a CPU receives an interrupt, it starts executing the code at the address found
in the corresponding gate of the IDT (see the earlier section “Hardware Handling of
Interrupts and Exceptions”).

As with other context switches, the need to save registers leaves the kernel developer
with a somewhat messy coding job, because the registers have to be saved and
restored using assembly language code. However, within those operations, the processor
is expected to call and return from a C function. In this section, we describe
the assembly language task of handling registers; in the next, we show some of the
acrobatics required in the C function that is subsequently invoked.

Saving registers is the first task of the interrupt handler. As already mentioned, the
address of the interrupt handler for IRQn is initially stored in the interrupt[n] entry
and then copied into the interrupt gate included in the proper IDT entry.

The interrupt array is built through a few assembly language instructions in the
arch/i386/kernel/entry.S file. The array includes NR_IRQS elements, where the NR_IRQS
macro yields either the number 224 if the kernel supports a recent I/O APIC chip,* or
the number 16 if the kernel uses the older 8259A PIC chips. The element at index n
in the array stores the address of the following two assembly language instructions:

    pushl $n-256
    jmp common_interrupt

The result is to save on the stack the IRQ number associated with the interrupt
minus 256. The kernel represents all IRQs through negative numbers, because it
reserves positive interrupt numbers to identify system calls (see Chapter 10). The
same code for all interrupt handlers can then be executed while referring to this
number. The common code starts at label common_interrupt and consists of the following
assembly language macros and instructions:

    common_interrupt:
        SAVE_ALL
        movl %esp,%eax
        call do_IRQ
        jmp ret_from_intr

The SAVE_ALL macro expands to the following fragment:

    cld
    push %es
    push %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    movl $__USER_DS,%edx
    movl %edx,%ds
    movl %edx,%es

SAVE_ALL saves all the CPU registers that may be used by the interrupt handler on the
stack, except for eflags, cs, eip, ss, and esp, which are already saved automatically by
the control unit (see the earlier section “Hardware Handling of Interrupts and Exceptions”).
The macro then loads the selector of the user data segment into ds and es.

After saving the registers, the address of the current top stack location is saved in the
eax register; then, the interrupt handler invokes the do_IRQ() function. When the ret
instruction of do_IRQ() is executed (when that function terminates) control is transferred
to ret_from_intr( ) (see the later section “Returning from Interrupts and
Exceptions”).

<h4 id="4.6.1.6">4.6.1.6 do_IRQ()函数</h4>

<h4 id="4.6.1.7">4.6.1.7 __do_IRQ()函数</h4>

<h4 id="4.6.1.8">4.6.1.8 Reviving a lost interrupt</h4>

The _ _do_IRQ() function is small and simple, yet it works properly in most cases.
Indeed, the IRQ_PENDING, IRQ_INPROGRESS, and IRQ_DISABLED flags ensure that interrupts
are correctly handled even when the hardware is misbehaving. However, things
may not work so smoothly in a multiprocessor system.

Suppose that a CPU has an IRQ line enabled. A hardware device raises the IRQ line,
and the multi-APIC system selects our CPU for handling the interrupt. Before the
CPU acknowledges the interrupt, the IRQ line is masked out by another CPU; as a
consequence, the IRQ_DISABLED flag is set. Right afterwards, our CPU starts handling
the pending interrupt; therefore, the do_IRQ() function acknowledges the interrupt
and then returns without executing the interrupt service routines because it finds the
IRQ_DISABLED flag set. Therefore, even though the interrupt occurred before the IRQ
line was disabled, it gets lost.

To cope with this scenario, the enable_irq() function, which is used by the kernel to
enable an IRQ line, checks first whether an interrupt has been lost. If so, the function
forces the hardware to generate a new occurrence of the lost interrupt:

    spin_lock_irqsave(&(irq_desc[irq].lock), flags);
    if (--irq_desc[irq].depth == 0) {
        irq_desc[irq].status &= ~IRQ_DISABLED;
        if (irq_desc[irq].status & (IRQ_PENDING | IRQ_REPLAY))
                == IRQ_PENDING) {
            irq_desc[irq].status |= IRQ_REPLAY;
            hw_resend_irq(irq_desc[irq].handler,irq);
        }
        irq_desc[irq].handler->enable(irq);
    }
    spin_lock_irqrestore(&(irq_desc[irq].lock), flags);

The function detects that an interrupt was lost by checking the value of the IRQ_PENDING
flag. The flag is always cleared when leaving the interrupt handler; therefore, if the IRQ
line is disabled and the flag is set, then an interrupt occurrence has been acknowledged
but not yet serviced. In this case the hw_resend_irq() function raises a new interrupt.
This is obtained by forcing the local APIC to generate a self-interrupt (see the later section
“Interprocessor Interrupt Handling”). The role of the IRQ_REPLAY flag is to ensure
that exactly one self-interrupt is generated. Remember that the _ _do_IRQ() function
clears that flag when it starts handling the interrupt.

<h4 id="4.6.1.7">4.6.1.7 中断服务程序</h4>

<h4 id="4.6.1.8">4.6.1.8 动态分配IRQ线</h4>

As noted in section “Interrupt vectors,” a few vectors are reserved for specific
devices, while the remaining ones are dynamically handled. There is, therefore, a way
in which the same IRQ line can be used by several hardware devices even if they do
not allow IRQ sharing. The trick is to serialize the activation of the hardware devices
so that just one owns the IRQ line at a time.

Before activating a device that is going to use an IRQ line, the corresponding driver
invokes request_irq( ). This function creates a new irqaction descriptor and initializes
it with the parameter values; it then invokes the setup_irq( ) function to insert
the descriptor in the proper IRQ list. The device driver aborts the operation if setup_
irq( ) returns an error code, which usually means that the IRQ line is already in use
by another device that does not allow interrupt sharing. When the device operation
is concluded, the driver invokes the free_irq( ) function to remove the descriptor
from the IRQ list and release the memory area.

Let’s see how this scheme works on a simple example. Assume a program wants to
address the /dev/fd0 device file, which corresponds to the first floppy disk on the system.*
The program can do this either by directly accessing /dev/fd0 or by mounting a
filesystem on it. Floppy disk controllers are usually assigned IRQ6; given this, a
floppy driver may issue the following request:

    request_irq(6, floppy_interrupt,
            SA_INTERRUPT|SA_SAMPLE_RANDOM, "floppy", NULL);

As can be observed, the floppy_interrupt( ) interrupt service routine must execute
with the interrupts disabled (SA_INTERRUPT flag set) and no sharing of the IRQ (SA_
SHIRQ flag missing). The SA_SAMPLE_RANDOM flag set means that accesses to the floppy
disk are a good source of random events to be used for the kernel random number
generator. When the operation on the floppy disk is concluded (either the I/O operation
on /dev/fd0 terminates or the filesystem is unmounted), the driver releases IRQ6:

    free_irq(6, NULL);

To insert an irqaction descriptor in the proper list, the kernel invokes the setup_irq(
) function, passing to it the parameters irq_nr, the IRQ number, and new (the
address of a previously allocated irqaction descriptor). This function:

1. Checks whether another device is already using the irq_nr IRQ and, if so,
whether the SA_SHIRQ flags in the irqaction descriptors of both devices specify
that the IRQ line can be shared. Returns an error code if the IRQ line cannot be
used.
2. Adds *new (the new irqaction descriptor pointed to by new) at the end of the list
to which irq_desc[irq_nr]->action points.
3. If no other device is sharing the same IRQ, the function clears the IRQ_DISABLED,
IRQ_AUTODETECT, IRQ_WAITING, and IRQ_INPROGRESS flags in the flags field of *new
and invokes the startup method of the irq_desc[irq_nr]->handler PIC object to
make sure that IRQ signals are enabled.

Here is an example of how setup_irq( ) is used, drawn from system initialization.
The kernel initializes the irq0 descriptor of the interval timer device by executing the
following instructions in the time_init( ) function (see Chapter 6):

    struct irqaction irq0 =
        {timer_interrupt, SA_INTERRUPT, 0, "timer", NULL, NULL};
    setup_irq(0, &irq0);

First, the irq0 variable of type irqaction is initialized: the handler field is set to the
address of the timer_interrupt( ) function, the flags field is set to SA_INTERRUPT, the
name field is set to "timer", and the fifth field is set to NULL to show that no dev_id
value is used. Next, the kernel invokes setup_irq( ) to insert irq0 in the list of
irqaction descriptors associated with IRQ0.



<h2 id="4.7">4.7 软中断和Tasklet</h2>

<h2 id="4.8">4.8 工作队列</h2>

<h2 id="4.9">4.9 中断和异常的返回</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

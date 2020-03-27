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
        + [4.5.1 为调用C函数准备环境](#4.5.1)
        + [4.5.2 真正的异常处理程序](#4.5.2)
    - [4.6 中断处理](#4.6)
        + [4.6.1 I/O中断处理](#4.6.1)
            * [4.6.1.1 中断向量表](#4.6.1.1)
            * [4.6.1.2 IRQ相关数据结构](#4.6.1.2)
            * [4.6.1.3 多核系统中的IRQ分配](#4.6.1.3)
            * [4.6.1.4 内核态堆栈](#4.6.1.4)
            * [4.6.1.5 为调用中断服务程序准备环境](#4.6.1.5)
            * [4.6.1.6 do_IRQ()函数](#4.6.1.6)
            * [4.6.1.7 __do_IRQ()](#4.6.1.7)
            * [4.6.1.8 恢复中断](#4.6.1.8)
            * [4.6.1.9 中断服务程序](#4.6.1.9)
            * [4.6.1.10 动态分配IRQ](#4.6.1.10)
        + [4.6.2 CPU间中断处理](#4.6.2)
    - [4.7 软中断和Tasklet](#4.7)
        + [4.7.1 软中断](#4.7.1)
            * [4.7.1.1 软中断使用的数据结构](#4.7.1.1)
            * [4.7.1.2 处理软中断](#4.7.1.2)
            * [4.7.1.3 do_softirq函数](#4.7.1.3)
            * [4.7.1.4 __do_softirq()函数](#4.7.1.4)
            * [4.7.1.5 ksoftirqd内核线程](#4.7.1.5)
        + [4.7.2 Tasklet](#4.7.2)
    - [4.8 工作队列](#4.8)
    - [4.9 中断和异常的返回](#4.9)

---

<h1 id="4">4 中断和异常</h1>

中断定义：通常被定义为改变CPU指令执行序列的事件。

中断可以分为异步和同步中断：

* **同步中断**，是由CPU在执行指令时由CPU控制单元产生的中断。这意味着，CPU必须停止指令的执行，转而响应中断。通常情况下，同步中断就是指 **异常**。

* **异步中断**，是由外部设备随机产生的，信号采样按照CPU时钟信号。异步中断就是我们通常情况下所指的中断。一般是定时器中断和I/O设备中断。

异常通常分为2类：一类是编程错误，另外一类就是需要内核处理的异常情况。编程错误，比如程序异常中止，处理这种异常，内核只需要给当前进程发送一个信号即可。而需要内核处理的异常，比如页错误、通过汇编语言指令比如int或sysenter等请求内核服务等，需要内核作出相应的处理。

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

            陷阱指令造成的异常。陷阱同Fault一样，因为没有破坏内核态栈中的任何东西，异常处理程序中止后，可以继续执行eip寄存器中的指令。它的设计目的主要是为了调试，告知调试器正在执行一个特殊的指令（比如，在程序里打一个断点）。一旦用户查看完断点处信息后，他就可以让程序继续执行了。

        + Abort

            发生严重错误时的异常。此时，CPU控制单元发生异常，但是无法确定发生错误的指令的准确位置，也就是说，在eip寄存器中的指令并不是造成错误的指令。这类错误一般是硬件错误或系统页表中非法或者不一致的地址等。控制单元发出信号，让CPU跳转到异常处理程序。Abort异常处理程序一般都是中止程序的执行。

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
|1| Debug         | trap/fault| debug() | SIGTRAP |
|2| NMI           | -     | nmi() | - |
|3| 断点           | trap | int3() | SIGTRAP |
|4| 溢出           | trap | overflow() | SIGSEGV |
|5| 边界检查        | fault | bounds() | SIGSEGV |
|6| 非法操作码      | fault | invalid_op() | SIGILL |
|7| 设备不可用      | fault | device_not_available() | - |
|8| 串行处理异常错误 | abort | doublefault_fn() | - |
|9| 协处理器错误    | abort | coprocessor_segment_overrun() | SIGFPE |
|10| 非法TSS       | fault | invalid_TSS() | SIGSEGV |
|11| 段引用错误     | fault | segment_not_present() | SIGBUS |
|12| 栈段错误       | fault | stack_segment() | SIGBUS |
|13| 常规保护       | fault | general_protection() | SIGSEGV |
|14| 页错误         | fault | page_fault() | SIGSEGV |
|15| Intel保留     | -     | - | - |
|16| 浮点错误       | fault | coprocessor_error() | SIGFPE |
|17| 对齐检查       | fault | alignment_check() | SIGBUS |
|18| 机器检查       | abort | machine_check() | - |
|19| SIMD浮点异常   | fault | simd_coprocessor_error() | SIGFPE |

Intel保留20-31未来使用。如上表所示，每个异常都有一个专门的处理函数处理，并给造成异常的进程发送一个信号。

<h3 id="4.2.3">4.2.3 中断描述符表</h3>

现在，我们已经知道了中断信号是如何从设备发出，然后经过高级可编程中断控制器的分配，到达各个指定的CPU中。那么，剩下的工作就是内核的了，内核使用一个中断描述符表（IDT），记录每个中断或者异常编号以及相应的处理函数。那么，收到中断信号后，将相应的处理函数的地址加载到eip寄存器中执行即可。

IDT中，每一项对应一个中断或者异常，大小8个字节。因而，IDT需要256x8=2048个字节大小的存储空间。

IDT的物理地址存储在CPU寄存器`idtr`中：包括IDT的基地址和最大长度。在使能中断之前，必须使用lidt汇编指令初始化IDT。

IDT包含三种类型的描述符，使用Type位域表示（40-43位）。下图分别解释了这三种描述符各个位的意义。

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

2. 读取IDT中的第N项；（在后面的描述中，假设包含的是中断门或陷阱门）

3. 获取GDT的基地址，遍历GDT找到IDT第N项中的段选择器标识的段描述符。这个描述符指定了包含中断或异常处理程序的段的基地址。

4. 确保中断合法性。

    首先比较cs寄存器中的CPL（当前特权等级）和包含在GDT中的段描述符的DPL（描述符特权等级），如果CPL小于DPL，产生`常规保护`异常，因为中断处理程序的特权等级不能比造成中断的程序的低。对于可编程异常，还会做进一步的安全检查：比较当前特权等级（CPL）和IDT中包含的描述符的DPL，如果DPL小于CPL，则产生`常规保护`的异常。后一项检查，可以阻止用户应用程序访问特定的trap或中断门。

5. 检查特权等级是否发生变化。如果CPL与描述符中的DPL不同，控制单元应该使用新特权等级下的堆栈。

    1. 读取tr寄存器，访问运行中的进程的TSS段；
    2. 使用新特权等级对应的堆栈段和堆栈指针加载ss和esp寄存器；（这些值存储在TSS中）
    3. 在新的堆栈中，保存旧任务的ss和esp寄存器值。（处理完中断或异常后，还要恢复到旧任务执行）

    > 其实对于Linux来说，只使用了supervisor和user两种特权等级。所以中断应该都是在supervisor特权等级下运行。

6. 根据造成异常的指令的逻辑地址，加载cs和eip寄存器（异常解决后，程序可以继续从这儿执行）；

7. 保存eflags、cs和eip到堆栈中；

8. 如果异常携带异常错误码，将其保存在堆栈中；

9. 根据IDT中的第N项内容，加载cs和eip寄存器。

至此，CPU控制单元跳转到中断或异常处理程序处开始执行。等到中断或异常处理完成后，把CPU的使用权让给之前被中断的进程，使用iret指令，该指令强迫控制单元执行下面步骤：

1. 加载被中断进程的cs，eip和eflags寄存器。（如果压栈过异常错误码，应该在执行iret指令之前弹出）
2. 检查CPL是否等于cs寄存器中的CPL，如果相等，则iret指令结束执行；否则，继续。
3. 加载旧特权等级的ss和esp寄存器值。
4. 检查ds、es、fs和gs寄存器中的值。如果它们之中任何一个的描述符中的DPL小于CPL，则清除相应的段寄存器。这么做，可以禁止用户态程序使用先前内核态的段寄存器。如果这些寄存器没有被清除，恶意用户态程序就可以利用它们访问内核地址空间。

<h2 id="4.3">4.3 嵌套中断和异常</h2>

讲解这部分之前，我们先阐述一个概念-**内核控制路径：就是一段在内核态执行的代码**，比如说，异常处理程序，中断处理程序，系统调用处理，内核线程等等在内核态执行的代码。所以，内核态程序被激活的方式有：

1. 系统调用（异常的一种）
2. 异常
3. 中断
4. 内核线程

上面的任意一种方式，都可以让CPU执行内核态的代码。比如，I/O设备引发一个中断，相应的内核态程序，首先，应该是保存内核态堆栈中的CPU寄存器的内容；然后，执行中断处理程序；最后，再恢复这些寄存器的内容。所以，在后面的描述中，我们使用`内核控制路径`这个术语代替一段可执行的内核态代码这种表述。使用`内核控制路径`的好处就是，它是从英语直译过来的，可能会更好地表达程序代码执行的顺序性，是一个过程；这样在描述中断嵌套时更有意义。

内核控制路径可以任意嵌套；如下图所示，用户态的程序被中断打断，进入内核态响应中断；而这时候又来了其它中断，就会响应最新的中断，以此类推；但是，执行完一个中断处理程序之后，会回到之前的状态执行。这样，最后又回到了用户态。

<img id="Figure_4-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_3.PNG">

图4-3 内核控制路径的一个嵌套异常的示例

允许内核控制路径嵌套的代价就是中断处理程序不能阻塞，也就是说，中断处理程序运行时不能发生进程切换。恢复执行嵌套内核控制路径的所有数据都存储在内核态堆栈中，而该堆栈又和当前进程紧紧绑定在一起。通俗的说，中断处理程序相当于当前进程的资源，切换进程之前该中断资源必须释放掉。

假设内核没有bug，那么大部分的异常发生在用户态。实际上，要么是编程错误，要么是调试器故意触发的。而`页错误`异常发生在内核态，它是内核在访问物理地址时不存在引发的异常。处理这样的异常，内核挂起当前进程，切换到新进程，直到该请求页可用。因为`页错误`异常绝不会引发进一步的异常，所以，有关联的内核控制路径最多是2个（第一个是系统调用造成的，第二个是页错误造成的）。也就是说，页错误的异常最多嵌套2层。

和异常相反，尽管内核代表当前进程处理这些中断，但是，I/O设备引发的中断和当前进程没有直接数据引用的关系。事实上，给定一个中断，无法推断出是哪个进程在运行。所以，中断的执行不会引起进程的切换，也就可以无限嵌套处理。

中断处理程序可以打断中断或异常处理程序执行，但是反过来，异常不能打断中断处理程序。中断处理程序绝对不能包含`页错误`的操作，因为这会诱发进程切换。

Linux嵌套执行中断或异常处理程序的两个主要原因是：

* 为了提高可编程中断控制器和设备控制器的吞吐量。内核正在处理一个中断的时候，能够及时响应另一个中断。
* 实现没有中断优先级的模型。这可以简化内核代码并提高可移植性。

在多核系统中，几个中断或异常处理程序可能会并发执行。更重要的是，异常处理程序可能由于进程切换，造成在一个CPU上启动，然后迁移到另一个CPU上执行。

<h2 id="4.4">4.4 初始化中断描述符表</h2>

至此，我们已经理解了X86架构如何在硬件层面如何处理中断和异常，那么接下来，我们看看Linux内核管理这些中断和异常。

同所有的设备一样，我们在使能硬件之前，必须先初始化其相关的数据结构。而Linux使用中断描述符表IDT记录管理所有的中断和异常。那么，首先，Linux内核应该把IDT的起始地址写入idtr寄存器，然后初始化所有的表项。这一步在初始化系统时完成。

因为汇编指令`int`允许用户进程发送任意编号的中断（0-255）。为此，IDT的初始化必须考虑阻止由用户进程int指令引发的非法中断和异常。可以通过将中断描述符表中的DPL域设为0来实现。如果用户进程试图发送非法中断信号，CPU控制单元比较CPL和DPL的值，发出`常规保护`的异常。

但是，大部分时候，用户态进程必须能够发送可编程异常。那么把相应的中断或陷阱门描述符的DPL域设为3即可。比如系统调用。

让我们看看Linux如何实现这种策略。

<h3 id="4.4.1">4.4.1 中断、陷阱和系统门</h3>

在之前的文章中，我们已经介绍过，Intel提供了三种类型的中断描述符：任务，中断和陷阱门描述符。Linux的分类有些不同：

1. 中断门

    和Intel的中断门相同。所有的用户进程不能访问（该门的DPL设为0）。所有Linux的中断处理程序都是通过中断门激活的，也就是说只能在内核态访问。

2. 系统门

    属于Intel的陷阱门，可以被用户态进程访问（该门的DPL设为3）。三个Linux异常处理程序对应的中断号分别是4、5和128，分别使用`into`、`bound`和`int $0x80`三条汇编指令发出对应的中断信号。

3. 系统中断门

    属于Intel的中断门，用户态进程可以访问（该门的DPL域设为3）。中断号为3的异常处理程序通过系统中断门激活，可以使用在用户态使用int3指令实现。

4. 陷阱门

    属于Intel陷阱门，不能被用户态程序访问（该门的DPL设为0）。用来访问大部分的异常处理程序。

5. 任务门

    属于Intel任务门，用户态进程不能访问（该门的DPL设为0）。专门访问处理`Double fault`异常的处理程序。

对应上面的5种分类，分别有相应的函数可以初始化IDT（这些函数与硬件架构息息相关），如下所示：

* set_intr_gate(n,addr)

    插入中断门。该门内的端选择器设为内核态代码所在的段。Offset被设为addr，就是中断处理程序的地址。DPL域设为0。

* set_system_gate(n,addr)

    插入系统门。其余描述与上面的函数相同。

* set_system_intr_gate(n,addr)

    插入系统中断门。该门内的端选择器设为内核态代码所在的段。Offset被设为addr，就是异常处理程序的地址。DPL域设为3。

* set_trap_gate(n,addr)

    插入陷阱门，DPL被设为3。其余与上面函数相同。

* set_task_gate(n,gdt)

    插入任务门。段选择器设为要执行的函数所在的段。Offset设为0，而DPL设为3。

<h3 id="4.4.2">4.4.2 IDT初次初始化</h3>

其实，IDT被初始化两次。第一次是在BIOS程序中，此时CPU还工作在实模式下。一旦Linux启动，IDT会被搬运到RAM的受保护区域并被第二次初始化，因为Linux不会使用任何BIOS程序。

IDT结构被存储在idt_table表中，包含256项。idt_descr变量存储IDT的大小和它的地址，在系统的初始化阶段，内核用来设置idtr寄存器，专用汇编指令是lidt。

内核初始化的时候，汇编函数setup_idt()用相同的中断门填充idt_table表的所有项，都指向ignore_int()中断处理函数：

    setup_idt:
        lea ignore_int, %edx
        movl $(__KERNEL_CS << 16), %eax
        movw %dx, %ax           /*  = 0x0010 = cs */
        movw $0x8e00, %dx       /* 中断门，DPL=0 */
        lea idt_table, %edi     /* 加载idt表的地址到寄存器edi中 */
        mov $256, %ecx
    rp_sidt:
        movl %eax, (%edi)       /* 设置中断处理函数 */
        movl %edx, 4(%edi)      /* 设置段描述符 */
        addl $8, %edi           /* 跳转到IDT表的下一项 */
        dec %ecx                /* 自减 */
        jne rp_sidt
        ret

中断处理函数`ignore_int()`，也是一个汇编语言编写的函数，相当于一个null函数，它执行：

1. 保存一些寄存器到堆栈中。
2. 调用printk()函数打印`Unknown interrupt`系统消息`。
3. 从堆栈中恢复寄存器的内容。
4. 执行iret指令回到调用处。

正常情况下，此时的中断处理函数ignore_int()是不应该被执行的。如果在console或者log日志中出现`Unknown interrupt`的消息，说明发生硬件错误或者内核错误。

完成这次IDT表的初始化之后，内核还会进行第二次初始化，用真正的trap或中断处理函数代替刚才的null函数。一旦这两步初始化都完成，IDT表就包含具体的中断、陷阱和系统门，用以控制每个中断请求。

接下来，我们分别从异常和中断进行详细的说明。

<h2 id="4.5">4.5 异常处理</h2>

当异常发生时，Linux内核给造成异常的进程发送一个信号，告知其发生了异常。比如，如果一个进程尝试除零操作，CPU会产生`除法错误`异常，相应的异常处理程序发送SIGFPE信号给当前进程，然后由其采取必要的步骤，恢复还是中止（如果该信号没有对应的处理程序，则中止）。

但是，除了这些常规的异常以外，Linux有时候会特意利用某些CPU异常管理硬件资源。比如，可以使用`Device not available`这个异常，结合cr0寄存器中的TS标志，强迫内核重新加载CPU的浮点寄存器，从而更新最新的值。还可以使用`Page Fault`页错误异常，用来推迟给进程分配新的页帧，直到该分配的时候。因为它的异常处理程序极其复杂，我们在后续的文章中再详细叙述这一部分的内容。

异常处理程序一般会执行下面三步：

1. 保存内核态堆栈中的大部分寄存器内容（这一部分一般是汇编语言编写）；
2. 处理异常（一般使用C语言函数实现）；
3. 退出异常处理程序（调用ret_from_exception()函数）。

为了更好地处理异常，必须正确地初始化IDT表中的每一项。这部分的工作都是由`trap_init()`函数实现的，通过调用set_trap_gate()、set_intr_gate()、set_system_gate()、set_system_intr_gate()和set_task_gate()这些辅助函数完成，`trap_init()`函数的一部分代码片段，如下所示：

    set_trap_gate(0,&divide_error);
    set_trap_gate(1,&debug);
    set_intr_gate(2,&nmi);
    set_system_intr_gate(3,&int3);
    set_system_gate(4,&overflow);
    set_system_gate(5,&bounds);
    set_trap_gate(6,&invalid_op);
    set_trap_gate(7,&device_not_available);
    set_task_gate(8,31);
    set_trap_gate(9,&coprocessor_segment_overrun);
    set_trap_gate(10,&invalid_TSS);
    set_trap_gate(11,&segment_not_present);
    set_trap_gate(12,&stack_segment);
    set_trap_gate(13,&general_protection);
    set_intr_gate(14,&page_fault);
    set_trap_gate(16,&coprocessor_error);
    set_trap_gate(17,&alignment_check);
    set_trap_gate(18,&machine_check);
    set_trap_gate(19,&simd_coprocessor_error);
    set_system_gate(128,&system_call);

值得特殊注意的是，中断号为8的异常`Double fault`，将其设为一个任务门，而不是陷阱门或系统门，这是因为它标志着内核发生了一个严重的错误。此时，内核认为堆栈中的值已经不可信，异常处理程序会尝试直接从寄存器中打印各个寄存器的值。当发生这个异常的时候，CPU从IDT表中的第9项中取出任务门描述符。该描述符指向存储在GDT表中的第32项的特定TSS段描述符。接下来，CPU从该TSS段描述符中加载eip和esp寄存器的值，然后处理器在此堆栈上，执行doublefault_fn()异常处理程序。

现在，让我们看看典型的异常处理程序到底执行什么操作吧。

<h3 id="4.5.1">4.5.1 为调用C函数准备环境</h3>

下面的描述中我们使用`handler_name`作为异常处理程序的名称。异常处理程序基本上都是下面这样的代码：（所有的异常和中断处理函数都可以在`linux\arch\x86\entry\entry_32.S`文件中找到）

    handler_name:
        pushl $0        /* 部分异常处理程序 */
        pushl $do_handler_name
        jmp error_code

上面的`pushl $0`汇编指令的作用就是在堆栈中本应该由控制单元自动插入硬件错误码的位置插入一个null值。然后就是把异常处理程序（C代码）的地址压栈。这个函数的命名方式是在异常处理函数的名称前缀`do_`字符。除了异常`Device not available`之外，`error_code`对于所有的异常处理程序都是一样的。`error_code`处的代码执行如下内容：

1. 保存上面提到的C函数可能使用的寄存器。
2. 发送cld指令，清除eflags中的DF方向标志，保证使用字符串指令的时候，edi和esi寄存器自增加。
3. 拷贝保存在堆栈`esp+36`处的硬件错误码写入到edx寄存器中，并将该堆栈中的值改写为-1。后面我们还要研究内核如何使用这个值区分出0x80异常。
4. 将堆栈`esp+32`处的C函数`do_handler_name()`的地址写入到edi寄存器中，将es的内容写入到堆栈中。
5. 将内核态堆栈的栈顶位置加载到eax寄存器中。
6. 将用户数据段选择器加载到ds和es寄存器中。
7. 调用edi寄存器中的C函数，此时，这个函数从eax和edx寄存器中获取参数，而不是从堆栈中。这种函数的调用方式，我们在学习__switch_to()函数时，已经了解过了。

> 总结：
>
> 我们前面已经提到过，异常处理程序和普通的进程是不一样的，它没有所谓的堆栈。但是，现在异常处理程序又是使用C语言编写的。要想使用C函数，必须手动构建好堆栈，所以，上面这7步的内容其实就是为执行`do_handler_name`函数构建好堆栈，而这个函数的特殊之处就是，参数是通过eax和edx寄存器传递过来的。

<h3 id="4.5.2">4.5.2 真正的异常处理程序</h3>

那`do_handler_name`之类的函数到底要执行什么内容呢？其实，它们最终也是调用一个统一处理函数`do_trap()`，它的主要代码如下所示。就是保存硬件错误码和异常号到当前进程描述符中，然后发送相应的信号给进程：

    current->thread.error_code = error_code;
    current->thread.trap_no = vector;
    force_sig(sig_number, current);

异常处理程序终止后，当前进程接收到信号。如果进程是在用户态，则信号交给进程自身的信号处理程序（如果存在的话）；如果是在内核态，则内核通常会杀死进程。

最后异常处理程序跳转到`ret_from_exception()`函数地址处，从异常状态返回。

<h2 id="4.6">4.6 中断处理</h2>

如前所述，我们知道异常的处理还是比较简单的，就是给相关的进程发送信号，而且不存在进程调度的问题，所以内核很快就处理完了异常。

但是，这种方法不适用于中断，因为当一个不相关的进程正在运行的时候，发送给特定进程的中断信号会被挂起，等到该进程执行的时候才会处理。所以，给中断发送一个信号没有太大意义。

另外，中断的处理与中断类型息息相关。所以，我们将中断分为3类：

1. I/O中断
2. 定时器中断
3. CPU之间的中断

<h3 id="4.6.1">4.6.1 I/O中断处理</h3>

中断资源是有限的，所以对于I/O中断处理程序来说，应该尽量为尽可能多的设备提供服务。比如PCI总线架构，几个设备共享同一个IRQ请求线。这意味中断矢量表是共享的，不能一一覆盖所有设备。比如下面的表4-3中，中断号43就被分配给了USB端口和声卡。但是，对于一些旧的架构来说，共享IRQ请求线不是那么可靠，比如ISA总线。

增强中断处理程序的灵活性，有下面两种方式：

* IRQ共享

    在每个中断处理程序中罗列所有共享该IRQ的设备的中断服务例程（ISR）。每次轮询一遍这些服务例程，判断是哪个设备发送的中断请求。所以，每次中断请求都要把所有的中断服务例程执行一遍。

* IRQ动态分配

    直到最后时刻，IRQ中断请求线才会与设备驱动程序关联起来。比如，只有当用户访问软盘设备的时候才会给软盘设备分配中断请求线IRQ。使用这种方法，即使不共享IRQ中断请求线，几个硬件设备也能使用相同的中断号。

众所周知，中断有轻重缓急之分，而且中断处理程序的执行时间不能过长。因为中断处理程序运行时，IRQ中断请求线的信号会被暂时忽略，所以，长时间执行且非重要的操作应该被延后执行。更为重要的是，代表中断处理程序执行的进程必须总是处于TASK_RUNING状态，或`系统冻结`中，因此，中断处理程序不能执行阻塞程序，比如I/O硬盘操作。

Linux将中断要执行的操作分为三类：

* 关键中断

    比如响应PIC控制器发送的中断，重新编程设置PIC或者设备控制器，更新设备和处理器访问的数据结构等。这些中断能够被快速执行且是关键数据，因为它们都必须被尽可能快的执行。在中断处理程序中立即执行这些关键操作，此时可屏蔽中断被禁止。

* 非关键中断

    更新只有处理器访问的数据结构的中断请求（比如，读取键盘按键按下后的键码）。这类中断在中断处理程序中也能很快完成处理。

* 非关键可延时中断

    比如拷贝缓存中的内容到进程的地址空间中的操作就是非关键可延时中断操作（比如，发送键盘的一行缓存到终端处理进程中）。这类操作完全可以延时一段时间执行，并不会影响内核操作。对于这类操作一般使用`软中断和tasklet`机制完成。

I/O中断处理的基本步骤是：

1. 保存IRQ值和内核态堆栈中寄存器值->恢复进程的时候使用。
2. 给PIC控制器发送应答，告知正在响应IRQ请求线，允许继续发送中断。
3. 执行中断服务例程（ISR）。
4. 从中断返回（跳转到ret_from_intr()函数地址）。

为了响应中断处理，需要几个数据结构和函数去描述IRQ请求线的状态和要执行的函数功能。图4-4展示了处理中断的过程原理图。其中的函数，后面描述。

<img id="Figure_4-4" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_4.PNG">

<h4 id="4.6.1.1">4.6.1.1 中断向量表</h4>

在表4-2中，我们列出了IRQ的分配，中断号对应32-238。另外，Linux使用中断号128实现系统调用。

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

对于IRQ可配置的设备，有三种方法选择IRQ中断请求线：

* 通过跳线帽（一般旧计算机时代使用）。

* 通过设备的程序进行设置。用户可以选择可用的IRQ请求线或者自行探查系统可用的IRQ中断请求线。

* 在系统启动阶段，按照硬件协议进行申请，然后通过协商，尽可能减少冲突。完成分配后，每个中断处理程序通过函数读取访问I/O设备的IRQ中断请求线。比如，遵循PCI总线标准的设备，可以使用一组类似pci_read_config_byte()的函数读取设备的配置空间。

表4-3展示了一个分配设备和IRQ的示例：

| IRQ | INT | Hardware device |
| --- | --- | --------------- |
| 0 | 32 | 定时器 |
| 1 | 33 | 键盘 |
| 2 | 34 | PIC级联 |
| 3 | 35 | 第二个串行端口 |
| 4 | 36 | 第一个串行端口 |
| 6 | 38 | 软盘 |
| 8 | 40 | 系统时钟 |
| 10| 42 | 网口 |
| 11| 43 | USB端口，声卡 |
| 12| 44 | PS/2鼠标 |
| 13| 45 | 协处理器 |
| 14| 46 | EIDE硬盘控制器的第一个链 |
| 15| 47 | EIDE硬盘控制器的第二个链 |

也就是说，内核必须在使能中断之前，知道哪个I/O设备对应哪个IRQ号。然后在设备驱动初始化的时候才能对应上正确的中断处理程序。

<h4 id="4.6.1.2">4.6.1.2 IRQ相关数据结构</h4>

那么，IRQ数据结构是什么样子呢？下图展示了IRQ数据结构以及它们之间的关系。该图中没有展示`软中断和tasklet`相关的数据结构和关系。因为我们后面会单独写文章对其进行阐述。

<img id="Figure_4-5" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_5.PNG">

中断矢量表中的每一项都包含一个`irq_desc_t`类型的描述符，它的成员如表4-4所示。所有的项都存储到`irq_desc`数组中。

表4-4 irq_desc_t结构成员

| 成员 | 描述 |
| ---- | ---- |
| handler | 指向PIC对象，响应PIC发送的中断请求 |
| handler_data | handler需要的数据 |
| action | 指向具体的中断服务例程 |
| status | 表明IRQ请求线的状态 |
| depth | IRQ线禁止使能标志 |
| irq_count | 中断计数（诊断使用） |
| irqs_unhandled | 未处理中断计数 |
| lock | 自旋锁，保护该数据结构的访问 |

非预期中断，就是那些可能没有中断服务例程（ISR）或者中断服务例程和中断请求线不匹配的中断。内核对于这类中断是不作处理的。但是内核如何检测这类中断呢？又是如何禁止这类中断呢？因为中断号是共享的，所以，内核不会一检测到非预期中断就禁止它，而是对于总的中断请求次数和未处理的中断次数进行计数。当总的中断次数达到100000次，而未处理的中断是99900次时，内核就会禁止该中断。

表4-5 展示了中断请求线的状态标志

| 标志 | 描述 |
| ---- | ---- |
| IRQ_INPROGRESS | IRQ的服务程序正在被执行 |
| IRQ_DISABLED   | IRQ线被禁止 |
| IRQ_PENDING    | IRQ被挂起 |
| IRQ_REPLAY     | IRQ被禁止，但是上一次还没有响应PIC |
| IRQ_AUTODETECT | 自动检测IRQ |
| IRQ_WAITING    | 内核在执行硬件设备探测时使用IRQ线;而且，相应的中断还没有被触发 |
| IRQ_LEVEL      | X86架构未使用 |
| IRQ_MASKED     | 未使用 |
| IRQ_PER_CPU    | X86架构未使用 |

`depth`和标志`IRQ_DISABLED`表明IRQ线被使能还是禁止。每次调用`disable_irq()`和`disable_irq_nosync()`函数，`depth`都会增加；如果`depth`大于0，则函数禁止IRQ线并且设置`IRQ_DISABLED`标志。相反，如果调用`enable_irq()`函数，`depth`会递减，如果`depth`等于0，则使能IRQ线并且清除`IRQ_DISABLED`标志。

系统启动时，调用`init_IRQ()`函数设置IRQ描述符中的`status`成员为`IRQ_DISABLED`。与讲解异常处理一样，也会调用`setup_idt()`类似的函数初始化IDT表，通过下面的代码段完成：

    for (i = 0; i < NR_IRQS; i++)
        if (i+32 != 128)
            set_intr_gate(i+32,interrupt[i]);

这段代码的功能就是遍历`interrupt`数组，查找各个中断处理程序的地址。需要注意的是，中断号128没有分配，留给系统调用作为异常使用。

除了8259A芯片之外，Linux还支持其它的PIC控制器，比如`SMP IO-APIC`、`Intel PIIX4内部的8259中断控制器`、`SGI的Visual Workstation Cobalt (IO-)APIC`。为了统一处理这些硬件，Linux内核使用了面向对象的编程思想，构建了一个PIC对象，包含PIC名称和7个PIC标准方法。这种设计的优点是驱动程序无需关注系统中到底是什么中断控制器，硬件的差异被屏蔽掉了。这个PIC对象的数据结构类型称为`hw_interrupt_type`。

我们更好理解，举一个具体的实例，假设计算机是单核，带有2个8259A中断控制器，提供16个标准的IRQ。那么`irq_desc_t`类型的描述符中的`handler`指向`hw_interrupt_type`类型的结构对象`i8259A_irq_type`，其成员如下所示：

    struct hw_interrupt_type i8259A_irq_type = {
        .typename = "XT-PIC",           /* PIC名称 */
        .startup = startup_8259A_irq,
        .shutdown = shutdown_8259A_irq,
        .enable = enable_8259A_irq,
        .disable = disable_8259A_irq,
        .ack = mask_and_ack_8259A,
        .end = end_8259A_irq,
        .set_affinity = NULL
    };

`"XT-PIC"`，中断控制器名称。`startup`和`shutdown`分别表示启动和关闭IRQ线，但是对于8259A来说，这两个函数与`enable`和`disable`两个函数相同。 `mask_and_ack_8259A()`应答中断控制器，`end_8259A_irq()`函数在中断处理程序结束时调用。`set_affinity`方法设为`NULL`， 这个方法是为多核系统设计的，用来声明CPU的亲和力`affinity`，也就是说为某个IRQ指定在哪个CPU上处理。

我们知道，多个设备可以共享一个IRQ。因此，内核必须为每个设备及其对应的中断维护一个数据结构，称为`irqaction`描述符。它的成员如下表所示：

表4-6 `irqaction`描述符的各个成员

| 成员 | 描述 |
| ---- | ---- |
| handler   | 中断服务例程（ISR） |
| flags     | 描述IRQ和设备之间的关系 |
| mask      | 未使用 |
| name      | I/O设备的名称 |
| dev_id    | 指向设备本身 |
| next      | 指向下一个irqaction |
| irq       | IRQ线 |
| dir       | 指向目录/proc/irq/n |

表4-7 `irqaction`的标志位

| 成员 | 描述 |
| ---- | ---- |
| SA_INTERRUPT | 执行中断处理程序时必须禁止中断 |
| SA_SHIRQ | 允许共享IRQ |
| SA_SAMPLE_RANDOM | 可以被当做随机数发生器 |

> `init_IRQ()`的代码实现随着硬件架构的发展，以及内核的不断优化升级，会不断变化，且变得越来越复杂。但是，万变不离其宗，核心的设计思想没变。

<h4 id="4.6.1.3">4.6.1.3 多核系统中的IRQ分配</h4>

我们知道SMP的全称是对称多处理系统，这意味，Linux内核不应该对一个CPU有任何偏向。于是，内核在CPU之间采用循环法（round-robin）分配IRQ。因此，所有的CPU响应中断的时间都差不多。

之前我们已经了解过，多APIC系统的分配IRQ机制非常复杂。

在系统引导阶段，负责引导的CPU执行`setup_IO_APIC_irqs()`函数初始化`I/O-APIC`芯片。也就是初始化其中断重定向表（24项），然后所有来自I/O设备的IRQ就可以被中继到各个CPU上，分配原则是`最低优先级优先`原则。在此期间，所有的CPU执行`setup_local_APIC()`函数，初始化自身的APIC控制器。当然也可以将中断控制器中的TPR（任务优先级寄存器）写入相同值，从而公平地对待每个CPU，按照循环的方式分配IRQ。一旦初始化完成，内核就不能再修改这个值了。至于实现循环，前面我们讲过了，请参考之前的文章。

简而言之，设备发出IRQ信号，多APIC系统选择一个CPU，并把中断信号发送给响应的私有APIC，继而，私有APIC中断CPU。

虽说初始化之后，内核本不应该在关心IRQ分配问题。但是不幸的是，有时候硬件在分配中断时会发生错误（比如，基于奔腾4的SMP主板就有这样的问题）。因此，Linux2.6内核使用一个特定的内核线程叫`kirqd`进行纠正IRQ的自动分配（如果有必要的话）。

内核线程使用多APIC系统一个很棒的功能，叫做CPU的IRQ亲和力：通过修改`I/O-APIC`的中断重定向表，将中断信号指定到新的CPU上。具体操作就是调用`set_ioapic_affinity_irq()`函数，它需要两个参数：需要重定向的IRQ矢量表和一个32位的掩码（用来表示接收IRQ的CPU）。系统管理员也可以通过写新的CPU位掩码到`/proc/irq/n/smp_affinity`文件中，修改响应中断的CPU。

`kirqd`内核线程周期性地执行`do_irq_balance()`函数，追踪最近一段时间内，每个CPU上接收到的中断次数。如果发现CPU的中断负载不均衡了，它就会选择将某个IRQ移到另一个负载低的CPU上，或者采用在所有的CPU上循环响应IRQ。

<h4 id="4.6.1.4">4.6.1.4 内核态堆栈</h4>

在学习标识进程的时候，我们已经知道每个进程的`thread_info`描述符和内核态堆栈使用一个联合体结构组合在一起，占用内存一个或者两个页帧，这取决于编译内核时的配置。如果这个联合体的大小是8KB，内核态堆栈可以被任何一种内核控制路径使用：异常处理程序，中断处理程序和可延时函数。相反，如果这个联合体的大小是4KB，内核使用三种类型的内核态堆栈：

* 异常堆栈

    处理异常时使用，包含系统调用。每个进程都有一个异常处理使用的堆栈。

* 硬IRQ堆栈

    用于处理中断。每个CPU具有一个硬IRQ堆栈。

* 软IRQ堆栈

    处理可延时函数时使用。比如，软中断或tasklet。每个CPU都有一个软IRQ堆栈。

软、硬IRQ堆栈分别使用`hardirq_stack`和`softirq_stack`两个数组存储。每个数组元素对应一个`irq_ctx`类型的联合体，占用一个页帧。该页帧的底部存储`thread_info`结构，其余的内存存储堆栈；因为堆栈的增长方向是递减的。因此软、硬IRQ堆栈与进程的堆栈非常相似，只是`thread_info`不同，一个是描述CPU，而另一个是描述进程。

<h4 id="4.6.1.5">4.6.1.5 为中断服务程序保存寄存器</h4>

我们已经知道，当CPU收到中断，它就会执行IDT表中对应的中断处理程序。

执行中断处理程序，意味着上下文切换。这部分的内容需要汇编语言编写，然后才能调用C函数。前面我们已经知道，中断处理程序的地址首先存储在`interrupt[]`数组中，然后才会被拷贝到IDT表中的某项对应的中断门。

中断数组的构建在`arch/i386/kernel/entry.S`文件中，都是汇编指令。数组的个数是`NR_IRQS`，如果内核支持`I/O-APIC`芯片，则`NR_IRQS`等于224，如果内核支持的是较旧的8259A中断控制器，则`NR_IRQS`等于16。数组的每一项包含的汇编函数的地址处的内容如下所示：

    pushl $n-256
    jmp common_interrupt

存储在堆栈上的IRQ号是中断减去256。也就是说，内核使用负数表示IRQ号，因为内核保留正数表示系统调用。对于通用中断代码，如下所示：

    common_interrupt:
        SAVE_ALL
        movl %esp,%eax
        call do_IRQ
        jmp ret_from_intr

宏`SAVE_ALL`展开如下所示：

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

`SAVE_ALL`保存中断处理程序可能用到的所有的CPU寄存器到堆栈上，除了eflags、cs、eip、ss和esp这些寄存器之外，因为这些寄存器是由CPU控制单元自动保存的。该宏用户代码段的选择符到ds寄存器中。

保存完所有的寄存器之后，栈顶位置就被存储在eax寄存器中；然后中断处理程序调用`do_IRQ()`函数。

<h4 id="4.6.1.6">4.6.1.6 do_IRQ()函数</h4>

函数`do_IRQ()`执行和中断有关的所有的服务例程，声明如下：

    __attribute__((regparm(3))) unsigned int do_IRQ(struct pt_regs *regs)

关键字`regparm`指示函数去eax寄存器中获取参数`regs`的值，如前所述，eax寄存器存储着中断使用的堆栈的栈顶位置。

函数`do_IRQ()`主要执行以下内容：

1. 执行`irq_enter()`宏，增加嵌套中断计数；
2. 如果堆栈的大小等于4KB，切换到硬IRQ堆栈；
3. 调用`__do_IRQ()`函数，然后把regs指针和IRQ号（regs->orig_eax）传递给它；
4. 如果在第2步切换到硬IRQ堆栈中，则拷贝ebx寄存器中的原始堆栈指针到esp寄存器中，以便切换回之前使用的异常堆栈或软IRQ堆栈中；
5. 执行`irq_exit()`宏，减少中断计数，检查是否有可延时处理的函数正在等待处理；
6. 终止：跳转到`ret_from_intr()`函数地址。

<h4 id="4.6.1.7">4.6.1.7 __do_IRQ()函数</h4>

`__do_IRQ()`函数接收IRQ号和指向`pt_regs`的指针作为参数，分别是通过eax和edx寄存器传递。然后，对中断作出应有的响应，代码片段如下所示：

    spin_lock(&(irq_desc[irq].lock));
    irq_desc[irq].handler->ack(irq);
    irq_desc[irq].status &= ~(IRQ_REPLAY | IRQ_WAITING);
    irq_desc[irq].status |= IRQ_PENDING;
    if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS))
            && irq_desc[irq].action) {
        irq_desc[irq].status |= IRQ_INPROGRESS;
        do {
            irq_desc[irq].status &= ~IRQ_PENDING;
            spin_unlock(&(irq_desc[irq].lock));
            handle_IRQ_event(irq, regs, irq_desc[irq].action);
            spin_lock(&(irq_desc[irq].lock));
        } while (irq_desc[irq].status & IRQ_PENDING);
        irq_desc[irq].status &= ~IRQ_INPROGRESS;
    }
    irq_desc[irq].handler->end(irq);
    spin_unlock(&(irq_desc[irq].lock));

上面的代码主要执行内容如下所示：

1. 加锁，保护IRQ描述符数据结构

    通过上面的代码，我们可以看出，在访问相应的IRQ描述符时，内核会请求自旋锁。这是防止不同CPU之间可能造成的并发访问。因为，在多核系统中，可能会发生同类型的其它CPU关心的中断，它们使用同一个IRQ描述符，所以造成访问冲突。

2. 响应PIC中断控制器

    加锁之后，函数调用IRQ描述符的`ack`方法，给中断控制器应答。如果使用的是旧的8259A中断控制器，使用mask_and_ack_8259A()响应PIC同时禁止IRQ线；屏蔽掉该IRQ线，保证CPU不再接收到这个类型的中断，直到中断处理程序完成处理。如果使用的是`I/O-APIC`，情况更为复杂。依赖于中断的类型，既可以使用`ack`方法响应PIC控制器也可以延时到中断处理程序结束再完成。

3. 设置IRQ描述符的标志

    设置`IRQ_PENDING`标志，因为此时已经应答过PIC中断控制器，但是还没有对其进行服务。也会清除`IRQ_WAITING`和`IRQ_REPLAY`标志。


4. 真正执行中断处理。

    此时，可能有三种意外情况需要处理：

    1. 设置了`IRQ_DISABLED`

        即使IRQ线被禁止，CPU还是有可能执行`__do_IRQ()`函数，所以需要特殊处理。

    2. 设置了`IRQ_INPROGRESS`

        多核系统中，此时可能另外一个CPU可能正在处理先前发生的相同中断。Linux对此的处理方式就是延后处理。这样的处理方式使内核架构更为简单，因为设备驱动程序的中断服务程序是不需要可重入的（它们的执行一般都是序列化的）。

    3. `irq_desc[irq].action`为空

        当没有与中断相关联的中断服务例程时，就会发生这种情况。通常，只有在内核探测硬件设备时才会发生这种情况。

    假设没有上面的三种情况，中断被正式处理。设置`IRQ_INPROGRESS`标志，并启动循环处理。每次迭代过程，清除`IRQ_PENDING`标志，释放中断自旋锁，然后执行调用`handle_IRQ_event()`执行中断服务程序。

5. 中断服务程序完成。

    释放自旋锁。

<h4 id="4.6.1.7.1">4.6.1.7.1 总结</h4>

其实内核经过这么多年的发展，在实现方式上已经发生了很大变化。但是其基本思想没变。比如我们以Linux4.4.203内核对于中断的处理为例，与上面的处理过程进行比较，理解其主要变化。

1. 调用do_IRQ函数。其入口位于entry_32.S文件中，是C语言实现的。

        common_interrupt:
            ASM_CLAC
            addl $-0x80, (%esp)  /* Adjust vector into the [-256, -1] range */
            SAVE_ALL
            TRACE_IRQS_OFF
            movl %esp, %eax
            call do_IRQ
            jmp ret_from_intr
            ENDPROC(common_interrupt)

2. do_IRQ函数原型为：

        /*
         + do_IRQ处理所有常规设备的IRQ。
         + 特殊的SMP系统中，CPU间的中断有自己特定的处理程序。
         */
        __visible unsigned int __irq_entry do_IRQ(struct pt_regs *regs)
        {
            struct pt_regs *old_regs = set_irq_regs(regs);
            struct irq_desc * desc;
            /* high bit used in ret_from_ code  */
            unsigned vector = ~regs->orig_ax;

            entering_irq();     /* 进入中断，并对中断进行嵌套计数 */

            /* entering_irq() tells RCU that we're not quiescent.  Check it. */
            RCU_LOCKDEP_WARN(!rcu_is_watching(), "IRQ failed to wake up RCU");

            desc = __this_cpu_read(vector_irq[vector]);   /* 取中段描述符 */

            if (!handle_irq(desc, regs)) {  /* handle_irq处理具体的中断服务程序 */
                ack_APIC_irq();

                if (desc != VECTOR_RETRIGGERED) {
                    pr_emerg_ratelimited("%s: %d.%d No irq handler for vector\n",
                                 __func__, smp_processor_id(),
                                 vector);
                } else {
                    __this_cpu_write(vector_irq[vector], VECTOR_UNUSED);
                }
            }

            exiting_irq(); /* 退出中断，并对中断进行嵌套计数递减 */

            set_irq_regs(old_regs);
            return 1;
        }

3. `handle_irq`函数最终调用的是下面的函数：

        static inline void generic_handle_irq_desc(struct irq_desc *desc)
        {
            desc->handle_irq(desc);
        }

4. 而我们之间已经说过`desc->handle_irq`的初始化在系统初始化时完成：

        //linux-2.6.32/arch/x86/kernel/irqinit.c
        void __init init_IRQ(void)
        {
            x86_init.irqs.intr_init();
        }
        //linux-2.6.32/arch/x86/kernel/x86_init.c
        struct x86_init_ops x86_init __initdata = {
        ......
            .irqs = {
                .pre_vector_init    = init_ISA_irqs, //被.intr_init调用
                .intr_init      = native_init_IRQ,
                .trap_init      = x86_init_noop,
            },
        ......
        }
        //linux-2.6.32/arch/x86/kernel/irqinit.c
        void __init native_init_IRQ(void)
        {
        ......
            /* Execute any quirks before the call gates are initialised: */
            x86_init.irqs.pre_vector_init(); //init_ISA_irqs
        ......
        }

        void __init init_ISA_irqs(void)
        {
            ......
            for (i = 0; i < NR_IRQS_LEGACY; i++) {
                ......
                set_irq_chip_and_handler_name(i, &i8259A_chip,
                                  handle_level_irq, "XT");
            }
        }

        void set_irq_chip_and_handler_name(unsigned int irq, struct irq_chip *chip,
                            irq_flow_handler_t handle, const char *name)
        {
            set_irq_chip(irq, chip);
            __set_irq_handler(irq, handle, 0, name);
        }

        void __set_irq_handler(unsigned int irq, irq_flow_handler_t handle,
                int is_chained,const char *name)
        {
            ....
            desc->handle_irq = handle;//handle 即为handle_level_irq
            ....
        }

5. 可见`desc->handle_irq(irq, desc);`执行的是`handle_level_irq(irq, desc)`。 我们进`入handle_level_irq(irq, desc)`看看都做了哪些操作：

        void handle_level_irq(unsigned int irq, struct irq_desc *desc)
        {
            mask_ack_irq(desc, irq); //屏蔽中断
            ......
            action = desc->action;
            action_ret = handle_IRQ_event(irq, action);
            ......
                desc->chip->unmask(irq); //打开中断
        }
        irqreturn_t handle_IRQ_event(unsigned int irq, struct irqaction *action)
        {
            ......
            do {
                ret = action->handler(irq, action->dev_id);//指向我们注册的中断处理函数
                ......
            } while (action);
            .....
        }

通过上面5步分析，我们知道，内核代码以及硬件设备在发生变化，但是中断处理的核心思想没有变。

<h4 id="4.6.1.8">4.6.1.8 恢复中断</h4>

The __do_IRQ() function is small and simple, yet it works properly in most cases. Indeed, the IRQ_PENDING, IRQ_INPROGRESS, and IRQ_DISABLED flags ensure that interrupts are correctly handled even when the hardware is misbehaving. However, things may not work so smoothly in a multiprocessor system.

Suppose that a CPU has an IRQ line enabled. A hardware device raises the IRQ line, and the multi-APIC system selects our CPU for handling the interrupt. Before the CPU acknowledges the interrupt, the IRQ line is masked out by another CPU; as a consequence, the IRQ_DISABLED flag is set. Right afterwards, our CPU starts handling the pending interrupt; therefore, the do_IRQ() function acknowledges the interrupt and then returns without executing the interrupt service routines because it finds the IRQ_DISABLED flag set. Therefore, even though the interrupt occurred before the IRQ line was disabled, it gets lost.

To cope with this scenario, the enable_irq() function, which is used by the kernel to enable an IRQ line, checks first whether an interrupt has been lost. If so, the function forces the hardware to generate a new occurrence of the lost interrupt:

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

The function detects that an interrupt was lost by checking the value of the IRQ_PENDING flag. The flag is always cleared when leaving the interrupt handler; therefore, if the IRQ line is disabled and the flag is set, then an interrupt occurrence has been acknowledged but not yet serviced. In this case the hw_resend_irq() function raises a new interrupt. This is obtained by forcing the local APIC to generate a self-interrupt (see the later section “Interprocessor Interrupt Handling”). The role of the IRQ_REPLAY flag is to ensure that exactly one self-interrupt is generated. Remember that the __do_IRQ() function clears that flag when it starts handling the interrupt.

<h4 id="4.6.1.9">4.6.1.9 中断服务程序</h4>

<h4 id="4.6.1.10">4.6.1.10 动态分配IRQ线</h4>

As noted in section “Interrupt vectors,” a few vectors are reserved for specific devices, while the remaining ones are dynamically handled. There is, therefore, a way in which the same IRQ line can be used by several hardware devices even if they do not allow IRQ sharing. The trick is to serialize the activation of the hardware devices so that just one owns the IRQ line at a time.

Before activating a device that is going to use an IRQ line, the corresponding driver invokes request_irq(). This function creates a new irqaction descriptor and initializes it with the parameter values; it then invokes the setup_irq() function to insert the descriptor in the proper IRQ list. The device driver aborts the operation if setup_irq() returns an error code, which usually means that the IRQ line is already in use by another device that does not allow interrupt sharing. When the device operation is concluded, the driver invokes the free_irq() function to remove the descriptor from the IRQ list and release the memory area.

Let’s see how this scheme works on a simple example. Assume a program wants to address the /dev/fd0 device file, which corresponds to the first floppy disk on the system.* The program can do this either by directly accessing /dev/fd0 or by mounting a filesystem on it. Floppy disk controllers are usually assigned IRQ6; given this, a floppy driver may issue the following request:

    request_irq(6, floppy_interrupt,
            SA_INTERRUPT|SA_SAMPLE_RANDOM, "floppy", NULL);

As can be observed, the floppy_interrupt() interrupt service routine must execute with the interrupts disabled (SA_INTERRUPT flag set) and no sharing of the IRQ (SA_SHIRQ flag missing). The SA_SAMPLE_RANDOM flag set means that accesses to the floppy disk are a good source of random events to be used for the kernel random number generator. When the operation on the floppy disk is concluded (either the I/O operation on /dev/fd0 terminates or the filesystem is unmounted), the driver releases IRQ6:

    free_irq(6, NULL);

To insert an irqaction descriptor in the proper list, the kernel invokes the setup_irq() function, passing to it the parameters irq_nr, the IRQ number, and new (the address of a previously allocated irqaction descriptor). This function:

1. Checks whether another device is already using the irq_nr IRQ and, if so, whether the SA_SHIRQ flags in the irqaction descriptors of both devices specify that the IRQ line can be shared. Returns an error code if the IRQ line cannot be used.
2. Adds *new (the new irqaction descriptor pointed to by new) at the end of the list to which irq_desc[irq_nr]->action points.
3. If no other device is sharing the same IRQ, the function clears the IRQ_DISABLED, IRQ_AUTODETECT, IRQ_WAITING, and IRQ_INPROGRESS flags in the flags field of *new and invokes the startup method of the irq_desc[irq_nr]->handler PIC object to make sure that IRQ signals are enabled.

Here is an example of how setup_irq() is used, drawn from system initialization. The kernel initializes the irq0 descriptor of the interval timer device by executing the following instructions in the time_init() function (see Chapter 6):

    struct irqaction irq0 =
        {timer_interrupt, SA_INTERRUPT, 0, "timer", NULL, NULL};
    setup_irq(0, &irq0);

First, the irq0 variable of type irqaction is initialized: the handler field is set to the address of the timer_interrupt() function, the flags field is set to SA_INTERRUPT, the name field is set to "timer", and the fifth field is set to NULL to show that no dev_id value is used. Next, the kernel invokes setup_irq() to insert irq0 in the list of irqaction descriptors associated with IRQ0.

<h3 id="4.6.2">4.6.2 CPU间中断处理</h3>

<h2 id="4.7">4.7 软中断和Tasklet</h2>

在之前的文章中，讲解中断处理相关的概念的时候，提到过有些任务不是紧急的，可以延后一段时间执行。因为中断服务例程都是顺序执行的，在响应一个中断的时候不应该被打断。相反，这些可延时任务执行时，可以使能中断。那么，将这些任务从中断处理程序中剥离出来，可以有效地保证内核对于中断响应时间尽可能短。这对于时间苛刻的应用来说，这是一个很重要的属性，尤其是那些要求中断请求必须在毫秒级别响应的应用。

Linux2.6内核使用两种手段满足这项挑战：软中断和tasklet，还有工作队列。其中，工作队列我们单独在一篇文章中讲解。

软中断和tasklet这两个术语是息息相关的，因为tasklet是基于软中断实现的。事实上，出现在内核源代码中的`软中断`概念有时候指的就是这两个术语的统称。另一个广泛使用的术语是`中断上下文`：可以是内核正在执行的中断处理程序，也可以是一个可延时处理的函数。

软中断是静态分配好的（编译时），而tasklet是在运行时分配并初始化的（比如，加载内核模块的时候）。因为软中断的实现是可重入的，使用自旋锁保护它们的数据结构。所以软中断可以在多个CPU上并发运行。tasklet不需要考虑这些，因为它的处理完全由内核控制，也就是说，相同类型的tasklet总是顺序执行的。换句话说，不可能同时有2个以上的CPU执行相同类型的tasklet。当然了，不同类型的tasklet完全可以在多个CPU上同时执行。完全顺序执行的tasklet简化了驱动开发者的工作，因为tasklet不需要考虑可重入设计。

既然已经理解了软中断和tasklet的机制，那么实现这样的可延时函数需要哪些步骤呢？如下所示：

1. 初始化

    定义一个可延时函数。这一步，一般在内核初始化自身或者加载内核模块时完成。

2. 激活

    将上面定义的函数挂起。也就是等待内核下一次的调度执行。激活可以发生在任何时候。

3. 禁止

    对于定义的函数，可以选择性的禁止执行。

4. 执行

    执行定义的延时函数。对于执行的时机，通过软中断控制。

激活和执行是绑定在一起的，也就是说，那个CPU激活延时函数就在那个CPU上执行。但这并不是总能提高系统性能。虽然从理论上说，绑定可延时函数到激活它的CPU上更有利于利用CPU硬件Cache。毕竟，可以想象的是，正在执行的内核线程要访问的数据结构也可能是可延时函数使用的数据。但是，因为等到延时函数执行的时候，已经过了一段时间，Cache中的相关行可能已经不存在了。更重要的是，总是把一个函数绑定到某个CPU上执行是有风险的，这个CPU可能负荷很重而其它的CPU可能比较空闲。

<h3 id="4.7.1">4.7.1 软中断</h3>

Linux2.6内核中，软中断的数量比较少。对于多数目的，这些tasklet足够了。因为不需要考虑重入，所以简单易用。事实上，只使用了6类软中断，如下表所示：

表4-9 Linux2.6中使用的软中断

| 软中断 | 优先级 | 描述 |
| ------ | ------ | ----- |
| HI_SOFTIRQ    | 0 | 处理高优先级的tasklet |
| TIMER_SOFTIRQ | 1 | 定时器中断 |
| NET_TX_SOFTIRQ| 2 | 给网卡发送数据 |
| NET_RX_SOFTIRQ| 3 | 从网卡接收数据 |
| SCSI_SOFTIRQ  | 4 | SCSI命令的后中断处理 |
| TASKLET_SOFTIRQ| 5 | 处理常规tasklet |

这里的优先级就是软中断的索引，数值越小，代表优先级越高。Linux软中断处理程序总是从索引0开始执行。

<h4 id="4.7.1.1">4.7.1.1 软中断使用的数据结构</h4>

软中断的主要数据结构是`softirq_vec`数组，包含类型为`softirq_action`的32个元素。软中断的优先级表示`softirq_action`类型元素在数组中的索引。也就是说，目前只使用了数组中的前6项。`softirq_action`包含2个指针：分别指向软中断函数和函数使用的数据。

另一个重要的数据是`preempt_count`，存储在进程描述符中的`thread_info`成员中，用来追踪记录内核抢占和内核控制路径嵌套层数。它又被划分为4部分，如下表所示：

表4-10 `preempt_count`各个位域

| 位 | 描述 |
| -- | ---- |
| 0-7  | 内核抢占禁止计数（最大值255） |
| 8-15 | 软中断禁用深度计数（最大值255） |
| 16-27| 硬中断计数（最大值4096）|
| 28   | PREEMPT_ACTIVE标志     |

关于内核抢占的话题我们还会再写一篇专门的文章进行阐述，故在此不再详述。

可以使用宏`in_interrupt()`访问硬中断和软中断计数器。如果这两个计数器都是0，则返回值为0；否则返回非0值。如果内核没有使用多个内核态堆栈，该宏查找的是当前进程的`thread_info`描述符。但是，如果使用了多个内核态堆栈，则查找`irq_ctx`联合体中的`thread_info`描述符。在此情况下，内核抢占肯定是禁止的，所以该宏返回的是非0值。

最后一个跟软中断实现相关的每个CPU的32位掩码，用来描述挂起的软中断。存储在`irq_cpustat_t`数据结构的`__softirq_pending`成员中。对其具体的操作函数是`local_softirq_pending()`宏，用来是否禁止某个中断。


<h4 id="4.7.1.2">4.7.1.2 处理软中断</h4>

软中断的初始化使用`open_softirq()`函数完成，函数原型如下所示：

    void open_softirq(int nr, void (*action)(struct softirq_action *))
    {
        softirq_vec[nr].action = action;
    }

软中断的激活使用方法`raise_softirq()`，其参数是软中断索引`nr`，主要执行下面的动作：

1. 执行`local_irq_save`宏保存`eflags`寄存器中的IF标志并且禁止中断。

2. 通过设备CPU软中断位掩码的相应位将软中断标记为挂起状态。

3. 如果`in_interrupt()`返回1，直接跳转到第5步。如果处于这种情况, 要么是当前中断上下文中正在调用`raise_softirq()`、或者软中断被禁止。

4. 否则，调用`wakeup_softirqd()`唤醒`ksoftirqd`内核线程。

5. 执行`local_irq_restore`宏恢复IF标志。

应该周期性地检查挂起状态的软中断，但是不能因此增加太重的负荷。所以，软中断的执行时间点非常重要。下面是几个重要的时间点：

* 调用`local_bh_enable()`函数使能软中断的时候。

* 当`do_IRQ()`函数完成I/O中断处理，调用`irq_exit()`宏时。

* 如果系统中使用的是`I/O-APIC`控制器，当`smp_apic_timer_interrupt()`函数处理完一个定时器中断的时候。

* 在多核系统中，当CPU处理完一个由`CALL_FUNCTION_VECTOR`CPU间的中断引发的函数时。

* 当一个特殊的ksoftirqd/n内核线程被唤醒时

<h4 id="4.7.1.3">4.7.1.3 do_softirq函数</h4>

如果某个时间点，检测到挂起的软中断（`local_softirq_pending()`非零），内核调用`do_softirq()`处理它们。这个函数执行的主要内容如下：

1. 如果`in_interrupt()`等于1，则函数返回。这表明中断上下文中正在调用do_softirq()函数，或者软中断被禁止。

2. 执行`local_irq_save`来保存IF标志的状态，并在本地CPU上禁用中断。

3. 如果`thread_union`等于4KB，如果有必要，切换到软IRQ堆栈中。

4. 调用`__do_softirq()`函数。

5. If the soft IRQ stack has been effectively switched in step 3 above, it restores the original stack pointer into the esp register, thus switching back to the exception stack that was in use before.
6. 如果软IRQ堆栈

6. Executes local_irq_restore to restore the state of the IF flag (local interrupts enabled or disabled) saved in step 2 and returns.


<h4 id="4.7.1.4">4.7.1.4 __do_softirq()函数</h4>

The __do_softirq() function reads the softirq bit mask of the local CPU and executes the deferrable functions corresponding to every set bit. While executing a softirq function, new pending softirqs might pop up; in order to ensure a low latency time for the deferrable funtions, __do_softirq() keeps running until all pending softirqs have been executed. This mechanism, however, could force __do_softirq() to run for long periods of time, thus considerably delaying User Mode processes. For that reason, __do_softirq() performs a fixed number of iterations and then returns. The remaining pending softirqs, if any, will be handled in due time by the ksoftirqd kernel thread described in the next section. Here is a short description of the actions performed by the function:

1. Initializes the iteration counter to 10.

2. Copies the softirq bit mask of the local CPU (selected by local_softirq_pending()) in the pending local variable.

3. Invokes local_bh_disable() to increase the softirq counter. It is somewhat counterintuitive that deferrable functions should be disabled before starting to execute them, but it really makes a lot of sense. Because the deferrable functions mostly run with interrupts enabled, an interrupt can be raised in the middle of the __do_softirq() function. When do_IRQ() executes the irq_exit() macro, another instance of the __do_softirq() function could be started. This has to be avoided, because deferrable functions must execute serially on the CPU. Thus, the first instance of __do_softirq() disables deferrable functions, so that every new instance of the function will exit at step 1 of do_softirq().

4. Clears the softirq bitmap of the local CPU, so that new softirqs can be activated (the value of the bit mask has already been saved in the pending local variable in step 2).

5. Executes local_irq_enable() to enable local interrupts.

6. For each bit set in the pending local variable, it executes the corresponding softirq function; recall that the function address for the softirq with index n is stored in softirq_vec[n]->action.

7. Executes local_irq_disable() to disable local interrupts.

8. Copies the softirq bit mask of the local CPU into the pending local variable and decreases the iteration counter one more time.

<h4 id="4.7.1.5">4.7.1.5 ksoftirqd内核线程</h4>

<h3 id="4.7.2">4.7.2 Tasklet</h3>

<h2 id="4.8">4.8 工作队列</h2>

<h2 id="4.9">4.9 中断和异常的返回</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

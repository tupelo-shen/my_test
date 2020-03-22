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

讲解这部分之前，我们先阐述一个概念-**内核控制路径：就是一段在内核态执行的代码**。我们知道，内核态程序被激活的方式有：

1. 系统调用（异常的一种）
2. 异常
3. 中断
4. 内核线程

上面的任意一种方式，都可以让CPU执行内核态的代码。比如，I/O设备引发一个中断，相应的内核态程序的开头部分的代码应该是保存内核态堆栈中的CPU寄存器的内容；结尾部分的代码应该是再恢复这些寄存器的内容。所以，在后面的描述中，我们使用`内核控制路径`这个术语代替一段可执行的内核态代码这种表述。使用`内核控制路径`的好处就是，它是从英语直译过来的，可能会更好地表达程序代码执行的顺序性，是一个过程；这样在描述中断嵌套时更有意义。

内核控制路径可以任意嵌套；如下图所示，用户态的程序被中断打断，进入内核态响应中断；而这时候又来了其它中断，就会响应最新的中断，以此类推；但是，执行完一个中断处理程序之后，会回到之前的状态执行。这样，最后又回到了用户态。

<img id="Figure_4-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_4_3.PNG">

图4-3 内核控制路径的一个嵌套异常的示例

允许内核控制路径嵌套的代价就是中断处理程序不能阻塞，也就是说，中断处理程序运行时不能发生进程切换。恢复执行嵌套内核控制路径的所有数据都存储在内核态堆栈中，而该堆栈又和当前进程紧紧绑定在一起。通俗的说，中断处理程序相当于当前进程的资源，切换进程之前该中断资源必须释放掉。

假设内核没有bug，那么大部分的异常发生在用户态。实际上，要么是编程错误，要么是调试器故意触发的。而`页错误`异常发生在内核态，它是内核在访问物理地址时不存在引发的异常。处理这样的异常，内核挂起当前进程，切换到新进程，直到该请求页可用。因为`页错误`异常绝不会引发进一步的异常，所以，有关系的内核控制路径最多是2个（第一个是系统调用造成的，第二个是页错误造成的）。也就是说，页错误的异常最多嵌套2层。

和异常相反，I/O设备引发的中断和当前进程没有数据上的关系，尽管内核代表当前进程处理这些中断。事实上，给定某个中断，无法推断出是哪个进程在运行。

中断处理程序可以打断中断或异常处理程序执行，但是反过来，异常不能打断中断处理程序。中断处理程序绝对不能包含`页错误`的操作，因为这会诱发进程切换。

Linux嵌套执行中断或异常处理程序的两个主要原因是：

* 为了提高可编程中断控制器和设备控制器的吞吐量。内核正在处理一个中断的时候，能够及时响应另一个中断。
* 实现没有中断优先级的模型。这可以简化内核代码并提高可移植性。

在多核系统中，几个中断或异常处理程序可能会并发执行。更重要的是，异常处理程序可能由于进程切换，造成在一个CPU上启动，然后迁移到另一个CPU上执行。

<h2 id="4.4">4.4 初始化中断描述符表</h2>

至此，我们已经理解了X86架构如何在硬件层面处理中断和异常，那么接下来，我们看看内核如何初始化IDT。

同所有的设备一样，我们在使能硬件之前，必须先初始化其相关的数据结构。那么，对于中断来说，首先应该把IDT的起始地址写入idtr寄存器，并初始化所有的表项。这一步在初始化系统时完成。

汇编指令int允许用户进程发送任意编号的中断（0-255）。为此，IDT的初始化必须考虑阻止由用户进程int指令引发的非法中断和异常。可以通过将中断描述符表中的DPL域设为0来实现。如果用户进程试图发送非法中断信号，CPU控制单元比较CPL和DPL的值，发出`常规保护`的异常。

但是，有时候，用户态进程必须能够发送可编程异常。那么把相应的中断或陷阱门描述符的DPL域设为3即可。比如系统调用。

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

    直到最后时刻，IRQ中断请求线才会与设备驱动程序关联起来。比如，只有当用户访问软盘涉笔的时候才会给软盘设备分配中断请求线IRQ。使用这种方法，即使不共享IRQ中断请求线，几个硬件设备也能使用相同的中断号。

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

也就是说，内核必须在使能中断之前，知道哪个I/O设备对应哪个IRQ号。然后在设备驱动初始化的时候才能对应上正确的中断处理程序。

<h4 id="4.6.1.2">4.6.1.2 IRQ数据结构</h4>

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

The depth field and the IRQ_DISABLED flag of the irq_desc_t descriptor specify whether the IRQ line is enabled or disabled. Every time the disable_irq() or disable_irq_nosync() function is invoked, the depth field is increased; if depth is equal to 0, the function disables the IRQ line and sets its IRQ_DISABLED flag.* Conversely, each invocation of the enable_irq() function decreases the field; if depth becomes 0, the function enables the IRQ line and clears its IRQ_DISABLED flag.
`depth`和标志`IRQ_DISABLED`表明IRQ线被使能还是禁止。每次调用`disable_irq()`和`disable_irq_nosync()`函数，`depth`都会增加；如果`depth`等于0，则函数禁止IRQ线并且设置`IRQ_DISABLED`标志。相反，如果调用`enable_irq()`函数，`depth`会递减，如果`depth`等于

During system initialization, the init_IRQ() function sets the status field of each IRQ main descriptor to IRQ_DISABLED. Moreover, init_IRQ() updates the IDT by replacing the interrupt gates set up by setup_idt() (see the section “Preliminary Initialization of the IDT,” earlier in this chapter) with new ones. This is accomplished through the following statements:

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

The first field in this structure, "XT-PIC", is the PIC name. Next come the pointers to six different functions used to program the PIC. The first two functions start up and shut down an IRQ line of the chip, respectively. But in the case of the 8259A chip, these functions coincide with the third and fourth functions, which enable and disable the line. The mask_and_ack_8259A() function acknowledges the IRQ received by sending the proper bytes to the 8259A I/O ports. The end_8259A_irq() function is invoked when the interrupt handler for the IRQ line terminates. The last set_affinity method is set to NULL: it is used in multiprocessor systems to declare the “affinity” of CPUs for specified IRQs—that is, which CPUs are enabled to handle specific IRQs.

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
to ret_from_intr() (see the later section “Returning from Interrupts and
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
invokes request_irq(). This function creates a new irqaction descriptor and initializes
it with the parameter values; it then invokes the setup_irq() function to insert
the descriptor in the proper IRQ list. The device driver aborts the operation if setup_
irq() returns an error code, which usually means that the IRQ line is already in use
by another device that does not allow interrupt sharing. When the device operation
is concluded, the driver invokes the free_irq() function to remove the descriptor
from the IRQ list and release the memory area.

Let’s see how this scheme works on a simple example. Assume a program wants to
address the /dev/fd0 device file, which corresponds to the first floppy disk on the system.*
The program can do this either by directly accessing /dev/fd0 or by mounting a
filesystem on it. Floppy disk controllers are usually assigned IRQ6; given this, a
floppy driver may issue the following request:

    request_irq(6, floppy_interrupt,
            SA_INTERRUPT|SA_SAMPLE_RANDOM, "floppy", NULL);

As can be observed, the floppy_interrupt() interrupt service routine must execute
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

Here is an example of how setup_irq() is used, drawn from system initialization.
The kernel initializes the irq0 descriptor of the interval timer device by executing the
following instructions in the time_init() function (see Chapter 6):

    struct irqaction irq0 =
        {timer_interrupt, SA_INTERRUPT, 0, "timer", NULL, NULL};
    setup_irq(0, &irq0);

First, the irq0 variable of type irqaction is initialized: the handler field is set to the
address of the timer_interrupt() function, the flags field is set to SA_INTERRUPT, the
name field is set to "timer", and the fifth field is set to NULL to show that no dev_id
value is used. Next, the kernel invokes setup_irq() to insert irq0 in the list of
irqaction descriptors associated with IRQ0.



<h2 id="4.7">4.7 软中断和Tasklet</h2>

<h2 id="4.8">4.8 工作队列</h2>

<h2 id="4.9">4.9 中断和异常的返回</h2>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

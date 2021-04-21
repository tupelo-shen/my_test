[TOC]

引子：

1. 在Linux系统中，除了我们所熟知的进程状态`TASK_RUNNING`，`TASK_INTERRUPTIBLE`，`TASK_STOPPED`等，还有一个`TASK_TRACED`。这表明这个进程处于什么状态？

2. `strace`可以方便的帮助我们记录进程所执行的系统调用，它是如何跟踪到进程执行的？

3. `gdb`是我们调试程序的利器，可以设置断点，单步跟踪程序。它的实现原理又是什么？

所有这一切的背后都隐藏着Linux所提供的一个强大的系统调用`ptrace()`。

## 1 ptrace系统调用

`ptrace`系统调从名字上看是用于进程跟踪的，它提供了父进程可以观察和控制其子进程执行的能力，并允许父进程检查和替换子进程的内核镜像(包括寄存器)的值。其基本原理是: 当使用了`ptrace`跟踪后，所有发送给被跟踪的子进程的信号(除了SIGKILL)，都会被转发给父进程，而子进程则会被阻塞，这时子进程的状态就会被系统标注为`TASK_TRACED`。而父进程收到信号后，就可以对停止下来的子进程进行检查和修改，然后让子进程继续运行。

其原型为：

```c
#include <sys/ptrace.h>
long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
```

> 1. `enum __ptrace_request request`：指示了ptrace要执行的命令。
> 2. `pid_t pid`: 指示`ptrace`要跟踪的进程。
> 3. `void *addr`: 指示要监控的内存地址。
> 4. `void *data`: 存放读取出的或者要写入的数据。

`ptrace`是如此的强大，以至于有很多大家所常用的工具都基于`ptrace`来实现，如`strace`和`gdb`。接下来，我们借由对`strace`和`gdb`的实现，来看看`ptrace`是如何使用的。

## 2 strace的实现

`strace`常常被用来拦截和记录进程所执行的系统调用，以及进程所收到的信号。如有这么一段程序：

```c
#include <stdio.h>
int main(){
    printf("Hello World!/n");
    return 0;
}
```

编译后，用strace跟踪:`strace ./HelloWorld`。

可以看到形如:

```shell
execve("./HelloWorld", ["./HelloWorld"], [/* 67 vars */]) = 0
brk(0)                                  = 0x804a000
mmap2(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0xb7f18000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
open("/home/supperman/WorkSpace/lib/tls/i686/sse2/libc.so.6", O_RDONLY) = -1 ENOENT (No such file or directory)
...
```

的一段输出，这就是在执行HelloWorld中，系统所执行的系统调用，以及他们的返回值。

下面我们用ptrace来研究一下它是怎么实现的。

```c
switch(pid = fork())
{
case -1:
    return -1;
case 0: //子进程
    ptrace(PTRACE_TRACEME,0,NULL,NULL);
    execl("./HelloWorld", "HelloWorld", NULL);
default: //父进程
    wait(&val); //等待并记录execve
    if(WIFEXITED(val))
        return 0;
    syscallID=ptrace(PTRACE_PEEKUSER, pid, ORIG_EAX*4, NULL);
    printf("Process executed system call ID = %ld/n",syscallID);
    ptrace(PTRACE_SYSCALL,pid,NULL,NULL);
    while(1)
    {
        wait(&val); //等待信号
        if(WIFEXITED(val)) //判断子进程是否退出
            return 0;
        if(flag==0) //第一次(进入系统调用)，获取系统调用的参数
        {
            syscallID=ptrace(PTRACE_PEEKUSER, pid, ORIG_EAX*4, NULL);
            printf("Process executed system call ID = %ld ",syscallID);
            flag=1;
        }
        else //第二次(退出系统调用)，获取系统调用的返回值
        {
            returnValue=ptrace(PTRACE_PEEKUSER, pid, EAX*4, NULL);
            printf("with return value= %ld/n", returnValue);
            flag=0;
        }
        ptrace(PTRACE_SYSCALL,pid,NULL,NULL);
    }
}
```

在上面的程序中，fork出的子进程先调用了ptrace(PTRACE_TRACEME)表示子进程让父进程跟踪自己。然后子进程调用`execl`加载执行了HelloWorld。而在父进程中则使用wait系统调用等待子进程的状态改变。子进程因为设置了`PTRACE_TRACEME`而在执行系统调用被系统停止(设置为`TASK_TRACED`)，这时父进程被唤醒，使用`ptrace(PTRACE_PEEKUSER,pid,...)`分别去读取子进程执行的系统调用ID(放在`ORIG_EAX`中)以及系统调用返回时的值(放在`EAX`中)。然后使用`ptrace(PTRACE_SYSCALL,pid,...)`指示子进程运行到下一次执行系统调用的时候(进入或者退出)，直到子进程退出为止。

程序的执行结果如下:

```shell
Process executed system call ID = 11
Process executed system call ID = 45 with return value= 134520832
Process executed system call ID = 192 with return value= -1208934400
Process executed system call ID = 33 with return value= -2
Process executed system call ID = 5 with return value= -2
```

其中，11号系统调用就是`execve`，45号是`brk`,192是`mmap2`,33是`access`,5是`open`...经过比对可以发现，和strace的输出结果一样。当然strace进行了更详尽和完善的处理，我们这里只是揭示其原理，感兴趣的同学可以去研究一下strace的实现。

> 1. 在系统调用执行的时候，会执行`pushl %eax`保存系统调用号`ORIG_EAX`在程序用户栈中。
> 2. 在系统调用返回的时候，会执行`movl %eax,EAX(%esp)`将系统调用的返回值放入寄存器`%eax`中。
> 3. `WIFEXITED()`宏用来判断子进程是否为正常退出的，如果是，它会返回一个非零值。
> 4. 被跟踪的程序在进入或者退出某次系统调用的时候都会触发一个`SIGTRAP`信号，而被父进程捕获。
> 5. `execve()`系统调用执行成功的时候并没有返回值，因为它开始执行一段新的程序，并没有"返回"的概念。失败的时候会返回-1。
> 6. 在父进程进行进行操作的时候，用`ps`查看，可以看到子进程的状态为`T`,表示子进程处于`TASK_TRACED`状态。当然为了更具操作性，你可以在父进程中加入`sleep()`。

## 3 GDB的实现

`GDB`是GNU发布的一个强大的程序调试工具，用以调试C/C++程序。可以使程序员在程序运行的时候观察程序在内存/寄存器中的使用情况。它的实现也是基于`ptrace`系统调用来完成的。

其原理是利用`ptrace`系统调用，在被调试程序和`gdb`之间建立跟踪关系。然后所有发送给被调试程序的信号(除`SIGKILL`)都会被`gdb`截获，`gdb`根据截获的信号，查看被调试程序相应的内存地址，并控制被调试的程序继续运行。GDB常用的使用方法有断点设置和单步跟踪，接下来我们来分析一下他们是如何实现的。

#### 3.1 建立调试关系

用gdb调试程序，可以直接`gdb ./test`，也可以`gdb <pid>`(test的进程号)。这对应着使用`ptrace`建立跟踪关系的两种方式:

1. `fork`

    利用`fork+execve`执行被测试的程序，子进程在执行`execve`之前调用`ptrace(PTRACE_TRACEME)`，建立了与父进程`(debugger)`的跟踪关系。如我们在分析strace时所示意的程序。

2. `attach`

    `debugger`可以调用`ptrace(PTRACE_ATTACH，pid,...)`，建立自己与进程号为`pid`的进程间的跟踪关系。即利用`PTRACE_ATTACH`，使自己变成被调试程序的父进程(用`ps`可以看到)。用`attach`建立起来的跟踪关系，可以调用`ptrace(PTRACE_DETACH，pid,...)`来解除。注意attach进程时的权限问题，如一个非root权限的进程是不能attach到一个root进程上的。


#### 3.2 断点原理

断点是大家在调试程序时常用的一个功能，如`break linenumber`，当执行到`linenumber`那一行的时候被调试程序会停止，等待debugger的进一步操作。

断点的实现原理，就是在指定的位置插入断点指令，当被调试的程序运行到断点的时候，产生`SIGTRAP`信号。该信号被`gdb`捕获并进行断点命中判定，当`gdb`判断出这次`SIGTRAP`是断点命中之后就会转入等待用户输入进行下一步处理，否则继续。

断点的设置原理: 在程序中设置断点，就是先将该位置的原来的指令保存，然后向该位置写入`int 3`。当执行到`int 3`的时候，发生软中断，内核会给子进程发出`SIGTRAP`信号，当然这个信号会被转发给父进程。然后用保存的指令替换`int 3`,等待恢复运行。

断点命中判定: `gdb`把所有的断点位置都存放在一个链表中，命中判定即把被调试程序当前停止的位置和链表中的断点位置进行比较，看是断点产生的信号，还是无关信号。

#### 3.3 单步跟踪原理

单步跟踪就是指在调试程序的时候，让程序运行一条指令/语句后就停下。GDB中常用的命令有`next`、`step`、`nexti`、`stepi`。单步跟踪又常分为语句单步`(next, step)`和指令单步`(nexti, stepi)`。

在linux上，指令单步可以通过ptrace来实现。调用`ptrace(PTRACE_SINGLESTEP,pid,...)`可以使被调试的进程在每执行完一条指令后就触发一个`SIGTRAP`信号，让GDB运行。下面来看一个例子:

```c
child = fork();
if(child == 0)
{
    execl("./HelloWorld", "HelloWorld", NULL);
}
else 
{
    ptrace(PTRACE_ATTACH,child,NULL,NULL);
    while(1)
    {
        wait(&val);
        if(WIFEXITED(val))
            break;
        count++;
        ptrace(PTRACE_SINGLESTEP,child,NULL,NULL);
    }
    printf("Total Instruction number= %d/n",count);
}
```

这段程序比较简单，子进程调用`execve`执行`HelloWorld`，而父进程则先调用`ptrace(PTRACE_ATTACH,pid,...)`建立与子进程的跟踪关系。然后调用`ptrace(PTRACE_SINGLESTEP, pid, ...)`让子进程一步一停，以统计子进程一共执行了多少条指令(你会发现一个简单的`HelloWorld`实际上也执行了好几万条指令才完成)。当然你也完全可以在这个时候查看`EIP`寄存器中存放的指令，或者某个变量的值，当然前提是你得知道这个变量在子进程内存镜像中的位置。

指令单步可以依靠硬件完成，如x86架构处理器支持单步模式(通过设置`EFLAGS`寄存器的`TF`标志实现)，每执行一条指令，就会产生一次异常(在`Intel 80386`以上的处理器上还提供了`DRx调试寄存器`以用于软件调试)。也可以通过软件完成，即在每条指令后面都插入一条断点指令，这样每执行一条指令都会产生一次软中断。

语句单步基于指令单步实现，即GDB算好每条语句所对应的指令，从什么地方开始到什么地方结束。然后在结束的地方插入断点，或者指令单步一步一步的走到结束点，再进行处理。

当然gdb的实现远比今天我们所说的内容要复杂，它能让我们很容易的监测，修改被调试的进程，比如通过行号，函数名，变量名。而要真正实现这些，一是需要在编译的时候提供足够的信息，如在gcc时加入`-g`选项，这样gcc会把一些程序信息放到生成的ELF文件中，包括函数符号表，行号，变量信息，宏定义等，以便日后gdb调试，当然生成的文件也会大一些。二是需要我们对ELF文件格式，进程的内存镜像(布局)以及程序的指令码十分熟悉。这样才能保证在正确的时机(断点发生？单步？)找到正确的内存地址(代码？数据？)并链接回正确的程序代码(这是哪个变量？程序第几行？)。感兴趣的同学可以找到相应的代码仔细分析一下。


## 4 结论

`ptrace`可以实时监测和修改另一个进程的运行，它是如此的强大以至于曾经因为它在unix-like平台(如Linux, *BSD)上产生了各种漏洞。但换言之，只要我们能掌握它的使用，就能开发出很多以前在用户态下不可能实现的应用。当然这可能需要我们掌握编译，文件格式，程序内存布局等相当多的底层知识。

最后让我们来回顾一下`ptrace`的使用:

1. 用`PTRACE_ATTACH`或者`PTRACE_TRACEME`建立进程间的跟踪关系。
2. `PTRACE_PEEKTEXT`、`PTRACE_PEEKDATA`、`PTRACE_PEEKUSR`等读取子进程内存/寄存器中保留的值。
3. `PTRACE_POKETEXT`、`PTRACE_POKEDATA`、`PTRACE_POKEUSR`等把值写入到被跟踪进程的内存/寄存器中。
4. 用`PTRACE_CONT`、`PTRACE_SYSCALL`、`PTRACE_SINGLESTEP`控制被跟踪进程以何种方式继续运行。
5. `PTRACE_DETACH`、`PTRACE_KILL`脱离进程间的跟踪关系。


>
>   1. 进程状态TASK_TRACED用以表示当前进程因为被父进程跟踪而被系统停止。
>   2. 如在子进程结束前，父进程结束，则trace关系解除。
>   3. 利用attach建立起来的跟踪关系，虽然ps看到双方为父子关系，但在"子进程"中调用getppid()仍会返回原来的父进程id。
>   4. 不能attach到自己不能跟踪的进程，如non-root进程跟踪root进程。
>   5. 已经被trace的进程，不能再次被attach。
>   6. 即使是用PTRACE_TRACEME建立起来的跟踪关系，也可以用DETACH的方式予以解除。
>   7. 因为进入/退出系统调用都会触发一次SIGTRAP，所以通常的做法是在第一次(进入)的时候读取系统调用的参数，在第二次(退出)的时候读取系统调用的返回值。但注意execve是个例外。
>   8. 程序调试时的断点由int 3设置完成，而单步跟踪则可由ptrace(PTRACE_SINGLESTEP)实现。
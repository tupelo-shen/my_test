本节课的核心内容：

* 多线程下禁止线程切换
* 条件断点
* 使用 GDB 调试多进程程序

# 多线程下禁止线程切换

假设现在有 5 个线程，除了主线程，工作线程都是下面这样的一个函数：

    void thread_proc(void* arg)
    {
        //代码行1
        //代码行2
        //代码行3
        //代码行4
        //代码行5
        //代码行6
        //代码行7
        //代码行8
        //代码行9
        //代码行10
        //代码行11
        //代码行12
        //代码行13
        //代码行14
        //代码行15
    }

为了能说清楚这个问题，我们把四个工作线程分别叫做 A、B、C、D。

假设 GDB 当前正在处于线程 A 的代码行 3 处，此时输入 next 命令，我们期望的是调试器跳到代码行 4 处；或者使用 “ **u** *代码行10* ”，那么我们期望输入 **u** 命令后调试器可以跳转到代码行 10 处。

但是在实际情况下，GDB 可能会跳转到代码行 1 或者代码行 2 处，甚至代码行 13、代码行 14 这样的地方也是有可能的，这不是调试器 bug，这是多线程程序的特点，当我们从代码行 4 处让程序 continue 时，线程 A 虽然会继续往下执行，但是如果此时系统的线程调度将 CPU 时间片切换到线程 B、C 或者 D 呢？那么程序最终停下来的时候，处于代码行 1 或者代码行 2 或者其他地方就不奇怪了，而此时打印相关的变量值，可能就不是我们需要的线程 A 的相关值。

为了解决调试多线程程序时出现的这种问题，GDB 提供了一个在调试时将程序执行流锁定在当前调试线程的命令：**set scheduler-locking on**。当然也可以关闭这一选项，使用 **set scheduler-locking off**。除了 on/off 这两个值选项，还有一个不太常用的值叫 step，这里就不介绍了。

# 条件断点

在实际调试中，我们一般会用到三种断点：普通断点、条件断点和硬件断点。

硬件断点又叫数据断点，这样的断点其实就是前面课程中介绍的用 **watch** 命令添加的部分断点（为什么是部分而不是全部，前面介绍原因了， **watch** 添加的断点有部分是通过软中断实现的，不属于硬件断点）。硬件断点的触发时机是监视的内存地址或者变量值发生变化。

普通断点就是除去条件断点和硬件断点以外的断点。

下面重点来介绍一下条件断点，所谓条件断点，就是满足某个条件才会触发的断点，这里先举一个直观的例子：

    void do_something_func(int i)
    {
       i ++;
       i = 100 * i;
    }

    int main()
    {
       for(int i = 0; i < 10000; ++i)
       {
          do_something_func(i);
       }

       return 0;
    }

在上述代码中，假如我们希望当变量 i=5000 时，进入 do_something_func() 函数追踪一下这个函数的执行细节。此时可以修改代码增加一个 i=5000 的 if 条件，然后重新编译链接调试，这样显然比较麻烦，尤其是对于一些大型项目，每次重新编译链接都需要花一定的时间，而且调试完了还得把程序修改回来。

有了条件断点就不需要这么麻烦了，添加条件断点的命令是 **break [lineNo] if [condition]**，其中 lineNo 是程序触发断点后需要停下的位置，condition 是断点触发的条件。这里可以写成 **break 11 if i==5000**，其中，11 就是调用 do_something_fun() 函数所在的行号。当然这里的行号必须是合理行号，如果行号非法或者行号位置不合理也不会触发这个断点。

    (gdb) break 11 if i==5000       
    Breakpoint 2 at 0x400514: file test1.c, line 10.
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/testgdb/test1 

    Breakpoint 1, main () at test1.c:9
    9          for(int i = 0; i < 10000; ++i)
    (gdb) c
    Continuing.

    Breakpoint 2, main () at test1.c:11
    11            do_something_func(i);
    (gdb) p i
    $1 = 5000

把 i 打印出来，GDB 确实是在 i=5000 时停下来了。

添加条件断点还有一个方法就是先添加一个普通断点，然后使用“condition 断点编号断点触发条件”这样的方式来添加。添加一下上述断点：

    (gdb) b 11
    Breakpoint 1 at 0x400514: file test1.c, line 11.
    (gdb) info b
    Num     Type           Disp Enb Address            What
    1       breakpoint     keep y   0x0000000000400514 in main at test1.c:11
    (gdb) condition 1 i==5000
    (gdb) r
    Starting program: /root/testgdb/test1 
    y

    Breakpoint 1, main () at test1.c:11
    11            do_something_func(i);
    Missing separate debuginfos, use: debuginfo-install glibc-2.17-196.el7_4.2.x86_64
    (gdb) p i
    $1 = 5000
    (gdb) 

同样的规则，如果断点编号不存在，也无法添加成功，GDB 会提示断点不存在：

    (gdb) condition 2 i==5000
    No breakpoint number 2.

# 使用 GDB 调试多进程程序

这里说的多进程程序指的是一个进程使用 Linux 系统调用 fork() 函数产生的子进程，没有相互关联的进程就是普通的 GDB 调试，不必刻意讨论。

在实际的应用中，如有这样一类程序，如 Nginx，对于客户端的连接是采用多进程模型，当 Nginx 接受客户端连接后，创建一个新的进程来处理这一路连接上的信息来往，新产生的进程与原进程互为父子关系，那么如何用 GDB 调试这样的父子进程呢？一般有两种方法：

* 用 GDB 先调试父进程，等子进程 fork 出来后，使用 gdb attach 到子进程上去，当然这需要重新开启一个 session 窗口用于调试，gdb attach 的用法在前面已经介绍过了；
* GDB 调试器提供了一个选项叫 follow-fork，可以使用 **show follow-fork mode** 查看当前值，也可以通过 **set follow-fork mode** 来设置是当一个进程 fork 出新的子进程时，GDB 是继续调试父进程还是子进程（取值是 child），默认是父进程（ 取值是 parent）。

    (gdb) show follow-fork mode     
    Debugger response to a program call of fork or vfork is "parent".
    (gdb) set follow-fork child
    (gdb) show follow-fork mode
    Debugger response to a program call of fork or vfork is "child".
    (gdb) 

建议读者自己写个程序，然后调用 fork() 函数去实践一下，若要想阅读和调试 Apache HTTP Server 或者 Nginx 这样的程序，这个技能是必须要掌握的。


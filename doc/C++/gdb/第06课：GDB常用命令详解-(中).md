本课的核心内容：

* *info* 和 *thread* 命令
* *next*，*step*，*until*，*finish*，*return* 和 *jump* 命令

# info和thread命令

在前面使用 info break 命令查看当前断点时介绍过，info 命令是一个复合指令，还可以用来查看当前进程的所有线程运行情况。下面以 redis-server 进程为例来演示一下，使用 delete 命令删掉所有断点，然后使用 run 命令重启一下 redis-server，等程序正常启动后，我们按快捷键 *Ctrl+C* 中断程序，然后使用 info thread 命令来查看当前进程有哪些线程，分别中断在何处：

    (gdb) delete
    Delete all breakpoints? (y or n) y
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/gdbtest/redis-4.0.11/src/redis-server
    [Thread debugging using libthread_db enabled]
    ...省略部分无关内容...
    53062:M 10 Sep 17:11:10.810 * Ready to accept connections
    ^C
    Program received signal SIGINT, Interrupt.
    0x00007ffff73ee923 in epoll_wait () from /lib64/libc.so.6
    (gdb) info thread
      Id   Target Id         Frame
      4    Thread 0x7fffef7fd700 (LWP 53065) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
      3    Thread 0x7fffefffe700 (LWP 53064) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
      2    Thread 0x7ffff07ff700 (LWP 53063) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
    * 1    Thread 0x7ffff7fec780 (LWP 53062) "redis-server" 0x00007ffff73ee923 in epoll_wait () from /lib64/libc.so.6

通过 *info thread* 的输出可以知道 redis-server 正常启动后，一共产生了 *4* 个线程，包括一个主线程和三个工作线程，线程编号（Id 那一列）分别是 4、3、2、1。三个工作线程（2、3、4）分别阻塞在 *Linux API pthread_cond_wait* 处，而主线程（1）阻塞在 *epoll_wait* 处。

> 注意：虽然第一栏的名称叫 Id，但第一栏的数值不是线程的 Id，第三栏括号里的内容（如 LWP 53065）中，53065 这样的数值才是当前线程真正的 Id。那 LWP 是什么意思呢？在早期的 Linux 系统的内核里面，其实不存在真正的线程实现，当时所有的线程都是用进程来实现的，这些模拟线程的进程被称为 Light Weight Process（轻量级进程），后来 Linux 系统有了真正的线程实现，这个名字仍然被保留了下来。

读者可能会有疑问：怎么知道线程 *1* 就是主线程？线程 2、线程 3、线程 4 就是工作线程呢？是不是因为线程 1 前面有个星号（\*）？错了，线程编号前面这个星号表示的是当前 GDB 作用于哪个线程，而不是主线程的意思。现在有 4 个线程，也就有 4 个调用堆栈，如果此时输入 *backtrace* 命令查看调用堆栈，由于当前 GDB 作用在线程 1，因此 *backtrace* 命令显示的一定是线程 1 的调用堆栈：

    (gdb) bt
    #0  0x00007ffff73ee923 in epoll_wait () from /lib64/libc.so.6
    #1  0x00000000004265df in aeApiPoll (tvp=0x7fffffffe300, eventLoop=0x7ffff08350a0) at ae_epoll.c:112
    #2  aeProcessEvents (eventLoop=eventLoop@entry=0x7ffff08350a0, flags=flags@entry=11) at ae.c:411
    #3  0x0000000000426aeb in aeMain (eventLoop=0x7ffff08350a0) at ae.c:501
    #4  0x00000000004238ef in main (argc=1, argv=0x7fffffffe648) at server.c:3899

由此可见，堆栈 #4 的 main() 函数也证实了上面的说法，即线程编号为 1 的线程是主线程。

如何切换到其他线程呢？可以通过“**thread [线程编号]**”切换到具体的线程上去。例如，想切换到线程 2 上去，只要输入 thread 2 即可，然后输入 bt 就能查看这个线程的调用堆栈了：

    (gdb) info thread
      Id   Target Id         Frame
      4    Thread 0x7fffef7fd700 (LWP 53065) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
      3    Thread 0x7fffefffe700 (LWP 53064) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
      2    Thread 0x7ffff07ff700 (LWP 53063) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
    * 1    Thread 0x7ffff7fec780 (LWP 53062) "redis-server" 0x00007ffff73ee923 in epoll_wait () from /lib64/libc.so.6
    (gdb) thread 2
    [Switching to thread 2 (Thread 0x7ffff07ff700 (LWP 53063))]
    #0  0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
    (gdb) bt
    #0  0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
    #1  0x000000000047a91c in bioProcessBackgroundJobs (arg=0x0) at bio.c:176
    #2  0x00007ffff76c0e25 in start_thread () from /lib64/libpthread.so.0
    #3  0x00007ffff73ee34d in clone () from /lib64/libc.so.6

因此利用 *info thread* 命令就可以调试多线程程序，当然用 GDB 调试多线程程序还有一个很麻烦的问题，我们将在后面的 GDB 高级调试技巧中介绍。请注意，当把 GDB 当前作用的线程切换到线程 2 上之后，线程 2 前面就被加上了星号：

    (gdb) info thread
      Id   Target Id         Frame
      4    Thread 0x7fffef7fd700 (LWP 53065) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
      3    Thread 0x7fffefffe700 (LWP 53064) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
    * 2    Thread 0x7ffff07ff700 (LWP 53063) "redis-server" 0x00007ffff76c4945 in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
      1    Thread 0x7ffff7fec780 (LWP 53062) "redis-server" 0x00007ffff73ee923 in epoll_wait () from /lib64/libc.so.6

info 命令还可以用来查看当前函数的参数值，组合命令是 **info args**，我们找个函数值多一点的堆栈函数来试一下：

    (gdb) thread 1
    [Switching to thread 1 (Thread 0x7ffff7fec780 (LWP 53062))]
    #0  0x00007ffff73ee923 in epoll_wait () from /lib64/libc.so.6
    (gdb) bt
    #0  0x00007ffff73ee923 in epoll_wait () from /lib64/libc.so.6
    #1  0x00000000004265df in aeApiPoll (tvp=0x7fffffffe300, eventLoop=0x7ffff08350a0) at ae_epoll.c:112
    #2  aeProcessEvents (eventLoop=eventLoop@entry=0x7ffff08350a0, flags=flags@entry=11) at ae.c:411
    #3  0x0000000000426aeb in aeMain (eventLoop=0x7ffff08350a0) at ae.c:501
    #4  0x00000000004238ef in main (argc=1, argv=0x7fffffffe648) at server.c:3899
    (gdb) f 2
    #2  aeProcessEvents (eventLoop=eventLoop@entry=0x7ffff08350a0, flags=flags@entry=11) at ae.c:411
    411             numevents = aeApiPoll(eventLoop, tvp);
    (gdb) info args
    eventLoop = 0x7ffff08350a0
    flags = 11
    (gdb)

上述代码片段切回至主线程 1，然后切换到堆栈 #2，堆栈 #2 调用处的函数是 *aeProcessEvents()* ，一共有两个参数，使用 info args 命令可以输出当前两个函数参数的值，参数 eventLoop 是一个指针类型的参数，对于指针类型的参数，GDB 默认会输出该变量的指针地址值，如果想输出该指针指向对象的值，在变量名前面加上 * 解引用即可，这里使用 p *eventLoop 命令：

    (gdb) p *eventLoop
    $26 = {maxfd = 11, setsize = 10128, timeEventNextId = 1, lastTime = 1536570672, events = 0x7ffff0871480, fired = 0x7ffff08c2e40, timeEventHead = 0x7ffff0822080,
      stop = 0, apidata = 0x7ffff08704a0, beforesleep = 0x429590 <beforeSleep>, aftersleep = 0x4296d0 <afterSleep>}

如果还要查看其成员值，继续使用 变量名 ->字段名 即可，在前面学习 print 命令时已经介绍过了，这里不再赘述。

上面介绍的是 info 命令最常用的三种方法，更多关于 info 的组合命令在 GDB 中输入 help info 就可以查看：

    (gdb) help info
    Generic command for showing things about the program being debugged.

    List of info subcommands:

    info address -- Describe where symbol SYM is stored
    info all-registers -- List of all registers and their contents
    info args -- Argument variables of current stack frame
    info auto-load -- Print current status of auto-loaded files
    info auto-load-scripts -- Print the list of automatically loaded Python scripts
    info auxv -- Display the inferior's auxiliary vector
    info bookmarks -- Status of user-settable bookmarks
    info breakpoints -- Status of specified breakpoints (all user-settable breakpoints if no argument)
    info checkpoints -- IDs of currently known checkpoints
    info classes -- All Objective-C classes
    info common -- Print out the values contained in a Fortran COMMON block
    info copying -- Conditions for redistributing copies of GDB
    info dcache -- Print information on the dcache performance
    info display -- Expressions to display when program stops
    info extensions -- All filename extensions associated with a source language
    info files -- Names of targets and files being debugged
    info float -- Print the status of the floating point unit
    info frame -- All about selected stack frame
    info frame-filter -- List all registered Python frame-filters
    info functions -- All function names
    info handle -- What debugger does when program gets various signals
    info inferiors -- IDs of specified inferiors (all inferiors if no argument)
    info line -- Core addresses of the code for a source line
    info locals -- Local variables of current stack frame
    info macro -- Show the definition of MACRO
    info macros -- Show the definitions of all macros at LINESPEC
    info mem -- Memory region attributes
    info os -- Show OS data ARG
    info pretty-printer -- GDB command to list all registered pretty-printers
    info probes -- Show available static probes
    info proc -- Show /proc process information about any running process
    info program -- Execution status of the program
    info record -- Info record options
    info registers -- List of integer registers and their contents
    info scope -- List the variables local to a scope
    ---Type <return> to continue, or q <return> to quit---
    info selectors -- All Objective-C selectors
    info set -- Show all GDB settings
    info sharedlibrary -- Status of loaded shared object libraries
    info signals -- What debugger does when program gets various signals
    info skip -- Display the status of skips
    info source -- Information about the current source file
    info sources -- Source files in the program
    info stack -- Backtrace of the stack
    info static-tracepoint-markers -- List target static tracepoints markers
    info symbol -- Describe what symbol is at location ADDR
    info target -- Names of targets and files being debugged
    info tasks -- Provide information about all known Ada tasks
    info terminal -- Print inferior's saved terminal status
    info threads -- Display currently known threads
    info tracepoints -- Status of specified tracepoints (all tracepoints if no argument)
    info tvariables -- Status of trace state variables and their values
    info type-printers -- GDB command to list all registered type-printers
    info types -- All type names
    info variables -- All global and static variable names
    info vector -- Print the status of the vector unit
    info vtbl -- Show the virtual function table for a C++ object
    info warranty -- Various kinds of warranty you do not have
    info watchpoints -- Status of specified watchpoints (all watchpoints if no argument)
    info win -- List of all displayed windows

    Type "help info" followed by info subcommand name for full documentation.
    Type "apropos word" to search for commands related to "word".
    Command name abbreviations are allowed if unambiguous.

# next、step、until、finish、return 和 jump 命令

这几个命令是我们用 GDB 调试程序时最常用的几个控制流命令，因此放在一起介绍。next 命令（简写为 n）是让 GDB 调到下一条命令去执行，这里的下一条命令不一定是代码的下一行，而是根据程序逻辑跳转到相应的位置。举个例子：

    int a = 0;
    if (a == 9)
    {
        print("a is equal to 9.\n");
    }

    int b = 10;
    print("b = %d.\n", b);

如果当前 GDB 中断在上述代码第 2 行，此时输入 **next** 命令 GDB 将调到第 7 行，因为这里的 if 条件并不满足。

这里有一个小技巧，在 GDB 命令行界面如果直接按下 **回车键**，默认是将最近一条命令重新执行一遍，因此，当使用 **next** 命令单步调试时，不必反复输入 **n** 命令，直接回车就可以了。

    3704    int main(int argc, char **argv) {
    (gdb) n
    3736        spt_init(argc, argv);
    (gdb) n
    3738        setlocale(LC_COLLATE,"");
    (gdb) n
    3739        zmalloc_set_oom_handler(redisOutOfMemoryHandler);
    (gdb) n
    3740        srand(time(NULL)^getpid());
    (gdb) n
    3752        server.exec_argv = zmalloc(sizeof(char*)*(argc+1));
    (gdb) n
    3740        srand(time(NULL)^getpid());
    (gdb) n
    3741        gettimeofday(&tv,NULL);
    (gdb) n
    3752        server.exec_argv = zmalloc(sizeof(char*)*(argc+1));
    (gdb)

上面的执行过程等价于输入第一个 n 后直接回车：

    (gdb) n
    3736        spt_init(argc, argv);
    (gdb)
    3738        setlocale(LC_COLLATE,"");
    (gdb)
    3739        zmalloc_set_oom_handler(redisOutOfMemoryHandler);
    (gdb)
    3740        srand(time(NULL)^getpid());
    (gdb)
    3752        server.exec_argv = zmalloc(sizeof(char*)*(argc+1));
    (gdb)

**next** 命令用调试的术语叫“单步步过”（*step over*），即遇到函数调用直接跳过，不进入函数体内部。而下面的 **step** 命令（简写为 s）就是“单步步入”（*step into*），顾名思义，就是遇到函数调用，进入函数内部。举个例子，在 redis-server 的 main() 函数中有个叫 spt_init(argc, argv) 的函数调用，当我们停在这一行时，输入 **s** 将进入这个函数内部。

    //为了说明问题本身，除去不相关的干扰，代码有删减
    int main(int argc, char **argv) {
        struct timeval tv;
        int j;
        /* We need to initialize our libraries, and the server configuration. */
        spt_init(argc, argv);
        setlocale(LC_COLLATE,"");
        zmalloc_set_oom_handler(redisOutOfMemoryHandler);
        srand(time(NULL)^getpid());
        gettimeofday(&tv,NULL);
        char hashseed[16];
        getRandomHexChars(hashseed,sizeof(hashseed));
        dictSetHashFunctionSeed((uint8_t*)hashseed);
        server.sentinel_mode = checkForSentinelMode(argc,argv);
        initServerConfig();
        moduleInitModulesSystem();
        //省略部分无关代码...
    }

演示一下，先使用 **b main** 命令在 main() 处加一个断点，然后使用 **r** 命令重新跑一下程序，会触发刚才加在 main() 函数处的断点，然后使用 **n** 命令让程序走到 *spt_init(argc, argv)* 函数调用处，再输入 **s** 命令就可以进入该函数了：

    (gdb) b main
    Breakpoint 3 at 0x423450: file server.c, line 3704.
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/redis-4.0.9/src/redis-server
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".

    Breakpoint 3, main (argc=1, argv=0x7fffffffe588) at server.c:3704
    3704    int main(int argc, char **argv) {
    (gdb) n
    3736        spt_init(argc, argv);
    (gdb) s
    spt_init (argc=argc@entry=1, argv=argv@entry=0x7fffffffe588) at setproctitle.c:152
    152     void spt_init(int argc, char *argv[]) {
    (gdb) l
    147
    148             return 0;
    149     } /* spt_copyargs() */
    150
    151
    152     void spt_init(int argc, char *argv[]) {
    153             char **envp = environ;
    154             char *base, *end, *nul, *tmp;
    155             int i, error;
    156
    (gdb)

说到 step 命令，还有一个需要注意的地方，就是当函数的参数也是函数调用时，我们使用 step 命令会依次进入各个函数，那么顺序是什么呢？举个例子，看下面这段代码：

    1  int fun1(int a, int b)
    2  {
    3     int c = a + b;
    4     c += 2;
    5     return c;
    6  }
    7
    8  int func2(int p, int q)
    9  {
    10    int t = q * p;
    11       return t * t;
    12 }
    13
    14 int func3(int m, int n)
    15 {
    16    return m + n;
    17 }
    18
    19 int main()
    20 {
    21    int c;
    22    c = func3(func1(1, 2),  func2(8, 9));
    23    printf("c=%d.\n", c);
    24    return 0;
    25 }

上述代码，程序入口是 main() 函数，在第 22 行 func3 使用 func1 和 func2 的返回值作为自己的参数，在第 22 行输入 **step** 命令，会先进入哪个函数呢？这里就需要补充一个知识点了—— 函数调用方式，我们常用的函数调用方式有 _cdecl 和 _stdcall，C++ 非静态成员函数的调用方式是 _thiscall 。在这些调用方式中，函数参数的传递本质上是函数参数的入栈过程，而这三种调用方式参数的入栈顺序都是从右往左的，因此，这段代码中并没有显式标明函数的调用方式，采用默认 _cdecl 方式。

当我们在第 22 行代码处输入 **step** 先进入的是 func2() ，当从 func2() 返回时再次输入 step 命令会接着进入 func1() ，当从 func1 返回时，此时两个参数已经计算出来了，这时候会最终进入 func3() 。理解这一点，在遇到这样的代码时，才能根据需要进入我们想要的函数中去调试。

实际调试时，我们在某个函数中调试一段时间后，不需要再一步步执行到函数返回处，希望直接执行完当前函数并回到上一层调用处，就可以使用 **finish** 命令。与 **finish** 命令类似的还有 **return** 命令， **return** 命令的作用是结束执行当前函数，还可以指定该函数的返回值。

这里需要注意一下二者的区别： **finish** 命令会执行函数到正常退出该函数；而 **return** 命令是立即结束执行当前函数并返回，也就是说，如果当前函数还有剩余的代码未执行完毕，也不会执行了。我们用一个例子来验证一下：

    1  #include <stdio.h>
    2
    3  int func()
    4  {
    5     int a = 9;
    6     printf("a=%d.\n", a);
    7
    8     int b = 8;
    9     printf("b=%d.\n", b);
    10    return a + b;
    11 }
    12
    13 int main()
    14 {
    15    int c = func();
    16    printf("c=%d.\n", c);
    17
    18    return 0;
    19 }

在 main() 函数处加一个断点，然后运行程序，在第 15 行使用 **step** 命令进入 func() 函数，接着单步到代码第 8 行，直接输入 **return** 命令，这样 func() 函数剩余的代码就不会继续执行了，因此 printf("b=%d.\n", b); 这一行就没有输出。同时由于我们没有在 **return** 命令中指定这个函数的返回值，因而最终在 main() 函数中得到的变量 c 的值是一个脏数据。这也就验证了我们上面说的：**return** 命令在当前位置立即结束当前函数的执行，并返回到上一层调用。

    (gdb) b main
    Breakpoint 1 at 0x40057d: file test.c, line 15.
    (gdb) r
    Starting program: /root/testreturn/test

    Breakpoint 1, main () at test.c:15
    15          int c = func();
    Missing separate debuginfos, use: debuginfo-install glibc-2.17-196.el7_4.2.x86_64
    (gdb) s
    func () at test.c:5
    5           int a = 9;
    (gdb) n
    6           printf("a=%d.\n", a);
    (gdb) n
    a=9.
    8           int b = 8;
    (gdb) return
    Make func return now? (y or n) y
    #0  0x0000000000400587 in main () at test.c:15
    15          int c = func();
    (gdb) n
    16          printf("c=%d.\n", c);
    (gdb) n
    c=-134250496.
    18          return 0;
    (gdb)

再次用 **return** 命令指定一个值试一下，这样得到变量 c 的值应该就是我们指定的值。

    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/testreturn/test

    Breakpoint 1, main () at test.c:15
    15          int c = func();
    (gdb) s
    func () at test.c:5
    5           int a = 9;
    (gdb) n
    6           printf("a=%d.\n", a);
    (gdb) n
    a=9.
    8           int b = 8;
    (gdb) return 9999
    Make func return now? (y or n) y
    #0  0x0000000000400587 in main () at test.c:15
    15          int c = func();
    (gdb) n
    16          printf("c=%d.\n", c);
    (gdb) n
    c=9999.
    18          return 0;
    (gdb) p c
    $1 = 9999
    (gdb)

仔细观察上述代码应该会发现，用 return 命令修改了函数的返回值，当使用 print 命令打印 c 值的时候，c 值也确实被修改成了 9999。

我们再对比一下使用 **finish** 命令来结束函数执行的结果。

    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/testreturn/test

    Breakpoint 1, main () at test.c:15
    15          int c = func();
    (gdb) s
    func () at test.c:5
    5           int a = 9;
    (gdb) n
    6           printf("a=%d.\n", a);
    (gdb) n
    a=9.
    8           int b = 8;
    (gdb) finish
    Run till exit from #0  func () at test.c:8
    b=8.
    0x0000000000400587 in main () at test.c:15
    15          int c = func();
    Value returned is $3 = 17
    (gdb) n
    16          printf("c=%d.\n", c);
    (gdb) n
    c=9999.
    18          return 0;
    (gdb)

结果和我们预期的一样， **finish** 正常结束函数，剩余的代码也会被正常执行。因此 c 的值是 17。

实际调试时，还有一个 **until** 命令（简写为 **u** ）可以指定程序运行到某一行停下来，还是以 redis-server 的代码为例：

    1812    void initServer(void) {
    1813        int j;
    1814
    1815        signal(SIGHUP, SIG_IGN);
    1816        signal(SIGPIPE, SIG_IGN);
    1817        setupSignalHandlers();
    1818
    1819        if (server.syslog_enabled) {
    1820            openlog(server.syslog_ident, LOG_PID | LOG_NDELAY | LOG_NOWAIT,
    1821                server.syslog_facility);
    1822        }
    1823
    1824        server.pid = getpid();
    1825        server.current_client = NULL;
    1826        server.clients = listCreate();
    1827        server.clients_to_close = listCreate();
    1828        server.slaves = listCreate();
    1829        server.monitors = listCreate();
    1830        server.clients_pending_write = listCreate();
    1831        server.slaveseldb = -1; /* Force to emit the first SELECT command. */
    1832        server.unblocked_clients = listCreate();
    1833        server.ready_keys = listCreate();
    1834        server.clients_waiting_acks = listCreate();
    1835        server.get_ack_from_slaves = 0;
    1836        server.clients_paused = 0;
    1837        server.system_memory_size = zmalloc_get_memory_size();
    1838
    1839        createSharedObjects();
    1840        adjustOpenFilesLimit();
    1841        server.el = aeCreateEventLoop(server.maxclients+CONFIG_FDSET_INCR);
    1842        if (server.el == NULL) {
    1843            serverLog(LL_WARNING,
    1844                "Failed creating the event loop. Error message: '%s'",
    1845                strerror(errno));
    1846            exit(1);
    1847        }

这是 redis-server 代码中 initServer() 函数的一个代码片段，位于文件 server.c 中，当停在第 1813 行，想直接跳到第 1839 行，可以直接输入 **u 1839**，这样就能快速执行完中间的代码。当然，也可以先在第 1839 行加一个断点，然后使用 **continue** 命令运行到这一行，但是使用 **until** 命令会更简便。

    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/redis-4.0.9/src/redis-server
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".

    Breakpoint 3, main (argc=1, argv=0x7fffffffe588) at server.c:3704
    3704    int main(int argc, char **argv) {
    (gdb) c
    Continuing.
    21574:C 14 Sep 06:42:36.978 # oO0OoO0OoO0Oo Redis is starting oO0OoO0OoO0Oo
    21574:C 14 Sep 06:42:36.978 # Redis version=4.0.9, bits=64, commit=00000000, modified=0, pid=21574, just started
    21574:C 14 Sep 06:42:36.979 # Warning: no config file specified, using the default config. In order to specify a config file use /root/redis-4.0.9/src/redis-server /path/to/redis.conf

    Breakpoint 4, initServer () at server.c:1812
    1812    void initServer(void) {
    (gdb) n
    1815        signal(SIGHUP, SIG_IGN);
    (gdb) u 1839
    initServer () at server.c:1839
    1839        createSharedObjects();
    (gdb)

**jump** 命令基本用法是：

    jump <location>

**location** 可以是程序的行号或者函数的地址， **jump** 会让程序执行流跳转到指定位置执行，当然其行为也是不可控制的，例如您跳过了某个对象的初始化代码，直接执行操作该对象的代码，那么可能会导致程序崩溃或其他意外行为。 **jump** 命令可以简写成 **j**，但是不可以简写成 **jmp**，其使用有一个注意事项，即如果 **jump** 跳转到的位置后续没有断点，那么 GDB 会执行完跳转处的代码会继续执行。举个例子：

    1 int somefunc()
    2 {
    3   //代码A
    4   //代码B
    5   //代码C
    6   //代码D
    7   //代码E
    8   //代码F
    9 }

假设我们的断点初始位置在行号 3 处（代码 A），这个时候我们使用 **jump 6**，那么程序会跳过代码 B 和 C 的执行，执行完代码 D（ 跳转点），程序并不会停在代码 6 处，而是继续执行后续代码，因此如果我们想查看执行跳转处的代码后的结果，需要在行号 6、7 或 8 处设置断点。

**jump** 命令除了跳过一些代码的执行外，还有一个妙用就是可以执行一些我们想要执行的代码，而这些代码在正常的逻辑下可能并不会执行（当然可能也因此会产生一些意外的结果，这需要读者自行斟酌使用）。举个例子，假设现在有如下代码：

    1  #include <stdio.h>
    2  int main()
    3  {
    4    int a = 0;
    5    if (a != 0)
    6    {
    7      printf("if condition\n");
    8    }
    9    else
    10   {
    11     printf("else condition\n");
    12   }
    13
    14   return 0;
    15 }

我们在行号 4 、14 处设置一个断点，当触发行号 4 处的断点后，正常情况下程序执行流会走 else 分支，我们可以使用 jump 7 强行让程序执行 if 分支，接着 GDB 会因触发行号 14 处的断点而停下来，此时我们接着执行 jump 11，程序会将 else 分支中的代码重新执行一遍。整个操作过程如下：

    [root@localhost testcore]# gdb test
    Reading symbols from /root/testcore/test...done.
    (gdb) b main
    Breakpoint 1 at 0x400545: file main.cpp, line 4.
    (gdb) b 14
    Breakpoint 2 at 0x400568: file main.cpp, line 14.
    (gdb) r
    Starting program: /root/testcore/test

    Breakpoint 1, main () at main.cpp:4
    4       int a = 0;
    Missing separate debuginfos, use: debuginfo-install glibc-2.17-260.el7.x86_64 libgcc-4.8.5-36.el7.x86_64 libstdc++-4.8.5-36.el7.x86_64
    (gdb) jump 7
    Continuing at 0x400552.
    if condition

    Breakpoint 2, main () at main.cpp:14
    14       return 0;
    (gdb) jump 11
    Continuing at 0x40055e.
    else condition

    Breakpoint 2, main () at main.cpp:14
    14       return 0;
    (gdb) c
    Continuing.
    [Inferior 1 (process 13349) exited normally]
    (gdb)

redis-server 在入口函数 main 处调用 initServer() ，我们使用 “**b initServer**” 、“**b 2025**”、“**b 2027**”在这个函数入口处、2025 行、2027 行增加三个断点，然后使用 **run** 命令重新运行一下程序，触发第一个断点后，继续输入 **c** 命令继续运行，然后触发 2025 行处的断点，接着输入 **jump 2027** ：

    (gdb) b 2025
    Breakpoint 5 at 0x42c8e7: file server.c, line 2025.
    (gdb) b 2027
    Breakpoint 6 at 0x42c8f8: file server.c, line 2027.
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) n
    Program not restarted.
    (gdb) b initServer
    Note: breakpoint 3 also set at pc 0x42c8b0.
    Breakpoint 7 at 0x42c8b0: file server.c, line 2013.
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/redis-5.0.3/src/redis-server
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".

    Breakpoint 1, main (argc=1, argv=0x7fffffffe4e8) at server.c:4003
    4003    int main(int argc, char **argv) {
    (gdb) c
    Continuing.
    13374:C 14 Jan 2019 15:12:16.571 # oO0OoO0OoO0Oo Redis is starting oO0OoO0OoO0Oo
    13374:C 14 Jan 2019 15:12:16.571 # Redis version=5.0.3, bits=64, commit=00000000, modified=0, pid=13374, just started
    13374:C 14 Jan 2019 15:12:16.571 # Warning: no config file specified, using the default config. In order to specify a config file use /root/redis-5.0.3/src/redis-server /path/to/redis.conf

    Breakpoint 3, initServer () at server.c:2013
    2013    void initServer(void) {
    (gdb) c
    Continuing.

    Breakpoint 5, initServer () at server.c:2025
    2025        server.hz = server.config_hz;
    (gdb) jump 2027
    Continuing at 0x42c8f8.

    Breakpoint 6, initServer () at server.c:2027
    2027        server.current_client = NULL;
    (gdb)

程序将 2026 行的代码跳过了，2026 行处的代码是获取当前进程 id：

    2026 server.pid = getpid();

由于这一行被跳过了，所以 server.pid 的值应该是一个无效的值，我们可以使用 **print** 命令将这个值打印出来看一下：

    (gdb) p server.pid
    $3 = 0

结果确实是 0 这个我们初始化的无效值。

> 本质上，jump 命令的作用类似于在 Visual Studio 中调试时，拖鼠标将程序从一个执行处拖到另外一个执行处。

# 小结

本节课介绍了 **info thread**、**next**、**step**、**until**、**finish**、 **return** 和 **jump** 命令，这些也是 GDB 调试过程中非常常用的命令，请读者务必掌握。
本课的核心内容：

* disassemble 命令
* set args 和 show args 命令
* tbreak 命令
* watch 命令
* display 命令

# disassemble命令

当进行一些高级调试时，我们可能需要查看某段代码的汇编指令去排查问题，或者是在调试一些没有调试信息的发布版程序时，也只能通过反汇编代码去定位问题，那么 **disassemble** 命令就派上用场了。

    initServer () at server.c:1839
    1839        createSharedObjects();
    (gdb) disassemble
    Dump of assembler code for function initServer:
       0x000000000042f450 <+0>:     push   %r12
       0x000000000042f452 <+2>:     mov    $0x1,%esi
       0x000000000042f457 <+7>:     mov    $0x1,%edi
       0x000000000042f45c <+12>:    push   %rbp
       0x000000000042f45d <+13>:    push   %rbx
       0x000000000042f45e <+14>:    callq  0x421eb0 <signal@plt>
       0x000000000042f463 <+19>:    mov    $0x1,%esi
       0x000000000042f468 <+24>:    mov    $0xd,%edi
       0x000000000042f46d <+29>:    callq  0x421eb0 <signal@plt>
       0x000000000042f472 <+34>:    callq  0x42f3a0 <setupSignalHandlers>
       0x000000000042f477 <+39>:    mov    0x316d52(%rip),%r8d        # 0x7461d0 <server+2128>
       0x000000000042f47e <+46>:    test   %r8d,%r8d
       0x000000000042f481 <+49>:    jne    0x42f928 <initServer+1240>
       0x000000000042f487 <+55>:    callq  0x421a50 <getpid@plt>
       0x000000000042f48c <+60>:    movq   $0x0,0x316701(%rip)        # 0x745b98 <server+536>
       0x000000000042f497 <+71>:    mov    %eax,0x3164e3(%rip)        # 0x745980 <server>
       0x000000000042f49d <+77>:    callq  0x423cb0 <listCreate>
       0x000000000042f4a2 <+82>:    mov    %rax,0x3166c7(%rip)        # 0x745b70 <server+496>
       0x000000000042f4a9 <+89>:    callq  0x423cb0 <listCreate>
       0x000000000042f4ae <+94>:    mov    %rax,0x3166c3(%rip)        # 0x745b78 <server+504>
       0x000000000042f4b5 <+101>:   callq  0x423cb0 <listCreate>
       0x000000000042f4ba <+106>:   mov    %rax,0x3166c7(%rip)        # 0x745b88 <server+520>
       0x000000000042f4c1 <+113>:   callq  0x423cb0 <listCreate>
       0x000000000042f4c6 <+118>:   mov    %rax,0x3166c3(%rip)        # 0x745b90 <server+528>
       0x000000000042f4cd <+125>:   callq  0x423cb0 <listCreate>
       0x000000000042f4d2 <+130>:   movl   $0xffffffff,0x316d6c(%rip)        # 0x746248 <server+2248>
       0x000000000042f4dc <+140>:   mov    %rax,0x31669d(%rip)        # 0x745b80 <server+512>
       0x000000000042f4e3 <+147>:   callq  0x423cb0 <listCreate>
       0x000000000042f4e8 <+152>:   mov    %rax,0x316ec9(%rip)        # 0x7463b8 <server+2616>
       0x000000000042f4ef <+159>:   callq  0x423cb0 <listCreate>
       0x000000000042f4f4 <+164>:   mov    %rax,0x316ec5(%rip)        # 0x7463c0 <server+2624>
       0x000000000042f4fb <+171>:   callq  0x423cb0 <listCreate>
       0x000000000042f500 <+176>:   movl   $0x0,0x316e7e(%rip)        # 0x746388 <server+2568>
       0x000000000042f50a <+186>:   mov    %rax,0x316e6f(%rip)        # 0x746380 <server+2560>
       0x000000000042f511 <+193>:   movl   $0x0,0x316685(%rip)        # 0x745ba0 <server+544>
       0x000000000042f51b <+203>:   callq  0x432e90 <zmalloc_get_memory_size>
       0x000000000042f520 <+208>:   mov    %rax,0x316fd9(%rip)        # 0x746500 <server+2944>
    => 0x000000000042f527 <+215>:   callq  0x42a7b0 <createSharedObjects>

GDB 默认反汇编为 AT&T 格式的指令，可以通过 **show disassembly-flavor** 查看，如果习惯 intel 汇编格式可以用命令 **set disassembly-flavor intel** 来设置。

    (gdb) set disassembly-flavor intel
    (gdb) disassemble
    Dump of assembler code for function initServer:
       0x000000000042f450 <+0>:     push   r12
       0x000000000042f452 <+2>:     mov    esi,0x1
       0x000000000042f457 <+7>:     mov    edi,0x1
       0x000000000042f45c <+12>:    push   rbp
       0x000000000042f45d <+13>:    push   rbx
       0x000000000042f45e <+14>:    call   0x421eb0 <signal@plt>
       0x000000000042f463 <+19>:    mov    esi,0x1
       0x000000000042f468 <+24>:    mov    edi,0xd
       0x000000000042f46d <+29>:    call   0x421eb0 <signal@plt>
       0x000000000042f472 <+34>:    call   0x42f3a0 <setupSignalHandlers>
       0x000000000042f477 <+39>:    mov    r8d,DWORD PTR [rip+0x316d52]        # 0x7461d0 <server+2128>
       0x000000000042f47e <+46>:    test   r8d,r8d
       0x000000000042f481 <+49>:    jne    0x42f928 <initServer+1240>
       0x000000000042f487 <+55>:    call   0x421a50 <getpid@plt>
       0x000000000042f48c <+60>:    mov    QWORD PTR [rip+0x316701],0x0        # 0x745b98 <server+536>
       0x000000000042f497 <+71>:    mov    DWORD PTR [rip+0x3164e3],eax        # 0x745980 <server>
       0x000000000042f49d <+77>:    call   0x423cb0 <listCreate>
       0x000000000042f4a2 <+82>:    mov    QWORD PTR [rip+0x3166c7],rax        # 0x745b70 <server+496>
       0x000000000042f4a9 <+89>:    call   0x423cb0 <listCreate>
       0x000000000042f4ae <+94>:    mov    QWORD PTR [rip+0x3166c3],rax        # 0x745b78 <server+504>
       0x000000000042f4b5 <+101>:   call   0x423cb0 <listCreate>
       0x000000000042f4ba <+106>:   mov    QWORD PTR [rip+0x3166c7],rax        # 0x745b88 <server+520>
       0x000000000042f4c1 <+113>:   call   0x423cb0 <listCreate>
       0x000000000042f4c6 <+118>:   mov    QWORD PTR [rip+0x3166c3],rax        # 0x745b90 <server+528>
       0x000000000042f4cd <+125>:   call   0x423cb0 <listCreate>
       0x000000000042f4d2 <+130>:   mov    DWORD PTR [rip+0x316d6c],0xffffffff        # 0x746248 <server+2248>
       0x000000000042f4dc <+140>:   mov    QWORD PTR [rip+0x31669d],rax        # 0x745b80 <server+512>
       0x000000000042f4e3 <+147>:   call   0x423cb0 <listCreate>
       0x000000000042f4e8 <+152>:   mov    QWORD PTR [rip+0x316ec9],rax        # 0x7463b8 <server+2616>
       0x000000000042f4ef <+159>:   call   0x423cb0 <listCreate>
       0x000000000042f4f4 <+164>:   mov    QWORD PTR [rip+0x316ec5],rax        # 0x7463c0 <server+2624>
       0x000000000042f4fb <+171>:   call   0x423cb0 <listCreate>
       0x000000000042f500 <+176>:   mov    DWORD PTR [rip+0x316e7e],0x0        # 0x746388 <server+2568>
       0x000000000042f50a <+186>:   mov    QWORD PTR [rip+0x316e6f],rax        # 0x746380 <server+2560>
       0x000000000042f511 <+193>:   mov    DWORD PTR [rip+0x316685],0x0        # 0x745ba0 <server+544>
       0x000000000042f51b <+203>:   call   0x432e90 <zmalloc_get_memory_size>
       0x000000000042f520 <+208>:   mov    QWORD PTR [rip+0x316fd9],rax        # 0x746500 <server+2944>
    => 0x000000000042f527 <+215>:   call   0x42a7b0 <createSharedObjects>

# set args 和 show args 命令

很多程序需要我们传递命令行参数。在 GDB 调试中，很多人会觉得可以使用 **gdb filename args** 这种形式来给 GDB 调试的程序传递命令行参数，这样是不行的。正确的做法是在用 GDB 附加程序后，在使用 **run** 命令之前，使用“set args 参数内容”来设置命令行参数。

还是以 redis-server 为例，Redis 启动时可以指定一个命令行参数，它的默认配置文件位于 redis-server 这个文件的上一层目录，因此我们可以在 GDB 中这样传递这个参数：**set args ../redis.conf**（即文件 redis.conf 位于当前程序 redis-server 的上一层目录），可以通过 **show args** 查看命令行参数是否设置成功。

    (gdb) set args ../redis.conf
    (gdb) show args
    Argument list to give program being debugged when it is started is "../redis.conf ".
    (gdb)

如果单个命令行参数之间含有空格，可以使用引号将参数包裹起来。

    (gdb) set args "999 xx" "hu jj"
    (gdb) show args
    Argument list to give program being debugged when it is started is ""999 xx" "hu jj"".
    (gdb)

如果想清除掉已经设置好的命令行参数，使用 **set args** 不加任何参数即可。

    (gdb) set args
    (gdb) show args
    Argument list to give program being debugged when it is started is "".
    (gdb)

# tbreak 命令

**tbreak** 命令也是添加一个断点，第一个字母“t”的意思是 temporarily（临时的），也就是说这个命令加的断点是临时的，所谓临时断点，就是一旦该断点触发一次后就会自动删除。添加断点的方法与上面介绍的 **break** 命令一模一样，这里不再赘述。

    (gdb) tbreak main
    Temporary breakpoint 1 at 0x423450: file server.c, line 3704.
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/redis-4.0.9/src/redis-server
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".

    Temporary breakpoint 1, main (argc=1, argv=0x7fffffffe588) at server.c:3704
    3704    int main(int argc, char **argv) {
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/redis-4.0.9/src/redis-server
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".
    21652:C 14 Sep 07:05:39.288 # oO0OoO0OoO0Oo Redis is starting oO0OoO0OoO0Oo
    21652:C 14 Sep 07:05:39.288 # Redis version=4.0.9, bits=64, commit=00000000, modified=0, pid=21652, just started
    21652:C 14 Sep 07:05:39.288 # Warning: no config file specified, using the default config. In order to specify a config file use /root/redis-4.0.9/src/redis-server /path/to/redis.conf
    21652:M 14 Sep 07:05:39.289 * Increased maximum number of open files to 10032 (it was originally set to 1024).
    [New Thread 0x7ffff07ff700 (LWP 21653)]
    [New Thread 0x7fffefffe700 (LWP 21654)]
    [New Thread 0x7fffef7fd700 (LWP 21655)]
                    _._
               _.-``__ ''-._
          _.-``    `.  `_.  ''-._           Redis 4.0.9 (00000000/0) 64 bit
      .-`` .-```.  ```\/    _.,_ ''-._
     (    '      ,       .-`  | `,    )     Running in standalone mode
     |`-._`-...-` __...-.``-._|'` _.-'|     Port: 6379
     |    `-._   `._    /     _.-'    |     PID: 21652
      `-._    `-._  `-./  _.-'    _.-'
     |`-._`-._    `-.__.-'    _.-'_.-'|
     |    `-._`-._        _.-'_.-'    |           http://redis.io
      `-._    `-._`-.__.-'_.-'    _.-'
     |`-._`-._    `-.__.-'    _.-'_.-'|
     |    `-._`-._        _.-'_.-'    |

上述代码，我们使用 **tbreak** 命令在 *main()* 函数处添加了一个断点，当断点触发后，再次运行程序不再触发断点，因为这个临时断点已经被删除。

# watch 命令

**watch** 命令是一个强大的命令，它可以用来监视一个变量或者一段内存，当这个变量或者该内存处的值发生变化时，GDB 就会中断下来。被监视的某个变量或者某个内存地址会产生一个 watch point（观察点）。

我在数年前去北京中关村软件园应聘一个 C++ 开发的职位，当时一个面试官问了这样一个问题：有一个变量其值被意外地改掉了，通过单步调试或者挨个检查使用该变量的代码工作量会非常大，如何快速地定位到该变量在哪里被修改了？其实，面试官想要的答案是“硬件断点”。具体什么是硬件断点，我将在后面高级调试课程中介绍，而 **watch** 命令就可以通过添加硬件断点来达到监视数据变化的目的。 **watch** 命令的使用方式是“**watch** *变量名或内存地址*”，一般有以下几种形式：

* 形式1：
    
        int i;
        watch i 

* 形式2:
    
        char *p;
        watch p 与 watch *p 

> watch p 与 watch *p 是有区别的，前者是查看 *(&p)，是 p 变量本身；后者是 p 所指内存的内容。我们需要查看地址，因为目的是要看某内存地址上的数据是怎样变化的。

* 形式3：
    
        char buf[128];
        watch buf 

这里是对 buf 的 128 个数据进行了监视，此时不是采用硬件断点，而是用软中断实现的。用软中断方式去检查内存变量是比较耗费 CPU 资源的，精确地指明地址是硬件中断。

> 注意：当设置的观察点是一个局部变量时，局部变量无效后，观察点也会失效。在观察点失效时 GDB 可能会提示如下信息：
> Watchpoint 2 deleted because the program has left the block in which its expression is valid.

# display 命令

**display** 命令监视的变量或者内存地址，每次程序中断下来都会自动输出这些变量或内存的值。例如，假设程序有一些全局变量，每次断点停下来我都希望 GDB 可以自动输出这些变量的最新值，那么使用“ **display** *变量名*”设置即可。

    Program received signal SIGINT, Interrupt.
    0x00007ffff71e2483 in epoll_wait () from /lib64/libc.so.6
    (gdb) display $ebx
    1: $ebx = 7988560
    (gdb) display /x $ebx
    2: /x $ebx = 0x79e550
    (gdb) display $eax
    3: $eax = -4
    (gdb) b main
    Breakpoint 8 at 0x4201f0: file server.c, line 4003.
    (gdb) r
    The program being debugged has been started already.
    Start it from the beginning? (y or n) y
    Starting program: /root/redis-5.0.3/src/redis-server
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".

    Breakpoint 8, main (argc=1, argv=0x7fffffffe4e8) at server.c:4003
    4003    int main(int argc, char **argv) {
    3: $eax = 4325872
    2: /x $ebx = 0x0
    1: $ebx = 0
    (gdb)

上述代码中，我使用 **display** 命令分别添加了寄存器 *ebp* 和寄存器 *eax*， *ebp* 寄存器分别使用十进制和十六进制两种形式输出其值，这样每次程序中断下来都会自动把这些值打印出来，可以使用 **info display** 查看当前已经自动添加了哪些值，使用 **delete display** 清除全部需要自动输出的变量，使用 **delete diaplay** *编号* 删除某个自动输出的变量。

    (gdb) delete display
    Delete all auto-display expressions? (y or n) n
    (gdb) delete display 3
    (gdb) info display
    Auto-display expressions now in effect:
    Num Enb Expression
    2:   y  $ebp
    1:   y  $eax


（文章大部分转载于：[https://consen.github.io/2018/01/17/debug-linux-kernel-with-qemu-and-gdb/](https://consen.github.io/2018/01/17/debug-linux-kernel-with-qemu-and-gdb/)）

排查Linux内核Bug，研究内核机制，除了查看资料阅读源码，还可通过调试器，动态分析内核执行流程。

QEMU模拟器原生支持GDB调试器，这样可以很方便地使用GDB的强大功能对操作系统进行调试，如设置断点；单步执行；查看调用栈、查看寄存器、查看内存、查看变量；修改变量改变执行流程等。

### 编译调试版内核

对内核进行调试需要解析符号信息，所以得编译一个调试版内核。

    $ cd linux-4.14
    $ make menuconfig
    $ make -j 20

这里需要开启内核参数`CONFIG_DEBUG_INFO`和`CONFIG_GDB_SCRIPTS`。GDB提供了Python接口来扩展功能，内核基于Python接口实现了一系列辅助脚本，简化内核调试，开启`CONFIG_GDB_SCRIPTS`参数就可以使用了。

    Kernel hacking  ---> 
        [*] Kernel debugging
        Compile-time checks and compiler options  --->
            [*] Compile the kernel with debug info
            [*]   Provide GDB scripts for kernel debugging

### 构建initramfs根文件系统

Linux系统启动阶段，boot loader加载完内核文件vmlinuz后，内核紧接着需要挂载磁盘根文件系统，但如果此时内核没有相应驱动，无法识别磁盘，就需要先加载驱动，而驱动又位于`/lib/modules`，得挂载根文件系统才能读取，这就陷入了一个两难境地，系统无法顺利启动。于是有了initramfs根文件系统，其中包含必要的设备驱动和工具，boot loader加载initramfs到内存中，内核会将其挂载到根目录`/`,然后运行`/init`脚本，挂载真正的磁盘根文件系统。

这里借助BusyBox构建极简initramfs，提供基本的用户态可执行程序。

编译BusyBox，配置`CONFIG_STATIC`参数，编译静态版BusyBox，编译好的可执行文件busybox不依赖动态链接库，可以独立运行，方便构建initramfs。

    $ cd busybox-1.28.0
    $ make menuconfig

选择配置项：

    Settings  --->
        [*] Build static binary (no shared libs)

执行编译、安装：

    $ make -j 20
    $ make install

会安装在_install目录:

    $ ls _install 
    bin  linuxrc  sbin  usr

创建initramfs，其中包含BusyBox可执行程序、必要的设备文件、启动脚本`init`。这里没有内核模块，如果需要调试内核模块，可将需要的内核模块包含进来。`init`脚本只挂载了虚拟文件系统`procfs`和`sysfs`，没有挂载磁盘根文件系统，所有调试操作都在内存中进行，不会落磁盘。

    $ mkdir initramfs
    $ cd initramfs
    $ cp ../_install/* -rf ./
    $ mkdir dev proc sys
    $ sudo cp -a /dev/{null,console,tty,tty1,tty2,tty3,tty4} dev/
    $ rm linuxrc
    $ vim init
    $ chmod a+x init
    $ ls
    $ bin   dev  init  proc  sbin  sys   usr

init文件内容：

    #!/bin/busybox sh         
    mount -t proc none /proc  
    mount -t sysfs none /sys  

    exec /sbin/init

打包initramfs:

    $ find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz

### 调试

    $ cd busybox-1.28.0
    $ qemu-system-i386 -s -kernel ./linux-4.4.203/arch/i386/boot/bzImage -initrd ./initramfs.cpio.gz -nographic -append "console=ttyS0"

* `-s`是`-gdb tcp::1234`缩写，监听1234端口，在GDB中可以通过`target remote localhost:1234`连接；
* `-kernel`指定编译好的调试版内核；
* `-initrd`指定制作的initramfs;
* `-nographic`取消图形输出窗口，使QEMU成简单的命令行程序；
* `-append "console=ttyS0"`将输出重定向到console，将会显示在标准输出stdio。

启动后的根目录, 就是initramfs中包含的内容：

    / # ls                    
    bin   dev  init  proc  root  sbin  sys   usr

<s>由于系统自带的GDB版本为7.2，内核辅助脚本无法使用，重新编译了一个新版GDB。</s><font color="blue">我的系统比较新，所以gdb版本是7.11，所以不需要重新编译。</font>

    $ cd gdb-7.9.1
    $ ./configure --with-python=$(which python2.7)
    $ make -j 20
    $ sudo make install

启动GDB:

    $ cd linux-4.14
    $ /usr/local/bin/gdb vmlinux
    (gdb) target remote localhost:1234

使用内核提供的GDB辅助调试功能：

    (gdb) apropos lx                                    
    function lx_current -- Return current task          
    function lx_module -- Find module by name and return the module variable                                 
    function lx_per_cpu -- Return per-cpu variable      
    function lx_task_by_pid -- Find Linux task by PID and return the task_struct variable                    
    ...(此处省略若干行)                      
    lx-symbols -- (Re-)load symbols of Linux kernel and currently loaded modules                             
    lx-version --  Report the Linux Version of the current kernel
    (gdb) lx-cmdline 
    console=ttyS0

在函数`cmdline_proc_show`设置断点，虚拟机中运行`cat /proc/cmdline`命令即会触发。

    (gdb) b cmdline_proc_show                           
    Breakpoint 1 at 0xffffffff81298d99: file fs/proc/cmdline.c, line 9.                                      
    (gdb) c                                             
    Continuing.                                         

    Breakpoint 1, cmdline_proc_show (m=0xffff880006695000, v=0x1 <irq_stack_union+1>) at fs/proc/cmdline.c:9
    9               seq_printf(m, "%s\n", saved_command_line);                                              
    (gdb) bt
    #0  cmdline_proc_show (m=0xffff880006695000, v=0x1 <irq_stack_union+1>) at fs/proc/cmdline.c:9
    #1  0xffffffff81247439 in seq_read (file=0xffff880006058b00, buf=<optimized out>, size=<optimized out>, ppos=<optimized out>) at fs/seq_file.c:234
    ......（此处省略）
    (gdb) p saved_command_line
    $2 = 0xffff880007e68980 "console=ttyS0"

### 获取当前进程

《深入理解Linux内核》第三版第三章–进程，讲到内核采用了一种精妙的设计来获取当前进程。

Linux把跟一个进程相关的`thread_info`和内核栈stack放在了同一内存区域，内核通过esp寄存器获得当前CPU上运行进程的内核栈栈底地址，该地址正好是`thread_info`地址，由于进程描述符指针task字段在`thread_info`结构体中偏移量为0，进而获得task。相关汇编指令如下：

    movl $0xffffe000, %ecx      /* 内核栈大小为8K，屏蔽低13位有效位。
    andl $esp, %ecx
    movl (%ecx), p

指令运行后，p就获得当前CPU上运行进程的描述符指针。

然而在调试器中调了下，发现这种机制早已经被废弃掉了。`thread_info`结构体中只剩下一个字段flags，进程描述符字段task已经删除，无法通过`thread_info`获取进程描述符了。

而且进程的`thread_info`也不再位于进程内核栈底了，而是放在了进程描述符task_struct结构体中，见提交[sched/core: Allow putting thread_info into task_struct](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=15f4eae70d365bba26854c90b6002aaabb18c8aa)和[x86: Move thread_info into task_struct](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=15f4eae70d365bba26854c90b6002aaabb18c8aa)，这样也无法通过esp寄存器获取`thread_info`地址了。

    (gdb) p $lx_current().thread_info
    $5 = {flags = 2147483648}

> <font color="blue">thread_info这个变量好像没有了，打印结果显示没有这个成员</font>

这样做是从安全角度考虑的，一方面可以防止esp寄存器泄露后进而泄露进程描述符指针，二是防止内核栈溢出覆盖`thread_info`。

Linux内核从2.6引入了Per-CPU变量，获取当前指针也是通过Per-CPU变量实现的。

    (gdb) p $lx_current().pid
    $50 = 77
    (gdb) p $lx_per_cpu("current_task").pid
    $52 = 77

### 补充

在gdb中输入命令`apropos lx`，没有任何输出，说明无法调用python辅助函数。

    (gdb) apropos lx

从stackoverflow网站上找到一篇文章[gdb-lx-symbols-undefined-command](https://stackoverflow.com/questions/29104491/gdb-lx-symbols-undefined-command)，里边提到：

    gdb -ex add-auto-load-safe-path /path/to/linux/kernel/source/root

    Now the GDB scripts are automatically loaded, and lx-symbols is available.

但是，按照上面进行操作后，进入gdb调试画面后，提示：

    To enable execution of this file add
        add-auto-load-safe-path /home/qemu2/qemu/linux-4.4.203/scripts/gdb/vmlinux-gdb.py
    line to your configuration file "/home/qemu2/.gdbinit".
    To completely disable this security protection add
        set auto-load safe-path /
    line to your configuration file "/home/qemu2/.gdbinit".

上面的意思是，为了能够使能`vmlinux-gdb.py`的执行，需要添加

    add-auto-load-safe-path /home/qemu2/qemu/linux-4.4.203/scripts/gdb/vmlinux-gdb.py

这行代码到我的配置文件`/home/qemu2/.gdbinit`中。但是，查看我的系统环境没有这个文件，于是自己新建了一个文件，并把上面的代码加入进入。但是在执行`source ./.gdbinit`命令时，提示`add-auto-load-safe-path`这个命令找不到，于是干脆把

    set auto-load safe-path /

这行代码添加到配置文件`/home/qemu2/.gdbinit`中，再执行`source ./.gdbinit`命令，没有错误发生。

于是启动内核代码，然后在另一个命令行窗口中执行gdb调试，就像上面的操作一样，显示：

    function lx_current -- Return current task
    function lx_module -- Find module by name and return the module variable
    function lx_per_cpu -- Return per-cpu variable
    function lx_task_by_pid -- Find Linux task by PID and return the task_struct variable
    function lx_thread_info -- Calculate Linux thread_info from task variable
    lx-dmesg -- Print Linux kernel log buffer
    lx-list-check -- Verify a list consistency
    lx-lsmod -- List currently loaded modules
    lx-ps -- Dump Linux tasks
    lx-symbols -- (Re-)load symbols of Linux kernel and currently loaded modules

至此，终于可以安心调试内核了。

#### 参考：

1. [Tips for Linux Kernel Development](http://eisen.io/slides/jeyu_tips_for_kernel_dev_cmps107_2017.pdf)
2. [How to Build A Custom Linux Kernel For Qemu](http://mgalgs.github.io/2015/05/16/how-to-build-a-custom-linux-kernel-for-qemu-2015-edition.html)
3. [Linux Kernel System Debugging](https://blog.0x972.info/?d=2014/11/27/18/45/48-linux-kernel-system-debugging-part-1-system-setup)
4. [Debugging kernel and modules via gdb](https://www.kernel.org/doc/html/latest/dev-tools/gdb-kernel-debugging.html)
5. [BusyBox simplifies embedded Linux systems](https://www.ibm.com/developerworks/library/l-busybox/index.html)
6. [Custom Initramfs](https://wiki.gentoo.org/wiki/Custom_Initramfs)
7. [Per-CPU variables](https://0xax.gitbooks.io/linux-insides/content/Concepts/per-cpu.html)
8. [Linux kernel debugging with GDB: getting a task running on a CPU](http://slavaim.blogspot.com/2017/09/linux-kernel-debugging-with-gdb-getting.html)
9. [gdb-kernel-debugging](https://www.kernel.org/doc/Documentation/dev-tools/gdb-kernel-debugging.rst)
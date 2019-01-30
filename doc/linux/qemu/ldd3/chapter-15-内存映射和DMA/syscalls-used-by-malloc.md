
# malloc使用的系统调用

英文文章源地址：[Syscalls used by malloc](https://sploitfun.wordpress.com/2015/02/11/syscalls-used-by-malloc/)

看本文章之前，你应该知道`malloc`使用系统调用获取内存。正如下图中所展示的，`malloc`调用`brk`或`mmap`系统调用去获取内存。

**brk**： 
`brk`通过增加程序`brk`的位置从内核中获取内存（没有被0初始化）。开始的时候，堆的初始位置(`start_brk`)和结束位置(`brk`)指向相同的位置。

* 当`ASLR`关闭时，`start_brk`和`brk`将指向`data/bss`段（end_data）的结尾。

* 当`ASLR`打开时，`start_brk`和`brk`将等于`data/bss`段（end_data）的结尾加上随机brk偏移。

![Figure 15-1-6](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_device_drivers_3_images/15-1-6.png)

上面的`进程虚拟地址布局`-图片展示了`start_brk`就是堆（heap）段的开始处，`brk`（program break）就是堆结束处。

实例：

    /* sbrk 和 brk 例子 */
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>

    int main()
    {
        void *curr_brk, *tmp_brk = NULL;

        printf("Welcome to sbrk example:%d\n", getpid());

        /* sbrk(0)给出了程序的break位置 */
        tmp_brk = curr_brk = sbrk(0);
        printf("Program Break Location1:%p\n", curr_brk);
        getchar();

        /* brk(addr) 增加/减少程序的break位置 */
        brk(curr_brk+4096);

        curr_brk = sbrk(0);
        printf("Program break Location2:%p\n", curr_brk);
        getchar();

        brk(tmp_brk);

        curr_brk = sbrk(0);
        printf("Program Break Location3:%p\n", curr_brk);
        getchar();

        return 0;
    }

输出分析：

1. 在增加程序`break`之前，通过下面的观察我们可以看到没有`heap`段，因而，`start_brk = brk = end_data = 0x804b000`.

        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$ ./sbrk 
        Welcome to sbrk example:6141
        Program Break Location1:0x804b000
        ...
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$ cat /proc/6141/maps
        ...
        0804a000-0804b000 rw-p 00001000 08:01 539624     /home/sploitfun/ptmalloc.ppt/syscalls/sbrk
        b7e21000-b7e22000 rw-p 00000000 00:00 0 
        ...
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$

2. 增加之后，通过下面的输出我们可以看到`heap`已经有了，因而，`start_brk = end_data = 0x804b000`和`brk = 0x804c000`.

        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$ ./sbrk 
        Welcome to sbrk example:6141
        Program Break Location1:0x804b000
        Program Break Location2:0x804c000
        ...
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$ cat /proc/6141/maps
        ...
        0804a000-0804b000 rw-p 00001000 08:01 539624     /home/sploitfun/ptmalloc.ppt/syscalls/sbrk
        0804b000-0804c000 rw-p 00000000 00:00 0          [heap]
        b7e21000-b7e22000 rw-p 00000000 00:00 0 
        ...
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$

在这里，

* 0804b000-0804c000 是堆的虚拟地址起始/结束范围。
* rw-p是读、写、可执行，私有/共享的权限标志。
* 00000000 是文件偏移量 – 因为它没有从任何文件映射，所以，在这儿是0。
* 00:00 主/次设备号 – 因为没有从任何文件映射，这儿是0。
* 0 节点号 – 因为没有从文件映射，所以是0。
* [heap] 堆的说明

**mmap**:

`malloc`使用`mmap`创建私有映射内存段。目的就是申请一段新内存（0填充），这段内存将由调用的进程专用。

示例：

    /* 使用mmap系统调用创建私有内存 */
    #include <stdio.h>
    #include <sys/mman.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdlib.h>

    void static inline errExit(const char* msg)
    {
            printf("%s failed. Exiting the process\n", msg);
            exit(-1);
    }

    int main()
    {
        int ret = -1;
        printf("Welcome to private anonymous mapping example::PID:%d\n", getpid());
        printf("Before mmap\n");
        getchar();
        char* addr = NULL;
        addr = mmap(NULL, (size_t)132*1024, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (addr == MAP_FAILED)
            errExit("mmap");
        printf("After mmap\n");
        getchar();

        /* Unmap mapped region. */
        ret = munmap(addr, (size_t)132*1024);
        if(ret == -1)
            errExit("munmap");
        printf("After munmap\n");
        getchar();
        return 0;
    }

输出分析：

1. 在mmap之前，通过下面的输出，我们只能看到属于共享库`libc.so`和`ld-linux.so`的内存映射：
    
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$ cat /proc/6067/maps
        08048000-08049000 r-xp 00000000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        08049000-0804a000 r--p 00000000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        0804a000-0804b000 rw-p 00001000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        b7e21000-b7e22000 rw-p 00000000 00:00 0 
        ...
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$

2. 在mmap之后，通过观察我们可以看到内存映射段（b7e00000-b7e22000，大小为我们设置的132k）和本就已经存在的内存映射（b7e21000-b7e22000）组合起来了：
    
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$ cat /proc/6067/maps
        08048000-08049000 r-xp 00000000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        08049000-0804a000 r--p 00000000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        0804a000-0804b000 rw-p 00001000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        b7e00000-b7e22000 rw-p 00000000 00:00 0 
        ...
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$

在这儿，

* b7e00000-b7e22000 该段的虚拟地址
* rw-p 是读、写、可执行，私有/共享的权限标志。
* 00000000 是文件偏移量 – 因为它没有从任何文件映射，所以，在这儿是0。
* 00:00 主/次设备号 – 因为没有从任何文件映射，这儿是0。
* 0 因为没有从文件映射，所以是0。

3. 在munmap之后，通过下面的输出，我们的内存映射段已经被返回给操作系统了。

        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$ cat /proc/6067/maps
        08048000-08049000 r-xp 00000000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        08049000-0804a000 r--p 00000000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        0804a000-0804b000 rw-p 00001000 08:01 539691     /home/sploitfun/ptmalloc.ppt/syscalls/mmap
        b7e21000-b7e22000 rw-p 00000000 00:00 0 
        ...
        sploitfun@sploitfun-VirtualBox:~/ptmalloc.ppt/syscalls$

注意，在我们的程序中`ASLR`被关闭。

相关文章： [栈缓存溢出](https://blog.csdn.net/shenwanjiang111/article/details/86692256)
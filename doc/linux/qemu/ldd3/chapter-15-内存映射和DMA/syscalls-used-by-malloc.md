
# malloc使用的系统调用

Having landed on this page, you should know malloc uses syscalls to obtain memory from the OS. As shown in the below picture malloc invokes either `brk` or `mmap` syscall to obtain memory.

brk: `brk` obtains memory (non zero initialized) from kernel by increasing program break location (brk). Initially start (`start_brk`) and end of heap segment (`brk`) would point to same location.

* When `ASLR` is turned off, `start_brk` and brk would point to end of data/bss segment (end_data).

* When ASLR is turned on, start_brk and brk would be equal to end of data/bss segment (end_data) plus random brk offset.

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
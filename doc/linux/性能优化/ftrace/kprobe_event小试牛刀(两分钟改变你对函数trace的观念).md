有时候想要知道Linux系统中哪个进程打开了某个文件，你会想到使用审计工具，或者自己写一个kprobe模块跟踪do_sys_open函数。

```c
long do_sys_open(int dfd, const char __user *filename, int flags, umode_t mode)
```

不需要这么麻烦，不管你在哪个架构平台上工作，只需使用echo 和 cat命令执行同样的操作就能办到(内核会自动创建kprobe)：

```shell
cd /sys/kernel/debug/tracing
echo 'p do_sys_open arg1=+0($arg2):string' > ./kprobe_events
echo 1 > ./events/kprobes/p_do_sys_open_0/enable
```

以上的`$argN`屏蔽了各个cpu架构获取函数参数必须使用架构相关的寄存器（比如在x86_64获取第二个函数参数可以用`%si`, 在riscv上使用`%a1`),其中对于riscv架构,maintainer已经接受了我的patch,很快就能合入Linux主线支持`$argN`。

然后任意cat一下,比如/open.c文件，再从ftrace环形队列缓冲区就能读到是进程号28257(cat)打开了什么文件。

```shell
cat /open.c
cat ./trace
```
```shell
...
cat-28257 [002] .... 730138.355766: p_do_sys_open_0: (do_sys_open+0x0/0x260) arg1="/open.c"
```
如果也想知道在打开文件时候的call trace:

```shell
echo 1 > ./options/stacktrace
cat /open.c 
cat ./trace
```
```shell
 cat-28271 [003] .... 730253.982115: p_do_sys_open_0: (do_sys_open+0x0/0x260) arg1="/open.c"
 cat-28271 [003] .... 730253.982115: <stack trace>
 => do_sys_open
 => do_syscall_64
 => entry_SYSCALL_64_after_hwframe
```

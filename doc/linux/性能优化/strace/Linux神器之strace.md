[TOC]

## 1 介绍
[strace](http://man7.org/linux/man-pages/man1/strace.1.html)是Linux下诊断和debug用户态与内核态交互的一个强有力工具，比如系统调用，信号发送和进程状态的更改等。无需编译目标源代码，可以直接追踪进程。尤其是，我们无法取得源代码的情况下，这简直就是一个利器。

`strace`是基于[ptrace](http://man7.org/linux/man-pages/man2/ptrace.2.html)的内核特性实现的。

## 2 功能特性

#### 2.1 跟踪运行中的进程

```shell
$ strace -p 26380
strace: Process 26380 attached
...
```

#### 2.2 统计系统调用

假设我们想统计每一次系统调用的所执行的时间、次数和出错的次数等，可以使用`-c`选项。下面以`uptime`程序为例，进行说明。

```shell
# strace -c uptime
 13:33:56 up  3:00,  1 user,  load average: 0.07, 0.04, 0.01
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 17.44    0.000206           9        22           mprotect
 13.21    0.000156           5        30           mmap
 11.09    0.000131           7        18           openat
 10.33    0.000122           6        20           read
  7.62    0.000090           8        12           alarm
  7.28    0.000086           5        18           close
  6.60    0.000078           8        10           rt_sigaction
  6.10    0.000072           5        16           fstat
  6.10    0.000072           9         8           fcntl
  3.73    0.000044           3        13        13 access
  2.79    0.000033           8         4           lseek
  1.61    0.000019          19         1           munmap
  1.44    0.000017           6         3           brk
  1.02    0.000012          12         1           write
  1.02    0.000012          12         1           arch_prctl
  0.68    0.000008           8         1           rt_sigprocmask
  0.68    0.000008           8         1           set_tid_address
  0.68    0.000008           8         1           prlimit64
  0.59    0.000007           7         1           set_robust_list
  0.00    0.000000           0         1           execve
------ ----------- ----------- --------- --------- ----------------
100.00    0.001181                   182        13 total
```

#### 2.3 打印调用路径和文件描述符相关的信息

```shell
~$ strace -yy cat /dev/null
execve("/bin/cat", ["cat", "/dev/null"], 0x7ffd16bb5ab0 /* 60 vars */) = 0
brk(NULL)                               = 0x559883332000
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3</etc/ld.so.cache>
fstat(3</etc/ld.so.cache>, {st_mode=S_IFREG|0644, st_size=74541, ...}) = 0
mmap(NULL, 74541, PROT_READ, MAP_PRIVATE, 3</etc/ld.so.cache>, 0) = 0x7fc18f8e4000
close(3</etc/ld.so.cache>)              = 0
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3</lib/x86_64-linux-gnu/libc-2.27.so>
read(3</lib/x86_64-linux-gnu/libc-2.27.so>, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\260\34\2\0\0\0\0\0"..., 832) = 832
fstat(3</lib/x86_64-linux-gnu/libc-2.27.so>, {st_mode=S_IFREG|0755, st_size=2030544, ...}) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fc18f8e2000
mmap(NULL, 4131552, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3</lib/x86_64-linux-gnu/libc-2.27.so>, 0) = 0x7fc18f2df000
mprotect(0x7fc18f4c6000, 2097152, PROT_NONE) = 0
mmap(0x7fc18f6c6000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3</lib/x86_64-linux-gnu/libc-2.27.so>, 0x1e7000) = 0x7fc18f6c6000
mmap(0x7fc18f6cc000, 15072, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7fc18f6cc000
close(3</lib/x86_64-linux-gnu/libc-2.27.so>) = 0
arch_prctl(ARCH_SET_FS, 0x7fc18f8e3540) = 0
mprotect(0x7fc18f6c6000, 16384, PROT_READ) = 0
mprotect(0x559881cd9000, 4096, PROT_READ) = 0
mprotect(0x7fc18f8f7000, 4096, PROT_READ) = 0
munmap(0x7fc18f8e4000, 74541)           = 0
brk(NULL)                               = 0x559883332000
brk(0x559883353000)                     = 0x559883353000
openat(AT_FDCWD, "/usr/lib/locale/locale-archive", O_RDONLY|O_CLOEXEC) = 3</usr/lib/locale/locale-archive>
fstat(3</usr/lib/locale/locale-archive>, {st_mode=S_IFREG|0644, st_size=10281936, ...}) = 0
mmap(NULL, 10281936, PROT_READ, MAP_PRIVATE, 3</usr/lib/locale/locale-archive>, 0) = 0x7fc18e910000
close(3</usr/lib/locale/locale-archive>) = 0
fstat(1</dev/pts/0>, {st_mode=S_IFCHR|0620, st_rdev=makedev(136, 0), ...}) = 0
openat(AT_FDCWD, "/dev/null", O_RDONLY) = 3</dev/null>
fstat(3</dev/null>, {st_mode=S_IFCHR|0666, st_rdev=makedev(1, 3), ...}) = 0
fadvise64(3</dev/null>, 0, 0, POSIX_FADV_SEQUENTIAL) = 0
mmap(NULL, 139264, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fc18f8c0000
read(3</dev/null>, "", 131072)          = 0
munmap(0x7fc18f8c0000, 139264)          = 0
close(3</dev/null>)                     = 0
close(1</dev/pts/0>)                    = 0
close(2</dev/pts/0>)                    = 0
exit_group(0)                           = ?
+++ exited with 0 +++
```

#### 2.4 过滤系统调用

```shell
-e trace=%clock    Trace all system calls that read or modify system clocks.
         %creds    Trace all system calls that read or modify user and group identifiers or capability sets.
         %desc     Trace all file descriptor related system calls.
         %file     Trace all system calls which take a file name as an argument.
         %fstat    Trace fstat and fstatat syscall variants.
         %fstatfs  Trace fstatfs, fstatfs64, fstatvfs, osf_fstatfs, and osf_fstatfs64 system calls.
         %ipc      Trace all IPC related system calls.
         %lstat    Trace lstat syscall variants.
         %memory   Trace all memory mapping related system calls.
         %network  Trace all the network related system calls.
         %process  Trace all system calls which involve process management.
         %pure     Trace syscalls that always succeed and have no arguments.
         %signal   Trace all signal related system calls.
         %stat     Trace stat syscall variants.
         %statfs   Trace statfs, statfs64, statvfs, osf_statfs, and osf_statfs64 system calls.
         %%stat    Trace syscalls used for requesting file status.
         %%statfs  Trace syscalls related to file system statistics.
```

示例：还是以`cat /dev/null`为例，打印出所有以文件名称作为参数的系统调用。

```shell
~$ strace -yy -e trace=%file cat /dev/null
execve("/bin/cat", ["cat", "/dev/null"], 0x7ffd0da0ab40 /* 60 vars */) = 0
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3</etc/ld.so.cache>
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3</lib/x86_64-linux-gnu/libc-2.27.so>
openat(AT_FDCWD, "/usr/lib/locale/locale-archive", O_RDONLY|O_CLOEXEC) = 3</usr/lib/locale/locale-archive>
openat(AT_FDCWD, "/dev/null", O_RDONLY) = 3</dev/null>
+++ exited with 0 +++
```

#### 2.5 只跟踪访问指定路径的系统调用

```shell
~$ strace -P /etc/ld.so.cache ls /var/empty
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=74541, ...}) = 0
mmap(NULL, 74541, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f0876290000
close(3)                                = 0
ls: cannot access '/var/empty': No such file or directory
+++ exited with 2 +++
```

#### 2.6 对从文件描述符读写的所有数据执行完整的十六进制和ASCII转储

```shell
$ strace -ewrite=1 -e trace=sendmsg ./recvmsg > /dev/null
sendmsg(1, {msg_name=NULL, msg_namelen=0, msg_iov=[{iov_base="012", iov_len=3},
{iov_base="34567", iov_len=5}, {iov_base="89abcde", iov_len=7}], msg_iovlen=3,
msg_controllen=0, msg_flags=0}, 0) = 15
 * 3 bytes in buffer 0
 | 00000  30 31 32                                          012              |
 * 5 bytes in buffer 1
 | 00000  33 34 35 36 37                                    34567            |
 * 7 bytes in buffer 2
 | 00000  38 39 61 62 63 64 65                              89abcde          |
+++ exited with 0 +++
```

#### 2.7 执行系统调用错误注入

```shell
$ strace -e trace=open -e fault=open cat
open("/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = -1 ENOSYS (Function not implemented) (INJECTED)
open("/lib/x86_64-linux-gnu/tls/x86_64/libc.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOSYS (Function not implemented) (INJECTED)
open("/lib/x86_64-linux-gnu/tls/libc.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOSYS (Function not implemented) (INJECTED)
open("/lib/x86_64-linux-gnu/x86_64/libc.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOSYS (Function not implemented) (INJECTED)
open("/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOSYS (Function not implemented) (INJECTED)
cat: error while loading shared libraries: libc.so.6: cannot open shared object file: Error 38
+++ exited with 127 +++
```

## 3 常用的参数

* `-c`

    统计每一系统调用的所执行的时间,次数和出错的次数等。

* `-d`

    显示有关标准错误的strace本身的一些调试输出。

* `-f`
  
    跟踪子进程，这些子进程是由于`fork(2)`系统调用而由当前跟踪的进程创建的。

* `-i`

    在系统调用时打印指令指针。

* `-t`

    跟踪的每一行都以时间为前缀。

* `-tt`

    如果给出两次，则打印时间将包括微秒。

* `-ttt`

    如果给定三次，则打印时间将包括微秒，并且前导部分将打印为自该**以来的秒数。

* `-T`

    显示花费在系统调用上的时间。这将记录每个系统调用的开始和结束之间的时间差。

* `-v`

    打印环境，统计信息，termios等调用的未缩写版本。这些结构在调用中非常常见，因此默认行为显示了结构成员的合理子集。使用此选项可获取所有详细信息。

* `-V`

    打印strace的版本号。

* `-e expr`

    限定表达式，用于修改要跟踪的事件或如何跟踪它们：

* `-e trace=set`

仅跟踪指定的系统调用集。该`-e`选项用于确定哪些系统调用可能是跟踪有用有用。例如，`trace=open`，`close`，`read`，`write`表示仅跟踪这四个系统调用。

* `-e trace=file`

    跟踪所有以文件名作为参数的系统调用。

* `-e trace=process`

跟踪涉及过程管理的所有系统调用。这对于观察进程的派生，等待和执行步骤很有用。

* `-e trace=network`

    跟踪所有与网络相关的系统调用。

* `-e trace=signal`

    跟踪所有与信号相关的系统调用。

* `-e trace=ipc`

    跟踪所有与IPC相关的系统调用。

* `-o 文件名`

    将跟踪输出写入文件名而不是stderr。

* `-p pid`

    使用进程ID pid附加到该进程并开始跟踪。跟踪可以随时通过键盘中断信号（CTRL -C）终止。

* `-S`

    按指定条件对`-c`选项打印的直方图输出进行排序。

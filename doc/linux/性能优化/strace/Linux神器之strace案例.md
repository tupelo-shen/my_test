## 1 用strace调试程序

在理想世界里，每当一个程序不能正常执行一个功能时，它就会给出一个有用的错误提示，告诉你在足够的改正错误的线索。但遗憾的是，我们不是生活在理想世界 里，起码不总是生活在理想世界里。有时候一个程序出现了问题，你无法找到原因。

这就是调试程序出现的原因。`strace`是一个必不可少的调试工具，`strace`用来监视系统调用。你不仅可以调试一个新开始的程序，也可以调试一个已经在运行的程序（把`strace`绑定到一个已有的PID上 面）。

首先让我们看一个真实的例子：启动KDE时出现问题。前一段时间，我在 启动KDE的时候出了问题，KDE的错误信息无法给我任何有帮助的线索。

```
_KDE_IceTransSocketCreateListener: failed to bind listener
_KDE_IceTransSocketUNIXCreateListener: ...SocketCreateListener() failed
_KDE_IceTransMakeAllCOTSServerListeners: failed to create listener for local

Cannot establish any listening sockets DCOPServer self-test failed.
```

对 我来说这个错误信息没有太多意义，只是一个对KDE来说至关重要的负责进程间通信的程序无法启动。我还可以知道这个错误和ICE协议（Inter Client Exchange）有关，除此之外，我不知道什么是KDE启动出错的原因。

我决定采用`strace`看一下在启动 `dcopserver`时到底程序做了什么：

```shell
strace -f -F -o ~/dcop-strace.txt dcopserver
```

> 这里，`-f -F`选项告诉`strace`同时跟踪`fork`和`vfork`出来的进程，`-o`选项把所有`strace`输出写到`~/dcop-strace.txt`里 面，`dcopserver`是要启动和调试的程序。

再次出现错误之后，我检查了错误输出文件`dcop-strace.txt`，文件里有很多系统调用的记录。在程序运行出错前的有关记录如下：

```txt
27207 mkdir("/tmp/.ICE-unix", 0777) = -1 EEXIST (File exists)
27207 lstat64("/tmp/.ICE-unix", {st_mode=S_IFDIR|S_ISVTX|0755, st_size=4096, ...}) = 0
27207 unlink("/tmp/.ICE-unix/dcop27207-1066844596") = -1 ENOENT (No such file or directory)
27207 bind(3, {sin_family=AF_UNIX, path="/tmp/.ICE-unix/dcop27207-1066844596"}, 38) = -1 EACCES (Permission denied) 
27207 write(2, "_KDE_IceTrans", 13) = 13
27207 write(2, "SocketCreateListener: failed to "..., 46) = 46
27207 close(3) = 0 27207 write(2, "_KDE_IceTrans", 13) = 13
27207 write(2, "SocketUNIXCreateListener: ...Soc"..., 59) = 59
27207 umask(0) = 0 27207 write(2, "_KDE_IceTrans", 13) = 13
27207 write(2, "MakeAllCOTSServerListeners: fail"..., 64) = 64
27207 write(2, "Cannot establish any listening s"..., 39) = 39
```

其中第1行显示程序试图创建`/tmp/.ICE-unix`目录，权限为`0777`，这个操作因为目录已经存在而失败了。第2个系统调用`lstat64`检查了目录状态，并显示这个目录的权限是`0755`，这里出现了第一个程序运行错误的线索：程序试图创建属性为`0777`的目录，但是已经存在了一个属性为`0755`的目录。第3个系统调用`unlink`试图删除一个文件，但是这个文件并不存在。这并不奇怪，因为这个操作只是试图删掉可能存在的老文件。

但是，第4行确认了错误所在。它试图绑定到`/tmp/.ICE-unix/dcop27207-1066844596`，但是出现了拒绝访问错误。`.ICE_unix`目录的用户和组都是`root`，并且只有所有者具有写权限。一个非root用户无法在这个目录下面建立文件，如果把目录属性改成`0777`， 则前面的操作有可能可以执行，而这正是第一步错误出现时进行过的操作。

所以我运行了`chmod 0777 /tmp/.ICE-unix`之后KDE就可以正常启动了，问题解决了。用`strace`进行跟踪调试，只需要花很短的几分钟时间跟踪程序运行，然后检查并分析输出文件。

> 运行`chmod 0777`只是一个测试，一般不要把一个目录设置成所有用户可读写，同时不设置粘滞位`(sticky bit)`。给目录设置粘滞位可以阻止一个用户随意删除可写目录下面其它人的文件。一般你会发现`/tmp`目录因为这个原因设置了粘滞位。KDE可以正常启动之后，运行`chmod +t /tmp/.ICE-unix`给`.ICE_unix`设置粘滞位。

## 2 解决库依赖问题

`starce`的另一个用处是解决和动态库相关的问题。当对一个可执行文件运行`ldd`时，它会告诉你程序使用的动态库和找到动态库的位置。但是如果你正在使用一个比较老的`glibc`版本（`2.2`或更早），你可能会有一个有`bug`的`ldd`程序，它可能会报告在一个目录下发现一个动态库，但是真正运行程序时动态连接程序 （`/lib/ld-linux.so.2`）却可能到另外一个目录去找动态连接库。这通常因为`/etc/ld.so.conf`和 `/etc/ld.so.cache`文件不一致，或者`/etc/ld.so.cache`被破坏。在`glibc 2.3.2`版本上这个错误不会出现，可能ld-linux的这个bug已经被解决了。

尽管这样，ldd并不能把所有程序依赖的动态库列出 来，系统调用`dlopen`可以在需要的时候自动调入需要的动态库，而这些库可能不会被ldd列出来。作为glibc的一部分的NSS（Name Server Switch）库就是一个典型的例子，NSS的一个作用就是告诉应用程序到哪里去寻找系统帐号数据库。应用程序不会直接连接到NSS库，glibc则会通过dlopen自动调入NSS库。如果这样的库偶然丢失，你不会被告知存在库依赖问题，但这样的程序就无法通过用户名解析得到用户ID了。让我们看一个例子：

`whoami`程序会给出你自己的用户名，这个程序在一些需要知道运行程序的真正用户的脚本程序里面非常有用，`whoami`的一个示例 输出如下：

```shell
# whoami
root
```

假设因为某种原因在升级glibc的过程中负责用户名和用户ID转换的库NSS丢失，我们可以通过把nss库改名来模拟这个环境：

```shell
# mv /lib/libnss_files.so.2 /lib/libnss_files.so.2.backup 
# whoami
whoami: cannot find username for UID 0
```

这里你可以看到，运行whoami时出现了错误，ldd程序的输出不会提供有用的帮助：

```shell
# ldd /usr/bin/whoami
libc.so.6 => /lib/libc.so.6 (0x4001f000)
/lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x40000000)
```

你只会看到`whoami`依赖`libc.so.6`和`ld-linux.so.2`，它没有给出运行`whoami`所必须的其他库。这里时用`strace`跟踪`whoami`时的输出：

```shell
~$ strace -o whoami-strace.txt whoami
open("/lib/libnss_files.so.2", O_RDONLY) = -1 ENOENT (No such file or directory)
open("/lib/i686/mmx/libnss_files.so.2", O_RDONLY) = -1 ENOENT (No such file or directory)
stat64("/lib/i686/mmx", 0xbffff190) = -1 ENOENT (No such file or directory) 
open("/lib/i686/libnss_files.so.2", O_RDONLY) = -1 ENOENT (No such file or directory)
stat64("/lib/i686", 0xbffff190) = -1 ENOENT (No such file or directory)
open("/lib/mmx/libnss_files.so.2", O_RDONLY) = -1 ENOENT (No such file or directory)
stat64("/lib/mmx", 0xbffff190) = -1 ENOENT (No such file or directory) 
open("/lib/libnss_files.so.2", O_RDONLY) = -1 ENOENT (No such file or directory)
stat64("/lib", {st_mode=S_IFDIR|0755, st_size=2352, ...}) = 0
open("/usr/lib/i686/mmx/libnss_files.so.2", O_RDONLY) = -1 ENOENT (No such file or directory)
stat64("/usr/lib/i686/mmx", 0xbffff190) = -1 ENOENT (No such file or directory) 
open("/usr/lib/i686/libnss_files.so.2", O_RDONLY) = -1 ENOENT (No such file or directory)
```

你可以发现在不同目录下面查找`libnss.so.2`的尝试，但是都失败了。如果没有`strace`这样的工具，很难发现这个错误是由于缺少动态库造成的。现在只需要找到`libnss.so.2`并把它放回到正确的位置就可以了。
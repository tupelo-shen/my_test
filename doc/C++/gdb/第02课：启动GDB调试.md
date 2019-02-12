使用GDB调试程序一般有3种方式：

    gdb filename            # 直接调试目标程序
    gdb attach pid          # 附加进程
    gdb filename corename   # 调试core文件

## 2.1 直接调试目标程序

在开发阶段或者研究别人的项目时，当编译成功生成目标二进制文件后，可以使用 `gdb filename` 直接启动这个程序的调试，其中 `filename` 是需要启动的调试程序文件名，这种方式是直接使用 GDB 启动一个程序进行调试。注意这里说的启动一个程序进行调试其实不严谨，因为实际上只是附加了一个可执行文件，并没有把程序启动起来；接着需要输入`run` 命令，程序才会真正的运行起来。关于 `run` 命令后面的课程中会详细介绍。上一课的 `GDB` 调试 `hello_server` 系列就是使用的这种方式。

## 2.2 附加进程

在某些情况下，一个程序已经启动了，我们想调试这个程序，但是又不想重启这个程序。假设有这样一个场景，我们的聊天测试服务器程序正在运行，运行一段时间之后，发现这个聊天服务器不能接受新的客户端连接了，这时肯定是不能重启程序的，如果重启，当前程序的各种状态信息就丢失了。怎么办呢？可以使用 gdb attach 进程 ID 来将 GDB 调试器附加到聊天测试服务器程序上。例如，假设聊天程序叫 chatserver，可以使用 ps 命令获取该进程的 PID，然后使用 gdb attach 就可以调试了，操作如下：

    [zhangyl@iZ238vnojlyZ flamingoserver]$ ps -ef | grep chatserver
    zhangyl  21462 21414  0 18:00 pts/2    00:00:00 grep --color=auto chatserver
    zhangyl  26621     1  5 Oct10 ?        2-17:54:42 ./chatserver -d

通过以上代码得到 chatserver 的 PID 为 26621，然后使用 gdb attach 26621 把 GDB 附加到 chatserver 进程，操作并输出如下：

    [zhangyl@localhost flamingoserver]$ gdb attach 26621
    Attaching to process 26661
    Reading symbols from /home/zhangyl/flamingoserver/chatserver...done.
    Reading symbols from /usr/lib64/mysql/libmysqlclient.so.18...Reading symbols from /usr/lib64/mysql/libmysqlclient.so.18...(no debugging symbols found)...done.
    Reading symbols from /lib64/libpthread.so.0...(no debugging symbols found)...done.
    [New LWP 42931]
    [New LWP 42930]
    [New LWP 42929]
    [New LWP 42928]
    [New LWP 42927]
    [New LWP 42926]
    [New LWP 42925]
    [New LWP 42924]
    [New LWP 42922]
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".
    Loaded symbols for /lib64/libpthread.so.0

为了节约篇幅，上述代码中我删掉了一些无关的信息。当提示 “Attaching to process 26621” 时就说明我们已经成功地将 GDB 附加到目标进程了。需要注意的是，程序使用了一些系统库（如 libc.so），由于这是发行版本的 Linux 系统，这些库是没有调试符号的，因而 GDB 会提示找不到这些库的调试符号。因为目的是调试 chatserver，对系统 API 调用的内部实现并不关注，所以这些提示可以不用关注，只要 chatserver 这个文件有调试信息即可。

当用 gdb attach 上目标进程后，调试器会暂停下来，此时可以使用 continue 命令让程序继续运行，或者加上相应的断点再继续运行程序（这里提到的 continue 命令不熟悉也没有关系，后续会详细介绍这些命令的使用方法）。

当调试完程序想结束此次调试时，而且不对当前进程 chatserver 有任何影响，也就是说想让这个程序继续运行，可以在 GDB 的命令行界面输入 detach 命令让程序与 GDB 调试器分离，这样 chatserver 就可以继续运行了：

    (gdb) detach
    Detaching from program: /home/zhangyl/flamingoserver/chatserver, process 42921

然后再退出 GDB 就可以了：

    (gdb) quit
    [zhangyl@localhost flamingoserver]$ 

## 2.3 调试core文件

有时候，服务器程序运行一段时间后会突然崩溃，这并不是我们希望看到的，需要解决这个问题。只要程序在崩溃的时候有 core 文件产生，就可以使用这个 core 文件来定位崩溃的原因。当然，Linux 系统默认是不开启程序崩溃产生 core 文件这一机制的，我们可以使用 ulimit -c 命令来查看系统是否开启了这一机制。

> 顺便提一句，ulimit 这个命令不仅仅可以查看 core 文件生成是否开启，还可以查看其他的一些功能，比如系统允许的最大文件描述符的数量等，具体可以使用 ulimit -a 命令来查看，由于这个内容与本课主题无关，这里不再赘述。

    [zhangyl@localhost flamingoserver]$ ulimit -a
    core file size          (blocks, -c) 0
    data seg size           (kbytes, -d) unlimited
    scheduling priority             (-e) 0
    file size               (blocks, -f) unlimited
    pending signals                 (-i) 15045
    max locked memory       (kbytes, -l) 64
    max memory size         (kbytes, -m) unlimited
    open files                      (-n) 1024
    pipe size            (512 bytes, -p) 8
    POSIX message queues     (bytes, -q) 819200
    real-time priority              (-r) 0
    stack size              (kbytes, -s) 8192
    cpu time               (seconds, -t) unlimited
    max user processes              (-u) 4096
    virtual memory          (kbytes, -v) unlimited
    file locks                      (-x) unlimited

发现 core file size 那一行默认是 0，表示关闭生成 core 文件，可以使用“ulimit 选项名 设置值”来修改。例如，可以将 core 文件生成改成具体某个值（最大允许的字节数），这里我们使用 ulimit -c unlimited（unlimited 是 -c 选项值）直接修改成不限制大小。

    [zhangyl@localhost flamingoserver]$ ulimit -c unlimited
    [zhangyl@localhost flamingoserver]$ ulimit -a
    core file size          (blocks, -c) unlimited
    data seg size           (kbytes, -d) unlimited
    scheduling priority             (-e) 0
    file size               (blocks, -f) unlimited
    pending signals                 (-i) 15045
    max locked memory       (kbytes, -l) 64
    max memory size         (kbytes, -m) unlimited
    open files                      (-n) 1024
    pipe size            (512 bytes, -p) 8
    POSIX message queues     (bytes, -q) 819200
    real-time priority              (-r) 0
    stack size              (kbytes, -s) 8192
    cpu time               (seconds, -t) unlimited
    max user processes              (-u) 4096
    virtual memory          (kbytes, -v) unlimited
    file locks                      (-x) unlimited

> 注意，这个命令容易记错，第一个 ulimit 是 Linux 命令， -c 选项后面的 unlimited 是选项的值，表示不限制大小，当然也可以改成具体的数值大小。很多初学者在学习这个命令时，总是把 ulimit 命令和 unlimited 取值搞混淆，如果读者能理解其含义，一般就不会混淆了。

还有一个问题就是，这样修改以后，当我们关闭这个 Linux 会话，设置项的值就会被还原成 0，而服务器程序一般是以后台程序（守护进程）长周期运行，也就是说当前会话虽然被关闭，服务器程序仍然继续在后台运行，这样这个程序在某个时刻崩溃后，是无法产生 core 文件的，这种情形不利于排查问题。因此，我们希望这个选项永久生效，永久生效的方式是把“ulimit -c unlimited”这一行加到 /etc/profile 文件中去，放到这个文件最后一行即可。

## 具体的例子

生成的 core 文件的默认命名方式是 core.pid，举个例子，比如某个程序当时运行时其进程 ID 是 16663，那么它崩溃产生的 core 文件的名称就是 core.16663。我们来看一个具体的例子，某次我发现服务器上的 msg_server 崩溃了，产生了一个如下的 core 文件：

    -rw------- 1 root root 10092544 Sep  9 15:14 core.21985

就可以通过这个 core.21985 文件来排查崩溃的原因，调试 core 文件的命令是：

    gdb filename corename

其中，filename 就是程序名，这里就是 msg_server；corename 是 core.21985，我们输入 gdb msg_server core.21985 来启动调试：

    [root@myaliyun msg_server]# gdb msg_server core.21985 
    Reading symbols from /root/teamtalkserver/src/msg_server/msg_server...done.
    [New LWP 21985]
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib64/libthread_db.so.1".
    Core was generated by `./msg_server -d'.
    Program terminated with signal 11, Segmentation fault.
    #0  0x00000000004ceb1f in std::less<CMsgConn*>::operator() (this=0x2283878, __x=@0x7ffca83563a0: 0x2284430, __y=@0x51: <error reading variable>)
        at /usr/include/c++/4.8.2/bits/stl_function.h:235
    235           { return __x < __y; }

可以看到程序崩溃的地方是在 stl_function.h 的第 235 行，然后通过 bt 命令（后续将详细介绍该命令）查看崩溃时的调用堆栈，进一步分析就能找到崩溃的原因。

    (gdb) bt
    #0  0x00000000004ceb1f in std::less<CMsgConn*>::operator() (this=0x2283878, __x=@0x7ffca83563a0: 0x2284430, __y=@0x51: <error reading variable>)
        at /usr/include/c++/4.8.2/bits/stl_function.h:235
    #1  0x00000000004cdd70 in std::_Rb_tree<CMsgConn*, CMsgConn*, std::_Identity<CMsgConn*>, std::less<CMsgConn*>, std::allocator<CMsgConn*> >::_M_get_insert_unique_pos
        (this=0x2283878, __k=@0x7ffca83563a0: 0x2284430) at /usr/include/c++/4.8.2/bits/stl_tree.h:1324
    #2  0x00000000004cd18a in std::_Rb_tree<CMsgConn*, CMsgConn*, std::_Identity<CMsgConn*>, std::less<CMsgConn*>, std::allocator<CMsgConn*> >::_M_insert_unique<CMsgConn* const&> (this=0x2283878, __v=@0x7ffca83563a0: 0x2284430) at /usr/include/c++/4.8.2/bits/stl_tree.h:1377
    #3  0x00000000004cc8bd in std::set<CMsgConn*, std::less<CMsgConn*>, std::allocator<CMsgConn*> >::insert (this=0x2283878, __x=@0x7ffca83563a0: 0x2284430)
        at /usr/include/c++/4.8.2/bits/stl_set.h:463
    #4  0x00000000004cb011 in CImUser::AddUnValidateMsgConn (this=0x2283820, pMsgConn=0x2284430) at /root/teamtalkserver/src/msg_server/ImUser.h:42
    #5  0x00000000004c64ae in CDBServConn::_HandleValidateResponse (this=0x227f6a0, pPdu=0x22860d0) at /root/teamtalkserver/src/msg_server/DBServConn.cpp:319
    #6  0x00000000004c5e3d in CDBServConn::HandlePdu (this=0x227f6a0, pPdu=0x22860d0) at /root/teamtalkserver/src/msg_server/DBServConn.cpp:203
    #7  0x00000000005022b3 in CImConn::OnRead (this=0x227f6a0) at /root/teamtalkserver/src/base/imconn.cpp:148
    #8  0x0000000000501db3 in imconn_callback (callback_data=0x7f4b20 <g_db_server_conn_map>, msg=3 '\003', handle=8, pParam=0x0)
        at /root/teamtalkserver/src/base/imconn.cpp:47
    #9  0x0000000000504025 in CBaseSocket::OnRead (this=0x227f820) at /root/teamtalkserver/src/base/BaseSocket.cpp:178
    #10 0x0000000000502f8a in CEventDispatch::StartDispatch (this=0x2279990, wait_timeout=100) at /root/teamtalkserver/src/base/EventDispatch.cpp:386
    #11 0x00000000004fddbe in netlib_eventloop (wait_timeout=100) at /root/teamtalkserver/src/base/netlib.cpp:160
    #12 0x00000000004d18c2 in main (argc=2, argv=0x7ffca8359978) at /root/teamtalkserver/src/msg_server/msg_server.cpp:213
    (gdb) 

堆栈 #4 就不是库代码了，我们可以排查这里的代码，然后找到问题原因。


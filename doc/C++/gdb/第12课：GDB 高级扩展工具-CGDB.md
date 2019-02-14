在使用 GDB 单步调试时，代码每执行一行才显示下一行，很多用惯了图形界面 IDE 调试的读者可能会觉得非常不方便，而 GDB TUI 可能看起来不错，但是存在经常花屏的问题，也让很多读者不胜其烦。那 Linux 下有没有既能在调试时动态显示当前调试处的文件代码，又能不花屏的工具呢？有的，这就是 CGDB。

CGDB 本质上是对 GDB 做了一层“包裹”，所有在 GDB 中可以使用的命令，在 CGDB 中也可以使用。

# CGDB 的安装

CGDB 的官网[请点击这里查看](http://cgdb.github.io/)，执行以下命令将 CGDB 压缩包下载到本地：

    wget https://cgdb.me/files/cgdb-0.7.0.tar.gz 

然后执行以下步骤解压、编译、安装：

    tar xvfz cgdb-0.7.0.tar.gz
    cd cgdb-0.7.0
    ./configure 
    make
    make install

> 安装过程中可能遇到的问题:
> ubuntu configure: error: CGDB requires curses.h or ncurses/curses.h to build.
> 
> sudo apt-get install libncurses5-dev libncursesw5-dev
>
> Please install makeinfo before installing
> 
> sudo apt-get install autoconf automake
>
> sudo apt-get install texinfo 
> 
> sudo apt-get install libreadline6 libreadline6-dev
>
> CentOS 系统可以百度，好多记录安装过程中的问题的解决方法。

# CGDB 的使用

安装成功以后，就可以使用 CGDB 了，在命令行输入 **cgdb** 命令启动 CGDB ，启动后界面如下：

![12-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/gdb/images/12-1.png)

界面分为上下两部分：上部为代码窗口，显示调试过程中的代码；下部就是 GDB 原来的命令窗口。默认窗口焦点在下部的命令窗口，如果想将焦点切换到上部的代码窗口，按键盘上的 Esc 键，之后再次按字母 i 键将使焦点回到命令窗口。

> 注意：这个“焦点窗口”的概念很重要，它决定着你当前可以操作的是上部代码窗口还是命令窗口（ 和GDB TUI 一样）。

我们用 Redis 自带的客户端程序 redis-cli 为例，输入以下命令启动调试：

    cgdb redis-cli

启动后的界面如下：

![12-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/gdb/images/12-2.png)

然后加两个断点，如下图所示：

![12-3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/gdb/images/12-3.png)

如上图所示，我们在程序的 main ( 上图中第 2824 行 ）和第 2832 行分别加了一个断点，添加断点以后，代码窗口的行号将会以红色显示，另外有一个绿色箭头指向当前执行的行（ 这里由于在 main 函数上加了个断点，绿色箭头指向第一个断点位置 ）。单步调试并步入第 2827 行的 sdsnew() 函数调用中，可以看到代码视图中相应的代码也发生了变化，并且绿色箭头始终指向当前执行的行数：

![12-4](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/gdb/images/12-4.png)

更多 CGDB 的用法可以查阅官网，也可以参考 CGDB 中文手册，[点击这里可查看详情](https://github.com/leeyiw/cgdb-manual-in-chinese/blob/master/SUMMARY.md)。

# CGDB 的不足之处

CGDB 虽然已经比原始的 GDB 和 GDB TUI 模式在代码显示方面改进了许多，但实际使用时，CGDB 中调用 GDB 的 print 命令无法显示字符串类型中的中文字符，要么显示乱码，要么不显示，会给程序调试带来很大的困扰，这点需要注意。

总的来说，CGDB 仍然能满足我们大多数场景下的调试，瑕不掩瑜， CGDB 在 Linux 系统中调试程序还是比 GDB 方便很多。
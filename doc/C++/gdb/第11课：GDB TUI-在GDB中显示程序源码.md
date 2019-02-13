很多 Linux 用户或者其他平台用户习惯了有强大的源码显示窗口的调试器，可能对 GDB 用 list 显示源码的方式非常不习惯，主要是因为 GDB 在调试的时候不能很好地展示源码。

GDB 中可以用 list 命令显示源码，但是 **list** 命令显示没有代码高亮，也不能一眼定位到正在执行的那行代码在整个代码中的位置。可以毫不夸张地说，这个问题是阻止很多人长期使用 GDB 的最大障碍，如此不便，以至于 GNU 都想办法解决了——使用 GDB 自带的 GDB TUI。

先来看一张效果图，是我在使用 GDB TUI 调试 redis-server 时的截图，这样看代码比使用 list 命令更方便。

![11-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/gdb/images/11-1.png)

# 开启 GDB TUI 模式

开启 GDB TUI 模式有两个方法。

* 方法一：
    
    使用 gdbtui 命令或者 gdb-tui 命令开启一个调试。

        gdbtui -q 需要调试的程序名

* 方法二：
    
    直接使用 GDB 调试代码，在需要的时候使用切换键 **Ctrl + X + A** 调出 GDB TUI 。

# GDB TUI 模式常用窗口

![11-2](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/gdb/images/11-2.png)

默认情况下，GDB TUI 模式会显示 *command* 窗口和 *source* 窗口，如上图所示，还有其他窗口，如下列举的四个常用的窗口：

* （cmd）command 命令窗口，可以输入调试命令
* （src）source 源代码窗口， 显示当前行、断点等信息
* （asm）assembly 汇编代码窗口
* （reg）register 寄存器窗口

可以通过 “**layout** + *窗口类型*” 命令来选择自己需要的窗口，例如，在 *cmd* 窗口输入 *layout asm* 则可以切换到汇编代码窗口。

![11-3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/gdb/images/11-3.png)

**layout** 命令还可以用来修改窗口布局，在 *cmd* 窗口中输入 **help layout**，常见的有：

    Usage: layout prev | next | <layout_name> 
    Layout names are:
       src   : Displays source and command windows.
       asm   : Displays disassembly and command windows.
       split : Displays source, disassembly and command windows.
       regs  : Displays register window. If existing layout
               is source/command or assembly/command, the 
               register window is displayed. If the
               source/assembly/command (split) is displayed, 
               the register window is displayed with 
               the window that has current logical focus.

另外，可以通过 **winheight** 命令修改各个窗口的大小，如下所示：

    (gdb) help winheight
    Set the height of a specified window.
    Usage: winheight <win_name> [+ | -] <#lines>
    Window names are:
    src  : the source window
    cmd  : the command window
    asm  : the disassembly window
    regs : the register display

    ##将代码窗口的高度扩大 5 行代码
    winheight src + 5
    ##将代码窗口的高度减小 4 代码
    winheight src - 4

当前 GDB TUI 窗口放大或者缩小以后，窗口中的内容不会自己刷新以适应新的窗口尺寸，我们可以通过 **space** 键强行刷新 GDB TUI 窗口。

# 窗口焦点切换

在默认设置下，**方向键** 和 **PageUp/PageDown** 都是用来控制 GDB TUI 的 *src* 窗口的。因此，我们常用上下键，在 *cmd* 窗口上显示前一条命令和后一条命令的功能就没有了，不过可以通过 **Ctrl + N/Ctrl + P** 来获取这个功能。

> 注意：通过方向键调整了GDB TUI 的 src 窗口以后，可以用 update 命令重新把焦点定位到当前执行的代码上。

我们可以通过 **focus** 命令来调整焦点位置，默认情况下焦点是在 src 窗口，通过 **focus next** 命令可以把焦点移到 cmd 窗口，这时候就可以像以前一样，通过方向键来切换上一条命令和下一条命令。同理，也可以使用 **focus prev** 切回到源码窗口，如果焦点不在 src 窗口，我们就不必使用方向键来浏览源码了。

    (gdb) help focus  
    help focus
    Set focus to named window or next/prev window.
    Usage: focus {<win> | next | prev}
    Valid Window names are:
    src  : the source window
    asm  : the disassembly window
    regs : the register display
    cmd  : the command window

# 小结

GDB TUI 提供了一个可视化的代码阅读功能，比使用 list 命令来查看代码要方便不少，有兴趣的读者可以尝试一下。

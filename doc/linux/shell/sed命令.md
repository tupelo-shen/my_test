Linux sed 命令是利用脚本来处理文本文件。

sed 可依照脚本的指令来处理、编辑文本文件。

sed 主要用来自动编辑一个或多个文件、简化对文件的反复操作、编写转换程序等。

[TOC]

# 1 语法

格式如下：
    
    sed [-hnV][-e<script>][-f<script文件>][文本文件]

## 1.1 参数说明

* -e<script\>或--expression=<script\> 以选项中指定的script来处理输入的文本文件。

* -f<script文件\>或--file=<script文件\> 以选项中指定的script文件来处理输入的文本文件。

* -h或--help 显示帮助。

* -n或--quiet或--silent 仅显示script处理后的结果。

* -V或--version 显示版本信息。

## 1.2 动作说明

* a ：新增， a 的后面可以接字串，而这些字串会在新的一行出现(目前的下一行)～
* c ：取代， c 的后面可以接字串，这些字串可以取代 n1,n2 之间的行！
* d ：删除，因为是删除啊，所以 d 后面通常不接任何东东；
* i ：插入， i 的后面可以接字串，而这些字串会在新的一行出现(目前的上一行)；
* p ：打印，亦即将某个选择的数据印出。通常 p 会与参数 sed -n 一起运行～
* s ：取代，可以直接进行取代的工作哩！通常这个 s 的动作可以搭配正规表示法！例如 1,20s/old/new/g 就是啦！

# 2 示例

在testfile文件的第四行后添加一行，并将结果输出到标准输出，在命令行提示符下输入如下命令：

    sed -e 4a\newLine testfile

首先查看testfile中的内容如下：

    $ cat testfile #查看testfile 中的内容  
    HELLO LINUX!  
    Linux is a free unix-type opterating system.  
    This is a linux testfile!  
    Linux test

使用sed命令后，输出结果如下：

    $ sed -e 4a\newline testfile #使用sed 在第四行后添加新字符串  
    HELLO LINUX! #testfile文件原有的内容  
    Linux is a free unix-type opterating system.  
    This is a linux testfile!  
    Linux test  
    newline

# 3 以行为单位的新增/删除

将`/etc/passwd`的内容列出并且列印行号，同时，请将第 2~5 行删除！

    [root@www ~]# nl /etc/passwd | sed '2,5d'
    1 root:x:0:0:root:/root:/bin/bash
    6 sync:x:5:0:sync:/sbin:/bin/sync
    7 shutdown:x:6:0:shutdown:/sbin:/sbin/shutdown
    .....(后面省略).....

sed 的动作为 '2,5d' ，那个 d 就是删除！因为 2-5 行给他删除了，所以显示的数据就没有 2-5 行罗～ 另外，注意一下，原本应该是要下达 sed -e 才对，没有 -e 也行啦！同时也要注意的是， sed 后面接的动作，请务必以 '' 两个单引号括住喔！

只要删除第 2 行

    nl /etc/passwd | sed '2d' 

要删除第 3 到最后一行

    nl /etc/passwd | sed '3,$d'

在第二行后(亦即是加在第三行)加上『drink tea?』字样！

    [root@www ~]# nl /etc/passwd | sed '2a drink tea'
    1 root:x:0:0:root:/root:/bin/bash
    2 bin:x:1:1:bin:/bin:/sbin/nologin
    drink tea
    3 daemon:x:2:2:daemon:/sbin:/sbin/nologin
    .....(后面省略).....

那如果是要在第二行前

    nl /etc/passwd | sed '2i drink tea' 

如果是要增加两行以上，在第二行后面加入两行字，例如 `Drink tea or .....` 与 `drink beer`?

    [root@www ~]# nl /etc/passwd | sed '2a Drink tea or ......\
    > drink beer ?'
    1 root:x:0:0:root:/root:/bin/bash
    2 bin:x:1:1:bin:/bin:/sbin/nologin
    Drink tea or ......
    drink beer ?
    3 daemon:x:2:2:daemon:/sbin:/sbin/nologin
    .....(后面省略).....

每一行之间都必须要以反斜杠『 \ 』来进行新行的添加喔！所以，上面的例子中，我们可以发现在第一行的最后面就有`\`存在。

# 4 以行为单位的替换与显示

将第2-5行的内容取代成为『No 2-5 number』呢？

    [root@www ~]# nl /etc/passwd | sed '2,5c No 2-5 number'
    1 root:x:0:0:root:/root:/bin/bash
    No 2-5 number
    6 sync:x:5:0:sync:/sbin:/bin/sync
    .....(后面省略).....

透过这个方法我们就能够将数据整行取代了！

仅列出`/etc/passwd`文件内的第`5-7`行

    [root@www ~]# nl /etc/passwd | sed -n '5,7p'
    5 lp:x:4:7:lp:/var/spool/lpd:/sbin/nologin
    6 sync:x:5:0:sync:/sbin:/bin/sync
    7 shutdown:x:6:0:shutdown:/sbin:/sbin/shutdown

可以透过这个 sed 的以行为单位的显示功能， 就能够将某一个文件内的某些行号选择出来显示。

# 5 数据的搜寻并显示

搜索 `/etc/passwd`有`root`关键字的行

    nl /etc/passwd | sed '/root/p'
    1  root:x:0:0:root:/root:/bin/bash
    1  root:x:0:0:root:/root:/bin/bash
    2  daemon:x:1:1:daemon:/usr/sbin:/bin/sh
    3  bin:x:2:2:bin:/bin:/bin/sh
    4  sys:x:3:3:sys:/dev:/bin/sh
    5  sync:x:4:65534:sync:/bin:/bin/sync
    ....下面忽略

# 6 sed与正则表达式

sed是强大，高效的处理正则表达式。一些复杂的任务，可以解决简单的正则表达式。任何命令行专家都知道正则表达式的威力。

本教程介绍了标准的正则表达式，POSIX类的正则表达式和元字符。考虑我们有一个文本文件 books.txt 将被处理，它有以下内容：

    A Storm of Swords, George R. R. Martin
    The Two Towers, J. R. R. Tolkien
    The Alchemist, Paulo Coelho
    The Fellowship of the Ring, J. R. R. Tolkien
    The Pilgrimage, Paulo Coelho
    A Game of Thrones, George R. R. Martin

## 6.1 标准的正则表达式

1. 行开始（^）

    插入符号(^)符号用于一行的开始匹配。下面的例子打印所有的行首匹配模式“the”行。

        [jerry]$ sed -n '/^The/ p' books.txt

    执行上面的代码，会得到如下结果：

        The Two Towers, J. R. R. Tolkien
        The Alchemist, Paulo Coelho
        The Fellowship of the Ring, J. R. R. Tolkien
        The Pilgrimage, Paulo Coelho

2. 行尾 ($)

    行尾是由美元符号（$）符号表示。下面的例子打印“Coelho”结尾的行。

        [jerry]$ sed -n '/Coelho$/ p' books.txt

    执行上面的代码，会得到如下结果：

        The Alchemist, Paulo Coelho 
        The Pilgrimage, Paulo Coelho

3. 单个字符(.)

    点（.）匹配除行字符结尾的任何单个字符。下面的例子打印所有三个字母的单词字符 “t” 结尾。

        [jerry]$ echo -e "cat\nbat\nrat\nmat\nbatting\nrats\nmats" | sed -n '/^..t$/p'



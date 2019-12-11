* [1 配置选项](#1)
    - [1.1 配置内核](#1.1)
    - [1.2 构建系统](#1.2)
* [2 菜单结构](#2)
* [3 Makefile](#3)

---

内核配置脚本文件的语法也比较简单，主要包括如下几个方面。

<h1 id="1">1 配置选项</h1>

大多数内核配置选项都对应Kconfig中的一个配置选项（config），比如说下面的代码：

        config MODVERSIONS
            bool "Module versioning support"
            help
                Usually, you have to use modules compiled with your kernel.
                Saying Y here makes it ...

config 关键字定义新的配置选项，之后的几行代码定义了该配置选项的属性。配置选项的属性包括类型、数据范围、输入提示、依赖关系、选择关系及帮助信息、默认值等。

1. 类型

    配置选项都必须指定类型，类型包括bool、tristate、string、hex 和 int。其中常用的是tristate 和string是2种基本类型，其余都是基于这2种基本类型。类型定义后可以紧跟输入提示，下面两段脚本是等价的：

        bool "Networking support"

    和

        bool
        prompt "Networking support"

    是等价的。

2. 提示信息

        prompt <prompt> [if <expr>]

    其中，可选的if用来表示该提示的依赖关系。

3. default值

        default <expr> [if <expr>]

    如果用户不设置对应的选项，配置选项的值就是默认值。

4. 依赖关系

        depends on（或者requires） <expr>

    如果定义了多重依赖关系，它们之间用“ &&”间隔。依赖关系也可以应用到该菜单中所有的其他选项（同样接受if 表达式）内，下面两段脚本是等价的：

        bool "foo" if BAR
        default y if BAR

    和

        depends on BAR
        bool "foo"
        default y

5. 选择关系（也称为反向依赖关系）的格式为：

        select <symbol> [if <expr>]

    A 如果选择了B，则在A 被选中的情况下，B 自动被选中。

6. 数据范围的格式为：

        range <symbol> <symbol> [if <expr>]

7. Kconfig 中的 expr（表达式）定义为：

        <expr> ::= <symbol>
                <symbol> '=' <symbol>
                <symbol> '!=' <symbol>
                '(' <expr> ')'
                '!' <expr>
                <expr> '&&' <expr>
                <expr> '||' <expr>

    看下面的示例代码：

        config SHDMA_R8A73A4
        depends on ARCH_R8A73A4 && SH_DMAE != n

    `ARCH_R8A73A4 && SH_DMAE != n`说明只有当ARCH_R8A73A4被选中，且SH_DMAE没有被选中的时候，才可能出现这个SHDMA_R8A73A4。

8. 为 int 和 hex 类型的选项设置可以接受的输入值范围，用户只能输入大于等于第一个symbol，且小于等于第二个symbol 的值。

9. help信息

        help（或---help---）
            start
            ...
            end

<h1 id="2">2 菜单结构</h1>

配置选项在菜单树结构中的位置可由两种方法决定。第一种方式为：

    menu "Network device support"
    depends on NET
    conf ig NETDEVICES
    …
    endmenu

所有处于"menu"和"endmenu"之间的配置选项都会成为"Network device support"的子菜单，而且，所有子菜单（config）选项都会继承父菜单（menu）的依赖关系，比如，Network device support对NET的依赖会被加到配置选项NETDEVICES的依赖列表中。

> menu 后面跟的“ Network device support”项仅仅是1个菜单，没有对应真实的配置选项，也不具备3 种不同的状态。这是它和config 的区别。

<h1 id="3">3 Makefile</h1>

这里主要对内核源代码各级子目录中的kbuild（内核的编译系统）Makefile 进行简单介绍，这部分是内核模块或设备驱动开发者最常接触到的。

Makefile 的语法包括如下几个方面。

1. 目标定义

    目标定义就是用来定义哪些内容要作为模块编译，哪些要编译并链接进内核。

    例如：

        obj-y += foo.o

    表示要由foo.c 或者foo.s 文件编译得到foo.o 并链接进内核（y表示无条件编译，所以不需要Kconfig 配置选项），而obj-m 则表示该文件要作为模块编译。obj-n 形式的目标不会被编译。

    更常见的做法是根据make menuconfig 后生成的config文件的CONFIG_前缀变量来决定文件的编译方式，如：

        obj-$(CONFIG_ISDN) += isdn.o

2. 多文件模块

    最简单的Makefile 仅需一行代码就够了。如果一个模块由多个文件组成，会稍微复杂一些，这时候应采用模块名加-y 或-objs 后缀的形式来定义模块的组成文件，如下：

        #
        # Makef ile for the linux ext2-f ilesystem routines.
        #
        obj-$(CONF iG_EXT2_FS) += ext2.o
        ext2-y := balloc.o dir.o f ile.o fsync.o ialloc.o inode.o \
                ioctl.o namei.o super.o symlink.o
        ext2-$(CONF iG_EXT2_FS_XATTR) += xattr.o xattr_user.o xattr_trusted.o
        ext2-$(CONF iG_EXT2_FS_POSIX_ACL) += acl.o
        ext2-$(CONF iG_EXT2_FS_SECURITY) += xattr_security.o
        ext2-$(CONF iG_EXT2_FS_XIP) += xip.o

    模块的名字为ext2，由balloc.o、dir.o、file.o 等多个目标文件最终链接生成ext2.o 直至ext2.ko 文件，并且是否包括xattr.o、acl.o 等则取决于内核配置文件的配置情况，例如，如果CONFIG_ EXT2_FS_POSIX_ACL 被选择，则编译acl.c 得到acl.o 并最终链接进ext2。

3. 目录层次的迭代

    如下例：

        obj-$(CONF iG_EXT2_FS) += ext2/

    当CONFIG_EXT2_FS 的值为y 或m 时，kbuild 将会把ext2 目录列入向下迭代的目标中。

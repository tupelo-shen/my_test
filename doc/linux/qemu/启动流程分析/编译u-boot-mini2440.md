# 0 主机环境

    Linux version 4.10.0-42-generic (buildd@lgw01-amd64-051) (gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.5) ) #46~16.04.1-Ubuntu SMP Mon Dec 4 15:57:59 UTC 2017

# 1 交叉编译工具

安装：

    sudo apt-get install gcc-arm-linux-gnueabi

这样安装后，交叉编译工具是最新版的，在编译较低内核版本时，发现遇到问题，所以，这儿可以尽量安装较低版本的交叉编译工具，比如4.7。当然，如果已经安装了最新版的交叉编译工具，可以按照后面一节，把版本降低即可解决。


# 2. 下载

下载命令：

    git clone  git://repo.or.cz/w/u-boot-openmoko/mini2440.git  uboot

或者打包下载

    wget http://repo.or.cz/w/u-boot-openmoko/mini2440.git/snapshot/HEAD.tar.gz


# 3. 编译

解压，并修改其名称为u-boot-for-mini2440：

    tar -xf HEAD.tar.gz
    mv [解压后的目录名称] u-boot-for-mini2440

进入解压后的文件目录

    cd u-boot-for-mini2440

配置Makefile文件，打开Makefile文件，CROSS_COMPILE变量赋值，即自己所使用的交叉编译工具链，比如我的是arm -linux-gnueabi-，保存退出，输入

    #  make mini2440_config
    #  make  -j4

或者直接输入下面的命令进行编译：

    #  make mini2440_config
    #  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -j4

稍等两分钟，即在当前目录下生成名为 *u-boot.bin* 的文件，注意如果想在之后使用u-boot的nfs下载文件功能，需要修改代码中的一部分， 将 *net/nfs.c* 文件中的 *NFS_TIMEOUT = 2UL* 修改为 *NFS_TIMEOUT = 20000UL* 否则会造成nfs文件下载失败， 如果不使用nfs下载功能，不改也可。

编译成功后：

    arm-linux-gnueabi-objcopy --gap-fill=0xff -O srec u-boot u-boot.srec
    arm-linux-gnueabi-objcopy --gap-fill=0xff -O binary u-boot u-boot.bin
    dd if=u-boot.bin of=u-boot-nand2k.bin bs=2K conv=sync
    115+1 records in
    116+0 records out
    237568 bytes (238 kB, 232 KiB) copied, 0.000626312 s, 379 MB/s
    dd if=u-boot.bin of=u-boot-nand16k.bin bs=16K conv=sync
    14+1 records in
    15+0 records out
    245760 bytes (246 kB, 240 KiB) copied, 0.00182879 s, 134 MB/s
    yokogawa@yokogawa-VirtualBox:~/qemu/u-boot-for-mini2440$

# 4. 遇到的问题

mkimage编译错误，报错内容：

    undefined reference to `image_print_contents_noindent`

或者编译其它内容时，也发现：

    undefined reference to `show_boot_progress`:

发现上述出现错误的地方都是inline函数，而且声明和定义在不同的文件，且前面都有inline关键字。觉得问题可能出在inline关键字的使用上，于是上网搜索资料后，发现一篇文章，分析的挺好，[《C语言inline详细讲解》](http://www.cnblogs.com/xkfz007/articles/2370640.html)其中一段话，是这样说的：

    本文介绍了GCC和C99标准中inline使用上的不同之处。inline属性在使用的时候，要注意以下两点：

    inline关键字在GCC参考文档中仅有对其使用在函数定义（Definition）上的描述，而没有提到其是否能用于函数声明（Declare）。

    从 inline的作用来看，其放置于函数声明中应当也是毫无作用的：inline只会影响函数在translation unit（可以简单理解为C源码文件）内的编译行为，只要超出了这个范围inline属性就没有任何作用了。所以inline关键字不应该出现在函数声明 中，没有任何作用不说，有时还可能造成编译错误（在包含了sys/compiler.h的情况下，声明中出现inline关键字的部分通常无法编译通 过）；
    inline关键字仅仅是建议编译器做内联展开处理，而不是强制。在gcc编译器中，如果编译优化设置为-O0，即使是inline函数也不会被内联展开，除非设置了强制内联（__attribute__((always_inline))）属性。

所以，将上面出错的地方的定义处，把inline关键字去掉，然后重新编译，通过。


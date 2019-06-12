使用gdb不仅可以很好地调试代码，也可以利用它来动态地分析代码。使用gdb调试QEMU需要做一些准备工作：

1. 编译QEMU时需要在执行configure脚本时的参数中加入–enable-debug。
2. 从QEMU官方网站上下载一个精简的镜像——*linux-0.2.img*。 *linux-0.2.img*只有8MB大小，启动后包含一些常用的shell命令，用于QEMU的测试。

        使用下面的命令，进行下载：

        $wget http://wiki.qemu.org/download/linux-0.2.img.bz2
        $bzip2 -d ./linux-0.2.img.bz2

3. 启动gdb调试QEMU：

        gdb --args qemu-system-x86_64 -enable-kvm -m 4096 -smp 4 linux-0.2.img

    -smp： 指定处理器个数

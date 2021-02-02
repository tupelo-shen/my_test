# TFTP

TFTP引导启动方式：

    ifaddr syn0 172.26.47.110;boot -r -o 0x80200000 -e 0x80200200 tftp://172.26.47.9/kernel.bin
    ifaddr syn0 192.168.0.110;boot -r -o 0x80200000 -e 0x80200200 tftp://192.168.0.2/kernel.bin
    ifaddr syn0 128.0.0.10;boot -r -o 0x80200000 -e 0x80200200 tftp://128.0.0.80/kernel.bin
    ifaddr syn0 128.0.32.10;boot -r -o 0x80200000 -e 0x80200200 tftp://128.0.32.80/kernel.bin
    ifaddr syn0 128.0.7.12;boot -r -o 0x80200000 -e 0x80200200 tftp://128.0.7.80/kernel.bin
    ifaddr syn0 128.0.7.140;boot -r -o 0x80200000 -e 0x80200200 tftp://128.0.7.80/kernel.bin
    ifaddr syn0 192.168.0.110;boot -r -o 0x40200000 -e 0x40200200 tftp://192.168.0.2/kernel.bin
    ifaddr syn0 192.168.0.110;boot -r -o 0x80200000 -e 0x80200200 tftp://192.168.0.2/kernel_lilina_20200417.bin
    mipsel-linux-objdump -D -b binary -m mips -EB rtscode.a>test1.txt
    mipsel-linux-gcc -g -c -o  s_cos.o s_cos.c #-g 源代码留在汇编代码中
    mipsel-linux-objdump -S -d s_cos.o > s_cos.S
    objdump -D -b binary -m mips xxx.bin > xxx.dis
    mipsel-linux-objdump -D -m mips -b binary -EL -M no-aliases test.bin
    
    ifconfig syn0  128.0.7.140;  boot -r -o 0x10200000 -e 0x10200200 tftp://128.0.7.80/kernel.bin
    ifconfig syn0  128.0.7.12;  boot -r -o 0x10200000 -e 0x10200200 tftp://128.0.7.80/kernel.bin

    KER____Hello World____2020_0403_TFS____
    KER____Hello World____2019_1115_TFS____SleepOK_1203.E

    /*****************************************************************************
     * 函数名称: ls2k_gpio_int_en
     * 作    者: shenwanjiang   
     * 功能描述: 使能GPIO中断
     * 参    数: gpio,  中断对应的管脚
     * 返 回 值: void
     *  
     * 修改日期:         修改人          修改内容
     *
     *****************************************************************************/

#### 1. 运行ejtag软件，连接调试器

    sudo ./ejtag_debug_usb -t  

#### 2. 测试ejtag是否正常
    
    jtagregs d4 1 1

返回结果为`5A5A5A5A`，ejtag正常。

#### 3. 测试龙芯芯片是否正常

    set

返回的寄存器非0，则说明龙芯正常；

#### 4. 设置配置

    source configs/config.ls2k

#### 5. download pmon 可执行程序

    program_cachelock /home/ubuntu/pmon-loongson3/zloader/gzrom.bin     # 不能使用~代替home目录
    或者
    program_cachelock       # 默认tmp目录，所以需要把bin文件拷贝到该目录下。

#### 6. 查看协处理器1

    cp0s 1 d4 16 1
    
    00000010: fe6b3593 

#### 7. 主板断电上电，重启ejtag

    jtagled trst:0 trst:1

#### 8. pmon在线更新

    ifaddr syn0 192.168.0.110; load -r -f 0xbfc00000  tftp://192.168.0.2/gzrom.bin
    ifaddr syn0 172.26.47.110; load -r -f 0xbfc00000  tftp://172.26.47.9/gzrom.bin
    ifaddr syn0 192.168.0.110; load tftp://192.168.0.2/vmlinuz; initrd tftp://192.168.0.2/rootfs.cpio.gz
    g console=tty console=ttyS0,115200
    ifaddr syn0 128.0.7.110; load -r -f 0xbfc00000  tftp://128.0.7.80/gzrom.bin
    unset FR

#### 9. 退出

    exit

#### 10. 编译pmon
    
    cd zloader.ls2k
    make cfg all tgt=rom ARCH=mips CROSS_COMPILE=mipsel-linux- DEBUG=-g
    cp gzrom.bin /tmp/

#### 11. pmon最新源码地址

```
http://ftp.loongnix.org/embedd/ls2k/pmon-loongson3.tar.gz
```

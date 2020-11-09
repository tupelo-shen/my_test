[TOC]

# 1 基本概念

`LVM（Logical Volume Manager）`：首先是基于实际的物理磁盘创建LVM分区，并创建物理卷PV。一个或多个物理卷可以用来创建卷组VG。然后基于卷组可以创建逻辑卷LV。只要在卷组中有可用空间，就可以随心所欲的创建逻辑卷。文件系统就是在逻辑卷上创建的，然后可以在操作系统中挂载和使用。

* 物理长度PE（`Physical Extent`）：物理长度是将物理卷组合为卷组后，所划分的最小存储单位，即逻辑意义上磁盘的最小存储单位。LVM默认PE大小为4MB。

* 物理卷PV（`Physical Volume`）：物理卷是LVM的最底层概念，是LVM的逻辑存储块，物理卷与磁盘分区是逻辑的对应关系。

* 卷组VG（`Volume Group`）：卷组是LVM逻辑概念上的磁盘设备，通过将单个或多个物理卷组合后生成卷组。卷组的大小取决于物理卷的容量以及个数。

* 逻辑卷LV（`Logical Volume`）：逻辑卷就是LVM逻辑意义上的分区，可以指定从卷组中提取多少容量来创建逻辑卷，最后对逻辑卷格式化并挂载使用。

# 2 演示环境

新增一块20G的硬盘，分区为`/dev/sdb1`，大小为`10G`，分区ID为8e：

常用fdisk交互式指令的说明

| 指令 | 说明 |
| ---- | ---- |
| d | 删除分区 |
| l | 显示分区类型 |
| m | 显示帮助菜单 |
| n | 新建分区 |
| p | 显示分区表信息 |
| q | 不保存退出 |
| t | 修改分区ID，可以通过l查看ID |
| w | 保存并退出 |

创建过程如下：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_1.png">

使分区生效：

    # partx -a /dev/sdb

查看分区表：

    # cat /proc/partitions
    或
    # fdisk -l /dev/sdb

# 3 PV/VG/LV相关的常用命令

| 名称 | 物理卷PV | 卷组VG | 逻辑卷LV |
| ---- | -------- | ------ | -------- |
| 创建 | pvcreate | vgcreate | lvcreate |
| 显示 | pvs\|pvscan\|pvdisplay | vgs\|vgscan\|vgdisplay | lvs\|lvscan\|lvdisplay |
| 扩容 |          | vgextend | lvextend|lvresize |
| 缩减 |          | vgreduce | lvreduce|lvresize |
| 删除 | pvremove | vgremove | lvremove |

#4 创建LV

1. 将物理磁盘初始化为物理卷PV：

        # pvcreate /dev/sdb1

2. 创建卷组VG，将PV加入到VG中：

        # vgcreate vgtest1 /dev/sdb1

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_2.png">

3. 基于VG创建逻辑卷LV：

        # lvcreate [选项] 卷组名

    选项：

    -l：指定使用多少个VG中的LE创建LV，[%{VG|PVS|FREE}]

    -L：指定LV大小

    -n：指定LV名称

    -p：指定权限，r为只读，rw为读写

    示例：

        # lvcreate -n lvtest1 -L 5G vgtest1
        或
        # lvcreate -n lvtest1 -l 50%FREE vgtest1

    上述两种方法都是创建大小为5G，名为lvtest1的逻辑卷

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_3.png">

4. 为逻辑卷创建文件系统

        # mke2fs -t ext4 /dev/vgtest1/lvtest1 
        或
        # mke2fs -t ext4 /dev/mapper/vgtest1-lvtest1

5. 创建挂载点

        # mkdir -pv /mnt/lvtest1
        # mount /dev/vgtest1/lvtest1 /mnt/lvtest1

6. 创建测试数据

        # cd /mnt/lvtest1
        # cp /etc/fstab .
        # cp /etc/rc.sysinit .
        # dd if=/dev/zero of=test bs=1M count=2048    //创建一个大小为2G的测试数据test
        # cat fstab        //能正常显示文件内容
        # cat rc.sysinit    //能正常显示文件内容

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_4.png">

7. 实现开机自动挂载

        # vim /etc/fstab  -->  /dev/vgtest1/lvtest1  /mnt/lvtest1  ext4defaults  0  0
        # mount-a
        # mount  -->  /dev/mapper/vgtest1-lvtest1 on /mnt/lvtest1type ext4 (rw)
        # cat /etc/mtab  -->  /dev/mapper/vgtest1-lvtest1 /mnt/lvtest1 ext4rw 0 0

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_5.png">

    > 备注：文件/etc/fstab和/etc/mtab的区别
    > 
    > * /etc/fstab：存放的是系统中的文件系统信息，是系统准备装载的。
    > 
    > * etc/mtab：存放的是现在系统已经装载的文件系统，包括操作系统建立的虚拟文件等。

# 5 扩容LV

1. 将之前`/dev/sdb`中再分出一个分区`/dev/sdb2`，大小为`5G`，分区ID为`8e`：`# fdisk /dev/sdb`

2. 将物理磁盘初始化为物理卷PV：`# pvcreate /dev/sdb2`

3. 将PV添加到指定VG中：`# vgextend vgtest1 /dev/sdb2`

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_6.png">

vgtest1的大小增加了5G。

# 6 扩容LV（可在线扩容，无需卸载LV)

1.  确保VG中有足够的可用空间供LV扩容，将lvtest1扩容5G。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_7.png">

2. 扩容物理边界，格式：

        # lvextend [选项] /dev/卷组名/逻辑卷名

    选项：

    -l：指定使用多少个VG中的LE，[%{VG|LV|PVS|FREE|ORIGIN}]

    -L：指定LV大小

    示例：

        # lvextend -L 10G /dev/vgtest1/lvtest1  或
        # lvextend -L +5G /dev/vgtest1/lvtest1  或
        # lvextend -l +50%FREE /dev/vgtest1/lvtest1

    上述三种方法都是将lvtest1扩容5G，大小为10G

3. 扩容文件系统：# resize2fs /dev/vgtest1/lvtest1

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_8.png">

4. 之前/mnt/lvtest1目录中的数据仍然能正常访问。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_9.png">

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_10.png">

    vgtest1的可用大小减少了5G，lvtest1的大小增加了5G。

# 7 缩减LV：

1. 确保缩减后LV的大小要比存储在LV中数据的总和要大，将lvtest1缩减至8G。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_11.png">

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_12.png">

2. 卸载逻辑卷lvtest1：

        # umount /mnt/lvtest1

3. 强制检测文件系统：

        # e2fsck -f /dev/vgtest1/lvtest1

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_13.png">

4. 缩减文件系统：

        # resize2fs /dev/vgtest1/lvtest1 8G

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_14.png">

5. 缩减物理边界：

        # lvreduce -L 8G /dev/vgtest1/lvtest1

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_15.png">

6. 挂载逻辑卷lvtest1使用：

        # mount /dev/vgtest1/lvtest1 /mnt/lvtest1

7. 之前/mnt/lvtest1目录中的数据仍然能正常访问。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_16.png">

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_17.png">

    PV中的/dev/sdb1的可用大小增加了2G，vgtest1的可用大小增加了2G，lvtest1的大小减少了2G。

# 8 缩减VG：

1. 新增一块大小为20G的磁盘，分区/dev/sdc1，大小10G，分区ID为8e，并初始化为PV，加入vgtest1中：

        # fdisk /dev/sdc
        # pvcreate /dev/sdc1
        # vgextend vgtest1 /dev/sdc1

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_18.png">

2. 将/dev/sdb1上的所有PV移动至/dev/sdc1中，然后删除/dev/sdb1，从而达到缩减VG的目的：

        # pvmove /dev/sdb1 /dev/sdc1

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_19.png">

3. 从卷组vgtest1中删除/dev/sdb1：

        # vgreduce vgtest1 /dev/sdb1

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_20.png">

    /dev/sdb1中的PV已经全部移动至/dev/sdc1中，vgtest1的大小也减少了10G。/dev/sdb1可以删除也可以加入别的VG中。

4. 之前/mnt/lvtest1目录中的数据仍然能正常访问。

# 9 删除LV：

1. 将上述的/dev/sdb1加入到新卷组vgtest2中，并创建lvtest2：

        # vgcreate vgtest2 /dev/sdb1
        # lvcreate -n lvtest2 -l 100%FREE vgtest2
        # mke2fs -t ext4 /dev/vgtest2/lvtest2
        # mkdir -pv /mnt/lvtest2
        # mount /dev/vgtest2/lvtest2 /mnt/lvtest2

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_21.png">

2. 卸载lvtest2：

        # umount /mnt/lvtest2

3. 删除lvtest2：

        # lvremove /dev/vgtest2/lvtest2

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_22.png">

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_23.png">

lvtest2已被删除，vgtest2可用大小为10G。

# 10 删除VG

1. 删除vgtest2：

        # vgremove vgtest2

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_24.png">

    vgtest2已经删除。

# 11 删除PV

1. 删除/dev/sdb1：

        # pvremove /dev/sdb1

2. 使用fdisk命令将无用的分区删除，或者将其分区ID从8e修改为83。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_25.png">

    PV中的/dev/sdb1已经删除。

# 12 创建快照卷

1. 为lvtest1创建大小为5G的快照卷lvtest1-snap：

        # lvcreate -s -L 5G -n lvtest1-snap -p r /dev/vgtest1/lvtest1      //-s：创建快照卷，-p r：表示属性为只读

2. 创建快照卷挂载点，挂载快照卷：

        # mkdir -pv /mnt/snap
        # mount /dev/vgtest1/lvtest1-snap /mnt/snap

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_26.png">

> 备注：快照卷是逻辑卷的一种，通常为只读。快照卷与原卷在同一个卷组中，因此创建快照卷之前要确保原卷所在的卷组中有足够空间可用。快照卷无须与原卷一样大，其大小取决于快照卷的存活时长内原卷的数据变化量。

3. 查看lvtest1和lvtest1-snap中的内容：

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_27.png">


    创建快照以后，快照之后所修改的数据，通过快照卷是访问不到的，因为它保存的是过去某一时刻的状态，可以备份或复制该时刻的数据。

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_28.png">

# 13 删除快照卷

1. 卸载快照卷：

        # umount /mnt/snap

2. 删除快照卷：

        # lvremove /dev/vgtest1/lvtest1-snap

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_29.png">

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_30.png">

# 14 其他常用操作

1. 重命名lvtest1为marion：

        # lvrename vgtest1 lvtest1 marion

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_31.png">

2. 重命名vgtest1为keyso：# vgrename vgtest1 keyso

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_32.png">
    
3. 合并卷组：创建一个大小为10G的新卷组vgtest，将vgtest合并到keyso中

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_33.png">

        # vgmerge keyso vgtest

    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/centos_7_lvm_usage_34.png">


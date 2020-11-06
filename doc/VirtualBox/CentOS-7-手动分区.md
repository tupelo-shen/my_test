[TOC]

# 1 添加文件系统和配置分区

安装CentOS-7时，如果选择了手动分区的选项，就会进入下面的界面。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/VirtualBox/images/ddmain.png">

安装CentOS至少需要3个分区：`/`、`/boot`和`swap`。但是，推荐安装4个分区：`/`、`/home`、`/boot`和`swap`。当然，可以根据自己的需求添加其它分区。

如果希望安装程序自动创建文件系统和挂载点，可以在左边的分区方案选择下拉框中，选择想要的分区方案，默认情况下，CentOS使用`LVM`。然后，点击上方的`自动创建挂载点`选项。系统会自动创建`/boot`、`/`和`swap`分区。

另外，可以点击`+`按钮，手动创建挂载点。然后，在弹出的右边面板上，选择新的挂载点，容量，设备类型，文件系统类型，标签和是否需要加密或者重新格式化分区。

需要注意的是，分区方案在下拉框中有`Standard Partition`、`BTRFS`、`LVM`和`LVM Thin Provisioning`四种。而`/boot`分区，无论你怎么选择，总是使用`Standard Partition`方案。

## 1.1 文件系统类型


# 2 创建LVM逻辑卷

# 3 推荐分区方案

## 3.1 分区上的设备

https://docs.centos.org/en-US/centos/install-guide/CustomSpoke-x86/#sect-recommended-partitioning-scheme-x86

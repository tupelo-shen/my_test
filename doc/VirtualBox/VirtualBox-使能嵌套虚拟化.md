本文主要讲述如何在VirtualBox中使能嵌套虚拟化，VirtualBox运行在Linux系统下。嵌套虚拟化允许我们在虚拟机中运行虚拟机。比如，在Virtualbox上运行一个CentOS的虚拟机，如果是能了嵌套VT特性，就可以在CentOS虚拟机中安装virtualbox或KVM，继而再在其上运行其它虚拟机。通俗的讲，就是在虚拟化环境中再搭建一个虚拟化环境。

# 1 使能嵌套虚拟化

从版本6.1开始，VirtualBox开始支持对AMD和Intel CPU的嵌套虚拟化。所以，确保使用的VirtualBox是最新版本。

既可以从命令行使能嵌套虚拟化，也可以通过GUI界面进行设定。首先，让我们看一下命令行方式。

# 2 命令行方式

1. 列出可以使用的虚拟机

        $ vboxmanage list vms

    可能的输出样式：

        "CentOS 8 Server" {73997fc7-4ae2-42bf-a11d-fcbe00721e13}
        "Ubuntu 20.04 Server" {a7cab540-51c2-4110-b489-a4ad13b71f96}

    如你所见，已经创建了两个虚拟机。

2. 为CentOS-8使能嵌套虚拟化

        $ VBoxManage modifyvm "Ubuntu 20.04 Server" --nested-hw-virt on

    也可以使用小写：

        $ vboxmanage modifyvm "Ubuntu 20.04 Server" --nested-hw-virt on

通过命令行方式，使能嵌套虚拟化功能后，就将CPU的硬件虚拟化功能穿透给了guest虚拟机。

上面的虚拟机名称使用了双引号，是因为其中有空格。如果没有空格，可以不加双引号。

# 3 图形化方式

打开VirtualBox管理器，点击想要使能嵌套虚拟化的机器，然后，选中`设置(settings)`。如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/VirtualBox/images/virtual-machine-settings-in-virtualbox_1.png">

选择`系统(System)`->`处理器(Processor)`，选中`使能嵌套VT-x/AMD-V(Enable Nested VT-x/AMD-V)`前面的单选框。点击OK，保存即可。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/VirtualBox/images/Enable-nested-virtualization-in-virtualbox-manager_2.png">

现在，这个虚拟机就可以充当hypervisor的角色，可以在里边运行虚拟机了。

# 4 验证嵌套虚拟化是否使能

在虚拟机中，输入命令：

    $ egrep --color -i "svm|vmx" /proc/cpuinfo

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/VirtualBox/images/Verify-If-nested-virtualization-is-enabled-in-virtualbox-VM.png">

如果你能看见`vmx`（Intel-VT技术）或`svm`（AMD-V技术）的字样，说明virtualbox上的客户机已经可以作为hypervisor工作，托管虚拟机了。

另外，你还可以使用`lscpu`命令

    $ lscpu

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/VirtualBox/images/Find-if-a-CPU-supports-virtualization-using-lscpu-command-in-Linux.png">

出现如图所示的字样即可。

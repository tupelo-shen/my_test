今天在 VirtualBox 中安装了 Ubuntu 作为虚拟机开发环境，途中安装 VirtualBox 提供的增强功能，但是报错，截图如下：

![问题图片](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/VirtualBox/Q%26A/images/Unable_to_mount_VBOXADDITIONS_question.PNG)

这样的问题乍一看很奇怪，文件系统无法识别。其实只需要重新安装一下 linux 映像就行，运行这条命令：

    sudo apt-get install --reinstall linux-image-$(uname -r)

然后重启，再安装，就OK了。
新建的共享文件夹，只有切换成root用户组时才能访问。

其他用户却没有权限，那是因为共享文件夹属于vboxsf组。把当前用户添加到vboxsf组就可以访问了。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/VirtualBox/images/ubuntu_share_folders_permission.png">

命令：  

    sudo adduser  当前用户名 vboxsf 

> 注意：用户组更改需要重启虚拟机，这里需要重启虚拟机，不然设置无效。

重启命令： 

    sudo reboot

这时候，就可以打开共享文件夹了。
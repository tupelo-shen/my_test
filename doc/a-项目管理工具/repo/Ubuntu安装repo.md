[TOC]

# 1 简介

首先简单介绍一下repo是什么：

repo是Android为了方便管理多个git库而开发的Python脚本。repo的出现，并非为了取代git，而是为了让Android开发者更为有效的利用git。

Android源码包含数百个git库，仅仅是下载这么多git库就是一项繁重的任务，所以在下载源码时，Android就引入了repo。 Android官方推荐下载repo的方法是通过Linux curl命令，下载完后，为repo脚本添加可执行权限：

    $ curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
    $ chmod a+x ~/bin/repo

由于国内Google访问受限，所以上述命令不一定能下载成功。其实，我们现在可以从很多第三方渠道找到repo脚本，只需要取下来，确保repo可以正确执行即可。

# 2 替代方法

安装步骤:

1. 根目录下创建.bin文件夹

        mkdir ~/.bin

2. 配置为临时环境变量(也可配置为永久的)

        PATH=~/.bin:$PATH

3. 下载repo

        git clone https://gerrit-googlesource.lug.ustc.edu.cn/git-repo

4. 将git-repo中的repo文件复制到`1`创建的.bin目录中

        cd git-repo
        cp repo ~/.bin/

    值得注意的是，这一步应该使用git clone进行拷贝，否则`.git`隐藏目录无法拷贝过去。如果确实想这样拷贝的话，使用下面的命令

        cp -R 

5. 修改权限

        sudo chmod a+x ~/.bin/repo

6. 修改镜像链接路径：

    我们需要将 `https://gerrit.googlesource.com/git-repo`  替换成中国科技技术大学的镜像链接  `https://mirrors.ustc.edu.cn/aosp/git-repo`。

    这个网址就是在repo文件中定义的。

7. 配置git参数：

        # git config --global user.name "Your Name"
        # git config --global user.email "Your Email"
        # git config --list

    
8. 创建同步源码的工作目录

        mkdir android_source

9. 在工作目录中创建`.repo/repo`目录

        mkdir -p .repo/repo

10. 将下载的git-repo目录下的内容拷贝到新创建的repo目录下。

        cp -r ~/git-repo/* ~/git-repo/.  .repo/repo

11. 初始化仓库

        repo init -u https://github.com/seL4/seL4test-manifest.git

12. 同步资源

        repo sync

# 3 参考文章

1. [Ubuntu安装repo](https://blog.csdn.net/haolask/article/details/102826032)

2. [Linux ubuntu repo安装方法](https://blog.csdn.net/u010117864/article/details/88805136)
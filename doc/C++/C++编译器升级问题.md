##如何在Ubuntu-14.04上安装g++-6.3 ?

ppa:ubuntu-toolchain仅为尝鲜测试版,并非官方稳定版.

以下仅用于学习,不建议进行以下操作,以免出现包依赖关系问题,影响系统稳定性.

[askUbuntu](https://askubuntu.com/questions/618474/how-to-install-the-latest-gcurrently-5-1-in-ubuntucurrently-14-04) 和 [github answer](https://gist.github.com/application2000/73fd6f4bf1be6600a2cf9f56315a2d91)

    #ppa方式安装
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-6 g++-6
    sudo apt-get install gcc-7 g++-7

    #系统自带的是gcc-4.8.4和g++-4.8.4,如果系统自带的是gcc-5.60和g++-5,那么就注释掉gcc-4.8.40这一行
    #sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 30 --slave /usr/bin/g++ g++ /usr/bin/g++-6
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 10 --slave /usr/bin/g++ g++ /usr/bin/g++-7

    sudo update-alternatives --config gcc

附:卸载ppa的方法

    $ sudo add-apt-repository --remove ppa:someppa/ppa

注意，上述命令不会同时删除任何已经安装或更新的软件包。

如果你想要 完整的删除一个PPA仓库并包括来自这个PPA安装或更新过的软件包，你需要ppa-purge命令。

    $ sudo apt-get install ppa-purge

然后使用如下命令删除PPA仓库和与之相关的软件包：

    $ sudo ppa-purge ppa:ppa:someppa/ppa
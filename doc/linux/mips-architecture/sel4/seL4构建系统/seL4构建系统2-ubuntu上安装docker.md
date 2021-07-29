To get started with Docker Engine on Ubuntu, make sure you meet the prerequisites, then install Docker.

## 1 前提条件

#### 1.1 OS要求

64位版本的Ubuntu软件：

* Ubuntu Hirsute 21.04
* Ubuntu Groovy 20.10
* Ubuntu Focal 20.04 (LTS)
* Ubuntu Bionic 18.04 (LTS)

`Docker` 支持`x86_64`、`amd64`、`armhf`和`arm64`架构。

#### 1.2 卸载旧版本

旧版本的`Docker`可能被称为`docker`、`docker.io`或`docker-engine`。如果之前安装过，请先卸载：

    sudo apt-get remove docker docker-engine docker.io containerd runc

It’s OK if apt-get reports that none of these packages are installed.

The contents of `/var/lib/docker/`, including images, containers, volumes, and networks, are preserved. If you do not need to save your existing data, and want to start with a clean installation, refer to the uninstall Docker Engine section at the bottom of this page.

#### 1.3 支持的存储驱动程序

Docker Engine on Ubuntu supports `overlay2`, `aufs` and `btrfs` storage drivers.

Docker Engine uses the `overlay2` storage driver by default. If you need to use `aufs` instead, you need to configure it manually. See [use the AUFS storage driver](https://docs.docker.com/storage/storagedriver/aufs-driver/)

## 2 安装方法

有多种不同的方法安装docker，根据需要自己选择：

* 【推荐方法】建立Docker仓库，然后从仓库中安装。这样安装简单，方便后续维护升级。
* 下载DEB包，手动安装和管理更新软件。这种方法是在离线状态下安装docker的一种有效方法。
* 在测试和开发环境中，有些用户可能使用[自动化脚本](https://docs.docker.com/engine/install/ubuntu/#install-using-the-convenience-script)安装Docker。

#### 2.1 使用仓库安装

在一台新机器上安装Docker时，先建立Docker仓库。然后，从仓库中安装和更新Docker。

###### 2.1.1 建立仓库

1. 更新`apt`包索引，安装必要的包，允许`apt`通过`HTTPS`使用仓库：

        sudo apt-get update
        sudo apt-get install \
            apt-transport-https \
            ca-certificates \
            curl \
            gnupg \
            lsb-release

2. 添加Docker官方GPG密钥:

        curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg


3. 使用以下命令设置`stable`存储库。要添加`nightly`存储库或`test`存储库，请在下面命令中的单词`stable`后面添加单词`nightly`或`test`(或两者都是)。

    * x86_64/amd64机器：

            echo \
            "deb [arch=amd64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
            $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null


    * armhf机器：

            echo \
            "deb [arch=armhf signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
            $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

    * arm64机器：

            echo \
            "deb [arch=arm64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
            $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

    > 注意：`lsb_release -cs`返回`Ubuntu`发布版本的名称，比如`xenial`。但是，有时候，有些发布版本比如`Linux Mint`之类，你可能需要修改`$(lsb_release -cs)`为父版本的Ubuntu发行版。比如，正在使用`Linux Mint Tessa`，可以使用`bionic`。Docker对于任何非正式版的Ubuntu不提供任何保证。

###### 2.1.2 安装Docker

1. 安装最新的`Docker`和`containerd`：

        sudo apt-get update
        sudo apt-get install docker-ce docker-ce-cli containerd.io

    > 如果有多个版本的Docker仓库，没有在安装命令中指定版本，默认就是最高版本的仓库。

2. 安装指定版本的`Docker`:

    * 列出仓库中所有可用的版本：

            apt-cache madison docker-ce

    * 安装指定版本的Docker，比如`5:18.09.1~3-0~ubuntu-xenial`:

            sudo apt-get install docker-ce=<VERSION_STRING> docker-ce-cli=<VERSION_STRING> containerd.io

3. 通过运行`hello-world`镜像来验证`Docker`是否正确安装。

        sudo docker run hello-world

    This command downloads a test image and runs it in a container. When the container runs, it prints an informational message and exits.

Docker Engine is installed and running. The `docker` group is created but no users are added to it. You need to use `sudo` to run Docker commands. Continue to [Linux postinstall](https://docs.docker.com/engine/install/linux-postinstall/) to allow non-privileged users to run Docker commands and for other optional configuration steps.

###### 2.1.3 更新Docker

运行：`sudo apt-get update`，重新安装最新版本。


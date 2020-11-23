本文档主要介绍如何快速搭建机器，构建seL4微内核和相关工程。

# 1 要求

首先安装下面两个工具：

* make

        sudo apt-get install make

* docker

        docker的安装，请参考[https://docs.docker.com/engine/installation](https://docs.docker.com/engine/installation)

为了方便，需要将当前用户添加到docker组中，命令如下：

    sudo usermod -aG docker $(whoami)
    newgrp docker #更新docker用户组

# 2 获取构建环境

获取资源：

    git clone https://github.com/SEL4PROJ/seL4-CAmkES-L4v-dockerfiles.git
    cd seL4-CAmkES-L4v-dockerfiles
    make user

This will give you a terminal inside a container that has all the relevant tools to build, simulate, and test seL4 & Camkes programs.

The first time you run this, docker will fetch the relevant images, which may take a while.

To map a particular directory into the container:

    make user HOST_DIR=/scratch/seL4_stuff  # as an example
    # Now /host in the container maps to /scratch/seL4_stuff

To make this easier to type, you can setup a bash alias such as this:

    echo $'alias container=\'make -C /<path>/<to>/seL4-CAmkES-L4v-dockerfiles user HOST_DIR=$(pwd)\'' >> ~/.bashrc
    # now open a new terminal, or run `source ~/.bashrc`

    echo $'alias container=\'make -C /home/sel4/seL4-CAmkES-L4v-dockerfiles user HOST_DIR=$(pwd)\'' >> ~/.bashrc

Replace `/<path>/<to>/` to match where you cloned the git repo of the docker files. This then allows you to run:

    container

to start the container in the current directory you are in.

# An example workflow:

A good workflow is to run two terminals:

* terminal A is just a normal terminal, and is used for git operations, editing (e.g., vim, emacs), and other normal operations.

* terminal B is running in a container, and is only used for compilation.

This gives you the flexibility to use all the normal tools you are used to, while having the seL4 dependencies separated from your machine.

# 3 Compiling seL4 test

Start two terminals (terminal A and terminal B).

In terminal A, run these commands:

    jblogs@host:~$ mkdir ~/seL4test
    jblogs@host:~$ cd ~/seL4test
    jblogs@host:~/seL4test$ repo init -u https://github.com/seL4/seL4test-manifest.git
    jblogs@host:~/seL4test$ repo sync

In terminal B, run these commands:

    jblogs@host:~$ cd ~/seL4test
    jblogs@host:~/seL4test$ container  # using the bash alias defined above
    jblogs@in-container:/host$ mkdir build-x86
    jblogs@in-container:/host$ cd build-x86
    jblogs@in-container:/host/build-x86$ ../init-build.sh -DPLATFORM=x86_64 -DSIMULATION=TRUE 
    jblogs@in-container:/host/build-x86$ ninja

If you need to make any code modifications or commit things to git, use terminal A. If you need to recompile or simulate an image, use terminal B.







## What is this?

这个仓库包含dockerfile文件，它映射了seL4、CAmkES和L4v的依赖关系。它还包含一些基础设施，方便使用容器。

这些可以作为可信系统用作回归测试的基本构件，因此，可以代表一个经过良好测试的最新环境：

## To run

从github上获取最新的dockfiles文件资源：

    git clone https://github.com/SEL4PROJ/seL4-CAmkES-L4v-dockerfiles.git
    cd seL4-CAmkES-L4v-dockerfiles

运行下面的命令可以获取在容器内的运行环境：

    make user

这会给你带来一个带有`CAmkES`依赖项的终端。你还可以使用下面的命令，分别指定：

    make user_sel4
    make user_camkes  # alias for 'make user'
    make user_l4v

容器将当前的工作目录从本地主机映射到容器中的/host目录下。你可以在其中读写文件。

如果想要映射到不同的文件夹下，可以使用下面的命令进行指定：

    make user_sel4 HOST_DIR=/scratch/sel4_stuff

你也可以使用'EXEC'命令指定要在容器内执行的命令：

    make user EXEC="bash -c 'echo hello world'"

如果你的机器中没有这些镜像，则将从DockerHub中pull镜像。

另外，你也可以在你的`bashrc`中定义一个bash函数，如下所示：

    function container() {
        if [[ $# > 0 ]]; then
            make -C /<path>/<to>/seL4-CAmkES-L4v-dockerfiles user HOST_DIR=$(pwd) EXEC="bash -c '""$@""'"
        else
            make -C /<path>/<to>/seL4-CAmkES-L4v-dockerfiles user HOST_DIR=$(pwd)
        fi
    }

将上面的`/<path>/<to>/`字符串替换成你下载的相关docker文件的git仓库。就可以使用下面的命令，

    container

直接从当前目录下进入container，或者使用下面的命令：

    container "echo hello && echo world"

直接从当前目录下进入container，并打印`hello world`。

### 编译seL4测试工程的示例

创建一个新目录`sel4test`:

    mkdir ~/sel4test

建立容器：

    make user HOST_DIR=~/sel4test
    # in-container terminal
    jblogs@in-container:/host$ 

获取seL4测试代码：

    jblogs@in-container:/host$ repo init -u https://github.com/seL4/sel4test-manifest.git
    jblogs@in-container:/host$ repo sync
    jblogs@in-container:/host$ ls
    apps configs Kbuild Kconfig kernel libs Makefile projects tools

编译和模拟seL4测试（x86-64版本）：

    jblogs@in-container:/host$ mkdir build-x86
    jblogs@in-container:/host$ cd build-x86
    jblogs@in-container:/host$ ../init-build.sh -DPLATFORM=x86_64 -DSIMULATION=TRUE
    jblogs@in-container:/host$ ninja
    # ... time passes...
    jblogs@in-container:/host$ ./simulate
    ...
    Test VSPACE0002 passed

        </testcase>

        <testcase classname="sel4test" name="Test all tests ran">

        </testcase>

    </testsuite>

    All is well in the universe


## 添加依赖

The images and dockerfiles for seL4/CAmkES/L4v only specify enough dependencies to pass the tests in the \*tests.dockerfile. The `extras.dockerfile` acts as as him between the DockerHub images and the `user.dockerfile`.

为`seL4/CAmkES/L4v`添加的镜像和docker文件，只是为tests.dockerfile中的测试指定了足够的依赖。

Adding dependencies into the `extras.dockerfile` will build them the next time you run `make user`, and then be cached from then on.

添加到`extras.dockerfile`中的依赖，当你下次运行`make user`时，构建它们并缓存。


## To build the local dockerfiles

To build the Dockerfiles locally, you will need to use the included `build.sh` script. It has a help menu:

    ./build.sh -h
        build.sh [-r] -b [sel4|camkes|l4v] -s [binary_decomp|cakeml|camkes_vis|cogent|riscv|rust|sysinit|] -s ... -e MAKE_CACHES=no -e ...
    
         -r     重新构建docker镜像（不使用docker缓存）
         -v     详细模式
         -s     标准模式
         -e     docker构建的构建参数(NAME=VALUE)。对每个构建参数使用-e。
         -p     首先，拉取基本镜像。不构建基础镜像，从网上获取。
    
        Sneaky hints:
         - To build 'prebuilt' images, you can run:
               build.sh -b [riscv|cakeml]
           but it will take a while!
         - You can actually run this with '-b sel4-rust', or any other existing image,
           but it will ruin the sorting of the name.

### 构建示例

编译seL4镜像：

    `./build.sh -b sel4`

Note that the `-b` flag stands for the `base image`. There are 3 base images: `sel4`, `camkes`, and `l4v`. Each base image includes the previous one, i.e.: the `camkes` image has everything the `sel4` image has, plus the camkes dependencies .

To add additional software to the image, you can use the `-s` flag, to add `software`. For example:

    `./build.sh -b sel4 -s riscv  # This adds the RISCV compilers`

    `./build.sh -b sel4 -s riscv -s rust  # This adds the RISCV compilers and a rust compiler`

You can also pass configuration variables through to docker (in docker terms, these are `BUILD_ARGS`) by using the `-e` flag. For example, you can tell the seL4 image to use GCC version 6 (instead of 8 ) by running:

    `./build.sh -b sel4 -e GCC_V8_AS_DEFAULT=no`

Or to turn off priming the build caches:

    `./build.sh -b sel4 -e MAKE_CACHES=no`

Or combined:

    `./build.sh -b sel4 -e GCC_V8_AS_DEFAULT=no -e MAKE_CACHES=no`

To speed things up, you can ask to pull the base image from DockerHub first with the `-p` flag:

    `./build.sh -p -b sel4 -s riscv  # This adds the RISCV compilers`



## Security

Running Docker on your machine has its own security risks which you should be aware of. Be sure to read the Docker documentation.

Of particular note in this case, your UID and GID are being baked into an image. Any other user on the host who is part of the docker group could spawn a separa te container of this image, and hence have read and write access to your files. Of course, if they are part of the docker group, they could do this anyway, but it just makes it a bit easier.

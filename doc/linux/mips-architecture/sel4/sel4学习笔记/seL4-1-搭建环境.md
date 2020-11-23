[TOC]

# 1 主机依赖

This page describes how to set up your host machine to build and run seL4 and its supported projects. To compile and use seL4 you can either:

* 推荐使用Docker构建seL4，Camkes和L4v：具体请参考[https://docs.sel4.systems/projects/dockerfiles/](https://docs.sel4.systems/projects/dockerfiles/).

* 直接在主机上安装。

# 2 获取Google的Repo 工具

The primary way of obtaining and managing seL4 project source is through the use of Google’s repo tool. To get repo, follow the instructions described in the section “Installing Repo” [here](https://source.android.com/setup/develop#installing-repo)

See the [RepoCheatsheet](https://docs.sel4.systems/projects/buildsystem/repo-cheatsheet) page for a quick explanation of how we use Repo.

# 3 seL4构建所需依赖

构建基于seL4的工程，需要下面两种依赖项：

## 3.1 基本的Build 依赖项

为了构建一个可用的开发环境，基于使用的系统安装基本的build包是很重要的。

### 3.1.1 Ubuntu安装方法

> The following instructions cover the build dependencies tested on Ubuntu 18.04 LTS. Note that earlier versions of Ubuntu (e.g. 16.04) may not be sufficient for building as some default development packages are stuck at older versions (e.g CMake 3.5.1, GCC 5.4 for 16.04). As dependencies and packages may be frequently changed, deprecated or updated these instructions may become out of date. If you discover any missing dependencies and packages we welcome new [contributions](https://docs.sel4.systems/DocsContributing) to the page.

> Note that we require a minimum CMake version of 3.12.0 while Ubuntu 18.04 contains 3.10.2. In order to correct this, a custom installation of CMake may be required which can be downloaded from: https://cmake.org/download/

Ubuntu上基本的build包称为`build-essential`。运行下面的命令可以安装：

    sudo apt-get update
    sudo apt-get install build-essential

为了在Ubuntu上构建seL4工程还需要安装下面这些基本的依赖项：

    sudo apt-get install cmake ccache ninja-build cmake-curses-gui
    sudo apt-get install python-dev python-pip python3-dev python3-pip
    sudo apt-get install libxml2-utils ncurses-dev
    sudo apt-get install curl git doxygen device-tree-compiler
    sudo apt-get install u-boot-tools
    sudo apt-get install protobuf-compiler python-protobuf

为了构建ARM上运行的目标程序还需要安装交叉编译工具链。另外，为了在模拟器上运行seL4工程，还需要安装一个Qemu虚拟机程序。命令如下所示：

    sudo apt-get install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi
    sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
    sudo apt-get install qemu-system-arm qemu-system-x86 qemu-system-misc

如果你对浮点数感兴趣，还可以安装支持浮点数编译的交叉编译工具：

    sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

## 3.2 Python依赖项

为了构建seL4、手册和功能安全验证，还需要安装python依赖项。可以运行下面的命令：

    pip3 install --user setuptools
    pip3 install --user sel4-deps
    # Currently we duplicate dependencies for python2 and python3 as a python3 upgrade is in process
    pip install --user setuptools
    pip install --user sel4-deps

有些系统python3使用pip，python2使用pip2；而有些系统python3使用pip3，python2使用pip。

# 4 CAmkES Build Dependencies

To build a CAmkES based project on seL4, additional dependencies need to be installed on your host machine. Projects using CAmkES (the seL4 component system) need Haskell and some extra python libraries in addition to the standard build tools. The following instructions cover the CAmkES build dependencies for Ubuntu/Debian. Please ensure you have installed the dependencies listed in sections [sel4 Build Dependencies](https://docs.sel4.systems/projects/buildsystem/host-dependencies.html#sel4-build-dependencies) and [Get Google’s Repo tool](https://docs.sel4.systems/projects/buildsystem/host-dependencies.html#get-googles-repo-tool) prior to building a CAmkES project.

## 4.1 Python Dependencies

The python dependencies required by the CAmkES build toolchain can be installed via pip:

    pip3 install --user camkes-deps
    # Currently we duplicate dependencies for python2 and python3 as a python3 upgrade is in process
    pip install --user camkes-deps

## 4.2 Haskell Dependencies

The CAmkES build toolchain additionally requires Haskell. You can install the [Haskell stack](https://haskellstack.org/) on your distribution by running:

    curl -sSL https://get.haskellstack.org/ | sh

If you prefer not to bypass your distribution’s package manager, you can do

    sudo apt-get install haskell-stack

## 4.3 Build Dependencies

### 4.3.1 Ubuntu

> Tested on Ubuntu 18.04 LTS

Install the following packages on your Ubuntu machine:

    sudo apt-get install clang gdb
    sudo apt-get install libssl-dev libclang-dev libcunit1-dev libsqlite3-dev
    sudo apt-get install qemu-kvm

### 4.3.2 Debian

For Debian Stretch or later

The dependencies listed in our docker files repository will work for a Debian installation. You can refer to this repository for an up-to-date list of base build dependencies. Specifically refer to the dependencies listed in the:

* [CAmkES Dockerfile](https://github.com/SEL4PROJ/seL4-CAmkES-L4v-dockerfiles/blob/master/camkes.dockerfile)

# 5 Building Proofs (l4v dependencies)

The proofs in the [L4.verified](https://github.com/seL4/l4v) repository use `Isabelle2017`. To best way to make sure you have all the dependencies on your host machine is to follow the instructions listed in sections [Base Build Dependencies](https://docs.sel4.systems/projects/buildsystem/host-dependencies.html#base-build-dependencies). However you can get away with avoiding a full cross compiler setup. The dependencies for Isabelle you will need at least are listed below:

## 5.1 Ubuntu安装方法

> Tested on Ubuntu 18.04 LTS

    sudo apt-get install libwww-perl libxml2-dev libxslt-dev
    sudo apt-get install mlton rsync
    sudo apt-get install texlive-fonts-recommended texlive-latex-extra texlive-metapost texlive-bibtex-extra

## 5.2 Debian安装方法

> For Debian Stretch or later

The dependencies listed in our docker files repository will work for a Debian installation. You can refer to this repository for an up-to-date list of base build dependencies. Specifically refer to the dependencies listed in the:

* [l4v Dockerfile](https://github.com/SEL4PROJ/seL4-CAmkES-L4v-dockerfiles/blob/master/l4v.dockerfile)

## 5.3 Haskell Dependencies

The Haskell tool-stack is required to build the Haskell kernel model. You can install [Haskell stack](https://haskellstack.org/) on your distribution by running:

    curl -sSL https://get.haskellstack.org/ | sh

If you prefer not to bypass your distribution’s package manager, you can do

    sudo apt-get install haskell-stack

## 5.4 Setup Isabelle

To setup Isabelle, you will firstly need to pull the [L4.verified](https://github.com/seL4/l4v) project source using the Google repo tool (installing the [Google repo tool](https://docs.sel4.systems/projects/buildsystem/host-dependencies.html#get-googles-repo-tool) is described in the Get Google’s Repo tool section. The following steps to setup Isabelle:

    # Create a directory to store the project source
    mkdir lv4_repo
    cd lv4_repo
    # Initialise the project repo
    repo init -u https://github.com/seL4/verification-manifest.git
    repo sync
    cd l4v
    mkdir -p ~/.isabelle/etc
    cp -i misc/etc/settings ~/.isabelle/etc/settings
    ./isabelle/bin/isabelle components -a
    ./isabelle/bin/isabelle jedit -bf
    ./isabelle/bin/isabelle build -bv HOL-Word

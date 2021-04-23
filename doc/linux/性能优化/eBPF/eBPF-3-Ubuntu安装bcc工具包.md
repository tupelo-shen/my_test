[TOC]

目前安装`bcc`有两种方式，一种是直接使用发行版提供的软件包，`Ubuntu`里叫`bpfcc-tools`，`CentOS7`中的是`bcc-tools`。另一种方式是源码编译安装。推荐通过源码编译安装。

> 第一种和第二种方式只能二选一，否则会有冲突导致不可用

有人反应第一种方式安装bcc后，BPF模块各种出错。目前通过源码编译安装是最稳定最安全的方法。下面将详细介绍通过源码编译安装`bcc`。

## 1 踩坑指南

* Linux发行版最好用Ubuntu，不要用CentOS7
* 尽量不要用曾经手动升级过内核的系统，用原生的发行版系统
* 命令安装与源码安装只可二选一，否则可能导致不可用
* 不要直接clone官方仓库，编译会缺文件，使用bcc的release包。
* 官方要求的依赖缺少对python3附加模块的支持，需要自己手动添加

## 2 源码编译安装`bcc`

#### 2.1 检查环境(特别高版本内核可以忽略此步)

**内核配置**：高版本的内核这些是标配，基本不用管，不放心也可以检查下。通过命令

```shell
less /boot/config-<kernel-version>
```

配置选项:

```shell
CONFIG_BPF=y
CONFIG_BPF_SYSCALL=y
# [optional, for tc filters]
CONFIG_NET_CLS_BPF=m
# [optional, for tc actions]
CONFIG_NET_ACT_BPF=m
CONFIG_BPF_JIT=y
# [for Linux kernel versions 4.1 through 4.6]
CONFIG_HAVE_BPF_JIT=y
# [for Linux kernel versions 4.7 and later]
CONFIG_HAVE_EBPF_JIT=y
# [optional, for kprobes]
CONFIG_BPF_EVENTS=y
CONFIG_NET_SCH_SFQ=m
CONFIG_NET_ACT_POLICE=m
CONFIG_NET_ACT_GACT=m
CONFIG_DUMMY=m
CONFIG_VXLAN=m
```

**构建工具**：这些是构建依赖的工具，和它们最低的版本要求(后面会安装，这里用于查看版本)

1. LLVM 3.7.1或者更新，会和BPF一起编译（默认选择）；
2. Clang，同LLVM；
3. cmake(>=3.1)，gcc(>=4.7)，flex，bison；
4. LuaJIT（支持Lua语言的时候需要）。

#### 2.2 正式安装

先安装所有依赖的工具

```shell
sudo apt-get -y install bison build-essential cmake flex git libedit-dev \
  libllvm6.0 llvm-6.0-dev libclang-6.0-dev python zlib1g-dev libelf-dev
```

除了官网要求的这些工具之外，还要额外安装几个python3的包:

```shell
sudo apt-get install python3-pip
```

> 虽然官网没有要求安装，但我在实际编译过程中发生编译中断，通过查资料发现是缺少python3的依赖包。Python基本解释器确实需要一些附加模块，这些未默认安装在Ubuntu 18.04

接下来在主目录新建一个文件夹，用来放bcc源码

```shell
mkdir ebpf; cd ebpf
```

下载bcc的release包：[点击下载](https://github.com/iovisor/bcc/releases)。选择`bcc-src-with-submodule.tar.gz`，然后解压。剩下的只要依次执行下列命令就可以安装成功了。

```shell
mkdir bcc/build; cd bcc/build
cmake ..
make
sudo make install
cmake -DPYTHON_CMD=python3 .. # build python3 binding
pushd src/python/
make
sudo make install
popd
```
如果cmake的过程中，出现下面的错误：

```shell
cmake ..
-- The C compiler identification is GNU 7.5.0
-- The CXX compiler identification is GNU 7.5.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
Not a git repository
To compare two paths outside a working tree:
usage: git diff [--no-index] <path> <path>
fatal: not a git repository (or any of the parent directories): .git
-- Latest recognized Git tag is HEAD-HASH-NOTFOUND
-- Git HEAD is GITDIR-NOTFOUND
-- Revision is EAD-HASH-NOTFOUND-GITDIR-N
-- Performing Test HAVE_NO_PIE_FLAG
-- Performing Test HAVE_NO_PIE_FLAG - Success
-- Performing Test HAVE_REALLOCARRAY_SUPPORT
-- Performing Test HAVE_REALLOCARRAY_SUPPORT - Success
-- Found LLVM: /usr/lib/llvm-6.0/include 6.0.0
-- Found BISON: /usr/bin/bison (found version "3.0.4") 
-- Found FLEX: /usr/bin/flex (found version "2.6.4") 
-- Found LibElf: /usr/lib/x86_64-linux-gnu/libelf.so  
-- Performing Test ELF_GETSHDRSTRNDX
-- Performing Test ELF_GETSHDRSTRNDX - Success
-- Using static-libstdc++
-- Could NOT find LuaJIT (missing: LUAJIT_LIBRARIES LUAJIT_INCLUDE_DIR) 
CMake Warning at tests/python/CMakeLists.txt:10 (message):
  Recommended test program 'netperf' not found


CMake Warning at tests/python/CMakeLists.txt:16 (message):
  Recommended test program 'iperf' or 'iperf3' not found


-- Configuring done
-- Generating done
-- Build files have been written to: /home/tupelo/ebpf/bcc/build
```

安装相应的包即可：

```shell
sudo apt-get install netperf
sudo apt-get install iperf
```
## 3 使用bcc tools里的工具

> bcc工具的默认安装目录为`/usr/share/bcc/tools`

使用`cachestat`查看缓存命中情况。

```shell
$ sudo /usr/share/bcc/tools/cachestat
[sudo] password for tupelo: 
    HITS   MISSES  DIRTIES HITRATIO   BUFFERS_MB  CACHED_MB
      34        0        0  100.00%          177       1655
       0        0        0    0.00%          177       1655
       0        0        0    0.00%          177       1655
       3        0        0  100.00%          177       1655
       0        0        3    0.00%          177       1655
       0        0        0    0.00%          177       1655
       0        0        0    0.00%          177       1655
       0        0        0    0.00%          177       1655
       3        0        0  100.00%          177       1655
       0        0        0    0.00%          177       1655
       0        0        0    0.00%          177       1655
       0        0        0    0.00%          177       1655
       0        0        0    0.00%          177       1655
```

## 4 参考文章

1. [https://zhuanlan.zhihu.com/p/57579051](https://zhuanlan.zhihu.com/p/57579051)

2. [https://blog.csdn.net/weixin_44395686/article/details/106712543](https://blog.csdn.net/weixin_44395686/article/details/106712543)
[TOC]

# 1 Supported platforms

Docker Engine is available on a variety of Linux platforms, macOS and Windows 10 through Docker Desktop, and as a static binary installation. Find your preferred operating system below.

# 1.1 Desktop

| 平台 | x86_64/amd64 |
| ---- | ------------ |
| Mac(macOS) | 支持 |
| Windows    | 支持 |

# 1.2 Server版

Docker provides `.deb` and `.rpm` packages from the following Linux distributions and architectures:

| 平台     | x86_64/amd64 | ARM | ARM64/AARCH64 |
| -------- | ------------ | --- | ------------- |
| CentOS   | √            |     | √             |
| Debian   | √            |  √  | √             |
| Fedora   | √            |     | √             |
| Raspbian |              |  √  | √             |
| Ubuntu   | √            |  √  | √             |

# 2 Release channels

Docker Engine has three types of update channels, stable, test, and nightly:

* The Stable channel gives you latest releases for general availability.

* The Test channel gives pre-releases that are ready for testing before general availability (GA).

* The Nightly channel gives you latest builds of work in progress for the next major release.

## 2.1 Stable

稳定版本使用`年.月`表示从主分支上引出的一个分支，比如19.03就表示19年3月从主分支上引出的一个分支。之后，如果再对这个分支进行修改，就会在后面添加一个版本号，比如19.03.1。

## 2.2 Test

达到某个预定里程碑后，发布的β版本。

## 2.3 Nightly

主分支上的重要版本节点。

# 3 建立docker环境

请参考：[Install Docker Engine on Ubuntu](https://docs.docker.com/engine/install/ubuntu/)

安装后：[Post-installation steps for Linux](https://docs.docker.com/engine/install/linux-postinstall/)



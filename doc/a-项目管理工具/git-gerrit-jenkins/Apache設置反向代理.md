反向代理有很多实现方式，本文主要讲述Apache的反向代理。

### 1. 简述

反向代理（`Reverse Proxy`）方式是指以代理服务器来接受internet上的连接请求，然后将请求转发给内部网络上的服务器，并将从服务器上得到的结果返回给internet上请求连接的客户端，此时代理服务器对外就表现为一个反向代理服务器。

下图就是一个简单的反向代理过程

![反向代理过程](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/apache%E5%8F%8D%E5%90%91%E4%BB%A3%E7%90%86%E8%BF%87%E7%A8%8B.png)

### 2. 配置

#### 1）、 确保Apache有这些模块，在
## 在`Ubuntu 16.04`上安装Gerrit

本文展示如何在`Ubuntu 16.04`上机器上安装`Gerrit`。

> 快速安装手册，可以参考[Quick get started guide](https://gerrit-documentation.storage.googleapis.com/Documentation/2.12.2/install-quick.html)
>
> 详细的安装手册，可以参考[Standalone Daemon Installation Guide.](https://gerrit-documentation.storage.googleapis.com/Documentation/2.15.3/install.html)

### 准备工作

1. 一台运行类Unix操作系统的服务器
2. Java SE 实时运行环境，1.8及其以后的版本(如果没有，可以使用下面的方法安装)

    a. 安装依赖包：

        sudo apt-get install python-software-properties

    b. 添加仓库源：

        sudo add-apt-repository ppa:webupd8team/java

    c. 更新软件包列表：

        sudo apt-get update

    d. 安装java JDK：

        sudo apt-get install oracle-java-installer

    安装过程中需要接受协议：接受即可。

    e. 查看java版本，看看是否安装成功：

        java version "1.8.0_191"


### 下载`Gerrit`

选择合适的Gerrit版本，进行下载。可用的Gerrit版本，可以参考[Gerrit Code Review - Releases](https://gerrit-documentation.storage.googleapis.com/Documentation/2.12.2/install-quick.html)

本手册使用的版本是`Gerrrit 2.15.2`，下载命令是：

    wget https://www.gerritcodereview.com/download/gerrit-2.14.2.war

如果想使用源代码进行安装，请参考[Developer Setup](https://gerrit-documentation.storage.googleapis.com/Documentation/2.15.3/dev-readme.html)。

### 安装和初始化Gerrit

命令行输入：

    java -jar gerrit*.war init --batch --dev -d ~/gerrittest

参数解释：

    1. --batch

        这个参数

运行完成后，命令行会显示下面这些信息：

    Generating SSH host key ... rsa(simple)... done
    Initialized /home/gerrit/gerrittest
    Executing /home/gerrit/gerrittest/bin/gerrit.sh start
    Starting Gerrit Code Review: OK

`Starting Gerrit Code Review: OK`，这条信息告知我们`Gerrit`服务正在运行。

1. **更新监听的URL**

把Gerrit监听的URL，从* 改为`localhost`，阻止外界与本服务链接。

    git config --file ~/gerrittest/etc/gerrit.config httpd.listenUrl 'http://localhost:8080'

`git config`是`gerrit`的配置方法。

2. **重启Gerrit服务**

必须重新启动身份验证类型的Gerrit服务，监听URL的改变才能生效。

    ~/gerrittest/bin/gerrit.sh restart

`~/gerrittest/`就是安装时，指定的目录。

3. **浏览Gerrit**

到现在，已经完成了Gerrit的基本安装，可以通过打开浏览器，查看安装的结果：

    http://localhost:8080

到目前为止，我们已经完成了一个本机上的简易`Gerrit`服务器。接下来，我们研究使用代理服务器，从远程服务器访问该`Gerrit`服务器。

## 反向代理

### 描述

`Gerrit`可以被配置在第3方web服务器下运行。这允许其它web服务器绑定到特权端口`80`(或用于SSL的`443`)。

### `Gerrit`配置

确保`'$site_path'/etc/gerrit.config`有属性`httpd.listenUrl`，使用 `'proxy-http://'` 或`'proxy-https://'` 和一个自由端口号进行配置。如果在安装的时候，也就是'init'的时候，代理支持被使能，这里应该已经配置了。

    [httpd]
        listenUrl = proxy-http://10.48.100.182:8081/ # started by the proxy; gerrit server ip and port

我的配置,位于`'$site_path'/etc/gerrit.config`：

    [gerrit]
        basePath = git
        serverId = 45493128-5b74-44c5-a0ef-41e9b72fd239
        canonicalWebUrl = http://10.48.100.182:8081/  # gerrit server ip and port
    [database]
        type = h2
        database = /home/gerrittest/gerrit/db/ReviewDB
    [noteDb "changes"]
        disableReviewDb = true
        primaryStorage = note db
        read = true
        sequence = true
        write = true
    [index]
        type = LUCENE
    [auth]
        type = HTTP #DEVELOPMENT_BECOME_ANY_ACCOUNT # authenticated by http
    [receive]
        enableSignedPush = false
    [sendemail]
        smtpServer = localhost
    [container]
        user = gerrittest
        javaHome = /usr/lib/jvm/java-8-oracle/jre
    [sshd]
        listenAddress = *:29418
    [httpd]
        listenUrl = proxy-http://10.48.100.182:8081/ # started by the proxy
    [cache]
        directory = cache
    [plugins]
        allowRemoteAdmin = true

### `Apache2`配置

需要使能必要的Apache2模块：

    a2enmod proxy_http
    a2dissite 000-default
    a2enmod ssl          ; # 可选，HTTPS或SSL需要

使用上面的`'http://'URL`设置'ProxyPass'行。 保证`ProxyPass` 和 `httpd.listenUrl` 匹配，或者能够重定向到正确的地址。

我的设置，位于`/etc/apache2/sites-available/gerrit.conf`，在`/etc/apache2/sites-enabled/`目录下有其链接文件。

    <VirtualHost *:80>
        ServerName 10.48.100.182
        ProxyRequests Off
        ProxyVia Off
        ProxyPreserveHost On
        <Proxy *>
            Require all granted # >= apache2 2.6 required
        </Proxy>

        <Location "/">
            AuthType Basic
            AuthName "Gerrit Code Review"
            Require valid-user
        AuthBasicProvider file
            AuthUserFile /etc/apache2/passwords
        </Location>

        AllowEncodedSlashes On
        ProxyPass /  http://10.48.100.182:8080/ nocanon
        ProxyPassReverse / http://10.48.100.182:8080/ nocanon

        ErrorLog /var/log/apache2/gerrit.error.log
        CustomLog /var/log/apache2/gerrit.access.log combined
    </VirtualHost>

2选项`'AllowEncodedSlashes On'`和`'ProxyPass .. nocanon'`在`Gerrit 2.6`及以后版本必须有。

使能配置的网站命令：

    sudo a2ensite gerrit

**注意**： 要记得编辑文件`/etc/apache2/ports.conf`，将监听端口改为`listen 8080`。



### 设置第一个 Gerrit 用户的帐号和密码

    $ sudo touch /etc/apache2/passwords
    $ sudo htpasswd -b /etc/apache2/passwords admin 123456 # administrator
    $ sudo htpasswd -b /etc/apache2/passwords gerrit1 123456 # general usr

`htpasswd `命令是 `apache2-utils `软件包中的一个工具。如果系统中还没有安装的话，通过如下命令进行安装：

    $ sudo apt-get install apache2-utils

后续再添加 Gerrit 用户可使用 `htpasswd -b /etc/apache2/passwords UserName PassWord`)。如果要创建一组授权用户， 则可以使用下面的命令：

    for i in $(seq 1 30); do echo user$i | sudo htpasswd -i /etc/apache2/passwords user$i; done

对于 Gerrit 来说，第一个成功登录的用户具有特殊意义-它会直接被赋予管理员权限。对于第一个账户，需要特别注意一下。

### 启动gerrit&启动apache2

    sudo ~/gerrittest/bin/gerrit.sh start
    sudo /etc/init.d/apache2 start
## 在linux上安装Gerrit的快速入门

本文展示如何在Linux机器上安装Gerrit。

> 该快速入门中提供的安装步骤仅用于演示的目的，并不是打算用来实际的开发环境中。
>
> 更详细的安装手册，可以参考[Standalone Daemon Installation Guide.](https://gerrit-documentation.storage.googleapis.com/Documentation/2.15.3/install.html)

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

选择合适的Gerrit版本，进行下载。可用的Gerrit版本，可以参考[Gerrit Code Review - Releases](https://gerrit-releases.storage.googleapis.com/index.html)

本手册使用的版本是`Gerrrit 2.15.2`，下载命令是：

    wget https://www.gerritcodereview.com/download/gerrit-2.14.2.war

如果想使用源代码进行安装，请参考[Developer Setup](https://gerrit-documentation.storage.googleapis.com/Documentation/2.15.3/dev-readme.html)。

### 安装和初始化Gerrit

命令行输入：

    java -jar gerrit*.war init --batch --dev -d ~/gerrit_testsite

参数解释：

    1. --batch

        这个参数

运行完成后，命令行会显示下面这些信息：

    Generating SSH host key ... rsa(simple)... done
    Initialized /home/gerrit/gerrit_testsite
    Executing /home/gerrit/gerrit_testsite/bin/gerrit.sh start
    Starting Gerrit Code Review: OK

`Starting Gerrit Code Review: OK`，这条信息告知我们`Gerrit`服务正在运行。

### 更新监听的URL

把Gerrit监听的URL，从* 改为`localhost`，阻止外界与本服务链接。

    git config --file ~/gerrit_testsite/etc/gerrit.config httpd.listenUrl 'http://localhost:8080'

### 重启Gerrit服务

必须重新启动身份验证类型的Gerrit服务，监听URL的改变才能生效。

    ~/gerrit_testsite/bin/gerrit.sh restart

### 浏览Gerrit

到现在，已经完成了Gerrit的基本安装，可以通过打开浏览器，查看安装的结果：

    http://localhost:8080

### 接下来的步骤

通过本手册，已经有了一个运行在你的机器上的简易版的Gerrit。你可以使用这个版本的Gerrit熟悉其功能。更为详细的安装请参照[Standalone Daemon Installation Guide](https://gerrit-documentation.storage.googleapis.com/Documentation/2.15.3/install.html)

### 设置第一个 Gerrit 用户的帐号和密码

    $ touch ./review_site/etc/passwd
    $ htpasswd -b ./review_site/etc/passwd admin admin
    Adding password for user admin

`htpasswd `命令是 `apache2-utils `软件包中的一个工具。如果系统中还没有安装的话，通过如下命令进行安装：

    $ sudo apt-get install apache2-utils

后续再添加 Gerrit 用户可使用 `htpasswd -b ./review_site/etc/passwd UserName PassWord`)

对于 Gerrit 来说，第一个成功登录的用户具有特殊意义-它会直接被赋予管理员权限。对于第一个账户，需要特别注意一下。

### 开启 Gerrit 服务器

Gerrit 服务器还可以通过如下的命令进行启动：

    $ review_site/bin/gerrit.sh start
    Starting Gerrit Code Review: FAILED

上面的命令，通过 review_site/bin/gerrit.sh start 启动 Gerrit Server，但是失败了。Gerrit Server 启动，需要监听 8080 端口，如前面的配置文件 review_site/etc/gerrit.config 中的 listenUrl 行所显示的那样。通过如下的命令查看 8080 端口的使用情况：

    $ sudo lsof -i -P | grep 8080
    java       9538          tomcat   53u  IPv6 17098680      0t0  TCP *:8080 (LISTEN)

可以看到 8080 端口被 tomcat 用户下的某个 Java 进程占用了。我们可以杀掉相应的应用来解决 Gerrit Server 启动失败的问题（Tomcat 的安装配置启动按照Ubuntu 16.04 Tomcat 8安装指南 一文的说明进行）：

    $ sudo systemctl stop tomcat
    $ sudo systemctl status tomcat
    ● tomcat.service - Apache Tomcat Web Application Container
       Loaded: loaded (/etc/systemd/system/tomcat.service; enabled; vendor preset: enabled)
       Active: inactive (dead) since 四 2017-11-23 10:23:02 CST; 4s ago
      Process: 9805 ExecStop=/opt/tomcat/bin/shutdown.sh (code=exited, status=0/SUCCESS)
      Process: 9527 ExecStart=/opt/tomcat/bin/startup.sh (code=exited, status=0/SUCCESS)
     Main PID: 9538 (code=exited, status=0/SUCCESS)

    11月 23 10:20:18 ThundeRobot startup.sh[9527]: Removing/clearing stale PID file.
    11月 23 10:20:18 ThundeRobot systemd[1]: Started Apache Tomcat Web Application Container.
    11月 23 10:23:01 ThundeRobot systemd[1]: Stopping Apache Tomcat Web Application Container...
    11月 23 10:23:01 ThundeRobot shutdown.sh[9805]: Using CATALINA_BASE:   /opt/tomcat
    11月 23 10:23:01 ThundeRobot shutdown.sh[9805]: Using CATALINA_HOME:   /opt/tomcat
    11月 23 10:23:01 ThundeRobot shutdown.sh[9805]: Using CATALINA_TMPDIR: /opt/tomcat/temp
    11月 23 10:23:01 ThundeRobot shutdown.sh[9805]: Using JRE_HOME:        /usr/lib/jvm/java-1.8.0-openjdk-amd64/jre
    11月 23 10:23:01 ThundeRobot shutdown.sh[9805]: Using CLASSPATH:       /opt/tomcat/bin/bootstrap.jar:/opt/tomcat/bin/tomcat-juli.jar
    11月 23 10:23:01 ThundeRobot shutdown.sh[9805]: Using CATALINA_PID:    /opt/tomcat/temp/tomcat.pid
    11月 23 10:23:02 ThundeRobot systemd[1]: Stopped Apache Tomcat Web Application Container.

然也可以通过修改 Gerrit Server 监听的端口来解决问题。

再次启动 Gerrit服务器：

    $ review_site/bin/gerrit.sh start
    Starting Gerrit Code Review: OK

可以看到 Gerrit Server 成功启动了。此时通过浏览器，打开 http://review.virtcloudgame.com:8080 将可以看到如下这样的页面：

### 修改认证方式和反向代理

为了通过更为强大的 Web 服务器来对外提供服务，同时方便 Gerrit服务器的`HTTP `用户认证方式可以正常工作， 需要设置反向代理。 这里使用 `nginx` 作为 Web 服务器。

首先更改 Gerrit 配置，使能代理；另外，使用反向代理后就可以直接使用 nginx 的 80 端口访问了，需要把 canonicalWebUrl 中的 8080 去掉，Gerrit Server 监听的端口也改为 8081：

    [gerrit]
        basePath = GerritResource
        serverId = 7b8058ff-932a-41ed-a1fa-6ea53dfba8e1
        canonicalWebUrl = http://review.virtcloudgame.com/
            useSSL=false
    . . . . . .
    [auth]
        type = HTTP
    . . . . . .
    [httpd]
            listenUrl = proxy-http://127.0.0.1:8081/

修改之后，重启 Gerrit Server：

    $ review_site/bin/gerrit.sh restart
    Stopping Gerrit Code Review: OK
    Starting Gerrit Code Review: OK

上面的修改将使 Gerrit Server 监听在 8081 端口上，同上，将认证方式修改为 HTTP。

Gerrit Server 强制要求使用反向代理，通过反向代理服务器提供的 Authorization 等 HTTP 头来获得用户认证信息。

接着配置 nginx。修改 nginx 的配置文件 /etc/nginx/nginx.conf，在它的 http 块中加入如下内容：

    server {
      listen 80;
      server_name review.virtcloudgame.com;

      location ^~ / {
            auth_basic "Restricted";
            auth_basic_user_file /home/gerrit/review_site/etc/passwd;
        proxy_pass        http://127.0.0.1:8081;
        proxy_set_header  X-Forwarded-For $remote_addr;
        proxy_set_header  Host $host;
      }
    }

auth_basic_user_file 行用户配置用户名和密码文件的保存路径。proxy_pass 行用户设置 Gerrit Server 的地址。

修改之后，让 nginx 重新加载配置文件：

    $ sudo nginx -s reload

这样就可以直接通过 nginx 监听的 80 端口访问 Gerrit Server 了。

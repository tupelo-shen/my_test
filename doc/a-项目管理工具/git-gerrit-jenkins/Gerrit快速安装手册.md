<h2 id="0">0 目录</h2>

* [1 在 Ubuntu 16.04 上安装Gerrit](#1)
    - [1.1 准备工作](#1.1)
    - [1.2 下载gerrit](#1.2)
    - [1.3 安装和初始化gerrit](#1.3)
    - [1.4 使用gerrit](#1.4)
* [2 反向代理](#2)
    - [2.1 描述](#2.1)
    - [2.2 Gerrit配置](#2.2)
    - [2.3 HTTP身份验证](#2.3)
    - [2.4 Apache2配置](#2.4)
    - [2.5 设置第一个 Gerrit 用户的帐号和密码](#2.5)
    - [2.6 启动gerrit&启动apache2](#2.6)
* [3 SSH登录](#3)
    - [3.1 生成SSH key](#3.1)
    - [3.2 设置Gerrit服务端SSH Key](#3.2)
* [4 使用gerrit](#4)
    - [4.1 添加项目](#4.1)
    - [4.2 修改并push到gerrit服务器](#4.2)
    - [4.3 完成review并merge](#4.3)
***

<h2 id="1">1 在 Ubuntu 16.04 上安装Gerrit</h2>

本文展示如何在`Ubuntu 16.04`上机器上安装`Gerrit`。

> 快速安装手册，可以参考[Quick get started guide](https://gerrit-documentation.storage.googleapis.com/Documentation/2.12.2/install-quick.html)
>
> 详细的安装手册，可以参考[Standalone Daemon Installation Guide.](https://gerrit-documentation.storage.googleapis.com/Documentation/2.15.3/install.html)
>
> 其它参考文章：
>
> （1） [HowTo: Demo Gerrit server on AWS EC2 instance](https://egeek.me/2016/04/10/howto-demo-gerrit-server-on-aws-ec2-instance/)
>

<h3 id="1.1">1.1 准备工作</h3>

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

3. git

    `gerrit`用来操作`git`仓库。安装非常简单：

        sudo apt-get install git

    验证安装是否成功：

        git --version

4. apache2

    `gerrit`反向代理时使用，安装：

        sudo apt-get install apache2

    验证：

        sudo /etc/init.d/apache2 start

5. gerrit管理帐号(可选，使用独立账号配置gerrit)

    用来管理gerrit。

        sudo adduser gerrit
        sudo passwd gerrit

    并将`gerrit`加入`sudo`权限

        sudo vim /etc/sudoers

    添加下面一句：

        gerrit  ALL=(ALL:ALL) ALL

<h3 id="1.2">1.2 下载Gerrit</h3>

选择合适的Gerrit版本，进行下载。可用的Gerrit版本，可以参考[Gerrit Code Review - Releases](https://gerrit-documentation.storage.googleapis.com/Documentation/2.12.2/install-quick.html)

本手册使用的版本是`Gerrrit 2.15.2`，下载命令是：

    wget https://www.gerritcodereview.com/download/gerrit-2.14.2.war

如果想使用源代码进行安装，请参考[Developer Setup](https://gerrit-documentation.storage.googleapis.com/Documentation/2.15.3/dev-readme.html)。

<h3 id="1.3">1.3 安装和初始化Gerrit</h3>

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

<h3 id="1.4">1.4 使用Gerrit</h3>

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

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="2">2 反向代理</h2>

<h3 id="2.1">2.1 描述</h3>

`Gerrit`可以被配置在第3方web服务器下运行。这允许其它web服务器绑定到特权端口`80`(或用于SSL的`443`)。

<h3 id="2.2">2.2 Gerrit配置</h3>

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

<h3 id="2.3">2.3 HTTP身份验证</h3>

参考文档:
[Gerrit Code Review - 配置](https://gerrit-documentation.storage.googleapis.com/Documentation/2.12.2/config-gerrit.html#auth)

使用`HTTP`身份验证时，`Gerrit`假定`servlet容器`或`前端Web服务器`在将请求交给`Gerrit`之前已执行了所有用户身份验证。

基于这个假设，`Gerrit`可以假设所有请求都已经过身份验证。因此， `login` 和 `logout` 不会显示在Web UI中。

使能`HTTP`身份验证：

    git config --file $site_path/etc/gerrit.config auth.type HTTP
    git config --file $site_path/etc/gerrit.config --unset auth.httpHeader
    git config --file $site_path/etc/gerrit.config auth.emailFormat '{0}@example.com'

* auth.type

    默认是`OpenID`，所以，这里需要根据所需进行设置。下面是一些常用的设置项：

    * HTTP

        `Gerrit`依赖于`HTTP`请求中提供的数据。这包括`HTTP`基本身份验证或某些类型的商业单点登录解决方案。 启用此设置后，身份验证必须在Web服务器或servlet容器中进行，而不是在Gerrit中进行。

    * HTTP_LDAP

        完全像上面的`HTTP`，但是还要根据从`LDAP`中用户的账户对象中获得的信息，预填充用户的全名和电子邮件地址。 用户的组成员关系也从`LDAP`中提取，使用户所属的任何`LDAP`组成为`Gerrit`的组。

    * LDAP

        `Gerrit`提示用户输入一个`用户名`和`密码`，然后通过与配置的ldap.server进行简单的绑定来验证用户名和密码。 使用此配置，web服务器不会涉及到用户身份验证。

        `LDAP`简单绑定请求中使用的实际用户名是帐户的完整`DN`，通过首先使用匿名请求或配置的`ldap.username`标识查询目录来发现该用户名。 如果将`ldap.authentication`设置为`GSSAPI`，`Gerrit`也可以使用`kerberos`。

    * LDAP_BIND

        `Gerrit`提示用户输入一个`用户名`和`密码`，然后通过与配置的`ldap.server`进行简单的绑定来验证用户名和密码。 使用此配置，web服务器不会涉及到用户身份验证。

        与`LDAP`不同的是，LDAP简单绑定请求中所使用的用户名就是用户在对话框中填入的真实字符串。不会使用`ldap.username`设置的标识中获取账户的信息。


    * DEVELOPMENT_BECOME_ANY_ACCOUNT

        **不要使用！！！**，仅在开发阶段的时候使用。不需要任何登录信息。

* auth.httpHeader

    必须是`unset`。如果设置为任何值（包括`Authorization`），则`Gerrit`将无法正确遵循标准的`Authorization HTTP`标头。

* auth.loginUrl

    在最终用户点击右上角的登录链接后，将浏览器重定向到的URL。 仅在auth.type设置为HTTP或HTTP_LDAP时使用。 使用企业单点登录解决方案的组织可能希望将浏览器重定向到SSO产品的登录页面，以完成登录过程并验证其凭据。

    如果设置，Gerrit允许匿名访问，直到最终用户执行登录并通过HTTP标头提供受信任的身份。 如果未设置，Gerrit需要具有受信任标识的HTTP标头，并返回错误页面“LoginRedirect.html”（如果此标头不存在）。

<h3 id="2.4">2.4 Apache2配置</h3>

需要使能必要的Apache2模块：

    a2enmod proxy_http
    a2dissite 000-default
    a2enmod ssl          ; # 可选，HTTPS或SSL需要

使用上面的`'http://'URL`设置'ProxyPass'行。 保证`ProxyPass` 和 `httpd.listenUrl` 匹配，或者能够重定向到正确的地址。

我的设置，位于`/etc/apache2/sites-available/gerrit.conf`，在`/etc/apache2/sites-enabled/`目录下有其链接文件。

    <VirtualHost *:8080>
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
        ProxyPass /  http://10.48.100.182:8081/ nocanon
        ProxyPassReverse / http://10.48.100.182:8081/ nocanon

        ErrorLog /var/log/apache2/gerrit.error.log
        CustomLog /var/log/apache2/gerrit.access.log combined
    </VirtualHost>

2选项`'AllowEncodedSlashes On'`和`'ProxyPass .. nocanon'`在`Gerrit 2.6`及以后版本必须有。

使能配置的网站命令：

    sudo a2ensite gerrit

**注意**： 要记得编辑文件`/etc/apache2/ports.conf`，将监听端口改为`listen 8080`。

<h3 id="2.5">2.5 设置第一个 Gerrit 用户的帐号和密码</h3>

    $ sudo touch /etc/apache2/passwords
    $ sudo htpasswd -b /etc/apache2/passwords admin 123456 # administrator
    $ sudo htpasswd -b /etc/apache2/passwords gerrit1 123456 # general usr

`htpasswd `命令是 `apache2-utils `软件包中的一个工具。如果系统中还没有安装的话，通过如下命令进行安装：

    $ sudo apt-get install apache2-utils

后续再添加 Gerrit 用户可使用 `htpasswd -b /etc/apache2/passwords UserName PassWord`)。如果要创建一组授权用户， 则可以使用下面的命令：

    for i in $(seq 1 30); do echo user$i | sudo htpasswd -i /etc/apache2/passwords user$i; done

对于 Gerrit 来说，第一个成功登录的用户具有特殊意义-它会直接被赋予管理员权限。对于第一个账户，需要特别注意一下。

###
<h3 id="2.6">2.6 启动gerrit&启动apache2</h3>

    sudo ~/gerrittest/bin/gerrit.sh start
    sudo /etc/init.d/apache2 start

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3">3 SSH登录</h2>

<h3 id="3.1">3.1 生成SSH key</h3>

首先查看的目录下是否已经具有`id_rsa` 和 `id_rsa.pub`两个文件。

生成SSH key：

    ssh-keygen -t rsa

<h3 id="3.2">3.2 设置Gerrit服务端SSH Key</h3>

查看ssh key：

    cat ~/.ssh/id_rsa.pub

结果：

>ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDmXV/aXLZ+orq37Yr+0rmw43xk2Cfd8qEhJ9dpdLM9vX/2BglvUH4DFNyevpvQHUmmZpXvtlhpjuJuD2tt5pzGY7ZtmDRwOXCvmkNwXG/nbREUoeWctHT1KUbArPCrxByEdW2VPNk3yrFk2ZTBio7GhHPl8Pzhi0kzJvBQulK0U77ywTZwpmaZz0SiDiwHCAoCv/VyDCVpLCzLB48yUnNpaxaujAhf19q7jrqWHWaV8UFAv9vD+eET/sB1EIVaNm3ymiRuQtSyranVm4aV2hXdFpDxvN4qQGI160cI9V+EeGXlz8zqgL2/KE6ykhnFzDQU6tFsClHGJ89AD7K5tEcj gerrittest@tupelo-VirtualBox

添加sshkey到gerrit服务器(gerrit 服务端)。此步骤与git流程类似，即将id_rsa.pub内容上传到git仓库，gerrit中帮我们管理git仓库

![Gerrit_SSHKey_Paste_Setting](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/Gerrit_SSHKey_Paste_Setting.png)

验证sshkey是否配置成功：

    ssh gerrit1@[gerrit服务器的IP地址] -p 29418

如果不知道gerrit服务器的IP地址，可以使用下面的命令获取：

    git config -f ~/gerrit_testsite/etc/gerrit.config gerrit.canonicalWebUrl

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="4">4 使用gerrit</h2>

<h3 id="4.1">4.1 添加项目</h3>

1. 使用gerrit添加新项目：（适用于开启新项目并使用gerrit）

        ssh -p 29418 gerrit1@[服务器的IP地址] gerrit create-project --empty-commit --name demo-project

或者使用gerrit管理界面(<font color=red>推荐</font>)

![gerrit_create_new_project](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_create_new_project.png)

2. 使用gerrit添加已有项目：（适用于已有项目下移植到gerrit中）

        ssh -p 29418 gerrit1@[服务器的IP地址] gerrit create-project --name exist-project

或者使用gerrit管理界面(<font color=red>推荐</font>)

![gerrit_create_exist_project](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_create_exist_project.png)

然后将已有项目与`gerrit`上建立的`exist-project`关联，即将已有代码库代码`push`到`gerrit`中进行管理。

    cd ~/gitcode/exist-project
    git push ssh://gerrit1@[服务器的IP地址]:29418/exist-project *:*

<h3 id="4.2">4.2 修改并push到gerrit服务器</h3>

1. pull代码及配置git hooks(git client)

        git clone ssh://gerrit1@[服务器的IP地址]:29418/demo-project

2. 更新git hooks

        gitdir=$(git rev-parse --git-dir); scp -p -P 29418 gerrit1@[服务器的IP地址]:hooks/commit-msg ${gitdir}/hooks/

    该过程用来在commit-msg中加入change-id，gerrit流程必备。

3. push 改动到远程服务器

        cd ~/demo-project
        date > testfile.txt
        git add testfile.txt
        git commit -m "My pretty test commit"
        git push origin HEAD:refs/for/master

    这里有可能报错：

        remote: Processing changes: refs: 1, done
        remote:
        remote: ERROR:  In commit 4732b67cac5f3e7d406c5ef5eeef63716c541b85
        remote: ERROR:  committer email address shenwanjiang2013@163.com
        remote: ERROR:  does not match your user account and you have no 'forge committer' permission.
        remote: ERROR:
        remote: ERROR:  You have not registered any email addresses.
        remote: ERROR:
        remote: ERROR:  To register an email address, please visit:
        remote: ERROR:  http://10.48.100.182:8081/#/settings/contact
        remote:
        remote:
        To ssh://gerrit1@10.48.100.182:29418/demo-project
         ! [remote rejected] HEAD -> refs/for/master (invalid committer)
        error: 无法推送一些引用到 'ssh://gerrit1@10.48.100.182:29418/demo-project'

    `push`代码时需要`commit email`与`gerrit account email`一致，否则无法`push`成功，可选择关闭`email notify`，并开启`forge user`权限，或者通过修改`gerrit`数据库`account email`信息

    <font color=red>修改`Gerrit`的配置文件`gerrit.config`和`secure.config`，内容分别如下</font>:

    * `gerrit.config`：

        [sendemail]
            enable = true
            smtpServer = smtp.163.com
            smtpServerPort = 465
            smtpEncryption = ssl
            smtpUser = 【你的邮箱账号】
            smtpPass = 【你的授权密码】
            sslVerify = false
            from = 【你的邮箱账号】

    * `secure.config`：

        [auth]
            registerEmailPrivateKey = ZIbpTyD12K8ZspVMsgkw5Tt0DkDL9FOlHV0=
        [sendemail]
            smtpPass = 【你的授权密码】



4. 提交成功的结果：

        gerrittest@tupelo-VirtualBox:~/demo-test/demo-project$ date > testfile.txt
        gerrittest@tupelo-VirtualBox:~/demo-test/demo-project$ git add testfile.txt
        gerrittest@tupelo-VirtualBox:~/demo-test/demo-project$ git commit -m "My pretty test commit"
        [master a3c1973] My pretty test commit
         1 file changed, 1 insertion(+)
         create mode 100644 testfile.txt
        gerrittest@tupelo-VirtualBox:~/demo-test/demo-project$ git push origin HEAD:refs/for/master
        Enter passphrase for key '/home/gerrittest/.ssh/id_rsa':
        对象计数中: 3, 完成.
        写入对象中: 100% (3/3), 338 bytes | 0 bytes/s, 完成.
        Total 3 (delta 0), reused 0 (delta 0)
        remote: Processing changes: new: 1, done
        remote:
        remote: New Changes:
        remote:   http://10.48.100.182:8081/#/c/demo-project/+/21 My pretty test commit
        remote:
        To ssh://gerrit1@10.48.100.182:29418/demo-project
         + [new branch]      HEAD -> refs/for/master

<h3 id="4.3">4.3 完成review并merge</h3>

使用`gerrit website`完成`code review`

a. 当完成push后，可在gerrit管理界面看到当前提交code review的change。

![Gerrit_check_commit_result](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/Gerrit_check_commit_result.png)

b. 打击上面提交的Commit，查看提交的详细信息（提交者可通过`Abandon`，放弃本次提交）：

![gerrit_push_result_details](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_push_result_details.png)

c. 添加`reviewers`

![gerrit_push_result_add_reviewer](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_push_result_add_reviewer.png)

d. 结果

![gerrit_push_result_after_add_reviewer](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_push_result_after_add_reviewer.png)

e. 以下是`Reviewer`的账户，可以看到收到的`Review`请求：

![gerrit_reviewer_check](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_reviewer_check.png)

f. 点击请求，进入其中，

![gerrit_reviewer_do_review](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_reviewer_do_review.png)

g. 完成`Review`并提交代码：

![gerrit_reviewer_complete_review](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/gerrit_reviewer_complete_review.png)


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="5">5 gerrit权限控制</h2>

在gerrit中权限控制是基于群组的. 每个用户有一个或者多个群组, 访问权限被赋予这些群组.访问权限不能赋予个人用户.

1. System Groups

在Gerrit系统自带下面的群组

* Anonymous Users
* Change Owner
* Project Owners
* Registered Users

2. Anonymous Users

所有用户都是匿名用户成员, 所有用户都能继承Anonymous Users所有访问权限.
当前只有Read access权限值得赋予给Anonymous Users群组, 因为其他权限都需要认证.

3. Project Owners

Project Owners的访问权限在Project范围内有效

4. Change Owner

Change Owner的访问权限在Change范围内有效

5. Registered Users

所有在页面上登录成功的用户都会自动注册为gerrit用户，属于Registered Users群组
Registered Users群组通常被赋予Code-Review -1..+1权限, 允许给需要审查代码投票, 但不会引起审查被批准和拒绝


<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
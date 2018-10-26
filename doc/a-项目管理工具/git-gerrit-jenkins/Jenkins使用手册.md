<h2 id="0">0 目录</h2>

* [安装Jenkins](#1)
    - [1.1 本机环境](#1.1)
    - [1.2 要求](#1.2)
    - [1.3 安装](#1.3)
* [2 启动Jenkins](#2)
* [3 打开防火墙](#3)
* [4 设置Jenkins](#4)

***

<h2 id="1">1 安装Jenkins</h2>

<h3 id="1.1">1.1 本机环境</h3>

1. 主机：

     Linux tupelo-VirtualBox 4.10.0-28-generic #32~16.04.2-Ubuntu SMP Thu Jul 20 10:19:13 UTC 2017 i686 i686 i686 GNU/Linux

<h3 id="1.2" >1.2 要求</h3>

* 硬件要求
    - 256 MB of RAM
    - 1 GB of drive space (although 10 GB is a recommended minimum if running Jenkins as a Docker container)
* 对于小团队，硬件配置要求
    - 1 GB+ of RAM
    - 50 GB+ of drive space
* 软件要求：
    - Java： java8版本，32位、64位均可
    - Web browser

<h3 id="1.3">1.3 安装</h3>

    wget -q -O - https://pkg.jenkins.io/debian/jenkins.io.key | sudo apt-key add -
    sudo sh -c 'echo deb http://pkg.jenkins.io/debian-stable binary/ > /etc/apt/sources.list.d/jenkins.list'
    sudo apt-get update
    sudo apt-get install jenkins

* `jenkins` 日志文件路径: ` /var/log/jenkins/jenkins.log`;
* 配置文件：：`/etc/default/jenkins`，包括默认端口为8080，端口有冲突时可以在这里以及脚本 `/etc/init.d/jenkins` 中进行相应的修改；

此时就可以通过`[your_IP]:8080 (如：192.168.54.108:8080)` 来访问Jenkins了。


<h2 id="2">2 启动Jenkins</h2>

    sudo systemctl start jenkins

由于`systemctl`不显示输出，我们将使用其`status`命令来验证它是否成功启动：

    sudo systemctl status jenkins

结果：

    ● jenkins.service - LSB: Start Jenkins at boot time
       Loaded: loaded (/etc/init.d/jenkins; bad; vendor preset: enabled)
       Active: active (exited) since 五 2018-10-26 11:47:16 CST; 51min ago
         Docs: man:systemd-sysv-generator(8)

`Active: active (exited)`说明正在运行，我们将调整防火墙规则，以便我们可以从网络浏览器到达`Jenkins`以完成初始设置。

http://10.48.100.182:8082/login?from=%2F

<h2 id="3">3 打开防火墙</h2>

默认情况下，Jenkins在端口8080上运行，因此我们将使用ufw打开该端口：

    sudo ufw allow 8080

检查UFW的状态可以看到新的规则。

    sudo ufw status

我们应该看到，从任何地方都可以访问8080端口：

    Status: active

    To                         Action      From
    --                         ------      ----
    OpenSSH                    ALLOW       Anywhere
    8080                       ALLOW       Anywhere
    OpenSSH (v6)               ALLOW       Anywhere (v6)
    8080 (v6)                  ALLOW       Anywhere (v6)
现在，Jenkins已安装，防火墙允许我们访问它，我们可以完成初始设置。

<h2 id="4">4 设置Jenkins</h2>

要设置我们的安装，我们将使用服务器域名或IP地址访问Jenkins的默认端口8080 ：

    http://10.48.100.182:8080

登录后会出现下面的初始界面：

![jenkins_init_interface](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/jenkins_init_interface.png)

在终端窗口中，我们将使用cat命令显示密码：

    sudo cat /var/lib/jenkins/secrets/initialAdminPassword

我们将从终端复制32个字符的字母数字密码，并将其粘贴到“管理员密码”字段中，然后单击“继续”。 下一个屏幕提供安装建议的插件或选择特定插件的选项。

![jenkins_getting_started](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/jenkins_getting_started.png)

我们将点击“安装建议的插件”选项，这将立即开始安装过程：

![jenkins_quick_setup](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/jenkins_quick_setup.png)

安装完成后，系统将提示您设置第一个管理用户。 可以跳过此步骤，并使用上面使用的初始密码作为admin继续，但是我们将花一点时间创建用户。

注意：默认Jenkins服务器未加密，因此使用此表单提交的数据不受保护。 当您准备好使用此安装时，请按照指南[如何使用Nginx反向代理将SSL配置为Jenkins](http://www.howtoing.com/how-to-configure-jenkins-with-ssl-using-an-nginx-reverse-proxy/) 。 这将保护用户凭据和关于通过Web界面发送的构建的信息。

![jenkins_setup_admin_account_info](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/jenkins_setup_admin_account_info.png)

一旦第一个管理员用户到位，你应该看到一个“Jenkins准备好了！” 确认屏幕。

点击“开始使用Jenkins”来访问主要的Jenkins仪表板：

![jenkins_start_interface](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/a-%E9%A1%B9%E7%9B%AE%E7%AE%A1%E7%90%86%E5%B7%A5%E5%85%B7/git-gerrit-jenkins/images/jenkins_start_interface.png)

在本教程中，我们使用项目提供的软件包安装了Jenkins，启动了服务器，打开了防火墙，并创建了一个管理用户。 在这一点上，您可以开始探索Jenkins。

完成探索后，如果您决定继续使用Jenkins，请按照指南， [如何使用Nginx反向代理使用SSL配置Jenkins](http://www.howtoing.com/how-to-configure-jenkins-with-ssl-using-an-nginx-reverse-proxy/)，以保护密码以及任何敏感的系统或将发送的产品信息在您的机器和服务器之间的纯文本。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
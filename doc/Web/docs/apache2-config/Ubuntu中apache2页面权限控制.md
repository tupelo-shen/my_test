# 更改apache配置文件

配置文件所在路径为：*/etc/apache2/sites-enabled*. 输入下面的命令修改文件：
    
    sudo vim /etc/apache2/sites-enabled/000-default.conf 

输入内容为：

    <Directory /var/www/html">
        Options Indexes MultiViews
        AllowOverride AuthConfig
        Order allow,deny
        Allow from all
    </Directory>

# 2. 创建账户密码文件

**（1） 使用下面的命令创建.htaccess文件** 

    sudo vim /var/www/html/.htaccess

**(2) 在.htaccess文件中加入下面的代码,并保存**

    AuthName "html"
    AuthType Basic
    AuthUserFile /var/www/html/.htpasswd
    require valid-user

**(3) 使用下面的命令创建.htpasswd，并修改文件权限**

    sudo vim /var/www/html/.htpasswd
    sudo chmod 766 /var/www/html/.htpasswd

**(4) 创建用户和密码**

第一次创建用户用下面的命令,以后设用户不需要-c参数

    htpasswd -c /var/www/html/.htpasswd admin

> 注: admin 就是用户名; 按提示输入两次密码(123)

# 3. 重启apache服务

输入下面的命令：
    
    sudo /etc/init.d/apache2 restart

# 4. 测试

在浏览器地址栏输入下面的地址: http://localhost/index.html  

则会自动弹出对话框，要求输入用户名密码。

### 要求

为了运行Gerrit服务，必须满足下面的条件：

    1. JRE 最低版本1.8

    2. SQL数据库，存储数据（`嵌入式H2`，或者使用自己的`MySQL`或`PostgreSQL`）。

### Configure Java for Strong Cryptography

Support for extra strength cryptographic ciphers: AES128CTR, AES256CTR, ARCFOUR256, and ARCFOUR128 can be enabled by downloading the Java Cryptography Extension (JCE) Unlimited Strength Jurisdiction Policy Files from Oracle and installing them into your JRE.

> 注意：
>
> Installing JCE extensions is optional and export restrictions may apply.

### 下载Gerrit

最新以及过去的Gerrit发布版都可以在[Gerrit Releases site](https://www.gerritcodereview.com/download/index.html)下载。

Download any current `*.war` package. The war will be referred to as gerrit.war from this point forward, so you may find it easier to rename the downloaded file.

If you would prefer to build Gerrit directly from source, review the notes under developer setup.

### Database Setup

During the init phase of Gerrit you will need to specify which database to use.

1. H2

    If you choose H2, Gerrit will automatically set up the embedded H2 database as backend so no set up or configuration is necessary.

    Using the embedded H2 database is the easiest way to get a Gerrit site up and running, making it ideal for proof of concepts or small team servers. On the flip side, H2 is not the recommended option for large corporate installations. This is because there is no easy way to interact with the database while Gerrit is offline, it’s not easy to backup the data, and it’s not possible to set up H2 in a load balanced/hotswap configuration.

    If this option interests you, you might want to consider the quick guide.

2. MySQL

    要求： MySQL 5.5及以后版本

    This option is also more complicated than the H2 option. Just as with PostgreSQL it’s also recommended for larger installations.

    Create a user for the web application within the database, assign it a password, create a database, and give the newly created user full rights on it:

        mysql

        CREATE USER 'gerrit'@'localhost' IDENTIFIED BY 'secret';
        CREATE DATABASE reviewdb;
        GRANT ALL ON reviewdb.* TO 'gerrit'@'localhost';
        FLUSH PRIVILEGES;

    Visit MySQL’s [documentation](http://dev.mysql.com/doc/) for further information regarding using MySQL.

### 初始化网站

Gerrit stores configuration files, the server’s SSH keys, and the managed Git repositories under a local directory, typically referred to as '$site_path'. If the embedded H2 database is being used, its data files will also be stored under this directory.
Gerrit在一个本地目录下，保存配置文件，服务器的SSH秘钥，管理的Git仓库。

You also have to decide where to store your server side git repositories. This can either be a relative path under '$site_path' or an absolute path anywhere on your server system. You have to choose a place before commencing your init phase.

Initialize a new site directory by running the init command, passing the path of the site directory to be created as an argument to the -d option. Its recommended that Gerrit Code Review be given its own user account on the host system:
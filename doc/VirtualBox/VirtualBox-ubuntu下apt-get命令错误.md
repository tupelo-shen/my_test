我们在虚拟机中运行Ubuntu系统，想要调用命令`apt-get`安装软件时，发生错误：

```shell
E: Could not get lock /var/lib/dpkg/lock - open (11: Resource temporarily unavailable) 
E: Unable to lock the administration directory (/var/lib/dpkg/), is another process using it? 
```

遇到这种错误，我们应该怎么办呢？

1. 找到并且杀掉所有的`apt-get`和`apt`进程

    运行下面的命令来生成所有含有`apt`的进程列表，你可以使用ps和grep命令并用管道组合来得到含有apt或者apt-get的进程。

    ```shell
    ps -A | grep apt
    ```
    找出所有的`apt`及`apt-get`进程，并杀死。

    ```shell
    $ sudo kill -9 进程号
    ```
    `9`可以使用符号`SIGKILL`代替。

2. 删除锁定文件

    锁定的文件会阻止 Linux 系统中某些文件或者数据的访问，这个概念也存在于 Windows 或者其他的操作系统中。一旦你运行了 `apt-get` 或者 `apt` 命令，锁定文件将会创建于`/var/lib/apt/lists/`、`/var/lib/dpkg/`、`/var/cache/apt/archives/`中。

    这有助于运行中的 apt-get 或者 apt 进程能够避免被其它需要使用相同文件的用户或者系统进程所打断。当该进程执行完毕后，锁定文件将会删除。

    当你没有看到 apt-get 或者 apt 进程的情况下在上面两个不同的文件夹中看到了锁定文件，这是因为进程由于某个原因被杀掉了，因此你需要删除锁定文件来避免该错误。

    首先运行下面的命令来移除 `/var/lib/dpkg/` 文件夹下的锁定文件：

    ```shell
    $ sudo rm /var/lib/dpkg/lock
    ```

    之后像下面这样强制重新配置软件包：

    ```shell
    $ sudo dpkg --configure -a
    ```

    也可以删除 `/var/lib/apt/lists/` 以及缓存文件夹下的锁定文件：

    ```shell
    $ sudo rm /var/lib/apt/lists/lock
    $ sudo rm /var/cache/apt/archives/lock
    ```

    接下来，更新你的软件包源列表：
    ```shell
    $ sudo apt update
    ```
    或者
    ```shell
    $ sudo apt-get update
    ```

完成。

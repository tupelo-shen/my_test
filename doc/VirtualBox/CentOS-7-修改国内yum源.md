转载于[https://www.jianshu.com/p/6ebaa6946c69](https://www.jianshu.com/p/6ebaa6946c69)

这就不用多解释了，一装系统就应该修改的操作。因为官方源在国外。

# 1 开启网络连接

对于新装的CentOS，默认是没有开启网络连接的，所以你看不到IP地址。

查看有线网络设备

    ip address

可以看到两个设备：

    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue
    2: enp1s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500

那么，有线网就是叫enp1s0啦，（虚拟机一般为 ens33 ）

    // 打开有线网配置
    vi /etc/sysconfig/network-scripts/ifcfg-enp1s0
    // 最后一行，修改为YES
    ONBOOT=YES
    // 重启网络服务
    systemctl restart network.service

> CentOS 6：网络重启的方法是`service network restart`或者`/etc/rc.d/init.d/network restart`

# 2 更换yum官方源

    # 下载wget工具
    yum install -y wget
    # 进入yum源配置文件所在文件夹
    cd /etc/yum.repos.d/
    # 备份本地yum源
    mv CentOS-Base.repo CentOS-Base.repo_bak
    # 获取国内yum源（阿里、163二选一）

    wget -O CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-7.repo
    # wget -O CentOS-Base.repo http://mirrors.163.com/.help/CentOS7-Base-163.repo

    # 清理yum缓存 
    yum clean all
    # 重建缓存 
    yum makecache 
    # 升级Linux系统
    yum -y update

# 3 增加epel源（可选）

    // 安装epel源
    yum install epel-release
    // 修改为阿里的epel源
    wget -O /etc/yum.repos.d/epel.repo http://mirrors.aliyun.com/repo/epel-7.repo

    // 安装yum源优先级管理工具
    yum install -y yum-priorities
    // 添加优先级（数字越小优先级越高）
    vim /etc/yum.repo.d/epel.repo
    priority=88
    // 添加优先级（这个数要小于epel里的88即可）
    vim /etc/yum.repo.d/Centos-7.repo
    priority=6

    // 开启yum源优先级功能
    vim /etc/yum/pluginconf.d/priorities.conf
    // 确保文件内容包含如下：
    [main]
    enabled=1

完成！可以愉快地搭建各种环境了。
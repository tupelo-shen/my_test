查找网络设备：

    ip route show | grep 'default' | awk '{print $5}'

通常是`eth0`，如果是使用的VirtualBox搭建的虚拟机，应该是'enp0s3'之类的。本文中，我们把网桥命名为`xenbr0`，设置`eth0`为slave设备。

首先，创建`xenbr0`，并设置合理的默认值：

    nmcli con add type bridge con-name xenbr0 ifname xenbr0
    nmcli con modify xenbr0 bridge.stp no
    nmcli con modify xenbr0 bridge.hello-time 0

显示设备：

    # nmcli con show
    NAME         UUID                                  TYPE            DEVICE 
    System eth0  1619a4a0-518e-4547-9ade-31dff5ef8f5e  802-3-ethernet  eth0  

设置`System eth0`为`xenbr0`的从设备：

    nmcli con modify "System eth0" connection.master xenbr0 connection.slave-type bridge

重启网络：

    systemctl restart network

查看是否正常设置：

    # ip addr
    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN 
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
        inet 127.0.0.1/8 scope host lo
           valid_lft forever preferred_lft forever
        inet6 ::1/128 scope host 
           valid_lft forever preferred_lft forever
    2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast master xenbr0 state UP qlen 1000
        link/ether 30:5b:d6:f1:d6:a2 brd ff:ff:ff:ff:ff:ff
    4: xenbr0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP 
        link/ether 30:5b:d6:f1:d6:a2 brd ff:ff:ff:ff:ff:ff
        inet 10.80.237.209/20 brd 10.80.239.255 scope global dynamic xenbr0
           valid_lft 1798sec preferred_lft 1798sec
        inet6 fe80::325b:d6ff:fef1:d6a2/64 scope link 
           valid_lft forever preferred_lft forever


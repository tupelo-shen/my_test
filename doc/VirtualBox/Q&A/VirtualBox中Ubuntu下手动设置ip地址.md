1. 编辑网络配置文件

        sudo vim /etc/network/interfaces

2. 如果设置静态ip地址，内容如下：

        # interfaces(5) file used by ifup(8) and ifdown(8)

        auto lo
        iface lo inet loopback

        auto enp0s3
        iface enp0s3 inet static
        address 10.48.100.246
        netmask 255.255.255.0
        gateway 10.48.100.1
        dns-nameservers 10.48.98.9 10.48.98.10

3. 如果设置动态ip地址，内容如下：

        # interfaces(5) file used by ifup(8) and ifdown(8)

        auto lo
        iface lo inet loopback

        # 注释掉下面的内容
        #auto enp0s3
        #iface enp0s3 inet static
        #address 10.48.100.246
        #netmask 255.255.255.0
        #gateway 10.48.100.1
        #dns-nameservers 10.48.98.9 10.48.98.10

2. 重新启动网络

        /etc/init.d/networking restart

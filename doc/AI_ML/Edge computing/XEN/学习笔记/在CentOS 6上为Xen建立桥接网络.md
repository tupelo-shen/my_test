WARNING: Before changing networking, be sure that you have non-network access to the target system in case something goes wrong.

Find your main network device:

    ip route show | grep 'default' | awk '{print $5}'

Normally this will be `eth0`. The rest of this guide will assume you want to make a bridge named `xenbr0` and set `eth0` as its slave.

Now go to the `/etc/sysconfig/network-scripts` directory, and make a file called `ifcfg-xenbr0` based on `ifcfg-eth0`:

    cat ifcfg-eth0 |
            sed "s/TYPE=.*$/TYPE=Bridge/" |
            sed "s/DEVICE=.*$/DEVICE=xenbr0/" |
            sed "s/NAME=.*$/NAME=xenbr0/" |
            sed "/UUID=/d" > ifcfg-xenbr0
    echo " DELAY=0" >> ifcfg-xenbr0

The resulting file should look something like this:

    TYPE=Bridge
    DEVICE=xenbr0
    ONBOOT=yes
    NM_CONTROLLED=yes
    BOOTPROTO=dhcp
    DEFROUTE=yes
    PEERDNS=yes
    PEERROUTES=yes
    IPV4_FAILURE_FATAL=yes
    IPV6INIT=no
    NAME=xenbr0
     DELAY=0

Now configure eth0 as xenbr0's slave:

    sed -i --follow-symlinks "s/BOOTPROTO=.*$/BOOTPROTO=none/;" ifcfg-eth0
    echo " BRIDGE=xenbr0" >> ifcfg-eth0

Finally, restart networking:

    service network restart

And verify that you have the bridge set up properly by running `ifconfig -a`:

    eth0      Link encap:Ethernet  HWaddr 30:5B:D6:F1:D6:F8  
              inet6 addr: fe80::325b:d6ff:fef1:d6f8/64 Scope:Link
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:8881 errors:0 dropped:0 overruns:0 frame:0
              TX packets:769 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:1000 
              RX bytes:700735 (684.3 KiB)  TX bytes:1470733 (1.4 MiB)
              Interrupt:40 

    lo        Link encap:Local Loopback  
              inet addr:127.0.0.1  Mask:255.0.0.0
              inet6 addr: ::1/128 Scope:Host
              UP LOOPBACK RUNNING  MTU:65536  Metric:1
              RX packets:0 errors:0 dropped:0 overruns:0 frame:0
              TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:0 
              RX bytes:0 (0.0 b)  TX bytes:0 (0.0 b)

    xenbr0    Link encap:Ethernet  HWaddr 30:5B:D6:F1:D6:F8  
              inet addr:10.80.236.237  Bcast:10.80.239.255  Mask:255.255.240.0
              inet6 addr: fe80::325b:d6ff:fef1:d6f8/64 Scope:Link
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:8104 errors:0 dropped:0 overruns:0 frame:0
              TX packets:535 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:0 
              RX bytes:630852 (616.0 KiB)  TX bytes:1442490 (1.3 MiB)
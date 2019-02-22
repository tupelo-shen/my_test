#!/bin/sh
module="scull"
device="scull"
mode="664"
# 调用insmod
# and use a pathname, as newer modutils don't look in . by default
/sbin/insmod ./$module.ko $* || exit 1
# 移除旧设备节点
rm -f /dev/${device}[0-3]
echo "test"
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3
# give appropriate group/permissions, and change the group.
# Not all distributions have staff, some have "wheel" instead.
group="root"
grep -q '^root:' /etc/group || group="wheel"
chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]
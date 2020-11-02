
# 1 by PryMar56

VirtSIG is working on it now, see #centos-virt on Freenode.

I've built the whole C8 virtualization stack 2 ways: xen-4.12.2 + qemu-3.01 and xen-4.13.0 + qemu-4.1.0. Of course, it all begins with the Xen build, then follows the qemu build, then libvirt, and so on.

Now take a look at the `virt module` that is part of AppStream repo and see that they started with qemu-kvm and left Xen out of the picture.

Any SRPM[^1] that is part of the `virt module` build is effectively out-of-bounds to a Xen stack build unless it is rebuilt without module tags.

I wrote up more on this here
[http://lists.ghettoforge.org/pipermail/users/2020-April/000013.html](http://lists.ghettoforge.org/pipermail/users/2020-April/000013.html)

I run Xen dom0 in C8 every day, but its using elrepo kernel-ml with python3 and the `virt module` is disabled during build and install.

# 2 by pwnyboy

Thanks. It appears they (VirtSIG) are in the 'built and testing' phase, as I can now see packages here:

[https://buildlogs.centos.org/centos/8/virt/x86_64/xen-412/Packages/x/](https://buildlogs.centos.org/centos/8/virt/x86_64/xen-412/Packages/x/)

but not on the public mirrors. Any idea where (and maybe when) completion will be announced, so I can keep checking back without having to read through scroll on IRC?

# 3 by PryMar56

if VirtSIG is waiting for the companion libvirt build, then it may take another 6 weeks or more. 

Like I mentioned before, the `virt module` is the immovable object, which now requires us to apply the irresistible force of redesign.

The libvirt build has a BR on `/usr/bin/qemu-img`. It also has a similar requires for 2 of the subpackages. This is the spoiler which can and will `throw a wrench into the fan.`

== dnf provides ==
admin@c8xen:~$ sudo dnf provides '/usr/bin/qemu-img'
qemu-img-15:2.12.0-88.module_el8.1.0+248+298dec18.x86_64 : QEMU command line tool for manipulating disk images
Repo : AppStream
Matched from:
Filename : /usr/bin/qemu-img

qemu-img-15:2.12.0-88.module_el8.1.0+258+1d2a1d58.1.x86_64 : QEMU command line tool for manipulating disk images
Repo : AppStream
Matched from:
Filename : /usr/bin/qemu-img

qemu-img-15:2.12.0-88.module_el8.1.0+266+ba744077.2.x86_64 : QEMU command line tool for manipulating disk images
Repo : AppStream
Matched from:
Filename : /usr/bin/qemu-img

qemuu-1:3.0.1+3-1.el8.x86_64 : Qemu upstream runtime environment
Repo : @System
Matched from:
Filename : /usr/bin/qemu-img
== snip provides ==

Notice the 3 */qemu-img provides coming from modules! This is why the VIrtSIG qemu-xen build must provide it too. They can do it
with a symlink. If they don't, then their libvirt builddep will import a module and the whole thing goes out-of-bounds.

Their qemu-xen build also needs this `conflicts`.
admin@c8xen:~$ sudo rpm -qi --conflicts qemuu
Name : qemuu
Epoch : 1
Version : 3.0.1+3
Release : 1.el8
Architecture: x86_64
Install Date: Mon 18 May 2020 11:31:34 AM PDT
snip
Source RPM : xen-4.12.3-1.el8.src.rpm
Build Date : Mon 18 May 2020 11:11:22 AM PDT
snip
Description :
This package contains the runtime programs which
form the qemuu part of the core Xen userspace environment.
qemu-img = 15:2.12.0
qemu-kvm-core = 0:4.5.0
qemu-system-x86

Without the provides on */qemu-img and conflicts against the module versions, the `virt module` will stop the whole project.

[^1]: RPM，英文全称是，`RedHat Package Manager`。顾名思义，这个套件管理程序是由红帽这家公司开发的。SRPM是RPM的升级版，提供的是源码。
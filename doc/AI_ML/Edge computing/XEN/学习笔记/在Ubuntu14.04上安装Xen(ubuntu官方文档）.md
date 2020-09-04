[TOC]

# 1 Introduction

The Xen Project hypervisor is an open-source type-1 or baremetal hypervisor, which makes it possible to run many instances of an operating system or indeed different operating systems in parallel on a single machine (or host). The Xen Project hypervisor is the only type-1 hypervisor that is available as open source. It is used as the basis for a number of different commercial and open source applications, such as: server virtualization, Infrastructure as a Service (IaaS), desktop virtualization, security applications, embedded and hardware appliances. The Xen Project hypervisor is powering the largest clouds in production today.[^1]

As of Ubuntu 11.10 (Oneiric), the default kernel included in Ubuntu can be used directly with the Xen hypervisor as the management (or control) domain (Dom0 or Domain0 in Xen terminology).

The rest of this guide gives a basic overview of how to set up a basic Xen system and create simple guests. Our example uses LVM for virtual disks and network bridging for virtual network cards. It also assumes Xen 4.4 (the version available in 14.04) and the xl toolstack. It assumes a familiarity with general virtualization issues, as well as with the specific Xen terminology. Please see the Xen wiki for more information.

# 2 During installation of Ubuntu

During the install of Ubuntu for the partitioning method choose "Guided - use the entire disk and setup LVM". Then, when prompted to enter "Amount of volume group to use for guided partitioning:" Enter a value just large enough for the Xen Dom0 system, leaving the rest for virtual disks. Enter a value smaller than the size of your installation drive. For example 10 GB or even 5 GB should be large enough for a minimal Xen Dom0 system. Entering a percentage of maximum size (e.g. 25%) is also a reasonable choice.

# 3 Installing Xen

Install a 64-bit hypervisor. (A 64-bit hypervisor works with a 32-bit dom0 kernel, but allows you to run 64-bit guests as well.)

    $ sudo apt-get install xen-hypervisor-amd64

As of Ubuntu 14.04, GRUB will automatically choose to boot Xen first if Xen is installed. If you're running a version of Ubuntu before 14.04, you'll have to modify GRUB to default booting to Xen; see below for details.

Now reboot:

    $ sudo reboot

And then verify that the installation has succeeded:

    $ sudo xl list
    Name                                        ID   Mem VCPUs      State   Time(s)
    Domain-0                                     0   945     1     r-----      11.3

# 4 Network Configuration

> It's assumed that you are using a wired interface for your network configuration. WiFi networks are tricky to virtualize and many times not even possible. If you are feeling adventurous, please start here [Xen in WiFi Networks](http://wiki.xen.org/wiki/Xen_in_WiFi_networks).

For this example, we will use the most common Xen network setup: bridged. You will also find an example on how to set up [Open vSwitch](http://openvswitch.org/) which has been available since Xen 4.3.

## 4.1 Disable Network Manager

If you are using Network Manager to control your internet connections, then you must first disable it as we will be manually configuring the connections. Please note that you are about to temporarily lose your internet connection so it's important that you are physically connected to the machine.

As documented in [NetworkManager](https://help.ubuntu.com/community/NetworkManager):

    $ sudo stop network-manager
    $ echo "manual" | sudo tee /etc/init/network-manager.override

## 4.2 Using bridge-utils

    $ sudo apt-get install bridge-utils

In a bridged setup, it is required that we assign the IP address to the bridged interface. Configure network interfaces so that they persist after reboot:

    $ sudo vi /etc/network/interfaces

    auto lo eth0 xenbr0
    iface lo inet loopback

    iface xenbr0 inet dhcp
      bridge_ports eth0

    iface eth0 inet manual

Restart networking to enable xenbr0 bridge:

    $ sudo ifdown eth0 && sudo ifup xenbr0 && sudo ifup eth0

The brctl command is useful for providing addition bridge information. See: man brctl

## 4.3 Using Open vSwitch

    $ sudo apt-get install openvswitch-switch

In a bridged setup, it is required that we assign the IP address to the bridged interface. Configure network interfaces so that they persist after reboot:

    $ sudo vi /etc/network/interfaces

    # interfaces(5) file used by ifup(8) and ifdown(8)
    auto lo eth0
    iface lo inet loopback

    iface eth0 inet manual

    allow-hotplug ovsbr0
    iface ovsbr0 inet dhcp

Set up Open vSwitch

    $ sudo ovs-vsctl add-br ovsbr0
    $ sudo ovs-vsctl set Bridge ovsbr0 stp_enable=false other_config:stp-max-age=6 other_config:stp-forward-delay=4
    $ sudo ovs-vsctl list Bridge
    $ sudo ovs-vsctl add-port ovsbr0 eth0

Now bring the interfaces up and acquire an IP address through DHCP. You should have your internet connection back at this point.

    $ sudo ip link set eth0 up
    $ sudo dhclient ovsbr0

## 4.4 Recommended Bridge Settings

For performance and security reasons, it's highly recommended[^2] that netfilter is disabled on all bridges.

    $ sudo vi /etc/sysctl.conf

    net.bridge.bridge-nf-call-ip6tables = 0
    net.bridge.bridge-nf-call-iptables = 0
    net.bridge.bridge-nf-call-arptables = 0

    $ sudo sysctl -p /etc/sysctl.conf
    # Note: These settings are created in /proc/sys/net. The bridge folder only appears to be created after first creating a bridge with the ''brctl' command.

# 5 Creating vms

There are many options for installing guest images:

1. [virt-builder](http://libguestfs.org/): A program for building VM disk images; part of the libguestfs set of tools

2. [xen-tools](http://xen-tools.org/): A set of scripts for creating various PV guests

3. [virt-manager](http://virt-manager.org/): A management system using libvirt

4. Converting an existing installation

5. Downloading pre-build guest images (e.g. [http://wiki.xen.org/wiki/Guest_VM_Images](http://wiki.xen.org/wiki/Guest_VM_Images))

Or you can manually create one, as described below.

## 5.1 Manually Create a PV Guest VM

In this section we will focus on Paravirtualized (or PV) guests. PV guests are guests that are made Xen-aware and therefore can be optimized for Xen.

As a simple example we'll create a PV guest in LVM logical volume (LV) by doing a network installation of Ubuntu (other distros such as Debian, Fedora, and CentOS can be installed in a similar way).

List your existing volume groups (VG) and choose where you'd like to create the new logical volume.

    $ sudo vgs

Create the logical volume (LV).

    $ sudo lvcreate -L 10G -n lv_vm_ubuntu /dev/<VGNAME>

Confirm that the new LV was successfully created.

    $ sudo lvs

### 5.1.1 Get Netboot Images

Choose an archive mirror [https://launchpad.net/ubuntu/+archivemirrors](https://launchpad.net/ubuntu/+archivemirrors).

    $ sudo mkdir -p /var/lib/xen/images/ubuntu-netboot/trusty14LTS
    $ cd /var/lib/xen/images/ubuntu-netboot/trusty14LTS
    $ wget http://<mirror>/ubuntu/dists/trusty/main/installer-amd64/current/images/netboot/xen/vmlinuz
    $ wget http://<mirror>/ubuntu/dists/trusty/main/installer-amd64/current/images/netboot/xen/initrd.gz

### 5.1.2 Set Up Initial Guest Configuration

    $ cd /etc/xen
    $ cp xlexample.pvlinux ubud1.cfg
    $ vi ubud1.cfg

    name = "ubud1"

    kernel = "/var/lib/xen/images/ubuntu-netboot/trusty14LTS/vmlinuz"
    ramdisk = "/var/lib/xen/images/ubuntu-netboot/trusty14LTS/initrd.gz"
    #bootloader = "/usr/lib/xen-4.4/bin/pygrub"

    memory = 1024
    vcpus = 1

    # Custom option for Open vSwitch
    vif = [ 'script=vif-openvswitch,bridge=ovsbr0' ]

    disk = [ '/dev/<VGNAME>/lv_vm_ubuntu,raw,xvda,rw' ]

    # You may also consider some other options
    # [[http://xenbits.xen.org/docs/4.4-testing/man/xl.cfg.5.html]]

Start the VM and connect to the console to perform the install.

    $ sudo xl create -c /etc/xen/ubud1.cfg

Once installed and back to command line, modify guest configuration to use the pygrub bootloader. These lines will change

    $ vi /etc/xen/ubud1.cfg

    #kernel = "/var/lib/xen/images/ubuntu-netboot/trusty14LTS/vmlinuz"
    #ramdisk = "/var/lib/xen/images/ubuntu-netboot/trusty14LTS/initrd.gz"
    bootloader = "/usr/lib/xen-4.4/bin/pygrub"

Now let's restart the VM with the new bootloader. (If the VM didn't shutdown after the install above, you may manually shut it down.)

    $ sudo xl shutdown ubud1
    $ sudo xl create -c /etc/xen/ubud1.cfg

### 5.1.3 Quick XL Console Tips

Connect to the VM console

    $ sudo xl console ubud1

Disconnect from the console

    Ctrl+]

### 5.2.1 Manually installing an HVM Guest VM

Download Install ISO.

[http://www.ubuntu.com/download/desktop](http://www.ubuntu.com/download/desktop)

    sudo pvs

choose your VG. Create a LV

    sudo lvcreate -L 4G -n ubuntu-hvm /dev/<VG>

Create a guest config file /etc/xen/ubuntu-hvm.cfg

    builder = "hvm"
    name = "ubuntu-hvm"
    memory = "512"
    vcpus = 1
    vif = ['']
    disk = ['phy:/dev/<VG>/ubuntu-hvm,hda,w','file:/root/ubuntu-12.04-desktop-amd64.iso,hdc:cdrom,r']
    vnc = 1
    boot="dc"

    xl create /etc/xen/ubuntu-hvm.cfg
    vncviewer localhost:0

After the install you can optionally remove the CDROM from the config and/or change the boot order.

For example `/etc/xen/ubuntu-hvm.cfg`:

    builder = "hvm"
    name = "ubuntu-hvm"
    memory = "512"
    vcpus = 1
    vif = ['']
    #disk = ['phy:/dev/<VG>/ubuntu-hvm,hda,w','file:/root/ubuntu-12.04-server-amd64.iso,hdc:cdrom,r']
    disk = ['phy:/dev/<VG>/ubuntu-hvm,hda,w']
    vnc = 1
    boot="c"
    #boot="dc"

# 6 Xen Toolstack Choices

[http://wiki.xen.org/wiki/Choice_of_Toolstacks](http://wiki.xen.org/wiki/Choice_of_Toolstacks)

## 6.1 Xen and xl

xl is a new toolstack written from the ground up to be a replacement for xend and xm. In Xen 4.4, xl is the default toolstack and xend is deprecated. It is planned to be removed in Xen 4.5.

xl is designed to be command-for-command compatible with xm. You should be able to use the same config file and the same commands you're used to; just use "xl" instead of "xm".

To switch back to xm, do the following:

    sudo sed -i 's/TOOLSTACK=.*\+/TOOLSTACK="xm"/' /etc/default/xen
    sudo reboot
    sudo xm list

xl and xm are very similar in functionality with a few notable exceptions: [http://wiki.xen.org/wiki/XL](http://wiki.xen.org/wiki/XL)

## 6.2 Xen and Libvirt

Ubuntu 14.04 contains libvirt 1.2.2, which contains support for Xen, both libxl and xend. If you specify "xen:///" as the hypervisor, it will automatically detect which is the appropriate method to use.

    sudo apt-get install virtinst

    sudo virt-install --connect=xen:/// --name u14.04 --ram 1024 --disk u14.04.img,size=4 --location http://ftp.ubuntu.com/ubuntu/dists/trusty/main/installer-amd64/

## 6.3 Xen and XAPI

1. [http://packages.ubuntu.com/precise/xcp-xapi](http://packages.ubuntu.com/precise/xcp-xapi)

2. [http://manpages.ubuntu.com/manpages/precise/man1/xapi.1.html](http://manpages.ubuntu.com/manpages/precise/man1/xapi.1.html)

3. [http://manpages.ubuntu.com/manpages/precise/man1/xe.1.html](http://manpages.ubuntu.com/manpages/precise/man1/xe.1.html)

## 6.4 Other tips and tricks

Create and format disk image file

    sudo mkdir -p /var/lib/xen/images
    sudo dd if=/dev/zero of=/var/lib/xen/images/ubuntu-guest.img bs=1M seek=3096 count=0
    sudo mkfs.ext4 -F /var/lib/xen/images/ubuntu-guest.img

## 6.5 Xen and Grub on older versions of Ubuntu

Modify GRUB to default to booting Xen ("Xen 4.1-amd64" should be replaced with the appropriate name, in 12.10 the line is "Ubuntu GNU/Linux, with Xen hypervisor". The current string can be obtained by looking for one of the menuentry lines in /boot/grub/grub.cfg. In theory the first element created by the 20_linux_xen script):

    sudo sed -i 's/GRUB_DEFAULT=.*\+/GRUB_DEFAULT="Xen 4.1-amd64"/' /etc/default/grub
    sudo update-grub

# 7 See Also

1. [http://wiki.xen.org/wiki/XenPCIpassthrough](http://wiki.xen.org/wiki/XenPCIpassthrough)

2. [http://wiki.xen.org/wiki/Xen_VGA_Passthrough](http://wiki.xen.org/wiki/Xen_VGA_Passthrough)

# 8 External Links

1. [http://wiki.xen.org/wiki/Debian_Guest_Installation_Using_Debian_Installer](http://wiki.xen.org/wiki/Debian_Guest_Installation_Using_Debian_Installer) - Netboot installation of PV guests
2. [http://wiki.xen.org/wiki/HostConfiguration/Networking](http://wiki.xen.org/wiki/HostConfiguration/Networking) - Networking configuration details from Xen.org wiki
3. [http://libvirt.org/uri.html#URI_file](http://libvirt.org/uri.html#URI_file) - Libvirt xend configuration
4. [http://wiki.xen.org/wiki/Xen_Man_Pages](http://wiki.xen.org/wiki/Xen_Man_Pages) - Xen Man pages
5. [http://xenbits.xen.org/docs/unstable/man/xmdomain.cfg.5.html](http://xenbits.xen.org/docs/unstable/man/xmdomain.cfg.5.html) - xm config options
6. [http://xenbits.xen.org/docs/unstable/man/xl.cfg.5.html](http://xenbits.xen.org/docs/unstable/man/xl.cfg.5.html) xl config options
7. [http://xenbits.xen.org/docs/unstable/misc/xl-disk-configuration.txt](http://xenbits.xen.org/docs/unstable/misc/xl-disk-configuration.txt) xl disk configuration
8. [http://serverfault.com/questions/390373/xen-4-1-host-dom0-with-blktap-disks-tapaio-not-connecting](http://serverfault.com/questions/390373/xen-4-1-host-dom0-with-blktap-disks-tapaio-not-connecting) blktap issues and fixes.

[^1]: Xen Wiki: [What is the Xen Project Hypervisor](http://wiki.xen.org/wiki/Xen_Overview#What_is_the_Xen_Project_Hypervisor.3F)

[^2]: Recommended Bridge Settings: [Network Configuration Examples](http://wiki.xen.org/wiki/Network_Configuration_Examples_(Xen_4.1%2B)) (2)
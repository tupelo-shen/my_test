[TOC]

This is a really useful development environment when you want everything running on a single box.

The setup includes:

* Virtual Box on your laptop or desktop

* XenServer running in a Virtual Box VM

* OpenStack running in a VM on XenServer

* Launching a VM on XenServer using OpenStack

# 1 Install Virtual Box

You can get Virtual Box from your favourite package manager, or from here: https://www.virtualbox.org/wiki/Downloads

Xen requires a 64-bit CPU, so make sure that you install, on a 64-bit operating system, the 64-bit version of Virtual Box.

# 2 Networking

As with any XenServer + OpenStack + DevStack deployment, it is worth getting your head straight around the networking.

While there are many options that could work, please try the recommended approach first. It will really help you understand what is possible.

The XenServer VM running on Virtual Box:

* a single bridged adapter onto your primary network

    - NAT is default, but not as useful
    - This will make XenServer appear on your network

* the above network is assumed to have DHCP (like that provided by a home router)

The Ubuntu VM running on XenServer:

* Always gets four interfaces
    
    - eth0: private networking with the XenServer, best to ignore this one
    - eth1: VM traffic network
    - eth2: management network (rabbit, mysql, keystone, etc)
    - eth3: public network (floating ip traffic, external access to APIs)

For a nice picture see the official docs: http://docs.openstack.org/folsom/openstack-compute/admin/content/introduction-to-xen.html#xenapi-deployment-architecture

# 3 Installing XenServer

First download the [XenServer](https://wiki.openstack.org/wiki/XenServer) ISO onto the machine running virtual box from the [XenServer download site](http://xenserver.org/overview-xenserver-open-source-virtualization/download.html)

Once complete you need to create a Virtual Box VM with:

* cd drive attached to the XenServer ISO

* a bridged network interface onto a network

    - you will need "Promiscuous Mode" at "Allow All" on the NIC to ensure the traffic for VMs running inside the XenServer VM will be allowed
    - ideally with a DHCP server and an internet connection (just like most home router networks)

* More than 2GB RAM (Dom0 takes 800MB, DomU takes 1GB+ and then some RAM needed to start VMs)

* enable the following motherboard options:

    - Enable IO APIC

    - Hardware clock in UTC

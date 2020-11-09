[参考地址](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/lvm_components)：https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/lvm_components

[TOC]

# 1 INTRODUCTION

This book describes the Logical Volume Manager (LVM), including information on running LVM in a clustered environment.

## 1.1 Audience

This book is intended to be used by system administrators managing systems running the Linux operating system. It requires familiarity with Red Hat Enterprise Linux 6.

## 1.2 SOFTWARE VERSIONS

Table 1.1. Software Versions

| Software | Description |
| -------- | ----------- |
| Red Hat Enterprise Linux 6 | refers to Red Hat Enterprise Linux 6 and higher |
| GFS2 | refers to GFS2 for Red Hat Enterprise Linux 6 and higher |

## 1.3 RELATED DOCUMENTATION

For more information about using Red Hat Enterprise Linux, see the following resources:

* Installation Guide — Documents relevant information regarding the installation of Red Hat Enterprise Linux 6.
* Deployment Guide — Documents relevant information regarding the deployment, configuration and administration of Red Hat Enterprise Linux 6.
* Storage Administration Guide — Provides instructions on how to effectively manage storage devices and file systems on Red Hat Enterprise Linux 6.

For more information about the High Availability Add-On and the Resilient Storage Add-On for Red Hat Enterprise Linux 6, see the following resources:

* High Availability Add-On Overview — Provides a high-level overview of the Red Hat High Availability Add-On.
* Global File System 2: Configuration and Administration — Provides information about installing, configuring, and maintaining Red Hat GFS2 (Red Hat Global File System 2), which is included in the Resilient Storage Add-On.
* DM Multipath — Provides information about using the Device-Mapper Multipath feature of Red Hat Enterprise Linux 6.
* Load Balancer Administration — Provides information on configuring high-performance systems and services with the Load Balancer Add-On, a set of integrated software components that provide Linux Virtual Servers (LVS) for balancing IP load across a set of real servers.
* Release Notes — Provides information about the current release of Red Hat products.

Red Hat documents are available in HTML, PDF, and RPM versions on the Red Hat Enterprise Linux Documentation CD and online at https://access.redhat.com/site/documentation/.

# 2 THE LVM LOGICAL VOLUME MANAGER

This chapter provides a summary of the features of the LVM logical volume manager that are new for the initial and subsequent releases of Red Hat Enterprise Linux 6. Following that, this chapter provides a high-level overview of the components of the Logical Volume Manager (LVM).

## 2.1. New and Changed Features

This section lists new and changed features of the LVM logical volume manager that are included with the initial and subsequent releases of Red Hat Enterprise Linux 6.

### 2.1.1. New and Changed Features for Red Hat Enterprise Linux 6.0

Red Hat Enterprise Linux 6.0 includes the following documentation and feature updates and changes.

* You can define how a mirrored logical volume behaves in the event of a device failure with the mirror_image_fault_policy and mirror_log_fault_policy parameters in the activation section of the lvm.conf file. When this parameter is set to remove, the system attempts to remove the faulty device and run without it. When this parameter is set to allocate, the system attempts to remove the faulty device and tries to allocate space on a new device to be a replacement for the failed device; this policy acts like the remove policy if no suitable device and space can be allocated for the replacement. For information on the LVM mirror failure policies, see [Section 5.4.3.1, “Mirrored Logical Volume Failure Policy”](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/mirror_create#mirror_failure_policy).

* For the Red Hat Enterprise Linux 6 release, the Linux I/O stack has been enhanced to process vendor-provided I/O limit information. This allows storage management tools, including LVM, to optimize data placement and access. This support can be disabled by changing the default values of data_alignment_detection and data_alignment_offset_detection in the lvm.conf file, although disabling this support is not recommended.

    For information on data alignment in LVM as well as information on changing the default values of data_alignment_detection and data_alignment_offset_detection, see the inline documentation for the /etc/lvm/lvm.conf file, which is also documented in [Appendix B, The LVM Configuration Files](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/config_file). For general information on support for the I/O Stack and I/O limits in Red Hat Enterprise Linux 6, see the Storage Administration Guide.

* In Red Hat Enterprise Linux 6, the Device Mapper provides direct support for udev integration. This synchronizes the Device Mapper with all udev processing related to Device Mapper devices, including LVM devices. For information on Device Mapper support for the udev device manager, see [Section A.3, “Device Mapper Support for the udev Device Manager”](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/udev_device_manager).

* For the Red Hat Enterprise Linux 6 release, you can use the `lvconvert --repair` command to repair a mirror after disk failure. This brings the mirror back into a consistent state. For information on the `lvconvert --repair` command, see Section 5.4.3.3, “Repairing a Mirrored Logical Device”.

* As of the Red Hat Enterprise Linux 6 release, you can use the `--merge` option of the `lvconvert` command to merge a snapshot into its origin volume. For information on merging snapshots, see [Section 5.4.8, “Merging Snapshot Volumes”](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/snapshot_merge).

* As of the Red Hat Enterprise Linux 6 release, you can use the --splitmirrors argument of the lvconvert command to split off a redundant image of a mirrored logical volume to form a new logical volume. For information on using this option, see [Section 5.4.3.2, “Splitting Off a Redundant Image of a Mirrored Logical Volume”](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/mirror_split).

* You can now create a mirror log for a mirrored logical device that is itself mirrored by using the `--mirrorlog mirrored` argument of the `lvcreate` command when creating a mirrored logical device. For information on using this option, see [Section 5.4.3, “Creating Mirrored Volumes”](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/logical_volume_manager_administration/mirror_create).

### 2.1.2. New and Changed Features for Red Hat Enterprise Linux 6.1


## 2.2 LOGICAL VOLUMES

Volume management creates a layer of abstraction over physical storage, allowing you to create logical storage volumes. This provides much greater flexibility in a number of ways than using physical storage directly. With a logical volume, you are not restricted to physical disk sizes. In addition, the hardware storage configuration is hidden from the software so it can be resized and moved without stopping applications or unmounting file systems. This can reduce operational costs.

Logical volumes provide the following advantages over using physical storage directly:

* Flexible capacity

    When using logical volumes, file systems can extend across multiple disks, since you can aggregate disks and partitions into a single logical volume.

* Resizeable storage pools

    You can extend logical volumes or reduce logical volumes in size with simple software commands, without reformatting and repartitioning the underlying disk devices.

* Online data relocation

    To deploy newer, faster, or more resilient storage subsystems, you can move data while your system is active. Data can be rearranged on disks while the disks are in use. For example, you can empty a hot-swappable disk before removing it.

* Convenient device naming

    Logical storage volumes can be managed in user-defined groups, which you can name according to your convenience.

* Disk striping

    You can create a logical volume that stripes data across two or more disks. This can dramatically increase throughput.

* Mirroring volumes

    Logical volumes provide a convenient way to configure a mirror for your data.

* Volume Snapshots

    Using logical volumes, you can take device snapshots for consistent backups or to test the effect of changes without affecting the real data.


The implementation of these features in LVM is described in the remainder of this document.

## 2.3. LVM架构概述

For the Red Hat Enterprise Linux 4 release of the Linux operating system, the original LVM1 logical volume manager was replaced by LVM2, which has a more generic kernel framework than LVM1. LVM2 provides the following improvements over LVM1:

* flexible capacity
* more efficient metadata storage
* better recovery format
* new ASCII metadata format
* atomic changes to metadata
* redundant copies of metadata

LVM2 is backwards compatible with LVM1, with the exception of snapshot and cluster support. You can convert a volume group from LVM1 format to LVM2 format with the `vgconvert` command. For information on converting LVM metadata format, see the `vgconvert(8)` man page.

The underlying physical storage unit of an LVM logical volume is a block device such as a partition or whole disk. This device is initialized as an LVM physical volume (PV).

To create an LVM logical volume, the physical volumes are combined into a volume group (VG). This creates a pool of disk space out of which LVM logical volumes (LVs) can be allocated. This process is analogous to the way in which disks are divided into partitions. A logical volume is used by file systems and applications (such as databases).

Figure 2.1, “LVM Logical Volume Components” shows the components of a simple LVM logical volume:

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/basic-lvm-volume_2_1.png">

For detailed information on the components of an LVM logical volume, see Chapter 3, LVM Components.

## 2.4. THE CLUSTERED LOGICAL VOLUME MANAGER (CLVM)

The Clustered Logical Volume Manager (CLVM) is a set of clustering extensions to LVM. These extensions allow a cluster of computers to manage shared storage (for example, on a SAN) using LVM. CLVM is part of the Resilient Storage Add-On.

Whether you should use CLVM depends on your system requirements:

* If only one node of your system requires access to the storage you are configuring as logical volumes, then you can use LVM without the CLVM extensions and the logical volumes created with that node are all local to the node.

* If you are using a clustered system for failover where only a single node that accesses the storage is active at any one time, you should use High Availability Logical Volume Management agents (HA-LVM).

* If more than one node of your cluster will require access to your storage which is then shared among the active nodes, then you must use CLVM. CLVM allows a user to configure logical volumes on shared storage by locking access to physical storage while a logical volume is being configured, and uses clustered locking services to manage the shared storage.

# 3 LVM COMPONENTS

This chapter describes the components of an LVM Logical volume.

# 3.1 物理卷

The underlying physical storage unit of an LVM logical volume is a block device such as a partition or whole disk. To use the device for an LVM logical volume the device must be initialized as a physical volume (PV). Initializing a block device as a physical volume places a label near the start of the device.

By default, the LVM label is placed in the second 512-byte sector. You can overwrite this default by placing the label on any of the first 4 sectors. This allows LVM volumes to co-exist with other users of these sectors, if necessary.

An LVM label provides correct identification and device ordering for a physical device, since devices can come up in any order when the system is booted. An LVM label remains persistent across reboots and throughout a cluster.

The LVM label identifies the device as an LVM physical volume. It contains a random unique identifier (the UUID) for the physical volume. It also stores the size of the block device in bytes, and it records where the LVM metadata will be stored on the device.

The LVM metadata contains the configuration details of the LVM volume groups on your system. By default, an identical copy of the metadata is maintained in every metadata area in every physical volume within the volume group. LVM metadata is small and stored as ASCII.

Currently LVM allows you to store 0, 1 or 2 identical copies of its metadata on each physical volume. The default is 1 copy. Once you configure the number of metadata copies on the physical volume, you cannot change that number at a later time. The first copy is stored at the start of the device, shortly after the label. If there is a second copy, it is placed at the end of the device. If you accidentally overwrite the area at the beginning of your disk by writing to a different disk than you intend, a second copy of the metadata at the end of the device will allow you to recover the metadata.

For detailed information about the LVM metadata and changing the metadata parameters, see Appendix E, LVM Volume Group Metadata.

### 3.1.1. LVM Physical Volume Layout

Figure 3.1, “Physical Volume layout” shows the layout of an LVM physical volume. The LVM label is on the second sector, followed by the metadata area, followed by the usable space on the device.

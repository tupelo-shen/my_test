[TOC]

# Goals

* The Virt-SIG aims to deliver a user consumable full stack for virtualization technologies that want to work with the SIG. This includes delivery, deployment, management, update and patch application (for full lifecycle management) of the baseline platform when deployed in sync with a technology curated by the Virt-SIG.

* The delivery stage would be executed via multiple online, updated yum repositories to complement ISO based delivery of install media, livecd/livedvd/livdusb media etc.

* The SIG will also attempt to maintain user grade documentation around the technologies we curate, along with represent the Virt-SIG at CentOS Dojos and related events in community and vendor neutral events.

* The SIG will only work with opensource, redistributable software. However some of the code we want to build and ship might not be mainline accepted as yet, and would be clearly indicated as such.

The CentOS Virt-SIG wants to build upon the already successfully released [Xen4CentOS](https://wiki.centos.org/Manuals/ReleaseNotes/Xen4-01) project. Moving forward the Xen4CentOS codebase will be shifted over to the Virt-SIG resources. We will also need to look at choosing versions of common components, such as qemu, libvirt, etc. that will work for interfacing SIGs and other virtualization components. It would probably be a good idea to carrying the latest KVM for those who want it (but ideally someone from the KVM community would step up and help with the work).

引用：

* [CentOS Virt SIG Proposal](https://wiki.centos.org/SpecialInterestGroup/Virtualization/Proposal)

# 2 Project

* Xen

    - [Xen4QuickStart](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart) Xen4CentOS quick-start guide
    - [Xen4CentOS](https://wiki.centos.org/Manuals/ReleaseNotes/Xen4-01) General introduction
    - [Xen4Libvirt](https://wiki.centos.org/HowTos/Xen/Xen4QuickStart/Xen4Libvirt) On using libvirt with the Xen4CentOS packages

* Docker

    - [Docker](https://wiki.centos.org/Cloud/Docker) Instructions for CS6, CS7 with general notes

* oVirt

    - [oVirt](https://wiki.centos.org/HowTos/oVirt) Introduction and instructions for testing


# 3 注意事项

* 2018-11-13 - Xen 4.6 is out of security support now
* 2018-11-13 - Xen 4.10 packages are being built
* 2018-11-13 - oVirt conference will be held in Milan on 2018-11-16, more info at http://www.ovirt-italia.it/2018/10/ovirt-conference-2018.html

* 2018-11-13 - libvirt is being rebased on upstream 4.9.0
* 2018-11-13 - oVirt packages are being rebased on upstream 4.2.7 for CentOS 7.6
* 2018-11-13 - Ansible is being rebased on version 2.7.1 for CentOS 7.6
* 2018-11-13 - qemu-kvm-ev is being rebased on version 2.12 for CentOS 7.6

# 4 Notes on package to be released

Packages are only signed / pushed Monday through Thursday (so as to not surprise the userbase with content on the weekend). Packages tagged for release after 9am UTC on Thursday will not be released to mirrors till Monday 11am UTC.

# 5 Mailing List

This SIG mailing list ([centos-virt](http://lists.centos.org/mailman/listinfo/centos-virt)) is open and can be joined freely.

IMPORTANT NOTE: if you are not subscribed to the list, posts to the list are auto-discarded! This is a long-standing and established mode of operation of all of the CentOS Project's mailing lists.

# 6 SIG 成员

SIG members are appointed by the SIG team leader, and maintain certain resources related to the SIG (e.g. documentation in the Wiki or packages). Please contact SIG members via the centos-virt list.

* Lead: GeorgeDunlap (irc:gwd)

* Xen Maintainer: GeorgeDunlap (irc:gwd)

* Xen Maintainer: Anthony Perard (irc:anthonyper)

* oVirt Maintainer: SandroBonazzola (irc:sbonazzo)

* oVirt Maintainer: Rafael Martins (irc:rafaelmartins)

* oVirt Maintainer: Tomasz Baranski (irc:tomo)

* Docker Maintainer: Lokesh Mandvekar (irc:lsm5)

* Docker Maintainer: Navid Shaikh (irc:nshaikh)

* Libvirt Maintainer: Jiri Denemark (irc:jirkade)

# 7 会议时间

Meetings are held every two weeks (on even weeks) on Tuesday at 1500 UTC in #centos-devel.

* Meetings will be minuted and IRC sessions are logged
* To participate, just turn up on #centos-devel 5 minutes before the meeting and let us know you are there to participate
* Minutes of past meetings available on request

# 8 Roadmap and (Action) TODO list

* Roadmap: [roadmap](https://wiki.centos.org/SpecialInterestGroup/Virtualization/Roadmap)
* TODO List (Actions): [status](https://wiki.centos.org/SpecialInterestGroup/Virtualization/Status) page (Note: fold into roadmap at some point)

# 9 Information on getting Started within the SIG

* Join the meetings (see above)
* [FOSDEM 2015 presentations on SIGs](http://www.slideshare.net/xen_com_mgr/fosdem-cent-os-si-gs-lars?ref=http://www.xenproject.org/component/allvideoshare/video/latest/fosdem15-centos-virt-sig.html) and [video going with it](http://mirrors.dotsrc.org/fosdem/2015/devroom-virtualisation/immutable__CAM_ONLY.mp4)
* [Introduction to Community Build System](http://wiki.centos.org/Events/Dojo/Brussels2015?action=AttachFile&do=get&target=CBS-DOJOBRUSSELS2015.pdf)

# 9 Editing this page

All SIG members should be able to edit this page, if you are unable to do so, please get in touch on the centos-docs list or contact one of the existing members.
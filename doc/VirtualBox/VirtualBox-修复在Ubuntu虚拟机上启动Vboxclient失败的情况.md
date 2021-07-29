[TOC]

[https://websiteforstudents.com/how-to-fix-vboxclient-fail-to-start-error-on-ubuntu-virtalbox-guest-machines/](https://websiteforstudents.com/how-to-fix-vboxclient-fail-to-start-error-on-ubuntu-virtalbox-guest-machines/)

If you upgrade to VirtualBox 5.2 virtualization software, you may likely see an error that reads “**VboxClient: VboxClient (seamless) fail to start. Stage: Setting guest IRQ**” when you logon to a new `Ubuntu 16.04 | 17.10 | 18.04` virtual guest machine.

If you run into that issue, the steps below should be a great place to start to help  you resolve that problem…

This brief tutorial shows students and new users how to resolve VirtualBox guest machine error where the guest machine is unable to integrate properly with the host machine…

When you’re ready, continue below

## Step 1: Prepare Ubuntu Guest Machine

Normally after installing Ubuntu guest machines on VirtualBox host, your first task will be to install VirtualBox Guest Additions Tools.. Guest Addition Software is a suite of utilities that enhances the virtual machine’s operating systems and improves the management of the machine.

Below is the screenshot of the error you’ll get when you logon to Ubuntu guest machine installed on the new VirtualBox 5.2 host software…

<img src="">
In this case, installing the tools won’t help… so before installing the tool, run the commands below to install required packages for Ubuntu guest machines..


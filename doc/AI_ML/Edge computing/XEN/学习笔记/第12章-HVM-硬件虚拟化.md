[TOC]

Throughout this book, we’ve described the standard Xen virtualization technique, paravirtualization. Paravirtualization is a wonderful thing—as we’ve already outlined, it allows excellent performance and strong isolation, two goals that are difficult to achieve simultaneously. (See Chapter 1 for more on this topic.)

However, paravirtualization requires a modified operating system. Although porting an OS to Xen is relatively painless, by the standards of such things, it’s not a trivial task, and it has the obvious limitation of being impossible with closed source operating systems. (While the Xen team did port Windows to Xen during the development process, no released version of Windows can run under Xen in paravirtualized mode.)

One way around this is to add extensions to the processor so that it supports virtualization in hardware, allowing unmodified operating systems to run on the “bare metal,” yet in a virtualized environment. Both Intel and AMD have done precisely that by extending the x86 architecture.

Intel uses the term VT-x to refer to the virtualization extensions for x86[^1]. (VT-i is Itanium’s hardware virtualization. For our purposes, it’s basically identical to VT-x. We will not discuss VT-i separately.[^2]) AMD likewise has a set of virtualization extensions.[^3] Most of the Xen-related documentation that you might find refers to the extensions by their internal code name, Pacifica, but you’ll also see the AMD marketing term SVM, for secure virtual machine.

Although VT-x and Pacifica are implemented slightly differently, we can gloss over the low-level implementation details and focus on capabilities. Both of these are supported by Xen. Both will allow you to run an unmodified operating system as a domU. Both of these will suffer a significant performance penalty on I/O. Although there are differences between the two, the differences are hidden behind an abstraction layer.

Properly speaking, it’s this abstraction layer that we refer to as HVM (hardware virtual machine)—a cross-platform way of hiding tedious implementation details from the system administrator. So, in this chapter, we’ll focus on the HVM interface and how to use it rather than on the specifics of either Intel’s or AMD’s technologies.

# 1 Principles of HVM

If you think back to the “concentric ring” security model that we introduced in [Chapter 1](https://wiki.prgmr.com/mediawiki/index.php/Chapter_1:_Xen:_A_High-Level_Overview), you can characterize the HVM extensions as adding a ring −1 inside (that is, with superior privileges to) ring 0. New processor opcodes, invisible to the virtual machine, are used to switch between the superprivileged mode and normal mode. The unmodified operating system runs in ring 0 and operates as usual, without knowing that there’s another layer between it and the hardware. When it makes a privileged system call, the call actually goes to ring −1 rather than the actual hardware, where the hypervisor will intercept it, pause the virtual machine, perform the call, and then resume the domU when the call is done.

Xen also has to handle memory a bit differently to accommodate unmodified guests. Because these unmodified guests aren’t aware of Xen’s memory structure, the hypervisor needs to use shadow page tables that present the illusion of contiguous physical memory starting at address 0, rather than the discontiguous physical page tables supported by Xen-aware operating systems. These shadows are in-memory copies of the page tables used by the hardware, as shown in Figure 12-1. Attempts to read and write to the page tables are intercepted and redirected to the shadow. While the guest runs, it reads its shadow page tables directly, while the hardware uses the pretranslated version supplied to it by the hypervisor.

shadow page tables. When the execution context switches to the guest, the hypervisor translates pseudophysical addresses found in the shadow page tables to machine physical addresses and updates the hardware to use the translated page tables, which the guest then accesses directly.

## 1.1 Device Access with HVM

Of course, if you’ve been paying attention thus far, you’re probably asking how the HVM domain can access devices if it hasn’t been modified to use the Xen virtual block and network devices. Excellent question!

The answer is twofold: First, during boot, Xen uses an emulated BIOS to provide simulations of standard PC devices, including disk, network, and framebuffer. This BIOS comes from the open source Bochs emulator at http://bochs.sourceforge.net/. Second, after the system has booted, when the domU expects to access SCSI, IDE, or Ethernet devices using native drivers, those devices are emulated using code originally found in the QEMU emulator. A userspace program, qemu-dm, handles translations between the native and emulated models of device access.

## 1.2 HVM Device Performance

This sort of translation, where we have to mediate hardware access by breaking out of virtualized mode using a software device emulation and then reentering the virtualized OS, is one of the trade-offs involved in running unmodified operating systems.[^4] Rather than simply querying the host machine for information using a lightweight page-flipping system, HVM domains access devices precisely as if they were physical hardware. This is quite slow.

Both AMD and Intel have done work aimed at letting guests use hardware directly, using an IOMMU (I/O Memory Management Unit) to translate domain-virtual addresses into the real PCI address space, just as the processor’s MMU handles the translations for virtual memory.[^5] However, this isn’t likely to replace the emulated devices any time soon.

## 1.3 HVM and SMP

SMP (symmetric multiprocessing) works with HVM just as with paravirtualized domains. Each virtual processor has its own control structure, which can in turn be serviced by any of the machine’s physical processors. In this case, by physical processors we mean logical processors as seen by the machine, including the virtual processors presented by SMT (simultaneous multithreading or hyperthreading).

To turn on SMP, include the following in the config file:

    acpi=1
    vcpus=<n>

(Where n is an integer greater than one. A single CPU does not imply SMP. Quite the opposite, in fact.)

> <font color="blue"> Although you can specify more CPUs than actually exist in the box, performance will . . . suffer. We strongly advise against it.</font>

Just as in paravirtualized domains, SMP works by providing a VCPU abstraction for each virtual CPU in the domain, as shown in Figure 12-2. Each VCPU can run on any physical CPU in the machine. Xen’s CPU-pinning mechanisms also work in the usual fashion.

Unfortunately, SMP support isn’t perfect. In particular, time is a difficult problem with HVM and SMP. Clock synchronization seems to be entirely unhandled, leading to constant complaints from the kernel with one of our test systems (CentOS 5, Xen version 3.0.3-rc5.el5, kernel 2.6.18-8.el5xen). Here’s an example:

    Timer ISR/0: Time went backwards: delta=-118088543 delta_cpu=25911457 shadow=157034917204
    off=452853530 processed=157605639580 cpu_processed=157461639580

Figure 12-2: As each domain’s time allocation comes up, its VCPU’s processor state is loaded onto the PCPU for further execution. Privileged updates to the VCPU control structure are handled by the hypervisor.

One other symptom of the problem is in bogomips values reported by /proc/cpuinfo—on a 2.4GHz core 2 duo system, we saw values ranging from 13.44 to 73400.32. In the dom0, each core showed 5996.61—an expected value.

Don’t worry, this might be unsettling, but it’s also harmless.

## 1.4 HVM and Migration

HVM migration works as of Xen 3.1. The migration support in HVM domains is based on that for paravirtualized domains but is extended to account for the fact that it takes place without the connivance of the guest OS. Instead, Xen itself pauses the VCPUs, while xc_save handles memory and CPU context. qemu-dm also takes a more active role, saving the state of emulated devices.

The point of all this is that you can migrate HVM domains just like paravirtualized domains, using the same commands, with the same caveats. (In particular, remember that attempts to migrate an HVM domain to a physical machine that doesn’t support HVM will fail ungracefully.)

# 2 Xen HVM vs. KVM

Of course, if your machine supports virtualization in hardware, you might be inclined to wonder what the point of Xen is, rather than, say, KVM or lguest.

There are some excellent reasons to consider the idea. KVM and lguest are both easier to install and less invasive than Xen. They support strong virtualization with good performance.

However, KVM is, at the moment, less mature than Xen. It’s not (yet) as fast, even with the kernel accelerator. Xen also supports paravirtualization, whereas KVM does not. Xen PV offers a handy way of migrating domUs and a good way of multiplexing virtual machines—that is, expanding to a two-level VM hierarchy. Most importantly, though, Xen’s paravirtualized device drivers lead to much faster virtual machines—a feature that KVM, as of this writing, lacks.

Similarly, lguest is smaller, lighter, and easier to install than Xen, but it doesn’t support features like SMP or PAE (though 64-bit kernels are in development). Lguest also doesn’t yet support suspend, resume, or migration. Nonetheless, right now it’s difficult to say which is better—all of these technologies are out there being actively developed. If you are truly silly, you might even decide to use some combination, running Xen hypervisors under KVM, with paravirtualized domains under that. Or you might use Xen for now but keep your options open for future deployments, perhaps when HVM-capable hardware becomes more common. These technologies are interesting and worth watching, but we’ll stick to our usual “wait and see” policy.

Indeed, Red Hat has opted to do exactly that, focusing its development efforts on a platform-independent interface layer, libvirt, allowing (we hope) for easy migration between virtualization options. See [Chapter 6](https://wiki.prgmr.com/mediawiki/index.php/Chapter_6:_domU_Management:_Tools_and_Frontends) for more on libvirt and its associated suite of management tools.

# 3 Working with HVM

Regardless of what sort of hardware virtualization you want to use, the first thing to do is check whether your machine supports HVM. To find out if you’ve got a supported processor, check /proc/cpuinfo. Processors that support VT-x will report vmx in the flags field, while Pacifica-enabled processors report svm.

Even if you have a supported processor, many manufacturers have HVM turned off in the BIOS by default. Check xm dmesg to ensure that the hypervisor has found and enabled HVM support correctly—it should report “(XEN) VMXON is done” for each CPU in the machine. If it doesn’t, poke about in the BIOS for an option to turn on hardware virtualization. On our boards, it’s under chipset features and called VT Technology. Your machine may vary.

The hypervisor will also report capabilities under /sys:

    # cat /sys/hypervisor/properties/capabilities
    xen-3.0-x86_32p hvm-3.0-x86_32 hvm-3.0-x86_32p

In this case, the two “hvm” entries show that HVM is supported in both PAE and non-PAE modes.

> NOTE: One of the minor advantages of HVM is that it sidesteps the PAE-matching problem that we’ve been prone to harp on. You can run any mix of PAE and non-PAE kernels and hypervisors in HVM mode, although paravirtualized domains will still need to match PAE-ness, even on HVM-capable machines.

# 4 Creating an HVM Domain

When you’ve got the hypervisor and domain 0 running on an HVM-capable machine, creating an HVM domain is much like creating any Xen guest.

Here’s a sample HVM config file. (It’s got a snippet of Python at the beginning to set the appropriate library directory, borrowed from the sample configs distributed with Xen.)

    import os, re
    arch = os.uname()[4]
    if re.search('64', arch):
    arch_libdir = 'lib64'
    else:
    arch_libdir = 'lib'

    device_model = '/usr/' + arch_libdir + '/xen/bin/qemu-dm'

    kernel = "/usr/lib/xen/boot/hvmloader" builder='hvm'
    memory = 384
    shadow_memory = 16
    name = "florizel"
    vcpus = 1
    vif = [ 'type=ioemu, bridge=xenbr0' ]
    disk = [ 'file:/opt/florizel.img,hda,w', 'file:/root/slackware-12.0-install-dvd.iso,hdc:cdrom,r' ]

    boot="cda"

    sdl=0
    vnc=1
    stdvga=0
    serial='pty'

Most of this is pretty standard stuff. It starts with a snippet of Python to choose the correct version of qemu-dm, then it launches into a standard domU config. The config file changes for HVM guests are approximately as follows:

    builder = "HVM"
    device_model = "/usr/lib/xen/bin/qemu-dm"
    kernel = "/usr/lib/xen/boot/hvmloader"

> NOTE: There are other directives you can put in, but these are the ones that you can’t leave out.

Breaking this down, the domain builder changes from the default to HVM, while the devices change from the standard Xen paravirtualized devices to the QEMU emulated devices. Finally, the kernel line specifies an HVM loader that loads a kernel from within the HVM domain’s filesystem, rather than the Linux kernel that would be specified in a PV configuration.

We’re already familiar with the kernel line, of course. The device_model line, however, is new. This option defines a userspace program that handles mediation between real and virtual devices. As far as we know, qemu-dm is the only option, but there’s no reason that other device emulators couldn’t be written.

There are some other directives that are only used by HVM domains.

    shadow_memory = 16
    boot="dca"

The shadow_memory directive specifies the amount of memory to use for shadow page tables. (Shadow page tables, of course, are the aforementioned copies of the tables that map process-virtual memory to physical memory.) Xen advises allocating at least 2KB per MB of domain memory, and “a few” MB per virtual CPU. Note that this memory is in addition to the domU’s allocation specified in the memory line.

Finally we have the boot directive. The entire concept of boot order, of course, doesn’t apply to standard Xen paravirtualized domains because the domain config file specifies either a kernel or a bootloader. However, because HVM emulates legacy BIOS functions, including the traditional bootstrap, Xen provides a mechanism to configure the boot order.

In that vein, it’s worth noting that one advantage of HVM is that it can effectively duplicate the QEMU install procedure we’ve already described, with Xen instead of QEMU. To recap, this allows you to install in a strongly partitioned virtual machine, using the distro’s own install tools, and end with a ready-to-run VM. We’ll leave the details as an exercise to the reader, of course (we wouldn’t want to take all the fun out of it).

Now that you’ve got the config file written, create the domain as usual:

    # xm create florizel
    Using config file "./florizel".
    Started domain florizel
    # xm list
    Name                                ID Mem(MiB) VCPUs State  Time(s)
    Domain-0                             0     3458     2 r----- 5020.8
    florizel                             6      396     1 r----- 14.6

## 4.1 A DIGRESSION ON THE DOMAIN BUILDER

Up until now, we’ve avoided discussing the domain builder, being content merely to gesture in its direction and note that it builds domains. For most purposes, it’s enough to think of it as analogous to the standard bootloader.

However, it has a much more difficult and involved task than a normal bootloader. When a Xen domU starts, it comes up in a radically different environment from the traditional PC “real mode.” Because an operating system’s already loaded, the processor is already in protected mode with paging enabled. The job of the domain builder is to bring the new domain up to speed—to generate the mapping between domain-virtual and physical memory, load the VM kernel at the appropriate address, and install interrupt handlers.

The default builder is linux, which builds a paravirtualized domain. (Usually Linux, but it’ll work for most paravirtualized OSs.)

The situation changes somewhat with HVM because the unmodified operating system isn’t expecting to take off running on a fully booted machine. To keep the OS happy, the domain builder initializes a complete simulation of real mode, inserting hooks and installing an emulated BIOS into the appropriate regions of pseudophysical memory.

Historically, the domain builder’s been implementation-dependent, with a “vmx” builder for Intel and a similar “svm” builder for AMD. However, with the advent of HVM as an abstraction layer, the administrator can simply specify HVM and let Xen figure out the details.

## 4.2  Interacting with an HVM Domain

One of the first changes you might notice is that connecting to the console using xm -c doesn’t work. The Xen console requires some infrastructure support to get working, which a PV-oblivious standard distro naturally doesn’t have.

So, while the machine is booting, let’s chat for a bit about how you actually log in to your shiny new domain.

As you’re no doubt sick of us mentioning, HVM is founded on an idea of total hardware simulation. This means that when the machine boots, it loads an emulated VGA BIOS, which gets drawn into a graphics window.

Xen knows about two targets for its emulated VGA graphics: VNC and SDL. VNC is the familiar and well-beloved network windowing system from AT&T, while SDL, *Simple DirectMedia Layer*, is better known as a lightweight hardware-accelerated graphics option.

We opted to stick with the VNC console for most of our Linux domUs to reap the benefits of network-centric computing.[^6]

Now that the domain is created, use VNC to access its console:

    # vncviewer 127.0.0.1<./pre>

    (Or use whatever the IP address of the Xen machine is.) If you have more
    than one domU using the VNC console, append a display number—for
    example, to access the console of the second domU:

    <pre># vncviewer 127.0.0.1:1

If the `vncunused=` option in the config file is set, the domain will take the first available display number. If not, it’ll take the display number that corresponds to its domain number. We tend to leave it set, but unset is fine too.

Somewhat to our surprise, X11 worked quite well out of the box with the `vesa` driver, taking over the VNC framebuffer console and providing a usable display without further configuration.

## 4.3  Getting the Standard Xen Console to Work

Now, logging in via the graphical console is an irritating bit of overhead and, we would argue, overkill for a server. Fortunately, you can circumvent this by using the standard Xen emulated serial console. First, make sure that your domU config file (e.g., /etc/xen/leontes) contains the following:

    serial='pty'

This directive tells QEMU (and therefore Xen) to pass serial output to the Xen console.

Now the bootloader needs to be told to pass its messages to the serial line. Add the following to /boot/grub.conf in the domU:

    serial --unit=0 --speed=115200 --word=8 --parity=no --stop=1
    terminal serial console

These two lines give GRUB some settings for the serial port and tell it to actually use the serial port as its console.

Next, set up the kernel to output its boot information via serial:

    title CentOS (2.6.18-8.el5)
    root (hd0,0)
    kernel /vmlinuz-2.6.18-8.el5 ro root=/dev/VolGroup00/LogVol00 rhgb quiet console=ttyS0
    module /initrd-2.6.18-8.el5.img

(If you’re loading the Xen hypervisor in HVM mode—which is a perfectly reasonable thing to do—your menu.lst file will look a bit different, of course.)

Finally, edit inittab to start a getty on the emulated serial line by adding a line like this:

    s1:12345:respawn:/sbin/agetty -L ttyS0 9600 vt100

Boot the machine, and you should be able to see messages and log in via both xm console and VNC.

# 5 HVM Devices

If you poke around [搜查] in your new HVM domain, you’ll see devices like “QEMU harddisk,” or “RealTek Ethernet controller.” These take the place of the corresponding Xen devices, like xenblock or xennet. Examine this dmesg output, for example:

    PIIX3: IDE controller at PCI slot 0000:00:01.1 PIIX3: chipset revision 0
    PIIX3: not 100% native mode: will probe IRQs later PCI: Setting latency timer
    of device 0000:00:01.1 to 64
    ide0: BM-DMA at 0xc000-0xc007, BIOS settings: hda:pio, hdb:pio ide0: BM-DMA at
    0xc008-0xc00f, BIOS settings: hdc:pio, hdd:pio
    Probing IDE interface ide0...
    hda: QEMU HARDDISK, ATA DISK drive

This shows the QEMU emulated hard drive. Further on, we see:

    eth0: RealTek RTL8139 at 0xc200, 00:16:3e:0b:7c:f0, IRQ 11 eth0: Identified
    8139 chip type 'RTL-8139'

The difference between PV and HVM domains doesn’t end in the domU, either. With HVM domains, you’ll see tap devices in the dom0.

Think of the tap devices as QEMU analogues to the vifs discussed earlier—bits shoved into them come out on the virtual network devices in the domU. You can manage them just like vifs—adding them to bridges, configuring them down or up, and so on.

## 5.1  Paravirtualized Drivers

The best solution to the problem of slow emulated HVM devices, in the context of Xen, is to use paravirtualized drivers that work on the same splitdriver model as non-HVM domains—backend drivers in domain 0, with a small frontend driver in domU that communicates with the backend via event channels, using ring buffers and page flipping. (See [Chapter 1](https://wiki.prgmr.com/mediawiki/index.php/Chapter_1:_Xen:_A_High-Level_Overview) for boring details on Xen’s split driver model.)

XenSource includes such drivers for Windows, of course. For users of open source Xen, Novell distributes an expensive driver package that performs the same function—paravirtualized device support for Windows. GPL drivers also exist for Windows on open source Xen (we discuss those in more detail in [Chapter 13](https://wiki.prgmr.com/mediawiki/index.php/Chapter_13:_Xen_and_Windows)).

However, you can also build PV drivers for Linux HVM domains. These drivers are included with the Xen source tree in the *unmodified_drivers* directory. Unfortunately, the kernel API keeps changing, so the PV drivers might not compile against your kernel version (the drivers with Xen 3.1 refused to compile with kernel versions 2.6.20 and above).

### 5.1.1   Compiling PV Drivers for HVM Linux

Nonetheless, the best way to figure out if the drivers will work is to try it. Here’s how we compiled our drivers out of a standard Xen source tree.

    # cd unmodified_drivers/linux-2.6 # ./mkbuildtree
    # make -C /usr/src/linux M=$PWD modules

This builds standard Xen devices as modules—install them into your modules tree and load them like any other driver, with insmod or modprobe. You’ll wind up with four modules, one each for block and network devices, one for xenbus, and one for PCI emulation. Load xen-platform-pci first, then xenbus, then block and network.

    # insmod xen-platform-pci
    # insmod xenbus
    # insmod xenblk
    # insmod xennet

Because we were using Slackware as our domU, we then built a minimal kernel—without drivers for the emulated IDE or Realtek network card—and built an initrd with the Xen devices included.

We also needed to modify `/etc/fstab` to refer to the Xen backend devices.

Finally (this is starting to seem like a lot of trouble, isn’t it?) we edited the domain’s configuration to specify netfront and blkfront instead of the ioemu devices. We did this by changing the device lines:

    vif = [ 'type=ioemu, bridge=xenbr0' ]
    disk = [ 'file:/opt/florizel.img,ioemu:hda,w', 'file:/root/slackware-12.0-
    install-dvd.iso,ioemu:hdc:cdrom,r' ]

to:

    vif = [ 'bridge=xenbr0' ]
    disk = [ 'file:/opt/florizel.img,hda,w', 'file:/root/slackware-12.0-installdvd.
    iso,hdc:cdrom,r' ]

and removing the device-model= line.

Modify these directions to work with your setup, of course.

# 6 And, for Our Next Trick . . .

As always, there are some areas of ongoing work. Both Intel and AMD have announced successor techniques for dealing with guest page tables by adding a new level to the page table structure. AMD terms the concept nested paging, while Intel calls it Extended Page Tables. IOMMU development is another exciting area of research.

HVM is nice in general, but of course all of this is a prelude to Chapter 13, where we’ll apply all of this stuff to getting a virtual Windows machine up and running in HVM mode. Stay tuned!

# 7 Footnotes

[^1]: Intel has a nice introduction to their virtualization extensions at [http://www.intel.com/technology/itj/2006/v10i3/3-xen/1-abstract.htm](http://www.intel.com/technology/itj/2006/v10i3/3-xen/1-abstract.htm) and a promotional overview page at http://www.intel.com/technology/platform-technology/virtualization/index.htm. They’re worth reading.

[^2]: Also, Gentle Reader, your humble authors lack a recent Itanium to play with. Please forward offers of hardware to lsc@prgmr.com.

[^3]: AMD has a light introduction to their extensions at http://developer.amd.com/TechnicalArticles/Articles/Pages/630200615.aspx

[^4]: As Intel points out, the actual implementation of HVM drivers is much better than this naïve model. For example, device access is asynchronous, meaning that the VM can do other things while waiting for I/O to complete.

[^5]: There’s an interesting paper on the topic at http://developer.amd.com/assets/IOMMU-ben-yehuda.pdf

[^6]: By which I mean, we didn’t want to have to stand up and walk over to a test machine to access the console.
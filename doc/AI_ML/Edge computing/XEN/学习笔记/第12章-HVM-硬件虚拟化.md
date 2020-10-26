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



[^1]: Intel has a nice introduction to their virtualization extensions at [http://www.intel.com/technology/itj/2006/v10i3/3-xen/1-abstract.htm](http://www.intel.com/technology/itj/2006/v10i3/3-xen/1-abstract.htm) and a promotional overview page at http://www.intel.com/technology/platform-technology/virtualization/index.htm. They’re worth reading.

[^2]: Also, Gentle Reader, your humble authors lack a recent Itanium to play with. Please forward offers of hardware to lsc@prgmr.com.

[^3]: AMD has a light introduction to their extensions at http://developer.amd.com/TechnicalArticles/Articles/Pages/630200615.aspx

[^4]: As Intel points out, the actual implementation of HVM drivers is much better than this naïve model. For example, device access is asynchronous, meaning that the VM can do other things while waiting for I/O to complete.

[^5]: There’s an interesting paper on the topic at http://developer.amd.com/assets/IOMMU-ben-yehuda.pdf

[^6]: By which I mean, we didn’t want to have to stand up and walk over to a test machine to access the console.
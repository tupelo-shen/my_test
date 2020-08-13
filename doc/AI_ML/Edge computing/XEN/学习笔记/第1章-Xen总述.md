We’ll start by explaining what makes Xen different from other virtualization techniques and then provide some low-level detail on how Xen works and how its components fit together.

# 1 虚拟化原理

First, we might want to mention that computers, even new and fast ones with modern multitasking operating systems, can perform only one instruction at a time.[^1] Now, you say, “But my computer is performing many tasks at once. Even now, I can see a clock running, hear music playing, download files, and chat with friends, all at the same time.” And this is true. However, what is actually happening is that the computer is switching between these different tasks so quickly that the delays become imperceptible. Just as a movie is a succession of still images that give the illusion of movement, a computer performs tasks that are so seamlessly interweaved as to appear simultaneous.

Virtualization just extends this metaphor a bit. Ordinarily, this multiplexing takes place under the direction of the operating system, which acts to supervise tasks and make sure that each receives its fair share of CPU time. Because the operating system must therefore schedule tasks to run on the CPU, this aspect of the operating system is called a scheduler. With Xen virtualization, the same process occurs, with entire operating systems taking the place of tasks. The scheduling aspect is handled by the Xen kernel, which runs on a level superior to the “supervising” guest operating systems, and which we thus call the hypervisor.

Of course, it’s not quite so simple—operating systems, even ones that have been modified to be Xen-friendly, use a different, more comprehensive, set of assumptions than applications, and switching between them is almost by definition going to involve more complexity.

So let’s look at an overview of how virtualization is traditionally done and how Xen’s design is new and different. A traditional virtual machine is designed to mimic a real machine in every way, such that it’s impossible to tell from within the virtual machine that it isn’t real. To preserve that illusion, fully virtualized machines intercept attempts to access hardware and emulate that hardware’s functionality in software—thus maintaining perfect compatibility with the applications inside the virtual machine. This layer of indirection makes the virtual machine very slow.

Xen bypasses this slowdown using an approach called paravirtualization— para as a prefix means similar to or alongside. As the name suggests, it’s not “real” virtualization in the traditional sense because it doesn’t try to provide a seamless illusion of a machine. Xen presents only a partial abstraction of the underlying hardware to the hosted operating system, exposing some aspects of the machine as limitations on the guest OS, which needs to know that it’s running on Xen and should handle certain hardware interactions accordingly.

> Newer processors incorporate support for hardware virtualization, allowing unmodified operating systems to run under Xen. See [Chapter 12](https://wiki.prgmr.com/mediawiki/index.php/Chapter_12:_HVM:_Beyond_Paravirtualization) for details.

Most of these limitations—by design—aren’t noticeable to the system’s users. To run under Xen, the guest OS kernel needs to be modified so that, for example, it asks Xen for memory rather than allocating it directly. One of the design goals for Xen was to have these changes occur in the hardwaredependent bits of the guest operating system, without changing the interface between the kernel and user-level software.

This design goal reduces the difficulty of moving to Xen by ensuring that existing binaries will work unmodified on the Xen guest OS and that the virtual machine will, in most regards, act exactly like a real one, at least from the perspective of the system’s end users.

[^1]: [baidu](http://xueshu.baidu.com/)
[^5]: [参考文章](https://wiki.prgmr.com/mediawiki/index.php/Chapter_1:_Xen:_A_High-Level_Overview#Footnotes)
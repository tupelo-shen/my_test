Many developers, users, and entire industries rely on virtualization, as provided by software like [Xen](http://xenproject.org/), [QEMU/KVM](http://qemu.org/), or [kvmtool](https://git.kernel.org/cgit/linux/kernel/git/will/kvmtool.git/). While QEMU can run a software-based virtual machine, and Xen can run cooperating paravirtualized OSes without hardware support, most current uses and deployments of virtualization rely on hardware-accelerated virtualization, as provided on many modern hardware platforms. Linux supports hardware virtualization via the Kernel Virtual Machine (KVM) API. In this article, we'll take a closer look at the KVM API, using it to directly set up a virtual machine without using any existing virtual machine implementation.

A virtual machine using KVM need not run a complete operating system or emulate a full suite of hardware devices. Using the KVM API, a program can run code inside a sandbox and provide arbitrary virtual hardware interfaces to that sandbox. If you want to emulate anything other than standard hardware, or run anything other than a standard operating system, you'll need to work with the KVM API used by virtual machine implementations. As a demonstration that KVM can run more (or less) than just a complete operating system, we'll instead run a small handful of instructions that simply compute 2+2 and print the result to an emulated serial port.

The KVM API provides an abstraction over the hardware-virtualization features of various platforms. However, any software making use of the KVM API still needs to handle certain machine-specific details, such as processor registers and expected hardware devices. For the purposes of this article, we'll set up an x86 virtual machine using [Intel VT](https://en.wikipedia.org/wiki/X86_virtualization#Intel_virtualization_.28VT-x.29). For another platform, you'd need to handle different registers, different virtual hardware, and different expectations about memory layout and initial state.

Linux内核中关于`KVM-API`的文档位于`Documentation/virtual/kvm/api.txt`，其它的文章位于`Documentation/virtual/kvm/`目录下。

This article includes snippets of sample code from a [fully functional sample program](https://lwn.net/Articles/658512/) (MIT licensed). The program makes extensive use of the [err() and errx() functions](http://man7.org/linux/man-pages/man3/err.3.html) for error handling; however, the snippets quoted in the article only include non-trivial error handling.

## 1 Definition of the sample virtual machine

A full virtual machine using KVM typically emulates a variety of virtual hardware devices and firmware functionality, as well as a potentially complex initial state and initial memory contents. For our sample virtual machine, we'll run the following 16-bit x86 code:

```asm
mov $0x3f8, %dx
add %bl, %al
add $'0', %al
out %al, (%dx)
mov $'\n', %al
out %al, (%dx)
hlt
```

These instructions will add the initial contents of the `al` and `bl` registers (which we will pre-initialize to `2`), convert the resulting sum (`4`) to ASCII by adding '0', output it to a serial port at `0x3f8` followed by a newline, and then halt.

Rather than reading code from an object file or executable, we'll pre-assemble these instructions (via `gcc` and `objdump`) into machine code stored in a static array:

```c
const uint8_t code[] = {
0xba, 0xf8, 0x03, /* mov $0x3f8, %dx */
0x00, 0xd8,       /* add %bl, %al */
0x04, '0',        /* add $'0', %al */
0xee,             /* out %al, (%dx) */
0xb0, '\n',       /* mov $'\n', %al */
0xee,             /* out %al, (%dx) */
0xf4,             /* hlt */
};
```

For our initial state, we will preload this code into the second page of guest "physical" memory(to avoid conflicting with a non-existent [real-mode interrupt descriptor table](https://en.wikipedia.org/wiki/Interrupt_descriptor_table) at address 0). `al` and `bl` will contain `2`, the code segment (`cs`) will have a base of `0`, and the instruction pointer (`ip`) will point to the start of the second page at `0x1000`.

Rather than the extensive set of virtual hardware typically provided by a virtual machine, we'll emulate only a trivial serial port on port `0x3f8`.

Finally, note that running 16-bit real-mode code with hardware VT support requires a processor with "unrestricted guest" support. The original VT implementations only supported protected mode with paging enabled; emulators like QEMU thus had to handle virtualization in software until reaching a paged protected mode (typically after OS boot), then feed the virtual system state into KVM to start doing hardware emulation. However, processors from the "Westmere" generation and newer support "unrestricted guest" mode, which adds hardware support for emulating 16-bit real mode, "big real mode", and protected mode without paging. The Linux KVM subsystem has supported the ["unrestricted guest" feature](https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=3a624e29c7587b79abab60e279f9d1a62a3d4716) since Linux 2.6.32 in June 2009.

## 2 构建虚拟机

First, we'll need to open `/dev/kvm`:

```c
kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);
```

We need read-write access to the device to set up a virtual machine, and all opens not explicitly intended for inheritance across [exec](http://man7.org/linux/man-pages/man3/exec.3.html) should use `O_CLOEXEC`.

Depending on your system, you likely have access to `/dev/kvm` either via a group named "kvm" or via an access control list (ACL) granting access to users logged in at the console.

Before you use the KVM API, you should make sure you have a version you can work with. Early versions of KVM had an unstable API with an increasing version number, but the KVM_API_VERSION last [changed to 12](https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=2ff81f70b56dc1cdd3bf2f08414608069db6ef1a) with Linux 2.6.22 in April 2007, and got [locked to that as a stable interface](https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=dea8caee7b6971ae90e9d303b5d98dbf2dafed53) in 2.6.24; since then, KVM API changes occur only via backward-compatible extensions (like all other kernel APIs). So, your application should first confirm that it has version 12, via the `KVM_GET_API_VERSION` `ioctl()`:

```c
ret = ioctl(kvm, KVM_GET_API_VERSION, NULL);
if (ret == -1)
    err(1, "KVM_GET_API_VERSION");
if (ret != 12)
    errx(1, "KVM_GET_API_VERSION %d, expected 12", ret);
```

After checking the version, you may want to check for any extensions you use, using the `KVM_CHECK_EXTENSION ioctl()`. However, for extensions that add new `ioctl()` calls, you can generally just call the `ioctl()`, which will fail with an error (`ENOTTY`) if it does not exist.

```c
ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
if (ret == -1)
    err(1, "KVM_CHECK_EXTENSION");
if (!ret)
    errx(1, "Required extension KVM_CAP_USER_MEM not available");
```

Next, we need to create a virtual machine (VM), which represents everything associated with one emulated system, including memory and one or more CPUs. KVM gives us a handle to this VM in the form of a file descriptor:

```c
vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
```

The VM will need some memory, which we provide in pages. This corresponds to the "physical" address space as seen by the VM. For performance, we wouldn't want to trap every memory access and emulate it by returning the corresponding data; instead, when a virtual CPU attempts to access memory, the hardware virtualization for that CPU will first try to satisfy that access via the memory pages we've configured. If that fails (due to the VM accessing a "physical" address without memory mapped to it), the kernel will then let the user of the KVM API handle the access, such as by emulating a memory-mapped I/O device or generating a fault.

For our simple example, we'll allocate a single page of memory to hold our code, using [mmap()](http://man7.org/linux/man-pages/man2/mmap.2.html) directly to obtain page-aligned zero-initialized memory:

```c
mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
```

We then need to copy our machine code into it:

```
memcpy(mem, code, sizeof(code));
```

And finally tell the KVM virtual machine about its spacious new 4096-byte memory:

```
struct kvm_userspace_memory_region region = {
    .slot = 0,
    .guest_phys_addr = 0x1000,
    .memory_size = 0x1000,
    .userspace_addr = (uint64_t)mem,
};
ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);
```


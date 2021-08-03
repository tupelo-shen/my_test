[TOC]

现在，许多开发者、用户和行业领域都依赖于虚拟化技术，如[XEN](http://xenproject.org/)、[QEMU/KVM](http://qemu.org/)和[kvmtool](https://git.kernel.org/cgit/linux/kernel/git/will/kvmtool.git/)。QEMU是软件模拟的虚拟机，XEN运行的是修改过的半虚拟化OS（不需要硬件支持虚拟化）。而目前大部分的虚拟化方案都是基于硬件虚拟化扩展而开发的。Linux通过内核模块`KVM`支持硬件虚拟化。本文，我们将通过手动编写虚拟机实现示例的方式，展示一些相关的KVM接口的调用方式和注意事项。

A virtual machine using KVM need not run a complete operating system or emulate a full suite of hardware devices. Using the KVM API, a program can run code inside a sandbox and provide arbitrary virtual hardware interfaces to that sandbox. If you want to emulate anything other than standard hardware, or run anything other than a standard operating system, you'll need to work with the KVM API used by virtual machine implementations. As a demonstration that KVM can run more (or less) than just a complete operating system, we'll instead run a small handful of instructions that simply compute 2+2 and print the result to an emulated serial port.

The KVM API provides an abstraction over the hardware-virtualization features of various platforms. However, any software making use of the KVM API still needs to handle certain machine-specific details, such as processor registers and expected hardware devices. For the purposes of this article, we'll set up an x86 virtual machine using [Intel VT](https://en.wikipedia.org/wiki/X86_virtualization#Intel_virtualization_.28VT-x.29). For another platform, you'd need to handle different registers, different virtual hardware, and different expectations about memory layout and initial state.

Linux内核中关于`KVM-API`的文档位于`Documentation/virtual/kvm/api.txt`，其它的文章位于`Documentation/virtual/kvm/`目录下。

本文的代码片段来源于[全功能示例程序](https://lwn.net/Articles/658512/) （MIT许可）。示例程序中大量使用了[err()和errx() 函数](http://man7.org/linux/man-pages/man3/err.3.html)进行错误处理，但是，本文只包含了一些重要的错误处理。

## 1 虚拟机代码示例

一个使用KVM的完整虚拟机，通常需要模拟各种虚拟硬件设备和固件功能、已经复杂的初始状态和初始内存等。对于我们的简单虚拟机示例的话，运行以下16位的x86代码：

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

首先，打开kvm设备文件`/dev/kvm`:

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

接下来，我们需要创建一个虚拟机（VM），包含内存、一个或多个CPU。KVM模块方便之处在于，以文件描述符的形式给了句柄去引用这个虚拟机。

```c
vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
```

这个虚拟机肯定需要一些虚拟内存，作为它的`物理地址`。为了提高性能，我们不希望陷入每次内存访问并通过返回相应的数据来模拟它；相反，如果虚拟CPU访问内存，CPU的硬件虚拟化首先
通过我们配置的内存页来满足访问。如果失败（由于VM访问一个没有内存映射的物理地址），内核将会让使用KVM接口的用户处理这次访问，比如模拟一个内存映射的I/O设备或直接生成错误。

对于我们这个简单的示例，我们申请了一个页（page）大小的内存，保存我们的代码。映射直接使用[mmap()](http://man7.org/linux/man-pages/man2/mmap.2.html) 函数，从而获取页对齐，并且初始化为0的内存区域。

```c
mem = mmap( NULL,                       /* start    */
            0x1000,                     /* length   */
            PROT_READ | PROT_WRITE,     /* prot     */
            MAP_SHARED | MAP_ANONYMOUS, /* flags    */
            -1,                         /* fd       */
            0                           /* offset   */
            );
```
> `start`，开始地址，一般设为`NULL`，代表让系统自动选定地址，映射成功后返回该地址；
> 
> `length`，映射的长度；
> 
> `prot`，映射区域的保护方式。可以为以下几种的组合：
> 
>   * `PROT_EXEC`：映射区域可被执行
>   * `PROT_READ`：映射区域可被读取
>   * `PROT_WRITE`：映射区域可被写入
>   * `PROT_NONE`：映射区域不能存取
> 
> `flags`，影响映射区域的各种特性。在调用`mmap()`时必须要指定`MAP_SHARED`或`MAP_PRIVATE`；
>   
>   * `MAP_FIXED`：如果参数`start`所指的地址无法成功建立映射时，则放弃映射，不对地址做修正。通常不鼓励使用；
>   * `MAP_SHARED`：对映射区域的写入数据会复制回文件内，而且允许其他映射该文件的进程共享。
>   * `MAP_PRIVATE`：对映射区域的写入操作会产生一个映射文件的复制，即私人的“写时拷贝”（`copy on write`）对此区域作的任何修改都不会写回原来的文件内容。
>   * `MAP_ANONYMOUS`：建立匿名映射。此时会忽略参数fd，不涉及文件，而且映射区域无法和其他进程共享。
>   * `MAP_DENYWRITE`：只允许对映射区域的写入操作，其他对文件直接写入的操作将会被拒绝。
>   * `MAP_LOCKED`：将映射区域锁定住，这表示该区域不会被置换（swap）。
> 
> `fd`，要映射到内存中的文件描述符。如果使用匿名内存映射时，即`flags`中设置了`MAP_ANONYMOUS`，`fd`设为`-1`。有些系统不支持匿名内存映射，则可以使用`fopen`打开`/dev/zero`文件，然后对该文件进行映射，可以同样达到匿名内存映射的效果。
> 
> `offset`，文件映射的偏移量，通常设置为0，代表从文件最前方开始对应，`offset`必须是`page`大小的整数倍。
> 
> 返回值，若映射成功则返回映射区的内存起始地址，否则返回`MAP_FAILED(-1)`，错误原因存于`errno`中。


把我们定义的机器码拷贝到刚刚申请的内存`mem`中：

```
memcpy(mem, code, sizeof(code));
```

告知虚拟机，它有4096字节大小的内存空间：

```
struct kvm_userspace_memory_region region = {
    .slot = 0,
    .guest_phys_addr = 0x1000,
    .memory_size = 0x1000,
    .userspace_addr = (uint64_t)mem,
};
ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);
```

* slot

    提供了我们传递给VM的内存块的索引；

* KVM_SET_USER_MEMORY_REGION

    如果这个宏和相同的`slot`号进行调用，则替换这个映射关系；如果这个宏和新的`slot`号一起调用，则创建一个独立的映射;

* guest_phys_addr

    指定guest虚拟机的`物理基地址`;

* userspace_addr

    指向当前进程中调用`mmap()`函数分配的后台内存；

    > 即使在32位的平台上，也总是使用64位的值

* memory_size

    指定要映射多少内存:一个页，4096字节。

现在，我们已经有了一个虚拟机，它的内存中也拷入了我们想要运行的代码。接下来，我们需要创建一个虚拟CPU，运行那段代码。基于KVM模块的虚拟CPU，代表了被模拟CPU的状态：包括处理器寄存器和其它的执行状态。同样，KVM模块提供了一个VCPU的句柄，也是一个文件描述符：

```c
vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, (unsigned long)0);
```

这儿的0代表虚拟CPU的索引。如果一个VM，具有多个CPU，则CPU的索引号从0到系统特定的限制。可以通过`KVM_CHECK_EXTENSION`检查`KVM_CAP_MAX_VCPUS`虚拟CPU个数获取。

每个虚拟CPU都有一个数据结构`struct kvm_run`，用来在内核和用户空间交换CPU的信息。无论什么时候硬件虚拟化停止（调用`vmexit`，比如模拟某些虚拟硬件设备的时候，`kvm_run`将会包含它为什么停止的信息。我们可以通过`mmap()`将这个数据结构映射到用户空间，但是，在此之前，我们需要知道这个数据结构的大小，以便知道映射多大的内存空间。可以通过调用`ioctl()`函数，传递`KVM_GET_VCPU_MMAP_SIZE`参数获得。

```c
mmap_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL);
```

> 值得注意的是，映射的大小往往会超过`kvm_run`数据结构的大小，因为内核也会使用`kvm_run`里的一些指向临时数据结构成员，导致实际的占用的空间大小大于`kvm_run`数据结构的大小。

有了这个大小，我们就可以通过`mmap()`映射`kvm_run`数据结构了：

```c
run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);
```

虚拟CPU`VCPU`包含处理器的寄存器状态，可以分为2类：标准寄存器和特殊寄存器；对应的数据结构分别是：`struct kvm_regs` 和 `struct kvm_sregs`。x86架构下，通用寄存器包含通用目的寄存器、指令寄存器（IP）和状态标志位（flags）；特殊寄存器主要包括段寄存器（CS）和控制寄存器。

在我们运行代码之前，我们需要设置这些寄存器的初始化状态。特殊寄存器的话，我们只需要设置段寄存器（CS），它的默认状态（连同初始的IP指针）指向内存顶部下面16字节的reset向量处，但是我们想要CS指向0。`kvm_sregs`中的每个`segement`指向一个完整的段描述符；我们不需要更改各个Flag标志或限制，但我们需要将`base`和`selector`字段归0，让段寄存器指向内存中的0地址。为了避免改变了其它的特殊寄存器状态位，我们先读取这些寄存器值，然后再将它们写回：

```c
ioctl(vcpufd, KVM_GET_SREGS, &sregs);
sregs.cs.base = 0;
sregs.cs.selector = 0;
ioctl(vcpufd, KVM_SET_SREGS, &sregs);
```

对于标准寄存器，初始指令寄存器（IP）指向我们的代码0x1000地址处（对应于在0地址的CS），操作数寄存器`ax`和`bx`设置为`2`，Flags寄存器的初始状态为`0x2`（这个值特定于x86架构，如果不设置，启动VM会失败）。

```c
struct kvm_regs regs = {
    .rip = 0x1000,
    .rax = 2,
    .rbx = 2,
    .rflags = 0x2,
};
ioctl(vcpufd, KVM_SET_REGS, &regs);
```

我们的VM和VCPU已经创建，内存已经映射并且初始化，也设置了寄存器的初始化状态，现在，是时候启动我们的虚拟机了。调用`ioctl()`，传递参数`KVM_RUN`即可。每次虚拟化停止（比如，我们模拟硬件时）的时候，成功返回。因此，此处我们作了一个`loop`循环处理。

```c
while (1) {
    ioctl(vcpufd, KVM_RUN, NULL);
    switch (run->exit_reason) {
        /* Handle exit */
    }
}
```

`KVM_RUN`命令会让VM一直在当前线程的上下文中运行，直到遇到模拟停止。要想运行支持多核的VM，用户空间的进程就必须生成多个线程，分别对不同的VCPU调用`KVM_RUN`命令。

想要知道`ioctl()`的退出原因，需要检查`run->exit_reason`。这里边包含几十种退出的原因。而本文的简单VM，我们只处理其中几种，其它的退出原因我们当作错误处理。

因为没有其它结束的信号，我们使用`hlt`指令作为退出的信号：

```c
    case KVM_EXIT_HLT:
        puts("KVM_EXIT_HLT");
        return 0;
```

为了让虚拟机中运行的代码输出结果，我们需要基于I/O端口`0x3f8`模拟一个串口。`run->io`中的字段分别表明输入输出方向、数据大小（1、2、4字节）、I/O端口、值的数量。为了传递数据，内核在`kvm_run`数据结构中映射了一个缓存，`run->io.data_offset`提供了该缓存的起始地址。

```c
    case KVM_EXIT_IO:
        if (run->io.direction == KVM_EXIT_IO_OUT &&
                run->io.size == 1 &&
                run->io.port == 0x3f8 &&
                run->io.count == 1)
            putchar(*(((char *)run) + run->io.data_offset));
        else
            errx(1, "unhandled KVM_EXIT_IO");
        break;
```

为了更好地调试和分析代码，通常需要处理一些常见的错误。尤其当改变了VM的初始条件时，经常出现`KVM_EXIT_FAIL_ENTRY`错误；它表明底层的硬件虚拟化机制（VT）由于设置的初始条件不满足它的要求而不能启动VM。比如，`flags`寄存器中没有设置比特位`0x2`，或者段或任务切换寄存器的初始值不符合各种设置条件，就会发生此错误。`hardware_entry_failure_reason`不能直接区分这些原因，需要仔细的阅读硬件手册进行详细的处理。

```c
    case KVM_EXIT_FAIL_ENTRY:
        errx(1, "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = 0x%llx",
            (unsigned long long)run->fail_entry.hardware_entry_failure_reason);
```

`KVM_EXIT_INTERNAL_ERROR`代表的是Linux内核的KVM模块内部的错误，而不是由于硬件造成的。特别的是，有一些情况下，为了性能（为了I/O合并一系列`vmexit`)， KVM内核模块会模拟一条或多条指令，而不是通过硬件直接访问。`run->internal.suberror`的值等于`KVM_INTERNAL_ERROR_EMULATION`表明VM遇到了一条非法指令。

```c
    case KVM_EXIT_INTERNAL_ERROR:
        errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
             run->internal.suberror);
```

完成上面的代码后，编译并运行，就会得到下面的结果：

```
$ ./kvm_test
4
KVM_EXIT_HLT
```

成功！我们成功运行了自定义的虚拟机代码：实现了`2+2`，将结果转换成ASCII码`4`，并将其写入到虚拟端口`0x3f8`。其中，写入端口的操作会造成，`ioctl()`调用返回`KVM_EXIT_IO`，我们模拟串口打印输出`4`。下一次循环执行，`ioctl()`调用会再次返回`KVM_EXIT_IO`，因为要输出`\n`。第三次、也是最后一次循环，会返回`KVM_EXIT_HLT`，我们打印一条提示信息并退出。

## 3 其它KVM API的特性

本文只是介绍了核心的`KVM`接口，但是忽略了一些其它重要的接口或者特性。

想要实现内存映射I/O设备的虚拟访问，需要查看`exit_reason`，判断`KVM_EXIT_MMIO`和`KVM_CAP_COALESCED_MMIO`的情况，减少`vmexit`的次数，`ioeventfd`机制在没有`vmexit`的情况下异步处理I/O。

对于硬件中断，需要理解`irqfd`机制，使用`KVM_CAP_IRQFD`扩展能力进行处理。`irqfd`机制提供了一个文件描述符，能够在不停止虚拟机的情况下，将硬件中断传递给虚拟机内部。虚拟机可以在单独的事件循环或者设备处理程序线程中写入数据，而为VCPU运行`KVM_RUN`的线程则将在下一个可能的机会处理终端。

x86虚拟机可能希望支持[CPUID](https://en.wikipedia.org/wiki/CPUID)和特定于模型的寄存器([MSR](https://en.wikipedia.org/wiki/Model-specific_register))，这两种寄存器都具有特定于体系结构的`ioctl()`，从而最小化`vmexit`的次数。

## 4 使用KVM API的应用

除了学习、调试虚拟机实现之外，为什么要直接使用`/dev/kvm`呢？

像`qemu-kvm`和`kvmtool`之类的虚拟机通常是模拟特定目标架构体系的标准硬件；比如，一台标准的x86架构的PC机。但如果想要模拟一个完全不同的系统，除了共享指令集之外，你可能需要实现一个新的VM。即使是一个已经成熟的虚拟机实现，新的`virtio`硬件设备的开发人员也需要对KVM的接口有着清晰的理解。

也可以像[novm](https://github.com/google/novm)和[kvmtool](https://git.kernel.org/cgit/linux/kernel/git/will/kvmtool.git/)一样使用KVM提供的API构建一个轻量级的VM，专门运行Linux系统。最近，`Clear Containers`项目使用[kvmtool][运行容器](https://lwn.net/Articles/644675/)，就是借助了硬件虚拟化。

另外，虚拟机也可以不需要运行操作系统。基于KVM的虚拟机可以实现一个没有虚拟硬件设备和操作系统的裸机运行环境。

## 5 本文的源代码

```c
#include <err.h>            /* glibc的一部分，提供错误打印函数 */
#include <fcntl.h>          /* unix标准头文件的一部分，提供open等函数 */
#include <linux/kvm.h>      /* linux kvm模块的头文件 */
#include <stdint.h>         /* C99，标准C库的头文件，提供几种扩展数据类型 */
#include <stdio.h>          /* 标准C库的一部分，提供标准输入输出函数 */
#include <stdlib.h>         /* 标准C库的一部分，常用的一些工具函数等 */
#include <string.h>         /* 标准C库的一部分，常用的一些字符串函数等 */
#include <sys/ioctl.h>      /* 系统相关C库的一部分，提供ioctl相关的函数等 */
#include <sys/mman.h>       /* 系统相关C库的一部分，提供mmap相关的函数等 */
#include <sys/stat.h>       /* 系统相关C库的一部分，提供获取文件属性等函数 */
#include <sys/types.h>      /* 系统相关C库的一部分，提供系统相关的一些类型定义 */

int main(void)
{
    const uint8_t code[] = {
    0xba, 0xf8, 0x03, /* mov $0x3f8, %dx */
    0x00, 0xd8,       /* add %bl, %al */
    0x04, '0',        /* add $'0', %al */
    0xee,             /* out %al, (%dx) */
    0xb0, '\n',       /* mov $'\n', %al */
    0xee,             /* out %al, (%dx) */
    0xf4,             /* hlt */
    };

    int                 kvm, vmfd, vcpufd, ret;
    size_t              mmap_size;
    uint8_t*            mem;
    struct kvm_run*     run;
    struct kvm_sregs    sregs;

    /* open '/dev/kvm'. */
    kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);

    /* Make sure we have the stable version of the API */
    ret = ioctl(kvm, KVM_GET_API_VERSION, NULL);
    if (ret == -1)
        err(1, "KVM_GET_API_VERSION");
    if (ret != 12)
        errx(1, "KVM_GET_API_VERSION %d, expected 12", ret);

    #if 0
    ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
    if (ret == -1)
        err(1, "KVM_CHECK_EXTENSION");
    if (!ret)
        errx(1, "Required extension KVM_CAP_USER_MEM not available");
    #endif

    /* create a virtual machine(VM) */
    vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
    if (vmfd == -1)
        err(1, "KVM_CREATE_VM");

    mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (!mem)
        err(1, "allocating guest memory");
    memcpy(mem, code, sizeof(code));

    struct kvm_userspace_memory_region region = {
        .slot = 0,
        .guest_phys_addr = 0x1000,
        .memory_size = 0x1000,
        .userspace_addr = (uint64_t)mem,
    };
    ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);

    vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, (unsigned long)0);

    mmap_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL);
    run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);

    /* setup the initial state of special registers */
    ioctl(vcpufd, KVM_GET_SREGS, &sregs);
    sregs.cs.base = 0;
    sregs.cs.selector = 0;
    ioctl(vcpufd, KVM_SET_SREGS, &sregs);

    /* setup the initial state of standard registers */
    struct kvm_regs regs = {
        .rip = 0x1000,
        .rax = 2,
        .rbx = 2,
        .rflags = 0x2,
    };
    ioctl(vcpufd, KVM_SET_REGS, &regs);

    /* loop */
    while (1) {
        ioctl(vcpufd, KVM_RUN, NULL);
        switch (run->exit_reason) {
            /* Handle exit */
            case KVM_EXIT_HLT:
                puts("KVM_EXIT_HLT");
                return 0;
            case KVM_EXIT_IO:
                if (run->io.direction == KVM_EXIT_IO_OUT &&
                        run->io.size == 1 &&
                        run->io.port == 0x3f8 &&
                        run->io.count == 1)
                    putchar(*(((char *)run) + run->io.data_offset));
                else
                errx(1, "unhandled KVM_EXIT_IO");
                break;
            case KVM_EXIT_FAIL_ENTRY:
                errx(1, "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = 0x%llx",
                    (unsigned long long)run->fail_entry.hardware_entry_failure_reason);
                break;
            case KVM_EXIT_INTERNAL_ERROR:
                errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
                    run->internal.suberror);
                break;
            default:
                errx(1, "exit_reason = 0x%x", run->exit_reason);
                break;
        }
    }
}
```
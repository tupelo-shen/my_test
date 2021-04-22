[TOC]

操作系统在用户应用程序和硬件（比如CPU、磁盘或打印机）之间提供了一个中间层，这样的做的好处有：

1. 用户不必研究硬件设备如何编程，如何使用，只关心业务逻辑的开发即可；
2. 提高了系统安全性。内核在满足接口调用之前，会在接口层对请求是否合法进行检查；
3. 提高了用户程序的可移植性。只要各个操作系统提供标准统一的接口，应用程序几乎不用修改。

本文我们对系统调用的接口进行一下阐述。

## 1 POSIX API和系统调用的区别

在开始之前，我们先来弄懂一个概念的区别：应用编程接口（API）和系统调用。前者是一个函数定义，明确如何获得给定的服务；而后者是通过软件中断向内核发起请求。

通俗的讲，应用编程接口（API）是对系统调用的封装。

Unix系统一般会以库文件的形式提供API接口给应用编程人员。比如说，标准C库`libc`提供了一些API，其实就是一些系统调用的封装。通常，每个系统调用都会有一个类似的封装函数，其定义了应用编程应该使用的API。

反过来说，就不恰当了。并不是每个API都对应一个系统调用。

* 首先，API是在用户态提供服务（比如说，math函数库，就是一堆数学算法的实现，与平台无关，也就没有理由进行系统调用）；
* 其次，单个API可以调用多个系统调用；
* 最后，多个API函数也可以调用同一个系统调用，只是封装的功能不同。比如，Linux系统下的libc库中实现了`malloc()`、`calloc()`和`free()`等API。代码中，都是申请、释放内存的过程。

POSIX标准是对API的规定，而不是系统调用。如果说一个系统是POSIX兼容的，那它一定是提供了一组标准的POSIX API给应用程序，而用户不必关心相应的函数实现。现在，很多系统都是POSIX兼容的。

从编程者的角度来说，API和系统调用之间的区别无关紧要，唯一重要的无非就是函数名称，参数类型和返回值的意义。但是，从内核开发者的角度来说，这种区别很重要，因为系统调用属于内核，而用户态下的各种API库不属于内核。

大多数系统调用的封装函数返回一个整型值，具体含义往往取决于对应的系统调用。返回`-1`，通常表明内核无法满足请求。至于说返回错误的原因可能有很多，比如错误的参数、缺乏可用的资源、硬件故障等等。具体的错误码一般存储在`errno`变量中，`libc`库中对其有定义。

POSIX标准通过宏定义的方式，指定了一些特殊的错误码。Linux内核，在80x86平台下，定义的这些错误码位于`include/asm-i386/errno.h`文件中。考虑到Unix系统中C语言的兼容性，`include/asm-i386/errno.h`文件又被包含在标准C库的头文件`/usr/include/errno.h`中。

## 2 系统调用处理程序和服务例程

用户态调用`系统调用`，CPU就会切换到内核态，启动内核函数的执行。80x86架构下，有两种方法调用系统调用。但是，这两种方法最终都会跳转到内核态中的一段汇编语言实现的一个函数中，这个函数我们称之为`系统调用处理程序`。

内核态实现了许多不同的系统调用，为了识别到底是调用的哪个系统调用，用户态进程必须传递给系统调用处理程序一个系统调用号。Linux内核中使用寄存器`eax`寄存器传递系统调用号。至于系统调用中其它需要传递的参数，我们后面再讨论。

所有的系统调用都返回一个整型值。系统调用返回值和封装函数的返回值约定往往不同。在内核中，正值或0，表示系统调用正常终止，而负值表示错误条件。后者的话，负数形式的错误码保存在errno变量中，返回给应用程序。内核不会处理这个errno变量，而是交给该系统调用的封装函数完成处理errno变量的任务。

系统调用处理程序，实现过程与其他异常处理程序类似，主要完成下面的工作：

* 保存寄存器内容到内核态堆栈中（这一步对所有的系统调用都是一样的，一般使用汇编语言实现）；

* 处理具体的系统调用，一般用C语言实现，称之为系统调用服务例程；

* 退出系统调用处理程序：将内核态堆栈中的寄存器内容加载到寄存器中，CPU从内核态切换到用户态（这一步对所有的系统调用都是一样的，一般使用汇编语言实现）。

> 这儿需要特别理解两个概念，`系统调用处理程序`和`系统调用服务例程`，这是根据英文直译过来的。区别就是系统调用处理程序就是接收系统调用的指令，保护现场，选择具体的系统调用，然后退出系统调用的过程；而系统调用服务例程，就是根据具体的系统调用号，执行具体的系统调用处理。

如下图所示，展示了应用程序调用系统调用的过程，我们可以清晰地看到，应用程序、封装函数、系统调用处理程序和系统调用服务例程之间的关系。箭头`→`表示执行的顺序。符号`SYSCALL`和`SYSEXIT`是切换CPU的汇编指令的占位符，可以实现用户态和内核态的切换。假设系统调用的名称为`xyz()`，则通常情况下，相应的系统调用服务例程为`sys_xyz()`。当然，这个规则也存在一些例外情况。

<img id="Figure_10-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_10_1.PNG">

为了匹配系统调用号和对应的服务例程，内核使用了一个系统调用匹配表，该表存储在`sys_call_table`数组中，该数组总共有`NR_syscalls`项（Linux 2.6.11内核版本下该值等于289）。系统调用号即为这个数组的索引，其存储的内容则为对应的系统调用服务例程的地址。

其实，`NR_syscalls`只是实现的系统调用的静态上限，并不表明，系统调用真实实现了。事实上，系统调用匹配表中的每一项都有可能被赋值为`sys_ni_syscall()`函数的地址，该函数是未实现系统调用的服务例程，仅仅是返回错误码`-ENOSYS`。

## 3 进入和退出系统调用

应用程序有两种方法发起系统调用：

* 执行`int $0x80`汇编指令；（旧版本的linux内核中，这是用户态切换到内核态的唯一方法）
* 执行`sysenter`汇编指令（这是Intel奔腾II引入的指令）。

同样，内核退出系统调用也有两种方法：

* 执行`iret`汇编指令；
* 执行`sysexit`汇编指令（也是Intel奔腾II引入的指令，同`sysenter`是一对）。

看上去，这两种方式没啥不同。但是，我们必须考虑其兼容性：

* 内核必须同时支持使用`int $0x80`指令的旧代码和使用`sysenter`指令的新代码；
* 使用`sysenter`指令的标准库必须能够处理只支持`int $0x80`指令的旧内核；
* 内核和标准库必须能够同时运行在不支持`sysenter`指令的较旧处理器和支持该指令的新处理器上。

后面，在讲解《通过`sysenter`指令发起系统调用》一节的时候，我们再来看Linux内核是如何解决兼容性问题的。

## 4 通过`int $0x80`指令发起系统调用

传统的系统调用方式就是利用`int`汇编指令。向量`128`（16进制为`0x80`）就是内核异常的入口点，该向量是专门留出来给系统调用使用的异常入口点。内核初始化的时候，`trap_init()`函数会为向量128建立对应的中断描述符表项。代码如下：

```c
set_system_gate(0x80, &system_call);
```

我们知道，Linux将中断、陷阱和系统门分开为不同的入口点。所以，上面的代码将0x80设置为系统门，对应的gate描述符的内容，需要修改，如下所示：

* Segment Selector

    `__KERNEL_CS`，设置为内核代码段的段选择器。

* Offset

    `system_call()`函数的地址。

* Type

    设为15。表明异常是一个trap，相应的处理程序不会禁止可屏蔽中断。（PS：我们调用系统调用的时候，当然不希望中断被屏蔽。）

* DPL（描述符权限）

    设为3。允许用户态进程调用异常处理程序。（PS：如果不允许，我们的用户态如何切换到内核态呢？）

初始化完成后，一旦用户态进程发出`int $0x80`指令，CPU就会切换到内核态执行，并且是从system_call地址处开始执行代码指令。

#### 4.1 `system_call()`函数

`system_call()`函数就是前面介绍的系统调用处理程序。它要干的工作就是保存`系统调用号`和异常处理程序使用的所有CPU寄存器。`eflags`、`cs`、`eip`、`ss`和 `esp`等寄存器由硬件自动保存，不需要软件干预。`SAVE_ALL`宏，实现寄存器的保存，包括将内核数据段的段选择器加载到`ds`和`es`寄存器中：

```c
system_call:
    pushl %eax
    SAVE_ALL
    movl $0xffffe000, %ebx /* or 0xfffff000 for 4-KB stacks */
    andl %esp, %ebx
```

The function then stores the address of the thread_info data structure of the current process in ebx (see the section “Identifying a Process” in Chapter 3). This is done by taking the value of the kernel stack pointer and rounding it up to a multiple of 4 or 8 KB (see the section “Identifying a Process” in Chapter 3).

Next, the system_call() function checks whether either one of the TIF_SYSCALL_TRACE and TIF_SYSCALL_AUDIT flags included in the flags field of the thread_info structure is set—that is, whether the system call invocations of the executed program are being traced by a debugger. If this is the case, system_call() invokes the do_syscall_trace() function twice: once right before and once right after the execution of the system call service routine (as described later). This function stops current and thus allows the debugging process to collect information about it.

A validity check is then performed on the system call number passed by the User Mode process. If it is greater than or equal to the number of entries in the system call dispatch table, the system call handler terminates:

```c
    cmpl $NR_syscalls, %eax
    jb nobadsys
    movl $(-ENOSYS), 24(%esp)
    jmp resume_userspace
nobadsys:
```

If the system call number is not valid, the function stores the -ENOSYS value in the stack location where the eax register has been saved—that is, at offset 24 from the current stack top. It then jumps to resume_userspace (see below). In this way, when the process resumes its execution in User Mode, it will find a negative return code in eax.

Finally, the specific service routine associated with the system call number contained in eax is invoked:

```c
call *sys_call_table(0, %eax, 4)
```

Because each entry in the dispatch table is 4 bytes long, the kernel finds the address of the service routine to be invoked by multiplying the system call number by 4, adding the initial address of the sys_call_table dispatch table, and extracting a pointer to the service routine from that slot in the table.

#### 4.2 退出系统调用

When the system call service routine terminates, the system_call() function gets its return code from eax and stores it in the stack location where the User Mode value of the eax register is saved:

```c
movl %eax, 24(%esp)
```

Thus, the User Mode process will find the return code of the system call in the eax register.

Then, the system_call() function disables the local interrupts and checks the flags in the thread_info structure of current:

```c
cli
movl 8(%ebp), %ecx
testw $0xffff, %cx
je restore_all
```

The flags field is at offset 8 in the thread_info structure; the mask 0xffff selects the
bits corresponding to all flags listed in Table 4-15 except TIF_POLLING_NRFLAG. If none
of these flags is set, the function jumps to the restore_all label: as described in the
section “Returning from Interrupts and Exceptions” in Chapter 4, this code restores
the contents of the registers saved on the Kernel Mode stack and executes an iret
assembly language instruction to resume the User Mode process. (You might refer to
the flow diagram in Figure 4-6.)

If any of the flags is set, then there is some work to be done before returning to User
Mode. If the TIF_SYSCALL_TRACE flag is set, the system_call() function invokes for the
second time the do_syscall_trace() function, then jumps to the resume_userspace
label. Otherwise, if the TIF_SYSCALL_TRACE flag is not set, the function jumps to the
work_pending label.

As explained in the section “Returning from Interrupts and Exceptions” in
Chapter 4, that code at the resume_userspace and work_pending labels checks for
rescheduling requests, virtual-8086 mode, pending signals, and single stepping; then
eventually a jump is done to the restore_all label to resume the execution of the
User Mode process.

## 5 通过`sysenter`指令发出系统调用

The int assembly language instruction is inherently slow because it performs several
consistency and security checks. (The instruction is described in detail in the section
“Hardware Handling of Interrupts and Exceptions” in Chapter 4.)

The sysenter instruction, dubbed in Intel documentation as “Fast System Call,” provides
a faster way to switch from User Mode to Kernel Mode.

### The sysenter instruction

The sysenter assembly language instruction makes use of three special registers that must be loaded with the following information:*

* SYSENTER_CS_MSR

    The Segment Selector of the kernel code segment

* SYSENTER_EIP_MSR

    The linear address of the kernel entry point

* SYSENTER_ESP_MSR

    The kernel stack pointer

> “MSR” is an acronym for “Model-Specific Register” and denotes a register that is present only in some models of 80 × 86 microprocessors.

When the sysenter instruction is executed, the CPU control unit:

1. Copies the content of SYSENTER_CS_MSR into cs.
2. Copies the content of SYSENTER_EIP_MSR into eip.
3. Copies the content of SYSENTER_ESP_MSR into esp.
4. Adds 8 to the value of SYSENTER_CS_MSR, and loads this value into ss.

Therefore, the CPU switches to Kernel Mode and starts executing the first instruction
of the kernel entry point. As we have seen in the section “The Linux GDT” in
Chapter 2, the kernel stack segment coincides with the kernel data segment, and the
corresponding descriptor follows the descriptor of the kernel code segment in the
Global Descriptor Table; therefore, step 4 loads the proper Segment Selector in the
ss register.
The three model-specific registers are initialized by the enable_sep_cpu() function,
which is executed once by every CPU in the system during the initialization of the
kernel. The function performs the following steps:

1. Writes the Segment Selector of the kernel code (__KERNEL_CS) in the SYSENTER_CS_MSR register.

2. Writes in the SYSENTER_CS_EIP register the linear address of the sysenter_entry() function described below.

3. Computes the linear address of the end of the local TSS, and writes this value in the SYSENTER_CS_ESP register.*

The setting of the SYSENTER_CS_ESP register deserves some comments. When a system call starts, the kernel stack is empty, thus the esp register should point to the end of the 4- or 8-KB memory area that includes the kernel stack and the descriptor of the current process (see Figure 3-2). The User Mode wrapper routine cannot properly set this register, because it does not know the address of this memory area; on the other hand, the value of the register must be set before switching to Kernel Mode. Therefore, the kernel initializes the register so as to encode the address of the Task State Segment of the local CPU. As we have described in step 3 of the __switch_to() function (see the section “Performing the Process Switch” in Chapter 3), at every process switch the kernel saves the kernel stack pointer of the current process in the esp0 field of the local TSS. Thus, the system call handler reads the esp register, computes the address of the esp0 field of the local TSS, and loads into the same esp register the proper kernel stack pointer.

### The vsyscall page

A wrapper function in the libc standard library can make use of the sysenter instruction
only if both the CPU and the Linux kernel support it.

This compatibility problem calls for a quite sophisticated solution. Essentially, in the initialization phase the sysenter_setup() function builds a page frame called vsyscall page containing a small ELF shared object (i.e., a tiny ELF dynamic library). When a process issues an execve() system call to start executing an ELF program, the code in the vsyscall page is dynamically linked to the process address space (see the section “The exec Functions” in Chapter 20). The code in the vsyscall page makes use of the best available instruction to issue a system call.

The sysenter_setup() function allocates a new page frame for the vsyscall page and associates its physical address with the FIX_VSYSCALL fix-mapped linear address (see the section “Fix-Mapped Linear Addresses” in Chapter 2). Then, the function copies in the page either one of two predefined ELF shared objects:

* If the CPU does not support sysenter, the function builds a vsyscall page that includes the code:
    
    ```c
    __kernel_vsyscall:
        int $0x80
        ret
    ```

* Otherwise, if the CPU does support sysenter, the function builds a vsyscall page that includes the code:

    ```c
    __kernel_vsyscall:
        pushl %ecx
        pushl %edx
        pushl %ebp
        movl %esp, %ebp
        sysenter
    ```

When a wrapper routine in the standard library must invoke a system call, it calls the __kernel_vsyscall() function, whatever it may be.

A final compatibility problem is due to old versions of the Linux kernel that do not support the sysenter instruction; in this case, of course, the kernel does not build the vsyscall page and the __kernel_vsyscall() function is not linked to the address space of the User Mode processes. When recent standard libraries recognize this fact, they simply execute the int $0x80 instruction to invoke the system calls.

### Entering the system call

The sequence of steps performed when a system call is issued via the sysenter instruction is the following:

1. The wrapper routine in the standard library loads the system call number into the eax register and calls the __kernel_vsyscall() function.

2. The __kernel_vsyscall() function saves on the User Mode stack the contents of ebp, edx, and ecx (these registers are going to be used by the system call handler), copies the user stack pointer in ebp, then executes the sysenter instruction.

3. The CPU switches from User Mode to Kernel Mode, and the kernel starts executing the sysenter_entry() function (pointed to by the SYSENTER_EIP_MSR register).

4. The sysenter_entry() assembly language function performs the following steps:

    * Sets up the kernel stack pointer:

        ```c
        movl -508(%esp), %esp
        ```

        Initially, the esp register points to the first location after the local TSS, which is 512bytes long. Therefore, the instruction loads in the esp register the contents of the field at offset 4 in the local TSS, that is, the contents of the esp0 field. As already explained, the esp0 field always stores the kernel stack pointer of the current process.

    * Enables local interrupts:

        ```c
        sti
        ```

    * Saves in the Kernel Mode stack the Segment Selector of the user data segment, the current user stack pointer, the eflags register, the Segment Selector of the user code segment, and the address of the instruction to be executed when exiting from the system call:

        ```c
        pushl $(__USER_DS)
        pushl %ebp
        pushfl
        pushl $(__USER_CS)
        pushl $SYSENTER_RETURN
        ```

        Observe that these instructions emulate some operations performed by the int assembly language instruction (steps 5c and 7 in the description of int in the section “Hardware Handling of Interrupts and Exceptions” in Chapter 4).

    * Restores in ebp the original value of the register passed by the wrapper routine:

        ```c
        movl (%ebp), %ebp
        ```

        This instruction does the job, because _ _kernel_vsyscall() saved on the User Mode stack the original value of ebp and then loaded in ebp the current value of the user stack pointer.

    * Invokes the system call handler by executing a sequence of instructions identical to that starting at the system_call label described in the earlier section “Issuing a System Call via the int $0x80 Instruction.”

### Exiting from the system call

When the system call service routine terminates, the sysenter_entry() function executes essentially the same operations as the system_call() function (see previous section). First, it gets the return code of the system call service routine from eax and stores it in the kernel stack location where the User Mode value of the eax register is saved. Then, the function disables the local interrupts and checks the flags in the thread_info structure of current. If any of the flags is set, then there is some work to be done before returning to User Mode. In order to avoid code duplication, this case is handled exactly as in the system_call() function, thus the function jumps to the resume_userspace or work_pending labels (see flow diagram in Figure 4-6 in Chapter 4). Eventually, the iret assembly language instruction fetches from the Kernel Mode stack the five arguments saved in step 4c by the sysenter_entry() function, and thus switches the CPU back to User Mode and starts executing the code at the SYSENTER_RETURN label (see below).

If the sysenter_entry() function determines that the flags are cleared, it performs a quick return to User Mode:

```c
movl 40(%esp), %edx
movl 52(%esp), %ecx
xorl %ebp, %ebp
sti
sysexit
```

The edx and ecx registers are loaded with a couple of the stack values saved by sysenter_entry() in step 4c in the previos section: edx gets the address of the SYSENTER_RETURN label, while ecx gets the current user data stack pointer.

### The sysexit instruction

The sysexit assembly language instruction is the companion of sysenter: it allows a fast switch from Kernel Mode to User Mode. When the instruction is executed, the CPU control unit performs the following steps:

1. Adds 16 to the value in the SYSENTER_CS_MSR register, and loads the result in the cs register.

2. Copies the content of the edx register into the eip register.

3. Adds 24 to the value in the SYSENTER_CS_MSR register, and loads the result in the ss register.

4. Copies the content of the ecx register into the esp register.

Because the SYSENTER_CS_MSR register is loaded with the Segment Selector of the kernel code, the cs register is loaded with the Segment Selector of the user code, while the ss register is loaded with the Segment Selector of the user data segment (see the section “The Linux GDT” in Chapter 2).

As a result, the CPU switches from Kernel Mode to User Mode and starts executing the instruction whose address is stored in the edx register.

### The SYSENTER_RETURN code

The code at the SYSENTER_RETURN label is stored in the vsyscall page, and it is executed when a system call entered via sysenter is being terminated, either by the iret instruction or the sysexit instruction.

The code simply restores the original contents of the ebp, edx, and ecx registers saved in the User Mode stack, and returns the control to the wrapper routine in the standard library:

```c
SYSENTER_RETURN:
    popl %ebp
    popl %edx
    popl %ecx
    ret
```


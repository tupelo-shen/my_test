操作系统在用户应用程序和硬件（比如CPU、磁盘或打印机）之间提供了一个中间层，这样的做的好处有：

1. 用户不必研究硬件设备如何编程，如何使用，只关心业务逻辑的开发即可；
2. 提高了系统安全性。内核在满足接口调用之前，会在接口层对请求是否合法进行检查；
3. 提高了用户程序的可移植性。只要各个操作系统提供标准统一的接口，应用程序几乎不用修改。

本文我们对系统调用的接口进行一下阐述。

在开始之前，我们先来弄懂一个概念的区别：应用编程接口（API）和系统调用。前者是一个函数定义，明确如何获得给定的服务；而后者是通过软件中断向内核发起请求。

通俗的讲，应用编程接口（API）是对系统调用的封装。

Unix系统一般会以库文件的形式提供API接口给应用编程人员。比如说，标准C库`libc`提供了一些API，其实就是一些系统调用的封装。通常，每个系统调用都会有一个类似的封装函数，其定义了应用编程应该使用的API。

反过来说，就不恰当了。并不是每个API都对应一个系统调用。

First of all, the API could offer its services directly in User Mode. (For something abstract such as math functions, there may be no reason to make system calls.) Second, a single API function could make several system calls. Moreover, several API functions could make the same system call, but wrap extra functionality around it. For instance, in Linux, the malloc( ), calloc( ), and free( ) APIs are implemented in the libc library. The code in this library keeps track of the allocation and deallocation requests and uses the brk( ) system call to enlarge or shrink the process heap (see the section “Managing the Heap” in Chapter 9).

The POSIX standard refers to APIs and not to system calls. A system can be certified as POSIX-compliant if it offers the proper set of APIs to the application programs, no matter how the corresponding functions are implemented. As a matter of fact, several non-Unix systems have been certified as POSIX-compliant, because they offer all traditional Unix services in User Mode libraries.

From the programmer’s point of view, the distinction between an API and a system call is irrelevant—the only things that matter are the function name, the parameter types, and the meaning of the return code. From the kernel designer’s point of view, however, the distinction does matter because system calls belong to the kernel, while User Mode libraries don’t.

Most wrapper routines return an integer value, whose meaning depends on the corresponding system call. A return value of –1 usually indicates that the kernel was unable to satisfy the process request. A failure in the system call handler may be caused by invalid parameters, a lack of available resources, hardware problems, and so on. The specific error code is contained in the errno variable, which is defined in the libc library.

Each error code is defined as a macro constant, which yields a corresponding positive integer value. The POSIX standard specifies the macro names of several error codes. In Linux, on 80 × 86 systems, these macros are defined in the header file include/asm-i386/errno.h. To allow portability of C programs among Unix systems, the include/asm-i386/errno.h header file is included, in turn, in the standard /usr/include/errno.h C library header file. Other systems have their own specialized subdirectories of header files.

## System Call Handler and Service Routines

When a User Mode process invokes a system call, the CPU switches to Kernel Mode
and starts the execution of a kernel function. As we will see in the next section, in the
80 × 86 architecture a Linux system call can be invoked in two different ways. The net
result of both methods, however, is a jump to an assembly language function called
the system call handler.

Because the kernel implements many different system calls, the User Mode process
must pass a parameter called the system call number to identify the required system
call; the eax register is used by Linux for this purpose. As we’ll see in the section
“Parameter Passing” later in this chapter, additional parameters are usually passed
when invoking a system call.

All system calls return an integer value. The conventions for these return values are
different from those for wrapper routines. In the kernel, positive or 0 values denote a
successful termination of the system call, while negative values denote an error condition.
In the latter case, the value is the negation of the error code that must be
returned to the application program in the errno variable. The errno variable is not
set or used by the kernel. Instead, the wrapper routines handle the task of setting this
variable after a return from a system call.

The system call handler, which has a structure similar to that of the other exception handlers, performs the following operations:

* Saves the contents of most registers in the Kernel Mode stack (this operation is common to all system calls and is coded in assembly language).

* Handles the system call by invoking a corresponding C function called the system call service routine.

* Exits from the handler: the registers are loaded with the values saved in the Kernel Mode stack, and the CPU is switched back from Kernel Mode to User Mode (this operation is common to all system calls and is coded in assembly language).

The name of the service routine associated with the xyz() system call is usually sys_xyz(); there are, however, a few exceptions to this rule.

Figure 10-1 illustrates the relationships between the application program that invokes
a system call, the corresponding wrapper routine, the system call handler, and the system
call service routine. The arrows denote the execution flow between the functions.
The terms “SYSCALL” and “SYSEXIT” are placeholders for the actual assembly language
instructions that switch the CPU, respectively, from User Mode to Kernel Mode and
from Kernel Mode to User Mode.

<img id="Figure_10-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/understanding_linux_kernel_10_1.PNG">

To associate each system call number with its corresponding service routine, the kernel
uses a system call dispatch table, which is stored in the sys_call_table array and
has NR_syscalls entries (289 in the Linux 2.6.11 kernel). The nth entry contains the
service routine address of the system call having number n.

The NR_syscalls macro is just a static limit on the maximum number of implementable
system calls; it does not indicate the number of system calls actually
implemented. Indeed, each entry of the dispatch table may contain the address of the
sys_ni_syscall( ) function, which is the service routine of the “nonimplemented”
system calls; it just returns the error code -ENOSYS.

## Entering and Exiting a System Call

Native applications* can invoke a system call in two different ways:

* By executing the int $0x80 assembly language instruction; in older versions of the Linux kernel, this was the only way to switch from User Mode to Kernel Mode.
* By executing the sysenter assembly language instruction, introduced in the Intel Pentium II microprocessors; this instruction is now supported by the Linux 2.6 kernel.

Similarly, the kernel can exit from a system call—thus switching the CPU back to User Mode—in two ways:

* By executing the iret assembly language instruction.
* By executing the sysexit assembly language instruction, which was introduced in the Intel Pentium II microprocessors together with the sysenter instruction.

However, supporting two different ways to enter the kernel is not as simple as it might look, because:

* The kernel must support both older libraries that only use the int $0x80 instruction and more recent ones that also use the sysenter instruction.
* A standard library that makes use of the sysenter instruction must be able to cope with older kernels that support only the int $0x80 instruction.
* The kernel and the standard library must be able to run both on older processors that do not include the sysenter instruction and on more recent ones that include it.

We will see in the section “Issuing a System Call via the sysenter Instruction” later in this chapter how the Linux kernel solves these compatibility problems.

## Issuing a System Call via the int $0x80 Instruction

The “traditional” way to invoke a system call makes use of the int assembly language
instruction, which was discussed in the section “Hardware Handling of Interrupts
and Exceptions” in Chapter 4.
The vector 128—in hexadecimal, 0x80—is associated with the kernel entry point.
The trap_init() function, invoked during kernel initialization, sets up the Interrupt
Descriptor Table entry corresponding to vector 128 as follows:

```c
set_system_gate(0x80, &system_call);
```

The call loads the following values into the gate descriptor fields (see the section “Interrupt, Trap, and System Gates” in Chapter 4):

* Segment Selector

    The __KERNEL_CS Segment Selector of the kernel code segment.

* Offset

    The pointer to the system_call( ) system call handler.

* Type

    Set to 15. Indicates that the exception is a Trap and that the corresponding handler does not disable maskable interrupts.

* DPL (Descriptor Privilege Level)

    Set to 3. This allows processes in User Mode to invoke the exception handler (see the section “Hardware Handling of Interrupts and Exceptions” in Chapter 4).

Therefore, when a User Mode process issues an int $0x80 instruction, the CPU switches into Kernel Mode and starts executing instructions from the system_call address.

### The system_call( ) function

The system_call( ) function starts by saving the system call number and all the CPU
registers that may be used by the exception handler on the stack—except for eflags,
cs, eip, ss, and esp, which have already been saved automatically by the control unit
(see the section “Hardware Handling of Interrupts and Exceptions” in Chapter 4).
The SAVE_ALL macro, which was already discussed in the section “I/O Interrupt Handling”
in Chapter 4, also loads the Segment Selector of the kernel data segment in ds
and es:

```c
system_call:
    pushl %eax
    SAVE_ALL
    movl $0xffffe000, %ebx /* or 0xfffff000 for 4-KB stacks */
    andl %esp, %ebx
```

The function then stores the address of the thread_info data structure of the current process in ebx (see the section “Identifying a Process” in Chapter 3). This is done by taking the value of the kernel stack pointer and rounding it up to a multiple of 4 or 8 KB (see the section “Identifying a Process” in Chapter 3).

Next, the system_call( ) function checks whether either one of the TIF_SYSCALL_TRACE and TIF_SYSCALL_AUDIT flags included in the flags field of the thread_info structure is set—that is, whether the system call invocations of the executed program are being traced by a debugger. If this is the case, system_call( ) invokes the do_syscall_trace( ) function twice: once right before and once right after the execution of the system call service routine (as described later). This function stops current and thus allows the debugging process to collect information about it.

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

### Exiting from the system call

When the system call service routine terminates, the system_call( ) function gets its
return code from eax and stores it in the stack location where the User Mode value of
the eax register is saved:

```c
movl %eax, 24(%esp)
```

Thus, the User Mode process will find the return code of the system call in the eax
register.

Then, the system_call() function disables the local interrupts and checks the flags in
the thread_info structure of current:

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

## Issuing a System Call via the sysenter Instruction

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

This compatibility problem calls for a quite sophisticated solution. Essentially, in the
initialization phase the sysenter_setup() function builds a page frame called vsyscall
page containing a small ELF shared object (i.e., a tiny ELF dynamic library). When a
process issues an execve() system call to start executing an ELF program, the code in
the vsyscall page is dynamically linked to the process address space (see the section
“The exec Functions” in Chapter 20). The code in the vsyscall page makes use of the
best available instruction to issue a system call.

The sysenter_setup() function allocates a new page frame for the vsyscall page and
associates its physical address with the FIX_VSYSCALL fix-mapped linear address (see
the section “Fix-Mapped Linear Addresses” in Chapter 2). Then, the function copies
in the page either one of two predefined ELF shared objects:

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

1. The wrapper routine in the standard library loads the system call number into
the eax register and calls the _ _kernel_vsyscall() function.

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

When the system call service routine terminates, the sysenter_entry( ) function executes
essentially the same operations as the system_call() function (see previous section). First, it gets the return code of the system call service routine from eax and
stores it in the kernel stack location where the User Mode value of the eax register is
saved. Then, the function disables the local interrupts and checks the flags in the
thread_info structure of current.
If any of the flags is set, then there is some work to be done before returning to User
Mode. In order to avoid code duplication, this case is handled exactly as in the
system_call() function, thus the function jumps to the resume_userspace or work_
pending labels (see flow diagram in Figure 4-6 in Chapter 4). Eventually, the iret
assembly language instruction fetches from the Kernel Mode stack the five arguments
saved in step 4c by the sysenter_entry() function, and thus switches the CPU
back to User Mode and starts executing the code at the SYSENTER_RETURN label (see
below).

If the sysenter_entry() function determines that the flags are cleared, it performs a quick return to User Mode:

    movl 40(%esp), %edx
    movl 52(%esp), %ecx
    xorl %ebp, %ebp
    sti
    sysexit

The edx and ecx registers are loaded with a couple of the stack values saved by sysenter_entry() in step 4c in the previos section: edx gets the address of the SYSENTER_RETURN label, while ecx gets the current user data stack pointer.

### The sysexit instruction

The sysexit assembly language instruction is the companion of sysenter: it allows a
fast switch from Kernel Mode to User Mode. When the instruction is executed, the
CPU control unit performs the following steps:

1. Adds 16 to the value in the SYSENTER_CS_MSR register, and loads the result in the cs register.

2. Copies the content of the edx register into the eip register.

3. Adds 24 to the value in the SYSENTER_CS_MSR register, and loads the result in the ss register.

4. Copies the content of the ecx register into the esp register.

Because the SYSENTER_CS_MSR register is loaded with the Segment Selector of the kernel
code, the cs register is loaded with the Segment Selector of the user code, while
the ss register is loaded with the Segment Selector of the user data segment (see the
section “The Linux GDT” in Chapter 2).

As a result, the CPU switches from Kernel Mode to User Mode and starts executing
the instruction whose address is stored in the edx register.

### The SYSENTER_RETURN code

The code at the SYSENTER_RETURN label is stored in the vsyscall page, and it is executed
when a system call entered via sysenter is being terminated, either by the iret
instruction or the sysexit instruction.

The code simply restores the original contents of the ebp, edx, and ecx registers saved
in the User Mode stack, and returns the control to the wrapper routine in the standard
library:

```c
SYSENTER_RETURN:
    popl %ebp
    popl %edx
    popl %ecx
    ret
```


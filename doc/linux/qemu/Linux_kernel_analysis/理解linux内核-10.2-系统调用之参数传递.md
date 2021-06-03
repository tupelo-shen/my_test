[TOC]

Like ordinary functions, system calls often require some input/output parameters, which may consist of actual values (i.e., numbers), addresses of variables in the address space of the User Mode process, or even addresses of data structures including pointers to User Mode functions (see the section “System Calls Related to Signal Handling” in Chapter 11).

同普通函数一样，系统调用也需要传参。

Because the system_call( ) and the sysenter_entry( ) functions are the common entry points for all system calls in Linux, each of them has at least one parameter: the system call number passed in the eax register. For instance, if an application program invokes the fork( ) wrapper routine, the eax register is set to 2 (i.e., __NR_fork) before executing the int $0x80 or sysenter assembly language instruction. Because the register is set by the wrapper routines included in the libc library, programmers do not usually care about the system call number.

Linux系统下，所有系统调用的入口点都是`system_call()`或`sysenter_entry()`，所以，这两个函数至少需要一个参数：系统调用号，其保存在寄存器`eax`中。要不然，内核怎么知道你想干什么呢？比如，我们在写多进程应用的时候，必须要用的`fork()`系统调用，在调用`int $80`或者`sysenter`汇编指令之前，必须设置`eax`寄存器的值为2（__NR_fork）。通常，这个操作都是在libc库中的函数中完成的，对于系统调用号我们不太在意而已。

The fork( ) system call does not require other parameters. However, many system
calls do require additional parameters, which must be explicitly passed by the application
program. For instance, the mmap( ) system call may require up to six additional
parameters (besides the system call number).

The parameters of ordinary C functions are usually passed by writing their values in
the active program stack (either the User Mode stack or the Kernel Mode stack).
Because system calls are a special kind of function that cross over from user to kernel
land, neither the User Mode or the Kernel Mode stacks can be used. Rather, system
call parameters are written in the CPU registers before issuing the system call.
The kernel then copies the parameters stored in the CPU registers onto the Kernel
Mode stack before invoking the system call service routine, because the latter is an
ordinary C function.

Why doesn’t the kernel copy parameters directly from the User Mode stack to the
Kernel Mode stack? First of all, working with two stacks at the same time is complex;
second, the use of registers makes the structure of the system call handler similar to
that of other exception handlers.

However, to pass parameters in registers, two conditions must be satisfied:

* The length of each parameter cannot exceed the length of a register (32 bits).
* The number of parameters must not exceed six, besides the system call number passed in eax, because 80 × 86 processors have a very limited number of registers.

The first condition is always true because, according to the POSIX standard, large
parameters that cannot be stored in a 32-bit register must be passed by reference. A
typical example is the settimeofday( ) system call, which must read a 64-bit structure.

However, system calls that require more than six parameters exist. In such cases, a
single register is used to point to a memory area in the process address space that
contains the parameter values. Of course, programmers do not have to care about
this workaround. As with every C function call, parameters are automatically saved
on the stack when the wrapper routine is invoked. This routine will find the appropriate
way to pass the parameters to the kernel.

The registers used to store the system call number and its parameters are, in increasing
order, eax (for the system call number), ebx, ecx, edx, esi, edi, and ebp. As seen
before, system_call( ) and sysenter_entry( ) save the values of these registers on the
Kernel Mode stack by using the SAVE_ALL macro. Therefore, when the system call service
routine goes to the stack, it finds the return address to system_call( ) or to
sysenter_entry( ), followed by the parameter stored in ebx (the first parameter of the
system call), the parameter stored in ecx, and so on (see the section “Saving the registers
for the interrupt handler” in Chapter 4). This stack configuration is exactly the
same as in an ordinary function call, and therefore the service routine can easily refer
to its parameters by using the usual C-language constructs.

Let’s look at an example. The sys_write( ) service routine, which handles the write( )
system call, is declared as:

```c
int sys_write (unsigned int fd, const char * buf, unsigned int count)
```

The C compiler produces an assembly language function that expects to find the fd,
buf, and count parameters on top of the stack, right below the return address, in the
locations used to save the contents of the ebx, ecx, and edx registers, respectively.

In a few cases, even if the system call doesn’t use any parameters, the corresponding
service routine needs to know the contents of the CPU registers right before the system
call was issued. For example, the do_fork( ) function that implements fork( )
needs to know the value of the registers in order to duplicate them in the child process
thread field (see the section “The thread field” in Chapter 3). In these cases, a single parameter of type pt_regs allows the service routine to access the values saved
in the Kernel Mode stack by the SAVE_ALL macro (see the section “The do_IRQ( )
function” in Chapter 4):

```c
int sys_fork (struct pt_regs regs)
```

The return value of a service routine must be written into the eax register. This is
automatically done by the C compiler when a return n; instruction is executed.

## Verifying the Parameters

All system call parameters must be carefully checked before the kernel attempts to
satisfy a user request. The type of check depends both on the system call and on the
specific parameter. Let’s go back to the write( ) system call introduced before: the fd
parameter should be a file descriptor that identifies a specific file, so sys_write( )
must check whether fd really is a file descriptor of a file previously opened and
whether the process is allowed to write into it (see the section “File-Handling System
Calls” in Chapter 1). If any of these conditions are not true, the handler must
return a negative value—in this case, the error code –EBADF.

One type of checking, however, is common to all system calls. Whenever a parameter
specifies an address, the kernel must check whether it is inside the process
address space. There are two possible ways to perform this check:

* Verify that the linear address belongs to the process address space and, if so, that
the memory region including it has the proper access rights.

* Verify just that the linear address is lower than PAGE_OFFSET (i.e., that it doesn’t
fall within the range of interval addresses reserved to the kernel).

Early Linux kernels performed the first type of checking. But it is quite time
consuming because it must be executed for each address parameter included in a system
call; furthermore, it is usually pointless because faulty programs are not very
common.

Therefore, starting with Version 2.2, Linux employs the second type of checking.
This is much more efficient because it does not require any scan of the process memory
region descriptors. Obviously, this is a very coarse check: verifying that the linear
address is smaller than PAGE_OFFSET is a necessary but not sufficient condition for
its validity. But there’s no risk in confining the kernel to this limited kind of check
because other errors will be caught later.

The approach followed is thus to defer the real checking until the last possible
moment—that is, until the Paging Unit translates the linear address into a physical
one. We will discuss in the section “Dynamic Address Checking: The Fix-up Code,”
later in this chapter, how the Page Fault exception handler succeeds in detecting
those bad addresses issued in Kernel Mode that were passed as parameters by User
Mode processes.

One might wonder at this point why the coarse check is performed at all. This type
of checking is actually crucial to preserve both process address spaces and the kernel
address space from illegal accesses. We saw in Chapter 2 that the RAM is mapped
starting from PAGE_OFFSET. This means that kernel routines are able to address all
pages present in memory. Thus, if the coarse check were not performed, a User
Mode process might pass an address belonging to the kernel address space as a
parameter and then be able to read or write every page present in memory without
causing a Page Fault exception.

The check on addresses passed to system calls is performed by the access_ok( )
macro, which acts on two parameters: addr and size. The macro checks the address
interval delimited by addr and addr + size – 1. It is essentially equivalent to the following
C function:

```c
int access_ok(const void * addr, unsigned long size)
{
    unsigned long a = (unsigned long) addr;
    if (a + size < a ||
        a + size > current_thread_info()->addr_limit.seg)
        return 0;
    return 1;
}
```

The function first verifies whether addr + size, the highest address to be checked, is
larger than 232–1; because unsigned long integers and pointers are represented by
the GNU C compiler (gcc) as 32-bit numbers, this is equivalent to checking for an
overflow condition. The function also checks whether addr + size exceeds the value
stored in the addr_limit.seg field of the thread_info structure of current. This field
usually has the value PAGE_OFFSET for normal processes and the value 0xffffffff for
kernel threads. The value of the addr_limit.seg field can be dynamically changed by
the get_fs and set_fs macros; this allows the kernel to bypass the security checks
made by access_ok(), so that it can invoke system call service routines, directly passing
to them addresses in the kernel data segment.

The verify_area() function performs the same check as the access_ok() macro;
although this function is considered obsolete, it is still widely used in the source
code.

## Accessing the Process Address Space

System call service routines often need to read or write data contained in the process’s
address space. Linux includes a set of macros that make this access easier.
We’ll describe two of them, called get_user( ) and put_user( ). The first can be used
to read 1, 2, or 4 consecutive bytes from an address, while the second can be used to
write data of those sizes into an address.

Each function accepts two arguments, a value x to transfer and a variable ptr. The second
variable also determines how many bytes to transfer. Thus, in get_user(x,ptr), the size of the variable pointed to by ptr causes the function to expand into a __get_user_1( ), __get_user_2( ), or __get_user_4( ) assembly language function. Let’s consider one of them, __get_user_2( ):

```c
__get_user_2:
    addl    $1, %eax
    jc      bad_get_user
    movl    $0xffffe000, %edx /* or 0xfffff000 for 4-KB stacks */
    andl    %esp, %edx
    cmpl    24(%edx), %eax
    jae     bad_get_user
2: movzwl   -1(%eax), %edx
    xorl %eax, %eax
    ret
bad_get_user:
    xorl %edx, %edx
    movl $-EFAULT, %eax
    ret
```

The eax register contains the address ptr of the first byte to be read. The first six
instructions essentially perform the same checks as the access_ok( ) macro: they
ensure that the 2 bytes to be read have addresses less than 4 GB as well as less than
the addr_limit.seg field of the current process. (This field is stored at offset 24 in the
thread_info structure of current, which appears in the first operand of the cmpl
instruction.)

If the addresses are valid, the function executes the movzwl instruction to store the
data to be read in the two least significant bytes of edx register while setting the highorder
bytes of edx to 0; then it sets a 0 return code in eax and terminates. If the
addresses are not valid, the function clears edx, sets the -EFAULT value into eax, and
terminates.

The put_user(x,ptr) macro is similar to the one discussed before, except it writes the
value x into the process address space starting from address ptr. Depending on the
size of x, it invokes either the __put_user_asm( ) macro (size of 1, 2, or 4 bytes) or the
__put_user_u64( ) macro (size of 8 bytes). Both macros return the value 0 in the eax
register if they succeed in writing the value, and -EFAULT otherwise.

Several other functions and macros are available to access the process address space
in Kernel Mode; they are listed in Table 10-1. Notice that many of them also have a
variant prefixed by two underscores (_ _). The ones without initial underscores take
extra time to check the validity of the linear address interval requested, while the
ones with the underscores bypass that check. Whenever the kernel must repeatedly
access the same memory area in the process address space, it is more efficient to
check the address once at the start and then access the process area without making
any further checks.

Table 10-1. Functions and macros that access the process address space

| 函数 | 行为 |
| ---- | ---- |
| get_user <br> __get_user | 从用户空间读取一个整型值（1、2、4字节）|
| put_user <br> __put_user | 写一个整型值到用户空间（1、2、4字节）|
| copy_from_user <br> __copy_from_user | 从用户空间拷贝一块任意大小的数据 |
| copy_to_user <br> __copy_to_user | 拷贝一块任意大小的数据到用户空间 |
| strncpy_from_user <br> __strncpy_from_user | 从用户空间拷贝null结尾的字符串 |
| strlen_user <br> strnlen_user | 返回用户空间中的字符串长度（以NULL结尾）|
| clear_user <br> __clear_user | 清空用户空间的一段内存区域（填0）|

## Dynamic Address Checking: The Fix-up Code

As seen previously, access_ok() makes a coarse check on the validity of linear
addresses passed as parameters of a system call. This check only ensures that the
User Mode process is not attempting to fiddle with the kernel address space; however,
the linear addresses passed as parameters still might not belong to the process
address space. In this case, a Page Fault exception will occur when the kernel tries to
use any of such bad addresses.

Before describing how the kernel detects this type of error, let’s specify the three cases
in which Page Fault exceptions may occur in Kernel Mode. These cases must be distinguished
by the Page Fault handler, because the actions to be taken are quite different.

1. The kernel attempts to address a page belonging to the process address space,
but either the corresponding page frame does not exist or the kernel tries to
write a read-only page. In these cases, the handler must allocate and initialize a
new page frame (see the sections “Demand Paging” and “Copy On Write” in
Chapter 9).

2. The kernel addresses a page belonging to its address space, but the corresponding
Page Table entry has not yet been initialized (see the section “Handling Noncontiguous
Memory Area Accesses” in Chapter 9). In this case, the kernel must
properly set up some entries in the Page Tables of the current process.

3. Some kernel functions include a programming bug that causes the exception to
be raised when that program is executed; alternatively, the exception might be
caused by a transient hardware error. When this occurs, the handler must perform
a kernel oops (see the section “Handling a Faulty Address Inside the Address
Space” in Chapter 9).

4. The case introduced in this chapter: a system call service routine attempts to
read or write into a memory area whose address has been passed as a system call
parameter, but that address does not belong to the process address space.

The Page Fault handler can easily recognize the first case by determining whether the
faulty linear address is included in one of the memory regions owned by the process.
It is also able to detect the second case by checking whether the corresponding master
kernel Page Table entry includes a proper non-null entry that maps the address.
Let’s now explain how the handler distinguishes the remaining two cases.

## The Exception Tables

The key to determining the source of a Page Fault lies in the narrow range of calls
that the kernel uses to access the process address space. Only the small group of
functions and macros described in the previous section are used to access this
address space; thus, if the exception is caused by an invalid parameter, the instruction
that caused it must be included in one of the functions or else be generated by
expanding one of the macros. The number of the instructions that address user space
is fairly small.

Therefore, it does not take much effort to put the address of each kernel instruction
that accesses the process address space into a structure called the exception table. If
we succeed in doing this, the rest is easy. When a Page Fault exception occurs in Kernel
Mode, the do_page_fault( ) handler examines the exception table: if it includes
the address of the instruction that triggered the exception, the error is caused by a
bad system call parameter; otherwise, it is caused by a more serious bug.

Linux defines several exception tables. The main exception table is automatically
generated by the C compiler when building the kernel program image. It is stored
in the __ex_table section of the kernel code segment, and its starting and ending
addresses are identified by two symbols produced by the C compiler: __start___
ex_table and __stop___ex_table.

Moreover, each dynamically loaded module of the kernel (see Appendix B) includes
its own local exception table. This table is automatically generated by the C compiler
when building the module image, and it is loaded into memory when the module
is inserted in the running kernel.

Each entry of an exception table is an exception_table_entry structure that has two
fields:

1. insn

    The linear address of an instruction that accesses the process address space

2. fixup

    The address of the assembly language code to be invoked when a Page Fault exception triggered by the instruction located at insn occurs

The fixup code consists of a few assembly language instructions that solve the problem
triggered by the exception. As we will see later in this section, the fix usually consists
of inserting a sequence of instructions that forces the service routine to return an
error code to the User Mode process. These instructions, which are usually defined in
the same macro or function that accesses the process address space, are placed by the
C compiler into a separate section of the kernel code segment called .fixup.

The search_exception_tables( ) function is used to search for a specified address in
all exception tables: if the address is included in a table, the function returns a pointer
to the corresponding exception_table_entry structure; otherwise, it returns NULL.
Thus the Page Fault handler do_page_fault( ) executes the following statements:

```c
if ((fixup = search_exception_tables(regs->eip))) {
    regs->eip = fixup->fixup;
    return 1;
}
```

The regs->eip field contains the value of the eip register saved on the Kernel Mode
stack when the exception occurred. If the value in the register (the instruction
pointer) is in an exception table, do_page_fault( ) replaces the saved value with the
address found in the entry returned by search_exception_tables( ). Then the Page
Fault handler terminates and the interrupted program resumes with execution of the
fixup code.

## Generating the Exception Tables and the Fixup Code

The GNU Assembler .section directive allows programmers to specify which section
of the executable file contains the code that follows. As we will see in
Chapter 20, an executable file includes a code segment, which in turn may be subdivided
into sections. Thus, the following assembly language instructions add an entry
into an exception table; the "a" attribute specifies that the section must be loaded
into memory together with the rest of the kernel image:

```c
.section _ _ex_table, "a"
    .long faulty_instruction_address, fixup_code_address
.previous
```

The .previous directive forces the assembler to insert the code that follows into the
section that was active when the last .section directive was encountered.
Let’s consider again the __get_user_1( ), __get_user_2( ), and __get_user_4( ) functions
mentioned before. The instructions that access the process address space are
those labeled as 1, 2, and 3:

```c
__get_user_1:
    [...]
1: movzbl (%eax), %edx
    [...]
__get_user_2:
    [...]
2: movzwl -1(%eax), %edx
    [...]
__get_user_4:
    [...]
3: movl -3(%eax), %edx
    [...]
bad_get_user:
    xorl %edx, %edx
    movl $-EFAULT, %eax
    ret
.section __ex_table,"a"
    .long 1b, bad_get_user
    .long 2b, bad_get_user
    .long 3b, bad_get_user
.previous
```

Each exception table entry consists of two labels. The first one is a numeric label
with a b suffix to indicate that the label is “backward;” in other words, it appears in a
previous line of the program. The fixup code is common to the three functions and is
labeled as bad_get_user. If a Page Fault exception is generated by the instructions at
label 1, 2, or 3, the fixup code is executed. It simply returns an –EFAULT error code to
the process that issued the system call.

Other kernel functions that act in the User Mode address space use the fixup code
technique. Consider, for instance, the strlen_user(string) macro. This macro
returns either the length of a null-terminated string passed as a parameter in a system
call or the value 0 on error. The macro essentially yields the following assembly
language instructions:

```c
    movl $0, %eax
    movl $0x7fffffff, %ecx
    movl %ecx, %ebx
    movl string, %edi
0: repne; scasb
    subl %ecx, %ebx
    movl %ebx, %eax
1:
.section .fixup,"ax"
2: xorl %eax, %eax
    jmp 1b
.previous
.section _ _ex_table,"a"
    .long 0b, 2b
.previous
```

The ecx and ebx registers are initialized with the 0x7fffffff value, which represents
the maximum allowed length for the string in the User Mode address space. The
repne;scasb assembly language instructions iteratively scan the string pointed to by
the edi register, looking for the value 0 (the end of string \0 character) in eax.
Because scasb decreases the ecx register at each iteration, the eax register ultimately
stores the total number of bytes scanned in the string (that is, the length of the
string).

The fixup code of the macro is inserted into the .fixup section. The "ax" attributes
specify that the section must be loaded into memory and that it contains executable
code. If a Page Fault exception is generated by the instructions at label 0, the fixup
code is executed; it simply loads the value 0 in eax—thus forcing the macro to return
a 0 error code instead of the string length—and then jumps to the 1 label, which corresponds
to the instruction following the macro.

The second .section directive adds an entry containing the address of the repne;
scasb instruction and the address of the corresponding fixup code in the _ _ex_table
section.


[TOC]

系统调用的设计目的主要是给用户态的程序调用使用的。但是，内核线程也可以调用这些系统调用，而它们却不能使用标准库的函数。另外，我们也可能添加一些系统调用到系统中，而不想修改标准C库。为了解决这些问题，并且简化相应的封装过程，Linux提供了7个宏：`_syscall0`到`_syscall6`。

> 关于如何添加新的系统调用到系统中，可以参考博客[《添加新的系统调用 _syscall0(int, mysyscall)》](https://www.cnblogs.com/aljxy/p/6047047.html)

## 1 宏说明

这些系统调用的宏中，数字0-6代表传递给系统调用的参数个数（不包括系统调用号）。也就是说，封装系统调用的参数个数不能超过6个，而且如果返回错误码不标准也不能使用。

每个宏实际需要`2 + 2*n`个参数，n，代表系统调用的参数个数。常量2，表示至少需要2个参数，分别指定返回类型和系统调用的名称。后面的每两个宏参数，指定系统调用的参数名称和类型。比如，fork系统调用不需要参数，则定义如下：

```c
_syscall0(int,fork)
```

## 2 宏的使用方法

下面，我们以write()系统调用的封装过程作为示例，演示这些宏的使用方法。

```c
_syscall3(int,write,int,fd,const char *,buf,unsigned int,count)
```

上面的宏定义会产生下面类似的C代码：

```c
int write(int fd, const char * buf, unsigned int count)
{
    long __res;
    asm("int $0x80"
        : "=a" (__res)
        : "0" (__NR_write), "b" ((long)fd),
          "c" ((long)buf),  "d" ((long)count));
    if ((unsigned long) __res >= (unsigned long)-129) {
        errno = -__res;
        __res = -1;
    }
    return (int) __res;
}
```

代码说明：

1. `__NR_write`宏定义是根据`_syscall3`定义中的第2个参数通过`##`字符串拼接符产生的。代表write()函数使用的系统调用号。

编译上述代码，产生如下的汇编代码：

```c
write:
    pushl %ebx              ; push ebx into stack
    movl 8(%esp), %ebx      ; put first parameter in ebx
    movl 12(%esp), %ecx     ; put second parameter in ecx
    movl 16(%esp), %edx     ; put third parameter in edx
    movl $4, %eax           ; put __NR_write in eax
    int $0x80               ; invoke system call
    cmpl $-129, %eax        ; check return code
    jbe .L1                 ; if no error, jump
    negl %eax               ; complement the value of eax
    movl %eax, errno        ; put result in errno
    movl $-1, %eax          ; set eax to -1
.L1: popl %ebx              ; pop ebx from stack
    ret                     ; return to calling program
```

汇编代码说明：

1. 在调用`int $80`指令之前，加载参数到CPU的寄存器中；
2. 返回值存储在`eax`寄存器中，主要表示错误码，在`（-1,-129）`之间（内核假定最大错误码为129，定义位于`include/generic/errno.h`头文件中。如果发生错误，则将`-eax`的值存储在全局变量`errno`中，函数返回-1；否则就返回`eax`寄存器的值。

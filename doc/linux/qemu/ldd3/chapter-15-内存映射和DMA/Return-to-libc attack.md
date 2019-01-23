*return-to-libc* 攻击是一种计算机安全攻击，从缓冲区溢出开始，调用的stack上的子例程的返回地址被使用当前进程可执行内存上已经存在的子例程的地址取代，从而绕过无可执行位功能（如果存在的话），而无需攻击者注入自己的代码。第一个例子是由 *Alexander Peslyak* 在1997年的Bugtraq邮件列表中提出的。

在遵循POSIX标准的操作系统上，C标准库（*libc*）通常用来为C语言编写的程序提供一个标准运行时环境。虽然，攻击者可以让代码返回任何地方，但是因为 *libc*总是被链接到程序，所以其是最有可能的目标，它为攻击者提供了有用的调用（比如说，用来执行 *shell* 命令的系统函数）。

#阻止*return-to-libc*攻击的方法

一个非可执行的stack能够阻止缓冲区溢出的使用，但是不能阻止 *return-to-libc* 攻击，因为在这种攻击中，只使用已经存在的可执行代码。另一方面，这些攻击只能调用已经存在的函数。栈溢出保护可以防止或阻止栈的损坏且可能会清除损坏的段。

"ASCII armoring" is a technique that can be used to obstruct this kind of attack. With ASCII armoring, all the system libraries (e.g. libc) addresses contain a NULL byte (0x00). This is commonly done by placing them in the first 0x01010101 bytes of memory (a few pages more than 16 MB, dubbed the "ASCII armor region"), as every address up to (but not including) this value contains at least one NULL byte. This makes it impossible to emplace code containing those addresses using string manipulation functions such as strcpy(). However, this technique does not work if the attacker has a way to overflow NULL bytes into the stack. If the program is too large to fit in the first 16 MiB, protection may be incomplete.[2] This technique is similar to another attack known as return-to-plt where, instead of returning to libc, the attacker uses the Procedure Linkage Table (PLT) functions loaded in the binary (e.g. system@plt, execve@plt, sprintf@plt, strcpy@plt, etc.).[3]

*ASCII armoring* 是一种可以阻止这类攻击的技术。使用 *ASCII armoring* ，所有的系统库（例如，*libc*地址包含一个 *NULL* 字节（0x00）。

*ASCII文本编码标准* 使用128唯一值（0-127）来表示英语中的字母、数字、和标点符号，以及一些不代表可打印字符的控制码。例如，大写字母 *A* 是ASCII字符 *65*，数字 *2* 是ASCII字符 *50*，字符 *}* 是ASCII字符 *125*， 元字符回车是ASCII字符 *13*。基于ASCII的系统使用7位来表示这些数值。

与此相反的是，大多数计算机在内存中使用8位的字节来组织存储在内存中的数据。包含机器可执行代码和非文本数据的文件通常包含所有的256（8位标示值）个可能的值。许多计算机程序依赖于7位文本和8位二进制数据之间的区别，如果非ASCII字符出现在预期仅包含ASCII文本的数据中，则无法正常工作。例如，如果未保留第8位的值，则程序则可能会将高于127的字节值解释为标志，告知其执行某些功能。

但是，通常希望能够通过基于文本的系统发送非文本的数据，比如说，把镜像文件附加到e-mail消息中。为了完成这个，数据必须以某种方式编码，例如，把8位的数据编码成7位ASCII字符（通常只使用字母、数字和标点符号这些可打印的字符）。在安全到达目的地后，在解码为安全的8位数据形式。这个过程被称为 *二进制到文本编码* 。许多程序执行此转换以允许数据传输，例如，PGP和GNU Privacy Guard（GPG）。

Address space layout randomization (ASLR) makes this type of attack extremely unlikely to succeed on 64-bit machines as the memory locations of functions are random. For 32-bit systems ASLR provides little benefit since there are only 16 bits available for randomization, and they can be defeated by brute force in a matter of minutes.
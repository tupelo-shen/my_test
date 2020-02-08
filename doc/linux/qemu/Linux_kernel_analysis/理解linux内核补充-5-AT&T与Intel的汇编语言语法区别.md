<h1 id="0">0 目录</h1>

* [1 操作数前缀](#1)
* [2 源/目的操作数顺序](#2)
* [3 寻址方式](#3)
* [4 标识长度的操作码前缀和后缀](#4)

---

x86汇编一直存在两种不同的语法，在intel的官方文档中使用intel语法，Windows也使用intel语法，而UNIX平台的汇编器一直使用AT&T语法。

AT&T 和 Intel 汇编语言的语法区别主要体现在操作数前缀、赋值方向、间接寻址语法、操作码的后缀上，而就具体的指令而言，在同一平台上，两种汇编语言是一致的。下面仅列出这两种汇编语言在语法上的不同点。

<h1 id="1">1 操作数前缀</h1>

在Intel的汇编语言语法中，寄存器和和立即数都没有前缀。但是在AT&T中，寄存器前冠以“％”，而立即数前冠以“$”。在Intel的语法中，十六进制和二进制立即数后缀分别冠以“h”和“b”，而在AT&T中，十六进制立即数前冠以“0x”：

| Intel 语法     | AT&T 语法         |
| -------------- | ----------------- |
| Mov eax,8      | movl $8,%eax      |
| Mov ebx,0ffffh | movl $0xffff,%ebx |
| int 80h        | int  $0x80        |

<h1 id="2">2 源/目的操作数顺序</h1>

Intel汇编语言的指令与AT&T的指令操作数的方向上正好相反：在Intel语法中，第一个操作数是目的操作数，第二个操作数源操作数。而在AT&T中，第一个数是源操作数，第二个数是目的操作数。)

| Intel 语法     | AT&T 语法         |
| -------------- | ----------------- |
| MOV EAX,8      | movl $8,%eax      |


<h1 id="3">3 寻址方式</h1>

与Intel的语法比较，AT&T间接寻址方式可能更晦涩难懂一些。

对于段寻址，地址计算公式是：
    
    address = disp + base + index * scale

其中，base是基址，disp为偏移地址，index*scale决定了第几个元素，scale为元素长度，只能为2的幂，index默认为0，scale默认为1。

Intel的指令格式是<font color="blue"> segreg: [base+index*scale+disp] </font>。

AT&T的格式是<font color="blue"> %segreg：disp(base,index,scale)</font>。

其中index/scale/disp/segreg 全部是可选的，完全可以简化掉。如果没有指定scale而指定了index，则scale 的缺省值为1。segreg段寄存器依赖于指令以及应用程序是运行在实模式还是保护模式下，在实模式下，它依赖于指令，而在保护模式下，segreg是多余的。在AT&T中，当立即数用在scale/disp中时， 不应当在其前冠以“$”前缀，而且scale,disp不需要加前缀“&”。另外在Intel中基地址使用`[]`，而在 AT&T 中则使用`()`。

| Intel 语法     | AT&T 语法         |
| -------------- | ----------------- |
| Instr foo,segreg：[base+index*scale+disp] | instr %segreg： disp(base,index,scale),foo      |
| [eax]                     | (%eax)                |
| [eax + _variable]         | _variable(%eax)       |
| [eax*4 + _array]          | _array(,%eax,4)       |
| [ebx + eax*8 + _array]    | _array(%ebx,%eax,8)   |

<h1 id="4">4 操作码后缀</h1>

在AT&T汇编中远程跳转指令和子过程调用指令的操作码使用前缀“l”，分别为`ljmp`，`lcall`，与之相应的返回指令伪`lret`。例如：

| Intel 语法     | AT&T 语法         |
| -------------- | ----------------- |
| LL SECTION:OFFSET     | lcall secion:offset |
| FAR SECTION:OFFSET    | ljmp secion:offset |
| FAR STACK_ADJUST      | lret $stack_adjust |

在AT&T的操作码后面有时还会有一个后缀，其含义就是指出操作码的大小。“l”表示长整数（32 位），“w”表示字（16 位），“b”表示字节（8 位）。而在Intel的语法中，则要在内存单元操作数的前面加上 byte ptr、 word ptr,和 dword ptr，“dword”对应“long”。

| Intel 语法     | AT&T 语法         |
| -------------- | ----------------- |
| Mov al,bl                 | movb %bl,%al |
| Mov ax,bx                 | movw %bx,%ax |
| Mov eax,ebx               | movl %ebx,%eax |
| Mov eax, dword ptr [ebx]  | movl (%ebx),%eax |

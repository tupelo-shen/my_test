#include <iostream>
 
extern "C" int func();
// 函数func的定义使用汇编语言编写
// 原始字符串文字可能非常有用
asm(R"(
.globl func
    .type func, @function
    func:
    .cfi_startproc
    movl $7, %eax
    ret
    .cfi_endproc
)");
 
int main()
{
    int n = func();
    // 扩展内联汇编
    asm ("leal (%0,%0,4),%0"
         : "=r" (n)
         : "0" (n));
    std::cout << "7*5 = " << n << std::endl; // 冲刷
 
    // 标准内联汇编
    asm ("movq $60, %rax\n\t" // Linux上的退出系统调用数字
         "movq $2,  %rdi\n\t" // 这个程序返回 2
         "syscall");
}

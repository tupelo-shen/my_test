// 移动构造函数和赋值
//#include <iostream>
#include <string>
#include "Common.h"

#include <iostream>
struct S {
    unsigned char b1 : 3;
    unsigned char :0; // 启动一个新的字节
    unsigned char b2 : 3;
    unsigned char b3 : 1;
};
int main()
{
    std::cout << sizeof(S) << '\n'; // 通常会打印2
}

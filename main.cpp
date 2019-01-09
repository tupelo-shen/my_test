//#include <vector>
#include <iostream>
// #include <chrono>
// #include <thread>
#include "Common.h"
// #include "Queue-pthread_s.h"
#include "Stack.h"

#include <cstdio>
struct S
{
    int f1(double d) {
        return printf("%f \n", d);      // 变量参数函数调用
    }
    int f2() {
        return f1(7);                   // 成员函数调用，等同于this->f1()
                                        // int型参数转化成double型
    }
};
void f() {
   puts("function called");             // 函数调用
}
int main()
{
    f();                                // 函数调用
    S s;
    s.f2();                             // 成员函数调用
}



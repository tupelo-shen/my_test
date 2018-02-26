#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include "Common.h"

#if 0
namespace Q {
    namespace V {    // V是Q的一个成员，它完全在Q内定义 
        // namespace Q::V { // C++17 替换上面两行
        // C是V的一个成员，它完全在V内定义;C::m只被声明；
        class C { 
        public:
            void m(); 
        };
        // f是V的一个成员，在这里只定义
        void f(); 
    }
    // V外的V成员函数f的定义，f的封闭命名空间仍然是全局命名空间Q和Q :: V
    void V::f()
    {
        extern void h(); // This declares ::Q::V::h
    }
    // 命名空间和类外 V::C::m 定义，的封闭命名空间仍然是全局命名空间Q和Q::V
    void V::C::m()
    {
        printf("V::C::m()");
    }
}
namespace Q {
    enum {
        ABC = 'a'
    };
}
#endif
#if 1
namespace Q {
    namespace V { // V原始命名空间定义
        void f(); // Q::V::f的声明
    }
    void V::f() {} // OK
    //void V::g() {} // Error: g()还不是V的成员
    namespace V { // V的扩展命名空间定义
        void g(); // Q::V::g的声明
    }
    namespace R {           // 不是Q的一个封闭命名空间
        void g() {}    // Error: cannot define Q::V::g inside R
    }
}

//void Q::V::g() {} // OK: 全局命名空间包括Q
#endif


int main()
{
    printf("ABC = ");

    SystemPause();
}

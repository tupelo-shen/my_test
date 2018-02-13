#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include "Common.h"

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

namespace A {
    int i;
}
namespace B {
    int i;
    int j;
    namespace C {
        namespace D {
            using namespace A;  // A 中所有的名称被导入到全局命名空间中
            int j;
            int k;
            int a = i;          // i是B::i, 因为A::i被B::i覆盖
        }
        using namespace D;      // D 中所有的名称被导入到 C 命名空间中
                                // A 中所有的名称被导入到全局命名空间中
        int k = 89; // OK to declare name identical to one introduced by a using
        int l = k;  // ambiguous: C::k or D::k
        int m = i;  // ok: B::i hides A::i
        int n = j;  // ok: D::j hides B::j
    }
}
namespace D 
{
   int d1;
   void f(char);
}
using namespace D;  // 导入D::d1, D::f, D::d2, D::f, E::e 和 E::f到全局命名空间中
 
int d1;             // OK: 声明时和D::d1没有冲突
namespace E {
    int e;
    void f(int);
}

// 扩展命名空间
namespace D 
{
    int d2;
    using namespace E; // transitive using-directive
    void f(int);
}
void f() {
    d1++; // error: ambiguous ::d1 or D::d1?
    ::d1++; // OK
    D::d1++; // OK
    d2++; // OK, d2 is D::d2
    e++; // OK: e is E::e due to transitive using
    f(1); // error: ambiguous: D::f(int) or E::f(int)?
    f('a'); // OK: the only f(char) is D::f(char)
}

int main()
{
    char abc[10] = {Q::ABC};
    Q::V::C c;
    c.m();

    printf("ABC = %s", abc);

    SystemPause();
}

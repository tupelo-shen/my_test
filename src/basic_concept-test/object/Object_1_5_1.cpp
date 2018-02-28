#include <cassert>
struct S {
    char c;   // 1 个字节
             // 3 个字节填充
    float f;   // 4 字节值
    bool operator==(const S& arg) const {    // 基于值的相等
        return c == arg.c && f == arg.f;
    }
};
assert(sizeof(S) == 8);
S s1 = {'a', 3.14};
S s2 = s1;
reinterpret_cast<char*>(&s1)[2] = 'b'; // 改变第2个字节
assert(s1 == s2); // 值没有改变

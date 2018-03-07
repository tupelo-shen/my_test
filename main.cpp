#include <cassert>
#include <iostream>
#include "Common.h"


//#include <cassert>
struct Base {};             // 空类
struct Derived1 : Base {
    int i;
};
struct Derived2 : Base {
    Base c;                // Base, 占据1个字节, 剩余字节填充
    int i;
};
struct Derived3 : Base {
    Derived1 c;           // 派生于Base, 占据sizeof(int)个字节
    int i;
};

int main()
{
    // 空基优化没有应用，base占用1个字节，Base成员占据1个字节，随后2个字节的填充，以满足对齐要求
    assert(sizeof(Derived2) == 2*sizeof(int));
 
    // 空基优化没有应用,base成员至少占用1个字节，然后加上填充物以满足第一个成员的对齐要求
    assert(sizeof(Derived3) == 3*sizeof(int));

    SystemPause();
}
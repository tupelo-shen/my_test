#include <cassert>
#include <iostream>
#include "Common.h"

struct S {
    char c;   // 1 byte value
             // 3 bytes padding
    float f;   // 4 bytes value
    bool operator==(const S& arg) const {    // value-based equality
        return c == arg.c && f == arg.f;
    }
};
// assert(sizeof(S) == 8);
S s1 = {'a', 3.14};
S s2 = s1;
//reinterpret_cast<char*>(&s1)[2] = 'b'; // change 2nd byte
// assert(s1 == s2); // value did not change

int main()
{
    std::cout << sizeof(S) << std::endl;

    if (s1 == s2)
    {
        std::cout << "s1 == s2" << std::endl;
    }
    else{
        std::cout << "s1 != s2" << std::endl;
    }

    reinterpret_cast<char *>(&s1)[2] = 'b';
    
    if (s1 == s2)
    {
        std::cout << "s1 == s2" << std::endl;
    }
    else{
        std::cout << "s1 != s2" << std::endl;
    }

    std::cout << *(reinterpret_cast<char *>(&s1)) << std::endl;
    std::cout << *(reinterpret_cast<char *>(&s1) + 1) << std::endl;
    std::cout << *(reinterpret_cast<char *>(&s1) + 2) << std::endl;
    std::cout << *(reinterpret_cast<char *>(&s1) + 3) << std::endl;
    std::cout << *(reinterpret_cast<char *>(&s1) + 4) << std::endl;

    SystemPause();
}

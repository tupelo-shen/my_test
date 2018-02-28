#include <cassert>
#include <iostream>
#include "Common.h"

struct S {
    char c;            
    float f;   
    bool operator==(const S& arg) const {   
        return c == arg.c && f == arg.f;
    }
};


int main(void)
{
    assert(sizeof(S) == 8);
    S s1 = {'a', 3.14};
    S s2 = s1;
    reinterpret_cast<char*>(&s1)[2] = 'b';
    printf("reinterpret_cast<char*>(&s1)[2] = %c\n", reinterpret_cast<char*>(&s1)[2]); 
    assert(s1 == s2);

    SystemPause();
}
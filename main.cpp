//#include <vector>
#include <iostream>
// #include <chrono>
// #include <thread>
#include "Common.h"
// #include "Queue-pthread_s.h"
#include "Stack.h"

#include <cstdio>
#include <new>
#include <map>

// std::map<bool,int> mymap{
//     {false, 10},
//     {true,  3},
//     {false, 7}
// };
// mymap.insert(pair<bool,int>(false,10));
// mymap.insert(pair<bool,int>(true,3));
// mymap.insert(pair<bool,int>(false,7));

int main(void)
{
    int     a[3][4] = { {1, 4, 7, 10}, {2, 5, 8, 11}, {3, 6, 9, 12} } ;
    int*    ptr = &a[0][0];
    int*    tmp = new int[3*4];
    
    // enum _WEEK
    // {
    //     MONDAY,
    //     TUESDAY,
    //     WEDNESDAY,
    //     THURSDAY,
    //     FRIDAY,
    //     SATURDAY,
    //     SUNDAY     
    // };
    
    enum class WEEK
    {
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY,
        SUNDAY     
    };

    WEEK day = WEEK::MONDAY;
    // _WEEK day_0 = 4;

    for ( auto& x : a)
    {
        std::cout << x << std::endl;
    }

    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 3; ++i)
        {
            std::cout << "a[" << i << "][" << j <<"] = "<< a[i][j] << " ; pointer = " << &a[i][j] << std::endl;
        }
    }

    for (int i = 0; i < 12; ++i)
    {
        printf("a + [%d]: 0x%p = %d\n", i, ptr+i, *(ptr+i));
    }

    for (int i = 0; i < 12; ++i)
    {
        printf("tmp + %d = 0x%p\n", i, tmp+i);
    }
    delete [] tmp;
}



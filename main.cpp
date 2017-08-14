#include <iostream>
#include "Common.h"
#include <sstream>
#include <string>
#include <list>
#include <algorithm>

#define MAX_PROMPT_CHAR             (255)

template <typename T>
inline bool FromString(const std::string& sString, T& tX)
{
    std::istringstream iStream(sString);
    return (iStream >> tX) ? true : false;
}

template <typename T>
inline std::string ToString(T tX)
{
    std::ostringstream oStream;
    oStream << tX;
    return oStream.str();
}
void SCMemCopyUnshort(void* to, const void* from,
        unsigned short size)
{
    unsigned short length = 0;
    unsigned int dst = (unsigned int)(to);
    unsigned int src = (unsigned int)(from);

    if((dst == 0) || (src == 0))
    {
        return;
    }

    while(*(*src))
    {
        ptr++;
        length++;
    }

    if(size >= MAX_PROMPT_CHAR)
    {
        size = MAX_PROMPT_CHAR;
    }
    
    if(length < size)
    {
        size = length;
    }

    memcpy(to, from, size * sizeof(*to));
}
using namespace std;
int main()
{ 
    unsigned short array[MAX_PROMPT_CHAR+1] = {1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};
    unsigned short array_a[MAX_PROMPT_CHAR+1] = {0};
    //unsigned short array[10] = {0};
     
    for(int i = 0; i < 20; i++)
    {
        printf("array[%d] = %d; array_a[%d] = %d;\n", i, array[i], i, array_a[i]);
    }
      
    //memcpy(array+3, array_a, sizeof(array_a));
    //memcpy(array+3, 0x0000, 14);
    SCMemCopyUnshort(array_a, array+1, 10);

    for(int i = 0; i < 20; i++)
    {
        printf("array[%d] = %d; array_a[%d] = %d;\n", i, array[i], i, array_a[i]);
    }

    SystemPause();
    return 0;
}

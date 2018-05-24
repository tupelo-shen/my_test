#include <iostream>
#include <string>
#include <typeinfo>
#include <float.h>
#include <limits.h>
#include <math.h>

using namespace std;

// 数字入力のFloat上下限ための構造体定義
typedef struct {
    float               max;    // 上限値
    float               min;    // 下限値
}FLOAT_LIMIT_INFO;
// 数字入力のint上下限ための構造体定義
typedef struct {
    int                 max;    // 上限値
    int                 min;    // 下限値
}INT_LIMIT_INFO;
// 数字入力のuint上下限ための構造体定義
typedef struct {
    unsigned int        max;    // 上限値
    unsigned int        min;    // 下限値
}UINT_LIMIT_INFO;
// 数字入力の上下限ためのunion定義
typedef union _LIMIT_INFO
{
    FLOAT_LIMIT_INFO    float_limit;
    INT_LIMIT_INFO      int_limit;
    UINT_LIMIT_INFO     uint_limit;
} LIMIT_INFO;

typedef struct _KBD_PARAM
{
    unsigned short      max_size;
    LIMIT_INFO          limit;
    _KBD_PARAM():
    max_size(getSize()),
    limit({FLT_MAX, -FLT_MAX}){} 
    unsigned short getSize()
    {
        return 4;
    }
}KBD_PARAM;

template<typename T>
void cutValueByLimit(T& val, const LIMIT_INFO& limit)
{
    if(typeid(T) == typeid(float))
    {
        val = (val > (T)limit.float_limit.max) ? (T)limit.float_limit.max : (val);
        val = (val < (T)limit.float_limit.min) ? (T)limit.float_limit.min : (val);        
    }
    else if(typeid(T) == typeid(int))
    {
        val = (val > (T)limit.int_limit.max) ? (T)limit.int_limit.max : (val);
        val = (val < (T)limit.int_limit.min) ? (T)limit.int_limit.min : (val);
    }
    else
    {
        val = (val > (T)limit.uint_limit.max) ? (T)limit.uint_limit.max : (val);
        val = (val < (T)limit.uint_limit.min) ? (T)limit.uint_limit.min : (val);
    }
}

template<typename T>
void cutValueByMaxVal(T& val, T max_val)
{
    if(val > max_val)
    {
        val = max_val;
    }
    else if(val < (-max_val))
    {
        val = -max_val;
    }
}
#define NKB_INTEGER_BITS_MAX 9

template<typename T, unsigned short digits_max>
T getMaxIntegerValByDigits(unsigned short digits)
{
    T ret_val = T();

    if(digits > digits_max)
    {
        if(typeid(T) == typeid(unsigned int))
        {
            ret_val = static_cast<T>(UINT_MAX);
        }
        else
        {
            ret_val = static_cast<T>(INT_MAX);
        }
        printf("1\n");
        printf("ret_val = %d\n", ret_val);
    }
    else
    {
        for(unsigned int i = 0; i < digits; i++)
        {
            ret_val += (9 * pow(10,i));
            printf("ret_val = %d\n", ret_val);
        }
        printf("2\n");

    }

    return ret_val;
}

LIMIT_INFO  lmt;
KBD_PARAM   m_info;

int main()
{
    lmt.int_limit.max = 10;
    lmt.int_limit.min = -10;
    //m_info.limit.float_limit.min = FLT_MIN;
    int            uint_a = 20;
    printf("int orig: %d\n", uint_a);
    cutValueByLimit(uint_a, lmt);    
    //cutValueByMaxVal<unsigned int>(uint_a, int_b);
    printf("int cutted: %d\n", uint_a);
    printf("============================\n");
    uint_a = getMaxIntegerValByDigits<unsigned int, NKB_INTEGER_BITS_MAX>(8);
    printf("uint max: %d\n", uint_a);
    printf("============================\n");
    printf("============================\n");
    uint_a = getMaxIntegerValByDigits<unsigned int, NKB_INTEGER_BITS_MAX>(11);
    printf("uint max: %u\n", uint_a);
    printf("============================\n");
    uint_a = getMaxIntegerValByDigits<int, NKB_INTEGER_BITS_MAX>(8);
    printf("int max: %d\n", uint_a);
    printf("============================\n");
    uint_a = getMaxIntegerValByDigits<int, NKB_INTEGER_BITS_MAX>(11);
    printf("int max: %d\n", uint_a);
    printf("============================\n"); 
    long total = 0;
    int x = 10, y = 0;
    for(int i = 0; i< 9; i++)
    {
        total += 9 * pow(x,i); /*调用pow函数*/
        printf("%ld\n",total);
    }           
}
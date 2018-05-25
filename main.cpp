#include <iostream>
#include <string>
#include <typeinfo>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <bitset>

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
//
typedef struct _FLOAT_INFO
{
    unsigned int fract3 : 8;
    unsigned int fract2 : 8;
    unsigned int fract1 : 7;
    unsigned int exp    : 8;
    unsigned int sign   : 1;
}FLOAT_INFO;

int explain_float(float a)
{
    FLOAT_INFO* p = (FLOAT_INFO*)&a;
    printf("float : %f\n", a);
    std::cout << "hex: " << std::bitset<sizeof(int)*8>((int&)a) << '\n';
    printf("sign: %d\n", p->sign);
    printf("exp: %d\n", p->exp-127);
    printf("fract1: %f\n", p->fract1/128.0+1);
    printf("fract2: %f\n", p->fract2/256.0/128.0);
    printf("fract3: %f\n", p->fract3/256.0/256.0/128.0);
    double d = p->fract1/128.0+1 + p->fract2/256.0/128.0 + p->fract3/256.0/256.0/128.0;
    printf("tail: %lf\n", d);
    printf("computer number: %lf\n", (p->sign==0?1:-1)*pow(2, p->exp-127)*d);
    printf("============================\n");

    return 0;
}
int ilog2(int a)
{
    float x=a;
    unsigned int ix = (unsigned int&)x;
    unsigned int exp = (ix >> 23) & 0xFF;
    int log2 = int(exp) - 127;

    return log2;
}

short SCGetExp(float value, unsigned short exponent)
{
    short e = 0;

    if (value >= 0.0f)  // 正数
    {
        if (value > powf(2.0f, static_cast<float>(exponent)) ||
                value < powf(2.0f, static_cast<float>(-exponent)))
        {
            e = static_cast<short>(logf(value)/logf(10.0f)+0.00001f);
        }
    }
    else // 非正数
    {
        if (value < -(powf(2.0f, static_cast<float>(exponent))) ||
                value > -(powf(2.0f, static_cast<float>(-exponent))))
        {
            e = -(static_cast<short>(logf(fabsf(value)/logf(10.0f))+0.00001f));
        }
    }

    return e;
}
unsigned int getFloatIntegerDigits(float value)
{
    return (log10f(fabsf(value)) + 1);
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
    int x = 10;
    for(int i = 0; i< 9; i++)
    {
        total += 9 * pow(x,i); /*调用pow函数*/
        printf("%ld\n",total);
    }
    printf("*****************************\n"); 
    explain_float(0);
    explain_float(1);
    explain_float(0.25);
    explain_float(0.75);
    explain_float(6);
    explain_float(128);
    explain_float(-123.456);  

    printf("*****************************\n");
    int exp;
    float float_a = 123.123f;   
    exp = getFloatIntegerDigits(float_a);
    printf("%f exp = %d\n",float_a, exp); 
    float_a = -123.123f;   
    exp = getFloatIntegerDigits(float_a);
    printf("%f exp = %d\n",float_a, exp);
    float_a = -1234.123f;   
    exp = getFloatIntegerDigits(float_a);
    printf("%f exp = %d\n",float_a, exp);        
        
}
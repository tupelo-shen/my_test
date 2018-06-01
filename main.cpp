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

const float             nkb_dec_digits_max[7]  =
        {0.0f, 0.9f, 0.99f, 0.999f, 0.9999f, 0.99999f, 0.999999f};
const float             nkb_float_integer_digits_max[30]  = {
    9.999999e+7f, 
    9.999999e+8f, 
    9.999999e+9f, 
    9.99999e+10f,
    9.99999e+11f,
    9.99999e+12f,
    9.99999e+13f,
    9.99999e+14f,
    9.99999e+15f,
    9.99999e+16f,
    9.99999e+17f,
    9.99999e+18f,
    9.99999e+19f,
    9.99999e+20f,
    9.99999e+21f,
    9.99999e+22f,
    9.99999e+23f,
    9.99999e+24f,
    9.99999e+25f,
    9.99999e+26f,
    9.99999e+27f,
    9.99999e+28f,
    9.99999e+29f,
    9.99999e+30f,
    9.99999e+31f,
    9.99999e+32f,
    9.99999e+33f,
    9.99999e+34f,
    9.99999e+35f,
    9.99999e+36f,
    };        
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
        else if(typeid(T) == typeid(int))
        {
            ret_val = static_cast<T>(INT_MAX);
        }
        else if(typeid(T) == typeid(float))
        {
            ret_val = static_cast<T>(FLT_MAX);
        }
    }
    else
    {
        if(typeid(T) == typeid(float))
        {
            if(digits > 7)
            {
                ret_val = nkb_float_integer_digits_max[digits-7-1];
            }
            else
            {
                ret_val = static_cast<T>(pow(10.0f,static_cast<int>(digits)) - 1);
            }
        }
        else
        {
            ret_val = static_cast<T>(pow(10.0f,static_cast<int>(digits)) - 1);
        }
        //printf("ret_val = %f\n", ret_val);
    }

    return ret_val;
}
//float getMaxFloatValByDigits
float getMaxDecValByDpDigits(unsigned short digits)
{
    if(digits > (7 - 1))
    {
        digits = 7-1;
    }
    return nkb_dec_digits_max[digits];
}


int ilog2(int a)
{
    float x=a;
    unsigned int ix = (unsigned int&)x;
    unsigned int exp = (ix >> 23) & 0xFF;
    int log2 = int(exp) - 127;

    return log2;
}

int getFloatIntegerDigits(float value)
{
    int ret_val;
    
    if(value == 0.0f)
    {
        ret_val = 0;
    }
    else
    {
        int exp = log10f(fabsf(value));
        if(exp >= 0)
        {
            ret_val = exp + 1;
        }
        else
        {
            ret_val = exp;
        }
    }

    return ret_val;
}

int main()
{

    int int_a = getMaxIntegerValByDigits<int, 9>(8);
    printf("int_a = %d;\n", int_a);

    int_a = getMaxIntegerValByDigits<int, 9>(9);
    printf("int_a = %d;\n", int_a);
    float a = int_a;
    printf("a = %f;\n", a);

    int_a = getMaxIntegerValByDigits<int, 9>(10);
    printf("int_a = %d;\n", int_a);


    float float_c;
    float float_a = getMaxIntegerValByDigits<float, 37>(3);
    float float_b = getMaxDecValByDpDigits(5);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c); 

    float_a = getMaxIntegerValByDigits<float, 37>(2);
    float_b = getMaxDecValByDpDigits(6);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(1);
    float_b = getMaxDecValByDpDigits(7);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(0);
    float_b = getMaxDecValByDpDigits(8);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(4);
    float_b = getMaxDecValByDpDigits(3);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %7f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(5);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %7f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(6);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %7f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(7);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(8);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a;// + float_b;
    printf("float_a = %x; float_b = %f;\n", *(unsigned int*)&float_a, float_b);
    printf("float_ab = %f\n", float_a);

    float_a = getMaxIntegerValByDigits<float, 37>(9);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a;// + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(10);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a;// + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(11);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a;// + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(20);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a;// + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);

    float_a = getMaxIntegerValByDigits<float, 37>(30);
    float_b = getMaxDecValByDpDigits(4);
    float_c = float_a;// + float_b;
    printf("float_a = %f; float_b = %f;\n", float_a, float_b);
    printf("float_ab = %f\n", float_c);


    int int_b = getFloatIntegerDigits(0.001);
    printf("int_b = %d;\n", int_b);
    int_b = getFloatIntegerDigits(0.01);
    printf("int_b = %d;\n", int_b); 
    int_b = getFloatIntegerDigits(0.1);
    printf("int_b = %d;\n", int_b);  
    int_b = getFloatIntegerDigits(0.0);
    printf("int_b = %d;\n", int_b);  
    int_b = getFloatIntegerDigits(1.234);
    printf("int_b = %d;\n", int_b);     
    int_b = getFloatIntegerDigits(12.34);
    printf("int_b = %d;\n", int_b); 

    int_b = getFloatIntegerDigits(-12.34);
    printf("int_b = %d;\n", int_b);                           
}
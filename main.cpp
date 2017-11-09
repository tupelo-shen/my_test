#include <cassert>
#include <iostream>
#include <cmath>
#include "Common.h"

// class ClassA
// {
// private:
//     int m_a,m_b,m_sum;

// public:
//     ClassA(int a, int b);
//     virtual ~ClassA();

//     void            add(int a, int b);
//     void            printClassA(void);
//     static

// };

// ClassA::ClassA(int a, int b) :
// m_a(a),
// m_b(b)
// {

// }
// ClassA::~ClassA()
// {
    
// }
// void ClassA::add(int a, int b)
// {
//     this->m_sum = a + b;
// }
// void ClassA::printClassA(void)
// {
//     printf("ClassA.m_a,ClassA.m_b: %d, %d\n", this->m_a, this->m_b);
//     printf("ClassA.m_sum: %d\n", m_sum);
// }

#define BASE_EXPONENT_ARRAY_SIZE            (41u)
#define BASE_EXPONENT_OUT_OF_LIMIT          (1.0e+21f)
#define LIMIT_EXPONENT                      (1.0e+6f)

bool IsFloatEqual(float compare_value, float ref_value)
{

    float ref_value_exponent = BASE_EXPONENT_OUT_OF_LIMIT; 

    const float base_exponent_array[BASE_EXPONENT_ARRAY_SIZE] =
    {
        1.0e-20f,
        1.0e-19f,
        1.0e-18f,
        1.0e-17f,
        1.0e-16f,
        1.0e-15f,
        1.0e-14f,
        1.0e-13f,
        1.0e-12f,
        1.0e-11f,
        1.0e-10f,
        1.0e-9f,
        1.0e-8f,
        1.0e-7f,
        1.0e-6f,
        1.0e-5f,
        1.0e-4f,
        1.0e-3f,
        1.0e-2f,
        1.0e-1f,
        1.0f,
        1.0e+1f,
        1.0e+2f,
        1.0e+3f,
        1.0e+4f,
        1.0e+5f,
        1.0e+6f,
        1.0e+7f,
        1.0e+8f,
        1.0e+9f,
        1.0e+10f,
        1.0e+11f,
        1.0e+12f,
        1.0e+13f,
        1.0e+14f,
        1.0e+15f,
        1.0e+16f,
        1.0e+17f,
        1.0e+18f,
        1.0e+19f,
        1.0e+20f
    };

    if (ref_value == 0.0f)
    {
        ref_value_exponent = 1.0f;
    }
    else
    {
        for(unsigned int i = 0; i < BASE_EXPONENT_ARRAY_SIZE; i++)
        {
            float ref_value_mantissa = ref_value / base_exponent_array[i];

            if(ref_value_mantissa >= 1.0f && ref_value_mantissa < 10.0f)
            {
                ref_value_exponent = base_exponent_array[i];
            }
            
        }       
    }

    if (ref_value_exponent == BASE_EXPONENT_OUT_OF_LIMIT)
    {
        return false;
    }

    int compare_value_int = compare_value / ref_value_exponent * LIMIT_EXPONENT;
    int ref_value_int = ref_value / ref_value_exponent * LIMIT_EXPONENT;
    compare_value_int = (compare_value_int + 5) / 10;
    ref_value_int = (ref_value_int + 5) / 10;

    if(compare_value_int == ref_value_int)
    {
        return true;
    }
    else
    {
        return false;  
    }   
}

int main(int argc, char *argv[])
{
#if 0
    ClassA *a = new ClassA(5, 4);
    a->printClassA();
    a->add(1000, 2000);
    a->printClassA();
#endif
    #if 1
    int max_size = 100;
    float a[max_size] = 
    {
        0.999f/powf(10.0f, -9.0f),
        0.9990001f/powf(10.0f, -9.0f),
        0.999001f/powf(10.0f, -9.0f),
        0.99901f/powf(10.0f, -9.0f),
        0.9991f/powf(10.0f, -9.0f),
        -0.999f/powf(10.0f, -9.0f),
        -0.9990001f/powf(10.0f, -9.0f),
        -0.999001f/powf(10.0f, -9.0f),
        -0.99901f/powf(10.0f, -9.0f),
        -0.9991f/powf(10.0f, -9.0f),
        0.00000001f,
        0.0000001f,
        0.000001f,
        0.00001f,
        0.0001f,
        0.001f,
        0.01f,
        0.1f,   
        0.0f,
        -0.0f,
        0.00000009f,
        0.0000009f,
        0.000009f,
        0.00009f,
        0.0009f,
        0.009f,
        0.09f,
        0.9f,
        0.00000001f,
        0.0000001f,
        0.000001f,
        0.00001f,
        0.0001f,
        0.001f,
        0.01f,
        0.1f,
    };
    float b[max_size] = 
    {
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        999.0f * powf(10.0f, 6.0f),
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0000001f,
        0.000001f,
        0.00001f,
        0.0001f,
        0.001f,
        0.01f,
        0.1f,
        1.0f,
        -0.00000001f,
        -0.0000001f,
        -0.000001f,
        -0.00001f,
        -0.0001f,
        -0.001f,
        -0.01f,
        -0.1f,
    };

    for (int i = 0; i < max_size; i++)
    {

        printf("Number: #[%d]\n", i);
        if(a[i] == b[i])
        {
            printf("%lf == %lf before calling function IsFloatEqual()\n" ,a[i], b[i]);
        }
        else
        {
            printf("%lf != %lf before calling function IsFloatEqual()\n",a[i], b[i]);
        }

        if(IsFloatEqual(a[i], b[i]))
        {
            printf("%lf == %lf after calling function IsFloatEqual()\n",a[i], b[i]);
        }
        else
        {
            printf("%lf != %lf after calling function IsFloatEqual()\n",a[i], b[i]);
        }
        std::cout << "---------------------------------------------------" << std::endl;
    }

    #endif
    //SystemPause();
}

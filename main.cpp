#include <cassert>
#include <iostream>
#include <cmath>
#include "Common.h"

bool IsFloatEqual(float compare_value, float ref_value)
{

    float base_exponent = 6.0f;
    float base_power = 10.0f;
    float min_exponent = -20.0f; // marco
    float max_exponent = 20.0f;     // marco
    float ref_value_exponent = 1000.0f; 

    if (ref_value == 0.0f)
    {
        ref_value_exponent = 0;
    }
    else
    {
        for(int i = min_exponent; i <= max_exponent; i++)
        {
            float ref_value_mantissa = ref_value / powf(base_power, static_cast<float>(i));

            if(ref_value_mantissa >= 1.0f && ref_value_mantissa < 10.0f)
            {
                ref_value_exponent = static_cast<float>(i);
            }
            
        }       
    }


    // [10e-12, 10e+12]
    if (ref_value_exponent > max_exponent || ref_value_exponent < min_exponent)
    {
        return false;
    }

    int compare_value_int = compare_value * powf(base_power, base_exponent - ref_value_exponent);
    int ref_value_int = ref_value * powf(base_power, base_exponent - ref_value_exponent);
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


    //SystemPause();
}

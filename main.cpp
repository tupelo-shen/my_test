#include <iostream>
#include <math.h>

float a = 0.999f/1.0e-9f;
float b = 9.99e+8;

int main(void)
{
    printf("a = %f\n b = %f\n", a, b);

    float err1 = fabsf(a-b);
    float err2 = fabsf((a-b)/b);

    printf("err1 = %f\nerr2 = %f\n", err1, err2);

    char test[10]={'a','b','c','d',0};
    for(int i=0; i<10; i++)
    {
        printf("char i = %c\n", test[i]);
    }

    sprintf(test, "%d", 5);
    for(int i=0; i<10; i++)
    {
        printf("char i = %c\n", test[i]);
    }

    return 0;   
}

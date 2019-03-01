#include <iostream>
#include <ctime>
#include "stl_math.h"

using namespace ana_stl;

time_t start_time;
time_t end_time;

int main(void)
{
    start_time = time(NULL);
    fibonacci(1);
    end_time = time(NULL);
    printf("time difference = %ld \n", end_time - start_time);

    start_time = time(NULL);
    fibonacci(10);
    end_time = time(NULL);
    printf("time difference = %ld \n", end_time - start_time);

    start_time = time(NULL);
    fibonacci(50);
    end_time = time(NULL);
    printf("time difference = %ld \n", end_time - start_time);

    test m_test;
    
    printf("m_test.add = %ld \n", m_test.add(1, 2));
    return 0;
}



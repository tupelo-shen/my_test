#include "stl_math.h"

using namespace ana_stl;

/*
 * desc.        斐波那契数列
 */
int ana_stl::fibonacci(int n)
{
    if (n <= 1) {
        return 1;
    }
    else {
        return ::fibonacci(n-1) + ::fibonacci(n-2);
    }
}


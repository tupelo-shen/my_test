#include <string>
#include <iostream>
#include "Common.h"

using namespace std;

int main()
{
    float val = -1.0f;
    char str[3] = {'t','2',0};

    sscanf(str, "%f", &val);

    printf("val = %f \n", val);

    SystemPause();
}

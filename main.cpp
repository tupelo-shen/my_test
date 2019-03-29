#include <iostream>
#include <boost/asio.hpp>
#include "SIM_SerialPort.h"
#include "COM_CrcCalculation.h"

using namespace std;

int main()
{
    unsigned char test_data[4] = {0x00,0x80,0xED,0x44};

    printf("test_data[0] of address = %d\n", (unsigned int)&test_data[0]);
    printf("test_data[1] of address = %d\n", (unsigned int)&test_data[1]);
    printf("test_data[2] of address = %d\n", (unsigned int)&test_data[2]);
    printf("test_data[3] of address = %d\n", (unsigned int)&test_data[3]);
    printf("test_data[0] = %2x\n", test_data[0]);
    printf("test_data[1] = %2x\n", test_data[1]);
    printf("test_data[2] = %2x\n", test_data[2]);
    printf("test_data[3] = %2x\n", test_data[3]);

    float * ptr = (float* )test_data;
    printf("float ptr = %f\n", *ptr);
    return 0;
}



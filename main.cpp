#include <iostream>
#include <unistd.h>
#include "test.h"
#include "Average.h"

int main(void)
{
    Average avg(0, 0);
    
    avg += 4;
    std::cout << avg << '\n'; // 4 / 1 = 4
    
    avg += 8;
    std::cout << avg << '\n'; // (4 + 8) / 2 = 6

    avg += 24;
    std::cout << avg << '\n'; // (4 + 8 + 24) / 3 = 12

    avg += -10;
    std::cout << avg << '\n'; // (4 + 8 + 24 - 10) / 4 = 6.5

    (avg += 6) += 10; // 2 calls chained together
    std::cout << avg << '\n'; // (4 + 8 + 24 - 10 + 6 + 10) / 6 = 7

    Average copy = avg;
    std::cout << copy << '\n';

	sleep(10);
	
	return 0;
}

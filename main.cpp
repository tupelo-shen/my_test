#include <iostream>
#include <unistd.h>
#include "test.h"

int main(void)
{
	int a = -3, b = 5;
	
	std::cout << " Print Result! " << std::endl;
	std::cout << add(a, b) << std::endl;
	
	sleep(10);
	
	return 0;
}

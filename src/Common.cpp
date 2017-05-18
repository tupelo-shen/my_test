
#include <iostream>
#include "Common.h"

void SystemPause(void)
{
    std::cin.clear();       // reset any error flags
    // ignore any characters in the input buffer until we find an enter character
    std::cin.ignore(32767, '\n');       
    std::cin.get();         // ger one more char from the user
}
#include <iostream>
#include "Common.h"
#include "basicDerived.h"
#include "BindingTest.h"
#include "VirtualTest.h"

int main()
{
    Derived1 *derived1 = new Derived1(5);
    Base1 *base1 = derived1;
    delete base1;
    
    SystemPause();
    
    return 0;
}

#ifndef _VIRTUAL_TEST_H_
#define _VIRTUAL_TEST_H_

#include <iostream>

class Base1
{
public:
    virtual ~Base1() // note: not virtual
    {
        std::cout << "Calling ~Base1()" << std::endl;
    }
};

class Derived1 : public Base1
{
private:
    int* m_array;
public:
    Derived1(int length)
    {
        m_array = new int[length];
    }
    virtual ~Derived1() // note: virtual
    {
        std::cout << "Calling ~Derived1()" << std::endl;
        delete[] m_array;
    }
};


#endif /* _VIRTUAL_TEST_H_ */
#ifndef _BASIC_DERIVED_H_
#define _BASIC_DERIVED_H_

#include <iostream>

class Base
{
protected:
    int m_value;
 
public:
    Base(int value=0)
        : m_value(value)
    {
        std::cout << "Base\n";
    }
 
    //int getvalue() const { return m_value; }
    void identify() { std::cout << "I am a Base\n"; }
};

class Derived: public Base
{
public:
    double m_cost;
 
    Derived(int value, double cost=0.0 )
        : Base(value), m_cost(cost)
    {
        std::cout << "Derived\n";
    }
 
    double getCost() const { return m_cost; }
    int getValue() const { return m_value; }
    // Here's our modified function
    void identify() { std::cout << "I am a Derived\n"; }
};

#endif /* _BASIC_DERIVED_H_ */

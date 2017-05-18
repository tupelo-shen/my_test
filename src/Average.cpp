#include "Average.h"

Average::Average(double sum = 0, double nbr = 0) 
    : m_sum(sum), m_nbr(nbr)
{

}

std::ostream & operator<<(std::ostream &out, Average & avg)
{
    out << (static_cast<double>(avg.m_sum) / avg.m_nbr);

    return out;
}
Average & Average::operator+=(int num)
{
    m_sum += num;
    ++m_nbr;

    return *this;
}
#ifndef _POINT_H_
#define _POINT_H_

#include <iostream>

class Average
{
private:
    int             m_sum ;
    unsigned char   m_nbr ;
public:
    Average(){};
    Average(double, double nbr);
    ~Average(){};
	
    friend std::ostream & operator<<(std::ostream & out, Average & avg);
	Average & operator+=(int num);
};

#endif /* _POINT_H_ */
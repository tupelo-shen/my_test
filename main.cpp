#include <iostream>
#include "Common.h"
#include "basicDerived.h"
#include "BindingTest.h"
#include "VirtualTest.h"

#include <list>
#include <algorithm>

    const float*        m_range_table;          // range table
    unsigned long       m_range_table_size;     // range table size
    unsigned long       m_range;
void setRangeList(unsigned long size, const float* list)
{
    if(size > 0)
    {
        m_range_table = list;
        m_range_table_size = size;
    }
}
float getRangeValue()
{
    float lvl;
    if(m_range < m_range_table_size)
    {
        lvl = m_range_table[m_range];
    }else
    {
        lvl = m_range_table[0];
    }

    return lvl;    
}

int main()
{
    using namespace std;

    // list<int> li;
    // for (int nCount = 0; nCount < 6; nCount++)
    //     li.push_back(nCount);

    // list<int>::const_iterator it;   // declare an iterator
    // it = min_element(li.begin(), li.end());
    //     cout << *it << " ";
    // it = max_element(li.begin(), li.end());
    // cout << *it << "\n";

    // list<int> li;
    // for (int i = 0; i < 6; i++)
    //     li.push_back(i);

    // list<int>::iterator it; //declare an iterator
    // it = find(li.begin(), li.end(), 3); // find the value 3 in the list
    // li.insert(it, 8); // use list::insert to insert the value 8 before it

    // for (it  = li.begin(); it != li.end(); it++) // for loop with iterators
    //     cout << *it << " ";

    // cout << endl;
    static const float default_range_table[] = { 6.0,4.0,2.0 };
    unsigned long i = sizeof(default_range_table) / sizeof(float);
    setRangeList(i , default_range_table);
    float f = getRangeValue();
    
    cout << i << endl;    
    SystemPause();
    return 0;
}

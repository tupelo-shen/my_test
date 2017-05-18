#ifndef _INTARRAY_H_
#define _INTARRAY_H_

#include <cassert>       // for assert
// #include <initializer_list>

class IntArray
{
private:
    int m_length;
    int *m_data;
public:
    IntArray() : m_length(0), m_data(NULL)
    {

    }
    IntArray(int length) : m_length(length)
    {
        m_data = new int[length];
    }
    // allow IntArray to be initialized via list initialization
    // IntArray(const std::initializer_list<int> &list): 
    //     IntArray(list.size()) // use delegating constructor to set up initial array
    // {
    //     // Now initialize our array from the list
    //     int count = 0;
    //     for (auto &element : list)
    //     {
    //         m_data[count] = element;
    //         ++count;
    //     }
    // }
    ~IntArray()
    {
        delete[] m_data;
    }
    void erase()
    {
        delete[] m_data;

        // We need to make sure we set m_data to 0 here, otherwise it 
        // be left pointing at deallocated memory!
        m_data = NULL;
        m_length = 0;
    }

    int& operator[](int index)
    {
        assert(index >= 0 && index < m_length);
        return m_data[index];
    }

    int getLength() { return m_length;}
    void reallocate(int newLength);
    void resize(int newLength);
    void insertBefore(int value, int index);
    void remove(int index);
    void insertAtBeginning(int value);
    void insertAtEnd(int value);
};
#endif
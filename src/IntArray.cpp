
#include "IntArray.h"

/*
 * Func name:   reallocate
 * Func disc:   reallocate resizes the array. Any existing elements
 *              will be destroyed. This function operates quickly.
 * Func Param:  [in] newLength
 * Func RetVal: None.
 */
void IntArray::reallocate(int newLength)
{
    // First, we delete any existing elements
    erase();

    // If our array is going to be empty now, return here
    if(newLength <= 0)
        return;

    // Then we have to allocate new elements.
    m_data = new int[newLength];
    m_length = newLength;
}
/*
 * Func name:   resize
 * Func disc:   resize resizes the array.  Any existing elements 
 *              will be kept.  This function operates slowly.
 * Func Param:  [in] newLength
 * Func RetVal: None.
 */
void IntArray::resize(int newLength)
{
    if (newLength == m_length)
        return;

    if (newLength <= 0)
    {
        erase();
        return;
    }

    int *data = new int[newLength];

    if (m_length > 0)
    {
        int elementsToCopy = (newLength > m_length) ? m_length : newLength;

        for (int index = 0; index < elementsToCopy; ++index)
            data[index] = m_data[index];
    }

    delete[] m_data;

    m_data = data;
    m_length = newLength;
}
/*
 * Func name:   insertBefore
 * Func disc:   insert our new element into the new array
 * Func Param:  [in]    value
 *                      index
 * Func RetVal: None.
 */
void IntArray::insertBefore(int value, int index)
{
    assert(index >= 0 && index <= m_length);

    int *data = new int[m_length + 1];

    for(int before = 0; before < index; ++before)
        data[before] = m_data[before];

    // insert our new element into the new array
    data[index] = value;

    for(int after = index; after < m_length; ++after)
        data[after + 1] = m_data[after];

    // Finally, delete the old array, and use the new array instead
    delete[] m_data;
    m_data = data;
    ++m_length;
}
/*
 * Func name:   remove
 * Func disc:   
 * Func Param:  [in] index
 * Func RetVal: None.
 */
void IntArray::remove(int index)
{
    assert(index >= 0 && index <= m_length);

    if (m_length == 1)
    {
        erase();
        return;
    }

    // First create a new array one element smaller than the old array
    int *data = new int[m_length - 1];

    for (int before = 0; before < index; ++before)
        data[before] = m_data[before];

    for (int after = index + 1; after < m_length; ++after)
        data[after - 1] = m_data[after];

    delete[] m_data;
    m_data = data;
    --m_length;
}
/*
 * Func name:   insertAtBeginning
 * Func disc:   
 * Func Param:  [in] value
 * Func RetVal: None.
 */
void IntArray::insertAtBeginning(int value)
{
    insertBefore(value, 0);
}
/*
 * Func name:   insertAtEnd
 * Func disc:   
 * Func Param:  [in] value
 * Func RetVal: None.
 */
void IntArray::insertAtEnd(int value)
{
    insertBefore(value, m_length);
}
/*
 * @Module Name：    Queue-pthread.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-02-27
 */
#ifndef __STACK_H__
#define __STACK_H__

#include <exception>

template<typename T, int MaxSize>
class Stack
{
public:
    Stack():index(0){}
    bool        empty() const { return index = 0;}
    bool        full() const { return index == MaxSize;}
    void        push(const T&);
    void        pop(void);
    T&          top(void);
    const T&    top(void) const;

private:
    int         index;
    T           elements[MaxSize];
};

template<typename T, int MaxSize>
void Stack<T, MaxSize>::push(const T& element)
{
    if(full())
        throw std::out_of_range("Stack<>::push(): full stack");
    elements[index++] = element;
}

template<typename T, int MaxSize>
void Stack<T, MaxSize>::pop(void)
{
    if (!empty())
        index--;
    else
        throw std::out_of_range("Stack<>::pop(): empty stack");
}

template<typename T, int MaxSize>
T& Stack<T, MaxSize>::top(void)
{
    if (empty())
        throw std::out_of_range("Stack<>::top(): empty stack");
    return elements[index-1];
}

template<typename T, int MaxSize>
const T& Stack<T, MaxSize>::top(void) const
{
    if (empty())
        throw std::out_of_range("Stack<>::top(): empty stack");
    return elements[index-1];
}

#endif /* __STACK_H__ */
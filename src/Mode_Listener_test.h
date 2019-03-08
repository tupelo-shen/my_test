#ifndef __MODE_LISTENER_TEST_H__
#define __MODE_LISTENER_TEST_H__

#include <iostream>

using namespace std;

// Listener 基类
class AbstractListener
{
public:
    AbstractListener() {};
    virtual ~AbstractListener() {};

    virtual void actionPerformed(int state) = 0;
};

// Listener 监听类
class StateListener: public AbstractListener
{
public:
    StateListener() {};
    virtual ~StateListener() {};

    virtual void actionPerformed(int state);
};

// Listener center(类似于操作系统（os）)
class ListenerCenter
{
private:
    StateListener   listenerList[50];       // 存储listener的数组
    int             count;                  // 当前已有的listener个数

public:
    ListenerCenter():count(0) {};
    virtual ~ListenerCenter() {};

    int             getCount() { return count; }
    void            addListener(StateListener& listener) { listenerList[this->count]=listener; return this->count;}
    void            tiggeredStateListener(int state);
};

ListenerCenter::tiggeredStateListener(int state)
{
    for(int i=0;i<this->count;i++){  
        listenerList[i].actionPerformed(state);  
    } 
}
#endif /* __MODE_LISTENER_TEST_H__ */
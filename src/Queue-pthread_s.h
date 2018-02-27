/*
 * @Module Name：    Queue-pthread.h 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-02-27
 */
#ifndef __QUEUE_PTHREAD_S_H__
#define __QUEUE_PTHREAD_S_H__

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <initializer_list>

/*
 * 线程安全队列
 * T为队列元素类型
 * 因为有std::mutex和std::condition_variable类成员,所以此类不支持复制构造函数也不支持赋值操作符(=)
 */
template <typename Type>
class Queue_s
{
private:
    mutable std::mutex                  m_mutex;
    mutable std::condition_variable     m_cv;
    using   queue_type = std::queue<Type>;
    queue_type                          m_queue;     
    //std::queue<Type>            *m_queue;

public:
    using value_type= typename queue_type::value_type;
    using container_type = typename queue_type::container_type;
    Queue_s()=default;
    Queue_s(const Queue_s&)=delete;
    Queue_s& operator=(const Queue_s&)=delete;

    /*
     * 使用迭代器为参数的构造函数,适用所有容器对象
     */
    template<typename _InputIterator>
    Queue_s(_InputIterator first, _InputIterator last)
    {
        for(auto itor=first;itor!=last;++itor){
            m_queue.push(*itor);
        }
    }
    explicit Queue_s(const container_type &c):m_queue(c){}
    /*
     * 使用初始化列表为参数的构造函数
     */
    Queue_s(std::initializer_list<value_type> list):Queue_s(list.begin(),list.end()){}

    /*
     * 将元素加入队列
     */
    void push(const value_type &new_value)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_queue.push(std::move(new_value));
        m_cv.notify_one();
    }

    /*
     * 从队列中弹出一个元素,如果队列为空就阻塞
     */
    value_type wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_cv.wait(lk, [this]{return !this->m_queue.empty();});
        auto value=std::move(m_queue.front());
        m_queue.pop();
        return value;
    }

    /*
     * 从队列中弹出一个元素,如果队列为空返回false
     */
    bool try_pop(value_type& value)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if(m_queue.empty())
            return false;
        value=std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    /*
     * 返回队列是否为空
     */
    auto empty() const->decltype(m_queue.empty()) 
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_queue.empty();
    }

    /*
     * 返回队列中元素数个
     */
    auto size() const->decltype(m_queue.size())
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_queue.size();
    }
}; /* Queue_s */

#endif /* __QUEUE_PTHREAD_S_H__ */
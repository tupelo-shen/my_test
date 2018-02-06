#include <iostream>
//#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <initializer_list>

// 这个是线程安全的队列，Synchoronized类的实现在下面
// 因为有std::mutex和std::condition_variable类成员,所以此类不支持复制构造函数也不支持赋值操作符(=)
template <class Type>
class Queue_s
{
private:
    std::mutex                  m_mutex;
    std::condition_variable     m_cv;     
    std::queue<Type>            *m_queue;

public:
    Queue_s()
    {
        m_queue = new std::queue<Type>();
    };
    Queue_s(const Queue_s&)=delete;
    Queue_s& operator=(const Queue_s&)=delete;

    /*
     * 使用迭代器为参数的构造函数,适用所有容器对象
     * */
    // template<typename _InputIterator>
    // threadsafe_queue(_InputIterator first, _InputIterator last){
    //     for(auto itor=first;itor!=last;++itor){
    //         data_queue.push(*itor);
    //     }
    // }

    ~Queue_s(){delete m_queue;};

    void push(const Type& type)
    {
        std::lock_guard<std::mutex> lk(m_mutex);   
        
        m_queue->push(type);  
        try  
        {  
            m_cv.notify_one();  
        }  
        catch(std::exception& ex)  
        {  
            std::cout << ex.what()<< std::endl;  
            throw;  
        } 
    };
    Type pop_nonblock()
    {
        Type type;  
   
        m_mutex.lock();
        while(!m_queue->empty())
        {  
            type = m_queue->front();  
            m_queue->pop();                 
        }
        m_mutex.unlock();

        return type;
    };
    Type pop_block()
    {
        Type type;  

        m_mutex.lock();
        // while(m_queue->empty())
        // {  
        //     try  
        //     {  
        //         wait();
        //     }
        //     catch(exception& ex)
        //     {
        //         cout<< ex.what()<<endl;  
        //         throw;  
        //     }                  
        // }  

        type = m_queue->front();  
        m_queue->pop();
        m_mutex.unlock();

        return type; 
    };
};

/*
 * @func:   Queue_s()
 * @Desc:   构造函数
 */
// template <class Type>
// Queue_s<Type>::Queue_s()
// {
//     m_queue = new std::queue<Type>();
// }

/*
 * @func:   ~Queue_s()
 * @Desc:   析构函数
 */
// template <class Type>
// Queue_s<Type>::~Queue_s()
// {
//     delete m_queue;
// }

/*
 * @func:   pop()
 * @Desc:   从队列中弹出一项
 */
// template <class Type>     
// Type Queue_s<Type>::pop_nonblock()
// {  
//     Type type;  
   
//     m_mutex.lock();
//     while(!m_queue->empty())
//     {  
//         type = m_queue->front();  
//         m_queue->pop();                 
//     }
//     m_mutex.unlock();

//     return type; 
// }

// /*
//  * @func:   pop()
//  * @Desc:   从队列中弹出一项
//  */
// template <class Type>     
// Type Queue_s<Type>::pop_block()
// {  
//     Type type;  
   
//     m_mutex.lock();
//     // while(m_queue->empty())
//     // {  
//     //     try  
//     //     {  
//     //         wait();
//     //     }
//     //     catch(exception& ex)
//     //     {
//     //         cout<< ex.what()<<endl;  
//     //         throw;  
//     //     }                  
//     // }  

//     type = m_queue->front();  
//     m_queue->pop();
//     m_mutex.unlock();

//     return type; 
// }

/*
 * @func:   push()
 * @Desc:   压入队列
 */
// template <class Type>    
// void Queue_s<Type>::push(const Type &type)
// {  
//     std::lock_guard<std::mutex> lk(m_mutex);   
    
//     m_queue->push(type);  
//     try  
//     {  
//         m_cv.notify_one();  
//     }  
//     catch(std::exception& ex)  
//     {  
//         std::cout << ex.what()<< std::endl;  
//         throw;  
//     }   
// }  

int f(Queue_s<int>& queue)
{
    static int i = 0;
    //synchronized { // begin synchronized block
        std::cout << i << " -> ";
        ++i;       // each call to f() obtains a unique value of i
        queue.push(i);
        std::cout << i << '\n';
        return i; // end synchronized block
    //}
}
#include <vector>
int testQueue_s()
{
    Queue_s<int> q();
    //Queue_s<int> &q_tmp = q;
    std::vector<std::thread> v(10);
    for(auto& t: v)
        t = std::thread([]{ for(int n = 0; n < 10; ++n) f(q); });
    for(auto& t: v)
        t.join();
    return 0;
}
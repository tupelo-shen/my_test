#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

// 这个是线程安全的队列，Synchoronized类的实现在下面
template <class Type>
class SyncQueue
{
public:
    SyncQueue();
    ~SyncQueue();
    void                push(const Type&);
    Type                pop();
    Type
private:
    std::mutex          m_mutex;
    std::condition_variable     
    queue<Type>         *m_queue;

};

/*
 * @func:   SyncQueue()
 * @Desc:   构造函数
 */
template <class Type>
SyncQueue<Type>::SyncQueue()
{
    m_queue = new queue<Type>() ;
}

/*
 * @func:   ~SyncQueue()
 * @Desc:   析构函数
 */
template <class Type>
SyncQueue<Type>::~SyncQueue()
{
    delete m_queue;
}

/*
 * @func:   pop()
 * @Desc:   从队列中弹出一项
 */
template <class Type>     
Type SyncQueue<Type>::pop_nonblock()
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
}

/*
 * @func:   pop()
 * @Desc:   从队列中弹出一项
 */
template <class Type>     
Type SyncQueue<Type>::pop_block()
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
}

/*
 * @func:   push()
 * @Desc:   压入队列
 */
template <class Type>    
void SyncQueue<Type>::push(const Type &type)
{  
   
    m_mutex.lock();  
    m_queue->push(type);  
    try  
    {  
        notify();  
    }  
    catch(exception& ex)  
    {  
        cout<<ex.what()<<endl;  
        throw;  
    }                  
    m_mutex.unlock();     
}  

int f()
{
    static int i = 0;
    synchronized { // begin synchronized block
        std::cout << i << " -> ";
        ++i;       // each call to f() obtains a unique value of i
        std::cout << i << '\n';
        return i; // end synchronized block
    }
}

int testTransactionalMemory()
{
    std::vector<std::thread> v(10);
    for(auto& t: v)
        t = std::thread([]{ for(int n = 0; n < 10; ++n) f(); });
    for(auto& t: v)
        t.join();
}
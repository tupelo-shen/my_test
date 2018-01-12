#include <iostream>
#include <vector>
#include <thread>

#include <queue>

using namespace std;

// 这个是线程安全的队列，Synchoronized类的实现在下面
template <class Type>
class SyncQueue : public Synchronized
{
public:
    SyncQueue();
    ~SyncQueue();
    void        push(const Type&);
    Type        pop();
private:
    queue<Type> *pQueue;
};

/*
 * @func:   SyncQueue()
 * @Desc:   构造函数
 */
template <class Type>
SyncQueue<Type>::~SyncQueue()
{
    pQueue = new queue<Type>() ;
}

/*
 * @func:   ~SyncQueue()
 * @Desc:   析构函数
 */
template <class Type>
SyncQueue<Type>::~SyncQueue()
{
    delete pQueue;
}

/*
 * @func:   pop()
 * @Desc:   从队列中弹出一项
 */
template <class Type>     
Type SyncQueue<Type>::pop()
{  
    Type type;  
   
    lock();
    while(pQueue->empty())
    {  
        try  
        {  
            wait();
        }
        catch(exception& ex)
        {
            cout<< ex.what()<<endl;  
            throw;  
        }                  
    }  

    type = pQueue->front();  
    pQueue->pop();
    unlock();

    return type; 
}

/*
 * @func:   push()
 * @Desc:   压入队列
 */
template <class Type>    
void SyncQueue<Type>::push(const Type &type)
{  
   
    lock();  
    pQueue->push(type);  
    try  
    {  
        notify();  
    }  
    catch(exception& ex)  
    {  
        cout<<ex.what()<<endl;  
        throw;  
    }                  
    unlock();     
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
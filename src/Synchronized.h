/**
 * @file    Synchronized.h
 * @brief   线程同步使用
 *
 * @author  Tupelo Shen
 * @version R1.01.01
 * @date    2018/01/10
 */
#include <ptread.h>

/*
 * @class   Synchronized
 * @brief   线程同步使用
 *
 */
class Synchronized  
{    
public:  
    Synchronized();    
    ~Synchronized();  
     
    // Causes current thread to wait until another thread   
    // invokes the notify() method or the notifyAll()   
    // method for this object.  
    void                wait();  

    // Causes current thread to wait until either another   
    // thread invokes the notify() method or the notifyAll()   
    // method for this object, or a specified amount of time   
    // has elapsed.  
    // @param timeout   
    //    timeout in milliseconds.  
    // @param  
    //    return TRUE if timeout occured, FALSE otherwise.  
    bool                wait(unsigned long timeout);

    // Wakes up a single thread that is waiting on this   
    // object's monitor.  
    void                notify();  

    // Wakes up all threads that are waiting on this object's   
    // monitor.  
    void                notify_all();  

    // Enter a critical section.  
    void                lock();  

    // Try to enter a critical section.  
    // @return TRUE if the attempt was successful, FALSE otherwise.  
    bool                trylock();  

    // Leave a critical section.  
    void                unlock();  


private:  
#ifdef WIN32  
    char                numNotifies;  
    HANDLE              semEvent;  
    HANDLE              semMutex;  
    bool                isLocked;  
#else  
    int                 cond_timed_wait(const timespec*);  
    pthread_cond_t      cond;  
    pthread_mutex_t     monitor;  
#endif  

};  
   
  
Synchronized::Synchronized()  
{ 
    int result;  

    memset(&monitor, 0, sizeof(monitor));  
    result = pthread_mutex_init(&monitor, 0);  

    if(result)  
    {  
        throw runtime_error("Synchronized mutex_init failed!");  
    }  

    memset(&cond, 0, sizeof(cond));  
    result = pthread_cond_init(&cond, 0);  
    if(result)  
    {  
        throw runtime_error("Synchronized cond_init failed!");  
    }  
     
}  
   
  
Synchronized::~Synchronized()  
   
{  
   
#ifdef WIN32  
   
        CloseHandle(semEvent);  
   
        CloseHandle(semMutex);  
   
#else  
   
        int result;  
   
        result = pthread_cond_destroy(&cond);  
   
//        if(result)  
   
//        {  
   
//                throw runtime_error("Synchronized cond_destroy failed!");  
   
//        }  
   
        result = pthread_mutex_destroy(&monitor);  
   
//        if(result)  
   
//        {  
   
//                throw runtime_error("Synchronized mutex_destroy failed!");  
   
//        }  
   
#endif  
   
}  
   
  
void Synchronized::wait()  
   
{
    cond_timed_wait(0);   
}  
   
     
int Synchronized::cond_timed_wait(const struct timespec *ts)   
{  
    int result;  

    if(ts)
        result = pthread_cond_timedwait(&cond, &monitor, ts);  
    else   
        result = pthread_cond_wait(&cond, &monitor);  

    return result;    
}  
   
#endif  
   
  
bool Synchronized::wait(unsigned long timeout)     
{    
    bool timeoutOccurred = false;  
    struct timespec ts;  
    struct timeval  tv;  

    gettimeofday(&tv, 0);  
    ts.tv_sec  = tv.tv_sec  + (int)timeout/1000;  
    ts.tv_nsec = (tv.tv_usec + (timeout %1000)*1000) * 1000;   

    int err;  

    if((err = cond_timed_wait(&ts)) > 0)  
    {  
        switch(err)  
        {  
            case ETIMEDOUT:
                timeoutOccurred = true;  
                break;  

            default:
                throw runtime_error("Synchronized: wait with timeout returned!");  
                break;  
        }  
    }  

    return timeoutOccurred;   
}  
   
  
void Synchronized::notify()  
   
{  
    int result;  

    result = pthread_cond_signal(&cond);  

    if(result)  
    {  
        throw runtime_error("Synchronized: notify failed!");  
    }  
}  
   
  
void Synchronized::notify_all()     
{
    int result;  

    result = pthread_cond_broadcast(&cond);  

    if(result)  

    {  

            throw runtime_error("Synchronized: notify_all failed!");  

    }  
}  
   
  
void Synchronized::lock()     
{
    pthread_mutex_lock(&monitor);  
}  
   
  
void Synchronized::unlock()  
{
    pthread_mutex_unlock(&monitor);   
} 
   
  
bool Synchronized::trylock()  
   
{  
    if(pthread_mutex_trylock(&monitor) == 0)  
        return true;  
    else  
        return false;    
}  


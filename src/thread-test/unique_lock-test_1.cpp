#include <iostream>
#include <thread>
#include <mutex>
//#include "./../Common.h"

std::mutex foo,bar;

void task_a()
{
    std::lock(foo, bar);    // foo和bar已被当前线程锁住
    /************************************************************************
    * adopting 初始化
    * adopt_lock是一个常量对象，通常作为参数传入给unique_lock或lock_guard的构造函数。
    * 新创建的unique_lock对象管理Mutex对象m,m应该是一个已经被当前线程锁定的Mutex对象。
    *************************************************************************/
    std::unique_lock<std::mutex> lck1(foo, std::adopt_lock);
    std::unique_lock<std::mutex> lck2(bar, std::adopt_lock);

    std::cout << "task a\n" << std::endl;
}

void task_b() {  
    //新创建的 unique_lock 对象不管理任何 Mutex 对象。  
    std::unique_lock<std::mutex> lck1, lck2;  
    /****************************************************** 
    * deferred 初始化: 
    *新创建的 unique_lock 对象管理 Mutex 对象 m，但是在初始化 
    *的时候并不锁住 Mutex 对象。 m 应该是一个没有当前线程锁住的  
    *Mutex 对象。 
    ******************************************************/  
    lck1 = std::unique_lock<std::mutex>(bar, std::defer_lock);  
    lck2 = std::unique_lock<std::mutex>(foo, std::defer_lock);  
    std::lock(lck1, lck2);  
    std::cout << "task b\n";  
}

int main() {  
    std::thread th1(task_a);  
    std::thread th2(task_b);  
    th1.join();  
    th2.join();  
    
    std::cin.clear();       // reset any error flags
    // ignore any characters in the input buffer until we find an enter character
    std::cin.ignore(32767, '\n');       
    std::cin.get();         // ger one more char from the user

    return EXIT_SUCCESS;  
}
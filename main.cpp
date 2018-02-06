// #include "TransactionalMemory.h"


// int main()
// {
//     testTransactionalMemory();
//     return 0;
// }
// #include <iostream>
// #include <vector>
// #include <thread>


// int f()
// {
//     static int i = 0;
//    //synchronized { // begin synchronized block
//         std::cout << i << " -> ";
//         ++i;       // each call to f() obtains a unique value of i
//         std::cout << i << '\n';
//         return i; // end synchronized block
//     //}
// }

// //int testTransactionalMemory()
// int main()
// {
//     std::vector<std::thread> v(10);
//     for(auto& t: v)
//         t = std::thread([]{ for(int n = 0; n < 10; ++n) f(); });
//     for(auto& t: v)
//         t.join();

//     return 0;
// }
#if 0
#include <mutex>
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <string>
 
struct Employee 
{ 
    Employee(std::string id) : id(id) {}
    std::string id;
    std::vector<std::string> lunch_partners;
    std::mutex m;
    std::string output() const
    {
        std::string ret = "Employee " + id + " has lunch partners: ";
        for( const auto& partner : lunch_partners )
            ret += partner + " ";
        return ret;
    }
};
 
void send_mail(Employee &, Employee &)
{
    // simulate a time-consuming messaging operation
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
 
void assign_lunch_partner(Employee &e1, Employee &e2)
{
    static std::mutex io_mutex;
    {
        std::lock_guard<std::mutex> lk(io_mutex);
        std::cout << e1.id << " and " << e2.id << " are waiting for locks" << std::endl;
    }
 
    // use std::lock to acquire two locks without worrying about 
    // other calls to assign_lunch_partner deadlocking us
    {
        std::lock(e1.m, e2.m);
        std::lock_guard<std::mutex> lk1(e1.m, std::adopt_lock);
        std::lock_guard<std::mutex> lk2(e2.m, std::adopt_lock);
// Equivalent code (if unique_locks are needed, e.g. for condition variables)
//        std::unique_lock<std::mutex> lk1(e1.m, std::defer_lock);
//        std::unique_lock<std::mutex> lk2(e2.m, std::defer_lock);
//        std::lock(lk1, lk2);
        {
            std::lock_guard<std::mutex> lk(io_mutex);
            std::cout << e1.id << " and " << e2.id << " got locks" << std::endl;
        }
        e1.lunch_partners.push_back(e2.id);
        e2.lunch_partners.push_back(e1.id);
    }
    send_mail(e1, e2);
    send_mail(e2, e1);
}
 
int main()
{
    Employee alice("alice"), bob("bob"), christina("christina"), dave("dave");
 
    // assign in parallel threads because mailing users about lunch assignments
    // takes a long time
    std::vector<std::thread> threads;
    threads.emplace_back(assign_lunch_partner, std::ref(alice), std::ref(bob));
    threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(bob));
    threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(alice));
    threads.emplace_back(assign_lunch_partner, std::ref(dave), std::ref(bob));
 
    for (auto &thread : threads) thread.join();
    std::cout << alice.output() << '\n'  << bob.output() << '\n'
              << christina.output() << '\n' << dave.output() << '\n';
}
#endif 
#if 0
#include <thread>
#include <mutex>
#include <iostream>
 
int g_i = 0;
std::mutex g_i_mutex;  // protects g_i
 
void safe_increment()
{
    std::lock_guard<std::mutex> lock(g_i_mutex);
    ++g_i;
    std::cout << std::this_thread::get_id() << ": " << g_i << '\n';
 
    // g_i_mutex is automatically released when lock
    // goes out of scope
}
 
int main()
{
    std::cout << "main: " << g_i << '\n';
 
    std::thread t1(safe_increment);
    std::thread t2(safe_increment);
 
    t1.join();
    t2.join();
 
    std::cout << "main: " << g_i << '\n';
}
#endif
#include <iostream>
#include "Common.h"
//#include "Queue-pthread_s.h"
int main(void)
{
    // testQueue_s();
    // SystemPause();
    int x = 4;
    auto y = [&r = x, x = x + 1]()->int
    {
        r += 2;
        return x * x;
    }();        // updates ::x to 6 and initializes y to 25

    std::cout << "x = " << x << "; y = " << y <<  ";"<< std::endl;
}
#if 0
//#include <Windows.h>
#include <iostream>

using namespace std;

int index = 0;

// 临界区结构对象
CRITICAL_SECTION g_cs;

HANDLE hMutex = NULL;

void changeMe()
{
    cout << index++ << endl;
}
void changeMe2()
{
 cout << index++ << endl;
}
void changeMe3()
{
 cout << index++ << endl;
}
DWORD WINAPI th1(LPVOID lpParameter)
{
 while(1)
 {
  Sleep(1600); //sleep 1.6 s
  // 进入临界区
  EnterCriticalSection(&g_cs);
  // 等待互斥对象通知
  //WaitForSingleObject(hMutex, INFINITE);
  // 对共享资源进行写入操作
  //cout << "a" << index++ << endl;
  changeMe();
  changeMe2();
  changeMe3();
  // 释放互斥对象
  //ReleaseMutex(hMutex);
  // 离开临界区
  LeaveCriticalSection(&g_cs);  
 }
 return 0;
}
DWORD WINAPI th2(LPVOID lpParameter)
{
 while(1)
 {
  
  Sleep(2000); //sleep 2 s
  // 进入临界区
  EnterCriticalSection(&g_cs);

  // 等待互斥对象通知
  //WaitForSingleObject(hMutex, INFINITE);
  //cout << "b" << index++ << endl;
  changeMe();
  changeMe2();
  changeMe3();
  // 释放互斥对象
  //ReleaseMutex(hMutex);

  // 离开临界区
  LeaveCriticalSection(&g_cs); 
 }
 return 0;
}
int main(int argc, char* argv[])
{
 // 创建互斥对象
 //hMutex = CreateMutex(NULL, TRUE, NULL);
 // 初始化临界区
 InitializeCriticalSection(&g_cs);
 HANDLE hThread1;
 HANDLE hThread2;
 hThread1 = CreateThread(NULL, 0, th1,  NULL, 0, NULL);
 hThread2 = CreateThread(NULL, 0, th2,  NULL, 0, NULL);
 int k; 
 cin >> k; 
 printf("Hello World!\n");
 return 0;
}
#endif
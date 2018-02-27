//#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include "Common.h"
#include "Queue-pthread_s.h"

using namespace std::chrono_literals;

int g_index = 10;
Queue_s<int> q;

void threadFunction_1()
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << "thread [" << this_id << "] start...\n";
    while(true)
    {
        //std::this_thread::sleep_for(1.5s);
        int value = 0;
        value = q.wait_and_pop();
        printf("wait_and_pop done! value=%d  thread id:%d\n", value, this_id);
    }
}
void threadFunction_2()
{
    std::cout << "threadFunction_2 start\n";
    while(true)
    {     
        std::this_thread::sleep_for(1s);  
        g_index++;
        q.push(g_index);
        printf("push %d\n", g_index); 
    }
}
int main()
{
    std::thread thd1(threadFunction_2);
    std::thread thd2(threadFunction_1);

    thd1.join();
    thd2.join();

    SystemPause();
}

//#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include "Common.h"
#include "Queue-pthread_s.h"

int g_index = 10;
Queue_s<int> q;
std::initializer_list<float> list = {1, 2, 3, 4, 5};
Queue_s<float> q1(list);

void threadFunction_1()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread::id this_id_1 = std::this_thread::get_id();
    // std::cout << "thread[" << this_id_1 << "] start..." << std::endl;
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        int value = 0;
        // value = q.wait_and_pop();
        if (q.try_pop((int&)value))
        {
            std::cout << "thread[" << this_id_1 <<
                "] poped: " << value << std::endl;
        }
    }
}
void threadFunction_2()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread::id this_id_2 = std::this_thread::get_id();
    std::cout << "thread[" << this_id_2 << "] start..." << std::endl;
    while(true)
    {     
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));  
        g_index++;
        q.push(g_index);
        std::cout << "thread[" << this_id_2 << "] pushed:" << g_index << std::endl; 
    }
}
int main()
{
    std::thread thd1(threadFunction_2);
    std::thread thd2(threadFunction_1);
    std::thread thd3(threadFunction_1);

    thd1.join();
    thd2.join();
    thd3.join();

    SystemPause();
}

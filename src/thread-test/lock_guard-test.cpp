#include <thread>
#include <mutex>
#include <iostream>
 
int g_i = 0;
std::mutex g_i_mutex;  // protects g_i
 
void safe_increment()
{
    std::lock_guard<std::mutex> lock(g_i_mutex);
    ++g_i;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << std::this_thread::get_id() << ": " << g_i << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // g_i_mutex is automatically released when lock
    // goes out of scope
}
 
int main()
{
    std::cout << "main: " << g_i << '\n';
 
    std::thread t1(safe_increment);
    std::thread t2(safe_increment);
    std::thread t3(safe_increment);
    std::thread t4(safe_increment);
    std::thread t5(safe_increment);
 
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    std::cout << "main: " << g_i << '\n';
}

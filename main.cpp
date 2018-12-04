//#include <vector>
#include <iostream>
// #include <chrono>
// #include <thread>
#include "Common.h"
// #include "Queue-pthread_s.h"
#include "Stack.h"

int main(void)
{
    try
    {
        Stack<int,10> int10Stack;
        Stack<int,20> int20Stack;
        for(int i = 0; i < 21; i++)
        {
            int20Stack.push(i);
            std::cout << "Pushed: " << i << std::endl;
        }
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 0;
    }
}
class Grid  
{  
public:  
    // ... 
    Grid(const Grid<T,EMPTY>& src);  
    Grid<T,EMPTY>& operator=( const Grid<T,EMPTY>& rhs);  
    // ...  
};  

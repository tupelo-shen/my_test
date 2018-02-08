#include <iostream>
#include "Common.h"
//#include "Queue-pthread_s.h"
int main(void)
{
#if 0
    int x = 4;
    auto y = [&r = x, x = x + 1]()->int
    {
        r += 2;
        return x * x;
    }();        // updates ::x to 6 and initializes y to 25

    std::cout << "x = " << x << "; y = " << y <<  ";"<< std::endl;

    // generic lambda, operator() is a template with two parameters
    auto glambda = [](auto a, auto&& b) 
    { 
        std::cout << "a = " << a << "; b = " << b <<  ";"<< std::endl;
        return a < b; 
    };
    bool b = glambda(4, 3.14); // ok
    if(b == false)
    {
        std::cout << "a >= b" << std::endl;
    }
    else
    {
        std::cout << "a < b" << std::endl;
    }

    // generic lambda, operator() is a template with one parameter
    auto vglambda = [](auto printer) 
    {
        return [=](auto&&... ts) // generic lambda, ts is a parameter pack
        { 
            printer(std::forward<decltype(ts)>(ts)...);
            return [=] { printer(ts...); }; // nullary lambda (takes no parameters)
        };
    };
    auto p = vglambda([](auto v1, auto v2, auto v3) 
        { std::cout << v1 << v2 << v3; });
    auto q = p(1, 'a', 3.14); // outputs 1a3.14
    q();                      // outputs 1a3.14
#endif

    // 在全局命名空间中没有操作符<<，但是ADL检查std命名空间，因为左边的参数在std中，且在
    // 其中能找到std::operator<<(std::ostream&, const char*)
    std::cout << "Test\n";

    // 相同，使用函数调用符号样式
    operator<<(std::cout, "Test\n"); 
 
    // 但是,这句就会报出"Error: 'endl' is not declared in this namespace"这样的错误
    // 因为这对于endl()来说，并不是一个函数调用，所以ADL并不适用
    std::cout << endl;
 
    // OK：这是一个函数调用，ADL检查std命名空间，因为endl的参数在std命名空间里,能够发现std::endl
    endl(std::cout);
 
    // 但是,这句就会报出"Error: 'endl' is not declared in this namespace"这样的错误
    // 因为子表达式(endl)不是一个函数调用表达式
    (endl)(std::cout);
}
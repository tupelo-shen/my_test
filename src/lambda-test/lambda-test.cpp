#include <iostream>
#if 0
auto make_function(int& x)
{
    return [&]{ std::cout << x << '\n';}; 
}

int main()
{
    int i = 3;
    auto f = make_function(i);      // the use of x in f binds directly to i
    i = 5;
    f();                            // OK; prints 5
}
#endif
#if 0 
int main()
{
    int a = 1, b = 1, c = 1;

    auto m1 = [a, &b, &c]() mutable
    {
        auto m2 = [a, b, &c]() mutable
        {
            std::cout << a << b << c << '\n';
            a = 4; b = 4; c = 4;
        };
        a = 3; b= 3; c = 3;
        m2();
    };

    a = 2; b = 2; c = 2;
    m1();                   // calls m2() and prints 123
    std::cout << a << b << c << '\n'; // prints 234
}
#endif
int main()
{
    int x = 4;
    auto y = [&r = x, x = x + 1]()->int
    {
        r += 2;
        return x * x;
    }();        // updates ::x to 6 and initializes y to 25

    std::cout << "x = " << x << "; y = " << y <<  "; r = " << r << std::endl;

}
#if 0
#include <iostream>
 
struct A { double x; };
const A* a;
 
decltype(a->x) y;                           // y的类型是double
decltype((a->x)) z = y;                     // z的类型是const double& (左值表达式)
 
template<typename T, typename U>
auto add(T t, U u) -> decltype(t + u)       // 返回类型依赖模板参数//返回类型可以被推导（C++14之后）
{
    return t+u;
}
 
int main() 
{
    int i = 33;
    decltype(i) j = i * 2;
 
    std::cout << "i = " << i << ", " << "j = " << j << '\n';
 
    auto f = [](int a, int b) -> int
    {
        return a * b;
    };
 
    decltype(f) g = f;                      // lambda函数的类型是唯一且未命名的
    i = f(2, 2);
    j = g(3, 3);
 
    std::cout << "i = " << i << ", " << "j = " << j << '\n';
}
#endif

#if 1
#include <iostream>
#include <string>
#include <typeinfo>
 
struct Base {};                             // 非多态
struct Derived : Base {};
 
struct Base2 { virtual void foo() {} };     // 多态
struct Derived2 : Base2 {};
 
int main() {
    int                 myint       = 50;
    std::string         mystr       = "string";
    double*             mydoubleptr = nullptr;
 
    std::cout << "myint has type: " << typeid(myint).name() << '\n'
              << "mystr has type: " << typeid(mystr).name() << '\n'
              << "mydoubleptr has type: " << typeid(mydoubleptr).name() << '\n';
 
    // std::cout << myint是多态类型的泛左值表达式；它会被求值
    const std::type_info& r1 = typeid(std::cout << myint);
    std::cout << '\n' << "std::cout<<myint has type : " << r1.name() << '\n';
 
    // std::printf()不是多态类型的泛左值表达式；不会被求值
    const std::type_info& r2 = typeid(std::printf("%d\n", myint));
    std::cout << "printf(\"%d\\n\",myint) has type : " << r2.name() << '\n';
 
    // 非多态左值是一个static类型
    Derived d1;
    Base& b1 = d1;
    std::cout << "reference to non-polymorphic base: " << typeid(b1).name() << '\n';
 
    Derived2 d2;
    Base2& b2 = d2;
    std::cout << "reference to polymorphic base: " << typeid(b2).name() << '\n';
 
    try {
        // 解引用一个null指针： 对于非多态表达式是OK的
        std::cout << "mydoubleptr points to " << typeid(*mydoubleptr).name() << '\n'; 
        // 解引用一个null指针： 对于多态表达式不是OK的
        Derived2* bad_ptr = nullptr;
        std::cout << "bad_ptr points to... ";
        std::cout << typeid(*bad_ptr).name() << '\n';
    } catch (const std::bad_typeid& e) {
         std::cout << " caught " << e.what() << '\n';
    }
}
#endif
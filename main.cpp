// 移动构造函数和赋值
#include <iostream>
#include <string>
using namespace std;

class Example6 {
    string* ptr;
public:
    Example6 (const string& str) : ptr(new string(str)) { }
    Example6 (const Example6& str) : ptr(new string(str.content())) { cout << "cope contructor function" << endl;}
    ~Example6 () {delete ptr;}
    // 移动构造函数，参数x不能是const Pointer&& x，
    // 因为要改变x的成员数据的值；
    // C++98不支持，C++0x（C++11）支持
    Example6 (Example6&& x) : ptr(x.ptr) 
    {
        cout << "move contructor function" << endl;
        x.ptr = nullptr;
    }
    // move assignment
    Example6& operator= (Example6&& x) 
    {
        cout << "move assignment function" << endl;
        delete ptr; 
        ptr = x.ptr;
        x.ptr=nullptr;
        return *this;
    }
    // access content:
    const string& content() const {return *ptr;}
    // addition:
    Example6 operator+(const Example6& rhs) 
    {
        return Example6(content()+rhs.content());
    }
};

// struct A
// {
//     std::string s;
//     int         k;

//     A() : s("test"), k(-1) {}
//     A(const A& o) : s(o.s), k(o.k) { cout<< "move failed!\n"; }
//     A(A&& o) noexcept :
//            s(std::move(o.s)),       // explicit move of a member of class type
//            k(std::exchange(o.k, 0)) // explicit move of a member of non-class type
//     { }
// };

// A f(A a)
// {
//     return a;
// }

// struct B : A
// {
//     std::string s2;
//     int n;
//     // implicit move constructor B::(B&&)
//     // calls A's move constructor
//     // calls s2's move constructor
//     // and makes a bitwise copy of n
// };

// struct C : B
// {
//     ~C() { } // destructor prevents implicit move constructor C::(C&&)
// };

// struct D : B
// {
//     D() { }
//     ~D() { }          // destructor would prevent implicit move constructor D::(D&&)
//     D(D&&) = default; // forces a move constructor anyway
// };

int main () {
    Example6 foo("Exam");           // 构造函数
    Example6 bar(move(foo));        // 移动构造函数
                                    // 调用move之后，foo变为一个右值引用变量，
                                    // 此时，foo所指向的字符串已经被"掏空"，所以此时不能再调用foo
    bar = bar + bar;                // 移动赋值，在这儿"="号右边的加法操作，产生一个临时值，即一个右值
                                    // 所以此时调用移动赋值语句

    cout << "bar's content: " << bar.content() << '\n';
    return 0;
}

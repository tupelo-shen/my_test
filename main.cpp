#include <iostream>
#if 0
class Base
{
public:
    Base() { Subtle(); }       // 打印 1

    virtual void Foo() = 0;
    void Subtle() { Foo(); }
};

class Derive : public Base
{
public:
    Derive() : Base(), m_pData(new int(2)) {}
    ~Derive() { delete m_pData; }

    virtual void Foo()
    {
        std::cout << *m_pData << std::endl;
    }

private:
    int* m_pData;
};

int main()
{
    Base* p = new Derive();
    p->Foo();
    delete p;
    return 0;
}
#endif

#if 1
class Base
{
public:
    Base() { PrintBase(); }

    void PrintBase()
    {
        std::cout << "Address of Base:" << this << std::endl;

        // 虚函数表的地址存在对象内存空间里的头四个字节
        int* vt = (int *)*((int *)this);
        std::cout << "Address of Base Vtable: " << vt << std::endl;

        // 通过vt来调用Foo函数，以证明vt指向的确实是虚函数表
        std::cout << "Call Foo by vt->";
        void (*pFoo)(Base* const) = (void (*)(Base* const))vt[0];
        (*pFoo)(this);

        std::cout << std::endl;
    }

    virtual void Foo() { std::cout << "Base" << std::endl; }
};

class Derive : public Base
{
public:
    Derive() : Base() { PrintDerive(); }

    void PrintDerive()
    {
        std::cout << "Address of Derive:" << this << std::endl;

        // 虚函数表的地址存在对象内存空间里的头四个字节
        int* vt = (int *)*((int *)this);
        std::cout << "Address of Derived Vtable: " << vt << std::endl;

        // 通过vt来调用Foo函数，以证明vt指向的确实是虚函数表
        std::cout << "Call Foo by vt->";
        void (*pFoo)(Base* const) = (void (*)(Base* const))vt[0];
        (*pFoo)(this);

        std::cout << std::endl;        
    }

    virtual void Foo() { std::cout << "Derive" << std::endl; }
};

int main()
{
    Base* p = new Derive();
    delete p;
    return 0;
}
#endif

#if 0
using namespace std;

class Polygon 
{
protected:
    int width, height;

public:
    void set_values(int a, int b)
    {
        width = a;
        height = b;
    }
};

class Rectangle : public Polygon
{
public:
    int area()
    {
        return width * height;
    }
};

class Triangle : public Polygon
{
public:
    int area()
    {
        return width * height / 2;
    }
};

int main()
{
    Rectangle rect;
    Triangle trgl;

    Polygon *ppoly1 = &rect;
    Polygon *ppoly2 = &trgl;
    ppoly1->set_values(4, 5);
    ppoly2->set_values(4, 5);
    cout << rect.area() << endl;
    cout << trgl.area() << endl;

    return 0;
}
#endif
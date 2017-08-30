/*
 * MODULE NAME:     main.cpp
 * DESCRIPTION:     Test for delete the pointer for type void * 
 *                  1.  C、C++ 中， void * 指针可以转化为任意类型的指针类型，在删除void *指针时，编译器往往会
 *                      发出如下警告：
 *                          warning: deleting 'void*' is undefined [enabled by default]
 *                          警告： 删除"void *"指针可能引发未知情况（默认打开警告）
 *                      
 *                  2.  使用delete pointer； 释放void 指针void *，系统会以释放普通指针(char, short, int,
 *                      long, long long)的方式来释放void *指向的内存空间；
 *                  3.  如果void *指向一个数组指针，那么由于释放指针时用了delete pointer从而导致内存泄漏，释放
 *                      指针正确做法是delete[] pointer；
 *                  4.  如果void *指向一个class类，那么系统由于认为void *指向一个普通的内存空间，所以释放指针时
 *                      系统class的析构函数不会调用；
 *                  5.  释放void *的解决方案：将void *转换为原来类型的指针，然后再调用delete释放指针，如果原来的
 *                      指针是数组指针，那么必须使用delete[]删除指向的内存空间。
 *                  6.  使用safe_delete释放明确的类会自动触发析构函数（如果析构函数为虚函数，那么先调用子类的析构
 *                      函数再调用子类的直接基类的析构函数）；
 *                  7.  使用safe_delete释放void*指针指向的类时，不会触发对应类的析构函数；
 *                  8.  如果使用safe_delete_void_ptr内联函数释放void*指针，那么由于在释放指针前，函数会将void*
 *                      指针转换为特定类型的函数指针，所以最终能够触发调用析构函数，并且不影响虚类的释放行为。
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include "Common.h"

using namespace std;

template <typename T>
inline void safe_delete(T *&target)
{
    if(nullptr != target)
    {
        delete target;
        target = nullptr;
    }
}

template <typename T>
inline void safe_delete_void_ptr(void *&target)
{
    if(nullptr != target)
    {
        T* temp = static_cast<T*>(target);
        delete temp;
        temp = nullptr;
        target = nullptr;
    }
}

class  A
{
public:
     A(string name)
     {
        this->name = name;
     };
    virtual ~ A()
    {
        cout << "base class A's destructor" << ",name:" << this->name << endl;
    };
public:
    string name;    
};

class AChild : public A
{
public:
    AChild(string name, string school)
    : A(name),
    school(school)
    {
    };
    ~AChild()
    {
        cout << "child class AChild's destructor" << ", name: " << this->name
            << ", school: " << this->school << endl;
    };

public:
    string school;
};
/*
 * FUNCTION NAME:   main
 * DESCRIPTION:     main loop
 */
int main()
{
    // 测试safe_delete释放普通类指针
    cout << "safe_delete pointer of type AChild" << endl;
    AChild *a_child = new AChild("jacky", "Shenzhen University");
    safe_delete(a_child);
    cout << endl;

    // 测试safe_delete释放 void* 指针 
    cout << "safe_delete  pointer of type void *" << endl;
    void *vp = new AChild("Polyn", "Southern University of Science and Technology");
    safe_delete(vp);
    cout << endl;

    // 测试safe_delete_void_ptr释放模板实例化为基类的void*指针
    std::cout<<"safe_delete_void_ptr pointer of type void * ==> A *" << endl;  
    void *vpA = new AChild("Heral", "Renmin University of China");  
    safe_delete_void_ptr<A>(vpA);  
    cout << endl;

    // 测试safe_delete_void_ptr释放模板实例化为子类的void*指针  
    std::cout<<"safe_delete_void_ptr pointer of type void * ==> AChild *" << endl;  
    void *vpAChild = new AChild("pevly", "Southeast University");  
    safe_delete_void_ptr<AChild>(vpAChild);

    SystemPause();
}

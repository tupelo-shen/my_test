[TOC]

## 1 为什么引入智能指针？

大家都知道，指针是 C++ 中非常重要的一部分，大家在初期学习 C++ 的时候一定学过类似这样的指针方式。

```c++
int  *ptr;
```

这种指针也被称为裸指针。使用裸指针会存在一些不足：

1. 如果使用裸指针分配内存后，忘记手动释放资源，会出现内存泄漏。

2. 如果使用多个裸指针指向同一资源，其中一个指针对资源进行释放，其它指针成为空悬指针，如果再次释放会存在不可预测的错误。
    
    <img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_1.PNG">
    
    上图中当 sp1 把资源释放后，sp2 成了空悬指针。空悬指针指的是指针所指向的对象已经释放的时候自身却没有被置为 nullptr。sp1 通过 `free/delete` 释放资源的内存时，内存不会立刻被系统回收，而是状态改变为可被其它地方申请的状态。这时当再次操作 sp2，这块内存可能被其它地方申请了，而具体被谁申请了是不确定的，因此可能导致的错误也是不可预测的。

3. 如果程序异常退出时，裸指针的释放资源的代码未能执行，也会造成内存泄漏。

为了改善裸指针的不足，确保资源的分配和释放是配对的，开发者提出了智能指针。智能指针主要是对裸指针进行了一次面向对象的封装，在构造函数中初始化资源地址，在析构函数中释放资源。当资源应该被释放时，指向它的智能指针可以确保自动地释放它。

C++ 库中，为智能指针提供了不带引用计数和带引用计数的两种方案。

引用计数用于表示有多少智能指针引用同一资源。不带引用计数的智能指针采用独占资源的方式，而带引用计数的智能指针则可以同时多个指向同一资源。下面介绍一下它们的主要特点和区别。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_2.PNG">

## 2 不带引用计数的智能指针

不带引用计数的智能指针包括 `auto_ptr`、`scoped_ptr` 和 `unique_ptr` 三种指针。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_3.PNG">

#### 2.1 auto_ptr

我们先来看个例子：

```c++
#include <memory>
int main()
{
    auto_ptr<int>  ptr(new int(6)); // 定义auto_ptr指针ptr
    auto_ptr<int>  ptr1(ptr);       // 拷贝构造ptr定义ptr1
    *ptr=8;                         // 对空指针ptr赋值会产生不可预料的错误
    return 0;
}
```

开始时 ptr 指向资源，一个整型数字 6，当用 ptr1 拷贝构造 ptr 时，ptr1 指向资源，而 ptr 则指向 nullptr。下一行程序中如果对空指针 ptr 赋值 8，将会产生不可预料的错误。

下图表示 auto_ptr 指针对资源的指向过程。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_4.PNG">

使用拷贝构造时，如果只有最后一个 auto_ptr 持有资源，其余 auto_ptr 持有的资源会被置为 nullptr。

因此需要注意，不能在容器中使用 auto_ptr，当容器发生拷贝时，原容器中 auto_ptr 持有的资源会置 nullptr。

下面我们再来看一下 auto_ptr 的部分源码和部分解析：

```c++
template<class _Ty>
class auto_ptr
{ 
public:
    typedef _Ty element_type;

    explicit auto_ptr(_Ty * _Ptr=nullptr) noexcept
        : _Myptr(_Ptr) // 初始化列表
    { 
        // 构造函数
    }

    auto_ptr(auto_ptr& _Right) noexcept
        : _Myptr(_Right.release())
    {
        // 拷贝构造函数，会调用release()函数
    }
  
    _Ty * release() noexcept
    {
        /* 使用拷贝构造时，最后一个auto_ptr持有资源，
         * 其余被置为nullptr
         */ 
        _Ty * _Tmp = _Myptr;
        _Myptr = nullptr;
        return (_Tmp);
    }
private:
    _Ty * _Myptr;   // 指向资源 
};
```
当试图调用 `auto_ptr` 的拷贝构造函数时，在初始化列表中调用了 `release()` 函数，`release()` 函数用一个 `_Tmp` 指针保存资源并返回用于初始化当前的 `auto_ptr` 的类成员 `_Myptr`，而 `_Right` 对应的 `_Myptr` 被置为 `nullptr`。

#### 2.2 scoped_ptr

`scoped_ptr` 和 `auto_ptr` 有些不同，它私有化了拷贝构造函数和赋值函数，资源的所有权无法进行转移，也无法在容器中使用。

下面使用一段代码表现 `scoped_ptr` 的特性，如果不规范使用会发生错误。

正确用法：

```c++
scoped_ptr<int> sp1(new int(6));    // 初始化sp1指针
```

错误用法：

```c++
scoped_ptr<int> sp2(sp1);   // 错误，无法拷贝构造
```

这种方法是错误的，因为 `scoped_ptr` 私有化了拷贝构造函数，无法显式调用。

```c++
scoped_ptr<int> sp3(new int(5)) // 初始化sp2指针
sp1=sp3;    // 错误，无法赋值
```

这种方法是错误的，因为 `scoped_ptr` 私有化了赋值构造函数，无法显式调用。

有时候面试官会问到，`scoped_ptr` 是如何保证资源的所有权的？这时候就可以按照上面的讲解来回答：

`scoped_ptr` 私有化了拷贝构造函数和赋值函数，资源的所有权无法进行转移，所以保证了资源的所有权。

然后再来看一下 `scoped_ptr` 的部分源码和部分解析：

```c++

template<class T> 
class scoped_ptr
{
private:
    T * px; 
    scoped_ptr(scoped_ptr const &); // 拷贝构造函数
    scoped_ptr & operator=(scoped_ptr const &); // 赋值构造函数
 
public:
    typedef T element_type;
    explicit scoped_ptr( T * p = nullptr ): px( p )
    {
    }
    ~scoped_ptr()   // 析构函数
};
```
`scoped_ptr` 通过私有化拷贝构造函数和赋值构造函数来拒绝浅拷贝的发生。

值得注意的是，`auto_ptr` 是通过将除最后一个以外的其它 `auto_ptr` 置 `nullptr` 来避免浅拷贝的发生，它的资源所有权是可以转移的。

而 `scoped_ptr` 是直接禁止了拷贝与赋值，资源所有权无法转移。

#### 2.3 unique_ptr

`unique_ptr` 删除了拷贝构造函数和赋值函数，因此不支持普通的拷贝或赋值操作。如下所示：

```c++

unique_ptr<int> p1(new int(6)); // 正确写法
unique_ptr<int> p2(p1);         // 这么写是错误的：
// unique_ptr不支持拷贝
unique_ptr<int> p3;
p3=p2;  // 这么写是错误的：unique_ptr不支持赋值
```

再来看一下 `unique_ptr` 的部分源码和部分解析：

```c++
template<class _Ty,class _Dx>
class unique_ptr: public _Unique_ptr_base<_Ty, _Dx>
{ 
public:
    typedef _Unique_ptr_base<_Ty, _Dx> _Mybase;
    typedef typename _Mybase::pointer pointer;
    typedef _Ty element_type;
    typedef _Dx deleter_type;

    unique_ptr(unique_ptr&& _Right) noexcept
        : _Mybase(_Right.release(),
    _STD forward<_Dx>(_Right.get_deleter()))
    { 
        // 右值引用的拷贝构造函数
    }

    unique_ptr& operator=(unique_ptr&& _Right) noexcept
    {
        // 提供了右值引用的operator=赋值构造函数
        if (this != _STD addressof(_Right))
        { 
            reset(_Right.release());
            this->get_deleter() = _STD forward<_Dx>
            (_Right.get_deleter());
        }
        return (*this);
    }
    /*
     * 删除了unique_ptr的拷贝构造和赋值函数，拒绝浅拷贝
     */
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
};
```
`unique_ptr` 和 `scoped_ptr` 一样禁止了拷贝构造和赋值构造，引入了带右值引用的拷贝构造和赋值。可以把 `unique_ptr` 作为函数的返回值。

不带引用计数的智能指针总结：

* 相同点：最终只有一个智能指针持有资源。
* 不同点：
    - auto_ptr 进行拷贝构造时，会对之前的 auto_ptr 的资源置 nullptr 操作；
    - scoped_ptr 通过私有化了拷贝构造和赋值函数杜绝浅拷贝；
    - unique_ptr 通过删除了拷贝构造和赋值函数函数杜绝浅拷贝，但引入了带右值引用的拷贝构造和赋值函数。

## 3 带引用计数的智能指针

当需要多个智能指针指向同一个资源时，使用带引用计数的智能指针。

每增加一个智能指针指向同一资源，资源引用计数加一，反之减一。当引用计数为零时，由最后一个指向资源的智能指针将资源进行释放。

下图表示带引用计数智能指针的工作过程。sp1 对象和 sp2 对象通过指针指向同一资源，引用计数器记录了引用资源的对象个数。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_5.PNG">

当 sp1 对象发生析构时，引用计数器的值减 1，由于引用计数不等于 0，资源并未释放，如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_6.PNG">

当 sp2 对象也发生析构，引用计数减为 0，资源释放，如下图所示：

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_7.PNG">

即引用计数可以保证多个智能指针指向资源时资源在所有智能对其取消引用再释放，避免过早释放产生空悬指针。带引用计数的智能指针包括 `shared_ptr` 和 `weak_ptr`。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_8.PNG">

#### 3.1 shared_ptr

`shared_ptr` 一般称为强智能指针，一个 `shared_ptr` 对资源进行引用时，资源的引用计数会增加一，通常用于管理对象的生命周期。只要有一个指向对象的 `shared_ptr` 存在，该对象就不会析构。

上图中引用计数的工作过程就使用了 `shared_ptr`。

#### 3.2 weak_ptr

`weak_ptr` 一般被称为弱智能指针，其对资源的引用不会引起资源的引用计数的变化，通常作为观察者，用于判断资源是否存在，并根据不同情况做出相应的操作。

比如，使用 `weak_ptr` 对资源进行弱引用，当调用 `weak_ptr` 的`lock()`方法时，若返回`nullptr`，则说明资源已经不存在，放弃对资源继续操作。否则，将返回一个 `shared_ptr` 对象，可以继续操作资源。

另外，一旦最后一个指向对象的 `shared_ptr` 被销毁，对象就会被释放。即使有 `weak_ptr` 指向对象，对象也还是会被释放。

既然它这引用都不算数，那它有什么用呢？

别急，我们来慢慢讲。

## 4 enable_shared_from_this 机制

考虑下面这样一个场景：

在多线程环境中，假设有一个对象池类 `ObjectPool` 和一个对象类 `Object`。`ObjectPool` 类主要实现通过不同的 `key` 返回对应 `Object` 对象。

要求同一程序中由 Object 类实例出的不同对象只有一个，即当多处用到同一个对象，Object 对象应该被共享。同时当对象不再需要时应该被析构，并删除对应的 key。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_9.PNG">

读者可能会说，这还不简单。 Show you code.

```c++

//场景代码
#include<memory>
class ObjectPool:boost::noncopyable
{
public:
    shared_ptr<Object> get(const string& key)
    {
        shared_ptr<Object> shObject;
        MutexLockGuard lock(mutex);
        weak_ptr<Object>& wkObject=object[key];
        shObject=wkObject.lock();
        // 对象存在，提升成功并返回
        if(!shObject){
            /* 对象不存在，提升失败，shOject重新
             * 指向新创建的Object对象，
             * 并绑定回调函数，让对象Oject需要析构时
             * 调用OjectPool对象的成员函数
             */
            shObject.reset(new Object(key), boost::bind(&ObjectPool::deleteObject,this, _1));
            wkObject=shObject;
        }
        return shObject;
    }
private:
    void deleteObject(Object* obj)
    {
        /* 回调函数，在对象需要析构时调用，从map中
         * 删除对象和对应的key
         */
        if(obj){
            MutexLockGuard lock(mutex);
            object.erase(obj->key());
        }
        delete obj;
    }
    mutable MutexLock mutex;
    std::map<string,weak_ptr<Object>> object;
    /*map中不能使用shared_ptr，这会导致Object对象永远不会被销毁*/
};
```

小牛说：你这有问题啊？

小艾答：有什么问题？为了实现 Object 类析构时调用 ObjectPool 的回调函数，代码中把 ObjectPool 的 this 指针保存在了 boost::function 处。

小牛说：那线程安全问题就来了。如果 ObjectPool 先于 Object 对象析构，就会发生 core dump。因为 ObjectPool 对象已经不存在了，也就没有办法调用其成员方法。

小艾问：那怎么解决呢？

小牛说：简单啊，只需将 this 指针替换成指向当前对象的 shared_ptr，从而保证在 Object 对象需要调用 ObjectPool::deleteObject 时 ObjectPool 还活着。你要不试试实现一下？

小艾说：那我写一个吧。

```c++
shared_ptr<A> getSharedPtr() 
{ 
    return shared_ptr<A>(this); 
}
```
小牛答：问题来了，在多线程环境中，在需要返回 this 对象时是无法得知对象的生存情况的。因此不能直接返回 this 对象。

给你普及个解决方法吧，你可以通过继承 enable_shared_from_this 模板对象，然后调用从基类继承而来的 shared_from_this 方法来安全返回指向同一资源对象的 shared_ptr.

小艾：为什么继承 enable_shared_from_this 模板对象就可以安全返回？

小牛：在回答你的问题前，我们先来讲讲 shared_ptr 的构造函数和拷贝构造函数对资源和引用计数影响的区别。

下面从 shared_ptr 的实现原理来看：

shared_ptr 从 `_Ptr_base` 继承了 element_type 和 `_Ref_count_base` 类型的两个成员变量。

```c++
template<class _Ty>
class _Ptr_base
{ 
private:
    element_type * _Ptr{nullptr};       // 指向资源的指针
    _Ref_count_base * _Rep{nullptr};    // 指向资源引用计数的指针
};
```
`_Ref_count_base` 中定义了原子类型的变量 `_Uses` 和 `_Weaks`，它们分别记录资源的引用个数和资源观察者的个数。

```c++
class __declspec(novtable) _Ref_count_base
{ 
private:
    _Atomic_counter_t _Uses;    // 记录资源引用个数
    _Atomic_counter_t _Weaks;   // 记录观察者个数
}
```

当要使用 `shared_ptr` 管理同一资源，调用 `shared_ptr` 的构造函数和拷贝构造函数是不一样的，它们虽然使得不同 `shared_ptr` 指向同一资源，但管理引用计数资源的方式却不一样。

下面给出两个 `shared_ptr` 管理同一资源（A对象）使用不同构造函数对引用计数对象的影响。

方式1：调用构造函数

```c++

class A
{
public:
    A(){}
    ~A(){}
};

A *p = new A(); 

shared_ptr<A> ptr1(p);  // 调用构造函数
shared_ptr<A> ptr2(p);  // 调用构造函数
```

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_10.PNG">

如上图所示，方式1中 ptr1 和 ptr2 都调用了 shared_ptr 的构造函数，该构造方式使得 ptr1 和 ptr2 都开辟了自已的引用资源对象 `_Ref_count_base`，即 `_Ref_count_base` 有两个，都记录了 A 对象的引用计数为 1，析构时 ptr1 和 ptr2 的引用计数各自减为0，导致 A 对象析构两次，出现逻辑错误。

方式2：调用拷贝构造函数

```c++
class A
{
public:
    A(){}
    ~A(){}
};

A *p = new A();

shared_ptr<A> ptr1(p);      // 调用构造函数
shared_ptr<A> ptr2(ptr1);   // 调用拷贝构造函数
```

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C%2B%2B/images/auto_pointer_1_11.PNG">

如上图所示，方式2中由于 ptr2 拷贝构造 ptr1，它们引用的 _Ref_count_base 是同一个，因此引用计数为 2，析构的时候 A 对象只析构一次，正常运行。

在明白了 shared_ptr 构造函数和拷贝构造函数的做的事情不同后，就能理解当需要返回一个需要 shared_ptr 管理的对象为什么不能写成 `return shared_ptr<A>(this)` 了。

小艾：说的没错，因为这样会调用 shared_ptr 的构造函数，对于 this 对象再创建一个新的引用计数对象，从而导致对象多次析构而出现逻辑错误。

小牛：再给你深入讲讲 enable_shared_from_this 的实现机制。

如下所示，enable_shared_from_this 类中包含一个作为观察者的成员变量。

```c++

template<class _Ty>
class enable_shared_from_this
{ 
public:
   mutable weak_ptr<_Ty> _Wptr; // 指向资源
};
```
当一个类继承了 enable_shared_from_this 类，就继承了 `_Wptr` 这个成员变量。

当使用 `shared_ptr<A>(new A())` 第一次构造智能指针对象时，就会初始化一个作为观察者的弱智能指针 _Wptr 指向A对象资源。

再通过 shared_from_this() 方法代替 shared_ptr 的普通构造函数来返回一个 shared_ptr 对象，从而避免产生额外的引用计数对象。

```c++
shared_ptr<A> getSharedPtr() 
{ 
    return shared_from_this(); 
}
```

在 shared_from_this 函数中,主要尝试将弱智能指针提升为强智能指针来返回一个 shared_ptr 对象。

这样还能在多线程环境中判断对象是否存活，存活即提升成功，安全返回。如果对象已经析构，则放弃提升，即起到了保证线程安全的作用。

小牛：了解了enable_shared_from_this，要不再试试改代码？

小艾：那我来改一下之前的代码。

第一处修改：

```c++
class ObjectPool:boost::noncopyable
//为
class ObjectPool:public boost::enable_shared_from_this<ObjectPool>，
                boost::noncopyable
{/*...*/};
```

第二处修改：

```c++
//改变
shared_ptr<Object> get(const string& key)
{
    /*...*/
    shObject.reset(new Object(key),
                    boost::bind(&ObjectPool::deleteObject,this,_1));
    /*...*/   
}
//为
shared_ptr<Object> get(const string& key)
{
    /*...*/
    shObject.reset(new Object(key),
                    boost::bind(&ObjectPool::deleteObject,shared_from_this(),_1));
    /*...*/   
}
```
完整代码：

```c++
#include<memory>
class ObjectPool:public boost::enable_shared_from_this<ObjectPool>，
                boost::noncopyable
{
public:
    shared_ptr<Object> get(const string& key)
    {
        shared_ptr<Object> shObject;
        MutexLockGuard lock(mutex);
        weak_ptr<Object>& wkObject=object[key];
        shObject=wkObject.lock();//对象存在，提升成功并返回
        if(!shObject){
            /* 对象不存在，提升失败，shOject重新
             * 指向新创建的Object对象，
             * 并绑定回调函数，让对象Oject需要析构时
             * 调用OjectPool对象的成员函数
             */
            shObject.reset(new Object(key),
                    boost::bind(&ObjectPool::deleteObject,shared_from_this(),_1));
            wkObject=shObject;
        }
        return shObject;
    }
private:
    void deleteObject(Object* obj)
    {
        /*
         * 回调函数，在对象需要析构时调用，从map中删除对象和对
         * 应的key*/
        if(obj){
            MutexLockGuard lock(mutex);
            object.erase(obj->key());
        }
        delete obj;
    }

    mutable MutexLock mutex;
    std::map<string, weak_ptr<Object>> object;
    /*map中不能使用shared_ptr，这会导致Oject对象永远不会被销毁*/
};
```

小牛：不错不错，这下懂了 shared_ptr 和 weak_ptr 结合的用法了吧。

带引用计数智能指针总结：

1. shared_ptr 会增加资源的引用计数，常用于管理对象的生命周期。
2. weak_ptr 不会增加资源的引用计数，常作为观察者用来判断对象是否存活。
3. 使用 shared_ptr 的普通拷贝构造函数会产生额外的引用计数对象，可能导致对象多次析构。使用 shared_ptr 的拷贝构造函数则只影响同一资源的同一引用计数的增减。
4. 当需要返回指向当前对象的 shared_ptr 时，优先使用 enable_shared_from_this 机制。

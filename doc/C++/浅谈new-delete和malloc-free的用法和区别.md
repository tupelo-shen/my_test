<h2 id="0">0 目录</h2>

* [1 new和delete的用法](#1)
* [2 new和delete深入理解](#2)
    - [2.1 operator new 和 operator delete](#2.1)
    - [2.2 new和delete背后机制](#2.1)
* [3 malloc和free的用法](#3)
* [4 new和malloc的区别](#4)

***

<h2 id="1">1 new和delete的用法</h2>

如下几行代码：

    int *pi = new int;
    int *pi = new int();
    int *pi = new int(1024);

1. 第一行代码就是动态生成了存储一个整型对象的空间，并把该空间的地址赋给整形指针`pi`；
2. 第二行和第一行一样，只是将分配的存储空间的内容初始化为`0`；
3. 第三行初始化为`1024`；

动态创建的对象必须在用完之后，必须再释放掉，避免造成内存泄漏。使用`delete`函数完成。`new`和`delete`是一对。

    delete pi；

此时，虽然`pi`所指向的地址里的内容虽然被释放，但是其所存放的地址还在。为了保险起见，可以在释放对象后，立即将其指针设置为`NULL`，清楚地表明不再指向任何对象。

    pi = NULL；

C++允许动态创建`const`对象：

    const int  *pi = new const int(1024);

动态创建的`const`对象必须进行初始化，并且初始化后的值不能再被改变。

当创建一个动态数组对象和进行内存释放时，执行以下语句：

    int *pi = new int[];        // 指针pi所指向的数组未初始化
    int *pi = new int[n];       // 指针pi指向长度为n的数组，未初始化
    int *pi = new int[]();      // 指针pi所指向的地址初始化为0
    delete[] pi;                // 回收pi所指向的数组

到这儿，不免会有疑问，`new`和`delete`到底是什么？操作符还是函数？

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="2">2 new和delete深入理解</h2>

<h3 id="2.1">2.1 概念理解</h3>

首先，`operator new` 与 `new operator`是不同的概念。`operator new`和 `operator delete`其实是C++ 语言标准库的库函数， 原型分别如下：

    void *operator new(size_t);         // 分配一个对象
    void *operator delete(void *);      // 释放一个对象

    void *operator new[](size_t);       // 分配一个数组
    void *operator delete[](void *);    // 释放一个数组

前面两个均是C++标准库函数，你可能会觉得这是函数吗？请不要怀疑，这就是函数！这两个函数和`malloc` 和 `free` 函数有点像了，都是用来申请和释放内存的，并且 `operator new `申请内存之后不对内存进行初始化，直接返回申请内存的指针。

<h3 id="2.2">2.2 new和delete背后机制</h3>

知道上面两个函数之后，我们用一个实例来解释 `new` 和 `delete` 背后的机制。 我们不用简单的 C++ 内置类型来举例， 使用复杂一点的类类型，定义一个类 A：

    class A
    {
    public:
        A(int v) : var(v)
        {
            fopen_s(&file, "test", "r");
        }
        ~A()
        {
            fclose(file);
        }

    private:
        int     var;
        FILE    *file;
    };

很简单，类 A 中有两个私有成员，有一个构造函数和一个析构函数，构造函数中初始化私有变量 var 以及打开一个文件， 析构函数关闭打开的文件。

我们使用

    class A *pA = new A(10);

来创建一个类的对象，返回其指针 pA。如下图所示 new 背后完成的工作：

![new_delete_1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/C++/images/new_delete_1.jpg)

简单总结一下：

首先需要调用上面提到的 operator new 标准库函数，传入的参数为 class A 的大小，这里为 8 个字节，至于为什么是 8 个字节，你可以看看《深入C++对象模型》一书，这里不做多解释。这样函数返回的是分配内存的起始地址，这里假设是 0x007da290。
上面分配的内存是未初始化的，也是未类型化的，第二步就在这一块原始的内存上对类对象进行初始化，调用的是相应的构造函数，这里是调用 A:A(10); 这个函数，从图中也可以看到对这块申请的内存进行了初始化，var=10, file 指向打开的文件。
最后一步就是返回新分配并构造好的对象的指针，这里 pA 就指向 0x007da290 这块内存，pA 的类型为类 A 对象的指针。
所有这三步，你都可以通过反汇编找到相应的汇编代码，在这里我就不列出了。

好了，那么 delete 都干了什么呢？还是接着上面的例子，如果这时想释放掉申请的类的对象怎么办？当然我们可以使用下面的语句来完成：

    delete pA;

delete 所做的事情如下图所示：

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="3">3 malloc和free的用法</h2>

两个函数的原型如下，他们都在头文件`stdlib.h`中声明。

    void *malloc(size_t size);
    void free(void *pointer);

示例代码如下：

    int *p = (int *)malloc(100);            //指向整型的指针p指向一个大小为100字节的内存的地址
    int *p = (int *)malloc(25*sizeof(int)); //指向整型的指针p指向一个25个int整型空间的地址

因为`malloc()`函数的返回值类型为`void *`，所以需要在函数前面进行相应的强制类型转换。当int占4个字节内存时，上述的两个语句代码获得的内存空间大小是相同的。分配内存后必须验证内存是否分配成功，完成后用`free()`释放内存，完整语句如下。

    int *p=(int *)malloc(int);
    if(pi==NULL)
        printf("Out of memory!\n");
    free (p);

另外还有两个分配内存的函数：calloc和realloc，他们的原型如下：

    void *calloc(size_t num_elements,size_t element_size);
    void realloc(void *tr , size_t new_size);

`malloc`和`calloc`间的主要区别在于, 后者在返回指向内存的指针之前把它初始化为0。 另一个区别是`calloc`的参数包括所需的元素的数量和每个元素的字节数。

`realloc`函数用于修改一个原先已经分配的内存块的大小。可以使一块内存扩大或缩小，如果扩大内存，则原来的内存块保持不变，在内存尾部增加新的内存块，切不进行初始化。如果缩小内存，则原来内存块从尾部进行删减。如果原先的内存块无法扩充，则新开辟一块内存，并复制原先的内存的内容，原先内存块失效无法再进行访问。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>

<h2 id="4">4 new和malloc的区别</h2>

1. 属性

    `new/delete`是C++关键字，需要编译器支持。`malloc/free`是库函数，需要头文件支持c。

2. 参数

    使用`new`操作符申请内存分配时无须指定内存块的大小，编译器会根据类型信息自行计算。而`malloc`则需要显式地指出内存的大小。

3. 返回类型

    `new`申请内存分配成功时，返回的是对象类型的指针，类型严格与对象匹配，无须进行类型转换，故`new`是符合类型安全性的操作符。 而`malloc`内存分配成功则是返回`void *` ，需要通过强制类型转换将`void*`指针转换成我们需要的类型。

4. 分配失败

    `new`内存分配失败时，会抛出`bac_alloc`异常。`malloc`分配内存失败时返回`NULL`。

5. 自定义类型

    `new`会先调用`operator new`函数，申请足够的内存（通常底层使用`malloc`实现）。 然后调用类型的构造函数， 初始化成员变量， 最后返回自定义类型指针。`delete`先调用析构函数，然后调用`operator delete`函数释放内存（通常底层使用`free`实现）。

    `malloc/free`是库函数，只能动态地申请和释放内存，无法强制要求其做自定义类型对象构造和析构工作。

6. 重载

    C++允许重载`new/delete`操作符，特别的，`placement new`的就不需要为对象分配内存，而是指定了一个地址作为内存起始区域，new在这段内存上为对象调用构造函数完成初始化工作，并返回此地址。而`malloc`不允许重载。

7. 内存区域

    `new`操作符从自由存储区（free store）上为对象动态分配内存空间，而`malloc`函数从堆上动态分配内存。自由存储区是C++基于`new`操作符的一个抽象概念，凡是通过`new`操作符进行内存申请，该内存即为自由存储区。而堆是操作系统中的术语，是操作系统所维护的一块特殊内存，用于程序的内存动态分配，C语言使用`malloc`从堆上分配内存， 使用`free`释放已分配的对应内存。自由存储区不等于堆，如上所述，布局`new`就可以不位于堆中。

**PS**:

> 在C++中，内存区分为5个区，分别是堆、栈、自由存储区、全局/静态存储区、常量存储区；
>
> 在C中，C内存区分为堆、栈、全局/静态存储区、常量存储区；
>
> `new`缺省的实现方式本质上是通过`malloc`的，这个时候，C++的自由存储区的概念和C的堆的概念是没有区别的， 但是, 如果我们通过重载`operator new `的方式把内存分配在一些全局变量上，那么这些内存就不属于堆区了，而是在`data segment`。也就是说，C++的自由存储区可以包括堆区，也可以包括其他区域。

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
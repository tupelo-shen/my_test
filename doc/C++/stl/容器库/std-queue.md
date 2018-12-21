
头文件**<queue\>**

声明如下：

    template<class T, class Container = std::deque<T> > class queue;

`queue`是一种容器适配器，是FIFO（先进先出）数据结构的实现，其中，元素插入容器的一端，从另一端读取。

`queue`被实现为一个容器适配器，它是使用特定容器类的封装对象作为其底层容器的类，提供一组特定的成员函数来访问其元素。元素被`push`到队尾。从队列的前面`pop`。

## 模板参数

* T
    
    存储元素的类型。如果T的类型和容器类的类型Container::value_type不一样时，其行为没有定义。（C++17之后）。

* Container
    
    用来存储元素的基础容器的类型。容器必须满足序列容器的要求。 此外，该容器类型还必须提供下面的函数：

    * back()
    * front()
    * push_back()
    * pop_front()

    标准容器std::deque和std：：list满足这些要求。

## 成员类型

| 成员类型 | 定义 |
| ------ | ------ |
| value_type | Container::value_type |
| size_type | Container::size_type |
| reference | Container::reference |
| const_reference | Container::const_reference |

## 成员函数

| 成员 | 说明 | 属性 |
| ------ | ------ | ------ |
| 构造函数 | 构造队列 | public |
| 析构函数 | 销毁队列 | public |
| operator= | 队列赋值 | public |

## 队列索引

| 成员 | 说明 | 属性 |
| ------ | ------ | ------ |
| front | 访问第一个元素 | public |
| back | 访问最后一个元素 | public |

## 操作方法

| 成员 | 说明 | 属性 |
| ------ | ------ | ------ |
| push | 插入一个元素到队尾 | public |
| emplace | 在队尾构造一个元素 | public |
| pop | 移除第一个元素 | public |
| swap | 交换内容 | public |

## 底层容器

| 成员 | 说明 | 属性 |
| ------ | ------ | ------ |
| Container c | 底层容器 | protected |
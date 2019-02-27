头文件**<utility\>**

声明如下：

    template<class T1, class T2> std::pair;                

*struct*模板 *std::pair* 提供了一种存储不同类型数据的方法。 它是 *std::tuple* 两个元素时的一种特殊情况。

## 模板参数

* T1, T2
    
    *pair* 存储的元素类型

## 成员函数

| 成员 | 说明 | 属性 |
| ------------- | -----------------| ------ |
| constructor   | 构造一个新的元组  | public |
| operator=     | 元组给元组赋值    | public |
| swap          | 交换2个元组的内容 | public |

## 非成员函数

| 成员 | 说明 | 属性 |
| --------------------- | -----------------| ---------------- |
| make_pair             | 创建一个由参数类型指定的pair对象       | 函数模板 |
| std::get(std::pair)   | 访问pair特定元素                      | 函数模板 |
| std::swap(std::pair)  | 特化std::swap算法                     | 函数模板 |

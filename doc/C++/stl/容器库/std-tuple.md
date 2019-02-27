头文件**<tuple\>**

声明如下：

    template<class ... Types >
    class tuple;                

类模板 *std::tuple* 是不同类型数据的集合，大小固定。 它是 *std::pair* 的泛化。

## 模板参数

* Types
    
    元组 *tuple* 存储的数据类型，支持空列表。

## 成员函数

| 成员 | 说明 | 属性 |
| ------------- | -----------------| ------ |
| constructor   | 构造一个新的元组  | public |
| operator=     | 元组给元组赋值    | public |
| swap          | 交换2个元组的内容 | public |

## 非成员函数

| 成员 | 说明 | 属性 |
| --------------------- | -----------------| ------ |
| make_tuple            | 创建一个由参数类型指定的元祖对象      | 函数模板 |
| tie                   | 创建左值引用元组或将元组分解为单个对象 | 函数模板 |
| forward_as_tuple      | 创建左值引用元组                      | 函数模板 |
| tuple_cat             | 截取一个元组创建一个新的元组            | 函数模板 |
| std::get(std::tuple)  | 访问元组特定元素                      | 函数模板 |
| std::swap(std::tuple) | 特化std::swap算法                     | 函数模板 |


## 举例

    #include <tuple>
    #include <iostream>
    #include <string>
    #include <stdexcept>

    std::tuple<double, char, std::string> get_student(int, id)
    {
        if (id == 0) return std::make_tuple(3.8, 'A', "Lisa Simpson");
        if (id == 1) return std::make_tuple(2.9, 'C', "Milhouse Van Houten");
        if (id == 2) return std::make_tuple(1.7, 'D', "Ralph Wiggum");
        throw std::invalid_argument("id");
    }

    int main(int argc, char const *argv[])
    {
        auto student0 = get_student(0);
        std::cout << "ID: 0, "
                  << "GPA: " << std::get<0>(student0) << ", "
                  << "grade: " << std::get<1>(student0) << ", "
                  << "name: " << std::get<2>(student0) << '\n';
        double gpa1;
        char grade1;
        std::string name1;
        std::tie(gpa1, grade1, name1) = get_student(1);
        std::cout << "ID: 1, "
                  << "GPA: " << gpa1 << ", "
                  << "grade: " << grade1 << ", "
                  << "name: " << name1 << '\n';
        
        // C++17 structured binding:
        auto [ gpa2, grade2, name2 ] = get_student(2);
        std::cout << "ID: 2, "
                  << "GPA: " << gpa2 << ", "
                  << "grade: " << grade2 << ", "
                  << "name: " << name2 << '\n';

        return 0;
    }
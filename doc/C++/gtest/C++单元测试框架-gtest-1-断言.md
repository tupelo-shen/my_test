<h1 id="0">0 目录</h1>

* [1 前言](#1)
* [2 bool值检查](#2)
* [3 数值型检查](#3)
* [4 字符串检查](#4)
* [5 显示返回成功或失败](#5)
* [6 异常检查](#6)
* [7 Predicate 检查](#7)
* [8 浮点检查](#8)


<h1 id="1">1 前言</h1>

*gtest*中，断言的宏可以理解为分为两类，一类是ASSERT系列，一类是EXPECT系列。

1. ASSERT_* 系列的断言，当检查点失败时，退出当前函数（注意：并非退出当前案例）。
 EXPECT_* 系列的断言，当检查点失败时，继续往下执行。

比如：

    EXPECT_EQ(3, add(1,2))  // 比较函数add的结果是否和3相等

假如，在此处你的预期结果是4，则会显示下面所示的输出：

    g:文件名称: error: Value of: Add(1, 2)
      Actual: 4
    Expected:3

如果你对自动输出的出错信息不满意的话，你还可以通过操作符 **<<** 将一些自定义的信息输出， 通常， 这对于调试或是对一些检查点的补充说明来说， 非常有用！

下面举个例子：

    for (int i = 0; i < x.size(); ++i)
    {
        EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
    }

这样，我们更方便定位出错信息。

<h1 id="2">2 bool值检查</h1> 

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_TRUE(条件表达式) | EXPECT_TRUE(条件表达式) | (条件表达式) is true |
| ASSERT_FALSE(条件表达式) | EXPECT_FALSE(条件表达式) | (条件表达式) is false |

<h1 id="3">3 数值型检查</h1>

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_EQ(expected, actual);  | EXPECT_EQ(val1, val2);    | expected == actual |
| ASSERT_NE(val1, val2);        | EXPECT_NE(val1, val2);    | val1 != val2 |
| ASSERT_LT(val1, val2);        | EXPECT_LT(val1, val2);    | val1 < val2 |
| ASSERT_LE(val1, val2);        | EXPECT_LE(val1, val2);    | val1 <= val2 |
| ASSERT_GT(val1, val2);        | EXPECT_GT(val1, val2);    | val1 > val2 |
| ASSERT_GE(val1, val2);        | EXPECT_GE(val1, val2);    | val1 >= val2 |

<h1 id="4">4 字符串检查</h1>

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_STREQ(expected_str, actual_str);       | EXPECT_STREQ(val1, val2);         | 2个字符串相同 |
| ASSERT_STRNE(str1, str2);                     | EXPECT_STRNE(val1, val2);         | 2个字符串不同 |
| ASSERT_STRCASEEQ(expected_str, actual_str);   | EXPECT_STRCASEEQ(val1, val2);     | 忽略大小写，2个字符串相同 |
| ASSERT_STRCASENE(str1, str2);                 | EXPECT_STRCASENE(val1, val2);     | 忽略大小写，2个字符串不同 |

*STREQ*和 *STRNE* 同时支持 char* 和 wchar_t* 类型的，*STRCASEEQ* 和 *STRCASENE* 却只接收char*，估计是不常用吧。下面是几个例子：

<h1 id="5">5 显示返回成功或失败</h1>

直接返回成功：*SUCCEED()*;返回失败的时候，分2种情况：致命断言（FAIL();）和非致命断言（ADD_FAILURE();）

<h1 id="6">6 异常检查</h1>

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_THROW(语句, 异常类型);    | EXPECT_THROW(语句, 异常类型);    | 语句所指定的代码抛出给定的异常 |
| ASSERT_ANY_THROW(语句);         | EXPECT_ANY_THROW(语句);         | 语句所指定的代码抛出任何一种异常 |
| ASSERT_NO_THROW(语句);          | EXPECT_NO_THROW(语句);          | 语句所指定的代码不抛出任何异常 |

举例：

    int Foo(int a, int b)
    {
        if (a == 0 || b == 0)
        {
            throw "don't do that";
        }
        int c = a % b;
        if (c == 0)
            return b;
        return Foo(b, c);
    }
    TEST(FooTest, HandleZeroInput)
    {
        EXPECT_ANY_THROW(Foo(10, 0));
        EXPECT_THROW(Foo(0, 5), char*);
    }


<h1 id="7">7 Predicate 检查</h1>

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_PRED1(pred1, val1);        | ASSERT_PRED1(pred1, val1);        | pred1(val1) 返回true |
| ASSERT_PRED2(pred2, val1, val2);  | ASSERT_PRED2(pred2, val1, val2);  | pred2(val1, val2) 返回true |
| ...                               | ...                               | ...   |

举例：

    bool MutuallyPrime(int m, int n)
    {
        return Foo(m , n) > 1;
    }

    TEST(PredicateAssertionTest, Demo)
    {
        int m = 5, n = 6;
        EXPECT_PRED2(MutuallyPrime, m, n);
    }

输入结果为：

    error: MutuallyPrime(m, n) evaluates to false, where
    m evaluates to 5
    n evaluates to 6

如果对这样的输出不满意的话，还可以自定义输出格式，通过如下：

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_PRED_FORMAT1(p_format1, val1);        | EXPECT_PRED_FORMAT1(p_format1, val1);        | p_format1(val1)成功 |
| ASSERT_PRED_FORMAT2(p_format2, val1, val2);  | EXPECT_PRED_FORMAT2(p_format2, val1, val2);    | p_format2(val1, val2) 成功 |
| ...                                           | ...                                           | ...   |

演示示例：

    testing::AssertionResult AssertFoo(const char* m_expr, const char* n_expr, const char* k_expr, int m, int n, int k) {
        if (Foo(m, n) == k)
            return testing::AssertionSuccess();
        testing::Message msg;
        msg << m_expr << " 和 " << n_expr << " 的最大公约数应该是：" << Foo(m, n) << " 而不是：" << k_expr;
        return testing::AssertionFailure(msg);
    }

    TEST(AssertFooTest, HandleFail)
    {
        EXPECT_PRED_FORMAT3(AssertFoo, 3, 6, 2);
    }

失败时，输出的信息为：
    
    error: 3 和 6 的最大公约数应该是：3 而不是：2

<h1 id="8">8 浮点检查</h1>

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_FLOAT_EQ(expected, actual);    | EXPECT_FLOAT_EQ(expected, actual);        | float几乎相等 |
| ASSERT_DOUBLE_EQ(expected, actual);   | EXPECT_DOUBLE_EQ(expected, actual);       | double几乎相等 |

对2个相近的数进行比较：

| Fatal 断言 | 非 Fatal 断言 | 验证 |
| ------ | ------ | ------ |
| ASSERT_NEAR(val1,val2,abs_error);    | EXPECT_NEAR(val1,val2,abs_error);     | 两个数之间的差不超过绝对值abs_error |

同时，还可以使用：

    EXPECT_PRED_FORMAT2(testing::FloatLE, val1, val2);
    EXPECT_PRED_FORMAT2(testing::DoubleLE, val1, val2);


转自<[2.玩转Google开源C++单元测试框架Google Test系列(gtest)之二 - 断言](http://www.cnblogs.com/coderzh/archive/2009/04/06/1430364.html)>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
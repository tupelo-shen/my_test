<h1 id="0">0 目录</h1>

* [1 前言](#1)
* [2 全局事件](#2)
* [3 TestSuite事件](#3)
* [4 TestCase事件](#4)
* [5 总结](#5)

<h1 id="1">1 前言</h1>

gtest提供了多种事件机制，非常方便我们在案例之前或之后做一些操作。总结一下gtest的事件一共有3种：

1. 全局的，所有案例执行前后。
2. TestSuite级别的，在某一批案例中第一个案例前，最后一个案例执行后。
3. TestCase级别的，每个TestCase前后。

<h1 id="2">2 全局事件</h1>

要实现全局事件，必须写一个类，继承 *testing::Environment*类，实现里面的 *SetUp* 和 *TearDown* 方法。

1. SetUp()方法在所有案例执行前执行
2. TearDown()方法在所有案例执行后执行

比如：

    class FooEnvironment : public testing::Environment
    {
    public:
        virtual void SetUp()
        {
            std::cout << "Foo FooEnvironment SetUP" << std::endl;
        }
        virtual void TearDown()
        {
            std::cout << "Foo FooEnvironment TearDown" << std::endl;
        }
    };

当然，这样还不够，我们还需要告诉gtest添加这个全局事件，我们需要在main函数中通过 *testing::AddGlobalTestEnvironment*方法将事件挂进来，也就是说，我们可以写很多个这样的类，然后将他们的事件都挂上去。

    int _tmain(int argc, _TCHAR* argv[])
    {
        testing::AddGlobalTestEnvironment(new FooEnvironment);
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

<h1 id="3">3 TestSuite事件</h1>

我们需要写一个类，继承 *testing::Test*，然后实现2个静态方法

1. SetUpTestCase() 方法在第一个TestCase之前执行
2. TearDownTestCase() 方法在最后一个TestCase之后执行

如：

    class FooTest : public testing::Test {
    protected:
        static void SetUpTestCase() {
            shared_resource_ = new ;
        }
        static void TearDownTestCase() {
            delete shared_resource_;
            shared_resource_ = NULL;
        }
        // Some expensive resource shared by all tests.
        static T* shared_resource_;
    };

在编写测试案例时，我们需要使用TEST_F这个宏，第一个参数必须是我们上面类的名字，代表一个TestSuite。
    
    TEST_F(FooTest, Test1)
    {
        // you can refer to shared_resource here 
    }
    TEST_F(FooTest, Test2)
    {
        // you can refer to shared_resource here 
    }

<h1 id="4">4 TestCase事件 </h1>

TestCase事件是挂在每个案例执行前后的，实现方式和上面的几乎一样，不过需要实现的是SetUp方法和TearDown方法：

1. SetUp()方法在每个TestCase之前执行
2. TearDown()方法在每个TestCase之后执行

如：

    class FooCalcTest:public testing::Test
    {
    protected:
        virtual void SetUp()
        {
            m_foo.Init();
        }
        virtual void TearDown()
        {
            m_foo.Finalize();
        }

        FooCalc m_foo;
    };

    TEST_F(FooCalcTest, HandleNoneZeroInput)
    {
        EXPECT_EQ(4, m_foo.Calc(12, 16));
    }

    TEST_F(FooCalcTest, HandleNoneZeroInput_Error)
    {
        EXPECT_EQ(5, m_foo.Calc(12, 16));
    }


<h1 id="5">5 总结 </h1>

gtest提供的这三种事件机制还是非常的简单和灵活的。同时，通过继承Test类，使用TEST_F宏，我们可以在案例之间共享一些通用方法，共享资源。使得我们的案例更加的简洁，清晰。

其它请参考<[C++单元测试框架-gtest-1-断言](https://blog.csdn.net/shenwanjiang111/article/details/87990504)>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
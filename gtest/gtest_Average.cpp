#include "gtest/gtest.h"
#include "Average.h"

// --------------------<测试 Factorial()>---------------------------------
// 测试负数的阶乘
TEST(FactorialTest, Negative) {
    
    // 这个测试被命名为"Negative"测试，"FactorialTest" 是测试用例名称

    EXPECT_EQ(1, Factorial(-5));
    EXPECT_EQ(1, Factorial(-1));
    EXPECT_GT(Factorial(-10), 0);
 
    // 1. EXPECT_EQ(expected, actual) 等同于 EXPECT_TRUE((expected) == (actual))
    // 2. 如果期望在断言（assert）失败时，打印期望值和真实值，则优先选用 EXPECT_EQ
    // 3. 但是 EXPECT_TRUE接受任何boolean表达式，其更通用。
}


// 测试“0”的阶乘
TEST(FactorialTest, Zero) {
    EXPECT_EQ(1, Factorial(0));
}

// 测试正数的阶乘
TEST(FactorialTest, Positive) {
    EXPECT_EQ(1, Factorial(1));
    EXPECT_EQ(2, Factorial(2));
    EXPECT_EQ(6, Factorial(3));
    EXPECT_EQ(40320, Factorial(8));
}

// --------------------<测试 IsPrime()>---------------------------------
// 测试负数是否为质数（也称素数）
TEST(IsPrimeTest, Negative) {
    
    // 这个测试属于IsPrimeTest测试用例
 
    EXPECT_FALSE(IsPrime(-1));
    EXPECT_FALSE(IsPrime(-2));
    EXPECT_FALSE(IsPrime(INT_MIN));     // INT_MIN 定义在 <limits.h> 文件中
}
 
// 测试 正输入(>0)
TEST(IsPrimeTest, Positive) {
    EXPECT_FALSE(IsPrime(0));
    EXPECT_FALSE(IsPrime(1));
    EXPECT_TRUE(IsPrime(2));
    EXPECT_TRUE(IsPrime(3));
    EXPECT_FALSE(IsPrime(4));
    EXPECT_TRUE(IsPrime(5));
    EXPECT_FALSE(IsPrime(6));
    EXPECT_FALSE(IsPrime(9));
    EXPECT_TRUE(IsPrime(11));
    EXPECT_TRUE(IsPrime(23));
}
// --------------------<测试 参数化>-----------------------------------
using namespace ::testing;

struct paramList
{
    bool    out;
    int     in;
};
// #include <map>
// static std::map<bool,int> mymap;
// mymap.insert(pair<bool,int>(false,10));
// mymap.insert(pair<bool,int>(true,3));
// mymap.insert(pair<bool,int>(false,7));
std::map<bool,int> mymap{
    {false, 10},
    {true,  3},
    {false, 7}
};
class A : public ::testing::TestWithParam<std::pair<const bool,int>>
{

};
TEST_P(A, IsPrime)
{
    // bool out = GetParam().out;
    // int in = GetParam().in;
    // bool out = testing::get<0>(GetParam());
    // int in = testing::get<1>(GetParam());
    bool out = testing::get<0>(GetParam());
    int in = testing::get<1>(GetParam());

    // std::map<bool, int>::iterator iter;
    // for(iter = GetParam().begin(); iter != GetParam().end(); iter++)
    // {
    //     bool out = iter->first;
    //     int  in = iter->second;
        EXPECT_EQ(out, IsPrime(in));
    // }
       
}
// INSTANTIATE_TEST_CASE_P(P,
//     A,
//     Values(paramList{false, 10}, paramList{true, 3}));
INSTANTIATE_TEST_CASE_P(P,
    A,
    ValuesIn(mymap.begin(), mymap.end()));
// class A_IsPrime_Test : public A 
// { 
//     public: 
//         A_IsPrime_Test() {} 
//         virtual void TestBody(); 
//     private: 
//         static int AddToRegistry() 
//         { 
//             ::testing::UnitTest::GetInstance()->parameterized_test_registry(). GetTestCasePatternHolder<A>
//                 ( "A", ::testing::internal::CodeLocation( "gtest/gtest_Average.cpp", 70))->
//                 AddTestPattern( "A", "IsPrime", new ::testing::internal::TestMetaFactory< A_IsPrime_Test>()); 

//             return 0; 
//         } 
//         static int gtest_registering_dummy_ __attribute__ ((unused)); 
//         A_IsPrime_Test(A_IsPrime_Test const &) ; 
//         void operator=(A_IsPrime_Test const &) ; 
// }; 
// int A_IsPrime_Test::gtest_registering_dummy_ = A_IsPrime_Test::AddToRegistry(); 
// void A_IsPrime_Test::TestBody()
// {
//     bool out = GetParam().out;
//     int in = GetParam().in;
//     switch (0) 
//     case 0: 
//     default: 
//         if (const ::testing::AssertionResult gtest_ar = (::testing::internal:: EqHelper<(sizeof(::testing::internal::IsNullLiteralHelper(out)) == 1)>::Compare("out", "IsPrime(in)", out, IsPrime(in)))) ; 
//         else 
//             ::testing::internal::AssertHelper(::testing::TestPartResult::kNonFatalFailure, "gtest/gtest_Average.cpp", 74, gtest_ar.failure_message()) = ::testing::Message();
// }
// INSTANTIATE_TEST_CASE_P(P,
//     A,
//     Values(paramList{false, 10}, paramList{true, 3}));
// static ::testing::internal::ParamGenerator<A::ParamType> gtest_PA_EvalGenerator_() 
// { 
//     return Values(paramList{false, 10}, paramList{true, 3}); 
// } 
// static ::std::string gtest_PA_EvalGenerateName_( const ::testing::TestParamInfo<A::ParamType>& info) 
// { 
//     return ::testing::internal::GetParamNameGen<A::ParamType> ()(info); 
// } 
// static int gtest_PA_dummy_ __attribute__ ((unused)) = ::testing::UnitTest::GetInstance()->parameterized_test_registry(). GetTestCasePatternHolder<A>( "A", ::testing::internal::CodeLocation( "gtest/gtest_Average.cpp", 79))->AddTestCaseInstantiation( "P", &gtest_PA_EvalGenerator_, &gtest_PA_EvalGenerateName_, "gtest/gtest_Average.cpp", 79);
// --------------------<测试 MyString>---------------------------------
// 示例 2: 展示了一个具有多个函数方法的Class的测试方法
//
// 通常，为了保持结构的整洁，按照函数进行一对一测试。当然，你也可以添加其它测试
 
// 测试 默认构造函数
TEST(MyString, DefaultConstructor) {
    const MyString s;
 
    // 断言 s.c_string() 返回 NULL.
    //
    // 如果在此处我们用 static_cast<const char *>(NULL) 代替 NULL 的话，
    // 在gcc3.4版本上会产生Warning。原因是 EXPECT_EQ 为了在失败时打印结果
    // 需要知道它的参数类型。因为 NULL被定义为 0，编译器将会使用int的格式化
    // 函数打印它。但是，gcc认为 NULL是一个指针，而不是 int型，因而会发生告警。
    //
    // 问题的根本原因就是C++缺少整数0和NULL指针常量之间的区别。不幸的是，
    // 我们必须忍受这个事实。
    EXPECT_STREQ(NULL, s.c_string());
 
    EXPECT_EQ(0u, s.Length());
}
 
const char kHelloString[] = "Hello, world!";
 
// 测试 参数为C字符串的构造函数
TEST(MyString, ConstructorFromCString) {
    const MyString s(kHelloString);
    EXPECT_EQ(0, strcmp(s.c_string(), kHelloString));
    EXPECT_EQ(sizeof(kHelloString)/sizeof(kHelloString[0]) - 1,
        s.Length());
}
 
// 测试 拷贝构造函数
TEST(MyString, CopyConstructor) {
    const MyString s1(kHelloString);
    const MyString s2 = s1;
    EXPECT_EQ(0, strcmp(s2.c_string(), kHelloString));
}
 
// 测试 Set 方法
TEST(MyString, Set) {
    MyString s;
 
    s.Set(kHelloString);
    EXPECT_EQ(0, strcmp(s.c_string(), kHelloString));
 
    // 用Set方法对自身的字符串再拷贝一份，应该没有变化
    s.Set(s.c_string());
    EXPECT_EQ(0, strcmp(s.c_string(), kHelloString));
 
    // 测试我们是否可以将字符串设为NULL
    s.Set(NULL);
    EXPECT_STREQ(NULL, s.c_string());
}

// --------------------<测试 MyString>---------------------------------
// 示例 3: Google Test 一个更为高级的功能-测试夹具（test fixture）
//
// 所谓的“测试夹具”， 就是存贮测试用例中所有测试共享的对象和函数的地方。尤其是
// 初始化代码和清除代码，这样可以避免重复的拷贝、粘贴操作。
// 
// 所有的测试在代码共享的意义上共享“测试夹具”代码，而不是数据共享。每个测试都
// 拥有测试夹具的拷贝。在一个测试中修改的数据是不会传递到另一个测试中去的。
// 
// 这样设计的思路就是，所有的测试是独立的、可重复的。尤其是，当一个测试失败
// 不能影响另一个测试的结果。如果一个测试的结果依赖于另一个测试的结果，那么
// 这2个测试应该被合并为1个更大的测试。
//
// 指示测试成功/失败的宏（EXPECT_TRUE，FAIL等）需要知道当前测试是什么（
// Google Test打印测试结果的时候，告诉你每个失败都属于哪个测试。）从技术上讲，
// 这些宏调用Test类的成员函数。因此，为什么应该子测试放入一个测试夹具的原因。
// 
// 为了使用 TEST_F 测试夹具这个功能，必须从 testing::Test 派生一个类
class QueueTest : public testing::Test {
protected:  // 使用public也可
 
    // 每个测试开始之前，都必须运行这个函数。在这里，你完成相关变量的初始化
    virtual void SetUp() {
        q1_.Enqueue(1);
        q2_.Enqueue(2);
        q2_.Enqueue(3);
    }

    // 每个测试运行之后，virtual void TearDown()都会被调用。如果没有cleanup工作
    // 要处理，则不需要提供该函数。
    //
    // virtual void TearDown() {
    // }
 
    // 某些测试可能会使用的一个辅助函数
    static int Double(int n) {
        return 2*n;
    }
 
    // 测试 Queue::Map() 使用的一个辅助函数
    void MapTester(const Queue<int> * q) {
        // 创建一个新的队列，每个元素都是原有队列对应元素的2倍
        const Queue<int> * const new_q = q->Map(Double);
 
        // 验证新的队列和原队列大小相同
        ASSERT_EQ(q->Size(), new_q->Size());
 
        // 验证2个队列中的元素是否为2倍关系
        for ( const QueueNode<int> * n1 = q->Head(), * n2 = new_q->Head();
            n1 != NULL; n1 = n1->next(), n2 = n2->next() ) {
                EXPECT_EQ(2 * n1->element(), n2->element());
        }
 
        delete new_q;
    }
 
    // 声明测试想使用的变量
    Queue<int> q0_;
    Queue<int> q1_;
    Queue<int> q2_;
};
 
// 为了使用“测试夹具”功能，用宏 TEST_F 代替 TEST
// 测试构造函数
TEST_F(QueueTest, DefaultConstructor) {
    // 可以访问数据
    EXPECT_EQ(0u, q0_.Size());
}
 
// 测试 Dequeue().
TEST_F(QueueTest, Dequeue) {
    int * n = q0_.Dequeue();
    EXPECT_TRUE(n == NULL);
 
    n = q1_.Dequeue();
    ASSERT_TRUE(n != NULL);
    EXPECT_EQ(1, *n);
    EXPECT_EQ(0u, q1_.Size());
    delete n;
 
    n = q2_.Dequeue();
    ASSERT_TRUE(n != NULL);
    EXPECT_EQ(2, *n);
    EXPECT_EQ(1u, q2_.Size());
    delete n;
}
 
// 测试 Queue::Map() 函数
TEST_F(QueueTest, Map) {
    MapTester(&q0_);
    MapTester(&q1_);
    MapTester(&q2_);
}

/*----------------------<示例 5--共有Test Fixture>--------------------*/
// <<<示例 5>>>: 本例展示了如何从测试夹具派生子夹具来编写多个测试用例
//
// 在使用测试夹具的时候，指定了使用该测试夹具的测试用例名称。
// 因此，一个测试夹具只能给一个测试用例使用。
//
// 有时，不止一个测试用例可能需要相同或略有不同的测试夹具。例如，你想确保
// 所有的GUI库的测试不会泄露重要的系统资源，比如，字体和画笔等。在Google 
// Test中，你可以将这些相同的测试放到一个测试夹具中，每一个测试用例从该
// 测试夹具中继承，然后使用。
// 
// 我们称这个测试夹具为“QuickTest”。作为基类，且没有测试用例。
// 
// 测试用例使用派生自“QuickTest”的自测试夹具。
class QuickTest : public testing::Test {
protected:
    // 要记住，SetUp()函数在测试开始之前执行。在此，记录测试开始的时间。
    virtual void SetUp() {
        start_time_ = time(NULL);
    }
 
    // 当测试完成时，立即调用 TearDown 函数。在这儿，我们用其检测测试是否太慢
    virtual void TearDown() {
        // 获取测试完成时的时间
        const time_t end_time = time(NULL);
 
        // 断言时间不能超过5s
        EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long.";
    }
 
    // UTC 时间 (单位：s) 测试开始
    time_t start_time_;
};
 
// 从QuickTest派生出一个新的测试夹具类-IntegerFunctionTest。该测试类中所有的测试
// 就是要求测试时间不能太慢。
class IntegerFunctionTest : public QuickTest {
    // 因为我们的逻辑都在QuickTest中，所以在此为空，当然了，你也可以在此添加新的逻辑或共享对象
};
 
// 现在开始编写 IntegerFunctionTest 测试用例的测试点
 
// 测试 Factorial()
TEST_F(IntegerFunctionTest, Factorial) {
    // 测试负数的阶乘
    EXPECT_EQ(1, Factorial(-5));
    EXPECT_EQ(1, Factorial(-1));
    EXPECT_GT(Factorial(-10), 0);
 
    // 测试 0 的阶乘.
    EXPECT_EQ(1, Factorial(0));
 
    // 测试正数的阶乘
    EXPECT_EQ(1, Factorial(1));
    EXPECT_EQ(2, Factorial(2));
    EXPECT_EQ(6, Factorial(3));
    EXPECT_EQ(40320, Factorial(8));
}
 
// 测试 IsPrime()
TEST_F(IntegerFunctionTest, IsPrime) {
    // 测试 负 input.
    EXPECT_FALSE(IsPrime(-1));
    EXPECT_FALSE(IsPrime(-2));
    EXPECT_FALSE(IsPrime(INT_MIN));
 
    // 测试 小数字
    EXPECT_FALSE(IsPrime(0));
    EXPECT_FALSE(IsPrime(1));
    EXPECT_TRUE(IsPrime(2));
    EXPECT_TRUE(IsPrime(3));
 
    // 测试 正输入
    EXPECT_FALSE(IsPrime(4));
    EXPECT_TRUE(IsPrime(5));
    EXPECT_FALSE(IsPrime(6));
    EXPECT_TRUE(IsPrime(23));
}

// 下面一个测试用例（命名为"QueueTest"也需要执行时间快，所以从 QuickTest 
// 再派生一个新的测试夹具-QueueTest1

// QueueTest 的测试夹具除了具有QuickTest中的内容之外，还有一些逻辑和共享对象。
// 我们在QueueTest1中定义自己独有的测试内容。
class QueueTest1 : public QuickTest {
protected:
    virtual void SetUp() {
        // 首先，我们调用父测试夹具类中的SetUp函数(QuickTest).
        QuickTest::SetUp();
 
        // 其次，添加该测试夹具的一些额外的设置
        q1_.Enqueue(1);
        q2_.Enqueue(2);
        q2_.Enqueue(3);
    }
 
    // By default, TearDown() inherits the behavior of
    // QuickTest::TearDown().  As we have no additional cleaning work
    // for QueueTest, we omit it here.
    //
    // virtual void TearDown() {
    //   QuickTest::TearDown();
    // }
 
    Queue<int> q0_;
    Queue<int> q1_;
    Queue<int> q2_;
};
 
// 现在让我们为 QueueTest1 测试用例编写测试点吧。
// 测试默认构造函数
TEST_F(QueueTest1, DefaultConstructor) {
    EXPECT_EQ(0u, q0_.Size());
}
 
// 测试 Dequeue().
TEST_F(QueueTest1, Dequeue) {
    int* n = q0_.Dequeue();
    EXPECT_TRUE(n == NULL);
 
    n = q1_.Dequeue();
    EXPECT_TRUE(n != NULL);
    EXPECT_EQ(1, *n);
    EXPECT_EQ(0u, q1_.Size());
    delete n;
 
    n = q2_.Dequeue();
    EXPECT_TRUE(n != NULL);
    EXPECT_EQ(2, *n);
    EXPECT_EQ(1u, q2_.Size());
    delete n;
}
 
/*-------------------<TYPED_TEST 和 TYPED_TEST_P 宏>------------------*/
// <<<示例6>>>：此示例演示如何测试同一接口的多个实现的公共属性(又称接口测试)。
 
// 首先，我们创建了一些工厂函数来创建实现的实例。如果你的实现可以通过相同的方法
// 实现，可以跳过这一步。
template <class T>
PrimeTable* CreatePrimeTable();
 
template <>
PrimeTable* CreatePrimeTable<OnTheFlyPrimeTable>() 
{
    return new OnTheFlyPrimeTable;
}
 
template <>
PrimeTable* CreatePrimeTable<PreCalculatedPrimeTable>() 
{
    return new PreCalculatedPrimeTable(10000);
}
 
// 然后，我们创建一个测试夹具类模板
template <class T>
class PrimeTableTest : public testing::Test 
{
protected:
    // 构造函数使用T调用工厂方法创建素数表
    PrimeTableTest() : table_(CreatePrimeTable<T>()) {}
 
    virtual ~PrimeTableTest() { delete table_; }
 
    // 请注意，我们通过基类接口而不是实际的实现类来测试真正的实现。这对于
    // 保持测试接近于真实环境的场景来说非常重要，因为真正的实现也是通过
    // 基类接口实现的。

    PrimeTable* const table_;
};
 
#if 1   // GTEST_HAS_TYPED_TEST

using testing::Types;

// Google Test 提供了2种重用不同类型测试的方法。第1种成为"typed tests"，
// 如果你在写测试用例的时候，已经知道所有要测试的类型时使用。
// 
// 为了写类型测试用例，首先使用
// 
//      TYPED_TEST_CASE(TestCaseName, TypeList);
// 
// 声明它并制定参数类型。与 TEST_F 一样， TestCaseName 必须与测试夹具名称匹配。
// 
// 我们要测试的type列表
typedef Types<OnTheFlyPrimeTable, PreCalculatedPrimeTable> Implementations;
 
TYPED_TEST_CASE(PrimeTableTest, Implementations);
 
// 然后使用 TYPED_TEST(TestCaseName, TestName) 定义一个类型测试
// 使用方法与 TEST_F 类似
TYPED_TEST(PrimeTableTest, ReturnsFalseForNonPrimes) {
    // 
    // 在测试体内，可以通过 TypeParam 引用类型参数，且可以通过 TestFixture
    // 引用“夹具类”。在本示例中，我们不需要。
    //
    // 因为我们使用了模板，因此C++在引用夹具类的成员时需要显式地使用"this"指针。
    EXPECT_FALSE(this->table_->IsPrime(-5));
    EXPECT_FALSE(this->table_->IsPrime(0));
    EXPECT_FALSE(this->table_->IsPrime(1));
    EXPECT_FALSE(this->table_->IsPrime(4));
    EXPECT_FALSE(this->table_->IsPrime(6));
    EXPECT_FALSE(this->table_->IsPrime(100));
}
 
TYPED_TEST(PrimeTableTest, ReturnsTrueForPrimes) {
    EXPECT_TRUE(this->table_->IsPrime(2));
    EXPECT_TRUE(this->table_->IsPrime(3));
    EXPECT_TRUE(this->table_->IsPrime(5));
    EXPECT_TRUE(this->table_->IsPrime(7));
    EXPECT_TRUE(this->table_->IsPrime(11));
    EXPECT_TRUE(this->table_->IsPrime(131));
}
 
TYPED_TEST(PrimeTableTest, CanGetNextPrime) {
    EXPECT_EQ(2, this->table_->GetNextPrime(0));
    EXPECT_EQ(3, this->table_->GetNextPrime(2));
    EXPECT_EQ(5, this->table_->GetNextPrime(3));
    EXPECT_EQ(7, this->table_->GetNextPrime(5));
    EXPECT_EQ(11, this->table_->GetNextPrime(7));
    EXPECT_EQ(131, this->table_->GetNextPrime(128));
}
 
// 就是这样！Google Test将会为TYPED_TEST_CASE用例中指定的每一种类型，重复
// 进行TYPED_TEST测试。这样，我们就不需要重复定义多次了。
 
#endif  // GTEST_HAS_TYPED_TEST
 
// #if GTEST_HAS_TYPED_TEST_P
#if 1 

using testing::Types;
 
// 但是有时候，我们在写测试用例的时候还不知道所有要测试的类型。例如，
// 你是接口的实现者，由其他开发者实现具体的实现，但是，又想写一组测试程序保证
// 它们的实现满足基本的要求，而你又不知道具体的实现。
//
// 如何在不提交类型参数的情况下编写测试？“ type-parameterized tests ”可以
// 帮助你。这比 “ typed tests ”测试更为复杂，但是，作为回报，你可以得到一个
// 可以在许多开发环境中重复使用的测试模式，这是一个很大的胜利。
// 
// 下面让我们来看一下如何实现：
// 首先，定义一个测试夹具类模板。在这儿，我们只是重用之前定义的夹具类 PrimeTableTest：
template <class T>
class PrimeTableTest2 : public PrimeTableTest<T> {
};
 
// 然后，声明测试用例。参数是“测试夹具”的名称，通常也是测试用例的名称。
// _P后缀用于“参数化”或“模式”
TYPED_TEST_CASE_P(PrimeTableTest2);

// 接下来，使用TYPED_TEST_P(TestCaseName, TestName) 定义一个测试，与 TEST_F相似
TYPED_TEST_P(PrimeTableTest2, ReturnsFalseForNonPrimes) {
    EXPECT_FALSE(this->table_->IsPrime(-5));
    EXPECT_FALSE(this->table_->IsPrime(0));
    EXPECT_FALSE(this->table_->IsPrime(1));
    EXPECT_FALSE(this->table_->IsPrime(4));
    EXPECT_FALSE(this->table_->IsPrime(6));
    EXPECT_FALSE(this->table_->IsPrime(100));
}
 
TYPED_TEST_P(PrimeTableTest2, ReturnsTrueForPrimes) {
    EXPECT_TRUE(this->table_->IsPrime(2));
    EXPECT_TRUE(this->table_->IsPrime(3));
    EXPECT_TRUE(this->table_->IsPrime(5));
    EXPECT_TRUE(this->table_->IsPrime(7));
    EXPECT_TRUE(this->table_->IsPrime(11));
    EXPECT_TRUE(this->table_->IsPrime(131));
}
 
TYPED_TEST_P(PrimeTableTest2, CanGetNextPrime) {
    EXPECT_EQ(2, this->table_->GetNextPrime(0));
    EXPECT_EQ(3, this->table_->GetNextPrime(2));
    EXPECT_EQ(5, this->table_->GetNextPrime(3));
    EXPECT_EQ(7, this->table_->GetNextPrime(5));
    EXPECT_EQ(11, this->table_->GetNextPrime(7));
    EXPECT_EQ(131, this->table_->GetNextPrime(128));
}
 
// 类型参数化测试与类型测试相比，多了一步：注册测试实例，也就是枚举想要的测试目标
REGISTER_TYPED_TEST_CASE_P(
    PrimeTableTest2,  // 第一个参数是测试用例名称，接下来的参数是各个测试的名称
    ReturnsFalseForNonPrimes, ReturnsTrueForPrimes, CanGetNextPrime);

// 至此，测试pattern就完成了。但是，你还没有进行任何真正的测试，因为还没有说明
// 运行那种类型的测试。

// 要将抽象测试模式（Pattern）转换成真实的测试，需要使用类型列表对其进行实例化。
// 通常，这些测试模式会被定义在某一个.h文件中，任何人都可以#include并实例化它。
// 甚至可以在同一程序中多次实例化它。为了区分不同的实例，可以为每个实例取不同
// 的名称该名称将会成为测试用例名称的一部分，可以用于测试过滤器。

// 指定类型，使用列表的格式。在编写TYPED_TEST_P()之类的测试时不必定义它。
typedef Types<OnTheFlyPrimeTable, PreCalculatedPrimeTable>
    PrimeTableImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(OnTheFlyAndPreCalculated,    // 实例名称
    PrimeTableTest2,             // 测试用例名称
    PrimeTableImplementations);  // 类型列表
 
#endif  // GTEST_HAS_TYPED_TEST_P
 
/*-------------------------------------------------------------------------
                        <TEST_P 宏>-<值参数化>                             
--------------------------------------------------------------------------*/
// <<<示例7>>>： 该示例展示了怎样测试interface的不同实现的共同属性，运用的方法就是
// [值参数化测试-value-parameterized tests]。在这个测试用例中，每个测试点都有一个
// 参数，该参数是一个接口指针，指向要测试的具体接口实现。

#if 1 // GTEST_HAS_PARAM_TEST
 
using ::testing::TestWithParam;
using ::testing::Values;
 
// 作为一般规则，为了防止测试影响后面的测试，你应该为每个测试点创建和销毁测试对象
// 而不是重用它们。在该示例中，我们将会为PrimeTable对象定义一个简单的工厂函数。
// 我们将会在测试用例的SetUp()方法中实例化对象，在 TearDown() 方法中删除它们。
typedef PrimeTable* CreatePrimeTableFunc();
 
PrimeTable* CreateOnTheFlyPrimeTable() {
    return new OnTheFlyPrimeTable();
}
 
template <size_t max_precalculated>
PrimeTable* CreatePreCalculatedPrimeTable() {
    return new PreCalculatedPrimeTable(max_precalculated);
}
 
// 在测试夹具类中，构造函数，SetUp()，和 TearDown()等函数体内，你可以通过GetParam()
// 方法引用测试参数。在本例中，测试参数是一个工厂函数，调用它在SetUp()函数产生
// PrimeTable的实例。
class PrimeTableTest1 : public TestWithParam<CreatePrimeTableFunc*> {
public:
    virtual ~PrimeTableTest1() { delete table_; }
    virtual void SetUp() { table_ = (*GetParam())(); }
    virtual void TearDown() {
        delete table_;
        table_ = NULL;
    }
 
protected:
    PrimeTable* table_;
};
 
TEST_P(PrimeTableTest1, ReturnsFalseForNonPrimes) {
    EXPECT_FALSE(table_->IsPrime(-5));
    EXPECT_FALSE(table_->IsPrime(0));
    EXPECT_FALSE(table_->IsPrime(1));
    EXPECT_FALSE(table_->IsPrime(4));
    EXPECT_FALSE(table_->IsPrime(6));
    EXPECT_FALSE(table_->IsPrime(100));
}
 
TEST_P(PrimeTableTest1, ReturnsTrueForPrimes) {
    EXPECT_TRUE(table_->IsPrime(2));
    EXPECT_TRUE(table_->IsPrime(3));
    EXPECT_TRUE(table_->IsPrime(5));
    EXPECT_TRUE(table_->IsPrime(7));
    EXPECT_TRUE(table_->IsPrime(11));
    EXPECT_TRUE(table_->IsPrime(131));
}
 
TEST_P(PrimeTableTest1, CanGetNextPrime) {
    EXPECT_EQ(2, table_->GetNextPrime(0));
    EXPECT_EQ(3, table_->GetNextPrime(2));
    EXPECT_EQ(5, table_->GetNextPrime(3));
    EXPECT_EQ(7, table_->GetNextPrime(5));
    EXPECT_EQ(11, table_->GetNextPrime(7));
    EXPECT_EQ(131, table_->GetNextPrime(128));
}

// 为了运行<值参数化>-测试，你需要实例化它们，或者将他们绑定到用作测试参数的值列表。
// 你可以在不同的编译单元里实例它们，甚至实例化多次。
INSTANTIATE_TEST_CASE_P(
    OnTheFlyAndPreCalculated,
    PrimeTableTest1,
    Values(&CreateOnTheFlyPrimeTable, &CreatePreCalculatedPrimeTable<1000>));
 
#else
 
// 对于某些编译器来说，Google Test可能不支持值参数化。如果我们使用条件编译来编译
// 引用gtest_main库的所有代码，那么MSVC链接器根本不会链接该库，因此会发生错误
// (fatal error LNK1561: entry point must be defined)。这个dummy测试保证
// gtest_main被链接。
TEST(DummyTest, ValueParameterizedTestsAreNotSupportedOnThisPlatform) {}
 
#endif  // GTEST_HAS_PARAM_TEST

/*-------------------------------------------------------------------------
                        <全局变量测试>                             
--------------------------------------------------------------------------*/
// <<< 示例 8 >>>: 本例演示了如何测试依赖某些全局标志变量的代码。
// Combine() 帮助列举这些标志所有可能的组合，每个测试给出一个组合作为参数。
 
#if GTEST_HAS_COMBINE // GTEST_HAS_COMBINE
 
// 假设我们想要引入一个新的、更高级的PrimeTable实现，它结合了PrecalcPrimeTable的
// 速度和OnTheFlyPrimeTable的多样性。在这个类中，它实例化了2个成员PrecalcPrimeTable
// 和 OnTheFlyPrimeTable，根据情况使用更为合适的那一个。但是在低内存的条件下，
// 可以告诉它不要实例化PrecalcPrimeTable，而仅仅使用OnTheFlyPrimeTable
class HybridPrimeTable : public PrimeTable {
public:
    HybridPrimeTable(bool force_on_the_fly, int max_precalculated)
        : on_the_fly_impl_(new OnTheFlyPrimeTable),
        precalc_impl_(force_on_the_fly ? NULL :
        new PreCalculatedPrimeTable(max_precalculated)),
        max_precalculated_(max_precalculated) {}
    virtual ~HybridPrimeTable() {
        delete on_the_fly_impl_;
        delete precalc_impl_;
    }
 
    virtual bool IsPrime(int n) const {
        if (precalc_impl_ != NULL && n < max_precalculated_)
            return precalc_impl_->IsPrime(n);
        else
            return on_the_fly_impl_->IsPrime(n);
    }
 
    virtual int GetNextPrime(int p) const {
        int next_prime = -1;
        if (precalc_impl_ != NULL && p < max_precalculated_)
            next_prime = precalc_impl_->GetNextPrime(p);
 
        return next_prime != -1 ? next_prime : on_the_fly_impl_->GetNextPrime(p);
    }
 
private:
    OnTheFlyPrimeTable* on_the_fly_impl_;
    PreCalculatedPrimeTable* precalc_impl_;
    int max_precalculated_;
};
 
using ::testing::TestWithParam;
using ::testing::Bool;
using ::testing::Values;
using ::testing::Combine;
 
// 为了测试 HybridPrimeTable 的所有代码分支，我们必须使用 PreCalculatedPrimeTable
// 范围内、外的数字进行测试，还有 PreCalculatedPrimeTable 被禁止的情况。我们定义
// 一个测试夹具类，它接受不同的参数组合，从而实例化一个HybridPrimeTable实例。
class PrimeTableTest3 : public TestWithParam< testing::tuple<bool, int> > {
protected:
    virtual void SetUp() {
        // This can be written as
        //
        // bool force_on_the_fly;
        // int max_precalculated;
        // tie(force_on_the_fly, max_precalculated) = GetParam();
        //
        // once the Google C++ Style Guide allows use of ::std::tr1::tie.
        //
        bool force_on_the_fly = testing::get<0>(GetParam());
        int max_precalculated = testing::get<1>(GetParam());
        table_ = new HybridPrimeTable(force_on_the_fly, max_precalculated);
    }
    virtual void TearDown() {
        delete table_;
        table_ = NULL;
    }
    HybridPrimeTable* table_;
};
 
TEST_P(PrimeTableTest3, ReturnsFalseForNonPrimes) {
    // 在测试体内，你可以通过GetParam()函数引用测试参数。在本测试case中，测试的参数是
    // PrimeTable接口指针，我们可以直接使用。
    // 请注意，你也可以将他保存在测试夹具 PrimeTableTest3的 SetUp()方法或构造函数中，
    // 并在测试中使用其保存的副本
 
    EXPECT_FALSE(table_->IsPrime(-5));
    EXPECT_FALSE(table_->IsPrime(0));
    EXPECT_FALSE(table_->IsPrime(1));
    EXPECT_FALSE(table_->IsPrime(4));
    EXPECT_FALSE(table_->IsPrime(6));
    EXPECT_FALSE(table_->IsPrime(100));
}
 
TEST_P(PrimeTableTest3, ReturnsTrueForPrimes) {
    EXPECT_TRUE(table_->IsPrime(2));
    EXPECT_TRUE(table_->IsPrime(3));
    EXPECT_TRUE(table_->IsPrime(5));
    EXPECT_TRUE(table_->IsPrime(7));
    EXPECT_TRUE(table_->IsPrime(11));
    EXPECT_TRUE(table_->IsPrime(131));
}
 
TEST_P(PrimeTableTest3, CanGetNextPrime) {
    EXPECT_EQ(2, table_->GetNextPrime(0));
    EXPECT_EQ(3, table_->GetNextPrime(2));
    EXPECT_EQ(5, table_->GetNextPrime(3));
    EXPECT_EQ(7, table_->GetNextPrime(5));
    EXPECT_EQ(11, table_->GetNextPrime(7));
    EXPECT_EQ(131, table_->GetNextPrime(128));
}
 
// 为了运行值参数化测试，必须要实例化它们，或者把他们绑定到一个数值列表上，然后作为
// 测试参数。可以在不同的编译单元实例化，也可以实例化多次。

// 在这里，我们使用值列表实例化我们的测试参数。我们必须组合bool型标志的所有可能值
// 和一些有用的int值去测试。在这里我们选择了一个小数（1）和素数查找范围以内的数（10）。
// Combine()将会产生所有可能的组合。
INSTANTIATE_TEST_CASE_P(MeaningfulTestParameters,
    PrimeTableTest3,
    Combine(Bool(), Values(1, 10)));
 
#else
 
// 对于某些编译器来说，Google Test可能不支持值参数化。如果我们使用条件编译来编译
// 引用gtest_main库的所有代码，那么MSVC链接器根本不会链接该库，因此会发生错误
// (fatal error LNK1561: entry point must be defined)。这个dummy测试保证
// gtest_main被链接。
TEST(DummyTest, CombineIsNotSupportedOnThisPlatform) {}
 
#endif  // GTEST_HAS_COMBINE

#if GTEST_HAS_COMBINE
class funcTest : public testing::TestWithParam<testing::tuple<int, int>>
{
protected:
    virtual void SetUp()
    {
        a = testing::get<0>(GetParam());
        b = testing::get<1>(GetParam());
    }
    virtual void TearDown()
    {

    }

    int a;
    int b;
};

int func(int a, int b)
{
    return a+b;
}

TEST_P(funcTest, test0)
{
    std::cout << a << "+" << b << "=" << func(a,b) << std::endl;
    // eq（a+b, func(a,b));
}

INSTANTIATE_TEST_CASE_P(MyFuncTest,
    funcTest,
    Combine(Values(1,2,3), Values(1,2)));

#else

TEST(DummyTest, CombineIsNotSupportedOnThisPlatform) {std::cout << "***\n";}
 
#endif  // GTEST_HAS_COMBINE
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
 
// Tests some trivial cases.
TEST(IsPrimeTest, Trivial) {
    EXPECT_FALSE(IsPrime(0));
    EXPECT_FALSE(IsPrime(1));
    EXPECT_TRUE(IsPrime(2));
    EXPECT_TRUE(IsPrime(3));
}
 
// Tests positive input.
TEST(IsPrimeTest, Positive) {
    EXPECT_FALSE(IsPrime(4));
    EXPECT_TRUE(IsPrime(5));
    EXPECT_FALSE(IsPrime(6));
    EXPECT_FALSE(IsPrime(9));
    EXPECT_TRUE(IsPrime(11));
    EXPECT_TRUE(IsPrime(23));
}

// --------------------<测试 MyString>---------------------------------
// 示例 2: 展示了一个具有多个函数方法的Class的测试方法
//
// 通常，为了保持结构的整洁，按照函数进行一对一测试。当然，你也可以添加其它测试
 
// 测试构造函数
TEST(MyString, DefaultConstructor) {
    const MyString s;
 
    // Asserts that s.c_string() returns NULL.
    //
    // If we write NULL instead of
    //
    //   static_cast<const char *>(NULL)
    //
    // in this assertion, it will generate a warning on gcc 3.4.  The
    // reason is that EXPECT_EQ needs to know the types of its
    // arguments in order to print them when it fails.  Since NULL is
    // #defined as 0, the compiler will use the formatter function for
    // int to print it.  However, gcc thinks that NULL should be used as
    // a pointer, not an int, and therefore complains.
    //
    // The root of the problem is C++'s lack of distinction between the
    // integer number 0 and the null pointer constant.  Unfortunately,
    // we have to live with this fact.
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
 
    // Set should work when the input pointer is the same as the one
    // already in the MyString object.
    s.Set(s.c_string());
    EXPECT_EQ(0, strcmp(s.c_string(), kHelloString));
 
    // Can we set the MyString to NULL?
    s.Set(NULL);
    EXPECT_STREQ(NULL, s.c_string());
}
 
//Sample 4: another basic example of using Google Test
// Tests the Increment() method.
TEST(Counter, Increment) {
    Counter c;
 
    // EXPECT_EQ() evaluates its arguments exactly once, so they
    // can have side effects.
 
    EXPECT_EQ(0, c.Increment());
    EXPECT_EQ(1, c.Increment());
    EXPECT_EQ(2, c.Increment());
}

// --------------------<测试 MyString>---------------------------------
// 示例 3: Google Test 一个更为高级的功能-测试夹具（test fixture）
//
// A test fixture is a place to hold objects and functions shared by
// all tests in a test case.  Using a test fixture avoids duplicating
// the test code necessary to initialize and cleanup those common
// objects for each test.  It is also useful for defining sub-routines
// that your tests need to invoke a lot.
//
// The tests share the test fixture in the sense of code sharing, not
// data sharing.  Each test is given its own fresh copy of the
// fixture.  You cannot expect the data modified by one test to be
// passed on to another test, which is a bad idea.
//
// The reason for this design is that tests should be independent and
// repeatable.  In particular, a test should not fail as the result of
// another test's failure.  If one test depends on info produced by
// another test, then the two tests should really be one big test.
//
// The macros for indicating the success/failure of a test
// (EXPECT_TRUE, FAIL, etc) need to know what the current test is
// (when Google Test prints the test result, it tells you which test
// each failure belongs to).  Technically, these macros invoke a
// member function of the Test class.  Therefore, you cannot use them
// in a global function.  That's why you should put test sub-routines
// in a test fixture.
 
// 为了使用 TEST_F 测试夹具这个功能，必须从 testing::Test 派生一个类
class QueueTest : public testing::Test {
protected:  // 使用public也可
 
    // 每个测试开始之前，都必须运行这个函数。在这里，你完成相关变量的初始化
    virtual void SetUp() {
        q1_.Enqueue(1);
        q2_.Enqueue(2);
        q2_.Enqueue(3);
    }
 
    // virtual void TearDown() will be called after each test is run.
    // You should define it if there is cleanup work to do.  Otherwise,
    // you don't have to provide it.
    //
    // virtual void TearDown() {
    // }
 
    // 某些测试可能会使用的一个辅助函数
    static int Double(int n) {
        return 2*n;
    }
 
    // 测试 Queue::Map() 使用的一个辅助函数
    void MapTester(const Queue<int> * q) {
        // Creates a new queue, where each element is twice as big as the
        // corresponding one in q.
        const Queue<int> * const new_q = q->Map(Double);
 
        // Verifies that the new queue has the same size as q.
        ASSERT_EQ(q->Size(), new_q->Size());
 
        // Verifies the relationship between the elements of the two queues.
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
 
// 示例 5: 本例展示了如何从测试夹具派生子夹具来多个测试用例
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
    // Remember that SetUp() is run immediately before a test starts.
    // This is a good place to record the start time.
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
 
// We derive a fixture named IntegerFunctionTest from the QuickTest
// fixture.  All tests using this fixture will be automatically
// required to be quick.
class IntegerFunctionTest : public QuickTest {
    // We don't need any more logic than already in the QuickTest fixture.
    // Therefore the body is empty.
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
 
// The next test case (named "QueueTest") also needs to be quick, so
// we derive another fixture from QuickTest.
//
// The QueueTest test fixture has some logic and shared objects in
// addition to what's in QuickTest already.  We define the additional
// stuff inside the body of the test fixture, as usual.
class QueueTest1 : public QuickTest {
protected:
    virtual void SetUp() {
        // First, we need to set up the super fixture (QuickTest).
        QuickTest::SetUp();
 
        // Second, some additional setup for this fixture.
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
 
// Now, let's write tests using the QueueTest fixture.
 
// Tests the default constructor.
TEST_F(QueueTest1, DefaultConstructor) {
    EXPECT_EQ(0u, q0_.Size());
}
 
// Tests Dequeue().
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
// 示例6：此示例演示如何测试同一接口的多个实现的公共属性(又称接口测试)。
 
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
 
// That's it!  Google Test will repeat each TYPED_TEST for each type
// in the type list specified in TYPED_TEST_CASE.  Sit back and be
// happy that you don't have to define them multiple times.
 
#endif  // GTEST_HAS_TYPED_TEST
 
#if GTEST_HAS_TYPED_TEST_P
 
using testing::Types;
 
// Sometimes, however, you don't yet know all the types that you want
// to test when you write the tests.  For example, if you are the
// author of an interface and expect other people to implement it, you
// might want to write a set of tests to make sure each implementation
// conforms to some basic requirements, but you don't know what
// implementations will be written in the future.
//
// How can you write the tests without committing to the type
// parameters?  That's what "type-parameterized tests" can do for you.
// It is a bit more involved than typed tests, but in return you get a
// test pattern that can be reused in many contexts, which is a big
// win.  Here's how you do it:
 
// First, define a test fixture class template.  Here we just reuse
// the PrimeTableTest fixture defined earlier:
// 但是有时候，我们在写测试用例的时候还不知道所有要测试的类型。例如，
// 你是接口的实现者，由其他开发者实现具体的实现，但是，又想写一组测试程序保证
// 它们的实现满足基本的要求，而你又不知打具体的实现。
//
// 如何在不提交类型参数的情况下编写测试？“ type-parameterized tests ”可以
// 帮助你。这比 “ typed tests ”测试更为复杂，但是，作为回报，你可以得到一个
// 可以在许多开发环境中重复使用的测试模式，这是一个很大的胜利。
// 
// 下面让我们来看一下如何实现：
// 首先，定义一个 
template <class T>
class PrimeTableTest2 : public PrimeTableTest<T> {
};
 
// Then, declare the test case.  The argument is the name of the test
// fixture, and also the name of the test case (as usual).  The _P
// suffix is for "parameterized" or "pattern".
TYPED_TEST_CASE_P(PrimeTableTest2);
 
// Next, use TYPED_TEST_P(TestCaseName, TestName) to define a test,
// similar to what you do with TEST_F.
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
 
// Type-parameterized tests involve one extra step: you have to
// enumerate the tests you defined:
REGISTER_TYPED_TEST_CASE_P(
    PrimeTableTest2,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    ReturnsFalseForNonPrimes, ReturnsTrueForPrimes, CanGetNextPrime);
 
// At this point the test pattern is done.  However, you don't have
// any real test yet as you haven't said which types you want to run
// the tests with.
 
// To turn the abstract test pattern into real tests, you instantiate
// it with a list of types.  Usually the test pattern will be defined
// in a .h file, and anyone can #include and instantiate it.  You can
// even instantiate it more than once in the same program.  To tell
// different instances apart, you give each of them a name, which will
// become part of the test case name and can be used in test filters.
 
// The list of types we want to test.  Note that it doesn't have to be
// defined at the time we write the TYPED_TEST_P()s.
typedef Types<OnTheFlyPrimeTable, PreCalculatedPrimeTable>
    PrimeTableImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(OnTheFlyAndPreCalculated,    // Instance name
    PrimeTableTest2,             // Test case name
    PrimeTableImplementations);  // Type list
 
#endif  // GTEST_HAS_TYPED_TEST_P
 
// /*-----------------------------TEST_P macro--------------------------------*/
// //Sample 7: This sample shows how to test common properties of multiple
// // implementations of an interface (aka interface tests) using
// // value-parameterized tests. Each test in the test case has
// // a parameter that is an interface pointer to an implementation
// // tested.
 
// #if GTEST_HAS_PARAM_TEST
 
// using ::testing::TestWithParam;
// using ::testing::Values;
 
// // As a general rule, to prevent a test from affecting the tests that come
// // after it, you should create and destroy the tested objects for each test
// // instead of reusing them.  In this sample we will define a simple factory
// // function for PrimeTable objects.  We will instantiate objects in test's
// // SetUp() method and delete them in TearDown() method.
// typedef PrimeTable* CreatePrimeTableFunc();
 
// PrimeTable* CreateOnTheFlyPrimeTable() {
//     return new OnTheFlyPrimeTable();
// }
 
// template <size_t max_precalculated>
// PrimeTable* CreatePreCalculatedPrimeTable() {
//     return new PreCalculatedPrimeTable(max_precalculated);
// }
 
// // Inside the test body, fixture constructor, SetUp(), and TearDown() you
// // can refer to the test parameter by GetParam().  In this case, the test
// // parameter is a factory function which we call in fixture's SetUp() to
// // create and store an instance of PrimeTable.
// class PrimeTableTest1 : public TestWithParam<CreatePrimeTableFunc*> {
// public:
//     virtual ~PrimeTableTest1() { delete table_; }
//     virtual void SetUp() { table_ = (*GetParam())(); }
//     virtual void TearDown() {
//         delete table_;
//         table_ = NULL;
//     }
 
// protected:
//     PrimeTable* table_;
// };
 
// TEST_P(PrimeTableTest1, ReturnsFalseForNonPrimes) {
//     EXPECT_FALSE(table_->IsPrime(-5));
//     EXPECT_FALSE(table_->IsPrime(0));
//     EXPECT_FALSE(table_->IsPrime(1));
//     EXPECT_FALSE(table_->IsPrime(4));
//     EXPECT_FALSE(table_->IsPrime(6));
//     EXPECT_FALSE(table_->IsPrime(100));
// }
 
// TEST_P(PrimeTableTest1, ReturnsTrueForPrimes) {
//     EXPECT_TRUE(table_->IsPrime(2));
//     EXPECT_TRUE(table_->IsPrime(3));
//     EXPECT_TRUE(table_->IsPrime(5));
//     EXPECT_TRUE(table_->IsPrime(7));
//     EXPECT_TRUE(table_->IsPrime(11));
//     EXPECT_TRUE(table_->IsPrime(131));
// }
 
// TEST_P(PrimeTableTest1, CanGetNextPrime) {
//     EXPECT_EQ(2, table_->GetNextPrime(0));
//     EXPECT_EQ(3, table_->GetNextPrime(2));
//     EXPECT_EQ(5, table_->GetNextPrime(3));
//     EXPECT_EQ(7, table_->GetNextPrime(5));
//     EXPECT_EQ(11, table_->GetNextPrime(7));
//     EXPECT_EQ(131, table_->GetNextPrime(128));
// }
 
// // In order to run value-parameterized tests, you need to instantiate them,
// // or bind them to a list of values which will be used as test parameters.
// // You can instantiate them in a different translation module, or even
// // instantiate them several times.
// //
// // Here, we instantiate our tests with a list of two PrimeTable object
// // factory functions:
// INSTANTIATE_TEST_CASE_P(
//     OnTheFlyAndPreCalculated,
//     PrimeTableTest1,
//     Values(&CreateOnTheFlyPrimeTable, &CreatePreCalculatedPrimeTable<1000>));
 
// #else
 
// // Google Test may not support value-parameterized tests with some
// // compilers. If we use conditional compilation to compile out all
// // code referring to the gtest_main library, MSVC linker will not link
// // that library at all and consequently complain about missing entry
// // point defined in that library (fatal error LNK1561: entry point
// // must be defined). This dummy test keeps gtest_main linked in.
// TEST(DummyTest, ValueParameterizedTestsAreNotSupportedOnThisPlatform) {}
 
// #endif  // GTEST_HAS_PARAM_TEST
 
// // Sample 8: This sample shows how to test code relying on some global flag variables.
// // Combine() helps with generating all possible combinations of such flags,
// // and each test is given one combination as a parameter.
 
// #if GTEST_HAS_COMBINE
 
// // Suppose we want to introduce a new, improved implementation of PrimeTable
// // which combines speed of PrecalcPrimeTable and versatility of
// // OnTheFlyPrimeTable (see prime_tables.h). Inside it instantiates both
// // PrecalcPrimeTable and OnTheFlyPrimeTable and uses the one that is more
// // appropriate under the circumstances. But in low memory conditions, it can be
// // told to instantiate without PrecalcPrimeTable instance at all and use only
// // OnTheFlyPrimeTable.
// class HybridPrimeTable : public PrimeTable {
// public:
//     HybridPrimeTable(bool force_on_the_fly, int max_precalculated)
//         : on_the_fly_impl_(new OnTheFlyPrimeTable),
//         precalc_impl_(force_on_the_fly ? NULL :
//         new PreCalculatedPrimeTable(max_precalculated)),
//         max_precalculated_(max_precalculated) {}
//     virtual ~HybridPrimeTable() {
//         delete on_the_fly_impl_;
//         delete precalc_impl_;
//     }
 
//     virtual bool IsPrime(int n) const {
//         if (precalc_impl_ != NULL && n < max_precalculated_)
//             return precalc_impl_->IsPrime(n);
//         else
//             return on_the_fly_impl_->IsPrime(n);
//     }
 
//     virtual int GetNextPrime(int p) const {
//         int next_prime = -1;
//         if (precalc_impl_ != NULL && p < max_precalculated_)
//             next_prime = precalc_impl_->GetNextPrime(p);
 
//         return next_prime != -1 ? next_prime : on_the_fly_impl_->GetNextPrime(p);
//     }
 
// private:
//     OnTheFlyPrimeTable* on_the_fly_impl_;
//     PreCalculatedPrimeTable* precalc_impl_;
//     int max_precalculated_;
// };
 
// using ::testing::TestWithParam;
// using ::testing::Bool;
// using ::testing::Values;
// using ::testing::Combine;
 
// // To test all code paths for HybridPrimeTable we must test it with numbers
// // both within and outside PreCalculatedPrimeTable's capacity and also with
// // PreCalculatedPrimeTable disabled. We do this by defining fixture which will
// // accept different combinations of parameters for instantiating a
// // HybridPrimeTable instance.
// class PrimeTableTest3 : public TestWithParam< ::std::tr1::tuple<bool, int> > {
// protected:
//     virtual void SetUp() {
//         // This can be written as
//         //
//         // bool force_on_the_fly;
//         // int max_precalculated;
//         // tie(force_on_the_fly, max_precalculated) = GetParam();
//         //
//         // once the Google C++ Style Guide allows use of ::std::tr1::tie.
//         //
//         bool force_on_the_fly = ::std::tr1::get<0>(GetParam());
//         int max_precalculated = ::std::tr1::get<1>(GetParam());
//         table_ = new HybridPrimeTable(force_on_the_fly, max_precalculated);
//     }
//     virtual void TearDown() {
//         delete table_;
//         table_ = NULL;
//     }
//     HybridPrimeTable* table_;
// };
 
// TEST_P(PrimeTableTest3, ReturnsFalseForNonPrimes) {
//     // Inside the test body, you can refer to the test parameter by GetParam().
//     // In this case, the test parameter is a PrimeTable interface pointer which
//     // we can use directly.
//     // Please note that you can also save it in the fixture's SetUp() method
//     // or constructor and use saved copy in the tests.
 
//     EXPECT_FALSE(table_->IsPrime(-5));
//     EXPECT_FALSE(table_->IsPrime(0));
//     EXPECT_FALSE(table_->IsPrime(1));
//     EXPECT_FALSE(table_->IsPrime(4));
//     EXPECT_FALSE(table_->IsPrime(6));
//     EXPECT_FALSE(table_->IsPrime(100));
// }
 
// TEST_P(PrimeTableTest3, ReturnsTrueForPrimes) {
//     EXPECT_TRUE(table_->IsPrime(2));
//     EXPECT_TRUE(table_->IsPrime(3));
//     EXPECT_TRUE(table_->IsPrime(5));
//     EXPECT_TRUE(table_->IsPrime(7));
//     EXPECT_TRUE(table_->IsPrime(11));
//     EXPECT_TRUE(table_->IsPrime(131));
// }
 
// TEST_P(PrimeTableTest3, CanGetNextPrime) {
//     EXPECT_EQ(2, table_->GetNextPrime(0));
//     EXPECT_EQ(3, table_->GetNextPrime(2));
//     EXPECT_EQ(5, table_->GetNextPrime(3));
//     EXPECT_EQ(7, table_->GetNextPrime(5));
//     EXPECT_EQ(11, table_->GetNextPrime(7));
//     EXPECT_EQ(131, table_->GetNextPrime(128));
// }
 
// // In order to run value-parameterized tests, you need to instantiate them,
// // or bind them to a list of values which will be used as test parameters.
// // You can instantiate them in a different translation module, or even
// // instantiate them several times.
// //
// // Here, we instantiate our tests with a list of parameters. We must combine
// // all variations of the boolean flag suppressing PrecalcPrimeTable and some
// // meaningful values for tests. We choose a small value (1), and a value that
// // will put some of the tested numbers beyond the capability of the
// // PrecalcPrimeTable instance and some inside it (10). Combine will produce all
// // possible combinations.
// INSTANTIATE_TEST_CASE_P(MeaningfulTestParameters,
//     PrimeTableTest3,
//     Combine(Bool(), Values(1, 10)));
 
// #else
 
// // Google Test may not support Combine() with some compilers. If we
// // use conditional compilation to compile out all code referring to
// // the gtest_main library, MSVC linker will not link that library at
// // all and consequently complain about missing entry point defined in
// // that library (fatal error LNK1561: entry point must be
// // defined). This dummy test keeps gtest_main linked in.
// TEST(DummyTest, CombineIsNotSupportedOnThisPlatform) {}
 
// #endif  // GTEST_HAS_COMBINE

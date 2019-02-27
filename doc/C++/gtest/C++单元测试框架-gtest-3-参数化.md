<h1 id="0">0 目录</h1>

* [1 前言](#1)
* [2 旧的方案](#2)
* [3 使用参数化后的方案](#3)
* [4 类型参数化](#4)

<h1 id="1">1 前言</h1>

在设计测试案例时，经常需要考虑给被测函数传入不同的值的情况。我们之前的做法通常是写一个通用方法，然后编写在测试案例调用它。即使使用了通用方法，这样的工作也是有很多重复性的，程序员都懒，都希望能够少写代码，多复用代码。Google的程序员也一样，他们考虑到了这个问题，并且提供了一个灵活的参数化测试的方案。

<h1 id="2">2 旧的方案</h1>

首先，列出被测函数（在gtest的example1.cc中）：

    // 判定是否为质数，如果n是质数，则返回true
    bool IsPrime(int n) 
    {
        // 判断1: 小数
        if (n <= 1) return false;
     
        // 判断1: 偶数（2是质数）
        if (n % 2 == 0) return n == 2;
     
        // 现在 n 奇数且 n >= 3.
        // 从 3 开始， 用每一个奇数 i去整除n，直到 n < i^2
        for (int i = 3; ; i += 2) {
            if (i > n/i) break;
            // 否则，n如果被i整除，就不是质数
            if (n % i == 0) return false;
        }
     
        // n在范围（1，n）中没有整数因子，因此是素数。
        return true;
    }

编写测试用例：

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

注意，在上面的代码中，我们复制了很多次，假设要测试的情况很多怎么办？我们是否可以通过传递参数的方式解决呢？

<h1 id="3">3 使用参数化后的方案</h1>

    // --------------------<测试 参数化>-----------------------------------
    using namespace ::testing;

    struct paramList
    {
        bool    out;
        int     in;
    };

    class IsPrimeParamTest : public ::testing::TestWithParam<struct paramList>
    {

    };
    TEST_P(IsPrimeParamTest, IsPrime)
    {
        bool out = GetParam().out;
        int in = GetParam().in;
        EXPECT_EQ(out, IsPrime(in));   
    }

    INSTANTIATE_TEST_CASE_P(IsPrimeParamTest,
        IsPrimeParamTest,
        Values(paramList{false, 10}, paramList{true, 3}));

方案分析：

1. 首先，我们定义一个结构体 *paramList*，然后用它作为参数化的类型。使用结构体的原因是把入力和预期值一起传进去。
2. 从 *::testing::TestWithParam* 派生一个测试夹具类 *IsPrimeParamTest*。
3. 然后，使用 *TEST_P*，声明测试代码，如上所示，简洁了很多。
4. 现在，我们已经准备好了测试夹具 *IsPrimeParamTest* 和测试用例，那么，接下来我们就需要实例化这个测试用例。在实例化的过程中， 我们要告诉 *gtest* 要测试的范围：
        
       INSTANTIATE_TEST_CASE_P(prefix, test_case_name, generator, ...)

    第一个参数，是测试用例的前缀，可以任意取；

    第二个参数，是测试用例的名称，需要和之前定义的参数化的类名称相同，比如：IsPrimeParamTest；

    第三个参数，可以理解为参数生成器，上面的例子使用了 *testing::Values* 函数，它的参数是结构体 *paramList* （这里是自定义的）；

    最后一个可选参数允许用户指定一个函数或函子，用来基于测试参数产生用户测试名称后缀（_suffix）。这个函数必须接受一个类型为 *testing::TestParamInfo<class ParamType>* 的参数，并返回 *std::string* 类型的值。

    注意： 测试名称必须是非空，唯一，且只能包含ASCII的字母、数字或下划线。

Google Test 还提供了一系列的参数生成的函数：

| 函数 | 说明 |
| ------ | ------ |
| Range(begin, end[, step])                 | 2个字符串相同 |
| Values(v1, v2, ..., vN)                   | 2个字符串不同 |
| ValuesIn(container)和ValuesIn(begin, end) | 忽略大小写，2个字符串相同 |
| Bool()                                    | 取false和true两个值 |
| Combine(g1, g2, ..., gN)                  | 每次分别从g1,g2,..gN中取出一个值，组合成一个元组(Tuple)作为一个参数。

> 说明：
> Combine(g1, g2, ..., gN), 这个功能只在提供了 *tr1/tuple>头的系统中有效。gtest会自动去判断是否支持 *tr/tuple*，如果你的系统确实支持，而gtest判断错误的话，你可以重新定义宏GTEST_HAS_TR1_TUPLE=1。

<h1 id="4">4 类型参数化</h1>

## 类型化测试

首先，我们先来看一个类型化测试的示例，要测试的代码为：

    /****************************************************************************/
    /**
     * @brief       纯虚接口 PrimeTable 定义
     */
    /****************************************************************************/
    class PrimeTable {
    public:
        virtual ~PrimeTable() {}
     
        // 如果n是素数，则返回素数
        virtual bool IsPrime(int n) const = 0;

        // 返回大于 p 的最小素数；如果下一个素数在素数表之外，返回 -1；
        virtual int GetNextPrime(int p) const = 0;
    };

    /****************************************************************************/
    /**
     * @brief       实现 #1 （OnTheFlyPrimeTable）即时计算素数
     */
    /****************************************************************************/
    class OnTheFlyPrimeTable : public PrimeTable 
    {
    public:
        virtual bool IsPrime(int n) const 
        {
            if (n <= 1) return false;
     
            for (int i = 2; i*i <= n; i++) 
            {
                // 如果n被一个除1和自身之外的整数整除，则不是素数
                if ((n % i) == 0) return false;
            }
     
            return true;
        }
     
        virtual int GetNextPrime(int p) const 
        {
            for (int n = p + 1; n > 0; n++) 
            {
                if (IsPrime(n)) return n;
            }
     
            return -1;
        }
    };

    /****************************************************************************/
    /**
     * @brief       实现 #2 PreCalculatedPrimeTable 预先计算素数并将结果存储在数组中
     */
    /****************************************************************************/
    // 实现 #2 预先计算素数并将结果存储在数组中。
    class PreCalculatedPrimeTable : public PrimeTable 
    {
    public:
        // 'max' specifies the maximum number the prime table holds.
        explicit PreCalculatedPrimeTable(int max)
            : is_prime_size_(max + 1), is_prime_(new bool[max + 1]) {
                CalculatePrimesUpTo(max);
        }
        virtual ~PreCalculatedPrimeTable() { delete[] is_prime_; }
     
        virtual bool IsPrime(int n) const {
            return 0 <= n && n < is_prime_size_ && is_prime_[n];
        }
     
        virtual int GetNextPrime(int p) const {
            for (int n = p + 1; n < is_prime_size_; n++) {
                if (is_prime_[n]) return n;
            }
     
            return -1;
        }
     
    private:
        void CalculatePrimesUpTo(int max) {
            ::std::fill(is_prime_, is_prime_ + is_prime_size_, true);
            is_prime_[0] = is_prime_[1] = false;
     
            for (int i = 2; i <= max; i++) {
                if (!is_prime_[i]) continue;
     
                // Marks all multiples of i (except i itself) as non-prime.
                for (int j = 2*i; j <= max; j += i) {
                    is_prime_[j] = false;
                }
            }
        }
     
        const int is_prime_size_;
        bool* const is_prime_;
     
        // Disables compiler warning "assignment operator could not be generated."
        void operator=(const PreCalculatedPrimeTable& rhs);
    };

现在，我们来编写测试用例：

    // <<<示例>>>：此示例演示如何测试同一接口的多个实现的公共属性(又称接口测试)。
     
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
     

## 类型参数化测试

    /*-------------------------------------------------------------------------
                            <TEST_P 宏>-<值参数化>                             
    --------------------------------------------------------------------------*/
    // <<<示例>>>： 该示例展示了怎样测试interface的不同实现的共同属性，运用的方法就是
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

其它请参考<[C++单元测试框架-gtest-1-断言](https://blog.csdn.net/shenwanjiang111/article/details/87990504)>
其它请参考<[C++单元测试框架-gtest-2-事件机制](https://blog.csdn.net/shenwanjiang111/article/details/87990728)>

<div style="text-align: right"><a href="#0">回到顶部</a><a name="_label0"></a></div>
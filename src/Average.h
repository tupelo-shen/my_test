#ifndef __AVERAGE_H__
#define __AVERAGE_H__

#include <iostream>
#include <string.h>
#include <algorithm>

// 返回 n! (n 的阶乘)。对于负数 n, n! 约定为 1.
int Factorial(int n);
// 判定是否为质数，如果n是质数，则返回true。
bool IsPrime(int n);

// 一个简单的字符串类
class MyString
{
private:
    const char* c_string_;
    const MyString& operator=(const MyString& str);

public:
    // 克隆一个C字符串（'\0'结束符），使用new分配内存
    static const char* CloneCString(const char* a_c_string);

    // 默认构造函数，构造一个NULL字符串
    MyString() : c_string_(NULL) {}

    // 通过克隆一个C字符串（'\0'结束符），构造一个字符串
    explicit MyString(const char* a_c_string) : c_string_(NULL) {
        Set(a_c_string);
    }

    // 拷贝构造函数
    MyString(const MyString& string) : c_string_(NULL) {
        Set(string.c_string_);
    }

    // 析构函数。
    // MyString打算设计成一个final类，所以析构函数不需要是virtual。
    ~MyString() { delete[] c_string_; }

    // 从this MyClass类中取得C-字符串
    const char* c_string() const { return c_string_; }

    size_t Length() const {
        return c_string_ == NULL ? 0 : strlen(c_string_);
    }

    // 设置C-字符串
    void Set(const char* c_string);
};

// Queue 是一个单链表队列，元素类型 E 必须支持拷贝构造函数
template <typename E>
class Queue;

// QueueNode 是队列 Queue 中的一个节点，它包含一个类型为E的元素和
// 一个指向下一个节点的指针。
template <typename E>
class QueueNode {
    friend class Queue<E>;
 
public:
    // 获取节点中的元素
    const E& element() const { return element_; }
 
    // 获取队列Queue中的下一个节点
    QueueNode* next() { return next_; }
    const QueueNode* next() const { return next_; }
 
private:
    // 用一个给定的元素创建一个节点。指向下一个节点的指针设为NULL
    explicit QueueNode(const E& an_element) : element_(an_element), next_(NULL) {}
 
    // 在这儿，禁止默认赋值构造函数和拷贝构造函数
    const QueueNode& operator = (const QueueNode&);
    QueueNode(const QueueNode&);
 
    E element_;
    QueueNode* next_;
};
 
template <typename E>
class Queue {
public:
    // 创建一个空队列
    Queue() : head_(NULL), last_(NULL), size_(0) {}
 
    // 析构函数，销毁队列
    ~Queue() { Clear(); }
 
    // 清除队列
    void Clear() {
        if (size_ > 0) {
            // 1. 删除每一个节点
            QueueNode<E>* node = head_;
            QueueNode<E>* next = node->next();
            for (; ;) {
                delete node;
                node = next;
                if (node == NULL) break;
                next = node->next();
            }
 
            // 2. 重置成员变量
            head_ = last_ = NULL;
            size_ = 0;
        }
    }
 
    // 得到队列中的元素个数
    size_t Size() const { return size_; }
 
    // 得到队列中的第一个（Head）元素，如果队列为空，则返回NULL
    QueueNode<E>* Head() { return head_; }
    const QueueNode<E>* Head() const { return head_; }
 
    // 得到队列中的最后一个（End）元素，如果队列为空，则返回NULL
    QueueNode<E>* Last() { return last_; }
    const QueueNode<E>* Last() const { return last_; }

    // 从队尾向队列中添加一个元素。使用拷贝构造函数拷贝一份element，然后存贮到
    // 队列中。
    void Enqueue(const E& element) {
        QueueNode<E>* new_node = new QueueNode<E>(element);
 
        if (size_ == 0) {
            head_ = last_ = new_node;
            size_ = 1;
        } else {
            last_->next_ = new_node;
            last_ = new_node;
            size_++;
        }
    }
 
    // 移除队列的头并返回这个节点的元素，如果这个队列为空则返回NULL
    E* Dequeue() {
        if (size_ == 0) {
            return NULL;
        }
 
        const QueueNode<E>* const old_head = head_;
        head_ = head_->next_;
        size_--;
        if (size_ == 0) {
            last_ = NULL;
        }
 
        E* element = new E(old_head->element());
        delete old_head;
 
        return element;
    }
 
    // 对队列中的每一个元素都应用一个函数，且返回新队列的结果。原队列不会受影响。
    template <typename F>
    Queue* Map(F function) const {
        Queue* new_queue = new Queue();
        for (const QueueNode<E>* node = head_; node != NULL; node = node->next_) {
            new_queue->Enqueue(function(node->element()));
        }
 
        return new_queue;
    }
 
private:
    QueueNode<E>* head_;        // 队列的第一个节点
    QueueNode<E>* last_;        // 队列的最后一个节点
    size_t size_;               // 队列的元素个数.
 
    // 我们不允许复制队列
    Queue(const Queue&);
    const Queue& operator = (const Queue&);
};

/****************************************************************************/
/**
 * @brief       一个简单的单调递增计数器
 *
 * @author      Tupelo Shen
 * @date        2019-02-22
 *
 * @param       なし
 *
 * @return      なし
 */
/****************************************************************************/
class Counter 
{
private:
    int counter_;
 
public:
    // Creates a counter that starts at 0.
    Counter() : counter_(0) {}
 
    // Returns the current counter value, and increments it.
    int Increment();
 
    // Prints the current counter value to STDOUT.
    void Print() const;
};

/****************************************************************************/
/**
 * @brief       纯虚接口 PrimeTable 定义
 *
 * @author      Tupelo Shen
 * @date        2019-02-22
 *
 * @param       なし
 *
 * @return      なし
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
 *
 * @author      Tupelo Shen
 * @date        2019-02-22
 *
 * @param       なし
 *
 * @return      なし
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
 *
 * @author      Tupelo Shen
 * @date        2019-02-22
 *
 * @param       なし
 *
 * @return      なし
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

class Average
{
private:
    int             m_sum ;
    unsigned char   m_nbr ;
public:
    Average(){};
    Average(double, double nbr);
    ~Average(){};
	
    friend std::ostream & operator<<(std::ostream & out, Average & avg);
	Average & operator+=(int num);
};

#endif /* __AVERAGE_H__ */
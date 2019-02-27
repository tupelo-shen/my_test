#include <stdio.h>
#include "Average.h"
#include "Common.h"

// n的阶乘 n! ，对于负数 n，n!=1.
int Factorial(int n) 
{
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
 
    return result;
}


// 判定是否为质数，如果n是质数，则返回true
bool IsPrime(int n) {
    // Trivial case 1: small numbers
    if (n <= 1) return false;
 
    // Trivial case 2: 偶数（2是质数）
    if (n % 2 == 0) return n == 2;
 
    // Now, we have that n is odd and n >= 3.
 
    // Try to divide n by every odd number i, starting from 3
    for (int i = 3; ; i += 2) {
        // We only have to try i up to the squre root of n
        if (i > n/i) break;
 
        // Now, we have i <= n/i < n.
        // If n is divisible by i, n is not prime.
        if (n % i == 0) return false;
    }
 
    // n has no integer factor in the range (1, n), and thus is prime.
    return true;
}
 
// Clones a 0-terminated C string, allocating memory using new.
const char* MyString::CloneCString(const char* a_c_string) {
    if (a_c_string == NULL) return NULL;
 
    const size_t len = strlen(a_c_string);
    char* const clone = new char[ len + 1 ];
    memcpy(clone, a_c_string, len + 1);
 
    return clone;
}
 
// Sets the 0-terminated C string this MyString object
// represents.
void MyString::Set(const char* a_c_string) {
    // Makes sure this works when c_string == c_string_
    const char* const temp = MyString::CloneCString(a_c_string);
    delete[] c_string_;
    c_string_ = temp;
}
 
// Returns the current counter value, and increments it.
int Counter::Increment() {
    return counter_++;
}
 
// Prints the current counter value to STDOUT.
void Counter::Print() const {
    printf("%d", counter_);
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
Average::Average(double sum = 0, double nbr = 0) 
    : m_sum(sum), m_nbr(nbr)
{

}

std::ostream & operator<<(std::ostream &out, Average & avg)
{
    out << (static_cast<double>(avg.m_sum) / avg.m_nbr);

    return out;
}
Average & Average::operator+=(int num)
{
    m_sum += num;
    ++m_nbr;

    return *this;
}
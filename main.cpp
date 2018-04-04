#include <iostream>
#include <stdexcept>
 
class conststr
{
    const char* p;
    std::size_t sz;
public:
    template<std::size_t N>
    constexpr conststr(const char(&a)[N]) : p(a), sz(N - 1) {}
 
    constexpr char operator[](std::size_t n) const
    {
        return n < sz ? p[n] : throw std::out_of_range("");
    }
    constexpr std::size_t size() const { return sz; }
};
 
constexpr std::size_t countlower(conststr s, std::size_t n = 0, 
    std::size_t c = 0)
{
    return n == s.size() ? c :
           s[n] >= 'a' && s[n] <= 'z' ? countlower(s, n + 1, c + 1) :
                                        countlower(s, n + 1, c);
}
 
// 输出函数要求编译时计算的常量，测试用
template<int n>
struct constN
{
    constN() { std::cout << n << '\n'; }
};
 
int main()
{
    std::cout << "the number of lowercase letters in \"Hello, world!\" is ";
    constN<countlower("Hello, world!")>(); // implicitly converted to conststr
    constN<5>();
    unsigned int m = 5;
    constN<m>();
}

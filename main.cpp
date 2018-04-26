#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

int main()
{
    //const char *s = "0123456789012345678901234567890";
    char array[10] = {'0','1','2','3','4','5','6','7','8','9'};
    char *p;
    p = strchr(array, '9');
    printf("%ld\n", array);
    printf("%ld\n", p);
    printf("%d\n",*(p+1));
    SystemPause();
    return 0;
}
///**
// * @brief   getDpFromFormat関数
// *
// *  小数桁数の位置を取得する
// *
// * @param[in]   なし
// * @return      dp         小数桁数の位置
// */
//unsigned short dlgKeyboard::getDpFromFormat(void)
//{
//    unsigned short dp = 0;
//
//    for(int i=0; (m_info.format[i]!=0) && (i<(FORMAT_STR_SIZE-1));i++)
//    {
//        if(m_info.format[i] == '.')
//        {
//            dp = i+1;
//        }
//    }
//
//    return dp;
//}
#include <iostream>
 
// simple non-type template parameter
template<int N>
struct S { int a[N]; };
 
template<const char*>
struct S2 {};
 
// complicated non-type example
template
<
    char c, // integral type
    int (&ra)[5], // lvalue reference to object (of array type)
    int (*pf)(int), // pointer to function
    int (S<10>::*a)[10] // pointer to member object (of type int[10])
> struct Complicated
{
    // calls the function selected at compile time
    // and stores the result in the array selected at compile time
    void foo(char base)
    {
        ra[4] = pf(c - base);
    }
};
 
S2<"fail"> s2; // error: string literal cannot be used
char okay[] = "okay"; // static object with linkage
S2< &okay[0] > s2; // error: array element has no linkage
S2<okay> s2; // works
 
int a[5];
int f(int n) { return n; }
 
int main()
{
    S<10> s; // s.a is an array of 10 int
    s.a[9] = 4;
 
    Complicated<'2', a, f, &S<10>::a> c;
    c.foo('0');
 
    std::cout << s.a[9] << a[4] << '\n';
}
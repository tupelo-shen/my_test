#include <iostream>
#include <ctime>
#include "stl_math.h"
#include <string>
#include "EEPROM_Driver.h"

using namespace std;
using namespace ana_stl;

int error = -1;
int vals[10];

template<class T1, class T2>
void func1(T1&, T2&)
{

}

template<class T>
void func2(T, T)
{

}

string foo(){return "hello world"; };
void bar(const string &s){};
// time_t start_time;
// time_t end_time;

int& put(int n)
{
    if ( n >= 0 && n <= 9)
        return vals[n];
    else
    {
        cout << "subscript error";
        return error;
    }

}

int main(void)
{
    // start_time = time(NULL);
    // fibonacci(1);
    // end_time = time(NULL);
    // printf("time difference = %ld \n", end_time - start_time);

    // start_time = time(NULL);
    // fibonacci(10);
    // end_time = time(NULL);
    // printf("time difference = %ld \n", end_time - start_time);

    // start_time = time(NULL);
    // fibonacci(50);
    // end_time = time(NULL);
    // printf("time difference = %ld \n", end_time - start_time);
    
    // char str1[] = "abc";
    // char str2[] = "abc";

    // const char str3[] = "abc";
    // const char str4[] = "abc";

    // const char *str5 = "abc";
    // const char *str6 = "abc";

    // char *str7 = "abc";
    // char *str8 = "abc";

    // cout << ( str1 == str2 ) << endl;//0  分别指向各自的栈内存
    // cout << ( str3 == str4 ) << endl;//0  分别指向各自的栈内存
    // cout << ( str5 == str6 ) << endl;//1指向文字常量区地址相同
    // cout << ( str7 == str8 ) << endl;//1指向文字常量区地址相同

    string str = "hello world!";
    const string &str1 = str;
    bar(foo( ));
    bar(str1);

    int a[10], b[10], c[20];

    func1(a, b);
    func1(b, c);
    func2(a, b);
    func2(b, c);

    // test the put() function
    cout << vals[0] << endl; 
    cout << vals[9] << endl; 
    put(0)=10; //以put(0)函数值作为左值，等价于vals[0]=10; 
    put(9)=20; //以put(9)函数值作为左值，等价于vals[9]=20; 
    cout << vals[0] << endl; 
    cout << vals[9] << endl;  
    char test_data[10] = {'I',' ','L','O','V','E',' ','Y','O','U'}; 
    char test_data_1[10] = {0};
    DRV_EEPROM_WriteData(0x10, test_data, 10);
    DRV_EEPROM_ReadData(0x10,test_data_1, 1);
    DRV_EEPROM_WriteData(0x100, test_data, 10);
    cout << test_data_1 << endl;
    return 0;
}



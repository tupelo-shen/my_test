#if 0
#include <iostream>
 
struct A { double x; };
const A* a;
 
decltype(a->x) y;                           // y的类型是double
decltype((a->x)) z = y;                     // z的类型是const double& (左值表达式)
 
template<typename T, typename U>
auto add(T t, U u) -> decltype(t + u)       // 返回类型依赖模板参数//返回类型可以被推导（C++14之后）
{
    return t+u;
}
 
int main() 
{
    int i = 33;
    decltype(i) j = i * 2;
 
    std::cout << "i = " << i << ", " << "j = " << j << '\n';
 
    auto f = [](int a, int b) -> int
    {
        return a * b;
    };
 
    decltype(f) g = f;                      // lambda函数的类型是唯一且未命名的
    i = f(2, 2);
    j = g(3, 3);
 
    std::cout << "i = " << i << ", " << "j = " << j << '\n';
}
#endif

#if 0
#include <iostream>
#include <string>
#include <typeinfo>
 
struct Base {};                             // 非多态
struct Derived : Base {};
 
struct Base2 { virtual void foo() {} };     // 多态
struct Derived2 : Base2 {};
 
int main() {
    int                 myint       = 50;
    std::string         mystr       = "string";
    double*             mydoubleptr = nullptr;
 
    std::cout << "myint has type: " << typeid(myint).name() << '\n'
              << "mystr has type: " << typeid(mystr).name() << '\n'
              << "mydoubleptr has type: " << typeid(mydoubleptr).name() << '\n';
 
    // std::cout << myint是多态类型的泛左值表达式；它会被求值
    const std::type_info& r1 = typeid(std::cout << myint);
    std::cout << '\n' << "std::cout<<myint has type : " << r1.name() << '\n';
 
    // std::printf()不是多态类型的泛左值表达式；不会被求值
    const std::type_info& r2 = typeid(std::printf("%d\n", myint));
    std::cout << "printf(\"%d\\n\",myint) has type : " << r2.name() << '\n';
 
    // 非多态左值是一个static类型
    Derived d1;
    Base& b1 = d1;
    std::cout << "reference to non-polymorphic base: " << typeid(b1).name() << '\n';
 
    Derived2 d2;
    Base2& b2 = d2;
    std::cout << "reference to polymorphic base: " << typeid(b2).name() << '\n';
 
    try {
        // 解引用一个null指针： 对于非多态表达式是OK的
        std::cout << "mydoubleptr points to " << typeid(*mydoubleptr).name() << '\n'; 
        // 解引用一个null指针： 对于多态表达式不是OK的
        Derived2* bad_ptr = nullptr;
        std::cout << "bad_ptr points to... ";
        std::cout << typeid(*bad_ptr).name() << '\n';
    } catch (const std::bad_typeid& e) {
         std::cout << " caught " << e.what() << '\n';
    }
}
#endif
#if 1
#include <iostream>
#include <string.h>

#define SC_MS_CODE_STR_NUM              (128)
#define SC_MS_CODE_DELIMITER_DASH       ("-")
#define SC_MS_CODE_DELIMITER_SLASH      ("/")
#define SC_MODEL_FIRST_DELIMITER        (0)
#define SC_MODEL_END_DELIMITER          (0)
#define SC_SUFFIX1_FIRST_DELIMITER      (1)
#define SC_SUFFIX1_END_DELIMITER        (3)
#define SC_SUFFIX2_FIRST_DELIMITER      (4)
#define SC_SUFFIX2_END_DELIMITER        (8)
#define SC_SUFFIX3_FIRST_DELIMITER      (9)
#define SC_SUFFIX3_END_DELIMITER        (11)
#define SC_OPTIONS_FIRST_DELIMITER      (1)
#define SC_OPTIONS_END_DELIMITER        (5)

// const char* str = "FLXA402-A-B-AB-NN-NN-NA-WR-N-N-J-NN/UM/H6/SCT/CB4/K";
// const char* str = "FLXA402-A-B-AB-NN-NN-NA-WR-N-N-J-/UM/H6/SCT/CB4/K";
const char* str = "FLXA402-A-B-AB-NN-NN-NA-WR-/UM/H6/SCT/CB4/K";
void spritMsCodeStr(
    const char* str,
    char* str_buff,
    const char* delimiter,
    unsigned short first_delimiter_pos,
    unsigned short end_delimiter_pos
    )
{
    int len, i, j;
    char* ptr;
    char buff[SC_MS_CODE_STR_NUM+1];

    strncpy(buff, str, SC_MS_CODE_STR_NUM);
    buff[SC_MS_CODE_STR_NUM] = '\0';

    ptr = strtok(buff, delimiter);
    if(first_delimiter_pos == 0 && end_delimiter_pos == 0) {
        if(ptr != NULL)
        {
            // Code title
            len = strlen(ptr);
            for(i = 0; i < len; i++) {
                *str_buff++ = *ptr++;
            }
            *str_buff = '\0';
        }
    }else {
        // Code suffix
        for(i = 1; i <= end_delimiter_pos; i++) {
            ptr = strtok(NULL, delimiter);
            if(ptr != NULL) {
                if(first_delimiter_pos <= i) {
                    len = strlen(ptr);
                    *str_buff++ = *delimiter;
                    for(j=0; j < len; j++) {
                        *str_buff++ = *ptr++;
                    }
                }
            }
        }
        *str_buff = '\0';
    }
}

int main()
{
    //char ms_code_val[SC_MS_CODE_STR_NUM];
    char ms_code_title_suffix[SC_MS_CODE_STR_NUM];
    char ms_code_buff[SC_MS_CODE_STR_NUM];

    printf("ms_code_origin = %s\n", str);

    spritMsCodeStr(str, 
        ms_code_title_suffix, 
        SC_MS_CODE_DELIMITER_SLASH, 
        SC_MODEL_FIRST_DELIMITER, 
        SC_MODEL_END_DELIMITER);
    printf("ms_code_title_suffix = %s\n", ms_code_title_suffix);

    spritMsCodeStr(ms_code_title_suffix, 
        ms_code_buff, 
        SC_MS_CODE_DELIMITER_DASH, 
        SC_MODEL_FIRST_DELIMITER, 
        SC_MODEL_END_DELIMITER);
    printf("ms_code_title = %s\n", ms_code_buff);

    spritMsCodeStr(ms_code_title_suffix, 
        ms_code_buff, 
        SC_MS_CODE_DELIMITER_DASH, 
        SC_SUFFIX1_FIRST_DELIMITER, 
        SC_SUFFIX1_END_DELIMITER);
    printf("ms_code_suffix1 = %s\n", ms_code_buff);

    spritMsCodeStr(ms_code_title_suffix, 
        ms_code_buff, 
        SC_MS_CODE_DELIMITER_DASH, 
        SC_SUFFIX2_FIRST_DELIMITER, 
        SC_SUFFIX2_END_DELIMITER);
    printf("ms_code_suffix2 = %s\n", ms_code_buff);

    spritMsCodeStr(ms_code_title_suffix, 
        ms_code_buff, 
        SC_MS_CODE_DELIMITER_DASH, 
        SC_SUFFIX3_FIRST_DELIMITER, 
        SC_SUFFIX3_END_DELIMITER);
    printf("ms_code_suffix3 = %s\n", ms_code_buff);

    spritMsCodeStr(str, 
        ms_code_buff, 
        SC_MS_CODE_DELIMITER_SLASH, 
        SC_OPTIONS_FIRST_DELIMITER, 
        SC_OPTIONS_END_DELIMITER);
    printf("ms_code_options = %s\n", ms_code_buff);
}
#endif
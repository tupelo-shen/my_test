#include <stdio.h>
#include <string.h>
#if 0
unsigned short getDigitNum(char* str, unsigned short max_size)
{
    unsigned short effective_digit = 0;
    bool dot_appear = false;
    bool non0_appear = false;

    if (str == NULL || strlen(str) > max_size) return 0;

    for (int i = strlen(str) - 1; i >= 0; i--)
    {
        if (str[i] == '.') // 小数点
        {
            dot_appear = true;
        }
        else if (str[i] == '0') // 0数字
        {
            if (dot_appear || non0_appear) effective_digit++; // 整数部のゼロと、小数部有効数字（1から9）前のゼロは有効数字
        }
        else if (str[i] >= '1' && str[i] <= '9') // 0以外の数字
        {
            non0_appear = true;
            effective_digit++;
        }
    }

    return effective_digit;
}

#else
unsigned short getDigitNum(char* str, unsigned short max_num)
{
    unsigned short effective_digit = 0;
    bool dot_appear = false;
    bool non0_appear = false;

    if (str == NULL || strlen(str) > max_num) return 0;

    char* p = strchr(str, '.');
    if(p)   /* 小数時 */
    {
        for (int i = strlen(str) - 1; i >= 0; i--)
        {
            if (str[i] == '.') // 小数点
            {
                dot_appear = true;
            }
            else if (str[i] == '0') // 0数字
            {
                if (dot_appear || non0_appear) effective_digit++; // 整数部のゼロと、小数部有効数字（1から9）前のゼロは有効数字
            }
            else if (str[i] >= '1' && str[i] <= '9') // 0以外の数字
            {
                non0_appear = true;
                effective_digit++;
            }
        }
    }
    else    /* 整数時 */
    {
        if(str[0] == '-')  /* 負数時 */
        {
            effective_digit = strlen(str) - 1;
        }
        else
        {
            effective_digit = strlen(str);
        }
    }

    return effective_digit;
}
#endif

bool isOverMaxDigit(char* str, int max_size)
{

    unsigned short  digit_cnt = 0;

    // プロンプト上数字は単位より出力値に変換する
    if(strlen(str) != 0)
    {
            digit_cnt = getDigitNum(str, 255);
            printf("digit_cnt = %d\n", digit_cnt);
            printf("************\n");

        if (digit_cnt > max_size) return true;
    }

    return false;
}
// class BaseClass
// {

// }
// class DerivedClass : pulbic BaseClass
// {

// }

int main(int argc, char const *argv[])
{

double a = 15.5;
short b = 7;
short c = a/b;
short d = (int)a%b;

char str[][10] = {"China","Beijing"};
char *p = (char *)str;
printf("%s\n", p+10);
printf("c = %d; d = %d\n", c, d);

    return 0;
}
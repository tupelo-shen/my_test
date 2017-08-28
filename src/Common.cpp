/*
 * @Module Name：    Common.cpp 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2017-08-18
 */
#include <iostream>
#include <cmath>
#include "Common.h"

/*
 * the position of decimal point
 */
#define DP_SC_0                 (0)
#define DP_SC_1                 (1)
#define DP_SC_2                 (2)
#define DP_SC_3                 (3)

/****************************************************************************/
/**
 * @brief   SystemPause()
 *          wait for inputing an enter character by users.
 *
 * @author  
 * @date    
 *
 * @param[in]
 *
 * @return
 */
/****************************************************************************/
extern "C" void SystemPause(void)
{
    std::cin.clear();       // reset any error flags
    // ignore any characters in the input buffer until we find an enter character
    std::cin.ignore(32767, '\n');       
    std::cin.get();         // ger one more char from the user
}
/****************************************************************************/
/**
 * @brief       SCMakeDigitStrOfDP()
 *        
 *
 * @author  
 * @date    
 *
 * @param[in]   
 *
 * @return
 */
/****************************************************************************/
extern "C" void SCMakeDigitStrOfDP( unsigned short* vaule_str_u16, int dp, float value)
{
    // char    value_str[SC_DIGITAL_VALUE_STR_LEN+1];
    // char    value_str_dp[SC_DIGITAL_VALUE_STR_LEN+1];
    // int     value_i;

    // if (value >= 0.0f)
    // {
    //     value_i = static_cast<int>((value * powf(10.0f, static_cast<float>(dp))) + 0.5f);
    // }
    // else
    // {
    //     value_i = static_cast<int>((value * powf(10.0f, static_cast<float>(dp))) - 0.5f);
    // }
    // sprintf(value_str, "%d", value_i);
    // SCAppendDP(value_str, dp, value_str_dp);
    // SCNumStrToUTF16(vaule_str_u16, value_str_dp);
}
/****************************************************************************/
/**
 * @brief   SCGetDPCondResis()
 *          get decimal position
 *
 * @author  
 * @date    
 *
 * @param[in]   float   value       // float value
 *
 * @return      int     dp          // the position for decimal point
 */
/****************************************************************************/
extern "C" int SCGetDPCondResis(float value)
{
    int     dp = DP_SC_0;
    float   absValue = fabsf(value); 

    if(absValue < 10.0f) {
        dp = DP_SC_3;
    }
    else if(absValue < 100.0f) {
        dp = DP_SC_2;
    }
    else if(absValue < 1000.0f) {
        dp = DP_SC_1;
    }
    else {
        dp = DP_SC_0;
    }

    return (dp);
}





/*
 * @Module Name：    Common.h
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2017-08-18
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

void    SystemPause(void);
int     SCGetDPCondResis(float value);
void    SCMakeDigitStrOfDP(unsigned short* value_str_u16, int dp, float value);

#ifdef __cplusplus
}   /* extern C */
#endif

#endif /* _COMMON_H_ */
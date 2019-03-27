/*
 * @Module Name：    Common.h
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2017-08-18
 */
#ifndef _COM_CRC32CALCULATION_H_
#define _COM_CRC32CALCULATION_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned char   crc_value1;
    unsigned char   crc_value2;
    unsigned char   crc_value3;
    unsigned char   crc_value4;
} CRC32_VALUE;

void    calculateCRC32(unsigned char* data, unsigned long length, CRC32_VALUE* crc_value);

#ifdef __cplusplus
}   /* extern C */
#endif

#endif /* _COMMON_H_ */
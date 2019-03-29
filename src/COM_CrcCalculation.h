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

#define CRC_INIT    (0xFFFF)

typedef union
{
    unsigned short      hw;
    struct
    {
        unsigned char   b1;
        unsigned char   b2;
    } CRC16_VALUE_BTYES;
} CRC16_VALUE;

typedef union
{
    unsigned long   dw;
    struct
    {
        unsigned char b1;
        unsigned char b2;
        unsigned char b3;
        unsigned char b4;
    }us2b;

    struct
    {
        unsigned short us1;
        unsigned short us2;
    }us1b;
} DS_4BYTE_UNION_MODULE;

typedef struct
{
    unsigned char   crc_value1;
    unsigned char   crc_value2;
    unsigned char   crc_value3;
    unsigned char   crc_value4;
} CRC32_VALUE;

void            calculateCRC32(unsigned char* data, unsigned long length, CRC32_VALUE* crc_value);
unsigned short  calculateCRC16(unsigned char* data, unsigned long length, unsigned short old_crc_value);

#ifdef __cplusplus
}   /* extern C */
#endif

#endif /* _COMMON_H_ */
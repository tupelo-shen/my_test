/*
 * @Module Name：    Common.cpp 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2017-08-18
 */
#include "COM_Crc32Calculation.h"

/****************************************************************************/
/**
 * @brief   calculateCRC32()
 *          
 *
 * @author      Tupelo Shen
 * @date        2010-03-27
 *
 * @param[in]
 *
 * @return
 */
/****************************************************************************/
// extern "C" void calculateCRC32(unsigned char* data, unsigned long length, CRC32_VALUE* crc_value)
// {
//     unsigned char crc_value1;
//     unsigned char crc_value2;
//     unsigned char crc_value3;
//     unsigned char crc_value4;
//     unsigned long data_address;

//     // Check whether the pointer is NULL or not.
//     // To data pointer, may also be NULL, but don't check it.
//     if (crc_value == NULL)
//     {
//         return;
//     }

//     data_address = static_cast<unsigned long *>(data);

//     crc_value1 = crc_value->crc_value1;
//     crc_value2 = crc_value->crc_value2;
//     crc_value3 = crc_value->crc_value3;
//     crc_value4 = crc_value->crc_value4;

//     while (length--)
//     {

//     }
// }

extern "C" void calculateCRC32(unsigned char* data, unsigned long length, CRC32_VALUE* crc_value)
{
    unsigned char crc_value1;
    unsigned char crc_value2;
    unsigned char crc_value3;
    unsigned char crc_value4;
    unsigned long data_address;

    // Check whether the pointer is NULL or not.
    // To data pointer, may also be NULL, but don't check it.
    if (crc_value == NULL)
    {
        return;
    }

    data_address = static_cast<unsigned long *>(data);

    crc_value1 = crc_value->crc_value1;
    crc_value2 = crc_value->crc_value2;
    crc_value3 = crc_value->crc_value3;
    crc_value4 = crc_value->crc_value4;

    while (length--)
    {

    }
}
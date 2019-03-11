/*
 * EEPROM_Driver.cpp
 *
 * @author  30032183
 * @version R1.01.01
 * @date    2019/03/07
 */
#include <boost/filesystem.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "EEPROM_Driver.h"

using namespace std;
using namespace boost;

#define EEPROM_RAM_SIZE         0x2000

char eeprom_ram[EEPROM_RAM_SIZE] = {0};

// locate the position for the bin file that is used as EEPROM
const char* fake_eeprom_filename = "fake_eeprom.bin";
/******************************************************************************/
/**
 * @brief   DRV_EEPROM_Initialize
 *
 *              空の関数
 *
 * @param[in]   void
 * @return      0           成功
 */
/******************************************************************************/
int DRV_EEPROM_Initialize( void)
{
    return (0);
}
/******************************************************************************/
/**
 * @brief   DRV_EEPROM_WriteData
 *
 *              EEPROM　へデータを書込み
 *
 * @param[in]   addr        データの開始のEEPROMのアドレス
 * @param[in]   data        書込み待ちのデータのバッファー
 * @param[in]   num         書込み待ちのデータサイズ
 * @return      0           成功
 */
/******************************************************************************/
int DRV_EEPROM_WriteData(unsigned short addr , char* data, unsigned int num)
{
    for (unsigned short i = addr; i < (addr + num); i++)
    {
        eeprom_ram[i] = *(data++);
    }

    try
    {
        ofstream ofs;
        ofs.open(fake_eeprom_filename, ios::out|ios::binary|ios::trunc );
        if(!ofs)
        {
            throw "File open error!!!";
        }
        ofs.write(eeprom_ram, EEPROM_RAM_SIZE);
        ofs.close();
    }
    catch( ... )
    {
        cout << "File open error!!!" << endl;
    }


    return 0;
}

/******************************************************************************/
/**
 * @brief   DRV_EEPROM_ReadData
 *
 *              EEPROM　からデータを書込み
 *
 * @param[in]   addr        データの開始のEEPROMのアドレス
 * @param[out]  data        読み出しデータのバッファー
 * @param[in]   num         読み出しサイズ
 * @return      0           成功
 */
/******************************************************************************/
int DRV_EEPROM_ReadData(unsigned short addr , char* data, unsigned int num)
{
    try
    {
        if (filesystem::exists(fake_eeprom_filename))
        {
            ifstream ifs;
            ifs.open(fake_eeprom_filename, ios::binary);
            if(!ifs)
            {
                throw "File open error!!!";
            }
            ifs.read(eeprom_ram, EEPROM_RAM_SIZE);
            ifs.close();
        }
        else
        {
            cout << "File Not Found!!!";
        }
    }
    catch( ... )
    {
        cout << "File open error!!!" << endl;
    }

    for (unsigned short i = addr; i < (addr + num); i++)
    {
        *(data++) = eeprom_ram[i];
    }

    return 0;
}


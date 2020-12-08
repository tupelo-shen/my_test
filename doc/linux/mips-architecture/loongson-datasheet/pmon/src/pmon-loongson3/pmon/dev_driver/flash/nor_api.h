
/***************************************************************************
# *文件：nor_api.h
# *描述：关于norflash 驱动 头文件,函数声明及变量定义等
# *作者：李丽娜
# *修改：
# *日期：2020.06.03
 ****************************************************************************/

#ifndef _NOR_API_H_
#define _NOR_API_H_

#include <target/ls2k_types.h>
 
//16MBYTES

#define NORFLASH_ID								0xC84018
#define NOR_BLOCK_COUNT           				256       	// NORFlash总块数  每块64KB

#define NOR_FLASH_SIZE							0x1000000    //16MBYTES
#define NOR_FLASH_PAGE_NUMBYTES   				256      	// 1 页大小为 256B
#define NOR_FLASH_PAGES_NUM						0x10000   //共计0x10000页

#define NOR_FLASH_SECTOR_NUMBYTES   			4096		//每个扇区共计4096B
#define NOR_FLASH_SECTOR_NUM					4096     //共计有4096个扇区

//#define NOR_FLASH_PAGES_PER_BLOCK 				256       	// 1 块 有 64KB   256页  共计256块
#define NOR_FLASH_BLOCK_NUMBYTES  				0x10000    	// 1块大小为 64KB


#define START_ADDR								0xFFF000////////最后一个扇区的起始位置4096*4095
#define WRITE_LEN										0x04

 

INT8U norflash_init(VOID);
INT8U nor_flash_read(INT32U addr, INT8U *buffer, INT32U cnt);
INT8U nor_flash_program(INT32U addr, INT8U *buffer, INT32U cnt);
INT8U nor_flash_erase(INT32U addr,INT32U cnt);
INT8U nor_flash_sector_erase(INT32U addr,INT32U cnt);

INT8U DDRchkinfo_into_norflash(INT32U *pdata);

#endif  /*__NOR_API_H_*/
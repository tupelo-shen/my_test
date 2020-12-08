
/***************************************************************************
# *文件：ls2k_spi.h
# *描述：关于ls2k_spi的驱动头文件
# *作者：李丽娜
# *修改：
# *日期：2020.06.03
 ****************************************************************************/

#ifndef _SPI_NORFLASH_H
#define _SPI_NORFLASH_H


#include <target/ls2k_types.h>

#define SELECT_NOR_FLAG             	0                                //存在norflash

#define  NORFLASH_BUSY					0x01

typedef struct spiTransfer 
{
    INT8U *txBuf;   
    INT8U *rxBuf;   
    INT32U  txLen;   
    INT32U  rxLen;    
}SPI_TRANSFER;


VOID spi_init_flash(VOID);

INT32U spi_nor_id(VOID);
INT8U spi_nor_read(INT32U norAddr, INT32U len, INT8U *pbuf);
INT8U spi_nor_write(INT32U norAddr, INT32U len,INT8U *pbuf);
INT8U spi_nor_block_Erase(INT32U norAddr);
INT8U spi_nor_sector_Erase(INT32U norAddr);


#endif /*_SPI_NORFLASH_H*/

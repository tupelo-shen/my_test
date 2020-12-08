/***************************************************************************
# *文件：spi_norflash.c
# *描述：关于ls2k_spi的驱动头文件
# *作者：李丽娜
# *修改：
# *日期：2020.11.03
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#undef _KERNEL
#include <errno.h>
#include <pmon.h>

#include "target/spi_norflash.h"



#define SPI_DEBUG     0
#if SPI_DEBUG
#define	DEBUG(...) 	printf(__VA_ARGS__);
#else
#define	DEBUG(...) 
#endif


#define SPI_BASE  0x1fff0220
#define PMON_ADDR 0xa1000000
#define FLASH_ADDR 0x000000

#define SPCR      0x0
#define SPSR      0x1
#define FIFO	  0x2
#define TXFIFO    0x2
#define RXFIFO    0x2
#define SPER      0x3
#define PARAM     0x4
#define SOFTCS    0x5
#define PARAM2    0x6

#define RFEMPTY 1


//#define KSEG1_STORE8(addr,val)	 *(volatile char *)(0xffffffffa0000000 | addr) = val
//#define KSEG1_LOAD8(addr)	 *(volatile char *)(0xffffffffa0000000 | addr)

#define KSEG1_STORE8(addr,val)	 *(volatile INT8U *)(0xffffffffa0000000 | addr) = val
#define KSEG1_LOAD8(addr)	 *(volatile INT8U *)(0xffffffffa0000000 | addr) 


#define SET_SPI(addr,val)        KSEG1_STORE8(SPI_BASE+addr,val)
#define GET_SPI(addr)            KSEG1_LOAD8(SPI_BASE+addr)


//SPI controller module

static VOID spi_initwrite(VOID)
{ 
	INT8U d=0;
  	SET_SPI(SPSR, 0xc0); 
  	SET_SPI(PARAM, 0x40);             //espr:0100
  	SET_SPI(PARAM2,0x01); 
#ifdef LS2H_SPI_HIGHSPEED
 	SET_SPI(SPER, 0x04); //spre:01 
  	SET_SPI(SPCR, 0x51);
#else	
	d=4;

	//lln_spi debugcode
#if 0
	printf("SPER set = 0x%x\r\n",(0x00 | ((d>>2)&3)));//0x01
	printf("SPCR set = 0x%x\r\n",(0x50 | (d&3)));//0x50
#endif

	SET_SPI(SPER, 0x00 | ((d>>2)&3)); //spre:01 
	SET_SPI(SPCR, 0x50 | (d&3));
#endif
	SET_SPI(SOFTCS,0xff);
}

static VOID spi_initread(VOID)
{
  	SET_SPI(PARAM, 0x41);             //espr:0100
}

static INT8U spiDevXfer(INT8U flashtype, SPI_TRANSFER * transfer)
{
	INT8U sts = LGC_TRUE;
	INT32U txLen=0;
	INT32U rxLen=0;
	INT32U i=0;
	INT8U val;	
	txLen=transfer->txLen;
	rxLen=transfer->rxLen;	

	if(flashtype==SELECT_NOR_FLAG)
	{
		val = 0x01|0xee;
	}
	else
	{
		val = 0xdf;
	}
	
	SET_SPI(SOFTCS, val);
	
	while(txLen>0)
	{
		SET_SPI(TXFIFO,transfer->txBuf[i++]);
		while((GET_SPI(SPSR)) & RFEMPTY == RFEMPTY)
		{
			
		}
		GET_SPI(RXFIFO);
		
		txLen--;
	}	
	i=0;
	while(rxLen>0)
	{
		SET_SPI(TXFIFO,0x00);
		while((GET_SPI(SPSR))& RFEMPTY == RFEMPTY)
		{
		
		}
		transfer->rxBuf[i++]= GET_SPI(RXFIFO);

		//if((!transfer->rxBuf[i-1])&&(i>1130))	
		//printf("\r\n 0x%x 0x%x  0x%x\r\n",(i-1),transfer->rxBuf[i-1], GET_SPI(RXFIFO));
		
		rxLen--;
	}		
	SET_SPI(SOFTCS,0x11|0xee);		
    return sts;
}



/*NORFLASH driver start*/
INT8U read_statereg_nor(void)
{
	
	INT8U reg_val;
	INT8U cmd=0;
	do
	{
		cmd = 0x05;//get norflash features command
		
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));
		
		transInfo.txBuf = &cmd;
		transInfo.txLen = 1;
		transInfo.rxBuf = &reg_val;
		transInfo.rxLen = 1;
		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);		
	}while(0);
	
	return reg_val;
}


INT32U  spi_nor_id(void)
{
	INT8U reg_val;
	INT32U id;
	INT8U id_value[3]={0};
	INT8U cmd[1] ={0};	

	//lln_spi added debugcode
	spi_initwrite();
	
	reg_val = read_statereg_nor();
	while(reg_val&0x01 == NORFLASH_BUSY)//ç­‰å¾…æ“ä½œå®ŒçŠ¶æ€å¯„å­˜å™¨  bit0è¢«æ¸�?
	{
		//DEBUG("spi_nor_id reg_val=0x%x\r\n",reg_val);
		reg_val = read_statereg_nor();
	}
	

	do
	{
		cmd[0] = 0x9F;
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));
		
		transInfo.txBuf = &cmd;
		transInfo.txLen = 1;
		transInfo.rxBuf = &id_value[0];
		transInfo.rxLen = 3;
		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);		
	}while(0);	
	id=(id_value[0]<<16)|(id_value[1]<<8)|id_value[2];	
	
	DEBUG("spi_nor_id=0x%x\r\n",id);
	return id;
}

VOID spi_nor_we_dis(VOID)
{
	INT8U reg_val;
	INT8U value = 0x00;
	INT8U cmd[1] ={0};	
	
#if 0	
	reg_val = read_statereg_nor();
	while(reg_val&0x01 == NORFLASH_BUSY)//ç­‰å¾…æ“ä½œå®ŒçŠ¶æ€å¯„å­˜å™¨  bit0è¢«æ¸�?
	{
		reg_val = read_statereg_nor();
	}
#endif	

	do
	{
		cmd[0] = 0x04;
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));
		
		transInfo.txBuf = &cmd;
		transInfo.txLen = 1;
		transInfo.rxBuf = &value;
		transInfo.rxLen = 0;
		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);		
	}while(0);
}


VOID spi_nor_we_en(VOID)
{
	INT8U reg_val;
	INT8U value = 0x00;
	INT8U cmd[1] ={0};	

	//printf("spi_nor_we_en\r\n");
		
	reg_val = read_statereg_nor();
	while(reg_val&0x01 == NORFLASH_BUSY)
	{
		DEBUG("0000000\r\n");
		reg_val = read_statereg_nor();

	}
		
	
	do
	{
		cmd[0] = 0x06;
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));
		
		transInfo.txBuf = &cmd;
		transInfo.txLen = 1;
		transInfo.rxBuf = &value;
		transInfo.rxLen = 0;
		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);		
	}while(0);
}

//
INT8U spi_nor_read(INT32U norAddr, INT32U len, INT8U *pbuf)
{
	INT32U i;
	INT8U reg_val;
	INT8U cmd[5]={0};
	INT32U cmdAddr=0;	
	memset (cmd, 0, 5);		
	cmdAddr=norAddr;	

	spi_initwrite(); //need init spi

	reg_val = read_statereg_nor();
	while(reg_val&0x01 == NORFLASH_BUSY)
	{
		DEBUG("0000000\r\n");
		reg_val = read_statereg_nor();

	}

	do
	{
		cmd[0] = 0x03;
		cmd[1] = (unsigned char)(cmdAddr>>16);
		cmd[2] = (unsigned char)(cmdAddr>>8);
		cmd[3] = (unsigned char)(cmdAddr);
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));
		
		transInfo.txBuf = &cmd;
		transInfo.txLen = 4;
		transInfo.rxBuf = pbuf;
		transInfo.rxLen = len;
		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);
	}
	while(0);

	return LGC_TRUE;
}

//页编程命�? 要求必须以页为单位执行写
//扇区编程命令没有实现
//max 256bytes  
static INT8U spiNorWrBuf[260]={0};
INT8U spi_nor_write(INT32U norAddr, INT32U len,INT8U *pbuf)
{
	INT32U i;
	INT8U reg_val;
	INT8U cmd[5]={0};
	INT32U cmdAddr=0;
	INT8U value=0;	
	memset (cmd, 0, 5);

	//lln_spi debug 
	//spi_initwrite();
	//printf("norwrite\r\n");

	spi_nor_we_en();
	
		
	cmdAddr=norAddr;
	do
	{
		spiNorWrBuf[0] = 0x02;
		spiNorWrBuf[1] = (unsigned char)(cmdAddr>>16);
		spiNorWrBuf[2] = (unsigned char)(cmdAddr>>8);
		spiNorWrBuf[3] = (unsigned char)(cmdAddr);		
		memcpy(&spiNorWrBuf[4],pbuf,len);		
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));		
		transInfo.txBuf = &spiNorWrBuf[0];
		transInfo.txLen = len+4;
		transInfo.rxBuf = NULL;
		transInfo.rxLen = 0;		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);
	}while(0);
	
	//check program end
	reg_val = read_statereg_nor();
	while(reg_val & NORFLASH_BUSY)
	{
		reg_val = read_statereg_nor();
		//DEBUG("writing\r\n");
	}
	return LGC_TRUE;
			
}

//block erase   any addr inside this block is valid
//norAddr:   实际地址偏移   not block number
INT8U spi_nor_block_Erase(INT32U norAddr)
{
	INT32U i;
	INT8U reg_val;
	INT8U cmd[4]={0};
	INT32U cmdAddr=0;
	INT8U value=0;	
	memset (cmd, 0, 4);	

		//lln_spi debug 
	spi_initwrite();
	 DEBUG("norErase\r\n");


	reg_val = read_statereg_nor();
	while(reg_val&0x01 == NORFLASH_BUSY)
	{
		//DEBUG("spi_nor_id reg_val=0x%x\r\n",reg_val);
		reg_val = read_statereg_nor();
		
	}


	spi_nor_we_en();	
	cmdAddr = norAddr;	
	do
	{
		cmd[0] = 0xD8;
		cmd[1] = (unsigned char)(cmdAddr>>16);
		cmd[2] = (unsigned char)(cmdAddr>>8);
		cmd[3] = (unsigned char)(cmdAddr);
		
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));
		
		transInfo.txBuf = &cmd[0];
		transInfo.txLen = 4;
		transInfo.rxBuf = NULL;
		transInfo.rxLen = 0;
		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);
	}while(0);	
	
	//check erase end
	reg_val = read_statereg_nor();
	while(reg_val & NORFLASH_BUSY)
	{
		reg_val = read_statereg_nor();
		//DEBUG("Erasing\r\n");
	}
	return LGC_TRUE;
}


//noraddr :Any address inside the sector is a valid address
INT8U spi_nor_sector_Erase(INT32U norAddr)
{
	int i;
	INT8U reg_val;
	INT8U cmd[4]={0};
	INT32U cmdAddr=0;
	INT8U value=0;	
	memset (cmd, 0, 4);	

	//lln_spi debug 
	spi_initwrite();
	DEBUG("norsectorErase\r\n");

	reg_val = read_statereg_nor();
	while(reg_val&0x01 == NORFLASH_BUSY)
	{
		//printf("spi_nor_id reg_val=0x%x\r\n",reg_val);
		reg_val = read_statereg_nor();
		
	}

	spi_nor_we_en();	
	cmdAddr = norAddr;	
	do
	{
		cmd[0] = 0x20;
		cmd[1] = (unsigned char)(cmdAddr>>16);
		cmd[2] = (unsigned char)(cmdAddr>>8);
		cmd[3] = (unsigned char)(cmdAddr);
		
		SPI_TRANSFER	transInfo;
		memset(&transInfo,0,sizeof(SPI_TRANSFER));
		
		transInfo.txBuf = &cmd[0];
		transInfo.txLen = 4;
		transInfo.rxBuf = NULL;
		transInfo.rxLen = 0;
		
		spiDevXfer(SELECT_NOR_FLAG,&transInfo);
	}while(0);	
	//check erase end
	reg_val = read_statereg_nor();
	while(reg_val & NORFLASH_BUSY)//
	{
		reg_val = read_statereg_nor();
		//DEBUG("Erasing\r\n");
	}
	return LGC_TRUE;
}

/*NORFLASH driver end*/


VOID spi_init_flash(VOID)
{
	spi_initwrite();	
	spi_initread();

	//debugcode
#if 0
	printf("SPSR=0x%x\r\n",GET_SPI(SPSR));//0x05
	printf("PARAM=0x%x\r\n",GET_SPI(PARAM));//0x41
	printf("PARAM2=0x%x\r\n",GET_SPI(PARAM2));//0x01
	printf("SPER=0x%x\r\n",GET_SPI(SPER));//0x01
	printf("SPCR=0x%x\r\n",GET_SPI(SPCR));//0x50
#endif
}



//////////norflash_drv






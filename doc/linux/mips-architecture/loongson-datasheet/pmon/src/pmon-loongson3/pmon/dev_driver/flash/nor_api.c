
/***************************************************************************
# *文件：nor_api.c
# *描述：关于nor flash 驱动代码
# *作者：李丽娜
# *修改：
# *日期：2020.06.03
 ****************************************************************************/


#include <stdio.h>
#include <string.h>

#include "nor_api.h"
#include "target/spi_norflash.h"


#define NOR_DEBUG     0
#if NOR_DEBUG
#define	DEBUG(...) 	printf(__VA_ARGS__);

#else
#define	DEBUG(...) 

#endif

INT8U sectorbuffer[NOR_FLASH_SECTOR_NUMBYTES];
/********************API*****************/ 
/*--------------------------------------------------------------------------
# 函数：    norflash_init
# 函数描述：
# 返回值：0x55：错误          0xaa：正确
--------------------------------------------------------------------------*/
INT8U norflash_init(VOID)
{
	INT32U id;	
	spi_init_flash();
	
	id=spi_nor_id();
	if(id == NORFLASH_ID)
	{
		DEBUG("\r\nnor id OK!\r\n");
		return LGC_TRUE;
	}	
	else
	{
		DEBUG("\r\nnor id ERROR!\r\n");
		return LGC_FALSE;
	}	
}

/*--------------------------------------------------------------------------
# 函数：    nor_flash_read
# 函数描述：读flash
# 返回值：0x55：错误          0xaa：正确
# 机制：
#	1. 支持任意地址任意长度读
--------------------------------------------------------------------------*/
INT8U nor_flash_read(INT32U addr, INT8U *buffer, INT32U cnt)
{
	if((addr+cnt)>NOR_FLASH_SIZE)
		return LGC_FALSE;
	spi_nor_read(addr,cnt,buffer);
	return LGC_TRUE;
}

/*--------------------------------------------------------------------------
# 函数：    nor_flash_program
# 函数描述：写flash
# 写入flash地址addr：
#           写入数据缓存buffer：     
#           写入数据的长度cnt：     
# 返回值：0x55：错误          0xaa：正确
# 机制：
#	1. 按扇区进行写
--------------------------------------------------------------------------*/
INT8U nor_flash_programsector(INT32U addr, INT8U *buffer, INT32U cnt)
{
	
	INT8U ret = 0;
	INT32U i=0;

	INT32U write_addr=0;
	INT32U  countsize =0;
	INT8U *ptr;	
	//INT8U sectorbuffer[NOR_FLASH_SECTOR_NUMBYTES];
	INT32U sectorheadsize=0;
	INT32U sectorcopysize=0;
	INT32U sectorlastbytes = 0;
    	
		
	//当前扇区前面不需要写的部分
	//由于Nor flash目前最小以扇区为单位进行擦除，所以如果续写时是从页内某地址开始的，
        //则必须先把该扇区开始到这一列的所有数据先读出来，然后一起按页写入

	if(((addr+cnt)>NOR_FLASH_SIZE)||(buffer==NULL))  
			return LGC_FALSE;
			
	
	sectorheadsize = addr%NOR_FLASH_SECTOR_NUMBYTES;


	if(sectorheadsize>0)
	{	
		DEBUG("\r\nsectorheadsize0=0x%x 0x%x!\r\n",cnt,sectorheadsize);
		nor_flash_read((addr-sectorheadsize),sectorbuffer,sectorheadsize);	 
	}


	if(cnt+sectorheadsize<NOR_FLASH_SECTOR_NUMBYTES)
	{


		sectorlastbytes = NOR_FLASH_SECTOR_NUMBYTES - cnt - sectorheadsize;		
		nor_flash_read((addr+cnt+sectorheadsize),&sectorbuffer[cnt+sectorheadsize],sectorlastbytes);	 
		sectorcopysize = cnt;

		DEBUG("\r\nsectorheadsize1=0x%x 0x%x 0x%x!\r\n",cnt,sectorheadsize,sectorlastbytes);
	}
	else
	{
		sectorcopysize = NOR_FLASH_SECTOR_NUMBYTES - sectorheadsize;
	}
	if(cnt<NOR_FLASH_SECTOR_NUMBYTES)
	{
		ptr=sectorbuffer;
		memcpy((void *)&sectorbuffer[sectorheadsize],buffer,sectorcopysize);  	
	}
	else
	{
		ptr=buffer;
	}
	countsize = NOR_FLASH_SECTOR_NUMBYTES;
	write_addr = addr - sectorheadsize; 
	
	ret = spi_nor_sector_Erase(addr);
	if(ret==LGC_FALSE)//ok:0   error:-1
	{
		DEBUG("\r\nErase Failure!\r\n");
		return LGC_FALSE;
	}

	
	while(countsize > 0)
	{	
//lln_debug 20200609
#if 0
		printf("\r\n\n\n norwrstart =0x%x!\r\n",write_addr);

		for(i=0;i<NOR_FLASH_PAGE_NUMBYTES;i++)
		{
			if((i%16==0)&&(i!=0))
				printf("\r\n");
			printf("0x%x ",ptr[i]);

		}

		printf("\r\n");
#endif


		ret=spi_nor_write(write_addr,NOR_FLASH_PAGE_NUMBYTES,ptr);
		
		if(ret==LGC_FALSE)//ok:0   error:-1
		{
			DEBUG("\r\nProgram Failure!\r\n");
			return LGC_FALSE;
		}
			
        countsize -= NOR_FLASH_PAGE_NUMBYTES;         //字节数减少
        ptr += NOR_FLASH_PAGE_NUMBYTES;           //读取的数据缓存向后移动
       	write_addr+=NOR_FLASH_PAGE_NUMBYTES;

       	//debugcode    
		
	}
    return LGC_TRUE;
}

/*--------------------------------------------------------------------------
# 函数：    nor_flash_program
# 函数描述：写flash
# 写入flash地址write_addr：
#           写入数据缓存buffer：     
#           写入数据的长度cnt：     
# 返回值：0x55：错误          0xaa：正确
# 机制：
#	1. 支持多块写
#	2.目前默认块都是好的---没有检测跳过坏块
--------------------------------------------------------------------------*/
INT8U nor_flash_program(INT32U write_addr, INT8U *buffer, INT32U cnt)
{
	INT8U ret;
	INT32U numbytes;
	INT32U currentsectorbytes;
	//计算写的起始块号、页号、列号（1列一个字节）
	DEBUG("lln_spi test nor_flash_program start\r\n");
	
	if ((write_addr < 0) || ((write_addr+cnt) > NOR_FLASH_SIZE)||(buffer==NULL))           
    {
        return LGC_FALSE;
    }
	
	//起始地址是否为扇区对齐，如果不是，计算该扇区写数据长度
	currentsectorbytes = NOR_FLASH_SECTOR_NUMBYTES - write_addr % NOR_FLASH_SECTOR_NUMBYTES;
	
	if(write_addr % NOR_FLASH_SECTOR_NUMBYTES)//首次写不是从块首开始写
	{	
		if(cnt<currentsectorbytes)
		{
			numbytes = cnt;
		}
		else
		{
			numbytes = currentsectorbytes;
		}
	}
	else
	{
		if(cnt > NOR_FLASH_SECTOR_NUMBYTES) 
		{
			numbytes = NOR_FLASH_SECTOR_NUMBYTES;	
		}
		else
		{
			numbytes = cnt;
		}
	}
	DEBUG("firstnumbytes=0x%x\r\n",numbytes);	

	
	while(cnt>0)
	{
		DEBUG("writesectorstart:0x%x 0x%x\r\n",write_addr,numbytes);
		
		ret = nor_flash_programsector(write_addr,buffer,numbytes);
		
		if(ret==LGC_FALSE)
			return LGC_FALSE;
			
		
		cnt -= numbytes;
		buffer += numbytes;
		write_addr +=numbytes; 
		if(cnt > NOR_FLASH_SECTOR_NUMBYTES) //剩余最后一块
		{
			numbytes = NOR_FLASH_SECTOR_NUMBYTES;	
		}
		else
		{
			numbytes = cnt;
		}
	}
	return LGC_TRUE;
	
}

/*--------------------------------------------------------------------------
# 函数：    nor_flash_erase
# 函数描述：擦除flash     
# 返回值：0x55：错误          0xaa：正确
# 机制：
#  按块擦除
--------------------------------------------------------------------------*/
INT8U nor_flash_erase(INT32U addr,INT32U cnt)
{
	int blockbytes;
	if ((addr < 0) || (addr >= NOR_FLASH_SIZE))           
    {
        DEBUG("nor_flash_erase error\r\n");
        return LGC_FALSE;
    }
	
	if(addr % NOR_FLASH_BLOCK_NUMBYTES)
	{
		blockbytes = NOR_FLASH_BLOCK_NUMBYTES - addr % NOR_FLASH_BLOCK_NUMBYTES;
		if(cnt<blockbytes)
		{
			blockbytes = cnt;
		}
	}
	else
	{
		if(cnt > NOR_FLASH_BLOCK_NUMBYTES) 	
		{
			blockbytes = NOR_FLASH_BLOCK_NUMBYTES;	
		}
		else
		{
			blockbytes = cnt;
		}
	}
	
	
	while(cnt>0)
	{
		
		DEBUG("erase addr:0x%x\r\n",addr);
		spi_nor_block_Erase(addr);
		addr += blockbytes;
		cnt -= blockbytes;

		if(cnt > NOR_FLASH_BLOCK_NUMBYTES) //剩余最后一块
		{
			blockbytes = NOR_FLASH_BLOCK_NUMBYTES;	
		}
		else
		{
			blockbytes = cnt;
		}
	}
   return LGC_TRUE;
	
}


/*--------------------------------------------------------------------------
# 函数：    nor_flash_erase
# 函数描述：扇区擦除flash     
# 返回值：0x55：错误          0xaa：正确
# 机制：
#  按扇区擦除
--------------------------------------------------------------------------*/
INT8U nor_flash_sector_erase(INT32U addr,INT32U cnt)
{
	INT32U sectorbytes;
	if ((addr < 0) || (addr >= NOR_FLASH_SIZE))           
    {
        DEBUG("nor_flash_erase error\r\n");
        return LGC_FALSE;
    }
	
	if(addr % NOR_FLASH_SECTOR_NUMBYTES)
	{
		sectorbytes = NOR_FLASH_SECTOR_NUMBYTES - addr % NOR_FLASH_SECTOR_NUMBYTES;
		if(cnt<sectorbytes)
		{
			sectorbytes = cnt;
		}
	}
	else
	{
		if(cnt > NOR_FLASH_SECTOR_NUMBYTES) 	
		{
			sectorbytes = NOR_FLASH_SECTOR_NUMBYTES;	
		}
		else
		{
			sectorbytes = cnt;
		}
	}
	
	
	while(cnt>0)
	{
		
		DEBUG("erase addr:0x%x\r\n",addr);
		spi_nor_sector_Erase(addr);
		addr += sectorbytes;
		cnt -= sectorbytes;

		if(cnt > NOR_FLASH_BLOCK_NUMBYTES) //剩余最后一块
		{
			sectorbytes = NOR_FLASH_BLOCK_NUMBYTES;	
		}
		else
		{
			sectorbytes = cnt;
		}
	}
   return LGC_TRUE;
	
}

/*--------------------------------------------------------------------------
# 函数：    DDRchkinfo_into_norflash
# 函数描述：将检测结果写至norflash的最后一个扇区的前4个字节     
# 返回值：0x55：错误          0xaa：正确
# 机制：
--------------------------------------------------------------------------*/

INT8U DDRchkinfo_into_norflash(INT32U *pdata)
{
	INT32U readback = 0;
	INT8U ret=LGC_TRUE;
	
	
	ret = norflash_init();
	if(ret==LGC_FALSE)
	{
		DEBUG("spi init error\r\n");
		return LGC_FALSE;
	}
	
	if(nor_flash_erase(START_ADDR,WRITE_LEN)==LGC_FALSE)
	{
	
		DEBUG("flash erase error\r\n");
		return LGC_FALSE;
	}
	DEBUG("lln_spi erase0 ok\r\n");
	
	if(nor_flash_program(START_ADDR,((INT8U *)pdata),WRITE_LEN)==LGC_FALSE)
	{
		DEBUG("flash program error\r\n");
		return LGC_FALSE;
	}
	DEBUG("lln_spi program0 ok\r\n");
	
	
	if(nor_flash_read(START_ADDR,((INT8U *)(&readback)),WRITE_LEN)==LGC_FALSE)
	{
		DEBUG("flash read error.0\r\n");
		return LGC_FALSE;
	}
	DEBUG("lln_spi read000 ok=0x%x 0x%x\r\n",readback,*pdata);
		
	
	if(readback!=*pdata)
	{
		DEBUG("\r\n wrerror :=0x%x  \r\n",readback);
		return LGC_FALSE;
	}
	
	DEBUG("\r\n wrok \r\n");
	return LGC_TRUE;
		
}




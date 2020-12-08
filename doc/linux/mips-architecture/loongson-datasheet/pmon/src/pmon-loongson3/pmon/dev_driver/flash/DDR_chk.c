
/***************************************************************************
# *文件：DDR_CHK.c
# *描述：关于ddr检测代码
# *作者：李丽娜
# *修改：
# *日期：2020.11.03
 ****************************************************************************/
#include <pmon.h>
#include <stdio.h>
#include <string.h>
#include <target/ls2k_types.h>
#include "nor_api.h"


#define DEBUG           0
#if DEBUG
#define DEBUG_MSG       printf
#else
#define DEBUG_MSG
#endif

#define OS_CODE_BASEADDR_IN_DDR				0x80200000
//#define DDR_TEST32_SIZE						0x4000000     //64*4MBYTES=256MB

#define DDR_TEST32_SIZE						0x400000  //16MB

#define DDR_TEST16_SIZE						(DDR_TEST32_SIZE*2 )
#define DDR_TEST8_SIZE						(DDR_TEST32_SIZE*4 )

#define DDR_32CHK_OK						0xAA
#define DDR_16CHK_OK						0xAA00
#define DDR_8CHK_OK							0xAA0000
#define DDR_32CHK_ERROR						0x55
#define DDR_16CHK_ERROR						0x5500
#define DDR_8CHK_ERROR						0x550000


int DDR_32wr_chk(void)
{
	uint32_t i = 0;
	uint32_t data = 0;
	DEBUG_MSG("DDR_32wr_chk start\r\n");
	//1. write data into ddr
	DEBUG_MSG("DDR_chk start1\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{
		if(i%2==0)
		{		
			*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR  + i*4) = 0x5a5a5a5a;
		}
		else 
		{	
			*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR+ i*4) = 0xa5a5a5a5;
		}		
	}
	//1. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		if(i%2==0)
		{
			if(0x5a5a5a5a!=data)
			{
				DEBUG_MSG("DDR_chk error1 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
		else
		{
			if(0xa5a5a5a5!=data)	
			{
				DEBUG_MSG("DDR_chk error1 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
	}
	DEBUG_MSG("DDR_chk end1\r\n");
	
	
	//2. write data into ddr
	DEBUG_MSG("DDR_chk start2\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{				
		if(i%2==0)
		{		
			*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = 0xc3c3c3c3;
		}
		else 
		{	
			*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = 0x3c3c3c3c;
		}		
	}
	//2. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		if(i%2==0)
		{
			if(0xc3c3c3c3!=data)
			{
				DEBUG_MSG("DDR_chk error2 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
		else
		{
			if(0x3c3c3c3c!=data)	
			{
				DEBUG_MSG("DDR_chk error2 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
	}
	DEBUG_MSG("DDR_chk end2\r\n");
	
	//3. write data into ddr
	DEBUG_MSG("DDR_chk start3\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{						
		*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = 0x55555555;	
	}
	//3. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		
		if(0x55555555!=data)
		{
			DEBUG_MSG("DDR_chk error3 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
		
	}
	DEBUG_MSG("DDR_chk end3\r\n");
	
	//4. write data into ddr
	DEBUG_MSG("DDR_chk start4\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{						
		*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = 0xaaaaaaaa;	
	}
	//4. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		
		if(0xaaaaaaaa!=data)
		{
			DEBUG_MSG("DDR_chk error4 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end4\r\n");
	
	//5. write data into ddr
	DEBUG_MSG("DDR_chk start4\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{						
		*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = i;	
	}
	//5. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		if(i!=data)
		{
			DEBUG_MSG("DDR_chk error5 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
		
	}
	DEBUG_MSG("DDR_chk end5\r\n");
	
	//6. write data into ddr
	DEBUG_MSG("DDR_chk start6\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{						
		*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = 0x33333333;	
	}
	//6. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		
		if(0x33333333!=data)
		{
			DEBUG_MSG("DDR_chk error6 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end6\r\n");
	
	//7. write data into ddr
	DEBUG_MSG("DDR_chk start7\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{						
		*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = 0xcccccccc;	
	}
	//7. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		
		if(0xcccccccc!=data)
		{
			DEBUG_MSG("DDR_chk error7 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end7\r\n");
	
	//8. write data into ddr
	DEBUG_MSG("DDR_chk start8\r\n");
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{						
		*(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4) = 0x77777777;	
	}
	//8. chk readdata from ddr
	for(i=0;i<DDR_TEST32_SIZE;i++)
	{			
		data = *(uint32_t *)(OS_CODE_BASEADDR_IN_DDR + i*4);
		
		if(0x77777777!=data)
		{
			DEBUG_MSG("DDR_chk error8 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end8\r\n\n\n");	
	return 0;
}

int DDR_16wr_chk(void)
{
	uint32_t i = 0;
	uint16_t data = 0;
	DEBUG_MSG("DDR_16wr_chk start\r\n");
	//1. write data into ddr
	DEBUG_MSG("DDR_chk start1\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{
		if(i%2==0)
		{		
			*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR  + i*2) = 0x5a5a;
		}
		else 
		{	
			*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR+ i*2) = 0xa5a5;
		}		
	}
	//1. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		if(i%2==0)
		{
			if(0x5a5a!=data)
			{
				DEBUG_MSG("DDR_chk error1 : 0x%x   0x%04X\n", i,data);
				return -1;
			}
		}
		else
		{
			if(0xa5a5!=data)	
			{
				DEBUG_MSG("DDR_chk error1 : 0x%x   0x%04X\n", i,data);
				return -1;
			}
		}
	}
	DEBUG_MSG("DDR_chk end1\r\n");
	
	
	//2. write data into ddr
	DEBUG_MSG("DDR_chk start2\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{				
		if(i%2==0)
		{		
			*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = 0xc3c3;
		}
		else 
		{	
			*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = 0x3c3c;
		}		
	}
	//2. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		if(i%2==0)
		{
			if(0xc3c3!=data)
			{
				DEBUG_MSG("DDR_chk error2 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
		else
		{
			if(0x3c3c!=data)	
			{
				DEBUG_MSG("DDR_chk error2 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
	}
	DEBUG_MSG("DDR_chk end2\r\n");
	
	//3. write data into ddr
	DEBUG_MSG("DDR_chk start3\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{						
		*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = 0x5555;	
	}
	//3. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		
		if(0x5555!=data)
		{
			DEBUG_MSG("DDR_chk error3 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
		
	}
	DEBUG_MSG("DDR_chk end3\r\n");
	
	//4. write data into ddr
	DEBUG_MSG("DDR_chk start4\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{						
		*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = 0xaaaa;	
	}
	//4. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		
		if(0xaaaa!=data)
		{
			DEBUG_MSG("DDR_chk error4 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end4\r\n");
	
	//5. write data into ddr
	DEBUG_MSG("DDR_chk start4\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{						
		*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = (uint16_t)i;
	}
	//5. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		if((uint16_t)i!=data)
		{
			DEBUG_MSG("DDR_chk error5 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
		
	}
	DEBUG_MSG("DDR_chk end5\r\n");
	
	//6. write data into ddr
	DEBUG_MSG("DDR_chk start6\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{						
		*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = 0x3333;	
	}
	//6. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		
		if(0x3333!=data)
		{
			DEBUG_MSG("DDR_chk error6 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end6\r\n");
	
	//7. write data into ddr
	DEBUG_MSG("DDR_chk start7\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{						
		*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = 0xcccc;	
	}
	//7. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		
		if(0xcccc!=data)
		{
			DEBUG_MSG("DDR_chk error7 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end7\r\n");
	
	//8. write data into ddr
	DEBUG_MSG("DDR_chk start8\r\n");
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{						
		*(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2) = 0x7777;	
	}
	//8. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint16_t *)(OS_CODE_BASEADDR_IN_DDR + i*2);
		
		if(0x7777!=data)
		{
			DEBUG_MSG("DDR_chk error8 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end8\r\n\n\n");	
	return 0;
}

int DDR_8wr_chk(void)
{
	uint32_t i = 0;
	uint8_t data = 0;
	DEBUG_MSG("DDR_8wr_chk start\r\n");
	//1. write data into ddr
	DEBUG_MSG("DDR_chk start1\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{
		if(i%2==0)
		{		
			*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR  + i) = 0x5a;
		}
		else 
		{	
			*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR+ i) = 0xa5;
		}		
	}
	//1. chk readdata from ddr
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		if(i%2==0)
		{
			if(0x5a!=data)
			{
				DEBUG_MSG("DDR_chk error1 : 0x%x   0x%04X\n", i,data);
				return -1;
			}
		}
		else
		{
			if(0xa5!=data)	
			{
				DEBUG_MSG("DDR_chk error1 : 0x%x   0x%04X\n", i,data);
				return -1;
			}
		}
	}
	DEBUG_MSG("DDR_chk end1\r\n");
	
	
	//2. write data into ddr
	DEBUG_MSG("DDR_chk start2\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{				
		if(i%2==0)
		{		
			*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = 0xc3;
		}
		else 
		{	
			*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = 0x3c;
		}		
	}
	//2. chk readdata from ddr
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		if(i%2==0)
		{
			if(0xc3!=data)
			{
				DEBUG_MSG("DDR_chk error2 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
		else
		{
			if(0x3c!=data)	
			{
				DEBUG_MSG("DDR_chk error2 : 0x%x   0x%08X\n", i,data);
				return -1;
			}
		}
	}
	DEBUG_MSG("DDR_chk end2\r\n");
	
	//3. write data into ddr
	DEBUG_MSG("DDR_chk start3\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{						
		*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = 0x55;	
	}
	//3. chk readdata from ddr
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		
		if(0x55!=data)
		{
			DEBUG_MSG("DDR_chk error3 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
		
	}
	DEBUG_MSG("DDR_chk end3\r\n");
	
	//4. write data into ddr
	DEBUG_MSG("DDR_chk start4\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{						
		*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = 0xaa;	
	}
	//4. chk readdata from ddr
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		
		if(0xaa!=data)
		{
			DEBUG_MSG("DDR_chk error4 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end4\r\n");
	
	//5. write data into ddr
	DEBUG_MSG("DDR_chk start4\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{						
		*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = (uint8_t)i;	
	}
	//5. chk readdata from ddr
	for(i=0;i<DDR_TEST16_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		if((uint8_t)i!=data)
		{
			DEBUG_MSG("DDR_chk error5 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
		
	}
	DEBUG_MSG("DDR_chk end5\r\n");
	
	//6. write data into ddr
	DEBUG_MSG("DDR_chk start6\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{						
		*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = 0x33;	
	}
	//6. chk readdata from ddr
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		
		if(0x33!=data)
		{
			DEBUG_MSG("DDR_chk error6 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end6\r\n");
	
	//7. write data into ddr
	DEBUG_MSG("DDR_chk start7\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{						
		*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = 0xcc;	
	}
	//7. chk readdata from ddr
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		
		if(0xcc!=data)
		{
			DEBUG_MSG("DDR_chk error7 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end7\r\n");
	
	//8. write data into ddr
	DEBUG_MSG("DDR_chk start8\r\n");
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{						
		*(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i) = 0x77;	
	}
	//8. chk readdata from ddr
	for(i=0;i<DDR_TEST8_SIZE;i++)
	{			
		data = *(uint8_t *)(OS_CODE_BASEADDR_IN_DDR + i);
		
		if(0x77!=data)
		{
			DEBUG_MSG("DDR_chk error8 : 0x%x   0x%08X\n", i,data);
			return -1;
		}
	}
	DEBUG_MSG("DDR_chk end8\r\n\n\n");
	return 0;	
}


void DDR_chk(void)
{
	uint32_t chkinfo = 0;
	if(!DDR_32wr_chk())
	{
		chkinfo |= DDR_32CHK_OK;
	}
	else
	{
		chkinfo |= DDR_32CHK_ERROR;	
	}
	if(!DDR_16wr_chk())
	{
		chkinfo |= DDR_16CHK_OK;
	}
	else
	{
		chkinfo |= DDR_16CHK_ERROR;	
	}
	if(!DDR_8wr_chk())
	{
		chkinfo |= DDR_8CHK_OK;
	}
	else
	{
		chkinfo |= DDR_8CHK_ERROR;	
	}
	//return chkinfo;
	
	////////写检查结果到norflash

	//chkinfo = 0x11223344;  //debugcode
	
	if(LGC_TRUE == DDRchkinfo_into_norflash(&chkinfo))
	{
		printf("DDR_chk OK\r\n");
	}
	else
	{
		printf("DDR_chk ERROR\r\n");
	}

	
}


static const Cmd Cmds[] =
{
    {"MyCmds"},
    {"DDR_chk","",0,"DDR_chk ",DDR_chk,0,99,CMD_REPEAT},
	{0,0}
};

static void init_cmd __P((void)) __attribute__ ((constructor));
  
static void init_cmd()
{
	cmdlist_expand(Cmds,1);
}



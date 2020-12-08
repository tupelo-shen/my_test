/*****************************************************************************/
/*文件名称：timer.c					     									     */
/*文件说明：定时器操作接口													             */
/*运行平台：LS2K1000														         */
/*版权说明：Copyright(c) 2020 Beijing.Hollysys.Co.Ltd.All rights reserved    */
/*****************************************************************************/
/*版本：1.00		修订人：张朋东     		修订日期：	2020.06.01				         */
/*修订历史：																         */
/*		2020.6.01 - V1.00  张朋东      										 */
/*		            创建。   												     */
/*****************************************************************************/


/*****************************************************************************/
/*                    Include                                                */
/*****************************************************************************/
#include <pmon.h>
#include <stdio.h>
#include <linux/types.h>
#include "target/timer.h"


/*****************************************************************************/
/*                    Macro                                                  */
/*****************************************************************************/
#define read32(addr)     (*(volatile INT32U *)(addr))
#define write32(b,addr) ((*(volatile INT32U  *)(addr)) = (b))


/*****************************************************************************/
/* 函数名称： void drv_InitTimer(void)                                            */
/* 功能说明： 初始化定时器                                                              */
/* 参    数：    无                                                              */
/* 返 回 值： 无                                                                  */
/*****************************************************************************/
void drv_InitTimer(void)
{
    /*LS2K1000定时器频率为固定125MHZ,只有一种计数模式（向上计数），所以初始化
        不需要   做任何操作*/
}


/*****************************************************************************/
/* 函数名称： void drv_EnTimer(void)                                              */
/* 功能说明： 使能定时器                                                               */
/* 参    数：    无                                                              */
/* 返 回 值： 无                                                                  */
/*****************************************************************************/
void drv_EnTimer(void)
{
    write32(ENABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //主计时器计时且允许定时器产生中断
}


/*****************************************************************************/
/* 函数名称： void drv_DisTimer(void)                                             */
/* 功能说明： 关闭定时器                                                               */
/* 参    数：    无                                                              */
/* 返 回 值： 无                                                                  */
/*****************************************************************************/
void drv_DisTimer(void)
{
    write32(DISABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //主计时器停止计时且所有的定时器都不再产生中断
}


/*****************************************************************************/
/* 函数名称： void drv_ReloadTimer(void)                                          */
/* 功能说明： 复位定时器                                                               */
/* 参    数：    无                                                              */
/* 返 回 值： 无                                                                  */
/*****************************************************************************/
void drv_ReloadTimer(void)
{
    write32(DISABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //主计时器停止计时且所有的定时器都不再产生中断

    write32(0,LS2K_HPET_MAIN_COUNTER_REG_LOW);                        //主定时器计数值设为0

    write32(ENABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //主计时器计时且允许定时器产生中断
}


/*****************************************************************************/
/* 函数名称： INT32U drv_GetCurrentTime(void)                                     */
/* 功能说明： 复位定时器                                                               */
/* 参    数：    无                                                              */
/* 返 回 值： 当前定时器值                                                             */
/*****************************************************************************/
INT32U drv_GetCurrentTime(void)
{
    INT32U val = 0;

    val = read32(LS2K_HPET_MAIN_COUNTER_REG_LOW);

    return (val);
}

/*****************************************************************************/
/* 函数名称： void initTimer(void)                                                */
/* 功能说明： 初始化定时器                                                              */
/* 参    数：    无                                                              */
/* 返 回 值： 无                                                                  */
/*****************************************************************************/
void initTimer(void)  
{
    /*LS2K1000定时器频率为固定125MHZ,只有一种计数模式（向上计数），所以初始化
    不需要   做任何操作*/
    write32(ENABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //主计时器计时且允许定时器产生中断
}

/*****************************************************************************/
/* 函数名称： void settimer(TIMER *ptimer, INT32U ntime)                          */
/* 功能说明： 设置定时器超时时间,计时单位 ms                                                   */
/* 参    数：    ptimer-定时器指针                                                   */
/*            ntime-定时器超时时间 最大为30000                                         */
/* 返 回 值： OK 参数有效，延时完成  ERROR 参数无效                                           */
/*****************************************************************************/
/*****************************************************************************/
INT8U settimer(TIMER *ptimer, INT32U ntime)
{
    INT8U ret;
    if (ntime <= 30000)      //延时值最大为30000
    {
        ptimer->settime = ntime * TIMER_MUTIPLIER; 
        
        ret = OK;
    }
    else
    {
        ret = ERROR;       
    }
    
    return (ret);    
}

/*****************************************************************************/
/* 函数名称： void starttimer(TIMER *ptimer)                                      */
/* 功能说明： 启动主计时器计时且允许所有定时器产生中断                                                */
/* 参    数：    ptimer-定时器指针                                                   */
/* 返 回 值： 无                                                                  */
/*****************************************************************************/
void starttimer(TIMER *ptimer)
{
    ptimer->bstart = 1;
    ptimer->starttime = read32(TIMER_REG);
}

/*****************************************************************************/
/* 函数名称： void stoptimer(TIMER *ptimer)                                       */
/* 功能说明： 停止指定的定时器                                                            */
/* 参    数：    ptimer-定时器指针                                                   */
/* 返 回 值： 无                                                                  */
/*****************************************************************************/
void stoptimer(TIMER *ptimer)
{
    ptimer->bstart = 0;
    ptimer->starttime = 0;
}


/*****************************************************************************/
/* 函数名称： INT8U isTimeout(const TIMER *ptimer)                                */
/* 功能说明： 判断定时器是否超时                                                           */
/* 参    数：    ptimer-定时器指针                                                   */
/* 返 回 值： 1-超时，0-未超时                                                         */
/*****************************************************************************/
INT8U isTimeout(const TIMER *ptimer)
{
    INT32U curtime = 0;
    INT8U rtn = 0;
    
    if (ptimer->bstart != 0)   //软定时启动
    {
        curtime = read32(TIMER_REG);
        if (curtime > ptimer->starttime)        //计时器向上计数
        {
            if (curtime - ptimer->starttime >= ptimer->settime)
            {
                rtn = 1;
            }
            else
            {
                rtn = 0;
            }
        }
        else
        {
            if ((curtime + ((unsigned int)0xffffffff - ptimer->starttime)) >= 
                   ptimer->settime)
            {
                rtn = 1;
            }
            else
            {
                rtn = 0;
            }
        }
    }
    else
    {
        rtn = 0;
    }
    
    return(rtn);
}

/*****************************************************************************/
/* 函数名称： void delamsy(INT32U ntime)                                          */
/* 功能说明： 延时函数 ,计时单位 ms                                                       */
/* 参    数：    ntime-延时时间,最大为30000                                            */
/* 返 回 值： OK 参数有效，延时完成  ERROR 参数无效                                           */
/*****************************************************************************/
 INT8U delayms(INT32U ntime)
{
    TIMER delt;
    INT8U ret;
    
    if (ntime <= 30000)      //延时值最大为30000
    {
        settimer(&delt, ntime);
        starttimer(&delt);
        while (isTimeout(&delt) != 1)
        {
            ; 
        }
        
        ret = OK;
    }
    else
    {
        ret = ERROR;       
    }

    return (ret);
 }






void test_big_mem_82xx(unsigned int cnt)
{
	unsigned int len=0;
	unsigned char *buf1=(unsigned char *)0x82000000;
	unsigned char *buf2=(unsigned char *)0x86000000;
	unsigned int startTime = 0;
    unsigned int endTime   = 0;
    unsigned int diffTime  = 0;
	len = buf2-buf1;
	startTime = drv_GetCurrentTime();
	memcpy(buf1, buf2, len);
	endTime = drv_GetCurrentTime();
	diffTime = endTime - startTime;
	printf("cnt=%08x    start=%08x    end=%08x    len=%08x    Time =%dms \r\n",cnt,buf1,buf2,len,diffTime/125000);
}


void test_big_mem_02xx(unsigned int cnt)
{
	unsigned int len=0;
	unsigned char *buf1=(unsigned char *)0x02000000;
	unsigned char *buf2=(unsigned char *)0x06000000;
	unsigned int startTime = 0;
    unsigned int endTime   = 0;
    unsigned int diffTime  = 0;
	len = buf2-buf1;
	startTime = drv_GetCurrentTime();
	memcpy(buf1, buf2, len);
	endTime = drv_GetCurrentTime();
	diffTime = endTime - startTime;
	printf("cnt=%08x    start=%08x    end=%08x    len=%08x    Time =%dms \r\n",cnt,buf1,buf2,len,diffTime/125000);
}

void test_memcpy_time(void)
{
    unsigned int i=0;
	
	unsigned int startTime = 0;
    unsigned int endTime = 0;
    unsigned int diffTime = 0;
    
	drv_InitTimer();
    drv_EnTimer();

	for(i=0;i<16;i++)	
	{	
		test_big_mem_82xx(i);
	}

	for(i=0;i<16;i++)	
	{	
		test_big_mem_02xx(i);
	}
}


static const Cmd Cmds[] =
{
    {"MyCmds"},
    {"memcpytest","",0,"memcpytest",test_memcpy_time,0,99,CMD_REPEAT},

	{0,0}
};

static void init_cmd __P((void)) __attribute__ ((constructor));
  
static void init_cmd()
{
	cmdlist_expand(Cmds,1);
}


/***************************** END timer.c *********************************/

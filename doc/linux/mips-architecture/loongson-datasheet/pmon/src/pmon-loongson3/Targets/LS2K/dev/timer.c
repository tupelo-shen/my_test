/*****************************************************************************/
/*�ļ����ƣ�timer.c					     									     */
/*�ļ�˵������ʱ�������ӿ�													             */
/*����ƽ̨��LS2K1000														         */
/*��Ȩ˵����Copyright(c) 2020 Beijing.Hollysys.Co.Ltd.All rights reserved    */
/*****************************************************************************/
/*�汾��1.00		�޶��ˣ�����     		�޶����ڣ�	2020.06.01				         */
/*�޶���ʷ��																         */
/*		2020.6.01 - V1.00  ����      										 */
/*		            ������   												     */
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
/* �������ƣ� void drv_InitTimer(void)                                            */
/* ����˵���� ��ʼ����ʱ��                                                              */
/* ��    ����    ��                                                              */
/* �� �� ֵ�� ��                                                                  */
/*****************************************************************************/
void drv_InitTimer(void)
{
    /*LS2K1000��ʱ��Ƶ��Ϊ�̶�125MHZ,ֻ��һ�ּ���ģʽ�����ϼ����������Գ�ʼ��
        ����Ҫ   ���κβ���*/
}


/*****************************************************************************/
/* �������ƣ� void drv_EnTimer(void)                                              */
/* ����˵���� ʹ�ܶ�ʱ��                                                               */
/* ��    ����    ��                                                              */
/* �� �� ֵ�� ��                                                                  */
/*****************************************************************************/
void drv_EnTimer(void)
{
    write32(ENABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //����ʱ����ʱ������ʱ�������ж�
}


/*****************************************************************************/
/* �������ƣ� void drv_DisTimer(void)                                             */
/* ����˵���� �رն�ʱ��                                                               */
/* ��    ����    ��                                                              */
/* �� �� ֵ�� ��                                                                  */
/*****************************************************************************/
void drv_DisTimer(void)
{
    write32(DISABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //����ʱ��ֹͣ��ʱ�����еĶ�ʱ�������ٲ����ж�
}


/*****************************************************************************/
/* �������ƣ� void drv_ReloadTimer(void)                                          */
/* ����˵���� ��λ��ʱ��                                                               */
/* ��    ����    ��                                                              */
/* �� �� ֵ�� ��                                                                  */
/*****************************************************************************/
void drv_ReloadTimer(void)
{
    write32(DISABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //����ʱ��ֹͣ��ʱ�����еĶ�ʱ�������ٲ����ж�

    write32(0,LS2K_HPET_MAIN_COUNTER_REG_LOW);                        //����ʱ������ֵ��Ϊ0

    write32(ENABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //����ʱ����ʱ������ʱ�������ж�
}


/*****************************************************************************/
/* �������ƣ� INT32U drv_GetCurrentTime(void)                                     */
/* ����˵���� ��λ��ʱ��                                                               */
/* ��    ����    ��                                                              */
/* �� �� ֵ�� ��ǰ��ʱ��ֵ                                                             */
/*****************************************************************************/
INT32U drv_GetCurrentTime(void)
{
    INT32U val = 0;

    val = read32(LS2K_HPET_MAIN_COUNTER_REG_LOW);

    return (val);
}

/*****************************************************************************/
/* �������ƣ� void initTimer(void)                                                */
/* ����˵���� ��ʼ����ʱ��                                                              */
/* ��    ����    ��                                                              */
/* �� �� ֵ�� ��                                                                  */
/*****************************************************************************/
void initTimer(void)  
{
    /*LS2K1000��ʱ��Ƶ��Ϊ�̶�125MHZ,ֻ��һ�ּ���ģʽ�����ϼ����������Գ�ʼ��
    ����Ҫ   ���κβ���*/
    write32(ENABLE<<ENABLE_CNF,LS2K_HPET_GEN_CNFIG_REG_LOW);         //����ʱ����ʱ������ʱ�������ж�
}

/*****************************************************************************/
/* �������ƣ� void settimer(TIMER *ptimer, INT32U ntime)                          */
/* ����˵���� ���ö�ʱ����ʱʱ��,��ʱ��λ ms                                                   */
/* ��    ����    ptimer-��ʱ��ָ��                                                   */
/*            ntime-��ʱ����ʱʱ�� ���Ϊ30000                                         */
/* �� �� ֵ�� OK ������Ч����ʱ���  ERROR ������Ч                                           */
/*****************************************************************************/
/*****************************************************************************/
INT8U settimer(TIMER *ptimer, INT32U ntime)
{
    INT8U ret;
    if (ntime <= 30000)      //��ʱֵ���Ϊ30000
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
/* �������ƣ� void starttimer(TIMER *ptimer)                                      */
/* ����˵���� ��������ʱ����ʱ���������ж�ʱ�������ж�                                                */
/* ��    ����    ptimer-��ʱ��ָ��                                                   */
/* �� �� ֵ�� ��                                                                  */
/*****************************************************************************/
void starttimer(TIMER *ptimer)
{
    ptimer->bstart = 1;
    ptimer->starttime = read32(TIMER_REG);
}

/*****************************************************************************/
/* �������ƣ� void stoptimer(TIMER *ptimer)                                       */
/* ����˵���� ָֹͣ���Ķ�ʱ��                                                            */
/* ��    ����    ptimer-��ʱ��ָ��                                                   */
/* �� �� ֵ�� ��                                                                  */
/*****************************************************************************/
void stoptimer(TIMER *ptimer)
{
    ptimer->bstart = 0;
    ptimer->starttime = 0;
}


/*****************************************************************************/
/* �������ƣ� INT8U isTimeout(const TIMER *ptimer)                                */
/* ����˵���� �ж϶�ʱ���Ƿ�ʱ                                                           */
/* ��    ����    ptimer-��ʱ��ָ��                                                   */
/* �� �� ֵ�� 1-��ʱ��0-δ��ʱ                                                         */
/*****************************************************************************/
INT8U isTimeout(const TIMER *ptimer)
{
    INT32U curtime = 0;
    INT8U rtn = 0;
    
    if (ptimer->bstart != 0)   //��ʱ����
    {
        curtime = read32(TIMER_REG);
        if (curtime > ptimer->starttime)        //��ʱ�����ϼ���
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
/* �������ƣ� void delamsy(INT32U ntime)                                          */
/* ����˵���� ��ʱ���� ,��ʱ��λ ms                                                       */
/* ��    ����    ntime-��ʱʱ��,���Ϊ30000                                            */
/* �� �� ֵ�� OK ������Ч����ʱ���  ERROR ������Ч                                           */
/*****************************************************************************/
 INT8U delayms(INT32U ntime)
{
    TIMER delt;
    INT8U ret;
    
    if (ntime <= 30000)      //��ʱֵ���Ϊ30000
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

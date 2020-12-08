/*****************************************************************************/
/*文件名称：timer.h					     									     */
/*文件说明：定时器操作接口													             */
/*运行平台：LS2K1000														         */
/*版权说明：Copyright(c) 2020 Beijing.Hollysys.Co.Ltd.All rights reserved    */
/*****************************************************************************/
/*版本：1.00		修订人：张朋东      		修订日期：	2020.06.01				         */
/*修订历史：																         */
/*		2020.6.01 - V1.00  张朋东      										 */
/*		            创建。   												     */
/*****************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include "typedef.h"

#define HPET_MAINTIMER_CLOCK	  		125000000UL   //125MHZ

//HPET 寄存器基址0x1FE04000为hpet的寄存器物理基地址，需要完成地址映射后取虚拟地址
//#define LS2K_DEV2_REG_BASE		0x0BFE00000
//32位
#define LS2K_DEV2_REG_BASE     0xBFE00000

//#define LS2K_HPET_REG_BASE		0x1FE04000
//hpet控制器内部寄存器物理基地址 地址位[15:12]固定为4
#define LS2K_HPET_REG_BASE		(LS2K_DEV2_REG_BASE | (0x1<<14))  


//hpet 的 64bit寄存器地址偏移
// General Capabilities and ID Register
#define LS2K_HPET_GEN_CAP_REG_LOW               (LS2K_HPET_REG_BASE+0x00000000)
#define LS2K_HPET_GEN_CAP_REG_HIGH              (LS2K_HPET_REG_BASE+0x00000004)

//	General Configuration Register
#define LS2K_HPET_GEN_CNFIG_REG_LOW             (LS2K_HPET_REG_BASE+0x00000010)
#define LS2K_HPET_GEN_CNFIG_REG_HIGH			(LS2K_HPET_REG_BASE+0x00000014)

//	 General Interrupt Status Register
#define LS2K_HPET_GEN_INT_STAT_REG_LOW			(LS2K_HPET_REG_BASE+0x00000020)
#define LS2K_HPET_GEN_INT_STAT_REG_HIGH			(LS2K_HPET_REG_BASE+0x00000024)

//Main Counter Value Register
#define LS2K_HPET_MAIN_COUNTER_REG_LOW			(LS2K_HPET_REG_BASE+0x000000F0)
#define LS2K_HPET_MAIN_COUNTER_REG_HIGH			(LS2K_HPET_REG_BASE+0x000000F4)

//Timer 0 Configuration and Capability Register
#define LS2K_HPET_TIM0_CNFIGCAP_REG_LOW			(LS2K_HPET_REG_BASE+0x00000100)
#define LS2K_HPET_TIM0_CNFIGCAP_REG_HIGH		(LS2K_HPET_REG_BASE+0x00000104)

//Timer 0 Comparator Value Register
/* 非周期模式下：此值等于主计时器值，则产生中断（前提：中断使能），不会因为中断产生发生变化
   周期模式下：若相等，则产生中断，且比较器值将累加最后一次软件写入比较器的值。
   比如当时写入0x0123h，那么主计时器等于0x0123时，产生中断
   比较器的值被修改为0x246
   当主计时器值达到0x246时，产生另一个中断
   比较器值被硬件修改为0x369h*/
#define LS2K_HPET_TIM0_COMPA_VAL_REG_LOW		(LS2K_HPET_REG_BASE+0x00000108)
#define LS2K_HPET_TIM0_COMPA_VAL_REG_HIGH		(LS2K_HPET_REG_BASE+0x0000010C)

//Timer 1 Configuration and Capability Register
#define LS2K_HPET_TIM1_CNFIGCAP_REG_LOW			(LS2K_HPET_REG_BASE+0x00000120)
#define LS2K_HPET_TIM1_CNFIGCAP_REG_HIGH		(LS2K_HPET_REG_BASE+0x00000124)

//Timer 1 Comparator Value Register
#define LS2K_HPET_TIM1_COMPA_VAL_REG_LOW		(LS2K_HPET_REG_BASE+0x00000128)
#define LS2K_HPET_TIM1_COMPA_VAL_REG_HIGH		(LS2K_HPET_REG_BASE+0x0000012C)

//Timer 2 Configuration and Capability Register
#define LS2K_HPET_TIM2_CNFIGCAP_REG_LOW			(LS2K_HPET_REG_BASE+0x00000140)
#define LS2K_HPET_TIM2_CNFIGCAP_REG_HIGH		(LS2K_HPET_REG_BASE+0x00000144)

//Timer 2 Comparator Value Register
#define LS2K_HPET_TIM2_COMPA_VAL_REG_LOW		(LS2K_HPET_REG_BASE+0x00000148)
#define LS2K_HPET_TIM2_COMPA_VAL_REG_HIGH		(LS2K_HPET_REG_BASE+0x0000014C)

//每个定时器的寄存器占用地址空间
#define	HPET_TIM_REG_SIZE							0x20	


//位偏移
//	General Configuration Register
#define ENABLE_CNF					0	// 主计时器使能位
// General Capabilities and ID Register
#define COUNT_SIZE_CAP				13  //  主计数器模式32or64bit
#define NUM_TIM_CAP					8	//  8~12bit

//TimerX Configuration and Capability Register
#define Tn_32MODE_CNF				8    //32bit模式 固定为0  只读
#define Tn_VAL_SET_CNF				6     //timer0下该位有效，通过对该位写1，软件能直接修改周期性定时器的累加器，无需清零操作
//   此位设置为1则使能定时器周期中断
#define Tn_TYPE_CNF					3  //定时器产生周期中断使能标志位  bit4=0（定时器不能产生周期中断标志） 则此位为0且只读  bit4=1，则此位可设置  

#define Tn_INT_ENB_CNF				2  //使能定时器产生中断
#define Tn_INT_TYPE_CNF				1	//中断触发模式 0：边沿触发  1：电平出发（此中断将一直有效直至软件清除）

//定时器个数
#define TIMER_NUM					3      //3个定时器对应设定值为2

/*lln verify   定时器125MHZ 1ms是125000 100ms = 125000_00*/
#define TIME_IRQ_COMPARATOR_1S_VAL		125000000			//比较器值   对于周期中断来说  即中断间隔时间ticks
#define TIME_IRQ_COMPARATOR_1MS_VAL		125000
#define TIME_IRQ_COMPARATOR_500US_VAL   62500



//lln verify    定时器中断号
#define TIMER0_IRQ_NUMBER					(21+8) 
#define TIMER1_IRQ_NUMBER					(38+8)
#define TIMER2_IRQ_NUMBER					(39+8)  


//计时器频率125MHZ,1ms的寄存器计数是125000
#define TIMER_MUTIPLIER  125000

#define TIMER_REG      LS2K_HPET_MAIN_COUNTER_REG_LOW    //主定时器计数寄存器

#define ENABLE         1
#define DISABLE        0

#define OK             0xAAU
#define ERROR          0x55U
typedef struct timer 
{
    INT32U settime;      /* 超时时间(单位：ms) */
    INT32U starttime;    /* 定时器启动时刻 */
    INT8U  bstart;       /* 定时器启动标志 */    
} TIMER;

void initTimer(void);  /* 初始化定时器 */

/* 软定时器 */
INT8U settimer(TIMER *ptimer, INT32U ntime);
void starttimer(TIMER *ptimer);
void stoptimer(TIMER *ptimer);
INT8U isTimeout(const TIMER *ptimer);

INT8U delayms(INT32U ntime);


void drv_InitTimer(void);
void drv_EnTimer(void);
void drv_DisTimer(void);
void drv_ReloadTimer(void);
INT32U drv_GetCurrentTime(void);


#endif /* _TIMER_H */


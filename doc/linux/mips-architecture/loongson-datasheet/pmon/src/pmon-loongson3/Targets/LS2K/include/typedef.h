/*****************************************************************************/
/*文件名称：typedef.h					     									     */
/*文件说明：定时器操作接口													             */
/*运行平台：LS2K1000														         */
/*版权说明：Copyright(c) 2020 Beijing.Hollysys.Co.Ltd.All rights reserved    */
/*****************************************************************************/
/*版本：1.00		修订人：张朋东      		修订日期：	2020.06.05				         */
/*修订历史：																         */
/*		2020.6.05 - V1.00  张朋东      										 */
/*		            创建。   												     */
/*****************************************************************************/

#ifndef _TYPEDEF_H
#define _TYPEDEF_H


/* 基本数据类型定义*/
typedef unsigned int   UID;
typedef unsigned char  BOOL; 
typedef unsigned char  INT8U;      
typedef signed   char  INT8S;     
typedef unsigned short INT16U;     
typedef signed   short INT16S;    
typedef unsigned long  INT32U;     
typedef signed   long  INT32S;    
typedef void           VOID;       
typedef INT8U *        LPBYTE;    
typedef INT16U *       LPWORD;     
typedef void *         LPVOID;    
typedef INT32U*        LPLONG;
typedef void           (*PFUNC)(void); 
 
/* BOOL类型返回值 */
#define FALSE               0
#define TRUE                1
 
/* INT8U类型返回值 */
#define LGC_TRUE               (0xAA)
#define LGC_FALSE              (0x55)
 
#ifndef NULL
#define NULL ((void *)0 ) /* 指针为空值 */
#endif
 
#define LOBYTE(w)           ((INT8U)(w))               
#define HIBYTE(w)           ((INT8U)(((INT16U)(w) >> 8) & 0xFF))
#define FST_BYTE(w)       (INT8U)(  (((INT32U)w)             >>24) & 0xFF)
#define SND_BYTE(w)      (INT8U)( ((((INT32U)w) <<  8) >>24) & 0xFF )
#define THD_BYTE(w)      (INT8U)( ((((INT32U)w) <<16) >>24) & 0xFF )
#define FTH_BYTE(w)      (INT8U)( ((((INT32U)w) <<24) >>24) & 0xFF )
#define MAKEWORD(l, h)      (INT16U)(((INT16U)(l)) | (((INT16U)(h)) << 8))
#define MAKELONG(l, h)      (INT32U)((INT32U)(((INT16U)(l)) | \
                            (((INT32U)((INT16U)(h))) << 16)))
#endif

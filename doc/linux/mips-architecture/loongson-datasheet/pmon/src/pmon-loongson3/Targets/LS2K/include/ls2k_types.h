#ifndef _LS2K_TYPES_H
#define _LS2K_TYPES_H

//typedef unsigned int   UID;

typedef unsigned int   INT32U;

typedef unsigned char  BOOL;
typedef unsigned char  INT8U;
typedef signed   char  INT8S;
typedef unsigned short INT16U;
typedef signed   short INT16S;
//typedef unsigned long  INT32U; 
typedef signed   long  INT32S;

typedef unsigned long long  INT64U;

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

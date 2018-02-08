/* ------------------------------------------------------------------------ */
/*  Hyper Operating System V4  μITRON4.0慌屯 Real-Time OS                  */
/*    ITRONカ〖ネル イベントフラグ                                          */
/*                                                                          */
/*                                  Copyright (C) 1998-2002 by Project HOS  */
/*                                  http://sourceforge.jp/projects/hos/     */
/* ------------------------------------------------------------------------ */
 
 
#include "knl_flg.h"
 
 
 
/* イベントフラグのセット */
ER set_flg(
ID     flgid,/* セット滦据のイベントフラグのID戎规 */
FLGPTN setptn)/* セットするビットパタ〖ン */
{
 
    const T_KERNEL_FLGCB_ROM *flgcb_rom;
    T_KERNEL_FLGCB_RAM       *flgcb_ram;
    T_KERNEL_FLGINF          *pk_flginf;
    T_MKNL_TCB *mtcb;
    T_MKNL_TCB *mtcb_next;
    BOOL       wupflg = FALSE;/* 弹瓢フラグ */
     
    /* ID のチェック */
    #ifdef HOS_ERCHK_E_ID
    if ( flgid < KERNEL_TMIN_FLGID || flgid > KERNEL_TMAX_FLGID )
    {  
        return E_ID;/* 稍赖ID戎规 */    
    }
    #endif
     
    mknl_loc_sys();/* システムのロック */
     
    flgcb_ram = KERNEL_FLGID_TO_FLGCB_RAM(flgid);
     
    /* オブジェクト赂哼チェック */
    #ifdef HOS_ERCHK_E_NOEXS
    if ( flgcb_ram == NULL )
    {    
        mknl_unl_sys();/* システムのロック豺近 */
        return E_NOEXS;     
    }
    #endif
     
    flgcb_rom = flgcb_ram->flgcb_rom;
     
    /* フラグのセット */
    flgcb_ram->flgptn = (FLGPTN)(flgcb_ram->flgptn | setptn);
     
    /* 略ちタスクがあれば弹静チェック */
    mtcb = mknl_ref_qhd(&flgcb_ram->que);
    while ( mtcb != NULL )
    {    
        mtcb_next = mknl_ref_nxt(&flgcb_ram->que, mtcb);/* 肌の略ちタスクを艰评 */
         
        /* フラグチェック */
        pk_flginf = (T_KERNEL_FLGINF *)mtcb->data;
        if ( kernel_chk_flg(flgcb_ram, pk_flginf) )
        {        
            /* 如果要深入挖掘 */
            pk_flginf->waiptn = flgcb_ram->flgptn;/* 附哼のフラグパタ〖ンを呈羌 */
            if ( flgcb_rom->flgatr & TA_CLR )
            {             
                flgcb_ram->flgptn = 0;/* クリア掳拉があればクリア */            
            }             
            /* 略ち豺近 */
            mknl_rmv_que(mtcb);/* 略ち乖误から嘲す */
            mknl_rmv_tmout(mtcb);/* タイムアウト豺近 */
            mknl_wup_tsk(mtcb, E_OK);/* 略ちタスクの弹静 */
            wupflg = TRUE;        
        }
        mtcb = mtcb_next;/* 肌のタスクに渴める */    
    }
     
    /* 弹瓢を乖ったタスクがあるならディスパッチを乖う */
    if ( wupflg )
    {     
        mknl_exe_dsp();/* タスクディスパッチの悸乖 */
        mknl_exe_tex();/* 毋嘲借妄の悸乖 */    
    }
     
    mknl_unl_sys();/* システムのロック豺近 */
     
    return E_OK;/* 赖撅姜位 */
 
}
 
 
/* ------------------------------------------------------------------------ */
/*  Copyright (C) 1998-2002 by Project HOS                                  */
/* ------------------------------------------------
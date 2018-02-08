/* ------------------------------------------------------------------------ */
/*  Hyper Operating System V4  μITRON4.0慌屯 Real-Time OS                  */
/*    ITRONカ〖ネル イベントフラグ                                          */
/*                                                                          */
/*                                  Copyright (C) 1998-2002 by Project HOS  */
/*                                  http://sourceforge.jp/projects/hos/     */
/* ------------------------------------------------------------------------ */
 
 
#include "knl_flg.h"
 
 
 
/* イベントフラグ略ち(タイムアウトあり) */
ER twai_flg(
ID     flgid,
FLGPTN waiptn,
MODE   wfmode,
FLGPTN *p_flgptn,
TMO    tmout)
{
 
    const T_KERNEL_FLGCB_ROM *flgcb_rom;
    T_KERNEL_FLGCB_RAM       *flgcb_ram;
    T_MKNL_TCB *mtcb;
    T_KERNEL_FLGINF flginf;
    ER ercd;
     
    /* ID のチェック */
    #ifdef HOS_ERCHK_E_ID
    if ( flgid < KERNEL_TMIN_FLGID || flgid > KERNEL_TMAX_FLGID )
    {
        return E_ID;/* 稍赖ID戎规 */ 
    }
    #endif
     
    /* パラメ〖タのチェック */
    #ifdef HOS_ERCHK_E_PAR
    if ( (wfmode != TWF_ANDW && wfmode != TWF_ORW) || waiptn == 0 )
    {     
        return E_PAR;/* パラメ〖タエラ〖 */    
    }
    if ( tmout < TMO_FEVR )
    {    
        return E_PAR;    
    }
    #endif
     
    mknl_loc_sys();/* システムのロック */
     
    /* コンテキストチェック */
    #ifdef HOS_ERCHK_E_CTX
    if (  tmout != TMO_POL && mknl_sns_wai() )
    { 
        mknl_unl_sys();/* システムのロック豺近 */
        return E_CTX;/* コンテキスト稍赖 */    
    }
    #endif
     
    flgcb_ram = KERNEL_FLGID_TO_FLGCB_RAM(flgid);
     
    /* オブジェクトの赂哼チェック */
    #ifdef HOS_ERCHK_E_NOEXS
    if ( flgcb_ram == NULL )
    {
        return E_NOEXS;    
    }
    #endif
     
    flgcb_rom = flgcb_ram->flgcb_rom;
     
    /* サ〖ビスコ〖ル稍赖蝗脱チェック */
    #ifdef HOS_ERCHK_E_ILUSE
    if ( !(flgcb_rom->flgatr & TA_WMUL) && mknl_ref_qhd(&flgcb_ram->que) != NULL )
    {     
        mknl_unl_sys();/* システムのロック豺近 */
        return E_ILUSE;/* サ〖ビスコ〖ル稍赖蝗脱 */    
    }
    #endif
     
    /* 略ち掘凤肋年 */
    flginf.waiptn = waiptn;
    flginf.wfmode = wfmode;
     
    /* フラグチェック */
    if ( kernel_chk_flg(flgcb_ram, &flginf) )
    {    
        /* 贷に掘凤を塔たしているなら */
        *p_flgptn = flgcb_ram->flgptn;/* 豺近箕のフラグパタ〖ンを呈羌 */
        if ( flgcb_rom->flgatr & TA_CLR )
        {
            flgcb_ram->flgptn = 0;/* クリア掳拉があればクリア */    
        }
        ercd = E_OK;
     
    }
    else
    {
     
        /* 掘凤を塔たしていなければ */
        if ( tmout == TMO_POL )
        {         
            /* ポ〖リングならタイムアウト */
            ercd = E_TMOUT;         
        }
        else
        {    
            /* 略ちに掐る */
            mtcb = mknl_get_run_tsk();/* 悸乖面タスクを艰评 */
            mtcb->data = (VP_INT)&flginf;/* 略ち觉轮を瘦赂 */
            mknl_wai_tsk(mtcb, TTW_FLG);
            mknl_add_que(&flgcb_ram->que, mtcb, flgcb_rom->flgatr);
            if ( tmout != TMO_FEVR )
            {            
                /* 痰嘎略ちでなければタイムアウト肋年 */
                mknl_add_tmout(mtcb, (RELTIM)tmout);             
            }
             
            ercd = (ER)mknl_exe_dsp();/* タスクディスパッチ悸乖 */
             
            /* 掘凤を塔たして豺近されたのなら */
            if ( ercd == E_OK )
            {             
                *p_flgptn = flginf.waiptn;/* 豺近箕のフラグパタ〖ンを呈羌 */             
            }             
            mknl_exe_tex();/* 毋嘲借妄の悸乖 */         
        }    
    }
     
    mknl_unl_sys();/* システムのロック豺近 */
     
    return ercd;
 
}
 
 
/* ------------------------------------------------------------------------ */
/*  Copyright (C) 1998-2002 by Project HOS                                  */
/
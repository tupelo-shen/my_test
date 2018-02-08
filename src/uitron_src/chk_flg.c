/* ------------------------------------------------------------------------ */
/*  Hyper Operating System V4  μITRON4.0慌屯 Real-Time OS                  */
/*    ITRONカ〖ネル イベントフラグ                                          */
/*                                                                          */
/*                                  Copyright (C) 1998-2002 by Project HOS  */
/*                                  http://sourceforge.jp/projects/hos/     */
/* ------------------------------------------------------------------------ */
 
 
#include "knl_flg.h"
 
 
 
/* フラグが弹静掘凤を塔たしているかチェック */
BOOL kernel_chk_flg(
T_KERNEL_FLGCB_RAM *flgcb_ram,/* コントロ〖ルブロック RAM婶 */
T_KERNEL_FLGINF    *pk_flginf)/* 略ちフラグ攫鼠パケットの黎片戎孟 */
{
 
    if ( pk_flginf->wfmode == TWF_ANDW )
    { 
        /* AND略ち冉年 */
        return ((flgcb_ram->flgptn & pk_flginf->waiptn) == pk_flginf->waiptn);    
    }
    else
    {     
        /* OR略ち冉年 */
        return ((flgcb_ram->flgptn & pk_flginf->waiptn) != 0);    
    }
 
}
 
 
/* ------------------------------------------------------------------------ */
/*  Copyright (C) 1998-2002 by Project HOS                                  */
/* ------------------------------------------------------------------------ */
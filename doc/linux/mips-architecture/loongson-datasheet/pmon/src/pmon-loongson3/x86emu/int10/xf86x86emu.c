/* $XFree86: xc/programs/Xserver/hw/xfree86/int10/xf86x86emu.c,v 1.11 2001/04/30 14:34:58 tsi Exp $ */
/*
 *                   XFree86 int10 module
 *   execute BIOS int 10h calls in x86 real mode environment
 *                 Copyright 1999 Egbert Eich
 */
#include <x86emu.h>
#include "xf86.h"
#define _INT10_PRIVATE
#include "xf86int10.h"
#include "xf86x86emu.h"

#include <stdio.h>
#include <stdarg.h>

#define M _X86EMU_env

static void x86emu_do_int(int num)
{
	Int10Current->num = num;
#ifdef USE_780E_VGA
	printf("int10current->num=0x%x\n",num);
#endif
	if (!int_handler(Int10Current)) {
	printf("int_handler_int10current is error,system halted...\n");
			X86EMU_halt_sys();
	}
}

void xf86ExecX86int10(xf86Int10InfoPtr pInt)
{
	int sig = setup_int(pInt);

	if (sig < 0)
		return;

	if (int_handler(pInt)) {
		X86EMU_exec();
	}

	finish_int(pInt, sig);
}

Bool xf86Int10ExecSetup(xf86Int10InfoPtr pInt)
{
	int i;
	X86EMU_intrFuncs intFuncs[256];
	X86EMU_pioFuncs pioFuncs = {
		(u8(*)(u16)) x_inb,
		(u16(*)(u16)) x_inw,
		(u32(*)(u16)) x_inl,
		(void (*)(u16, u8))x_outb,
		(void (*)(u16, u16))x_outw,
		(void (*)(u16, u32))x_outl
	};

	X86EMU_memFuncs memFuncs = {
		(u8(*)(u32)) Mem_rb,
		(u16(*)(u32)) Mem_rw,
		(u32(*)(u32)) Mem_rl,
		(void (*)(u32, u8))Mem_wb,
		(void (*)(u32, u16))Mem_ww,
		(void (*)(u32, u32))Mem_wl
	};

	X86EMU_setupMemFuncs(&memFuncs);

	pInt->cpuRegs = &M;
	M.mem_base = 0;
	M.mem_size = 1024 * 1024 + 1024;
	X86EMU_setupPioFuncs(&pioFuncs);

	for (i = 0; i < 256; i++)
		intFuncs[i] = x86emu_do_int;
	X86EMU_setupIntrFuncs(intFuncs);
	return TRUE;
}

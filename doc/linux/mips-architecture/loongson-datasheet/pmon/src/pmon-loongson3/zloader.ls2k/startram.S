#ifndef _KERNEL
#define _KERNEL
#endif


/*
 * The bits in the status register.  All bits are active when set to 1.
 */
#define SR_COP_USABILITY	0xf0000000
#define SR_COP_0_BIT		0x10000000
#define SR_COP_1_BIT		0x20000000
#define SR_RP			0x08000000
#define SR_FR_32		0x04000000
#define SR_RE			0x02000000
#define SR_BOOT_EXC_VEC		0x00400000
#define SR_TLB_SHUTDOWN		0x00200000
#define SR_SOFT_RESET		0x00100000
#define SR_DIAG_CH		0x00040000
#define SR_DIAG_CE		0x00020000
#define SR_DIAG_DE		0x00010000
#define SR_KX			0x00000080
#define SR_SX			0x00000040
#define SR_UX			0x00000020
#define SR_KSU_MASK		0x00000018
#define SR_KSU_USER		0x00000010
#define SR_KSU_SUPER		0x00000008
#define SR_KSU_KERNEL		0x00000000
#define SR_ERL			0x00000004
#define SR_EXL			0x00000002
#define SR_INT_ENAB		0x00000001
/*#define SR_INT_MASK		0x0000ff00*/




#define zero	$0	/* always zero */
#define AT	$at	/* assembler temp */
#define v0	$2	/* return value */
#define v1	$3
#define a0	$4	/* argument registers */
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8	/* temp registers (not saved across subroutine calls) */
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16	/* saved across subroutine calls (callee saved) */
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24	/* two more temp registers */
#define t9	$25
#define k0	$26	/* kernel temporary */
#define k1	$27
#define gp	$28	/* global pointer */
#define sp	$29	/* stack pointer */
#define s8	$30	/* one more callee saved */
#define ra	$31	/* return address */


/*
 * Coprocessor 0 registers:
 */
#define COP_0_TLB_INDEX		$0
#define COP_0_TLB_RANDOM	$1
#define COP_0_TLB_LO0		$2
#define COP_0_TLB_LO1		$3
#define COP_0_TLB_CONTEXT	$4
#define COP_0_TLB_PG_MASK	$5
#define COP_0_TLB_WIRED		$6
#define COP_0_BAD_VADDR		$8
#define COP_0_COUNT		$9
#define COP_0_TLB_HI		$10
#define COP_0_COMPARE		$11
#define COP_0_STATUS_REG	$12
#define COP_0_CAUSE_REG		$13
#define COP_0_EXC_PC		$14
#define COP_0_PRID		$15
#define COP_0_CONFIG		$16
#define COP_0_LLADDR		$17
#define COP_0_WATCH_LO		$18
#define COP_0_WATCH_HI		$19
#define COP_0_TLB_XCONTEXT	$20
#define COP_0_ECC		$26
#define COP_0_CACHE_ERR		$27
#define COP_0_TAG_LO		$28
#define COP_0_TAG_HI		$29
#define COP_0_ERROR_PC		$30
/* RM7000 specific */
#define COP_0_WATCH_1		$18
#define COP_0_WATCH_2		$19
#define COP_0_WATCH_M		$24
#define COP_0_PC_COUNT		$25
#define COP_0_PC_CTRL		$22

#define	COP_0_ICR		$20		/* CFC */
#define	COP_0_DERR_0		$26		/* CFC */
#define	COP_0_DERR_1		$27		/* CFC */
#define SR_BOOT_EXC_VEC		0x00400000
//---------------------------------------------------

#define CONFIG_CACHE_64K_4WAY 1

#define tmpsize		s1
#define msize		s2
#define sdShape		s3
#define bonito		s4
#define dbg		s5
#define sdCfg		s6

#define	CFG_IB 0x00000020
#define CFG_DB 0x00000010
#define CFG_C_WBACK 3
#define CFG_BE 0x00008000
#define CFG_EPMASK 0x0f000000
#define CFG_EPD 0x00000000
#define CFG_EM_R4K 0x00000000
#define CFG_EMMASK 0x00c00000
#define CFG_AD 0x00800000

#define CP0_CONFIG $16
#define CP0_TAGLO  $28
#define CP0_TAGHI  $29

#define DDR100 0x04041091
#define	DDR266 0x0410435e
#define DDR300 0x041453df




#define	PRINTSTR(x) \
	.rdata;98: .asciz x; .text; la a0, 98b; bal stringserial; nop




/*
 *   Register usage:
 *
 *	s0	link versus load offset, used to relocate absolute adresses.
 *	s1	free
 *	s2	memory size.
 *	s3	sdShape.
 *	s4	Bonito base address.
 *	s5	dbg.
 *	s6	sdCfg.
 *	s7	rasave.
 *	s8	L3 Cache size.
 */

	.set	noreorder
	.globl	_start
	.globl	start
	.globl	__main
_start:
start:
	.globl	stack
stack = start - 0x4000		/* Place PMON stack below PMON start in RAM */
	mtc0	zero, COP_0_STATUS_REG
	mtc0	zero, COP_0_CAUSE_REG
	li	t0, SR_BOOT_EXC_VEC	/* Exception to Boostrap Location */
	mtc0	t0, COP_0_STATUS_REG
    nop

	mfc0    t0, COP_0_STATUS_REG
	li	t1, 0x64000000|SR_KX|SR_SX|SR_UX|SR_BOOT_EXC_VEC      # {cu3,cu2,cu1,cu0}<={0110, status_fr<=1,0xe0 to enable 64bit space
	or	t0, t0, t1
	mtc0	t0, COP_0_STATUS_REG
	mtc0	zero, COP_0_CAUSE_REG


	la	sp, stack
	la	gp, _gp
	move s1,a3 //struct callvectors *cv

	bal	locate			/* Get current execute address */
	nop

/*
 *  We get here from executing a bal to get the PC value of the current execute
 *  location into ra. Check to see if we run from ROM or if this is ramloaded.
 */
locate:
	la	s0,start
	subu	s0,ra,s0
	and	s0,0xffff0000
	or s0,zero,zero
	#li s0,1

	#PRINTSTR("fun=start\r\n")
	PRINTSTR("initreg1:")
	nop
	move    a0, ra
	bal     hexserial
	nop
	PRINTSTR("\r\n")



#if 1 //Ê¹ÄÜcache
        mfc0   a0,COP_0_CONFIG
        and    a0,a0,0xfffffff8
        or     a0,a0,0x3
        mtc0   a0,COP_0_CONFIG
#endif

	/* Clear BSS */
	la	a0, _edata
	la	a2, _end
2:	sw	zero, 0(a0)
	bne	a2, a0, 2b
	addu	a0, 4

	li	a0, 0
	sw	a0, CpuTertiaryCacheSize /* Set L3 cache size */
	li  a0,MEMSIZE
	move a1,s1


    lui  t0, 0xBFE0
    li   t1, 0x55
    sb   t1, 0(t0)
1:
	nop
	#b 1b

 
	la	v0, initmips
	jalr	v0
	nop
stuck:
	b	stuck
	nop




#ifndef _MIPS_ASM_H
#define _MIPS_ASM_H
#ifndef ABICALLS
#define    ABICALLS    .abicalls
#endif
#if defined(ABICALLS) && !defined(_KERNEL)
    ABICALLS
#endif
#define RCSID(x)
#if defined(__MIPSEL__)
#define LWLO    lwl
#define LWHI    lwr
#define    SWLO    swl
#define    SWHI    swr
#else
#if defined(__MIPSEB__)
#define LWLO    lwr
#define LWHI    lwl
#define    SWLO    swr
#define    SWHI    swl
#else
#error "__MIPSEL__ or __MIPSEB__ must be defined"
#endif
#endif
#if defined(ABICALLS) && !defined(_KERNEL)
#define    ABISETUP        \
    .set    noreorder;    \
    .cpload    t9;        \
    .set    reorder;
#else
#define    ABISETUP
#endif
#if defined(GPROF) || defined(PROF)
#define    MCOUNT            \
    subu     sp, sp, 32;    \
    .cprestore 16;        \
    sw    ra, 28(sp);    \
    sw    gp, 24(sp);    \
    .set    noat;        \
    .set    noreorder;    \
    move    AT, ra;        \
    jal    _mcount;    \
    subu    sp, sp, 8;    \
    lw    ra, 28(sp);    \
    addu    sp, sp, 32;    \
    .set reorder;        \
    .set    at;
#else
#define    MCOUNT
#endif
#define LEAF(x)            \
    .align    3;        \
    .globl x;        \
    .ent x, 0;        \
x: ;                \
    .frame sp, 0, ra;    \
    ABISETUP        \
    MCOUNT

#define    ALEAF(x)        \
    .globl    x;        \
x:
#define NLEAF(x)        \
    .align    3;        \
    .globl x;        \
    .ent x, 0;        \
x: ;                \
    .frame sp, 0, ra;    \
    ABISETUP
#define NON_LEAF(x, fsize, retpc) \
    .align    3;        \
    .globl x;        \
    .ent x, 0;        \
x: ;                \
    .frame sp, fsize, retpc; \
    ABISETUP        \
    MCOUNT
#define NNON_LEAF(x, fsize, retpc) \
    .align    3;        \
    .globl x;        \
    .ent x, 0;        \
x: ;                \
    .frame sp, fsize, retpc    \
    ABISETUP
#define END(x) \
    .end x
#define PANIC(msg) \
    la    a0, 9f; \
    jal    panic; \
    MSG(msg)
#define    PRINTF(msg) \
    la    a0, 9f; \
    jal    printf; \
    MSG(msg)
#define    MSG(msg) \
    .rdata; \
9:    .asciiz    msg; \
    .text
#define ASMSTR(str) \
    .asciiz str; \
    .align    3
#if (_MIPS_SZPTR == 32)
#define PTR_ADD        add
#define PTR_ADDU    addu
#define PTR_ADDI    addi
#define PTR_ADDIU    addiu
#define PTR_SUB        sub
#define PTR_SUBU    subu
#define PTR_L        lw
#define PTR_S        sw
#define PTR_LA        la
#define PTR_LI        li
#define PTR_SLL        sll
#define PTR_SLLV    sllv
#define PTR_SRL        srl
#define PTR_SRLV    srlv
#define PTR_SRA        sra
#define PTR_SRAV    srav
#define PTR_SCALESHIFT    2
#define PTR        .word
#define PTRSIZE        4
#define PTRLOG        2
#endif
#if (_MIPS_SZPTR == 64)
#define PTR_ADD        dadd
#define PTR_ADDU    daddu
#define PTR_ADDI    daddi
#define PTR_ADDIU    daddiu
#define PTR_SUB        dsub
#define PTR_SUBU    dsubu
#define PTR_L        ld
#define PTR_S        sd
#define PTR_LA        dla
#define PTR_LI        dli
#define PTR_SLL        dsll
#define PTR_SLLV    dsllv
#define PTR_SRL        dsrl
#define PTR_SRLV    dsrlv
#define PTR_SRA        dsra
#define PTR_SRAV    dsrav
#define PTR_SCALESHIFT    3
#define PTR        .dword
#define PTRSIZE        8
#define PTRLOG        3
#endif
#endif

#define COM1_BASE_ADDR  0xbfe00000


LEAF(tgt_putchar)
	nop
	la	v0, COM1_BASE_ADDR
1:
	lbu	v1, 5(v0)
	and	v1, 0x20
	beqz	v1, 1b
	nop
	sb	a0, 0(v0)
	jr	ra
	nop	
END(tgt_putchar)


/*output int var in ascii format*/
LEAF(hexserial)
	.set mips32r2
	.set noat
	nop
	move	a2, ra
	move	a1, a0
	li	a3, 7
1:
	rol	a0, a1, 4
	move	a1, a0
	and	a0, 0xf
	la	v0, hexchar
	addu	v0, a0
	bal	tgt_putchar
	lbu	a0, 0(v0)

	bnez	a3, 1b
	addu	a3, -1

	jr	a2
	nop
END(hexserial)






LEAF(tgt_putchar_old)
	nop
	la	v0, COM1_BASE_ADDR
1:
	lbu	v1, 5(v0)
	and	v1, 0x20
	beqz	v1, 1b
	nop

	sb	a0, 0(v0)
#	li  a0, 0x5a
#	lbu a0, 0(v0)	
#2:
#	lbu	v1, 5(v0)
#	and	v1, 0x20
#	beqz	v1, 2b
#	nop
#	sb	a0, 0(v0)
#	j	ra
	jr	ra
	nop	
END(tgt_putchar_old)
#ednif


/*
 * Simple character printing routine used before full initialization
 */

LEAF(stringserial)
	move	a2, ra
	addu	a1, a0, s0
	lbu	a0, 0(a1)
1:
	beqz	a0, 2f
	nop
	bal	tgt_putchar
	addiu	a1, 1
	b	1b
	lbu	a0, 0(a1)
2:
	j	a2
	nop
END(stringserial)

LEAF(outstring)
	move	a2, ra
	move	a1, a0
	lbu	a0, 0(a1)
1:
	beqz	a0, 2f
	nop
	bal	tgt_putchar
	addiu	a1, 1
	b	1b
	lbu	a0, 0(a1)

2:
	j	a2
	nop
END(outstring)

LEAF(hexserial_old)
	move	a2, ra
	move	a1, a0
	li	a3, 7
1:
	rol	a0, a1, 4
	move	a1, a0
	and	a0, 0xf
	la	v0, hexchar
	addu	v0, s0
	addu	v0, a0
#ifdef BOOT_FROM_NAND
#ifdef NAND_ECC_MODE
	addiu   v0, v0,0x400 
#endif
#endif
	bal	tgt_putchar
	lbu	a0, 0(v0)

	bnez	a3, 1b
	addu	a3, -1

	j	a2
	nop
END(hexserial_old)



	.rdata
hexchar:
	.ascii	"0123456789abcdef"

	.text
	.align	2

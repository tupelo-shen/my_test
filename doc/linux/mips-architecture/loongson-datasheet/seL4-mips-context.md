enum _register {
    MIPS_REG_v0         = 0 ,   /* return value */
    MIPS_REG_v1         = 1 ,
    MIPS_REG_a0         = 2 ,   /* argument registers */
    capRegister         = 2 ,
    badgeRegister       = 2 ,
    MIPS_REG_a1         = 3 ,
    msgInfoRegister     = 3 ,
    MIPS_REG_a2         = 4 ,
    MIPS_REG_a3         = 5 ,
    MIPS_REG_t0         = 6 ,   /* temp registers (not saved across subroutine calls) */
    MIPS_REG_t1         = 7 ,
    MIPS_REG_t2         = 8 ,
    MIPS_REG_t3         = 9 ,
    MIPS_REG_t4         = 10 ,
    MIPS_REG_t5         = 11 ,
    MIPS_REG_t6         = 12 ,
    MIPS_REG_t7         = 13 ,
    MIPS_REG_s0         = 14 ,   /* saved across subroutine calls (callee saved) */
    MIPS_REG_s1         = 15 ,
    MIPS_REG_s2         = 16 ,
    MIPS_REG_s3         = 17 ,
    MIPS_REG_s4         = 18 ,
    MIPS_REG_s5         = 19 ,
    MIPS_REG_s6         = 20 ,
    MIPS_REG_s7         = 21 ,
    MIPS_REG_t8         = 22 ,  /* two more temp registers */
    MIPS_REG_t9         = 23 ,
    MIPS_REG_gp         = 24 ,  /* global pointer */
    MIPS_REG_sp         = 25 ,  /* stack pointer */
    MIPS_REG_fp         = 26 ,  /* one more callee saved. s8 */
    MIPS_REG_ra         = 27 ,  /* return address */
    MIPS_REG_k0         = 28 ,  /* kernel temporary */
    MIPS_REG_k1         = 29 ,  /*               */
    MIPS_C0_status      = 30 ,  /* 龙芯用的是status  */
    MIPS_C0_epc         = 31 ,  /* epc 寄存器 */
    FaultInstruction    = 32 ,  /* 错误指令寄存器 */
    /* user readable/writable thread ID register.
     * name comes from the ARM manual */
    TPIDRURW = 33,             /*  */
    MIPS_REG_HI         = 34 ,  /* 乘除法指令会使用这两个寄存器 added by shenwanjiang */
    MIPS_REG_LO         = 35 ,  /* 乘除法指令会使用这两个寄存器 added by shenwanjiang */
    n_contextRegisters  = 36 ,
};

typedef struct seL4_UserContext_ {
    seL4_Word mips_c0_epc    ;
    seL4_Word mips_reg_sp     ;
    seL4_Word mips_c0_status ;
    seL4_Word mips_reg_a0     ;
    seL4_Word mips_reg_a1     ;
    seL4_Word mips_reg_v0     ;
    seL4_Word mips_reg_v1     ;
    seL4_Word mips_reg_a2     ;
    seL4_Word mips_reg_a3     ;
    seL4_Word mips_reg_t0     ;
    seL4_Word mips_reg_t1     ;
    seL4_Word mips_reg_t2     ;
    seL4_Word mips_reg_t3     ;
    seL4_Word mips_reg_t4     ;
    seL4_Word mips_reg_t5     ;
    seL4_Word mips_reg_t6     ;
    seL4_Word mips_reg_t7     ;
    seL4_Word mips_reg_s0     ;
    seL4_Word mips_reg_s1     ;
    seL4_Word mips_reg_s2     ;
    seL4_Word mips_reg_s3     ;
    seL4_Word mips_reg_s4     ;
    seL4_Word mips_reg_s5     ;
    seL4_Word mips_reg_s6     ;
    seL4_Word mips_reg_s7     ;
    seL4_Word mips_reg_t8     ;
    seL4_Word mips_reg_t9     ;
    seL4_Word mips_reg_gp     ;
    seL4_Word mips_reg_fp     ;
    seL4_Word mips_reg_ra     ;
    seL4_Word mips_reg_k0     ;
    seL4_Word mips_reg_k1     ;
} seL4_UserContext;


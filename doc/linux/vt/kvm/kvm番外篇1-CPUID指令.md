[TOC]

The CPUID instruction can be used to retrieve various amount of information about your cpu, like its vendor string and model number, the size of internal caches and (more interesting), the list of CPU features supported.

## 1 如何使用CPUID指令

#### 1.1 检查CPUID是否可用

Prior to using the CPUID instruction, you should also make sure the processor supports it by testing the 'ID' bit (0x200000) in `eflags`. This bit is modifiable only when the CPUID instruction is supported. For systems that don't support CPUID, changing the 'ID' bit will have no effect.

> Implementing this routine in for example C can lead to issues, because the compiler may change EFLAGS at any time.

This assembly routine checks if CPUID is supported:

```c
    pushfd                               // Save EFLAGS
    pushfd                               // Store EFLAGS
    xor dword [esp],0x00200000           // Invert the ID bit in stored EFLAGS
    popfd                                // Load stored EFLAGS (with ID bit inverted)
    pushfd                               // Store EFLAGS again (ID bit may or may not be inverted)
    pop eax                              // eax = modified EFLAGS (ID bit may or may not be inverted)
    xor eax,[esp]                        // eax = whichever bits were changed
    popfd                                // Restore original EFLAGS
    and eax,0x00200000                   // eax = zero if ID bit can't be changed, else non-zero
    ret
```

> Note 1: There are some old CPUs where CPUID is supported but the ID bit in EFLAGS is not (NexGen). The are also CPUs that support CPUID if and only if it has to be enabled first (Cyrix M1).
>
> Note 2: You can simply attempt to execute the CPUID instruction and see if you get an invalid opcode exception. This avoids problems with CPUs that do support CPUID but don't support the ID bit in EFLAGS; and is likely to be faster for CPUs that do support CPUID (and slower for CPUs that don't).

#### 1.2 基本使用方法

The idea of the CPUID instruction is that you can call it with different values in EAX, and it will return different information about the processor. For example, if we want the Vendor ID String (see below), we should code something like that:

```c
mov eax, 0x0
cpuid
```

There are differences between AMD and Intel. According to the Intel CPUID application note, we should first check the Vendor ID String for "GenuineIntel" before taking out information, such as the Processor Signature, Processor Feature Flags, etc.

#### 1.3 CPU供应商ID

When called with `EAX = 0`, `CPUID` returns the vendor ID string in `EBX`, `EDX` and `ECX`. Writing these to memory in this order results in a 12-character string. These can be tested against known Vendor ID strings:

```c
/* Vendor-strings. */
#define CPUID_VENDOR_OLDAMD       "AMDisbetter!" /* early engineering samples of AMD K5 processor */
#define CPUID_VENDOR_AMD          "AuthenticAMD"
#define CPUID_VENDOR_INTEL        "GenuineIntel"
#define CPUID_VENDOR_VIA          "CentaurHauls"
#define CPUID_VENDOR_OLDTRANSMETA "TransmetaCPU"
#define CPUID_VENDOR_TRANSMETA    "GenuineTMx86"
#define CPUID_VENDOR_CYRIX        "CyrixInstead"
#define CPUID_VENDOR_CENTAUR      "CentaurHauls"
#define CPUID_VENDOR_NEXGEN       "NexGenDriven"
#define CPUID_VENDOR_UMC          "UMC UMC UMC "
#define CPUID_VENDOR_SIS          "SiS SiS SiS "
#define CPUID_VENDOR_NSC          "Geode by NSC"
#define CPUID_VENDOR_RISE         "RiseRiseRise"
#define CPUID_VENDOR_VORTEX       "Vortex86 SoC"
#define CPUID_VENDOR_VIA          "VIA VIA VIA "
 
/*Vendor-strings from Virtual Machines.*/
#define CPUID_VENDOR_VMWARE       "VMwareVMware"
#define CPUID_VENDOR_XENHVM       "XenVMMXenVMM"
#define CPUID_VENDOR_MICROSOFT_HV "Microsoft Hv"
#define CPUID_VENDOR_PARALLELS    " lrpepyh vr"
```
You already know that the Vendor ID String is returned in `EBX`, `ECX`, `EDX`. Let us take an Intel processor. It should return "`GenuineIntel`". Look at the following text to see how the string is placed in the registers:

```
      MSB         LSB
EBX = 'u' 'n' 'e' 'G'
EDX = 'I' 'e' 'n' 'i'
ECX = 'l' 'e' 't' 'n' 
```

Also, `EAX` is set to the maximum EAX value supported for `CPUID` calls, as not all queries are supported on all processors.

#### 1.3 CPU特性

When called with `EAX = 1` (`CPUID_GETFEATURES`), `CPUID` returns a bit field in `EDX` containing the following values. Note that different brands of CPUs may have given different meanings to these. Recent processors also use `ECX` for features (which form a different set), with which you should be very careful as some old CPUs return bogus information in this register.

```c
enum {
    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_ETPRD        = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_PCIDE        = 1 << 17, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_x2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_AES          = 1 << 25, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
 
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLF          = 1 << 19, 
    CPUID_FEAT_EDX_DTES         = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM1          = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
};
```

## 2 2 GCC下使用CPUID指令

Alternatively, one can use the `__get_cpuid` function that comes with `GCC`. To use this function, include `<cpuid.h>`.

```c
#include <cpuid.h>
 
/* 示例：得到CPU模型编号 */
static int get_model(void)
{
    int ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);
    return ebx;
}
 
/* 示例：检查内置的本地APIC */
static int check_apic(void)
{
    unsigned int eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, %edx);
    return edx & CPUID_FEAT_EDX_APIC;
}
```
`CPUID` can be invoked with various request codes (in `eax`) and will return values in general registers (much as a built-in service interrupt). The following code is made Public Domain out of Clicker's `x86/cpu.h`

```c
/* DEPRECATED: You should use the <cpuid.h> header that comes with GCC instead. */
 
enum cpuid_requests {
  CPUID_GETVENDORSTRING,
  CPUID_GETFEATURES,
  CPUID_GETTLB,
  CPUID_GETSERIAL,
 
  CPUID_INTELEXTENDED=0x80000000,
  CPUID_INTELFEATURES,
  CPUID_INTELBRANDSTRING,
  CPUID_INTELBRANDSTRINGMORE,
  CPUID_INTELBRANDSTRINGEND,
};
 
/** issue a single request to CPUID. Fits 'intel features', for instance
 *  note that even if only "eax" and "edx" are of interest, other registers
 *  will be modified by the operation, so we need to tell the compiler about it.
 */
static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
  asm volatile("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
}
 
/** issue a complete request, storing general registers output as a string
 */
static inline int cpuid_string(int code, uint32_t where[4]) {
  asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)),
               "=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
  return (int)where[0];
}
```

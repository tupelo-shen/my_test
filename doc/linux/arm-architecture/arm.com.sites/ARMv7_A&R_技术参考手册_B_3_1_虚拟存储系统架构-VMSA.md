[TOC]

## 1 关于VMSA

> This chapter describes the ARMv7 VMSA, including the Security Extensions, the Multiprocessing Extensions, the Large Physical Address Extension (LPAE), and the Virtualization Extensions. This is referred to as the Extended VMSAv7. This chapter also describes the differences in VMSAv7 implementations that do not include some or all of these extensions.
> 
> For details of the VMSA differences in previous versions of the ARM architecture see:
> 
>   * VMSA support on page D12-2521 for ARMv6
>   * Virtual memory support on page D15-2606 for the ARMv4 and ARMv5 architectures.

In `VMSAv7`, a Memory Management Unit (MMU) controls address translation, access permissions, and memory attribute determination and checking, for memory accesses made by the processor. The MMU is controlled by system control registers, that can also disable the MMU. This chapter includes a definition the behavior of the memory system when the MMU is disabled.

The Extended VMSAv7 provides multiple stages of memory system control, as follows:

* for operation in Secure state, a single stage of memory system control

* for operation in Non-secure state, up to two stages of memory system control:

    - when executing at PL2, a single stage of memory system control
    - when executing at PL1 or PL0, two stages of memory system control.

Each supported stage of memory system control is provided by an MMU, with its own independent set of controls. Therefore, the Extended VMSAv7 provides the following MMUs:

* Secure PL1&0 stage 1 MMU
* Non-secure PL2 stage 1 MMU
* Non-secure PL1&0 stage 1 MMU
* Non-secure PL1&0 stage 2 MMU.

> The model of having a separate MMU for each stage of memory control is an architectural abstraction. It does not indicate any specific hardware requirements for an Extended VMSAv7 processor implementation. The architecture requires only that the behavior of any VMSAv7 processor matches the behavior described in this manual.s

These features mean the Extended VMSAv7 can support a hierarchy of software supervision, for example an Operating System and a hypervisor.

Each MMU uses a set of address translations and associated memory properties held in memory mapped tables called translation tables.

If an implementation does not include the Security Extensions, it has only a single security state, with a single MMU with controls equivalent to the Secure state MMU controls.

If an implementation does not include the Virtualization Extensions then:

* it does not support execution at PL2
* it Non-secure state, it provides only the Non-secure PL1&0 stage 1 MMU.

For an MMU, the translation tables define the following properties:

#### 1.1 访问secure或non-secure地址映射

If an implementation includes the Security Extensions, the translation table entries determine whether an access from Secure state accesses the Secure or the Non-secure address map. Any access from Non-secure state accesses the Non-secure address map.

#### 1.2 内存访问权限控制

This controls whether a program is permitted to access a memory region. For instruction and data access, the possible settings are:

* no access
* read-only
* write-only
* read/write.

For instruction accesses, additional controls determine whether instructions can be fetched and executed from the memory region.

If a processor attempts an access that is not permitted, a memory fault is signaled to the processor.

#### 1.3 内存属性

These describe the properties of a memory region. The top-level attribute, the Memory type, is one of Strongly-ordered, Device, or Normal. Device and Normal memory regions can have additional attributes, see Summary of ARMv7 memory attributes on page A3-127.

#### 1.4 地址转换映射

在`stage 1`阶段的转换中，将虚拟地址（`VA`）作为输入，可以转换成不同的地址：

* 如果只有一个`stage`的地址转换，则转换后的地址就是物理地址（`PA`）；
* 如果提供了两个`stage`的地址转换，则输出地址是中间物理地址（`IPA`）；

> 在`ARMv7`架构里，类似于操作系统的软件层，定义或者使用`stage 1`的地址转换时，可能不会意识到`IPA`和`PA`的区别。 

在`stage 1`阶段的转换中，将`IPA`转换成`PA`。

不同的安全状态和内存访问的特权级别，定义了不同的地址转换机制。图B3-1展示了VMSA转换机制，以及相关的转换阶段和MMU。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARMv7_A_picture_B_3_1.PNG">

> 从概念上讲，只有`stage 1`阶段MMU的转换机制，等价于一个固定的，扁平化的`stage 2`从IPA到PA的映射。

系统控制协处理器`CP15`控制了`VMSA`，包括定义地址转换表的位置，使能、配置MMU硬件管理单元。另外，如果发生地址访问错误的时候向CPU进行报告。

## 2 VMSA描述中的地址类型

在描述`VMSAv7`架构过程中，涉及到下列地址类型：

> These descriptions relate to a VMSAv7 description and therefore sometimes differ from the generic definitions given in the Glossary.

* 虚拟地址-VA

    An address used in an instruction, as a data or instruction address, is a Virtual Address (VA).

    An address held in the PC, LR, or SP, is a VA.

    The VA map runs from zero to the size of the VA space. For ARMv7, the maximum VA space is 4GB, giving a maximum VA range of 0x00000000-0xFFFFFFFF.

* 修改过的虚拟地址-MVA

    On an implementation that implements and uses the FCSE, the FCSE takes a VA and transforms it to an MVA. This is a preliminary address translation, performed before the address translation described in this chapter.

    Otherwise, MVA is a synonym for VA.

    > Appendix D10 Fast Context Switch Extension (FCSE) describes the FCSE. From ARMv6, ARM deprecates any use of the FCSE. The FCSE is:
    > 
    > * OPTIONAL and deprecated in an ARMv7 implementation that does not include the Multiprocessing Extensions.
    > 
    > * Obsolete from the introduction of the Multiprocessing Extensions.

* 中间物理地址-IPA

    In a translation regime that provides two stages of address translation, the IPA is the address after the stage 1 translation, and is the input address for the stage 2 translation.

    In a translation regime that provides only one stage of address translation, the IPA is identical to the PA.

    In ARM VMSA implementations, only one stage of address translation is provided:

    - if the implementation does not include the Virtualization Extensions
    - when executing in Secure state
    - when executing in Hyp mode.

* 物理地址-PA

    The address of a location in the Secure or Non-secure memory map. That is, an output address from the processor to the memory system.


## 3 VMSA实现中的地址空间

The ARMv7 architecture supports:

* A VA address space of up to 32 bits. The actual width is IMPLEMENTATION DEFINED.

* An IPA address space of up to 40 bits. The translation tables and associated system control registers define the width of the implemented address space.

> The Large Physical Address Extension defines two translation table formats. The Long-descriptor format gives access to the full 40-bit IPA or PA address space at a granularity of 4KB. The Short-descriptor format:
> 
> * Gives access to a 32-bit PA address space at 4KB granularity.
> 
> * Optionally, gives access to a 40-bit PA address space, but only at 16MB granularity.

If an implementation includes the Security Extensions, the address maps are defined independently for Secure and Non-secure operation, providing two independent 40-bit address spaces, where:

* a VA accessed from Non-secure state can only be translated to the Non-secure address map

* a VA accessed from Secure state can be translated to either the Secure or the Non-secure address map.

## 4 关于地址转换

Address translation is the process of mapping one address type to another, for example, mapping VAs to IPAs, or mapping VAs to PAs. A translation table defines the mapping from one address type to another, and a Translation table base register indicates the start of a translation table. Each implemented MMU shown in VMSA translation regimes, and associated MMUs on page B3-1309 requires its own set of translation tables.

For PL1&0 stage 1 translations, the mapping can be split between two tables, one controlling the lower part of the VA space, and the other controlling the upper part of the VA space. This can be used, for example, so that:

* one table defines the mapping for operating system and I/O addresses, that do not change on a context switch

* a second table defines the mapping for application-specific addresses, and therefore might require updating on a context switch.

The VMSAv7 implementation options determine the supported MMUs, and therefore the supported address translations:

* VMSAv7 without the Security Extensions

    Supports only a single PL1&0 stage 1 MMU. Operation of this MMU can be split between two sets of translation tables, defined by TTBR0 and TTBR1, and controlled by TTBCR.

* VMSAv7 with the Security Extensions but without the Virtualization Extensions

    Supports only the Secure PL1&0 stage 1 MMU and the Non-secure PL1&0 stage 1 MMU. Operation of each of these MMUs can be split between two sets of translation tables, defined by the Secure and Non-secure copies of TTBR0 and TTBR1, and controlled by the Secure and Non-secure copies of TTBCR.

* VMSAv7 with Virtualization Extensions

    The implementation supports all of the MMUs, as follows:

    - Secure PL1&0 stage 1 MMU

        Operation of this MMU can be split between two sets of translation tables, defined by the Secure copies of TTBR0 and TTBR1, and controlled by the Secure copy of TTBCR.

    - Non-secure PL2 stage 1 MMU

        The HTTBR defines the translation table for this MMU, controlled by HTCR.

    - Non-secure PL1&0 stage 1 MMU

        Operation of this MMU can be split between two sets of translation tables, defined by the Non-secure copies of TTBR0 and TTBR1 and controlled by the Non-secure copy of TTBCR.

    - Non-secure PL1&0 stage 2 control

        The VTTBR defines the translation table for this MMU, controlled by VTCR.

Figure B3-2 shows the possible memory translations in a VMSAv7 implementation that includes the Virtualization Extensions, and indicates the required privilege level to define each set of translation tables:

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/ARMv7_A_picture_B_3_2.PNG">

In general:

* the translation from VA to PA can require multiple stages of address translation, as Figure B3-2 shows
* a single stage of address translation takes an input address and translates it to an output address.

A full translation table lookup is called a `translation table walk`. It is performed automatically by hardware, and can have a significant cost in execution time. To support fine granularity of the VA to PA mapping, a single input address to output address translation can require multiple accesses to the translation tables, with each access giving finer granularity. Each access is described as a level of address lookup. The final level of the lookup defines:

* the required output address
* the attributes and access permissions of the addressed memory.

Translation Lookaside Buffers (TLBs) reduce the average cost of a memory access by caching the results of translation table walks. TLBs behave as caches of the translation table information, and the VMSA provides TLB maintenance operations for the management of TLB contents.

> The ARM architecture permits TLBs to hold any translation table entry that does not directly cause a Translation fault or an Access flag fault.

To reduce the software overhead of TLB maintenance, the VMSA distinguishes between Global pages and Process-specific pages. The Address Space Identifier (ASID) identifies pages associated with a specific process and provides a mechanism for changing process-specific tables without having to maintain the TLB structures.

If an implementation includes the Virtualization Extensions, the virtual machine identifier (VMID) identifies the current virtual machine, with its own independent ASID space. The TLB entries include this VMID information, meaning TLBs do not require explicit invalidation when changing from one virtual machine to another, if the virtual machines have different VMIDs. For stage 2 translations, all translations are associated with the current VMID, and there is no concept of global entries.


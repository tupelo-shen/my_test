Hello guys!

Welcome to the fourth part of the “Hypervisor From Scratch”. This part is primarily about translating guest address through Extended Page Table (EPT) and its implementation. We also see how shadow tables work and other cool stuff.

First of all, make sure to read the earlier parts before reading this topic as these parts are really dependent on each other also you should have a basic understanding of paging mechanism and how page tables work. A good article is [here](https://www.triplefault.io/2017/07/introduction-to-ia-32e-hardware-paging.html) for paging tables.

Most of this topic derived from  **Chapter 28 – (VMX SUPPORT FOR ADDRESS TRANSLATION)** available at Intel 64 and IA-32 architectures software developer’s manual combined volumes 3.

The full source code of this tutorial is available on GitHub: [https://github.com/SinaKarvandi/Hypervisor-From-Scratch](https://github.com/SinaKarvandi/Hypervisor-From-Scratch)

> Note: Please keep in mind that hypervisors change during the time because new features added to the operating systems or using new technologies, for example, updates to Meltdown & Spectre have made a lot of changes to the hypervisors, so if you want to use Hypervisor From Scratch in your projects, researches or whatever, you have to use the driver from the latest parts of these tutorial series as this tutorial is actively updated and changes are applied to the newer parts (earlier parts keep untouched) so you might encounter errors and instability problems in the earlier parts thus make sure to use the latest parts in real-world projects.

Before starting, I should give my thanks to [Petr Beneš](https://twitter.com/PetrBenes), as this part would never be completed without his help.

Note: This part tends to give you basic information about EPT, the main implementation of EPT for our hypervisor is explained in [part 7](https://rayanfam.com/topics/hypervisor-from-scratch-part-7/).

# 1 Introduction

**Second Level Address Translation (SLAT)** or nested paging, is an extended layer in the paging mechanism that is used to map hardware-based virtualization virtual addresses into the physical memory.

**AMD** implemented **SLAT** through the **Rapid Virtualization Indexing (RVI)**  technology known as **Nested Page Tables** (NPT) since the introduction of its third-generation **Opteron** processors and microarchitecture code name **Barcelona**.  **Intel** also implemented **SLAT** in **Intel® VT-x technologies** since the introduction of microarchitecture code name **Nehalem** and its known as **Extended Page Table (EPT)** and is used in  Core i9, Core i7, Core i5, and Core i3 processors.

**ARM** processors also have some kind of implementation known as known as **Stage-2 page-tables**.

There are two methods, the first one is Shadow Page Tables and the second one is Extended Page Tables.

# 2 Software-assisted paging (Shadow Page Tables)

Shadow page tables are used by the hypervisor to keep track of the state of physical memory in which the guest thinks that it has access to physical memory but in the real world, the hardware prevents it to access hardware memory otherwise it will control the host and it is not what it intended to be.

In this case, VMM maintains shadow page tables that map guest-virtual pages directly to machine pages and any guest modifications to V->P tables synced to VMM V->M shadow page tables.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/hypervisor_from_scratch_4_1.png">

By the way, using Shadow Page Table is not recommended today as always lead to VMM traps (which result in a vast amount of VM-Exits) and losses the performance due to the TLB flush on every switch and another caveat is that there is a memory overhead due to shadow copying of guest page tables.

# 3 Hardware-assisted paging (Extended Page Table)

To reduce the complexity of Shadow Page Tables and avoiding the excessive vm-exits and reducing the number of TLB flushes, EPT, a hardware-assisted paging strategy implemented to increase the performance.

According to a VMware evaluation paper: “EPT provides performance gains of up to 48% for MMU-intensive benchmarks and up to 600% for MMU-intensive microbenchmarks”.

EPT implemented one more page table hierarchy, to map Guest-Virtual Address to Guest-Physical address which is valid in the main memory.

In EPT,

* One page table is maintained by guest OS, which is used to generate the guest-physical address.
* The other page table is maintained by VMM, which is used to map guest physical address to host physical address.

so for each memory access operation, EPT MMU directly gets the guest physical address from the guest page table and then gets the host physical address by the VMM mapping table automatically.

# 4 Extended Page Table vs Shadow Page Table 

EPT:

* Walk any requested address

    - Appropriate to programs that have a large amount of page table miss when executing
    - Less chance to exit VM (less context switch)

* Two-layer EPT

    - Means each access needs to walk two tables

* Easier to develop

    - Many particular registers
    - Hardware helps guest OS to notify the VMM

SPT:

* Only walk when SPT entry miss

    - Appropriate to programs that would access only some addresses frequently
    - Every access might be intercepted by VMM (many traps)

* One reference

    - Fast and convenient when page hit

* Hard to develop
    
    - Two-layer structure
    - Complicated reverse map
    - Permission emulation

# 5 Detecting Support for EPT, NPT

If you want to see whether your system supports EPT on Intel processor or NPT on AMD processor without using assembly (CPUID), you can download **coreinfo.exe** from Sysinternals, then run it. The last line will show you if your processor supports EPT or NPT.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/hypervisor_from_scratch_4_2.png">

# 6 EPT Translation

EPT defines a layer of address translation that augments the translation of linear addresses.

The extended page-table mechanism (EPT) is a feature that can be used to support the virtualization of physical memory. When EPT is in use, certain addresses that would normally be treated as physical addresses (and used to access memory) are instead treated as guest-physical addresses. Guest-physical addresses are translated by traversing a set of EPT paging structures to produce physical addresses that are used to access memory.

**EPT is used when the “enable EPT” VM-execution control is 1. It translates the guest-physical addresses used in VMX non-root operation and those used by VM entry for event injection.**

EPT translation is exactly like regular paging translation but with some minor differences. In paging, the processor translates Virtual Address to Physical Address while in EPT translation you want to translate a Guest Virtual Address to Host Physical Address.

If you’re familiar with paging, the 3rd control register (CR3) is the base address of PML4 Table (in an x64 processor or more generally it points to root paging directory), in EPT guest is not aware of EPT Translation so it has CR3 too but this CR3 is used to convert Guest Virtual Address to Guest Physical Address, whenever you find your target Guest Physical Address, it’s **EPT mechanism that treats your Guest Physical Address like a virtual address and the EPTP is the CR3. **

Just think about the above sentence one more time!

So your target physical address should be divided into 4 part, the first 9 bits points to EPT PML4E (note that PML4 base address is in EPTP), the second 9 bits point the EPT PDPT Entry (the base address of PDPT comes from EPT PML4E), the third 9 bits point to EPT PD Entry (the base address of PD comes from EPT PDPTE) and the last 9 bit of the guest physical address point to an entry in EPT PT table (the base address of PT comes form EPT PDE) and now the EPT PT Entry points to the host physical address of the corresponding page.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/Edge%20computing/XEN/%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/images/hypervisor_from_scratch_4_3.png">

You might ask, as a simple Virtual to Physical Address translation involves accessing 4 physical address, so what happens ?! 

The answer is the processor internally translates all tables physical address one by one, that’s why paging and accessing memory in a guest software is slower than regular address translation. The following picture illustrates the operations for a Guest Virtual Address to Host Physical Address.
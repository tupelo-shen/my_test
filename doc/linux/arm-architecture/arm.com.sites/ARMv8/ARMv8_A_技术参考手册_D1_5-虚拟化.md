[TOC]

## 1 虚拟化

The support for virtualization described in this section applies only to an implementation that includes EL2.

When enabled in the current Security state, EL2 provides a set of features that support virtualizing an Armv8-A implementation. The basic model of a virtualized system involves:

* A hypervisor, running in EL2, that is responsible for switching between virtual machines. A virtual machine comprises EL1 and EL0.

* A number of Guest operating systems. A Guest OS runs on a virtual machine in EL1.

* For each Guest operating system, applications, that run on the virtual machine of that Guest OS, usually in EL0.

> In some systems, a Guest OS is unaware that it is running on a virtual machine, and is unaware of any other Guest OS. In other systems, a hypervisor makes the Guest OS aware of these facts. The Armv8-A architecture supports both of these models.

The hypervisor assigns a VMID to each virtual machine.

EL2 supports Guest OS management and provides controls to:

* Provide virtual values for the contents of a small number of identification registers. A read of one of these registers by a Guest OS or the applications for a Guest OS returns the virtual value.

* Trap various operations, including memory management operations and accesses to many other registers. A trapped operation generates an exception that is taken to EL2. See Configurable instruction enables and disables, and trap controls on page D1-2510.

* Route interrupts to the appropriate one of:

    - The current Guest OS.
    - A Guest OS that is not currently running.
    - The hypervisor.

Armv8.1 introduces the Virtualization Host Extensions (VHE) that provide enhanced support for Type 2 hypervisors. For more information, see Virtualization Host Extensions on page D5-2787.

In an implementation that includes EL2:

* The implementation provides an independent translation regime for memory accesses from EL2, the EL2 translation regime. An implementation that includes FEAT_VHE also supports an alternative EL2&0 translation regime.

    > An implementation that includes FEAT_VHE can be configured so that the EL2&0 translation regime is used both for accesses from EL2 and for accesses from EL0.

* For the EL1&0 translation regime, address translation occurs in two stages:

    - Stage 1 maps the virtual address (VA) to an intermediate physical address (IPA). This is managed at EL1, usually by a Guest OS. The Guest OS believes that the IPA is the physical address (PA).

    - Stage 2 maps the IPA to the PA. This is managed at EL2. The Guest OS might be completely unaware of this stage.

* When FEAT_NV is implemented, a Guest Hypervisor can be run at EL1. For more information on how this affects address translation, see Nested virtualization on page D5-2793.

* When FEAT_NV2 is implemented, then accesses of EL1 and EL2 registers that would be trapped are instead transformed into memory accesses. For more information, see Enhanced support for nested virtualization on page D5-2795.

For more information on the translation regimes, see Chapter D5 The AArch64 Virtual Memory System Architecture.

## 2 EL2对异常模型的影响

An implementation that includes EL2 implements the following exceptions:

* HVC on page C6-1035.

* Traps to EL2. EL2 configurable controls on page D1-2516, describes these.

* All of the virtual interrupts:

    - Virtual SError.
    - Virtual IRQ.
    - Virtual FIQ.
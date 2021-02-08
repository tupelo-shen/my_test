[TOC]

The Linux kernel has always been an ideal place to implement monitoring/observability, networking, and security. Unfortunately this was often impractical as it required changing kernel source code or loading kernel modules, and resulted in layers of abstractions stacked on top of each other. eBPF is a revolutionary technology that can run sandboxed programs in the Linux kernel without changing kernel source code or loading kernel modules.

By making the Linux kernel programmable, infrastructure software can leverage existing layers, making them more intelligent and feature-rich without continuing to add additional layers of complexity to the system or compromising execution efficiency and safety.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_function_block.png">

eBPF has resulted in the development of a completely new generation of software able to reprogram the behavior the Linux kernel and even apply logic across multiple subsystems which were traditionally completely independent.

### Security

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_security_subblock.png">

Building on the foundation of seeing and understanding all system calls and combining that with a packet and socket-level view of all networking operations allows for revolutionary new approaches to securing systems. While aspects of system call filtering, network-level filtering, and process context tracing have typically been handled by completely independent systems, eBPF allows for combining the visibility and control of all aspects to create security systems operating on more context with better level of control.

### Tracing & Profiling

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_tracing_subblock.png">

The ability to attach eBPF programs to trace points as well as kernel and user application probe points allows unprecedented visibility into the runtime behavior of applications and the system itself. By giving introspection abilities to both the application and system side, both views can be combined, allowing powerful and unique insights to troubleshoot system performance problems. Advanced statistical data structures allow to extract meaningful visibility data in an efficient manner, without requiring the export of vast amounts of sampling data as typically done by similar systems.

### Networking

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_networking_subblock.png">

The combination of programmability and efficiency makes eBPF a natural fit for all packet processing requirements of networking solutions. The programmability of eBPF enables adding additional protocol parsers and easily program any forwarding logic to meet changing requirements without ever leaving the packet processing context of the Linux kernel. The efficiency provided by the JIT compiler provides execution performance close to that of natively compiled in-kernel code.

### Observability & Monitoring

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_monitor_subblock.png">

Instead of relying on static counters and gauges exposed by the operating system, eBPF enables the collection & in-kernel aggregation of custom metrics and generation of visibility events based on a wide range of possible sources. This extends the depth of visibility that can be achieved as well as reduces the overall system overhead significantly by only collecting the visibility data required and by generating histograms and similar data structures at the source of the event instead of relying on the export of samples.


## Introduction to eBPF

The following chapters are a quick introduction into eBPF. If you would like to learn more about eBPF, see the [eBPF & XDP Reference Guide](https://cilium.readthedocs.io/en/stable/bpf/). Whether you are a developer looking to build an eBPF program, or interested in leveraging a solution that uses eBPF, it is useful to understand the basic concepts and architecture.

### Hook Overview

eBPF programs are event-driven and are run when the kernel or an application passes a certain hook point. Pre-defined hooks include system calls, function entry/exit, kernel tracepoints, network events, and several others.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_introduce_hook.png">

If a predefined hook does not exist for a particular need, it is possible to create a kernel probe (kprobe) or user probe (uprobe) to attach eBPF programs almost anywhere in kernel or user applications.

### How are eBPF programs written?

In a lot of scenarios, eBPF is not used directly but indirectly via projects like [Cilium](https://ebpf.io/projects/#cilium), [bcc](https://ebpf.io/projects/#bcc), or [bpftrace](https://ebpf.io/projects/#bpftrace) which provide an abstraction on top of eBPF and do not require to write programs directly but instead offer the ability to specify intent-based definitions which are then implemented with eBPF.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_introduce_hook.png">

If no higher-level abstraction exists, programs need to be written directly. The Linux kernel expects eBPF programs to be loaded in the form of bytecode. While it is of course possible to write bytecode directly, the more common development practice is to leverage a compiler suite like [LLVM](https://llvm.org/) to compile pseudo-C code into eBPF bytecode.


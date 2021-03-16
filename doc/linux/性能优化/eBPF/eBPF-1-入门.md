[TOC]

The Linux kernel has always been an ideal place to implement monitoring/observability, networking, and security. Unfortunately this was often impractical as it required changing kernel source code or loading kernel modules, and resulted in layers of abstractions stacked on top of each other. eBPF is a revolutionary technology that can run sandboxed programs in the Linux kernel without changing kernel source code or loading kernel modules.

Linux内核是一个实施`监控/观测`、`网络通信`和`安全`的理想地方。但不幸的是，往往需要修改内核源代码或者加载内核模块才能实现。

By making the Linux kernel programmable, infrastructure software can leverage existing layers, making them more intelligent and feature-rich without continuing to add additional layers of complexity to the system or compromising execution efficiency and safety.

通过使Linux内核可编程，基础架构软件可以利用现有的层，使它们更智能、功能更丰富，而不必继续增加系统的复杂性层或降低执行效率和安全性。

<div align=center>
<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_function_block.png">
</div>

eBPF has resulted in the development of a completely new generation of software able to reprogram the behavior the Linux kernel and even apply logic across multiple subsystems which were traditionally completely independent.

eBPF导致了一种新的Linux内核的开发方式，可以重定义内核的行为，甚至可以在几个互相独立的子系统之上建立逻辑关系。

### 1 安全

<div align=center>
<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_security_subblock.png">
</div>

Building on the foundation of seeing and understanding all system calls and combining that with a packet and socket-level view of all networking operations allows for revolutionary new approaches to securing systems. While aspects of system call filtering, network-level filtering, and process context tracing have typically been handled by completely independent systems, eBPF allows for combining the visibility and control of all aspects to create security systems operating on more context with better level of control.

在查看并理解所有系统调用的基础上，并结合数据包和套接字层的网络操作，可以提供构建安全系统的革命性的新方法。传统意义上，系统调用过滤，网络过滤，进程上下文追踪等完全都是由独立的子系统进行处理，eBPF允许综合这些方面的视角和控制，在更高控制层面创建安全系统。

### 2 追踪和分析

<div align=center>
<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_tracing_subblock.png">
</div>

The ability to attach eBPF programs to trace points as well as kernel and user application probe points allows unprecedented visibility into the runtime behavior of applications and the system itself. By giving introspection abilities to both the application and system side, both views can be combined, allowing powerful and unique insights to troubleshoot system performance problems. Advanced statistical data structures allow to extract meaningful visibility data in an efficient manner, without requiring the export of vast amounts of sampling data as typically done by similar systems.

通过将eBPF程序添加到追踪（trace）点、内核和用户程序的探测（probe）点，将会带来前所未有的观察应用程序和系统运行时行为的视角。通过赋予应用和系统自省能力，将这两视角结合在一起，允许对系统性能问题进行强大而独特的诊断。高级的统计数据结构允许高效地抽取观察数据，而不用像传统上相似的系统需要导出大量的采样数据。

### 3 网络

<div align=center>
<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_networking_subblock.png">
</div>

The combination of programmability and efficiency makes eBPF a natural fit for all packet processing requirements of networking solutions. The programmability of eBPF enables adding additional protocol parsers and easily program any forwarding logic to meet changing requirements without ever leaving the packet processing context of the Linux kernel. The efficiency provided by the JIT compiler provides execution performance close to that of natively compiled in-kernel code.


可编程性和效率使eBPF天然适合于网络解决方案的所有包处理需求。eBPF的可编程性允许添加额外的协议解析器，并可以轻松地对任何转发逻辑进行编程，以满足不断变化的需求，而无需离开Linux内核的包处理上下文。JIT编译器提供的效率提供了接近于本机编译的内核代码的执行性能。

### 4 观察和监控

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_monitor_subblock.png">

Instead of relying on static counters and gauges exposed by the operating system, eBPF enables the collection & in-kernel aggregation of custom metrics and generation of visibility events based on a wide range of possible sources. This extends the depth of visibility that can be achieved as well as reduces the overall system overhead significantly by only collecting the visibility data required and by generating histograms and similar data structures at the source of the event instead of relying on the export of samples.

不依赖于操作系统提供的静态计数器和计量工具，eBPF允许用户自定义的度量标准的组合或者聚合，并且产生基于广泛来源的可见性事件。通过只收集所需的可见性数据，在事件的源头生成直方图和类似的数据结构，而不是依赖于样本的导出，这可以扩展可实现的可见性深度，并显著减少整个系统的开销。

## 5 eBPF简介

The following chapters are a quick introduction into eBPF. If you would like to learn more about eBPF, see the [eBPF & XDP Reference Guide](https://cilium.readthedocs.io/en/stable/bpf/). Whether you are a developer looking to build an eBPF program, or interested in leveraging a solution that uses eBPF, it is useful to understand the basic concepts and architecture.

下面我们简要描述一下eBPF。如果想要了解更多，请参考[eBPF & XDP Reference Guide](https://cilium.readthedocs.io/en/stable/bpf/)。不管你是想要构建eBPF程序的开发者，还是对使用eBPF感兴趣，理解基本概念和架构都是很有帮助的。

### 5.1 钩子的理解

钩子（英文名称为`Hook`），是eBPF程序（事件驱动）运行的切入点。当内核或应用程序传递某个钩子点的时候，eBPF程序开始运行。预定义的钩子包括系统调用，函数进入和退出，内核跟踪点，网络事件以及一些其它钩子。

eBPF programs are event-driven and are run when the kernel or an application passes a certain hook point. Pre-defined hooks include system calls, function entry/exit, kernel tracepoints, network events, and several others.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_introduce_hook.png">

If a predefined hook does not exist for a particular need, it is possible to create a kernel probe (kprobe) or user probe (uprobe) to attach eBPF programs almost anywhere in kernel or user applications.

如果已经定义好的钩子（hook）不能满足特殊需求，也可以在内核或应用程序中创建内核探测(kprobe)或用户探测(uprobe)，附加eBPF程序。

### 5.2 如何编写eBPF程序？

In a lot of scenarios, eBPF is not used directly but indirectly via projects like [Cilium](https://ebpf.io/projects/#cilium), [bcc](https://ebpf.io/projects/#bcc), or [bpftrace](https://ebpf.io/projects/#bpftrace) which provide an abstraction on top of eBPF and do not require to write programs directly but instead offer the ability to specify intent-based definitions which are then implemented with eBPF.

在许多场景下，是不能直接使用eBPF的，而是通过[Cilium](https://ebpf.io/projects/#cilium)、[bcc](https://ebpf.io/projects/#bcc)或[bpftrace](https://ebpf.io/projects/#bpftrace)这些工具间接使用eBPF，这些工具是对eBPF的更高层抽象，使用eBPF实现了某些特定的能力，而不要求编写程序。

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96/eBPF/images/eBPF_introduce_hook.png">

If no higher-level abstraction exists, programs need to be written directly. The Linux kernel expects eBPF programs to be loaded in the form of bytecode. While it is of course possible to write bytecode directly, the more common development practice is to leverage a compiler suite like [LLVM](https://llvm.org/) to compile pseudo-C code into eBPF bytecode.

如果没有这些抽象工具，就需要我们直接写程序了。Linux内核期望eBPF程序使用字节码的形式加载。所以，我们可以直接使用字节码进行编程，也可以利用[LLVM](https://llvm.org/)之类的编译工具，将伪C代码编译成eBPF字节码。


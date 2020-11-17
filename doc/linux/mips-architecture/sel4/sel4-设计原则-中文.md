[TOC]

最近，一直在研究`seL4`微内核架构。但是，对于其设计原则一直含含糊糊。正好读到了`seL4`之父[Gernot Heiser的博客](https://link.zhihu.com/?target=https%3A//microkerneldude.wordpress.com/)，有种醍醐灌顶之感。于是，拜读完大师的文章后，总结成文。是膜拜之情的流露，亦是**好记性不如烂笔头**的体现。

大师对于`seL4`微内核的总结简单来说，就是**六大原则，三不准则**：

* 六大原则：

    1. 验证
    2. 最小化
    3. 策略自由
    4. 性能
    5. 安全性
    6. Don’t pay for what you don’t use

* 三不准则

    1. Stopping you from shooting yourself in the foot
    2. Ease of use
    3. Hardware abstraction


# 1 原则

These are the drivers of seL4’s design.

## 1.1 验证

通过**形式化验证**一直是`seL4`的核心卖点，其努力证明自身是符合**功能安全**的。

形式化验证是影响设计的一个非常重要的因素，因为它打破了一些设计的折衷处理。比如说，为了提高系统性能，引入了并发系统。但是，并发系统很难被验证。于是，`seL4`的设计将一切并发性排除在外，并被证明，永远不会触发内核内部的异常。这不是说`seL4`不考虑内核的性能，而是尽量在提高性能的同时，保持干净整洁的设计。

形式化验证带来的另一个挑战就是微内核在演变的过程中，重新验证带来的巨大难度。所以，`seL4`微内核在设计的过程中，尽量考虑降低重新验证的复杂度，不得不放弃了一些设计。有时候不得不修改一些代码或者API，只是为了让重新验证更简单。

## 1.2 最小化

这个原则不是`seL4`所独有，也是其祖先`L4`微内核的核心原则。Liedtke在其著名的论文[On μ-kernel construction](https://www.cse.unsw.edu.au/~cs9242/19/papers/Liedtke_95.pdf)表述，

> A concept is tolerated inside the µ-kernel only if moving it outside the kernel, i.e., permitting competing implementations, would prevent implementation of the system’s required functionality.

这是一个非常理想化的目标。一方面，最小化是L4家族的根本设计理念；另一方面，最小化也是方便seL4做形式化验证的重要条件（文章[Verification cost grows roughly with the square of the code size](https://ts.data61.csiro.au/publications/nictaabstracts/Matichuk_MAJKS_15.abstract.pml)也验证了这个观点）。

为了保持最小化，seL4微内核除了基本的中断控制器、定时器和MMU的驱动程序之外，其余的工作都放到了用户态，甚至是内核的内存管理都由用户态进行管理。

## 1.3 通用性

通用性，这是L4微内核和其它内核的一个重要特性。虽然说，最小化是L4家族的核心设计理念，但是，seL4微内核的设计者也在努力解决其通用性。比如说，seL4微内核[新引入的一个调度模型](https://ts.data61.csiro.au/publications/csiroabstracts/Lyons_MAH_18.abstract.pml)，支持更为广泛的实时（RT）操作系统，尤其是混合临界系统（MCS），严格的RT代码和不受信任的代码共存。事实证明，MCS内核使用API解决了许多问题。

## 1.4 策略自由

机制和策略是一对孪生的矛盾体，其相爱相杀。在[1975年的一篇关于`Hydra OS`的论文](https://www.systems.ethz.ch/sites/default/files/file/aos2012/Reading/week9/Policy%20mechanism%20separation%20in%20Hydra.pdf)中，就充分论证了**机制-策略**分离的原则。同样的思想，也出现在[Brinch Hansen’s 1970 Nucleus paper](http://ddhf.dk/site_dk/rc/pbh/MonitorPaper.pdf)这篇文章中，它描述了所有微内核的最初架构。其实，机制就是最小化和通用性的结果，而策略则在此基础上构建OS的其它功能。

seL4微内核的内存管理就是将内核的内存管理策略拿到了内核之外，交给用户自由实现。与此相似的是，MCS模型也是将时间的管理放权给了用户态，由其直接处理。这些都是将宏内核的一些机制实现，变成了策略实现的案例。

## 1.5 性能

性能，尤其是IPC通信机制的性能，是L4微内核的核心驱动力：正如`Liedtke`在其93年的论文[Improving IPC by kernel design](https://www.cse.unsw.edu.au/~cs9242/19/papers/Liedtke_93.pdf)中所说的，IPC性能是最主要的。这么说的原因就是，Liedtke最初的微内核表示，微内核可以运行很快（参考[allowed the construction of performant systems on top](http://os.itec.kit.edu/downloads/publ_1997_haertig-ua_ukernel-performance.pdf)一文）。但是，在早期的微内核基础上，构建可用系统的尝试失败了，比如-Mach，IPC带来的负载太高了。

大师声称，seL4微内核的设计从开始就立足于真实世界，绝不在性能设计上妥协。在其最早的[关于seL4论文](https://ts.data61.csiro.au/publications/nictaabstracts/Klein_EHACDEEKNSTW_09.abstract.pml)中，其微内核的性能比最快的内核损失小于10%。经过这么多年的发展，相信现在的`seL4`运行更加丝滑！😊！

seL4的口号就是，**Security is no excuse for bad performance!**。so、现在除了形式化验证之外，性能成为了seL4区别于其它微内核的一大特性。它真正奠定了基准。

### 1.5.1 Performance: Focus on the hot code paths

This performance aspect results from the observation that in any system, some operations are used more frequently than others, and overall performance can be maximised by shifting cost from the frequently used “hot” operations to infrequently-used ones.

The prime example of a hot operation is IPC: in a microkernel-based system, all system services are provided by user-level servers that are invoked by IPC, so this is the predominant operation. Notifications, which are semaphore-style synchronisation primitives, are also frequently used. Handling of exceptions and interrupts is also frequently performance-critical (eg exceptions are used for emulation/virtualisation), but exceptions are mapped onto IPC and interrupts onto Notifications, so optimising those will benefit exception and interrupt handling.

But not all of seL4’s IPC functionality is equally performance-critical. IPC allows transferring capabilities, which is an inherently more costly operation (requires Cspace manipulations). If the “simple”, function-call-style IPC can be made faster while slightly penalising the more complex variants, then this is a winner. It’s the idea behind the IPC fast path, which does the minimal checking that all its preconditions are true, and then executes a very tight code sequence.

Where is the boundary between “hot” and “cold” code? A good way to think of this is in term of changes to kernel state, i.e. the set of data structures held by the kernel. Every system call changes kernel state (even if only to update accounting information). But there are simple invocation sequences that return the kernel into what I’ll call the same logical state as before the sequence began, meaning that the kernel states only differ in accounting information (execution time charged to threads).

For example, the basic RPC-like server invocation (aka protected procedure call) results in the following changes of logical kernel state, assuming we start with the server blocked on its request endpoint in the receive phase of seL4_ReplyRecv():

例如，类似于RPC通信的服务器，会导致如下所示的内核状态的逻辑变化，假设我们请求服务器在`seL4_ReplyRecv()`调用的接收阶段阻塞于它所请求的端点能力上。

1. 客户端执行`seL4_Call()`，内核将客户端的状态更改为blocked状态；
2. 内核设置reply对象的状态；
3. 内核将调度上下文从客户端移动到服务器端；
4. 内核解除服务器的阻塞，让其执行请求；
5. 处理完请求后，服务器执行seL4_ReplyRecv()调用，将reply对象发送给客户端，内核接着阻塞服务器；
6. 内核返回调度上下文给客户端；
7. 内核解除客户端的阻塞，让其继续执行。

As far as the kernel is concerned, we’re now back at the initial logical state, and state change was temporary, and should be made fast. This is in contrast to typical Cspace manipulations: these are reversible too (eg. a capability can be transferred and later revoked), but not in a simple (in the user view primitive) operation such as a round-trip IPC. An IPC that transfers a capability will leave the receiver’s Cspace changed, and that change is typically long-lived. It’s also inherently more expensive than a basic IPC.

Such logical-state–preserving operations may be accelerated by a degree of laziness, as exemplified in the scheduler optimisation called Benno scheduling: When unblocking a thread on an IPC receive, we don’t immediately insert it into the ready queue, as it is likely to be blocked again soon, which would undo the queue operation just performed. Instead we only insert the presently running thread into the ready queue if it gets preempted. This avoids any queue-manipulation operations during the logical-state–preserving IPC round-trip.

### 1.5.2 Performance: Don’t pay for what you don’t use

While much of the above principles are discussed in several places, including our [20-years experience paper](https://ts.data61.csiro.au/publications/nictaabstracts/Heiser_Elphinstone_16.abstract.pml), this aspect is somewhat buried inside the brains of the designers, but is important for understanding some design decisions. A feature that may benefit a particular use case may have a (small but noticeable) cost to other, frequent use cases.

An example is the “long IPC” that was a feature of original seL4. It supported copying of large message buffers by the kernel, which seems like a good optimisation for sharing bulk data. But the only actual use case was Posix emulation (specifically broken aspects of Posix). But there is a cost even if you don’t use it: the performance cost of extra checks on the IPC path, and the complexity cost of the kernel having to deal with nested exceptions (page faults happening during long IPC). In the end we decided it was a bad design, and a gross violation of minimality (the copying can be done by a trusted user-level server).

A similar argument is behind not supporting scalability of the kernel for large number of cores. [On closer consideration, this is a bad idea](https://ts.data61.csiro.au/publications/nictaabstracts/Peters_DEH_15.abstract.pml): migrating even a single cache line from one end of a manycore chip to the other takes 100s of cycles, of the order of a the cost of a complete IPC. So, if you really want this, you should implement it at user level (i.e. a multikernel design). Sharing a kernel image makes sense where the cost of using a multicore configuration would be significantly higher when implementing it at user level, as is the case in a closely-coupled multicore, where the cores share an L2 cache. This is the case we support, while we have no plans scaling the single-kernel-image design to loosely-coupled multicores (not sharing an L2).

## 1.6 Security

Security was always high on the list of requirements for L4 kernels, although in the past came in many cases with a degradation in performance and flexibility. Even before seL4 it was the reason for adopting capabilities for access control, first in our OKL4 microkernel.

In seL4 security is a core principle, the kernel is fundamentally designed for providing the strongest possible isolation. It is the main driver behind seL4’s radical approach to resource management: after booting, the kernel never allocates any memory, it has no heap. Instead, when performing an operation that requires kernel memory (e.g. creating an address space, which requires memory for page tables) the caller must provide this memory explicitly to the kernel (by a process called retyping, which converts user-controlled Untyped, i.e. free, memory into kernel memory).

The model makes management of kernel memory the responsibility of (trusted and protected) usermode processes. The model not only enhances minimality and policy-freedom, it is the core enabler of our proofs of security enforcement. Its aim is to make it possible to reason (formally) about the security properties of seL4-based systems.

Most recently we have taken a step further in [extending isolation (and thus security) to timing properties](https://ts.data61.csiro.au/publications/csiroabstracts/Ge_YCH_19.abstract.pml), as a principled way of eliminating timing channels. This is still very much a research topic and not yet ready for the production kernel.

### 1.6.1 Security: Least privilege (POLA)

An important part of (design for) security is the principle of least privilege, also known as the principle of least authority (with the catchy acronym POLA). It means that any component should only ever have the privileges (power) it needs to do its job, but not more. The fine-grained access control provided by capabilities is a great enabler of POLA, which is why we switched to caps even before seL4.

### 1.6.2 Security: Delegation and revocation

Least privilege is potentially at odds with performance, as a simple way to minimise authority given to a component is to have it to obtain explicit approval from an external (trusted) security monitor for everything it does – clearly not a good approach. Furthermore, a security-oriented design is likely hierarchical: a subsystem has limited privileges, and it contains sub-sub-systems with even more limited privileges. Enforcing such internal boundaries in subsystems should not be the duty of the top-level (most privileged) component.

This calls for mechanisms for delegating the exercise of (reduced) privilege. If a component has rights to a resource, such as physical memory, it should be able to create an (isolated) subcomponent that with the same or reduced rights to that resource. And it must be able to revoke the delegation.

Capabilities, as seL4 provides them, support effective delegation of (partial) privilege. For example, if you want to have a subsystem that manages a subset of physical resources autonomously then you can supply it with some Untyped memory, which it can then manage without interfering with other components’ memory use. If you want to restrict the component (eg an OS personality) further, you can instead supply it with with caps to pools of TCBs, frames, address spaces etc. It can then manage the pools, but would have to appeal to a higher authority to move memory between the pools. In short, the authority give to the subsystem can be made to match the system’s resource management policy.

# 2 Anti-Principles

There are a number of properties many people would like from the API, but we are explicitly not providing them for reasons I will explain.

## 2.1 Stopping you from shooting yourself in the foot: not the kernel’s job

In fact, the kernel does its best not to limit the size of guns you can use; aiming the gun is your problem (see policy-freedom). Specifically, it’s the system-designer’s job to ensure that dangerous tools can only be used by those who are trusted (ideally proved) to use them responsibly.

However, stopping someone without explicit authorisation from shooting you in the back is the kernel’s job; that’s a core aspect of security.

## 2.2 Ease of use: not a goal of a good mirokernel API

seL4 is a minimal wrapper around hardware that provides just enough so you can build secure and performant systems on top. Consequently, you shouldn’t expect it to be easier to use than bare metal.

Usability is important for building practical systems, of course, but the way to achieve it is by providing higher-level abstraction layers. These are inevitably optimised for particular classes of use cases, introduce policy and reduce generality. This is fine, as the abstraction layer can be replaced if you want to implement a different use case (see the definition of Minimality).

## 2.3 Hardware abstraction: not a goal of the seL4 API

This should be obvious from the above: HW abstraction supports ease of use and introduces overhead. But it will also hide details that are important in some cases.

An example is memory management. Different architectures specify different page-table formats. Trying to fit them under the same abstraction would inevitably lose detail, and prevent seL4-based systems from making full use of whatever hardware support there may be. An obvious example would be architectures that use hashed page tables, such as certain versions of the Power architecture. Forcing a hierarchical page-table structure would imply inefficiencies.

All presently supported architectures use hierarchical page tables, so we could be tempted to ignore that particular aspect. But even on our supported architectures (x64, Arm and RISC-V) there are subtle difference in the semantics of page table which would be unwise to hide.

# 3 Trade-offs

Like just about everything in systems, our principles are frequently in conflict, meaning it is necessary to find a design that represents the best trade-off between them. Especially performance and verifiability frequently run against an otherwise attractive choice. Conversely, performance considerations occasionally lead to non-minimal features (and increased pain for our verifiers).

Clearly, **these principles, other than security and verification, are not absolute**. In this sense, good microkernel design is still an art, and understanding the trade-offs is key to good results. **But these principles are excellent and time-honoured drivers of good design**.
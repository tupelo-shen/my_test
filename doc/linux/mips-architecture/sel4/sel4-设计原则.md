seL4 has been our team’s greatest achievement, but it didn’t fall out of the sky: it was the result of 15 years of research, and has evolved further for the past 10 years.

From the beginning, the design of seL4 has been driven by a number of principles. But a recent internal discussion about some fine points of the spec (as well as some discussions with externals) reminded me that some of these principles are in the minds of the designers but not really documented. This can lead to people (internal as well as external to Trustworthy Systems) arguing for APIs that are not in the spirit of seL4. Hence I’ll try to write up these principles.

The core drivers of the seL4 API are:

1. Verification
2. Minimality
3. Policy freedom
4. Performance
5. Security
6. Don’t pay for what you don’t use

Then there are a few non-goals:

1. Stopping you from shooting yourself in the foot
2. Ease of use
3. Hardware abstraction


# 1 Principles

These are the drivers of seL4’s design.

## 1.1 Verification

This is most obvious, and is seL4’s core claim to fame: we have a proof that the implementation adheres to the spec (functional correctness) as well as proofs of higher-level security properties. And with any changes to the kernel, the proof needs to be updated to re-establish functional correctness.

Verification has a significant impact on some design decisions – it shifts the trade-off between alternative designs. For example, concurrent systems are notoriously hard to verify, so we do everything to keep concurrency out of the kernel. Examples include the virtual TCB array and “long IPC” that was used in the original L4. These designs led to exceptions happening while in kernel mode. Such nested exceptions introduce concurrency, and are not acceptable for seL4, which is designed (and proved) never to trigger an exception inside the kernel. Interestingly, [these problematic designs were already abandoned before we did seL4](https://ts.data61.csiro.au/publications/nictaabstracts/Heiser_Elphinstone_16.abstract.pml) (for simplicity and performance reasons), so verification did not force an inferior design. But it is an important factor in every design consideration, and certainly forces us to think carefully about every design choice. And this careful consideration has frequently led to a cleaner design.

A core challenge during kernel evolution is that re-verification takes much longer than the code changes that triggers it. We need to minimise the re-verification cost, so it is critical that code changes don’t make the life of our verifiers harder than necessary. In many cases, the impact of a code change on the re-verification effort is hard to predict, even for our experienced kernel engineers (and much harder for externals, which can sometimes lead to frustration, if externals suggest a change we have to decline as it would create a verification headache).

We handle this by [keeping verification engineers in the loop](https://research.csiro.au/tsblog/lockstep-kernel-verification/) when proposing changes to the kernel. This helps a lot, but does not eliminate surprises. Many verification challenges only show up when trying to re-prove kernel invariants, and frequently these lead to changes in the code, sometimes even the API, to make verification easier.

## 1.2 Minimality

This principle pre-dates seL4 by many years, and is the core principle of the original L4 microkernel, the ancestor of seL4 (and the whole [L4 microkernel family](https://en.wikipedia.org/wiki/L4_microkernel_family)). It was expressed by Liedtke in his seminal Hall-of-Fame paper [On μ-kernel construction](https://www.cse.unsw.edu.au/~cs9242/19/papers/Liedtke_95.pdf) as:

> A concept is tolerated inside the µ-kernel only if moving it outside the kernel, i.e., permitting competing implementations, would prevent implementation of the system’s required functionality.

This is a fairly categorical aim, and really an ideal, in that every kernel I’ve seen violates it at least a bit. But it is an excellent design driver, that has served the L4 microkernel family well for over a quarter of a century. With seL4 it has taken on additional significance: [Verification cost grows roughly with the square of the code size](https://ts.data61.csiro.au/publications/nictaabstracts/Matichuk_MAJKS_15.abstract.pml), an excellent motivation for keeping it small!

Aspects of minimality are that we have no device drivers in the kernel (other than the interrupt-controller and a timer), and seL4’s extreme approach to memory management, where even kernel memory is managed by user-level code.

## 1.3 Generality

This is also an original aim of L4 (and microkernels in general): be a (trustworthy) foundation for a very large class of system designs. It is the principle that stops minimality from reducing the kernel size to zero.

In reality, there have always been use cases that have been better supported by the kernel than others. And much of our research over the years has been driven by addressing this: evolving the kernel to support a growing class of use cases well. For example, the biggest change to seL4’s functionality has been the [introduction of a new scheduling model](https://ts.data61.csiro.au/publications/csiroabstracts/Lyons_MAH_18.abstract.pml), with the aim of supporting a wider class of real-time (RT) systems, especially mixed-criticality systems (MCS), where critical RT code co-exists with untrusted code. As it turns out, the MCS kernel cleans up a number of other issues with the API, which make it an improvement even if your use case isn’t an MCS.

## 1.4 Policy freedom

This principle predates even the original L4 by decades. Policy-mechanism separation was explicitly stated as a principle in [a 1975 paper about the Hydra OS](https://www.systems.ethz.ch/sites/default/files/file/aos2012/Reading/week9/Policy%20mechanism%20separation%20in%20Hydra.pdf), but the idea is already clearly contained in [Brinch Hansen’s 1970 Nucleus paper](http://ddhf.dk/site_dk/rc/pbh/MonitorPaper.pdf), which describes the arch-ancestor of all microkernels. It really is a consequence of minimality and generality: if you want to keep your kernel minimal yet general, you must focus on the basic mechanisms, and build everything else on top.

seL4’s memory-management model takes a big chunk of (kernel memory-management) policy out of the kernel. Similarly for the MCS model, which introduces principled, capability-authorised user-level management of time as just another first-class resource.

## 1.5 Performance

Performance, especially of the critical IPC operation, has always been a core driver of L4 kernels: IPC performance is the master, as Liedtke expressed it in his ’93 paper [Improving IPC by kernel design](https://www.cse.unsw.edu.au/~cs9242/19/papers/Liedtke_93.pdf). The reason was that attempts to build usable systems on earlier microkernels, especially Mach, failed because the kernel introduced too much overhead. Liedtke’s original kernel showed that microkernels can be fast, and [allowed the construction of performant systems on top](http://os.itec.kit.edu/downloads/publ_1997_haertig-ua_ukernel-performance.pdf).

From the beginning, seL4 was designed to be suitable for real-world use, and we therefore considered an uncompromising design for performance as essential. My message to the team at the beginning of the project was “I will not consider the project a success if we lose more than 10% performance compared to our fastest kernel to date.” At the time of [the original seL4 paper](https://ts.data61.csiro.au/publications/nictaabstracts/Klein_EHACDEEKNSTW_09.abstract.pml), we were right at that 10% limit. But further optimisation got us to the point where seL4 became faster than all our earlier kernels.

This became the tagline for seL4: Security is no excuse for bad performance! Besides its verification, performance is what sets seL4 apart from all other microkernels – it really sets the benchmark.

### 1.5.1 Performance: Focus on the hot code paths

This performance aspect results from the observation that in any system, some operations are used more frequently than others, and overall performance can be maximised by shifting cost from the frequently used “hot” operations to infrequently-used ones.

The prime example of a hot operation is IPC: in a microkernel-based system, all system services are provided by user-level servers that are invoked by IPC, so this is the predominant operation. Notifications, which are semaphore-style synchronisation primitives, are also frequently used. Handling of exceptions and interrupts is also frequently performance-critical (eg exceptions are used for emulation/virtualisation), but exceptions are mapped onto IPC and interrupts onto Notifications, so optimising those will benefit exception and interrupt handling.

But not all of seL4’s IPC functionality is equally performance-critical. IPC allows transferring capabilities, which is an inherently more costly operation (requires Cspace manipulations). If the “simple”, function-call-style IPC can be made faster while slightly penalising the more complex variants, then this is a winner. It’s the idea behind the IPC fast path, which does the minimal checking that all its preconditions are true, and then executes a very tight code sequence.

Where is the boundary between “hot” and “cold” code? A good way to think of this is in term of changes to kernel state, i.e. the set of data structures held by the kernel. Every system call changes kernel state (even if only to update accounting information). But there are simple invocation sequences that return the kernel into what I’ll call the same logical state as before the sequence began, meaning that the kernel states only differ in accounting information (execution time charged to threads).

For example, the basic RPC-like server invocation (aka protected procedure call) results in the following changes of logical kernel state, assuming we start with the server blocked on its request endpoint in the receive phase of seL4_ReplyRecv():

1. client performs seL4_Call(), kernel changes client state to blocked
2. kernel sets reply object state
3. kernel moves scheduling context from client to server
4. kernel unblocks server, which executes the request
5. having handled the request, server performs seL4_ReplyRecv() on the reply object to respond to the client, kernel blocks server
6. kernel returns scheduling context to client
7. kernel unblocks client, which continues executing.

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
[TOC]

## 1 微内核

对于操作系统内核，我们并不陌生，比如，linux内核，都是高度复杂的软件，包括管理资源所需要的一切服务，访问硬件，存储信息到文件系统，处理网络数据包，控制用户态进程等。因此，内核需要控制整个机器的特权。

<img src="1_1">

The high functional requirements and the broad range of existing hardware causes such a kernel to grow huge. Users expect different network cards to be supported, different file systems, a wide variety of network protocols, and a lot more. Consequently, a typical Linux kernel contains far more than 500,000 lines of code. It is impossible to fully avoid bugs and security leaks at a system of such scale. Bug-prone code in the kernel can corrupt the proper operation of the whole system and thus, can impose fatal consequences. As illustrated in the right side of the figure, a broken network driver can corrupt the whole operating system.

With virtual address spaces, modern operating systems and hardware platforms provide mechanisms to isolate concurrently running user applications. Each user application runs within a dedicated address space and interacts in a safe, well-controlled manner with other user applications only via mechanisms provided by the kernel. This way, the kernel effectively protects user applications from each other.

>   总结：
> 
>   1. 内核代码越来越复杂，容易产生bug，从而影响整个系统；
> 
>   2. 虚拟地址保护了用户进程之间的安全隔离，但无法保证内核、驱动、协议栈之间的隔离；


<img src="1_2">

Microkernel-based systems use these techniques not only for user applications but also for device drivers, file systems, and other typical kernel-level services. Therefore, the effect of a bug-prone component is locally restricted. The microkernel withdraws all non-needed privileges from each component and thereby shrinks the overall complexity of code running in privileged mode by an order of magnitude compared to a monolithic kernel. For example, a typical microkernel of the L4 family is implemented in less than 20,000 lines of code. As illustrated in the figure on the right, all components are protected from each other by address spaces. Thus, one component cannot inspect or corrupt other components without proper authorization. Communication between components can only happen by using communication mechanisms provided by the microkernel. If one component of the system gets corrupted by a bug or an attack, the fault is locally restricted to the broken component. Furthermore, the microkernel enforces CPU time scheduling and can grant guaranteed processing time to user processes. No de-privileged system component is able to violate such guarantees. Therefore, a microkernel can safely execute sensitive applications, de-privileged system services, and large untrusted applications side by side on one machine.

Whereas microkernels deliver fault isolation and separation of concerns well in theory, in practice, this is only half of a solution because all those de-privileged components must be appropriately organized to make the approach effective. Because, by definition, a microkernel does not implement policy but only mechanisms, the policy must be provided by someone else. One approach is the introduction of a central policy management component with a global view on the whole system, controlled by a specially-privileged administrator. The complexity and manageability of a centralized policy, however, depends on the scale of the system.

In contrast, Genode extends the microkernel idea of de-composing the operating system code to the idea of de-composing also the system policy by imposing a strict organizational structure onto each part of the system.

> **微内核特点**：
> 
>   1. 对系统内所有组件提供了隔离，不管是内核、驱动、协议栈、文件系统还是用户态应用程序；
>   
>   2. 高度抽象、简化内核代码，降低其复杂性；
>   
>   3. 调度路径短，进程的执行时间能够得到保证；
> 
> **微内核缺点**：
> 
>   1. 微内核因为高度抽象，只提供机制，并不提供策略；
> 
>   2. 一种方法就是建立集中式的中央策略管理组件，具有整个系统的全局视图；缺点是，如果系统规模过大，这个组件的复杂性和可管理性就会增加；
> 
>   3. 微内核的缺点正是`Genode`诞生的原因。设计思想就是，借鉴微内核拆解操作系统代码为各个组件的方法，将整个系统按照严格的组织结构进行分解。

## 2 组织结构

For understanding the architecture of Genode, it is helpful to draw an analogy to the internal operation of a company. The key of how a company successfully scales with a growing number of employees is its organizational structure. The same applies for the scalability of an operating system with regard to the richness of its functionality. To keep a high number of employees manageable, successful companies implement policies in a distributed way. Rather than having one single person dictating everybody's tasks directly, policy decisions are taken and applied hierarchically. The heart of the Genode OS architecture is an organizational structure that loosely resembles the structure of a hierarchically organized company.

> Genode组织结构类似于松散的公司分层组织结构。

<img src="1_3">

As illustrated in the figure on the right, Genode organizes processes as a tree. The red arrows symbolize that child processes are created out of the resources of their respective parents. When creating a child process, a parent fully defines the virtual environment in which the new process gets executed. The child, in turn, can further create children from its assigned resources, thereby creating an arbitrarily structured subsystem. Each parent maintains full control over the subsystems it created and defines their inter-relationship, for example by selectively permitting communication between them or by assigning physical resources. The parent-child interface is the same at each hierarchy level, which makes this organizational approach recursively applicable. Using this recursive property as a tool clears the way for separating policy and mechanism in very flexible ways and facilitates the strict separation of duties.

For an application of Genode's organizational structure to a real-world scenario refer to Stefan Kalkowski's [diploma thesis](http://genode-labs.com/publications/kalkowski-diplom-2006.pdf).

## 3 主要特点

#### 3.1 最小特权原则

When created, a new process is only allowed to communicate with its immediate parent. It is otherwise completely isolated from the rest of the system. The parent can selectively assign communication rights to enable a child to communicate with the outside of the child's subsystem.

> 1. 创建的子进程只能与它的直接父进程进行通信，与系统的其它部分是隔离的；
> 
> 2. 父进程可以选择性的赋予子进程与其兄弟进程的通信；
> 
> 3. 创建的子进程可以与它的子进程进行通信；

#### 3.2 避免使用全局变量，将隔离最大化

To maximize the isolation between unrelated subsystems, Genode does not rely on globally visible names or IDs. Following the idea that launching a directed attack is difficult if the target is invisible, Genode operates without the need for any visible global information such as file names, process IDs, thread IDs, device nodes, or port numbers. Genode is explicitly designed for kernels with support for kernel-protected localized names (capabilities).

#### 3.3 通过特定于应用的可信计算基来控制复杂度

Because software complexity correlates with the likelihood for bugs, having security-sensitive functionality depending on high-complexity software is risky. The term trusted_computing_base (TCB) was coined to describe the amount of code that must not be compromised to uphold security. In addition to the code of the sensitive application, the TCB comprises each system component that has direct or indirect control over the execution of the application (affecting availability and integrity) or that can access the processed information (affecting confidentiality and integrity). On monolithic OSes, the TCB complexity can be regarded as a global system property because it is dominated by the complexity of the kernel and the privileged processes, which are essentially the same for each concurrently executed application. On Genode, the amount of security-critical code can largely differ for each application depending on the position of the application within Genode's process tree and the used services. To illustrate the difference, an email-signing application executed on Linux has to rely on a TCB complexity of millions of lines of code (LOC). Most of the code, however, does not provide functionality required to perform the actual cryptographic function of the signing application. Still, the credentials of the user are exposed to an overly complex TCB including the network stack, device drivers, and file systems. In contrast, Genode allows the cryptographic function to be executed with a specific TCB that consists only of components that are needed to perform the signing function. For the signing application, the TCB would contain the microkernel (20 KLOC), the Genode OS framework (10 KLOC), a minimally-complex GUI (2 KLOC), and the signing application (15 KLOC). These components stack up to a complexity of less than 50,000 LOC.

Genode tailors the trusted computing base for each application individually. The figure on the right illustrates the TCB of the yellow marked process. Naturally, it contains the hierarchy of parents and those processes that provide services used by the application (the left component at the third level).

#### 3.4 物理资源的平衡和统计

The majority of current operating systems try to hide the fact that physical resources such as memory, network bandwidth, or graphics bandwidth are limited. For example, on such systems, processes expect to be able to allocate memory from an unlimited pool whereas the operating system puts a swapping mechanism in place to uphold the illusion of having unlimited memory. This approach, however, sacrifices deterministic system behaviour. Furthermore, resources used by the kernel on behalf of applications, for example the resources needed by device drivers, are not accounted to the end-using applications at all. To resort to the analogy of operating a company, giving departments the illusion of having unlimited resources and refusing to track resource usage would ultimately lead to unbalanced finances.

In contrast, Genode facilitates the explicit accounting of physical resources to processes. Similar to an upper-level manager who trades company resources among his managed departments, each parent in Genode's process tree trades resources among its children. Genode enables the lending and regaining of physical resources between clients and servers according to a chain of command. For example, for the service of providing a GUI window, a GUI server can demand the client to lend a negotiated amount of memory needed for managing the window during the window's lifetime. Genode provides this resource-trading mechanism across arbitrary hierarchy levels. Thereby, physical resources get correctly accounted at all times. Even though Genode allows for dynamic workload and arbitrarily structured process trees, the system remains deterministic.

#### 3.5 虚拟化技术促进了应用程序的兼容性

Because operating systems without applications are barely useful, compatibility to existing applications is a major concern. In the past, the concern of losing compatibility often prevented design legacies to be disposed of. Modern virtualization technology is the key to overcome this problem. In an preliminary study, a user-level version of the Linux kernel (L4Linux) was successfully ported to the Genode OS Framework running on a L4 kernel. This study suggests that Genode combined with an appropriate kernel or hypervisor is suited as a virtual-machine hosting platform. As illustrated in the figure, from an organizational point of view, a virtual machine is implemented as a leaf node in Genode's process tree. Genode not only facilitates the use of virtual machines for application compatibility but also the re-use of existing device drivers. The baby Tux in the figure symbolizes an original Linux device driver being executed in a device-driver environment.

## Genode architecture documents

In the software world, high complexity of a problem solution comes along with a high risk for bugs and vulnerabilities. This correlation is particular perturbing for todays commodity operating systems with their tremendous complexity. The numerous approaches to increase the user's confidence in the correct functioning of software comprise exhaustive tests, code auditing, static code analysis, and formal verification. Such quality-assurance measures are either rather shallow or they scale badly with increasing complexity.

The Genode OS architecture focuses on the root of the problem by providing means to minimize the underlying system complexity for each security-sensitive application individually. On the other hand, we want to enable multiple applications to execute on the system at the same time whereas each application may have different functional requirements from the operating system. Todays operating systems provide a functional superset of the requirements of all applications and thus, violate the principle of minimalism for each single application. Genode resolves the conflict between the principle of minimalism and the versatility of the operating system by decomposing the operating system into small components and by providing a way to execute those components isolated and independent from each other. Components can be device drivers, protocol stacks such as file systems and network stacks, native applications, and containers for executing legacy software. Each application depends only on the functionality of a bounded set of components that we call application-specific trusted computing base_(TCB). If the TCBs of two applications are executed completely isolated and independent from each other, we consider both TCBs as minimal.

In practice however, we want to share physical resources between multiple applications without sacrificing their independence. Therefore, the operating-system design has to enable the assignment of physical resources to each application and its TCB to maintain independence from other applications. Furthermore, rather than living in complete isolation, components require to communicate with each other to cooperate. The operating-system design must enable components to create other components and get them to know each other while maintaining isolation from uninvolved parts of the system.


The book "Genode Foundations" describes the Genode OS Framework in a holistic and comprehensive way. It equips the reader with a thorough understanding of the architecture, assists developers with the explanation of the development environment and system configuration, and provides a look under the hood of the framework. Furthermore, it contains the specification of the framework's programming interface. [Download the book](http://genode.org/)...
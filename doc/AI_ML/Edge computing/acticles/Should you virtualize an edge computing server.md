
> The need to pack a lot of compute power into a small package leaves many wondering whether containers and serverless frameworks could replace virtualization at the edge.

IT teams have been steadily moving their workloads to the network's edge to process data closer to its source. Many of these workloads run in virtual environments, but some IT professionals question whether it makes sense to virtualize an edge computing server at all. 

The exact meaning of edge computing and how it's implemented outside of the data center is still up for debate. Some think of edge computing in terms of intelligent devices. Others envision intermediary gateways that process client data. Still others imagine micro data centers that service the needs of remote workers or satellite offices.

Despite the disparity in perspectives, however, they all share a common characteristic: Client-generated data is processed at the network's periphery as close to its source as possible.

Edge computing is much different than using a centralized data center. For example, administrators usually manage edge computing servers remotely, often using tools with intermittent network access. In addition, edge sites typically have space and power restrictions that make it difficult to add capacity to an existing system or to significantly modify the architecture. In some cases, an edge site might require specialized hardware or need to connect to other edge sites.

A number of factors push organizations to the edge, particularly mobile computing and IoT, which generate massive amounts of data. The mega data center can no longer meet the demands of these technologies, which results in an increase in data latencies and network bottlenecks.

At the same time, emerging technologies are making edge computing more practical and even more cost-effective than traditional approaches, as it addresses the limitations of the centralized model.

# The edge computing server and virtualization

To keep edge processing as efficient as possible, some teams run containers or serverless architectures on bare metal to avoid the overhead that comes with hypervisors and VMs.

In some cases, this might be a good approach, but even in an edge environment, virtualization has benefits -- flexibility, security, maintenance and resource utilization, to name a few. Virtualization will likely remain an important component in many edge scenarios, at least for intermediary gateways or micro data centers. Even if applications run in containers, they can still be hosted in VMs.

Researchers view the VM as an essential component of edge computing and believe that admins can use VM overlays to enable more rapid provisioning and to move workloads between servers. But researchers are not the only ones focused on bringing virtualization to the edge.

For example, Wind River's open source project StarlingX makes components of its Titanium Cloud portfolio available through the OpenStack Foundation. One of the goals of the project is to address common requirements for virtualizing an edge computing server. The code already includes a virtual infrastructure manager (VIM), as well as VIM helper components.

VMware is also committed to edge computing and is working on ways to virtualize compute resources throughout the entire data flow, including edge environments. For example, VMware offers hyper-converged infrastructure software powered by vSAN. Shops can use the software to support edge scenarios through VMware vSphere and the VMware Pulse IoT Center. This provides a system that includes secure, enterprise-grade IoT device management and monitoring.

Other vendors are also moving toward the edge, and virtualization is playing a key role. Although edge computing doesn't necessarily imply virtualization, it by no means rules it out and, in fact, often embraces it.

# Administration on the edge

Along with edge computing come a number of challenges for admins trying to manage virtual environments. The lack of industry standards governing edge computing only adds to the complexities.

As computing resources move out of the data center and into the network's periphery, asset and application management are becoming increasingly difficult, especially because much of it is carried out remotely. Admins must come up with ways to deploy these systems, perform ongoing maintenance and monitor the infrastructures and applications for performance issues and trouble spots, and address such issues as fault tolerance and disaster recovery.

If an IT team manages only one edge environment, they should be able to maintain it without too much difficulty. But if the team must manage multiple edge environments and each one serves different functions and is configured differently, the difficulties grow exponentially. For example, some systems might run VMs, some might run containers and some might do both. The systems might also operate on different hardware, use different APIs and protocols, and execute different applications and services.

Admins must be able to coordinate all these environments, yet allow them to operate independently. Edge computing is an industry in its infancy, and network-wide management capabilities have yet to catch up.

But management isn't the only challenge. An edge computing server often has resource constraints, which can make it difficult to change the physical structure or accommodate fluctuating workloads. These challenges go beyond such capabilities as VM migration.

In addition, admins might have to contend with interoperability issues between the source devices and edge systems, as well as between multiple edge systems. This is made all the more difficult by the different configurations and lack of industry standards.

One of the biggest challenges that admins face is ensuring that all sensitive data is secure and privacy is protected. Edge computing's distributed nature increases the number of attack vectors, which makes the entire network more vulnerable to attack, and the different configurations increase the risks.

For example, one system might run containers in VMs and the other on bare metal, resulting in disparity in the methods IT uses to control security. The distributed nature can also make it more difficult to address compliance and regulatory issues. And, given the monitoring challenges that come with edge computing, the risks of undetected intrusions are even greater.
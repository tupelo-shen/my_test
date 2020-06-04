# 引言

Following productivity gains in the 1980s and 1990s from techniques such as lean manufacturing, just-in-time manufacturing made popular by Toyota, and Six Sigma, industrial companies have experienced a steady decline in efficiency. Today annual productivity gains average an abysmal .5%. With few places left to turn for continued operational improvements, industrial organizations had to look for new ways to improve production, performance, and profit.

# Enter digital industrial transformation.

Using technology innovations, industrial companies are beginning to drive new levels of performance and productivity. And while cloud computing is a major enabler of industrial transformation, edge computing is rapidly becoming a key part of the Industrial Internet of Things (IIoT) equation to accelerate digital transformation.

Edge computing is not a new concept, but several trends have come together to create an opportunity to help industrial organizations turn massive amounts of machine-based data into actionable intelligence closer to the source of the data.

This blog covers the many facets of edge computing, including defining what is meant by edge computing, what the components of edge computing are, what’s driving its viability today and the implications, as well as its role in distributed computing in coordination with cloud computing. It also provides some edge computing examples as well as GE Digital’s approach to industrial edge computing.

# 什么是边缘计算？

**The computing infrastructure that exists close to the sources of data.**

In the context of IIoT, 'edge' refers to the computing infrastructure that exists close to the sources of data, for example, industrial machines (e.g. wind turbine, magnetic resonance (MR) scanner, undersea blowout preventers), industrial controllers such as SCADA systems, and time series databases aggregating data from a variety of equipment and sensors. These edge computing devices typically reside away from the centralize computing available in the cloud.

Wikipedia defines Edge Computing as “pushing the frontier of computing applications, data, and services away from centralized nodes to the logical extremes of a network. It enables analytics and data gathering to occur at the source of the data. This approach requires leveraging resources that may not be continuously connected to a network such as laptops, smartphones, tablets and sensors.”

The role of edge computing to date has mostly been used to ingest, store, filter, and send data to cloud systems. We are at a point in time, however, where edge computing systems are packing more compute, storage, and analytic power to consume and act on the data at the machine location. This capability of edge computing will be more than valuable to industrial organizations—it will be indispensable.

# What does edge computing mean for industry?

**The promise of the Industrial Internet includes significant investment in the coming years.**

Industry pundits have calculated that tens of billions of connected things will generate massive volumes of data from disparate sources. The promise of the Industrial Internet includes significant investment in the coming years. Management consulting firm, McKinsey & Co., estimates that the Industrial Internet of Things (IIoT) will create $7.5T in value by 2025. The Industrial IoT brings together minds and machines—connecting people to machine data that accelerate digital industrial transformation.

*By applying big data, advanced analytics, and machine learning to operations, industrials can reduce unplanned downtime, improve asset performance, lower cost of maintenance, and open up potential for new business models that capture as-yet untapped value from machine data.*

Over the past several years, industrial organizations have started to incorporate cloud into their operations to glean insights from large volumes of data that are helping achieve key business outcomes including reduced unplanned downtime, higher production efficiency, lower energy consumption and so on. The cloud still plays a critical role in enabling new levels of performance through the Industrial IoT, where significant computing power is required to effectively manage vast data volumes from machines.

But as more compute, store, and analytic capability is bundled into smaller devices that sit closer to the source of data—namely, industrial machines—edge computing will be instrumental in enabling edge processing to deliver on the promise of the Industrial IoT.

**While edge computing isn’t new, there are several key drivers making edge computing a more viable reality today:**

1. Cost of compute and sensors continue to plunge
2. More computing power executed in smaller footprint devices (such as a gateway or sensor hub)
3. Ever-growing volume of data from machines and/or the environment (e.g. weather or market pricing)
4. Modern machine learning and analytics

These factors collide to help companies turn massive amounts of data into insightful and intelligent actions right at the edge.

**For industrial organizations, edge computing will become critical in the following use cases:**

* Low/intermittent connectivity (such as a remote location)
* Bandwidth and associated high cost of transferring data to the cloud
* Low latency, such as closed-loop interaction between machine insights and actuation (i.e. taking action on the machine)
* Immediacy[即时性] of analysis (say, a technician working in the field to check machine performance)
* Access to temporal data for real-time analytics
* Compliance, regulation, or cyber security constraints

The business implications of edge computing are compelling. While there are many outcomes that edge computing can enable for industrial organizations, the Edge Computing Consortium[边缘计算联盟] identifies the following:

1. Predictive maintenance
    
    * Reducing costs 
    * Security assurance
    * Product-to-service extension(new revenue streams)

2. Energy Efficiency Management
    
    * Lower energy consumption
    * Lower maintenance costs 
    * Higher reliability
    * Smart manufacturing

        * Increased customer demands mean product service life is dramatically reduced

            * Customization of production modes
            * Small-quantity and multi-batch modes are beginning to replace high-volume manufacturing

3. Flexible device replacement

    * Flexible adjustments to production plan
    * Rapid deployment of new processes and models

# Edge computing[边缘计算] vs. Fog computing[雾计算]

## Pushing intelligence and computing power closer to the source of the data.

Some have joked that Fog Computing is bad marketing term for what is really edge computing. But fog computing and its role in the Internet of Things (IoT) has similar goals as edge computing—push intelligence and computing power closer to the source of the data… machines such as pumps, turbines, sensors, and more. [Fog networks](http://www.webopedia.com/TERM/F/fog-computing.html), or fogging, is a decentralized computing infrastructure in which data, compute, storage, and applications are distributed in the most logical, efficient place between the data source and the cloud. But whereas fog networks focus on edge devices that speak to each other, such as IoT gateways, edge computing is focused on the devices and technology that are actually attached to the ‘thing’, such as industrial machines.

# Edge computing vs. Cloud computing

## Working together. 

For industrial companies to fully realize the value of the massive amounts of data being generated by machines, [edge computing and cloud computing](https://www.ge.com/digital/iiot-platform/predix-cloud-and-predix-private-cloud) must work together.

When you consider edge and cloud, think about the way you use your two hands. You will use one or both depending on action required. Apply that to an IIoT example, where one hand is edge and the other hand is cloud, and you can quickly see how in certain workloads your “edge hand” will play a more prominent role while in other situations your “cloud hand” will take a lead position. And there will be times when both your edge hand and cloud hand are needed in equal measure.

Scenarios in which edge computing will dominate include a need for low latency (speed is of the essence) or where there are bandwidth constraints (locations such as a mine or an offshore oil platform that make it neither practical or affordable, and in some cases impossible, to send all data from machines to the cloud). It will also be important when Internet or cellular connections are spotty. Cloud computing will take a more dominant position when actions require significant computing power, managing data volumes from across plants, asset health monitoring, and machine learning, and so on.

The bottom line is this: cloud and edge are both necessary to industrial operations to gain the most value from today’s sophisticated, varied, and volume of data applied across cloud and edge, wherever it makes the most sense to achieve the desired outcomes.

# Edge computing in action 

## Here’s a couple of edge computing examples to help bring the concept of edge computing to life:

## Autonomous vehicles

With autonomous automobiles—essentially a datacenter on wheels—edge computing plays a dominant role. GE Digital partner, Intel, estimates that autonomous cars, with hundreds of on-vehicle sensors, will generate 40TB of data for every eight hours of driving. That’s a lot of data. It is unsafe, unnecessary, and impractical to send all that data to the cloud.

It’s unsafe because the sensing, thinking, and acting attributes of edge computing in this use case must be done in real-time with ultra-low latency to ensure safe operation for passengers and the public. An autonomous car sending data to the cloud for analysis and decision-making as it traverses city streets and highways would prove catastrophic. For example, consider a child chasing a ball into the street in front of an oncoming autonomous car. In this scenario, low latency is required for decision and subsequent actuation (the car needs to brake NOW!).

It’s unnecessary to send all that data to the cloud because this particular set of data has only short-term value (a particular ball, a particular child on a collision with a particular car). Speed of actuation on that data is paramount. It’s simply impractical (not to mention cost-prohibitive) to transport vast volumes of data generated from machines to the cloud.

However, the cloud is still an important part of IIoT equation. The simple fact that the car had to respond to such an immediate and specific event might be valuable data when aggregated into a digital twin, and compared with the performance of other cars of its class.

## Fleet Management

In a scenario where a company has a fleet (think trucking company, for example), the main goal could be to ingest, aggregate, and send data from multiple operational data points (think wheels, brakes, battery, electrical) to the cloud. The cloud performs analytics to monitor the health of key operational components. A fleet manager utilizes a fleet management solution to proactively service the vehicle to maximize uptime and lower cost. The operator can track KPIs such as cost over time by part, and/or the average cost of a given truck model over time. This in turn helps maintain optimal performance at a lower cost and higher safety.

# What is an Edge server?

**Where the digital world meets the real world.**

Edge servers can be defined as “a computer for running middleware or applications that sits close to the edge of the network, where the digital world meets the real world. Edge servers are put in warehouses, distribution centers and factories, as opposed to corporate headquarters [公司总部].”

# What is mobile edge computing?

**At the edge of the cellular network**

Mobile edge computing (MEC) is a network architecture concept that enables cloud computing capabilities and an IT service environment at the edge of the cellular network. The basic idea behind MEC is that by running applications and performing related processing tasks closer to the cellular customer, network congestion is reduced and applications perform better.

GE’s recent news of mobile edge connectivity reflects the innovations the company is leading that allows industrial organizations to gain more value from their industrial assets. GE’s 2017 partnership with Intel, Ericsson, Honeywell and U.C. Berkley on a 5G Innovators Initiative is designed to open up opportunities for Industrial companies looking to optimize their assets and operations that need connectivity from the edge to the cloud. “Connecting those assets to GE’s Predix platform and using the innovations emerging from 5G wireless will help them unlock efficiency, increase manageability and drive sustainability. Building a thriving ecosystem of innovators who use the next generation of digital connectivity to sustain and surprise our customers – in industries ranging from manufacturing to health care – is key to everyone’s success,” said Peter Marx, vice president, advanced concepts, GE Digital.

Also, GE Digital, Qualcomm Technologies and Nokia announced a successful demonstration of a private LTE network for Industrial IoT. As the news describes, “Industrial companies often have local connectivity needs and operate in remote locations or temporary sites, such as mines, power plants, offshore oil platforms, factories, warehouses or ports—connectivity for these environments can be challenging. A standalone LTE network to serve devices and users within a localized area can help improve performance and reliability for these industrial settings.”

# Example of edge devices

Examples of GE’s edge technology
An edge device can be defined in several ways. You could think of an edge device as an entry point into enterprise or service provider core networks. Examples include routers, switches, integrated access devices (IADs), multiplexers, and a variety of metropolitan area network (MAN) and wide area network (WAN) access devices. Edge devices also provide connections into carrier and service provider networks.

 

When GE Digital talks about industrial edge computing devices, we are referring to operational technology such as sensor hubs, actuators, controllers, and IoT gateways that comprise deterministic environments found in industrial operations. Examples of GE’s edge technology includes products from GE Automation & Controls such as control systems ([Programmable Automation Controllers](http://www.geautomation.com/products/programmable-automation-controllers), [Mark VIe Distributed Control System](http://www.geautomation.com/products/mark-vie)), as well as 3rd party hardware systems from Dell, HP and others.

# GE Digital's approach to edge computing 

**Value from machine-driven data and assets**

Edge computing as a concept has been around for many years. However, as we see more compute, storage, and analytic capabilities in smaller devices, these powerful edge computing devices are poised to give industrials the ability extrapolate more value from machine-driven data. GE Digital estimates that today roughly 3% of machine data is contributing to any meaningful outcomes. That leaves 97% of value yet to be tapped by industrial organizations that will help them further monetize the value of the assets.

GE Digital's edge approach is to move analytics and applications closer to machines, particularly to support workloads and data that add operational value when executed at the edge (nearer to machines that generate the data). By enabling compute, storage, and analytics closer to machines, GE Digital edge software and hardware offerings—as well as third-party Predix Platform-ready edge applications—create a tight union between control logic and modern applications. This enables industrials and healthcare providers to bring visibility, control, and analytic insights to many parts of infrastructure and operations—from factory shop floors to hospital operating rooms, from offshore oil platforms to electricity production.

# Edge computing software

**Unlocking new value from machine-based insights**

As part of GE’s own digital industrial transformation journey, it created Predix Platform—an applications and services platform developed for industrial companies by an industrial company, spanning cloud and edge devices to improve productivity, deliver higher uptime, and drive down costs.

GE Digital’s [Predix Machine](https://www.predix.io/services/service.html?id=1185) software unlocks new value from machine-based insights that drive increased operational agility while reducing risk and preserving machine investments. It is designed to connect, run, and manage applications in close proximity to the source of the data—the physical industrial machines and assets.

Predix Machine allows industrial companies to track, manage, and communicate with all network edge devices anytime, anywhere. In concert with Predix Cloud, Predix Machine allows for advanced monitoring and diagnostics, machine performance optimization, proactive maintenance, and operational intelligence. This gives industrials the flexibility to manage and process machine data wherever it makes the most sense for optimal operation—at the edge, in the cloud or a combination of the two.

Predix Machine can process and route data to Predix Cloud with reliable and secure cloud connectivity, enabling cloud applications to process, analyze, and act on data generated from connected devices without having to manage any infrastructure. Predix Machine meets unique security, privacy, and data governance regulations and policies for companies around the world.
# 目录

* [0 Introduction](#0)
    - [0.1 About This Book](#0.1)
* [1 Introduction](#1)
    - [1.1 Grasping the Basics of ML](#1.1)
    - [1.2 Differentiation and Cost Reduction](#1.2)
    - [1.3 Exploring ML Opportunities](#1.3)

---

<h1 id="0"> Introduction</h1>

From Alan Turing’s 1950 prediction that "machines will eventually compete with men," through decades of research in labs and think tanks, machine learning (ML) has finally reached its viability point, exploding into the domain of engineering and into people’s daily lives.

The technology is now moving quickly. ML is no longer the preserve of distant, cloud-based data centers. A dramatic shift in the capabilities of compute processing power and ML algorithms is driving applications, training, and inference back to the edge of the network — to the smart devices that are already an intrinsic part of everyday life.

<h2 id="0.1">0.1 About This Book</h2>

`Embedded Machine Learning Design For Dummies`, Arm Special Edition, shows you that adding machine learning to any device is not only possible but relatively easy to do. This book highlights key challenges and explains why it is vital to address them at the earliest stages of planning. The book addresses how to approach your platform configuration and explains why software matters. Finally, the book explores the importance of an ecosystem perspective in ML development and gives examples of interesting ML solutions at the edge.

<h1 id="1">1 Realizing Why ML Is Moving to the Edge</h1>

本章重点：

* Grasping the basics of ML
* Understanding differentiation and cost reduction

Machine learning (ML) represents the greatest inflection point in computing for more than a generation — and it’s already having a significant impact across virtually every market. It’s leading to dramatic advances in connected car technologies, changing the face of healthcare, and influencing how city infrastructure is controlled. It’s also affecting less obvious sectors such as farming, where device-born intelligence is enabling super-efficient watering practices, precisely targeted pest and disease control, and the optimization of crop harvesting.

The potential for ML is so far-reaching, it’s hard to imagine a sector that won’t be affected. For users, ML promises new levels of insight and convenience — at home, at work, and at leisure. For manufacturers, it offers the chance to make processes far more efficient and to create new business models and services.

<h2 id="1.1">1.1 Grasping the Basics of ML</h2>

The terms artificial intelligence (AI) and ML are often used interchangeably. However, in data science, the terms are distinct. This book uses the following definitions:

* AI is an umbrella term relating to hardware or software that enables a machine to mimic human intelligence. A range of techniques are used to deliver that “intelligence” including ML, computer vision, and natural language processing.
* ML is a subset of AI, as shown in Figure 1-1. ML uses statistical techniques to enable programs to “learn” through training, rather than being programmed with rules.

ML systems process training data to progressively improve performance on a task, providing results that improve with experience. Data is taken from the edge — be that an IoT device, edge server, or edge device — and sent to the cloud to be used for training.

Machine learning is a subset of artificial intelligence.

    ALgorithms < Deep learning < Machine Learning < Artificial intelligence

Once an ML system is trained, it can analyze new data and categorize it in the context of the training data. This is known as inference.

ML is performed in one of two locations:

* Cloud

    ML training is typically performed on remote, power-intensive, and compute-intensive server systems.

* Edge

    ML inference is usually done locally, on the device that will deliver the outputs. The term edge may refer to an IoT device, edge server, or edge device.

<h2 id="1.2">1.2 Differentiation and Cost Reduction</h2>

While the first wave of ML focused on cloud computing, the combination of improved techniques for shrinking models to run on low-power hardware and increased compute capabilities on edge devices is opening possibilities for dramatic advantages in differentiation and unit cost reduction:

* Reduced latency; increased reliability and safety

    Latency in the form of an unresponsive app or a page that won’t load is an annoyance for the user, but many time-critical applications — such as automotive systems — simply cannot rely on connectivity to the cloud because a delay in response might have serious safety implications and seriously affect vehicle performance.

* Power and cost

    Transmitting data from a device to a cloud server increases the power cost of performing ML because moving data around a system takes power. Cloud- or network-performed ML also adds a bandwidth tax, which can be significant because ML tends to be data-intensive. By performing as much ML as possible on-device, the cost and complexity burden on the network and cloud infrastructure is reduced.

* Privacy and security

    Consumers and corporations are increasingly becoming aware of data security. No one wants their privacy breached, but the risks are amplified when data is constantly shifted to the cloud and back. When processing is done on-device, legislative issues around the storing or transmission of data — and compliance with privacy regulations, such as the European Union’s recent General Data Protection Regulation (GDPR) — are minimized.

* Personalization

    In addition to privacy and security, performing ML on-device can lead to a more personalized compute experience. As more devices become “intelligent,” they will need to adapt and provide a contextualized response to their immediate environment — instantaneously. When these devices connect users to the things they care about, AI becomes accessible and personal. Maintaining unique, customized models for every user in the cloud is a significant ongoing expense, so edge devices that can run their own customized models will provide a competitive advantage.

Ultimately, ML at the edge delivers a more reliable, responsive, and secure user experience that reduces perunit cost, personal data risk, and power requirements — and isn’t dependent on network connections.

<h2 id="1.3">1.3 Exploring ML Opportunities</h2>

ML is not about a new type of device; it’s about every device. ML enables devices to contextualize their immediate environments far better — using data such as vision, sound, heat, and vibration. This innovation is driving new business models, reducing costs, and optimizing performance across a range of parameters.

Although the benefits of ML are exciting, taking the first steps to add ML capability to your product may seem
daunting. ML processing requirements vary significantly according to the model and workload; no “one-sizefits-
all” solution exists. Almost all models allow accuracy and performance to be traded freely. This flexibility
allows a perfect fit between device hardware and the model capability, but it raises additional questions:

1. What are the use cases?
2. Which neural network (NN) model provides the best performance/accuracy trade-off?
3. Which hardware should you choose to complement it? Can lower-capability hardware be used with a reduced-accuracy model? Can the model be tuned to make use of all the available RAM?
4. Which tools are available to help a team answer these questions?
5. Most importantly: How can these issues be balanced to deliver the best performance at the best unit cost?

<h1 id="2">2 Configuring Your ML Environment</h1>

本章重点：

1. Understanding the components of an ML platform.
2. Making choices for your ML environment.
3. Learning from a case study.

Selecting the right solution for your application entails a series of trade-offs: from small, lowpower microcontroller units (MCUs) for cost- and power-constrained systems to central processors (CPUs) for greater performance and general-purpose programmability; graphics processors (GPUs) for faster performance with graphics-intensive applications; and neural processors (NPUs) for the most intensive and efficient ML processing. This chapter leads you through the process.

<h2 id="2.1">2.1 Understanding the Components of an ML Platform</h2>

What does your platform need to run ML workloads? Perhaps surprisingly, in hardware terms it may need nothing more than you already have. Many platforms are already efficiently running ML applications on CPUs and GPUs alone. (You can see some examples in Chapter 5.)

For some use cases, higher performance requirements may demand a dedicated ML processor, such as a neural processing unit (NPU). However, adding an NPU to an already crowded mix of CPUs and GPUs running ML applications can create complexity if the software stack for each processor type is different. This issue creates a need for a software layer that hides hardware complexity from the software applications.

> Heterogeneous computing [异构计算]() refers to systems that use more than one kind of processor or core to achieve better performance and greater efficiency.

A heterogeneous compute platform allows application developers to write ML applications using their favorite NN frameworks — such as Google's TensorFlow or Facebook's Caffe and Caffe2 — and target a variety of processor types from multiple vendors. As shown in Figure 2-1, the software layer between the application and the hardware handles the translation of the workload to target the applicable and available core types automatically.

![]()
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

![Figure 2-1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/AI_ML/ARM_AI_ML/ARM_RESOURCES/images/1-2-1.PNG)

FIGTURE 2-1: An example of a heterogeneous ML compute platform with a variety of core types and open-source software.

Open standards can help to reduce complexity and assure future compatibility with solutions from a variety of vendors. One example of this is the Open Neural Network Exchange Format (ONNX), supported by Arm, Microsoft, Facebook, Amazon, and others to provide a common format between training frameworks and runtime engines.

You have many options for running an ML model:

* Low-power/always-on microcontroller CPUs:

    Small models can be run on these cores with a very low power budget. Example: detecting activity or behavior that wakes up the rest of the system to perform more detailed analysis.

> Sometimes microcontroller CPUs have DSP extensions, known as Digital Signal Controllers, which allow for faster processing, thus providing clean signals for ML without the need for a separate DSP. This feature saves cost, area, and development time, especially for those without a DSP software development background.

* High-efficiency, general-purpose CPUs:

    Depending on the configuration and number of cores, these can comfortably run entire workloads. Example: real-time speech recognition for closed-caption generation.

* Multimedia and GPU cores:

    These often run the same models as a mobile-class core with increased performance or efficiency. If a design already requires a GPU, offloading the ML workload may allow you to reuse this silicon for maximum cost-benefit.

* Dedicated NPUs:

    Dedicated silicon offers the highest efficiency for running general-purpose ML models in a low-power environment. Example: determining which pixels of a real-time HD video feed correspond to a person.

Knowing exactly which components are required in your device demands careful consideration of multiple factors, which the next section discusses.

<h2 id="2.2">2.2 Making Choices for Your ML Environment</h2>

The right hardware for your application, what you need from a dedicated NPU, and how you integrate it into systems and tools varies from case to case. However, the following capabilities are important to consider as part of your architectural design for a constrained environment:

1. Processing:

    What type of data ingestion and processing does your edge solution require? This may depend on the type of models you run and the number of models running at the same time. How complex are these models? Are they compute- or bandwidth-constrained?

2. Connectivity:

    What can be done locally on the device? What requires a connection to the cloud? What must stay in the cloud?

3. Integration:

    Are there any integrations or dependencies that must be managed? What NN frameworks will the developer community be using? How will the models developed in a variety of different frameworks be supported on the system?

4. Power, size, and heat:

    Does your edge device have power, size, and thermal constraints?

5. Accuracy:

    What is the desired accuracy? Although you might initially think that the higher the accuracy, the better, that isn’t always true. After a certain level, you hit a point of diminishing returns. In certain scenarios, a 2 ercent increase in accuracy may require a 10x increase in compute and memory requirements, for example. You should clearly understand what level of accuracy is required for any particular use case.

6. Privacy:

    What are the privacy and security concerns around your edge solution?

7. Workload:

    Will an NPU be mostly idle and only used for work that an otherwise idle CPU or GPU can accomplish? On the other hand, if a CPU or GPU is capable of running a model, might an NPU be a better solution if those resources are already heavily loaded?

Taking these points into consideration, you can look at the different components that are required. Is a CPU or GPU sufficient? Do you need to add an NPU? How big must that NPU be to service your requirements within the cost constraints? Look at the different core types available in the market and match them to your area and power budgets, as well as your compute requirements.

The following case study offers an example of the complexity that can be managed on an embedded device at the edge.

<h2 id="2.3">2.3 Case Study: An Edge ML Solution for Asthma Patients</h2>

The Amiko Respiro provides smart inhaler technology that helps asthma sufferers breathe more easily. The solution includes ML-powered sensors as add-ons to standard inhalers to improve asthma treatment. These smart sensors must be low-powered, scalable, and cost-efficient enough to work with a patient-facing app on a connected platform.

At the center of Respiro’s sensor module is an ultra-lowpower Arm Cortex-M processor, enabling:

* Processing that takes place securely on the device with no need to connect to the cloud
* Bluetooth low-power (BTLE) connectivity to a smartphone app
* An efficiency profile that extends device battery life

The solution uses ML to interpret vibration data from the
inhaler. The sensor is trained to recognize the patient’s
breathing pattern and inhalation time and can calculate
important parameters such as lung capacity and
inhalation
technique.


The processor allows the Respiro to run real-time ML
algorithms that recognize behavior patterns and interpret
data within the sensor module itself. The user doesn’t
need to wait for back-end infrastructure to process
detailed sensor data. When the user presses the trigger,
the module instantly recognizes the breath data pattern
and provides low-latency, private user feedback.

The Respiro sensor is bundled with an app that the
patient installs on a smartphone. The sensor collects
inhaler use data without disrupting the medication delivery
pathway and sends data and feedback to the app. The
sensor also has the flexibility to add new features and
easily scale up to deliver further innovative connected
healthcare solutions going forward.

<h1 id="3">3 Why Software Really Matters</h1>

本章重点：

1. Reducing time to market with off-the-shelf hardware, tools, and simulators
2. Differentiating through software
3. Training and deploying ML models

According to the famous quote by the prominent
technologist Mark Andressen, “software is eating
the world.” It’s certainly capable of eating development
budgets and delaying product schedules if you
get it wrong. However, because software innovation and
ML functionality are increasingly becoming key differentiators
for many device-makers, if you’re building ML
into your product, you need a strong grasp on your
specific
software needs.


<h2 id="3.1">3.1 Reducing Time to Market with Off-The-Shelf Hardware, Tools, and Simulators</h2>

The quickest way to de-risk software and ML model
development is to do your prototyping with off-the-shelf
hardware that has functionality as close to your final
product as possible. That can be through the use of
industry-standard CPUs and GPUs or a previous iteration
of the product itself.

To achieve what you need, it’s advisable to choose
components
sourced from a large-scale provider —
preferably from an ecosystem with a wide range of possible
prototype platforms. This means you’ll be able to
select technology similar to your final design in the areas
that matter — such as performance and power or with
specific hardware connectivity.

Once a prototype platform is in the hands of the software
team, the next step is to create a useful development environment.
Depending on your device, this might include:

* An operating system
* Compilers
* Performance libraries
* Debugging and profiling tools
* ML frameworks

Development teams take time to build expertise in these
tools and libraries. Building on a platform that’s already
familiar to your team increases productivity and the
capacity for innovation, greatly reducing the risk to your
software development.

> Ideally, the tools and the software your team
develops should be portable to your final
platform. Common standards and architectures
pave the way for a smooth and
painless integration when early hardware
becomes ready for testing, minimizing the
chance of last-minute delays and performance
surprises.

If your chosen architecture provides simulators and fast
modeling tools, this risk is minimized even further.
Today’s fast models and simulators are capable of bringing
up an entire operating system in simulation and running
the final application code even before silicon is
ready, with cycle simulators providing further correctness
and performance predictions. Projects that use simulation
effectively can often successfully deploy the same
software directly onto the first silicon.

In short, to effectively de-risk software development,
select an architecture and off-the-shelf prototyping
platform that

* Closely reflects the functionality required in the final product
* Supports a common set of tools, libraries, and frameworks that is familiar to your team
* Provides robust simulation solutions that minimize time spent integrating software and hardware


<h2 id="3.2">3.2 Differentiating Through Software</h2>

A decision for hardware architecture is also a decision for
its software stack and the ecosystem supporting it. Large
ecosystems frequently have dedicated teams continuously
optimizing and improving their software stacks.
For example, Arm provides Arm NN, Compute Library,
and CMSIS-NN. Over a four-month period, the performance
of NNs such as AlexNet, Inception, SqueezeNet,
and VGG-16 improved 1.6x to 2.6x on Cortex-A series
CPUs and the Mali GPU, as shown in Figure 3-1.

Developing on top of an actively developed and maintained
stack ensures you benefit from future performance
and security improvements without taking time away
from your own development efforts.


<h1 id="0">0 目录</h1>
* [1 概述](#1)
    - [1.1 关于Cortex-A8](#1.1)
    - [1.2 ARMv7-A架构](#1.2)
    - [1.3 处理器的组成](#1.3)


---

<h1 id="1">1 概述</h1>

This guide shows you how to perform real-time image recognition on a low-power Arm Cortex-M7 processor, using Arm’s CMSIS-NN library. The Cortex-M7 processor is found in a range of solutions from a variety of microcontroller vendors.

Increased compute performance in the smallest Cortex-M based devices is enabling more data processing to move to the edge. *Machine learning* (ML) no longer needs to take place in the cloud or only on advanced application processors. Now, you can take advantage of the benefits afforded by edge computing, such as reduced bandwidth and time-to-decision, increased privacy and reliability, all without relying on an internet connection.

ML at the edge offers new possibilities for new real-time decision-making applications, from voice recognition for smart speakers to facial detection for surveillance cameras.

<h1 id="2">2 准备</h1>

To complete this guide, you'll need some equipment and to download some software for building the final code that runs on the board:

<h2 id="2.1">2.1 硬件</h2>

* [STM32F746G-DISCO discovery board (Cortex-M7)](https://os.mbed.com/platforms/ST-Discovery-F746NG/)
* [STM32F4DIS-CAM Camera module](https://uk.farnell.com/stmicroelectronics/stm32f4dis-cam/module-1-3mp-camera-f4-discovery/dp/2250206?CMP=i-ddd7-00001003)

All the steps can be easily adapted for a different board. We have chosen a specific board to demonstrate the full workflow of how to set up, deploy, and run the code.

<h2 id="2.2">2.2 软件</h2>

* Ubuntu 16.04
* Python 2.7.12
* Caffe
* GNU Arm Embedded [Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). Recommended version: gcc-arm-none-eabi-7-2017-q4-major

<h1 id="3">3 Define the problem and the model</h1>

Our objective here is to find the best way to have your embedded device run image recognition, in order to make your edge device more responsive, reliable, energy efficient and secure by ensuring that your data is processed on the device, rather than in the cloud.

Neural networks (NN) are a class of machine learning (ML) algorithms that have demonstrated good accuracy on image classification, object detection, speech recognition and natural language processing applications. The most popular types of neural networks are multi-layer perceptron (MLP), convolutional neural networks (CNN) and recurrent neural networks (RNN).

In this case we have chosen to use a CNN, provided in the Caffe examples, for [CIFAR-10](http://www.cs.toronto.edu/~kriz/cifar.html) image classification task, where the input image passes through the CNN layers to classify it into one of the 10 output classes. Typical CNN models have different types of layers to process the input data, below you can see the ones in the chosen CNN:
[TOC]

## 1 变化

* 支持其它的ZYNQ平台，`Zynq UltraScale+ MPSoC`（`Xilinx ZCU102`, `ARMv8a`, `Cortex A53`）；
* 支持x86上的`multiboot2`引导加载程序(来自`Genode Labs`的更改贡献)；
* 弃用`seL4_CapData_t`类型及其相关函数；
* `fastpath`通道改进：如果有两个处于运行态的线程，其中一个目标线程在调度器中拥有最高优先级，那么这个目标线程就会走`fastpath`通道。之前，如果在高优先级线程和低优先级线程之间进行IPC通信，不会使用`fastpath`通道;
* 因为上一项的变化，如果在相同优先级的线程之间发送非阻塞IPC，调度行为发生变化：发送方被调度，而不是接收方；
* 支持在`armv8/aarch64`架构平台上做基准测试；
* 增加额外的x86平台相关的`bootinfo`信息，用于在`multiboot2`阶段检索所需信息使用；
* 增加debug选项，将x86性能监控计数器导出到用户态。

## 2 升级说明

* `seL4_CapData_t`数据结构应该被`seL4_Word`替换。`badge`标记的构造应该直接使用`x`而不是原先的`seL4_CapData_Badge_new(x)`，能力的`guard`标志位的构造应该是`seL4_CNode_CapData_new(x, y)`而不是`seL4_CapData_Guard_new(x, y)`；
* 依赖于非阻塞IPC，而在相同优先级线程之间进行切换的代码可能会被中断。

## 3 完整的修改日志

可以使用`git log`命令获取：`git log 7.0.0..8.0.0`。

## 4 更多详细信息

参考`V8.0.0`的手册。
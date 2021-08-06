[TOC]

## 1 变化

* Debugging option on x86 for syscall interface to read/write MSRs (this is an, equally dangerous, alternative to dangerous code injection)
* Mitigation for Meltdown (https://meltdownattack.com) on x86-64 implemented. Mitigation is via a form of kernel page table isolation through the use of a Static Kernel Image with Microstate (SKIM) window that is used for trapping to and from the kernel address space. This can be enabled/disabled through the build configuration depending on whether you are running on vulnerable hardware or not.
* Mitigation for Spectre (https://spectreattack.com) on x86 against the kernel implemented. Default is software mitigation and is the best performing so users need to do nothing. This does not prevent user processes from exploiting each other.
* x86配置选项，用于在上下文切换时执行分支预测，以防止使用间接分支预测器的用户进程之间的`Spectre`类型攻击；
* x86配置选项，用于在上下文切换时刷新RSB，以防止使用RSB的用户进程之间的`Spectre`类型攻击；
* 为x86架构的TSC频率（高精度时间戳计数器，单条指令可读取）定义扩展`bootinfo`头；
* x86 TSC frequency exported in extended bootinfo header
* `archInfo`不再是`bootinfo`数据结构的成员。只有在x86架构上保存`TSC`频率时使用，而且它已经挪到扩展`bootinfo`中了；
* 设置线程优先级和最大控制优先级（MCP）的接口发生变化；
    - 对于这两个调用接口，用户现在必须提供`TCB`能力`auth`；
    - 请求的MCP/优先级将会根据`auth`能力的MCP进行检查；
    - 对调用的TCB进行前面的行为检查，这可能会导致混淆的代理问题。
* `seL4_TCB_Configure`不再使用参数`prio`和`mcp`。分别使用`seL4_TCB_SetPriority`和`seL4_TCB_SetMCPriority`进行设置；
* `seL4_TCB_SetPriority`和`seL4_TCB_SetMCPriority`使用`seL4_Word`代替`seL4_Uint8`类型。`seL4_MaxPrio`仍然是`255`；
* `seL4_TCB_SetSchedParams`是一种新方法，可以在同一个系统调用中设置MCP和优先级；
* 因为某些build选项配置的原因，TCB对象的大小增加了。


## 2 升级说明

* 设置任务优先级的方法发生了改变。请使用`seL4_TCB_SetSchedParams`、`SetPriority`、`seL4_TCB_SetMCPriority`进行设置。

## 3 完整的修改日志

可以使用`git log`命令获取：`git log 8.0.0..9.0.0`。

## 4 更多详细信息

参考`V9.0.0`的手册。
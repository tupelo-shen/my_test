[TOC]

## 1 变化

* `boot`引导信息中的数据结构没有声明为`packed`：

    - 以前都是packed的（GCC的属性）；
    - 一些字段的长度被设置，避免填充；
    - 这是一个源码兼容的改变；

* ARM平台现在可以设置`IRQHandler`能力的触发方式；

    - `seL4_IRQControl_GetTrigger`允许用户获取`IRQHandler`能力，并设置其在中断控制器里的触发方式。

* 添加对NVIDIA的`Jetson TX2` (`ARMv8a`, `Cortex A57`)板卡的支持；

* 为`raspberry pi 3`添加`AARCH64`支持；

* 代码编译使用`jinja2`代替`tempita`；

* 为运行多个ARM虚拟机增加了`AARCH32 HYP`支持；

* 更新了`AARCH32 HYP VCPU`寄存器；

* 为所有平台提供设置`TLSBase`的新接口；
    
    - seL4_TCB_SetTLSBase

* `Kbuild/Kconfig/Makefile`编译系统移除。

## 2 升级说明

无

## 3 完整的修改日志

可以使用`git log`命令获取：`git log 10.0.0..10.1.0`。

## 4 更多详细信息

参考`V10.1.0`的手册。
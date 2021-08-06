[TOC]

## 1 变化

* 在64位平台上，`seL4_MessageInfo`的`label`成员是52位宽。如果用户态的程序在使用下面的这些函数将`label`字段通过掩码设置为20位宽时，执行会终止。

    - seL4_MessageInfo_new
    - seL4_MessageInfo_get_label
    - seL4_MessageInfo_set_label

* 在`RISC-V`架构上实现初始原型移植。目前，只运行在`Spike模拟平台`上，只支持64位模式，且不支持FPU和多核（SMP）。

## 2 升级说明

无。

## 3 完整的修改日志

可以使用`git log`命令获取：`git log 9.0.0..9.0.1`。

## 4 更多详细信息

参考`V9.0.1`的手册。
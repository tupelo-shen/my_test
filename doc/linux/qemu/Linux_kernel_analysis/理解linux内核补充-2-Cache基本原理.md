参考文章：[Cache基本原理之：结构](https://www.jianshu.com/p/2b51b981fcaf)

## Cache最小单位

目前，主流CPU使用的Cache，最小的数据搬送单元基本都是64字节。通常，人们称之为`Cache line`或者`Cache 行`。每个cache line除了包含数据，还包含TAG（地址信息）和状态信息。

## 关联方式

Cache的替换策略决定了主存中的数据块会拷贝到cache中的哪个位置，如果对于一块数据（大小为一个cache line ），只有一个cache line与之对应，我们称之为”直接映射 (Direct map)”；如果该数据块可以和cache中的任意一个cache line对应，则称之为”全相联（Full-Associative）”而目前更多的实现方式是采用”N路组相连（N Way Set-Associative）”的方式，即内存中的某一块数据可能在cache中的N个位置出现，N可能是2，4，8，12，或其他值。

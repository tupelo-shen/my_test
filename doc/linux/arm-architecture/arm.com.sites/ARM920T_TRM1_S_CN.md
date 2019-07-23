
<h2 id="4.2">4.2 ICache</h2>

ARM920T 包含一个16KB的ICache，它有512个line，每个line大小32字节（8个Word）。

ICache中的指令可以被锁住，以避免被linefill覆盖掉。锁指令这种操作的粒度是Cache的1/64，也就是64个字（256字节）。

所有指令的访问都应由MMU进行权限和转换的检查。由MMU放弃的指令读取不会导致linefill或指令读取出现在AMBA ASB总线接口上。

---
> 为了明确，在下面的文章中，I 位（CP15寄存器1的bit 12）被称为Icr 位；来自与被访问的地址相关的MMU转换表描述符相关的C位称为Ctt位。

---

<h3 id="4.2.1">4.2.1 ICache组织结构</h3>



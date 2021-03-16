MIPS N32 ABI 是 MIPS N64 ABI 的一个变种，在保留 MIPS N64 ABI 的几乎所有特性的情况下，使用了 32-bit 的指针。理由是在一个 32-bit 地址空间足够使用的应用程序上使用 64-bit 的指针不会带来任何好处。虽然它也是 32-bit 指针的，但和 MIPS O32 ABI 的差异还是非常之大的，这也是为什么应该将它归类到 MIPS64 家族中的原因了。

下面我们具体分析一下 MIPS N32 ABI 与 MIPS N64 ABI 的调用约定，会将其中的差异标记出来。

## 1 基本数据类型

| C类型 | 汇编名称 | N32数据宽度(字节) | N64数据宽度(字节) |
| ----- | -------- | ----------------- | ----------------- |
| char  | byte     | 1                 | 1                 |
| short | half     | 2                 | 2                 |
| int   | word     | 4                 | 4                 |
| long  | word     | 4                 | 8                 |
| long long | dword| 8                 | 8                 |
| float | word     | 4                 | 4                 |
| double| dword    | 8                 | 8                 |

## 2 对齐要求

上述的数据类型，只有当自然对齐的情况下，才可以使用标准的 MIPS 访存指令直接处理。MIPS N32 与 N64 都要求栈对齐到 16 字节。

## 3 函数调用约定

### 3.1 常规参数传递

MIPS N32／N64 ABI 约定了多达 8 个通用寄存器（a0-a7）和 8 个双精度浮点寄存器（$f12-$f19）用于传递前 8 个参数，其后的所有参数在栈中传递，并且在栈中没有空间保留给前 8 个参数（这一点不同与 MIPS O32 ABI）。所有的常规参数（寄存器、栈中）的都是占用 8 字节空间，寄存器中的参数会符号扩展，就像是加载到寄存器中。通常情况下，前 8 个参数具体是在通用寄存器还是浮点寄存器是由函数原型中的类型决定的，如下例：

```c
void
func (int a, float b, double c, void *d)
{
    // a : GPR a0
    // b : FPR $f13
    // c : FPR $f14
    // d : GPR a3
}
 
func (0, 1.0, 2.1, NULL);
```

### 3.2 结构体参数传递

C语言允许使用结构体类型作为参数，为了和 MIPS 规则保持一致，被传递的结构体就成为了参数结构中的一部分，其内部结构布局和其通常的存储器映像是完全相同的。在C结构体内，字节或半字紧缩到单个字的存储器单元中，所以当我们通过寄存器来传递概念上属于驻留内存的结构时，我们不得不用数据装满寄存器以模仿存储器的数据排布。如下例：

```c
// Little-Endian
 
struct Arg
{
    char a;
    short b;
    int c;
    double d;
    int e;
};
 
void
test (struct Arg a)
{
    // a, b, c : a0 (value: 0x0000006400018863)
    // d : $f13 (value: 0x4008cccccccccccd)
    // e : a2 (value: 0x000000000000ff00)
}
 
struct Arg a = { 'c', 1, 100, 3.1, 0xff00 };
test (a);
```
上面的例子中，a0 寄存器的值有些复杂，其中低 8-bit 对应于 a = 0x63，bit8 – bit15 用于对齐，其值无效，bit16 – bit31 对应于 b = 0x0001, bit32 – bit63 对应于 c = 0x00000064

### 3.3 联合体参数传递

联合体中即有定点又有浮点数据类型时，以此联合体类型作为参数时，通过通用寄存器传递参数。如下例：

```c
union T
{
    uint64_t u;
    double d;
};
 
void
func (int a, union T b)
{
    // a : GPR a0
    // b : GPR a1
}
 
union T ud;
ud.d = 3.1;
func (0xff00, ud);
```

### 3.4 可变参数传递

目标函数原型是可变参数的情况下，前 8 个参数中确定类型的部分（第一个参数必然是确定的）根据类型传递，其余的参数统一在通用寄存器中传递。如下例：

```c
void
func0 (int a, ...)
{
    // a : GPR a0
    // b : GPR a1
    // c : GPR a2
    // d : GPR a3
}
 
func0 (0, 1, 1.0, NULL);
 
void
func1 (float a, ...)
{
    // a : FPR $f12
    // b : GPR a1
    // c : GPR a2
    // d : GPR a3
}
 
func1 (1.0, 0, 1, 1.0);
```

### 3.5 返回值传递

在返回基本数据类型的情况下，与常规参数传递一样，有专用的寄存器约定为传递返回值，整型在通用寄存器 v0 中返回，浮点在浮点寄存器 $f0 中返回。比较复杂的情况是返回结构体等导出数据类型，根据返回数据的长度可分为两种情况：

* 返回数据长度小于等于 16 字节，又按结构体成员的数据类型分为三种情况：

    * 所有成员都是整数型（包含指针）或整数与浮点型混合，返回数据在v0（低64位）和v1（高64位）寄存器传递，寄存器中数据布局与传递结构体参数相同。
    * 所有成员都是浮点型且数量小于等于2，其实就4种组合：float/float, float/double, double/double, double/float，这种情况下使用f0（第一个成员）和f2（第二个成员）寄存器返回。需要格外注意的是float类型成员独立占用一个浮点寄存器。
    * 所有成员都是浮点型且数量大于2，返回数据在v0（低64位）和v1（高64位）寄存器传递，寄存器中数据布局与传递结构体参数相同。（其实与情况a相同，独立出来为了引起注意。）

* 返回数据长度大于16字节，则调用者使用a0传递一个返回结构体的指针（也就是说调用者分配空间），其原有参数依次后移传递，返回时通过v0寄存器返回传入的结构体指针。

```c
// 1.a
struct RetVal
{
    char a;
    int b;
    float c;
};
 
struct RetVal
func (int a)
{
    // a : GPR a0
 
    // return:
    // a : GPR v0 (bit0 - bit7)
    // b : GPR v0 (bit32 - bit63)
    // c : GPR v1 (bit0 - bit31)
}
 
func (0xff);
 
// 1.b
struct RetVal
{
    float a;
    float b;
};
 
struct RetVal
func (int a)
{
    // a : GPR a0
 
    // return:
    // a : FPR f0
    // b : FPR f2
}
 
func (0xff);
 
// 1.c
struct RetVal
{
    float a;
    float b;
    float c;
    float d;
};
 
struct RetVal
func (int a)
{
    // a : GPR a0
 
    // return:
    // a : GPR v0 (bit0 - bit31)
    // b : GPR v0 (bit32 - bit63)
    // c : GPR v1 (bit0 - bit31)
    // d : GPR v1 (bit32 - bit63)
}
 
func (0xff);
 
// 2
struct RetVal
{
    char a;
    int b;
    float c;
    double d;
};
 
struct RetVal
func (int a)
{
    // AddressOf (RetVal) : GPR a0
    // a : GPR a1
 
    // return AddressOf (RetVal) : GPR v0
}
 
func (0xff);
```

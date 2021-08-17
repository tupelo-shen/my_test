[TOC]

大`S`、小`s`我们再熟悉不过了，据说性格完全不同。大S，甜美安静；小s，聒噪好动；不知你喜欢哪一个呢？哈哈，你想多了，一个苦逼码农，单身汪还敢想入非非......。

我说的大大`S`、小`s`，其实是这俩哥们了：`readelf -S file`和`readelf -s file`；是不是傻傻分不清？

## 1 readelf -S

那我们先来看看大S吧：

```
$ arm-linux-gnueabi-readelf -h
    ...
  -S --section-headers  显示section的header
     --sections         --section-headers的别名
    ...
  -s --syms              显示符号表
     --symbols           --syms的别名
    ...
```

#### 1.1 显示区头信息 | Section Header

```
$ arm-linux-gnueabi-readelf -S sel4test-tests
There are 31 section headers, starting at offset 0x21c024:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .note.gnu.build-i NOTE            00010114 000114 000024 00   A  0   0  4
  [ 2] .init             PROGBITS        00010138 000138 000010 00  AX  0   0  4
  [ 3] .text             PROGBITS        00010148 000148 0af904 00  AX  0   0  8
  [ 4] .fini             PROGBITS        000bfa4c 0afa4c 000010 00  AX  0   0  4
  [ 5] .rodata           PROGBITS        000bfa5c 0afa5c 01b68c 00   A  0   0  4
  [ 6] .ARM.extab        PROGBITS        000db0e8 0cb0e8 00009c 00   A  0   0  4
  [ 7] .ARM.exidx        ARM_EXIDX       000db184 0cb184 000120 00  AL  3   0  4
  [ 8] .eh_frame         PROGBITS        000db2a4 0cb2a4 000004 00   A  0   0  4
  [ 9] .tdata            PROGBITS        000dcfd0 0cbfd0 00001c 00 WAT  0   0  4
  [10] .tbss             NOBITS          000dcfec 0cbfec 000020 00 WAT  0   0  4
  [11] .init_array       INIT_ARRAY      000dcfec 0cbfec 000010 04  WA  0   0  4
  [12] .fini_array       FINI_ARRAY      000dcffc 0cbffc 000004 04  WA  0   0  4
  [13] .got              PROGBITS        000dd000 0cc000 000024 04  WA  0   0  4
  [14] .data             PROGBITS        000dd028 0cc028 000978 00  WA  0   0  8
  [15] _test_case        PROGBITS        000dd9a0 0cc9a0 0025c0 00  WA  0   0  4
  [16] __vsyscall        PROGBITS        000dff60 0cef60 000004 00  WA  0   0  4
  [17] _ps_irqchips      PROGBITS        000dff64 0cef64 000014 00  WA  0   0  4
  [18] .bss              NOBITS          000e0000 0cef78 13b048 00  WA  0   0 4096
  [19] .comment          PROGBITS        00000000 0cef78 000030 01  MS  0   0  1
  [20] .ARM.attributes   ARM_ATTRIBUTES  00000000 0cefa8 00002d 00      0   0  1
  [21] .debug_aranges    PROGBITS        00000000 0cefd8 000d18 00      0   0  8
  [22] .debug_info       PROGBITS        00000000 0cfcf0 0bf594 00      0   0  1
  [23] .debug_abbrev     PROGBITS        00000000 18f284 00feca 00      0   0  1
  [24] .debug_line       PROGBITS        00000000 19f14e 0356d8 00      0   0  1
  [25] .debug_frame      PROGBITS        00000000 1d4828 016348 00      0   0  4
  [26] .debug_str        PROGBITS        00000000 1eab70 010750 01  MS  0   0  1
  [27] .debug_ranges     PROGBITS        00000000 1fb2c0 0001d8 00      0   0  8
  [28] .symtab           SYMTAB          00000000 1fb498 0150c0 10     29 4441  4
  [29] .strtab           STRTAB          00000000 210558 00b996 00      0   0  1
  [30] .shstrtab         STRTAB          00000000 21beee 000134 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  y (purecode), p (processor specific)
```

> 解读：结果主要分为三个部分：区名称（`section header name`）、区类型（`section header type`）和区标志（`section header flag`）
> 
> * `Name`部分：出现了一些熟悉的内容 `.bss`，`.text`，但更多是看不懂的 `.fini`，`.plt` ，`.relname`；
> * `Type`部分：就有更多看不懂的 `NULL`，`PROGBITS`，`INIT_ARRAY`等等；
> * `Flag`部分：好像也似懂非懂；

一个区只属于一个类型，具有排它性，跟男人，女人一样。但身上可以贴多个标签：可以是码农，可以是高富帅，可以是脱发男，不对!!! 码农你还想是高富帅，想多了，脱发才是你的标配。例如:

* 代码区(`.text`)属于`PROGBITS`类型被贴上了`AX (alloc + execute)`标签。原来代码区可以被CPU取指运行是因为在`ELF`中被贴上了可运行标签。但注意`.text`是只读不可写，因为它身上没有`write`标签；

* 再看熟悉两个数据区`.bss`和`.data`，它们都有`WA(write+alloc)`标签，`可写`+`运行过程中需要占用内存`。但，二者区别是类型的不同，`.bss`是`NOBITS`类型，`.data`是`PROGBITS`类型。

#### 1.2 区名称 | Section Header Name

一般缩写为`SHN`。

在ELF文件中有一些特定的区是预定义好的，其内容是指令代码或者控制信息.这些区专门为操作系统使用，对于不同的操作系统，这些区的类型和属性有所不同。

在构建可执行程序时，链接器(linker)可能需要把一些独立的目标文件和库文件链接在一起，在这个过程中，链接器要解析各个文件中的相互引用，调整某些目标文件中的绝对引用，并重定位指令码。

每种操作系统都有自己的一套链接模型，但总的来说，不外乎静态和动态两类：

* **静态链接**：所有的目标文件和动态链接库被静态地绑定在一起，所有的符号都被解析出来.所创建的目标文件是完整的，运行时不依赖于任何外部的库。

* **动态链接**：所有的目标文件，系统共享资源以及共享库以动态的形式链接在一起，外部库的内容没有完整地拷贝进来。如果创建的是可执行文件的话，程序在运行的时候，在构建时所依赖的那些库必须在系统中能找到，把它们一并装载之后，程序才能运行起来。运行期间如何解析那些动态链接进来的符号引用，不同的系统有各自不同的方式。

根据区功能划分:

* 有些区包含调试信息，比如`.debug`和`.line`区；
* 有些区包含程序控制信息，比如`.bss`，`.data`，`.data1`，`.rodata`和`.rodata1`这些区；
* 还有一些区含有程序或控制信息，这些区由系统使用，有指定的类型和属性。它们中的大多数都将用于链接过程。动态链接过程所需要的信息由`.dynsym`，`.dynstr`，`.interp`，`.hash`，`.dynamic`，`.rel`，`.rela`，`.got`，`.plt`等区提供。其中有些区(比如`.plt`和`.got`)的内容依处理器而不同，但它们都支持同样的链接模型。

以点号`.`为前缀的区名字是为系统保留的。应用程序也可以构造自己的区，但最好不要取与上述系统已定义的区相同的名字，也不要取以点号开头的名字，以避免潜在的冲突，注意，目标文件中区的名字并不具有唯一性，可以存在多个相同名字的区。具体如下:

```
区名              描述说明
.bss            本区中包含目标文件中未初始化的全局变量。一般情况下，可执行程序在开始运行的时候，系统会把这一区内容清零。但是，
                在运行期间的bss区是由系统初始化而成的，而在目标文件中.bss区并不包含任何内容，其长度为0，所以它的区类型为NOBITS。
.comment        本区包含版本控制信息。
.data           这个区不陌生，用于存放程序中被初始化过的全局变量。在目标文件中，它们是占用实际的存储空间的，与.bss区不同。
.debug          本区中含有调试信息，内容格式没有统一规定。所有以".debug"为前缀的区名字都是保留的。
.dynamic        本区包含动态链接信息，并且可能有SHF_ALLOC和SHF_WRITE等属性。是否具有SHF_WRITE属性取决于操作系统和处理器。
.dynstr         本区含有用于动态链接的字符串，一般是那些与符号表相关的名字。具有SHF_ALLOC属性。
.dynsym         本区含有动态链接符号表.具有SHF_ALLOC属性，因为它需要在运行时被加载。
.got            本区包含全局偏移量表(global offset table)。
.hash           本区包含一张符号哈希表。
.init           本区包含进程初始化时要执行的程序指令，当程序开始运行时，系统会在进入主函数之前执行这一区中的代码。
.fini           程序终止代码区，当程序结束运行时，系统会在最后执行这一区中的代码.
.interp         本区含有ELF程序解析器的路径名。如果本区被包含在某个可装载的区中，那么本区的属性中应置SHF_ALLOC标志位，否则不置此标志。
.line           本区也是一个用于调试的区，它包含那些调试符号的行号，为程序指令码与源文件的行号建立起联系.其内容格式没有统一规定。
.note           本区所包含的信息在第2章"注释区(note section)"部分描述。
.plt            本区包含函数链接表。动态链接时使用的过程链接表(precedure linkage table)
.relname        同下
.relaname       这两个区含有重定位信息.如果本区被包含在某个可装载的区中，那么本区的属性中应置SHF_ALLOC标志位，否则不置此标志.注意，这两个区的名字中"name"是可替换的部分，执照惯例，
                对哪一区做重定位就把"name"换成哪一区的名字.比如，.text区的重定位区的名字将是.rel.text或.rela.text。
.rodata         同下
.rodata1        本区包含程序中的只读数据，在程序装载时，它们一般会被装入进程空间中那些只读的区中去。
.shstrtab       本区是"区名字表"，含有所有其它区的名字，如 `.data`，`.bss`，`.text`...
.strtab         本区用于存放字符串，主要是那些符号表项的名字.如果一个目标文件有一个可装载的区，并且其中含有符号表，存储的是变量名，函数名等。
.symtab         本区用于存放符号表.如果一个目标文件有一个可载入的区，并且其中含有符号表，那么本区的属性中应该有SHF_ALLOC。
.text           本区包含程序指令代码。
.rel.text       重定位的地方在.text段内，以offset指定具体要定位位置。在连接时候由连接器完成。注意比较.text段前后变化。
                指的是比较.o文件和最终的执行文件（或者动态库文件）。就是重定位前后比较，以上是说明了具体比较对象而已。 
                一般由编译器编译产生，存在于obj文件内。
.rel.dyn        重定位的地方在.got段内。主要是针对外部数据变量符号。例如全局数据。重定位在程序运行时定位，
                一般是在.init段内。定位过程：获得符号对应value后，根据rel.dyn表中对应的offset，修改.got表对应位置的
                value。另外，.rel.dyn 含义是指和dyn有关，一般是指在程序运行时候，动态加载。
                区别于rel.plt，rel.plt是指和plt相关，具体是指在某个函数被调用时候加载。
                一般由连接器产生，存在于可执行文件或者动态库文件。
.rel.plt        重定位的地方在.got.plt段内（注意也是.got内，具体区分而已）。 主要是针对外部函数符号。一般是函数首次
                被调用时候重定位。可看汇编，理解其首次访问是如何重定位的，实际很简单，就是初次重定位函数地址，
                然后把最终函数地址放到.got.plt内，以后读取该.got.plt就直接得到最终函数地址(参考过程说明)。  
                所有外部函数调用都是经过一个对应桩函数，这些桩函数都在.plt段内。 
                一般由连接器产生，存在于可执行文件或者动态库文件。
                借助这两个辅助段可以动态修改对应.got和.got.plt段，从而实现运行时重定位。
.rel.data      常量区重定位信息
.rel.rodata    数据段重定位信息    
```

#### 1.3 区类型 | Section Header Type

简称:`SHT`

```
SHT_NULL        本区头是一个无效的（非活动的）区头，它也没有对应的区.本区头中的其它成员的值也都是没有意义的。
SHT_PROGBITS    本区所含有的信息是由程序定义的，本区内容的格式和含义都由程序来决定。
SHT_SYMTAB      同DYNSYM
SHT_DYNSYM      这两类区都含有符号表.目前，目标文件中最多只能各包含一个这两种区，但这种限制以后可能会取消。
                一般来说，SYMTAB提供的符号用于在创建目标文件的时候编辑链接，在运行期间也有可能会用于动态链接。
                SYMTAB包含完整的符号表，它往往会包含很多在运行期间(动态链接)用不到的符号.所以，一个目标文件
                可以再有一个DYNSYM区，它含有一个较小的符号表，专门用于动态链接。
SHT_STRTAB      本区是字符串表.目标文件中可以包含多个字符串表区。
SHT_RELA        本区是一个重定位区，含有带明确加数(addend)的重定位项，对于32位类型的目标文件来说，
                这个加数就是Elf32_Rela.一个目标文件可能含有多个重定位区。
SHT_HASH        本区包含一张哈希表.所有参与动态链接的目标文件都必须要包含一个符号哈希表.目前，一个目标文件中最多只能有一个哈希表，
                但这一限制以后可能会取消。
SHT_DYNAMIC     本区包含的是动态链接信息。目前，一个目标文件中最多只能有一个DYNAMIC区，
                但这一限制以后可能会取消。
SHT_NOTE        本区包含的信息用于以某种方式来标记本文件。
SHT_NOBITS      这一区的内容是空的，区并不占用实际的空间。只代表一个逻辑上的位置概念，并不代表实际的内容。
SHT_REL         本区是一个重定位区，含有带明确加数的重定位项，对于32位类型的目标文件来说，这个加数就是Elf32_Rel。一个目标文件可能含有多个重定位区。
SHT_SHLIB       此值是一个保留值，暂未指定语义。
SHT_LOPROC      为特殊处理器保留的区类型索引值的下边界。
SHT_HIPROC      为特殊处理器保留的区类型索引值的上边界。LOPROC ~ HIPROC区间是为特殊处理器区类型的保留值。
SHT_LOUSER      为应用程序保留区类型索引值的下边界。
SHT_HIUSER      为应用程序保留区类型索引值的下边界。LOUSER ~ HIUSER区间的区类型可由应用程序自行定义，是一区保留值。
```

> 解读：
> 
> * `.bss` 类型为 `NOBITS`，这一区的内容是空的，区并不占用实际的空间， 没有初值的全局变量就放在这个区。它是真没有值，由运行过程中映射到哪个地址就取哪个地址的值。鬼知道跑哪个位置的
> * `PROGBITS`本区内容的格式和含义都由程序来决定，属于这个区的内容还挺多的 `.text`，`.data`， `.init`， `.rodata` ，这些区默认自带运行时数据。不需要你额外提供，区别是这些自带数据运行时可不可以被改变。`.data`可以被程序运行时逻辑所修改，`.rodata`不可改，即常量数据。

#### 1.4 区标签 | Section Head Flag

简称:`SHF`

```
Key to Flags:
  W (write)， A (alloc)， X (execute)， M (merge)， S (strings)， I (info)，
  L (link order)， O (extra OS processing required)， G (group)， T (TLS)，
  C (compressed)， x (unknown)， o (OS specific)， E (exclude)，
  l (large)， p (processor specific)
```

说明：

```
名字         值       描述
SHF_WRITE       0x01        如果此标志被设置，表示本区所包含的内容在进程运行过程中是可写的。
SHF_ALLOC       0x02        如果此标志被设置，表示本区内容在进程运行过程中要占用内存单元。并不是所有区。
                            都会占用实际的内存，有一些起控制作用的区，在目标文件映射到进程空间时，并不需要占用内存。
SHF_EXECUTE     0x04        如果此标志被设置，表示本区内容是指令代码。
```

此处看下与数据相关的三个区，仔细对照看参数发现其真正的区别.

```
  [ 5] .rodata           PROGBITS        000bfa5c 0afa5c 01b68c 00   A  0   0  4
  [14] .data             PROGBITS        000dd028 0cc028 000978 00  WA  0   0  8
  [18] .bss              NOBITS          000e0000 0cef78 13b048 00  WA  0   0 4096
```

## 2 readelf -s

说完大`S`再来说小`s`。

#### 2.1 显示所有符号表 | Symbol Table

```
$ arm-linux-gnueabi-readelf -s sel4test-tests 

Symbol table '.symtab' contains 5388 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
   ...省略
  5024: 000169e4   788 FUNC    GLOBAL DEFAULT    3 main
```

#### 2.2 符号表绑定 | Symbol Table Bind

简称 `STB`

```
STB_LOCAL       表明本符号是一个本地符号.它只出现在本文件中，在本文件外该符号无效。
                所以在不同的文件中可以定义相同的符号名，它们之间不会互相影响。
STB_GLOBAL      表明本符号是一个全局符号。当有多个文件被链接在一起时，在所有文件中该符号都是可见的。
                正常情况下，在一个文件中定义的全局符号，一定是在其它文件中需要被引用，否则无须定义为全局。
STB_WEAK        类似于全局符号，但是相对于STB_GLOBAL，它们的优先级更低。
                全局符号(global symbol)和弱符号(weak symbol)在以下两方面有区别：
                1. 当链接编辑器把若干个可重定位目标文件链接起来时，同名的STB_GLOBAL符号不允许出现多次。
                    而如果在一个目标文件中已经定义了一个全局的符号(global symbol)，当一个同名的弱符号(weak symbol)出现时，并不会发生错误。
                    链接编辑器会以全局符号为准，忽略弱符号。与全局符号相似，如果已经存在的是一个公用符号，即st_shndx域为SHN_COMMON值的符号，当一个同名的弱符号(weak symbol)出现时，也不会发生错误。
                    链接编辑器会以公用符号为准，忽略弱符号。
                2. 在查找符号定义时，链接编辑器可能会搜索存档的库文件。如果是查找全局符号，链接编辑器会提取包含该未定义
                    的全局符号的存档成员，存档成员可能是一个全局的符号，也可能是弱符号。而如果是查找弱符号，链接编辑器不会去提取存档成员。未解析的弱符号值为0。
STB_LOPROC ~ STB_HIPROC 为特殊处理器保留的属性区间。
```

#### 2.3 符号表类型 | Symbol Table Type

简称 `STT`

```
STT_NOTYPE      本符号类型未指定。
STT_OBJECT      本符号是一个数据对象，比如变量，数组等。
STT_FUNC        本符号是一个函数，或者其它的可执行代码。函数符号在共享目标文件中有特殊的意义。当另外一个目标文件引用一个共享目标文件中的函数符号时，链接编辑器为被引用符号自动创建一个链接表项。非STT_FUNC类型的共享目标符号不会通过这种链接表项被自动引用。
STT_SECTION     本符号与一个区相关联，用于重定位，通常具有STB_LOCAL属性。
STT_FILE        本符号是一个文件符号，它具有STB_LOCAL属性，它的区索引值是SHN_ABS。在符号表中如果存在本类符号的话，它会出现在所有STB_LOCAL类符号的前部。
STT_LOPROC ~ STT_HIPROC 这一区间的符号类型为特殊处理器保留。
```

#### 2.4 符号表可见性 | Symbol Table Visibility

简称 `STV`

```
STV_DEFAULT     当符号的可见性是STV_DEFAULT时，那么该符号的可见性由符号的绑定属性决定。
                这类情况下，（可执行文件和共享库中的）全局符号和弱符号默认是外部可访问的，
                本地符号默认外部是无法被访问的。但是，可见性是STV_DEFAULT的全局符号和弱符号是可被覆盖的。
                什么意思？举个最典型的例子，共享库中的可见性值为STV_DEFAULTD的全局符号和弱符号
                是可被可执行文件中的同名符号覆盖的。
STV_HIDDEN      当符号的可见性是STV_HIDDEN时，证明该符号是外部无法访问的。这个属性主要
                用来控制共享库对外接口的数量。需要注意的是，一个可见性为STV_HIDDEN的数据对象，
                如果能获取到该符号的地址，那么依然是可以访问或者修改该数据对象的。在可重定位文件中，
                如果一个符号的可见性是STV_HIDDEN的话，那么在链接生成可执行文件或者共享库的过程中，
                该符号要么被删除，要么绑定属性变成STB_LOCAL。
STV_PROTECTED   当符号的可见性是STV_PROTECTED时，它是外部可见的，这点跟可见性是STV_DEFAULT的一样，
                但不同的是它是不可覆盖的。这样的符号在共享库中比较常见。不可覆盖意味着如果是在该符号
                所在的共享库中访问这个符号，那么就一定是访问的这个符号，尽管可执行文件中也会存在
                同样名字的符号也不会被覆盖掉。规定绑定属性为STB_LOCAL的符号的可见性不可以是STV_PROTECTED。
STV_INTERNAL    该可见性属性的含义可以由处理器补充定义，以进一步约束隐藏的符号。 处理器补充程序的定义
                应使通用工具可以安全地将内部符号视为隐藏符号。当可重定位对象包含在可执行文件或共享对象中时，
                可重定位对象中包含的内部符号必须被链接编辑器删除或转换为STB_LOCAL绑定。
```

#### 2.5 符号表索引 | Symbol Table Ndx

简称 STN 任何一个符号表项的定义都与某一个"区"相联系，因为符号是为区而定义，在区中被引用。本数据成员即指明了相关联的区。本数据成员是一个索引值，它指向相关联的区在区头表中的索引。在重定位过程中，区的位置会改变，本数据成员的值也随之改变，继续指向区的新位置。当本数据成员指向下面三种特殊的区索引值时，本符号具有如下特别的意义：

```
SHN_ABS     符号的值是绝对的，具有常量性，在重定位过程中，此值不需要改变。
SHN_COMMON  本符号所关联的是一个还没有分配的公共区，本符号的值规定了其内容的字区对齐规则，
            与sh_addralign相似。也就是说，链接器会为本符号分配存储空间，而且其起始地址是
            向st_value对齐的。本符号的值指明了要分配的字区数。
SHN_UNDEF   当一个符号指向第1区(SHN_UNDEF)时，表明本符号在当前目标文件中未定义，在链接过程中，
            链接器会找到此符号被定义的文件，并把这些文件链接在一起。
            本文件中对该符号的引用会被链接到实际的定义上去。
```


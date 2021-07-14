> There are only two hard things in Computer Science: cache invalidation and naming things.
>
> -- Phil Karlton

[TOC]

## 1 Cache 基础

对cache的掌握，对于Linux工程师（其他的非Linux工程师也一样）写出高效能代码，以及优化Linux系统的性能是至关重要的。简单来说，cache快，内存慢，硬盘更慢。在一个典型的现代CPU中比较接近改进的哈佛结构，cache的排布大概是这样的：

<img id="Figure_1-1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-Cache-understanding-1-1.PNG">

可以使用`lstopo`命令查看机器的Cache布局，如下所示：

<img id="Figure_1-2" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-2.PNG">

## 2 Cache 架构

现代的cache基本按照这个模式来组织：SET、WAY、TAG、INDEX，这几个概念是理解Cache的关键。随便打开一个数据手册，就可以看到这样的字眼：

<img id="Figure_1-3" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-3.PNG">

翻译成中文就是4路（way）组（set）相联，VIPT表现为PIPT（这尼玛什么鬼？），cacheline的长度是64字节。

下面我们来想象一个16KB大小的cache，假设是4路组相联，cacheline的长度是64字节。Cacheline的概念比较简单，cache的整个替换是以行为单位的，一行64个字节里面读了任何一个字节，其实整个64字节就进入了cache。

比如下面两段程序，前者的计算量是后者的8倍：

程序1：test0.c

```c
int a[0xFF00000] __attribute__ ((aligned (64)));

int main(void)
{
    int c;
    for (int i = 0; i < 0xFF00000; i+=2)
    {
        c *= a[i];
    }
}
```

程序2：test1.c

```c
int a[0xFF00000] __attribute__ ((aligned (64)));

int main(void)
{
    int c;
    for (int i = 0; i < 0xFF00000; i+=16)
    {
        c *= a[i];
    }
}
```

但是它的执行时间，则远远不到后者的8倍：

```shell
ubuntu@ubuntu-VirtualBox:~/test$ time ./test0

real    0m0.385s
user    0m0.200s
sys     0m0.180s
```

```shell
ubuntu@ubuntu-VirtualBox:~/test$ time ./test1

real    0m0.162s
user    0m0.044s
sys     0m0.112s
```

16KB的cache是4way的话，每个set包括4*64B，则整个cache分为`16KB/64B/4 = 64set`，也即2的6次方。当CPU从cache里面读数据的时候，它会用地址位的`BIT6-BIT11`来寻址set，`BIT0-BIT5`是cacheline内的offset。

<img id="Figure_1-4" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-4.PNG">

比如CPU访问地址

0 <font color="red">000000</font> XXXXXX

或者

1 <font color="red">000000</font> XXXXXX

或者

YYYY <font color="red">000000</font> XXXXXX

由于它们红色的6位都相同，所以他们全部都会找到第0个set的cacheline。第0个set里面有4个way，之后硬件会用地址的高位如0,1，YYYY作为tag，去检索这4个way的tag是否与地址的高位相同，而且cacheline是否有效，如果tag匹配且cacheline有效，则cache命中。

所以地址YYYYYY<font color="red">000000</font>XXXXXX全部都是找第0个set，YYYYYY<font color="red">000001</font>XXXXXX全部都是找第1个set，YYYYYY<font color="red">111111</font>XXXXXX全部都是找第63个set。每个set中的4个way，都有可能命中。

中间红色的位就是INDEX，前面YYYY这些位就是TAG。具体的实现可以是用虚拟地址或者物理地址的相应位做TAG或者INDEX。如果用虚拟地址做TAG，我们叫VT；如果用物理地址做TAG，我们叫PT；如果用虚拟地址做INDEX，我们叫VI；如果用物理地址做TAG，我们叫PT。工程中碰到的cache可能有这么些组合： VIVT、VIPT、PIPT。

VIVT的硬件实现开销最低，但是软件维护成本高；PIPT的硬件实现开销最高，但是软件维护成本最低；VIPT介于二者之间，但是有些硬件是VIPT，但是behave as PIPT，这样对软件而言，维护成本与PIPT一样。

> 在VIVT的情况下，CPU发出的虚拟地址，不需要经过MMU的转化，直接就可以去查cache。
>
> <img id="Figure_1-5" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-5.PNG">
> 
> 而在VIPT和PIPT的场景下，都涉及到虚拟地址转换为物理地址后，再去比对cache的过程。VIPT如下：
> 
> <img id="Figure_1-6" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-6.PNG">
> 
> PIPT如下：
> 
> <img id="Figure_1-7" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-7.PNG">

从图上看起来，VIVT的硬件实现效率很高，不需要经过MMU就可以去查cache了。不过，对软件来说，这是个灾难。因为VIVT有严重的歧义和别名问题。

* 歧义：一个虚拟地址先后指向两个（或者多个）物理地址
* 别名：两个（或者多个）虚拟地址同时指向一个物理地址

这里我们重点看别名问题。比如2个虚拟地址对应同一个物理地址，基于VIVT的逻辑，无论是INDEX还是TAG，2个虚拟地址都是可能不一样的(尽管他们的物理地址一样，但是物理地址在cache比对中完全不掺和)，这样它们完全可能在2个cacheline同时命中。

<img id="Figure_1-8" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-8.PNG">

由于2个虚拟地址指向1个物理地址，这样CPU写过第一个虚拟地址后，写入cacheline1。CPU读第2个虚拟地址，读到的是过时的cacheline2，这样就出现了不一致。所以，为了避免这种情况，软件必须写完虚拟地址1后，对虚拟地址1对应的cache执行clean，对虚拟地址2对应的cache执行invalidate。

而PIPT完全没有这样的问题，因为无论多少虚拟地址对应一个物理地址，由于物理地址一样，我们是基于物理地址去寻找和比对cache的，所以不可能出现这种别名问题。

<img id="Figure_1-9" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-9.PNG">

那么VIPT有没有可能出现别名呢？答案是有可能，也有可能不能。如果VI恰好对于PI，就不可能，这个时候，VIPT对软件而言就是PIPT了：

```c
VI=PI
PT=PT
```

那么什么时候VI会等于PI呢？这个时候我们来回忆下虚拟地址往物理地址的转换过程，它是以页为单位的。假设一页是4K，那么地址的低12位虚拟地址和物理地址是完全一样的。回忆我们前面的地址：

YYYYY<font color="red">000000</font>XXXXXX

其中红色的000000是INDEX。在我们的例子中，红色的6位和后面的XXXXXX（cache内部偏移）加起来正好12位，所以这个000000经过虚实转换后，其实还是000000的，这个时候VI=PI，VIPT没有别名问题。

我们原先假设的cache是：16KB大小的cache，假设是4路组相联，cacheline的长度是64字节，这样我们正好需要红色的6位来作为INDEX。但是如果我们把cache的大小增加为32KB，这样我们需要  32KB/4/64B=128=2^7，也即7位来做INDEX。

YYYYY<font color="red">000000</font>XXXXXX

这样VI就可能不等于PI了，因为红色的最高位超过了2^12的范围，完全可能出现如下2个虚拟地址，指向同一个物理地址：

<img id="Figure_1-10" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-10.PNG">

这样就出现了别名问题，我们在工程里，可能可以通过一些办法避免这种别名问题，比如软件在建立虚实转换的时候，把虚实转换往2^13而不是2^12对齐，让物理地址的低13位而不是低12位与物理地址相同，这样强行绕开别名问题，下图中，2个虚拟地址指向了同一个物理地址，但是它们的INDEX是相同的，这样VI=PI，就绕开了别名问题。这通常是PAGE COLOURING技术中的一种技巧。

<img id="Figure_1-11" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-11.PNG">

如果这种PAGE COLOURING的限制对软件仍然不可接受，而我们又想享受VIPT的INDEX不需要经过MMU虚实转换的快捷？有没有什么硬件技术来解决VIPT别名问题呢？确实是存在的，现代CPU很多都是把L1 CACHE做成VIPT，但是表现地（behave as）像PIPT。这是怎么做到的呢？

这要求VIPT的cache，硬件上具备alias detection的能力。比如，硬件知道YYYY0000000XXXXXX既有可能出现在第0000000，又可能出现在1000000这2个set，然后硬件自动去比对这2个set里面是否出现映射到相同物理地址的cacheline，并从硬件上解决好别名同步，那么软件就完全不用操心了。

下面我们记住一个简单的规则：

* 对于VIPT，如果cache的size除以WAY数，小于等于1个page的大小，则天然VI=PI，无别名问题；
* 对于VIPT，如果cache的size除以WAY数，大于1个page的大小，则天然VI≠PI，有别名问题；这个时候又分成2种情况:
    - 硬件不具备alias detection能力，软件需要pagecolouring；
    - 硬件具备alias detection能力，软件把cache当成PIPT用。

比如cache大小64KB，4WAY，PAGE SIZE是4K，显然有别名问题；这个时候，如果cache改为16WAY，或者PAGE SIZE改为16K，不再有别名问题。为什么？感觉小学数学知识也能算得清。

## 3 Cache 一致性

Cache的一致性有这么几个层面：

1. 一个CPU的icache和dcache的同步问题；
2. 多个CPU各自的cache同步问题
3. CPU与设备（其实也可能是个异构处理器，不过在Linux运行的CPU眼里，都是设备，都是DMA）的cache同步问题。

<img id="Figure_1-12" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-12.PNG">

先看一下ICACHE和DCACHE同步问题。由于程序的运行而言，指令流的都流过icache，而指令中涉及到的数据流经过dcache。所以对于自修改的代码（Self-Modifying Code）而言，比如我们修改了内存p这个位置的代码（典型多见于JIT compiler），这个时候我们是通过store的方式去写的p，所以新的指令会进入dcache。但是我们接下来去执行p位置的指令的时候，icache里面可能命中的是修改之前的指令。

<img id="Figure_1-13" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-13.PNG">

所以这个时候软件需要把dcache的东西clean出去，然后让icache invalidate，这个开销显然还是比较大的。

但是，比如ARM64的N1处理器，它支持硬件的icache同步，详见文档：[The Arm Neoverse N1 Platform: Building Blocks for the Next-Gen Cloud-to-Edge Infrastructure SoC]

<img id="Figure_1-14" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-14.PNG">

特别注意画红色的几行。软件维护的成本实际很高，还涉及到icache的invalidation向所有核广播的动作。

接下来的一个问题就是多个核之间的cache同步。下面是一个简化版的处理器，CPU_A和B共享了一个L3，CPU_C和CPU_D共享了一个L3。实际的硬件架构由于涉及到NUMA，会比这个更加复杂，但是这个图反映层级关系是足够了。

<img id="Figure_1-15" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-15.PNG">

比如CPU_A读了一个地址p的变量？CPU_B、C、D又读，难道B,C,D又必须从RAM里面经过L3,L2,L1再读一遍吗？这个显然是没有必要的，在硬件上，cache的snooping控制单元，可以协助直接把CPU_A的p地址cache拷贝到CPU_B、C和D的cache。

<img id="Figure_1-16" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-16.PNG">

这样A-B-C-D都得到了相同的p地址的棕色小球。

假设CPU B这个时候，把棕色小球写成红色，而其他CPU里面还是棕色，这样就会不一致了：

<img id="Figure_1-17" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-17.PNG">

这个时候怎么办？这里面显然需要一个协议，典型的多核cache同步协议有MESI和MOESI。MOESI相对MESI有些细微的差异，不影响对全局的理解。下面我们重点看MESI协议。

MESI协议定义了4种状态：

1. M（Modified）: 当前cache的内容有效，数据已被修改而且与内存中的数据不一致，数据只在当前cache里存在；类似RAM里面是棕色球，B里面是红色球（CACHE与RAM不一致），A、C、D都没有球

<img id="Figure_1-18" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-18.PNG">

2. E（Exclusive）：当前cache的内容有效，数据与内存中的数据一致，数据只在当前cache里存在；类似RAM里面是棕色球，B里面是棕色球（RAM和CACHE一致），A、C、D都没有球。

<img id="Figure_1-19" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-19.PNG">

3. S（Shared）：当前cache的内容有效，数据与内存中的数据一致，数据在多个cache里存在。类似如下图，在CPU A-B-C里面cache的棕色球都与RAM一致。

<img id="Figure_1-20" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-20.PNG">

4. I（Invalid）：当前cache无效。前面三幅图里面cache没有球的那些都是属于这个情况。

然后它有个状态机:

<img id="Figure_1-21" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-21.PNG">

这个状态机比较难记，死记硬背是记不住的，也没必要记，它讲的cache原先的状态，经过一个硬件在本cache或者其他cache的读写操作后，各个cache的状态会如何变迁。所以，硬件上不仅仅是监控本CPU的cache读写行为，还会监控其他CPU的。只需要记住一点：这个状态机是为了保证多核之间cache的一致性，比如一个干净的数据，可以在多个CPU的cache share，这个没有一致性问题；但是，假设其中一个CPU写过了，比如A-B-C本来是这样：

<img id="Figure_1-22" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-22.PNG">

然后B被写过了：

<img id="Figure_1-23" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-23.PNG">

这样A、C的cache实际是过时的数据，这是不允许的。这个时候，硬件会自动把A、C的cache invalidate掉，不需要软件的干预，A、C其实变地相当于不命中这个球了：

<img id="Figure_1-24" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-24.PNG">

这个时候，你可能会继续问，如果C要读这个球呢？它目前的状态在B里面是modified的，而且与RAM不一致，这个时候，硬件会把红球clean，然后B、C、RAM变地一致，B、C的状态都变化为S（Shared）：

<img id="Figure_1-25" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-25.PNG">

这一系列的动作虽然由硬件完成，但是对软件而言不是免费的，因为它耗费了时间。如果编程的时候不注意，引起了硬件的大量cache同步行为，则程序的效率可能会急剧下降。

为了让大家直观感受到这个cache同步的开销，下面我们写一个程序，这个程序有2个线程，一个写变量，一个读变量：

test2.c

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

static int x __attribute__ ((aligned(64)));
static int y __attribute__ ((aligned(64)));

void* thread_fun1(void* param)
{
    for (int i = 0; i < 1000000000; ++i)
    {
        x++;
    }

    return NULL;
}

void* thread_fun2(void* param)
{
    volatile int c;
    for (int i = 0; i < 1000000000; ++i)
    {
        c = x;
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t   tid1, tid2;
    
    pthread_create(&tid1, NULL, thread_fun1, NULL);
    pthread_create(&tid2, NULL, thread_fun2, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}
```

这个程序里，x和y都是cacheline对齐的，这个程序的thread1的写，会不停地与thread2的读，进行cache同步。

它的执行时间为：

```c
ubuntu@ubuntu-VirtualBox:~/test$ time ./test2

real    0m3.795s
user    0m7.084s
sys     0m0.000s
```
它在2个CPU上的userspace共运行了7.084秒，累计这个程序从开始到结束的对应真实世界的时间是3.795秒（就是从命令开始到命令结束的时间）。

如果我们把程序改一句话，把thread2里面的`c = x`改为`c = y`，这样2个线程在2个CPU运行的时候，读写的是不同的cacheline，就没有这个硬件的cache同步开销了：

```c
void* thread_fun2(void* param)
{
    volatile int c;
    for (int i = 0; i < 1000000000; ++i)
    {
        // c = x;
        c = y;
    }

    return NULL;
}
```

它的运行时间：

```
ubuntu@ubuntu-VirtualBox:~/test$ time ./test2

real    0m1.656s
user    0m3.284s
sys     0m0.000s
```

现在只需要1.656秒，减小了一半。

感觉前面那个test2，双核的帮助甚至都不大。如果我们改为单核跑呢？

```c
ubuntu@ubuntu-VirtualBox:~/test$ time taskset -c 0 ./test2

real    0m3.274s
user    0m3.256s
sys     0m0.000s
```

它单核跑，居然只需要3.274秒跑完，而双核跑，需要3.795s跑完。单核跑完这个程序，甚至比双核还快，有没有惊掉下巴？！！！因为单核里面没有cache同步的开销。

下一个cache同步的重大问题，就是设备与CPU之间。如果设备感知不到CPU的cache的话（下图中的红色数据流向不经过cache），这样，做DMA前后，CPU就需要进行相关的cacheclean和invalidate的动作，软件的开销会比较大。

<img id="Figure_1-26" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-26.PNG">

这些软件的动作，若我们在Linux编程的时候，使用的是streaming DMA APIs的话，都会被类似这样的API自动搞定：

```c
dma_map_single()
dma_unmap_single()
dma_sync_single_for_cpu()
dma_sync_single_for_device()
dma_sync_sg_for_cpu()
dma_sync_sg_for_device()
```

如果是使用的dma_alloc_coherent() API呢，则设备和CPU之间的buffer是cache一致的，不需要每次DMA进行同步。对于不支持硬件cache一致性的设备而言，很可能dma_alloc_coherent()会把CPU对那段DMA buffer的访问设置为uncachable的。

这些API把底层的硬件差异封装掉了，如果硬件不支持CPU和设备的cache同步的话，延时还是比较大的。那么，对于底层硬件而言，更好的实现方式，应该仍然是硬件帮我们来搞定。比如我们需要修改总线协议，延伸红线的触角：

<img id="Figure_1-27" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-27.PNG">

当设备访问RAM的时候，可以去snoop CPU的cache：

* 如果做内存到外设的DMA，则直接从CPU的cache取modified的数据；
* 如果做外设到内存的DMA，则直接把CPU的cache invalidate掉。

这样，就实现硬件意义上的cache同步。当然，硬件的cache同步，还有一些其他方法，原理上是类似的。注意，这种同步仍然不是免费的，它仍然会消耗bus cycles的。实际上，cache的同步开销还与距离相关，可以说距离越远，同步开销越大，比如下图中A、B的同步开销比A、C小。

<img id="Figure_1-28" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-28.PNG">

对于一个NUMA服务器而言，跨NUMA的cache同步开销显然是要比NUMA内的同步开销大。

## 4 意识到CACHE的编程

通过上一节的代码，读者应该意识到了cache的问题不处理好，程序的运行性能会急剧下降。所以意识到cache的编程，对程序员是至关重要的。

从CPU流水线的角度讲，任何的内存访问延迟都可以简化为如下公式：

    Average Access Latency = Hit Time + Miss Rate × Miss Penalty

cache miss会导致CPU的stall状态，从而影响性能。现代CPU的微架构分了frontend和backend。frontend负责fetch指令给backend执行，backend执行依赖运算能力和Memory子系统（包括cache）延迟。

<img id="Figure_1-29" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-29.PNG">

backend执行中访问数据导致的cache miss会导致backend stall，从而降低IPC（instructions per cycle）。减小cache的miss，实际上是一个软硬件协同设计的任务。比如硬件方面，它支持预取prefetch，通过分析cache miss的pattern，硬件可以提前预取数据，在流水线需要某个数据前，提前先取到cache，从而CPU流水线跑到需要它的时候，不再miss。当然，硬件不一定有那么聪明，也许它可以学会一些简单的pattern。但是，对于复杂的无规律的数据，则可能需要软件通过预取指令，来暗示CPU进行预取。

## 5 cache预取

比如在ARM处理器上就有一条指令叫pld，prefetch可以用pld指令：

```c

static inline void prefetch(const void *ptr)
{
        __asm__ __volatile__(
                "pld\t%a0"
                :: "p" (ptr));
}
```
眼见为实，我们随便从Linux内核里面找一个commit：

<img id="Figure_1-30" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-30.PNG">

因为我们从WiFi收到了一个skb，我们很快就要访问这个skb里面的数据来进行packet的分类以及交给IP stack处理了，不如我们先prefetch一下，这样后面等需要访问这个skb->data的时候，流水线可以直接命中cache，从而不打断。

预取的原理有点类似今天星期五，咱们在上海office，下周一需要北京分公司的人来上海office开会。于是，我们通知北京office的人周末坐飞机过来，这样周一开会的时候就不必等他们了。不预取的情况下，会议开始后，再等北京的人飞过来，会导致stall状态。

任何东西最终还是要落实到代码，`talk is cheap，show me the code。`下面这个是经典的二分查找法代码，这个代码是网上抄的。

```c
#include <stdlib.h>
#include <time.h>

int binarySearch(int *array, int nbr_of_elements, int key)
{
    int low = 0, high = nbr_of_elements - 1, mid;

    while (low <= high)
    {
        mid = (low + high) / 2;
#ifdef DO_PREFETCH
        __builtin_prefetch(&array[(mid + 1 + high)/2], 0, 1);
        __builtin_prefetch(&array[(low + mid - 1 )/2], 0, 1);
#endif

        if (array[mid] < key)
        {
            low = mid + 1;
        }
        else if (array[mid] == key)
        {
            return mid;
        }
        else if (array[mid] > key)
        {
            high = mid - 1;
        }
    }

    return -1;
}

int main()
{
    int SIZE = 1024 * 1024 * 128;
    int *array = malloc(SIZE * sizeof(int));

    for (int i = 0; i < SIZE; ++i)
    {
        array[i] = i;
    }

    int NUM_LOOKUPS = 1024 * 1024 * 8;
    srand(time(NULL));
    int *lookups = malloc(NUM_LOOKUPS * sizeof(int));
    for (int i = 0; i < NUM_LOOKUPS; ++i)
    {
        lookups[i] = rand() % SIZE;
    }

    for (int i = 0; i < NUM_LOOKUPS; ++i)
    {
        int result = binarySearch(array, SIZE, lookups[i]);
    }

    return 0;
}
```

特别留意ifdef DO_PREFETCH包着的代码，它提前预取了下次的中间值。我们来对比下，不预取和预取情况下，这个同样的代码执行时间的差异。先把cpufreq的影响尽可能关闭掉，设置为performance:

```
ubuntu@ubuntu-VirtualBox:~$ sudo cpupower frequency-set \
--governor performance
Setting cpu: 0
Setting cpu: 1
Setting cpu: 2
Setting cpu: 3
Setting cpu: 4
Setting cpu: 5
Setting cpu: 6
Setting cpu: 7
```

然后，对比差异：

```
ubuntu@ubuntu-VirtualBox:~/test$ gcc -O3 -std=c11 -DDO_PREFETCH test3.c -o test3ubuntu@ubuntu-VirtualBox:~/test$ gcc -O3 -std=c11 test3.c -o test4
ubuntu@ubuntu-VirtualBox:~/test$ time ./test3

real    0m8.949s
user    0m8.760s
sys 0m0.120s
ubuntu@ubuntu-VirtualBox:~/test$ time ./test4

real    0m8.976s
user    0m8.804s
sys 0m0.140s
ubuntu@ubuntu-VirtualBox:~/test$ time ./test3

real    0m8.808s
user    0m8.656s
sys 0m0.136s
ubuntu@ubuntu-VirtualBox:~/test$ time ./test4

real    0m8.825s
user    0m8.696s
sys 0m0.132s
ubuntu@ubuntu-VirtualBox:~/test$ 
```

开启`prefetch`的执行时间优于未开启预取的情况。

现在我们来通过基于perf的[pmu-tools](https://github.com/andikleen/pmu-tools)，对上面的程序进行topdown分析，分析的时候，为了尽可能减小其他因子的影响，我们把程序通过taskset运行到CPU0。

先看不prefetch的情况，很明显，程序是backend_bound的，其中DRAM_Bound占比大，达到75.8%。

<img id="Figure_1-31" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-31.PNG">

开启prefetch的情况呢？程序依然是backend_bound的，其中，backend bound的主体依然是DRAM_Bound，但是比例缩小到了60.7%。

<img id="Figure_1-32" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-32.PNG">

DRAM_Bound主要对应cycle_activity.stalls_l3_miss事件，我们通过perf stat来分别进行搜集：

<img id="Figure_1-33" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-33.PNG">

我们看到，执行prefetch情况下，指令的条数明显多了，但是它的insn per cycle变大了，所以总的时间cycles反而减小。其中最主要的原因是cycle_activity.stalls_l3_miss变小了很多次。

这个时候，我们可以进一步通过录制mem_load_retired.l3_miss来分析究竟代码哪里出了问题，先看noprefetch情况：

<img id="Figure_1-34" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-34.PNG">

焦点在main函数：

<img id="Figure_1-35" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-35.PNG">

继续annotate一下：

<img id="Figure_1-36" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-36.PNG">

明显问题出在`array[mid] < key`这句话这里。做prefetch的情况下呢？

<img id="Figure_1-37" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-37.PNG">

main的占比明显变小了（99.93% -> 80.00%）：

<img id="Figure_1-38" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-38.PNG">

继续annotate一下：

<img id="Figure_1-39" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-39.PNG">

热点被分散了，预取缓解了Memory_Bound的情况。

## 5 避免false sharing

前面我们提到过，数据如果在一个cacheline，被多核访问的时候，多核间运行的cache一致性协议，会导致cacheline在多核间的同步。这个同步会有很大的延迟，是工程里著名的false sharing问题。

比如下面一个结构体

```c
struct s
{
    int a;
    int b;
}
```

如果1个线程读写a，另外一个线程读写b，那么两个线程就有机会在不同的核，于是产生cacheline同步行为的来回颠簸。但是，如果我们把a和b之间padding一些区域，就可以把这两个缠绕在一起的人拉开：

```c
struct s
{
    int a;
    char padding[cacheline_size - sizeof(int)];
    int b;
}
```

因此，在实际的工程中，我们经常看到有人对数据的位置进行移位，或者在2个可能引起false sharing的数据间填充数据进行padding。这样的代码在内核不甚枚举，我们随便找一个：

<img id="Figure_1-40" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-40.PNG">

它特别提到在tw_count后面60个字节（L1_CACHE_BYTES - sizeof(atomic_t)）的padding，从而避免false sharing：

<img id="Figure_1-41" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-41.PNG">

下面这个则是通过移动结构体内部成员的位置，相关数据的cacheline分开的：

<img id="Figure_1-42" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-42.PNG">

这个改动有明显的性能提升，最高可达9.9%。代码里面也有明显地注释，usage和parent原先靠地太近，一个频繁写，一个频繁读。移开了2边互相不打架了：

<img id="Figure_1-43" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/qemu/Linux_kernel_analysis/images/extend-cache-understanding-1-43.PNG">

把理论和代码能对上的感觉真TNND爽。无论是996,还是007,都必须留些时间来思考，来让理论和实践结合，否则，就变成漫无目的的内卷，这样一定会卷输的。内卷并不可悲，可悲的是卷不赢别人。
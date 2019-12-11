#

首先判断是否需要建立静态映射表。

    #ifdef CONFIG_MMU
    extern void iotable_init(struct map_desc *, int);
    #else
    #define iotable_init(map,num)   do { } while (0)
    #endif

# ARM linux内存分布

下面是linux kernel为ARM处理器建立的虚拟内存分布。它指定了那些内存空间是基于架构的平台能使用的，哪些是通用代码使用的内存空间。

ARM CPU可以寻址最大4GB的虚拟内存，被用户空间、内核、硬件设备共享。

为了以后添加新设备，保留了部分虚拟内存。


| Start    |   End    | Use |
|----------| ---------|-----------------------|
| ffff8000 | ffffffff | copy_user_page/clear_user_page use.对于SA11xx和Xscale，这被用来建立一个minicache映射 |
| ffff4000 | ffffffff | ARMv6架构及以后的CPU，用来缓存别名 |
| ffff1000 | ffff7fff | 保留，所有的平台都不能用 |
| ffff0000 | ffff0fff | CPU vector页表。CPU中断向量表被映射到这里（前提是CPU支持向量表重映射，V位控制）|
| fffe0000 | fffeffff | XScale cache刷新区域，在文件proc-xscale.S中，用来刷新整个D-Cache(XScale没有TCM) |
| fffe8000 | fffeffff | DTCM映射区域，与平台有关 |
| fffe0000 | fffe7fff | ITCM映射区域，与平台有关 |
| ffc00000 | ffefffff | Fixmap mapping region.  Addresses provided by fix_to_virt() will be located here.|
| fee00000 | feffffff | Mapping of PCI I/O space. This is a static mapping within the vmalloc space. |
| VMALLOC_START| VMALLOC_END-1 |   vmalloc()/ioremap()空间。vmalloc/ioremap返回的地址空间，iotable_init建立的静态映射表. VMALLOC_START依赖于high_memory的值，VMALLOC_END=0xff800000 |
| PAGE_OFFSET| high_memory-1 | 内核直接重映射的内存区域，跟平台RAM大小有关，通常是1:1的关系 |
| PKMAP_BASE | PAGE_OFFSET-1 | 永久的内核映射。一种将HIGHMEM内存页映射到内核空间的方法 |
| MODULES_VADDR | MODULES_END-1 | 内核模块空间。通过insmod方法插入的内核模块放到该区域，使用动态映射的方法 |
| 00001000 | TASK_SIZE-1 | 用户空间映射。通过mmap()系统调用建立的每个线程有关的映射 |
| 00000000 | 00000fff | CPU vector页表/空指针陷阱。如果CPU不支持中断向量表重映射，就放到这儿。内核和用户空间访问NULL指针，被捕获 |

在使用虚拟内存的过程中，请不要与上表冲突，以免造成不可引导的内核。

由于将来的cpu可能会影响内核映射布局，所以用户程序不能访问任何没有在其0x0001000内映射到TASK_SIZE地址范围的内存。如果他们希望访问这些区域，他们必须使用open()和mmap()设置自己的映射。

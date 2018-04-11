一、简单说明
----------
　　/etc/init.d 是 /etc/rc.d/init.d 的软链接(soft link)。可以通过 ll 命令查看。
```
ls -ld /etc/init.d
lrwxrwxrwx. 1 root root 11 Aug 30 2015 /etc/init.d -> rc.d/init.d
```
都是用来放服务脚本的，当Linux启动时，会寻找这些目录中的服务脚本，并根据脚本的run level 确定不同的启动级别。
　　在制作服务脚本的过程中，使用了Linux的两个版本，CentOS和Ubuntu，需要在两个版本中都可以开机启动服务。但Ubuntu没有 */etc/rc.d/init.d* 这个目录。所以，为了保持同一种服务在CentOS和Ubuntu使用的统一性，将服务脚本（*注：服务脚本在两个不同版本中是不同的*）都放在 /etc/init.d 目录下，最终达到的效果是相同的。
　　需要说明的是：在CentOS和Ubuntu两个版本中，除了服务脚本放置的目录是相同的，服务脚本的编写及服务配置都是不同的。
比如CentOS使用Chkconfig进行配置，而Ubuntu使用sysv-rc-conf进行配置。

二、系统启动过程
------------
　　1）BIOS自检 ，BIOS的功能由两部分组成，分别是POST码和Runtime服务。POST阶段完成后它将从存储器中被清除，而Runtime服务会被一直保留，用于目标操作系统的启动。BIOS两个阶段所做的详细工作如下：

　　- 步骤1：上电自检POST(Power-on self test)，主要负责检测系统外围关键设备（如：CPU、内存、显卡、I/O、键盘鼠标等）是否正常。例如，最常见的是内存松动的情况，BIOS自检阶段会报错，系统就无法启动起来；

　　- 步骤2：步骤1成功后，便会执行一段小程序用来枚举本地设备并对其初始化。这一步主要是根据我们在BIOS中设置的系统启动顺序来搜索用于启动系统的驱动器，如硬盘、光盘、U盘、软盘和网络等。我们以硬盘启动为例，BIOS此时去读取硬盘驱动器的第一个扇区(MBR，512字节)，然后执行里面的代码。实际上这里BIOS并不关心启动设备第一个扇区中是什么内容，它只是负责读取该扇区内容、并执行。

　　至此，BIOS的任务就完成了，此后将系统启动的控制权移交到MBR部分的代码。

　　2）系统引导，通常情况下，诸如lilo、grub这些常见的引导程序都直接安装在MBR中。详细过程，请自行google

　　3）启动内核，它首先会去解析grub的配置文件/boot/grub/grub.conf，然后加载内核镜像到内存中，并将控制权转交给内核。而内核会立即初始化系统中各设备并做相关的配置工作，其中包括CPU、I/O、存储设备等。

　　关于Linux的设备驱动程序的加载，有一部分驱动程序直接被编译进内核镜像中，另一部分驱动程序则是以模块的形式放在initrd(ramdisk)中。

　　 Linux内核需要适应多种不同的硬件架构，但是将所有的硬件驱动编入内核又是不实际的，而且内核也不可能每新出一种硬件结构，就将该硬件的设备驱动写入内核。实际上Linux的内核镜像仅是包含了基本的硬件驱动，在系统安装过程中会检测系统硬件信息，根据安装信息和系统硬件信息将一部分设备驱动写入 initrd 。这样在以后启动系统时，一部分设备驱动就放在initrd中来加载。这里有必要给大家再多介绍一下initrd这个东东：

　　 initrd 的英文含义是 bootloader initialized RAM disk，就是由 boot loader 初始化的内存盘。在 linu2.6内核启动前，boot loader 会将存储介质中的 initrd 文件加载到内存，内核启动时会在访问真正的根文件系统前先访问该内存中的 initrd 文件系统。在 boot loader 配置了 initrd 的情况下，内核启动被分成了两个阶段，第一阶段先执行 initrd 文件系统中的init，完成加载驱动模块等任务，第二阶段才会执行真正的根文件系统中的 /sbin/init 进程。

　　通过以上分析，grub的stage2将initrd加载到内存里，让后将其中的内容释放到内容中，内核便去执行initrd中的init脚本，这时内核将控制权交给了init文件处理。我们简单浏览一下init脚本的内容，发现它也主要是加载各种存储介质相关的设备驱动程序。当所需的驱动程序加载完后，会创建一个根设备，然后将根文件系统rootfs以只读的方式挂载。这一步结束后，释放未使用的内存，转换到真正的根文件系统上面去，同时运行/sbin/init程序，执行系统的1号进程。此后系统的控制权就全权交给/sbin/init进程了。

　　初始化系统，接下来就是初始化系统的工作了，/sbin/init进程是系统其他所有进程的父进程，当它接管了系统的控制权先之后，它首先会去读取/etc/inittab文件来执行相应的脚本进行系统初始化，如设置键盘、字体，装载模块，设置网络等。主要包括以下工作：

　　（1）执行系统初始化脚本(/etc/rc.d/rc.sysinit)，对系统进行基本的配置，以读写方式挂载根文件系统及其它文件系统，到此系统算是基本运行起来了，后面需要进行运行级别的确定及相应服务的启动。

　　（2）执行/etc/rc.d/rc脚本。该文件定义了服务启动的顺序是先K后S，而具体的每个运行级别的服务状态是放在/etc/rc.d/rc*.d（*=0~6）目录下，所有的文件均是指向/etc/init.d下相应文件的符号链接。rc.sysinit通过分析/etc/inittab文件来确定系统的启动级别，然后才去执行/etc/rc.d/rc*.d下的文件。
```
/etc/init.d-> /etc/rc.d/init.d
/etc/rc ->/etc/rc.d/rc
/etc/rc*.d ->/etc/rc.d/rc*.d
/etc/rc.local-> /etc/rc.d/rc.local
/etc/rc.sysinit-> /etc/rc.d/rc.sysinit
```
　　我们以启动级别3为例来简要说明一下，/etc/rc.d/rc3.d目录，该目录下的内容全部都是以 S 或 K 开头的链接文件，都链接到"/etc/rc.d/init.d"目录下的各种shell脚本。S表示的是启动时需要start的服务内容，K表示关机时需要关闭的服务内容。/etc/rc.d/rc*.d中的系统服务会在系统后台启动，如果要对某个运行级别中的服务进行更具体的定制，通过chkconfig命令来操作，或者通过setup、ntsys、system-config-services来进行定制。如果我们需要自己增加启动的内容，可以在init.d目录中增加相关的shell脚本，然后在rc*.d目录中建立链接文件指向该shell脚本。这些shell脚本的启动或结束顺序是由S或K字母后面的数字决定，数字越小的脚本越先执行。例如，/etc/rc.d/rc3.d /S01sysstat就比/etc/rc.d/rc3.d /S99local先执行。

　　（3）执行用户自定义引导程序/etc/rc.d/rc.local。其实当执行/etc/rc.d/rc3.d/S99local时，它就是在执行/etc/rc.d/rc.local。S99local是指向rc.local的符号链接。就是一般来说，自定义的程序不需要执行上面所说的繁琐的建立shell增加链接文件的步骤，只需要将命令放在rc.local里面就可以了，这个shell脚本就是保留给用户自定义启动内容的。

　　（4）完成了系统所有的启动任务后，linux会启动终端或X-Window来等待用户登录。tty1,tty2,tty3...这表示在运行等级1，2，3，4的时候，都会执行"/sbin/mingetty"，而且执行了6个，所以linux会有6个纯文本终端，mingetty就是启动终端的命令。除了这6个之外还会执行"/etc/X11/prefdm-nodaemon"这个主要启动X-Window

至此，系统就启动完毕了。

接下来就是执行/bin/login程序，进入登录状态

 

三、init.d目录包含许多系统各种服务的启动和停止脚本。

　　/etc/init.d里的shell脚本能够响应start，stop，restart，reload命令来管理某个具体的应用。比如经常看到的命令： /etc/init.d/networking start 这些脚本也可被其他trigger直接激活执行，这些trigger被软连接在/etc/rcN.d/中。这些原理似乎可以用来写daemon程序，让某些程序在开关机时运行。
# 0.简介

QEMU是一个通用的、开源的机器或应用模拟器和虚拟器。

QEMU is capable of emulating a complete machine in software without any
need for hardware virtualization support. By using dynamic translation,
it achieves very good performance. QEMU can also integrate with the Xen
and KVM hypervisors to provide emulated hardware while allowing the
hypervisor to manage the CPU. With hypervisor support, QEMU can achieve
near native performance for CPUs. When QEMU emulates CPUs directly it is
capable of running operating systems made for one machine (e.g. an ARMv7
board) on a different machine (e.g. an x86_64 PC board).

QEMU is also capable of providing userspace API virtualization for Linux
and BSD kernel interfaces. This allows binaries compiled against one
architecture ABI (e.g. the Linux PPC64 ABI) to be run on a host using a
different architecture ABI (e.g. the Linux x86_64 ABI). This does not
involve any hardware emulation, simply CPU and syscall emulation.

QEMU aims to fit into a variety of use cases. It can be invoked directly
by users wishing to have full control over its behaviour and settings.
It also aims to facilitate integration into higher level management
layers, by providing a stable command line interface and monitor API.
It is commonly invoked indirectly via the libvirt library when using
open source applications such as oVirt, OpenStack and virt-manager.

QEMU as a whole is released under the GNU General Public License,
version 2. For full licensing details, consult the LICENSE file.


# 1.构建

QEMU是一种多平台软件，旨在可在所有现代Linux平台，OS-X，Win32（通过 *Mingw64* 工具链）和各种其它UNIX目标上构建。 构建QEMU的简单步骤是：

    mkdir build
    cd build
    ../configure
    make

详细的编译过程可以参考我的另一篇文章： [<Ubuntu 16.04 编译 QEMU\>](https://blog.csdn.net/shenwanjiang111/article/details/82898119)。

还可以在 QEMU 官方网站上查看线上帮助信息：

    https://qemu.org/Hosts/Linux
    https://qemu.org/Hosts/Mac
    https://qemu.org/Hosts/W32


# 2.提交补丁

QEMU源代码使用git进行管理和维护：

    git clone https://git.qemu.org/git/qemu.git

当提交补丁时，一个常用的方法就是使用 *'git format-patch'* 以及 *'git send-email'* 格式化邮件并发送到 qemu-devel@nongnu.org 邮件列表。 All patches submitted must contain a 'Signed-off-by' line from the author. Patches should follow the guidelines set out in the HACKING and CODING_STYLE files.

有关提交补丁的更多信息，请访问QEMU网站：

    https://qemu.org/Contribute/SubmitAPatch
    https://qemu.org/Contribute/TrivialPatches

The QEMU website is also maintained under source control.

    git clone https://git.qemu.org/git/qemu-web.git
    https://www.qemu.org/2017/02/04/the-new-qemu-website-is-up/

为了使上述的过程不那么繁琐，有人创建了使用小程序 *git-publish*，强烈建议使用它用来执行定期更新，甚至只是发送连续的patch系列的修订版。 它也需要一个有效的 "git send-email"设置，默认情况下，不会自行执行所有操作，因此，上面的步骤需要手动完成一次。

对于安装说明，请前往：

    https://github.com/stefanha/git-publish

'git-publish' 的工作流程是：

    $ git checkout master -b my-feature
    $ # 处理新提交，为每个提交添加“Signed-off-by”行。
    $ git publish

Your patch series will be sent and tagged as my-feature-v1 if you need to refer
back to it in the future.

Sending v2:

    $ git checkout my-feature # same topic branch
    $ # making changes to the commits (using 'git rebase', for example)
    $ git publish

Your patch series will be sent with 'v2' tag in the subject and the git tip
will be tagged as my-feature-v2.

Bug reporting
=============

The QEMU project uses Launchpad as its primary upstream bug tracker. Bugs
found when running code built from QEMU git or upstream released sources
should be reported via:

    https://bugs.launchpad.net/qemu/

If using QEMU via an operating system vendor pre-built binary package, it
is preferable to report bugs to the vendor's own bug tracker first. If
the bug is also known to affect latest upstream code, it can also be
reported via launchpad.

For additional information on bug reporting consult:

    https://qemu.org/Contribute/ReportABug


Contact
=======

The QEMU community can be contacted in a number of ways, with the two
main methods being email and IRC

 - qemu-devel@nongnu.org
   https://lists.nongnu.org/mailman/listinfo/qemu-devel
 - #qemu on irc.oftc.net

Information on additional methods of contacting the community can be
found online via the QEMU website:

  https://qemu.org/Contribute/StartHere

-- End
[TOC]

## 1 源代码

源代码位置：

* [https://gitlab.com/kvm-unit-tests/kvm-unit-tests.git](https://gitlab.com/kvm-unit-tests/kvm-unit-tests.git)

## 2 kvm单元测试框架介绍

kvm-unit-tests is a project as old as KVM. As its name suggests, it's purpose is to provide unit tests for KVM. The unit tests are tiny guest operating systems that generally execute only tens of lines of C and assembler test code in order to obtain its PASS/FAIL result. Unit tests provide KVM and virtual hardware functional testing by targeting the features through minimal implementations of their use per the hardware specification. The simplicity of unit tests make them easy to verify they are correct, easy to maintain, and easy to use in timing measurements. Unit tests are also often used for quick and dirty bug reproducers. The reproducers may then be kept as regression tests. It's strongly encouraged that patches implementing new KVM features are submitted with accompanying unit tests.

While a single unit test is focused on a single feature, all unit tests share the minimal system initialization and setup code. There are also several functions made shareable across all unit tests, comprising a unit test API. The setup code and API implementation are briefly described in the next section, "Framework". We then describe testdevs in the "Testdevs" section, which are extensions to KVM's userspace that provide special support for unit tests. Section "API" lists the subsystems, e.g. MMU, SMP, that the API covers, along with a few descriptions of what the API supports. It specifically avoids listing any actual function declarations though, as those may change (use the source, Luke!). The "Running tests" section gives all the details necessary to build and run tests, and section "Adding a test" provides an example of adding a test. Finally, section "Contributing" explains where and how to submit patches.

## 3 Framework

The kvm-unit-tests framework supports multiple architectures; currently i386, x86_64, armv7 (arm), armv8 (arm64), ppc64, ppc64le, and s390x. The framework makes sharing code between similar architectures, e.g. arm and arm64, easier by adopting Linux's configured asm symlink. With the asm symlink each architecture gets its own copy of the headers, but then may opt to share the same code.

The framework has the following components:

* Test building support

    Test building is done through makefiles and some supporting bash scripts.

* Shared code for test setup and API

    Test setup code includes, for example, early system init, MMU enablement, and UART init. The API provides some common libc functions, e.g. strcpy, atol, malloc, printf, as well as some low-level helper functions commonly seen in kernel code, e.g. irq_enable/disable, virt_to_phys/phys_to_virt, and some kvm-unit-tests specific API for, e.g., installing exception handlers and reporting test success/failure.

* Test running support

    Test running is provided with a few bash scripts, using a unit tests configuration file as input. Generally tests are run from within the source root directory using the supporting scripts, but tests may optionally be built as standalone tests as well. More information about the standalone building and running is in the section "Running tests".

## 4 Testdevs

Like all guests, a kvm-unit-test unit test (a mini guest) is run not only with KVM, but also with KVM's userspace. It's useful for unit tests to be able to open a test specific communication channel to KVM's userspace, allowing it to send commands for host-controlled behaviors or guest external invoked events. In particular a channel is useful for initiating an exit, i.e. to quit the unit test. Testdevs fill these roles. The following are testdevs currently in QEMU

* isa-debug-exit

    x86 device that opens an I/O port. When the I/O port is written it induces an exit, using the value written to form the exit code. Note, the exit code written is modified with "(code << 1) | 1". Thus, a unit test exiting with zero for success induces an exit from QEMU with 1.

* pc-testdev

    x86 device that opens several I/O ports, where each port provides an interface to a helper function for the unit tests. One such function is interrupt injection.

* pci-testdev

    A PCI "device" that when read and written tests PCI accesses.

* edu

    A PCI device that supports testing both INTx and MSI interrupts and DMA transfers.

* testdev

    An architecture neutral testdev that takes its commands in postfix notation over a serial channel. Unit tests add an additional serial channel to their unit test guest config (the first being for test output), and then bind this device to it. kvm-unit-tests has minimal support for virtio in order to allow the additional serial channel to be an instance of virtio-serial. Currently testdev only supports the command "codeq", which works exactly like the isa-debug-exit testdev.

## 5 API

There are three API categories in kvm-unit-tests 1) libc, 2) functions typical of kernel code, and 3) kvm-unit-tests specific. Very little libc has been implemented, but some of the most commonly used functions, such as strcpy, memset, malloc, printf, assert, exit, and others are available. To give an overview of (2), it's best to break them down by subsytem

* Device discovery

    - ACPI - minimal table search support. Currently x86-only.
    - Device tree - libfdt and a device tree library wrapping libfdt to accommodate the use of device trees conforming to the Linux documentation. For example, there is a function that gets the "bootargs" from /chosen, which are then fed into the unit test's main function's inputs (argc, argv) by the setup code before the unit test starts.

* Vectors

    - Functions to install exception handlers.

* Memory

    - Functions for memory allocation. Free memory is prepared for allocation during system init before the unit test starts.
    - Functions for MMU enable/disable, TLB flushing, PTE setting, etc.

* SMP

    - Functions to boot secondaries, iterate online cpus, etc.
    - Barriers, spinlocks, atomic ops, cpumasks, etc.

* I/O

    - Output messages to the UART. The UART is initialized during system init before the unit test starts.
    - Functions to read/write MMIO
    - Functions to read/write I/O ports (x86-only)
    - Functions for accessing PCI devices

* Power management

    - PSCI (arm/arm64-only)
    - RTAS (PowerPC-only)

* 中断控制器

    - Functions to enable/disable, send IPIs, etc.
    - Functions to enable/disable IRQs

* Virtio

    - Buffer sending support. Currently virtio-mmio only.

* Misc

    - Special register accessors
    - Switch to user mode support
    - Linux's asm-offsets generation, which can be used for structures that need to be accessed from assembly.

Note, many of the names for the functions implementing the above are kvm-unit-tests specific, making them also part of the kvm-unit-tests specific API. However, at least for arm/arm64, any function that implements something for which the Linux kernel already has a function, then we use the same name (and exact same type signature, if possible). The kvm-unit-tests specific API also includes some testing specific functions, such as report() and report_summary(). The report* functions should be used to report PASS/FAIL results of the tests, and the overall test result summary.

## 6 运行测试程序

Here are a few examples of building and running tests

* 在主机上直接运行所有的单元测试用例

    ```shell
    $ git clone https://gitlab.com/kvm-unit-tests/kvm-unit-tests.git
    $ cd kvm-unit-tests/
    $ ./configure
    $ make
    $ ./run_tests.sh
    ```
* 交叉编译并在特定的QEMU虚拟的开发板上运行所有的单元测试用例

    ```shell
    $ ./configure --arch=arm64 --cross-prefix=aarch64-linux-gnu-
    $ make
    $ export QEMU=/path/to/qemu-system-aarch64
    $ ./run_tests.sh
    ```

* 并行编译并运行

    ```shell
    $ ./configure
    $ make -j               # 并行编译
    $ ./run_tests.sh -j 4   # 同时运行4个单元测试用例
    ```

* 通过附加的QEMU命令行选项，运行单个测试

    ```shell
    $ ./arm-run arm/selftest.flat -smp 4 -append smp
    ```

    > Note1: run_tests.sh runs each test in $TEST_DIR/unittests.cfg (TEST_DIR, along with some other variables, is defined in config.mak after running configure. See './configure -h' for a list of supported options.)
    >
    > Note2: When a unit test is run separately, all output goes to stdout. When unit tests are run through run_tests.sh, then each test has its output redirected to a file in the logs directory named from the test name in the unittests.cfg file, e.g. the pci-test for arm/arm64 has its output logged to 'logs/pci-test.log'.

* Building and running standalone tests

    ```
    $ make standalone
    $ tests/hypercall # example run of a standalone test
    ```

Tests may be installed with 'make install', which copies the standalone version of each test to `$PREFIX/share/kvm-unit-tests/`

## 7 Running tests via Avocado

kvm-unit-tests may be run as an [Avocado](http://avocado-framework.readthedocs.io/en/latest/) external testsuite using the [Avocado kvm-unit-tests runner script](https://github.com/avocado-framework/avocado/blob/master/contrib/testsuites/run-kvm-unit-test.sh). Check the available options with `sh run-kvm-unit-test.sh -h`. By default it downloads the latest kvm-unit-tests and runs all available tests.

```
$ sh contrib/testsuites/run-kvm-unit-test.sh 
JOB ID     : 216c5cf937b07befd9d2bc1dd496714fce280f22
JOB LOG    : /home/medic/avocado/job-results/job-2017-02-23T16.49-216c5cf/job.log
 (01/42) access: PASS (4.46 s)
 (02/42) apic: FAIL (4.42 s)
 (03/42) apic-split: FAIL (4.41 s)
...
 (41/42) vmx: FAIL (1.64 s)
 (42/42) xsave: PASS (1.28 s)
RESULTS    : PASS 33 | ERROR 0 | FAIL 9 | SKIP 0 | WARN 0 | INTERRUPT 0
TESTS TIME : 114.34 s
JOB HTML   : /home/medic/avocado/job-results/job-2017-02-23T16.49-216c5cf/html/results.html
```

## 8 添加测试

1. 创建新的单元测试用例的main()代码:

```c
$ cat > x86/new-unit-test.c
#include <libcflat.h>

int main(int ac, char **av)
{
        report(true, "hello!");
        return report_summary();
}
```

2. 修改`makefile`，比如 `x86/Makefile.common`，将添加的单元测试添加到编译系统中：

    ```
    tests-common += $(TEST_DIR)/new-unit-test.flat
    ```

    > Note, the tests-common variable identifies tests shared between similar architectures, e.g. i386 and x86_64 or arm and arm64. Use the tests makefile variable of a specific architecture's makefile to build the test specifically for that architecture.

3. 编译并运行测试：

    ```
    $ ./configure
    $ make
    $ x86/run x86/new-unit-test.flat
    ```

## 9 Contributing

To contribute new tests or enhancements and fixes to the framework, please submit patches to the KVM mailing list with the additional tag 'kvm-unit-tests' in the subject, i.e. [kvm-unit-tests PATCH]
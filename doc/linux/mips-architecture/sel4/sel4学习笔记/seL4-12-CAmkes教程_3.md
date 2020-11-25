[TOC]

本文主要内容：引导一个基本的静态CAmkES应用，描述其组件并将它们链接到一起。

# 1 要求

1. 建立主机开发环境，参考[seL4-1-搭建环境](TODO)

2. 完成[`Camkes 1`示例](TODO)

# 2 初始化教程

    # For instructions about obtaining the tutorial sources see https://docs.sel4.systems/Tutorials/#get-the-code
    #
    # Follow these instructions to initialise the tutorial
    # initialising the build directory with a tutorial exercise
    ./init --tut hello-camkes-2
    # building the tutorial exercise
    cd hello-camkes-2_build
    ninja

# 3 学习目标

1. 理解如何在CAmkES中表示和实现事件。

2. 了解如何使用Dataport接口。

# 4 走查

请记住，本文将按照在幻灯片演示中引导用户完成这些教程步骤的顺序来介绍这些步骤，除非将几个类似的任务合并在一起以避免重复。另外，如果任务步骤涵盖了已经讨论过的内容，本文将省略它。

## 4.1 TASK 1


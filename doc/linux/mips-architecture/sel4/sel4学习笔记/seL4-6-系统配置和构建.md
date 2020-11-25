[TOC]

# 1 System configuration and building

seL4 and related projects use the CMake family of tools to implement its build system. The seL4 build system refers to a collection of CMake scripts that manage:

* system configuration: Configuration options such as platform flags and application settings.
* dependency structures: Tracking build order dependencies
* builds: Generating binary artifacts that can then be deployed on hardware.

The following documentation covers how to use the seL4 build system to:

* perform system configuration and builds
* incorporating the build system into a project
* Kernel stand-alone configuration and builds

# 2 CMake基本知识

For a complete guide to CMake you can read the [extensive documentation](https://cmake.org/cmake/help/latest/), but for the purposes here we will assume a particular workflow with CMake involving out of tree builds.

CMake is not itself a build tool, but rather is a build generator. This means that it generates build scripts, typically Makefiles or Ninja scripts, which will be used either by a tool like GNU Make or Ninja to perform the actual build.

# 3 前提条件

本文假设：

1. CMake of an appropriate version is installed
2. You are using the Ninja CMake generator
3. You understand how to checkout projects using the repo tool as described on the Getting started page
4. You have the required dependencies installed to build your project.

# 4 [配置和构建seL4工程](https://docs.sel4.systems/Developing/Building/Using)

This page contains documentation for how to interact with and build a project that is using this build system. For new project development, see [incorporating the build system](https://docs.sel4.systems/Developing/Building/Incorporating).

# 5 [将构建系统合并到工程中](https://docs.sel4.systems/Developing/Building/Incorporating)

This page describes the CMake-based build system in detail, with sufficient information for integrating it into new projects. There are a few different pieces that can be fit together in different ways depending on your project’s needs and desired customisation. This is reflected in the split of CMake files spread across several repositories.

# 6 [单独编译内核](https://docs.sel4.systems/Developing/Building/seL4Standalone)

This page contains documentation for building the seL4 kernel standalone — without a userlevel or any loaders that run before the kernel. This is typically used for verification or where a project wants to fully control userlevel and prekernel initialisation. We only maintain verification configurations for kernel only builds. This page assumes familiarity with using the seL4 project build system and builds upon the previous documentation.

# 7 Gotchas

List of gotchas and easy mistakes that can be made when using cmake

* Configuration files passed to to cmake with -C must end in .cmake, otherwise CMake will silently throw away your file

# 8 旧编译系统

We used to use a Kbuild based build system. Old documentation for it can be found [here](https://docs.sel4.systems/Developing/Building/OldBuildSystem/).

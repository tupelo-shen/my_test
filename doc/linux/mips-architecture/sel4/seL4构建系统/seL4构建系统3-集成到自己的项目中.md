This page describes the CMake-based build system in detail, with sufficient information for integrating it into new projects. There are a few different pieces that can be fit together in different ways depending on your project’s needs and desired customisation. This is reflected in the split of CMake files spread across several repositories.

## 1 基本的结构

The build system is in two pieces. One piece is in the [seL4 kernel repository](https://github.com/seL4/seL4), which contains the compiler toolchain and flags settings as well as helpers for generating configurations. The other piece is in [seL4_tools/cmake-tool](https://github.com/seL4/seL4_tools/tree/master/cmake-tool), which contains helpers for combining libraries and binaries into a final system image (along with the kernel).

This structure means that the kernel is completely responsible for building itself, but exports the settings and binaries for use by the rest of the build system.

The `cmake-tool` directory has the following files of interest:

* `default-CMakeLists.txt`: An example `CMakeLists.txt` file, which could be used as the `CMakeLists.txt` file in the top-level directory of a project. Simply, this file includes `all.cmake`, under the assumption of a directory structure where the `cmake-tool` repository is in a directory named `tools`. To use this file, [project manifests](https://docs.sel4.systems/RepoCheatsheet) are expected to create a symbolic link to this file, named `CMakeLists.txt`, at the top-level project directory.

* `all.cmake`: A wrapper that includes `base.cmake`, `projects.cmake` and `configuration.cmake`. This file can be used by projects which do not alter these three files.

* `base.cmake`: Constructs the basic build targets (`kernel`, `libsel4`, and `elfloader-tool`), in addition to basic compilation flags and helper routines, required to build an seL4 project, and can be used as a base for further targets in a project through the `add_subdirectory` routine or otherwise.

* `projects.cmake`: Adds build targets through `add_subdirectory` assuming a default project layout. Essentially it adds any `CMakeLists.txt` files it finds in any subdirectories of the projects directory.

* `configuration.cmake`: Provides a target for a library called Configuration that emulates the legacy autoconf.h header. Since the autoconf.h header contained configuration variables for the entire project this rule needs to come after all other targets and scripts which add to the configuration space.

* `common.cmake`: File included by `base.cmake` with some generic helper routines.

* `flags.cmake`: File included by `base.cmake` which sets up build flags and linker invocations.

* `init-build.sh`: A shell script which performs the initial configuration and generation for a new CMake build directory.

* `helpers/*`: Helper functions that are commonly imported by `common.cmake`

## 2 内核目录

The file `base.cmake` assumes that the seL4 kernel is in a specific location. Consider the following example:

```
awesome_system/
├── kernel/
│   └── CMakeLists.txt
├── projects/
│   ├── awesome_system/
│   │   └── CMakeLists.txt
│   └── seL4_libs/
│       └── CMakeLists.txt
├── tools/
│   └── cmake-tool/
│       ├── base.cmake
│       ├── all.cmake
│       └── default-CMakeLists.txt
├── .repo/
└── CMakeLists.txt -> tools/cmake-tool/default-CMakeLists.txt
```

When `awesome_system/` is used as the root source directory to initialise a CMake build directory and `tools/cmake-tool/all.cmake` is used, `base.cmake` expects the kernel to be at `awesome_system/kernel`.

The kernel can be placed in a different location, as described below.

```
awesome_system/
├── seL4/
│   └── CMakeLists.txt
├── projects/
│   ├── awesome_system/
│   │   └── CMakeLists.txt
│   └── seL4_libs/
│       └── CMakeLists.txt
├── tools/
│   └── cmake-tool/
│       ├── base.cmake
│       ├── all.cmake
│       └── default-CMakeLists.txt
├── .repo/
└── CMakeLists.txt -> tools/cmake-tool/default-CMakeLists.txt
```

For the example above, where the kernel is in a directory called `seL4`, the default kernel location can be overriden when invoking `cmake` with `-DKERNEL_PATH=seL4`.

## 3 Advanced structures

Other project layouts can be used. Consider the following example:

```
awesome_system/
├── seL4/
│   └── CMakeLists.txt
├── awesome/
│   └── CMakeLists.txt
├── seL4_libs/
│   └── CMakeLists.txt
├── buildsystem/
│   └── cmake-tool/
│       ├── base.cmake
│       ├── all.cmake
│       └── default-CMakeLists.txt
└── .repo/
```

The example above departs from the default in the following ways:

* no `CMakeLists.txt` file in the root directory,
* `tools` directory has been renamed to `buildsystem`,
* `kernel` directory has been renamed to `seL4`,
* and the `projects` directory is omitted.

We now describe how to achieve such a project structure:

To place the `CMakeLists.txt` in `awesome_system/awesome` directory then initialise CMake, assuming a build directory that is also in the `awesome_system` directory, do something like:

```
sel4@host:~/awesome_directory$ cmake -DCROSS_COMPILER_PREFIX=toolchain-prefix -DCMAKE_TOOLCHAIN_FILE=../seL4/gcc.cmake -DKERNEL_PATH=../seL4 -G Ninja ../awesome
```

Importantly, the path for `CMAKE_TOOLCHAIN_FILE` is resolved immediately by CMake, and so is relative to the build directory, whereas the `KERNEL_PATH` is resolved whilst processing `awesome_system/awesome/CMakeLists.txt` and is relative to that directory.

The contents of `awesome_system/awesome/CMakeLists.txt` would be something like:

```
cmake_minimum_required(VERSION 3.7.2)
include(../buildsystem/cmake-tool/base.cmake)
add_subdirectory(../seL4_libs seL4_libs)
include(../buildsystem/cmake-tool/configuration.cmake)
```

This looks pretty much like all.cmake except that we do not include projects.cmake as we do not have a projects folder. projects.cmake would be redundant to include, as it would not resolve any files. all.cmake cannot be included as we need to include specific subdirectories (in the example seL4_libs) between setting up the base flags and environment and finalising the Configuration library. We needed to give an explicit build directory (the second argument in add_subdirectory) as we are giving a directory that is not a subdirectory of the root source directory.

For simplicity, the kernel path can be encoded directly into the projects top-level CMakeLists.txt. To achieve this the following line:

```
set(KERNEL_PATH ../seL4)
```

should be included before

```
include(../buildsystem/cmake-tool/base.cmake)
```

in `awesome_system/awesome/CMakeLists.txt`, thus removing the need for `-DKERNEL_PATH` in the `cmake` invocation.

## 4 配置

For compatibility with the legacy build system, various helpers and systems exist in order to achieve the following:

* Automate configuration variables that appear in the `cmake-gui` with various kinds of dependencies.

* Generate C-style configuration headers that declare these variables in format similar to the legacy build system.

* Generate `autoconf.h` headers so legacy code using `#include <autoconf.h>` works.

The following fragment of CMake script demonstrates how these three things fit together:

```makefile
# 设置configure_string为空，因为各种`config_*`会自动追加这个变量
set(configure_string "")

# 声明一个配置变量，在CMake脚本和cmake-gui中为 EnableAwesome;
# 在C头文件中为 CONFIG_HAVE_AWESOME；
config_option(EnableAwesome HAVE_AWESOME "Makes library awesome" DEFAULT ON)

# 
add_config_library(MyLibrary "${configure_string}")
generate_autoconf(MyLibraryAutoconf "MyLibrary")
target_link_libraries(MyLibrary PUBLIC MyLibrary_Config)
target_link_libraries(LegacyApplication PRIVATE MyLibrary MyLibraryAutoconf)
```

在上面的示例中：

* 

## 1 引言

A CMake-based buildsystem is organized as a set of high-level logical targets. Each target corresponds to an executable or library, or is a custom target containing custom commands. Dependencies between the targets are expressed in the buildsystem to determine the build order and the rules for regeneration in response to change.

## 2 二进制目标

Executables and libraries are defined using the `add_executable()` and `add_library()` commands. The resulting binary files have appropriate `PREFIX`, `SUFFIX` and extensions for the platform targeted. Dependencies between binary targets are expressed using the `target_link_libraries()` command:

```makefile
add_library(archive archive.cpp zip.cpp lzma.cpp)
add_executable(zipapp zipapp.cpp)
target_link_libraries(zipapp archive)
```

`archive` is defined as a `STATIC` library -- an archive containing objects compiled from `archive.cpp`, `zip.cpp`, and `lzma.cpp`. `zipapp` is defined as an executable formed by compiling and linking `zipapp.cpp`. When linking the `zipapp` executable, the `archive` static library is linked in.

#### 2.1 可执行文件

The `add_executable()` command defines an executable target:

```makefile
add_executable(mytool mytool.cpp)
```

Commands such as `add_custom_command()`, which generates rules to be run at build time can transparently use an EXECUTABLE target as a COMMAND executable. The buildsystem rules will ensure that the executable is built before attempting to run the command.

#### 2.2 库类型

###### 2.2.1 正常类型

By default, the `add_library()` command defines a `STATIC` library, unless a type is specified. A type may be specified when using the command:

```
add_library(archive SHARED archive.cpp zip.cpp lzma.cpp)
add_library(archive STATIC archive.cpp zip.cpp lzma.cpp)
```

###### 2.2.2 对象库

## 3 构建规范和使用要求

## 4 伪目标

#### 4.1 导入目标

#### 4.2 别名目标

#### 4.3 Interface库

An `INTERFACE` library target does not compile sources and does not produce a library artifact on disk, so it has no `LOCATION`.

It may specify usage requirements such as `INTERFACE_INCLUDE_DIRECTORIES`, `INTERFACE_COMPILE_DEFINITIONS`, `INTERFACE_COMPILE_OPTIONS`, `INTERFACE_LINK_LIBRARIES`, `INTERFACE_SOURCES`, and `INTERFACE_POSITION_INDEPENDENT_CODE`. Only the `INTERFACE` modes of the `target_include_directories()`, `target_compile_definitions()`, `target_compile_options()`, `target_sources()`, and `target_link_libraries()` commands may be used with `INTERFACE` libraries.

Since CMake 3.19, an INTERFACE library target may optionally contain source files. An interface library that contains source files will be included as a build target in the generated buildsystem. It does not compile sources, but may contain custom commands to generate other sources. Additionally, IDEs will show the source files as part of the target for interactive reading and editing.

A primary use-case for INTERFACE libraries is header-only libraries.

```makefile
add_library(Eigen INTERFACE
  src/eigen.h
  src/vector.h
  src/matrix.h
  )
target_include_directories(Eigen INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include/Eigen>
)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 Eigen)
```

Here, the usage requirements from the `Eigen` target are consumed and used when compiling, but it has no effect on linking.

Another use-case is to employ an entirely target-focussed design for usage requirements:

```makefile
add_library(pic_on INTERFACE)
set_property(TARGET pic_on PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)
add_library(pic_off INTERFACE)
set_property(TARGET pic_off PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE OFF)

add_library(enable_rtti INTERFACE)
target_compile_options(enable_rtti INTERFACE
  $<$<OR:$<COMPILER_ID:GNU>,$<COMPILER_ID:Clang>>:-rtti>
)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 pic_on enable_rtti)
```

This way, the build specification of exe1 is expressed entirely as linked targets, and the complexity of compiler-specific flags is encapsulated in an INTERFACE library target.

INTERFACE libraries may be installed and exported. Any content they refer to must be installed separately:

```makefile
set(Eigen_headers
  src/eigen.h
  src/vector.h
  src/matrix.h
  )
add_library(Eigen INTERFACE ${Eigen_headers})
target_include_directories(Eigen INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include/Eigen>
)

install(TARGETS Eigen EXPORT eigenExport)
install(EXPORT eigenExport NAMESPACE Upstream::
  DESTINATION lib/cmake/Eigen
)
install(FILES ${Eigen_headers}
  DESTINATION include/Eigen
)
```
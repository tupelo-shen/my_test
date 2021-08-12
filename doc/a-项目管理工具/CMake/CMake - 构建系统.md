[TOC]

## 1 引言

A CMake-based buildsystem is organized as a set of high-level logical targets. Each target corresponds to an executable or library, or is a custom target containing custom commands. Dependencies between the targets are expressed in the buildsystem to determine the build order and the rules for regeneration in response to change.

## 2 二进制目标

使用`add_executable()`和`add_library()`命令定义可执行文件和库文件。产生的二进制文件会带有与目标平台匹配的前缀`PREFIX`、后缀`SUFFIX`和扩展名。二进制目标文件之间的依赖关系使用`target_link_libraries()`命令进行定义。

```makefile
add_library(archive archive.cpp zip.cpp lzma.cpp)
add_executable(zipapp zipapp.cpp)
target_link_libraries(zipapp archive)
```

`archive`被定义为一个`STATIC`静态库，包含的目标由`archive.cpp`、`zip.cpp`和`lzma.cpp`三个源文件定义而来。`zipapp`是一个可执行文件，由`zipapp.cpp`源文件编译而来。在`zipapp`的链接阶段，`archive`库文件会被链接进来。

#### 2.1 可执行文件

`add_executable()`命令定义一个可执行文件：

```makefile
add_executable(mytool mytool.cpp)
```

`add_custom_command()`命令，可以用来在构建时，自定义构建规则：

* 生成文件

    ```makefile
    add_custom_command(
        OUTPUT out.c
        COMMAND someTool -i ${CMAKE_CURRENT_SOURCE_DIR}/in.txt
                     -o out.c
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/in.txt
        VERBATIM)
    add_library(myLib out.c)
    ```

    该段代码的内容就是：添加了用户自定义的命令，运行工具命令`someTool`产生源文件`out.c`。然后，将`out.c`作为库文件的一部分进行编译。产生源文件的规则会随着输入文件`in.txt`的变化进行重新构建。

* 根据事件进行构建

    ```makefile
    add_executable(myExe myExe.c)
    add_custom_command(
      TARGET myExe POST_BUILD
      COMMAND someHasher -i "$<TARGET_FILE:myExe>"
                         -o "$<TARGET_FILE:myExe>.hash"
      VERBATIM)
    ```
    在可执行文件`myExe`完成链接后（`POST_BUILD`事件进行控制），运行`someHasher`命令生成一个`.hash`文件。

#### 2.2 库类型

###### 2.2.1 常用类型

默认情况下，`add_library()`定义的是`STATIC`静态库，除非指定类型：

```makefile
add_library(archive SHARED archive.cpp zip.cpp lzma.cpp)
add_library(archive STATIC archive.cpp zip.cpp lzma.cpp)
```

全局变量`BUILD_SHARED_LIBS`可以改变`add_library()`的默认行为，比如，设置默认为构建共享库。

在整个构建系统中，某个库是`SHARED`还是`STATIC`是无关紧要的，命令、依赖和其它API的工作方式与库类型无关。但是`MODULE`类型的库与此不同，因为它不需要链接；也就是说，`target_link_libraries()`右边一般不会调用它。这种类型的库，一般用作运行时的一种插件技术。如果库不导出为任何非托管符号（例如，Windows资源DLL，`C++/CLI DLL`），则要求库不是共享库，因为CMake希望共享库至少导出一个符号。

```
add_library(archive MODULE 7z.cpp)
```

###### 2.2.2 Apple框架

有时候，`SHARED`共享库可能会使用`FRAMEWORK`目标属性创建用于`macOS`或`iOS`的框架包。如果一个库需要设置`FRAMEWORK`目标属性，也应该设置`FRAMEWORK_VERSION`目标属性。通常设为`A`，这是`macOS`的习惯。`MACOSX_FRAMEWORK_IDENTIFIER`设置`CFBundleIdentifier`关键字，唯一地标识框架包。

```makefile
add_library(MyFramework SHARED MyFramework.cpp)
set_target_properties(MyFramework PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION A     # 版本"A"是macOS的习惯
    MACOSX_FRAMEWORK_IDENTIFIER org.cmake.MyFramework
)
```

###### 2.2.3 OBJECT库类型

`OBJECT`库文件类型定义了一堆目标文件的非归档集合，这些目标文件是通过给定的源文件编译而来。这样的库文件类型，可以作为其它目标文件的输入，语法是`$<TARGET_OBJECTS:name>`。这是一个生成器表达式（generator expression），用来给其它目标文件提供`OBJECT`库的内容。

```makefile
add_library(archive OBJECT archive.cpp zip.cpp lzma.cpp)
add_library(archiveExtras STATIC $<TARGET_OBJECTS:archive> extras.cpp)
add_executable(test_exe $<TARGET_OBJECTS:archive> test.cpp)
```

在其它目标文件的链接（或归档）阶段，使用`OBJECT`库提供的目标文件集合和自身的源文件产生的目标文件共同生成新的目标文件。

或者，对象库可以链接到其它目标文件:

```makefile
add_library(archive OBJECT archive.cpp zip.cpp lzma.cpp)

add_library(archiveExtras STATIC extras.cpp)
target_link_libraries(archiveExtras PUBLIC archive)

add_executable(test_exe test.cpp)
target_link_libraries(test_exe archive)
```

在其它目标文件的链接（或归档）阶段，直接使用`OBJECT`库提供的目标文件进行链接。此外，在编译其它目标文件自身依赖的源文件时，会尊重`OBJECT`库的要求。也就是说，这些使用要求会传递到其它目标对象的依赖中。

使用`add_custom_command(TARGET)`时，不能将对象库文件作为`TARGET`。但是，通过语法`$<TARGET_OBJECTS:objlib>`，可以将目标文件的列表作为`add_custom_command(OUTPUT)`或`file(GENERATE)`的输入。

## 3 构建规范和使用要求

`target_include_directories()`、`target_compile_definitions()`和`target_compile_options()`命令指定二进制目标的构建规范和使用要求。这些命令分别填充`INCLUDE_DIRECTORIES`、`COMPILE_DEFINITIONS`和`COMPILE_OPTIONS`目标属性，还可能会包括`INTERFACE_INCLUDE_DIRECTORIES`、`INTERFACE_COMPILE_DEFINITIONS`和`INTERFACE_COMPILE_OPTIONS`目标属性。

这几种命令，每个都有`PRIVATE`、`PUBLIC`和`INTERFACE`模式。`PRIVATE`只设置`non-INTERFACE_`类型目标属性；`INTERFACE`只设置`INTERFACE_`类型的目标属性；而`PUBLIC`两种目标属性都设置。每个命令都可以多次调用这几个关键字：

```makefile
target_compile_definitions(archive
    PRIVATE BUILDING_WITH_LZMA
    INTERFACE USING_ARCHIVE_LIB
)
```

> 注意，`使用要求`不是为了下游的构建过程中，方便使用`COMPILE_OPTIONS`或`COMPILE_DEFINITIONS`等变量而设计的一种方法。这些属性的内容是必须的，不是推荐项或者为了方便。

> 请参阅`cmake-packages(7)`手册的[Creating Relocatable Packages](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#creating-relocatable-packages)一节，了解当创建要重新发布的包的使用要求时必须采用的额外的注意事项。

#### 3.1 目标属性

在编译二进制目标的源文件时，适当地使用`INCLUDE_DIRECTORIES`、`COMPILE_DEFINITIONS`和`COMPILE_OPTIONS`目标属性的内容。

`INCLUDE_DIRECTORIES`中的条目将以`-I`或`-isystem`前缀，并按照属性值的出现顺序添加到编译行。

`COMPILE_DEFINITIONS`中的条目以`-D`或`/D`作为前缀添加到编译行，不指定的顺序。`DEFINE_SYMBOL`目标属性也被添加为编译定义，作为`SHARED`库和`MODULE`库目标的特殊方便用例。

`COMPILE_OPTIONS`指定编译选项，以属性值的先后作为顺序。几个编译选项有特殊的处理，比如，`POSITION_INDEPENDENT_CODE`。

`INTERFACE_INCLUDE_DIRECTORIES`、`INTERFACE_COMPILE_DEFINITIONS`和`INTERFACE_COMPILE_OPTIONS`目标属性的内容是`使用要求`——它们指定使用者必须使用哪些内容来正确编译和链接它们的目标。对于任何二进制目标，`target_link_libraries()`命令中指定的每个目标上的每个`INTERFACE_`属性的内容将被使用：

```makefile
set(srcs archive.cpp zip.cpp)
if (LZMA_FOUND)
    list(APPEND srcs lzma.cpp)
endif()
add_library(archive SHARED ${srcs})
if (LZMA_FOUND)
    # archive库的源文件使用 -DBUILDING_WITH_LZMA 选项进行编译
    target_compile_definitions(archive PRIVATE BUILDING_WITH_LZMA)
endif()
target_compile_definitions(archive INTERFACE USING_ARCHIVE_LIB)

add_executable(consumer)
# 链接 consumer 到 archive，并且使用archive的使用要求。
# consumer的源文件使用-DUSING_ARCHIVE_LIB选项进行编译。
target_link_libraries(consumer archive)
```

因为经常需要将源目录和相应的构建目录添加到`INCLUDE_DIRECTORIES`中，所以使用`CMAKE_INCLUDE_CURRENT_DIR`变量可以方便地将相应的目录添加到所有目标的`INCLUDE_DIRECTORIES`中。可以使用变量`CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE`，将相应的目录添加到所有目标的`INTERFACE_INCLUDE_DIRECTORIES`中。通过使用`target_link_libraries()`命令，可以方便地使用多个不同目录中的目标。

#### 3.2 使用要求的传递性

目标的使用要求可以传递给依赖项。通俗的讲，就是构建一个目标，对满足它的要求同样适用于依赖于它的目标文件。`target_link_libraries()`有3个关键字控制传递性，分别是`PRIVATE`、`INTERFACE`和`PUBLIC`。

```makefile
add_library(archive archive.cpp)
target_compile_definitions(archive INTERFACE USING_ARCHIVE_LIB)

add_library(serialization serialization.cpp)
target_compile_definitions(serialization INTERFACE USING_SERIALIZATION_LIB)

add_library(archiveExtras extras.cpp)
target_link_libraries(archiveExtras PUBLIC archive)
target_link_libraries(archiveExtras PRIVATE serialization)
# archiveExtras使用-DUSING_ARCHIVE_LIB和-DUSING_SERIALIZATION_LIB进行编译

add_executable(consumer consumer.cpp)
# consumer使用-DUSING_ARCHIVE_LIB进行编译
target_link_libraries(consumer archiveExtras)
```

由于`archive`是`archiveExtras`的`PUBLIC`公共依赖项，那么，它的使用要求也传递给了`consumer`。因为`serialization`是`archiveExtras`的`PRIVATE`私有依赖项，那么，它的使用要求不会传递给`consumer`。

通常情况下，如果依赖项只在库的实现中使用，而不是在头文件中使用，则应该在`target_link_libraries()`使用`PRIVATE`关键字指定它。如果在库的头文件中额外使用了一个依赖项(例如，用于类继承)，那么它应该被指定为`PUBLIC`依赖项。一个不被库的实现使用，而只被它的头文件使用的依赖应该被指定为一个`INTERFACE`接口依赖。这几个关键字可以组合使用：

```makefile
target_link_libraries(archiveExtras
    PUBLIC archive
    PRIVATE serialization
)
```

使用要求可以从依赖项的目标属性对应的`INTERFACE_*`变量中，读取出来，然后附加到操作目标的非`non-INTERFACE_*`变量上。例如，依赖项的`INTERFACE_INCLUDE_DIRECTORIES`可以读取出来然后添加到操作目标的`INCLUDE_DIRECTORIES`变量中。如果依赖于依赖项的顺序，`target_link_libraries()`的依赖项顺序不能产生正确的编译，可以使用恰当的命令，直接设置目标属性更新依赖项的顺序。

假设目标的要链接的库文件按照顺序依次是：`lib1`、`lib2`、`lib3`；但是，包含目录必须使用`lib3`、`lib1`、`lib2`的顺序，可以通过下面的代码实现：

```makefile
target_link_libraries(myExe lib1 lib2 lib3)
target_include_directories(myExe
  PRIVATE $<TARGET_PROPERTY:lib3,INTERFACE_INCLUDE_DIRECTORIES>)
```

为了安装，而使用命令[install(EXPORT)](https://cmake.org/cmake/help/latest/command/install.html#command:install)导出的目标文件，其`使用要求`请参考[Creating Packages](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#creating-packages)。

#### 3.3 INTERFACE属性的兼容性

有些目标属性需要在每个依赖项接口和目标之间兼容。例如，`POSITION_INDEPENDENT_CODE`目标属性可以指定一个布尔值，表示是否应该将目标编译为位置独立的代码，这特定于平台。还可以使用`INTERFACE_POSITION_INDEPENDENT_CODE`属性，指定依赖项的使用要求，然后通过传递性，通知目标文件也编译为位置无关码。

```makefile
add_executable(exe1 exe1.cpp)
set_property(TARGET exe1 PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(lib1 SHARED lib1.cpp)
set_property(TARGET lib1 PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)

add_executable(exe2 exe2.cpp)
target_link_libraries(exe2 lib1)
```

这儿，`exe1`和`exe2`都被编译为`位置无关码(position-independent-code)`。`lib1`也将会被编译为位置无关码，因为`SHARED`库的默认设置就是这样的。如果依赖项有冲突的、不兼容的需求，[cmake(1)](https://cmake.org/cmake/help/latest/manual/cmake.1.html#manual:cmake(1))会给出诊断:

```makefile
add_library(lib1 SHARED lib1.cpp)
set_property(TARGET lib1 PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)

add_library(lib2 SHARED lib2.cpp)
set_property(TARGET lib2 PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE OFF)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 lib1)
set_property(TARGET exe1 PROPERTY POSITION_INDEPENDENT_CODE OFF)

add_executable(exe2 exe2.cpp)
target_link_libraries(exe2 lib1 lib2)
```

`lib1`的要求`INTERFACE_POSITION_INDEPENDENT_CODE`和`exe1`的要求`POSITION_INDEPENDENT_CODE`不兼容。该库`lib1`要求使用者必须编译为位置无关码，而`exe1`没有指定编译为位置无关码，所以就会发送一个诊断报告信息。

`lib1`和`lib2`的要求不兼容。一个要求使用者必须编译为位置无关码，而另一个不要求。因为`exe2`需要链接它们两个，而它们却是冲突的，所以CMake会发送错误信息，如下所示：

```makefile
CMake Error: The INTERFACE_POSITION_INDEPENDENT_CODE property of "lib2" does
not agree with the value of POSITION_INDEPENDENT_CODE already determined
for "exe2".
```

为了兼容，`POSITION_INDEPENDENT_CODE`设置的布尔值，应该与所有依赖项的`INTERFACE_POSITION_INDEPENDENT_CODE`在意义上相同。

`接口要求的兼容性`还可以扩展到其它属性，通过设置`COMPATIBLE_INTERFACE_BOOL`目标属性即可。每个指定的属性必须在目标和依赖项传递过来的带有`INTERFACE_*`前缀的属性之间兼容：

```makefile
add_library(lib1Version2 SHARED lib1_v2.cpp)
set_property(TARGET lib1Version2 PROPERTY INTERFACE_CUSTOM_PROP ON)
set_property(TARGET lib1Version2 APPEND PROPERTY
  COMPATIBLE_INTERFACE_BOOL CUSTOM_PROP
)

add_library(lib1Version3 SHARED lib1_v3.cpp)
set_property(TARGET lib1Version3 PROPERTY INTERFACE_CUSTOM_PROP OFF)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 lib1Version2) # CUSTOM_PROP will be ON

add_executable(exe2 exe2.cpp)
target_link_libraries(exe2 lib1Version2 lib1Version3) # Diagnostic
```

`接口要求的兼容性`还可以设置非布尔值，通过`COMPATIBLE_INTERFACE_STRING`目标属性指定。指定的值既可以是未指定的，也可以所有依赖项的相同字符串进行比较。这对于确保某个库的多个不兼容版本不会因为目标的要求传递性而链接到一起，至关重要。

```makefile
add_library(lib1Version2 SHARED lib1_v2.cpp)
set_property(TARGET lib1Version2 PROPERTY INTERFACE_LIB_VERSION 2)
set_property(TARGET lib1Version2 APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING LIB_VERSION
)

add_library(lib1Version3 SHARED lib1_v3.cpp)
set_property(TARGET lib1Version3 PROPERTY INTERFACE_LIB_VERSION 3)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 lib1Version2) # LIB_VERSION will be "2"

add_executable(exe2 exe2.cpp)
target_link_libraries(exe2 lib1Version2 lib1Version3) # Diagnostic
```

`COMPATIBLE_INTERFACE_NUMBER_MAX`属性用于对内容进行数值计算，并计算所有指定内容中的最大数量：

```makefile
add_library(lib1Version2 SHARED lib1_v2.cpp)
set_property(TARGET lib1Version2 PROPERTY INTERFACE_CONTAINER_SIZE_REQUIRED 200)
set_property(TARGET lib1Version2 APPEND PROPERTY
  COMPATIBLE_INTERFACE_NUMBER_MAX CONTAINER_SIZE_REQUIRED
)

add_library(lib1Version3 SHARED lib1_v3.cpp)
set_property(TARGET lib1Version3 PROPERTY INTERFACE_CONTAINER_SIZE_REQUIRED 1000)

add_executable(exe1 exe1.cpp)
# CONTAINER_SIZE_REQUIRED will be "200"
target_link_libraries(exe1 lib1Version2)

add_executable(exe2 exe2.cpp)
# CONTAINER_SIZE_REQUIRED will be "1000"
target_link_libraries(exe2 lib1Version2 lib1Version3)
```

类似地，可以使用`COMPATIBLE_INTERFACE_NUMBER_MIN`计算来自依赖项的属性的数值最小值。

每个计算出来的“兼容”属性值都可以在生成时使用`生成器表达式`在消费者中读取。

对于每个依赖项，每个兼容接口属性中指定的属性集不能与任何其他属性中指定的属性集相交。

#### 3.4 调试属性的来源

因为构建规范可以由依赖关系来决定，所以创建目标的代码和负责设置构建规范的代码的局部缺失可能会使代码更加难以推理。`CMake(1)`提供了一个调试工具来打印属性内容的来源，这些属性可能由依赖项决定。可以调试的属性在`CMAKE_DEBUG_TARGET_PROPERTIES`变量文档中列出:

```makefile
set(CMAKE_DEBUG_TARGET_PROPERTIES
  INCLUDE_DIRECTORIES
  COMPILE_DEFINITIONS
  POSITION_INDEPENDENT_CODE
  CONTAINER_SIZE_REQUIRED
  LIB_VERSION
)
add_executable(exe1 exe1.cpp)
```

对于在`COMPATIBLE_INTERFACE_BOOL`或`COMPATIBLE_INTERFACE_STRING`中列出的属性，调试输出显示哪个目标负责设置该属性，以及哪个其他依赖项也定义了该属性。在`COMPATIBLE_INTERFACE_NUMBER_MAX`和`COMPATIBLE_INTERFACE_NUMBER_MIN`的情况下，调试输出显示每个依赖项的属性值，以及该值是否决定了新的极端值。


#### 3.5 构建规范中的生成器表达式

构建规范可以使用生成器表达式，其中包含的内容可能是有条件的或仅在生成时才能知道的。例如，计算出的属性的兼容值可以通过`TARGET_PROPERTY`表达式读取:

```makefile
add_library(lib1Version2 SHARED lib1_v2.cpp)
set_property(TARGET lib1Version2 PROPERTY
  INTERFACE_CONTAINER_SIZE_REQUIRED 200)
set_property(TARGET lib1Version2 APPEND PROPERTY
  COMPATIBLE_INTERFACE_NUMBER_MAX CONTAINER_SIZE_REQUIRED
)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 lib1Version2)
target_compile_definitions(exe1 PRIVATE
    CONTAINER_SIZE=$<TARGET_PROPERTY:CONTAINER_SIZE_REQUIRED>
)
```

在本例中，将使用`-DCONTAINER_SIZE=200`编译`exe1`的源文件。

Configuration determined build specifications may be conveniently set using the CONFIG generator expression.

```makefile
target_compile_definitions(exe1 PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
)
```

The CONFIG parameter is compared case-insensitively with the configuration being built. In the presence of IMPORTED targets, the content of MAP_IMPORTED_CONFIG_DEBUG is also accounted for by this expression.

Some buildsystems generated by cmake(1) have a predetermined build-configuration set in the CMAKE_BUILD_TYPE variable. The buildsystem for the IDEs such as Visual Studio and Xcode are generated independent of the build-configuration, and the actual build configuration is not known until build-time. Therefore, code such as

```makefile
string(TOLOWER ${CMAKE_BUILD_TYPE} _type)
if (_type STREQUAL debug)
  target_compile_definitions(exe1 PRIVATE DEBUG_BUILD)
endif()
```

may appear to work for Makefile Generators and Ninja generators, but is not portable to IDE generators. Additionally, the IMPORTED configuration-mappings are not accounted for with code like this, so it should be avoided.

`TARGET_PROPERTY`和`TARGET_POLICY`生成器表达式会根据要生成目标的上下文进行评估。这意味着，基于不同的生成目标，`使用要求`规范可能会不同：

```makefile
add_library(lib1 lib1.cpp)
target_compile_definitions(lib1 INTERFACE
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:LIB1_WITH_EXE>
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:LIB1_WITH_SHARED_LIB>
  $<$<TARGET_POLICY:CMP0041>:CONSUMER_CMP0041_NEW>
)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 lib1)

cmake_policy(SET CMP0041 NEW)

add_library(shared_lib shared_lib.cpp)
target_link_libraries(shared_lib lib1)
```

`exe1`将会使用`-DLIB1_WITH_EXE`进行编译；`shared_lib`共享库将会使用`-DLIB1_WITH_SHARED_LIB`和`-DCONSUMER_CMP0041_NEW`进行编译，因为在`shared_lib`创建之前，策略`CMP0041`开始生效。

The BUILD_INTERFACE expression wraps requirements which are only used when consumed from a target in the same buildsystem, or when consumed from a target exported to the build directory using the export() command. The INSTALL_INTERFACE expression wraps requirements which are only used when consumed from a target which has been installed and exported with the install(EXPORT) command:

```
add_library(ClimbingStats climbingstats.cpp)
target_compile_definitions(ClimbingStats INTERFACE
  $<BUILD_INTERFACE:ClimbingStats_FROM_BUILD_LOCATION>
  $<INSTALL_INTERFACE:ClimbingStats_FROM_INSTALLED_LOCATION>
)
install(TARGETS ClimbingStats EXPORT libExport ${InstallArgs})
install(EXPORT libExport NAMESPACE Upstream::
        DESTINATION lib/cmake/ClimbingStats)
export(EXPORT libExport NAMESPACE Upstream::)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 ClimbingStats)
```

在本例中，`exe1`使用`-DClimbingStats_FROM_BUILD_LOCATION`进行编译。导出命令生成的IMPORTED目标，省略了`INSTALL_INTERFACE`或`BUILD_INTERFACE`，也没有`*_INTERFACE`标记。一个使用`ClimbingStats`的独立的工程可以包含如下的内容：

```makefile
find_package(ClimbingStats REQUIRED)

add_executable(Downstream main.cpp)
target_link_libraries(Downstream Upstream::ClimbingStats)
```

取决于是从构建位置还是安装位置使用了`ClimbingStats`包，`Downstream`目标将使用`-DClimbingStats_FROM_BUILD_LOCATION`或`-DClimbingStats_FROM_INSTALL_LOCATION`编译。有关`包`和`导出`的更多信息，请参阅`cmake-packages(7)`手册。

###### 3.5.1 Include目录和使用要求

Include目录被指定为`使用要求`和作为生成器表达式时，有一些特殊的考虑。`target_include_directories()`接受相对和绝对目录。

```makefile
add_library(lib1 lib1.cpp)
target_include_directories(lib1 PRIVATE
  /absolute/path
  relative/path
)
```

相对路径是相对于命令出现的源目录进行解释的。`IMPORTED`目标的`INTERFACE_INCLUDE_DIRECTORIES`中不允许使用相对路径。

在一些重要的生成器表达式中，可以在`INSTALL_INTERFACE`表达式的参数中，使用`INSTALL_PREFIX`前缀。主要用于指定一些特定的位置作为安装目录的前缀。

BUILD目录和INSTALL目录是两回事。`BUILD_INTERFACE`和`INSTALL_INTERFACE`描述在不同的位置使用要求。`INSTALL_INTERFACE`表达式允许相对路径，被解释为相对于安装前缀的位置，如下所示：

```makefile
add_library(ClimbingStats climbingstats.cpp)
target_include_directories(ClimbingStats INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
  $<INSTALL_INTERFACE:/absolute/path>
  $<INSTALL_INTERFACE:relative/path>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/$<CONFIG>/generated>
)
```

Two convenience APIs are provided relating to include directories usage requirements. The CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE variable may be enabled, with an equivalent effect to:

```makefile
set_property(TARGET tgt APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}>
)
```

for each target affected. The convenience for installed targets is an INCLUDES DESTINATION component with the install(TARGETS) command:

```makefile
install(TARGETS foo bar bat EXPORT tgts ${dest_args}
  INCLUDES DESTINATION include
)
install(EXPORT tgts ${other_args})
install(FILES ${headers} DESTINATION include)
```

This is equivalent to appending ${CMAKE_INSTALL_PREFIX}/include to the INTERFACE_INCLUDE_DIRECTORIES of each of the installed IMPORTED targets when generated by install(EXPORT).

When the INTERFACE_INCLUDE_DIRECTORIES of an imported target is consumed, the entries in the property are treated as SYSTEM include directories, as if they were listed in the INTERFACE_SYSTEM_INCLUDE_DIRECTORIES of the dependency. This can result in omission of compiler warnings for headers found in those directories. This behavior for Imported Targets may be controlled by setting the NO_SYSTEM_FROM_IMPORTED target property on the consumers of imported targets.

If a binary target is linked transitively to a macOS FRAMEWORK, the Headers directory of the framework is also treated as a usage requirement. This has the same effect as passing the framework directory as an include directory.

#### 3.6 链接库和生成器表达式

Like build specifications, link libraries may be specified with generator expression conditions. However, as consumption of usage requirements is based on collection from linked dependencies, there is an additional limitation that the link dependencies must form a "directed acyclic graph". That is, if linking to a target is dependent on the value of a target property, that target property may not be dependent on the linked dependencies:

```makefile
add_library(lib1 lib1.cpp)
add_library(lib2 lib2.cpp)
target_link_libraries(lib1 PUBLIC
  $<$<TARGET_PROPERTY:POSITION_INDEPENDENT_CODE>:lib2>
)
add_library(lib3 lib3.cpp)
set_property(TARGET lib3 PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)

add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 lib1 lib3)
```

As the value of the POSITION_INDEPENDENT_CODE property of the exe1 target is dependent on the linked libraries (lib3), and the edge of linking exe1 is determined by the same POSITION_INDEPENDENT_CODE property, the dependency graph above contains a cycle. cmake(1) issues an error message.

#### 3.7 Output Artifacts

The buildsystem targets created by the add_library() and add_executable() commands create rules to create binary outputs. The exact output location of the binaries can only be determined at generate-time because it can depend on the build-configuration and the link-language of linked dependencies etc. TARGET_FILE, TARGET_LINKER_FILE and related expressions can be used to access the name and location of generated binaries. These expressions do not work for OBJECT libraries however, as there is no single file generated by such libraries which is relevant to the expressions.

There are three kinds of output artifacts that may be build by targets as detailed in the following sections. Their classification differs between DLL platforms and non-DLL platforms. All Windows-based systems including Cygwin are DLL platforms.

###### 3.7.1 Runtime Output Artifacts

A runtime output artifact of a buildsystem target may be:

* The executable file (e.g. .exe) of an executable target created by the add_executable() command.

* On DLL platforms: the executable file (e.g. .dll) of a shared library target created by the add_library() command with the SHARED option.

The RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME target properties may be used to control runtime output artifact locations and names in the build tree.

###### 3.7.2 Library Output Artifacts

A library output artifact of a buildsystem target may be:

* The loadable module file (e.g. .dll or .so) of a module library target created by the add_library() command with the MODULE option.

* On non-DLL platforms: the shared library file (e.g. .so or .dylib) of a shared library target created by the add_library() command with the SHARED option.

The LIBRARY_OUTPUT_DIRECTORY and LIBRARY_OUTPUT_NAME target properties may be used to control library output artifact locations and names in the build tree.

###### 3.7.3 Archive Output Artifacts

An archive output artifact of a buildsystem target may be:

* The static library file (e.g. .lib or .a) of a static library target created by the add_library() command with the STATIC option.

* On DLL platforms: the import library file (e.g. .lib) of a shared library target created by the add_library() command with the SHARED option. This file is only guaranteed to exist if the library exports at least one unmanaged symbol.

* On DLL platforms: the import library file (e.g. .lib) of an executable target created by the add_executable() command when its ENABLE_EXPORTS target property is set.

* On AIX: the linker import file (e.g. .imp) of an executable target created by the add_executable() command when its ENABLE_EXPORTS target property is set.

The ARCHIVE_OUTPUT_DIRECTORY and ARCHIVE_OUTPUT_NAME target properties may be used to control archive output artifact locations and names in the build tree.

#### 3.8 Directory-Scoped Commands

The target_include_directories(), target_compile_definitions() and target_compile_options() commands have an effect on only one target at a time. The commands add_compile_definitions(), add_compile_options() and include_directories() have a similar function, but operate at directory scope instead of target scope for convenience.

## 4 伪目标

Some target types do not represent outputs of the buildsystem, but only inputs such as external dependencies, aliases or other non-build artifacts. Pseudo targets are not represented in the generated buildsystem.

#### 4.1 导入目标

An IMPORTED target represents a pre-existing dependency. Usually such targets are defined by an upstream package and should be treated as immutable. After declaring an IMPORTED target one can adjust its target properties by using the customary commands such as target_compile_definitions(), target_include_directories(), target_compile_options() or target_link_libraries() just like with any other regular target.

IMPORTED targets may have the same usage requirement properties populated as binary targets, such as INTERFACE_INCLUDE_DIRECTORIES, INTERFACE_COMPILE_DEFINITIONS, INTERFACE_COMPILE_OPTIONS, INTERFACE_LINK_LIBRARIES, and INTERFACE_POSITION_INDEPENDENT_CODE.

The LOCATION may also be read from an IMPORTED target, though there is rarely reason to do so. Commands such as add_custom_command() can transparently use an IMPORTED EXECUTABLE target as a COMMAND executable.

The scope of the definition of an IMPORTED target is the directory where it was defined. It may be accessed and used from subdirectories, but not from parent directories or sibling directories. The scope is similar to the scope of a cmake variable.

It is also possible to define a GLOBAL IMPORTED target which is accessible globally in the buildsystem.

See the cmake-packages(7) manual for more on creating packages with IMPORTED targets.

#### 4.2 别名目标

An ALIAS target is a name which may be used interchangeably with a binary target name in read-only contexts. A primary use-case for ALIAS targets is for example or unit test executables accompanying a library, which may be part of the same buildsystem or built separately based on user configuration.

```makefile
add_library(lib1 lib1.cpp)
install(TARGETS lib1 EXPORT lib1Export ${dest_args})
install(EXPORT lib1Export NAMESPACE Upstream:: ${other_args})

add_library(Upstream::lib1 ALIAS lib1)
```

In another directory, we can link unconditionally to the Upstream::lib1 target, which may be an IMPORTED target from a package, or an ALIAS target if built as part of the same buildsystem.

```makefile
if (NOT TARGET Upstream::lib1)
  find_package(lib1 REQUIRED)
endif()
add_executable(exe1 exe1.cpp)
target_link_libraries(exe1 Upstream::lib1)
```

ALIAS targets are not mutable, installable or exportable. They are entirely local to the buildsystem description. A name can be tested for whether it is an ALIAS name by reading the ALIASED_TARGET property from it:

```makefile
get_target_property(_aliased Upstream::lib1 ALIASED_TARGET)
if(_aliased)
  message(STATUS "The name Upstream::lib1 is an ALIAS for ${_aliased}.")
endif()
```

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
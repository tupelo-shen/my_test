本文将自己常用的一个makefile一步步按照解决需求的思路，逐步深入。并没有枯燥地讲解makefile的基础，而是在涉及的地方进行说明。这样，在解决问题的兴奋中，逐渐也掌握了makefile编写的真谛。希望对大家有帮助。

# 1 makefile初探

如果没有makefile，我们编译C或者C++代码只能是手动一个个敲，然后再编译。比如，有一个文件`hello.c`,

    gcc -c -o hello hello.c

是不是so easy？但是当有成百上千个`.c`文件的时候，还要一个个敲，太难了。。。。。。聪明的程序员自然不会这么傻：如果下一个shell脚本，让其批量处理所有的文件不就好了吗。基于这个目的，Makefile诞生了。

对于上面的编译命令，有两个编译选项，-c和-o。上面的代码也可以拆开写：

    gcc -c hello.c        #把.c生成.o。-c表示只编译不链接
    gcc -o hello hello.o  #把.o文件生成可执行程序。-o表示生成可执行程序

如果将上面的编译写进makefile，那就是：

    # hello由hello.o生成，所以hello依赖于hello.o，
    # 依赖关系用冒号（:）隔开，左边的叫做目标（target），
    # 右边称为依赖（prerequisites）
    hello : hello.o     
        gcc -o hello hello.o

    hello.o : hello.c
        gcc -c hello.o hello.c

执行make或make hello命令，就可以得到可行性程序hello了。

所以，从上面的代码可以看出Makefile就是根据依赖关系，执行相应的命令的一个脚本文件。其可以简化为一个公式：


    target ... : prerequisites ...
        command

其它一切都是为这个公式服务的。

> makefile隐含规则
> 
> .o文件对.c文件的规则会自动执行，所以上面的makefile可以只保留下面的代码：
> 
>       hello : hello.o     
>           gcc -o hello hello.o

我们在写代码的过程中，经常需要修改，重新编译。所以，有时候需要删除之前编译的结果，makefile提供了为目标clean，一个小的完整makefile如下所示：

    # hello由hello.o生成，所以hello依赖于hello.o;
    # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
    # 右边称为依赖（prerequisites）
    hello:hello.o
        gcc -o hello hello.o

    # hello.o:hello.c
    #   gcc -c hello.c

    .PHONY: clean   # 伪目标的意义就是，无论如何都执行下面的命令
    clean:
        rm -f hello hello.o

# 2 如何编写多个源文件的makefile

假设我们有10个文件，我们称之为hello.c、hello1.c、hello2.c、...、hello9.c，该如何写makefile呢？

1. 最笨的方法-列举法

        # hello由hello.o生成，所以hello依赖于hello.o;
        # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
        # 右边称为依赖（prerequisites）
        hello:hello.o hello1.o hello2.o hello3.o hello4.o hello5.o hello6.o hello7.o hello8.o hello9.o
            gcc -o hello hello.o hello1.o hello2.o hello3.o hello4.o hello5.o hello6.o hello7.o hello8.o hello9.o

        # hello.o:hello.c
        #   gcc -c hello.c

        .PHONY: clean   # 伪目标的意义就是，无论如何都执行下面的命令
        clean:
            rm -f hello hello.o hello1.o hello2.o hello3.o hello4.o hello5.o hello6.o hello7.o hello8.o hello9.o

    这完全没有问题，只是有点傻而已，如果我们有一千个文件呢？难不成也要手写列举？程序员当然没有那么傻

2. wildcard、patsubst以及模式匹配%

    新的makefile代码如下：

        # hello由hello.o生成，所以hello依赖于hello.o;
        # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
        # 右边称为依赖（prerequisites）
        SRC_DIR := .
        OBJS    +=  $(patsubst %.c,%.o,$(notdir $(wildcard $(SRC_DIR)/*.c)))
        hello: $(OBJS)
            gcc -o hello $(OBJS)

        # hello.o:hello.c
        #   gcc -c hello.c

        .PHONY: clean   # 伪目标的意义就是，无论如何都执行下面的命令
        clean:
            rm -f hello $(OBJS)

    这样的写法是不是比方法1的简洁许多？而且如果后续再添加新的源文件都不需要更改makefile。

3. makefile中的3个符号`$@`,`$^`,`$<`

    我们在看别的项目中的makefile时，经常发现这三个符号，它们到底是意思呢？我们以上面的工程和makefile为例进行说明。新的makefile为：

        # hello由hello.o生成，所以hello依赖于hello.o及多个hellox.o文件;
        # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
        # 右边称为依赖（prerequisites）
        SRC_DIR := .
        OBJS    +=  $(patsubst %.c,%.o,$(notdir $(wildcard $(SRC_DIR)/*.c)))
        hello: $(OBJS)
            gcc -o $@ $^
        # $@即hello，$^即$(OBJS)中，所有的.o目标文件
        # 如果此处只有一个依赖文件，比如hello.o，也可以用$<替代$^

        .PHONY: clean       # 伪目标的意义就是，无论如何都执行下面的命令
        clean:
            rm -f hello $(OBJS)

    从上面的实现中，可以看出：

        * `$@`是指所有的编译目标；
        * `$<`是指第一个依赖文件;
        * `$^`是指多个依赖文件，比如上面示例;

# 3 添加子目录下的源文件

假设，我们在当前目录下添加一个子目录`sub/`，并且在该目录下创建3个文件，分别是：`sub_routine1.c`、`sub_routine2.c`和`sub_routine3.c`。并且hello可执行程序的生成，依赖这3个文件。新的makefile如下：

    # hello由hello.o生成，所以hello依赖于hello.o及多个hellox.o文件;
    # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
    # 右边称为依赖（prerequisites）
    SRC_DIR     := .
    SUB_SRC_DIR := ./sub
    OBJS        +=  $(patsubst %.c,%.o, $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c))

    hello: $(OBJS)
        gcc -o $@ $^
    # $@即hello，$^即$(OBJS)中，所有的.o目标文件
    # 如果此处只有一个依赖文件，比如hello.o，也可以用$<替代$^

    .PHONY: clean       # 伪目标的意义就是，无论如何都执行下面的命令
    clean:
        rm -f hello $(OBJS)


* 上面的makefile中需要掌握的语法是：

    1. patsubst

        模式匹配文本并替换。第一个参数，匹配模式；第二个参数，替换字符串；第三个参数，要替换的文本字符串。

    2. wildcard

        列举符合通配符表达式的所有源文件，输出以空格为分隔符的列表。

我们看一下make后的效果，下面是工程目录下的文件树：

    .
    ├── hello
    ├── hello1.c
    ├── hello1.o
    ├── hello2.c
    ├── hello2.o
    ├── hello3.c
    ├── hello3.o
    ├── hello4.c
    ├── hello4.o
    ├── hello5.c
    ├── hello5.o
    ├── hello6.c
    ├── hello6.o
    ├── hello7.c
    ├── hello7.o
    ├── hello8.c
    ├── hello8.o
    ├── hello9.c
    ├── hello9.o
    ├── hello.c
    ├── hello.o
    ├── makefile
    └── sub
        ├── sub_routine1.c
        ├── sub_routine1.o
        ├── sub_routine2.c
        └── sub_routine2.o

虽然这个makefile使用上没有任何问题，但是，我们发现上面的.o目标文件遵循`就近原则`，散落在各个目录下。有时候，我们可能希望生成一个编译目录，把所有的中间过程文件放入其中，从而不会污染我们的源代码，那该怎么实现呢？

# 4 指定编译目录

假设我们在源码root目录下创建一个新目录build/，作为编译过程中间文件的存放位置，新的makefile如下：

    # hello由hello.o生成，所以hello依赖于hello.o及多个hellox.o文件;
    # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
    # 右边称为依赖（prerequisites）
    SRC_DIR     := .
    SUB_SRC_DIR := ./sub

    # 构建目录
    BUILD_DIR   := ./build

    # 目标文件
    SRCS    := $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c)
    OBJS    += $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(notdir $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c))))

    hello: $(OBJS)
        gcc -o $@ $^
    # $@即hello，$^即$(OBJS)中，所有的.o目标文件
    # 如果此处只有一个依赖文件，比如hello.o，也可以用$<替代$^

    # 这时候因为目标文件已经脱离了源文件的环境，
    # 用makefile隐含规则自动编译已经不可行了。
    # 我们显式执行编译
    $(BUILD_DIR)/%.o:$(SRC_DIR)/%.c
        gcc -c -o $@ $^
    $(BUILD_DIR)/%.o:$(SUB_SRC_DIR)/%.c
        gcc -c -o $@ $^

    .PHONY: clean       # 伪目标的意义就是，无论如何都执行下面的命令
    clean:
        rm -f hello $(OBJS)

* 上面需要掌握的函数是

    1. notdir

        去掉文件前面的目录。

    2. addprefix
    
        添加前缀目录。

对于上面的代码，思路就是把所有的目标文件拷贝到`build/`目录下，依赖的源文件还在原位置不动。

# 6 添加头文件的支持

我们再次修改目录结构，并添加两个头文件`sub_routine1.h`和`sub_routine2.h`。源代码结构如下：

    .
    ├── build
    ├── hello1.c
    ├── hello2.c
    ├── hello3.c
    ├── hello4.c
    ├── hello5.c
    ├── hello6.c
    ├── hello7.c
    ├── hello8.c
    ├── hello9.c
    ├── hello.c
    ├── makefile
    └── sub
        ├── inc
        │   ├── sub_routine1.h
        │   └── sub_routine2.h
        └── src
            ├── sub_routine1.c
            └── sub_routine2.c

新的makefile如下：

    # hello由hello.o生成，所以hello依赖于hello.o及多个hellox.o文件;
    # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
    # 右边称为依赖（prerequisites）
    SRC_DIR     := .
    SUB_SRC_DIR := ./sub/src
    # 头文件目录
    SUB_INCS_DIR:= -I ./sub/inc
    # 构建目录
    BUILD_DIR   := ./build

    # 目标文件
    # OBJS      +=  $(patsubst %.c,%.o, $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c))
    SRCS    := $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c)
    OBJS    += $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(notdir $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c))))

    TARGET  := hello

    all: $(TARGET)
        @echo 'build is completed!'

    $(TARGET): $(OBJS)
        gcc -o $(TARGET) $^
    # $@即hello，$^即$(OBJS)中，所有的.o目标文件
    # 如果此处只有一个依赖文件，比如hello.o，也可以用$<替代$^

    # 这时候因为目标文件已经脱离了源文件的环境，
    # 用makefile隐含规则自动编译已经不可行了。
    # 我们显式执行编译
    $(BUILD_DIR)/%.o:$(SRC_DIR)/%.c
        gcc -c $(SUB_INCS_DIR) -o $@ $^
    $(BUILD_DIR)/%.o:$(SUB_SRC_DIR)/%.c
        gcc -c $(SUB_INCS_DIR) -o $@ $^

    .PHONY: clean       # 伪目标的意义就是，无论如何都执行下面的命令
    clean:
        rm -f $(TARGET) $(OBJS)

查看变量`SUB_INCS_DIR`的增加，即明白怎样添加头文件了。

上面的代码有一个问题就是，如果你改动了.c文件。代表依赖关系改变了，会重新编译那个文件。但是，修改头文件不会引起重新编译，因为编译器认为没有发生依赖关系上的改动。

# 7 添加依赖关系的支持

有时候，我们改动了某个头文件或者源文件，不想`make clean`，再重新`make`。只想直接编译改动相关的文件而已。那么，按照这个需求实现的新makefile，内容如下所示：

    # 编译器
    CC := gcc
    # hello由hello.o生成，所以hello依赖于hello.o及多个hellox.o文件;
    # 依赖关系用冒号（:）隔开，左边的叫做目标（target）;
    # 右边称为依赖（prerequisites）
    SRC_DIR     := .
    SUB_SRC_DIR := ./sub/src
    # 头文件目录
    SUB_INCS_DIR:= -I ./sub/inc
    # 构建目录
    BUILD_DIR   := ./build
    DEPS_DIR    := $(BUILD_DIR)/deps

    # 源文件
    # OBJS      +=  $(patsubst %.c,%.o, $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c))
    SRCS    := $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c)
    # 目标文件
    OBJS    += $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(notdir $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c))))
    # 添加依赖关系
    C_DEPS  := $(addprefix $(DEPS_DIR)/,$(patsubst %.c,%.d,$(notdir $(wildcard $(SRC_DIR)/*.c $(SUB_SRC_DIR)/*.c))))

    TARGET  := hello

    all: $(TARGET)
        @echo 'build is completed!'

    $(TARGET): $(OBJS)
        $(CC) -o $(TARGET) $^
    # $@即hello，$^即$(OBJS)中，所有的.o目标文件
    # 如果此处只有一个依赖文件，比如hello.o，也可以用$<替代$^

    # 1. 这时候因为目标文件已经脱离了源文件的环境，
    # 用makefile隐含规则自动编译已经不可行了。
    # 我们显式执行编译。
    # 2. 把之前的$^修改为$<，要不然不符合一一对应的关系
    $(BUILD_DIR)/%.o:$(SRC_DIR)/%.c
        $(CC) -c $(SUB_INCS_DIR)  $< -o $@
    $(BUILD_DIR)/%.o:$(SUB_SRC_DIR)/%.c
        $(CC) -c $(SUB_INCS_DIR)  $< -o $@

    # 添加依赖关系
    $(DEPS_DIR)/%.d : $(SRC_DIR)/%.c
        @set -e; $(CC) -MM $(SUB_INCS_DIR) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,$(BUILD_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$
    $(DEPS_DIR)/%.d : $(SUB_SRC_DIR)/%.c
        @set -e; $(CC) -MM $(SUB_INCS_DIR) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,$(BUILD_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

    include $(C_DEPS)

    .PHONY: clean       # 伪目标的意义就是，无论如何都执行下面的命令
    clean:
        rm -f $(TARGET) $(OBJS) $(C_DEPS)

现在编译后的文件树结构为：

    .
    ├── build
    │   ├── deps
    │   │   ├── hello1.d
    │   │   ├── hello2.d
    │   │   ├── hello.d
    │   │   ├── sub_routine1.d
    │   │   └── sub_routine2.d
    │   ├── hello1.o
    │   ├── hello2.o
    │   ├── hello.o
    │   ├── sub_routine1.o
    │   └── sub_routine2.o
    ├── hello
    ├── hello1.c
    ├── hello2.c
    ├── hello.c
    ├── makefile
    └── sub
        ├── inc
        │   ├── sub_routine1.h
        │   └── sub_routine2.h
        └── src
            ├── sub_routine1.c
            └── sub_routine2.c

至此，makefile的讲解到此为止了。上面的makefile应该能满足大部分中小规模的项目开发了。

如果你还有其它的想法，可以与我交流。
[TOC]

Python的优势之一就是具有大量丰富的、通用的标准库，这使得它可以成为为命令行编写脚本的最佳编程语言之一。既然是为命令行编写脚本，提供一个良好的命令行接口就成为一件很重要的事情了。而Python的`argparse`库恰恰擅长此事。今天就让我们一睹芳容吧。

通过本文，你将掌握：

* argparse库是什么，为什么对使用Python编写命令行脚本时很重要
* 怎样使用argparse库快速构建一个命令行接口
* argparse库的高阶用法

# 1 命令行接口-CLI

命令行接口是一种与命令行脚本交互的工具。`argparse`库主要是想替代旧的`getopt`和`optparse`模块。

The Python argparse library:

* Allows the use of positional arguments
* Allows the customization of the prefix chars
* Supports variable numbers of parameters for a single option
* Supports subcommands (A main command line parser can use other command line parsers depending on some arguments.)

Before getting started, you need to know how a command line interface works, so open a terminal on your computer and execute the command ls to get the list of the files contained in the current directory like this:

    $ ls
    dcdb_20180201.sg4    mastro35.sg4        openings.sg4
    dcdb_20180201.si4    mastro35.si4        openings.si4
    dcdb_20180201.sn4    mastro35.sn4        openings.sn4

As you can see, there are a bunch of files in the current directory, but the command didn’t return a lot of information about these files.

The good news is that you don’t need to look around for another program to have a richer list of the files contained in the current directory. You also don’t need to modify the ls command yourself, because it adopts a command line interface, that is just a set of tokens (called arguments) that you can use to configure the behavior of this command.

Now try to execute the command ls again, but with adding the `-l` option to the command line as in the example below:

    $ ls -l
    total 641824
    -rw-------  1 dave  staff  204558286  5 Mar  2018 dcdb_20180201.sg4
    -rw-------  1 dave  staff  110588409  5 Mar  2018 dcdb_20180201.si4
    -rw-------  1 dave  staff    2937516  5 Mar  2018 dcdb_20180201.sn4
    -rw-------  1 dave  staff     550127 27 Mar  2018 mastro35.sg4
    -rw-------  1 dave  staff      15974 11 Gen 17:01 mastro35.si4
    -rw-------  1 dave  staff       3636 27 Mar  2018 mastro35.sn4
    -rw-------  1 dave  staff      29128 17 Apr  2018 openings.sg4
    -rw-------  1 dave  staff        276 17 Apr  2018 openings.si4
    -rw-------  1 dave  staff         86 18 Apr  2018 openings.sn4

The output is very different now. The command has returned a lot of information about the permissions, owner, group, and size of each file and the total directory occupation on the disk.

This is because you used the command line interface of the ls command and specified the -l option that enables the long format, a special format that returns a lot more information for every single file listed.

In order to familiarize yourself with this topic, you’re going to read a lot about arguments, options, and parameters, so let’s clarify the terminology right away:

* An `argument` is a single part of a command line, delimited by blanks.
* An `option` is a particular type of argument (or a part of an argument) that can modify the behavior of the command line.
* A `parameter` is a particular type of argument that provides additional information to a single option or command.

Consider the following command:

    $ ls -l -s -k /var/log

In this example, you have five different arguments:

* `ls`: the name of the command you are executing
* `-l`: an option to enable the long list format
* `-s`: an option to print the allocated size of each file
* `-k`: an option to have the size in kilobytes
* `/var/log`: a parameter that provides additional information (the path to list) to the command

Note that, if you have multiple options in a single command line, then you can combine them into a single argument like this:

    $ ls -lsk /var/log

Here you have only three arguments:

* `ls` : the name of the command you are executing
* `-lsk`: the three different options you want to enable (a combination of -l, -s, and -k)
* `/var/log`: a parameter that provides additional information (the path to list) to the command

# 2 什么时候使用命令行接口呢？

Now that you know what a command line interface is, you may be wondering when it’s a good idea to implement one in your programs. The rule of thumb is that, if you want to provide a **user-friendly approach to configuring your program**, then you should consider a command line interface, and the standard way to do it is by using the Python argparse library.

Even if you’re creating a complex command line program that needs a **configuration file** to work, if you want to let your user specify which configuration file to use, it’s a good idea to accept this value by creating a command line interface with the Python argparse library.

# 3 怎样使用argparse库呢？

Using the Python argparse library has four steps:

1. Import the Python argparse library
2. Create the parser
3. Add optional and positional arguments to the parser
4. Execute `.parse_args()`

After you execute `.parse_args()`, what you get is a Namespace object that contains a simple property for each input argument received from the command line.

In order to see these four steps in detail with an example, let’s pretend you’re creating a program named `myls.py` that lists the files contained in the current directory. Here’s a possible implementation of your command line interface without using the Python argparse library:

    # myls.py
    import os
    import sys

    if len(sys.argv) > 2:
        print('You have specified too many arguments')
        sys.exit()

    if len(sys.argv) < 2:
        print('You need to specify the path to be listed')
        sys.exit()

    input_path = sys.argv[1]

    if not os.path.isdir(input_path):
        print('The path specified does not exist')
        sys.exit()

    print('\n'.join(os.listdir(input_path)))


This is a possible implementation of the command line interface for your program that doesn’t use the Python `argparse` library, but if you try to execute it, then you’ll see that it works:

    $ python myls.py
    You need to specify the path to be listed

    $ python myls.py /mnt /proc /dev
    You have specified too many arguments

    $ python myls.py /mnt
    dir1
    dir2

As you can see, the script does work, but the output is quite different from the output you’d expect from a standard built-in command.

Now, let’s see how the Python argparse library can improve this code:

    # myls.py
    # Import the argparse library
    import argparse

    import os
    import sys

    # Create the parser
    my_parser = argparse.ArgumentParser(description='List the content of a folder')

    # Add the arguments
    my_parser.add_argument('Path',
                           metavar='path',
                           type=str,
                           help='the path to list')

    # Execute the parse_args() method
    args = my_parser.parse_args()

    input_path = args.Path

    if not os.path.isdir(input_path):
        print('The path specified does not exist')
        sys.exit()

    print('\n'.join(os.listdir(input_path)))

The code has changed a lot with the introduction of the Python argparse library.

The first big difference compared to the previous version is that the if statements to check the arguments provided by the user are gone because the library will check the presence of the arguments for us.

We’ve imported the Python argparse library, created a simple parser with a brief description of the program’s goal, and defined the positional argument we want to get from the user. Lastly, we have executed `.parse_args()` to parse the input arguments and get a Namespace object that contains the user input.

Now, if you run this code, you’ll see that with just four lines of code. You have a very different output:

    $ python myls.py
    usage: myls.py [-h] path
    myls.py: error: the following arguments are required: path

As you can see, the program has detected that you needed at least a positional argument (path), and so the execution of the program has been interrupted with a specific error message.

You may also have noticed that now your program accepts an optional `-h` flag, like in the example below:

    $ python myls.py -h
    usage: myls.py [-h] path

    List the content of a folder

    positional arguments:
    path        the path to list

    optional arguments:
    -h, --help  show this help message and exit

Good, now the program responds to the `-h` flag, displaying a help message that tells the user how to use the program. Isn’t that neat, considering that you didn’t even need to ask for that feature?

Lastly, with just four lines of code, now the args variable is a Namespace object, which has a property for each argument that has been gathered from the command line. That’s super convenient.

# 4 argparse库的高阶应用

In the previous section, you learned the basic usage of the Python argparse library, and now you can implement a simple command line interfaces for all your programs. However, there’s a lot more that you can achieve with this library. In this section, you’ll see almost everything this library can offer you.

## 4.1 设置程序的名称

By default, the library uses the value of the sys.argv[0] element to set the name of the program, which as you probably already know is the name of the Python script you have executed. However, you can specify the name of your program just by using the `prog` keyword:

    # Create the parser
    my_parser = argparse.ArgumentParser(prog='myls',
                                        description='List the content of a folder')

With the `prog` keyword, you specify the name of the program that will be used in the help text:

As you can see, now the program name is just `myls` instead of `myls.py`.

## 4.2 显示一个用户自定义使用帮助

By default, the program usage help has a standard format defined by the Python argparse library. However, you can customize it with the usage keyword like this:

    # Create the parser
    my_parser = argparse.ArgumentParser(prog='myls',
                                        usage='%(prog)s [options] path',
                                        description='List the content of a folder')

Note that, at runtime, the `%(prog)s` token is automatically replaced with the name of your program:

    $ python myls.py
    usage: myls [options] path
    myls: error: too few arguments

As you can see, the help of the program now shows a different usage string, where the [-h] option has been replaced by a generic [options] token.

## 4.3 在帮助文本的前后添加文本显示

To customize the text displayed before and after the arguments help text, you can use two different keywords:

* *description*: for the text that is shown before the help text
* *epilog*: for the text shown after the help text

You’ve already seen the `description` keyword in the previous chapter, so let’s see an example of how the `epilog` keyword works:

    # Create the parser
    my_parser = argparse.ArgumentParser(description='List the content of a folder',
                                        epilog='Enjoy the program! :)')

The `epilog` keyword here has customized the text that will be shown after the standard help text:

    $ python myls.py -h
    usage: myls.py [-h] path

    List the content of a folder

    positional arguments:
    path        the path to list

    optional arguments:
    -h, --help  show this help message and exit

    Enjoy the program! :)

Now the output shows the extra text that has been customized by the epilog keyword.

## 4.4 定制前缀字符

我们知道，命令行的可选选项一般都使用`-`连接符作为前缀。假设我不想使用这个符号，而是使用`/`作为前缀的话，怎么办呢？`argparse`库提供了`prefix_chars`关键字，可以帮你定制：

    my_parser = argparse.ArgumentParser(description='List the content of a folder',
                                        epilog='Enjoy the program! :)',
                                        prefix_chars='/')

After the redefinition, the program now supports a completely different prefix char, and the help text has changed accordingly:

    $ python myls.py
    usage: myls.py [/h] path
    myls.py: error: too few arguments

As you can see, now your program does not support the -h flag but the /h flag. That’s especially useful when you’re coding for Microsoft Windows because Windows users are used to these prefix chars when working with the command line.

## 4.5 为参数配置文件指定前缀字符

当处理非常长或复杂的命令行时，最好将参数保存到外部文件中，然后让程序从其中加载参数。Python的`argparse`库可以为您完成这项工作。

为了测试这个特性，创建下面的Python程序:

    # fromfile_example.py
    import argparse

    my_parser = argparse.ArgumentParser(fromfile_prefix_chars='@')

    my_parser.add_argument('a',
                           help='a first argument')
    my_parser.add_argument('b',
                           help='a second argument')
    my_parser.add_argument('c',
                           help='a third argument')
    my_parser.add_argument('d',
                           help='a fourth argument')
    my_parser.add_argument('e',
                           help='a fifth argument')
    my_parser.add_argument('-v',
                           '--verbose',
                           action='store_true',
                           help='an optional argument')
    # Execute parse_args()
    args = my_parser.parse_args()

    print('If you read this line it means that you have provided '
          'all the parameters')

注意上面的代码，我们在创建parser的时候，使用了关键字`fromfile_prefix_chars`。

如果直接执行，会得到如下的错误信息：

    $ python fromfile_example.py
    usage: fromfile_example.py [-h] [-v] a b c d e
    fromfile_example.py: error: the following arguments are required: a, b, c, d, e

接下来，我们创建一个文本文件`args.txt`，内容如下：

    first
    second
    third
    fourth
    fifth

由于我们指定了前缀字符，用来指示获取参数的外部文件。执行如下的命令，会得到想要的结果：

    $ python fromfile_example.py @args.txt
    If you read this line it means that you have provided all the parameters

从上面的示例中，我们看到`argparse`从文件`args.txt`读取参数。

## 4.6 缩写识别功能

`argparse`库还提供了处理缩写的功能。下面的程序会打印`--input`参数指定的数值：

    # abbrev_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('--input', action='store', type=int, required=True)
    my_parser.add_argument('--id', action='store', type=int)

    args = my_parser.parse_args()

    print(args.input)

现在，让我们分析一下`argparse`是如何处理缩写的。我们可以调用多次这个小程序，每次指定不同的input参数，看看效果：

    $ python abbrev_example.py --input 42
    42
    $ python abbrev_example.py --inpu 42
    42
    $ python abbrev_example.py --inp 42
    42
    $ python abbrev_example.py --in 42
    42

我们可以看到，input单词不断缩短，解析器都能解析。但是，如果仅仅指定`--i`的话，会报错：

    $ python abbrev_example.py --i 42
    usage: abbrev_example.py [-h] --input INPUT [--id ID]
    abbrev_example.py: error: ambiguous option: --i could match --input, --id

因为解析器中有两个参数，`--input`和`--id`，它无法判断`--i`是哪一个参数。

如果不喜欢，可以禁止这个功能。使用关键字`allow_abbrev`，设置为`False`，如下所示：

    # abbrev_example.py
    import argparse

    my_parser = argparse.ArgumentParser(allow_abbrev=False)
    my_parser.add_argument('--input', action='store', type=int, required=True)

    args = my_parser.parse_args()

    print(args.input)

此时，再运行会报错：

    $ python abbrev_example.py --inp 42
    usage: abbrev_example.py [-h] --input INPUT
    abbrev_example.py: error: the following arguments are required: --input

## 4.7 使用自动help功能

在前面的示例中，我们添加`-h`选项，可以获取help帮助信息。这是`arpparse`库一个很有用的功能，不需要任何编码。但是，有时候我们想要禁止这个功能。可以使用关键字`add_help`，将其设为`False`：

    my_parser = argparse.ArgumentParser(description='List the content of a folder',
                                        add_help=False)

此时，运行代码，不会接受`-h`选项：

    $ myls.py
    usage: myls.py path
    myls.py: error: the following arguments are required: path

## 4.8 设置参数的名称和标志

基本上，命令行参数有两种类型：

* 位置参数
* 可选参数

位置参数是命令需要操作的目标。在前面的示例中，`path`参数就是一个位置参数，我们的程序没有它不能工作。

拿linux系统上的cp命令来说，其标准用法是：

    $ cp [OPTION]... [-T] SOURCE DEST

在这儿，第一个位置参数是`SOURCE`，代表的是我们想要拷贝的源文件；第二个位置参数是`DEST`，代表的是我们想要拷贝的目的地。

而可选参数不是强制的，通过它可以修改命令运行时的行为。在`cp`的示例中，可选参数`-r`，表示可以递归拷贝目录。

从语法上说，可选参数一般以`-`或`--`开始，而位置参数不需要。

添加可选参数需要调用方法`.add_argument()`，使用`-`开始命名一个新的参数。

我们修改`myls.py`文件：

    # myls.py
    # Import the argparse library
    import argparse

    import os
    import sys

    # Create the parser
    my_parser = argparse.ArgumentParser(description='List the content of a folder')

    # Add the arguments
    my_parser.add_argument('Path',
                           metavar='path',
                           type=str,
                           help='the path to list')
    my_parser.add_argument('-l',
                           '--long',
                           action='store_true',
                           help='enable the long listing format')

    # Execute parse_args()
    args = my_parser.parse_args()

    input_path = args.Path

    if not os.path.isdir(input_path):
        print('The path specified does not exist')
        sys.exit()

    for line in os.listdir(input_path):
        if args.long:  # Simplified long listing
            size = os.stat(os.path.join(input_path, line)).st_size
            line = '%10d  %s' % (size, line)
        print(line)

运行程序，查看是否接收`-l`选项：
    
    $ python myls.py -h
    usage: myls.py [-h] [-l] path

    List the content of a folder

    positional arguments:
    path        the path to list

    optional arguments:
    -h, --help  show this help message and exit
    -l, --long  enable the long listing format

我们可以看到，`-l`选项可以使用了。

## 4.9 设置参数的动作

有时候，我们想要指定参数的行为，比如是存储、常量还是列表等等。下面是几种常见的参数动作：

* **store** - 存储参数的输入值到Namespace对象。（这是默认行为）
* **store_const** - 只要指定了可选参数，存储一个设定的常量。也就是说，不需要输入值。
* **store_true** - 将可选参数存储为`Boolean`类型的True。未指定存储为False。
* **store_false** - 将可选参数存储为`Boolean`类型的False。未指定存储为True。
* **append** - 将指定的可选参数添加到列表中。
* **append_const** - 将一个指定的常量添加到列表中。
* **count** - 存储指定可选参数的次数，类型为int。
* **help** - 显示帮助信息。
* **version** - 显示程序的版本信息。

示例：

    # actions_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.version = '1.0'
    my_parser.add_argument('-a', action='store')
    my_parser.add_argument('-b', action='store_const', const=42)
    my_parser.add_argument('-c', action='store_true')
    my_parser.add_argument('-d', action='store_false')
    my_parser.add_argument('-e', action='append')
    my_parser.add_argument('-f', action='append_const', const=42)
    my_parser.add_argument('-g', action='count')
    my_parser.add_argument('-i', action='help')
    my_parser.add_argument('-j', action='version')

    args = my_parser.parse_args()

    print(vars(args))

测试1：

    $ python actions_example.py
    {'a': None, 'b': None, 'c': False, 'd': True, 'e': None, 'f': None, 'g': None}

如您所见，除了设置是否存储为布尔型的可选参数，默认值都是`None`。

测试2：

    $ python actions_example.py -a 42
    {'a': '42', 'b': None, 'c': False, 'd': True, 'e': None, 'f': None, 'g': None}

    $ python actions_example.py -a "test"
    {'a': 'test', 'b': None, 'c': False, 'd': True, 'e': None, 'f': None, 'g': None}

如上所示，`store`不假思索地将我们设置的输入值存储起来。

测试3：

    $ python actions_example.py -b
    {'a': None, 'b': 42, 'c': False, 'd': True, 'e': None, 'f': None, 'g': None}

只要指定了`-b`，就会存储常量42。

测试4：

    $ python actions_example.py
    {'a': None, 'b': None, 'c': False, 'd': True, 'e': None, 'f': None, 'g': None}
    $ python actions_example.py -c
    {'a': None, 'b': None, 'c': True, 'd': True, 'e': None, 'f': None, 'g': None}
    $ python actions_example.py -d
    {'a': None, 'b': None, 'c': False, 'd': False, 'e': None, 'f': None, 'g': None}

只要指定了`-c`或`-d`，存储为对应的布尔型值。

测试5-`append`:

    $ python actions_example.py -e me -e you -e us
    {'a': None, 'b': None, 'c': False, 'd': True, 'e': ['me', 'you', 'us'], 'f': None, 'g': None}

测试6-`append_const`:

    $ python actions_example.py -f -f
    {'a': None, 'b': None, 'c': False, 'd': True, 'e': None, 'f': [42, 42], 'g': None}

测试7-`count`：

    $ python actions_example.py -ggg
    {'a': None, 'b': None, 'c': False, 'd': True, 'e': None, 'f': None, 'g': 3}
    $ python actions_example.py -ggggg
    {'a': None, 'b': None, 'c': False, 'd': True, 'e': None, 'f': None, 'g': 5}

测试8-`help`：

    $ python actions_example.py -i
    usage: actions_example.py [-h] [-a A] [-b] [-c] [-d] [-e E] [-f] [-g] [-i]
    [-j]

    optional arguments:
    -h, --help  show this help message and exit
    -a A
    -b
    -c
    -d
    -e E
    -f
    -g
    -i
    -j          show program's version number and exit

测试9-`version`:

    $ python actions_example.py -j
    1.0

### 4.9.1 定制参数的动作

有时候，我们可能想要定制化参数的动作行为。这可以通过子类化`argparse.Action`类，并实现两个方法来完成。

我们来看一下下面的示例，实现了用户自定义的`store`行为，仅仅比标准的繁琐些而已。

    # custom_action.py
    import argparse

    class VerboseStore(argparse.Action):
        def __init__(self, option_strings, dest, nargs=None, **kwargs):
            if nargs is not None:
                raise ValueError('nargs not allowed')
            super(VerboseStore, self).__init__(option_strings, dest, **kwargs)

        def __call__(self, parser, namespace, values, option_string=None):
            print('Here I am, setting the ' \
                  'values %r for the %r option...' % (values, option_string))
            setattr(namespace, self.dest, values)

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-i', '--input', action=VerboseStore, type=int)

    args = my_parser.parse_args()

    print(vars(args))

测试：

    $ python custom_action.py -i 42
    Here I am, setting the values 42 for the '-i' option...
    {'input': 42}

## 4.10 设置可选参数的输入值个数

默认情况下，解析器假设每个参数对应一个输入值。但是，可以通过关键字`nargs`改变这个默认行为。

示例如下，我们创建一个可选参数，要求3个输入值。

    # nargs_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('--input', action='store', type=int, nargs=3)
    args = my_parser.parse_args()
    print(args.input)

现在，可选项`--input`接受3个输入值：

    $ python nargs_example.py --input 42
    usage: nargs_example.py [-h] [--input INPUT INPUT INPUT]
    nargs_example.py: error: argument --input: expected 3 arguments

    $ python nargs_example.py --input 42 42 42
    [42, 42, 42]

`nargs`关键字也可以接受下面的设置：

* **？** - 单个值，可选
* **\*** - 类似于通配符，可以多个值
* **+** - 类似于`*`，但要求至少一个输入值
* **argparse.REMAINDER** - 命令行中剩余所有的输入值

示例代码1-`？`：这个示例中，位置参数`input`接受提供的单个值，但是，如果不提供的话，使用关键字`default`指定的默认值。

    # nargs_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('input',
                           action='store',
                           nargs='?',
                           default='my default value')
    args = my_parser.parse_args()
    print(args.input)

测试：

    $ python nargs_example.py 'my custom value'
    my custom value

    $ python nargs_example.py
    my default value

示例代码2-`*`：如果设置了通配符，则将所有的输入值添加到列表中。

    # nargs_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('input',
                           action='store',
                           nargs='*',
                           default='my default value')
    args = my_parser.parse_args()
    print(args.input)

测试：

    $ python nargs_example.py me you us
    ['me', 'you', 'us']

    $ python nargs_example.py
    my default value


示例代码3-`+`：这种情况下，要求至少一个输入值。

    # nargs_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('input', action='store', nargs='+')
    args = my_parser.parse_args()
    print(args.input)

如果执行上面的代码，不添加位置参数，则会报错：

    $ python nargs_example.py me you us
    ['me', 'you', 'us']

    $ python nargs_example.py
    usage: nargs_example.py [-h] input [input ...]
    nargs_example.py: error: the following arguments are required: input

示例代码4-`argparse.REMAINDER`：就是把所有的剩余的输入值，存储为一个列表。

    # nargs_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('first', action='store')
    my_parser.add_argument('others', action='store', nargs=argparse.REMAINDER)

    args = my_parser.parse_args()

    print('first = %r' % args.first)
    print('others = %r' % args.others)

测试：

    $ python nargs_example.py me you us
    first = 'me'
    others = ['you', 'us']

## 4.11 设置参数的默认值

我们知道，如果在命令行中不指定可选参数的话，解析器默认存储为`None`。但是，可以通过关键字`default`设置一个默认值。

示例1：

    # default_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-a', action='store', default='42')
    args = my_parser.parse_args()
    print(vars(args))

测试：

    $ python default_example.py
    {'a': '42'}

## 4.12 设置参数的类型

通过关键字`type`可以设置参数类型，默认情况是字符串。

示例代码：

    # type_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-a', action='store', type=int)
    args = my_parser.parse_args()
    print(vars(args))

指定参数的类型是int，告诉`argparse`，Namespace对象的`.a`属性必须是int（代替string）。

测试：

    $ python type_example.py -a 42
    {'a': 42}

运行时会检查参数的值，如果类型不对，会报错。

## 4.13 设置参数的允许值范围

示例代码：

    # choices_ex.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-a', action='store', choices=['head', 'tail'])

    args = my_parser.parse_args()

甚至可以使用函数`range()`指定可接受的范围：

    # choices_ex.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-a', action='store', type=int, choices=range(1, 5))
    args = my_parser.parse_args()
    print(vars(args))

测试：

    $ python choices_ex.py -a 4
    {'a': 4}

    $ python choices_ex.py -a 40
    usage: choices_ex.py [-h] [-a {1,2,3,4}]
    choices_ex.py: error: argument -a: invalid choice: 40 (choose from 1, 2, 3, 4)

## 4.14 设置可选参数是否必须

使用关键字`required`设置。

    # required_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-a',
                           action='store',
                           choices=['head', 'tail'],
                           required=True)
    args = my_parser.parse_args()
    print(vars(args))

测试：

    $ python required_example.py
    usage: required_example.py [-h] -a {head,tail}
    required_example.py: error: the following arguments are required: -a

    $ python required_example.py -a head
    {'a': 'head'}

## 4.15 设置帮助信息

使用关键字`help`。

示例代码：

    # help_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-a',
                           action='store',
                           choices=['head', 'tail'],
                           help='set the user choice to head or tail')
    args = my_parser.parse_args()
    print(vars(args))

上面的代码展示了如何定义一个自定义的help文本给`-a`参数，给用户提供更多可用的帮助信息。

测试：

    $ python help_example.py -h
    usage: help_example.py [-h] [-a {head,tail}]

    optional arguments:
    -h, --help      show this help message and exit
    -a {head, tail}  set the user choice to head or tail

## 4.16 定义互斥组

`argparse`库允许建立互相排斥的一组选项。

示例代码：

    # groups.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_group = my_parser.add_mutually_exclusive_group(required=True)

    my_group.add_argument('-v', '--verbose', action='store_true')
    my_group.add_argument('-s', '--silent', action='store_true')
    args = my_parser.parse_args()
    print(vars(args))

你可以指定`-v`或`-s`标志，但是不能在同一个命令行里出现。测试如下：

    $ python groups.py -h
    usage: groups.py [-h] (-v | -s)

    optional arguments:
      -h, --help     show this help message and exit
      -v, --verbose
      -s, --silent

    $ python groups.py -v -s
    usage: groups.py [-h] (-v | -s)
    groups.py: error: argument -s/--silent: not allowed with argument -v/--verbose

## 4.17 设置帮助信息中的参数名称

如果一个参数接受输入值，那么给输入值设置名称，方便parser解析器产生帮助信息，是非常有用的。方法就是使用`metavar`关键字。下面的示例中，使用`metavar`关键字给`-v`标志指定一个名称：

    # metavar_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-v',
                           '--verbosity',
                           action='store',
                           type=int,
                           metavar='LEVEL')
    args = my_parser.parse_args()
    print(vars(args))

运行程序，指定`-h`标志，帮助信息就会给`-v`标志的输入值设置名称`LEVEL`。

    $ python metavar_example.py -h
    usage: metavar_example.py [-h] [-v LEVEL]

    optional arguments:
      -h, --help            show this help message and exit
      -v LEVEL, --verbosity LEVEL

## 4.18 设置属性的名称

正如我们已经看到的，当我们向`parser`解析器添加参数的时候，该参数的值被存储在Namespace对象的属性中。默认情况下，这个属性的命名方式是，使用传递进行的第一个实参作为位置参数，第二个形参作为该属性的名称。

如果一个选项使用了`-`连字符，则在属性名称中被转换为`_`下划线：

    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-v',
                           '--verbosity-level',
                           action='store',
                           type=int)
    args = my_parser.parse_args()
    print(args.verbosity_level)

但是，可以使用`dest`关键字修改这个属性的名称：

    # dest_example.py
    import argparse

    my_parser = argparse.ArgumentParser()
    my_parser.add_argument('-v',
                           '--verbosity',
                           action='store',
                           type=int,
                           dest='my_verbosity_level')
    args = my_parser.parse_args()
    print(vars(args))

运行程序，你会发现`args`变量包含一个`.my_verbosity_level`的属性，即使默认名称被指定为`.verbosity`：

    $ python dest_example.py -v 42
    {'my_verbosity_level': 42}

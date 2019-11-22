Sublime Text3编写Python代码很方便，但是自带的Python编译方式无法获取input()函数的内容，为此，有2种方法可以获取input输入。一种是使用Windows自带的cmd命令行运行Python程序；另一种是使用插件SublimeREPL运行程序。

# 1 cmd命令行

打开`sublime->tool->build system->build new system`，添加下面的内容，并保存为`Python37`：

    {
        "encoding": "utf-8",
        "working_dir": "$file_path",
        "shell_cmd": "python.exe -u \"$file\"",
        "file_regex": "^[ ]*File \"(...*?)\", line ([0-9]*)",
        "selector": "source.python",
        "variants":
        [
            {
                "name" :"RunInCmd",
                "shell" :true,
                "shell_cmd":"start cmd /c \" python \"$file\" & pause\""
            }

        ]
    }

# 2 安装插件SublimeREPL

1. 使用快捷键`CTRL+SHIFT+P`，打开`Package Control`，选择`SublimeREPL`进行安装。

2. 使用工具`Tools-> SublimeREPL->Python->Python-RUN current file`运行代码即可获取input输入。

3. 使用快捷键简化操作流程，方法如下：

    * 选择`Sublime Text 3-> Preferences-> Key Bindings`

    * 输入下列内容并保存：

            [
                {

                "keys":["f5"],
                "caption": "SublimeREPL: Python - RUN current file",
                "command": "run_existing_window_command", "args": {"id": "repl_python_run",
                "file": "config/Python/Main.sublime-menu"}

                }
            ]

# 其它

编译C/C++语言请参考<[Sublime Text 3直接编译执行C/C++程序](https://blog.csdn.net/shenwanjiang111/article/details/53728941)>
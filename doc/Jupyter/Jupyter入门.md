引言

应该使用哪个 IDE/环境/工具？这是人们在做数据科学项目时最常问的问题之一。可以想到，我们不乏可用的选择——从 R Studio 或 PyCharm 等语言特定的 IDE 到 Sublime Text 或 Atom 等编辑器——选择太多可能会让初学者难以下手。

如果说有什么每个数据科学家都应该使用或必须了解的工具，那非 Jupyter Notebooks 莫属了（之前也被称为 iPython 笔记本）。Jupyter Notebooks 很强大，功能多，可共享，并且提供了在同一环境中执行数据可视化的功能。

Jupyter Notebooks 允许数据科学家创建和共享他们的文档，从代码到全面的报告都可以。它们能帮助数据科学家简化工作流程，实现更高的生产力和更便捷的协作。由于这些以及你将在下面看到的原因，Jupyter Notebooks 成了数据科学家最常用的工具之一。

![jupyter_icon](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Jupyter/images/jupyter_icon.jpg)

在本文中，我们将介绍 Jupyter Notebooks 并深入了解它的功能和优势。

读完本文之后，你就知道你应该为你的机器学习项目使用 Jupyter Notebooks 的原因了。你也会知道为什么人们会认为 Jupyter Notebooks 比该领域内的其它标准工具更好。

准备好学习了吗？现在开始吧！

目录

* Jupyter Notebooks 是什么？
* 如何安装 Jupyter Notebooks？
* 开始上手！
* 使用神奇的功能
* 不只限于 Python——在 Jupyter Notebooks 中使用 R、Julia 和 JavaScript
* Jupyter Notebooks 中的交互式仪表盘——何乐不为？
* 键盘快捷键——节省时间且更有生产力！
* 有用的 Jupyter Notebooks 扩展
* 保存和共享你的笔记本
* JupyterLab——Jupyter Notebooks 的进化
* 最佳实践和技巧

## 1. Jupyter Notebooks 是什么？

Jupyter Notebooks 是一款开源的网络应用，我们可以将其用于创建和共享代码与文档。

其提供了一个环境，你无需离开这个环境，就可以在其中编写你的代码、运行代码、查看输出、可视化数据并查看结果。因此，这是一款可执行端到端的数据科学工作流程的便捷工具，其中包括数据清理、统计建模、构建和训练机器学习模型、可视化数据等等。

当你还处于原型开发阶段时，Jupyter Notebooks 的作用更是引人注目。这是因为你的代码是按独立单元的形式编写的，而且这些单元是独立执行的。这让用户可以测试一个项目中的特定代码块，而无需从项目开始处执行代码。很多其它 IDE 环境（比如 RStudio）也有其它几种方式能做到这一点，但我个人觉得 Jupyter 的单个单元结构是最好的。

正如你将在本文中看到的那样，这些笔记本非常灵活，能为数据科学家提供强大的交互能力和工具。它们甚至允许你运行 Python 之外的其它语言，比如 R、SQL 等。因为它们比单纯的 IDE 平台更具交互性，所以它们被广泛用于以更具教学性的方式展示代码。

## 2. 如何安装 Jupyter Notebooks？

你可能已经猜到了，你首先需要在你的机器上安装 Python。Python 2.7 或 Python 3.3（或更新版本）都可以。

1. Anaconda

对新用户而言，一般的共识是你应该使用 Anaconda 发行版来安装 Python 和 Jupyter Notebooks。

Anaconda 会同时安装这两个工具，并且还包含相当多数据科学和机器学习社区常用的软件包。你可以在这里下载最新版的[Anaconda](https://www.anaconda.com/download)：

2.pip 方法

如果你因为某些原因不愿意使用 Anaconda，那么你需要确保你的机器运行着最新版的 pip。该怎么做呢？如果你已经安装了 Python，那么就已经有 pip 了。你可以使用以下代码升级到最新版的 pip：

    #Linux and OSX pip install -U pip setuptools
    #Windows python -m pip install -U pip setuptools

pip 安装好之后，继续安装 Jupyter：

    #For Python2
    pip install jupyter
    #For Python3
    pip3 install jupyter

你可以在这里查看官方的 Jupyter 安装文档：https://jupyter.readthedocs.io/en/latest/install.html

## 3. 开始上手！

现在你已经知道这些笔记本是什么以及如何将其安装到你的机器上了。现在开始使用吧！

要运行你的 Jupyter Notebooks，只需在命令行输入以下命令即可！

jupyter notebook

完成之后，Jupyter Notebooks 就会在你的默认网络浏览器打开，地址是：

    http://localhost:8888/tree

在某些情况下，它可能不会自动打开。而是会在终端/命令行生成一个 URL，并带有令牌密钥提示。你需要将包含这个令牌密钥在内的整个 URL 都复制并粘贴到你的浏览器，然后才能打开一个笔记本。

打开笔记本后，你会看到顶部有三个选项卡：Files、Running 和 Clusters。其中，Files 基本上就是列出所有文件，Running 是展示你当前打开的终端和笔记本，Clusters 是由 IPython 并行提供的。

要打开一个新的 Jupyter 笔记本，点击页面右侧的「New」选项。你在这里会看到 4 个需要选择的选项：

* Python 3
* Text File
* Folder
* Terminal

选择 Text File，你会得到一个空面板。你可以添加任何字母、单词和数字。其基本上可以看作是一个文本编辑器（类似于 Ubuntu 的文本编辑器）。你可以在其中选择语言（有很多语言选项），所以你可以在这里编写脚本。你也可以查找和替换该文件中的词。

选择 Folder 选项时，你会创建一个新的文件夹，你可以在其中放入文件，重命名或删除它。各种操作都可以。

Terminal 完全类似于在 Mac 或 Linux 机器上的终端（或 Windows 上的 cmd）。其能在你的网络浏览器内执行一些支持终端会话的工作。在这个终端输入 python，你就可以开始写你的 Python 脚本了！

但在本文中，我们重点关注的是笔记本，所以我们从 New 选项中选择 Python 3。你会看到下面的屏幕：

![new_python3](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Jupyter/images/new_python3.jpg)

然后你可以从导入最常见的 Python 库开始：pandas 和 numpy。在代码上面的菜单中，你有一些操作各个单元的选项：添加、编辑、剪切、向上和向下移动单元、运行单元内的代码、停止代码、保存工作以及重启 kernel。

![import_common_lib](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Jupyter/images/import_common_lib.jpg)

在上图所示的下拉菜单中，你还有 4 个选项：

* Code——不言而喻，就是写代码的地方。
* Markdown——这是写文本的地方。你可以在运行一段代码后添加你的结论、添加注释等。
* Raw NBConvert——这是一个可将你的笔记本转换成另一种格式（比如 HTML）的命令行工具。
* Heading——这是你添加标题的地方，这样你可以将不同的章节分开，让你的笔记本看起来更整齐更清晰。这个现在已经被转换成 Markdown 选项本身了。输入一个「##」之后，后面输入的内容就会被视为一个标题。

使用 Jupyter Notebooks 的神奇功能

Jupyter Notebooks 的开发者已经在其中内置了一些预定义的神奇功能，能让你的生活更轻松，让你的工作更具交互性。你可以运行下面的命令来查看功能列表（注：% 符号通常不需要，因为自动补齐功能通常是开启的）：

    %lsmagic

你会看到列出了很多选择，你甚至可能能认出其中一些！`%clear`、`%autosave`、`%debug` 和 `%mkdir` 等功能你以前肯定见过。现在，神奇的命令可以以两种方式运行：

* 逐行方式
* 逐单元方式

顾名思义，逐行方式是执行单行的命令，而逐单元方式则是执行不止一行的命令，而是执行整个单元中的整个代码块。

在逐行方式中，所有给定的命令必须以 % 字符开头；而在逐单元方式中，所有的命令必须以 %% 开头。我们看看下列示例以便更好地理解：

逐行方式：
    
    %time a = range(10)

逐单元方式：
    
    

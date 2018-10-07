* [1 pandas安装](#1)
* [2 pandas创建对象](#2)
    - [2.1 Series](#2.1)
* [3 重命名文件和文件夹](#3)

`pandas` 对于数据分析的人员来说都是必须熟悉的第三方库，`pandas` 在科学计算上有很大的优势，特别是对于数据分析人员来说，相当的重要。`python`中有了`Numpy` ，但是`Numpy` 还是比较数学化，还需要有一种库能够更加具体的代表数据模型，我们都非常的清楚在数据处理中`EXCEL` 扮演着非常重要的作用，表格的模式是数据模型最好的一种展现形式。

`pandas` 是对表格数据模型在`python`上的模拟，它有简单的像`SQL` 对数据的处理，能够方便的在`python`上实现。

<h1 id="1">1 pandas安装</h1>

`pandas` 在`python`上的安装同样的使用`pip`进行：

    pip install pandas

<h1 id="1">2 pandas创建对象</h1>

`pandas` 有两种数据结构：`Series` 和 `DataFrame` 。

<h2 id="2.1">2.1 Series</h2>

`Series` 像`python`中的数据`list` 一样，每个数据都有自己的索引。从`list`创建 `Series`。

    >>> import pandas as pd
    >>> s1 = pd.Series([100,23,'bugingcode'])
    >>> s1
    0           100
    1            23
    2    bugingcode
    dtype: object
    >>>

在`Series`中添加相应的索引：
    
    >>> import numpy as np
    >>> ts = pd.Series(np.random.randn(365), index=np.arange(1,366))
    >>> ts

在`index`中设置索引值是一个从`1`到`366`的值。

`Series` 的数据结构最像的是`python`中的字典，从字典中创建`Series`：

    sd = {'xiaoming':14,'tom':15,'john':13}
    s4 = pd.Series(sd)

这时候可以看到`Series` 已经是自带索引`index`。

`pandas` 本身跟 `python`的另外一个第三方库`Matplotlib`有很多的连接，`Matplotlib` 一个最经常用到的是用来展示数据的，如果还对`Matplotlib` 不了解的话，后面的章节会进行介绍，现在先拿过来直接用下，如果还没有安装的话，一样的用pip命令安装 `pip install Matplotlib` ， 展示如下数据：

    import pandas as pd
    import numpy as np
    import matplotlib.pyplot as plt

    ts = pd.Series(np.random.randn(365), index=np.arange(1,366))
    ts.plot()
    plt.show()
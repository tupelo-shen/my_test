本文旨在为大家介绍两种修改 Sublime 注释字体颜色的方式：

1. Sublime 自带主题注释颜色的修改。
2. 通过 Package Control 安装的主题注释颜色的修改。

## Sublime 自带主题注释颜色的修改

> 注：这里以使用默认主题 Monokai 为例进行说明。

1. `Preferences -> Package Control -> Install Package`，或者使用快捷键：`ctrl + shift + p`，调出命令面板，然后手动输入 `Package Control: Install Package`

2. 搜索 `Package Resource Viewer` 并安装

3. 快捷键 `ctrl + shift + p` 调出命令面板，搜索 `Package Resource Viewer`。然后，选中 `Open Resource` 项。

4. 搜索“Color Scheme -Default”

5. 搜索”Monokai.tmTheme”,并打开这个XML文件。因为修改的是Monkai的配色方案，所以在Preferences设置的配色方案也要选择这个。否则修改没有效果。

6. 搜索关键字“comment”，找到#75715E,按照自己的喜欢修改颜色，我修改的是绿色，就改为#0AA344。

## 通过 Package Control 安装的主题注释颜色的修改

> 以MarkdownEditor为例

因为，也需要安装 Package Resource Viewer，因此前三步与上述相同，不再赘述。

1. 同上
2. 同上
3. 同上
4. 搜索`Markdown Editing`，选择之。在弹出的内容中，选择自己使用的颜色主题。之后的步骤同上面相同，省略之。

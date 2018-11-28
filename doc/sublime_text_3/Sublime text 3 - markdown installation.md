# 1. Sublime text 3 安装

略

# 2. 安装markdownediting 和markdownpreview

MarkdownPreview不仅支持在浏览器中预览markdown文件，还可以导出html代码。下面我们通过Package Control安装。


# 3. 使用方法

## 3.1 markdownpreview 浏览文件

### 方法1：
使用ctrl+shift+P，在“github”，“gitlap”和“markdown”中选择一个打开浏览即可。

### 方法2：
使用快捷键。默认情况下，markdownpreview没有快捷键。但是可以添加。菜单工具栏中选择“Preferences”->"Key Binding"后，添加下面这条语句即可：

    { "keys": ["alt+m"], "command": "markdown_preview", "args": {"target": "browser", "parser":"markdown"}}

这里： 

* *"alt+m"*：可设置为自己喜欢的按键。 

* *"parser"："markdown"*也可设置为*"parser":"github"*，改为使用Github在线API解析markdown。

# 4 中文字体问题
## 4.1 改善中文字体显示问题

`sublime text3`默认中文显示，字体非常怪异。需要设置一下，才能看上去比较正常。只需在`preference->setting->settings-user`中添加下面的一行代码即可。
    
    "font_options": ["gdi"]

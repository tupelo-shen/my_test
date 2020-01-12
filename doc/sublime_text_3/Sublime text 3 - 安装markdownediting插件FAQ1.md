1. 问题：
    
    `Sublime Text3 `安装 `markdownediting`插件,报错 `Error loading syntax file "Packages/Markdown/Markdown.tmLanguage"`:

2. 解决方法：

参考 [github README.MD](https://github.com/SublimeText-Markdown/MarkdownEditing/blob/master/README.md#error-loading-syntax-file)

清理掉`C:\Users\Administrator\AppData\Roaming\Sublime Text 3\Local`目录下的`Session.sublime_session`文件，重新打开sublime编辑器就好了。

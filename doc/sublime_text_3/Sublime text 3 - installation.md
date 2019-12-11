1. 下载

    [官网](https://www.sublimetext.com/3)下载最新的源代码或者你需要的版本。

2. 解压

    $ tar -xjvf file.tar.bz2

3. 打开sublime_text_3程序,测试压缩包程序是否正常。也可以在终端输入命令打开程序。

    $ cd sublime_text_3
    $ ./sublime_text_3

4. 生成桌面快捷方式

    * 解压目录下/sublime_text_3/sublime_text.desktop文件拷贝到/usr/share/applications/目录下就可以

            其内容如下：

            [Desktop Entry]
            Version=1.0
            Type=Application
            Name=Sublime Text
            GenericName=Text Editor
            Comment=Sophisticated text editor for code, markup and prose
            Exec=/opt/sublime_text/sublime_text %F
            Terminal=false
            MimeType=text/plain;
            Icon=sublime-text
            Categories=TextEditor;Development;
            StartupNotify=true
            Actions=Window;Document;

            [Desktop Action Window]
            Name=New Window
            Exec=/opt/sublime_text/sublime_text -n
            OnlyShowIn=Unity;

            [Desktop Action Document]
            Name=New File
            Exec=/opt/sublime_text/sublime_text --command new_file
            OnlyShowIn=Unity;

    * 将其修改为(主要是`/opt/sublime_text/`->`/home/qemu4/software/sublime-text_3/`)：

            [Desktop Entry]
            Version=1.0
            Type=Application
            Name=Sublime Text
            GenericName=Text Editor
            Comment=Sophisticated text editor for code, markup and prose
            Exec=/home/qemu4/software/sublime-text_3/sublime_text %F
            Terminal=false
            MimeType=text/plain;
            Icon=/home/qemu4/software/sublime-text_3/Icon/128x128/sublime-text.png
            Categories=TextEditor;Development;
            StartupNotify=true
            Actions=Window;Document;

            [Desktop Action Window]
            Name=New Window
            Exec=/home/qemu4/software/sublime-text_3/sublime_text -n
            OnlyShowIn=Unity;

            [Desktop Action Document]
            Name=New File
            Exec=/home/qemu4/software/sublime-text_3/sublime_text --command new_file
            OnlyShowIn=Unity;

    * 将其拷贝到~/Desktop/目录下就可以了。


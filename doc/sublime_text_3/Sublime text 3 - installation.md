1. 下载

    [官网](https://www.sublimetext.com/3)下载最新的源代码或者你需要的版本。

2. 解压到`/opt`目录下：

        $ tar -xjvf <压缩文件名称>.tar.bz2  -C /opt

3. 解压后，会出现`sublime_text_3`目录，进入其中，调用`./sublime_text`命令，或者双击sublime_text图标都可以，打开sublime。

    $ cd sublime_text_3
    $ ./sublime_text

    这种方式，每次都得进入相应的目录下，根据上面的命令打开sublime，非常不方便。

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

5. 生成桌面方式二

    将sublime_text_3文件夹改名为sublime_text。为啥要把这个文件夹安排到这个路径并且还要改名呢，因为该文件夹下的sublime_text.desktop文件中，路径都是：

        Exec=/opt/sublime_text/sublime_text

    将sublime_text.desktop文件复制到/usr/share/applications。这里需要管理员权限，记得加sudo：

        sudo cp sublime_text.desktop /usr/share/applications/

    OK，完成这一步之后，可以在Applications中搜到 sublime。

6. 添加为默认打开方式

    右键单击该文件,然后点击属性,打开属性面板。然后进入open with的选项,选择应用后,点击 set as default。

7. 正常安装的sublime是可以在终端输入subl进入sublime的，但是这种安装方法不行。那我们就创建软链接咯。

    ln -s /opt/sublime_text/sublime_text /usr/local/bin/subl

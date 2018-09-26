* [1 Git常用命令](#1)
* [2 从远程仓库更新代码到本地仓库](#2)
* [3 重命名文件和文件夹](#3)

<h1 id="1">1 Git常用命令</h1>

建立工作区的命令：

```
clone       克隆仓库到新目录下
init        建立一个空Git仓库或重新初始化已经存在的仓库
```

当前改变的操作命令

```
add         添加文件内容到索引
mv          移动或重命名文件，目录，或符号链接
reset       复位当前HEAD版本到指定的一个状态
rm          从工作树和索引中移除文件
```

历史和状态的检查命令

```
bisect      使用二分查找法去发现引入了bug的commit操作
grep        打印匹配格式的行
log         显示commit信息
show        显示操作对象的多种信息
status      显示工作树状态
```

管理你的操作历史

```
branch          列出，创建或删除分支
checkout        切换分支或者恢复工作树文件
commit          提交、记录改动到仓库中
diff            比较commit之间或者commit和工作树之间的变化
merge           合并
rebase          取最新的origin head版本，然后把你的提交合并，如果冲突，解决冲突，重新提交
tag             创建，列出，删除或验证一个用GPG标记的tag对象
```

协作命令

```
fetch           从别的仓库下载对象和refs
pull            从另一个仓库或者本地分支，抓取或者合并
push            更新相关对象的远程引用
```

<h1 id="2">2 从远程仓库更新代码到本地仓库</h1>

首先，查看远程仓库方法：

    $ git remote -v

    origin  git@github.com:tupelo-shen/ATC.git (fetch)
    origin  git@github.com:tupelo-shen/ATC.git (push)

其次，这儿介绍远程仓库更新代码到本地仓库的两种方法：

<h2 id="2.1">2.1 方法1</h2>

远程仓库为origin master，本地仓库分支为master，那么：

* 从远程获取最新版本到本地

        $ git fetch origin master

这句的意思是：从远程的origin仓库的master分支下载代码到本地的origin master

* 比较本地的仓库和远程参考的区别

        $ git log -p master.. origin/master

因为我的本地仓库和远程仓库代码相同所以没有其他任何信息。

* 把远程下载下来的代码合并到本地仓库，远程的和本地的合并

        $ git merge origin/master

<h2 id="2.2">2.2 方法2</h2>

远程仓库为origin master，本地仓库分支为`atc_develop001`(本地分支名称)，那么：

* 从远程获取最新版本到本地

        $ git fetch origin master:temp
        Enter passphrase for key '/c/Users/30032183/.ssh/id_rsa':
        From github.com:tupelo-shen/ATC
            * [new branch]      master     -> temp

上面第一句，把远程分支更新到临时分支temp的命令；第二句提示输入密码，输入后enter就可以了。

* 比较本地的仓库和远程参考的区别

        $ git diff temp diff --git

....................(此处应为不同信息的提示）

* 合并temp分支到atc_develop001分支

        $ git merge temp

合并提示信息

合并的时候可能会出现冲突，有时间了再把如何处理冲突写一篇博客补充上。

* 如果不想要temp分支了，可以删除此分支

        $ git branch -d temp

总结：方法2更好理解，更安全，对于pull也可以更新代码到本地，相当于fetch+merge，多人写作的话不够安全。


<h1 id="3">3 重命名文件和文件夹</h1>

    git mv -f oldfolder newfolder

    git add -u newfolder

-u 选项会更新已经追踪的文件和文件夹。

    $ git commit -m "changed the foldername whaddup"

    $ git mv foldername tempname && git mv tempname folderName

在大小写不敏感的系统中，如windows，重命名文件的大小写,使用临时文件名

    $ git mv -n foldername folderName
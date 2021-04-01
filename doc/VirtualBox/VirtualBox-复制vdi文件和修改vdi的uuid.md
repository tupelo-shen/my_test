## 1 复制vdi文件：VBoxManage clonehd

因为VirtualBox不允许注册重复的uuid，而每个vdi文件都有一个唯一的uuid。所以要想拷贝一份vdi文件再次在VBOX中注册，简单的复制是不行的。此时我们需要用到命令VBoxManage clonehd，这个命令在克隆vdi文件时会给新文件设置一个uuid【注：要运行这个命令，先打开命令提示行，并进入到virtual box的安装目录】。事例如下：

```shell
D:\Program Files\Oracle\VirtualBox>VBoxManage clonehd "E:\VirtualBox\Ubuntu 12.04\Ubuntu 12.04.vdi" "E:\VirtualBox\Ubuntu 12.04\Ubuntu_12.04.vdi"
```

结果如下：

```shell
0%...10%...20%...30%...40%...50%...60%...70%...80%...90%...100%
Clone hard disk created in format 'VDI'. UUID: cf70d484-a3f0-4a87-953b-d1c8ec602c59
```

## 2 修改vdi的uuid：VBoxManage internalcommands sethduuid

当出现两个vdi的uuid相同的错误时，可以使用命令VBoxManage internalcommands sethduuid修改vdi文件的uuid,事例如下：

```shell
D:\Program Files\Oracle\VirtualBox>VBoxManage internalcommands sethduuid E:\VirtualBox\Win7_Ultimate_SP1_1\Win7_Ultimate_SP1.vdi
```

结果如下：

```shell
UUID changed to: 3b5f507c-dda7-409c-a2ef-ee075435558d
```

但是当路径中有空格时，要对路径加上双引号，如下：

```shell
D:\Program Files\Oracle\VirtualBox>VBoxManage internalcommands sethduuid "E:\VirtualBox\Ubuntu 12.04.vdi"
```

结果如下：

```shell
UUID changed to: 04094fb4-0a42-413a-a25d-9d02d804bd2a
```

> 使用VBoxManage命令时，需要先在命令行中切换到VirtualBox的安装目录下。
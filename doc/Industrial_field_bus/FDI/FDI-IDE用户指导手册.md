<h1 id="0">0 目录</h1>

* [3 FDI包](#3)
    - [3.1 第1步：配置EDDL编译器](#3.1)
        + [3.1.1 基础](#3.1.1)
        + [3.1.2 FF编译器](#3.1.2)
        + [3.1.3 HART编译器](#3.1.3)
        + [3.1.4 PROFIBUS/PROFINET编译器](#3.1.4)
    - [3.2 第2步：创建工程](#3.2)
        + [3.2.1 工程向导](#3.2.1)
    - [3.6 第6步：生成包](#3.6)


***

<h1 id="3">3 引言</h1>

<h2 id="3.1">3.1 第一步：配置EDDL编译器</h2>

<h3 id="3.1.1">3.1.1 基础</h3>

EDDL code representing the device functions and parameters must be tokenized before it can be used in the context of an FDI Package. Tokenizing is a kind of compilation of EDDL code, linking libraries and so on into a binary format. FDI defines a unified binary format for all EDDs used in FDI Packages. The FDI Package IDE and its Tokenizers are capable to generate FDI Binary Format EDDs as well as legacy binary format EDDs.

For each communication protocol, a separate tokenizer is required, which has protocol specific knowledge that is required to create the final binary version of the EDD from EDDL sources.

A tokenizer is a software product in a defined folder structure. During the lifecycle of a communication protocol, updates of this tokenizer may be required for bug fixes and feature extensions.

> <font color="red">Warning</font>
>
> <font color="red">Perform EDD Tokenizer base configurations / or changes of the configuration before the creating of development projects since changes are not reflected into already existing Ant‐Script files used for tokenizer invocation. Apply and OK buttons are only enabled if the information entered in a dialog is consistent and correct.</font>

<h3 id="3.1.2">3.1.2 FF编译器</h3>

<h3 id="3.1.3">3.1.3 HART编译器</h3>

<img id="Pic_3_1_3_1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_IDE_USER_GUIDE_3_1_3_1.PNG">

HART编译器的配置界面

<h4 id="3.1.3.1">3.1.3.1 HART编译器</h4>

Eclipse菜单中，选择`Window`->`Preferences`就会出现上面的配置界面。按照上图，打开HART编译器的配置界面。

1. 标准DD库

    HART编译器需要包含标准库文件进行编译，可以通过查看目录`C:\HCF\DDL\library\000000`是否存在，判断标准库是否已经安装。

2. 通用

    * **Tokenizer**：选择编译器执行文件，比如`C:\HCF\DDL\bin\DdtA.exe`
    * **Release Path**：编译结果存放位置，比如`C:\HCF\DDL\library`

3. 选择自定义词典

    **Alternative Standard Dictionary File**：选择词典文件，默认位置是`C:\HCF\DDL\library\standard.dc8`，默认词典来源于`FieldComm Group`。通常这个区域为空。

4. Include路径

    * **Add Include…**：标准头文件，目录`C:\HCF\DDL\dev`应该被包含
    * **Remove Selection**：删除某个标准头文件包含
    * **Remove All**：删除所有标准头文件包含

5. 可选命令行配置

    * 可选参数：命令行参数列表在编译器手册或说明文档中描述。通常，这个区域应该为空。

6. 按下`Apply`按钮，保存更改
7. 按下`OK`按钮，关闭配置窗口

<h3 id="3.1.4">3.1.4 PROFIBUS/PROFINET编译器</h3>

<h2 id="3.2">3.2 第2步：创建工程</h2>

<h3 id="3.2.1">3.2.1 工程向导</h3>

1. 选择`File`->`New`->`Project`。一个`New Project`的对话框出现，会显示几个向导选项和工程类型，可以用它们新建工程。
2. 选择FDI类型，有下面几个工程选项：

    <img id="Pic_3_2_1_1" src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/Industrial_field_bus/FDI/images/FDI_IDE_USER_GUIDE_3_2_1_1.PNG">

    工程类型

    * **Existing FDI/UIP Project**: 选择已有的开发工程
    * **New FDI Package Project**: 新建一个FDI包开发工程
    * **New UIP Packaging Project**：新建一个UIP包开发工程

    > UIP一般使用微软的`.NET`技术进行开发，在Visual Studio完成。这儿主要负责将UIP和创建的目录文件进行打包。


3. 选择`New FDI Package Project`，启动填写信息向导对话框。强制信息使用`*`进行标记。

    * **FDI Project Name (\*)**：填写工程名称，比如，`MyFirst_FDI_DevicePackage`。
    * **FDI Project Folder (\*)**:保存目录，保存工程所有文件，比如，`C:\Users\tester\Desktop`。
        - 可以使用`Window` ‐> `Preferences` ‐> `Node FDI`修改目录。
    * **Package Type \[prefilled\] (\*)**:选择创建FDI包的类型，根据需求选择。
        - Device： 描述设备类型（一般是通讯终端）。该示例使用此项。
        - UIP：用户终端设备上的用户界面程序。
        - Communication：描述通信设备类型（与FDT框架下的通信DTM类似的产物）。
        - Profile：自定义类型。
    * **Supported FDI version \[prefilled\]**:支持的FDI版本。
    * **Manufacturer Information**:制造商信息。可以使用`Window` ‐> `Preferences` ‐> `Node FDI`修改。
        - Name(*)： 公司名称
        - Address：公司地址
        - Phone:
        - Email:
        - URL:
        - Image:只支持`.PNG`格式，分辨率为`256 × 256`、`64 × 64`、`32 × 32` 或 `16 × 16`；手持设备的话，大小只能是`16 × 16`。
4. 设备类型接口信息：在这一步提供FDI包中描述的main/first设备类型（其它的设备需要后面添加，可以查看`‐> see Change Project Details`）。强制信息使用`*`进行标记。
    * **EDDL file (*)**:添加主要的EDDL源代码文件（后缀为\*.ddl)
    * **Device name (*)**:
    * **Classification (*)**:
    * **Interface (*)**:
    * **Protocol Version (*)**
    * **Manufacturer Id (*)**:
    * **Device model (*)**:
    * **Communication Role (*)**
    * **List of communication profile support files (*)**
    * **Product URI (for FDI Communication Packages only)**

<h2 id="3.5">3.5 第5步：编译EDD</h2>

<h3 id="3.5.1">3.5.1 基础</h3>


<h2 id="3.6">3.6 第6步：生成包</h2>

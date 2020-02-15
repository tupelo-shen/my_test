## DD的开发步骤  

1. 编辑DDL文件（设备描述文件）  
2. 用Tokenizer（即DDT8.exe）这个编译器将DDL文件编译为二进制文件（.fm8等）和符号文件（.sym），这些文件供FieldMate和SDC625使用。
  
编译的前提是DDL文件所在目录还要包括“Methods.h”、“Macros.h”、“Std.sy8”、“Devices.cfg”几个文件，它们存在于“C:\HCF\DDL\dev”目录中。前三个是共通定义，最后一个（Devices.cfg）是设备的配置文件，编译器靠这个文件判断输出应该是哪一个二进制DD文件。准备好这些文件后执行：  
    
    ```shell
    C:\HCF\DDL\bin\DDT8.exe Sample1_r1.ddl
    ```

然后可在Devices.cfg文件中HCF定义的那个标号中找到编译的输出，有3个，分别是XXXX.fm8、XXXX.sym、XXXX.im8。

二进制文件后缀名规则：

> .fm5 ： HART DD-IDE v5  
> .fm6 ： HART DD-IDE v6  
> .fm8 ： HART DD-IDE v8  
> .ffo ： 旧的FF（Tranditional）  
> .ff5 ： 新的FF（Enhanced）


3. 再经过链接器fclink.exe将.fm8链接为手持终端自定义文件（.hdd和.hhd），用于475等手操器。  
        
        ```shell
        fclink.exe -x "0000f9 00f9 01 01"
        ```


这样便可以在当前目录下生成“F9F90101.hdd”和“F9F90101.hhd”两个文件，这两个文件用于配置手操器。  

4. FCG提供了HART的Simulator，即XMTR-DD+SDC625，使用方法如下：  

        ```shell
        C:\HCF\DDL\bin\XmtrDD.exe
        (在XmtrDD的图形界面中选择某个设备的DD，点击select按钮)
        "C:\Program Files (x86)\SDC\SDC625.exe" -S
        ```

注意，SDC625的运行在Win10下有些问题，需要先在SDC625.exe右键菜单中选择“适配兼容模式”。  

以上这些命令行操作也可在DD-IDE中完成。  
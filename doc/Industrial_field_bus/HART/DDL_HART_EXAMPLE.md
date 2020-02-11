# DD是什么？

DD是<font color="blue">`一个现场设备的数据和操作过程的格式化描述，包括命令、菜单和显示格式等`</font>。它准确地描述了通过HART通信可以对设备做什么操作。使用文本格式编写，但是为了效率，一般转化成编码后的格式使用。

HART使用<font color="blue">`通用命令`</font>访问通用参数，比如设备制造商、设备类型、Tag和描述，以及读取主变量和最多3个其它过程或派生变量，这些通用命令是设备必须实现的。HART还有一些<font color="blue">`常用命令`</font>，用于访问那些常用但不是通用的功能，比如，改变模拟信号范围或读取更多的变量。

但是HART还允许每个设备制造商可以实现一些<font color="blue">`特定于设备的命令`</font>，访问设备中独有的功能和数据。使用这些命令编写主机应用程序是很困难的，而DD就可以解决这个问题，它可以提供关键的信息。

# DD中有什么？

DD主要包括：<font color="blue">`变量-VARIABLE、命令-COMMAND、方法METHOD和菜单-MENU。`</font>

对于每个`VARIABLE`，DD需要指定数据类型（如：整数、浮点数、字母数字类型枚举）、如何显示、显示的名称、相关单位、帮助文本（描述变量的意义或如何使用它）。

对于每个`COMMAND`，指定命令和应答的数据结构，以及可能的应答的状态位。

`METHOD`描述了操作过程，用户可以根据指导进行操作，比如重新校正设备。

DD还定义了`MENU`结构，主机可以用它为用户提供查找变量和方法的菜单。

# 一个简单的DD示例

一个简单的DDL示例，包括变量的描述、获取该变量的MENU及写入变量新值的命令COMMAND。

方括号[xxx]中的短语是对标准常用短语词典的引用，该词典是DD计划的一部分，并提供了几种语言的现成翻译。

花括号`{}`和分号`;`借鉴与C语言，`/*`和`*/`进行注释。

    VARIABLE low_flow_cutoff
       {
       LABEL "Low flow cutoff";
       HELP "Low Flow Cutoff: the value below which the process variable
         will indicate zero, to prevent noise or a small zero error
         being interpreted as a real flow rate.";
       TYPE FLOAT
          {
          DISPLAY_FORMAT "6.4f";   /* ##.#### */
          }
       CONSTANT_UNIT "%";
       HANDLING READ & WRITE;
       }

    MENU configure_input
       {
       LABEL "Configure input";
       ITEMS
          {
          flow_units,          /* variable */
          rerange,             /* edit-display */
          low_flow_cutoff,     /* variable */
          flow_tube_config,    /* menu */
          }
       }

    COMMAND write_low_flow_cutoff
       {
       NUMBER 133;
       OPERATION WRITE;
       TRANSACTION
          {
          REQUEST
             {
             low_flow_cutoff
             }
          REPLY
             {
             response_code,
             device_status,
             low_flow_cutoff
             }
          }
       RESPONSE_CODES
          {
          0, SUCCESS,           [no_command_specific_errors];
          3, DATA_ENTRY_ERROR,  [passed_parameter_too_large];
          4, DATA_ENTRY_ERROR,  [passed_parameter_too_small];
          5, MISC_ERROR,        [too_few_data_bytes_received];
          7, MODE_ERROR         [in_write_protect_mode];
          }
       }

1. COMMAND0


| CMD   |          | 0 | 1-2   | 3  | 4 - 6| 7   | 7      | 8    | 9-11     | 12 | 13  | 14-15 | 16 | 17-18 | 19-20 | 21 |
| ----- | -------- | - | ----- | -- | ----- | --- | ------ | ---- | -------- | -- | --- | ----- | -- | ----- | ----- | -- |  
| 0000  | 24 00 00 |fe | 37 1a | 01 | 07 01 01 |00001| 000    | 00   | 00 00 01 | 05 |01 | 00 01 | 00 | 00 37 | 00 37 | 01 |
|       |          |254| 0x371A| 1  | 7.1.1 | 设备  | 软件  |高5位|低5位   | -    | -        | s->m |   |       |    |       |       |    |
|       |          |254| AXG4A |前序|版本号|电子 |物理信号|标志位| 设备ID   | 前序 |   |       |    |  55   |  55   | DP |

140. COMAND140

| CMD   | LEN | ADDR  | PARAM_ID | CONTENTS  |
| ----  | --  | ----- | ----     | ---- ---- |
| 0140  | 08  | 00 00 | 092e     | c2c7 ff7d |
| 读浮点数|数据长度|地址|参数ID|-99.999|

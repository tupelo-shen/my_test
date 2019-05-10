#include <iostream>
#include <boost/asio.hpp>
#include "SIM_SerialPort.h"
#include "COM_CrcCalculation.h"

using namespace std;

int main()
{
    unsigned char test_data[4] = {0x00,0x80,0xED,0x44};

    printf("test_data[0] of address = %d\n", (unsigned int)&test_data[0]);
    printf("test_data[1] of address = %d\n", (unsigned int)&test_data[1]);
    printf("test_data[2] of address = %d\n", (unsigned int)&test_data[2]);
    printf("test_data[3] of address = %d\n", (unsigned int)&test_data[3]);
    printf("test_data[0] = %2x\n", test_data[0]);
    printf("test_data[1] = %2x\n", test_data[1]);
    printf("test_data[2] = %2x\n", test_data[2]);
    printf("test_data[3] = %2x\n", test_data[3]);

    float * ptr = (float* )test_data;
    printf("float ptr = %f\n", *ptr);
    return 0;
}


[File]
    ...
[Device]
    VendCode = 65535;
    VendName = "Widget-Works, Inc.";
    ProdType = 43;
    ProdTypeStr = “Generic Device”;
    ProdCode = 42;
    MajRev = 1;                         $ Device Major Revision
    MinRev = 1;                         $ Device Minor Revision
    ProdName = "Smart-Widget";
    Catalog = “1499-DVG”;
    Icon = “example.ico”;
    DeviceStatusAssembly = 1;
    DeviceStatusAssembly2 = 2;
[ParamClass]
    ...
[Assembly]
    Assem1 =
        “Device Status, bits 4-7”,      $ name
        ,                               $ no path
        ,                               $ size - unspecified
        ,                               $ reserved
        ,,                              $ reserved
        4, Param1;                      $ describes bits 4-7
    Assem2 =
        “Device Status, bits 12-15”,    $ name
        ,                               $ no path
        ,                               $ size - unspecified
        ,                               $ reserved
        ,,                              $ reserved
        4, Param2;                      $ describes bits 12-15
    ...
[Params]
    Param1 =
        0,                              $ reserved, shall equal 0
        ,,                              $ Link Path Size, Link Path
        0x0030,                         $ Descriptor
        0xC6,                           $ Data Type
        1,                              $ Data Size in bytes
        "Device Status Values, bits 4-7", $ name
        "",                             $ units
        "",                             $ help string
        ,,0,                            $ min, max, default values
        ,,,,                            $ mult, div, base, offset scaling
        ,,,,                            $ mult, div, base, offset links
        ;                               $ decimal places
    Enum1 = $ values 0-7 reflect the open definitions in table 5A-2.4 combined with vendor specific values 10 - 13
        0,"Self-Testing or Unknown",
        1,"Firmware Update in Progress",
        2,"At least one faulted I/O connection",
        3,"No I/O connections established",
        4,"Non-Volatile Configuration bad",
        5,"Major Fault – either bit 10 or bit 11 is true (1)",
        6,"At least one I/O connection in run mode",
        7,"At least one I/O connection established, all in idle mode",
        10,"Stored Configuration/Program Invalid",
        11,"Backplane not found",
        12,"All I/O connections in run mode",
        13,"All I/O connections in idle mode";
    Param2 =
        0,                              $ reserved, shall equal 0
        ,,                              $ Link Path Size, Link Path
        0x0030,                         $ Descriptor
        0xC6,                           $ Data Type
        1,                              $ Data Size in bytes
        "Device Status Values, bits 12-15", $ name
        "",                             $ units
        "",                             $ help string
        ,,0,                            $ min, max, default values
        ,,,,                            $ mult, div, base, offset scaling
        ,,,,                            $ mult, div, base, offset links
        ;                               $ decimal places
    Enum2 = $ values 0-3 reflect vendor specific definitions
        0,"Unknown – not supported",
        1,"Mode Transitioning",
        2,"Test Mode",
        3,"Keyswitch in remote mode"; ...
[Groups]
    ...

[ParamClass]
    CfgAssembly = 0;                    $ 为了使用CfgAssemblyExa关键字，必须设为0
    CfgAssemblyExa = Assem1;            $ EDS集合，列出了设备中所有配置项

[Params]
    Param1 =
        0,                                  $ 保留字段，应为0
        6,"20 0F 24 01 30 01",              $ 路径大小，路径
        0x0000,                             $ 描述符
        0xC6,                               $ 数据类型 - USINT
        1,                                  $ 数据大小，以字节为单位
        "Temperature format",               $ 参数名称
        "",                                 $ 没有单位
        "Select temperature format for the device", $ 帮助文本
        0,2,0,                              $ 最小、最大、默认值
        ,,,,                                $ mult, div, base, offset scaling
        ,,,,                                $ mult, div, base, offset links
        ;                                   $ decimal places
    Enum1 =
        0, “Fahrenheit:, 1, “Celsius”, 2, “Kelvin”;
    
    Param2 =
        0,                                  $ reserved, shall equal 0
        6,"20 68 24 01 30 01",              $ Path Size, Path
        0x0000,                             $ Descriptor
        0xC7,                               $ Data Type - UINT
        2,                                  $ Data Size in bytes
        "Desired Device Temperature",       $ name
        "",                                 $ no units
        "Select desired device temperature",$ help string
        ,,0,                                $ min, max, default data values
        ,,,,                                $ mult, div, base, offset scaling
        ,,,,                                $ mult, div, base, offset links
        ;                                   $ decimal places

    Param48 =
        0,                                  $ reserved, shall equal 0
        6,"20 0F 24 30 30 01",              $ Path Size, Path
        0x0000,                             $ Descriptor
        0xC7,                               $ Data Type - UINT
        2,                                  $ Data Size in bytes
        "Actual Device Temperature",        $ name
        ,                                   $ units
        "Device internal temperature",      $ help string
        ,,0x0055,                           $ min, max, default data values
        ,,,,                                $ mult, div, base, offset scaling
        ,,,,                                $ mult, div, base, offset links
        ;                                   $ decimal places

[Assembly]
    Assem1 =
        "Configuration",                    $ 集合名称
        ,                                   $ 没有路径，各个成员中指定路径
        ,                                   $ 大小 - 未指定
        ,                                   $ 描述符
        ,,                                  $ 保留
        ,Param1,                            $ 成员大小，成员引用
        ,Param2;                            $ 成员大小，成员引用


[File]
    ...
[Device]
    ...
[ParamClass]
    ...
[Params]
    ...
[Groups]
    Group1 = "Setup", 3, 1, Variant2, Proxy1;   $ group 1 named Setup consists
                                                $ of Param1, Variant2 & ProxyParam1
    Group2 = "Monitor", 3, 2, 3, Proxy4;        $ group 2 named Monitor consists of
                                                $ Param2, Param3 and ProxyParam4
    Group3 = "Maintenance", 2, 1, 3;            $ group 3 named Maintenance
                                                $ consists of Param1 and Param3

[Params] 
    Param1 =
        0,                                      $ first field shall equal 0 
        6, "20 0F 24 01 30 01",                 $ path size, path 
        0x0000,                                 $ descriptor 
        0xD2,                                   $ data type : 16-bit WORD 
        2,                                      $ data size in bytes 
        "Idle state",                           $ name 
        "",                                     $ units 
        "User Manual p48",                      $ help string
        0, 2, 1,                                $ min, max, default data values
        0, 0, 0, 0,                             $ mult, dev, base, offset scaling not used 
        0, 0, 0, 0,                             $ mult, dev, base, offset link not used 
        0;                                      $ decimal places not used 
    Param2 = 
        0, 
        6, "20 0F 24 02 30 01",                 $ path size, path 
        0x0000, 
        0xD2, 
        2, 
        "Fault state", 
        "", 
        "User Manual p49", 
        0, 2, 2, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0; 

[Assembly] 
    Revision = 2; 
    Assem5 = 
        "configuration",                        $ 集合名称
        "20 04 24 05 30 03",                    $ 链接路径
        1,                                      $ 大小 - 未指定
        ,                                       $ 描述符
        ,,                                      $ 保留
        4, Param1,                              $ 成员大小，成员引用
        3, Param2,                              $ 成员大小，成员引用
        1, ;                                    $ 成员大小，成员引用 

[Params]
    Param1 = 
        0,                                      $ first field shall equal 0
        ,,                                      $ path size, path
        0x0002,                                 $ descriptor
        0xC6,                                   $ data type: Unsigned short integer
        1,                                      $ data size in bytes
        "Channel 0 Range Selection Bits",       $ name
        "",                                     $ units
        "",                                     $ help string
        0,3,0,                                  $ min, max, default data values
        0,0,0,0,                                $ mult, dev, base, offset scaling not used
        0,0,0,0,                                $ mult, dev, base, offset link not used
        0;                                      $ decimal places
    Enum1 = 0, “Off”, 1, “4-20mA”, 2, “0-10V”, 3, “-10 to +10V”
    $ Param2, Param3 & Param4 are similar for Channel 2, Channel 3 & Channel 4

[Assembly]
    Assem1 = “Configuration Data”, "20 7D 24 00 30 0C",,,,,
    1,Param1:0,                                 $ Channel 0 full range bit
    1,Param2:0,                                 $ Channel 1 full range bit
    1,Param3:0,                                 $ Channel 2 full range bit
    1,Param4:0,                                 $ Channel 3 full range bit
    4,,                                         $ Not used – must be 0 [fill bits]
    1,Param1:1,                                 $ Channel 0 selection bit
    1,Param2:1,                                 $ Channel 1 selection bit
    1,Param3:1,                                 $ Channel 2 selection bit
    1,Param4:1,                                 $ Channel 3 selection bit
    4,;                                         $ Not used – must be 0

[Params]
    Param1 =
        0,                                      $ first field shall equal 0
        ,,                                      $ path size, path
        0x0002,                                 $ descriptor
        0xC6                                    $ data type : Unsigned short integer
        1,                                      $ data size in bytes
        "Output Channel 0 Configuration",       $ name
        "",                                     $ units
        "",                                     $ help string
        0,2,0,                                  $ min, max, default data values
        0,0,0,0,                                $ mult, dev, base, offset scaling not used
        0,0,0,0,                                $ mult, dev, base, offset link not used
        0;                                      $ decimal places not used
    Enum1 = 0, “Channel not configured”, 1, “4-20mA”, 2, “+-10V”;
    Param2 =
        0,                                      $ first field shall equal 0
        ,,                                      $ path size, path
        0x0044,                                 $ descriptor (scaling and decimal precision)
        0xC7,                                   $ data type : 16 bit unsigned integer
        2,                                      $ data size in bytes
        "Output Channel 0 4-20mA ",             $ name
        "mA",                                   $ units
        "",                                     $ help string
        0,30840,0,                              $ min, max, default data values
        1,1927,100,7712,                        $ mult, dev, base, offset
        0,0,0,0,                                $ mult, dev, base, offset link not used
        2;                                      $ decimal places
    Param3 =
        0,                                      $ first field shall equal 0
        ,,                                      $ path size, path
        0x0044,                                 $ descriptor (scaling and decimal precision)
        0xC7,                                   $ data type : 16 bit unsigned integer
        2,                                      $ data size in bytes
        "Output Channel 0 +-10V ",              $ name
        "V",                                    $ units
        "",                                     $ help string
        31969,33567,31969,                      $ min, max, default data values
        1,79,100,-32769,                        $ mult, dev, base, offset
        0,0,0,0,                                $ mult, dev, base, offset link not used
        2;                                      $ decimal places
    Param4 =
        0,                                      $ first field shall equal 0
        ,,                                      $ path size, path
        0x0044,                                 $ descriptor (scaling and decimal precision)
        0xC7,                                   $ data type : 16 bit unsigned integer
        2,                                      $ data size in bytes
        "Output Channel 0 0-20mA ",             $ name
        "mA",                                   $ units
        "",                                     $ help string
        0, 31208,0,                             $ min, max, default data values
        1,1560,1000,0,                          $ mult, dev, base, offset
        0,0,0,0,                                $ mult, dev, base, offset link not used
        2;                                      $ decimal places

[Assembly]
    Assem1 =
        “configuration”,”20 7D 24 00 30 0C”, ,,,,
        4,Param1;                               $ Output Channel 0 Configuration
        Variant1 = “Output Channel 0 Configuration”, $ name
        ,,,,                                    $ Reserved fields
        Assem1:0:4,                             $ 使用Assem1中的Param1的值选择下面的参数中的一个：
        0x01, Param2,                           $ 如果上面的值=0x01，然后使用参数-Param2
        0x02, Param3,                           $ 上面值=0x02 时，用户需要指定使用哪个参数（Param3或Param4）
        0x02, Param4;                           $ 上面值=0x02 时，用户需要指定使用哪个参数（Param3或Param4）
    Assem2 =
        “output” ,,,,,,
        4, Variant1;

[Params]
    Param1 =
        0,                              $ first field shall equal 0
        7, "20 93 25 1A 40 30 09",      $ path size, path
        0x02,                           $ descriptor
        0xD1,                           $ data type: BYTE
        1,                              $ data size in bytes
        "M-S Output", "",               $ name, help string
        "",                             $ units
        0x01, 0x10, 0x1F,               $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Enum1 =
        0, "Set - Status/Fdbk",
        1, "Set - DL A Out, Reset – DL B Out",
        3, "Reset - DL C Out",
        4, "Set - DL D Out";
    Param10 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC7,                           $ data type: UINT
        2,                              $ data size in bytes
        "Logic Status", "",             $ name, help string
        "",                             $ units
        0, 65535, 0,                    $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param11 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC3,                           $ data type: INT
        2,                              $ data size in bytes
        "Feedback", "",                 $ name, help string
        "",                             $ units
        -32767, 32767, 0,               $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param12 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out A1", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param13 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out A2", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param14 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out B1", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param15 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out B2", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param16 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out C1", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param17 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out C2", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param18 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out D1", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used
    Param19 =
        0,                              $ first field shall equal 0
        ,,                              $ path size, path
        0x00,                           $ descriptor
        0xC8,                           $ data type: UDINT
        4,                              $ data size in bytes
        "Data Out D2", "",              $ name, help string
        "",                             $ units
        0, 857, 0,                      $ min, max, default data values
        ,,,,                            $ scaling, not used
        ,,,,                            $ link scaling, not used
        ;                               $ decimal places, not used

[Assembly]
    Assem1 = "Input Assembly",          $ Name
        ,                               $ Path
        ,                               $ Data Size
        0x00,                           $ Descriptor
        ,,                              $ Reserved
        , BitStringVariant1;            $ Variable input

    BitStringVariant1 =
        "Output Data",,                 $ Name, Help String
        Param1,                         $ Switch 选择器
        0,Assem2,,                      $ Status/Fdbk Selection
        1,Assem3,Assem4,                $ DL A Out Selection or DL B Out Selection
        3,,Assem5,                      $ DL C Out Selection
        4,Assem6,;                      $ DL D Out Selection

    Assem2 = "Logic Status & Feedback", $ Name
        ,                               $ Path
        4,                              $ Data Size
        0x00,                           $ Descriptor
        ,,                              $ Reserved
        16, Param10,                    $ Logic Status
        16, Param11;                    $ Feedback

    Assem3 = "DL A Out",                $ Name
        ,                               $ Path
        8,                              $ Data Size
        0x00,                           $ Descriptor
        ,,                              $ Reserved
        32, Param12,                    $ Datalink A1 Out
        32, Param13;                    $ Datalink A2 Out

    Assem4 = "DL B Out",                $ Name
        ,                               $ Path
        8,                              $ Data Size
        0x00,                           $ Descriptor
        ,,                              $ Reserved
        32, Param14,                    $ Datalink B1 Out
        32, Param15;                    $ Datalink B2 Out

    Assem5 = "DL C Out",                $ Name
        ,                               $ Path
        8,                              $ Data Size
        0x00,                           $ Descriptor
        ,,                              $ Reserved
        32, Param16,                    $ Datalink C1 Out
        32, Param17;                    $ Datalink C2 Out

    Assem6 = "DL D Out",                $ Name
        ,                               $ Path
        8,                              $ Data Size
        0x00,                           $ Descriptor
        ,,                              $ Reserved
        32, Param18,                    $ Datalink D1 Out
        32, Param19;                    $ Datalink D2 Out

[Params]
    Param1 =
        0,                              $ reserved, shall equal 0
        0,,                             $ link path size, link path
        0x0000,                         $ descriptor
        0xC7,                           $ data type (UINT)
        2,                              $ data size in bytes
        "Speed",                        $ name
        "RPMs",                         $ units
        "Element of speeds",            $ help string
        ,,100,                          $ min, max, default data values
        ,,,,                            $ mult, div, base, offset scaling
        ,,,,                            $ mult, div, base, offset links
        ;                               $ decimal places

    Param2 =
        0,                              $ reserved, shall equal 0
        0,,                             $ link path size, link path
        0x0000,                         $ descriptor
        0xD9,                           $ data type (SHORT_STRING)
        1,                              $ data size in bytes
        "Device Name",                  $ name
        "",                             $ units
        "Device Name",                  $ help string
        1,25,"Device Name n",           $ min, max, default data values
        ,,,,                            $ mult, div, base, offset scaling
        ,,,,                            $ mult, div, base, offset links
        ;                               $ decimal places

[Assembly]
    Revision = 2;
    Array1 =                            $ one dimension array
        "Channel 1 Speeds",             $ name of array
        "20 64 24 01 30 01",            $ link Path (vendor specific object attribute)
        0x0000,                         $ descriptor
        "Array of Speeds",              $ help string
        ,,,                             $ reserved
        16,                             $ array element size
        Param1,                         $ array element type
        1,                              $ one dimensional array
        5;                              $ number of elements in 1st dimension

    Array2 =                            $ multiple dimension array
        "Channel Speeds",               $ name of array
        "20 64 24 01 30 02",            $ link Path (vendor specific object attribute)
        0x0000,                         $ descriptor
        "Array of Speeds",              $ help string
        ,,,                             $ reserved
        16,                             $ array element size
        Param1,                         $ array element type
        2,                              $ two dimensional array
        2,                              $ number of elements in 1st dimension
        3;                              $ number of elements in 2nd dimension

    Array3 =                            $ single dimension array of variable size elements (SHORT_STRING)
        "Device Names",                 $ name of array
        "20 04 24 23 30 03",            $ link Path
        0x0000,                         $ descriptor
        "Array of Device Names",        $ help string
        ,,,                             $ reserved
        ,                               $ array element size (no size because element type
                                        $ is variable size)
        Param2,                         $ array element type
        1,                              $ one dimensional array
        20;                             $ number of elements in array

ProxyAssem1 =
    "Module Input Data",                $ Assem name
    "20 04 24 SLOT 30 0A",              $ no path
    2,                                  $ 2 bytes of input per slot
    0b0000000000000010,                 $ fixed slot size, expanded adapter rack
                                        $ Assem, adapter excluded
    , ,                                 $ reserved, reserved
    ModuleMemberList;                   $ the module EDS specifies the format of
                                        $ the module input data

Module in slot 1 EDS:
ProxiedAssem1 = ,,,,,,                  $ Assem name, path, size, descriptor,
                                        $ reserved, reserved
    16,;                                $ 16 位输入数据, format unspecified

Module in slot 2 EDS:
ProxiedAssem1 = ,,,,,,                  $ Assem name, path, size, descriptor,
                                        $ reserved, reserved
    4, Param1,                          $ 4 位输入数据
    12, Param2;                         $ 12 位输入数据

Module in slot 4 EDS:

ProxiedAssem1 = ,,,,,,                  $ Assem name, path, size, descriptor,
                                        $ reserved, reserved
    ;                                   $ no input data

Module in slot 5 EDS:

ProxiedAssem1 = ,,,,,,                  $ Assem name, path, size, descriptor,
                                        $ reserved, reserved
    4, Param1,                          $ 4 位输入数据
    12, Param2;                         $ 12 位输入数据

Module in slot 6 EDS:

ProxiedAssem1 = ,,,,,,                  $ Assem name, path, size, descriptor,
                                        $ reserved, reserved
    4, Param1,                          $ 4 位输入数据
    12, Param2;                         $ 12 位输入数据

Module in slot 7 EDS:

ProxiedAssem1 = ,,,,,,                  $ Assem name, path, size, descriptor,
                                        $ reserved, reserved
    ;                                   $ no input data

Module in slot 8 EDS:

ProxiedAssem1 = ,,,,,,                  $ Assem name, path, size, descriptor,
                                        $ reserved, reserved
    4, Param1,                          $ 4 位输入数据
    12, Param2;                         $ 12 位输入数据

[Params]
    Param1 =
        0,                              $ reserved, shall equal 0
        6,"20 0F 24 01 30 01",          $ Path Size, Path
        0x0400,                         $ Descriptor – Indirect Parameter
        0xC6,                           $ Data Type - USINT
        1,                              $ Data Size in bytes
        "Pointer",                      $ name
        "",                             $ no units
        "Select parameter number to be used in input assembly", $ help string
        2,100,48,                       $ min, max, default data values
        ,,,,                            $ mult, div, base, offset scaling
        ,,,,                            $ mult, div, base, offset links
        ;                               $ decimal places

    Param48 =
        0,                              $ reserved, shall equal 0
        6,"20 0F 24 30 30 01",          $ Path Size, Path
        0x0000,                         $ Descriptor
        0xC7,                           $ Data Type - UINT
        2,                              $ Data Size in bytes
        "Device Temperature",           $ name
        "Fahrenheit",                   $ units
        "Device internal temperature",  $ help string
        ,,0x0055,                       $ min, max, default data values
        ,,,,                            $ mult, div, base, offset scaling
        ,,,,                            $ mult, div, base, offset links
        ;                               $ decimal places

[Assembly]
    Assem1 =
        "Input",                        $ name
        "20 04 24 02 30 30",            $ Path
        ,                               $ Size - unspecified
        0x0000 ,                        $ Descriptor – treat indirect parameters as pointers
        ,,                              $ Reserved
        ,Param3,                        $ Member size, Member reference
        ,Param4,                        $ Member size, Member reference
        ,Param1;                        $ Member size, Member reference (user selected parameter)

    Assem2 =
        "Config",                       $ name
        "20 04 24 01 30 30",            $ Path
        ,                               $ Size - unspecified
        0x0800 ,                        $ Descriptor – treat indirect parameters as direct parameters
        ,,                              $ Reserved
        ,Param1;                        $ Member size, Member reference (user selectable
                                        $ parameter for input assembly) 

[File]
    DescText = "Smart Widget EDS File";
    CreateDate = 04-03-1994;            $ created
    CreateTime = 17:51:44;
    ModDate = 04-06-1994;               $ last changed
    ModTime = 22:07:30;
    Revision = 2.1;                     $ revision of EDS

[Device]
    VendCode = 65535;
    VendName = "Widget-Works, Inc.";
    ProdType = 43;
    ProdTypeStr = "Generic Device";
    ProdCode = 42;
    MajRev = 1;
    MinRev = 1;
    ProdName = "Smart-Widget";
    Catalog = "1499-DVG";

[ParamClass]
    MaxInst = 3;
    Descriptor = 0x0E;
    CfgAssembly = 3;

[Params]
    Param1 = 0, 6,"20 64 24 01 30 01", 0x0014, 0xC6, 1,"Preset","V",
        "User Manual p33", 0, 5, 1, 1, 1, 1, 0, , , , , 2;

    Param2 =                            $ parameter instance
        0,                              $ First field shall equal 0
        1, "20 04",                     $ path size, path
        0x0014,                         $ descriptor - in hex format
        0xC6,                           $ data type: Unsigned 8-Bit Integer (USINT)
        1,                              $ data size
        “Trigger”,                      $ name
        “Hz”,                           $ units
        “User Manual p49”,              $ help string
        0, 2, 0,                        $ min, max, default data values
        1, 1, 1, 0,                     $ mult, div, base, offset scaling
        0, 0, 0, 0,                     $ mult, div, base, offset links not used 2; $ decimal places

[Groups]
    Group1 = "Setup", 2, 1, 2;          $ group 1
    Group2 = "Monitor", 2, 2, 3;        $ group 2
    Group3 = "Maintenance", 2, 1, 3;    $ group 3   

[Params] 
    Param1 =                            $ specifies read buffer 
    0, , ,                              $ no path means not directly accessible 
    0x0004,                             $ descriptor : support scaling 
    0xC6, 1,                            $ USINT, 1 byte 
    "Read",                             $ name 
    "", "",                             $ units & help string 
    64, 95, 64,                         $ min, max, default data values 
    1, 1, 1, -63,                       $ mult, div, base, offset scaling 
    0, 0, 0, 0, 0;                      $ mult, div, base, offset link & decimal (not used)

    Param2 =                            $ specifies write buffer 
        0, , ,                          $ no path means not directly accessible 
        0x0004,                         $ descriptor : support scaling 
        0xC6, 1,                        $ USINT, 1 byte 
        "Write",                        $ name 
        "", "",                         $ units & help string 
        160, 191, 160,                  $ min, max, default data values 
        1, 1, 1, -159,                  $ mult, div, base, offset scaling 
        0, 0, 0, 0, 0;                  $ mult, div, base, offset link & decimal $(not used)

[Connection Manager] 
    Connection1 = 
        0x04010002,                     $ trigger & transport 
                                        $ class 1, cyclic, exclusive-owner 
        0x44244401, $ point/multicast & priority & real time format 
                                        $ fixed, 32-bit headers, scheduled, 
                                        $ OT point-to-point, TO multicast 
        , 16, ,                         $ OT RPI, size, format 
        , 12, ,                         $ TO RPI, size, format 
        , ,                             $ config part 1 (not used) 
        , ,                             $ config part 2 (not used) 
        "read/write",                   $ connection name 
        "",                             $ Help string 
        "20 04 24 01 2C [Param2] 2C [Param1]";
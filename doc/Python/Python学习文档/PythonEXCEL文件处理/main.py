#-*-coding:utf-8-*-
"""
@author: Tupelo Shen
@time:2019/07/01
@email:Wanjiang.Shen@cn.yokogawa.com
"""

import logging
import xlrd
import datetime

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s')
excel_file_name = "YCM-KH-EIP-U013-R0-Admag_TI_Parameter_List_For_EIP.xlsx"
eds_file_name = "ADMAG_TI_0101.EDS"

EDS_FILE_HEAD = "$ Electronic Data Sheet. Copyright by Yokogawa.\n\n"
SEGMENT_LINE_END = ";\n"
TABS_BEFORE_ENTRY = "\t\t"
TABS_BEFORE_FEILD = "\t\t\t\t"
TABS_BEFORE_COMMENT = "\t\t\t\t\t\t\t\t\t\t"
file_contents = []

# Data Type of EtherNet/IP
DATA_TYPE_DICT = {
    'BOOL':         0xC1,
    'SINT':         0xC2,
    'INT':          0xC3,
    'DINT':         0xC4,
    'LINT':         0xC5,
    'USINT':        0xC6,
    'UINT':         0xC7,
    'UDINT':        0xC8,
    'ULINT':        0xC9,
    'REAL':         0xCA,
    'LREAL':        0xCB,
    'STIME':        0xCC,
    'DATE':         0xCD,
    'TIME_OF_DAY':  0xCE,
    'DATE_AND_TIME':0xCF,
    'STRING':       0xD0,
    'BYTE':         0xD1,
    'WORD':         0xD2,
    'DWORD':        0xD3,
    'LWORD':        0xD4,
    'STRING2':      0xD5,
    'FTIME':        0xD6,
    'LTIME':        0xD7,
    'ITIME':        0xD8,
    'STRINGN':      0xD9,
    'SHORT_STRING': 0xDA,
    'TIME':         0xDB,
    'EPATH':        0xDC,
    'ENGUNIT':      0xDD,
    'STRINGI':      0xDE
}

# get the code for Data Type of EtherNet/IP by string
def get_data_type_by_string(string):
    return DATA_TYPE_DICT[string]

def is_int_type(string):
    if string == 'SINT' or \
        string == 'INT' or \
        string == 'DINT' or \
        string == 'LINT' or \
        string == 'USINT' or \
        string == 'UINT' or \
        string == 'UDINT' or \
        string == 'ULINT' :
        return True
    else:
        return False

def get_tabs_of_comment(string):
    tabs = ""
    tmp = ""
    string_size = int(len(string)/4)
    
    if string_size > len(TABS_BEFORE_COMMENT) - len(TABS_BEFORE_FEILD):
        tabs = ""
        return tabs
    
    for i in range(len(TABS_BEFORE_COMMENT) - len(TABS_BEFORE_FEILD)-string_size):
        tabs+= "\t"
        # if string == ',,,,':
        #     tmp+="*"
    # print(tmp)
    return tabs

def read_excel(file):
    loop_start = False
    global file_contents
    #open the excel file and read data from it.
    data = xlrd.open_workbook(file)

    # get the contents in a sheet by its name
    table = data.sheet_by_name('ParameterList-EIP')

    # debug - print the name, rows and collomns of this sheet
    print(table.name, table.nrows, table.ncols)

    # [File] segment
    segment_line = "[File]\n"
    file_contents.append(segment_line)
    desc_text_line = TABS_BEFORE_ENTRY + "DescText = " + "\"YOKOGAWA ADMAG TI EDS\"" + SEGMENT_LINE_END
    file_contents.append(desc_text_line)
    now_time = datetime.datetime.now()
    create_date_line = TABS_BEFORE_ENTRY + "CreateDate = " + now_time.strftime('%m-%d-%Y') + SEGMENT_LINE_END
    file_contents.append(create_date_line)
    create_time_line = TABS_BEFORE_ENTRY + "CreateTime = " + now_time.strftime('%H:%M:%S') + SEGMENT_LINE_END
    file_contents.append(create_time_line)
    modify_date_line = TABS_BEFORE_ENTRY + "ModDate = " + now_time.strftime('%m-%d-%Y') + SEGMENT_LINE_END
    file_contents.append(modify_date_line)
    modify_time_line = TABS_BEFORE_ENTRY + "ModTime = " + now_time.strftime('%H:%M:%S') + SEGMENT_LINE_END
    file_contents.append(modify_time_line)
    revision_line = TABS_BEFORE_ENTRY + "Revision = " + "1.1" + SEGMENT_LINE_END
    file_contents.append(revision_line)
    homeurl_line = TABS_BEFORE_ENTRY + "HomeURL = " + "\"https://www.yokogawa.com/\"" + SEGMENT_LINE_END + "\n"
    file_contents.append(homeurl_line)

    # [Device] segment
    segment_line = "[Device]\n"
    file_contents.append(segment_line)
    VendCode_line = TABS_BEFORE_ENTRY + "VendCode = " + "250" + SEGMENT_LINE_END
    file_contents.append(VendCode_line)
    VendName_line = TABS_BEFORE_ENTRY + "VendName = " + "\"Yokogawa Electric Corporation\"" + SEGMENT_LINE_END
    file_contents.append(VendName_line)
    ProdType_line = TABS_BEFORE_ENTRY + "ProdType = " + "36" + SEGMENT_LINE_END
    file_contents.append(ProdType_line)
    ProdTypeStr_line = TABS_BEFORE_ENTRY + "ProdTypeStr = " + "\"Fluid Flow Controller\"" + SEGMENT_LINE_END
    file_contents.append(ProdTypeStr_line)
    ProdCode_line = TABS_BEFORE_ENTRY + "ProdCode = " + "11" + SEGMENT_LINE_END
    file_contents.append(ProdCode_line)
    MajRev_line = TABS_BEFORE_ENTRY + "MajRev = " + "1" + SEGMENT_LINE_END
    file_contents.append(MajRev_line)
    MinRev_line = TABS_BEFORE_ENTRY + "MinRev = " + "1" + SEGMENT_LINE_END
    file_contents.append(MinRev_line)
    ProdName_line = TABS_BEFORE_ENTRY + "ProdName = " + "\"ADMAG_TI\"" + SEGMENT_LINE_END
    file_contents.append(ProdName_line)
    Catalog_line = TABS_BEFORE_ENTRY + "Catalog = " + "\"ADMAG_TI\"" + SEGMENT_LINE_END
    file_contents.append(Catalog_line)
    Icon_line = TABS_BEFORE_ENTRY + "Icon = " + "\"ADMAG_TI.ico\"" + SEGMENT_LINE_END + "\n"
    file_contents.append(Icon_line)

    # [Device Classification] segment
    segment_line = "[Device Classification]\n"
    file_contents.append(segment_line)
    Class1_line = TABS_BEFORE_ENTRY + "Class1 = " + "EtherNetIP" + SEGMENT_LINE_END + "\n"
    file_contents.append(Class1_line)

    # loop for [Params] segment
    for x in range(0,table.nrows):
        # print(table.cell(x, 0).value)
        if table.cell(x, 0).value == 1 and loop_start == False:
            loop_start = True
            file_contents.append("[Params]\n")

        if loop_start == True and (table.cell(x, 0).value == '#' or table.cell(x, 0).value == ''):
            loop_start = False
            break

        if loop_start == True :
            
            # EntryN (N=0,1,...,N)
            entry_line = TABS_BEFORE_ENTRY + "Param%d =\n"%(table.cell(x, 0).value)
            file_contents.append(entry_line)

            # $ Reserved (always 0)
            value = "0,"
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Reserved (always 0)\n"
            file_contents.append(field_line + comment_line)

            # $ Link path size, Link Path (Class, Instance, Attribute)
            if table.cell(x, 5).value != '' or \
                table.cell(x, 6).value != '' or \
                table.cell(x, 7).value != '':
                value = "6," + "\"20 %X 24 %X 30 %X\"," %\
                        (int(table.cell(x, 5).value), \
                        int(table.cell(x, 6).value), \
                        int(table.cell(x, 7).value))
                
            else:
                value = "0,\"\","
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Link path size\n" + \
                            TABS_BEFORE_COMMENT + "$ Link path (class, instance, attribute)\n"
            file_contents.append(field_line + comment_line)

            # $ Descriptor
            if table.cell(x, 8).value != '':
                value = "%s," % (table.cell(x, 8).value)
            else:
                value = "0x0000,"
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Descriptor\n"
            file_contents.append(field_line + comment_line)

            # $ Data Type
            if table.cell(x, 12).value != '':
                value = "0x%2X," % (get_data_type_by_string(table.cell(x, 12).value))
                comment_line = get_tabs_of_comment(value) + "$ Data type ( %s )\n" % (table.cell(x, 12).value)
            else:
                value = "0xC6,"
                comment_line = get_tabs_of_comment(value) + "$ Data type ( USINT )\n"
            field_line = TABS_BEFORE_FEILD + value
            file_contents.append(field_line + comment_line)

            # $ Data size
            if table.cell(x, 11).value != '':
                value = "%s," % (int(table.cell(x, 11).value))
            else:
                value = "1,"
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Data size\n"
            file_contents.append(field_line + comment_line)

            # $ Parameter name
            if table.cell(x, 2).value != '':
                value = "\"%s\"," % (table.cell(x, 2).value)
            else:
                value = "\"\","
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Parameter name\n"
            file_contents.append(field_line + comment_line)

            # $ Units string
            if table.cell(x, 13).value == '-':
                value = "\"\","
            else:
                value = "\"%s\"," % (table.cell(x, 13).value)
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Units string\n"
            file_contents.append(field_line + comment_line)

            # $ Help string
            value = "\"%s\"," % (table.cell(x, 17).value)
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Help string\n"
            file_contents.append(field_line + comment_line)

            # min, max , default value
            if is_int_type(table.cell(x, 12).value) :
                lmt_val_format = "%d,"
                dft_val_format = "%d,"
            elif table.cell(x, 12).value == 'STRING' :
                lmt_val_format = "%d,"
                dft_val_format = "%s,"
            else:
                lmt_val_format = "%s,"
                dft_val_format = "%s,"
            # $ Minimum value
            value = lmt_val_format % (table.cell(x, 15).value)            
            # $ Maximum value
            value += lmt_val_format % (table.cell(x, 14).value)
            # $ Default value
            if table.cell(x, 16).value != '':
                value += dft_val_format % (table.cell(x, 16).value)
            else:
                value += "0," 
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Minimum value\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Maximum value\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Default value\n"
            file_contents.append(field_line + comment_line)

            # $ Scaling multiplier, divider, base, offset (not used)
            value = "," + "," + "," + ","
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Scaling multiplier (not used)\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Scaling divider (not used)\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Scaling base (not used)\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Scaling offset (not used)\n"
            file_contents.append(field_line + comment_line)

            # $ Multiplier, Divisor, Base, Offset link (not used)
            value = "," + "," + "," + ","
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Multiplier link (not used)\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Divisor link (not used)\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Base link (not used)\n"
            comment_line += TABS_BEFORE_COMMENT + "$ Offset link (not used)\n"
            file_contents.append(field_line + comment_line)          

            # $ Decimal precision (not used)
            value = ";"
            field_line = TABS_BEFORE_FEILD + value
            comment_line = get_tabs_of_comment(value) + "$ Decimal precision (not used)\n"
            file_contents.append(field_line + comment_line)

            # enum
            enum_name_value = table.cell(x, 18).value
            if enum_name_value != '' and enum_name_value != '-':
                # EnumN
                enum_code_list = table.cell(x, 19).value.split("\n")
                enum_string_list = table.cell(x, 20).value.split("\n")
                if len(enum_code_list) == len(enum_string_list) :
                    entry_line = TABS_BEFORE_ENTRY + "Enum%d =\n"%(table.cell(x, 0).value)
                    file_contents.append(entry_line)

                    # Selection options
                    for index in range(len(enum_code_list)):
                        if table.cell(x, 2).value == 'Status group 0' or\
                            table.cell(x, 2).value == 'Status group 1' or\
                            table.cell(x, 2).value == 'Status group 2' or\
                            table.cell(x, 2).value == 'Status group 3' :
                            value = "%d,\"%s\"" % (int(enum_code_list[index],16), enum_string_list[index])
                        else:
                            value = "%s,\"%s\"" % (enum_code_list[index], enum_string_list[index])
                        if index == len(enum_code_list)-1 :
                            value+=";\n\n"
                        else:
                            value+=",\n"
                        field_line = TABS_BEFORE_FEILD + value
                        file_contents.append(field_line)

            else:
                file_contents.append("\n")


    return file_contents
    
def write_eds(dest_file, file_contents):
    output = open(dest_file, 'w+')

    # file declaration
    output.write(EDS_FILE_HEAD)

    # write contents
    for line in file_contents:
        output.write(line)

    output.close()


if __name__ == "__main__":
    
    # read_excel(file)
    data = read_excel(excel_file_name);

    # write_eds(file, contents)
    write_eds(eds_file_name, data)
    # logger.info("Main progaram is terminating!")

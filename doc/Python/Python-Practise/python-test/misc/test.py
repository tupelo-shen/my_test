# -*- coding: utf-8 -*-
import time
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# plt.rcParams['font.sans-serif'] = ['KaiTi']
# plt.rcParams['font.serif'] = ['KaiTi']

ts_0 = pd.Series(np.random.randn(365), index=np.arange(1,366))
ts_0.plot()
plt.show()

# ts_1 = pd.Series(np.random.randn(365), index=pd.date_range('01/01/2017',periods=365))
# ts_1 = ts_1.cumsum()
# ts_1.plot()
# plt.show()

# df = pd.DataFrame(np.random.randn(8,6), index=pd.date_range('01/01/2018',periods=8),columns=list('ABCDEF'))
# print(df)
# df.plot()
# plt.show()

# df_0 = pd.read_excel('123.xlsx',sheet_name='Sheet1')
# print(df_0)
# print(type(df_0))
# df_0.plot()
# plt.show()

# df_0 = pd.read_excel('1234.xls',sheet_name='Sheet1')
# print(df_0)

# df_0 = pd.read_excel('D://test-github/my_test/doc/Python/Python-Practise/python-test/misc/123.xlsx',sheet_name= 'Sheet1')
# print(df_0)

# time.sleep(20)
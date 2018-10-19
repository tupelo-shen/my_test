#!/usr/bin/python
# -*- coding: utf-8 -*-

""" python non blocking input
"""
__author__ = 'Tupelo Shen'
__version__=  '2018/10/19'

import sys
import select

def raw_input_nb(time_out = 0.001, prompt=""):
    out_prompt = prompt
    if out_prompt:
        sys.stdout.write(out_prompt + '\n')

    if select.select([sys.stdin], [], [], time_out) == ([sys.stdin], [], []):
        return (sys.stdin.readline()[:-1]) # sys.stdin.readline( ).strip('\n')
        # print sys.stdin.readline()

def raw_input_nb_test():
    while True:
        test_str = raw_input_nb(10)
        sys.stdout.write(test_str + '\n')

if __name__ == "__main__":
    raw_input_nb_test()

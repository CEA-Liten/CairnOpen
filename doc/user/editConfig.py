# -*- coding: utf-8 -*-
"""
Created on Fri Dec  6 08:06:25 2024

@author: sc258201
"""

import os
import sys

def writeConfig(config):
    with open("./config.ini", 'w', encoding='utf-8') as infile:
        print(config)
        conf_list=config.split(',')
        for conf in conf_list:
            infile.write(conf)
            infile.write("\n")

if __name__ == '__main__':
    writeConfig(sys.argv[1])
    #writeConfig("CEA=True,OPEN=False")
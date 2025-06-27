# -*- coding: utf-8 -*-
"""
Created on Tue Jun 15 13:45:53 2021

@author: GVIGE000
"""

from lxml import etree 
import pandas as pd

def parse_xml(xml_filename):
    tree = etree.parse(xml_filename+'.xml') #découpage sous forme d'arbre du document
    root = tree.getroot() #on se place a la racine de l'arbre
    testsuites = root.findall("testsuite")
    classname = []
    testname = []
    name = []
    time = []
    passed = []
    failed = []
    skipped = []
    message = []
    print("parser xml process")
    for testsuite in testsuites:
        testcases = testsuite.findall("testcase")
        for testcase in testcases:
            cur_classname = testcase.get('classname')
            list_classname = cur_classname.split('.')
            cur_classname = '.'.join(list_classname[:-1])
            classname.append(cur_classname)
            testname.append(list_classname[-1])
            name.append(testcase.get('name'))
            time.append(float(testcase.get('time')))
            cur_passed = 0
            cur_failed = 0
            cur_skipped = 0
            cur_message = ''
            if len(testcase): #le test est soit failed soit skipped
                if testcase[0].tag=='skipped':
                    cur_skipped = 1
                if testcase[0].tag=='failure' or testcase[0].tag=='error':
                    cur_failed = 1
                cur_message = testcase[0].get('message')
            else:
                cur_passed = 1           
            
            passed.append(cur_passed)
            failed.append(cur_failed)
            skipped.append(cur_skipped)
            message.append(cur_message)
    out_df = pd.DataFrame({'classname':classname, 'testname': testname, 'name':name, 'time':time, 'passed':passed, 'failed':failed, 'skipped':skipped, 'message':message})
    return out_df
# -*- coding: utf-8 -*-
"""
@author: G.Lavialle - 07/10/2020 : create RunSensitivity function
@modified A.Ruby 20/10/2020 : integrate with utility tools into PerseeSensParam
@modified A.Ruby 22/10/2020 : add testcase report generation
@modified A.Ruby 04/11/2020 : add possible sensitivity to input time series
"""

import sys
import os
import shutil
import pandas as pd
import cairn as crn
import PerseePasteResults as ppr

print("charge persee sens param")

def run_sensitivity_manual_sampling(testcase, app_home, tab_param_name, tmax, verifyParamType, timeStepFile="", tsFileList=[]):
    test_adress = os.path.join(app_home,testcase+".json")
    tab_param_adress = os.path.join(app_home, tab_param_name)

    inst = crn.CairnAPI()
    problem = inst.read_study(test_adress)
    for s in tsFileList:
        problem.add_timeseries(s)
    
    tab_param = pd.read_csv(tab_param_adress, sep=";", decimal=".", header=[0,1], index_col=0)
    crn.run_sensitivity(problem, tab_param)
    
    
    ppr.PasteResultsMonoLoc(app_home, "Report_s", "PLAN", file_out="sumupall_sens.csv", list_order=tab_param.index)

if __name__ == '__main__':
    print("----------------------- Run Sensitivity Arguments------------------------------")
    print(sys.argv)
    # argment list : 
    # 1. app_home
    # 2. test_case
    # 3. tab_echantillonnage
    # 4. tmax
    # 5. verifyParamType
    # 6 and more . tsFileList (or timeStepFile)
    tsFileList = []
    timeStepFile = ""
    if len(sys.argv) > 5:  # ne pas modifier ! appel par PerseeGUI
        app_home = sys.argv[1]
        print("App Home:", app_home)
        testcase=sys.argv[2]
        print("Test Case Name:", testcase)

        if sys.argv[3]!="":
            tab_param_name=sys.argv[3]
        else:
            tab_param_name="tab_echantillonnage.csv"
        print("Tab Echantillonnage File:", tab_param_name)

        if sys.argv[4]!="":
            tmax = int(sys.argv[4])
        else:
            tmax = 10000
        print("Time Limit (s):", tmax)

        if sys.argv[5]!="":
            verifyParamType = (int(sys.argv[5]) > 0)
        else:
            verifyParamType = True
        print("Verify Parameter Types:", verifyParamType)

    #Timeseries files (and timeStepFile)
    if len(sys.argv) > 6:
        for i in range(6, len(sys.argv)):
            if sys.argv[i].startswith("TimeStepFile-"):#assumes that only one TimeStepFile could be provided 
                timeStepFile = sys.argv[i].split("TimeStepFile-")[1]
                if timeStepFile != "":
                    print("TimeStepFile :", timeStepFile)
            elif sys.argv[i] != "":
                tsFileList.append(sys.argv[i])
        
    if tsFileList == [] and os.path.exists(os.path.join(app_home, testcase+"_dataseries.csv")):
        print("There is no timeseries loaded. The timeseries "+testcase+"_dataseries.csv will be used!")
        tsFileList.append(testcase+"_dataseries.csv")
    print("TSFileList :", tsFileList)

    print("-------------------------------------------------------------------------------")

    if tsFileList != []:
        run_sensitivity_manual_sampling(testcase, app_home, tab_param_name, tmax, verifyParamType, timeStepFile=timeStepFile, tsFileList=tsFileList)
        print("end sensitivity")
    else:
        print("Error: check if there is at least one timeseries loaded", flush=True)


  

# -*- coding: utf-8 -*-
"""
@author: G.Lavialle - 07/10/2020 : create RunSensitivity function
@modified A.Ruby 20/10/2020 : integrate with utility tools into PerseeSensParam
@modified A.Ruby 22/10/2020 : add testcase report generation
@modified A.Ruby 04/11/2020 : add possible sensitivity to input time series
"""

import os
import shutil
import pandas as pd
import itertools
import json
import sys

from CairnNRT import CairnNRT

import numpy as np
import re
import copy
import PerseePasteResults as ppr
from PerseeExtractResult import ExtractResult
from PerseeExtractResult import saveStudyFiles
from PerseeExtractResult import moveFile
import Utilities as ut


print("charge persee sens param")

def resetSTR(filename, str1, str2):
    """ Reset n file filename str1 to str2 if str1 matches one of regular expressions 
    'model.parameter'='n.m'e'p' with optionnal fields '.m', e'p' 
    using re.fullmatch function
    """
    # search pattern elem.var=3.14 
#    regex="([a-zA-Z]+\.[a-zA-Z]+)=((\d+\.\d+)|(\d+)|(\d+\.)|true|false)"
#    m=re.search(regex,line)
    lines = []
    with open(filename, 'r', newline='') as f:
        for line in f:
            regex2 = str1+"(|-)((\d+\.\d+)|(\d+)|(\d+\.)|(\d+\.\d+)e((|-)\d+)|(\d+\.)e((|-)\d+)|(\d+)e((|-)\d+)|(true|True)|(false|False))(\n|\r\n|)"
            m = re.fullmatch(regex2, line)
            if m:
                lines.append(str2+'\r\n')
                print("YES : ", str1, " reset to ", str2, "in", line)
            else:
                if str1 in line:
                    print("NO : ", str1, " not replaced by ", str2, "in", line)
                lines.append(line)

    with open(filename, 'w', newline='') as f:
        for line in lines:
            f.write(line)
            
def replaceSTR(filename, str1, str2):
    """ Replace in filename all occurences of str1 by str2 
    using replace function
    """
    with open(filename, 'r', newline='') as f:
        lines = [line.replace(str1, str2) for line in f]
        
    with open(filename, 'w', newline='') as f:
        for line in lines:
            f.write(line)

def changeParamInJson(filename: str, tableToChange: str):
    """replace in the json the good parameters
    args:
    filename : address to the file .json
    tableToChange:  address to csv dataframe which contains the following columns
        Component: name of the component to modify
        Type: OptionListJson / ParamListJson / TimeSeriesListJson / nodePortsData following the type of data to change
        Key: name of the option
        Value: New value to load
    """
    if os.path.isfile(filename):
        print("change param in json")

    try:
        changes = pd.read_csv(tableToChange, sep=';', encoding = "utf-8")
    except UnicodeDecodeError:
        changes = pd.read_csv(tableToChange, sep=';', encoding = "ISO-8859-1")

    with open(filename, 'r',encoding="utf-8") as study_file:
        parser = json.load(study_file)
    parser = ut.loadStudyCompo(parser)
    c = 0
    changes["found"] = False
    for compo in ((parser["Components"])):
        if (compo["nodeName"]) in list(changes["Components"]):
            print("change ",compo["nodeName"])
            for i in changes[changes["Components"]==compo["nodeName"]].index:
                paramType = str(changes.loc[i]["Type"]).strip()
                paramName = str(changes.loc[i]["Key"]).strip()
                paramValue = changes.loc[i]["Value"]
                for j in range(len(compo[paramType])):
                    if "key" in compo[paramType][j]:
                        if compo[paramType][j]["key"] == paramName:
                            try:
                                if paramType == "portImpactsListJson" or paramType == "envImpactsListJson":
                                    if paramValue.lower() == "true":
                                        compo[paramType][j]["value"] = True
                                    elif paramValue.lower() == "false":
                                        compo[paramType][j]["value"] = False
                                    else:
                                        compo[paramType][j]["value"] = float(paramValue)
                                else:
                                    compo[paramType][j]["value"] = float(paramValue)    
                            except:
                                compo[paramType][j]["value"] = str(paramValue).strip()
                            changes.loc[i,"found"]=True
                    else: # case when parsing ports
                        nodePortsData = paramName.split("--")
                        if compo[paramType][j]["pos"] == str(nodePortsData[0]).strip(): # example pos = right
                            for u in compo[paramType][j]["ports"]: 
                                if u["name"] == str(nodePortsData[1]).strip():  # exmample PortR0
                                    attName = str(nodePortsData[2]).strip() # exmample "coeff"
                                    try:
                                        u[attName] = float(paramValue)
                                    except:
                                        u[attName] = str(paramValue).strip()
                                    changes.loc[i, "found"] = True
            parser["Components"][c]=compo
        else:
            print("do not change",compo["nodeName"])
        c+=1
    json_obj = json.dumps(parser, indent=4, ensure_ascii=False)
    with open(filename, "w",encoding="utf-8") as outfile:
        outfile.write(json_obj)
    changes.to_csv(tableToChange,sep=";",encoding="utf-8")
    return(parser)

def changeSettings(projectsPath, TestCase, str1, str2):
    """ Replace string str1 by str2 in PERSEE settings file matching template : TestCase+"_settings.ini"
    """
    file_name = projectsPath+"\\"+TestCase+"_settings.ini"
    if os.path.isfile(file_name):
        print ("- Change settings of ", file_name, flush=True)
        resetSTR(file_name, str1, str2)

def changeXml(projectsPath, TestCase, str1, str2):
    """ Replace string str1 by str2 in PERSEE / PEGASE xml architecture files matching template : TestCase+".xml"
    """
    file_name = projectsPath+"\\"+TestCase+".xml"
    if os.path.isfile(file_name):
       print("- Change XML of ", file_name, str1, str2, flush=True)
       replaceSTR(file_name, str1, str2)

def read_config_entree(testcase):
    """ csv configuration file reading to perform sensitivity on input parmaters in PERSEE settings file
    """
    liste_cal = []
    liste_param = []
    
    entree = testcase + "_sens_entree.csv"
    entree_link = testcase + "_sens_linkedvariables.csv"
    if not os.path.isfile(entree):
       print("Missing entry file describing sensitivity parameters :"+entree)
       return liste_param, liste_cal
     
    try:
        fichierE = pd.read_csv(entree, sep=";",na_values=-1, encoding = "utf-8")
    except UnicodeDecodeError:
        fichierE = pd.read_csv(entree, sep=";",na_values=-1, encoding = "ISO-8859-1")

    liste = fichierE.values.tolist()
    for i,param in enumerate(liste):
      liste_param.append(param[0])
      if param[1]!="false":
          liste_cal.append(np.arange(float(param[1]), float(param[2])+float(param[3]), float(param[3])))
      if len(param)>4:
          if param[4]!="false":
              liste_cal.append(param[4].split(","))

    liste_cal2 = list(itertools.product(*liste_cal))
    liste_cal2 = np.array(liste_cal2)
    tab = pd.DataFrame(liste_cal2, columns=[i.replace(".", "__") for i in liste_param])
    for col in tab.columns:
        try:
            tab[col] = tab[col].astype(float)
        except:
            continue
    if not os.path.isfile(entree_link):
       print("Missing entry file describing linked parameters :"+entree_link)
       return liste_param, liste_cal2, tab

    try:
        liens = pd.read_csv(entree_link, sep=";", index_col="Variable", encoding = "utf-8")
    except UnicodeDecodeError:
        liens = pd.read_csv(entree_link, sep=";", index_col="Variable", encoding = "ISO-8859-1")

    for c in liens.index:
        #tab["form"+c]=liens.loc[c,"Formule"]
        #tab[c]= np.diag(tab.eval(tab["form"+c].replace('.','é'),target=tab))
        print((c+"="+liens.loc[c,"Formule"]).replace('..', "__"))
        tab=tab.eval((c+"="+liens.loc[c,"Formule"]).replace('..', "__"))
        #tab = tab.drop("form"+c,axis=1)
    liste_param = [i.replace("__",".") for i in tab.columns]
    liste_cal3 = [list(tab.loc[i]) for i in range(len(tab))]
    tab.to_csv("tab_echantillonnage.csv",sep=";",encoding="utf-8")
    return liste_param,liste_cal3, tab



def get_number_format(liste_cal):
    nrun = len(liste_cal)
    form = "{:01}"
    if nrun > 9   : form="{:02}"
    if nrun > 99  : form="{:03}"
    if nrun > 999 : form="{:04}"
    return form

def read_config_TS(testcase):
    """ csv configuration file reading to perform sensitivity on Time Series file in PEGASE xml file
    first column = name of timeseries to be replaced
    second column = name of timeseries to be applied
    return list of time series to replace and list of new time series to use
    empty list returned means no Time Series sensitivity performed
    """
    liste_param = []
    liste_TS_init = []
    
    entree = testcase + "_sens_TS.csv"
    if not os.path.isfile(entree):
       print ("No entry file describing sensitivity TimeSeries :"+entree)
       return [""],[""]
     
    try:
        fichierE = pd.read_csv(entree, sep=";", encoding = "utf-8")
    except UnicodeDecodeError:
        fichierE = pd.read_csv(entree, sep=";", encoding = "ISO-8859-1")

    liste = fichierE.values.tolist()
    
    for param in liste:
      liste_param.append(param[1])
      liste_TS_init.append(param[0])
      
    return liste_param, liste_TS_init

def runSensibPerseeJson(running_dir:str, tnr, testcase:str, maxtime:int, generate_report:bool, tab_param:pd.DataFrame, paste_results=False, timeStepFile="", tsFileList=[]):
    """
    run a sensitivity study with tab_param as parameter defintion
    :param running_dir: directory where the study is located
    :param tnr: python class to run tnr (see PerseeGUI/Deps/Scripts
    :param testcase: name of the case
    :param maxtime: maximum time for each persee run
    :param generate_report: bool , whether to generate the report from PerseeExtractResults or not
    :param tab_param: Dataframe "Components","Type","Key","Value" wich contains the parameters to set for each study. Can be generated with read config entree or manually
    :param paste_results: paste the _plan results in a file after the run
    :param tsFileList: list of timeseries files
    :return: nothing
    """
    copie = "copy_" + testcase
    form = get_number_format(tab_param.index)
    print("RunsensibPerseeJson with param ", tab_param, " file")

    df_to_change = pd.DataFrame(columns=["Components","Type","Key","Value"])
    i=0
    for c in tab_param.columns:

        elems = c.split("__")
        elems.append(0)
        df_to_change.loc[i]=(elems)
        i+=1
    df_to_change.to_csv(running_dir+"\\"+"_to_change.csv",sep=";",encoding="utf-8")
    for i in range(len(tab_param)):
        #str_test = str(form.format(i + 1))
        str_test = (tab_param.index[i])
        new_testcase_loc = testcase + "_" + str(str_test)

        # write copy of PERSEE description file
        shutil.copyfile(os.path.join(running_dir, testcase + ".json"), os.path.join(running_dir, new_testcase_loc + ".json"))
        
        print(df_to_change,"value")
        print( tab_param, str_test)
        df_to_change["Value"] = list(tab_param.loc[str_test])
        df_to_change.to_csv(os.path.join(running_dir,new_testcase_loc+"_to_change.csv"),sep=";",encoding="utf-8")
        changeParamInJson(os.path.join(running_dir,new_testcase_loc + ".json"),os.path.join(running_dir,new_testcase_loc+"_to_change.csv"))
        # Run test
        print("runCairn",new_testcase_loc + ".json")
        dataseriesList = copy.deepcopy(tsFileList)
        dataseries = testcase+"_dataseries.csv"
        if len(tsFileList) == 1:
            dataseries = os.path.basename(tsFileList[0])
            dataseriesList = [] #runPersee assumes one time series (tnr_series) if tsFileList is empty
        exitCode = tnr.runPersee(tnr_dir="", tnr_xml=new_testcase_loc + ".json",tnr_series=dataseries, maxtime=maxtime, timeStepFile=timeStepFile, tsFileList=dataseriesList)
        if exitCode == -2:
            print("Run Sensitivity is stopped because the Time Limit of %d seconds has been reached!"%maxtime, flush=True)
            exit(1)

        # Renaming files locally before saving
        moveFile(copie + ".json", new_testcase_loc)

        folder_report = running_dir + ut.getOSsep() + "Report_s" + str(str_test)
        # move all files to saving study folder
        isSuccessful = saveStudyFiles(running_dir, new_testcase_loc, folder_report, False)

        #!!! Attention : This message is used by Persee GUI !!!
        if isSuccessful == 0:
            print("Case %d is successfully completed"%(i+1), flush=True) 

        if generate_report:
            ExtractResult(running_dir, new_testcase_loc, "s" + str(str_test), folder_report, 0)
    if paste_results:
        print("paste results")
        ppr.PasteResults(running_dir, "Report_s", "")

def generate_summary(testcase, liste_param, liste_cal, new_TS, folder_report=r".",from_tab=False,tab_echantillonnage=pd.DataFrame()):
    """ Write summary of the sensitivity analysis
    testcase: nom du cas
    liste_param: nom des paramètres
    liste_cal: valeurs des paramètres
    tab: tableau des paramètres et valeurs
    new_TS: noms des timeseries
    """
    if from_tab == False:
        # création des bonnes colonnes à partir des paramétrages
        dserie = pd.DataFrame(liste_cal,columns=liste_param)
    else:
        dserie = tab_echantillonnage

    #form = get_number_format((dserie).index)
    #dserie["cas"] = ["Report_s"+str(form.format(i+1)) for i in range(len(dserie))]
    dserie["cas"] = ["Report_s"+str(i) for i in dserie.index.values] 
    dserie = dserie.set_index('cas')

    #re-order 
    list_index = [str(i) for i in tab_echantillonnage.index.values] #first column

    paste_res = ppr.PasteResultsMonoLoc(folder_report, "Report_s", "PLAN",file_out="SumupAll_s.csv", list_order=list_index)
    conf_sortie = testcase + "_sens_sortie.csv"

    if (not os.path.isfile(conf_sortie)):
        print("Missing entry file describing sensitivity parameters :" + conf_sortie)
        paste_res_complet = paste_res.transpose()
        paste_res_param_values_complet = dserie.join(paste_res_complet)
        paste_res_param_values_complet.to_csv("Report_sensib_full"+testcase+".csv",sep=";",encoding="utf-8")
    else:
        try:
            fichierS = pd.read_csv(conf_sortie, sep=";", index_col=None, encoding = "utf-8")
        except UnicodeDecodeError:
            fichierS = pd.read_csv(conf_sortie, sep=";", index_col=None, encoding = "ISO-8859-1")
        fichierS["Component.Variable"] = fichierS["Param"].map(str) + "." + fichierS["Param2"].map(str)
        fichierS = fichierS.set_index("Component.Variable")
        fichierS.replace("nan", "Nan", inplace=True)
        fichierS = fichierS.fillna("Nan")

        paste_res_filtre = paste_res.loc[paste_res.index.isin(list(fichierS.index))].transpose()
        paste_res_param_values = dserie.join(paste_res_filtre)
        paste_res_complet = paste_res.transpose()
        paste_res_param_values_complet = dserie.join(paste_res_complet)
        paste_res_param_values.to_csv("Report_Sensib_"+testcase+".csv",sep=";",encoding="utf-8")
        paste_res_param_values_complet.to_csv("Report_sensib_full"+testcase+".csv",sep=";",encoding="utf-8")

def run_sensitivity_echantillonnage_manuel(testcase, app_home, tab_param_name, tmax = 10000, verifyParamType = True, timeStepFile="", tsFileList=[]):
    tnr = CairnNRT(app_home)
    tab_param_file = os.path.join(app_home, tab_param_name)    
    try:
        tab_param = pd.read_csv(tab_param_file, sep=";", index_col=0, low_memory=False, encoding = "utf-8") 
    except UnicodeDecodeError:
        tab_param = pd.read_csv(tab_param_file, sep=";", index_col=0, low_memory=False, encoding = "ISO-8859-1") 
    print("File used for echantillonnage:", tab_param_file)
    if verifyParamType:
        print("Checking tab_echantillonnage file ...")
        check_tab_param(app_home, testcase, tab_param)
    generate_report = False
    runSensibPerseeJson(app_home, tnr, testcase, tmax, generate_report, tab_param, paste_results=False, timeStepFile=timeStepFile, tsFileList=tsFileList)
    list_index = [str(i) for i in tab_param.index.values] #first column
    ppr.PasteResultsMonoLoc(app_home, "Report_s", "PLAN", file_out="sumupall_sens.csv", list_order=list_index)
    generate_summary(testcase, [], [], "", folder_report=app_home, from_tab=True, tab_echantillonnage=tab_param)


#checking tab_param content
def check_tab_param(app_home, testcase, tab_param):
    """ checking content of the sampling cases
    app_home: study folder
    testcase: study name
    tab_param: case files
    """
    studyFile =  os.path.join(app_home,testcase+'.json')
    print("Getting tab_echantillonnage ...")
    tab_param_keys = tab_param.keys()

    #getting study 
    with open(studyFile, "r", encoding="utf-8") as f:
        studyContent = json.load(f)
    dictComponentsParam = ut.get_study_param(studyContent, tab_param_keys)

    #checking keys
    paramStudyDtypes = ut.checking_header_tab_param(tab_param_keys, dictComponentsParam)
    
    #checking type of values
    ut.checking_tab_param_rows(tab_param, paramStudyDtypes)
    
    print("checking tab_echantillonnage is done ...")
    print("-------------------------------------------------------------------------------")


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
        run_sensitivity_echantillonnage_manuel(testcase, app_home, tab_param_name, tmax, verifyParamType, timeStepFile=timeStepFile, tsFileList=tsFileList)
        print("end sensitivity")
    else:
        print("Error: check if there is at least one timeseries loaded", flush=True)


  

# -*- coding: utf-8 -*-
"""
@author: ARuby - 20/02/2020
@author: MVallee - 30/10/2020(move into separate files)
"""

import os
import shutil
import sys
from os import path
import pandas as pd
import datetime
import Utilities as ut

import PerseePasteResults as ppr
import HTMLWriter as hw

sys.path.append(os.path.dirname(sys.argv[0]))
script_auto_report_dir = os.path.dirname(__file__)

def saveStudyFiles(projectsPath, TestCase, folder_report, UseCopyMode=True, Name_scenario="", config_extract_name="config_Extract.xml"):
    #TODO: why not providing ScenarioName when calling persee (so no need to copy files) ?!
    list_files = ["_results_PLAN.csv", "_results_HIST.csv", "_results_multiObj.csv",
                  "_model.lp", ".xml", ".json", "_self.json", "_cplex.log", 
                  "_to_change.csv", "_optim.log", "_Results.csv", "_results_SAVE.csv","_results_rollinghorizon.csv", 
                  "_results_EnvImpactMass.csv", "_PortEnvImpactParameters.csv", "_EnvImpactParameters.csv", "_Parameters.csv", 
                  "_solverRunningTime.csv", "_optimalSize.csv"]

    extract_files = ["config_Colors.csv", config_extract_name]  #edited
    # file_name_mst = projectsPath+ut.getOSsep()+TestCase+"_mipstart.mst"
    if os.path.isfile(folder_report):
        os.remove(folder_report)
    if os.path.isdir(folder_report):
        shutil.rmtree(folder_report, ignore_errors=True)
        print("Clean folder_report ", folder_report)
        os.listdir()
        if not os.path.exists(folder_report):
            os.mkdir(folder_report)
            print("Create folder_report again ", folder_report)
    else:
        os.mkdir(folder_report)
        print("Create folder_report ", folder_report)

    isSuccessful = -1; #Used for Run Sensitvity
    if UseCopyMode:
        for f in list_files:
            saveFile(projectsPath + ut.getOSsep() + TestCase + f, folder_report)
        for f in extract_files:
            if (os.path.isfile(projectsPath + ut.getOSsep() + f)):
                saveFile(projectsPath + ut.getOSsep() + f, folder_report)
            elif (os.path.isfile(projectsPath + f)):
                saveFile(projectsPath + ut.getOSsep() + f, folder_report)
            else:
                print("FAIL to save ", f," due to error of path")
    else:
        for f in list_files:
            if f == "_results_PLAN.csv" or f == "_Results.csv": #if PLAN and Results exist, then assume that the case is successful 
                isSuccessful = moveFile(projectsPath + ut.getOSsep() + TestCase + f, folder_report)
            else:
                moveFile(projectsPath + ut.getOSsep() + TestCase + f, folder_report)
        for f in extract_files:
            saveFile(projectsPath + ut.getOSsep() + f, folder_report)
    return isSuccessful


def saveFile(file_name, folder_report):
    if os.path.isfile(file_name):
        # print("Attempt to save ",file_name, "in folder_report ",folder_report )
        shutil.copy(file_name, folder_report)
    else:
        print("FAIL to save ", file_name, os.path.isfile(file_name),
              "in folder_report ", folder_report)


def moveFile(file_name, folder_report):
    if not os.path.isdir(folder_report):
        print("FAIL to access ", os.path.isdir(folder_report)," folder_report ", folder_report)
        return -1

    if os.path.isfile(file_name):
        # print("Attempt to save ",file_name, "in folder_report ",folder_report )
        shutil.move(file_name, folder_report)
        return 0
    else:
        print("FAIL to save ", file_name, os.path.isfile(file_name), "in folder_report ", folder_report)
        return -1


def ExtractResult(projectsPath, TestCase, ConfigCase, folder_report, saveStudy=1, dynamic_csv='', config_extract_name="config_Extract.xml"):
    print("projectsPath  :", projectsPath)
    file_confidential_png = os.path.dirname(path.realpath(__file__)) + ut.getOSsep() + "PerseeDocGen\\confidentiel.PNG"
    file_liten_sty = os.path.dirname(path.realpath(__file__)) + ut.getOSsep() + "PerseeDocGen\\liten.sty"

    ##    folder_report=projectsPath+ut.getOSsep()+"Report_"+ConfigCase
    if saveStudy >= 1:
        print("Saving results to  :", folder_report)
        saveStudyFiles(projectsPath, TestCase, folder_report, True, config_extract_name=config_extract_name)

    if os.path.isfile(file_liten_sty):
        print("Adding liten_sty")
        shutil.copy(file_liten_sty, folder_report)

    if not os.path.isdir(folder_report + ut.getOSsep() + 'FIGURES'):
        print("Adding file_confidential_png")
        os.mkdir(folder_report + ut.getOSsep() + 'FIGURES')
        shutil.copy(file_confidential_png, folder_report + ut.getOSsep() + "FIGURES")
    else:
        shutil.rmtree(folder_report + ut.getOSsep() + 'FIGURES', ignore_errors=True)
        if not os.path.isdir(folder_report + ut.getOSsep() + 'FIGURES'):
            os.mkdir(folder_report + ut.getOSsep() + 'FIGURES')
        print("Adding file_confidential_png")
        shutil.copy(file_confidential_png, folder_report + ut.getOSsep() + "FIGURES")

    if not os.path.isdir(folder_report + ut.getOSsep() + 'TEXPARTS'):
        os.mkdir(folder_report + ut.getOSsep() + 'TEXPARTS')
    else:
        shutil.rmtree(folder_report + ut.getOSsep() + 'TEXPARTS', ignore_errors=True)
        if not os.path.isdir(folder_report + ut.getOSsep() + 'TEXPARTS'):
            os.mkdir(folder_report + ut.getOSsep() + 'TEXPARTS')

    print("Generating Report:")

    file_name = folder_report + ut.getOSsep() + TestCase + "_results_PLAN.csv"
    if not (os.path.isfile(file_name)):
        file_name = folder_report + ut.getOSsep() + TestCase + "_Result_PLAN.csv"
    sumup_name = folder_report + ut.getOSsep() + "SUMUP.csv"
    sumup_name_bak = folder_report + ut.getOSsep() + "SUMUP.bak.csv"

    configfile_name = projectsPath + ut.getOSsep() + config_extract_name  #edited
    graph_properties_list = gw.readGraphProperties(TestCase, ConfigCase, folder_report, configfile_name)

    if os.path.isfile(folder_report + ut.getOSsep() + 'FbsfFramework.log') and 'Unknown' in open(
            folder_report + ut.getOSsep() + 'FbsfFramework.log').read():
        print("Error no solution found for this test ! " + TestCase + " in " + folder_report)
    else:
        print("Does it exist ? ", file_name, folder_report, TestCase)
        if os.path.isfile(file_name):
            if os.path.isfile(sumup_name):
                shutil.copyfile(sumup_name, sumup_name_bak)
                os.remove(sumup_name)
            print("\n- Build graphGeneration ", file_name, flush=True)
            graphGeneration(TestCase, ConfigCase, file_name, dynamic_csv, folder_report, sumup_name,
                            graph_properties_list)

        file_name = folder_report + ut.getOSsep() + TestCase + "_settings.ini"
        print("Does it exist ? ", file_name)
        if os.path.isfile(file_name):
            if not os.path.isdir(folder_report):
                os.mkdir(folder_report)
            print("\n- Build reportGeneration ", file_name, flush=True)
            reportGeneration(TestCase, ConfigCase, file_name, folder_report, graph_properties_list)


def CopyStudy(projectsPath, TestCase, folder_report, config_extract_name="config_Extract.xml"):
    print("projectsPath  :", projectsPath)
    print("Saving results to  :", folder_report)
    saveStudyFiles(projectsPath, TestCase, folder_report, True, config_extract_name=config_extract_name)


def GenerateReport(projectsPath, TestCase, ConfigCase, folder_report, saveStudy=1, dynamic_csv='', config_extract_name="config_Extract.xml"):
    file_confidential_png = os.path.dirname(path.realpath(__file__)) + ut.getOSsep() + "PerseeDocGen\\confidentiel.PNG"
    file_liten_sty = os.path.dirname(path.realpath(__file__)) + ut.getOSsep() + "PerseeDocGen\\liten.sty"
    print("Generating Report:")
    if os.path.isfile(file_liten_sty):
        print("Adding liten_sty")
        shutil.copy(file_liten_sty, folder_report)

    if not os.path.isdir(folder_report + ut.getOSsep() + 'FIGURES'):
        print("Adding file_confidential_png")
        os.mkdir(folder_report + ut.getOSsep() + 'FIGURES')
        shutil.copy(file_confidential_png, folder_report + ut.getOSsep() + "FIGURES")
    else:
        shutil.rmtree(folder_report + ut.getOSsep() + 'FIGURES', ignore_errors=True)
        if not os.path.isdir(folder_report + ut.getOSsep() + 'FIGURES'):
            os.mkdir(folder_report + ut.getOSsep() + 'FIGURES')
        print("Adding file_confidential_png")
        shutil.copy(file_confidential_png, folder_report + ut.getOSsep() + "FIGURES")
    if not os.path.isdir(folder_report + ut.getOSsep() + 'TEXPARTS'):
        os.mkdir(folder_report + ut.getOSsep() + 'TEXPARTS')
    else:
        shutil.rmtree(folder_report + ut.getOSsep() + 'TEXPARTS', ignore_errors=True)
        if not os.path.isdir(folder_report + ut.getOSsep() + 'TEXPARTS'):
            os.mkdir(folder_report + ut.getOSsep() + 'TEXPARTS')
    file_name = folder_report + ut.getOSsep() + TestCase + "_results_PLAN.csv"

    if not (os.path.isfile(file_name)):
        file_name = folder_report + ut.getOSsep() + TestCase + "_Result_PLAN.csv"
    sumup_name = folder_report + ut.getOSsep() + "SUMUP.csv"
    sumup_name_bak = folder_report + ut.getOSsep() + "SUMUP.bak.csv"

    
    configfile_name = projectsPath + ut.getOSsep() + config_extract_name  #edited
    graphProperties_list = gw.readGraphProperties(TestCase, ConfigCase, folder_report, configfile_name)

    if os.path.isfile(folder_report + ut.getOSsep() + 'FbsfFramework.log') and 'Unknown' in open(
            folder_report + ut.getOSsep() + 'FbsfFramework.log').read():
        print("Error no solution found for this test ! " + TestCase + " in " + folder_report)
    else:
        print("Does it exist ? ", file_name, folder_report, TestCase)
        if os.path.isfile(file_name):
            if os.path.isfile(sumup_name):
                shutil.copyfile(sumup_name, sumup_name_bak)
                os.remove(sumup_name)
            print("\n- Build graphGeneration ", file_name, flush=True)
            graphGeneration(TestCase, ConfigCase, file_name, dynamic_csv, folder_report, sumup_name,
                            graphProperties_list)

        file_name = folder_report + ut.getOSsep() + TestCase + "_settings.ini"
        print("Does it exist ? ", file_name)
        if os.path.isfile(file_name):
            if not os.path.isdir(folder_report):
                os.mkdir(folder_report)
            print("\n- Build reportGeneration ", file_name, flush=True)
            reportGeneration(TestCase, ConfigCase, file_name, folder_report, graphProperties_list)


def GenerateHTMLReport(app_home, TestCase, prefix, scenarioList=[], config_file_name="", historplan="PLAN", tab_echantillonnage=""):
    scenarioName = "_".join(scenarioList)
    file_name_desc = app_home + ut.getOSsep() + TestCase + ".json"
    print("Read graph properties ... ")
    if config_file_name == "":
        config_file = app_home + ut.getOSsep() + 'config_Extract.xml'
    else:
        config_file = config_file_name
    xml, sankey_overwrite, sankey_name, liste_graphes_types = hw.readGraphProperties(config_file)

    print("Write Sankey...")
    if sankey_overwrite:
        if sankey_name == "":
            sankey_name = ut.sankey_xml_from_json(file_name_desc, app_home + ut.getOSsep() + scenarioName + 'sankey.xml',
                                                  overwrite=sankey_overwrite, aggregate=True)
            print("Sankey generated in ", sankey_name)
        else:
            sankey_name = ut.sankey_xml_from_json(file_name_desc, app_home + ut.getOSsep() + sankey_name,
                                                  overwrite=sankey_overwrite, aggregate=True)
            print("Sankey generated in ", sankey_name)
    print("liste_graphes_types", liste_graphes_types)

    #Create data dictionary 
    table = {}

    #Generate "ResolCplex" 
    if "ResolCplex" in liste_graphes_types:
        table["ResolCplex"] = ppr.getCplexInfo(app_home, prefix, scenarioList, tab_echantillonnage)


    #Generate "TableDiff" 
    data_param, list_report = ppr.gatherParamInJson(app_home, prefix, scenarioList, tab_echantillonnage)
    data_param.T.to_csv(app_home + "\\allparameters.csv", sep=";", encoding="utf-8")
    if "TableDiff" in liste_graphes_types:
        #In the case where there is only one report drop "TableDiff"
        if len(list_report) > 1:
            table["TableDiff"] = ppr.compare_json(data_param)
        else:
            liste_graphes_types.remove("TableDiff") 
            for element in xml:
                if element["type"] == "TableDiff":
                    print("TableDiff has been dropped because there is only one case!")
                    xml.remove(element)

    #Set the order as in tab_echantillonnage.csv if exists 
    ppr.PasteResultsMonoLoc(app_home, prefix, historplan, scenarioList=scenarioList, list_order=list_report)
    try:
        df = pd.read_csv(app_home + ut.getOSsep() + 'SUMUPALL.csv', sep=';', encoding = "utf-8")
    except UnicodeDecodeError:
        df = pd.read_csv(app_home + ut.getOSsep() + 'SUMUPALL.csv', sep=';', encoding = "ISO-8859-1")
    df_plan = df.set_index("Component.Variable")

    #Generate df_all
    df_diff = pd. DataFrame()
    if "TableDiff" in liste_graphes_types:
        df_diff = table["TableDiff"].transpose()
        df_diff.to_csv(app_home + "\\diff.csv", sep=";", encoding="utf-8")
    if df_diff.empty or not("Case" in df_diff.index):
        print("df diff empty!")
        df_all = df_plan
    else:
        df_diff.columns = df_diff.loc["Case"]
        df_diff = df_diff.drop("Case")
        df_all = pd.concat([df_plan,df_diff])
    df_all_ind = df_all.index.drop_duplicates(keep=False)
    df_all = df_all.loc[df_all_ind]
    df_all.to_csv(app_home+"\\sumupallwithdiff.csv",sep=";", encoding="utf-8")

    #Set "XYGraphMultiStudy"
    if "XYGraphMultiStudy" in liste_graphes_types:
        table["XYGraphMultiStudy"] = df_all

    print("Extract dataseries ...", sankey_name)
    graphProperties = [config_file_name.split('\\')[-1], sankey_name.split('\\')[-1]]  #edited
    ppr.PasteSortieFromGraphProperties(app_home, TestCase, scenarioList, prefix, graphProperties, list_order=list_report)
    print(app_home + ut.getOSsep() + 'SUMUP_TS_' + prefix + '.csv')
    try:
        df_ts = pd.read_csv(app_home + ut.getOSsep() + 'SUMUP_TS_' + prefix + '.csv', sep=';', encoding = "utf-8")
    except UnicodeDecodeError:
        df_ts = pd.read_csv(app_home + ut.getOSsep() + 'SUMUP_TS_' + prefix + '.csv', sep=';', encoding = "ISO-8859-1")

    print("Generate figures ...")
    figures = hw.generate_figures(xml, df_all, df_ts, app_home, tables=table)

    print("Write HTML...")
    #Create a new dir for HTML report
    timestamp = datetime.datetime.now().strftime("%Y.%d.%m_%Hh%Mm%Ss")
    html_report_dir = app_home + ut.getOSsep() + "html_report_" + TestCase + "_" + timestamp
    os.mkdir(html_report_dir) 
    if os.path.isfile(app_home + ut.getOSsep() + "plotly.min.js") == True:
        shutil.copy(app_home + ut.getOSsep() + "plotly.min.js", html_report_dir + ut.getOSsep() + "plotly.min.js")
    else:
        shutil.copy(os.getenv('CAIRN_APP') + ut.getOSsep() + 'Scripts' + ut.getOSsep() + 'PerseeDocGen' + ut.getOSsep() + 'plotly.min.js',
                    html_report_dir + ut.getOSsep() + "plotly.min.js")
    name_html = html_report_dir + ut.getOSsep() + 'report' + timestamp + '.html'
    hw.to_HTML(figures, name_html)
    return name_html


if __name__ == '__main__':
    if len(sys.argv) > 6:  # ne pas modifier ! appel par PerseeGUI
        #print(sys.argv)
        app_home = sys.argv[1]
        testcase = sys.argv[2]
        prefix = "Report"
        scenarioName = sys.argv[3] 
        scenarioList = []
        if len(scenarioName) > 0:
            scenarioList = scenarioName.split(';')
        staticStudy = int(sys.argv[4])
        configFileName = sys.argv[5]
        colorfile = "config_Colors.csv"

        if configFileName != "methodCopyStudy":
            if configFileName != '':
                print("use config file", configFileName)
            else:
                configFileName = "config_Extract.xml"
            print("prefix: ", prefix)
            if staticStudy: #latex
                if len(scenarioName) > 0:
                    print("scenario:", scenarioName)
                else:
                    print("scenario: . main dir")
            else: #html
                if len(scenarioName) > 0:
                    print("scenarios:", scenarioList)
                else:
                    print("scenarios: * all scenarios")
        tab_echantillonnage_file = sys.argv[6]
        #if tab_echantillonnage_file == "":
        #    tab_echantillonnage_file = "tab_echantillonnage.csv"
    else:
        app_home = r"D:\Users\PP265749\git\PerseeGui\Test_Persee\formation_persee"
        testcase = "formation_persee" 
        prefix = "Report_s"
        scenarioList = []       
        #saveStudy = 1
        studyName = "frompycharm"
        latexExport = False
        staticStudy = False
        reportFolder = ""
        sys.path.append(os.path.dirname(app_home + ut.getOSsep()))
        configFile = "config_Extract.xml"
        configFileName = "config_Extract.xml"
        colorfile = "config_Colors.csv"

    print("Module sys searching Path is ", sys.path)
    config_file = app_home + ut.getOSsep() + configFileName
    if os.path.isfile(config_file):
        print("Read: ", config_file)
    else:
        shutil.copy(script_auto_report_dir + ut.getOSsep() +'config_Extract_example.xml',config_file)
    if os.path.isfile(app_home + ut.getOSsep() + colorfile) == False:
        shutil.copy(script_auto_report_dir + ut.getOSsep() + 'config_Colors_example.csv',colorfile)
    if os.path.isfile(app_home + ut.getOSsep() + "plotly.min.js") == False:
        shutil.copy(script_auto_report_dir + ut.getOSsep() + 'plotly.min.js',
                    app_home + ut.getOSsep() + "plotly.min.js")

    if configFileName == "methodCopyStudy": #copy result files 
        CopyStudy(app_home, testcase, "Report_"+scenarioName) # scenarioName == folder name
    elif staticStudy: #latex report
        if scenarioName == "":
            folder_report = app_home + ut.getOSsep()  
            dynamic_csv = app_home + ut.getOSsep() + testcase + "_Sortie.csv"
        else:
            folder_report = app_home + ut.getOSsep() + prefix + '_' + scenarioName
            dynamic_csv = app_home + ut.getOSsep() + prefix + '_' + scenarioName + ut.getOSsep() + testcase + "_Sortie.csv"

        GenerateReport(app_home, testcase, scenarioName, folder_report, dynamic_csv=dynamic_csv, config_extract_name=config_file)
    else: #html report
        htmlFile = GenerateHTMLReport(app_home, testcase, prefix, scenarioList=scenarioList, config_file_name=config_file, historplan="PLAN", tab_echantillonnage=tab_echantillonnage_file)
        #open the html file using cmd windows
        #os.system("start " + htmlFile)
        os.system('start "" "'+ htmlFile+'"')

# -*- coding: utf-8 -*-
"""
Created on Tue Dec 19 10:21:24 2017

@author: AR170699 from https://codereview.stackexchange.com/questions/109770/python-script-to-delete-sections-of-text
"""

import csv
import os
import sys
import re
import pandas as pd
sys.path.append(os.path.dirname(sys.argv[0]))
cairn_path = os.path.dirname(sys.argv[0])+"//..//..//"
sys.path.append(cairn_path)
print(sys.path)
print("Module sys searching Path is ",sys.path)

from codeAnalyzer import extract_str
from codeAnalyzer import extractComment
from codeAnalyzer import getParameter
from codeAnalyzer import getVariables
from codeAnalyzer import getMethods
from codeAnalyzer import getComponentParameters
from codeAnalyzer import analyseBloc


global deleted_lines, total_lines

deleted_lines = 0
total_lines = 0

def write_title(title,underline_char,double_titre= False):
    if double_titre:
        return(underline_char*len(title) + "\n" +title + "\n" + underline_char*len(title)+"\n\n")
    else:
        return(title + "\n" + underline_char*len(title)+"\n\n")

def filter_lines_rst(f, modelname,folder_csv,list_param): # filtre le fichier .cpp
    lines = f.readlines()
    varDico, exprList, lines_to_ignore = getVariables(lines)
    var_table = pd.DataFrame.from_dict(varDico, orient = 'index', columns = ["dimension","Associated Expr" ,"size", "varMin", "varMax", "varType"])
    modelnameUsed=modelname#.replace("\_","")
    var_table.to_csv(folder_csv+modelnameUsed+"varList.csv",sep=",",index_label="Variable name")
    
    
    yield write_title("Model variables of "+ modelnameUsed, "^")

    yield ".. csv-table:: " + modelnameUsed + " variables table"+"\n"
    yield "    :file: ../csv/" + modelnameUsed + "varList.csv"+"\n"
    yield "    :header-rows: 1"+"\n\n"
    
    methods = getMethods(lines)
    """
    yield write_title("Model equations of "+ modelnameUsed,"^")

    #yield "\label{" + modelnameUsed + "Equations}\n"
    # if 'gms' in modelnameUsed:
    #     return
    text = ''
    if 'buildModel' in methods.keys():
        yield write_title("Build model "+modelnameUsed, "*")
        yield analyseBloc(methods['buildModel'],1,varDico,exprList,list_param,lines_to_ignore = lines_to_ignore)
    for method in methods.keys():
        if method!='buildModel':
            yield write_title(method+ " method of "+modelnameUsed, "*")
            yield analyseBloc(methods[method],1,varDico,exprList,list_param)
    text += '\n'
    yield text
    """

def formatParamDeclarationLine(line):
# Discard all lines up to and including the stop marker
    key, args, val = getParameter(line)
    print(key,args,val)
    key = key#.replace(r'\\_',r"\_")
    try:
    #   line=key+" & "+val[5]+" & "+val[0]+" & " + val[1] + "\\\\\n"
        line = [key,val[5],val[0], val[1] ]
    except TypeError:
    #   line=key+" & "+str(val[5])+" & "+str(val[0])+" & " + str(val[1]) + "\\\\\n"
        line = [key, str(val[5]), str(val[0]), str(val[1])]
    return line

def h_filter_lines(f, start_delete, stop_delete, modelname,folder_csv):
    """
    Given a file handle, generate all lines except those between the specified
    text markers.
    """
    global deleted_lines
    lines = iter(f)
    deleted_lines_save = deleted_lines
    iHead=0

    modelnameUsed=modelname#.replace("\_","")
    list_Head=[]
    list_IO=[]
    list_TS=[]
    list_RH=[]
    list_param=[]
    list_Data=[]
    list_paramConfig=[]
    list_DataConfig=[]
    declareModelConfigurationParameters=-1
    declareModelInterface=-1
    declareModelParameters=-1

    ## parse the header file looking for configuration params, params, data, input time series and data from component
    ##sorting out params, input and output by type
    try:
        while True:
           # print ("Looking for start_delete ",start_delete," stop_delete ",stop_delete, " Line ",line )
            line = next(lines)
            line=line#.replace("_","\_")
            line=line.replace("&&", "and")
            line=line.replace("||", "or")
            if ("DO NOT SHOW" not in line):
                if ("* \details" in line):
                    iHead=1
                if (iHead==1 and "*/" in line) :
                    iHead=0
                if ("void declareModelConfigurationParameters" in line and not "=0" in line):
                    declareModelConfigurationParameters = 1
                if (declareModelConfigurationParameters > 0 and "{" in line and not "}" in line):
                    declareModelConfigurationParameters += 1
                if (declareModelConfigurationParameters > 0 and "}" in line and not "{" in line):
                    declareModelConfigurationParameters -= 1
                if (declareModelConfigurationParameters>=0 and "}" in line and not "{" in line):
                    declareModelConfigurationParameters = -1

                if ((iHead == 1)):
                    line = format_HeadPart(line, list_Head)
                if ("addIO" in line and not "addIOs" in line):
                    # Discard all lines up to and including the stop marker
                    linedesc=extract_str(line, "\/\*\*", "\*\/")
                    imode = 0
                    if ("addIO " in line or "addIO1d" in line):
                        imode=1
                        line=line.replace("addIO1d","addIO")
                    line=extract_str(line, "addIO", "\;")
                    line=line.replace("&","")
                    if (imode==1):
                        line=line.replace(",","& & ",1)
                    else:    
                        line=line.replace(",","&",2)
                    line=line.replace(","," , ")
                    line=line.replace('"',"")
                    line=line.replace(")","")
                    line=line.replace("(","")
                    line=line+" & "+linedesc
                    line=line+"\\\\\n"
                    list_IO.append(line.split("&"))
                if ( "InputData->addParameter" in line and not "DO NOT SHOW" in line):
                    if (declareModelConfigurationParameters<=0):
                        list_Data.append(formatParamDeclarationLine(line))
                    else:
                        list_DataConfig.append(formatParamDeclarationLine(line))
                if ( "InputParam->addParameter" in line and not "DO NOT SHOW" in line):
                    if (declareModelConfigurationParameters<=0):
                        list_param.append(formatParamDeclarationLine(line))#.replace("\n","").split("&"))
                    else:
                        list_paramConfig.append(formatParamDeclarationLine(line))#.replace("\n","").split("&"))
                        #print("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv",formatParamDeclarationLine(line).replace("\n","").split("&"))
                if ( "InputDataFull->addParameter" in line and not "DO NOT SHOW" in line):
                    list_RH.append(formatParamDeclarationLine(line))
                if ( "InputDataTS->addParameter" in line and not "DO NOT SHOW" in line):
                    list_TS.append(formatParamDeclarationLine(line))
    except UnicodeDecodeError:
        print ("UnicodeError detected - abort automatic treatment for this file")
        deleted_lines = deleted_lines_save
        pass
    except StopIteration:
        pass
    return(list_Head,list_IO,list_TS,list_RH,list_param,list_Data,list_paramConfig,list_DataConfig)




def write_table(tableName, table_description, modelnameUsed, list_Head, list_table,folder_csv, description = ""):
    print("write ", tableName, " of ", modelnameUsed)
    print(list_table, list_Head)
    # try:
    if len(list_table)>0:
        paramConfig_table = pd.DataFrame(list_table, columns = list_Head)
        paramConfig_location = folder_csv+modelnameUsed+tableName+".csv"
        os.makedirs(folder_csv,exist_ok=True)
        paramConfig_table.to_csv(paramConfig_location,sep=",", index=False)
        

        yield write_title(table_description + " of "+ modelnameUsed,'^')

        yield description + "\n\n"

        yield "See also :ref:`"+table_description + " of TechnicalSubModel"+"` for generic options\n\n"

        yield ".. csv-table:: " + modelnameUsed +"\n"
        if modelnameUsed in ["TechnicalSubModel"]:
            yield "    :file: csv/"+modelnameUsed +tableName+".csv\n"
        else:
            yield "    :file: ../csv/"+modelnameUsed +tableName+".csv\n"
        yield "    :header-rows: 1"+"\n\n"


def format_HeadPart(line, list_Head):
    line = line.replace("/**", "")
    line = line.replace("*", "")
    line = line.replace("\details", "")
    #line = line.replace("\\", "")
    #line = line.replace("begin{itemize}", "\\begin{itemize}")
    #line = line.replace("item ", "\\item ")
    #line = line.replace("textbf", "\\textbf")
    #line = line.replace("end{itemize}", "\\end{itemize}")
    #line = line.replace("*", "")
    #line = line.replace("&", "\&")
    #line = line.replace(";", "\\\\\n")
    #line = line.replace("_", r"\_")
    if (not "file" in line):
        list_Head.append(line)
    return line



def nb_lines(f):
    """
    Given a file handle, generate all lines except those between the specified
    text markers.
    """
    lines = iter(f)
    igrep = 0
    try:
        while True :
#            print ("Looking for start_delete ",start_delete," stop_delete ",stop_delete, " Line ",line )
            line = next(lines)
            igrep = igrep+1
    except StopIteration:
        f.seek(0)
        return igrep
    except UnicodeDecodeError:
        f.seek(0)
        return igrep


def make_toc(directory,models_list):
    "creates table of contents for all the models in each folder"
    for i in (models_list.keys()):
        rst_file = directory+i+"_toc.rst"
        with open( rst_file, 'w') as o:
            o.write(rst_file+"\n\n")
            o.write(".. toctree::\n")
            o.write("   :maxdepth: 1\n\n")
            for j in models_list[i]:
                o.write("   "+i+"/"+j+"\n")

def redact_files(perseegui_dir, directory_name0, dest_folder):
    """
    Write a documentation of all the models from the code .cpp and .h.
    """
    global deleted_lines, total_lines
    start_delete="str1"
    stop_delete="str2"
    total_files = 0
    nb_files = 0 
    dest_folder_path = perseegui_dir + dest_folder
    directory_name=perseegui_dir+directory_name0
    print ("In redact_files ",directory_name)
    models_list = dict()
    for dirpath, dirnames, filenames in os.walk(directory_name):
        print("dirpath: ", dirpath)
        print("dirnames: ", dirnames)
        print("filenames:", filenames)
        for file in filenames:
            if 'cpp' in file:
                sub_directory = dirpath.replace(directory_name,"")
                print("dirname i :",sub_directory)
                if sub_directory.replace("\\","") not in list(models_list.keys()): # add the model to the list to append it to the toc
                    models_list[sub_directory.replace("\\","")] = []
                models_list[sub_directory.replace("\\","")].append(file.replace(".cpp",''))
                rst_file =  dest_folder_path + "/models/" + sub_directory+"/" +file.replace(".cpp",".rst")
                cfile=file.replace(".h",".cpp")
                hfile=file.replace(".cpp",".h")
                subsection=file.replace('.cpp','')#.replace('_','\_')
                # wait = input("subsection "+ subsection)
                os.makedirs(dest_folder_path + "/models/" + sub_directory, exist_ok=True)				
                with open( rst_file, 'w') as o:
                    total_files = total_files + 1
                    o.write(write_title(subsection,'~'))
                    ref = file.replace('.cpp','').replace('_','')
                    # o.write("\n\n.._"+ref+"\n\n")
                    o.write(write_title("Model interface of "+subsection,"^"))
                    #  read the .h files
                    with open(os.path.join(dirpath, hfile), 'r+') as h:
                        total_lines=total_lines+nb_lines(h)
                        deleted_lines_save = deleted_lines
                        list_head, list_IO,list_TS,list_RH,list_param,list_Data,list_paramConfig,list_DataConfig = h_filter_lines(h, start_delete, 
                                                                                                                       stop_delete,subsection ,
                                                                                                                       dest_folder_path + "/models/csv/")
                        #  todo : a refaire avec l'API plus tard.
                        filtered = list(write_table("paramConfig", "Parameters of configuration ", subsection, 
                                                    ["Parameter name","Unit","Mandatory","Description"], 
                                                    list_paramConfig, dest_folder+ "/models/csv/", 
                                                    description=""))
                        filtered += list(write_table("paramList", "Parameter table ", subsection, 
                                                    ["Expression name","Unit","Mandatory","Description"], list_param, dest_folder_path+ "/models/csv/"))
                        filtered += list(write_table("listIO", "Expressions table ", subsection, 
                                                   ["Expression name","Unit","Mandatory","Description"], list_IO, dest_folder_path+ "/models/csv/"))                        
                        filtered += list(write_table("listTS", "Time series table ", subsection, 
                                                    ["Input time series name","Unit","Mandatory","Description"], list_TS, dest_folder_path+ "/models/csv/"))
                        filtered += list(write_table("list_RH", "Rolling horizon expressions ", subsection, 
                                                    ["Expression name","Unit","Mandatory","Description"], list_RH, dest_folder_path+ "/models/csv/"))
                        filtered += list(write_table("list_Data", "Additionnal data needed by the model ", subsection, 
                                                    ["Data name","Unit","Mandatory","Description"], list_Data, dest_folder_path+ "/models/csv/"))
                        
                        print ("- Writing filtered file : ",hfile)
                        nb_files = nb_files + 1
                        o.writelines(list_head)
                        o.writelines("\n")
                        o.writelines(filtered)
                        o.flush()
                    total_files = total_files + 1
                    list_param_and_conf = ["m"+i[0] for i in list_param] + ["m"+i[0] for i in list_paramConfig] + ["m"+i[0] for i in list_TS]
                    #  read the .cpp files
                    with open(os.path.join(dirpath, cfile), 'r+') as f:
                        total_lines=total_lines+nb_lines(f)
                        filtered = list(filter_lines_rst(f, subsection,dest_folder_path+ "/models/csv/", list_param_and_conf))
                        print ("- Writing filtered file : ",cfile)
                        nb_files = nb_files + 1
                        o.writelines(filtered)
                        o.flush()
    make_toc(dest_folder_path + "/models/",models_list)
    if (total_files >0):
        print (" Handled ",nb_files," files over ", total_files, " total files exmined, that is ",100*nb_files/total_files,"%")
    if (total_lines >0):
        print (" Deleted ",deleted_lines," lines over ", total_lines, " total lines exmined, that is ",100*deleted_lines/total_lines,"%")

    #
    print("directory_name is",directory_name)
    #writeSumUptoTex("", "",perseegui_dir+"/lib/export/Persee/doc/SubModelsSumUp.csv",perseegui_dir+"/lib/export/Persee/doc/HTML_LATEX/input/")
    csv_folder = dest_folder+ "/models/csv" 
    # generate_components_documentation(directory_name,csv_folder)

def outputComponentParametersTable2(text, paramDico, directory_name, subsection,folder_csv):
    print ("- Writing file : " + subsection + ".tex")
    text += write_title("Component Configuration Parameters of "+ subsection,"-")
    # folder_csv = directory_name+"/../doc/HTML_LATEX/input/csv_param"
    
    var_table = pd.DataFrame.from_dict(paramDico, orient = 'index', columns = ["User Access Name", "Mandatory", "Description", "Possible values"])
    modelnameUsed=subsection#.replace("\_","")
    var_table.to_csv(folder_csv+modelnameUsed+"paramList.csv",sep=",")

    yield ".. csv-table:: Table of Configuration Parameters for component "+subsection+"\n"
    yield "  :file: "+"../csv/"+modelnameUsed+"paramList.csv"
    yield "  :header-rows: 1"

                    
def generate_components_documentation(directory_name,csv_folder):
    print ("===========================================")
    print ("Components documentation")
    print ("===========================================")
    #read common parameter in MilpComponent.cpp file 
    f = open(directory_name+'/../core/base/MilpComponent.cpp')
    lines = f.readlines()
    # lines = lines.replace("_",r"\_")
    paramDicoCommon = getComponentParameters({},lines,False)
    componentList = ['GenConverterCompo','StorageCompo','GridCompo','SourceLoadCompo','SourceENRCompo','EnergyVector',
                      'BusFlowBalance', 'BusSameValue', 'MultiObjCompo', 'Solver', 'TecEcoAnalysis', 'ModelType']
    for dirpath, dirnames, filenames in os.walk(directory_name):
        for file in filenames:
            if file.endswith('.cpp') and any(x in file for x in componentList) and not 'moc_' in file:
                #read component description in header file  
                hfile=file.replace(".cpp",".h")
                try:
                    with open(os.path.join(dirpath, hfile), 'r+') as f:
                        lines = f.readlines()
                    i=0
                    textBloc = ''
                    while i < len(lines):
                        # if "Solver" in file:
                        #     print(lines[i])
                        # lines[i] = lines[i].replace("_",r"\_")
                        if lines[i].lstrip().startswith('/**') and '\\brief' in lines[i+1]:
                            textBloc,i = extractComment(i,lines)
                            break
                        i+=1
                    text = write_title("Brief Description",'-')
                    text += textBloc
                    #read component parameters in cpp file
                    with open(os.path.join(dirpath, file), 'r+') as f:
                        lines = f.readlines()
                    paramDicoAll = paramDicoCommon.copy() # MilpComponent parameters added to the list
                    paramDicoAll.update(getComponentParameters(paramDicoAll,lines,True))
                    component=file.replace('.cpp','')#.replace('_','\_')
                    outputComponentParametersTable2(text, paramDicoAll, directory_name, component,csv_folder )
                except:
                    print("file "+os.path.join(dirpath, hfile) + " not found")

    return

if __name__ == '__main__':
    if len(sys.argv) > 3:
        redact_files(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        redact_files(cairn_path, "//src//models", "//doc//user//")
        redact_files(cairn_path, "//src//core//submodels", "//doc//user//")

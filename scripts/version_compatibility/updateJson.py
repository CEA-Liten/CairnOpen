# -*- coding: utf-8 -*-
"""
Created on Mon Jan  9 09:45:52 2023

@author: PP265749
"""
import os
import json
import pandas as pd

def updateToV3point4(filename: str,table_to_replace):
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
    with open(filename, 'r') as study_file:
        parser = json.load(study_file)
    changes = pd.read_csv(table_to_replace,sep=";")
    c = 0
    for compo in ((parser["Components"])):
        if (compo["nodeType"]) in list(changes["nodeType"]):
            print("change ",compo["nodeName"], "to componentPERSEEType",list(changes[changes["nodeType"]==compo["nodeType"]]["componentPERSEEType"])[0] )
            compo["componentPERSEEType"]=list(changes[changes["nodeType"]==compo["nodeType"]]["componentPERSEEType"])[0]
            #parser[c] = compo
            c+=1
        else:
            print("do not change",compo["nodeName"])
    json_obj = json.dumps(parser, indent=4)
    with open(filename, "w") as outfile:
        outfile.write(json_obj)
    return(parser)


filename = "test_bouin_7_cont\\bouin_7_cont.json"


table_to_replace = "tableUpdatetov34.csv"
updateToV3point4(filename,table_to_replace)

filename  = "test_bouin_7_cont_controle\\bouin_7_cont_controleNew.json"
updateToV3point4(filename,table_to_replace)

filename  = "test_MultiObjectif\\multiobj.json"
updateToV3point4(filename,table_to_replace)
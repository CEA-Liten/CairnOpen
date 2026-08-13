# -*- coding: utf-8 -*-

import os
import sys
import pandas as pd
sys.path.append(os.path.dirname(sys.argv[0]))
cairn_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(cairn_path)

try:
    import cairn as crn    
except:
    import cairnopen as crn
    

PortTable = [ "name"]
VarTable = [ "name"]
ParamTable = [ "name", "description", "isMandatory"]

def addResults(resTable : pd.DataFrame, values : list)->pd.DataFrame:
    if resTable.empty:
        resTable = pd.DataFrame([values], columns=resTable.columns)
    else:
        resTable = pd.concat([pd.DataFrame([values], columns=resTable.columns), resTable],  ignore_index=True)
    return resTable


def writeTable(destPath : str, component , defTable : list, suffix : str=''):
    
    resTable = pd.DataFrame(columns=defTable)
    params = component.settings.copy()
    params.sort(reverse=True)

    for param in params:       
        pparam = component.get_setting(param)    
        pvalues = []
        for pt in defTable:                    
            pvalues.append(getattr(pparam,pt))
        resTable = addResults(resTable, pvalues)
    resPath = os.path.join(destPath, component.name+suffix+'.csv')
    resTable.to_csv(resPath, index=False)

def writeVars(destPath : str, component , defTable : list, suffix : str=''):
    
    resTable = pd.DataFrame(columns=defTable)
    params = component.variables.copy()
    params.sort(reverse=True)

    for param in params:       
        pvalues = param
        # pparam = component.get_setting(param)    
        # pvalues = []
        # for pt in defTable:                    
        #     pvalues.append(getattr(pparam,pt))
        resTable = addResults(resTable, pvalues)
    resPath = os.path.join(destPath, component.name+suffix+'.csv')
    resTable.to_csv(resPath, index=False)


def writePorts(destPath : str, component , defTable : list, suffix : str=''):    
    resTable = None
    ports = component.default_ports
    for port in ports:    
        p = component.get_port(port) 
        if resTable is None:
            c = defTable + p.settings
            resTable = pd.DataFrame(columns=c)
                        
        pvalues = []
        for pt in defTable:                    
            pvalues.append(getattr(p,pt))
        params = p.settings        
        for param in params:       
            pparam = p.get_setting(param) 
            pvalues.append(pparam.value)
        resTable = addResults(resTable, pvalues)
    resPath = os.path.join(destPath, component.name+suffix+'.csv')
    resTable.to_csv(resPath, index=False)


def doDoc(destPath : str):

    csvPath = os.path.join(cairn_path, destPath, "csv")
    os.makedirs(csvPath, exist_ok=True)

    cairnAPI = crn.CairnAPI()
    study = cairnAPI.create_study("doc")    
    models = cairnAPI.all_models    
    cMaterial = study.create_energy_carrier("cMaterial", "Material")
    cElectrical = study.create_energy_carrier("cElectrical", "Electrical")

    for model in models:    
        if model!='MyModel':
            try: 
                component = study.create_component(model, model)
                ports = component.default_ports       
                for port in ports:
                    p = component.get_port(port)
                    #pCarrierType = p.get_setting_value("carrierType")
                    #if pCarrierType == "Electrical":
                    #    p.set_carrier(cElectrical)
                    #else:
                    p.set_carrier(cMaterial)
                    #writeTable(destPath, p, PortTable)   
                writePorts(csvPath, component, PortTable, 'Ports')                                
            except:
                # it's a bus
                try:
                    component = study.create_bus(model, model, cMaterial)
                except:
                    pass
                         
            writeTable(csvPath, component, ParamTable, 'paramList')
            writeVars(csvPath, component, VarTable, 'listIO')
           
    pass
    


if __name__ == '__main__':
    doDoc("..//..//doc//user//models")
    
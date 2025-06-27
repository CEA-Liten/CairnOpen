
from io import TextIOWrapper
import pandas as pd
import numpy as np

MAXERR = 100000 # erreur si string différentes


def checkPLAN(file_plan_ref : str, file_plan : str, logs : TextIOWrapper, threshold : float) -> float: 
    #  Comparaison de 2 fichiers de type PLAN
    #
    # file_plan_ref : nom complet du fichier 1 à comparer
    # file_plan : nom complet du fichier 2 à comparer
    #
    # logs : fichier de logs (a été ouvert)
    # threshold : si l'erreur est au dessus de ce seuil, affichage dans le log
    #
    # retourne la plus grande difference

    dash = '-' * 40
    logs.write(dash+'\n')
    logs.write('FILES\n') 
    logs.write(dash+'\n')
    logs.write('Filename reference = ' + file_plan_ref+'\n')
    logs.write('Filename resultats = ' + file_plan+'\n\n')
 
    logs.write(dash+'\n')
    logs.write('DIFFERENCES\n') 
    logs.write(dash+'\n')
    logs.write('Variable name : reference value, result value, err\n') 

    valuesRef = getPLAN(file_plan_ref)
    values = getPLAN(file_plan)

    maxErr = 0
    noFind = list()
    find = list()
    for ref in valuesRef:
        err = diffLinePLAN(ref, values, logs, threshold)
        if err<0:
            #variable non trouvée
            noFind.append(ref[0].replace('$$', ','))
        else:
            maxErr = max(maxErr, err)
            find.append(ref[0])

    if len(noFind)>0:
        logs.write('\n')
        logs.write(dash+'\n')
        logs.write('Variables in reference, but NOT in results\n') 
        logs.write(dash+'\n')
        for v in noFind:
            logs.write(v+'\n')
        maxErr = MAXERR #if a variable is in reference, but NOT in results, returns "they differ" for safety!

    if len(find)<len(values):
        logs.write('\n')
        logs.write(dash+'\n')
        logs.write('Variables NOT in reference, but in results\n') 
        logs.write(dash+'\n')
        for v in values:
            if v[0] not in find:
                logs.write(v[0].replace('$$',',')+'\n')

    logs.write('\n')
    return maxErr
  


def diffLinePLAN(ref, values, logs : TextIOWrapper, threshold : float) -> float:
    # recherche la reference dans les résultats, si trouvée -> comparaison
    # retourne l'erreur: positive si différence, nulle si identique, négative si non trouvée
    err = -1    
    for v in values:
        if (ref[0].replace(" ", "") == v[0].replace(" ", "")):
            # la variable a été touvée, comparaison
            err = diffValuesPLAN(ref[1], v[1])
            if err < 0 or err > threshold:                
                logs.write(ref[0].replace('$$', ',') + ': ' + str(ref[1]) + ", " + str(v[1]) + ", ")
                if err<0:
                    # erreur str
                    logs.write("Differences of string")
                    err = MAXERR
                elif err>0:
                    logs.write(str(err))

                logs.write('\n')            
            break  
         
    return err


def diffValuesPLAN(ref:str, value:str, eps=1e-6) -> float:
    # comparaison entre la valeur de référence (ref) et le résultat (value)
    # retourne l'erreur: positive si différence, nulle si identique, 
    # négative si il s'agit d'une comparaison entre chaines de caractères   
    err = 0
    v_ref = getFloat(ref)
    v_res = getFloat(value)
    if not isinstance(v_ref, str) and not isinstance(v_res, str):
        if v_ref != v_res:    
            if (v_ref == 0.0 or v_ref == 1.0) and (v_res == 0.0 or v_res == 1.0):
                #Binary indicators
                err = -1
            else:
                #Non-Binary indicators
                if abs(v_ref) < eps:
                       v_ref = 1.0                
                err = abs((v_res-v_ref)/v_ref)*100.0              
    elif isinstance(v_ref, str) and isinstance(v_res, str):
        # string
        if ref != value:
            err = -1
    else:
        err = -2
    
    return err


def getFloat(value):
    
    try:
        v = float(value)
    except :
        v = ''
    return v


def getPLAN(fileName : str):

    rs = pd.read_csv(fileName, 
                     sep=';', 
                     decimal='.',      
                     on_bad_lines='warn',
                     index_col = [0,1],
                     skip_blank_lines=True,                    
                     na_values=['nan'], 
                     na_filter=True)

    names = rs.axes[0].to_flat_index().tolist()
    values = rs.values.tolist()
   
    y = [('$$'.join(x), v[0]) for x, v in zip(names, values) if isinstance(x[0], str) and x[0].strip()!='**']
    
    return y


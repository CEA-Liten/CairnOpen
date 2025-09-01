 # -*- coding: utf-8 -*-

import os
import sys

def generate_modelTypes_file(srcDir, outputFile, includePrivateModels=True):
    print("======================================================")
    print("Generation of file" + outputFile)
    print("======================================================")

    print("Option WITH_PRIVATEMODELS: " + str(includePrivateModels))

    modelTypes = {}

    modelsDirList = [modelsDir]
    if includePrivateModels:
        modelsDirList.append(privateModelsDir)

    for mdir in modelsDirList:
        print("Looking for models in: " + srcDir + mdir)
        for dirpath, dirnames, filenames in os.walk(srcDir + mdir):
            for cfile in filenames: #FileNameLoop
                if "debug" in dirpath.lower() or "release" in dirpath.lower():
                    break #FileNameLoop
                if cfile.endswith('.cpp'):  
                    #Filter for models 
                    hfile = cfile.replace('.cpp', '.h')
                    if not os.path.exists(os.path.join(dirpath, hfile)): 
                        continue #Not a candidate file for a model

                    model_name = cfile.replace('.cpp', '')

                    #Obtain type from path
                    model_type = os.path.basename(dirpath)
     
                    #Add model to the dictionary
                    modelTypes[model_name] = model_type

    #Write data to file
    exportData(modelTypes, outputFile)

    print(modelTypes)

    print("======================================================")
    print("                          Done")
    print("======================================================\n")
 
    return

def exportData(data, filename):
    with open(filename, 'w') as file:
        for key in data:
            file.write(key+","+data[key]+"\n") #key,value per line

if __name__ == '__main__':

    WITH_PRIVATEMODELS = True

    if len(sys.argv) > 1 and sys.argv[1] != "ON":
        WITH_PRIVATEMODELS = False

    global modelsDir
    global privateModelsDir

    modelsDir = r"\models"
    privateModelsDir = r"\privateModels"

    generate_modelTypes_file(r"..\src", r"..\resources\modelTypes.txt", WITH_PRIVATEMODELS)

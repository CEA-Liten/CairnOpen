# -*- coding: utf-8 -*-
"""
Created on Tue Dec 19 10:21:24 2017

@author: AR170699 from https://codereview.stackexchange.com/questions/109770/python-script-to-delete-sections-of-text
"""

import csv
from ctypes.wintypes import DOUBLE
import os
import sys
import re
import copy

subModelsDir = r"\core\submodels"
modelsDir = r"\models"
privateModelsDir = r"\privateModels"

def extract_str(text, str1, str2):
    """
    Extracts a substring from the given text that is found between two specified strings.

    Args:
        text (str): The original string to search within.
        str1 (str): The starting delimiter string.
        str2 (str): The ending delimiter string.

    Returns:
        str: The substring found between str1 and str2.
             If the delimiters are not found, returns an empty string.
    """
    try:
        strsearch = str1 + '(.+?)' + str2
        found = re.search(strsearch, text).group(1)
    except AttributeError:
        found = ''
    return found


def extractBetweenParentheses(s):
    """
    Extracts the substring found between the first matching pair of parentheses.
    
    Returns an empty string if no valid pair is found.
    """
    # regex cannot process both 'if (t(i))' and 'if (i>0) t(i)=1;'
    j = 0
    ideb = 0
    for i, c in enumerate(s):
        if c == ')':
            j -= 1
            if j == 0:
                break
            if j < 0:
                return ''
        elif c == '(':
            if j == 0:
                ideb = i + 1
            j += 1
    return s[ideb:i]

def linePreprocessing(line): 
    line = line.replace('\t', '    ')
    line = line.split('//')[0]
    line = line.replace('mModel->MIPModeler::MIPModel::add','mModel->add')
    line = line.replace('MIPModeler::MIPConstraint','')
    line = line.replace('MIPModeler::MIPExpression','MIPExpression')
    return line

def lineFormatting(line, varDico,exprList,list_param): 
    line = ' '.join(line.split()) #Substitute multiple whitespace with single whitespace
    line = line.replace('{','') #no brace in output string
    line = line.replace('}','') #no brace in output string
    line = line.replace('0.f','0')
    if line[-1] == ';':
        line = line[:-1]
    line = line.replace('std::min<unsigned int>','min')
    line = line.replace('for (int ','for (')
    line = line.replace('uint64_t ','')
    line = line.replace('unsigned int','')
    line = line.replace('const double*','')
    line = line.replace('double ','')
    line = line.replace('std::','')
    line = line.replace('MIPModeler::MIPPiecewiseLinearisation',' \\textbf{\\textcolor{blue}{MIPModeler::MIPPiecewiseLinearisation}}')
    line = line.replace('MIPModeler::isConvexSet',' \\textbf{\\textcolor{blue}{MIPModeler::isConvexSet}}')

    for var in list(varDico.keys())+exprList:
        try:
            line = re.sub(r'\b'+var+r'\b',r"**"+var+"** ",line) # whole word only
        except:
           print("Error in regex")     
    # list_param2 = ["m"+i[0] for i in list_param]
    for param in list_param:
        line = re.sub(r'\b'+param+r'\b',r"*"+param+"* ",line) # whole word only
    # line = line.replace("_","\_")
    line = line.replace("&","\&")
    line = line.replace("%","\%")
    if ((line.replace("+","").replace("[t]","").replace("(t)","").replace(";","").split("=")[0])[1:] in exprList 
          and (line.replace("+","").replace("[t]","").replace("(t)","").replace(";","").split("=")[1])[1:] in varDico.keys()):
        return ''
    return line
    
def extractConstraint(line):
    line0 = extractBetweenParentheses(line)
    line = line0.split(',')[0]
    if len(line0.split(','))>1:
        name = extractBetweenParentheses(line0.split(',')[1]).replace("\"","")
    else:
        name = "CST"
    if line[0]=='(':
        line = extractBetweenParentheses(line)
    return line,name
   
def isConstraint(line):
    return "->add" in line and any(x in line for x in ['==', '>=', '<='])

def isBlankOrComment(line):
    line = line.strip()
    return line[0:2]=='//' or not line

instructions_to_ignore = ["mAllocate" ]

def lineToIgnore(line):
    for i in instructions_to_ignore:
        if i in line:
            return True
    if isBlankOrComment(line):
        return True
    elif line.strip().startswith('{') or line.strip().endswith('}'):
        return True
    elif line.strip().startswith(('qInfo','qDebug','qWarning','return','Q_ASSERT')):
        return True
    elif (any(x in line for x in ['.resize', '.close()'])): 
        return True
    elif extract_str(line, '->add', ';') and not isConstraint(line):
        return True # variables when their constructor is called
    elif extract_str(line, 'mInputIndicators->getMap', ';'):
        return True # variables when their constructor is called
    elif line.strip().startswith('if') and "getMIPExpression" in line:
        return True  #control line where the left-operand is a getMIPExpression function call and not a variable/expression
    elif extract_str(line, '<<', ';'):
        return True # variables when their constructor is called
    elif ".clear()" in line:
        return True
    return False
    
def isControl(line):
    ret = extract_str(line, "if", "\)") != '' and line.strip().startswith('if')
    ret = ret or extract_str(line, "for", "\)") != '' and line.strip().startswith('for')
    ret = ret or line.strip().startswith('while')
    return ret

def isClassMethod(line):
    if not line:
        return False
    else:
        return '::' in line and line[0]!=' '
    
def isMethodCall(line):
    strsearch = '->' + '(.+?)' + '\(' + '(.*?)' + '\)'
    found = re.search(strsearch, line)
    if not found:
        strsearch = '\.' + '(.+?)' + '\(' + '(.*?)' + '\)'
        found = re.search(strsearch, line)
    if not found:
        strsearch = '::' + '(.+?)' + '\(' + '(.*?)' + '\)'
        found = re.search(strsearch, line)
    if not found:
        strsearch = '^\s*' + '([\w]+)' + '\(' + '(.*?)' + '\)'
        found = re.search(strsearch, line)
    return found and found.group(1)!='add'

def isDerivedClass(currentclass, lines):
    currentclass = currentclass.strip()
    suffix = "public"
    strsearch = 'class' + '([\s\w]*)' + currentclass + '(\s*):(\s*)' + suffix 
    for line in lines: 
        line = line.strip()
        found = re.match(strsearch, line)
        if found:
            derivedClass = line[found.end():len(line)]
            derivedClass = derivedClass.split("{")[0]
            derivedClass = derivedClass.split(",")[0]
            derivedClass = derivedClass.replace(" ","")
            return derivedClass
    return "QOBJECT"

def getMethods(lines):
    methodDico = {}
    i=0
    while i<len(lines):
        line = lines[i]
        if isClassMethod(line) and "DO NOT SHOW" not in line:
            methodName = extract_str(line, "::", "\(")
            if methodName and methodName[0]!='~': #destructor is skipped
                methodLines, i = extractBloc(i,lines)
                methodDico[methodName] = methodLines
        i=i+1
    return methodDico
                
def getVariables(lines):
    varDico = {}
    expressions = []
    lines_to_ignore = []
    for i,line in enumerate(lines):
        if ('MIPExpression' in line) and not lineToIgnore(line):
            if '= MIPExpression' in line:
                dimension = line.split('MIPExpression')[1][0]
            if '=' in line: 
                varName = line.split('=')[0].strip()
                expressions.append(varName)
        elif "+=" in line and not lineToIgnore(line):
            line_without_t = line.replace("[t]","").replace(";","").split("//")[0].strip()
            line_without_t = line_without_t.replace("(t)","")
            line_without_t = line_without_t.replace("(mVectTypicalPeriods)","")
            expression,variable = line_without_t.strip().split("+=")[0].strip(),line_without_t.strip().split("+=")[1].strip()
            if variable in varDico.keys() and expression in expressions:
                varDico[variable][1] = expression
                lines_to_ignore.append(i)
        elif ('MIPVariable' in line) and not lineToIgnore(line):
            variable = "variable"
            # if 'MIPExpression' in line:
            #     variable = "expression"
            #     dimension = line.split('MIPExpression')[1][0]
            # else:
            dimension = line.split('MIPVariable')[1][0]

            varName = ''
            
            if '=' in line: 
                varName = line.split('=')[0].strip()
            elif line.strip().startswith('std::vector'): #vector of MIPVariable treated at push_back call
                pass
            elif 'push_back' in line:
                varName = line.split('.push_back')[0].strip()
                varName = varName + '[i]' # to emphasize that it is a vector
                line = extractBetweenParentheses(line)
            else:
                strsearch = 'MIPVariable(.+?)\s+(.+);'
                found = re.search(strsearch, line)
                if found:  #example: 'MIPModeler::MIPVariable3D weightVar;'
                    varName = found.group(2)
            size=''
            varMin = ''
            varMax = ''
            varType = 'double'
            if dimension=='0':            
                args = extractBetweenParentheses(line).split(',')
                varMin = args[0]
                if len(args)>=2:
                    varMax = args[1]
                if len(args)>=3 and 'MIP_INT' in args[2]:
                    varType = 'int'
            if dimension=='1':
                args = extractBetweenParentheses(line).split(',')
                size = args[0]
                if len(args)>=3:
                    varMin = args[1]
                    varMax = args[2]
                if len(args)>=4 and 'MIP_INT' in args[3]:
                    varType = 'int'
            if varName:
                varDico[varName] = [dimension,"", size, varMin, varMax, varType]
    return varDico,expressions,lines_to_ignore

def extractMandatoryValue(phrase):
    isFlag = ""
    phrase = phrase.split('SExtFunctionFlag')[0]
    phrase = phrase.strip().strip('(').strip('}')
    phrase_parts = phrase.split('{')
    if len(phrase_parts) == 3 or len(phrase_parts) == 4:
        fType = phrase_parts[1].strip().strip(',')
        if fType == "eFTypeOrNot":
            phr = phrase_parts[2].strip().strip(',').strip('}')
            if len(phr) > 1:#for safety
                flags = phr.split(',')
                for flag in flags:
                    if flag != "":
                        isFlag += flag.replace('&', '') + "||"
            if len(phrase_parts) == 4:
                nphr = phrase_parts[3].strip().strip(',').strip('}')
                if len(nphr) > 1:#for safety
                    notFlags = nphr.split(',') 
                for flag in notFlags:
                    if flag != "":
                        isFlag += "!" + flag.replace('&', '') + "||"
            return isFlag.strip("||")
        #
        elif fType == "eFTypeNotAnd":
            nphr = phrase_parts[2].strip().strip(',').strip('}')
            if len(nphr) > 1:#for safety
                notFlags = nphr.split(',')
                for flag in notFlags:
                    if flag != "":
                        isFlag += "!" + flag.replace('&', '') + "&&"
            if len(phrase_parts) == 4:
                phr = phrase_parts[3].strip().strip(',').strip('}')
                if len(phr) > 1:#for safety
                    flags = phr.split(',') 
                for flag in flags:
                    if flag != "":
                        isFlag += flag.replace('&', '') + "&&"
            return isFlag.strip("&&")
        elif fType == "eFTypeNotAndOr": 
            nphr = phrase_parts[2].strip().strip(',').strip('}')
            if len(nphr) > 1:#for safety
                notFlags = nphr.split(',')
                for flag in notFlags:
                    if flag != "" :
                        isFlag += "!" + flag.replace('&', '') + "&&"
            if len(phrase_parts) == 4:
                phr = phrase_parts[3].strip().strip(',').strip('}')
                if len(phr) > 1:#for safety
                    flags = phr.split(',') 
                if len(flags) > 0:
                    isFlag += " (" 
                for flag in flags:
                    if flag != "":
                        isFlag += flag.replace('&', '') + "||"
            isFlag = isFlag.strip("||") + ")"
            return isFlag
        else: 
            return "true"
    else:
        return "true"

def extractParamAgrs(args_raw):
    args = []
    isUsed = ""
    isBlocking = ""
    args_parts = args_raw.split('SFunctionFlag')
    if len(args_parts) == 1:
        #Discard "SExtFunctionFlag" and always use value "true"
        if "SExtFunctionFlag" in args_parts[0]:
            SFunctionFlag_parts = args_parts[0].split('SExtFunctionFlag')
            args1 = SFunctionFlag_parts[0].strip().strip(',').split(',')
            args.extend(args1)
            if len(args) == 3:
                #isBlocking is "SExtFunctionFlag" 
                args.append("true") #mandatory
                if len(SFunctionFlag_parts) > 2:
                    #Also isUsed is "SExtFunctionFlag" 
                    args.append("true") #isUsed
            else:#len(args) == 4
                #Only isUsed is "SExtFunctionFlag" 
                args.append("true") #isUsed
            args2_part = []
            i = 2
            if len(SFunctionFlag_parts) == 2:
                i = 1
            args2_part = SFunctionFlag_parts[i].split(')')[1].strip().strip('}),')
            args2 = args2_part.split(',')
            args.extend(args2)
        else:
            args = args_parts[0].split(',')
    elif len(args_parts) < 4:
        args1 = args_parts[0].strip().strip(',').split(',')
        args2 = []
        if len(args_parts) == 2:
            if len(args1) == 4: #name, var, defaultvalue, mandatory
                args2_part = args_parts[1].split(')')[-1].strip().strip('}),')
                args2 = args2_part.split(',')
                isUsed = extractMandatoryValue(args_parts[1].split(')')[0].strip('}'))
            elif len(args1) == 3: #name, var, defaultvalue
                args2_part = args_parts[1].split(')')[-1].strip().strip(',')
                args2 = args2_part.split(',')
                isBlocking = extractMandatoryValue(args_parts[1].split(')')[0].strip('}'))
            elif len(args1) == 2: #name, var (case of timeseries)
                args2_part = args_parts[1].split(')')[-1].strip().strip(',')
                args2 = args2_part.split(',')
                isBlocking = extractMandatoryValue(args_parts[1].split(')')[0].strip('}'))
            else:
                print("Warning: something is not expected for parameter " + args1[0])
                print("The add parameter line cannot be correctly parsed.")
                print(args_raw)
        else:#len(args_parts) == 3
            isBlocking = extractMandatoryValue(args_parts[1].split(')')[0].strip('}'))
            args2_part = args_parts[2].split(')')[1].strip().strip(',')
            args2 = args2_part.split(',')
            isUsed = extractMandatoryValue(args_parts[2].split(')')[0].strip('}'))
        args.extend(args1)
        if isBlocking != "":
            if "SExtFunctionFlag" in isBlocking:
                isBlocking = "true"
            args.append(isBlocking)
        if isUsed != "":
            if "SExtFunctionFlag" in isBlocking:
                isUsed = "true"
            args.append(isUsed)
        args.extend(args2)
    else:
        print("Warning: something is not expected!")
        print("The add parameter line cannot be correctly parsed.")
        print(args_raw)
    return args

def isUniquePort(portList, port):
    portId = port["ID"]
    portName = port["Name"]
    for port_i in portList:
        if port_i["ID"] == portId and port_i["Name"] == portName:
            return True
    return False

def getDefaultPortsFromHLines(hlines):
    defaultPortsList = []
    isPortLine = False;
    port = {}
    for line in hlines:
        if isBlankOrComment(line): 
            continue
        if "void" in line and "initDefaultPorts" in line:
            isPortLine = True #start

        if(isPortLine):
            if '}' in line: 
                break #end
            if '[' in line and ']' in line and '='in line and ';' in line:
                line = line.split(';')[0]
                line = line.split('[')[1]
                key = line.split(']')[0]
                value = line.split('=')[1]
                key = key.strip().strip('\"')
                value = value.strip().strip('()\"')
                if key == "Direction":
                    if "KPROD" in value:
                        value = "OUTPUT"
                    elif "KCONS" in value:
                        value = "INPUT"
                    elif "KDATA" in value:
                        value = "DATAEXCHANGE"
                port[key] = value
            elif "insert" in line:
                value = line.split('(')[1].split(',')[0].strip().strip('\"')
                port["ID"] = value
                if not isUniquePort(defaultPortsList, port):
                    defaultPortsList.append(port)
                port = {};
    return defaultPortsList

def getDefaultPorts(maindir, filepath, hfilename):
    defaultPortsList = []

    #Read model .h lines
    with open(os.path.join(filepath, hfilename), 'r') as h:
        hlines = h.readlines()

    #Get ports from model .h lines
    defaultPortsList = getDefaultPortsFromHLines(hlines)

    #Check for ports in the mother class
    possibleClasses = ["SubModel", "TechnicalSubModel", "BusSubModel", "OperationSubModel", "ConverterSubModel", "StorageSubModel", "SourceLoadSubModel", "GridSubModel"]
    motherClass = isDerivedClass(hfilename.replace('.h', ''), hlines)

    #Stop at first .h contains ports
    #This is a safety step to avoid taking overridden ports.
    #Currently, there is no any model with default ports in multiple files.
    #This condition can be relaxed when needed. 
    #Another solution is to force the ports to always be inside the corresponding model.h 
    #Examples 1: ElectrolyzerDetailed.h doesn't contain ports. The ports are taken from Electrolyzer.h
    #          One can define initDefaultPorts() as virtual in Electrolyzer.h then override the method in ElectrolyzerDetailed.h even if the ports are the same
    #Example 2: initDefaultPorts() in BuildingFlexible.h overrides the method (virtual) initDefaultPorts() in SourceLoadSubModel.h 
    #           So, one should not read the ports from SourceLoadSubModel.h  and stop at BuildingFlexible.h
    #           Checking the port uniqueness using isUniquePort() should be enough. Buy, an additional safety step might be better.
    #           Another solution is to remove initDefaultPorts() from SourceLoadSubModel.h and move it to the related model.h files, so to forece the ports in model.h
    if len(defaultPortsList) != 0:
        return defaultPortsList

    i = 0
    maxLevels = 10 #to avoid infinite loop in case of error
    #ElectrolyzerDetailed -> Electrolyzer -> ConverterSubModel
    #StorageThermal -> StorageLinearBounds -> StorageGen -> StorageSubModel
    while i < maxLevels:
        motherFile = getMotherFile(maindir, filepath, possibleClasses, motherClass)
        if os.path.exists(motherFile): 
            motherhlines = []
            with open(motherFile, 'r') as mh:
                motherhlines = mh.readlines()
            portsList = getDefaultPortsFromHLines(motherhlines)
            for port in portsList:
                if not isUniquePort(defaultPortsList, port):
                    defaultPortsList.append(port)
            #get the mother class of the next level 
            motherClass = isDerivedClass(motherClass, motherhlines)
        else:
            print("Default Ports: file " + motherFile + " not found for model " + hfilename.replace('.h', ''))

        #Stop at first .h contains ports
        if len(defaultPortsList) != 0:
            return defaultPortsList

        i += 1
        if motherClass in possibleClasses and i < 9:
            i = 9 #next iteration is the last iteration

    return defaultPortsList
            
def getMotherFile(maindir, filepath, possibleClasses, motherClass):
    global modelsDir
    global subModelsDir
    motherFile = ""
    if "private" in filepath:
        if motherClass in possibleClasses:
            motherFile = os.path.join(maindir + subModelsDir, motherClass + '.h') 
        else: 
            for modeldir in [maindir + modelsDir, filepath.split("private")[0]]:
                for path, dirnames, filenames in os.walk(modeldir):
                    for file in filenames: #FileNameLoop
                        if "debug" in path.lower() or "release" in path.lower():
                            break #FileNameLoop
                        if file == motherClass + '.h':
                            motherFile = os.path.join(path, motherClass + '.h') 
                            break #FileNameLoop
                    if motherFile != "":
                        break
                if motherFile != "":
                    break
    else:#inside submodels
        if motherClass in possibleClasses:
            motherFile = os.path.join(maindir + subModelsDir, motherClass + '.h') 
        else:
            motherFile = os.path.join(filepath, motherClass + '.h')
    return motherFile

def getParameter(line):
    """
    va chercher toutes les valeurs entre les parenthèses et renvoie : 
    """
    if 'addTimeSeries' in line:
        line = line.replace('"', '').split('addTimeSeries')[1]
    elif 'addConfig' in line:
        line = line.replace('"', '').split('addConfig')[1]
    elif 'addParameter' in line:
        line = line.replace('"', '').split('addParameter')[1]
    else:
        line = line.replace('"', '').split('->add')[1]

    args_raw = extractBetweenParentheses(line)
    args = extractParamAgrs(args_raw)
    #--------------------------- get key ------------------------------
    if "QString::number(i+1)" in args[0].strip():
        i = 1
        varName = args[0].replace("+QString::number(i+1)", str(i))
        key = varName.strip()
    else:
        key = args[0].strip()
            
    key = key.replace('KPRODCOSTREF()', 'ComputeProductionCost')
    key = key.replace('+', '')
    key = key.replace('  ', ' ')
    if "aPortName" in key:
        key = key.split('.')[1].strip()

    #--------------------------- get val ------------------------------
    #Take the comment that is provided after the line (in green in C++)
    comment = extract_str(line, "\/\*\*", "\*\/")

    #Mandatory, comment, predef, typeparam, typevar, unit, default, showInConfig
    val = [True, comment, [], '', '', '-', 0, []]
    #default value
    if len(args) > 2:
        val[6] = args[2].replace("INFINITY_VAL", "1.e12").strip()
    #isBlocking
    if len(args) > 3:
        val[0] = args[3].replace("!m","!").replace("! m","!").replace("||m","||").replace("|| m","||")\
                        .replace("(m","(").replace("&&m","&&").replace("&& m","&&").replace("&m","").strip() 

    #args[4] is the attribute "isUsed". No need to write it to Model.json

    #comment
    if len(args) > 5:
        if args[5] != "" and args[5] != " ":
            val[1] = args[5].strip()
    if "QString::number(i+1)" in val[1].strip():
        val[1] = val[1].replace("+QString::number(i+1)", "1")

    #unit
    if len(args) > 6:
        unit = args[6].replace("+/+", "/").replace("mCurrency", "Currency").replace("aCurrency", "Currency") \
            .replace("mObjectiveUnit", "ObjectiveUnit").replace("aObjectiveUnit", "ObjectiveUnit") \
            .replace("hour", "h").replace("getOptimalSizeUnit()", "OptUnit").replace("OptimalSizeUnit", "OptUnit").replace("%","\%").replace("+","").replace(" ", "").replace("{", "").replace("}", "")
        if unit != '' and unit != ' ':
            if isValidUnit(unit):
                val[5] = unit
            else:
                pass
        # set configs or multiple units
        if len(args) > 7:
            for x in args[7:]:
                y=x.replace( '{','').replace('}','')
                if y != "" and y != " " and "Unit" not in y and "unit" not in y: #condition y != " " is what we need
                    val[7].append(y)
    else:
        pass
    return key, args, val

def getParameterVector(line):
    """
    va chercher toutes les valeurs entre les parenthèses et renvoie : 
    """
    if 'addTimeSeries' in line:
        line = line.replace('"', '').split('addTimeSeries')[1]
    elif 'addPerfParam' in line:
        line = line.replace('"', '').split('addPerfParam')[1]
    else:
        line = line.replace('"', '').split('->add')[1]

    args_raw = extractBetweenParentheses(line)
    args = extractParamAgrs(args_raw)
    #--------------------------- get key ------------------------------
    if "QString::number(i+1)" in args[0].strip():
        i = 1
        varName = args[0].replace("+QString::number(i+1)", str(i))
        key = varName.strip()
    else:
        key = args[0].strip()
            
    key = key.replace('KPRODCOSTREF()', 'ComputeProductionCost')
    key = key.replace('+', '')
    key = key.replace('  ', ' ')
    if "aPortName" in key:
        key = key.split('.')[1].strip()

    #--------------------------- get val ------------------------------
    #Take the comment that is provided after the line (in green in C++)
    comment = extract_str(line, "\/\*\*", "\*\/")

    #Mandatory, comment, predef, typeparam, typevar, unit, default, showInConfig
    val = [True, comment, [], '', '', '-', 0, []]
    #isBlocking
    if len(args) > 2:
        val[0] = args[2].replace("!m","!").replace("! m","!").replace("||m","||").replace("|| m","||")\
                        .replace("(m","(").replace("&&m","&&").replace("&& m","&&").replace("&m","").strip()  
   
    #args[3] is the attribute "isUsed". No need to write it to Model.json

    #comment
    if len(args) > 4:
        if args[4] != "" and args[4] != " ":
            val[1] = args[4].strip()
    #unit
    if len(args) > 5:
        unit = args[5].replace("+/+", "/").replace("mCurrency", "Currency").replace("aCurrency", "Currency") \
            .replace("mObjectiveUnit", "ObjectiveUnit").replace("aObjectiveUnit", "ObjectiveUnit") \
            .replace("hour", "h").replace("getOptimalSizeUnit()", "OptUnit").replace("OptimalSizeUnit", "OptUnit").replace("%","\%").replace("+","").replace(" ", "").replace("{", "").replace("}", "")
        if unit != '' and unit != ' ':
            if isValidUnit(unit):
                val[5] = unit
            else:
                pass
        # set configs or multiple units
        if len(args) > 6:
            for x in args[6:]:
                y=x.replace( '{','').replace('}','')
                if y != "" and y != " " and "Unit" not in y and "unit" not in y: #condition y != " " is what we need
                    val[7].append(y)
    else:
        pass
    return key, args, val

def getIndicator(line):
    """
    va chercher toutes les valeurs entre les parenthèses et renvoie : 
    """
    line = line.replace('"', '').split('->add')[1]
    args = extractBetweenParentheses(line).split(',')
    #--------------------------- get key ------------------------------
    if "QString::number(i+1)" in args[0].strip():
        i = 1
        varName = args[0].replace("+QString::number(i+1)", str(i))
        key = varName.strip()
    else:
        key = args[0].strip()
            
    key = key.replace('KPRODCOSTREF()', 'ComputeProductionCost')
    key = key.replace('+', '')
    key = key.replace('  ', ' ')
    if "aPortName" in key:
        key = key.split('.')[1].strip()

    #--------------------------- get val ------------------------------
    #Take the comment that is provided after the line (in green in C++)
    comment = extract_str(line, "\/\*\*", "\*\/")

    #Mandatory, comment, predef, typeparam, typevar, unit, default, showInConfig
    val = [True, comment, [], '', '', '-', 0, []]
    #isBlocking
    if len(args) > 2:
        val[0] = args[2].replace("!m","!").replace("! m","!").replace("||m","||").replace("|| m","||")\
                        .replace("(m","(").replace("&&m","&&").replace("&& m","&&").replace("&m","").strip()  
   
    #comment
    if len(args) > 3:
        if args[3] != "" and args[3] != " ":
            val[1] = args[3].strip()
    if "QString::number(i+1)" in val[1].strip():
        val[1] = val[1].replace("+QString::number(i+1)", "1")

    #unit
    if len(args) > 4:
        unit = args[4].replace("+/+", "/").replace("mCurrency", "Currency").replace("aCurrency", "Currency") \
            .replace("mObjectiveUnit", "ObjectiveUnit").replace("aObjectiveUnit", "ObjectiveUnit") \
            .replace("hour", "h").replace("getOptimalSizeUnit()", "OptUnit").replace("OptimalSizeUnit", "OptUnit").replace("%","\%").replace("+","").replace(" ", "").replace("{", "").replace("}", "")
        if unit != '' and unit != ' ':
            if isValidUnit(unit):
                val[5] = unit
            else:
                pass
        # set configs or multiple units
        if len(args) > 5:
            for x in args[5:]:
                y=x.replace( '{','').replace('}','')
                if y != "" and y != " " and "Unit" not in y and "unit" not in y: #condition y != " " is what we need
                    if "QString::number(i+1)" in y.strip():
                        y = y.replace("+QString::number(i+1)", "1")
                    val[7].append(y)
    else:
        pass
    return key, args, val

def isValidUnit(unit):
    for pos_unit in ["kg","%", "t/", "W", "kW", "MW", "kWh", "MWh", "J", "kJ", "h", "/h ", "hour", "/s ", "second",
                     "/K","degK","degC", " degC", "currency", "Currency", "Currency/", "/Currency", "EUR", "EUR/", "/EUR", "objectiveunit", "objectiveUnit", "ObjectiveUnit", "ObjectiveUnit/", "/ObjectiveUnit",
                      "FluxUnit", "fluxUnit", "MassUnit", "EnergyUnit", "StorageUnit", "storageUnit", "PowerUnit", "/degC", "/degK", "/FluxUnit", "/MassUnit", "/EnergyUnit", "/StorageUnit", "/PowerUnit",
                     "degC/", "degK/", "FluxUnit/", "MassUnit/", "EnergyUnit/", "StorageUnit/", "PowerUnit/","km","km/h","m/s","m", "/WeightUnit", "WeightUnit", "WeightUnit/","SurfaceUnit", "/SurfaceUnit", "SurfaceUnit/"
                     "m2","unit","Bar","Pascal","MPa","-","FlowrateUnit", "bool", "file", "Nb", "string", "InstalledSizeUnit", "ImpactUnit", "year", "Year", "timestep", "timeStep", "TimeStep"]:
        if unit.find(pos_unit) >= 0:
            return True
    return False

# def analyseLine(line,level,varDico):
#     text = ''
#     if level<=0:
#         indents = "\\noindent "
#     else:
#         indents = "\\indent "*level
#     line = linePreprocessing(line)
#     if isClassMethod(line):
#         methodName = extract_str(line, "::", "\(")
#         text += indents + "\\textbf {" + methodName.replace("_","\_") + " method }\\hrulefill \\\\\n"
#     elif extract_str(line, "for", "\)") != '' and line.strip().startswith('for'):
#         line = "for (" + extractBetweenParentheses(line) + ")"
#         text += indents + "\\textbf {"+lineFormatting(line, varDico)+"}\\\\\n"
#     elif extract_str(line, "while", "\)") != '':
#         line = "while (" + extractBetweenParentheses(line) + ")"
#         text += indents + "\\textbf {"+lineFormatting(line, varDico)+"}\\\\\n"
#     elif extract_str(line, "if", "\)") != '':
#         line = line.split('if')[0] + "if  (" + extractBetweenParentheses(line) + ")"#may be 'else if(...)'
#         text += indents + "\\textbf {"+lineFormatting(line, varDico)+"}\\\\\n"
#     elif line.strip().startswith('else'):
#         text += indents + "\\textbf {"+lineFormatting(line, varDico)+"}\\\\\n"
#     elif (extract_str(line, "MIPVariable", ";") != ''):
#         pass # variable are processed separately
#     elif (extract_str(line,"closeExpression",';')!=''):
#         pass
#     elif isConstraint(line):
#         line,name = extractConstraint(line)
#         line += "\\hspace*{\\fill} ["+name+"]"
#         text += indents + "\\textit {"+lineFormatting(line, varDico)+"} \\\\\n"
#     elif isMethodCall(line):
#         text += indents + "\\textit {"+lineFormatting(line, varDico)
#         if 'SubModel::' in line:
#             text += "[see \\ref{SubModelEquations}]"
#         text += "} \\\\\n"
#     elif (extract_str(line, "\[", "\]") != '' ):
#         text += indents + "\\textit {"+lineFormatting(line, varDico)+"} \\\\\n"
#     elif (extract_str(line, "fix", "\)") != '' ):
#         text += indents + "\\textit {"+lineFormatting(line, varDico)+"} \\\\\n"
#     elif (any(x in line for x in ['+', '-', '*', '/'])):
#         text += indents + "\\textit {"+lineFormatting(line, varDico)+"} \\\\\n"
#     return text

def analyseLineRST(line,level,varDico,exprList,list_param):
    text = ''
    if level<=0:
        indents = "\n\t"
    else:
        indents = "\n\t" + level*"\t"
    line = linePreprocessing(line)
    if isClassMethod(line):
        methodName = extract_str(line, "::", "\(")
        #text += indents + "\\mathbf {" + methodName.replace("_","\_") + " method } \\\\\n"
    elif extract_str(line, "for", "\)") != '' and line.strip().startswith('for'):
        line = "for (" + extractBetweenParentheses(line) + ")"
        text += indents +lineFormatting(line, varDico,exprList,list_param)+"\n"
    elif extract_str(line, "while", "\)") != '':
        line = "while (" + extractBetweenParentheses(line) + ")"
        text += indents +lineFormatting(line, varDico,exprList,list_param)+"\n"
    elif extract_str(line, "if", "\)") != '':
        line = line.split('if')[0] + "if  (" + extractBetweenParentheses(line) + ")"#may be 'else if(...)'
        text += indents + lineFormatting(line, varDico,exprList,list_param)+"\n"
    elif line.strip().startswith('else'):
        text += indents + lineFormatting(line, varDico,exprList,list_param)+"\n"
    elif (extract_str(line, "MIPVariable", ";") != ''):
        pass # variable are processed separately
    elif (extract_str(line,"closeExpression",';')!=''):
        pass
    elif isConstraint(line):
        line,name = extractConstraint(line)
        text += indents +lineFormatting(line, varDico,exprList,list_param)+""
    elif isMethodCall(line):
        text += indents + lineFormatting(line, varDico,exprList,list_param)
        if 'TechnicalSubModel::' in line:
            text += "[see :ref:`TechnicalSubModelEquations`]"
        elif 'SubModel::' in line:
            text += "[see :ref:`SubModelEquations`]"
        elif 'BusSubModel::' in line:
            text += "[see :ref:`BusSubModelEquations`]"
        elif 'OperationSubModel::' in line:
            text += "[see :ref:`OperationSubModelEquations`]"
        text += "\n"
    elif (extract_str(line, "\[", "\]") != '' ):
        text += indents + lineFormatting(line, varDico,exprList,list_param)+""
    elif (extract_str(line, "fix", "\)") != '' ):
        text += indents +lineFormatting(line, varDico,exprList,list_param)+""
    elif (any(x in line for x in ['+', '-', '*', '/'])):
        text += indents + lineFormatting(line, varDico,exprList,list_param)+""

    return text

def extractComment(i,lines):
    bloc=[]
    while True:
        if ("*/" in lines[i]):
            break
        bloc.append(lines[i])
        i += 1 
        if ( i == len(lines) ):
            print("abnormal missing */ ",i,lines[i-1])
            break
    text='\n'
    for line in bloc:
        line = line.lstrip()
        line = line.replace("/**","")
        line = line[1:]
        line = line.replace("* \details","")
        line = line.replace("*/","\n")
        #line = line.replace("*","")
        # line = line.replace("_","\_")
        # line = line.replace("\\\\_","\\_")
        line = line.strip()
        if (not "file" in line and not "version" in line and not "date" in line and not "author" in line and not "brief" in line ):
            text += line + "\n"
    return text+ "\n",i
    
def extractBloc(i,lines,lines_to_ignore=[]):
    # bloc ends when line indent is <= first line indent
    line = linePreprocessing(lines[i])
    bloc=[line]
    indent = len(line) - len(line.lstrip())
    while i<len(lines)-1:
        line = linePreprocessing(lines[i+1])
        if not isBlankOrComment(line) and i not in lines_to_ignore:
            ind = len(line) - len(line.lstrip())
            if ind<=indent and line.strip()[0]!='{':  #reach end of bloc ?
                break
            bloc.append(lines[i+1])
        i=i+1
    if line.strip().startswith('}'):
        i=i+1
    if len(bloc)==1:
        line = bloc[0]
        if isControl(line) and i not in lines_to_ignore: #control and instruction on the same line => split
            bloc = [lines[i].split(')')[0] + ')']
            bloc.append(extract_str(line, "\)","\n").strip() +'\n')
        else:
            i=i+1
    return bloc,i #renvoie le numero de derniere ligne du bloc

def extractInstruction(i,lines): #extract an instruction split on multiple lines
    instruction = ''
    while i<len(lines):
        line = lines[i].split('//')[0] # remove comment 
        instruction += line.replace('\n','')
        if ';' in line:
            break
        if '{' in line: # case of member initializer list
            instruction = ''
            break        
        i = i+1
    return instruction,i
    
def analyseBloc(lines,level,varDico,exprList,list_param, lines_to_ignore=[]): #level=0 corresponds to method
    text = ''
    i = 1
    while i < len(lines):
        line = lines[i]
        if not lineToIgnore(line) and i not in lines_to_ignore:
            if line.lstrip().startswith('/**'):
                textBloc,i = extractComment(i,lines)
                #print("extract comment:", textBloc)
                textBloc += "\n.. container:: boxed-text\n"#\t\\begin{align*}"
                #textBloc+=".. math::\n"
            elif (extract_str(line, "for", "\)") != '' and line.strip().startswith('for')
                  or extract_str(line, "while", "\)") != ''
                  or line.strip().startswith('else')
                  or extract_str(line, "if", "\)") != ''):
                bloc,i = extractBloc(i,lines,lines_to_ignore = lines_to_ignore)
                if level==1 and i == 1:
                    textBloc = "\n.. container:: boxed-text\n"#\t\\begin{align*}"
                else:
                    textBloc = ""
                textBloc += analyseBloc(bloc,level+1,varDico,exprList,list_param)
            else: # so the bloc is an instruction which ends with a ';'
                textBloc = ""
                if ("setExpSizeMax" in line):
                    textBloc += "see :ref:`Model equations of SubModel`\n\n"
                    textBloc += ".. container:: boxed-text\n"
                if ';' not in line:
                    line,i = extractInstruction(i,lines)
                    ana = analyseLineRST(line,level+1,varDico,exprList,list_param)
                else:
                    ana = analyseLineRST(line,level+1,varDico,exprList,list_param)
                if len(ana)>0:
                    #textBloc = "\t.. math::\n"
                    textBloc += analyseLineRST(line,level+1,varDico,exprList,list_param)
                    if level==1 and i == 1:
                        textBloc += "\n.. container:: boxed-text\n"
                    else:
                        textBloc += "\n"
                else:
                    textBloc = ""
                
            text += textBloc
        i = i + 1
    if text!='': #add first line of bloc (for, if,...) only if it is not empty
        text = analyseLineRST(lines[0],level-1,varDico,exprList,list_param) + text
   #if level==0:
   #     text += "\\\\\n"
    return text

def getParamPossibleValues(key, lines):
    valSet = []
    text1 = "=="
    text2 = key + '.contains('
    text3 = "!="
    for line in lines:
        if not isBlankOrComment(line):
            line = line.replace(' ', '')
            if key in line and text1 in line and not '""' in line:
                parts = line.split('==')
                val = extract_str(parts[1], '"', '"')
                # val = val.replace('_', '\_')
                if val and valSet.count(val) == 0:
                    valSet.append(val)
            elif text2 in line:
                parts = line.split('||')
                for p in parts:
                    val = extract_str(p, '"', '"')
                    # val = val.replace('_', '\_')
                    if val and valSet.count(val) == 0:
                        valSet.append(val)
            elif key in line and text3 in line and not '""' in line:
                parts = line.split('!=')
                val = extract_str(parts[1], '"', '"')
                # val = val.replace('_', '\_')
                if val and valSet.count(val) == 0:
                    valSet.append(val)
    return valSet

def getComponentParameters(paramDico, lines, searchPossibleValues):
    strSearch1 = 'CompoInputParam->addParameter'
    for line in lines:
        if not isBlankOrComment(line):
            if strSearch1 in line:
                # get parameters and description from declaration
                line = line.replace('"','')
                args = extractBetweenParentheses(line).split(',')
                key = args[0]
                key = key.replace('KPRODCOSTREF()','ComputeProductionCost')
                # key = key.replace('_','\_')
                val = ['','','']
                if len(args)>2: 
                    val[0] = args[2] #isBlocking
                if len(args)>3: 
                    if len(args[3])>2:
                        val[1] = ','.join(args[3:]) #comment
                paramDico[key] = val
    # get parameter possible value by parsing the file
    if searchPossibleValues:
        if 'ModelClass' in paramDico.keys():
            possibleValues = getParamPossibleValues("mCompoModelClassName", lines)
            paramDico['ModelClass'][2] = possibleValues
        if 'Model' in paramDico.keys():
            possibleValues = getParamPossibleValues("mCompoModelName", lines)
            paramDico['Model'][2] = possibleValues
        if 'Solver' in paramDico.keys():
            possibleValues = getParamPossibleValues("mSolverName", lines)
            paramDico['Solver'][2] = possibleValues
        if 'Type' in paramDico.keys():
            possibleValues = getParamPossibleValues("mType", lines)
            paramDico['Type'][2] = possibleValues
        
    return paramDico

def getFullModelParameters(paramDico, lines_h, varDico):  
    strConfig  = 'addConfig'         
    strParam  = 'addParameter'   
    strInputEnvImpacts = 'InputEnvImpacts->addParameter'
    strInputPortImpacts = 'InputPortImpacts->addParameter'
    strInputPortImpactsTS = 'InputPortImpactsTS->addParameter'
    strTimeSeries= 'addTimeSeries'
    strPerf1 = 'addPerfParam'
    strPerf2 = 'InputPerfParam->addParameter'
    strIndicator = 'InputIndicators->addIndicator'

    # Mandatory, comment, predef, typeparam, typevar, unit, default, showInConfig
    # val = [True, comment, [], '', '', unit, 0, []]

    for line in lines_h:
        if not isBlankOrComment(line):
            if "void" in line or "addParameters" in line or "addPerfParam(mInputPerfParam)" in line or "->addParameter(aParamName" in line:
                continue
            if strConfig in line or strParam in line or strPerf1 in line or strPerf2 in line or strInputEnvImpacts in line or strInputPortImpacts in line or strInputPortImpactsTS in line:
                if strPerf1 in line or strPerf2 in line or strInputPortImpactsTS in line:
                    key, args, val = getParameterVector(line)
                else:
                    key, args, val = getParameter(line)

                try:
                    varName = args[1].split('&')[1].split(',')[0].split('[')[0]
                    if varDico[varName]:
                        if strInputEnvImpacts in line:
                            val[3] = 'EnvImpact'
                        elif strInputPortImpacts in line or strInputPortImpactsTS in line:
                            val[3] = 'PortImpact'
                        elif strPerf1 in line or strPerf2 in line:
                            val[3] ='PerfParameter'
                        else:
                            val[3] = 'Parameter'
                        val[4] = varDico[varName]
          
                        if key == 'WeightUnit':
                            val[2] = getPossibleWeightUnits(lines_h) #predefined values
                except KeyError:
                    print("Model varDico anomalie1 ", key, args[0], line)
                    val[3] = 'Parameter' #'N/A' #a parameter might get replaced in a model that is an extension of another model
                    if key == "OptimizeControlOnly": #Exception TODO: check why mOptimizeControlOnly is missing from varDico
                        val[4] = 'bool'
                    else:
                        val[4] = 'N/A'
                except IndexError:
                    print("Model varDico anomalie2 ", key, args, line)
                    val[3] = 'N/A'
                    val[4] = 'N/A'

                if strPerf1 in line or strPerf2 in line or strInputPortImpactsTS in line:
                    getDefaultValue(lines_h, val, varName) #Is really needed? Do they have a default value!
                else:
                    parseDefaultValue(val)

                if strInputPortImpactsTS in line: #timeseries under 'PortImpact'
                    val[4] = "string"
                    val[6] = ''

                paramDico[key] = val

                if 'INPUT1' in key:
                    paramDico[key.replace('1', '2')] = val
                    paramDico[key.replace('1', '3')] = val
                if 'Efficiency1' in key:
                    paramDico[key.replace('1', '2')] = val
                if 'OUTPUT1' in key:
                    paramDico[key.replace('1', '2')] = val

            if strTimeSeries in line: #time series 
                key, args, val = getParameterVector(line) 
                val[3] = 'InputBoundaryConditions'
                val[4] = "string"
                val[6] = ''
                varName = args[1].split('&')[1].split(',')[0].split('[')[0]
                if varName in varDico:
                    getDefaultValue(lines_h, val, varName)
                else:
                    print("Pas compris : ",key, varName)
                paramDico[key] = val

            if strIndicator in line:
                # get parameters and description from declaration
                key, args, val = getIndicator(line)
                if "Discount Factor" in key and "QString" in key:
                    continue
                val[3] = 'Indicator'
                if val[0] == "exp":
                    val[0] = "ExportIndicators"
                paramDico[key+'::Indicator'] = val # and trem 'Indicator' to the key to avoid key conflict with 'Displayable' elements
    return paramDico

def getPossibleWeightUnits(lines):
    for line in lines:
        if "setPossibleWeightUnits" in line and "void" not in line:
            return [x.strip() for x in line.split('{')[1].split('}')[0].replace('"', '').split(',')]  
    return []

def getBaseEnvImpactsList(lines): #static list !!
    #for line in lines:
    #    if "setPossibleImpactNames" in line and "void" not in line:
    #        return [x.strip() for x in line.split('{')[1].split('}')[0].replace('"', '').split(',')] #extractBetweenParentheses(line).replace('{', '').replace('}', '').split(',')
    return ["Climate change#Global Warming Potential 100","Ozone depletion#Ozone Depletion Potential","Human toxicity-cancer effects#Comparative Toxic Unit for humans",
 "Human toxicity-noncancer effects#Comparative Toxic Unit for humans","Particulate matter-Respiratory inorganics#Human health effects associated with exposure to PM",
 "Ionising radiation human health#Human exposure efficiency relative to U","Photochemical ozone formation#Tropospheric ozone concentration increase","Acidification#Accumulated Exceedance",
 "Eutrophication-terrestrial#Accumulated Exceedance","Eutrophication-Aquatic freshwater#Fraction of nutrients reaching freshwater and compartment","Eutrophication-aquatic marine#Fraction of nutrients reaching marine and compartment",
 "Ecotoxicity freshwater#Comparative Toxic Unit for ecosystems", "Land use#Soil quality index", "Water use#User Deprivation Potential", "Resource use-Minerals and metals#Abiotic Resource Depletion","Resource use-Energy carriers#Abiotic Resource Depletion"]

def getBaseEnvImpactShortNames(lines):  #static list !!
    #for line in lines:
    #    if "setPossibleImpactShortNames" in line and "void" not in line:
    #        return [x.strip() for x in line.split('{')[1].split('}')[0].replace('"', '').split(',')] #extractBetweenParentheses(line).replace('{', '').replace('}', '').split(',')
    return ["GWP","ODP","HTox-c","HTox-nc","PM","IRP","POCP","AP","EP-t", "EP-fw","EP-m","Ecotox", "LU", "WU","ADP-mm","ADP-f"]

def getBaseEnvImpactUnitsList(lines): #static list !!
    #for line in lines:
    #    if "setPossibleImpactUnits" in line and "void" not in line:
    #        return [x.strip() for x in line.split('{')[1].split('}')[0].replace('"', '').split(',')] #extractBetweenParentheses(line).replace('{', '').replace('}', '').split(',')
    return ["kg CO2 eq",  "kg CFC-11 eq", "CTUh","CTUh","-","kBq U","kg NMVOC eq",
            "mol H+ eq", "mol N eq", "kg P eq", "kg N eq", "CTUe", "-", "kg world eq deprived","kg Sb eq","MJ"]

    
def getModelCompoParameters(paramDico, lines_cpp, searchPossibleValues, varDico, file, dirpath, lines_h):
    strOption = 'mCompoInputParam->addParameter'
    strParam = 'mCompoInputSettings->addParameter'
    strConfig = 'mConfigParam->addParameter'
    strEnvParam = 'mCompoEnvImpactsParam->addParameter'
    strEnergyvectorTS = "mTimeSeriesParam->addParameter"
    for line in lines_cpp:
        if not isBlankOrComment(line):
            if strOption in line or strParam in line or strConfig in line or strEnvParam in line:
                line = line.replace('"', '')
                if 'ConsideredEnvironmentalImpacts' in line:
                    key, args, val = getParameterVector(line)
                else:
                    key, args, val = getParameter(line)
                key = key.replace('+QString::number(i+1)', '1')
                if strOption in line: 
                    classe = 'Option'
                if strParam in line or strConfig in line: 
                    classe = 'Parameter'
                if strEnvParam in line: 
                    classe = 'EnvImpact'

                # do not handle generic variable names !
                if ('varName' in key):
                    continue
                if ('SerieNamelist' in key):
                    continue

                if ('aParamName' in key):
                    continue
                elif ('mName' in key):
                    key = key.replace('mName', '')

                if ('.Coeff' in key):
                    val[3] = 'Option'
                    val[4] = 'float'
                    val[6] = 1.
                    key = key.replace('+', '').strip('m')
                    if not val[1]:
                        val[1] = 'Multiplying coefficient on profile'
                elif ('.Offset' in key):
                    val[3] = 'Option'
                    val[4] = 'float'
                    val[6] = 0.
                    key = key.replace('+', '').strip('m')
                    if not val[1]:
                        val[1] = 'Offset coefficient on profile'
                elif 'ZE' in key and "SimulationControl" not in file:
                    val[3] = 'InputBoundaryConditions'
                    val[4] = "string"
                    val[6] = ''
                elif key == 'ConsideredEnvironmentalImpacts':
                    #Special parameter for environmental impacts
                    val[2] = [] #getBaseEnvImpactsList(lines_cpp) #get predefined values from CairnCore directly 
                    val[3] = 'Parameter'
                    val[4] = 'checklist' #var type
                    val[6] = [] #default value
                else:
                    val[3] = classe
                    val[4] = "string"

                    if "Add" in key or "default = false" in val[1] or "default = true" in val[1]:
                        val[4] = "bool"

                    if any(x in val[1] for x in ['Use', 'time series', 'Name']):
                        val[4] = "string"

                    varName = args[1].split('&')[1].split(',')[0].split('[')[0]

                    if varName in varDico:
                        val[4] = varDico[varName]
                        parseDefaultValue(val)
                    elif "omponent" in varName:
                        for variable in varDico:
                            if key in variable:
                                val[4] = varDico[variable]
                                parseDefaultValue(val)
                                break
                    else:
                        print("Pas compris : ",key, varName)

                if any(x in key for x in ["MassUnit", "EnergyUnit", "PowerUnit", "FlowrateUnit"]):
                    val[0] = True
                    
                if key == "Solver":
                    val[4] = "string"
                paramDico[key] = val

            elif strEnergyvectorTS in line:
                key, args, val = getParameter(line)
                val[3] = 'InputBoundaryConditions'
                val[4] = "string"
                val[6] = ''
                varName = args[1].split('&')[1].split(',')[0].split('[')[0]
                if varName in varDico:
                    getDefaultValue(lines_h, val, varName)
                else:
                    print("Pas compris : ",key, varName)
                paramDico[key] = val

    # get parameter possible value by parsing the file
    if 'Direction' in paramDico:
        if "Grid" in file:
            paramDico['Direction'][2] = ["InjectToGrid", "ExtractFromGrid"]
            paramDico['Direction'][6] = "ExtractFromGrid"

        elif "SourceLoad" in file or "BuildingFlexible" in file:
            paramDico['Direction'][2] = ["Source", "Sink"]
            paramDico['Direction'][6] = "Sink"
            
    if 'ObjectiveType' in paramDico:
        paramDico['ObjectiveType'][2] = ["","Add","Blended","Lexicographic"]
        
    if searchPossibleValues:
        if 'ModelClass' in paramDico: #Is not easier to add 'ModelClass' in getFullModelParameters and simple read the hlines of the model ?!
            #possibleValues0 = getParamPossibleValues("mCompoModelClassName", lines_cpp)
            possibleValues = getPossibleModelClasses(file)  
            if possibleValues:
                # paramDico['Model'][2] = possibleValues
                paramDico['ModelClass'][2] = possibleValues
                paramDico['ModelClass'][6] = file.split('.')[0]
                #print("===============> ModelClass possibleValues ", paramDico['ModelClass'][2], file)
            else:
                paramDico['ModelClass'][2] = [file.split('.')[0]]
                paramDico['ModelClass'][6] = file.split('.')[0]
                
        if 'Model' in paramDico:
            possibleValues=[]
            possibleValues0=[]
            possibleValues0 = getParamPossibleValues("mCompoModelName", lines_cpp)
            for value in possibleValues0:
                if value in file.split('.')[0]:
                # paramDico['Model'][2] = possibleValues
                    paramDico['Model'][2] = [value]
                    paramDico['Model'][6] = value
                    #print("===============> Model possibleValues ", paramDico['Model'][2], file)
        if 'Solver' in file:
            print("YOUPI found Solver : ")
            possibleValues=[]
            if 'Model' in paramDico:
                possibleValues = ["MIPModeler", "GAMS"]
                if possibleValues:
                    # paramDico['Model'][2] = possibleValues
                    paramDico['Model'][2] = possibleValues
                    paramDico['Model'][6] = possibleValues[0]
                    #print("===============> Model possibleValues ", paramDico['Model'][2], file)
            possibleValues=[]
            if 'Solver' in paramDico:
                possibleValues = getParamPossibleValues("mSolverName", lines_cpp)
                if possibleValues:
                    paramDico['Solver'][2] = possibleValues
                    paramDico['Solver'][6] = possibleValues[0]
                    #print("Solver possibleValues", possibleValues, file)
            possibleValues=[]
            if 'Category' in paramDico:
                possibleValues = getParamPossibleValues("mProblemType", lines_cpp)
                if possibleValues:
                    paramDico['Category'][2] = possibleValues
                    paramDico['Category'][6] = possibleValues[1]
                    #print("Solver possibleValues", possibleValues, file)
            possibleValues=["YES", "NO"]
            if 'WriteLp' in paramDico:
                paramDico['WriteLp'][2] = possibleValues
                paramDico['WriteLp'][6] = possibleValues[1]
            if 'ReadParamFile' in paramDico:
                paramDico['ReadParamFile'][2] = possibleValues
                paramDico['ReadParamFile'][6] = possibleValues[1]
            if 'WriteMipStart' in paramDico:
                paramDico['WriteMipStart'][2] = possibleValues
                paramDico['WriteMipStart'][6] = possibleValues[1]
    
    return paramDico

def getPossibleModelClasses(file):
    global modelsDir
    global privateModelsDir
    possibleModelClasses_0 = []
    model = file.replace('.cpp', '').replace('.h', '')
    hfile = model + '.h'
    possibleModelClasses_0.append(model)
    for modeldir in [modelsDir, privateModelsDir]:
        for dirpath, dirnames, filenames in os.walk(r"..\..\..\src" + modeldir):
            for candidate_file in filenames: #FileNameLoop
                if "debug" in dirpath.lower() or "release" in dirpath.lower():
                    break #FileNameLoop
                if candidate_file != hfile: 
                    continue 
                with open(os.path.join(dirpath, hfile), 'r') as h:
                    hlines = h.readlines()
                #Obtain linkPerseeModelClass
                for line in hlines:
                    if "linkPerseeModelClass" in line and hfile in line:
                        #linkPerseeModelClass StorageGen.h StorageLinearBounds.h StorageThermal.h Battery_V1.h StorageSeasonnal.h 
                        possibleValues = line.split(' ')
                        for value in possibleValues:
                            value = value.strip() 
                            if not value.endswith('.h'):
                                continue
                            value = value.replace('.h', '')
                            if value not in possibleModelClasses_0:
                                print (" => add possible value :", value)
                                possibleModelClasses_0.append(value)
                        break

    #Verify that the linked models exist !
    possibleModelClasses = []
    possibleModelClasses.append(model)
    for linkedModel in possibleModelClasses_0:
        for modeldir in [modelsDir, privateModelsDir]:
            for dirpath, dirnames, filenames in os.walk(r"..\..\..\src" + modeldir):
                for candidate_file in filenames: #FileNameLoop
                    if "debug" in dirpath.lower() or "release" in dirpath.lower():
                        break #FileNameLoop
                    if candidate_file == linkedModel+'.h' and linkedModel not in possibleModelClasses:
                        possibleModelClasses.append(linkedModel)
    return possibleModelClasses

#def getPossibleModelClasses(file, possibleValues):
#    possibleModelClasses = []
#    model = file.replace('.cpp', '').replace('.h', '')
#    hfile = model + '.h'
#    possibleModelClasses.append(model)
#    for modeldir in [r"\submodels", r"\..\privateSubmodels"]:
#        for dirpath, dirnames, filenames in os.walk(r"..\..\lib\export\Persee\core" + modeldir):
#            for candidate_file in filenames: #FileNameLoop
#                if "debug" in dirpath.lower() or "release" in dirpath.lower():
#                    break #FileNameLoop
#                if candidate_file.endswith('.cpp'):  
#                    candidate_hfile = candidate_file.replace('.cpp', '.h')
#                    candidate_model = candidate_file.replace('.cpp', '')
#                    #Filter for possible linked models 
#                    if not os.path.exists(os.path.join(dirpath, candidate_hfile)): 
#                        continue #Not a candidate file for a model
#                    if candidate_model not in possibleValues:
#                        continue #not a linkPerseeModelClass candidate 
#                    with open(os.path.join(dirpath, candidate_hfile), 'r') as h:
#                        candidate_hlines = h.readlines()
#                    #Verify if it is a linked model class
#                    for line in candidate_hlines:
#                        if "linkPerseeModelClass" in line and hfile in line:
#                            if candidate_model not in possibleModelClasses:
#                                print (" => add possible value :", candidate_model)
#                                possibleModelClasses.append(candidate_model)
#                            break
#    return possibleModelClasses
  
def commonradical(str1, str2):
    i=0
    while (i < len(str1) and i < len(str2)):
        if str1[i] != str2[i]:
            return i
        i=i+1
    return i

def parseDefaultValue(val):
    if type(val[6]) == type(1):
        val[4] = "int"
    elif type(val[6]) == type(1.0):
        val[4] = "double"
    elif type(val[6]) == type(True):
        val[4] = "bool"
    elif type(val[6]) == type("string"):
        val[6] = val[6].strip()
        if val[6] == '':
            val[4] = "string"
        else:
            try:
                val[6] = int(val[6])
                val[4] = "int"
            except ValueError:
                try:
                    val[6] = float(val[6])
                    val[4] = "double"
                except ValueError:
                    if val[6] == 'true':
                        val[6] = True
                        val[4] = "bool"
                    elif val[6] == 'false':
                        val[6] = False  
                        val[4] = "bool"
                    else:
                        val[4] = "string"
    #
    if val[4] == "bool":
        val[2] = [True, False]       
#end def

def getDefaultValue(lines, val, varName):
    for l1 in lines:
        l1mod = l1.split("\n")[0].split(";")[0].split(",")[0]
        if varName in l1mod:
            valdec1 = extract_initialValueConstructor(l1mod, varName)
            if valdec1 != "":
                if '"' in valdec1:
                    if val[4] == "":
                        val[4] = "string"
                    val[6] = valdec1.replace('"', '')
                else:
                    if val[4] == "":
                        val[4] = "double"
                    val[6] = valdec1
                #break
            valdec2 = extract_initialValueEqual(l1mod,varName)
            if valdec2 != "":
                if '"' in valdec2:
                    val[4] = "string"
                    val[6] = valdec2.replace('"','')
                elif 'bool' in l1mod:
                    val[4] = "bool"
                    val[6] = valdec2              
                elif 'int' in l1mod:
                    val[4] = "int"
                    val[6] = valdec2
                elif 'float' in l1mod or 'double' in l1mod:
                    val[4] = "double"
                    val[6] = valdec2
                else:
                    val[6] = valdec2
                #break

    if val[4] == "bool":
        val[2] = [True, False]
        if val[6] == 'true' or val[6] == '1' or val[6] == 1:
            val[6] = True
        elif val[6] == 'false' or val[6] == '0' or val[6] == 0:
            val[6] = False       
        if val[6] == '' and "default = true" in val[1]:
            val[6] = True
        if val[6] == '' and "default = false" in val[1]:
            val[6] = False            
    elif val[4] == "int":
        if val[6] != "":
            val[6] = int(val[6])
    elif val[4] == "double" or val[4] == "float":
        if val[6] != "":
            val[6] = float(val[6])

strdeb='(|\s+)(|[a-zA-Z/]+)(|\s+)'
spc = '(|\s+)'
#streol = spc+'[,;]'
streol = spc
strbool='(true|false)|"[a-zA-Z0-9\-/]+"' #This is bool and string
strfloat='(|-)((\d+\.\d+)|(\d+)|(\d+\.)|(\d+\.\d+)e((|-)\d+)|(\d+\.)e((|-)\d+)|(\d+)e((|-)\d+))'

def extract_initialValueConstructor(text, str1): #ToDo: it doesn't match last line because it doesn't followed by ,
    strsearch = spc+str1+spc+'\('+spc+'('+strbool+'|'+strfloat+')'+spc+'\)'+streol
    try:
        found = re.fullmatch(strsearch, text).group(0)
        found = found.split("(")[1].split(")")[0].strip()
    except AttributeError:
        found = ''
    # strsearch found in the original string
    return found

def extract_initialValueEqual(text, str1):
    strsearch = strdeb+str1+spc+'='+spc+'('+strbool+'|'+strfloat+')'+streol
    try:
        found = re.fullmatch(strsearch, text).group(0)
        found = found.split("=")[1].strip()
    except AttributeError:
        found = ''
        try:
            strsearch = strdeb+str1+spc+'\{'+spc+'('+strbool+'|'+strfloat+')'+spc+'\}'+streol
            found = re.fullmatch(strsearch, text).group(0)
            found = found.split("{")[1].strip()
            found = found.split("}")[0].strip()
        except AttributeError:
            found = ''
    # strsearch found in the original string
    return found

def fillExpressionDico(paramDico, lines, model=""):
    varList = {}
    optimalSizeExp = ""
    for line in lines:
        if 'addIO' in line or 'addControlIO' in line:
            if lineToIgnore(line) or "void " in line or "aParamName" in line: 
                continue
            #
            varName = ''
            args = extractBetweenParentheses(line).split(',')
            comment = extract_str(line, "\/\*\*", "\*\/")
            comment = comment.replace('"', '').strip(' ')
            if "key" in args[0].strip(): #The case of "Deep Layers Values" of NeuralNetwork
                continue
            elif "QString::number(i+1)" in args[0].strip() and (model == "MultiConverter" or model == "Cogeneration"): #The case of "INPUTFlux/OUTPUTFlux" of Cogeneration (default 3) and MultiConverter (default 1)
                k = 0
                if model == "MultiConverter": 
                    k = 1
                elif  model == "Cogeneration": 
                    k = 3
                for i in range(1, k+1): #The variables are re-processed in GUI based on the number of inputs/outputs
                    varName = args[0].replace('"+QString::number(i+1)', str(i)).replace('"', '').replace('+', '').strip(' ')
                    #comment_i = comment.replace(' i ', ' '+str(i)+' ').strip(' ')  
                    if varName:
                        varList[varName] = [comment] #comment_i]
            elif "QString::number(i)" in args[0].strip() and model == "NeuralNetwork": #The case of "Input_/Output_" of NeuralNetwork (default 1)
                varName = args[0].replace('"+QString::number(i)', "0").replace('"', '').replace('+', '').strip(' ')
                #comment = comment.replace(' i ', ' 0 ').strip(' ')
                if varName:
                    varList[varName] = [comment]
            else:
                varName = args[0].strip(' ').strip('"')
                if varName:
                    varList[varName] = [comment]
        if 'setOptimalSizeExpression' in line and "void" not in line:
            if not isBlankOrComment(line):
                l1 = line.split(")")
                if len(l1) > 1:
                    l2 = l1[0].split("(")
                    if len(l2) > 1:
                        optimalSizeExp = l2[1].strip().replace("\'", "").replace("\"", "")

    for varName in varList:
        if not varName in paramDico:#first element is used for isOptimalSizeExp. Mandatory is not important for UsableVar
            paramDico[varName] = [False, varList[varName][0], [], 'UsableVar', '', '', '', []]
            # print("adding Expression ", varName, varList[varName][0])

    #set isOptimalSizeExp
    if optimalSizeExp in paramDico.keys():
        paramDico[optimalSizeExp][0] = True
            
    return paramDico

def computeVarDicoFromHfile(lines):
    varDico = {}
    varType = ""
    varName = ""
    # be careful: std::vector stands for both int or double 
    typelist = ["int", "uint", "bool", "double", "std::vector", "std::vector<double>", "std::vector<int>", "uint64_t",
                "float", "QString", "string", "MIPData1D", "MIPModeler::MIPData1D", "MIPModeler::MIPData2D", "QVector<QString>","QStringList","QVector<bool>"]
    isProtected = 0

    if len(lines) == 0:
        print("No lines to analyze !")
        return {}

    for line in lines:
        #        if isBlankOrComment(line) or any (x in line for x in ['(',')',',','*','&']) :
        if any(y in line for y in ["protected", "private"]):
            isProtected = 1
        if isProtected == 0:
            continue
        if isBlankOrComment(line):
            continue

        liste_fields = line.split(';')[0].split('=')[0].split('{')[0].split('/')[0].strip().split(' ')
        if any(x in liste_fields for x in typelist):
            varType = liste_fields[0].replace("QVector<QString>", "string").replace("QStringList","string").replace("QString", "string").replace("QVector<float>", "float1D").replace(
                "std::vector", "vector").replace("vector<double>", "double").replace("uint64_t", "int").replace("MIPData1D", "double1D").replace("MIPData2D", "double2D").replace("QVector<bool>", "bool").replace(
                "MIPModeler::", "").replace("uint", "int")

            for k in range(1, len(liste_fields)):
                if 'i' in liste_fields[k]:
                    for i in [1, 2, 3]:
                        varName = liste_fields[k].replace('+QString::number(i+1)', '1').replace("[i]", "")
                        varDico[varName] = varType
                else:
                    varName = liste_fields[k].replace('+QString::number(i+1)', '1').replace("[i]", "")
                    varDico[varName] = varType
                # if "NbYear" in varName:
                # print(" Adding to vardico1 : ", varName, varDico[varName], k, line)
        elif ',' in line and varType != "":
            for k in range(1, len(liste_fields)):
                if 'i' in liste_fields[k]:
                    for i in [1, 2, 3]:
                        varName = liste_fields[k].replace('+QString::number(i+1)', '1').replace("[i]", "")
                        varDico[varName] = varType
                else:
                    varName = liste_fields[k].replace('+QString::number(i+1)', '1').replace("[i]", "")
                    varDico[varName] = varType
                # print (" Adding to vardico2 : ",varName, line)
        else:
            varType = ""
        # special treatment for multiple declarations - varType remains until end of declaration by ';'
        if ';' in line:
            varType = ""
    return varDico


def computeVarDicoFromFfile(lines, file):
    varDico = computeVarDicoFromHfile(lines)

    return varDico



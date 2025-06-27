# -*- coding: utf-8 -*-
"""
@author: ARuby - 20/02/2020
@author: MVallee - 30/10/2020 (move into separate file)
"""


import csv
from re import ASCII
import numpy as np
from sys import platform as _platform
import os
import json
import matplotlib.colors as col


from lxml import etree, objectify

def getOSsep():
    if _platform == "win32":
      return '\\'
    elif _platform == "linux":
      return '/'

def wordpath(word):
    return getOSsep()+word+getOSsep()

def setColor(projectsPath, x_labels_pos):
    #file should be filled according to https://xkcd.com/color/rgb/
    configfile_name=projectsPath+getOSsep()+"config_Colors.csv"

    if os.path.isfile(configfile_name):
        fields=convertCSV(configfile_name, ';')
#        print("Use color configuration file: ",configfile_name," !!")
#        print(fields)
    else:
        print("Use default colors :",configfile_name,os.path.isfile(configfile_name))
        fields=[]
        fields.append(["Elec","xkcd:light orange"])
        fields.append(["Converter","xkcd:tan"])
        fields.append(["PV","xkcd:gold"])
        fields.append(["WT","xkcd:chartreuse"])
        fields.append(["Rankine","xkcd:purple"])
        fields.append(["Heat","xkcd:dark red"])
        fields.append(["Therm","xkcd:brick red"])
        fields.append(["Solar","xkcd:burnt orange"])
        fields.append(["LNG","xkcd:dark teal"])
        fields.append(["Fluid","xkcd:aqua"])
        fields.append(["NG","xkcd:teal"])
        fields.append(["Gas","xkcd:grey"])
        fields.append(["Methan","xkcd:dark teal"])
        fields.append(["H2","xkcd:blue green"])
        fields.append(["Bio","xkcd:forest green"])

    x_color = np.array([col.to_hex("xkcd:orange") for _ in range (len(x_labels_pos))])
    for i, v in enumerate(x_labels_pos):
      for elem in fields:
          is_elem_in_label = elem[0].replace(' ','') in x_labels_pos[i] or elem[0] in x_labels_pos[i]
          if (is_elem_in_label):
              x_color[i] = col.to_hex(elem[1])
    return x_color

def convertCSV(excelName, mdelimiter=';'):
    """Reads the line of a CSV file into a list
    """
    
    list_lines=[]
    with open(excelName, newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=mdelimiter, quotechar='|')
        for row in spamreader:
           list_lines.append(row) 

    return list_lines



def writeFilteredCSV(fichiercsv, lst_strfilter_blk, lst_exclude, threshold, 
                     sumup_name, filter_negative_values, write_csvfile):
    """Writes a CSV file contains only some Persee variables
    """
    
    x_labels=[]
    y_values=[]
    title=""
    csv_filename=sumup_name
    
    print(" - Writting file "+csv_filename+" including contributions of ",
          lst_strfilter_blk, " excluding ",lst_exclude)
#    with open(csv_filename, 'w', newline='') as csvfile:
    with open(csv_filename, 'a+', newline='') as csvfile:
        spamwriter = csv.writer(csvfile, delimiter=';',
                            quotechar=' ', quoting=csv.QUOTE_MINIMAL)
        for line in fichiercsv:
#            print(strfilter, line)
            for word in line:
               for strfilter_blk in lst_strfilter_blk:
                   if strfilter_blk in word:
                        if isValid(word, lst_exclude) and isValid(line[0], 
                                  lst_exclude):
                          if (write_csvfile) :
                              spamwriter.writerow(line)
                          if (filter_negative_values and float(line[2]) > threshold) \
                          or ( not filter_negative_values and abs(float(line[2])) > threshold):
                              
                              x_labels.append(line[0])
                              y_values.append(float(line[2]))
                              title=line[1]
                          continue
    return x_labels, y_values, title

def isValid (word, lst_exclude):
    """ Tells if a word is NOT in list_exclude
    """
    
    for exclude in lst_exclude:
       if exclude in word:
           return False
    return True

def writeSettingstoCsv(fichiercsv,folder):
    """ Writes a list to a CSV file
    """
    
    csv_filename=folder+getOSsep()+'Settings.csv'

        
    print(" - Writting file "+csv_filename)
    with open(csv_filename, 'w', newline='') as csvfile:
        spamwriter = csv.writer(csvfile, delimiter=';',
                            quotechar=' ', quoting=csv.QUOTE_MINIMAL)

        for line in fichiercsv:
#            print(strfilter, line)
            formatted_line=[]
            word_count=0
            for word in line:
                if (word_count==0):
                    formatted_line.append(word.replace("."," ; ",1).replace('=',' ; ').replace(';;',''))
                else:
                    formatted_line.append(word.replace('=',' ; '))
                word_count+=1
            spamwriter.writerow(formatted_line)


def func(pct, allvals):
    """Formats a value (to be detailed)
    """
    # TODO add details
    
    global_sum=sum(allvals)
    if (global_sum < 100):
        absolute = (pct/100.*np.sum(allvals))
        sum_values='{:.1f}% - {:,.2f}'.format(pct, absolute)
    elif (global_sum < 1.e6):
        absolute = (pct/100.*np.sum(allvals))
        sum_values='{:.1f}% - {:,.2f} '.format(pct, absolute)
    elif (global_sum < 1.e9):
        absolute = (pct/100.*np.sum(allvals)/1.e6)
        sum_values='{:.1f}% - {:,.2f} million '.format(pct, absolute)
    elif (global_sum < 1.e12):
        absolute = (pct/100.*np.sum(allvals)/1.e9)
        sum_values='{:.1f}% - {:,.2f} billion '.format(pct, absolute)
    else:
        absolute = (pct/100.*np.sum(allvals)/1.e12)
        sum_values='{:.1f}% - {:,.2f} trillion '.format(pct, absolute)
        
    return sum_values

def func1(pct, allvals):
    """Formats a value (to be detailed)
    """
    # TODO add details
    
    global_sum=sum(allvals)
    if (global_sum < 100):
        absolute = (pct/100.*np.sum(allvals))
        sum_values='{:.1f}%'.format(pct)
    elif (global_sum < 1.e6):
        absolute = (pct/100.*np.sum(allvals))
        sum_values='{:.1f}%'.format(pct)
    elif (global_sum < 1.e9):
        absolute = (pct/100.*np.sum(allvals)/1.e6)
        sum_values='{:.1f}%'.format(pct)
    elif (global_sum < 1.e12):
        absolute = (pct/100.*np.sum(allvals)/1.e9)
        sum_values='{:.1f}%'.format(pct)
    else:
        absolute = (pct/100.*np.sum(allvals)/1.e12)
        sum_values='{:.1f}%'.format(pct)
    return sum_values

def funcvalonly(y_values):
    """Formats a value (to be detailed)
    """
    # TODO add details
    
    global_val=sum(y_values)
    global_sum = abs(global_val)
    if (global_sum < 100):
        sum_values=' {:,.2f}'.format(global_val)
    elif (global_sum < 1.e6):
        sum_values=' {:,.2f} '.format(global_val)
    elif (global_sum < 1.e9):
        sum_values=' {:,.2f} million '.format(global_val/1.e6)
    elif (global_sum < 1.e12):
        sum_values=' {:,.2f} billion '.format(global_val/1.e9)
    else:
        sum_values=' {:,.2f} trillion '.format(global_val/1.e12)
    return sum_values

def funcscalar(global_val):
    """Formats a value (to be detailed)
    """
    # TODO add details
    
    global_sum = abs(global_val)
    if (global_sum < 100):
        sum_values=' {:,.2f}'.format(global_val)
    elif (global_sum < 1.e6):
        sum_values=' {:,.2f} '.format(global_val)
    elif (global_sum < 1.e9):
        sum_values=' {:,.2f} million '.format(global_val/1.e6)
    elif (global_sum < 1.e12):
        sum_values=' {:,.2f} billion '.format(global_val/1.e9)
    else:
        sum_values=' {:,.2f} trillion '.format(global_val/1.e12)
    return sum_values


def reOrder (x_labels,y_values):
    """Reorder both list of labels and values in asceding order
    
    Uses np.argsort on y_values to define the order
    Used to sort labels for plots
    """
    
    #sort labels for increasing values of y
    x_pos_out=[]
    y_pos_out=[]
    x_pos = np.array(x_labels)
    y_pos = np.array(y_values, dtype=float)
    
    ind = np.argsort(y_pos)
#    y_pos = np.sort(y_pos)
    
    for i in range(len(x_pos)):
        x_pos_out.append(x_pos[ind[i]])
        y_pos_out.append(y_pos[ind[i]])
    
#    xy = np.array([x_pos,y_pos])    
#    print(xy)
    
    return x_pos_out, y_pos_out



def elem2dict(node):
    """
    Convert an lxml.etree node tree into a dict.
    ONLY PARSES TEXT, NOT TAG ATTRIBUTES
    https://stackoverflow.com/a/66103841

    TODO Check xmltodict library
    """
    result = {}

    for element in node.iterchildren():
        # Remove namespace prefix
        key = element.tag.split("}")[1] if "}" in element.tag else element.tag

        # Process element as tree element if the inner XML contains non-whitespace content
        if element.text and element.text.strip():
            value = element.text.strip()
        else:
            value = elem2dict(element)
        if key in result:

            if type(result[key]) is list:
                result[key].append(value)
            else:
                tempvalue = result[key]
                result[key] = [tempvalue, value]
        else:
            result[key] = value
    return result


def sankey_xml_from_desc(xml:str, output:str, overwrite:bool=False, aggregate:bool=False):
    """
    DEPRECATED
    Parse a PERSEE _desc.xml file for all nodes and links between components of the system. 
    Excludes any DATAEXCHANGE link.
    Generate a xml file in a specific format to plot sankey diagrams.

    Args:
        xml (str): path to a Persee _desc.xml file
        output (str): name / path to the output .xml file
        overwrite (bool, optional): If True, overwrite the output file if it exists. Defaults to False, creating a different file.
        aggregate (bool, optional): If True aggregate buses by energy vector. Defaults to False, keeping physical buses (safer, but may produce diagrams with many nodes).

    Returns:
        (str): name / path to the output .xml file
    """    
    if not overwrite:
        i=1
        while os.path.isfile(output):
            print(f"! Warning  file {output} already exists")
            output = output.split('.')[0] + f'.{i:0>2}.xml' 
            i+=1
        if i > 1:
            print(f"--- output changed to {output}")

    parser = etree.XMLParser(remove_comments=True)
    tree = objectify.parse(xml, parser=parser)

    # All Energy Vectors
    try:
        eVectors = {e.get('id').strip() :e.find('StorageUnit').text.strip() for e in  tree.findall('.//EnergyVector')}
    except:
        eVectors = {"unknown" for e in tree.findall('.//EnergyVector')}

    # All Energy Buses
    # Dictionnary {Bus: EnergyVector}
    eBus = {e.get('id'): e.find('VectorName').text for e in  tree.findall('.//BusFlowBalance')}
    eBus.update({e.get('id'): e.find('VectorName').text for e in  tree.findall('.//BusSameValue')})

    # All Storages
    eStorages = [e.get('id') for e in  tree.findall('.//Storage')]

    # Converters
    es = tree.findall('.//Converter')
    es += tree.findall('.//SourceLoad')
    es += tree.findall('.//Grid')
    es += tree.findall('.//Source')
    
    # Default name of PERSEE variable according to component type
    default = {'Grid': 'GridFlow', 'Source':'SourceLoadFlow', 'Load':'SourceLoadFlow'}

    fluxes = {}
    # Converters, loads, Grid, Source
    for e in es:
        id = e.get('id')
        for p in e.getchildren():
            use = p.get('Use', '').strip()
            if ('Port' not in p.tag) or use == 'DATAEXCHANGE' or type(p) == "NoneType" :
                continue
            bus = p.text.strip()
            vec = eBus.get(bus) if aggregate else bus
            d = fluxes.get(vec)

            if d is None:
                fluxes[vec] = {'conso': [], 'prod': []}
                d = fluxes[vec]

            entry = {'variable' : id + '.' + p.get('Variable', default.get(e.tag, 'OUTPUTFlux1'))} 
            if p.get('Coeff'):
                entry['factor'] = float(p.get('Coeff'))
        
            if use == 'INPUT':
                d['conso'].append(entry)
            elif use == 'OUTPUT':
                d['prod'].append(entry)
            elif e.find('Direction') is not None:
                if e.find('Direction').text.strip() in ['Source', 'ExtractFromGrid']:
                    d['prod'].append(entry)
                else:
                    d['conso'].append(entry)

#    Storages
    for e in tree.findall('.//Storage'):
        id = e.get('id')
        p = e.find('Port1')
        for p in e.getchildren():
            use = p.get('Use', '').strip()
            if ('Port' not in p.tag) or use == 'DATAEXCHANGE' or type(p) == "NoneType" :
                continue
            bus = p.text.strip()
            vec = eBus.get(bus) if aggregate else bus
            d = fluxes.get(vec)

            if d is None:
                fluxes[vec] = {'conso': [], 'prod': []}
                d = fluxes[vec]

            d['conso'].append({'variable' : id + ".Flow"})
            d['prod'].append({'variable' : id + ".Flow"})
                    
    root = etree.Element("graph")
    storages = etree.SubElement(root, 'storages')
    storages.text = ','.join(eStorages)
    for key, val in fluxes.items():
        vec = etree.SubElement(root, 'vector')
        name = etree.SubElement(vec, 'name')
        name.text = key
        unit = etree.SubElement(vec, 'unit')
        unit.text = eVectors.get(eBus.get(key), 'MWh')

        consos = etree.SubElement(vec, 'conso')
        for conso in val['conso']:
            flow = etree.SubElement(consos, 'flow')
            var = etree.SubElement(flow, 'variable')
            var.text = conso['variable']
            if conso.get('factor') is not None:
                fac = etree.SubElement(flow, 'factor')
                fac.text = str(conso['factor'])

        prods = etree.SubElement(vec, 'prod')
        for prod in val['prod']:
            flow = etree.SubElement(prods, 'flow')
            var = etree.SubElement(flow, 'variable')
            var.text = prod['variable']
            if prod.get('factor') is not None:
                fac = etree.SubElement(flow, 'factor')
                fac.text = str(prod['factor'])

    et = etree.ElementTree(root)
    et.write(output, pretty_print=True)

    return output




def elem2dict(node):
    """
    Convert an lxml.etree node tree into a dict.
    ONLY PARSES TEXT, NOT TAG ATTRIBUTES
    https://stackoverflow.com/a/66103841

    TODO Check xmltodict library
    """
    result = {}

    for element in node.iterchildren():
        # Remove namespace prefix
        key = element.tag.split("}")[1] if "}" in element.tag else element.tag

        # Process element as tree element if the inner XML contains non-whitespace content
        if element.text and element.text.strip():
            value = element.text.strip()
        else:
            value = elem2dict(element)
        if key in result:

            if type(result[key]) is list:
                result[key].append(value)
            else:
                tempvalue = result[key]
                result[key] = [tempvalue, value]
        else:
            result[key] = value
    return result


def sankey_xml_from_desc(xml:str, output:str, overwrite:bool=False, aggregate:bool=False):
    """
    DEPRECATED
    Parse a PERSEE _desc.xml file for all nodes and links between components of the system.
    Excludes any DATAEXCHANGE link.
    Generate a xml file in a specific format to plot sankey diagrams.

    Args:
        xml (str): path to a Persee _desc.xml file
        output (str): name / path to the output .xml file
        overwrite (bool, optional): If True, overwrite the output file if it exists. Defaults to False, creating a different file.
        aggregate (bool, optional): If True aggregate buses by energy vector. Defaults to False, keeping physical buses (safer, but may produce diagrams with many nodes).

    Returns:
        (str): name / path to the output .xml file
    """    
    if not overwrite:
        i=1
        while os.path.isfile(output):
            print(f"! Warning  file {output} already exists")
            output = output.split('.')[0] + f'.{i:0>2}.xml' 
            i+=1
        if i > 1:
            print(f"--- output changed to {output}")

    parser = etree.XMLParser(remove_comments=True)
    tree = objectify.parse(xml, parser=parser)

    # All Energy Vectors
    eVectors = {e.get('id').strip() :e.find('EnergyUnit').text.strip() for e in  tree.findall('.//EnergyVector')}
    eConvert = {e.get('id').strip() :e.find('LHV') for e in  tree.findall('.//EnergyVector')}

    # All Energy Buses
    # Dictionnary {Bus: EnergyVector}
    eBus = {e.get('id'): e.find('VectorName').text for e in  tree.findall('.//BusFlowBalance')}
    eBus.update({e.get('id'): e.find('VectorName').text for e in  tree.findall('.//BusSameValue')})

    # All Storages
    eStorages = [e.get('id') for e in  tree.findall('.//Storage')]

    # Converters
    es = tree.findall('.//Converter')
    es += tree.findall('.//SourceLoad')
    es += tree.findall('.//Grid')
    es += tree.findall('.//Source')
    
    # Default name of PERSEE variable according to component type
    default = {'Grid': 'GridFlow', 'Source':'SourceLoadFlow', 'Load':'SourceLoadFlow'}

    fluxes = {}
    # Converters, loads, Grid, Source
    for e in es:
        id = e.get('id')
        for p in e.getchildren():
            use = p.get('Use', '').strip()
            if ('Port' not in p.tag) or use == 'DATAEXCHANGE' or type(p) == "NoneType" :
                continue
            bus = p.text.strip()
            vec = eBus.get(bus) if aggregate else bus
            d = fluxes.get(vec)

            if d is None:
                fluxes[vec] = {'conso': [], 'prod': []}
                d = fluxes[vec]

            entry = {'variable' : id + '.' + p.get('Variable', default.get(e.tag, 'OUTPUTFlux1'))} 
            if p.get('Coeff'):
                entry['factor'] = float(p.get('Coeff'))
        
            if use == 'INPUT':
                d['conso'].append(entry)
            elif use == 'OUTPUT':
                d['prod'].append(entry)
            elif e.find('Direction') is not None:
                if e.find('Direction').text.strip() in ['Source', 'ExtractFromGrid']:
                    d['prod'].append(entry)
                else:
                    d['conso'].append(entry)

#    Storages
    for e in tree.findall('.//Storage'):
        id = e.get('id')
        p = e.find('Port1')
        for p in e.getchildren():
            use = p.get('Use', '').strip()
            if ('Port' not in p.tag) or use == 'DATAEXCHANGE' or type(p) == "NoneType" :
                continue
            bus = p.text.strip()
            vec = eBus.get(bus) if aggregate else bus
            d = fluxes.get(vec)

            if d is None:
                fluxes[vec] = {'conso': [], 'prod': []}
                d = fluxes[vec]

            d['conso'].append({'variable' : id + ".Flow"})
            d['prod'].append({'variable' : id + ".Flow"})
                    
    root = etree.Element("graph")
    storages = etree.SubElement(root, 'storages')
    storages.text = ','.join(eStorages)
    for key, val in fluxes.items():
        vec = etree.SubElement(root, 'vector')
        name = etree.SubElement(vec, 'name')
        name.text = key
        unit = etree.SubElement(vec, 'unit')
        unit.text = eVectors.get(eBus.get(key), 'kjsdljhgfksdj')
        convert = etree.SubElement(vec, 'convert')
        convert.text = eConvert.get(eBus.get(key), 'None')

        consos = etree.SubElement(vec, 'conso')
        for conso in val['conso']:
            flow = etree.SubElement(consos, 'flow')
            var = etree.SubElement(flow, 'variable')
            var.text = conso['variable']
            if conso.get('factor') is not None:
                fac = etree.SubElement(flow, 'factor')
                fac.text = str(conso['factor'])

        prods = etree.SubElement(vec, 'prod')
        for prod in val['prod']:
            flow = etree.SubElement(prods, 'flow')
            var = etree.SubElement(flow, 'variable')
            var.text = prod['variable']
            if prod.get('factor') is not None:
                fac = etree.SubElement(flow, 'factor')
                fac.text = str(prod['factor'])

    et = etree.ElementTree(root)
    et.write(output, pretty_print=True)

    return output


def sankey_xml_from_json(json_file: str, output: str, overwrite: bool = False, aggregate: bool = False):
    """
    Parse a PERSEE project.json file for all nodes and links between components of the system.
    Excludes any DATAEXCHANGE link.
    Generate a xml file in a specific format to plot sankey diagrams.

    Args:
        json (str): path to a Persee _desc.xml file
        output (str): name / path to the output .xml file
        overwrite (bool, optional): If True, overwrite the output file if it exists. Defaults to False, creating a different file.
        aggregate (bool, optional): If True aggregate buses by energy vector. Defaults to False, keeping physical buses (safer, but may produce diagrams with many nodes).

    Returns:
        (str): name / path to the output .xml file
    """
    if not overwrite:
        i = 1
        while os.path.isfile(output):
            print(f"! Warning  file {output} already exists")
            output = output.split('.')[0] + f'.{i:0>2}.xml'
            i += 1
        if i > 1:
            print(f"--- output changed to {output}")

    with open(json_file, 'r') as study_file:
        parser = json.load(study_file)

    parser = loadStudyCompo(parser, elem_list = ["Components"])
    # All Energy Vectors
    eVectors = dict()
    eConvert = dict()
    for i in parser["Components"]:
        if i["componentPERSEEType"] == "EnergyVector":
            name = (i["nodeName"])
            massUnit = ""
            powerUnit = ""
            massCarrier = False
            for j in i["optionListJson"]:
                if j["key"] == 'IsMassCarrier':
                    massCarrier = j["value"]
                    print("mass",name,massCarrier)
                if j["key"] == 'MassUnit':
                    massUnit = j["value"]
                    print(massUnit)
                if j["key"] == 'PowerUnit':
                    powerUnit = j["value"]
                if j["key"] == 'IsFuelCarrier':
                    fuelCarrier = j["value"]
                    print("fuel",name,fuelCarrier)
            print(type(massCarrier))
            for j in i["paramListJson"]:
                if j["key"] == 'LHV':
                    conversion = j["value"]
                    
            if massCarrier == True:
                print("masse!")
                eVectors[name] = massUnit
                if fuelCarrier == True:
                    eConvert[name] = conversion
                    eVectors[name] = powerUnit
            else:
                eVectors[name] = powerUnit

    eBus = dict()
    eStorages = []
    list_sto = []
    es = []
    other_types = ["Converter", "SourceLoad", "Grid", "Source","Storage"]
    for i in parser["Components"]:
        if i["componentPERSEEType"] == "BusFlowBalance" or i["componentPERSEEType"] == "BusSameValue":
            eBus[i["nodeName"]] = i["componentCarrier"]
        if i["componentPERSEEType"] in other_types:
            es.append(i)

    fluxes = {}
    # Converters, loads, Grid, Source
    for e in es:
        name = e["nodeName"]
        id = e["nodeId"]
        print("writeinsankey: ",name, id)
        for p0 in e["nodePortsData"]:
            for p in p0["ports"]:
                use = p.get('direction', '').strip()
                if use == 'DATAEXCHANGE' or type(p) == "NoneType" or p.get('direction') == '':
                    print("noeud ignoré", p)
                    continue
                # bus = p.text.strip()
                bus = ""
                vec = p["carrier"] if aggregate else ""
                d = fluxes.get(vec)

                if d is None:
                    fluxes[vec] = {'conso': [], 'prod': []}
                    d = fluxes[vec]
                print("name", name, p)
                entry = {'variable': name + '.' + p.get('variable')}
                # entry = {'variable': id + '.' + p.get('Variable', default.get(e.tag, 'OUTPUTFlux1'))}
                if p.get('coeff'):
                    entry['factor'] = float(p.get('coeff'))
                if (e["componentPERSEEType"] == "Storage" and p.get('variable') == "Flow"):
                    entry["variable"] = name + '.ChargeFlow'
                    d['conso'].append(entry)
                    entry2 = entry.copy()
                    entry2['variable'] = name + '.DischargeFlow'
                    d['prod'].append(entry2)
                    eStorages.append(i["nodeName"])
                    list_sto.append(i)
                elif use == 'INPUT' :
                    d['conso'].append(entry)
                elif use == 'OUTPUT':
                    d['prod'].append(entry)

    root = etree.Element("graph")
    storages = etree.SubElement(root, 'storages')
    storages.text = ','.join(eStorages) # anciennement eStorages
    for key, val in fluxes.items():
        vec = etree.SubElement(root, 'vector')
        name = etree.SubElement(vec, 'name')
        name.text = key
        unit = etree.SubElement(vec, 'unit')
        unit.text = eVectors.get(key, 'MWh')
        conv = etree.SubElement(vec, 'conversion')
        conv.text = str(eConvert.get(key,1.))

        consos = etree.SubElement(vec, 'conso')
        for conso in val['conso']:
            flow = etree.SubElement(consos, 'flow')
            var = etree.SubElement(flow, 'variable')
            var.text = conso['variable']
            if conso.get('factor') is not None:
                fac = etree.SubElement(flow, 'factor')
                fac.text = str(conso['factor'])

        prods = etree.SubElement(vec, 'prod')
        for prod in val['prod']:
            flow = etree.SubElement(prods, 'flow')
            var = etree.SubElement(flow, 'variable')
            var.text = prod['variable']
            if prod.get('factor') is not None:
                fac = etree.SubElement(flow, 'factor')
                fac.text = str(prod['factor'])

    et = etree.ElementTree(root)
    et.write(output, pretty_print=True)

    return output


def checking_header_tab_param(tab_param_keys, dictComponentsParam):
    """
    using into PerseeSensParam.py
    header tab_param file validation

    Args:
        tab_param_keys (list): 1st line of tab param file
        dictJsonStudy (dict): set of parameters extracted from study.json file
    Returns:
        paramStudyDtypes (list) : instance of parameters
    """
    paramStudyDtypes = []
    componentParams = {}
    listParams = []
    for t_key in tab_param_keys:
        tab_param_elem = t_key.split("__")
        if len(tab_param_elem) != 3:
            continue
        compoName = str(tab_param_elem[0]).strip()
        paramType = str(tab_param_elem[1]).strip()
        paramName = str(tab_param_elem[2]).strip()
        print("Checking ", compoName, paramType, paramName)
        for c_key in dictComponentsParam.keys():
            if compoName == c_key.strip():
                componentParams = dictComponentsParam[c_key]
        for p_key in componentParams.keys():
            if p_key.strip() == paramType:
                listParams = componentParams[p_key]

        #check if param exists
        if not paramName in str(listParams): #abort exec
            #!!! Attention : This message is used by Persee GUI !!!
            print("Error: parameter not found!", flush=True)
            raise Exception("param key: " + compoName + str(paramName) + " not found .. please review tab_param file")
        else:
            componentGenExp = next(item for item in listParams if item["key"] == paramName)
            try:
                #try converting to integer
                float(componentGenExp["value"])
                paramStudyDtypes.append("number")
            except ValueError:
                paramStudyDtypes.append(type(componentGenExp["value"]).__name__)
    #replace int/float with number
    paramStudyDtypes = ["number" if e =='float' or e == 'float64' or e =='int' or e == 'int64' or e == 'long' else e for e in paramStudyDtypes]
    print("Tab Param Types ", paramStudyDtypes)
    return paramStudyDtypes

def checking_tab_param_rows(tab_param, paramStudyDtypes):
    """
    using into PerseeSensParam.py
    rows tab_param file validation

    Args:
        tab_param (list): the sampling file
        studyContent (dict): the content of study.json file
        paramStudyDtypes (list): instance of parameters (from checking_header_tab_param fct)
    """
    for val in tab_param.values:
        list_param=[]
        for elem in val:
            if isinstance(elem, str):
                if(elem.lower() == 'true' or elem.lower() == 'false'):
                    #detecting bool
                    list_param.append(type(True).__name__)
                    continue
            try:
                #try converting to integer
                float(elem)
                list_param.append("number")
            except ValueError:
                list_param.append(type(elem).__name__)
        #default value could be int
        #case value is mostly float
        #replace int/float with number in order to compare instances
        list_param = ["number" if e =='float' or e == 'float64' or e =='int' or e == 'int64' or e == 'long' else e for e in list_param]
        if not (paramStudyDtypes==list_param) :
            #!!! Attention : This message is used by Persee GUI !!!
            print("Error: wrong parameter type!", flush=True)
            print("study              :", paramStudyDtypes)
            print("tab_echantillonnage:", list_param)
            raise Exception("please review element type of this row : ", str(val))
    #returns void

def get_study_param(studyContent, tab_param_keys=[]):
    """
    extract study's params from study.json file
    # TODO: there is no notion of the component : is it normal ? 
    args:
        studyContent (dict): from study.json

    return:
        dictJsonStudy: dict of parameters indexed by listParamJson values
    """
    listParamJson=['paramListJson', 'optionListJson', 'timeSeriesListJson', 'envImpactsListJson', 'portImpactsListJson']

    dictComponentsParam = {}
    for key in tab_param_keys:
        compoName = key.split("__")[0]
        CompoParamJson = { key: [] for key in listParamJson}
        CompoParamJson['nodePortsData'] = []
        dictComponentsParam[compoName] = CompoParamJson

    studyContent = loadStudyCompo(studyContent)
    for componentParam in studyContent['Components']:
        compoName = componentParam["nodeName"]
        for compo_key in dictComponentsParam.keys(): #InnerLoop
            name_utf8 = compoName.encode("utf-8")
            key_utf8 = compo_key.encode("utf-8")
            name_ascii = name_utf8.decode("ascii", "ignore")
            key_ascii = key_utf8.decode("ascii", "ignore")
            if name_ascii == key_ascii:
                print("Found componenet: ", compoName)
                print("componentParam: ",componentParam)
                for key in listParamJson :
                    if key in componentParam:
                        dictComponentsParam[compo_key][key] = componentParam[key]
                if 'nodePortsData' in componentParam.keys():
                    for side in componentParam['nodePortsData']:
                        print("componentParam")
                        side_name = side["pos"]
                        for p in side["ports"]:
                            port_name=p["name"]
                            key = side_name+"--"+port_name+'--'
                            if "coeff" in p:
                                dictComponentsParam[compo_key]["nodePortsData"].append(dict({"key":key+"coeff", "value":p["coeff"]}))
                            if "offset" in p:
                                dictComponentsParam[compo_key]["nodePortsData"].append(dict({"key":key+"offset", "value":p["offset"]}))
                    break #InnerLoop
    return dictComponentsParam

def loadStudyCompo(jsonContent, elem_list = ["Components","Links","Groups"]):
    # load any study into a dictionnary, assembling Pages.
    studyContent = jsonContent.copy()
    if "Components" not in studyContent:
        nbPages = studyContent["numberPages"]
        p = dict()
        for e in elem_list:
            p[e]=[]
            for i in range(1,nbPages+1): 
                print("page",i)
                if e in elem_list:
                    if e in studyContent["Page"+str(i)]:
                        p[e]+=(studyContent["Page"+str(i)][e])
        studyContent = p
    return studyContent
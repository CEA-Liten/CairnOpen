"""
@author: YMiftah - 17/03/2022
"""

from typing import Dict, List
from xmlrpc.client import boolean
import numpy as np
import pandas as pd
import os
from lxml import etree, objectify
import traceback

import HTMLGraphs
import Utilities as ut
import math
import sys
import shutil
import PerseePasteResults as ppr
import datetime

sys.path.append(os.path.dirname(sys.argv[0]))
script_auto_report_dir = os.path.dirname(__file__)

GRAPH_FUNCTIONS = {
    "PieChart": HTMLGraphs.PieChart,
    "BarGraph": HTMLGraphs.BarGraph,
    "StackBarGraph": HTMLGraphs.StackBarGraph,
    "XYGraph": HTMLGraphs.XYGraph,
    "DynamicGraph": HTMLGraphs.DynamicGraph,
    "MonotonicLoadGraph": HTMLGraphs.MonotonicLoadGraph,
    "SankeyGraph": HTMLGraphs.SankeyGraph,
    "AreaGraph":HTMLGraphs.AreaGraph,
    "TableDiff":HTMLGraphs.Table,
    "MultiPieChart":HTMLGraphs.MultiPieChart,
    "NPV":HTMLGraphs.NPV,
    "DistributionGraph":HTMLGraphs.DistributionGraph,
    "ResolCplex":HTMLGraphs.ResolCplex,
    "XYGraphMultiStudy":HTMLGraphs.XYGraphMultiStudy,
    "Table":HTMLGraphs.SimpleTable,
}

# Dictionnary of graph types that necessitate a SUMUP_TS file
NEED_TS = {
    "SankeyGraph": True,
    "AreaGraph":True,
    "DynamicGraph": True,
    "MonotonicLoadGraph": True,
    "XYGraph": True,
    "DistributionGraph":True
    }

NEED_Table = {
    "TableDiff":True,
    "ResolCplex":True,
    "XYGraphMultiStudy":True
}

class Figure(object):
    """
    Wrapper object containing a plotly figure and the graph properties parsed from a xml file

    """    
    def __init__(self, fig, graphProperties):
        self.fig = fig
        self.gp = graphProperties

    def to_html(self, *args, **kwargs):
        """Write the plotly figure in html format

        Args:
            Arguments of plotly.graph_objects.Figure.to_html()
        """
        return self.fig.to_html(*args, **kwargs)

    def getgp(self, key: str, default=None):
        """Get a graph property

        Args:
            key (str): key to look up
            default (, optional): Value to return if key is not found. Defaults to None.
        """
        return self.gp.get(key, default)


def readGraphProperties(graphPropertiesFile: str) -> List[dict]:
    """
    Extract graph properties from a xml file

    Args:
        graphPropertiesFile (str): path to a graphproperties .xml file

    Returns:
        graphProperties_list (List[dict]): List of graph properties
        sankey_overwrite (bool): read the option of rewriting sankey graph (keyword "overwrite")
    """
    sankey_overwrite=""
    sankey_name=[]
    liste_graphes_types = []
    graphProperties_list = []  # liste de dictionnaires
    if os.path.isfile(graphPropertiesFile):
        parser = etree.XMLParser(remove_comments=True)
        tree = objectify.parse(graphPropertiesFile, parser=parser)
        for graphElem in tree.xpath("/graph_list/graph"):
            graphProperties = ut.elem2dict(graphElem)
            graphProperties_list.append(graphProperties)
            liste_graphes_types.append(graphProperties.get("type"))
            if graphProperties.get("type") == "SankeyGraph" or graphProperties.get("type") == "AreaGraph":
                if graphProperties.get("overwrite") == "True":
                    sankey_overwrite = graphProperties.get("file_path")
                sankey_name.append(graphProperties.get("file_path"))
        reportProperties={}
        for reportParam in tree.xpath("/graph_list/report_param"):
            reportProperties = ut.elem2dict(reportParam)
    return (graphProperties_list, sankey_overwrite,sankey_name,liste_graphes_types, reportProperties)


def filter(df: pd.DataFrame, gp: dict):
    """
    Filter a DataFrame of a sumup data my matching keys
    defined in graph properties

    Args:
        df (pd.DataFrame): DataFrame of sumup
        gp (dict): graph properties

    Returns:
        data: Filtered view of df according to graph properties
    """
    df["Component.Variable"] = df.index
    if "search_strings" in gp:
        # Filter data according to search strings
        lst_strfilter_blk = gp["search_strings"].split(",")
        print("Looking for string : ",lst_strfilter_blk)
        if "divideBy" in gp: 
            lst_strfilter_blk += gp["divideBy"].split(",")
        if "xlabel" in gp: 
            lst_strfilter_blk += gp["xlabel"].split(",")
        selected = df["Component.Variable"].str.contains("|".join(lst_strfilter_blk),na=False)

        # Exclude from excluded strings
        if "excluded_strings" in gp:
            lst_exclude = gp["excluded_strings"].split(",")
            selected = selected & (
                ~df["Component.Variable"].str.contains("|".join(lst_exclude),na=False)
            )
        # filtered dataframe
        res = df[selected]

        # Discard below threshold
        threshold = float(gp.get("threshold", 1e-3))
        # fill with NaN so it doesnt appear in charts")
        bis = res.set_index("Component.Variable").astype("float")
        res = bis.where(abs(bis) > threshold, np.nan)
        # Remove rows and columns that are full nans
        res.dropna(axis=1, how="all", inplace=True)
        res.dropna(axis=0, how="all", inplace=True)
        res = res.reset_index()

        # Drop Variable name
        res["Component"] = res["Component.Variable"].apply(lambda x: x.split(".")[0])
        res["Variable"] = res["Component.Variable"].apply(lambda x: x.split(".")[1])
        res = res.drop("Component.Variable", axis=1)
    else:
        res = df

    return res


def generate_figures(
    graphProperties: List[dict],
    df: pd.DataFrame,
    df_ts: pd.DataFrame,
    app_home: str,
    tables: dict = {}
    ) -> List[Figure]:
    """
    Generate figures from a sumupfile and a time series sumup file

    the sumupfile must be written in the following semi-colon sv format:
                       Component.Variable   T2_A0_ref_GolfeMexique_2005 ... T2_A3_FullENRWind_GolfeMexique_2005
    0   ConsoLNG_Plant11kV.  EnvEmissi...                           0.0 ...                                 0.0
    1   ConsoLNG_Plant11kV.  EnvEmissi...                           0.0 ...                                 0.0
    ...

    the time series dataframe must be written in the following semi-colon sv format:
    
                ConfigCase                      Variable                    0       1
    0  T2bis_A6_Curt_GT...                     Data.Time  2004-12-31 02:00:00     ...
    1  T2bis_A6_Curt_GT...    Converter11kV_690V.PowerIn                    0     ...   
    2  T2bis_A6_Curt_GT...   Converter11kV_690V.PowerOut                    0     ...   
    3  T2bis_A6_Curt_GT...     Converter11kV_6kV.PowerIn                    0     ...  
    ...

    Args:
        graphProperties (List[dict]): graph properties read from a graph properties.xml
        df (pd.DataFrame): DataFrame of a sumupfile

    Returns:
        figures (List[Figure]): list of Figure objects (encapsulation of a plotly figure and a graphproperty)
    """
    figures = []
    for gp in graphProperties:
        # Generate Figure
        func = GRAPH_FUNCTIONS.get(gp["type"], None)
        if func is None:
            continue

        # DATA provided to the graph function is either the SUMUP_PLAN or 
        # The SUMUP_TS
        if NEED_TS.get(gp["type"], False):
            data = df_ts
        elif NEED_Table.get(gp["type"],False):
            data = tables[gp["type"]]
            gp["ncolumns"] = data.keys().size
        else:
            # Filter SUMUP_PLAN according to gp
            data = filter(df, gp)
            if len(data) == 0:
                print(f'! Warning in Figure {gp.get("type")} with title {gp.get("title")}')
                print(f'--- All zeros in data, empty figure discarded')
                continue

        # Generate Figure
        func = GRAPH_FUNCTIONS[gp["type"]]
        if gp["type"] == "SankeyGraph" or gp["type"]=="AreaGraph":
            gp1 = [gp, app_home]
        else:
            gp1 = gp
        try:
            figures.append(Figure(func(data, gp1,folder=app_home), gp))
        except Exception:
            print("==================")
            print("ERROR: figure",gp["title"] ,"not plotted")
            print(traceback.format_exc())
            print("================")
    return figures


def to_HTML(figures: List[Figure], output: str):
    """
    Create the HTML report

    Args:
        figures(List[Figure]) : List of Figure objects
        output (str): name of the html file
    """
    with open(f"{output}", "w") as f:
        print(f'Dumping figures to file {output}')
        f.write(
            r'<head> <title>Report</title> </head> <body style="background-color:#447adb30;" leftmargin="50">'
        )
        f.write(r"<h1>List of figures </h1>")
        f.write(r"<ul>")
        list_titles, list_fig_titles = [], []
        for fig in figures:
            if fig.getgp("title"):
                #default value is not working properly fig.getgp("title", "No Title")
                title = fig.getgp("title")
            else:
                title = "No Title"
            #
            if title in list_titles:
                fig_title = title + " %s"%(list_titles.count(title)+1)
            else:
                fig_title = title
            list_titles.append(title)
            list_fig_titles.append(fig_title)
            #
            f.write(f'<li> <a href="#{fig_title.replace(" ","_")}"> {fig_title} </a></li>')
        f.write(r"</ul>")
        f.write(r"<h1>Figures </h1>")
        for i, fig in enumerate(figures): 
            print('--- Writing figure', list_fig_titles[i])
            if fig.getgp("title"):
                f.write(f'<h2 id={list_fig_titles[i]}> {fig.getgp("title")} </h2>')
            if fig.getgp("comment"):
                f.write(f'<p> {fig.getgp("comment")} </p>')
            #if fig.getgp("title"):
            #f.write(f'<h2 id={list_fig_titles[i].replace(" ","_")}> {list_fig_titles[i]} </h2> <a href="#">top</a>')
            width = "100%"
            if fig.getgp("type") == "TableDiff":
                ncolumns = fig.getgp("ncolumns")  
                if ncolumns > 7:
                    width = str(100*math.ceil(ncolumns/7))+"%"
                    print(width)
            f.write(fig.to_html(full_html=False,include_plotlyjs="directory", default_height= "80%", default_width= width))
    print(f"Written : {output}")


def to_HTML_singlegraph(figure: Figure, output: str):
    """
    Create the HTML report

    Args:
        figures(List[Figure]) : List of Figure objects
        output (str): name of the html file
    """
    with open(f"{output}", "w") as f:
        print(f'Dumping figures to file {output}')
        f.write(figure.to_html(full_html=False,include_plotlyjs="directory", default_height= "80%"))
    print(f"Written : {output}")

def GenerateHTMLReport(app_home, TestCase, prefix, scenarioList=[], config_file_name="", historplan="PLAN", tab_echantillonnage=""):
    # TODO: move to HTMLWriter
    file_name_desc = app_home + ut.getOSsep() + TestCase + ".json"
    print("Read graph properties ... ")
    if config_file_name == "":
        config_file = app_home + ut.getOSsep() + 'config_Extract.xml'
    else:
        config_file = config_file_name
    xml, sankey_overwrite, sankey_name, liste_graphes_types, reportProperties = readGraphProperties(config_file)

    print("Write Sankey...")
    if sankey_overwrite!="":
        ut.sankey_xml_from_json(file_name_desc, app_home + ut.getOSsep() + sankey_overwrite,
                                              overwrite=True, aggregate=True)
        print("Sankey generated in ", sankey_overwrite)
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
        df_diff.columns = df_diff.loc[("Case","Case")]
        df_diff = df_diff.drop("Case")
        df_all = pd.concat([df_plan,df_diff])
    df_all_ind = df_all.index.drop_duplicates(keep=False)
    df_all = df_all.loc[df_all_ind]
    df_all.to_csv(app_home+"\\sumupallwithdiff.csv",sep=";", encoding="utf-8")

    #Set "XYGraphMultiStudy"
    if "XYGraphMultiStudy" in liste_graphes_types:
        table["XYGraphMultiStudy"] = df_all

    print("Extract dataseries ...", sankey_name)
    graphProperties = [config_file_name.split('\\')[-1]]  #edited
    for sn in sankey_name:
        graphProperties.append(sn.split('\\')[-1])
    ppr.PasteSortieFromGraphProperties(app_home, TestCase, scenarioList, prefix, graphProperties, list_order=list_report)
    print(app_home + ut.getOSsep() + 'SUMUP_TS_' + prefix + '.csv')
    try:
        df_ts = pd.read_csv(app_home + ut.getOSsep() + 'SUMUP_TS_' + prefix + '.csv', sep=';', encoding = "utf-8")
    except UnicodeDecodeError:
        df_ts = pd.read_csv(app_home + ut.getOSsep() + 'SUMUP_TS_' + prefix + '.csv', sep=';', encoding = "ISO-8859-1")

    print("Generate figures ...")
    figures = generate_figures(xml, df_all, df_ts, app_home, tables=table)

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
    name_html = html_report_dir + ut.getOSsep() + 'report' 
    if (reportProperties.get("file") == "separate"):
        for f in figures:
            to_HTML_singlegraph(f,name_html + f.getgp("title", default=f.getgp("type")) + ".html")
    else:
        to_HTML(figures, name_html + '.html')
    return name_html


if __name__ == '__main__':
    if len(sys.argv) > 5:  # Do not modify! Called by gui
        app_home = sys.argv[1]
        testcase = sys.argv[2]

        prefix = "Report"
        print("prefix: ", prefix)

        scenarioName = sys.argv[3] 
        scenarioList = []
        if len(scenarioName) > 0:
            scenarioList = scenarioName.split(';')
            print("scenarios:", scenarioList)
        else:
            print("scenarios: * all scenarios")

        configFileName = sys.argv[4]
        colorfile = "config_Colors.csv"
        if configFileName != '':
            print("use config file", configFileName)
        else:
            configFileName = "config_Extract.xml"

        tab_echantillonnage_file = sys.argv[5] #used for order  
 
        openHTMLReport = False
        if len(sys.argv) > 6:
            openHTMLReport = int(sys.argv[6])

    else:
        app_home = r"D:\Users\PP265749\git\cairnopen\tests\integration\\cairn_training"
        testcase = "cairn_training" 
        prefix = "Report_s"
        scenarioList = []       
        #saveStudy = 1
        studyName = "frompycharm"
        reportFolder = ""
        sys.path.append(os.path.dirname(app_home + ut.getOSsep()))
        configFile = "config_Extract.xml"
        configFileName = "config_Extract.xml"
        colorfile = "config_Colors.csv"
        tab_echantillonnage_file="sampling.csv"
        openHTMLReport=True

    print("Module sys searching Path is ", sys.path)
    config_file = os.path.join(app_home, configFileName) 
    if os.path.isfile(config_file):
        print("Read: ", config_file)
    else:
        shutil.copy(os.path.join(script_auto_report_dir,'config_Extract_example.xml'),config_file)
    if os.path.isfile(os.path.join(app_home,colorfile)) == False:
        shutil.copy(os.path.join(script_auto_report_dir,'config_Colors_example.csv'),colorfile)
    if os.path.isfile(os.path.join(app_home, "plotly.min.js")) == False:
        shutil.copy(os.path.join(script_auto_report_dir,'plotly.min.js'),
                    app_home + ut.getOSsep() + "plotly.min.js")
    

    htmlFile = GenerateHTMLReport(app_home, testcase, prefix, scenarioList=scenarioList, config_file_name=config_file, historplan="PLAN", tab_echantillonnage=tab_echantillonnage_file)
    #open the html file using cmd windows
    #os.system("start " + htmlFile)
    if openHTMLReport:
        #print('start "" "'+ htmlFile+'"')
        htmlFile = htmlFile+".html"
        os.system('start "" "'+ htmlFile+'"')

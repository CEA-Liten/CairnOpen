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
import Utilities
import math

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
    sankey_overwrite=False
    sankey_name=""
    liste_graphes_types = []
    graphProperties_list = []  # liste de dictionnaires
    if os.path.isfile(graphPropertiesFile):
        parser = etree.XMLParser(remove_comments=True)
        tree = objectify.parse(graphPropertiesFile, parser=parser)
        for graphElem in tree.xpath("/graph_list/graph"):
            graphProperties = Utilities.elem2dict(graphElem)
            graphProperties_list.append(graphProperties)
            liste_graphes_types.append(graphProperties.get("type"))
            if graphProperties.get("type") == "SankeyGraph" or graphProperties.get("type") == "AreaGraph":
                if graphProperties.get("overwrite") == "True":
                    sankey_overwrite = True
                sankey_name = graphProperties.get("file_path")
    return (graphProperties_list, sankey_overwrite,sankey_name,liste_graphes_types)


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
        selected = df["Component.Variable"].str.contains("|".join(lst_strfilter_blk))

        # Exclude from excluded strings
        if "excluded_strings" in gp:
            lst_exclude = gp["excluded_strings"].split(",")
            selected = selected & (
                ~df["Component.Variable"].str.contains("|".join(lst_exclude))
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
            if fig.getgp("section_title"):
                f.write(f'<h2 id={list_fig_titles[i]}> {fig.getgp("section_title")} </h2>')
            if fig.getgp("comment"):
                f.write(f'<p> {fig.getgp("comment")} </p>')
            #if fig.getgp("title"):
            f.write(f'<h2 id={list_fig_titles[i].replace(" ","_")}> {list_fig_titles[i]} </h2> <a href="#">top</a>')
            width = "100%"
            if fig.getgp("type") == "TableDiff":
                ncolumns = fig.getgp("ncolumns")  
                if ncolumns > 7:
                    width = str(100*math.ceil(ncolumns/7))+"%"
                    print(width)
            f.write(fig.to_html(full_html=False,include_plotlyjs="directory", default_height= "80%", default_width= width))
    print(f"Written : {output}")

"""
@author: YMiftah - 18/03/2022
modified Pimprenelle Parmentier
"""
import os
import plotly.express as px
import plotly.graph_objects as go
import pandas as pd
import numpy as np
from plotly.subplots import make_subplots
import matplotlib.colors as col
import Utilities as utils
from lxml import etree, objectify
from plotly.colors import n_colors
import traceback


from warnings import simplefilter
simplefilter(action="ignore", category=pd.errors.PerformanceWarning)

# Default colors and line styles
COLOR_SCALE = px.colors.qualitative.Bold + px.colors.qualitative.Pastel + px.colors.qualitative.Set3
LINES = ["solid", "dot", "dash", "longdash", "dashdot", "longdashdot"]

"""
Remark: each function takes the same types of arguments:
 - a pandas dataframe **already** filtered with the values from "sumupallwithdiff.csv", or [study]_results_rollinghorizon.csv
 - the part of the config extract as a dict.
 
It is possible for most of graph to use a graphical charter of colors defined in config_color.csv, by activating "colors"=true.
Folder arg of the function is the used to locate this file. 
"""

def PieChart(data: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a Pie Chart

    Args:
        data (pd.DataFrame): Pandas DataFrame of a sumup.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    df = data

    # Only absolute values (costs / consumptions / productions ...)
    df.loc[:, ~df.columns.isin(['Component', 'Variable'])] = df.loc[:,
                                                             ~df.columns.isin(['Component', 'Variable'])].abs()
    # colors of the components are defined by config_color.csv file.
    if folder != "" and graphProperties.get("color", False) == "True":
        x_color = utils.setColor(folder, df["Component"])
        c_dic = {}
        for i, c in enumerate(df["Component"]):
            c_dic[c] = (x_color[i])
        fig = px.pie(
            df, values=df.columns[1], names="Component", title=graphProperties["title"], color="Component",
            color_discrete_map=c_dic
        )
    # colors of the components are randomly defined
    else:
        fig = px.pie(
            df, values=df.columns[1], names="Component", title=graphProperties["title"]
        )

    # Create buttons to select the report.
    buttons = []
    for c in df.drop(["Component", "Variable"], axis=1).columns:
        buttons.append(
            dict(
                method="update",
                label=c,
                args=[{"values": [df[c]], "names": [df["Component"]]}]
            )
        )

    # Update and show figure
    fig.update_layout(updatemenus=[dict(buttons=buttons, direction="down")])
    return fig


def MultiPieChart(data: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a Pie Chart

    Args:
        data (pd.DataFrame): Pandas DataFrame of a sumup.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure of piechart for all the cases, with sizes proportional to the total value.
        List of possible parameters in xml:
        - colors (default False)
        - nbcol (default 3)
        - search_strings (filter)
        - excluded_strings (filter)
        - threshold (filter)
    """
    df = data
    # Only positive values (costs)
    df.loc[:, ~df.columns.isin(['Component', 'Variable'])] = df.loc[:,
                                                             ~df.columns.isin(['Component', 'Variable'])].abs()
    ddf = df.drop(["Variable"], axis=1)  # we only keep the names of components

    # the dataframe is transformed in another dataframe of 4 columns:
    # - one for the index (to used),
    # - one for the name of case, (previously columns)
    # - one for the name of the component,
    # - one for the value
    melted = pd.melt(ddf, id_vars="Component", var_name="Case")
    # the pies are plotted in several columns (set by nbcol parameter).
    # the number of lines is deduced.
    list_cases = list(ddf.columns)
    list_cases.pop()  # remove the column "Component"
    nb_col = int(graphProperties.get("nbcol", 3))
    nb_cases = len(list_cases)
    nb_lines = int(nb_cases / nb_col) + 1
    # characteristic of subplot (spec) for pies.
    specs = [[{'type': 'domain'} for i in range(nb_col)] for j in range(nb_lines)]
    fig = make_subplots(nb_lines, nb_col, specs=specs, subplot_titles=list_cases)
    k = 0
    index_position = [(i, j) for i in range(1, nb_lines + 1) for j in range(1, nb_col + 1)]
    for c in list_cases:
        df_sel = melted[melted["Case"] == c]
        if folder != "" and graphProperties.get("color", True) == "True":
            x_color = utils.setColor(folder, df["Component"])
            c_dic = {}
            for i, c in enumerate(df["Component"]):
                c_dic[c] = x_color[i]
            fig.add_trace(go.Pie(
                labels=df_sel["Component"], values=df_sel["value"], scalegroup='one', marker_colors=x_color),
                index_position[k][0], index_position[k][1])
        else:
            fig.add_trace(go.Pie(
                labels=df_sel["Component"], values=df_sel["value"], scalegroup='one'),
                index_position[k][0], index_position[k][1])
        k += 1
    fig.update_layout(title_text=graphProperties.get("title", "MultiPieChart"))
    return fig


def BarGraph(df: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a BarGraph

    Args:
        df (pd.DataFrame): Pandas DataFrame of a sumup.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure of bargraph, either grouped by component or by case.
        List of possible parameters in xml:
        - colors (default False)
        - search_strings (filter)
        - excluded_strings (filter)
        - threshold (filter)
    """
    if graphProperties.get("groupbycase") == "True":
        ddf = df
        ddf["Var"] = df["Component"] + '.' + df["Variable"]
        ddf.drop(["Variable"], axis=1, inplace=True)
        ddf.drop(["Component"], axis=1, inplace=True)
        # the dataframe is transformed in another dataframe of 4 columns:
        # - one for the index (to used),
        # - one for the name of case, (previously columns)
        # - one for the name of the "Var",
        # - one for the value
        melted = pd.melt(ddf, id_vars="Var", var_name="Case")
        # plot the colors defined in config_color.csv
        if folder != "" and graphProperties.get("color", False) == "True":
            x_color = utils.setColor(folder, ddf["Var"].apply(lambda x: x.split(".")[0]))
            c_dic = {}
            for i, c in enumerate(ddf["Var"]):
                c_dic[c] = x_color[i]
            fig = px.bar(
                melted,
                y="value",
                x="Case",
                color="Var",
                barmode="group",
                title=graphProperties["title"],
                color_discrete_sequence=x_color
            )
            fig.update_xaxes(title=graphProperties.get("xlabel", "Components"))
        # plot the colors randomly
        else:
            fig = px.bar(
                melted,
                y="value",
                x="Case",
                color="Var",
                barmode="group",
                title=graphProperties["title"],
                color_discrete_sequence=COLOR_SCALE
            )
            fig.update_xaxes(title=graphProperties.get("xlabel", "Components"))
    else:
        df["Component"] = df["Component"] + '.' + df["Variable"]
        df.drop(["Variable"], axis=1, inplace=True)
        print("df:",df)
        melted = pd.melt(df, id_vars="Component", var_name="Case")
        fig = px.bar(
            melted,
            y="value",
            x="Component",
            color="Case",
            barmode="group",
            title=graphProperties["title"],
            color_discrete_sequence=COLOR_SCALE
        )
    fig.update_yaxes(title=graphProperties.get("ylabel", ""))
    return fig


def StackBarGraph(df: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a BarGraph

    Args:
        df (pd.DataFrame): Pandas DataFrame of a sumup.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly stackbargraph, with one stack for one case
        It is possible to plot this stack bar graph with a characteristic of the case as x-axis
        (by default it's the name of the case).
        List of possible parameters in xml:
        - xlabel (default Case)
        - colors (default False)
        - search_strings (filter)
        - excluded_strings (filter)
        - threshold (filter)
    """
    case = graphProperties.get("xlabel", "Case")
    # try to replace the name of the case with a line taken in "sumupallwithdiff".
    if case != "Case":
        try:
            case_str = case.replace(' . ', '.').split(".")[1]
            print(case_str)
            line_case = list(df[df.Variable.str.contains(case_str)].index)[0]
            df.loc[line_case, "Variable"] = "Variable"
            df.loc[line_case, "Component"] = "Component"
            df.columns = list(df.loc[line_case].astype(str))
            df.drop(index=line_case, axis=0, inplace=True)
        #if it doesn't work, we take the name of the case.
        except Exception:
            print(case, "not found !")
            print("*****************")
            print(traceback.format_exc())
            print("*************")
            case = "Case"
    # to compute for instance the decomposed LCOH
    # it's possible to divide by a line in sumupallwithdiff
    # be carefull to add this line in search string to have it in the filtered results.
    if graphProperties.__contains__("divideBy"):
        div = graphProperties.get("divideBy","")
        diviseur = df[df.Variable.str.contains(div)]
        df["Component"].loc[diviseur.index] = "Diviseur"
        for c in df.columns:
            if c != "Variable" and c!="Component":
                df[c] = df[c].astype(float)/float(diviseur[c].values[0])
        df = df.drop(diviseur.index, axis=0)
    # the dataframe is transformed in another dataframe of 4 columns:
    # - one for the index (to used),
    # - one for the name of case, (previously columns)
    # - one for the name of the component,
    # - one for the value
    melted = pd.melt(
        df.drop("Variable",axis=1),
        id_vars="Component",
        var_name=case
    )
    if folder != "" and graphProperties.get("color", False) == "True":
        x_color = utils.setColor(folder, df["Component"])
        c_dic = {}
        for i, c in enumerate(df["Component"]):
            c_dic[c] = (x_color[i])
        fig = px.bar(
            melted,
            x=case,
            y="value",
            color="Component",
            title=graphProperties.get("title", ''),
            color_discrete_map=c_dic
        )
    else:
        fig = px.bar(
            melted,
            x=case,
            y="value",
            color="Component",
            title=graphProperties.get("title", ''),
        )

    fig.update_layout(barmode='relative')
    fig.update_yaxes(title=graphProperties.get("ylabel", ""))
    fig.update_xaxes(title=graphProperties.get("xlabel", "Case"))
    return fig


def XYGraph(df_ts: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a ScatterGraph

    Args:
        df (pd.DataFrame): Pandas DataFrame of a sumup_ts.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    xvar = graphProperties.get('xvar').split(',')  # only the first of the list will be used.
    yvar = graphProperties.get('yvar').split(',')  # as many as wanted
    variables = xvar + yvar
    dd = df_ts[df_ts.Variable.isin(variables)]

    fig = go.Figure()

    if graphProperties.get('minimum_date') and graphProperties.get('maximum_date'):
       fig['layout']['xaxis'].update(range=[
           graphProperties.get('minimum_date').strip("'"),
           graphProperties.get('maximum_date').strip("'")])


    for i, case in enumerate(dd.ConfigCase.unique()):
        print(i, case)
        data = dd[dd.ConfigCase == case]
        data = data.drop("ConfigCase", axis=1).set_index('Variable').T
        data = data.dropna(how='all')
        for j, var in enumerate(data.columns):
            if var == 'Data.Time':
                continue
            if var == xvar[0]:
                continue
            try:
                fig.add_trace(
                    go.Scatter(x=data[xvar[0]].astype(float), y=data[var].astype(float), name=var,
                               legendgroup=i, legendgrouptitle={'text': case}, mode='markers',
                               visible=True if i == 0 else 'legendonly', marker_color=COLOR_SCALE[i]
                               )
                )
            except:
                print(xvar[0] +" and " + var + " not found !!!!!")
                continue
    fig.update_layout(
        title_text=graphProperties.get('title', ''),
        legend=dict(groupclick="toggleitem")
    )
    fig.update_yaxes(title=graphProperties.get("ylabel", str(yvar)))
    fig.update_xaxes(title=graphProperties.get("xlabel", xvar[0]))
    return fig


def XYGraphMultiStudy(df: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a ScatterGraph

    Args:
        df (pd.DataFrame): Pandas DataFrame of a sumup.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    print(df)
    #df["Component.Variable"] = df["Component"] + "." + df["Variable"]
    #df = df.drop(["Component", "Variable"], axis=1)
    #df = df.set_index(df["Component.Variable"])
    try:
        df = df.drop(["Component.Variable"],axis=1)
    except:
        print("No component.Variable")
    ddf = df.transpose()
    print(os.getcwd())
    #ddf.to_csv(os.getcwd()+'test_dsklkhj.csv')
    xvar = ""
    yvar = []
    found_y = False
    for c in ddf.columns:
        compoName = str(graphProperties["xvar"].split(".")[0]).strip()
        indicatorName = str(graphProperties["xvar"].split(".")[1]).strip()
        if compoName in str(c) and indicatorName in str(c):
            xvar = c
        for y in graphProperties["yvar"].split(","):
            compoName = str(graphProperties["yvar"].split(".")[0]).strip()
            indicatorName = str(graphProperties["yvar"].split(".")[1]).strip()
            if compoName in str(c) and indicatorName in str(c):
                yvar.append(c)
                found_y = True
    if found_y == False:
        print("None of " + graphProperties["yvar"]+ " found in results")
        ddf["0"]=0
        yvar.append("0")

    if len(yvar) > 0 and len(xvar) > 0:
        print("XYGraphMultiStudy Plot: ", xvar, yvar)
        fig = px.scatter(
            ddf,
            x=pd.to_numeric(ddf[xvar], errors='coerce', downcast='float'),
            y=(ddf[yvar[0]].astype(float)),
        )
    else:
        print("XYGraphMultiStudy Plot: ", xvar, "[ALL]")
        fig=px.scatter(ddf)
    fig.update_yaxes(title=graphProperties.get("ylabel", yvar[0]))
    fig.update_xaxes(title=graphProperties.get("xlabel", xvar))
    return fig


def MonotonicLoadGraph(df_ts: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a MonotonicLoadGraph (time series)

    Args:
        df_ts (pd.DataFrame): Pandas DataFrame of a sumup_ts.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    variables = graphProperties.get('plotted_variables').split(',')
    dd = df_ts[df_ts.Variable.isin(variables + ['Data.Time'])]
    ascending = graphProperties.get('ascending','False')
    if ascending=="True":
        print("ascending = true")
        asc = True
    else:
        asc = False
    fig = go.Figure()
    for i, case in enumerate(dd.ConfigCase.unique()):
        data = dd[dd.ConfigCase == case]
        data = data.drop("ConfigCase", axis=1).set_index('Variable').T
        data['Data.Time'] = pd.to_datetime(data['Data.Time'])
        data.loc[:, data.columns != 'Data.Time'] = data.loc[:, data.columns != 'Data.Time'].astype(float)
        data = data.dropna(how='all')
        data.sort_values('Data.Time', inplace=True)

        for j, var in enumerate(data.columns):
            if var == 'Data.Time':
                continue
            foo = data[['Data.Time', var]].sort_values(var, ascending=asc)
            x = foo['Data.Time'].max() - foo['Data.Time'].min()
            x = np.linspace(0, x.total_seconds() / 3600, foo.shape[0])
            fig.add_trace(
                go.Scatter(x=x, y=foo[var], name=var,
                           legendgroup=i, legendgrouptitle={'text': case},
                           visible=True if i == 0 else 'legendonly',
                           line=dict(color=COLOR_SCALE[i], dash=LINES[j])
                           )
            )

    fig.update_layout(
        title_text=graphProperties.get('title', ''),
        legend=dict(groupclick="toggleitem")
    )

    fig.update_yaxes(title=graphProperties.get("ylabel", "ylabel"))
    fig.update_xaxes(title=graphProperties.get("xlabel", "Time [hours]"))
    return fig


def DynamicGraph(df_ts: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a DynamicGraph (time series)

    Args:
        df_ts (pd.DataFrame): Pandas DataFrame of a sumup_ts.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    variables = graphProperties.get('plotted_variables').split(',')
    dd = df_ts[df_ts.Variable.isin(variables + ['Data.Time'])]

    fig = go.Figure()
    for i, case in enumerate(dd.ConfigCase.unique()):
        data = dd[dd.ConfigCase == case]
        data = data.drop("ConfigCase", axis=1).set_index('Variable').T
        data['Data.Time'] = pd.to_datetime(data['Data.Time']) #  origin=datetime.date(2030,1,1) 
        data.loc[:, data.columns != 'Data.Time'] = data.loc[:, data.columns != 'Data.Time'].astype(float)
        data = data.dropna(how='all')
        data.sort_values('Data.Time', inplace=True)

        for j, var in enumerate(data.columns):
            if var == 'Data.Time':
                continue
            fig.add_trace(
                go.Scatter(x=data['Data.Time'], y=data[var], name=var,
                           legendgroup=i, legendgrouptitle={'text': case},
                           visible=True if i == 0 else 'legendonly',
                           line=dict(color=COLOR_SCALE[i], dash=LINES[j])
                           )
            )

    # Add range slider
    fig.update_layout(
        xaxis=dict(
            rangeslider=dict(
                visible=True,
            ),
            type="date"
        ),
        title_text=graphProperties.get('title', ''),
        legend=dict(groupclick="toggleitem")
    )

    if graphProperties.get('minimum_date') and graphProperties.get('maximum_date'):
        fig['layout']['xaxis'].update(range=[
            graphProperties.get('minimum_date').strip("'"),
            graphProperties.get('maximum_date').strip("'")])

    fig.update_yaxes(title=graphProperties.get("ylabel", "ylabel"))
    fig.update_xaxes(title=graphProperties.get("xlabel", "Time"))
    return fig

def DistributionGraph(df_ts: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a Bar graph (time series)

    Args:
        df_ts (pd.DataFrame): Pandas DataFrame of a sumup_ts.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    variables = graphProperties.get('plotted_variables').split(',')
    dd = df_ts[df_ts.Variable.isin(variables + ['Data.Time'])]

    fig = go.Figure()
    for i, case in enumerate(dd.ConfigCase.unique()):
        data = dd[dd.ConfigCase == case]
        data = data.drop("ConfigCase", axis=1).set_index('Variable').T
        data['Data.Time'] = pd.to_datetime(data['Data.Time'])
        data.loc[:, data.columns != 'Data.Time'] = data.loc[:, data.columns != 'Data.Time'].astype(float)
        data = data.dropna(how='all')
        data.sort_values('Data.Time', inplace=True)

        for j, var in enumerate(data.columns):
            if var == 'Data.Time':
                continue
            fig.add_trace(
                go.Histogram(x=data[var], name=var,
                           legendgroup=i, legendgrouptitle={'text': case},
                           visible=True if i == 0 else 'legendonly',
                           marker_color=COLOR_SCALE[i]
                           )
            )

    if graphProperties.get('minimum_date') and graphProperties.get('maximum_date'):
        fig['layout']['xaxis'].update(range=[
            graphProperties.get('minimum_date').strip("'"),
            graphProperties.get('maximum_date').strip("'")])

    fig.update_layout(barmode='overlay')
    fig.update_traces(opacity=0.75)
    fig.update_yaxes(title=graphProperties.get("ylabel", "Number of timestep"))
    fig.update_xaxes(title=graphProperties.get("xlabel", "MW"))
    return fig

def NPV(df: pd.DataFrame, graphProperties: dict, folder=""):
    print("dataframe:", df)
    df_loc = df.copy()
    df_loc.set_index("Component.Variable",inplace=True)
    try:
        tab_annees = df_loc.loc["TecEcoAnalysis#1 . NbYear "]
        tab_capex = df_loc.loc["TecEcoAnalysis#1 . Total CAPEX "].astype(float)
        tab_opex = df_loc.loc["TecEcoAnalysis#1 . UnDiscounted Net OPEX "].astype(float)
        tab_discount_rate = df_loc.loc["TecEcoAnalysis#1 . Discount Rate "].astype(float)
    except:
        tab_annees = df_loc.loc["TecEco . NbYear "]
        tab_capex = df_loc.loc["TecEco . Total CAPEX "].astype(float)
        tab_opex = df_loc.loc["TecEco . UnDiscounted Net OPEX "].astype(float)
        tab_discount_rate = df_loc.loc["TecEco . Discount Rate "].astype(float)
    dept_year = graphProperties.get("start_year",0)
    j=0
    fig = go.Figure()
    for c in df_loc.columns:
        cf1 = - float(tab_capex[c])
        cash_flow = []
        for t in range(int(tab_annees[c])):
            cf1 = cf1 - float(tab_opex[c]) / (pow(1 + float(tab_discount_rate[c]), t))
            cash_flow.append(cf1)
        fig.add_trace(go.Scatter(x=[dept_year + i for i in range(int(tab_annees[c]))], y=cash_flow,
                                 mode='lines+markers',name=c,line=dict(color=COLOR_SCALE[j])))
        fig.add_trace(go.Scatter(x=[dept_year-1], y=[(-float(tab_capex[c]))], mode='markers',
                                 name=c+" capex",line=dict(color=COLOR_SCALE[j])))
        j+=1
    fig.update_layout(barmode='relative')
    fig.update_layout(title=graphProperties.get("title","Cashflow for each case"))
    fig.update_yaxes(title=graphProperties.get("ylabel", "Currency"))
    fig.update_xaxes(title=graphProperties.get("xlabel", "Year"))
    return fig

def SimpleTable(df_table: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a Table

    Args:
        df_ts (pd.DataFrame): Pandas DataFrame of a sumup_ts.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    # put the first two coluns as Component and Variable
    df_table = df_table[['Variable'] + [col for col in df_table.columns if col != 'Variable']]
    df_table = df_table[['Component'] + [col for col in df_table.columns if col != 'Component']]
    df_table.sort_values(by="Component",inplace=True)
    fig = go.Figure(data=[go.Table(header=dict(values= list(df_table.columns)),
                                        cells=dict(values=[df_table[name] for name in df_table.columns]))])
    return fig


def Table(df_table: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Draws a Table of diff between several studies

    Args:
        df_table (pd.DataFrame): Pandas DataFrame of diff determined previously
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    colors = n_colors('rgb(255, 200, 200)', 'rgb(200, 0, 0)', len(df_table.index) + 1, colortype='rgb')
    tab_col = [COLOR_SCALE]
    # coloration of cells depending on their values.
    for col in (df_table.columns[1:]):
        i = df_table[col]
        try:
            i_f = pd.to_numeric(i)
            maxi = i_f.max()
            mini = i_f.min()
            tab_col.append([colors[int(len(df_table.index) * (val - mini) / (maxi - mini))] for val in i_f])
        except:
            try:
                values = list(set(i.tolist()))
                maxi = len(values)
                mini = 0

                indexes = [values.index(j) for j in i]
                tab_col.append([(colors[int(len(df_table.index) * (val - mini) / (maxi - mini))]) for val in indexes])
            except:
                tab_col.append(["grey" for val in i])
    try:
        fig = go.Figure(data=[go.Table(header=dict(values=[c.replace("paramListJson.","").replace("optionListJson.","") for c in list(df_table.columns)]),
                                    cells=dict(values=[df_table[name] for name in df_table.columns],
                                                fill=dict(color=tab_col)))
                            ])
    except:
        fig = go.Figure(data=[go.Table(header=dict(
            values=[c.replace("paramListJson.", "").replace("optionListJson.", "") for c in list(df_table.columns)]),
                                        cells=dict(values=[df_table[name] for name in df_table.columns]))
                                ])
    fig.update_layout(
        title_text=graphProperties.get('title', '')
    )
    try:
        fig.update_layout(xaxis=dict(rangeslider=dict(visible=True),
                             type="linear"))
    except Exception as e:
        #cannot add the croolbar
        print(e)
        pass
    return fig


def preprocess_sankey(df_ts: pd.DataFrame, graphProperties: dict, folder=""):
    """Process a SUMUP_TS_Report.csv file into a useable format for a sankey diagram
    
    Input file must be in the following format, with variables written as rows:

                ConfigCase                      Variable                    0       1
    0  T2bis_A6_Curt_GT...                     Data.Time  2004-12-31 02:00:00     ...
    1  T2bis_A6_Curt_GT...    Converter11kV_690V.PowerIn                    0     ...   
    2  T2bis_A6_Curt_GT...   Converter11kV_690V.PowerOut                    0     ...   
    3  T2bis_A6_Curt_GT...     Converter11kV_6kV.PowerIn                    0     ... 


    Args:
        df_ts (pd.DataFrame): DataFrame of a SUMUP_TS file
        graphProperties (dict): Dictionnary of graph properties + a string for having app_home

    Return:
        A pandas DataFrame with following format:

                ConfigCase     Source           Flow               Sink
    0  T2bis_A6_Curt_GT...  PV_Field1   83127.831901  NodeEquality135kV   
    1  T2bis_A6_Curt_GT...  PV_Field1   83210.911495  NodeEquality135kV   
    2  T2bis_A6_Curt_GT...  PV_Field1   83127.831901  NodeEquality135kV 
    ...

    """

    sankey = graphProperties[1] + "\\" + graphProperties[0].get('file_path')
    parser = etree.XMLParser(remove_comments=True)
    vector_selected = graphProperties[0].get('vector',{})

    print("Use sankey contribution : ", sankey)
    if not os.path.isfile(sankey):
        print(" This file doesn't exist !! ", sankey)
        return df_ts
    else:
        tree = objectify.parse(sankey, parser=parser)

    tree = etree.parse(sankey)

    # !! Inplace modification of graphProperties
    graphProperties[0].update(utils.elem2dict(tree.getroot()))

    # ---------- Filter df_ts based on 'minimum_time' and 'maximum_time' ---------
    #This is based on the format of SUMUP_TS_Report.csv

    first_col = None
    last_col = None

    time_col_name_1 = None
    time_col_name_2 = None

    #Name of Time|Date column
    if graphProperties[0].get('minimum_time') is not None:
        time_col_name_1 = "Time"
    if graphProperties[0].get('maximum_time') is not None: 
        time_col_name_2 = "Time"

    if graphProperties[0].get('minimum_date') is not None:
        time_col_name_1 = "Data.Time"
    if graphProperties[0].get('maximum_date') is not None: 
        time_col_name_2 = "Data.Time"

    #If attribute 'minimum_time' or 'maximum_time' or 'minimum_date' or 'maximum_date' exists 
    if time_col_name_1 is not None or time_col_name_2 is not None: 
        #look fir "Time" row
        i_row_Time_1 = None
        for i, val in enumerate(df_ts["Variable"].tolist()):
            if val == time_col_name_1:
                i_row_Time_1 = i
                break
        i_row_Time_2 = None
        for i, val in enumerate(df_ts["Variable"].tolist()):
            if val == time_col_name_2:
                i_row_Time_2 = i
                break
        #
        if i_row_Time_1 is not None: 
            minimum_time = None 
            if graphProperties[0].get('minimum_time') is not None:
                try:
                    minimum_time = float(graphProperties[0].get('minimum_time'))
                    print("minimum_time: ", minimum_time)
                except ValueError:
                    print("Warnning: minimum_time %s is not a number. As a result, it is not possible to apply minimum_time."%graphProperties[0].get('minimum_time'))
            if graphProperties[0].get('minimum_date') is not None:
                minimum_time = graphProperties[0].get('minimum_date').replace('"', '')
                print("minimum_date: ", minimum_time)
            #
            if minimum_time is not None:
                for c, col_name in enumerate(df_ts.columns):
                    if col_name == "ConfigCase" or col_name == "Variable":
                        continue
                    col_list = df_ts[col_name].tolist()
                    if type(minimum_time) == type(1.0): #is float
                        try:
                            if float(col_list[i_row_Time_1]) >= minimum_time and first_col is None:
                                first_col = c
                                print("First column name %s, index %s: "%(col_name, first_col))
                                break
                        except ValueError:
                            print("Warnning: time %s in column %s of %s is not a number"%(col_list[i_row_Time_1], col_name, graphProperties[0].get('file_path')))
                    elif col_list[i_row_Time_1] == minimum_time and first_col is None:
                        first_col = c
                        print("First column name %s, index %s: "%(col_name, first_col))
                        break

        if i_row_Time_2 is not None: 
            maximum_time = None 
            #If attribute 'maximum_time' exists 
            if graphProperties[0].get('maximum_time') is not None:
                try:
                    maximum_time = float(graphProperties[0].get('maximum_time'))
                    print("maximum_time: ", maximum_time)
                except ValueError:
                    print("Warnning: maximum_time %s is not a number. As a result, it is not possible to apply maximum_time."%graphProperties[0].get('maximum_time'))
            if graphProperties[0].get('maximum_date') is not None:
                maximum_time = graphProperties[0].get('maximum_date').replace('"', '')
                print("maximum_date: ", maximum_time)
            if maximum_time is not None:
                previous_col = None
                for c, col_name in enumerate(df_ts.columns):
                    if col_name == "ConfigCase" or col_name == "Variable":
                        continue
                    col_list = df_ts[col_name].tolist()
                    if type(maximum_time) == type(1.0): #is float
                        try:
                            if float(col_list[i_row_Time_2]) > maximum_time and last_col is None:
                                last_col = previous_col
                                print("Last column name %s, index %s: "%(previous_col_name,last_col))
                                break
                        except ValueError:
                            print("Warnning: time %s in column %s of %s is not a number"%(col_list[i_row_Time_2], col_name, graphProperties[0].get('file_path')))
                    elif col_list[i_row_Time_2] == maximum_time and last_col is None:
                        last_col = c
                        print("Last column name %s, index %s: "%(col_name,last_col))
                        break
                    previous_col = c
                    previous_col_name = col_name
        elif minimum_time is not None or maximum_time is not None:
            print("Warnning: no column found for Time. As a result, it is not possible to apply minimum_time and/or maximum_time.")

    #-----------------------------------------------------------------------------

    try:
        storage_names = graphProperties[0].get("storages", "").split(',')
    except:
        storage_names=[]
    if storage_names == [''] or len(storage_names) == 0:
        storage_names = []
    print("dataframe:", df_ts)
    ddf_ts = df_ts[~df_ts.Variable.isin(['Data.Time'])]
    df = ddf_ts.loc[:, ["ConfigCase", "Variable"]]
    if first_col is None:
        first_col = 2
    if last_col is None:
        print("Columns from index %s to the end"%first_col)
        df["sum"] = ddf_ts.iloc[:, first_col:].astype(float).sum(axis=1)
    else:
        print("Columns from index %s to index %s"%(first_col, last_col))
        df["sum"] = ddf_ts.iloc[:, first_col:last_col].astype(float).sum(axis=1)

    df = df.pivot_table(index="ConfigCase", columns="Variable", values="sum")

    df_sankey = pd.DataFrame()
    print("===================================", graphProperties[0])
    for v in graphProperties[0].get("vector"):
        
        try:
            name = v.get("name")
        except:
            # si il n'y a qu'un seul vecteur énergétique, v est égal à "name" je ne sais pas pourquoi.
            v=graphProperties[0]["vector"]
            name = v.get("name")
        if name in vector_selected or vector_selected=={}:
            print("name",name,"in",vector_selected)
            # ===== Process Productions
            flows = v.get("prod").get("flow")
            conversion = float(v.get("conversion",1))
            # list of dicts if multiple flows, dict if single flow
            if type(flows) == list:
                prod = [(f.get("variable"), float(f.get("factor", 1))*conversion) for f in flows]
            elif type(flows) == dict:
                prod = [(flows.get("variable"), float(flows.get("factor", 1))*conversion)]
            else:
                prod = []
            # Filter variables to keep only those found in the data
            keys, coef = [], []
            for k, c in prod:
                if k not in df.columns:
                    print(f"! Warning in figure {graphProperties[0].get('type')} - {graphProperties[0].get('title')}")
                    print(f"--- Variable {k} not found in data")
                else:
                    keys.append(k)
                    coef.append(c)

            P = pd.melt(
                (df[keys] * np.array(coef)).reset_index(),
                id_vars="ConfigCase",
                value_name="Flow",
            )
            P["Sink"] = name
            P.rename(columns={"Variable": "Source"}, inplace=True)
            P.Source = P.Source.apply(lambda x: x.split(".")[0])

            df_sankey = pd.concat([df_sankey, P])

            # ======= Process consumptions 
            flows = v.get("conso").get("flow")
            # list of dicts if multiple flows, dict if single flow
            if type(flows) == list:
                conso=[]
                for f in flows:
                    print(f.get("variable").split(".")[-1])
                    component = f.get("variable").split(".")[0]
                    variable = f.get("variable").split(".")[-1]
                    if component in storage_names and variable == "Flow":
                        conso.append((f.get("variable").replace("Flow","ChargeFlow"), float(f.get("factor", 1)) * conversion) )
                        prod.append((f.get("variable").replace("Flow","DischargeFlow"), float(f.get("factor", 1)) * conversion) )
                    else:
                        conso.append((f.get("variable"), float(f.get("factor", 1)) * conversion) )
            elif type(flows) == dict:
                component,variable = flows.get("variable").split(".")[0],flows.get("variable").split(".")[-1]
                if component in storage_names and variable == "Flow":
                    conso = [(flows.get("variable").replace("Flow","ChargeFlow"), float(flows.get("factor", 1)) * conversion)]
                    prod.append[(flows.get("variable").replace("Flow","DischargeFlow"), float(flows.get("factor", 1)) * conversion) ]
                else:
                    conso = [(flows.get("variable"), float(flows.get("factor", 1)) * conversion) ]
            else:
                conso = []
 
            # Filter variables to keep only those found in the data
            keys, coef = [], []
            for k, c in conso:
                if k not in df.columns:
                    print(f"! Warning in figure {graphProperties[0].get('type')} - {graphProperties[0].get('title')}")
                    print(f"--- Variable {k} not found in data")
                else:
                    keys.append(k)
                    coef.append(c)
            P = pd.melt(
                (df[keys] * np.array(coef)).reset_index(),
                id_vars="ConfigCase",
                value_name="Flow",
            )
            P["Source"] = name
            P.rename(columns={"Variable": "Sink"}, inplace=True)
            P.Sink = P.Sink.apply(lambda x: x.split(".")[0])
        else:
            print("problem in sankey: ", name, "vector", v)
            print("vector selected", vector_selected)

        df_sankey = pd.concat([df_sankey, P])

    # remove storages
    if len(storage_names) > 0:
        # Drop storages and recup from sankey diagram
        # The code computes the net flow of a storage
        # The net flow can be positive if SOCInit > 0 (more discharge than charge)
        # - affect negative difference to a loss bus
        # - affect positive difference to a SoC Init bus
        regex = "|".join(storage_names)

        A = df_sankey[df_sankey.Source.str.match(regex)]  # DisCharge # traiter différemment le flux bidirectionnel ? 
        B = df_sankey[df_sankey.Sink.str.match(regex)]  # charge

        A = A.reset_index(drop=True).sort_values(['ConfigCase', 'Sink', 'Source'])
        B = B.reset_index(drop=True).sort_values(['ConfigCase', 'Sink', 'Source'])

        B["Flow"] = B["Flow"] - A["Flow"]  # Charge - Discharge
        pos = B.Flow > 0  # Discharge is lower than charge because of losses
        B.loc[pos, "Sink"] = B.loc[pos, "Sink"] + '_Loss'

        neg = B.Flow < 0  # Discharge is higher than charge because of SoC Init
        B.loc[neg, "Sink"] = B.loc[neg, "Sink"] + '_InitSOC'

        B.loc[neg, ["Sink", "Source"]] = B.loc[neg, ["Source", "Sink"]].values
        B['Flow'] = abs(B['Flow'])

        df_sankey = df_sankey[~df_sankey["Source"].str.contains(regex, regex=True)]
        df_sankey = df_sankey[~df_sankey["Sink"].str.contains(regex, regex=True)]

        df_sankey = pd.concat([df_sankey, B])

    return df_sankey


def SankeyGraph(df_ts: pd.DataFrame, graphProperties: dict, folder=""):
    """Process a SUMUP_TS file into a sankey diagram

    Args:
        df_ts (pd.DataFrame): DataFrame of a SUMUP_TS file
        graphProperties (dict): Dictionnary of graph properties + a string for having app_home (avoid to write full adress for sankey graphs)

    Return:
        A pandas DataFrame
    """
    # Preprocess the data
    sankey = preprocess_sankey(df_ts, graphProperties)

    # find units:
    try:
        try:
            vec2unit = {v.get('name'): v.get('unit') for v in graphProperties[0].get('vector')}
        except:
            vec2unit = {graphProperties[0]['vector']['name']: graphProperties[0]['vector']['unit']}
    except:
        vec2unit = {v.get('name'):"notfound" for v in graphProperties[0].get('vector')}
    sankey['Unit'] = None
    filter = sankey.Sink.isin(vec2unit.keys())
    sankey.loc[filter, 'Unit'] = sankey.loc[filter, 'Sink'].apply(lambda x: vec2unit.get(x))
    filter = sankey.Source.isin(vec2unit.keys())
    sankey.loc[filter, 'Unit'] = sankey.loc[filter, 'Source'].apply(lambda x: vec2unit.get(x))

    # filter almost zeros
    sankey["Flow"] = sankey["Flow"].apply(lambda x: max(x - 1e-6, 0))

    labels = list(set(sankey["Sink"].unique()) | set(sankey["Source"].unique()))
    code = {l: i for i, l in enumerate(labels)}

    sources = {
        case: [
            code[l["Source"]] for _, l in sankey.iterrows() if l["ConfigCase"] == case
        ]
        for case in sankey.ConfigCase.unique()
    }
    targets = {
        case: [code[l["Sink"]] for _, l in sankey.iterrows() if l["ConfigCase"] == case]
        for case in sankey.ConfigCase.unique()
    }
    values = {
        case: [l["Flow"] for _, l in sankey.iterrows() if l["ConfigCase"] == case]
        for case in sankey.ConfigCase.unique()
    }
    units = {
        case: [l["Unit"] for _, l in sankey.iterrows() if l["ConfigCase"] == case]
        for case in sankey.ConfigCase.unique()
    }

    base = list(sources.keys())[0]
    components = labels
    x_color = utils.setColor(folder, components)
    sources_col = {
        case: utils.setColor(folder, [
            (l["Source"]) for _, l in sankey.iterrows() if l["ConfigCase"] == case
        ])
        for case in sankey.ConfigCase.unique()
    }
    sources_names = {
        case: [
            (l["Source"]) for _, l in sankey.iterrows() if l["ConfigCase"] == case
        ]
        for case in sankey.ConfigCase.unique()
    }

    sources_col_rgba = {}
    for colo in (sources_col):
        sources_col_rgba[colo] = []
        for i, c in enumerate(sources_col[colo]):
            sources_col_rgba[colo].append("rgba" + str(col.to_rgba(c, 0.4)))
    fig_sankey = go.Figure()
    fig_sankey.add_trace(
        go.Sankey(
            arrangement="snap",
            node=dict(
                pad=20,
                thickness=20,
                line=dict(width=0.5),
                color=x_color,
                label=labels
            ),
            link=dict(
                source=sources[base],  # indices correspond to labels, eg A1, A2, A1, B1, ...
                target=targets[base],
                value=values[base],
                label=units[base],
                color=sources_col_rgba[base]
            )
        )
    )

    buttons = [
        dict(
            method="update",
            label=col,
            visible=True,
            args=[
                dict(
                    arrangement="snap",
                    node=dict(
                        pad=20,
                        thickness=20,
                        line=dict(color="black", width=0.5),
                        label=labels,
                        color=x_color,
                    ),
                    link=dict(
                        source=sources[col],  # indices correspond to labels, eg A1, A2, A1, B1, ...
                        target=targets[col],
                        value=values[col],
                        color=sources_col_rgba[base]
                    ),
                ),
            ],
        )
        for col in sankey.ConfigCase.unique()
    ]

    # some adjustments to the updatemenus
    updatemenu = [{"buttons": buttons, "direction": "down", "showactive": True,
                   'x': graphProperties[0].get('xpos', 0.5),
                   'y': graphProperties[0].get('ypos', 1.1),
                   }]

    # add dropdown menus to the figure
    fig_sankey.update_layout(updatemenus=updatemenu,
                             title_text=graphProperties[0].get('title', ''),
                             )

    return fig_sankey

def AreaGraph(df_ts: pd.DataFrame, graphProperties: dict, folder=""):

    # Preprocess the data
    sankey = (graphProperties[1] + "\\" + graphProperties[0].get('file_path'))
    parser = etree.XMLParser(remove_comments=True)

    print("AreaGraph: use sankey contribution : ", sankey)
    if not os.path.isfile(sankey):
        print(" Ce fichier n existe pas !! ", sankey)
        return df_ts
    else:
        tree = objectify.parse(sankey, parser=parser)

    tree_sankey = etree.parse(sankey)
    root = tree.getroot()
    variables=[]
    coeff = {}
    cond = False
    storages = ""
    for child in root:
        if child.tag == "storages":
            storages = child.text
            if storages == None:
                storages=[]
        else:
            vect = child.find('name').text
            if vect==graphProperties[0].get('vector'):
                cond = True
                print(vect," selectionné")
                for conso_flow in child.find("conso"):
                    var = conso_flow.find('variable').text
                    variables.append(var)
                    if type(conso_flow.find('factor')) is not type(None) and len(conso_flow.find('factor')) > 0:
                        coeff[conso_flow.find('variable').text] = -float(conso_flow.find('factor').text)
                    else:
                        coeff[conso_flow.find('variable').text] = -1
                for prod_flow in child.find("prod"):
                    var = prod_flow.find('variable').text
                    variables.append(prod_flow.find('variable').text)
                    if type(conso_flow.find('factor')) is not type(None) and len(conso_flow.find('factor')) > 0:
                        coeff[prod_flow.find('variable').text] = float(prod_flow.find('factor').text)
                    else:
                        coeff[prod_flow.find('variable').text] = 1
    if cond == False:
        fig_err = go.Figure()
        fig_err.update_layout(title = "Error: please add a line <vector>vector_name</vector> (with a valid vector name) to the figure")
        return fig_err
    title="Plot Title",
    components = [v.split('.')[0] for v in variables]
    x_color = utils.setColor(folder, components)
    x_color_var = {}
    for var in variables:
        x_color_var[var] = x_color[components.index(var.split('.')[0])]

    dd = df_ts[df_ts.Variable.isin(variables + ['Data.Time'])]

    fig = go.Figure()
    for i, case in enumerate(dd.ConfigCase.unique()):
        data = dd[dd.ConfigCase == case]
        data = data.drop("ConfigCase", axis=1).set_index('Variable').T
        print(data['Data.Time'] )
        print(case)
        data['Data.Time'] = pd.to_datetime(data['Data.Time'])
        data.loc[:, data.columns != 'Data.Time'] = data.loc[:, data.columns != 'Data.Time'].astype(float)
        data = data.dropna(how='all')
        data.sort_values('Data.Time', inplace=True)
        sum_col = data.drop('Data.Time', axis=1).max()
        sum_col_sort = sum_col.sort_values()
        for j, var in enumerate(sum_col_sort.index):
            if var == 'Data.Time':
                continue
            if var.split('.')[0] in storages:
                data_pos = data[var] * coeff[var]
                data_pos[data_pos<=0] = 0
                data_neg = data[var] * coeff[var]
                data_neg[data_neg>=0] = 0
                fig.add_trace(
                    go.Scatter(x=data['Data.Time'], y=-data_pos, name=var,
                               hoverinfo='x+y', stackgroup=case+'two',
                               legendgroup=i, legendgrouptitle={'text': case},
                               visible=True if i == 0 else 'legendonly',
                               line=dict(color=x_color_var[var]), fill='tonexty'
                               )
                )
                fig.add_trace(
                    go.Scatter(x=data['Data.Time'], y=-data_neg, name=var,
                               hoverinfo='x+y', stackgroup=case+'one',
                               legendgroup=i, legendgrouptitle={'text': case},
                               visible=True if i == 0 else 'legendonly',
                               line=dict(color=x_color_var[var]), fill='tonexty'
                               )
                )
            elif coeff[var]>0:
                fig.add_trace(
                    go.Scatter(x=data['Data.Time'], y=coeff[var]*data[var], name=var,
                               hoverinfo='x+y',stackgroup=case+'one',
                               legendgroup=i, legendgrouptitle={'text': case},
                               visible=True if i == 0 else 'legendonly',
                               line=dict(color=x_color_var[var]),fill='tonexty'
                               )
                )
            else:
                fig.add_trace(
                    go.Scatter(x=data['Data.Time'], y=coeff[var] * data[var], name=var,
                               hoverinfo='x+y', stackgroup=case+'two',
                               legendgroup=i, legendgrouptitle={'text': case},
                               visible=True if i == 0 else 'legendonly',
                               line=dict(color=x_color_var[var]), fill='tonexty'
                               )
                )
    # Add range slider
    fig.update_layout(
        xaxis=dict(
            rangeslider=dict(
                visible=True,
            ),
            type="date"
        ),
        title_text=graphProperties[0].get('title', ''),
        legend=dict(groupclick="toggleitem")
    )

    if graphProperties[0].get('minimum_date') and graphProperties[0].get('maximum_date'):
        fig['layout']['xaxis'].update(range=[
            graphProperties[0].get('minimum_date').strip("'"),
            graphProperties[0].get('maximum_date').strip("'")])

    fig.update_yaxes(title=graphProperties[0].get("ylabel", "ylabel"))
    fig.update_xaxes(title=graphProperties[0].get("xlabel", "Time"))
    return fig

def ResolCplex(df_table: pd.DataFrame, graphProperties: dict, folder=""):
    """
    Create a Table

    Args:
        df_ts (pd.DataFrame): Pandas DataFrame of a sumup_ts.csv file
        graphProperties (dict): Dictionnary of graph properties

    Return:
        A plotly figure
    """
    fig = make_subplots(
        rows=2, cols=1,
        shared_xaxes=True,
        vertical_spacing=0.03,
        specs=[[{"type": "table"}],
               [{'secondary_y':True}]]
    )
    #fig.add_trace(go.Table(header=dict(values=[c.replace("paramListJson.", "").replace("optionListJson.", "") for c in list(df_table.columns)]),
    #                       cells=dict(values=[df_table[name] for name in df_table.columns]))
    #              , row=1, col=1)
    fig.update_layout(
        title_text=graphProperties.get('title', '')
    )
    print(df_table["precision"].replace("%","", regex=True))
    fig.add_trace(go.Table(header=dict(values=[c for c in list(df_table.columns)]),
                           cells=dict(values=[df_table[name] for name in df_table.columns]))
                  , row=1, col=1)
    fig.add_trace(go.Bar(x=df_table["Case"], y=(-df_table["objectif"].astype(float)),name="NPV",
                             error_y=dict(type='data',
                                          symmetric=False,
                                          array=-df_table["objectif"].astype(float).mul(((df_table["precision"].replace("%","", regex=True)).astype(float)))/100,
                                          arrayminus=[0 for i in range(len(df_table))])),
                         secondary_y=False,row=2,col=1)
    fig.add_trace(go.Scatter(x=df_table["Case"], y=df_table["tmps_resolution"], name="Time of resolution"),secondary_y=True, row=2,col=1)
    fig.update_yaxes(title_text="NPV", secondary_y=False,row=2,col=1)
    fig.update_yaxes(title_text="Time of resolution (sec)", secondary_y=True, row=2, col=1)
    return fig
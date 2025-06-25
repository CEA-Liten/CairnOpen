# -*- coding: utf-8 -*-
"""
20/08/2018

comparison of 2 csv files
- The NaNs in the file are converted to empty spaces to enable the comparison
- The differences between files are categorised as numeric or NaN
- For numeric differences, the absolute and relative error between the reference and the result is calculted and displayed on the plots and in the logfile
- For the NaNs, no error is calculated, but there is also a logfile output and a plot
- If a column ends with a sequence of NaNs, this last sequence is not used for the comparison of the files.
- check the presence of dateTime column, or only Time in seconds

Bokeh plots
To install this package with conda run one of the following:
conda install -c bokeh bokeh 
conda install -c bokeh/label/dev bokeh
To install this package with pip, one of the following:
pip install -i https://pypi.anaconda.org/bokeh/simple bokeh
pip install -i https://pypi.anaconda.org/bokeh/label/dev/simple bokeh


@author : MN.Descamps
"""
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

#import logging as log
import pandas as pd
import numpy as np

import filecmp
import time
import os

from datetime import datetime

#from pandas.util.testing import assert_frame_equal
#from bokeh.plotting import figure, output_file, show,save
def sort_files(file_plan_ref, file_plan):
    
    df_res=pd.read_csv(file_plan, header=None, sep=";")
    df_ref=pd.read_csv(file_plan_ref, header=None, sep=";")
    
    df_res.columns=["Compo","Var","Val"]
    df_ref.columns=["Compo","Var","Val"]

    df_res.sort_values(by=["Compo","Var","Val"],ignore_index=True,inplace=True)
    df_ref.sort_values(by=["Compo","Var","Val"],ignore_index=True,inplace=True)
    
    df_res.to_csv(file_plan, index=None, header=None, sep=";")
    df_ref.to_csv(file_plan_ref, index=None, header=None, sep=";")    

def compare_plan(file_plan_ref, file_plan):
#compare Persee PLAN file with reference
#        perf_data = [pd.read_csv(os.getcwd()+"\\"+f+"\\Report_"+loc+"\\SUMUP.csv", sep=";",header=None) 
#                     for f in liste_etudes
#                    ]
    file_res = open(file_plan, 'r')
    file_ref = open(file_plan_ref, 'r')
    res = file_res.readlines()
    ref = file_ref.readlines()
    file_res.close()
    file_ref.close()    

    nbline = min(len(res),len(ref))
    maxErr = 0;
    for i in range(nbline):
        if res[i]!=ref[i]:
            s_res = res[i].split(';')
            s_ref = ref[i].split(';')
            try:
                v_res = float(s_res[-1])
                v_ref = float(s_ref[-1])
                if abs(v_ref) < 1e-6:
                    v_ref = 1.0                
                err = abs(v_res-v_ref)/v_ref*100.0
                maxErr = max(maxErr, err)
            except ValueError:
                pass
    return maxErr

def compare_results(app_home, filename_reference="Sortie-reference.csv", filename_resultats="Sortie.csv", skip_rows=[1,2], d=',', logfileName="logfile.txt", log_dir=''):
    infos = {}
    infos["error_message"] = ''
    nan_diff_error = False

    dash = '-' * 40
    logfile = open(logfileName, 'w')

    print(dash)
    print('FILES') 
    print(dash)
    print('Filename reference = ' + filename_reference)
    print('Filename resultats = ' + filename_resultats +' \n')

    logfile.write(dash+'\n')
    logfile.write('FILES\n') 
    logfile.write(dash+'\n')
    logfile.write('Filename reference = ' + filename_reference+'\n')
    logfile.write('Filename resultats = ' + filename_resultats+'\n\n')
    
    # if files are identical, no need to go further
    infos["identical_files"] = filecmp.cmp(filename_reference, filename_resultats)
    if infos["identical_files"]:
        print('Files are identical\n')
        logfile.write('Files are identical\n')
        return infos

    """ ------------------
     compare shape and content of data
    ----------------------""" 
    print(dash)
    print('SHAPE AND CONTENT OF DATA') 
    print(dash)
    logfile.write(dash+'\n')
    logfile.write('SHAPE AND CONTENT OF DATA\n') 
    logfile.write(dash+'\n')

    logfile.flush()

    # check existence of datetime column, or just time(s) column
    #check strings in the cells of 1st line. If no strings, it should mean that there is no column DateTime (?) 
    rs = pd.read_csv(filename_reference, sep=';', decimal=d,skiprows=skip_rows, na_values=['nan'], na_filter=True)

    rs_firstLine = rs.iloc[0]
    Ncell_string = 0
    for i in range(0,len(rs_firstLine)):
        if type(rs_firstLine[i]) ==str:
            column_dateTime = str(rs_firstLine.index[i])
            dateTime_position = i    #indicates where the datetime column is located
            Ncell_string +=1


    try: #case with dateTime
        if Ncell_string > 1:
            print('Several cells contain strings, check input file')
            logfile.write('Several cells contain strings, check input file\n')
            #sys.exit()
            # try to find Data.Time in column names
            for i, col in enumerate(rs.columns):
                columnName=col.replace(" ","")
                if "Data.Time" == columnName:
                    dateTime_position = i
        string_date, string_time = rs_firstLine[dateTime_position].split(' ')
        #dateparser needed for correctly interpreting months/day 
        if len(string_time) == 8:
            dateparse = lambda x :datetime.strptime(x, '%d/%m/%Y %H:%M:%S')
        elif len(string_time) == 6:
            dateparse = lambda x : datetime.strptime(x, '%d/%m/%Y %H:%M')
        else:
            print('Format dateTime not recognized')
            logfile.write('Format dateTime not recognized\n')        
            sys.exit()
            
        resultats = pd.read_csv(filename_resultats, sep=';', decimal=',', index_col=dateTime_position, skiprows=skip_rows,
                            parse_dates=[1], date_parser=dateparse, na_values=['NAN              '], na_filter=True)
        reference = pd.read_csv(filename_reference, sep=';',decimal=',', index_col=dateTime_position, skiprows=skip_rows,
                            parse_dates=[1], date_parser=dateparse, na_values=['nan'], na_filter=True)   
        resultats = convert_df(resultats,False)
        reference = convert_df(reference,False)
    
        if (True in resultats.index.duplicated()):
            raise ValueError
    
    except  NameError:
        print('No column dateTime, use Time (s) as index') 
        logfile.write('No column dateTime, use Time (s) as index\n')   
        logfile.flush()
        resultats = pd.read_csv(filename_resultats, sep=';', decimal=',', index_col=0, skiprows=skip_rows,
                            na_values=['NAN              '], na_filter=True)
        reference = pd.read_csv(filename_reference, sep=';',decimal=',', index_col=0, skiprows=skip_rows,
                            na_values=['nan'], na_filter=True)
        resultats = convert_df(resultats,False)
        reference = convert_df(reference,False)
    except ValueError:
        print('Column dateTime cannot be used (duplicated value due to time change)') 
        logfile.write('Column dateTime cannot be used (duplicated value due to time change or other issue), use Time (s) as index\n')   
        logfile.flush()
        resultats = pd.read_csv(filename_resultats, sep=';', decimal=',', index_col=0, skiprows=skip_rows,
                            na_values=['NAN              '], na_filter=True)
        logfile.write('Successfull read of resultats\n')   
        logfile.flush()
        reference = pd.read_csv(filename_reference, sep=';',decimal=',', index_col=0, skiprows=skip_rows,
                            na_values=['nan'], na_filter=True)
        resultats = convert_df(resultats,True)
        reference = convert_df(reference,True)
        logfile.write('Successfull read of reference\n')   
        logfile.flush()

    #check the fields of each columns, and adjust to be the same in both data set if initially different 
    resultats_fields = resultats.columns.tolist()
    logfile.write('resultats_fields ='+str( resultats_fields)+'\n' )
    logfile.flush()
    reference_fields = reference.columns.tolist()
    logfile.write('reference_fields ='+str( reference_fields)+'\n' )
    logfile.flush()
    list_of_commonFields = list(set(resultats_fields).intersection(reference_fields))
    list_of_differentFields = list(set(resultats_fields).symmetric_difference(reference_fields))    
    logfile.write('list_of_differentFields ='+str( list_of_differentFields)+'\n' )
    logfile.flush()
    if list_of_differentFields: #if true, there are different fields between the sets of data
        print('Extra fields' + str( list_of_differentFields) +' , removing extra fields for comparison')      
        logfile.write('Extra fields' + str( list_of_differentFields) +' , removing extra fields for comparison\n')  
        logfile.flush()
        #further refine to find out which element has been added/removed
        for field in list_of_differentFields:
            if field not in resultats_fields:
                print(str(field) + ' is in ' + filename_reference + ' but not in ' + filename_resultats)
                logfile.write(str(field) + ' is in ' + filename_reference + ' but not in ' + filename_resultats+'\n')            
            if field not in reference_fields:
                print(str(field) + ' is in ' + filename_resultats + ' but not in ' + filename_reference)
                logfile.write(str(field) + ' is in ' + filename_resultats + ' but not in ' + filename_reference+'\n')            
        
        list_of_commonFields = list(set(resultats_fields).intersection(reference_fields))    
     
        resultats = resultats[list_of_commonFields]
        reference = reference[list_of_commonFields]

    print('\n')
    logfile.write('Now check the number of lines of both set of data\n')
    logfile.flush()

    #check the number of lines of both set of data
    shape_resultats = resultats.shape
    shape_reference = reference.shape
        
    if shape_resultats[0] > shape_reference[0]:
        index_size = reference.index.size
        print("Shape of results %d x %d " % shape_resultats)
        print("Shape of results %d x %d " % shape_resultats)
        print("Shape of reference %d x %d " % shape_reference)
        logfile.write('Time series of new results longer than reference\n')
        logfile.write("Shape of results %d x %d \n" % shape_resultats)
        logfile.write("Shape of reference %d x %d \n" % shape_reference)
        logfile.flush()
    elif shape_resultats[0] < shape_reference[0]:
        index_size = resultats.index.size
        print('Time series of reference longer than new results')  
        print("Shape of results %d x %d " % shape_resultats)
        print("Shape of reference %d x %d " % shape_reference)    
        logfile.write('Time series of reference longer than new results\n')  
        logfile.write("Shape of results %d x %d \n" % shape_resultats)
        logfile.write("Shape of reference %d x %d \n" % shape_reference)
        logfile.flush()
    else:
        index_size = reference.index.size
        print('Same length of time series for new results and reference')
        print("Shape of data %d x %d " % shape_resultats)    
        logfile.write('Same length of time series for new results and reference\n')
        logfile.write("Shape of data %d x %d \n" % shape_resultats)
        logfile.flush()

    print('\n')
    logfile.write('Replace nan by empty string and adjust shape\n')
    logfile.flush()

    """ -----------------
     replace nan by empty string and adjust shape
    ----------------------"""
    resultats_raw = resultats
    reference_raw = reference

    resultats = resultats.fillna('')
    reference = reference.fillna('')


    resultats = resultats.iloc[0:index_size,:]
    reference = reference.iloc[0:index_size,:]

        
    """ -----------------
     identify the columns differing between reference and new results
    ----------------------"""
    bool_compareMatrix = (resultats != reference)         #boolean matrix of difference for the whole data set
    bool_compareColumn = bool_compareMatrix.any()        #boolean matrix of difference for all column name
    bool_difColumn = bool_compareColumn[bool_compareColumn]  #keep only the column with differences
    list_difColumn = bool_difColumn.index.tolist()			#list of the columns where there is at least one difference


    """ -----------------
     compute difference metrics and plot, per column
    ----------------------"""
    reference_is_nan = 0

    #nPlo = len(list_difColumn)
    iPlo = 0
    plt.ioff() #Turn interactive plotting off


    print(dash)
    print('DIFFERENCES') 
    print(dash)

    logfile.write('\n')
    logfile.write(dash+'\n')
    logfile.write('DIFFERENCES\n') 
    logfile.write(dash+'\n')
    logfile.flush()

    list_end_with_NaN =[]
    list_diff_NaN = []
    
    max_diff_mean = 0.0
    max_diff_std  = 0.0
    tstart = time.time()
    #loop over all column with differences
    for column in list_difColumn:
        list_index_elementChange_NaN=[]   #list with only the change related to NaN
        list_index_elementChange_noNaN=[] #list with only the change related to real values
        index_EndNaN = [] #list with only the NaN located at the end of the column
        
        #keep only the elements True of the diff matrix
        series_resultats = resultats[column]
        series_reference = reference[column]   
           
        #check if nan are located at the end of the result file => if so, disregarded them
        last=0
        L_res=len(series_resultats)
        index_res = series_resultats.index.tolist()
        #logfile.write('L_res = '+str(L_res)+'\n')
        #logfile.flush()
        for k in range(0,L_res-2):  #loop from end
            if series_resultats.index[L_res-k-1] == '' and series_resultats.index[L_res-k-2] == '':
                index_EndNaN.append(index_res[L_res-k-1])
                last=k
            if series_resultats.index[L_res-k-1] == '' and series_resultats.index[L_res-k-2] != '': #make sure only the last chunk on NaN is considered
                break
        if last:
            list_end_with_NaN.append(column )
            
        index_EndNaN.append(index_res[L_res-last-2])
        series_resultat_noEndNaN = series_resultats.drop(labels=index_EndNaN)   
            
        #prepare the list of all differences, except the NaN at the end    
        bool_series = bool_compareMatrix[column]
        bool_series_noEndNaN = bool_series.drop(labels=index_EndNaN)   
        list_index_elementChange = bool_series_noEndNaN[bool_series].index.tolist()    
              
        #construct the list of differences due to NaN (replaced by empty strings)
        for k in list_index_elementChange:
            if series_reference[k] == ''  or series_resultats[k] == '':
                list_index_elementChange_NaN.append(k)   #list of NaN differences              

        #construct the list of differences due to values
        for k in list_index_elementChange:
            if series_reference[k] != ''  and series_resultats[k] != '':
                list_index_elementChange_noNaN.append(k) #list of real number differences  

        # If there is no Nan diff, process the numerical differences
#mod AR ensure consistence with line 322
#        if not list_index_elementChange_NaN:
        diff_mean = 0.0
        diff_std = 0.0
        if list_index_elementChange_noNaN:
            # remove non numeric elements
            series_reference_val = series_reference.replace('', np.nan).dropna()
            series_resultats_val = series_resultats.replace('', np.nan).dropna()
            rm = series_reference_val.abs().mean()
            # if the value is to small, take absolute difference
            if rm < 1e-6:
                rm = 1.0
            diff_mean = 100*abs(series_resultats_val.mean()- series_reference_val.mean())/rm
            max_diff_mean = max(max_diff_mean, diff_mean)
            rs = series_reference_val.std()
            if rs < 1e-6:
                rs = 1.0
            diff_std = 100*abs(series_resultats_val.std() - series_reference_val.std())/rs
            max_diff_std = max(max_diff_std, diff_std)
                                     
        #process the numerical differences
        if column==list_difColumn[0]:
            logfile.write('{0:40s}{1:>12s}{2:>15s}{3:>14s}{4:>15s}{5:>14s}{6:>12s}\n'.format('Heading - Column','maxerror_abs','time','maxerror_rel','time','mean diff','std diff'))
            print(        '{0:40s}{1:>12s}{2:>15s}{3:>14s}{4:>15s}{5:>14s}{6:>12s}'.format('Heading - Column','maxerror_abs','time','maxerror_rel','time','mean diff','std diff'))
        if list_index_elementChange_noNaN:
            #absolute difference
            diff_abs = abs(series_resultats[list_index_elementChange_noNaN] - series_reference[list_index_elementChange_noNaN])
            max_error_abs = diff_abs.max()
            idxmax_error_abs = diff_abs.values.argmax()    
            timOfMaxErr_abs = list_index_elementChange_noNaN[idxmax_error_abs]

            #relative difference
            ref_abs = 0.005 * (abs(series_resultats[list_index_elementChange_noNaN]) + abs(series_reference[list_index_elementChange_noNaN]))
            #if the value is to small, take absolute difference
            ref_abs[ref_abs<1e-6]=1.0
            diff_rel = diff_abs / ref_abs
            max_error_rel = diff_rel.max()
            idxmax_error_rel = diff_rel.values.argmax() 
            timOfMaxErr_rel = list_index_elementChange_noNaN[idxmax_error_rel]

            logfile.write('{0:40s}{1:14.2E}{2:>15s}{3:14.2E}{4:>15s}{5:12.2E}{6:12.2E}\n'.format(column,max_error_abs,str(timOfMaxErr_abs),max_error_rel,str(timOfMaxErr_rel),diff_mean,diff_std))
            print('{0:40s}{1:12.2E}{2:>15s}{3:14.2E}{4:>15s}{5:14.2E}{6:12.2E}'.format(column,max_error_abs,str(timOfMaxErr_abs),max_error_rel,str(timOfMaxErr_rel),diff_mean,diff_std))
        else:
            logfile.write('{0:40s}{1:20s}\n'.format(column,"Nan differences"))
            print('{0:40s}{1:20s}'.format(column,"Nan differences"))
           
        #process the NaN differences 
        if list_index_elementChange_NaN:
            nan_diff_error = True
            list_diff_NaN.append(column )  
            timOfMaxErr_abs_nan=list_index_elementChange_NaN[0]  #mark the first nan of the column
            if series_reference[timOfMaxErr_abs_nan] == '':  # boolean for the bokeh plot
                reference_is_nan = 1
            else:
                reference_is_nan = 0     
                    
        """ -----------------
        Save plots to png file (one per column)
        ----------------------"""      
        
        #plot if there is any differences
# save figure
    
#        logfile.write('Plot differences '+str(iPlo)+'\n')
#        logfile.flush()
        if iPlo < 500:
            if list_index_elementChange_NaN or diff_mean>0.5 or diff_std>0.5:
                fig1,ax = plt.subplots(figsize=(10,6))
                plt.gcf().subplots_adjust(hspace=0.8)
                reference_raw[column].plot(ax=ax,label='reference',color='k')
                resultats_raw[column].plot(ax=ax,label='new')
                if diff_mean>0.5 or diff_std>0.5:
                    ax.axvline(x=timOfMaxErr_abs,color='r')
                if list_index_elementChange_NaN:
                    ax.axvline(x=timOfMaxErr_abs_nan,color='g')       
                ax.set_xlabel('')
        
                ax.set_title(column); 
                ax.legend() 
                column_no_space = column.replace(" ", "")
                plt.savefig(os.path.join(app_home, log_dir, column_no_space+'.png'), dpi = 200)
                plt.close(fig1) 
                #logfile.write('...OK '+str(column)+'\n')
                logfile.flush()
        
                iPlo+=1
                #plt.clf()
        
        """ -----------------
        Create bokeh plot (interactive html figure) 
        ----------------------"""
        bokeh_plot = False
        if bokeh_plot:      
            if list_index_elementChange_NaN or list_index_elementChange_noNaN:
                output_file(str(column)+'.html')
                TOOLS = "pan,wheel_zoom,box_zoom,reset"
                p = figure(title=str(column),tools=TOOLS,plot_width=1200, plot_height=600,x_axis_type='datetime')
                p.line(reference_raw[column].index,reference_raw[column],legend="reference",line_color="black",line_width=1)
                p.line(resultats_raw[column].index,resultats_raw[column],legend="resultats",line_color="blue",line_width=1)
                if list_index_elementChange_noNaN:
                    p.square(timOfMaxErr_abs,resultats_raw[column][timOfMaxErr_abs],size=15, line_color="red", fill_color="red",legend='max error')
                if list_index_elementChange_NaN:
                    if reference_is_nan == 1:
                        p.square(timOfMaxErr_abs_nan,resultats_raw[column][timOfMaxErr_abs_nan],size=15, line_color="green", fill_color="green",legend='NaN')  
                    else:
                        p.square(timOfMaxErr_abs_nan,reference_raw[column][timOfMaxErr_abs_nan],size=15, line_color="green", fill_color="green",legend='NaN')  
                p.legend.location = "top_left"
                show(p)
                save(p) 

    print()
    print ("Elapsed CPU Time : ",time.time() - tstart)

    infos["number_of_plot"] = iPlo
    infos["different_fields"] = len(list_of_differentFields)>0
    infos["number_of_common_fields"] = len(list_of_commonFields)
    infos["different_length"] = (shape_resultats[0] != shape_reference[0])
    infos["mean_diff_error"] = max_diff_mean>0.5
    infos["std_diff_error"] = max_diff_std>0.5
    infos["nan_diff_error"] = nan_diff_error
    
    error_message = ''
    if len(list_of_differentFields)>0 and len(list_of_commonFields)<=0:
        error_message += "Different fields. "
    if shape_resultats[0] != shape_reference[0]:
        error_message += "Different length. "
    if nan_diff_error:
        error_message += "Nan differences. "
    if max_diff_mean>0.5:
        error_message += "Mean difference > 0.5 %. "
    if max_diff_std>0.5:
        error_message += "Standard deviation difference > 0.5 %. "
    
    infos["error_message"] = error_message
    
    logfile.write(dash+'\n')
    for key, value in infos.items():
        logfile.write('{0:25s}{1:15s}\n'.format(key,str(value)))

    logfile.close()
    
#    print(infos)

    return infos

def convert_df(df,removeDataTime):
    df.columns = df.columns.str.strip()
    df = df.loc[:, df.columns.str.len()>0]
    df = df.loc[:, ~df.columns.str.contains('^Unnamed')]
    if removeDataTime:
        df = df.reset_index(drop=True)
        df = df.loc[:, ~df.columns.str.contains('Data.Time')]
    df = df.astype(float)
    return df


if __name__ == "__main__":
    name_test = "test_rh"
    #name_folder = name_test
    name_folder="Test_rolling_horizon"
    chemin = 'D:\\Users\\PP265749\\git\\Persee\\Test_Persee\\'+name_folder + "\\TNR\\"
    fich1 = chemin +  name_test + '_Sortie-reference.csv'
    fich2 = chemin +  name_test + '_Sortie.csv'
#    chemin = ''
#    fich1 = chemin + 'H2Gui_Sortie.csv'
#    fich2 = chemin + 'H2Gui_Sortie-Reference.csv'
#    fich1 = r'F:\TNR_Pegase\TNR_Deps\trunk\Exemples\Test_MacroGridLabDH\TNR\Output_Json_8.csv'
#    fich2 = r'F:\TNR_Pegase\TNR_Deps\trunk\Exemples\Test_MacroGridLabDH\TNR\References_TimeDepend\OutputSst_PegaseComparison_Json_8.csv'
    # chemin = 'F:\\TNR_Pegase\\TNR_Deps_Persee\\trunk\\Exemples\\Test_Persee\\Test_Etienne2\\TNR\\'
    # fich1 = chemin + 'HipocampeSortie.csv'
    # fich2 = chemin + 'HipocampeSortie-Reference.csv'
    #df = read_csv2(fich2)
    infos = compare_results('.',fich1,fich2, [1,2], '.')


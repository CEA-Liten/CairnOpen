# -*- coding: utf-8 -*-
"""
Created on Wed Nov 25 14:27:40 2020

@author: yd799345
"""
import os
import sys
import datetime
import JunitXmlParser
import urllib.parse
import traceback

import pandas


###FUNCTIONS###

def read_update_summary(df, file_name):
    """

    """
    df['update'] = False
    file = open(file_name, 'r')
    lines = file.readlines()
    file.close()
    print("into update summary")
    for line in lines:
        print(line)
        lineListe = line.split(';')
        test = lineListe[2].strip().replace('\\', '.')
        val = lineListe[0].strip()
        df.loc[df['classname'] == test, 'update'] = val
    return df

def general_info(lines, df):
    """
    build head info
    """
    lines += '<table>'
    lines += '<tr>'
    lines += '<td align="right">'
    if df['failed'].sum() == 0:
        lines += '<img src="' + jenkins_url + '/static/e59dfe28/images/32x32/blue.gif" />'
        lines += '</td>'
        lines += '<td valign="center"><b style="font-size: 200%;">BUILD SUCCESS</b></td>'
    else:
        lines += '<img src="' + jenkins_url + '/static/e59dfe28/images/32x32/red.gif" />'
        lines += '</td>'
        lines += '<td valign="center"><b style="font-size: 200%;">BUILD FAILURE</b></td>'
    lines += '</tr>'
    lines += '<tr>'
    lines += '<td>Build URL:</td>'
    lines += '<td>' + build_url + '</td>'
    lines += '</tr>'
    lines += '<tr>'
    lines += '<td>Project:</td>'
    lines += '<td>' + JOB + '</td>'
    lines += '</tr>'
    lines += '<tr>'
    lines += '<td>Date of build:</td>'
    now = datetime.datetime.now()
    lines += '<td>' + now.strftime("%a, %d %b %Y %H:%M:%S") + '</td>'
    lines += '</tr>'
    lines += '<tr>'
    lines += '<td>Build duration:</td>'
    total_duration = int(df['time'].sum())
    h = total_duration // 3600
    min = (total_duration - h * 3600) // 60
    lines += '<td>' + '{0:d} hr {1:d} min'.format(h, min) + '</td>'
    lines += '</tr>'
    lines += '</table>'
    lines += '<br />'
    return lines

def df2html(df):
    """
    Generate the html report with df content
    ----------
    Parameters:
    df: the data frame

    """
    lines = '<!DOCTYPE html>'
    lines += '<html>'
    lines += '<head>'
    lines += '<title></title>'
    lines += '</head>'

    lines += general_info(lines, df)

    lines += '<body>'
    lines += '<table cellpadding="4" style="border: 1px solid #000000; border-collapse: collapse;" border="1">'

    lines += '<tr>'
    lines += '<th>Package</th>'
    lines += '<th>Test name</th>'
    lines += '<th>Name</th>'
    lines += '<th>Duration (sec)</th>'
    lines += '<th>Message</th>'
    lines += '<th>log</th>'
    lines += '<th>Curve</th>'
    lines += '<th>Status</th>'
    lines += '<th>Reference update</th>'
    lines += '</tr>'

    lines += '<tbody>'
    for i, row in df.iterrows():
        lines += '<tr>'
        lines += '<td'
        if row['failed_diff'] > 0:
            lines += ' style="background-color:#FF0000"'
        if row['failed_diff'] < 0 and row['failed'] == 0:
            lines += ' style="background-color:#00FF00"'
        lines += '>' + row['classname'] + '</td>' #package column
        lines += '<td>' + row['testname'] + '</td>' #testname column
        lines += '<td>' + row['name'] + '</td>' #Name column
        pathlist = row['classname'].split('.')
        lines += '<td>' + str(round(row['time'], 2)) + '</td>' #Duration column
        lines += '<td>' + row['message'] + '</td>'  #Message column
        #wip modifier link_log à partir de log_file

        global jobRootdir
        pathtest = '\\'.join(pathlist)

        #check for double dirname :
        new_path = pathtest.replace("test_StdAlone", "")

        if not os.path.isdir(f"{jobRootdir}\\{new_path}") :
            splitted_path = new_path.split("\\")
            splitted_path.pop(-1)
            new_path = "\\".join(splitted_path)
        print(f"new_path : {new_path}")
        search_path = os.path.join(jobRootdir, new_path)
        print(f"search path : {search_path}")
        
        jobTestPath =  os.path.join(row['classname'], row['testname'], row['name'])
        print("job test path: "+jobTestPath)
        # on construit l'url pointant vers le fichier de log dans le workspace de Jenkins
        link_log = f"<a href={file_url}\\{jobTestPath}> log </a>"   
        lines += '<td>' + link_log + '</td>'    #log column

        lines += '<td>'
        path_curve = os.path.join(jobRootdir, '\\'.join(pathlist), 'TNR', row['name'])
        if os.path.exists(path_curve) and row['failed'] > 0:
            # on recupere la liste des fichiers *.png dans le dossier du test courant
            curveList = [fn for fn in os.listdir(path_curve) if fn.endswith('.png')]
            for curveName in curveList:
                # on construit l'url pointant vers chaque fichier png dans le workspace de Jenkins
                urlCurveName = urllib.parse.quote_plus(curveName)
                curveLink = file_url + '/' + JOBreldir + '/' + '/'.join(pathlist) + '/TNR/' + row['name'] + '/' + urlCurveName
                lines += '<a href=' + curveLink + '>' + curveName[:-4] + '</a>' + '<br>'
        lines += '</td>'  #curve column

        if row['failed'] == 0 and row['skipped'] == 0:
            lines += '<td>OK</td>' #status column
        elif row['passed'] == 0 and row['skipped'] == 0:
            lines += '<td>error</td>' #status column
        elif row['passed'] == 0 and row['failed'] == 0:
            lines += '<td>skipped</td>' #status column
        else:
            lines += '<td>?</td>' #status column
        if 'update' in df.columns:
            lines += '<td>' + str(row['update']) + '</td>' #Reference update column
        else:
            lines += '<td></td>'
        lines += '</tr>'
    lines += '</tbody>'
    lines += '</table>'
    lines += '</body></html>'
    return lines

####SCRIPT###
JOB = ''
make_url = 'Yes' #tobedeleted

if len(sys.argv) >= 2:
    JOB = sys.argv[1]
if len(sys.argv) >= 3:
    make_url = sys.argv[2]

##getting var env
workspace = os.getenv('WORKSPACE', None)
jenkins_url = os.getenv('JENKINS_URL', 'https://jenkins-lset.appli.gre-rke2.intra.cea.fr/')
build_url = os.getenv('BUILD_URL', '')
build_number = os.getenv('BUILD_NUMBER', '')
branch_name = os.getenv('BRANCH_NAME', '')
#TODO using os.getenv
project_name = "Persee"
folder_name = "cairn_pipeline"


file_url = f"{jenkins_url}/job/{project_name}/job/{folder_name}/{branch_name}/{build_number}/testReport/"

print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
print('JOB: '+ JOB)

xml_filename = os.path.join(workspace, JOB)
xml_filename_old = xml_filename + '_old'

summary_file = xml_filename + '_update.txt'
jobRootdir = os.path.join(xml_filename, os.pardir)
JOBreldir = os.path.join(JOB, os.pardir)

print('workspace: ' + workspace)
print('jenkins_url: ' + jenkins_url)

print("jobRootdir : " + jobRootdir)
print("JOB : " + JOB)
print("JOBreldir : "+JOBreldir)
print("summary_file : " + summary_file)
print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

print ("file_url built : "+file_url)
print ("file_url ideal : "+build_url+'../ws/')
print('xml_filename : ' + xml_filename)

if os.path.exists(xml_filename + '.xml'):
    df = JunitXmlParser.parse_xml(xml_filename)
else:
    print(xml_filename + '.xml does not exist')
    df=pandas.DataFrame({'failed':[]})
if os.path.exists(xml_filename_old + '.xml'):
    df_old = JunitXmlParser.parse_xml(xml_filename_old)
else:
    print(xml_filename_old + '.xml does not exist')
    df_old=pandas.DataFrame({'failed':[]})

df['failed_diff'] = df['failed'] - df_old['failed']


if os.path.exists(summary_file):
    df = read_update_summary(df, summary_file)
else:
    print('Warning: file ' + summary_file + ' not found')


html = df2html(df)
html_file_name = xml_filename + '.html'
print(f"html file : {html_file_name}")
outputFile = open(html_file_name, 'w')
outputFile.write(html)
outputFile.close()
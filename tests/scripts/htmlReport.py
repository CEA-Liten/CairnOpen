# -*- coding: utf-8 -*-
"""
Created on Wed Nov 25 14:27:40 2020

@author: GVIGE000
"""
import os
import sys
import datetime
import JunitXmlParser
import urllib.parse
import traceback

import pandas

def getlogfile(dir):
    logfile = None
    for file in os.listdir(dir):
        if "logfile.txt" in file:
            logfile = f"{dir}\\{file}"
            return logfile
        elif os.path.isdir(f"{dir}\\{file}"):
            logfile = getlogfile(f"{dir}\\{file}")
            if logfile :
                return logfile
    return logfile
    


def read_update_summary(df, file_name):
    df['update'] = False
    file = open(file_name, 'r')
    lines = file.readlines()
    file.close()
    for line in lines:
        lineListe = line.split(';')
        test = lineListe[2].strip().replace('\\', '.')
        val = lineListe[0].strip()
        df.loc[df['classname'] == test, 'update'] = val
    return df


def general_info(lines, df):
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


def df2html(df, alt_path=None):
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
        lines += '>' + row['classname'] + '</td>'
        lines += '<td>' + row['name'] + '</td>'
        pathlist = row['classname'].split('.')
        lines += '<td>' + str(round(row['time'], 2)) + '</td>'
        lines += '<td>' + row['message'] + '</td>'
        #wip modifier link_log à partir de log_file_2

        global jobRootdir
        if alt_path:
            jobRootdir = alt_path

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
        try:
            log_file_2 = getlogfile(search_path)
        except Exception as e:
            print(traceback.format_exc())
            log_file_2 = None
        #print(f"log_file_2 : {log_file_2}")
        log_file = os.path.join(jobRootdir, '\\'.join(pathlist), 'TNR', row['name'], 'logfile.txt')
        if log_file_2 and os.path.isfile(log_file_2):
            print("Looking for logfile in htmlreport.py : ", log_file_2)
            logfile_for_path = "\\".join(log_file_2.split("/"))
            logfile_for_path = "\\".join(log_file_2.split("\\")[4:])
            # on construit l'url pointant vers le fichier de log dans le workspace de Jenkins
            #link_log = '<a href=' + file_url + '/' + JOBreldir + '/' + '/'.join(pathlist) + '/TNR/' + row['name'] + '/logfile.txt>' + 'log </a>'
            link_log_2 = f"<a href={file_url}\\{JOBreldir}\\{logfile_for_path}> log </a>"
            lines += '<td>' + link_log_2 + '</td>'
        else:
            print("Skip missing logfile in htmlreport.py : ", log_file_2)
            lines += '<td> </td>'

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
        lines += '</td>'

        if row['failed'] == 0 and row['skipped'] == 0:
            lines += '<td>OK</td>'
        elif row['passed'] == 0 and row['skipped'] == 0:
            lines += '<td>error</td>'
        elif row['passed'] == 0 and row['failed'] == 0:
            lines += '<td>skipped</td>'
        else:
            lines += '<td>?</td>'
        if 'update' in df.columns:
            lines += '<td>' + str(row['update']) + '</td>'
        else:
            lines += '<td></td>'
        lines += '</tr>'
    lines += '</tbody>'
    lines += '</table>'
    lines += '</body></html>'
    return lines


JOB = ''
make_url = 'Yes'

if len(sys.argv) >= 2:
    JOB = sys.argv[1]
if len(sys.argv) >= 3:
    make_url = sys.argv[2]

workspace = os.getenv('WORKSPACE', None)
jenkins_url = os.getenv('JENKINS_URL', None)
build_url = os.getenv('BUILD_URL', '')
build_number = os.getenv('BUILD_NUMBER', '')
branch_name = os.getenv('BRANCH_NAME', '')

# TEST*******************************************
# workspace = r'F:\TNR_Pegase\TNR_Sources\trunk'
# JOB = 'Persee-TNR.xml'
# TEST*******************************************


print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
print(JOB)
if JOB.endswith('.xml'):
    JOB = JOB[:-4]

if workspace:
    print('workspace: ' + workspace)
    xml_filename = workspace + '\\' + JOB
else:
    print('variable WORKSPACE pas trouvee')
    xml_filename = JOB

if jenkins_url:
    print('jenkins_url: ' + jenkins_url)
else:
    print('variable jenkins_url pas trouvee')
    jenkins_url = 'http://grewp773app.intra.cea.fr:8080/'
if build_url:
    print('build_url: ' + build_url)
else:
    print('variable build_url pas trouvee')


summary_file = xml_filename + '_update.txt'

jobRootdir = os.path.join(xml_filename, os.pardir)
JOBreldir = os.path.join(JOB, os.pardir)

print("jobRootdir : " + jobRootdir)
print("JOB : " + JOB)
print("JOBreldir : "+JOBreldir)

print("summary_file : " + summary_file)
print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

alt_path = None
if make_url=='Yes':    
    if 'Persee_Project' in workspace:
        file_url = f"{jenkins_url}//job/Persee_AllProject/job/persee_pipeline/{build_number}/execution/node/5/ws/"
        xml_filename = f"{workspace}\\tests\\Cairn-TNR"
        print(' in elif xml_filename : ' + xml_filename)
    elif 'multi' in workspace:
        import platform
        if 'Linux' in platform.system():
            file_url = f"{jenkins_url}/job/Pegase_Project/job/multi_branch_pegase/job/{branch_name}/{build_number}/execution/node/15/ws/"
            xml_filename = JOB
            summary_file = xml_filename + '_update.txt'
            jobRootdir = xml_filename[:-10]
            JOBreldir = os.path.join(JOB, os.pardir)
        else:
            file_url = f"{jenkins_url}/job/Pegase_Project/job/multi_branch_pegase/job/{branch_name}/{build_number}/execution/node/17/ws/"
    elif 'Persee_' in xml_filename:  ##=='TNR_Persee':
        file_url = jenkins_url + 'job/Persee_Project/job/TNR_Persee_Integration/job/Persee_TNR/ws/'
        file_url = build_url+'../ws/'
    elif 'Pegase_pipeline' in workspace:
        file_url = f"{jenkins_url}/job/Pegase_Project/job/Pegase_pipeline/{build_number}/execution/node/3/ws/"
    else:
        file_url = ''
        xml_filename = ''

    
else:
    file_url = build_url+'../ws/'

print ("file_url built : "+file_url)
print ("file_url ideal : "+build_url+'../ws/')
print('xml_filename : ' + xml_filename)

xml_filename_old = xml_filename + '_old'


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

#summary_file = os.path.join(workspace, summary_file)

if os.path.exists(summary_file):
    df = read_update_summary(df, summary_file)
else:
    print('Warning: file ' + summary_file + ' not found')
html = df2html(df, alt_path)
html_file_name = xml_filename + '.html'
print(f"html file : {html_file_name}")
outputFile = open(html_file_name, 'w')
outputFile.write(html)
outputFile.close()


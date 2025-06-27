from distutils.command import build

import os
import subprocess
import psutil
import shutil
import sys
import re
import pandas as pd
from os import path, remove
from subprocess import Popen
from compare_results import compare_results
from compare_results import compare_plan
from compare_results import sort_files

from checkPLAN import checkPLAN
from compare_lp import test_comparaison
from compareJson import compareJson

from sys import platform


class CairnNRT:
    def __init__(self, app_home="", max_time=3600):
        self.__buildDone = False
        self.__app_home = app_home
        self.__script_home = os.path.dirname(os.path.realpath(__file__))
        self.__max_time = max_time
        self.__file_plan = ''
        self.__file_plan_ref = ''
        self.__file_hist = ''
        self.__file_hist_ref = ''
        self.__directory_res = ''
        self.__file_res = ''
        self.__file_ref = ''
        self.__test_name = ''

    def setPlanHistFiles(self):
        for file in os.listdir(self.__app_home):
            if file.endswith("PLAN.csv"):
                self.__file_plan = os.path.join(self.__app_home,
                                                file)  # this file comes from svn extract, it will be rewritten during the run step
                self.__file_plan_ref = self.__file_plan.replace("PLAN.csv", "PLAN-Reference.csv")
            elif file.endswith("HIST.csv"):
                self.__file_hist = os.path.join(self.__app_home,
                                                file)  # this file comes from svn extract, it will be rewritten during the run step
                self.__file_hist_ref = self.__file_hist.replace("HIST.csv", "HIST-Reference.csv")

    def copyPlanHistFiles(self):
        if os.path.exists(self.__file_plan_ref) and not os.path.exists(self.__file_plan_ref):
            os.rename(self.__file_plan, self.__file_plan_ref)
        if os.path.exists(self.__file_plan_ref) and not os.path.exists(self.__file_hist_ref):
            os.rename(self.__file_hist, self.__file_hist_ref)

    def run(self, tnr_dir="TNR", tnr_xml="TNR.xml", maxtime=1200):
        if os.getenv('BUILD_STEP') is None or 'RUN' in os.getenv('BUILD_STEP'):
            self.setPlanHistFiles()
            self.copyPlanHistFiles()

            xml_file = os.path.join(self.__app_home, tnr_dir, tnr_xml)
            assert os.path.exists(xml_file), "XML file not found " + xml_file
            print("Running with ", xml_file)
           
            if platform == "win32":
                run_script = os.path.join(self.__script_home, "runTNR.bat")
            elif platform == "linux":
                run_script = os.path.join(self.__script_home, "runTNR.sh")
            assert os.path.exists(run_script), "run script not found " + run_script

            run_args = [run_script, tnr_dir, tnr_xml, self.__app_home]
            p = Popen(run_args, cwd=self.__app_home)
            pid = p.pid
            try:
                # ensure long time optimization can reach the end - 1200 s clearly too short !
                stdout, stderr = p.communicate(timeout=maxtime)
            except subprocess.TimeoutExpired:
                for proc in psutil.process_iter():
                    if proc.pid == pid:
                        proc.terminate()
            print('Returning code=',p.returncode)
            assert p.returncode >= 0, " *** ERROR detected while running test *** "

    #tnr_name is not used ! Can be used as ScenarioName
    def runPersee(self, tnr_dir="TNR", tnr_xml="Study.json", tnr_series="Study_dataseries.csv", tnr_name="Study",
                  maxtime=1200, timeStepFile="", tsFileList=[]): 
        if os.getenv('BUILD_STEP') is None or 'RUN' in os.getenv('BUILD_STEP'):
            self.setPlanHistFiles()
            self.copyPlanHistFiles()

            #script
            run_script = os.path.join(self.__script_home, "runjson.bat")
            assert os.path.exists(run_script), "run script not found " + run_script

            #study file
            study_file = os.path.join(self.__app_home, tnr_dir, tnr_xml)
            assert os.path.exists(study_file), "Study file not found " + study_file
            print("Running with ", study_file)

            run_args = [run_script, study_file]

            #dataseries files : use either tnr_series or tsFileList as time series file/files
            csv_file_list = []
            if tsFileList != []:
                print("timeseries list ...")
                for tsfile in tsFileList:
                    print('tsfile :', tsfile)
                    csv_file_list.append(os.path.join(self.__app_home, tnr_dir, tsfile))
                    assert os.path.exists(tsfile), "TimeSeries file not found " + tsfile
                run_args.extend(csv_file_list)
            else:
                csv_file = os.path.join(self.__app_home, tnr_dir, tnr_series)
                assert os.path.exists(csv_file), "TimeSeries file not found " + csv_file
                run_args.append(csv_file)
            
            #TimeStepFile
            if timeStepFile != "":
                timeStep_file = os.path.join(self.__app_home, tnr_dir, timeStepFile)
                assert os.path.exists(csv_file), "TimeStepFile file not found " + timeStep_file
                run_args.append("-TimeStepFile")
                run_args.append(timeStep_file)

            print(run_args)

            p = Popen(run_args, cwd=self.__app_home)
            pid = p.pid

            print("CairnCmd process id: %d"%pid, flush=True)
            try:
                # ensure long time optimization can reach the end - 1200 s clearly too short !
                stdout, stderr = p.communicate(timeout=maxtime)
            except subprocess.TimeoutExpired:
                for proc in psutil.process_iter():
                    if proc.pid == pid:
                        proc.terminate()
                return -2
            print('Returning code=', p.returncode)
            assert p.returncode >= 0, " *** ERROR detected while running test *** "
            return p.returncode

    def output(self, msg, logfile, summary_update_file, updated):
        print(msg)
        logfile.write(msg)
        if summary_update_file:
            file_res = self.__file_res.replace(self.__directory_res, '')
            file_ref = self.__file_ref.replace(self.__directory_res, '')
            file_plan = self.__file_plan.replace(self.__directory_res, '')
            file_hist = self.__file_hist.replace(self.__directory_res, '')
            summary_update_file.write(
                "{0:<10s};{1:<40s};{2:<50s};{3:<80s};{4:<80s};{5:<80s};{6:<80s}".format(str(updated),
                                                                                        self.__directory_res,
                                                                                        self.__test_name, file_res,
                                                                                        file_ref, file_plan, file_hist))
            summary_update_file.write('\n')
            summary_update_file.flush();

    def checkPlanHist(self, planHist, planHist_file, planHist_ref_file, logfile, summary_update_file):
        updated = False
        threshold = 0.1
        if os.path.exists(planHist_file) and os.path.exists(planHist_ref_file):
            err = checkPLAN(planHist_ref_file, planHist_file, logfile, threshold)
            if err < threshold:
                updated = True
                self.output(planHist + ' file difference < ' + str(threshold) + '%\n', logfile, summary_update_file, updated)
            else:
                self.output(planHist + ' file difference > ' + str(threshold) + '%\n', logfile, summary_update_file, updated)
        else:
            self.output(planHist + ' file not found\n', logfile, summary_update_file, updated)
        return updated

    def checkGlobal(self, typeFile='PLAN', fileNew='PLAN.csv', fileRef='refPLAN.csv'):
        tnr_dir = '.'
        fileNew_abs = os.path.join(self.__app_home, tnr_dir, fileNew)
        fileRef_abs = os.path.join(self.__app_home, tnr_dir, fileRef)

        updated = True
        logfile = open(os.path.join(self.__app_home, tnr_dir, 'diff_' + typeFile + '_file.log'), 'a')
        summary_update_file = open(os.path.join(self.__app_home, tnr_dir, "summary_file.txt"), 'a')
        
        updated = self.checkPlanHist(typeFile, fileNew_abs, fileRef_abs, logfile, summary_update_file)

        logfile.close()
        summary_update_file.close()

        assert updated, typeFile + " file differ"

        return updated

    def checklp(self, typeFile='lp', fileNew='.lp', fileRef='_ref.lp'):
        tnr_dir = '.'
        fileNew_abs = os.path.join(self.__app_home, tnr_dir, fileNew)
        fileRef_abs = os.path.join(self.__app_home, tnr_dir, fileRef)

        updated = False
        logfile = open(os.path.join(self.__app_home, tnr_dir, 'diff_' + typeFile + '_file.log'), 'a')
        summary_update_file = open(os.path.join(self.__app_home, tnr_dir, "summary_file.txt"), 'a')
        if os.path.exists(fileNew_abs) and os.path.exists(fileRef_abs):
            updated = (test_comparaison(fileNew_abs,fileRef_abs))
            if not(updated):
                self.output(" difference in the lp files", logfile, summary_update_file, updated)
            else:
                self.output(" NO difference in the lp files", logfile, summary_update_file, updated)
        else:
            self.output(typeFile + ' file not found\n', logfile, summary_update_file, updated)
        logfile.close()
        summary_update_file.close()

        assert updated, typeFile + " file differ"

        return updated

    def checkJson(self, typeFile='json', fileNew='new_study.json', fileRef='study.json', skipCompoX=""):
        tnr_dir = '.'
        noDiff = False
        file_json = os.path.join(self.__app_home, tnr_dir, fileNew)
        file_json_ref = os.path.join(self.__app_home, tnr_dir, fileRef)
        logfile = open(os.path.join(self.__app_home, tnr_dir, 'diff_' + typeFile + '_file.log'), 'a')
        summary_update_file = open(os.path.join(self.__app_home, tnr_dir, "summary_file.txt"), 'a')

        if os.path.exists(file_json) and os.path.exists(file_json_ref):
            noDiff = compareJson(file_json_ref, file_json, logfile, skipCompoX)
            if noDiff:
                self.output("NO difference in the json files", logfile, summary_update_file, noDiff)
            else:
                self.output("Difference in the json files", logfile, summary_update_file, noDiff)
        else:
            self.output(typeFile + ' file not found\n', logfile, summary_update_file, noDiff)

        logfile.close()
        summary_update_file.close()
        assert noDiff, typeFile + " file differ"
        return noDiff

    def update(self, infos, log_file):
        found = re.search('(.+?)trunk', self.__file_res)
        print("Update ", self.__file_res)
        icasePegaseRoot = 1
        if found != None:
            self.__directory_res = found.group(1) + 'trunk'
            self.__test_name = self.__app_home.replace(self.__directory_res, '').strip('\\')
        else:
            found = re.search('(.+?)Persee_(.+?)Project', self.__file_res)
            print("Update ", self.__file_res)
            if found != None:
                self.__directory_res = found.group(1) + 'Persee_Project'
                self.__test_name = self.__app_home.replace(self.__directory_res, '').strip('\\')
                icasePegaseRoot = 0
            else:
                print("icasePegaseRoot=",icasePegaseRoot)

        report_file_name = os.getenv('REPORT')
        print("")
        if report_file_name:
            if (icasePegaseRoot==1):
                summary_update_name = os.path.join(self.__directory_res, report_file_name[:-4] + '_update.txt')
            else:
                summary_update_name = os.path.join(self.__app_home.replace(self.__test_name, ''), report_file_name[:-4] + '_update.txt')
            print("Opening REPORT as ",summary_update_name, self.__app_home,self.__app_home.replace(self.__test_name, ''),self.__directory_res,report_file_name )
        else:
            summary_update_name = os.path.join(self.__directory_res, 'summary_update.txt')
        summary_update_file = open(summary_update_name, 'a')
        self.setPlanHistFiles()
        # automatic update of reference results
        # self.output('\n'+'CairnNRT.update : BUILD_STEP = ' + os.getenv('BUILD_STEP')+'\n', log_file,None,None)
        if os.getenv('BUILD_STEP') is None or 'UPDATE' in os.getenv('BUILD_STEP'):
            updated = False
            logfile = open(log_file, 'a')
            dash = '-' * 40
            self.output('\n' + dash + '\nREFERENCE UPDATE\n' + dash + '\n', logfile, None, None)
            if infos["identical_files"] == True:
                self.output('Identical files', logfile, summary_update_file, updated)
            elif infos["different_length"] == True:
                self.output('Different length', logfile, summary_update_file, updated)
            elif infos["nan_diff_error"] == True:
                self.output('Nan differences', logfile, summary_update_file, updated)
            elif infos["mean_diff_error"] == False and infos["std_diff_error"] == False and infos[
                "number_of_common_fields"] >= 3:
                # at least 3 identical columns (one column in addition to Time and Data.time)
                updated = True
                self.output('mean diff and std diff < 0.5 %', logfile, summary_update_file, updated)
            else:
                updated = self.checkPlanHist('PLAN', self.__file_plan, self.__file_plan_ref, logfile,
                                             summary_update_file)
                if not updated:
                    updated = self.checkPlanHist('HIST', self.__file_hist, self.__file_hist_ref, logfile,
                                                 summary_update_file)
            if updated:
                pass
            logfile.close()
            summary_update_file.close()

    def check(self, tnr_dir="TNR", tnr_ref="Sortie-reference.csv", tnr_result="Sortie.csv", skip_rows=[1, 2],
              decimal=','):
        if os.getenv('BUILD_STEP') is None or 'CHECK' in os.getenv('BUILD_STEP'):
            if tnr_dir.startswith("./"):
                tnr_dir = tnr_dir[2:]
            self.__file_ref = os.path.join(self.__app_home, tnr_dir, tnr_ref)
            assert os.path.exists(self.__file_ref), "Reference file not found " + self.__file_ref

            self.__file_res = os.path.join(self.__app_home, tnr_dir, tnr_result)
            assert os.path.exists(self.__file_res), "Result file not found " + self.__file_res

            print("Check ", self.__file_res, os.getenv('BUILD_STEP'))
            log_dir = sys._getframe(1).f_code.co_name
            if not os.path.exists(os.path.join(self.__app_home, tnr_dir, log_dir)):
                os.makedirs(os.path.join(self.__app_home, tnr_dir, log_dir))
            log_file = os.path.join(self.__app_home, tnr_dir, log_dir, "logfile.txt")

            infos = compare_results(os.path.join(self.__app_home, tnr_dir), self.__file_ref, self.__file_res, skip_rows,
                                    decimal, log_file, log_dir)
            self.update(infos, log_file)
            assert infos['error_message'] == '', infos['error_message']
    



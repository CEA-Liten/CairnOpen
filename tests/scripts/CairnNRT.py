from distutils.command import build

import os
import subprocess
import psutil
import shutil
import sys
import re
import pandas as pd
from os import path, remove
from subprocess import Popen, check_output
from compare_results import new_compare_results
from compare_results import compare_plan_rempl

from compare_lp import test_comparaison
from compareJson import compareJson

from sys import platform
import cairn as crn


def updateFile(file_current, file_ref, overwrite, keep_current = True):
    change = 0
    if (not os.path.exists(file_ref)) and os.path.exists(file_current):
        os.rename(file_current,file_ref)
        change = 1
    elif overwrite and os.path.exists(file_current):
        os.remove(file_ref)
        os.rename(file_current,file_ref)
        change = 1
    else:
        print("File" + file_ref + "already exists, don't overwrite")
    if not keep_current:
        if os.path.exists(file_current):
            os.remove(file_current)
    return change

class CairnNRT:
    def __init__(self, name="", sampling="", dataseries = [], app_home="", max_time=3600, timestepFile = "", rolling_horizon=False, pegase=False):
        self.__buildDone = False
        self.__app_home = app_home
        self.__script_home = os.path.dirname(os.path.realpath(__file__))
        self.__max_time = max_time
        self.__test_name = name
        self.__study_file = os.path.join(app_home, name+'.json')
        self.__file_plan = os.path.join(app_home, name+'_results_PLAN.csv')
        self.__file_plan_ref = os.path.join(app_home, name+'_results_PLAN_Ref.csv')
        self.__file_hist = os.path.join(app_home, name+'_results_HIST.csv')
        self.__file_hist_ref = os.path.join(app_home, name+'_results_HIST_Ref.csv')
        self.__directory_res = os.path.join(app_home, name+'_NRT')
        self.__rolling_horizon = rolling_horizon
        self.__file_res = os.path.join(app_home, name+'_results_Results.csv')
        self.__file_ref = os.path.join(app_home, name+'_Results_Ref.csv')
        self.__pegase = pegase
        if pegase:
            self.__file_res = os.path.join(app_home, name+'_pegase_Results.csv')
            self.__file_ref = os.path.join(app_home, name+'_pegase_Results_Ref.csv')
        self.__file_lp = os.path.join(app_home,"checklp", name+"_model.lp")
        self.__file_lp_ref = os.path.join(app_home, name+"_model_Ref.lp")
        self.__logfile = open(os.path.join(app_home, 'report_testing.txt'),'w')
        self.__summary_file = os.path.join(app_home, 'summary_testing.txt')
        self.__sampling_results = os.path.join(app_home, 'sampling_results.csv')
        self.__sampling_results_ref = os.path.join(app_home, 'sampling_results_ref.csv')
        self.__sampling_kpi = os.path.join(app_home, 'kpi_sampling.csv')
        self.__threshold = 0.0001
        if sampling != "":
            self.__sampling = os.path.join(app_home,sampling)
        else:
            self.__sampling = ""
        if dataseries == []:
            self.__dataseries = [os.path.join(app_home, name+'_dataseries.csv')]
        else:
            self.__dataseries = dataseries

    def check_study_file_existence(self):
        assert os.path.isfile(self.__study_file), self.__study_file + " not found"
        for i in self.__dataseries:
            assert os.path.isfile(i), i + " not found"
        if self.__sampling == "" and os.path.isfile(os.path.join(self.__app_home,"sampling.csv")):
            self.__sampling = os.path.join(self.__app_home,"sampling.csv")

    def check_study_fileref_existence(self):
        assert os.path.isfile(self.__study_file), self.__study_file + " not found"
        assert os.path.isfile(self.__file_plan_ref), self.__file_plan_ref + " not found"
        assert os.path.isfile(self.__file_hist_ref), self.__file_hist_ref + " not found"
        assert os.path.isfile(self.__file_ref), self.__file_ref + " not found"
        if self.__sampling != "":
            assert os.path.isfile(self.__sampling), self.__sampling + " not found"
        for i in self.__dataseries:
            assert os.path.isfile(i), i + " not found"


    def runCairn(self):
        inst = crn.CairnAPI(False)
        print("======================= running test ===================")
        print(self.__study_file)
        print(self.__dataseries)
        problem = inst.read_study(self.__study_file)
        for ts in self.__dataseries:
            problem.add_timeseries(ts)
        if problem.simulation_control["NbCycle"]>1:
            self.__file_res = os.path.join(self.__app_home, self.__test_name+'_results_rollinghorizon.csv')
            self.__file_ref = os.path.join(self.__app_home, self.__test_name+'_Results_rh_Ref.csv')
        solution = problem.run()
        assert solution.status == "Optimal"
        return problem, inst
    
    def runPegase(self, tnr_dir="", tnr_xml="TNR.xml", maxtime=1200):
        if os.getenv('BUILD_STEP') is None or 'RUN' in os.getenv('BUILD_STEP'):
            #self.setPlanHistFiles()
            #self.copyPlanHistFiles()

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
            except Exception as e:
                assert "error while running Pegase:", e
            print('Returning code=',p.returncode)
            assert p.returncode >= 0, " *** ERROR detected while running test *** "


    
    def runSampling(self):
        inst = crn.CairnAPI(False)
        if self.__sampling != "":
            df_sens = pd.read_csv(self.__sampling, sep=";", decimal=".", header=[0,1], index_col=0)
        if os.path.exists(self.__sampling_kpi):
            df_kpi = pd.read_csv(self.__sampling_kpi, sep=";")
        else:
            df_kpi = pd.DataFrame()
        problem = inst.read_study(self.__study_file)
        for ts in self.__dataseries:
            problem.add_timeseries(ts)
        tab_results = crn.run_sensitivity(problem,df_sens,indicators=df_kpi)
        inst.close_study()
        return tab_results


    def updateTest(self, overwrite, keep_current = False, checklp=True):
        if self.__pegase==False:
            problem, inst = self.runCairn()
        else:
            self.runPegase(tnr_xml=self.__test_name+".xml")
        updateFile(self.__file_plan,self.__file_plan_ref,overwrite, keep_current=keep_current)
        updateFile(self.__file_hist,self.__file_hist_ref,overwrite, keep_current=keep_current)
        updateFile(self.__file_res,self.__file_ref,overwrite, keep_current=keep_current)
        if checklp:
            futuresize = problem.simulation_control["FutureSize"]
            problem.simulation_control = {"FutureSize":10}
            try:
                problem.run("checklp")
            except:
                problem.simulation_control = {"FutureSize":futuresize}
                problem.run("checklp")
            updateFile(self.__file_lp,self.__file_lp_ref,True, keep_current=keep_current)
            problem.simulation_control = {"FutureSize":futuresize}
        if self.__sampling!="":
            tab_results = self.runSampling()
            tab_results.to_csv(self.__sampling_results_ref, sep=";")
        if self.__pegase==False:
            inst.close_study()

    def generic_testing(self):
        status = {}
        problem,inst = self.runCairn()
        future_size = problem.simulation_control["FutureSize"]
        try:
            problem.simulation_control = {"FutureSize":10}
            solution = problem.run("checklp")
            status["RUNLPFILE"] = solution.status
            
        except:
            problem.simulation_control = {"FutureSize":future_size}
            solution = problem.run("checklp")
            status["RUNLPFILE"] = solution.status
        problem.simulation_control = {"FutureSize":future_size}
        status["LPFILE"] = self.checklp()
        
        status["OPTIM"] = problem.run().status
        status["PLAN"] = self.checkPlanHist("PLAN")
        status["HIST"] = self.checkPlanHist("HIST")
        status["TIMESERIES"] = self.check(0.001)
        
        inst.close_study()
        return(status)
        
    def sampling_test(self):

        tab_res = self.runSampling()
        try:
            tab_ref = pd.read_csv(self.__sampling_results_ref,sep=";", index_col=0)
        except:
            print("tab ref not found")
            tab_ref = 0
            return "No reference"
        tab_res.to_csv(self.__sampling_results,sep=";")
        diff = ((tab_res.round(decimals=3).reindex(sorted(tab_res.columns), axis=1)).compare((tab_ref.round(decimals=3)).reindex(sorted(tab_ref.columns), axis=1)))
        self.output("\n Sampling test \n ================ \n")
        if len(diff)==0:
            self.output("Results are identical\n")
        else:
            self.output("Difference in the sampling\n")
            self.output(diff.to_string())
        if len(diff)>0:
            return "Failed"
        else:
            return "Success"

    def output(self, msg):
        print(msg)
        self.__logfile.write("\n")
        self.__logfile.write(msg)
        

    def checkPlanHist(self,planOrHist,threshold = 0.1):
        status = True
        if planOrHist == "PLAN" or planOrHist == "":
            planHist_file = self.__file_plan
            planHist_ref_file = self.__file_plan_ref
        else :
            planHist_file = self.__file_hist
            planHist_ref_file = self.__file_hist_ref
        err = compare_plan_rempl(planOrHist, planHist_ref_file, planHist_file, self.__logfile, threshold)
        if err < threshold:
            status = True
            self.output(planOrHist + ' file difference < ' + str(threshold) + ': %\n')
        else:
            status = False
            self.output(planOrHist + ' file difference > ' + str(threshold) + '%\n')
        return status

    def checklp(self):
        same_file,str_diff = (test_comparaison(self.__file_lp,self.__file_lp_ref))       
        if not(same_file):
            self.output("\n Difference in the lp files" + self.__file_lp+ " and " +self.__file_lp_ref + "\n")
        else:
            self.output("\n NO difference in the lp files" + self.__file_lp+ " and " +self.__file_lp_ref + "\n")
        self.output("LP file check")
        self.output("================")
        self.output(str_diff)
        return (same_file)

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
                self.output("NO difference in the json files\n")
            else:
                self.output("Difference in the json files\n")
        else:
            self.output(typeFile + ' file not found\n')

        logfile.close()
        summary_update_file.close()

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

    def check(self,folder_res = "",file_res = "", file_ref = "",threshold=0.01,):
        # change name of results files if pegase
        if file_res != "":
            self.__file_res = os.path.join(self.__app_home,folder_res,file_res)
        if file_ref != "":
            self.__file_ref = os.path.join(self.__app_home,folder_res,file_ref)
        if os.getenv('BUILD_STEP') is None or 'CHECK' in os.getenv('BUILD_STEP'):
            if not os.path.exists(self.__directory_res):
                os.makedirs(self.__directory_res)
            infos = new_compare_results(self.__app_home, self.__file_res, self.__file_ref, self.__logfile, self.__directory_res, threshold, pegase=self.__pegase)
        
        return infos



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
from compare_results import compare_csv_files as compare_csv

from compare_lp import test_comparaison
from compareJson import compareJson

from sys import platform
try:
    import cairn as crn
except:
    import cairnopen as crn
import time

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
        
    def set_Results_ref(self, refPath):
        self.__file_ref = refPath

    def set_Results_file(self, refPath):
        self.__file_res = refPath

    def set_PLAN_ref(self, refPath):
        self.__file_plan_ref = refPath

    def set_HIST_ref(self, refPath):
        self.__file_hist_ref = refPath
    
    def set_LP_ref(self, refPath):
        self.__file_lp_ref = refPath

 
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
        simulation_control = problem.get_simulation_control()
        if simulation_control.get_setting_value("NbCycle")>1:
            self.__file_res = os.path.join(self.__app_home, self.__test_name+'_results_rollinghorizon.csv')
            self.__file_ref = os.path.join(self.__app_home, self.__test_name+'_Results_rh_Ref.csv')
        solution = problem.run()
        assert solution.status == "Optimal"
        return problem, inst, solution
    
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
            problem, inst, sol = self.runCairn()
        else:
            self.runPegase(tnr_xml=self.__test_name+".xml")
        updateFile(self.__file_plan,self.__file_plan_ref,overwrite, keep_current=keep_current)
        updateFile(self.__file_hist,self.__file_hist_ref,overwrite, keep_current=keep_current)
        updateFile(self.__file_res,self.__file_ref,overwrite, keep_current=keep_current)
        if checklp:
            simulation_control = problem.get_simulation_control()
            future_size = simulation_control.get_setting_value("FutureSize")
            simulation_control.set_setting_value("FutureSize",10)
            try:
                problem.run("checklp")
            except:
                simulation_control.set_setting_value("FutureSize",future_size)
                problem.run("checklp")
            updateFile(self.__file_lp,self.__file_lp_ref,True, keep_current=keep_current)
            simulation_control.set_setting_value("FutureSize",future_size)
        if self.__sampling!="":
            tab_results = self.runSampling()
            tab_results.to_csv(self.__sampling_results_ref, sep=";")
        if self.__pegase==False:
            inst.close_study()

    def generic_testing(self, skip_col=[], modif_check_lp=True):
        status = {}
        problem,inst,solution = self.runCairn()
        status["OPTIM"] = solution.status
        status["PLAN"] = self.checkPlanHist("PLAN")
        status["HIST"] = self.checkPlanHist("HIST")
        status["TIMESERIES"] = self.check(0.001, skip_col=skip_col)
        status["RUNLPFILE"] = False

        simulation_control = problem.get_simulation_control()
        future_size = simulation_control.get_setting_value("FutureSize")
        past_size = simulation_control.get_setting_value("PastSize")
        nb_cycle = simulation_control.get_setting_value("NbCycle")
        time_shift = simulation_control.get_setting_value("TimeShift")
        try:
            if modif_check_lp:
                simulation_control.set_setting_value("PastSize",1)
                simulation_control.set_setting_value("NbCycle",1)
                simulation_control.set_setting_value("TimeShift",1)
                simulation_control.set_setting_value("FutureSize",10)
            solution = problem.run("checklp")
            status["RUNLPFILE"] = solution.status
        except:
            status["RUNLPFILE"] = False

        if modif_check_lp:
            simulation_control.set_setting_value("PastSize", past_size)
            simulation_control.set_setting_value("NbCycle", nb_cycle)
            simulation_control.set_setting_value("FutureSize", future_size)
            simulation_control.set_setting_value("TimeShift", time_shift)

        if status["RUNLPFILE"]:
            status["LPFILE"] = self.checklp()
        else:
            status["LPFILE"] = False
        
        inst.close_study()
        return(status)
        
    def sampling_test(self, check_ts=False):
        tab_res = self.runSampling()
        res_sampling = {}
        try:
            tab_ref = pd.read_csv(self.__sampling_results_ref,sep=";", index_col=0)
        except:
            print("tab ref not found")
            tab_ref = 0
            res_sampling["PLAN"] = "Failed: no reference"
            return res_sampling
        tab_res.to_csv(self.__sampling_results,sep=";")

        #PLAN
        #force dtype of Case column to string in order to avoid type difference
        tab_res["Case"] = tab_res["Case"].astype(str)
        tab_ref["Case"] = tab_ref["Case"].astype(str)
        diff = (tab_res.round(decimals=3).reindex(sorted(tab_res.columns), axis=1)).compare(tab_ref.round(decimals=3).reindex(sorted(tab_ref.columns), axis=1))
        self.output("\n Sampling test \n ================ \n")
        if len(diff)==0:
            self.output("Results are identical\n")
            res_sampling["PLAN"] = "Success"
        else:
            self.output("Difference in the sampling\n")
            self.output(diff.to_string())
            res_sampling["PLAN"] = "Failed:" + str(tab_ref['Case'][diff.index].to_list())

        #TIMESERIES
        if check_ts:
            res_ts = []
            for case in tab_ref['Case'].to_list():
                status = self.check_only_ref_columns("Report_s"+case, self.__test_name+"_results_Results.csv", self.__test_name+"_Results_Ref.csv",0.001)
                if not status:
                    res_ts.append("Failed:"+case)
            if len(res_ts) == 0:
                res_sampling["TIMESERIES"] = "Success"
            else:
                res_sampling["TIMESERIES"] = str(res_ts)

        return res_sampling

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
        status = False
        test = '>'
        if isinstance(err,float) or isinstance(err,int) :
            if abs(err) < threshold:
                status = True
                test = '<'
        
            self.output(planOrHist + ' file difference '+str(abs(err))+ test + ' ' + str(threshold) + '%\n')

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

    def compare_csv_files(self, app_home, file_res, file_ref, threshold=0.1):
        status = False
        file_results   = path.join(app_home, file_res)
        file_reference = path.join(app_home, file_ref)
        logfile = open(os.path.join(app_home, 'report_testing.txt'),'a')

        if os.path.exists(file_results) and os.path.exists(file_reference):
            status = compare_csv(file_results, file_reference, logfile, threshold=0.1)
            if status:
                logfile.write("NO difference in the csv files\n\n")
            else:
                logfile.write("Difference in the csv files\n\n")
        else: 
            logfile.write('Csv file not found!\n\n')

        assert status == True

    def checkJson(self, app_home, fileNew='new_study.json', fileRef='study.json', skipCompoX=""):
        status = False
        file_json = os.path.join(app_home, fileNew)
        file_json_ref = os.path.join(app_home, fileRef)
        logfile = open(os.path.join(app_home, 'report_testing.txt'),'a')

        if os.path.exists(file_json) and os.path.exists(file_json_ref):
            status = compareJson(file_json_ref, file_json, logfile, skipCompoX)
            if status:
                logfile.write("NO difference in the json files\n")
            else:
                logfile.write("Difference in the json files\n")
        else:
            logfile.write('Json file not found\n')

        assert status == True

    def contain_file_with_extension(self, directory, extension, seconds):
        """
        Recursively checks if a file with the given extension exists in the directory,
        and that it was created within the last `seconds`
        """
        status = False
        logfile = open(os.path.join(directory, 'report_testing.txt'),'a')

        now = time.time()
        for root, _, files in os.walk(directory):
            for file in files:
                if file.lower().endswith(extension.lower()):
                    file_path = os.path.join(root, file)
                    created_time = os.path.getctime(file_path)
                    if (now - created_time) <= seconds:
                        dash = '-' * 40
                        logfile.write(dash)
                        logfile.write(dash+'\n')
                        logfile.write('HTML Report Found: %s\n'%file_path)
                        status = True
                        break
            if status:
                break

        assert status == True

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

    def check(self,folder_res = "",file_res = "", file_ref = "",threshold=0.01,skip_col=[]):
        # change name of results files if pegase
        if file_res != "":
            self.__file_res = os.path.join(self.__app_home,folder_res,file_res)
        if file_ref != "":
            self.__file_ref = os.path.join(self.__app_home,folder_res,file_ref)
        if os.getenv('BUILD_STEP') is None or 'CHECK' in os.getenv('BUILD_STEP'):
            if not os.path.exists(self.__directory_res):
                os.makedirs(self.__directory_res)
            infos = new_compare_results(self.__app_home, self.__file_res, self.__file_ref, self.__logfile, self.__directory_res, threshold, pegase=self.__pegase,skip_col=skip_col)
        
        return infos
    
    def checkResults(self, threshold=0.01,skip_col=[]):
               
        if os.getenv('BUILD_STEP') is None or 'CHECK' in os.getenv('BUILD_STEP'):
            if not os.path.exists(self.__directory_res):
                os.makedirs(self.__directory_res)
            infos = new_compare_results(self.__app_home, self.__file_res, self.__file_ref, self.__logfile, self.__directory_res, threshold, pegase=self.__pegase,skip_col=skip_col)
        
        return infos

    def check_only_ref_columns(self, folder_res="", file_res="", file_ref="", threshold=0.01):
        if file_res != "":
            self.__file_res = os.path.join(self.__app_home, folder_res, file_res)
        if file_ref != "":
            self.__file_ref = os.path.join(self.__app_home, folder_res, file_ref)
        #Find the columns to skip by keeping only the columns that are defined in the ref file
        results_df = pd.read_csv(self.__file_res, sep=";", index_col=[0]).dropna(axis=1, how="all")
        results_df_ref = pd.read_csv(self.__file_ref, sep=";", index_col=[0]).dropna(axis=1, how="all")
        skip_col = list(set(results_df.columns) - set(results_df_ref.columns))

        if not os.path.exists(self.__directory_res):
            os.makedirs(self.__directory_res)
        infos = new_compare_results(self.__app_home, self.__file_res, self.__file_ref, self.__logfile,
                                    self.__directory_res, threshold, pegase=self.__pegase, skip_col=skip_col)

        return infos

    def data_collection(self,compo_type=[],param_list=[]):
        inst = crn.CairnAPI()
        problem = inst.read_study(self.__study_file)
        dico = dict()
        for comp in compo_type:
            compos = problem.get_components(comp)
            for c in compos:
                compo_crn = problem.get_component(c)
                dico[c]=[]
                for p in param_list:
                    try:
                        v= compo_crn.get_indicator_value(p)
                    except:
                        v="Not applicable"
                    dico[c].append([p,v])
        return dico
            

        
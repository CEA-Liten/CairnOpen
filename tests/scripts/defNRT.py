
import os
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
import time

class defNRT:
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


        

import os
import subprocess
import psutil
import pandas as pd
from os import path, remove
from subprocess import Popen, check_output

from sys import platform
try:
    import cairn as crn
except:
    import cairnopen as crn
import time
import defNRT

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

class CairnNRT(defNRT.defNRT):        
    
    def runCairn(self):
        inst = crn.CairnAPI(False)
        print("======================= running test ===================")
        print(self._defNRT__study_file)
        print(self._defNRT__dataseries)
        problem = inst.read_study(self._defNRT__study_file)
        for ts in self._defNRT__dataseries:
            problem.add_timeseries(ts)
        simulation_control = problem.get_simulation_control()
        if simulation_control.get_setting_value("NbCycle")>1:
            self._defNRT__file_res = os.path.join(self._defNRT__app_home, self._defNRT__test_name+'_results_rollinghorizon.csv')
            self._defNRT__file_ref = os.path.join(self._defNRT__app_home, self._defNRT__test_name+'_Results_rh_Ref.csv')
        solution = problem.run()
        assert solution.status == "Optimal"
        return problem, inst, solution
    
    def runPegase(self, tnr_dir="", tnr_xml="TNR.xml", maxtime=1200):
        if os.getenv('BUILD_STEP') is None or 'RUN' in os.getenv('BUILD_STEP'):
            #self.setPlanHistFiles()
            #self.copyPlanHistFiles()

            xml_file = os.path.join(self._defNRT__app_home, tnr_dir, tnr_xml)
            assert os.path.exists(xml_file), "XML file not found " + xml_file
            print("Running with ", xml_file)
           
            if platform == "win32":
                run_script = os.path.join(self._defNRT__script_home, "runTNR.bat")
            elif platform == "linux":
                run_script = os.path.join(self._defNRT__script_home, "runTNR.sh")
            assert os.path.exists(run_script), "run script not found " + run_script

            run_args = [run_script, tnr_dir, tnr_xml, self._defNRT__app_home]
            p = Popen(run_args, cwd=self._defNRT__app_home)
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
        if self._defNRT__sampling != "":
            df_sens = pd.read_csv(self._defNRT__sampling, sep=";", decimal=".", header=[0,1], index_col=0)
        if os.path.exists(self._defNRT__sampling_kpi):
            df_kpi = pd.read_csv(self._defNRT__sampling_kpi, sep=";")
        else:
            df_kpi = pd.DataFrame()
        problem = inst.read_study(self._defNRT__study_file)
        for ts in self._defNRT__dataseries:
            problem.add_timeseries(ts)
        tab_results = crn.run_sensitivity(problem,df_sens,indicators=df_kpi)
        inst.close_study()
        return tab_results


    def updateTest(self, overwrite, keep_current = False, checklp=True):
        if self._defNRT__pegase==False:
            problem, inst, sol = self.runCairn()
        else:
            self.runPegase(tnr_xml=self._defNRT__test_name+".xml")
        updateFile(self._defNRT__file_plan,self._defNRT__file_plan_ref,overwrite, keep_current=keep_current)
        updateFile(self._defNRT__file_hist,self._defNRT__file_hist_ref,overwrite, keep_current=keep_current)
        updateFile(self._defNRT__file_res,self._defNRT__file_ref,overwrite, keep_current=keep_current)
        if checklp:
            simulation_control = problem.get_simulation_control()
            future_size = simulation_control.get_setting_value("FutureSize")
            simulation_control.set_setting_value("FutureSize",10)
            try:
                problem.run("checklp")
            except:
                simulation_control.set_setting_value("FutureSize",future_size)
                problem.run("checklp")
            updateFile(self._defNRT__file_lp,self._defNRT__file_lp_ref,True, keep_current=keep_current)
            simulation_control.set_setting_value("FutureSize",future_size)
        if self._defNRT__sampling!="":
            tab_results = self.runSampling()
            tab_results.to_csv(self._defNRT__sampling_results_ref, sep=";")
        if self._defNRT__pegase==False:
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
            tab_ref = pd.read_csv(self._defNRT__sampling_results_ref,sep=";", index_col=0)
        except:
            print("tab ref not found")
            tab_ref = 0
            res_sampling["PLAN"] = "Failed: no reference"
            return res_sampling
        tab_res.to_csv(self._defNRT__sampling_results,sep=";")

        #PLAN
        #force dtype of Case column to string in order to avoid type difference
        tab_res["Case"] = tab_res["Case"].astype(str)
        tab_ref["Case"] = tab_ref["Case"].astype(str)
        diff = (tab_res.round(decimals=3).sort_values('Case', ignore_index=True).reindex(sorted(tab_res.columns), axis=1)).compare(tab_ref.round(decimals=3).sort_values('Case', ignore_index=True).reindex(sorted(tab_ref.columns), axis=1))
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
                status = self.check_only_ref_columns("Report_s"+case, self._defNRT__test_name+"_results_Results.csv", self._defNRT__test_name+"_Results_Ref.csv",0.001)
                if not status:
                    res_ts.append("Failed:"+case)
            if len(res_ts) == 0:
                res_sampling["TIMESERIES"] = "Success"
            else:
                res_sampling["TIMESERIES"] = str(res_ts)

        return res_sampling

    
    def data_collection(self, compo_type=[], param_list=[]):
        inst = crn.CairnAPI()
        problem = inst.read_study(self._defNRT__study_file)
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
    
            

        
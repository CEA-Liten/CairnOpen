import sys
import os
import defNRT as NRT



def checkResults(name_study, app_home, refResults="", refPLAN="", refHIST="", refLP="", rollingHorizon=""):
    tnr = NRT.defNRT(name=name_study,app_home=app_home)

    status = {}        
    #status["OPTIM"] = solution.status
    if refPLAN!="":
        tnr.set_PLAN_ref(refPLAN)
        status["PLAN"] = tnr.checkPlanHist("PLAN")
    if refHIST!="":
        tnr.set_HIST_ref(refHIST)
        status["HIST"] = tnr.checkPlanHist("HIST")
    if refLP:        
        tnr.set_LP_ref(refLP)
        status["LPFILE"] = tnr.checklp()

    tnr.set_Results_ref(refResults)
    if rollingHorizon!="":
        tnr.set_Results_file(rollingHorizon)
    status["TIMESERIES"] = tnr.checkResults(threshold=0.001)

    # status à écrire
    resultsfile = open(os.path.join(app_home, name_study + '_checkResults.txt'),'w')
    for k,v in status.items():        
        resultsfile.write(str(k)+";"+str(v)+"\n")

def getRefParam(param : str, name : str) -> str :
    retParam = param
    if param=="0": retParam=""
        
    print("Ref {}: {}".format(name, retParam))
    return retParam

if __name__ == '__main__':
    print("----------------------- checkResults arguments------------------------------")
    # argment list : 
    # 1. app_home
    # 2. test_case    
    if len(sys.argv) > 2: 
        app_home = sys.argv[1]
        print("App Home:", app_home)
        testcase=sys.argv[2]
        print("Test Case Name:", testcase)

    # options:
    # 3. nom de la référence Results
    # 4. nom de la référence PLAN (pas de vérification si 0)
    # 5. nom de la référence HIST (pas de vérification si 0)
    # 6. nom de la référence LP (pas de vérification si 0)
    # 7. nom du fichier results pour le mode 'rollinghorizon'
    refResults=""
    refPLAN=""
    refHIST=""
    refLP=""
    rollingHorizon=""

    if len(sys.argv) > 3:    
        refResults = getRefParam(sys.argv[3], "Results")  
    if len(sys.argv) > 4: 
        refPLAN = getRefParam(sys.argv[4], "PLAN")                
    if len(sys.argv) > 5: 
        refHIST = getRefParam(sys.argv[5], "HIST")                
    if len(sys.argv) > 6: 
        refLP = getRefParam(sys.argv[6], "LP")                        
    if len(sys.argv) > 7: 
        rollingHorizon = getRefParam(sys.argv[7], "RH")                        
        
    checkResults(testcase, app_home, refResults, refPLAN, refHIST, refLP, rollingHorizon)


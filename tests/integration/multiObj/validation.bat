set fic1=%~dp0\test_multiobj_results_Results.csv
set fic2=%~dp0\test_multiobj_results_Results_Ref.csv
copy %fic1% %fic2%
set fic1=%~dp0\test_multiobj_results_PLAN.csv
set fic2=%~dp0\test_multiobj_results_PLAN_Ref.csv
copy %fic1% %fic2%
set fic1=%~dp0\test_multiobj_results_HIST.csv
set fic2=%~dp0\test_multiobj_results_HIST_Ref.csv
copy %fic1% %fic2%

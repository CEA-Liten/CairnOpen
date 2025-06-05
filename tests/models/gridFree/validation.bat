set fic1=%~dp0\TNR\test_methanizer_Results.csv
set fic2=%~dp0\TNR\test_methanizer_Results-Reference.csv
copy %fic1% %fic2%

copy %~dp0\test_methanizer_results_PLAN.csv %~dp0\test_methanizer_results_PLAN_REF.csv

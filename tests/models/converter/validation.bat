set TESTCASE=%~dp0\test_converter
copy %TESTCASE%_Results.csv %TESTCASE%_Results_Ref.csv
copy %TESTCASE%_results_PLAN.csv %TESTCASE%_results_PLAN_Ref.csv
copy %TESTCASE%_results_HIST.csv %TESTCASE%_results_HIST_Ref.csv
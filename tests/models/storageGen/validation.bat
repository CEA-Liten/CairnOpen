set TESTCASE=%~dp0\Test_battery
copy %TESTCASE%_results_PLAN.csv %TESTCASE%_results_PLAN_REF.csv
copy %TESTCASE%_results.csv %TESTCASE%_results-Reference.csv

for %%x in (base withCO2GridTS withReplacement) do (
copy %~dp0\Report_s%%x\test_envimpacts_%%x_results_PLAN.csv %~dp0\test_envimpacts_%%x_results_PLAN_Ref.csv
copy %~dp0\Report_s%%x\test_envimpacts_%%x_results_HIST.csv %~dp0\test_envimpacts_%%x_results_HIST_Ref.csv
copy %~dp0\Report_s%%x\test_envimpacts_%%x_Results.csv %~dp0\test_envimpacts_%%x_Results_Ref.csv
)

for %%x in (optimGWP optimNPV constraint) do (
copy %~dp0\Report_s%%x\test_envimpactsconstraint_%%x_results_PLAN.csv %~dp0\test_envimpactsconstraint_%%x_results_PLAN_Ref.csv
copy %~dp0\Report_s%%x\test_envimpactsconstraint_%%x_results_HIST.csv %~dp0\test_envimpactsconstraint_%%x_results_HIST_Ref.csv
copy %~dp0\Report_s%%x\test_envimpactsconstraint_%%x_Results.csv %~dp0\test_envimpactsconstraint_%%x_Results_Ref.csv
)
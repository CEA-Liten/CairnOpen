#! /bin/bash

#source ${PEGASE_HOME}/TNR_env.sh
source ${PEGASE_HOME}/GenericAppEnv.sh
source ${PEGASE_HOME}/lib/import/TestingScripts/pythonEnv.sh

export PYTHONPATH=${PEGASE_HOME}/lib/import/TestingScripts/RunPegaseTests:$PYTHONPATH && python3 -m pytest -m Linux --junitxml=$PEGASE_HOME/tests/Pegase-TNR.xml tests/
export PYTHONPATH=${PEGASE_HOME}/lib/import/TestingScripts/RunPegaseTests:$PYTHONPATH && python3 $PEGASE_HOME/lib/import/TestingScripts/RunPegaseTests/htmlReport.py $PEGASE_HOME/tests/Pegase-TNR

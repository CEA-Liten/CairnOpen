#! /bin/bash

#source ${CAIRN_HOME}/TNR_env.sh
source ${CAIRN_HOME}/GenericAppEnv.sh
source ${CAIRN_HOME}/tests/scripts/pythonEnv.sh

python -m pytest -m Linux --junitxml=$PEGASE_HOME/tests/Pegase-TNR.xml tests/
#export PYTHONPATH=${PEGASE_HOME}/lib/import/TestingScripts/RunPegaseTests:$PYTHONPATH && python3 $PEGASE_HOME/lib/import/TestingScripts/RunPegaseTests/htmlReport.py $PEGASE_HOME/tests/Pegase-TNR

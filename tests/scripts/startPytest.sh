#!/bin/bash

export HTML_ARG="tests/reports/Cairn-TNR"
export REPORT_UPDATE=Cairn-TNR_update.txt
export REPORT=$CAIRN_HOME/tests/reports/Cairn-TNR.xml

#source ${CAIRN_HOME}/TNR_env.sh
source ${CAIRN_HOME}/tests/scripts/pythonEnv.sh

python -m pytest -v --junitxml $REPORT tests/ --ignore=${CAIRN_HOME}/tests/privateTests/Tests_Base_National --ignore=${CAIRN_HOME}/tests/privateTests/pegase --ignore=${CAIRN_HOME}/tests/toolbox/uranie -k "not Wind.Test_Eolien_Qualygrids and not bouin_7_cont.test_bouin_7_cont"

echo "in html"
python -u ${CAIRN_HOME}/tests/scripts/htmlReportLste.py $HTML_ARG

echo "ending"
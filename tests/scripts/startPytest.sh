#!/bin/bash

# Checking VAR in env
if ! [[ -v CAIRN_HOME ]];
then
    echo "\t☸  [ERROR]: CAIRN_HOME is missing, please set it and run again"
    exit
fi

cd ${CAIRN_HOME}

if [ ! -d "./tests/reports" ]; then
    mkdir ./tests/reports 
fi


export OPTION=$1
if [ "$OPTION" == "" ]; then
    export OPTION=Release
fi;

export TESTDIR=$2
if [ "$TESTDIR" == "" ]; then
    export TESTDIR=tests/apipython
fi;

source GenericAppEnv.sh $OPTION
export REPORT=$CAIRN_HOME/tests/reports/CairnPytest-TNR

python -m pytest -v --junitxml $REPORT.xml $TESTDIR --ignore=${CAIRN_HOME}/tests/privateTests/Tests_Base_National --ignore=${CAIRN_HOME}/tests/privateTests/pegase --ignore=${CAIRN_HOME}/tests/toolbox/uranie -k "not Wind.Test_Eolien_Qualygrids and not bouin_7_cont.test_bouin_7_cont"

echo "in html"
junit2html  $REPORT.xml $REPORT.html

echo "ending"
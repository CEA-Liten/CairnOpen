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
    export TESTDIR=out/$OPTION
fi;

source GenericAppEnv.sh $OPTION
export REPORT=$CAIRN_HOME/tests/reports/CairnCtest-TNR

ctest --build-config $OPTION --test-dir $TESTDIR --output-junit $REPORT.xml || true

#ctest -T Test -T Coverage --build-config $OPTION --test-dir $TESTDIR

#convert to html
junit2html  $REPORT.xml $REPORT.html

#force script to return code 0
exit 0

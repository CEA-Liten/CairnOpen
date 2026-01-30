#!/bin/bash

# Coloring OUPTUT
export RESET='\e[1;0m'
export BLACK='\e[1;30m'
export RED='\e[1;31m'
export GREEN='\e[1;32m'
export YELLOW='\e[1;33m'
export BLUE='\e[1;34m'
export MAGENTA='\e[1;35m'
export CYAN='\e[1;36m'
export WHITE='\e[1;37m'

set -e

# Checking VAR in env
if ! [[ -v CAIRN_HOME ]];
then
	echo -e "$RED \t☸  [ERROR]: CAIRN_HOME is missing, please set it and run again"
	exit
fi

cd ${CAIRN_HOME}

if [ ! -d "./tests/reports" ]; then
	mkdir ./tests/reports 
fi


export OPTION=$1
if [ "$OPTION" == "" ]; then
 	export OPTION=release
fi;

export TESTDIR=$2
if [ "$TESTDIR" == "" ]; then
 	export TESTDIR=out/$OPTION
fi;

source GenericAppEnv.sh $OPTION


ctest --build-config $OPTION --test-dir $TESTDIR --output-junit $CAIRN_HOME/tests/reports/CairnCtest-TNR.xml || true

#convert to html
#pip install junit2html
#junit2html  $CAIRN_HOME/tests/reports/CairnCtest-TNR.xml $CAIRN_HOME/tests/reports/CairnCtest-TNR.html

#force script to return code 0
exit 0

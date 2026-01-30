#!/bin/bash
# Setting CAIRN Python environnement

source ${CAIRN_HOME}/GenericAppEnv.sh 

# Set build environnement
echo -e "\n$YELLOW➡  Setting up Python environnement $RESET"

export PATH="$PYTHON_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$PYTHON_HOME/lib:$LD_LIBRARY_PATH"

echo -e "$CYAN \t☸  Activating Python environnement $RESET"
source ${PYTHON_VENV}/bin/activate

echo -e "$GREEN \t✅ Python is: $(which python3)"
echo -e "\t✅ PYTHON_HOME is $PYTHON_HOME"
echo -e "\t✅ PYTHON_VENV is $PYTHON_VENV"
echo -e "\t✅ PATH is $PATH"
echo -e "\t✅ LD_LIBRARY_PATH is $LD_LIBRARY_PATH"
echo -e "$CYAN \t☸  Setting requirements, it might take some time, see req.log $RESET"
python3 -m pip install  -r ${CAIRN_HOME}/tests/scripts/reqs_tests.txt > ${CAIRN_HOME}/reqs.log
python3 -m pip install pandas pyyaml requests tzlocal > ${CAIRN_HOME}/reqs.log

echo -e "$GREEN \t✅ Python status: Running $RESET"



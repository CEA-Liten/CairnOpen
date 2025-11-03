# Setting CAIRN Python environnement

# Set build environnement
echo -e "\n$YELLOW➡  Setting up Python environnement $RESET"
echo -e "$PYTHON_HOME"
echo -e "$PYTHON_VENV"

echo -e "$CYAN \t☸  Activating Python environnement $RESET"
source ${PYTHON_VENV}/bin/activate
export PATH=$PYTHON_HOME/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_HOME/lib:$LD_LIBRARY_PATH
echo -e "$GREEN \t✅ Python is: $(which python3) $RESET"
echo "$PATH"
#echo -e "$CYAN \t☸  Setting requirements, it might take some time, see req.log $RESET"
python3 -m pip install  -r ${CAIRN_HOME}/tests/scripts/reqs_core.txt >${CAIRN_HOME}/reqs.log
python3 -m pip install  -r ${CAIRN_HOME}/tests/scripts/reqs_TNR.txt >${CAIRN_HOME}/reqs.log
python3 -m pip install pandas pyyaml requests tzlocal >${PEGASE_HOME}/reqs.log

echo -e "$GREEN \t✅ Python status: Running $RESET"
#echo -e "$GREEN \t✅ Python is: $(which python3) $RESET"



#!/bin/bash
OLD_PATH=$PATH
OLD_LDPATH=$LD_LIBRARY_PATH

TNR_DIR=$1
TNR_XML=$2
RUN_DIR=$3

if [ -e $TNR_DIR/$TNR_XML ]
then
  cp -r $TNR_DIR/$TNR_XML .
else
  rm $TNR_XML
fi

cd $RUN_DIR
echo -e "\t | RUN_DIR is ${RUN_DIR}"
#export USE_FBSF_FULL_BATCH=true
. ./AppEnv.sh

echo -e "\t | TNR_XML is ${TNR_XML}"
echo -e "\t | USE_FBSF_FULL_BATCH is ${USE_FBSF_FULL_BATCH}"
export QT_DEBUG_PLUGINS=1

if [ "$USE_FBSF_FULL_BATCH" = "true" ]; then 
	FbsfBatch $TNR_XML
else
	xvfb-run FbsfFramework $TNR_XML -no-gui
fi; 



export PATH=$OLD_PATH
export LD_LIBRARY_PATH=$OLD_LDPATH

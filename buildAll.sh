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

cmake --version

export BUILD_TYPE=$1
if [ "$BUILD_TYPE" = "" ]; then 
    export BUILD_TYPE=Release
fi;

export OPTIONS=$2
export OPTION_PRIVATE=-DWITH_PRIVATEMODELS=OFF
if [ "$OPTIONS" = "all" ]; then 
    export OPTION_PRIVATE=-DWITH_PRIVATEMODELS=ON
fi;

export INSTALLDEPS=$3
export OPTION_DEPS=
if [ "$INSTALLDEPS" = "deps" ]; then 
    export OPTION_DEPS=-DDEPS_ROOT:STRING=/home/share/570-Energie/570.15-TRILOGY/tools/DepsCairn
fi;

export OPTIONS_COV=$4
export OPTION_COV=
if [ "$OPTIONS_COV" = "cov" ]; then 
    export OPTION_COV=-DTEST_WITH_COVERAGE=ON
fi;

export OPTIONS_FILE=$5
if [ "$OPTIONS_FILE" = "" ]; then 
    export OPTIONS_FILE=linux_options.cmake
fi;


echo -e "\t | BUILD_TYPE set to ${BUILD_TYPE}"
echo -e "\t | OPTION_PRIVATE set to ${OPTION_PRIVATE}"
echo -e "\t | OPTION_DEPS set to ${OPTION_DEPS}"
echo -e "\t | OPTIONS_FILE set to ${OPTIONS_FILE}"


export BUILD_PATH=out/${BUILD_TYPE}
if [ -d "${BUILD_PATH}" ]; then
  rm -r "${BUILD_PATH}"
fi
mkdir -p "${BUILD_PATH}"
echo -e "\t | BUILD_PATH is ${BUILD_PATH}"

export INSTALL_PATH=bin/${BUILD_TYPE}
if [ -d "${INSTALL_PATH}" ]; then
  rm -r "${INSTALL_PATH}"
fi
echo -e "\t | INSTALL_PATH is ${INSTALL_PATH}"

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DUSER_OPTIONS_FILE=cmake/${OPTIONS_FILE} -DPRESETNAME=${BUILD_TYPE} ${OPTION_PRIVATE} ${OPTION_DEPS} ${OPTION_COV} -S . -B ${BUILD_PATH}

cmake --build ${BUILD_PATH}  --config ${BUILD_TYPE} -j $(nproc)

cmake --install ${BUILD_PATH} --config ${BUILD_TYPE} --prefix ${INSTALL_PATH}



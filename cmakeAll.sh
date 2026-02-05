#!/bin/bash

#export PATH=/home/prog/cmake/cmake-3.29.3/u20/bin:$PATH
cmake --version

export BUILD_TYPE=$1
if [ "$BUILD_TYPE" = "" ]; then 
    export BUILD_TYPE=release
fi;

export OPTIONS_FILE=$2
if [ "$OPTIONS_FILE" = "" ]; then 
    export OPTIONS_FILE=linux_options.cmake
fi;
echo -e "\t | OPTIONS_FILE set to ${OPTIONS_FILE}"
echo -e "\t | BUILD_TYPE set to ${BUILD_TYPE}"

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

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DUSER_OPTIONS_FILE=cmake/${OPTIONS_FILE} -S . -B ${BUILD_PATH}

cmake --build ${BUILD_PATH}  --config ${BUILD_TYPE} -j $(nproc)

cmake --install ${BUILD_PATH} --config ${BUILD_TYPE} --prefix ${INSTALL_PATH}

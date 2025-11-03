#
#	Cairn wheel installation
#
include($ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/installWheel_options.cmake)

message("====== Install Wheel =========")
message("Python: ${Python_EXECUTABLE}")
message("INSTALL_WHEEL_VENV: ${INSTALL_WHEEL_VENV}")
message("DEST_DIR: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}")

find_package (Python COMPONENTS Interpreter)
message("Python: ${Python_EXECUTABLE}")

# create virtual environment
execute_process (COMMAND "${Python_EXECUTABLE}" -m venv "${INSTALL_WHEEL_VENV}" COMMAND_ERROR_IS_FATAL ANY)

# Activate python environment => !! ne fonctionnne pas
#set (ENV{VIRTUAL_ENV} "${INSTALL_WHEEL_VENV}") # 
#set (Python_FIND_VIRTUALENV ONLY)
#unset(Python_EXECUTABLE)
#find_package (Python COMPONENTS Interpreter)
#message("Python: ${Python_EXECUTABLE}")

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	set(PythonCMD ${INSTALL_WHEEL_VENV}/scripts/pip)
else()
	set(PythonCMD ${INSTALL_WHEEL_VENV}/bin/pip)
endif()

execute_process(COMMAND ${PythonCMD} install -r "${CAIRNTESTS_HOME}/scripts/reqs_core.txt")
execute_process(COMMAND ${PythonCMD} install -r "${CAIRNTESTS_HOME}/scripts/reqs_NRT.txt")
execute_process(COMMAND ${PythonCMD} install -r "${CAIRNTESTS_HOME}/scripts/reqs_core.txt")
# uninstall previous cairn
execute_process(COMMAND ${PythonCMD} uninstall -y cairn)

# get whl file
file(GLOB FILES_LIST $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../*.whl)
message("whl files: ${FILES_LIST}")


# install cairn with the wheel file
list(LENGTH FILES_LIST nbWhl)
if (nbWhl GREATER 0)
	# take the first elem
	list(GET FILES_LIST 0 WHL_FILE)
	message("whl file: ${WHL_FILE}")
	execute_process(COMMAND ${PythonCMD} install "${WHL_FILE}")
endif()




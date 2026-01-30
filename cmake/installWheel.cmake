#
#	Cairn wheel installation
#
include($ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/installWheel_options.cmake)

message("====== Install Wheel =========")
message("Python: ${Python_EXECUTABLE}")
message("INSTALL_WHEEL_VENV: ${INSTALL_WHEEL_VENV}")
message("DEST_DIR: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}")

 # create virtual environment
execute_process (COMMAND "${Python_EXECUTABLE}" -m venv "${INSTALL_WHEEL_VENV}" COMMAND_ERROR_IS_FATAL ANY)

if(CMAKE_HOST_SYSTEM_NAME MATCHES Windows)
	set(PythonCMD ${INSTALL_WHEEL_VENV}/scripts/pip)
else()
	set(PythonCMD ${INSTALL_WHEEL_VENV}/bin/pip)
endif()
message("PythonCMD: ${PythonCMD}")
execute_process(COMMAND ${PythonCMD} install -r "${CAIRNTESTS_HOME}/scripts/reqs_tests.txt")

# uninstall previous cairn
execute_process(COMMAND ${PythonCMD} uninstall -y cairn)

# get whl file
file(GLOB FILES_LIST $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../cairn-${PROJECT_VERSION}*.whl)

# install cairn with the wheel file
list(LENGTH FILES_LIST nbWhl)
if (nbWhl GREATER 0)
	# take the first elem
	list(GET FILES_LIST 0 WHL_FILE)
	message("whl file: ${WHL_FILE}")
	execute_process(COMMAND ${PythonCMD} install "${WHL_FILE}")
endif()




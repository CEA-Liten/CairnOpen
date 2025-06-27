# CMakeList.txt : projet CMake pour Cairn
cmake_minimum_required (VERSION 3.23)

# Set cmake modules directories
# (i.e. the those which contain all user-defined FindXXX.cmake files among other things)
set(CMAKE_MODULE_PATH 
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/lib/CMakeTools
)

# Force out-of-source build
if ("${CMAKE_SOURCE_DIR}" MATCHES "${CMAKE_BINARY_DIR}")
  message (SEND_ERROR "In source building not supported (not recommanded indeed). You need to :
    * cleanup your source directory :  \"rm -rf ./CMakeFiles/ ./CMakeCache.txt\"
    * try configure process again in a new directory
    (e.g. mkdir <anywhere>/build ; cd <anywhere>/build ; cmake ${CMAKE_SOURCE_DIR}) ...")
  return()
endif()

# If not given, turn build type to release mode.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release." FORCE)
endif()

# To avoid section limitation in .obj file
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Gy-")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

# Some useful macros and functions
include(CMakeTools)

# cmake project name
set(PROJECT_NAME cairn)

# Read user option file, if provided on command line
if(USER_OPTIONS_FILE)
  # Check for tilde in file name (not handled properly by cmake)
  string(FIND ${USER_OPTIONS_FILE} "\~" res)
  if(res EQUAL 0)
    string(REPLACE "\~" "$ENV{HOME}" USER_OPTIONS_FILE ${USER_OPTIONS_FILE})
  endif()
  if(NOT IS_ABSOLUTE ${USER_OPTIONS_FILE})
    get_filename_component(USER_OPTIONS_FILE ${USER_OPTIONS_FILE} REALPATH
      BASE_DIR ${CMAKE_SOURCE_DIR})
  endif()
  message("\n Load user-defined options set from file ${USER_OPTIONS_FILE}\n")
  # Configure to take into account any change in ${USER_OPTIONS_FILE}
  configure_file(${USER_OPTIONS_FILE} current_options.cmake COPYONLY)
  include(${CMAKE_CURRENT_BINARY_DIR}/current_options.cmake)
else()
  message("\n Load default configuration set from file ${CMAKE_SOURCE_DIR}/cmake/default_options.cmake\n")
  include(default_options)
endif()


# ============= The project =============
# Set project name and project languages
include(ProjectSetup)
project(${PROJECT_NAME} LANGUAGES CXX C  VERSION ${${PROJECT_NAME}_VERSION}) 
set(CMAKE_CXX_STANDARD 17)  

# ========= Tests setup =========
if(WITH_TESTING)  
  include(CTest)
  enable_testing()  
endif()

# Common setup
if (BUILD_MIPMODELER)  
    message("\n--------------------------------------------------")    
    add_subdirectory(${MIPMODELER_HOME})             
endif()

if (BUILD_CAIRN)  
    message("\n--------------------------------------------------")
    add_subdirectory(${CAIRN_HOME})

    if (BUILD_MODULECAIRN)
        message("\n--------------------------------------------------")
        list(APPEND CMAKE_MODULE_PATH ${PEGASE_INSTALL}/cmake) 
        message("PEGASE_INSTALL: ${PEGASE_INSTALL}")
        message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")        
        add_subdirectory(${MODULECAIRN_HOME})        
    endif()
   
endif()

if (BUILD_CAIRNGUI) 
    message("\n--------------------------------------------------")
    add_subdirectory(${CAIRNGUI_HOME})   
endif()


# ============= Summary =============
message(STATUS "============================================ Summary ============================================")
message(STATUS "${PROJECT_NAME} version ${PROJECT_VERSION} is now ready for compilation and installation.")
message(STATUS "C++ Compiler : ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ flags : ${CMAKE_CXX_FLAGS}")
message(STATUS "C Compiler : ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "C flags : ${CMAKE_C_FLAGS}")
message(STATUS "Compilation mode is : ${CMAKE_BUILD_TYPE}")
message(STATUS "Code Sources are in : ${CMAKE_SOURCE_DIR}")
message(STATUS "Testing is : ${WITH_TESTING}")
message(STATUS "PyBind is : ${WITH_PYBIND}")
message(STATUS "Privatemodels is : ${WITH_PRIVATEMODELS}")
message(STATUS "Build GUI : ${BUILD_CAIRNGUI}")
message(STATUS "Licence is : ${WITH_LICENCE}")
message(STATUS "=================================================================================================\n")

project(doc_developpeur)

find_package(Doxygen REQUIRED)

# Find all the public headers
get_target_property(PERSEE_PUBLIC_HEADER_DIR doc_developpeur INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE PERSEE_PUBLIC_HEADERS ${PERSEE_PUBLIC_HEADER_DIR}/*.h)

set(DOXYGEN_INPUT_DIR "\"${PROJECT_SOURCE_DIR}/../../../lib/export/Persee\" \"${PROJECT_SOURCE_DIR}/dev_user_guide\"")
set(DOXYGEN_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../../htmlDoxy)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYGEN_EXCLUDE_DIRS "\"${PROJECT_SOURCE_DIR}/../../../lib/export/Persee/core/utils\" \"${PROJECT_SOURCE_DIR}/../../../lib/export/Persee/Test_API\" \"${PROJECT_SOURCE_DIR}/../../../lib/export/Persee/Test_Persee/Tests_Base_National\"")
set(DOXYGEN_LOGO_FILE ${PROJECT_SOURCE_DIR}/images/Persee.png)

#Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${PERSEE_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs")

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

# Add the cmake folder so the FindSphinx module is found
#[[
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_SOURCE_DIR}/../../htmlDoxySphinx)

add_custom_target(Sphinx ALL
                  COMMAND ${SPHINX_EXECUTABLE} -b html
                  # Tell Breathe where to find the Doxygen output
                  -Dbreathe_projects.Persee=${DOXYGEN_OUTPUT_DIR}/xml
                  ${SPHINX_SOURCE} ${SPHINX_BUILD}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx")
]]
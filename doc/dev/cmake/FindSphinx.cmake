#Look for an executable called sphinx-build

set(SPHINX_HOME C:/PythonPegase/3_10_9/envDocPersee/Scripts CACHE INTERNAL "Sphinx home path")

find_program(SPHINX_EXECUTABLE
             NAMES sphinx-build
             PATHS "${SPHINX_HOME}"
             DOC "Path to sphinx-build executable")

include(FindPackageHandleStandardArgs)

#Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Sphinx
                                  "Failed to find sphinx-build executable"
                                  SPHINX_EXECUTABLE)
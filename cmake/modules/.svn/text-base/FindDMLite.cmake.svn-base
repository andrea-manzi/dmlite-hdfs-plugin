# - Try to find DMLITE libraries
#
#  DMLITE_FOUND - System has DMLITE
#  DMLITE_INCLUDE_DIR - The DMLITE include directory
#  DMLITE_LIBRARIES - The libraries needed to use DMLITE
#
# DMLITE_LOCATION
#   setting this enables search for dmlite libraries / headers in this location


# -----------------------------------------------------
# DMLITE Libraries
# -----------------------------------------------------
find_library(DMLITE_LIBRARIES
    NAMES dmlite
    HINTS ${DMLITE_LOCATION}/lib ${DMLITE_LOCATION}/lib64 ${DMLITE_LOCATION}/lib32
    DOC "The main dmlite library"
)
if(DMLITE_LIBRARIES)
    message(STATUS "dmlite library found in ${DMLITE_LIBRARIES}")
endif()

# -----------------------------------------------------
# DMLITE Include Directories
# -----------------------------------------------------
find_path(DMLITE_INCLUDE_DIR
    NAMES dmlite/c/dmlite.h dmlite/cpp/dmlite++.h
    HINTS ${DMLITE_LOCATION} ${DMLITE_LOCATION}/include ${DMLITE_LOCATION}/include/*
    DOC "The dmlite include directory"
)
if(DMLITE_INCLUDE_DIR)
    message(STATUS "dmlite includes found in ${DMLITE_INCLUDE_DIR}")
endif()

# -----------------------------------------------------
# handle the QUIETLY and REQUIRED arguments and set DMLITE_FOUND to TRUE if
# all listed variables are TRUE
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(dmlite DEFAULT_MSG DMLITE_LIBRARIES DMLITE_INCLUDE_DIR)
mark_as_advanced(DMLITE_INCLUDE_DIR DMLITE_LIBRARIES)

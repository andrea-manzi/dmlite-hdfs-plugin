#
# This module detects if hadoop is installed and determines where the
# include files and libraries are.
#
# This code sets the following variables:
# 
# HDFS_LIBRARIES   = full path to the hdfs libraries
# HDFS_INCLUDE_DIR = include dir to be used when using the hdfs library
# HDFS_FOUND       = set to true if hdfs was found successfully
#
# HDFS_LOCATION
#   setting this enables search for hdfs libraries / headers in this location


# -----------------------------------------------------
# DPM Libraries
# -----------------------------------------------------
find_library(HDFS_LIBRARIES
    NAMES hdfs
    HINTS ${HDFS_LOCATION}/lib ${HDFS_LOCATION}/lib64 ${HDFS_LOCATION}/lib32
    DOC "The main hdfs library"
)

# -----------------------------------------------------
# DPM Include Directories
# -----------------------------------------------------
find_path(HDFS_INCLUDE_DIR 
    NAMES hdfs.h 
    HINTS ${HDFS_LOCATION} ${HDFS_LOCATION}/include ${HDFS_LOCATION}/include/*
    DOC "The hdfs include directory"
)
if(HDFS_INCLUDE_DIR)
    message(STATUS "hdfs includes found in ${HDFS_INCLUDE_DIR}")
endif()

# -----------------------------------------------------
# handle the QUIETLY and REQUIRED arguments and set DPM_FOUND to TRUE if 
# all listed variables are TRUE
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(hdfs DEFAULT_MSG HDFS_LIBRARIES HDFS_INCLUDE_DIR)
mark_as_advanced(HDFS_INCLUDE_DIR HDFS_LIBRARIES)


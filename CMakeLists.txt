cmake_minimum_required (VERSION 2.6)

project (dmlite-plugins-hdfs)

# Version
set(dmlite_plugins_hdfs_MAJOR 0)
set(dmlite_plugins_hdfs_MINOR 7)
set(dmlite_plugins_hdfs_PATCH 3)

#--------------------------------
# needed for Boost 2.8
set (Boost_NO_BOOST_CMAKE ON)
#---------------------------------

# Lib suffix
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set (LIB_SUFFIX ""
  		CACHE STRING "library directory suffix value")
else (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set (LIB_SUFFIX "64"
		CACHE STRING "library directory suffix value")
endif (CMAKE_SIZEOF_VOID_P EQUAL 4)

# Cmake module directory
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

# Definitions
add_definitions (-D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64)

# Subdirectories
add_subdirectory (src)
add_subdirectory (etc)
#add_subdirectory (test)


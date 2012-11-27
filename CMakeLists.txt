cmake_minimum_required (VERSION 2.6)

project (dmlite-plugins-hdfs)

# Version
set(dmlite_plugins_hdfs_MAJOR 0)
set(dmlite_plugins_hdfs_MINOR 6)
set(dmlite_plugins_hdfs_PATCH 0)

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

# Configuration files
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/etc/hdfs-io.conf.in
                ${CMAKE_CURRENT_BINARY_DIR}/etc/hdfs-io.conf
                @ONLY)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/etc/hdfs-pool.conf.in
                ${CMAKE_CURRENT_BINARY_DIR}/etc/hdfs-pool.conf
                @ONLY)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/etc/hdfs-ns.conf.in
                ${CMAKE_CURRENT_BINARY_DIR}/etc/hdfs-ns.conf
                @ONLY)



install (FILES       ${CMAKE_CURRENT_BINARY_DIR}/etc/hdfs-io.conf
                     ${CMAKE_CURRENT_BINARY_DIR}/etc/hdfs-pool.conf 
		     ${CMAKE_CURRENT_BINARY_DIR}/etc/hdfs-ns.conf
         DESTINATION etc/dmlite.conf.d)

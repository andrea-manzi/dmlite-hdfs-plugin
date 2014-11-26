cmake_minimum_required (VERSION 2.6)

# -------------------------------------------------------
# Detect if an environement variable exists for JAVA_HOME
# Set it to the alternatives java sdk otherwise
# -------------------------------------------------------
if(NOT ENV{JAVA_HOME})
  SET(ENV{JAVA_HOME} "/etc/alternatives/java_sdk")
endif(NOT ENV{JAVA_HOME})

# ----------------------------------------------------
# libjvm.so, libhdfs.so and their associated headers
# are required for this build, this two package finder
# set the cmake variable correctly.
# ----------------------------------------------------
set (BOOST_INCLUDEDIR "/usr/include/boost141") # EPEL5
find_package(Boost 1.41 REQUIRED)
find_package(Hadoop REQUIRED)
find_package(JNI    REQUIRED)
find_package(DMLite REQUIRED)

# ------------------------------------------
# Hadoop module compilation and installation
# ------------------------------------------
include_directories(${Boost_INCLUDE_DIR} ${HDFS_INCLUDE_DIR} ${JNI_INCLUDE_DIRS} ${DMLITE_INCLUDE_DIR})

add_library(hdfs SHARED Hdfs.cpp
                        HdfsIO.cpp
			HdfsNS.cpp
                        HdfsPool.cpp
			HdfsUtil.cpp
			HdfsAuthn.cpp
			Throw.cpp)

target_link_libraries (hdfs dl ${DMLITE_LIBRARIES} ${HDFS_LIBRARIES})
set_target_properties (hdfs PROPERTIES PREFIX "plugin_")

install(TARGETS       hdfs
        LIBRARY       DESTINATION usr/lib${LIB_SUFFIX}/dmlite/)

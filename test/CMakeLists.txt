cmake_minimum_required (VERSION 2.6)

# -------------------------------------------------------
# Detect if an environement variable exists for JAVA_HOME
# Set it to the alternatives java sdk otherwise
# -------------------------------------------------------
if(NOT ENV{JAVA_HOME})
  SET(ENV{JAVA_HOME} "/etc/alternatives/java_sdk")
endif(NOT ENV{JAVA_HOME})


set (BOOST_INCLUDEDIR "/usr/include/boost141") # EPEL5
find_package(Boost 1.41 REQUIRED)
find_package(Hadoop REQUIRED)
find_package(JNI    REQUIRED)
find_package(DMLite REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${HDFS_INCLUDE_DIR} ${JNI_INCLUDE_DIRS} ${DMLITE_INCLUDE_DIR} )


configure_file (${CMAKE_CURRENT_SOURCE_DIR}/hdfs.conf
                ${CMAKE_CURRENT_BINARY_DIR}/hdfs.conf)

add_executable        (test-hdfs test-hdfs.cpp)
target_link_libraries (test-hdfs dl ${DMLITE_LIBRARIES} ${JNI_INCLUDE_LIBRARIES} ${HDFS_LIBRARIES})

#add_executable        (test-hdfs-io test-hdfs-io.cpp)
#target_link_libraries (test-hdfs-io ${DMLITE_LIBRARIES}  ${HDFS_LIBRARIES})


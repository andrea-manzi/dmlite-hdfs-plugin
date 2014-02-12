cmake_minimum_required (VERSION 2.6)

project (dmlite)

# Version
set(dmlite_MAJOR 0)
set(dmlite_MINOR 3)
set(dmlite_PATCH 1)

# Host certificate location
set (HOST_CERTIFICATE "/etc/grid-security/hostcert.pem" CACHE STRING "Location of the host certificate")

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

# Include path
include_directories (${CMAKE_SOURCE_DIR}/include/)

# Definitions
add_definitions (-D_REENTRANT)

# Subdirectories
add_subdirectory (etc)
add_subdirectory (utils)
add_subdirectory (core)
add_subdirectory (plugins)
add_subdirectory (tests)

# Config header
configure_file (${CMAKE_SOURCE_DIR}/include/dmlite/common/dm_config.h.cmake
                ${CMAKE_SOURCE_DIR}/include/dmlite/common/dm_config.h)

# Install headers
install (DIRECTORY   include/
         DESTINATION usr/include
         COMPONENT   dmlite-devel
         PATTERN     ".svn" EXCLUDE
         PATTERN     ".cmake" EXCLUDE)

# Generate documentation
find_package (Doxygen)

if (DOXYGEN_FOUND)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-man.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-man @ONLY)

  add_custom_target(doc ALL
                    DEPENDS doc/html doc/man)

  add_custom_command(OUTPUT doc/html
                     COMMAND ${DOXYGEN_EXECUTABLE} ARGS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                     COMMENT "Generating API documentation with Doxygen" VERBATIM)

  add_custom_command(OUTPUT doc/man
                     COMMAND ${DOXYGEN_EXECUTABLE} ARGS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-man
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                     COMMENT "Generating man documentation with Doxygen" VERBATIM)


  install (DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/doc/html/
           DESTINATION usr/share/doc/${PROJECT_NAME}-${dmlite_MAJOR}.${dmlite_MINOR}.${dmlite_PATCH}/api
           COMPONENT   dmlite-devel)
  install (DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/doc/man/
           DESTINATION usr/share/man
           COMPONENT   dmlite-devel)

endif (DOXYGEN_FOUND)

# Configure CTest testing
ENABLE_TESTING()

cmake_minimum_required (VERSION 2.6)

# NOTE: Functionality is actually tested by the CPP tests
#       These here are intended to check if the mapping between C and CPP is
#       working as intended, so a mock plugin is used.

add_library           (mock SHARED plugin_mock.cpp plugin_mock.h)
set_target_properties (mock PROPERTIES PREFIX "plugin_")
target_link_libraries (mock dmlite)

add_library           (utils STATIC utils.c utils.h)

add_executable        (test-any-c test-any.c)
target_link_libraries (test-any-c dmlite utils)

add_executable        (test-catalog-c test-catalog.c)
target_link_libraries (test-catalog-c dmlite utils)

add_executable        (test-converters test-converters.cpp)
target_link_libraries (test-converters dmlite utils)

add_executable        (test-io-c test-io.c)
target_link_libraries (test-io-c dmlite utils)

add_executable        (test-manager-c test-manager.c)
target_link_libraries (test-manager-c dmlite utils)

add_executable        (test-pool-c test-pool.c)
target_link_libraries (test-pool-c dmlite utils)

# Install
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
         DESTINATION /usr/share/tests/dmlite/c
         FILES_MATCHING PATTERN test-*-c
                        PATTERN plugin_mock.so
                        PATTERN *.o   EXCLUDE
                        PATTERN *.dir EXCLUDE
                        PATTERN .svn  EXCLUDE
                        PATTERN CMakeFiles EXCLUDE)

# Add CTest tests
ENABLE_TESTING()

ADD_TEST(test-any        ${CMAKE_CURRENT_BINARY_DIR}/test-any-c)
ADD_TEST(test-catalog    ${CMAKE_CURRENT_BINARY_DIR}/test-catalog-c)
ADD_TEST(test-converters ${CMAKE_CURRENT_BINARY_DIR}/test-converters)
ADD_TEST(test-io         ${CMAKE_CURRENT_BINARY_DIR}/test-io-c)
ADD_TEST(test-manager    ${CMAKE_CURRENT_BINARY_DIR}/test-manager-c)
ADD_TEST(test-pool       ${CMAKE_CURRENT_BINARY_DIR}/test-pool-c)

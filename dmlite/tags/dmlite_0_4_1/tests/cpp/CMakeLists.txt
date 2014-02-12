cmake_minimum_required (VERSION 2.6)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/NsMySql.conf
                ${CMAKE_CURRENT_BINARY_DIR}/NsMySql.conf)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/NsOracle.conf
                ${CMAKE_CURRENT_BINARY_DIR}/NsOracle.conf)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/MemcacheNsMySql.conf
                ${CMAKE_CURRENT_BINARY_DIR}/MemcacheNsMySql.conf)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/NsAdapter.conf
                ${CMAKE_CURRENT_BINARY_DIR}/NsAdapter.conf)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/DpmAdapter.conf
                ${CMAKE_CURRENT_BINARY_DIR}/DpmAdapter.conf)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/DpmMySql.conf
                ${CMAKE_CURRENT_BINARY_DIR}/DpmMySql.conf)

add_executable        (init-backend init-backend.cpp)
target_link_libraries (init-backend dmlite dl)

add_library           (test-base STATIC test-base.cpp)

add_executable        (test-acls test-acls.cpp)
target_link_libraries (test-acls test-base dmlite cppunit dl)

add_executable        (test-authn test-authn.cpp)
target_link_libraries (test-authn test-base dmlite cppunit dl)

add_executable        (test-chdir test-chdir.cpp)
target_link_libraries (test-chdir test-base dmlite cppunit dl)

add_executable        (test-checkperm test-checkperm.cpp)
target_link_libraries (test-checkperm dmlite cppunit dl)

add_executable        (test-checksum test-checksum.cpp)
target_link_libraries (test-checksum test-base dmlite cppunit dl)

add_executable        (test-chown test-chown.cpp)
target_link_libraries (test-chown test-base dmlite cppunit dl)

add_executable        (test-comment test-comment.cpp)
target_link_libraries (test-comment test-base dmlite cppunit dl)

add_executable        (test-create test-create.cpp)
target_link_libraries (test-create test-base dmlite cppunit dl)

add_executable        (test-extensible test-extensible.cpp)
target_link_libraries (test-extensible dmlite cppunit)

add_executable        (test-guid test-guid.cpp)
target_link_libraries (test-guid test-base dmlite cppunit dl)

add_executable        (test-io test-io.cpp)
target_link_libraries (test-io dmlite cppunit dl)

add_executable        (test-opendir test-opendir.cpp)
target_link_libraries (test-opendir test-base dmlite cppunit dl)

add_executable        (test-poolcontainer test-poolcontainer.cpp)
target_link_libraries (test-poolcontainer dmlite cppunit pthread)

add_executable        (test-pools test-pools.cpp)
target_link_libraries (test-pools test-base dmlite cppunit dl)

add_executable        (test-rename test-rename.cpp)
target_link_libraries (test-rename test-base dmlite cppunit dl)

add_executable        (test-replicas test-replicas.cpp)
target_link_libraries (test-replicas test-base dmlite cppunit dl)

add_executable        (test-security test-security.cpp)
target_link_libraries (test-security test-base dmlite cppunit dl)

add_executable        (test-split_path test-split_path.cpp)
target_link_libraries (test-split_path dmlite dl)

add_executable        (test-split_url test-split_url.cpp)
target_link_libraries (test-split_url dmlite dl)

add_executable        (test-stat test-stat.cpp)
target_link_libraries (test-stat test-base dmlite cppunit dl)

add_executable        (test-symlink test-symlink.cpp)
target_link_libraries (test-symlink test-base dmlite cppunit dl)

add_executable        (test-utime test-utime.cpp)
target_link_libraries (test-utime test-base dmlite cppunit dl)

add_executable        (test-xattr test-xattr.cpp)
target_link_libraries (test-xattr test-base dmlite cppunit dl)

# Install
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
         DESTINATION /usr/share/tests/dmlite/cpp
         FILES_MATCHING PATTERN test-*
                        PATTERN init-*
                        PATTERN *.o   EXCLUDE
                        PATTERN *.cpp EXCLUDE
                        PATTERN *.h   EXCLUDE
                        PATTERN *.dir EXCLUDE
                        PATTERN .svn  EXCLUDE
                        PATTERN CMakeFiles EXCLUDE)

# Configure CTest testing
ENABLE_TESTING()

# Set Test Parameters
#set (CONFIG MemcacheNsMySql.conf)
set (CONFIG   NsMySql.conf)
set (FSCONFIG DpmMySql.conf)

# Add CTest tests
ADD_TEST(test-acls          ${CMAKE_CURRENT_BINARY_DIR}/test-acls ${CONFIG})
ADD_TEST(test-authn         ${CMAKE_CURRENT_BINARY_DIR}/test-authn ${CONFIG})
ADD_TEST(test-chdir         ${CMAKE_CURRENT_BINARY_DIR}/test-chdir ${CONFIG})
ADD_TEST(test-checkperm     ${CMAKE_CURRENT_BINARY_DIR}/test-checkperm ${CONFIG})
ADD_TEST(test-checksum      ${CMAKE_CURRENT_BINARY_DIR}/test-checksum ${CONFIG})
ADD_TEST(test-chown         ${CMAKE_CURRENT_BINARY_DIR}/test-chown ${CONFIG})
ADD_TEST(test-comment       ${CMAKE_CURRENT_BINARY_DIR}/test-comment ${CONFIG})
ADD_TEST(test-create        ${CMAKE_CURRENT_BINARY_DIR}/test-create ${CONFIG})
ADD_TEST(test-extensible    ${CMAKE_CURRENT_BINARY_DIR}/test-extensible ${CONFIG})
ADD_TEST(test-guid          ${CMAKE_CURRENT_BINARY_DIR}/test-guid ${CONFIG})
ADD_TEST(test-io            ${CMAKE_CURRENT_BINARY_DIR}/test-io ${FSCONFIG})
ADD_TEST(test-opendir       ${CMAKE_CURRENT_BINARY_DIR}/test-opendir ${CONFIG})
ADD_TEST(test-pools         ${CMAKE_CURRENT_BINARY_DIR}/test-pools ${FSCONFIG})
ADD_TEST(test-poolcontainer ${CMAKE_CURRENT_BINARY_DIR}/test-poolcontainer)
ADD_TEST(test-rename        ${CMAKE_CURRENT_BINARY_DIR}/test-rename ${CONFIG})
ADD_TEST(test-replicas      ${CMAKE_CURRENT_BINARY_DIR}/test-replicas ${CONFIG})
ADD_TEST(test-security      ${CMAKE_CURRENT_BINARY_DIR}/test-security ${CONFIG})
ADD_TEST(test-split_path    ${CMAKE_CURRENT_BINARY_DIR}/test-split_path ${CONFIG})
ADD_TEST(test-split_url     ${CMAKE_CURRENT_BINARY_DIR}/test-split_url ${CONFIG})
ADD_TEST(test-stat          ${CMAKE_CURRENT_BINARY_DIR}/test-stat ${CONFIG})
ADD_TEST(test-symlink       ${CMAKE_CURRENT_BINARY_DIR}/test-symlink ${CONFIG})
ADD_TEST(test-utime         ${CMAKE_CURRENT_BINARY_DIR}/test-utime ${CONFIG})
ADD_TEST(test-xattr         ${CMAKE_CURRENT_BINARY_DIR}/test-xattr ${CONFIG})

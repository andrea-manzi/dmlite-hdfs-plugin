cmake_minimum_required (VERSION 2.6)

add_subdirectory (c)
add_subdirectory (core)
add_subdirectory (utils)

add_library(dmlite SHARED ${DMLITE_CAPI_SOURCES}
                          ${DMLITE_CORE_SOURCES}
                          ${DMLITE_UTILS_SOURCES})

target_link_libraries (dmlite boost_regex crypto dl pthread ssl)

set_target_properties(dmlite PROPERTIES
                             VERSION ${dmlite_MAJOR}.${dmlite_MINOR}.${dmlite_PATCH}
                             SOVERSION ${dmlite_MAJOR})

install(TARGETS dmlite
        LIBRARY DESTINATION usr/lib${LIB_SUFFIX})

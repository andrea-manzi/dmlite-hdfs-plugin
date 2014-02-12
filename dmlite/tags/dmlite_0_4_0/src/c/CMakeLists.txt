cmake_minimum_required (VERSION 2.6)

set(DMLITE_CAPI_SOURCES c/Any-C.cpp
                        c/Catalog-C.cpp
                        c/Inode-C.cpp
                        c/IO-C.cpp
                        c/PluginManager-C.cpp
                        c/Pool-C.cpp
                        c/Utils-C.cpp
    PARENT_SCOPE)

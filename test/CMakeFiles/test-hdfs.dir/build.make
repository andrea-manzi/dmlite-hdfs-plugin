# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/dmlite-plugins-hdfs/trunk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/dmlite-plugins-hdfs/trunk

# Include any dependencies generated for this target.
include test/CMakeFiles/test-hdfs.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test-hdfs.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test-hdfs.dir/flags.make

test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o: test/CMakeFiles/test-hdfs.dir/flags.make
test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o: test/test-hdfs.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /root/dmlite-plugins-hdfs/trunk/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o"
	cd /root/dmlite-plugins-hdfs/trunk/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o -c /root/dmlite-plugins-hdfs/trunk/test/test-hdfs.cpp

test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-hdfs.dir/test-hdfs.cpp.i"
	cd /root/dmlite-plugins-hdfs/trunk/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /root/dmlite-plugins-hdfs/trunk/test/test-hdfs.cpp > CMakeFiles/test-hdfs.dir/test-hdfs.cpp.i

test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-hdfs.dir/test-hdfs.cpp.s"
	cd /root/dmlite-plugins-hdfs/trunk/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /root/dmlite-plugins-hdfs/trunk/test/test-hdfs.cpp -o CMakeFiles/test-hdfs.dir/test-hdfs.cpp.s

test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.requires:
.PHONY : test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.requires

test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.provides: test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/test-hdfs.dir/build.make test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.provides.build
.PHONY : test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.provides

test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.provides.build: test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o

# Object files for target test-hdfs
test__hdfs_OBJECTS = \
"CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o"

# External object files for target test-hdfs
test__hdfs_EXTERNAL_OBJECTS =

test/test-hdfs: test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o
test/test-hdfs: test/CMakeFiles/test-hdfs.dir/build.make
test/test-hdfs: /usr/lib64/libdmlite.so
test/test-hdfs: /usr/lib64/libhdfs.so
test/test-hdfs: test/CMakeFiles/test-hdfs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test-hdfs"
	cd /root/dmlite-plugins-hdfs/trunk/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-hdfs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test-hdfs.dir/build: test/test-hdfs
.PHONY : test/CMakeFiles/test-hdfs.dir/build

test/CMakeFiles/test-hdfs.dir/requires: test/CMakeFiles/test-hdfs.dir/test-hdfs.cpp.o.requires
.PHONY : test/CMakeFiles/test-hdfs.dir/requires

test/CMakeFiles/test-hdfs.dir/clean:
	cd /root/dmlite-plugins-hdfs/trunk/test && $(CMAKE_COMMAND) -P CMakeFiles/test-hdfs.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test-hdfs.dir/clean

test/CMakeFiles/test-hdfs.dir/depend:
	cd /root/dmlite-plugins-hdfs/trunk && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/dmlite-plugins-hdfs/trunk /root/dmlite-plugins-hdfs/trunk/test /root/dmlite-plugins-hdfs/trunk /root/dmlite-plugins-hdfs/trunk/test /root/dmlite-plugins-hdfs/trunk/test/CMakeFiles/test-hdfs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test-hdfs.dir/depend

# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.16.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.16.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vectord/GitHubSL/JSON/chapter6

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vectord/GitHubSL/JSON/chapter6/build

# Include any dependencies generated for this target.
include CMakeFiles/leptjson_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/leptjson_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/leptjson_test.dir/flags.make

CMakeFiles/leptjson_test.dir/test.c.o: CMakeFiles/leptjson_test.dir/flags.make
CMakeFiles/leptjson_test.dir/test.c.o: ../test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/vectord/GitHubSL/JSON/chapter6/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/leptjson_test.dir/test.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/leptjson_test.dir/test.c.o   -c /Users/vectord/GitHubSL/JSON/chapter6/test.c

CMakeFiles/leptjson_test.dir/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/leptjson_test.dir/test.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/vectord/GitHubSL/JSON/chapter6/test.c > CMakeFiles/leptjson_test.dir/test.c.i

CMakeFiles/leptjson_test.dir/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/leptjson_test.dir/test.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/vectord/GitHubSL/JSON/chapter6/test.c -o CMakeFiles/leptjson_test.dir/test.c.s

# Object files for target leptjson_test
leptjson_test_OBJECTS = \
"CMakeFiles/leptjson_test.dir/test.c.o"

# External object files for target leptjson_test
leptjson_test_EXTERNAL_OBJECTS =

leptjson_test: CMakeFiles/leptjson_test.dir/test.c.o
leptjson_test: CMakeFiles/leptjson_test.dir/build.make
leptjson_test: libleptjson.a
leptjson_test: CMakeFiles/leptjson_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/vectord/GitHubSL/JSON/chapter6/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable leptjson_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/leptjson_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/leptjson_test.dir/build: leptjson_test

.PHONY : CMakeFiles/leptjson_test.dir/build

CMakeFiles/leptjson_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/leptjson_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/leptjson_test.dir/clean

CMakeFiles/leptjson_test.dir/depend:
	cd /Users/vectord/GitHubSL/JSON/chapter6/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vectord/GitHubSL/JSON/chapter6 /Users/vectord/GitHubSL/JSON/chapter6 /Users/vectord/GitHubSL/JSON/chapter6/build /Users/vectord/GitHubSL/JSON/chapter6/build /Users/vectord/GitHubSL/JSON/chapter6/build/CMakeFiles/leptjson_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/leptjson_test.dir/depend


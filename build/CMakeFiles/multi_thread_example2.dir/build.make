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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/feng_shihao/fsh_file/rt_pthread_tutorial

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/feng_shihao/fsh_file/rt_pthread_tutorial/build

# Include any dependencies generated for this target.
include CMakeFiles/multi_thread_example2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/multi_thread_example2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/multi_thread_example2.dir/flags.make

CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.o: CMakeFiles/multi_thread_example2.dir/flags.make
CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.o: ../src/multi_thread_example2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/feng_shihao/fsh_file/rt_pthread_tutorial/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.o -c /home/feng_shihao/fsh_file/rt_pthread_tutorial/src/multi_thread_example2.cpp

CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/feng_shihao/fsh_file/rt_pthread_tutorial/src/multi_thread_example2.cpp > CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.i

CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/feng_shihao/fsh_file/rt_pthread_tutorial/src/multi_thread_example2.cpp -o CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.s

# Object files for target multi_thread_example2
multi_thread_example2_OBJECTS = \
"CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.o"

# External object files for target multi_thread_example2
multi_thread_example2_EXTERNAL_OBJECTS =

multi_thread_example2: CMakeFiles/multi_thread_example2.dir/src/multi_thread_example2.cpp.o
multi_thread_example2: CMakeFiles/multi_thread_example2.dir/build.make
multi_thread_example2: CMakeFiles/multi_thread_example2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/feng_shihao/fsh_file/rt_pthread_tutorial/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable multi_thread_example2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/multi_thread_example2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/multi_thread_example2.dir/build: multi_thread_example2

.PHONY : CMakeFiles/multi_thread_example2.dir/build

CMakeFiles/multi_thread_example2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/multi_thread_example2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/multi_thread_example2.dir/clean

CMakeFiles/multi_thread_example2.dir/depend:
	cd /home/feng_shihao/fsh_file/rt_pthread_tutorial/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/feng_shihao/fsh_file/rt_pthread_tutorial /home/feng_shihao/fsh_file/rt_pthread_tutorial /home/feng_shihao/fsh_file/rt_pthread_tutorial/build /home/feng_shihao/fsh_file/rt_pthread_tutorial/build /home/feng_shihao/fsh_file/rt_pthread_tutorial/build/CMakeFiles/multi_thread_example2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/multi_thread_example2.dir/depend


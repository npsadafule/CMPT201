# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cmpt201/units/02-tools/a4-npsadafule/cmake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cmpt201/units/02-tools/a4-npsadafule/cmake/build

# Include any dependencies generated for this target.
include CMakeFiles/random.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/random.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/random.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/random.dir/flags.make

CMakeFiles/random.dir/src/main.c.o: CMakeFiles/random.dir/flags.make
CMakeFiles/random.dir/src/main.c.o: /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/main.c
CMakeFiles/random.dir/src/main.c.o: CMakeFiles/random.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/cmpt201/units/02-tools/a4-npsadafule/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/random.dir/src/main.c.o"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/random.dir/src/main.c.o -MF CMakeFiles/random.dir/src/main.c.o.d -o CMakeFiles/random.dir/src/main.c.o -c /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/main.c

CMakeFiles/random.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/random.dir/src/main.c.i"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/main.c > CMakeFiles/random.dir/src/main.c.i

CMakeFiles/random.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/random.dir/src/main.c.s"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/main.c -o CMakeFiles/random.dir/src/main.c.s

CMakeFiles/random.dir/src/random_range.c.o: CMakeFiles/random.dir/flags.make
CMakeFiles/random.dir/src/random_range.c.o: /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/random_range.c
CMakeFiles/random.dir/src/random_range.c.o: CMakeFiles/random.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/cmpt201/units/02-tools/a4-npsadafule/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/random.dir/src/random_range.c.o"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/random.dir/src/random_range.c.o -MF CMakeFiles/random.dir/src/random_range.c.o.d -o CMakeFiles/random.dir/src/random_range.c.o -c /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/random_range.c

CMakeFiles/random.dir/src/random_range.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/random.dir/src/random_range.c.i"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/random_range.c > CMakeFiles/random.dir/src/random_range.c.i

CMakeFiles/random.dir/src/random_range.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/random.dir/src/random_range.c.s"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/cmpt201/units/02-tools/a4-npsadafule/cmake/src/random_range.c -o CMakeFiles/random.dir/src/random_range.c.s

# Object files for target random
random_OBJECTS = \
"CMakeFiles/random.dir/src/main.c.o" \
"CMakeFiles/random.dir/src/random_range.c.o"

# External object files for target random
random_EXTERNAL_OBJECTS =

random: CMakeFiles/random.dir/src/main.c.o
random: CMakeFiles/random.dir/src/random_range.c.o
random: CMakeFiles/random.dir/build.make
random: CMakeFiles/random.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/cmpt201/units/02-tools/a4-npsadafule/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable random"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/random.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/random.dir/build: random
.PHONY : CMakeFiles/random.dir/build

CMakeFiles/random.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/random.dir/cmake_clean.cmake
.PHONY : CMakeFiles/random.dir/clean

CMakeFiles/random.dir/depend:
	cd /home/cmpt201/units/02-tools/a4-npsadafule/cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cmpt201/units/02-tools/a4-npsadafule/cmake /home/cmpt201/units/02-tools/a4-npsadafule/cmake /home/cmpt201/units/02-tools/a4-npsadafule/cmake/build /home/cmpt201/units/02-tools/a4-npsadafule/cmake/build /home/cmpt201/units/02-tools/a4-npsadafule/cmake/build/CMakeFiles/random.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/random.dir/depend

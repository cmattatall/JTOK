# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Adam\Desktop\JTOK

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Adam\Desktop\JTOK\build

# Include any dependencies generated for this target.
include tests/CMakeFiles/negativeJsonParsingTest.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/negativeJsonParsingTest.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/negativeJsonParsingTest.dir/flags.make

tests/CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.obj: tests/CMakeFiles/negativeJsonParsingTest.dir/flags.make
tests/CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.obj: tests/CMakeFiles/negativeJsonParsingTest.dir/includes_C.rsp
tests/CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.obj: ../tests/negativeParseTest.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Adam\Desktop\JTOK\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.obj"
	cd /d C:\Users\Adam\Desktop\JTOK\build\tests && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\negativeJsonParsingTest.dir\negativeParseTest.c.obj -c C:\Users\Adam\Desktop\JTOK\tests\negativeParseTest.c

tests/CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.i"
	cd /d C:\Users\Adam\Desktop\JTOK\build\tests && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Adam\Desktop\JTOK\tests\negativeParseTest.c > CMakeFiles\negativeJsonParsingTest.dir\negativeParseTest.c.i

tests/CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.s"
	cd /d C:\Users\Adam\Desktop\JTOK\build\tests && C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\Adam\Desktop\JTOK\tests\negativeParseTest.c -o CMakeFiles\negativeJsonParsingTest.dir\negativeParseTest.c.s

# Object files for target negativeJsonParsingTest
negativeJsonParsingTest_OBJECTS = \
"CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.obj"

# External object files for target negativeJsonParsingTest
negativeJsonParsingTest_EXTERNAL_OBJECTS =

tests/negativeJsonParsingTest.exe: tests/CMakeFiles/negativeJsonParsingTest.dir/negativeParseTest.c.obj
tests/negativeJsonParsingTest.exe: tests/CMakeFiles/negativeJsonParsingTest.dir/build.make
tests/negativeJsonParsingTest.exe: libJTOK.a
tests/negativeJsonParsingTest.exe: tests/CMakeFiles/negativeJsonParsingTest.dir/linklibs.rsp
tests/negativeJsonParsingTest.exe: tests/CMakeFiles/negativeJsonParsingTest.dir/objects1.rsp
tests/negativeJsonParsingTest.exe: tests/CMakeFiles/negativeJsonParsingTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Adam\Desktop\JTOK\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable negativeJsonParsingTest.exe"
	cd /d C:\Users\Adam\Desktop\JTOK\build\tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\negativeJsonParsingTest.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/negativeJsonParsingTest.dir/build: tests/negativeJsonParsingTest.exe

.PHONY : tests/CMakeFiles/negativeJsonParsingTest.dir/build

tests/CMakeFiles/negativeJsonParsingTest.dir/clean:
	cd /d C:\Users\Adam\Desktop\JTOK\build\tests && $(CMAKE_COMMAND) -P CMakeFiles\negativeJsonParsingTest.dir\cmake_clean.cmake
.PHONY : tests/CMakeFiles/negativeJsonParsingTest.dir/clean

tests/CMakeFiles/negativeJsonParsingTest.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Adam\Desktop\JTOK C:\Users\Adam\Desktop\JTOK\tests C:\Users\Adam\Desktop\JTOK\build C:\Users\Adam\Desktop\JTOK\build\tests C:\Users\Adam\Desktop\JTOK\build\tests\CMakeFiles\negativeJsonParsingTest.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/negativeJsonParsingTest.dir/depend

# CMake generated Testfile for 
# Source directory: C:/Users/Adam/Desktop/JTOK/tests
# Build directory: C:/Users/Adam/Desktop/JTOK/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(positiveJsonParsingTest "C:/Users/Adam/Desktop/JTOK/build/tests/positiveJsonParsingTest.exe" "--build-generator" "MinGW Makefiles" "--test-command" "C:/Program Files/CMake/bin/ctest.exe")
set_tests_properties(positiveJsonParsingTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Adam/Desktop/JTOK/tests/CMakeLists.txt;7;add_test;C:/Users/Adam/Desktop/JTOK/tests/CMakeLists.txt;0;")
add_test(negativeJsonParsingTest "C:/Users/Adam/Desktop/JTOK/build/tests/negativeJsonParsingTest.exe" "--build-generator" "MinGW Makefiles" "--test-command" "C:/Program Files/CMake/bin/ctest.exe")
set_tests_properties(negativeJsonParsingTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Adam/Desktop/JTOK/tests/CMakeLists.txt;18;add_test;C:/Users/Adam/Desktop/JTOK/tests/CMakeLists.txt;0;")

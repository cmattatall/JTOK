# CMake generated Testfile for 
# Source directory: /home/carl/workspace_v10/ADCS_Firmware/JTOK/tests
# Build directory: /home/carl/workspace_v10/ADCS_Firmware/JTOK/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(positiveJsonParsingTest "/home/carl/workspace_v10/ADCS_Firmware/JTOK/build/tests/positiveJsonParsingTest" "--build-generator" "Unix Makefiles" "--test-command" "/usr/local/bin/ctest")
set_tests_properties(positiveJsonParsingTest PROPERTIES  _BACKTRACE_TRIPLES "/home/carl/workspace_v10/ADCS_Firmware/JTOK/tests/CMakeLists.txt;7;add_test;/home/carl/workspace_v10/ADCS_Firmware/JTOK/tests/CMakeLists.txt;0;")
add_test(negativeJsonParsingTest "/home/carl/workspace_v10/ADCS_Firmware/JTOK/build/tests/negativeJsonParsingTest" "--build-generator" "Unix Makefiles" "--test-command" "/usr/local/bin/ctest")
set_tests_properties(negativeJsonParsingTest PROPERTIES  _BACKTRACE_TRIPLES "/home/carl/workspace_v10/ADCS_Firmware/JTOK/tests/CMakeLists.txt;18;add_test;/home/carl/workspace_v10/ADCS_Firmware/JTOK/tests/CMakeLists.txt;0;")

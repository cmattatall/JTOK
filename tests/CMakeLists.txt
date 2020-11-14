cmake_minimum_required(VERSION 3.16)

add_test(NAME TestName COMMAND TargetName)

add_test(
  NAME
    positiveJsonParsingTest
  COMMAND
    "${CMAKE_CTEST_COMMAND}"
             --build-and-test "${My_SOURCE_DIR}/examples/simple"
                              "${CMAKE_CURRENT_BINARY_DIR}/simple"
             --build-generator "${CMAKE_GENERATOR}"
             --test-command "${CMAKE_CTEST_COMMAND}"
)
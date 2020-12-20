cmake_minimum_required(VERSION 3.16)
project(
    JTOK 
    VERSION 1.0 
    DESCRIPTION "STATICALLY ALLOCATED JSON LIBRARY FOR EMBEDDED SYSTEMS. LOOSELY BASED ON JSMN"
    LANGUAGES C
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # vscode

add_library(JTOK STATIC)
target_include_directories(JTOK PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_sources(JTOK PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/jtok.c")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
    option(PACKAGE_TESTS "Build the tests" ON)
endif()



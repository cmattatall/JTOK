cmake_minimum_required(VERSION 3.16)

project(JTOK VERSION 1.0 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # vscode


add_library(JTOK STATIC)


target_include_directories(JTOK PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_sources(JTOK PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/jtok.c")

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
    option(PACKAGE_TESTS "Build the tests" ON)
    if(PACKAGE_TESTS)
        enable_testing()
        include(GoogleTest)
        add_subdirectory(tests)
    endif()

endif()

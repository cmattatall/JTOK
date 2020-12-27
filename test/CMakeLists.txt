# TEST CREATION SCRIPT
# ALL C FILES IN THIS DIRECTORY WILL BE ADDED TO THE TEST SUITE
# 
# THUS, A TEST SHOULD BE SIMPLE, SINGLE SOURCE FILE with a mainline
# intended to test a very specific feature
cmake_minimum_required(VERSION 3.16)
file(GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")


file(GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
foreach(src ${test_sources})
    get_filename_component(test_suffix ${src} NAME_WLE)
    set(test_target "${LIB}_${test_suffix}")
    if(NOT TARGET ${test_target})
        add_executable(${test_target})
        target_sources(${test_target} PRIVATE ${src})
        if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
            target_compile_options(${test_target} PRIVATE "-Wall")
            target_compile_options(${test_target} PRIVATE "-Wshadow")
        endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

        target_link_libraries(${test_target} PRIVATE ${LIB})
        target_link_libraries(${test_target} PRIVATE JTOK)

        if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
            get_filename_component(${LIB}_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
        else()
            set(${LIB}_TEST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})        
        endif(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)

        add_test(
            NAME ${test_target}
            COMMAND valgrind ${${LIB}_TEST_DIR}/${test_target}
            --build-generator "${CMAKE_GENERATOR}"
            --test-command "${CMAKE_CTEST_COMMAND}"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        ) 
    endif(NOT TARGET ${test_target})
    unset(${LIB}_TEST_DIR)
    unset(test_target)
endforeach(src ${test_sources})
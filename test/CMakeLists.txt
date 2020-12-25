# TEST CREATION SCRIPT
# ALL C FILES IN THIS DIRECTORY WILL BE ADDED TO THE TEST SUITE
# 
# THUS, A TEST SHOULD BE SIMPLE, SINGLE SOURCE FILE with a mainline
# intended to test a very specific feature
cmake_minimum_required(VERSION 3.16)
file(GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")


## ALL other .c files that haven't been added as tests will be added now
foreach(src ${test_sources})
    get_filename_component(test_suffix ${src} NAME_WLE)
    set(test_target "${LIB}_${test_suffix}")
    if(NOT TARGET ${test_target})
        add_executable(${test_target})
        target_sources(${test_target} PRIVATE ${src})
        target_link_libraries(${test_target} PRIVATE ${LIB})
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
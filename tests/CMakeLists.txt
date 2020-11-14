cmake_minimum_required(VERSION 3.16)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    add_executable(positiveJsonParsingTest)
    target_sources(positiveJsonParsingTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/positiveParseTest.c")
    target_link_libraries(positiveJsonParsingTest PUBLIC JTOK)

    add_test(NAME positiveJsonParsingTest 
            COMMAND positiveJsonParsingTest
            --build-generator "${CMAKE_GENERATOR}"
            --test-command "${CMAKE_CTEST_COMMAND}"
    )


endif()







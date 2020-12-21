cmake_minimum_required(VERSION 3.16)

option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)

project(
    JTOK
    VERSION 1.0 
    DESCRIPTION "STATICALLY ALLOCATED JSON LIBRARY FOR EMBEDDED SYSTEMS. LOOSELY BASED ON JSMN"
    LANGUAGES C
)

set(LIB "${PROJECT_NAME}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # vscode

add_library(${LIB} STATIC)
target_include_directories(${LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_sources(${LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/jtok.c")
target_compile_options(${LIB} PRIVATE "-Wall")

if(BUILD_TESTING)
    enable_testing()
    include(CTest)
    add_subdirectory(test)
else()
    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        enable_testing()
        include(CTest)
        add_subdirectory(test)
    endif()
endif()




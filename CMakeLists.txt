cmake_minimum_required(VERSION 3.16)

################################################################################
#  OPTIONS GO HERE
################################################################################
option(BUILD_TESTING "[ON/OFF] Boolean to choose to cross compile or not" OFF)

project(
    JTOK
    VERSION 1.0
    DESCRIPTION "STATIC JSON LIBRARY FOR EMBEDDED SYSTEMS. BASED ON JSMN"
    LANGUAGES C
)
set(CURRENT_TARGET ${PROJECT_NAME})

################################################################################
# BUILD TYPE STUFF
################################################################################
set(SUPPORTED_BUILD_TYPES "")
list(APPEND SUPPORTED_BUILD_TYPES "Debug")
list(APPEND SUPPORTED_BUILD_TYPES "Release")
list(APPEND SUPPORTED_BUILD_TYPES "MinSizeRel")
list(APPEND SUPPORTED_BUILD_TYPES "RelWithDebInfo")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type chosen by the user at configure time" FORCE)
endif(NOT CMAKE_BUILD_TYPE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${SUPPORTED_BUILD_TYPES})


################################################################################
# DETECT SOURCES RECURSIVELY FROM src FOLDER AND ADD TO BUILD TARGET
################################################################################
set(CURRENT_TARGET "${PROJECT_NAME}") # this is PROJECT_NAME, NOT CMAKE_PROJECT_NAME
message("CONFIGURING TARGET : ${CURRENT_TARGET}")
add_library(${CURRENT_TARGET})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE ${CURRENT_TARGET}_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(${CURRENT_TARGET} PRIVATE ${${CURRENT_TARGET}_sources})


################################################################################
# DETECT PRIVATE HEADERS RECURSIVELY FROM src FOLDER
################################################################################
file(GLOB_RECURSE ${CURRENT_TARGET}_private_headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
set(${CURRENT_TARGET}_private_include_directories "")
foreach(hdr ${${CURRENT_TARGET}_private_headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND ${CURRENT_TARGET}_private_include_directories ${hdr_dir})
endforeach(hdr ${${CURRENT_TARGET}_private_headers})
list(REMOVE_DUPLICATES ${CURRENT_TARGET}_private_include_directories)
target_include_directories(${CURRENT_TARGET} PRIVATE ${${CURRENT_TARGET}_private_include_directories})



################################################################################
# DETECT PUBLIC HEADERS RECURSIVELY FROM inc FOLDER
################################################################################
file(GLOB_RECURSE ${CURRENT_TARGET}_public_headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
set(${CURRENT_TARGET}_public_include_directories "")
foreach(hdr ${${CURRENT_TARGET}_public_headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND ${CURRENT_TARGET}_public_include_directories ${hdr_dir})
endforeach(hdr ${${CURRENT_TARGET}_public_headers})
list(REMOVE_DUPLICATES ${CURRENT_TARGET}_public_include_directories)
target_include_directories(${CURRENT_TARGET} PUBLIC ${${CURRENT_TARGET}_public_include_directories})


################################################################################
# TEST CONFIGURATION
################################################################################
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


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    target_compile_options(${CURRENT_TARGET} PRIVATE "-Wall")
    target_compile_options(${CURRENT_TARGET} PRIVATE "-Wextra")
    target_compile_options(${CURRENT_TARGET} PRIVATE "-Wshadow")
else()
    target_compile_options(${CURRENT_TARGET} PRIVATE "-Wno-unused-function")
endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)


################################################################################
# LINK AGAINST THE NECESSARY LIBRARIES
################################################################################


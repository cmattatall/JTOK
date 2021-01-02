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
set(LIB "${PROJECT_NAME}") # this is PROJECT_NAME, NOT CMAKE_PROJECT_NAME
message("CONFIGURING TARGET : ${LIB}")
add_library(${LIB})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE ${LIB}_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(${LIB} PRIVATE ${${LIB}_sources})


################################################################################
# DETECT PRIVATE HEADERS RECURSIVELY FROM src FOLDER
################################################################################
file(GLOB_RECURSE ${LIB}_private_headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
set(${LIB}_private_include_directories "")
foreach(hdr ${${LIB}_private_headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND ${LIB}_private_include_directories ${hdr_dir})
endforeach(hdr ${${LIB}_private_headers})
list(REMOVE_DUPLICATES ${LIB}_private_include_directories)
target_include_directories(${LIB} PRIVATE ${${LIB}_private_include_directories})



################################################################################
# DETECT PUBLIC HEADERS RECURSIVELY FROM inc FOLDER
################################################################################
file(GLOB_RECURSE ${LIB}_public_headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
set(${LIB}_public_include_directories "")
foreach(hdr ${${LIB}_public_headers})
    get_filename_component(hdr_dir ${hdr} DIRECTORY)
    list(APPEND ${LIB}_public_include_directories ${hdr_dir})
endforeach(hdr ${${LIB}_public_headers})
list(REMOVE_DUPLICATES ${LIB}_public_include_directories)
target_include_directories(${LIB} PUBLIC ${${LIB}_public_include_directories})


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
    target_compile_options(${LIB} PRIVATE "-Wall")
    target_compile_options(${LIB} PRIVATE "-Wextra")
    target_compile_options(${LIB} PRIVATE "-Wshadow")
else()
    target_compile_options(${LIB} PRIVATE "-Wno-unused-function")
endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)


################################################################################
# LINK AGAINST THE NECESSARY LIBRARIES
################################################################################


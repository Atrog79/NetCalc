# Base CMAKE code from the CMAKE class by LT Reece
cmake_minimum_required(VERSION 3.16) # set version

project(
    JQRCalcProjects
    LANGUAGES "C"
    ) # name project

set(SOURCES           # showing how to set multiple source files
    src/simple_main.c
    src/cli_handle.c
   )

option(DEBUG "Debug Macro Definition" OFF) # disabled by default

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("--- SIMPLECALC DEBUG BUILD ---")
    add_definitions(-DDEBUG)
else()
    message("--- SIMPLECALC RELEASE BUILD ---")
endif()

add_compile_options(-Wall -Wextra -Werror -pedantic -O3)  # compiler flags

add_executable(simplecalc ${SOURCES}) # add executable

target_include_directories(simplecalc PRIVATE include/) # include all .h files in include/. must be after add_executable 

target_link_libraries(simplecalc LINK_PUBLIC Common)
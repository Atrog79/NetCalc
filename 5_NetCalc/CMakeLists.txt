# Base CMAKE code from the CMAKE class by LT Reece
cmake_minimum_required(VERSION 3.16) # set version

project(JQRCalcProjects
    LANGUAGES "C"
) # name project

find_program(CLANG_TIDY_PROG clang-tidy)

set(CMAKE_C_COMPILER gcc)
if(CLANG_TIDY_PROG)
    set(CMAKE_C_CLANG_TIDY
        ${CLANG_TIDY_PROG};
        "--checks=* -llvm-include-order, -cppcoreguidelines-*, -readability-magic-numbers, -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling, -hiccp-no-assembler, -pthread")
endif()

set(SOURCES
    src/net_main.c
    src/net_handler.c
    src/net_converts.c
)

include_directories(include ../4_ThreadCalc/include)

option(DEBUG "Debug Macro Definition" OFF) # disabled by default

add_executable(netcalc ${SOURCES})
target_include_directories(netcalc PRIVATE ./include)
target_link_libraries(netcalc LINK_PUBLIC threadpool)
target_link_libraries(netcalc LINK_PUBLIC Common)

target_compile_options(netcalc PUBLIC -pthread) # compiler flags

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("--- NETCALC DEBUG BUILD ---")
    add_definitions(-DDEBUG)
    target_compile_options(netcalc PUBLIC -Wall -pedantic -Werror -Wextra -fsanitize=address)
    target_link_libraries(netcalc LINK_PUBLIC -fsanitize=address)
else()
    message("--- NETCALC RELEASE BUILD ---")
endif()
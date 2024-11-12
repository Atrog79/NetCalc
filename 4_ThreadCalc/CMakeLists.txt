# Base CMAKE code from the CMAKE class by LT Reece
cmake_minimum_required(VERSION 3.16) # set version

project(Thread_calc
    LANGUAGES "C"
) # name project

find_program(CLANG_TIDY_PROG clang-tidy)
set(CMAKE_C_COMPILER gcc)

if(CLANG_TIDY_PROG)
    set(CMAKE_C_CLANG_TIDY
        ${CLANG_TIDY_PROG};
        "--checks=* -llvm-include-order, -cppcoreguidelines-*, -readability-magic-numbers, -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling, -hiccp-no-assembler, -pthread")
endif()

set(SOURCES # showing how to set multiple source files
    src/threadpool.c
    src/queue.c
    src/signals.c
)

option(DEBUG "Debug Macro Definition" OFF) # disabled by default

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("--- THREADCALC DEBUG BUILD ---")
else()
    message("--- THREADCALC RELEASE BUILD ---")
endif()

add_library(threadpool ${SOURCES})
include("tests_src/tests.cmake")
add_executable(threadcalc
    src/thread_main.c
    src/thread_handle.c)
target_link_libraries(threadpool PUBLIC -pthread)
target_link_libraries(threadcalc LINK_PUBLIC threadpool -pthread Common)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    target_compile_options(threadcalc INTERFACE -g -Wall -pedantic -Werror -Wextra -pthread -static-libsan)
else()
    target_compile_options(threadcalc INTERFACE -Wall -pedantic -Werror -Wextra -pthread -static-libsan)
endif()
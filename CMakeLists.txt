cmake_minimum_required(VERSION 3.16)

enable_testing()
set(CTEST_RERUN_FAILED ON)
set(CTEST_OUTPUT_ON_FAILURE ON)

project(
    JQRCalcProjects
    LANGUAGES "C"
    DESCRIPTION "JQR Example Projects"
)

include (formatting.cmake)


include_directories(0_Common/include)

add_subdirectory(0_Common)
add_subdirectory(1_SimpleCalc)
add_subdirectory(4_ThreadCalc)
add_subdirectory(5_NetCalc)

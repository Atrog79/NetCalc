cmake_minimum_required(VERSION 3.16)

project(
    JQRCalcProjects
    LANGUAGES "C"
    )
add_library(Common src/math_funcs_thirty_two.c src/calc_handle_thirty_two.c src/debug.c src/file_io.c src/calc_handle.c src/math_funcs.c src/comparisons.c)


target_include_directories(Common PUBLIC include/)

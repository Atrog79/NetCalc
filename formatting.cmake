string(CONCAT CMAKE_CHECKS
    "*,-readability-function-cognitive-complexity,"
    "-altera*,"
    "-cert-dcl03-c,"
    "-misc-static-assert,"
    "-llvm-include-order,"
    "-llvmlibc-*,"
    "-hicpp-*,"
    "-concurrency-mt-unsafe,"
    "-bugprone-easily-swappable-parameters,"
    "-cppcoreguidelines-*,"
    "-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "*** building in DEBUG mode ***")
    enable_testing()
    add_compile_options(-std=c99 -Wall -Wextra -pedantic -Werror -O3
        -fsanitize=address)
    link_libraries(-fsanitize=address)
    add_compile_definitions(DEBUG)
    set(CMAKE_C_CLANG_TIDY
        "clang-tidy" "-checks=${CMAKE_CHECKS}" "-warnings-as-errors=*"
    )
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "*** building in RELEASE mode ***")
    add_compile_options(-std=c99 -Wall -Wextra -pedantic -O3)
    add_compile_definitions(NDEBUG)
    set(CMAKE_C_CLANG_TIDY
        "clang-tidy" "-checks=${CMAKE_CHECKS}"
    )
endif()
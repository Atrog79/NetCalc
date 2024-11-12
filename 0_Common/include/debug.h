#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MSG(...) (void)fprintf(stderr, __VA_ARGS__)

// If the project is built in debug mode then the DEBUG_PRINT Macro is used
#ifdef DEBUG

#    define RED   "\x1B[31m"
#    define GREEN "\x1B[32m"
#    define RESET "\x1B[0m"

#    define __FILENAME__ \
        (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#    define DEBUG_PRINT(...)             \
        do                               \
        {                                \
            printf("%s@ %s%s:%s%d%s | ", \
                   RED,                  \
                   __FILENAME__,         \
                   RESET,                \
                   GREEN,                \
                   __LINE__,             \
                   RESET);               \
            printf(__VA_ARGS__);         \
        } while (0);

#else

#    define DEBUG_PRINT(...) \
        do                   \
        {                    \
        } while (0);

#endif // DEBUG

#endif
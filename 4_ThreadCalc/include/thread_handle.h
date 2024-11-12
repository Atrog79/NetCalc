#ifndef _THREAD_HANDLE_H
#define _THREAD_HANDLE_H

#define _GNU_SOURCE

#include <dirent.h> // for opendir
#include <errno.h>
#include <fcntl.h>
#include <limits.h> // for realpath
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // for stat
#include <sys/sysmacros.h>
#include <sys/types.h> // for stat
#include <unistd.h>    // for access()
#include <string.h>

#include "utils.h"
#include "threadpool.h"
#include "debug.h"
#include "file_io.h"
#include "calc_handle.h"
#include "signals.h"
#include "math_funcs.h"

#define HEADER_SIZE   27
#define MAX_PATH      4096
#define EQUATION_SIZE 32
#define RWRR          0622
#define SOLUTION_SIZE 14
#define DIRENT_SIZE   256

typedef struct parse_args
{
    int in_file_stream;
    int out_file_stream;
    int num_equations;
} parse_args_t;

typedef struct file_header     file_header_t;
typedef struct equation        equation_t;
typedef struct solved_equation solve_equation_t;

/**
 * @brief Checks whether a directory exists and is readable
 *
 * @param directory_name name of directory to check
 * @return int 0 if the directory exists and is writable, -1 otherwise
 */
int check_in_directory(const char * directory_name, DIR ** pp_dir_des);

/**
 * @brief Checks whether a directory exists and is writable
 *
 * @param directory_name name of directory to check
 * @return int 0 if the directory exists and is writable, -1 otherwise
 */
int check_out_directory(const char * directory_name, DIR ** pp_dir_des);

/**
 * @brief Checks that the conversion by strtol is done without error
 *
 * @param errno_ errno value from strtol
 * @param tester the value passed from strtol
 * @param endptr_p end pointer from strtol
 * @return int 0 on success, -1 on failure
 */
int check_convert_to_long(int errno_, long tester, char ** endptr_p);

/**
 * @brief Takes in a struct with
 *
 * @param args
 * @return void*
 */
void * parse_file(void * args);

/**
 * @brief Loops through the directory and adds files to the job queue
 *
 * @param p_in_directory Input directory
 * @param in_dir_name normalized name
 * @param out_dir_name output directory normalized name
 * @param threadpool_p threadpool to be added to
 * @return int 0 on success or -1 on failure
 */
int directory_parse(DIR *          p_in_directory,
                    char *         in_dir_name,
                    char *         out_dir_name,
                    threadpool_t * threadpool_p);

/**
 * @brief Reads in the contents of the file after the header and processes them
 *
 * @param file_stream file_stream to read from
 * @param out_directory_name directory to put the results
 * @param filename filename to use to make the output file
 * @return int 0 for success
 * @return int -1 for failure
 */
int process_equations(int file_stream, int out_file_des, int64_t num_equations);

/**
 * @brief check that the file has a valid header
 *
 * @param file_stream file to check
 * @return int 0 if successful
 * @return int -1 if failed
 */
int check_header(int             file_des,
                 int64_t *       num_equations,
                 file_header_t * p_header);

/**
 * @brief Create a file for output
 *
 * @param out_directory_name where to put the file
 * @param filename the name of the input file to correlate with the resultant
 * file.
 * @param equation equation struct with header info
 * @return int -1 for failure, otherwise file descriptor
 */
int create_file(const char *    p_out_directory_name,
                const char *    filename,
                file_header_t * header);

/**
 * @brief Write results to a file
 *
 * @param file_des file descriptor of the output file
 * @param calc_result calc_t for solution and operator
 * @param equation equation id
 * @return int 0 for success
 * @return int -1 for failure
 */
size_t write_file(int file_des, calc_t * calc_result, equation_t * equation);
#endif

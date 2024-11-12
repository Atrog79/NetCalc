#ifndef _CLI_HANDLE_H
#define _CLI_HANDLE_H

#include "calc_handle_thirty_two.h"
#include "math_funcs_thirty_two.h"
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BASE           10
#define VALID_NUM_ARGS 4

typedef union converter
{
    int32_t  val;
    uint32_t uval;
} converter_u;

/**
 * @brief Takes in command line arguments and checks for the correct number and
 * type of arguments
 *
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @param num1 the first operand
 * @param num2 the second operand
 * @param oper the operator
 * @return int 0 for success
 * @return int -1 for failure
 */
int parse_args(int           argc,
               char **       argv,
               converter_u * p_num1,
               converter_u * p_num2,
               char **       pp_oper);

/**
 * @brief convert a signed number from string representation to int
 * representation
 *
 * @param str the string representation of the number
 * @param converted_num the numerical representation of the number
 * @return int 0 for success
 * @return int -1 for failure
 */
int convert_from_string(char * p_str, converter_u * p_converted_num);

/**
 * @brief convert an unsigned number from string representation to int
 * representation
 *
 * @param str the string representation of the number
 * @param converted_num the numerical representation of the number
 * @return int 0 for success
 * @return int -1 for failure
 */
int convert_from_string_unsigned(char * p_str, converter_u * p_converted_num);

#endif /* _CLI_HANDLE_H */
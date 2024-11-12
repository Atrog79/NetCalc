#ifndef _CALC_HANDLE_H
#define _CALC_HANDLE_H

#define _GNU_SOURCE // for strncmp()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "math_funcs.h"

#define NUM_OPERS 13
#define MAX_CHARS 4
#define LEFT      1
#define RIGHT     2
#define SIGNED    1
#define UNSIGNED  2

typedef struct calc calc_t;

// Create a function pointer to represent the variety of functions
typedef int (*math_func)(convert_u num1, convert_u num2, convert_u * p_result);

enum op
{
    ADD64  = 1,
    SUB64  = 2,
    MULT64 = 3,
    DIV64  = 4,
    MOD64  = 5,
    SHL64  = 6,
    SHR64  = 7,
    AND64  = 8,
    OR64   = 9,
    XOR64  = 10,
    ROL64  = 11,
    ROR64  = 12
};

/**
 * @brief initializes a calc_t struct pointer
 *
 * @return  a new calc_t*
 */
calc_t * calc_init(void);

/**
 * @brief Set the math func value for the calc_t struct
 *
 * @param oper the int representing the operation
 * @param calc_var the calc_t struct pointer
 * @return int 0 for success
 * @return int -1 for failure
 */
int set_math_func(int const * p_oper, calc_t * p_calc_var);

/**
 * @brief Solve the equation that is in the filled calc_var struct
 *
 * @param calc_var the calc_t struct pointer
 * @return int 0 for success
 * @return int -1 for failure
 */
int solve_func(calc_t * p_calc_var);

/**
 * @brief Set the two operands
 *
 * @param calc the calc_t struct pointer
 * @param operand1 the first operand of the equation
 * @param operand2 the second operand of the equation
 * @param oper the string value for the operation
 * @return int 0 for success
 * @return int -1 for failure
 */


/**
 * @brief translate the string oper into the int representation of the operator
 * and store it in translated_num
 *
 * @param oper the string representation of the operation
 * @param translated_num the integer representation of the operation
 * @return int 0 for success
 * @return int -1 for failure
 */
int translate_op(char * p_oper, int * p_translated_num);

/**
 * @brief print the results of the operation to the user
 *
 * @param result_calc the final result of the calc_t struct to be printed
 * @return int 0 for success
 * @return int -1 for failure
 */
int print_result(calc_t * p_result_calc);

/**
 * @brief determines whether
 *
 * @param oper the string representation of the operation
 * @return int 0 for success
 * @return int -1 for failure
 */
int choose_check(char * p_oper);

/**
 * @brief Set the two operands and operator
 *
 * @param calc the calc_t struct pointer
 * @param operand1 the first operand of the equation
 * @param operand2 the second operand of the equation
 * @param p_oper the integer value for the operation
 * @return int 0 for success
 * @return int -1 for failure
 */
int set_calc_file(calc_t *       p_calc,
                  int64_t        operand1,
                  int64_t        operand2,
                  int8_t const * p_oper);

#endif /* _CALC_HANDLE_H */
#ifndef _CALC_HANDLE_THIRTY_TWO_H
#define _CALC_HANDLE_THIRTY_TWO_H

#define _GNU_SOURCE // for strncmp()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "math_funcs_thirty_two.h"

#include "debug.h"

#define NUM_OPERS 13
#define MAX_CHARS 4
#define LEFT      1
#define RIGHT     2
#define SIGNED    1
#define UNSIGNED  2

typedef struct calc calc_t_32;

// Create a function pointer to represent the variety of functions
typedef int (*math_func_32)(convert_u_32   num1,
                            convert_u_32   num2,
                            convert_u_32 * p_result);

enum op_
{
    ADD  = 1,
    SUB  = 2,
    MULT = 3,
    DIV  = 4,
    MOD  = 5,
    SHL  = 6,
    SHR  = 7,
    AND  = 8,
    OR   = 9,
    XOR  = 10,
    ROL  = 11,
    ROR  = 12
};

/**
 * @brief initializes a calc_t_32 struct pointer
 *
 * @return  a new calc_t_32*
 */
calc_t_32 * calc_init_32(void);

/**
 * @brief Set the math func value for the calc_t_32 struct
 *
 * @param oper the int representing the operation
 * @param calc_var the calc_t_32 struct pointer
 * @return int 0 for success
 * @return int -1 for failure
 */
int set_math_func_32(int const * p_oper, calc_t_32 * p_calc_var);

/**
 * @brief Solve the equation that is in the filled calc_var struct
 *
 * @param calc_var the calc_t_32 struct pointer
 * @return int 0 for success
 * @return int -1 for failure
 */
int solve_func_32(calc_t_32 * p_calc_var);

// and returns a value and modifies an ouput parameter
/**
 * @brief Set the two operands
 *
 * @param calc the calc_t_32 struct pointer
 * @param operand1 the first operand of the equation
 * @param operand2 the second operand of the equation
 * @param oper the string value for the operation
 * @return int 0 for success
 * @return int -1 for failure
 */
int set_calc_32(calc_t_32 * p_calc,
                int32_t     operand1,
                int32_t     operand2,
                char *      p_oper);

/**
 * @brief translate the string oper into the int representation of the operator
 * and store it in translated_num
 *
 * @param oper the string representation of the operation
 * @param translated_num the integer representation of the operation
 * @return int 0 for success
 * @return int -1 for failure
 */
int translate_op_32(char * p_oper, int * p_translated_num);

/**
 * @brief print the results of the operation to the user
 *
 * @param result_calc the final result of the calc_t_32 struct to be printed
 * @return int 0 for success
 * @return int -1 for failure
 */
int print_result_32(calc_t_32 * p_result_calc);

/**
 * @brief determines whether
 *
 * @param oper the string representation of the operation
 * @return int 0 for success
 * @return int -1 for failure
 */
int choose_check_32(char * p_oper);

/**
 * @brief Set the two operands and operator
 *
 * @param calc the calc_t_32 struct pointer
 * @param operand1 the first operand of the equation
 * @param operand2 the second operand of the equation
 * @param p_oper the integer value for the operation
 * @return int 0 for success
 * @return int -1 for failure
 */

#endif /* _CALC_HANDLE_H */
#ifndef _MATH_FUNCS_C
#define _MATH_FUNCS_C

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_BITS 64

typedef union convert convert_u;
/**
 * @brief Function for adding num1 and num2
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of adding num1 and num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int add_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for subtracting num2 from num1
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of subtracting num1 and num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int sub_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for multiplying num1 and num2
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of multiplying num1 and num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int mult_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for divinding num1 by num2
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of dividing num1 and num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int div_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for the modulus of num1 by num2
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of modulus  num1 and num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int mod_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for shifting num1 num2 bits to the left
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of shifting num1 by num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int shift_l_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for shifting num1 num2 bits to the right
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of shifting num1 by num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int shift_r_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for anding num1 and num2
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of anding num1 by num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int and_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for oring num1 and num2
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of oring num1 by num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int or_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for xoring num1 and num2
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of xoring num1 by num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int xor_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for rotating num1 num2 bits to the left
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of rotating num1 by num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int rotate_l_func(convert_u num1, convert_u num2, convert_u * p_result);

/**
 * @brief Function for rotating num1 num2 bits to the right
 *
 * @param num1 The first operand
 * @param num2 The second operand
 * @param p_result The p_result of rotating num1 by num2
 * @return int 0 if the function succeeds
 * @return int -1 if the function fails
 */
int rotate_r_func(convert_u num1, convert_u num2, convert_u * p_result);

#endif /* _MATH_FUNCS_C */
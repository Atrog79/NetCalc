#include "math_funcs.h"

union convert
{
    int64_t  num;
    uint64_t unum;
};

/**
 * @brief Check that the add function will not overflow or underflow
 *
 * @param num1 the first operand
 * @param num2 the second operand
 * @return int 0 for success
 * @return int -1 for failure
 */
static int validate_add(int64_t num1, int64_t num2)
{
    int err = -1;

    // Check for overflow
    if ((num1 > 0) && (num2 > (INT64_MAX - num1)))
    {
        goto END;
    }
    // Check for underflow
    if ((num1 < 0) && (num2 < (INT64_MIN - num1)))
    {
        goto END;
    }

    err = 0;

END:
    return err;
}

// Covers 4.1.6: addition
int add_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("Result is NULL\n");
        goto END;
    }

    err = validate_add(num1.num, num2.num);
    if (0 != err)
    {
        printf("Overflow error\n");
        goto END;
    }

    p_result->num = num1.num + num2.num;

END:
    return err;
}

/**
 * @brief check that the subtract function will not over or underflow
 *
 * @param num1 the first operand
 * @param num2 the second operand
 * @return int 0 for success
 * @return int -1 for failure
 */
static int validate_sub(int64_t num1, int64_t num2)
{
    int err = -1;

    // Check for overflow
    if ((num1 > 0) && (num2 < (INT64_MIN + num1)))
    {
        goto END;
    }
    // Check for underflow
    if ((num1 < 0) && (num2 > (INT64_MAX + num1)))
    {
        goto END;
    }

    err = 0;

END:
    return err;
}

// Covers 4.1.6: subtration
int sub_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("sub_func p_result is NULL\n");
        goto END;
    }

    err = validate_sub(num1.num, num2.num);
    if (0 != err)
    {
        printf("Underflow error\n");
        goto END;
    }

    p_result->num = num1.num - num2.num;
    err           = 0;

END:
    return err;
}

/**
 * @brief Check that multiplication function will not overflow
 *
 * @param num1 the first operand
 * @param num2 the second operand
 * @return int 0 for success
 * @return int -1 for failure
 */
static int validate_multi(int64_t num1, int64_t num2)
{
    int err = -1;

    if (((num1 > 0) && (num2 > 0) && (num1 > INT64_MAX / num2)) ||
        ((num1 < 0) && (num2 < 0) && (num1 < INT64_MAX / num2)))
    {
        // Overflow occured
        goto END;
    }
    if (((num1 < 0) && (num2 > 0) && (num1 < INT64_MIN / num2)) ||
        ((num1 > 0) && (num2 < 0) && (num2 < INT64_MIN / num1)))
    {
        // Underflow occured
        goto END;
    }

    err = 0;

END:
    return err;
}

// Covers 4.1.6: multiplication
int mult_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mult_func p_result is NULL\n");
        goto END;
    }

    err = validate_multi(num1.num, num2.num);
    if (0 != err)
    {
        printf("Overflow/Underflow error\n");
        goto END;
    }

    p_result->num = num1.num * num2.num;
    err           = 0;

END:
    return err;
}
/**
 * @brief Check that Division function will not overflow
 *
 * @param num1 the first operand
 * @param num2 the second operand
 * @return int 0 for success
 * @return int -1 for failure
 */
static int validate_divide(int64_t num1, int64_t num2)
{
    int err = -1;

    if (0 == num2)
    {
        goto END;
    }

    if ((INT64_MIN == num1) && (-1 == num2))
    {
        goto END;
    }

    err = 0;

END:
    return err;
}
// Covers 4.1.6: division
int div_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("div_func p_result is NULL\n");
        goto END;
    }
    err = validate_divide(num1.num, num2.num);

    if (0 != err)
    {
        printf("Zero error or overflow\n");
        goto END;
    }

    p_result->num = num1.num / num2.num;
    err           = 0;

END:
    return err;
}

// Covers 4.1.6: Modulus
int mod_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }
    if (0 == num2.num)
    {
        printf("Divisor cannot be 0\n");
        goto END;
    }

    p_result->num =
        num1.num % llabs(num2.num); // Converting num2 to the absolute value of
                                    // num2 will not change the result
    err = 0;                        // and will prevent strange buffer behaviour

END:
    return err;
}

int shift_l_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }

    p_result->unum = num1.unum << num2.unum;
    err            = 0;

END:
    return err;
}

int shift_r_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }

    p_result->unum = num1.unum >> num2.unum;
    err            = 0;

END:
    return err;
}

int and_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }

    p_result->unum = num1.unum & num2.unum;
    err            = 0;

END:
    return err;
}

int or_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }

    p_result->unum = num1.unum | num2.unum;
    err            = 0;

END:
    return err;
}

int xor_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }

    p_result->unum = num1.unum ^ num2.unum;
    err            = 0;

END:
    return err;
}

int rotate_l_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }

    p_result->unum =
        ((num1.unum << num2.unum) |
         (num1.unum >>
          (NUM_BITS - num2.unum))); // C does not support rotate operators so we
                                    // need to do it manually
    err =
        0; // code for rotation from
           // https://stackoverflow.com/questions/13099406/why-doesnt-c-have-rotate-left-right-operators

END:
    return err;
}

int rotate_r_func(convert_u num1, convert_u num2, convert_u * p_result)
{
    int err = -1;

    if (NULL == p_result)
    {
        printf("mod_func p_result is NULL\n");
        goto END;
    }

    p_result->unum =
        ((num1.unum >> num2.unum) |
         (num1.unum << (NUM_BITS -
                        num2.unum))); // C does not support rotate operators so
                                      // we need to do it manually
    err =
        0; // code for rotation from
           // https://stackoverflow.com/questions/13099406/why-doesnt-c-have-rotate-left-right-operators

END:
    return err;
}

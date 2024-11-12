#include "calc_handle.h"

union convert
{
    int64_t  result;
    uint64_t u_result;
};

// Covers 4.1.9: A function pointer
struct calc
{
    convert_u num1;
    convert_u num2;
    convert_u result;
    int       oper;
    math_func func_ptr;
};

// For the Shifts, boolean, and rotates u_int should be accessible
// Covers 4.1.4: Demonstrates use of uint64_t, int64_t

// Covers 4.1.5: Demonstrate use of An array
const char * p_ops_str_64[] = { "",   "+", "-", "*", "/",   "%",  "<<",
                                ">>", "&", "|", "^", "<<<", ">>>" };

// Covers 4.1.12: Demonstrate use of switch statement
int set_math_func(int const * p_oper, calc_t * p_calc_var)
{
    int err = -1;

    if ((NULL == p_calc_var) || (NULL == p_oper))
    {
        goto END;
    }

    switch (*p_oper)
    {
        case ADD64:
            DEBUG_PRINT("ADDING ");
            p_calc_var->func_ptr = add_func;
            break;
        case SUB64:
            DEBUG_PRINT("SUBTRACTING ");
            p_calc_var->func_ptr = sub_func;
            break;
        case MULT64:
            DEBUG_PRINT("MULTIPLYING ");
            p_calc_var->func_ptr = mult_func;
            break;
        case DIV64:
            DEBUG_PRINT("DIVIDING ");
            p_calc_var->func_ptr = div_func;
            break;
        case MOD64:
            DEBUG_PRINT("MODULUS ");
            p_calc_var->func_ptr = mod_func;
            break;
        case SHL64:
            DEBUG_PRINT("SHIFT LEFT ");
            p_calc_var->func_ptr = shift_l_func;
            break;
        case SHR64:
            DEBUG_PRINT("SHIFT RIGHT ");
            p_calc_var->func_ptr = shift_r_func;
            break;
        case AND64:
            DEBUG_PRINT("AND ");
            p_calc_var->func_ptr = and_func;
            break;
        case OR64:
            DEBUG_PRINT("OR ");
            p_calc_var->func_ptr = or_func;
            break;
        case XOR64:
            DEBUG_PRINT("XOR ");
            p_calc_var->func_ptr = xor_func;
            break;
        case ROL64:
            DEBUG_PRINT("ROTATE LEFT ");
            p_calc_var->func_ptr = rotate_l_func;
            break;
        case ROR64:
            DEBUG_PRINT("ROTATE RIGHT ");
            p_calc_var->func_ptr = rotate_r_func;
            break;
        default:
            printf("Unknown operator %d\n", *p_oper);
            goto END;
            break;
    }

    err = 0;
END:
    return err;
}

calc_t * calc_init(void)
{
    calc_t * p_rtn_calc = calloc(1, sizeof(calc_t));
    return p_rtn_calc;
}

int solve_func(calc_t * p_calc_var)
{
    int err = -1;

    if (NULL == p_calc_var)
    {
        printf("Equation is NULL\n");
        goto END;
    }
    if (NULL == p_calc_var->func_ptr)
    {
        printf("Function pointer is NULL\n");
        goto END;
    }

    err = p_calc_var->func_ptr(
        p_calc_var->num1, p_calc_var->num2, &p_calc_var->result);

END:
    return err;
}
/*
int set_calc(calc_t *p_calc, int64_t operand1, int64_t operand2, char *p_oper)
{
    int err = -1;
    int rep_op = 0;

    if ((NULL == p_calc) || (NULL == p_oper))
    {
        goto END;
    }

    err = translate_op(p_oper, &rep_op);
    if (0 != err)
    {
        printf("failed to translate operation\n");
        goto END;
    }

    err = set_math_func(&rep_op, p_calc);
    if (0 != err)
    {
        printf("failed to set math function\n");
        goto END;
    }

    p_calc->num1.result = operand1;
    p_calc->num2.result = operand2;
    p_calc->oper = rep_op;

END:
    return err;
}
*/
int set_calc_file(calc_t *       p_calc,
                  int64_t        operand1,
                  int64_t        operand2,
                  int8_t const * p_oper)
{
    int err    = -1;
    int rep_op = 0;

    if ((NULL == p_calc) || (NULL == p_oper))
    {
        goto END;
    }

    rep_op = (unsigned char)*p_oper;

    err = set_math_func(&rep_op, p_calc);
    if (0 != err)
    {
        printf("failed to set math function\n");
        goto END;
    }

    p_calc->num1.result = operand1;
    p_calc->num2.result = operand2;
    DEBUG_PRINT("OPER 1: %ld, OPER 2: %ld\n", operand1, operand2);
    p_calc->oper = rep_op;

END:
    return err;
}

int choose_check(char * p_oper)
{
    int err  = -1;
    int iter = 0;
    if (NULL == p_oper)
    {
        printf("Empty oper string\n");
        goto END;
    }

    for (int idx = 1; idx < NUM_OPERS;
         idx++) // i starts at 1 because the first element of ops is ""
    {
        iter++;
        if (0 == strncmp(p_oper, p_ops_str_64[idx], MAX_CHARS))
        {
            break;
        }
    }

    // a number less than the int value MOD(5) is a signed operation
    // a number greater than the int value MOD(5) is an unsigned operation
    if (MOD64 >= iter)
    {
        err = SIGNED;
    }
    else
    {
        err = UNSIGNED;
    }

END:
    return err;
}

// Cover 4.1.11: Demostrate Declaring an integer pointer
int translate_op(char * p_oper, int * p_translated_num)
{
    int err = -1;

    if (NULL == p_oper)
    {
        printf("Empty oper string\n");
        goto END;
    }

    // Covers 4.1.11: Demonstrate assigning a value to a pointer
    // Covers 4.1.12: Demonstrate use of a for loop and an if statement
    for (int idx = 1; idx < NUM_OPERS;
         idx++) // i starts at 1 because the first element of ops is ""
    {
        if (0 == strncmp(p_oper, p_ops_str_64[idx], MAX_CHARS))
        {
            *p_translated_num = idx;
            break;
        }
    }

    // Covers 4.1.11: Demonstrate dereferencing a variable to get its value
    if (0 == *p_translated_num)
    {
        DEBUG_PRINT("%p\n",
                    (void *)p_translated_num); // Covers 4.1.11: Demostrate
                                               // printing address of a variable
        printf("Invalid operator: %s\n", p_oper);
        goto END;
    }

    err = 0;

END:
    return err;
}

int print_result(calc_t * p_result_calc)
{
    int err = -1;

    if (NULL == p_result_calc)
    {
        printf("Print struct is NULL\n");
        goto END;
    }

    // Operations are represented by enums, everything above modulus requires an
    // unsigned result type Covers 4.1.12: if else statement and effective use
    // of goto statement
    if (MOD64 >= p_result_calc->oper)
    {
        printf("%ld\n", p_result_calc->result.result);
        DEBUG_PRINT("PRINTING SIGNED \n");
    }
    else
    {
        DEBUG_PRINT("PRINTING UNSIGNED \n");
        convert_u res;
        res.u_result = p_result_calc->result.u_result;

        printf("%lu\n", res.u_result);
    }
    err = 0;
END:
    return err;
}

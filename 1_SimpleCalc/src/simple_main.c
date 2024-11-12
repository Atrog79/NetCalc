#include "simple_main.h"

// Covers 4.1.7: Demostrate int argc and char * argv[]
// Covers 4.1.3: Demostrate int
int main(int argc, char * argv[])
{
    int         err        = -1;
    converter_u num1       = { 0 };
    converter_u num2       = { 0 };
    char *      p_oper     = NULL;
    calc_t_32 * p_new_calc = NULL;

    err = parse_args(argc, argv, &num1, &num2, &p_oper);
    if (0 != err)
    {
        printf("Error parsing arguments\n");
        goto END;
    }

    p_new_calc = calc_init_32();
    if (NULL == p_new_calc)
    {
        goto END;
    }

    err = set_calc_32(p_new_calc, num1.val, num2.val, p_oper);
    if (0 != err)
    {
        printf("Failed to set calc");
        goto END;
    }

    err = solve_func_32(p_new_calc);
    if (0 != err)
    {
        printf("Failed to solve\n");
        goto END;
    }

    print_result_32(p_new_calc);

END:
    free(p_new_calc);
    p_new_calc = NULL;
    return err;
}

#include "cli_handle.h"

// Covers 4.1.10: Validating input recieved matches input expected
int parse_args(int           argc,
               char **       argv,
               converter_u * p_num1,
               converter_u * p_num2,
               char **       pp_oper) // Apparently operator is a reserved word
{
    int err        = -1;
    int check_type = 0;

    if (VALID_NUM_ARGS != argc)
    {
        printf("Invalid number of arguments\n");
        goto END;
    }

    *pp_oper = argv[2];

    check_type = choose_check_32(*pp_oper);

    if (SIGNED == check_type)
    {
        err = convert_from_string(argv[1], p_num1);
        if (0 != err)
        {
            printf("Failed to convert string\n");
            goto END;
        }

        err = convert_from_string(argv[3], p_num2);
        if (0 != err)
        {
            printf("Failed to convert string\n");
            goto END;
        }
    }
    else if (UNSIGNED == check_type)
    {
        // Check that the first character of the unsigned arguement is not a
        // negator
        if (('-' == argv[1][0]) || '-' == argv[3][0])
        {
            printf("Unsigned argument can not be negative\n");
            goto END;
        }

        err = convert_from_string_unsigned(argv[1], p_num1);
        if (0 != err)
        {
            printf("Failed to convert string\n");
            goto END;
        }

        err = convert_from_string_unsigned(argv[3], p_num2);
        if (0 != err)
        {
            printf("Failed to convert string\n");
            goto END;
        }
    }

    else
    {
        printf("Failed to convert operator\n");
    }

END:
    return err;
}

// Covers 4.1.3: Demonstrate use of char
int convert_from_string(char * p_str, converter_u * p_converted_num)
{
    int err = -1;

    errno           = 0; // must set before use
    char * p_ENDptr = NULL;

    long tester = 0;

    tester = strtol(p_str, &p_ENDptr, BASE);
    DEBUG_PRINT(
        "tester = %ld | 32_MAX %d | 32_MIN %d\n", tester, INT32_MAX, INT32_MIN);

    if (0 != errno)
    {
        printf("Error converting to integer\n");
        goto END;
    }
    else if ((ERANGE == errno) && (LONG_MIN == tester))
    {
        printf("Underflow error\n");
        goto END;
    }
    else if ((ERANGE == errno) && (LONG_MAX == tester))
    {
        printf("Overflow error\n");
        goto END;
    }

    if ((INT32_MAX < tester) || (INT32_MIN > tester))
    {
        printf("Number is out of bounds of 32bits\n");
        goto END;
    }

    if (*p_ENDptr == tester && *p_ENDptr != 0)
    {
        printf("No digits\n");
        goto END;
    }

    if ('\0' != *p_ENDptr)
    {
        printf("Not a valid number\n");
        goto END;
    }

    p_converted_num->val = (int32_t)tester;
    err                  = 0;

END:
    return err;
}

int convert_from_string_unsigned(char * p_str, converter_u * p_converted_num)
{
    int err = -1;

    errno           = 0; // must set before use
    char * p_ENDptr = NULL;

    // Using a union to avoid a narrowing conversion later on
    unsigned long utester = 0;

    utester = strtoul(p_str, &p_ENDptr, BASE);

    if (0 != errno)
    {
        printf("Error converting to integer\n");
        goto END;
    }
    else if ((ERANGE == errno) && (ULONG_MAX == utester))
    {
        printf("Overflow error\n");
        goto END;
    }

    if (EINVAL == errno)
    {
        printf("Bad base\n");
        goto END;
    }

    if ('\0' != *p_ENDptr)
    {
        printf("Not a valid number\n");
        goto END;
    }
    if (UINT32_MAX < utester)
    {
        printf("Overflow error\n");
        goto END;
    }

    p_converted_num->uval = (uint32_t)utester;
    err                   = 0;

END:
    return err;
}

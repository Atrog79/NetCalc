#include "net_converts.h"

int check_convert_to_long(int errno_, long tester, char ** endptr_p)
{
    int err = ERROR;
    if (0 != errno_)
    {
        ERR_MSG("Error converting to integer\n");
        goto END;
    }
    if ((ERANGE == errno_) && (LONG_MIN == tester))
    {
        ERR_MSG("Underflow error\n");
        goto END;
    }
    if ((ERANGE == errno_) && (LONG_MAX == tester))
    {
        ERR_MSG("Overflow error\n");
        goto END;
    }

    if ((INT32_MAX < tester) || (INT32_MIN > tester))
    {
        ERR_MSG("Number is out of bounds of 32bits\n");
        goto END;
    }

    if ((tester == **endptr_p) && (*endptr_p != 0))
    {
        ERR_MSG("No digits\n");
        goto END;
    }

    err = SUCCESS;
END:
    return err;
}

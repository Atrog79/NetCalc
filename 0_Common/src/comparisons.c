#include "comparisons.h"

comp_rtns_t int_comp(void * node_one_data, void * node_two_data)
{
    int ret = ERROR;

    if ((NULL == node_one_data) || (NULL == node_two_data))
    {
        goto END;
    }

    if (*(int *)node_one_data == *(int *)node_two_data)
    {
        ret = EQUAL;
    }
    else if (*(int *)node_one_data > *(int *)node_two_data)
    {
        ret = ONE;
    }
    else
    {
        ret = TWO;
    }

END:
    return ret;
}

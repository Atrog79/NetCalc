#include "net_main.h"

int main(int argc, char * argv[])
{
    int            opt;
    int            err          = ERROR;
    size_t         num_threads  = 0;
    char *         endptr_p     = NULL;
    long           tester       = 0;
    threadpool_t * threadpool_p = NULL;
    char           port[6]      = { 0 };

    errno = 0;

    while (ERROR != (opt = getopt(argc, argv, ":n:p:h")))
    {
        switch (opt)
        {
            case 'n':
                tester = strtol(optarg, &endptr_p, BASE);
                err    = check_convert_to_long(errno, tester, &endptr_p);
                if (err != 0)
                {
                    ERR_MSG("Error in arguement n: \n");
                    goto END;
                }
                num_threads = tester;
                break;
            case 'p':
                if(0 != strcmp(port, "\0"))
                {
                    ERR_MSG("Port already set");
                    goto END;
                }
                DEBUG_PRINT("%s a\n", optarg);
                tester = strtol(optarg, &endptr_p, BASE);
                err    = check_convert_to_long(errno, tester, &endptr_p);
                if (err != 0)
                {
                    ERR_MSG("Error in arguement n: \n");
                    goto END;
                }
                if ((2000 > tester) || (65535 < tester))
                {
                    ERR_MSG("Invalid port number\n\n");
                    goto END;
                }
                strncpy(port, optarg, 5);
                break;
            case 'h':
                ERR_MSG("USAGE: /netcalc -p <port> -n <number of threads>");
                goto EXIT;
            case '?':
                ERR_MSG("unknown option: %c\n", optopt);
                goto END;
                break;
        }
    }

    if(0 != optopt)
    {
        ERR_MSG("Unknown option or invalid argument\n\n");
        goto END;
    }

    if(NULL != argv[optind])
    {
        ERR_MSG("Invalid additional arguements\n\n");
        goto END;
    }

    if (0 == num_threads)
    {
        DEBUG_PRINT("THREADS NOT SPECIFIED\n");
        num_threads = 4;
    }

    if (0 == strcmp(port, "\0"))
    {
        DEBUG_PRINT("USING DEFAULT port 31337\n");
        strncpy(port, "31337", 6);
    }

    err = signal_action_setup();
    if (0 != err)
    {
        ERR_MSG("Unable to setup signal\n");
        goto END;
    }

    threadpool_p = threadpool_create(num_threads);
    if (NULL == threadpool_p)
    {
        ERR_MSG("threadpool_create failed\n");
        goto END;
    }

    err = start_listener(port, threadpool_p);
    if (0 != err)
    {
        ERR_MSG("Listener error\n");
        goto END;
    }
    else
    {
        DEBUG_PRINT("Server closed normally\n");
    }

    err = SUCCESS;
END:
    threadpool_destroy(&threadpool_p);

EXIT:
    return err;
}

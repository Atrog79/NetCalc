#include "thread_main.h"

int main(int argc, char * argv[])
{
    int            opt;
    int            err                      = 0;
    size_t         num_threads              = 0;
    char *         endptr_p                 = NULL;
    long           tester                   = 0;
    threadpool_t * threadpool_p             = NULL;
    char           directories[2][MAX_PATH] = { 0 };

    DIR * in_dir_des_p  = NULL;
    DIR * out_dir_des_p = NULL;

    if ((MIN_ARGS > argc) || (MAX_ARGS < argc))
    {
        ERR_MSG("NOT correct number arguments\n");
        goto END;
    }

    while ((opt = getopt(argc, argv, ":n:")) != ERROR)
    {
        switch (opt)
        {
            case 'n':
                tester = strtol(optarg, &endptr_p, BASE);
                err    = check_convert_to_long(errno, tester, &endptr_p);
                if (SUCCESS != err)
                {
                    ERR_MSG("Error in arguement n: \n");
                    goto END;
                }
                num_threads = tester;
                break;
            default:
                ERR_MSG("unknown option: %c\n", optopt);
                goto END;
                break;
        }
    }

    errno = 0;
    // optind is for the extra arguments
    // which are not parsed

    for (int iter = 0; optind < argc; optind++, iter++)
    {
        DEBUG_PRINT("extra arguments: %s\n", argv[optind]);
        if (2 > iter)
        {
            snprintf(directories[iter], MAX_PATH, "%s", argv[optind]);
        }
    }

    if (SUCCESS == num_threads)
    {
        DEBUG_PRINT("THREADS NOT SPECIFIED\n");
        num_threads = 4;
    }

    err = signal_action_setup();
    if (SUCCESS != err)
    {
        ERR_MSG("Unable to setup signal\n");
        goto END;
    }

    err = check_in_directory(directories[IN_DIR], &in_dir_des_p);
    if (SUCCESS != err)
    {
        ERR_MSG("Invalid input directory\n");
        goto END;
    }
    if (NULL == in_dir_des_p)
    {
        ERR_MSG("Invalid input directory descriptor\n");
        goto END;
    }

    err = check_out_directory(directories[OUT_DIR], &out_dir_des_p);
    if (SUCCESS != err)
    {
        ERR_MSG("Invalid output directory\n");
        goto END;
    }
    if (NULL == out_dir_des_p)
    {
        ERR_MSG("Invalid output directory descriptor\n");
        goto END;
    }

    threadpool_p = threadpool_create(num_threads);
    if (NULL == threadpool_p)
    {
        ERR_MSG("threadpool_create failed\n");
        goto END;
    }

    err = directory_parse(
        in_dir_des_p, directories[IN_DIR], directories[OUT_DIR], threadpool_p);

    if (0 != err)
    {
        ERR_MSG("Failed to parse input directory\n");
        goto END;
    }

END:
    if (NULL != threadpool_p)
    {
        threadpool_destroy(&threadpool_p);
    }

    if (in_dir_des_p)
    {
        closedir(in_dir_des_p);
    }

    if (out_dir_des_p)
    {
        closedir(out_dir_des_p);
    }
    return 0;
}

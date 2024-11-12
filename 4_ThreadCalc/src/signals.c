#include "signals.h"

volatile sig_atomic_t signal_flag_g = 0;

void signal_handler(int signal)
{
    if (SIGUSR1 == signal)
    {
        signal_flag_g = 1;
    }
    if (SIGINT == signal)
    {
        signal_flag_g = 2;
    }
}

int signal_action_setup()
{
    int              err    = ERROR;
    struct sigaction action = { 0 };
    action.sa_handler       = signal_handler;
    action.sa_flags         = 0;

    err = sigaction(SIGUSR1, &action, NULL);
    if (err != 0)
    {
        goto END;
    }
    err = sigaction(SIGINT, &action, NULL);
    if (err != 0)
    {
        goto END;
    }
END:
    return err;
}

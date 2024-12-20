#ifndef SIGNAL_TESTS_H
#define SIGNAL_TESTS_H

#define _XOPEN_SOURCE 700 // prevent incomplete class type "struct sigaction"

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "signals.h"
#include "utils.h"

/**
 * @brief Test a signal handler with CUnit
 * Test that SIGUSR1 signal is caught and the global signal number is set to 1
 * Test that SIGINT signal is caught and the global signal number is set to 2
 *
 * Calls signal_action_setup() in test
 */
void test_signal_handler(void);

#endif
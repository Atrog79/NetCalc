#ifndef THREAD_TESTS_H
#define THREAD_TESTS_H

#define _XOPEN_SOURCE 700 // prevent incomplete class type "struct sigaction"

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdbool.h>

#include "threadpool.h"
#include "utils.h"

#define DEFAULT_THREADPOOL_SIZE     10
#define DEFAULT_SLEEP_DURATION_MS   500
#define NSEC_PER_MS                 1000000
#define DEFAULT_SLEEP_DURATION_NSEC (DEFAULT_SLEEP_DURATION_MS * NSEC_PER_MS)

/**
 * @brief Test create new threadpool.
 *
 */
void test_threadpool_create(void);

/**
 * @brief Test queueing a job in the threadpool.
 *
 */
void test_threadpool_add_job(void);

/**
 * @brief Test queue job handles null pool.
 *
 */
void test_threadpool_add_job_null_pool(void);

/**
 * @brief Test queue job handles null job.
 *
 */
void test_threadpool_add_null_job(void);

/**
 * @brief Test limits of threadpool.
 *
 */
void test_threadpool_limit(void);

/**
 * @brief Test shutdown threadpool.
 *
 */
void test_threadpool_shutdown(void);

#endif

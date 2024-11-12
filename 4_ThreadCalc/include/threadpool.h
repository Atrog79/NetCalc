#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include "utils.h"

/**
 * @note A queue is recommended, not required.
 */
#include "queue.h"

#define MAX_JOBS 200
#define EMPTY    0
#define ACTIVE   1
#define INACTIVE 0

enum solve
{
    INT  = 1,
    UINT = 2
};

/**
 * @brief A threadpool type. Internals to be implemented by trainee.
 */
typedef struct threadpool_t
{
    size_t              thread_count; /* number of threads in the pool */
    queue_t *           job_pool_p;
    pthread_cond_t      work_to_do;
    pthread_t *         threads_p;
    pthread_mutex_t     mutex_p;
    volatile atomic_int active;

} threadpool_t;

/**
 * @brief Create a new threadpool and instantiate as required.
 *
 * @param thread_count The number of threads to create in the threadpool
 *
 * @return A threadpool instance of type threadpool_t on success.
 *          NULL on failure
 */
threadpool_t * threadpool_create(size_t thread_count);

/**
 * @brief Nice shutdown of threadpool. Do not take any more work.
 * Finish the work that has already been accepted.
 *
 * @param pool_p A valid threadpool instance
 *
 * @return SUCCESS, ERROR
 */
int threadpool_shutdown(threadpool_t * pool_p);

/**
 * @brief Destroy a threadpool. Clean up all resources and memory.
 *
 * @return SUCCESS, ERROR
 */
int threadpool_destroy(threadpool_t ** pool_pp);

/**
 * @brief Add a job to to the threadpool to work on.
 *
 * @param pool_p The valid pool to execute the job
 * @param job The job to be executed by the pool
 * @param del_f A user defined function to free and clean up arg_p, if not
 * required, set to NULL. Internal structure of arg will be known to the del_f,
 * and it must safely handle NULL inputs.
 * @param arg_p The argument(s) required by the job, if any. Internal structure
 * of arg will be known to the job.
 *
 * @note A valid job must include the function pointer job_f. The job may not
 * include an arg and thus NULL must be accepted as an arg. The free_f may also
 * be NULL.
 *
 * @return SUCCESS, ERROR
 */
int threadpool_add_job(threadpool_t * pool_p,
                       job_f          job,
                       free_f         del_f,
                       void *         arg_p);

#endif

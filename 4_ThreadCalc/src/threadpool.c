#include "threadpool.h"

/**
 * @brief breaks down an invalid thread pool
 *
 * @param pool_p thread pool to break down
 */
static void failed_start(threadpool_t * pool_p);

/**
 * @brief Initial function the thread goes into, waits for job here
 *
 * @param arg arguements to pass
 * @return void*
 */
static void * start_thread(void * arg);

threadpool_t * threadpool_create(size_t thread_count)
{
    threadpool_t * rtn_threadpool_p = NULL;
    size_t         iter             = 0;
    int            err              = 0;

    if (0 == thread_count)
    {
        goto END;
    }

    rtn_threadpool_p = calloc(1, sizeof(threadpool_t));
    if (NULL == rtn_threadpool_p)
    {
        goto END;
    }
    rtn_threadpool_p->thread_count = thread_count;
    rtn_threadpool_p->job_pool_p   = queue_new(MAX_JOBS);
    if (NULL == rtn_threadpool_p->job_pool_p)
    {
        failed_start(rtn_threadpool_p);
        rtn_threadpool_p = NULL;
        goto END;
    }

    rtn_threadpool_p->threads_p = calloc(thread_count, sizeof(pthread_t));
    if (NULL == rtn_threadpool_p->threads_p)
    {
        failed_start(rtn_threadpool_p);
        rtn_threadpool_p = NULL;
        goto END;
    }
    pthread_mutex_init(&(rtn_threadpool_p->mutex_p), NULL);
    if (NULL == &(rtn_threadpool_p->mutex_p))
    {
        failed_start(rtn_threadpool_p);
        rtn_threadpool_p = NULL;
        goto END;
    }

    pthread_cond_init(&(rtn_threadpool_p->work_to_do), NULL);
    if (NULL == &(rtn_threadpool_p->work_to_do))
    {
        failed_start(rtn_threadpool_p);
        rtn_threadpool_p = NULL;
        goto END;
    }

    rtn_threadpool_p->active = 1;
    for (iter = 0; iter < thread_count; iter++)
    {
        err = pthread_create(&rtn_threadpool_p->threads_p[iter],
                             NULL,
                             &start_thread,
                             rtn_threadpool_p);
        if (0 != err)
        {
            failed_start(rtn_threadpool_p);
            rtn_threadpool_p = NULL;
            goto END;
        }
    }
END:
    return rtn_threadpool_p;
}

int threadpool_shutdown(threadpool_t * pool_p)
{
    int err = ERROR;
    if (NULL == pool_p)
    {
        goto END;
    }
    pthread_mutex_lock(&(pool_p->mutex_p));
    pool_p->active = INACTIVE;
    err            = SUCCESS;
    pthread_mutex_unlock(&(pool_p->mutex_p));
END:
    return err;
}

int threadpool_destroy(threadpool_t ** pool_pp)
{
    int    err  = ERROR;
    size_t iter = 0;
    if (NULL == *pool_pp)
    {
        goto END;
    }

    threadpool_shutdown(*pool_pp);

    pthread_cond_broadcast(&(*pool_pp)->work_to_do);
    for (iter = 0; iter < (*pool_pp)->thread_count; iter++)
    {
        pthread_join((*pool_pp)->threads_p[iter], NULL);
    }
    err = pthread_mutex_destroy(&((*pool_pp)->mutex_p));
    if (0 != err)
    {
        goto END;
    }
    err = pthread_cond_destroy(&((*pool_pp)->work_to_do));
    if (0 != err)
    {
        goto END;
    }
    queue_destroy(&((*pool_pp)->job_pool_p));
    free((*pool_pp)->threads_p);
    (*pool_pp)->threads_p = NULL;
    free((*pool_pp));
    (*pool_pp) = NULL;

    err = SUCCESS;
END:
    return err;
}

int threadpool_add_job(threadpool_t * pool_p,
                       job_f          job,
                       free_f         del_f,
                       void *         arg_p)
{
    int err = ERROR;
    if ((NULL == pool_p) || (NULL == job) || (0 == pool_p->active))
    {
        goto END;
    }

    pthread_mutex_lock(&(pool_p->mutex_p));
    queue_enqueue(pool_p->job_pool_p, job, del_f, arg_p);
    err = pthread_mutex_unlock(&(pool_p->mutex_p));
    if (err != SUCCESS)
    {
        goto END;
    }
    err = pthread_cond_signal(&(pool_p->work_to_do));

END:
    return err;
}

static void failed_start(threadpool_t * pool_p)
{
    pool_p->active = 0;
    if (NULL != pool_p->job_pool_p)
    {
        queue_destroy(&pool_p->job_pool_p);
    }
    if (NULL != &(pool_p->mutex_p))
    {
        pthread_mutex_destroy(&(pool_p->mutex_p));
    }
    if (NULL != pool_p->threads_p)
    {
        free(pool_p->threads_p);
        pool_p->threads_p = NULL;
    }
    if (NULL != &(pool_p->work_to_do))
    {
        pthread_cond_destroy(&(pool_p->work_to_do));
    }
    if (NULL != pool_p)
    {
        free(pool_p);
        pool_p = NULL;
    }
}

static void * start_thread(void * arg)
{
    if (NULL == arg)
    {
        goto END;
    }
    threadpool_t * pool_p = (threadpool_t *)arg;
    for (;;)
    {

        job_f  job_p  = NULL;
        free_f del_p  = NULL;
        void * args_p = NULL;
        pthread_mutex_lock(&(pool_p->mutex_p));
        while ((EMPTY == pool_p->job_pool_p->size) &&
               (ACTIVE == pool_p->active))
        {
            pthread_cond_wait(&(pool_p->work_to_do), &(pool_p->mutex_p));
        }
        queue_dequeue(pool_p->job_pool_p, &job_p, &del_p, &args_p);
        pthread_mutex_unlock(&(pool_p->mutex_p));
        if (NULL != job_p)
        {
            job_p(args_p);
            if (NULL != del_p)
            {
                del_p(args_p);
            }
        }

        if ((EMPTY == pool_p->job_pool_p->size) && (INACTIVE == pool_p->active))
        {
            break;
        }
    }
END:
    return NULL;
}

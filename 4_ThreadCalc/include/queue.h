#ifndef QUEUE_H
#define QUEUE_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

#define DEFAULT_QUEUE_SIZE 300

typedef struct node_t
{
    free_f del_f;
    job_f  task;
    void * arg;
} node_t;

/**
 * @brief Array implementation of a queue data structure.  This queue should
 * support arbitrary data types.
 *
 */
typedef struct queue_t
{
    atomic_int capacity;
    atomic_int size;
    atomic_int front;
    atomic_int rear;
    node_t **  items;
} queue_t;

/**
 * @brief Creates a new queue
 *
 * @param max_queue_size  the maximum size of the queue
 * @param del a custom function used to delete values held in the queue
 *            This is necessary to support deletion of arbitrary, user-defined
 *            data types.
 * @return queue_t* a pointer to the newly allocated queue object on success
 */
queue_t * queue_new(size_t max_queue_size);

/**
 * @brief Destroy the queue and all items it contains, deallocating any memory
 *        that was allocated.
 *
 * Asserts that queue is not NULL.
 * Queue pointer is set to NULL after memory is deallocated.
 *
 * @param queue a reference to a pointer to an allocated queue
 */
void queue_destroy(queue_t ** queue);

/**
 * @brief Adds an item to the queue
 *
 * Asserts that the queue pointer is not NULL.
 * Asserts that the queue is not full.
 *
 * @param queue a pointer to an allocated queue object
 *
 * @return EXIT_SUCCESS / EXIT_FAILURE
 */
int queue_enqueue(queue_t * queue, job_f task, free_f del_f, void * arg);

/**
 * @brief Removes an item from the queue
 *
 * Asserts that the queue pointer is not NULL.
 * Asserts that the queue is not empty.
 *
 * @param queue a pointer to an allocated queue object
 * @param value references a pointer to the value dequeued from the queue
 */
int queue_dequeue(queue_t * queue, job_f * task, free_f * del_f, void ** arg);

/**
 * @brief returns 1 if the queue is empty 0 otherwise
 *
 * @param queue queue to look at
 * @return int 1 if the queue is empty 0 otherwise
 */
int queue_emptycheck(queue_t * queue);
#endif

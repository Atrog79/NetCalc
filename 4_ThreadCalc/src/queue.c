#include "queue.h"
#include "queue.h"

/**
 * @brief returns a pointer to the next node in the queue
 *
 * @param queue queue to look at
 * @return node_t* next node in the queue
 */
static node_t * queue_peek(queue_t * queue);

/**
 * @brief checks that the queue isn't full
 *
 * @param queue queue to check
 * @return int returns 1 if queue is full 0 otherwise
 */
static int queue_fullcheck(queue_t * queue);

queue_t * queue_new(size_t capacity)
{
    queue_t * p_queue = NULL;
    if (0 == capacity)
    {
        goto END;
    }

    p_queue = calloc(1, sizeof(queue_t));
    if (NULL == p_queue)
    {
        goto END;
    }

    p_queue->capacity = capacity;

    p_queue->size  = 0;
    p_queue->items = calloc(capacity, sizeof(node_t *));
    p_queue->front = 0;
    p_queue->rear  = 0;

    // Failed to allocate itemsay for nodes
    if (NULL == p_queue->items)
    {
        free(p_queue);
        p_queue = NULL;
        goto END;
    }

END:
    return p_queue;
}

static int queue_fullcheck(queue_t * queue)
{
    int err = 1;
    if (NULL == queue)
    {
        goto END;
    }

    if (queue->capacity == queue->size)
    {
        goto END;
    }

    err = 0;
END:
    return err;
}

int queue_emptycheck(queue_t * queue)
{
    int err = 1;
    if (NULL == queue)
    {
        goto END;
    }
    if (0 == queue->size)
    {
        goto END;
    }

    err = 0;

END:
    return err;
}

int queue_enqueue(queue_t * queue, job_f task, free_f del_f, void * arg)
{
    int err = 1;
    if (0 != queue_fullcheck(queue))
    {
        goto END;
    }
    if (NULL == arg)
    {
        goto END;
    }

    node_t * p_new_node = calloc(1, sizeof(node_t));
    if (NULL == p_new_node)
    {
        goto END;
    }

    p_new_node->arg   = arg;
    p_new_node->task  = task;
    p_new_node->del_f = del_f;

    if (0 != queue_emptycheck(queue))
    {
        queue->items[queue->rear] = p_new_node;
        queue->size++;
        goto END;
    }
    queue->rear               = ((queue->rear + 1) % queue->capacity);
    queue->items[queue->rear] = p_new_node;
    queue->size++;

    err = 0;
END:
    return err;
}

int queue_dequeue(queue_t * queue, job_f * task, free_f * del_f, void ** arg)
{
    node_t * p_rtn_node = NULL;
    int      err        = ERROR;
    p_rtn_node          = queue_peek(queue);

    if (NULL == p_rtn_node)
    {
        goto END;
    }

    *task  = p_rtn_node->task;
    *del_f = p_rtn_node->del_f;
    *arg   = p_rtn_node->arg;

    queue->items[queue->front] = NULL;
    queue->front               = ((queue->front + 1) % queue->capacity);
    queue->size--;

    if (0 != queue_emptycheck(queue))
    {
        queue->front = 0;
        queue->rear  = 0;
    }

    // No longer need the reference to the whole node, since it is getting split
    free(p_rtn_node);
    p_rtn_node = NULL;
END:
    return err;
}

static node_t * queue_peek(queue_t * queue)
{
    node_t * p_rtn_node = NULL;
    if (0 != queue_emptycheck(queue))
    {
        goto END;
    }

    if (queue->front == queue->rear)
    {
        p_rtn_node = queue->items[queue->front];
        goto END;
    }

    p_rtn_node = queue->items[queue->front];

END:
    return p_rtn_node;
}

int queue_clear(queue_t * queue)
{
    int err = queue_emptycheck(queue);
    if (0 != err)
    {
        goto END;
    }

    int iter = 0;
    for (iter = 0; iter < queue->size; iter++)
    {
        // Custom free would be used here to free data
        // Not being used to avoid unused code warnings
        free(queue->items[(queue->front + iter) % queue->capacity]);
        queue->items[(queue->front + iter) % queue->capacity] = NULL;
    }

    queue->front = 0;
    queue->rear  = 0;

    queue->size = 0;

    err = 0;
END:
    return err;
}

void queue_destroy(queue_t ** queue_addr)
{
    if (NULL == *queue_addr)
    {
        goto END;
    }
    queue_clear(*queue_addr);

    free((*queue_addr)->items);
    (*queue_addr)->items = NULL;

    free(*queue_addr);
    *queue_addr = NULL;

END:
    return;
}

void custom_free(void * mem_addr)
{
    free(mem_addr);
    mem_addr = NULL;
}

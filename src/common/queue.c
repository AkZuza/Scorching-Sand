#include "queue.h"
#include <memory.h>
#include <stdlib.h>

void queue_create(Queue *queue, uint64_t elem_size, uint64_t initial_size)
{
    queue->data = calloc(elem_size, initial_size);
    queue->count = 0;
    queue->elem_size = elem_size;
    queue->capacity = initial_size;
    queue->front = 0;
    queue->rear = 0;
}

void queue_free(Queue *queue)
{
    free(queue->data);
}

void queue_enqueue(Queue *queue, void *element)
{
    if (queue->count+1 == queue->capacity)
    {
        if(queue_resize(queue, queue->capacity * 2) == 0)
            return;
    }
                                                 
    queue->count += 1;
    char *dest_data = ((char*)queue->data) + (queue->rear*queue->elem_size);
    memcpy(dest_data, element, queue->elem_size);
    queue->rear = (queue->rear + 1) % queue->capacity;
}

void* queue_front(Queue *queue)
{
    char *dest_data = ((char*)queue->data) + (queue->front*queue->elem_size);
    return (void*)dest_data;
}

void queue_dequeue(Queue *queue)
{
    if (queue->count == 0) return;
    queue->count -= 1;
    queue->front = (queue->front + 1) % queue->capacity;
}

void* queue_rear(Queue *queue)
{
    char *dest_data = ((char*)queue->data) + ((queue->rear-1)*queue->elem_size);
    return (void*)dest_data;
}

int queue_resize(Queue *queue, uint64_t new_size)
{
    void *new_data = realloc(queue->data, queue->elem_size*new_size);
    if (new_data == NULL) 0;
    queue->data = new_data;
    queue->capacity = new_size;
    return 1;
}

uint64_t queue_count(const Queue const* queue)
{
    return queue->count;
}

uint64_t queue_capacity(const Queue const* queue)
{
    return queue->capacity;
}


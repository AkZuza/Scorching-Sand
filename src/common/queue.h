#pragma once

#include <stdint.h>

struct Queue
{
    void *data;
    uint64_t elem_size;
    uint64_t count;
    uint64_t capacity;
    uint64_t front;
    uint64_t rear;
};
typedef struct Queue Queue;

void queue_create(Queue* queue, uint64_t elem_size, uint64_t initial_size);
void queue_free(Queue *queue);
void queue_enqueue(Queue *queue, void *element);
void* queue_front(Queue *queue);
void queue_dequeue(Queue *queue);
void* queue_rear(Queue *queue);
int queue_resize(Queue *queue, uint64_t new_size);
uint64_t queue_count(const Queue const* queue);
uint64_t queue_capacity(const Queue const* queue);

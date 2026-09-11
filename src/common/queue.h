#pragma once

#include <stdint.h>

void* queue_create(uint64_t elem_size, uint64_t initial_size);
void queue_free(void *queue);
void queue_enqueue(void *queue, void *element);
void* queue_front(void *queue);
void queue_dequeue(void *queue);
void* queue_rear(void *queue);

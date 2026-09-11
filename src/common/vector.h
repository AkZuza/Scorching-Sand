#pragma once

#include <stdint.h>
#include <stddef.h>

void*   vector_create(size_t elem_size, size_t count);
void    vector_append(void *data, void *elem);
void    vector_remove(void *data);
size_t  vector_count(void *data);
size_t  vector_capacity(void *data);
void    vector_resize(void *data, size_t new_size);
void    vector_free(void *vec);


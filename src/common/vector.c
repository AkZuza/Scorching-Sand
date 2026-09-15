#include "vector.h"
#include <memory.h>
#include <stdlib.h>

#define GET_VECTOR(data) (((struct VectorState *)(data))-1)
#define GET_DATA(vector) ((void*)(vector + 1))

struct VectorState
{
    size_t count;
    size_t capacity;
    size_t elem_size;
};

void* vector_create(size_t elem_size, size_t count)
{
    if (count == 0) return NULL;
    size_t size_bytes = elem_size*count + sizeof(struct VectorState);
    struct VectorState *vec = (struct VectorState *)malloc(size_bytes);
    if (vec == NULL) return NULL;
    void *data = GET_DATA(vec); 
    vec->elem_size = elem_size;
    vec->count = 0;
    vec->capacity = count;

    return data;
}

void* vector_append(void *data, void *elem)
{
    struct VectorState *vector = GET_VECTOR(data);
    size_t new_size = vector->count + 1;
    if (new_size > vector->capacity)
    {
        new_size *= 2;
        size_t new_size_bytes = new_size * vector->elem_size + sizeof(struct VectorState);
        struct VectorState *new_vector = realloc(vector, new_size_bytes);
        if (new_vector == NULL) return NULL; // Failed to realloc, do nothing
        vector = new_vector;
        vector->capacity = new_size;
    }
    
    data = GET_DATA(vector);
    void *dest_data = (void*)( ((char*)(data)) + (vector->count*vector->elem_size) );
    memcpy(dest_data, elem, vector->elem_size);
    vector->count += 1;

    return data;
}

void vector_remove(void *data)
{
    struct VectorState *vector = GET_VECTOR(data);
    if (vector->count > 0)
    {
        vector->count -= 1;
    }
}

void vector_free(void *data)
{
    struct VectorState *vector = GET_VECTOR(data);
    free(vector);
}

size_t  vector_count(void *data)
{
    return GET_VECTOR(data)->count;
}

size_t  vector_capacity(void *data)
{
    return GET_VECTOR(data)->capacity;
}

void* vector_resize(void *data, size_t new_size)
{
    struct VectorState *vector = GET_VECTOR(data);
    if (new_size > vector->capacity)
    {
        new_size *= 2;
        size_t new_size_bytes = new_size * vector->elem_size + sizeof(struct VectorState);
        struct VectorState *new_vector = realloc(vector, new_size_bytes);
        if (new_vector == NULL) return NULL; // Failed to realloc, do nothing
        vector = new_vector;
        vector->capacity = new_size;
    }
    
    if (new_size < vector->capacity)
    {
        vector->count = new_size;
    }

    return GET_DATA(vector);
}

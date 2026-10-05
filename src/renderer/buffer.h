#pragma once
#include <stdint.h>
#include <stddef.h>

// Buffer - Vertex Buffer and Index Buffer

typedef enum BufferType {
    BUFFER_TYPE_VERTEX,
    BUFFER_TYPE_INDEX,
} BufferType;

typedef enum BufferUsageFlag {
    BUFFER_USAGE_FLAG_VERTEX_BUFFER = 0x01,
    BUFFER_USAGE_FLAG_INDEX_BUFFER = 0x2,
} BufferUsageFlag;

typedef enum Format {
    FORMAT_FLOAT4,
    FORMAT_FLOAT3,
    FORMAT_FLOAT2,
    FORMAT_FLOAT1,
} Format;

typedef struct ElementFormat {
    Format format;
    char name[64];
} ElementFormat;

typedef struct BufferLayout {
    uint32_t count;
    ElementFormat *layout;
} BufferLayout;

typedef struct BufferDescription {
    BufferType buffer_type;
    BufferUsageFlag buffer_usage_flag;
    size_t buffer_size;
    size_t element_stride;
    BufferLayout *layout;
} BufferDescription;

typedef struct Buffer {
    size_t buffer_size;
    void *handle;
} Buffer;

Buffer r_buffer_create(const BufferDescription *description);
void r_buffer_destroy(Buffer *buffer);
void r_buffer_write(Buffer *buffer, size_t offset, size_t size, void *data);
void r_bind_vertex_buffers(Buffer **buffers, uint32_t count);

#include "buffer.h"
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <common/vector.h>
#include <vulkan/vulkan_core.h>
#include "renderer.h"
#include "renderer/renderer_core.h"

#define IS_FLAG_SET(var, flag) (((var)&(flag))!=0)
#define IS_FLAG_UNSET(var, flag) (((var)&(flag))==0)

Buffer r_buffer_create(const BufferDescription *description)
{
    const Renderer *renderer = r_get_renderer();

    VkBuffer buffer;
    VkBufferCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    };

    createInfo.size = description->buffer_size;

    // Buffer Usage
    createInfo.usage = 
        (VK_BUFFER_USAGE_INDEX_BUFFER_BIT * IS_FLAG_SET(description->buffer_usage_flag, BUFFER_USAGE_FLAG_INDEX_BUFFER)) |
        (VK_BUFFER_USAGE_VERTEX_BUFFER_BIT * IS_FLAG_SET(description->buffer_usage_flag, BUFFER_USAGE_FLAG_VERTEX_BUFFER))
    ;

    createInfo.sharingMode= VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(
            renderer->core.device, 
            &createInfo, 
            NULL, 
            &buffer
    );
    VK_RESULT_CHECK(result);

    Buffer buf = {
        .buffer_size = description->buffer_size,
        .handle = (void *)buffer,
    };
    return buf;
}

void r_buffer_destroy(Buffer *buffer)
{
    const Renderer *renderer = r_get_renderer();
    vkDestroyBuffer(renderer->core.device, (VkBuffer)buffer->handle, NULL);
}

void r_buffer_write(Buffer *buffer, size_t offset, size_t size, void *data)
{
    const Renderer *renderer = r_get_renderer();
    void *dst;
}


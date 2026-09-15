#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>

#define VK_RESULT_CHECK(result)

typedef struct RendererCore
{
    VkInstance              instance;
    VkPhysicalDevice        physical_device;
    VkDevice                device;
    VkSwapchainKHR          swapchain;
    VkSurfaceKHR            surface;
} RendererCore;

typedef struct RendererCoreInit
{
    VkSurfaceKHR                    surface;
    uint32_t                        instance_extension_count;
    const char                      **instance_extensions;
    uint32_t                        device_extension_count;
    const char                      **device_extensions;
    uint32_t                        layer_count;
    const char                      **layers;
} RendererCoreInit;

void rc_initialize(RendererCore *renderer_core, const RendererCoreInit *init);
void rc_destroy(const RendererCore *renderer_core);

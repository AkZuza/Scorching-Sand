#include "renderer.h"
#include "renderer/renderer_core.h"
#include <vulkan/vulkan_core.h>

#define GLFW_PLATFORM_WAYLAND
#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

Renderer *g_renderer = NULL;

void r_initialize(Renderer *renderer)
{
    const uint32_t instance_extension_count = 2;
    const char * instance_extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_wayland_surface"
    };

    const RendererCoreInit init = {
        .instance_extension_count = instance_extension_count,
        .instance_extensions = instance_extensions
    };

    rc_initialize(&renderer->core, &init, renderer->window);

    g_renderer = renderer;
}

void r_destroy(const Renderer *renderer)
{
    rc_destroy(&renderer->core);
}

const Renderer* r_get_renderer()
{
    return g_renderer;
}

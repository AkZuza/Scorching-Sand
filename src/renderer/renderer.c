#include "renderer.h"
#include <vulkan/vulkan_core.h>
#include <log/log.h>

#define GLFW_PLATFORM_WAYLAND
#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

Renderer *g_renderer = NULL;

typedef struct RendererState {
    
} RendererState;

void _create_basic_renderer(Renderer const *renderer);

Renderer* r_create_renderer(const Window window)
{
    if (g_renderer != NULL)
    {
        LOG_WARN("Renderer creation: Creating more than instance is invalid\n");
        return NULL;
    }

    Renderer* renderer = malloc(sizeof(Renderer) + sizeof(RendererState));
    renderer->window = window;
    g_renderer = renderer;
    return renderer;
}

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
}

void r_destroy(const Renderer *renderer)
{
    rc_destroy(&renderer->core);
    free((void*)renderer);
}

const Renderer* r_get_renderer()
{
    return g_renderer;
}

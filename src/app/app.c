#include "app.h"
#include "window/window.h"
#include <event/event.h>
#include <renderer/renderer_core.h>
#include <vulkan/vulkan_core.h>

#define APP_STATE(app) ((AppState*)(app))
#define APP_REF(state) ((App)(state))

struct AppState 
{
    AppOnCreateFunction     create_func;
    AppOnUpdateFunction     update_func;
    AppOnDestroyFunction    destroy_func;
    Window                  window;
    RendererCore            renderer_core; // TO BE REMOVED after implementing Renderer
};
typedef struct AppState AppState;

App create_app(const struct AppInfo *info)
{
    Window window = create_window(info->window_info);

    AppState *state = malloc(sizeof(AppState));
    state->create_func = info->create_func;
    state->update_func = info->update_func;
    state->destroy_func = info->destroy_func;
    state->window = window;

    em_initialize_event_manager();
    
    // Remove this section after implementing RendererCore
    {
        const uint32_t instance_extension_count = 2;
        const char *instance_extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            "VK_KHR_wayland_surface"
        };

        RendererCoreInit rc_init = {
            .instance_extension_count = instance_extension_count,
            .instance_extensions = instance_extensions
        };
        rc_initialize(&state->renderer_core, &rc_init, window);
    }

    return APP_REF(state);
}

void destroy_app(const App app)
{
    AppState *state = APP_STATE(app);
    destroy_window(state->window);
    free(state);
}

void app_run(const App app)
{
    AppState *state = APP_STATE(app);
    Window window = state->window;

    state->create_func();
    while(is_window_running(window))
    {
        update_window(window);
        state->update_func(0.0);
    }
    state->destroy_func();
}


Window get_window(const App app)
{
    AppState *state = APP_STATE(app);
    return state->window;
}

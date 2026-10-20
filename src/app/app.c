#include "app.h"
#include "window/window.h"
#include <event/event.h>
#include <renderer/renderer.h>
#include <vulkan/vulkan_core.h>

#define APP_STATE(app) ((AppState*)(app))
#define APP_REF(state) ((App)(state))

struct AppState 
{
    AppOnCreateFunction     create_func;
    AppOnUpdateFunction     update_func;
    AppOnDestroyFunction    destroy_func;
    Window                  window;
    Renderer*                renderer;
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
    state->renderer = r_create_renderer(window);

    em_initialize_event_manager();
    r_initialize(state->renderer);

    return APP_REF(state);
}

void destroy_app(const App app)
{
    AppState *state = APP_STATE(app);
    r_destroy(state->renderer);
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

#include "app.h"

#define APP_STATE(app) ((AppState*)(app))
#define APP_REF(state) ((App)(state))

struct AppState 
{
    AppOnCreateFunction     create_func;
    AppOnUpdateFunction     update_func;
    AppOnDestroyFunction    destroy_func;
    Window window;
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

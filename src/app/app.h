#pragma once

#include <window/window.h>
#include <renderer/renderer.h>

typedef void* App;
typedef void (*AppOnCreateFunction)(void);
typedef void (*AppOnUpdateFunction)(double);
typedef void (*AppOnDestroyFunction)(void);

struct AppInfo 
{
    AppOnCreateFunction     create_func;
    AppOnUpdateFunction     update_func;
    AppOnDestroyFunction    destroy_func;
    struct WindowInfo       *window_info;
};

App create_app(const struct AppInfo *info);
void destroy_app(const App app);
void app_run(const App app);
Window get_window(const App app);

/*
 * get_renderer
 * get_event_system
 * get_audio_manager
 * get_asset_manager
 */

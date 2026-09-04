#pragma once

typedef void (*AppOnCreateFunction)(void);
typedef void (*AppOnUpdateFunction)(double);
typedef void (*AppOnDestroyFunction)(void);

struct AppInfo 
{
    AppOnCreateFunction     create_func;
    AppOnUpdateFunction     update_func;
    AppOnDestroyFunction    destroy_func;
};

/*
 * get_window
 * get_renderer
 * get_event_system
 * get_audio_manager
 * get_asset_manager
 */

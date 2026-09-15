#include "event/event.h"
#include <stdio.h>
#include <scorching_sand.h>
#include <common/queue.h>

void on_create();
void on_update(double);
void on_destroy();
void _on_window_change_callback(const Event *event, void *user_data);

Window g_window = NULL;
App g_app = NULL;

int main(int argc, char** argv)
{
    struct WindowInfo windowInfo = {};
    windowInfo.width = 1280;
    windowInfo.height = 720;
    windowInfo.flags = WINDOW_FLAG_RESIZABLE;
    windowInfo.title = "Sandbox - Scorching Sand";

    struct AppInfo appInfo = {};
    appInfo.create_func = on_create;
    appInfo.update_func = on_update;
    appInfo.destroy_func = on_destroy;
    appInfo.window_info = &windowInfo;

    g_app = create_app(&appInfo);
    app_run(g_app);
    destroy_app(g_app);
    return 0;
}

void on_create()
{
    printf("on_create\n");
    g_window = get_window(g_app);

    EventSubscribeInfo size_change_callback_info = {
        .event_type = EVENT_TYPE_WINDOW_RESIZE,
        .user_data = NULL,
        .on_event_callback = _on_window_change_callback 
    };

    em_subscribe_to_event(&size_change_callback_info);
}

void on_update(double elapsed)
{
    static uint32_t x, y, width, height;
    uint32_t cx, cy, cwidth, cheight;

}

void on_destroy()
{
    printf("on_destroy\n");
}

void _on_window_change_callback(const Event *event, void *user_data)
{
    printf("Size change: %d %d\n", event->data.window_size.width, event->data.window_size.height);
}

#include <stdio.h>
#include <scorching_sand.h>
#include <common/queue.h>

void on_create();
void on_update(double);
void on_destroy();

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
}

void on_update(double elapsed)
{
    static uint32_t x, y, width, height;
    uint32_t cx, cy, cwidth, cheight;

    get_window_pos(g_window, &cx, &cy);
    get_window_size(g_window, &cwidth, &cheight);

    if (cx != x || cy != y || cwidth != width || cheight != height)
    {
        x = cx;
        y = cy;
        width = cwidth;
        height = cheight;
        printf("Pos: (%u %u)   Size: (%u %u)\n", x, y, width, height);
    }
}

void on_destroy()
{
    printf("on_destroy\n");
}

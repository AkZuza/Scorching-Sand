#pragma once
#include <common.h>
#include <vulkan/vulkan.h>

typedef (void*) Window;

enum WindowFlags 
{
    WINDOW_FLAG_RESIZABLE   = 1<<0,
    WINDOW_FLAG_ON_TOP      = 1<<1
};

struct WindowInfo 
{
    uint32_t        width,
    uint32_t        height,
    const char      *title,
    WindowFlags     flags
};

Window          create_window(struct WindowInfo *info);
void            destroy_window(Window window);
void            close_window(Window window);
void            update_window(Window window);
int             is_window_running(Window window);
void            set_window_title(Window window, const char* new_title);
const char*     get_window_title(Window window);
void            set_window_size(Window window, uint32_t width, uint32_t height);
void            get_window_size(Window window, uint32_t *width, uint32_t *height);
void            set_window_pos(Window window, uint32_t x, uint32_t y);
void            get_window_pos(Window window, uint32_t *x, uint32_t *y);
VkSurfaceKHR    get_vulkan_surface(Window window);

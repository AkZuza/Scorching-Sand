#include "window.h"
#include <GLFW/glfw3.h>

#define MAX_WINDOW_TITLE_LENGTH 256
#define WINDOW_STATE(window) ((WindowState*)(window))
#define WINDOW_REF(window_state) ((Window)(window_state))
#define WINDOW_INSTANCE(ptr) ((WindowInstance*)(ptr))

struct WindowState
{
    char title[MAX_WINDOW_TITLE_LENGTH];
    uint32_t width;
    uint32_t height;
    int32_t x;
    int32_t y;
    int is_running;
};
typedef struct WindowState WindowState;

struct WindowInstance
{
    GLFWwindow *window;
    WindowState *state;
};
typedef struct WindowInstance WindowInstance;

void set_window_callbacks(WindowInstance *state);

Window  create_window(struct WindowInfo *info)
{
    glfwInit();
    
    int resizable = ((info->flags & WINDOW_FLAG_RESIZABLE) != 0) ? GLFW_TRUE : GLFW_FALSE;
    int stay_on_top = ((info->flags & WINDOW_FLAG_ON_TOP) != 0) ? GLFW_TRUE : GLFW_FALSE;

    glfwWindowHint(GLFW_RESIZABLE, resizable);
    glfwWindowHint(GLFW_FLOATING, stay_on_top);

    GLFWwindow *window = glfwCreateWindow(
            info->width,
            info->height,
            info->title, 
            NULL,
            NULL
    );
    
    if (window == NULL) return NULL;
    glfwMakeContextCurrent(window);

    int32_t xpos, ypos;
    glfwGetWindowPos(window, (int32_t*)&xpos, (int32_t*)&ypos);

    WindowState *state = malloc(sizeof(WindowState));
    state->width = info->width;
    state->height = info->height;
    state->x = xpos;
    state->y = ypos;
    state->is_running = 1;
    strncpy(
        state->title,
        info->title,
        MAX_WINDOW_TITLE_LENGTH
    );

    WindowInstance *instance = malloc(sizeof(WindowInstance));
    instance->window = window;
    instance->state = state;

    set_window_callbacks(instance);
    
    return WINDOW_REF(instance);
}

void destroy_window(Window window)
{
    WindowInstance *instance = WINDOW_INSTANCE(window);
    WindowState *state = instance->state;
    if (state->is_running) close_window(window);
    glfwDestroyWindow(instance->window);
    glfwTerminate();
    free(state);
    free(instance);
}

void close_window(Window window)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    WindowState *state = inst->state;
    glfwSetWindowShouldClose(inst->window, GLFW_TRUE); 
    state->is_running = false;
}

void update_window(Window window)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    glfwPollEvents();
    glfwSwapBuffers(inst->window);
}

int is_window_running(Window window)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    WindowState *state = inst->state;
    return state->is_running;
}

void set_window_title(Window window, const char* new_title)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    WindowState *state = inst->state;
    glfwSetWindowTitle(inst->window, new_title);
    strncpy(state->title, new_title, MAX_WINDOW_TITLE_LENGTH);
}

const char* get_window_title(Window window)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    WindowState *state = inst->state;
    return state->title;
}

void set_window_size(Window window, uint32_t width, uint32_t height)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    glfwSetWindowSize(inst->window, width, height);
}

void get_window_size(Window window, uint32_t *width, uint32_t *height)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    WindowState *state = inst->state;
    *width = state->width;
    *height = state->height;
}

void set_window_pos(Window window, uint32_t x, uint32_t y)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    glfwSetWindowPos(inst->window, x, y);
}

void get_window_pos(Window window, uint32_t *x, uint32_t *y)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    WindowState *state = inst->state;
    *x = state->x;
    *y = state->y;
}

void get_window_framebuffer_size(Window window, uint32_t *width, uint32_t *height)
{
    WindowInstance *inst = WINDOW_INSTANCE(window);
    int w, h;
    
    glfwGetFramebufferSize(inst->window, &w, &h);
    *width = w;
    *height = h;
}

VkSurfaceKHR    get_vulkan_surface(Window window);

// GLFW CALLBACKS
void _glfw_close_callback(GLFWwindow *window)
{
    WindowState *state = WINDOW_STATE(glfwGetWindowUserPointer(window));
    state->is_running = false;
}

void _glfw_size_callback(GLFWwindow *window, int width, int height)
{
    WindowState *state = WINDOW_STATE(glfwGetWindowUserPointer(window));
    state->width = width;
    state->height = height;
}

void _glfw_pos_callback(GLFWwindow *window, int x, int y)
{
    WindowState *state = WINDOW_STATE(glfwGetWindowUserPointer(window));
    state->x = x;
    state->y = y;
}

void set_window_callbacks(WindowInstance *inst)
{
    WindowState *state = inst->state;
    GLFWwindow *window = inst->window;
    glfwSetWindowUserPointer(window, (void*)state);

    glfwSetWindowCloseCallback(window, _glfw_close_callback);
    glfwSetWindowSizeCallback(window, _glfw_size_callback);
    glfwSetWindowPosCallback(window, _glfw_pos_callback);
}

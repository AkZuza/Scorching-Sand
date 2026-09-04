#include "window.h"
#include <GLFW/glfw3.h>

#define MAX_WINDOW_TITLE_LENGTH 256
#define WINDOW_STATE(window) ((WindowState*)(window))
#define WINDOW_REF(window_state) ((Window)(window_state))

struct WindowState
{
    char title[MAX_WINDOW_TITLE_LENGTH];
    GLFWwindow *window;
    uint32_t width;
    uint32_t height;
    int32_t x;
    int32_t y;
    int is_running;
};
typedef struct WindowState WindowState;

void set_window_callbacks(WindowState *state);

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
    state->window = window;
    strncpy(
        state->title,
        info->title,
        MAX_WINDOW_TITLE_LENGTH
    );

    set_window_callbacks(state);
    
    return WINDOW_REF(state);
}

void destroy_window(Window window)
{
    WindowState *state = WINDOW_STATE(window);
    if (state->is_running) close_window(window);
    glfwDestroyWindow(state->window);
    glfwTerminate();
    free(state);
}

void close_window(Window window)
{
    WindowState *state = WINDOW_STATE(window);
    glfwSetWindowShouldClose(state->window, GLFW_TRUE); 
    state->is_running = false;
}

void update_window(Window window)
{
    WindowState *state = WINDOW_STATE(window);
    glfwPollEvents();
    glfwSwapBuffers(state->window);
}

int is_window_running(Window window)
{
    WindowState *state = WINDOW_STATE(window);
    return state->is_running;
}

void set_window_title(Window window, const char* new_title)
{
    WindowState *state = WINDOW_STATE(window);
    glfwSetWindowTitle(state->window, new_title);
    strncpy(state->title, new_title, MAX_WINDOW_TITLE_LENGTH);
}

const char* get_window_title(Window window)
{
    WindowState *state = WINDOW_STATE(window);
    return state->title;
}

void set_window_size(Window window, uint32_t width, uint32_t height)
{
    WindowState *state = WINDOW_STATE(window);
    state->width = width;
    state->height = height;
    glfwSetWindowSize(state->window, width, height);
}

void get_window_size(Window window, uint32_t *width, uint32_t *height)
{
    WindowState *state = WINDOW_STATE(window);
    *width = state->width;
    *height = state->height;
}

void set_window_pos(Window window, uint32_t x, uint32_t y)
{
    WindowState *state = WINDOW_STATE(window);
    glfwSetWindowPos(state->window, x, y);
    state->x = x;
    state->y = y;
}

void get_window_pos(Window window, uint32_t *x, uint32_t *y)
{
    WindowState *state = WINDOW_STATE(window);
    int xpos, ypos;
    glfwGetWindowPos(state->window, &xpos, &ypos);
    *x = xpos;
    *y = ypos;
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

void set_window_callbacks(WindowState *state)
{
    GLFWwindow *window = state->window;
    glfwSetWindowUserPointer(window, (void*)state);

    glfwSetWindowCloseCallback(window, _glfw_close_callback);
    glfwSetWindowSizeCallback(window, _glfw_size_callback);
    glfwSetWindowPosCallback(window, _glfw_pos_callback);
}
